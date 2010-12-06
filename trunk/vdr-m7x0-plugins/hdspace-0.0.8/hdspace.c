/*
 * hdspace.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/videodir.h>
#include <vdr/tools.h>
#include "i18n.h"


#ifdef DEBUG
   #define debug(x...) dsyslog("HDSpace: " x);
   #define error(x...) esyslog("HDSpace: " x);
#else
   #define debug(x...) ;
   #define error(x...) esyslog("HDSpace: " x);
#endif

#define DEFAULT_CHARS "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-_@."

#ifdef HAVE_SENDMAIL
   #define SENDMAIL "sendmail"
#else
   #define SENDMAIL "mini_sendmail"
#endif

static const char *VERSION        = "0.0.8";
static const char *DESCRIPTION    = tr("Send e-mail when HD Space gets low");
//We do not need a entry in the main menu
//static const char *MAINMENUENTRY  = "HD space";
int i_shedule = 60; //Time in minutes between checks
char st_sendTo [128] = ""; //Mail user name
char st_server [128] = ""; //Mailserver
char st_from [128] = "VDR"; //Mail from
int i_LastFreeGB = -1; //The GB that was free the last run
int isDisplayOnOSD = 1; //Disk to check
int i_warn = 10; //Warn level in MB

// --------------------------------- Menu Setup HdSpace --------------------------------------------
class cMenuSetupHdSpace : public cMenuSetupPage{
private:
public:
   cMenuSetupHdSpace(void);
   virtual void Store(void);
};

cMenuSetupHdSpace::cMenuSetupHdSpace(void){
   //Build setup entries
   Add(new cMenuEditIntItem(tr("Check interval [min]"), &i_shedule, 1, 999));
   Add(new cMenuEditStrItem(tr("Mail from name"), st_from, sizeof(st_from), DEFAULT_CHARS));
   Add(new cMenuEditStrItem(tr("Mail sendTo names"), st_sendTo, sizeof(st_sendTo), DEFAULT_CHARS));
#ifndef HAVE_SENDMAIL
   Add(new cMenuEditStrItem(tr("Mail server name"), st_server, sizeof(st_server), DEFAULT_CHARS));
#endif
   Add(new cMenuEditBoolItem(tr("Display warning on TV"), &isDisplayOnOSD,  tr("No"), tr("Yes")));
   Add(new cMenuEditIntItem(tr("Min free hd space [GB]"), &i_warn, 1, 999));
}

void cMenuSetupHdSpace::Store(void){
   SetupStore("Shedule", i_shedule);
   SetupStore("From", st_from);
   SetupStore("User", st_sendTo);
   SetupStore("Server", st_server);
   SetupStore("OSD", isDisplayOnOSD);
   SetupStore("Warn", i_warn);
}


//--------------------------- Background Monitor Classs ---------------------------------------------
class cBackgroundMonitor : public cThread{
private:
   virtual void Action(void);
   bool isActive;
public:
   cBackgroundMonitor(void);
   ~cBackgroundMonitor();
};

cBackgroundMonitor::cBackgroundMonitor(void):cThread("hdSpace"){
}

cBackgroundMonitor::~cBackgroundMonitor(){
   isActive=false;
}

void cBackgroundMonitor::Action(void){
   char buffer[128]; //Comannd line
   int i_OSD;
   FILE *cmd_pipe;
   bool oldIaMode=getIaMode();

   //Needed to read the output of "df -h"
   char *line = NULL;
   FILE *df_pipe;
   cReadLine ReadLine;

   isActive=true;

   // let VDR do its startup
   cCondWait::SleepMs( 20*1000);//20 seconds

   time_t nextUpdate = time(NULL);
   while (isActive && Running()){
      //Check if the iamode has changed to on
      if (getIaMode()==1 && oldIaMode==0){
         nextUpdate = time(NULL);
      }
      oldIaMode=getIaMode();

      time_t now = time(NULL);
      if (now < nextUpdate){
         //Time for next check not reatched, wait 1 second and restart main loop
         cCondWait::SleepMs(1000);//1 seconds
         continue;
      }

      //Check if we have mail translation for current OSD language
      i_OSD=Setup.OSDLanguage;
      if (strlen(MailLow[0][i_OSD])==0){
         isyslog("HDSpace: No translation for mail texts found in language %s",I18nLanguages()[i_OSD]);
         i_OSD=0;
      }

      //Get free space
      int i_FreeMB, i_UsedMB, i_FreeGB;
      int i_Percent = VideoDiskSpace(&i_FreeMB, &i_UsedMB);
      i_FreeGB=i_FreeMB/1024;
      if (i_FreeGB==0 && i_Percent==0){
         //workaround for display if no medium is attached
         i_Percent=100;
      }
      //Check if Free space is blow limit
      if (i_FreeGB < i_warn){
         isyslog("HDSpace: Free space is below warning level! (Free:%i LastFree:%i Warn:%i)", i_FreeGB, i_LastFreeGB, i_warn);

         //Show warning on OSD, if enabeld
         if (isDisplayOnOSD==1){
            snprintf(buffer, sizeof(buffer), tr("Recording space is %i%%(%iGB). Limit:%iGB"), 100-i_Percent , i_FreeGB , i_warn );
            Skins.QueueMessage( mtWarning, buffer, 5, -1);
         }

         //Send mail if user and server is filled
#ifdef HAVE_SENDMAIL
         if (strlen(st_sendTo)>0 && i_FreeGB!=i_LastFreeGB){
            //Creating command line  (sendmail sendTo)
            snprintf(buffer, sizeof(buffer), "%s %s", SENDMAIL , st_sendTo);
#else
         if (strlen(st_sendTo)>0 && strlen(st_server)>0 && i_FreeGB!=i_LastFreeGB){
            //Creating command line  (mini_sendmail -fvon -sserver -v sendTo)
            snprintf(buffer, sizeof(buffer), "%s -f%s -s%s -v %s 2>&1 |logger -p debug -t sendmail", SENDMAIL , st_from , st_server, st_sendTo);
#endif
            //starting mail command
            debug("Executing \"%s\"", buffer);
            cmd_pipe = popen (buffer, "w"); //Open mini_sendmail or sendmail
#ifdef HAVE_SENDMAIL
            fprintf(cmd_pipe, "From: %s\n",st_from);
            fprintf(cmd_pipe, "To: %s\n", st_sendTo);
#endif
            //Get the hostname into the buffer
            gethostname(buffer, sizeof(buffer));
            //Subject
            fprintf(cmd_pipe, "Subject: ");
            fprintf(cmd_pipe, MailLow[0][i_OSD],100-i_Percent,buffer,i_FreeGB );
            fprintf(cmd_pipe, "Content-Type: text/plain; charset=ISO-8859-15\n");
            fprintf(cmd_pipe, "\n");
            //Mail body
            fprintf(cmd_pipe, MailLow[1][i_OSD]);
            fprintf(cmd_pipe, MailLow[2][i_OSD],buffer);
            fprintf(cmd_pipe, MailLow[3][i_OSD],i_warn);
            fprintf(cmd_pipe, MailLow[4][i_OSD],i_FreeGB);
            fprintf(cmd_pipe, MailLow[5][i_OSD]);
            fprintf(cmd_pipe, MailLow[6][i_OSD],i_FreeGB);
            fprintf(cmd_pipe, MailLow[7][i_OSD]);
            fprintf(cmd_pipe, MailLow[8][i_OSD],i_FreeMB/1536,i_FreeMB/819);
            //Attach 'df -h' output
            fprintf(cmd_pipe, "\nTechnical information:\n");
            df_pipe = popen("df -h", "r");
            if (df_pipe){//read response from command
               while ( (line = ReadLine.Read(df_pipe) ) !=  NULL){
                  fprintf(cmd_pipe,"%s\n",line);
               }
               //cleanup
               pclose(df_pipe);
            }

            fprintf(cmd_pipe, ".\n"); //End mail
            if (pclose (cmd_pipe) != 0){
               error("Error while executing \"%s\"!", buffer);
            }
         }//Mail send end
       }//Warn end

      //Check is space is the first time over the limet
      if (i_LastFreeGB < i_warn  && i_FreeGB >= i_warn){
         isyslog("HDSpace: Free space is OK now! (Free:%i LastFree:%i Warn:%i)", i_FreeGB, i_LastFreeGB, i_warn);

         //Show warning on OSD, if enabeld
         if (isDisplayOnOSD==1){
            snprintf(buffer, sizeof(buffer), tr("Recording space OK. Limit:%iGB Now:%iGB"), i_warn, i_FreeGB);
            Skins.QueueMessage( mtWarning, buffer, 5, -1);
         }

         //Send mail if user and server is filled
#ifdef HAVE_SENDMAIL
         if (strlen(st_sendTo)>0){
            //Creating command line  (sendmail sendTo)
            snprintf(buffer, sizeof(buffer), "%s %s", SENDMAIL , st_sendTo);
#else
         if (strlen(st_sendTo)>0 && strlen(st_server)>0){
            //Creating command line  (mini_sendmail -fvon -sserver -v sendTo)
            snprintf(buffer, sizeof(buffer), "%s -f%s -s%s -v %s 2>&1 |logger -p debug -t sendmail", SENDMAIL , st_from , st_server, st_sendTo);
#endif
            //starting mail command
            debug("Executing \"%s\"", buffer);
            cmd_pipe = popen (buffer, "w"); //Open mini_sendmail or sendmail
#ifdef HAVE_SENDMAIL
            fprintf(cmd_pipe, "From: %s\n",st_from);
            fprintf(cmd_pipe, "To: %s\n", st_sendTo);
#endif
            //Get the hostname into the buffer
            gethostname(buffer, sizeof(buffer));
            //Subject
            fprintf(cmd_pipe, "Subject: ");
            fprintf(cmd_pipe, MailOK[0][i_OSD],100-i_Percent,buffer,i_FreeGB );
            fprintf(cmd_pipe, "Content-Type: text/plain; charset=ISO-8859-15\n");
            fprintf(cmd_pipe, "\n");
            //Mail body
            fprintf(cmd_pipe, MailOK[1][i_OSD]);
            fprintf(cmd_pipe, MailOK[2][i_OSD],buffer);
            fprintf(cmd_pipe, MailOK[3][i_OSD],i_warn);
            fprintf(cmd_pipe, MailOK[4][i_OSD],i_FreeGB);
            //Attach 'df -h' output
            fprintf(cmd_pipe, "\nTechnical information:\n");
            df_pipe = popen("df -h", "r");
            if (df_pipe){//read response from command
               while ( (line = ReadLine.Read(df_pipe) ) !=  NULL){
                  fprintf(cmd_pipe,"%s\n",line);
               }
               //cleanup
               pclose(df_pipe);
            }

            fprintf(cmd_pipe, ".\n"); //End mail
            if (pclose (cmd_pipe) != 0){
               error("Error while executing \"%s\"!", buffer);
            }
         }//Mail send end
      }//Warn end

      //Update LastFreeGB
      i_LastFreeGB=i_FreeGB;

      //Calculate next intervall
      nextUpdate = long(time(NULL)/60)*60 + (i_shedule * 60);
   }//End while(isActive)
}


//--------------------------   Plugin Class --------------------------------------------------
class cPluginHdspace : public cPlugin{
private:
  // Add any member variables or functions you may need here.
   cBackgroundMonitor *monitor;
public:
  cPluginHdspace(void);
  virtual ~cPluginHdspace();
  virtual const char *Version(void){ return VERSION; }
  virtual const char *Description(void){ return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
//We do not need a entry in the main menu
//  virtual const char *MainMenuEntry(void){ return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginHdspace::cPluginHdspace(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginHdspace::~cPluginHdspace()
{
  // Clean up after yourself!
}

const char *cPluginHdspace::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginHdspace::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginHdspace::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginHdspace::Start(void)
{
  // Start any background activities the plugin shall perform.
   RegisterI18n(Phrases);

   //Check if we had a last stored Free size, take the current if not
   if (i_LastFreeGB==-1){
      int i_FreeMB, i_UsedMB;
      VideoDiskSpace(&i_FreeMB, &i_UsedMB);
      i_LastFreeGB=i_FreeMB/1024;
   }

   //Start drive monitor
   monitor = new cBackgroundMonitor;
   //Start Monitor
   monitor->Start();

  return true;
}

void cPluginHdspace::Stop(void)
{
  // Stop any background activities the plugin shall perform.

   //Delete drive monitor
   delete monitor;

   //Store data
   SetupStore("LastFreeGB",i_LastFreeGB);
}

void cPluginHdspace::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginHdspace::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginHdspace::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginHdspace::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginHdspace::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupHdSpace;
}

bool cPluginHdspace::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
   if(!strcasecmp(Name, "Shedule")){
      i_shedule = atoi(Value);
   }else if (!strcasecmp(Name, "From")){
      strcpy(st_from,Value);
   }else if (!strcasecmp(Name, "User")){
      strcpy(st_sendTo,Value);
   }else if (!strcasecmp(Name, "Server")){
      strcpy(st_server,Value);
   }else if (!strcasecmp(Name, "OSD")){
      isDisplayOnOSD = atoi(Value);
   }else if (!strcasecmp(Name, "Warn")){
      i_warn = atoi(Value);
   }else if (!strcasecmp(Name, "LastFreeGB")){
      i_LastFreeGB = atoi(Value);
   }else{
      return false;
   }
   return true;
}

bool cPluginHdspace::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginHdspace::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginHdspace::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginHdspace); // Don't touch this!

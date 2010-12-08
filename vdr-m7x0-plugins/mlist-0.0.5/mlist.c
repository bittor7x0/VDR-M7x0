/*
 * mlist.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/osdbase.h>
#include <time.h>

static const char *VERSION        = "0.0.5";
static const char *DESCRIPTION    = trNOOP("Displays the message history");
static const char *MAINMENUENTRY  = trNOOP("Message History");

// --------------------------- cMlistConfig-------------------------------------
struct cMlistConfig {
public:
  cMlistConfig(void);
  int  iHideMenuEntry;
};

cMlistConfig MlistConfig;

cMlistConfig::cMlistConfig(void) {
  iHideMenuEntry = false;
}

// --------------------------- cMenuSetupMlist ----------------------------------------
class cMenuSetupMlist : public cMenuSetupPage {
private:
  int  iNewHideMenuEntry;
protected:
  virtual void Store(void);
public:
  cMenuSetupMlist(void);
};

void cMenuSetupMlist::Store(void) {
  MlistConfig.iHideMenuEntry = iNewHideMenuEntry;
  SetupStore("HideMenuEntry", MlistConfig.iHideMenuEntry);
}

cMenuSetupMlist::cMenuSetupMlist(void) {
  iNewHideMenuEntry = MlistConfig.iHideMenuEntry;
  Add(new cMenuEditBoolItem (tr("Hide mainmenu entry"),     &iNewHideMenuEntry));
}

// --------------------------- cMessage ----------------------------------------

class cMessage : public cListObject {
private:
  char *msg;
  time_t msg_time;
public:
  cMessage(const char *Message) { 
    msg = new char[strlen(Message)+1]; 
    msg_time = time(NULL);
    strcpy(msg, Message); 
  }
  char *Message() { return msg; }
  time_t* MessageTime() { return &msg_time; }
};

// --------------------------- cMlistMenu --------------------------------------

class cMlistMenu : public cOsdMenu {
private:
  cList<cMessage> *mlist;
  void ClearList();
  void PopulateList();
public:
  cMlistMenu(cList<cMessage> *mlist);
  virtual eOSState ProcessKey(eKeys Key);
};

cMlistMenu::cMlistMenu(cList<cMessage> *mlist)
:cOsdMenu(tr(MAINMENUENTRY)) {
  SetHelp(NULL, NULL, tr("Delete"), NULL);
  this->mlist = mlist;
  PopulateList();
}

void cMlistMenu::PopulateList() {
// clear any osditems
  Clear();
// add new osditems
  for (cMessage *msg = mlist->Last(); msg; msg = mlist->Prev(msg)) {
    struct tm *broken_time = localtime(msg->MessageTime());
    char *formatted_time;
    asprintf(&formatted_time, "%02i:%02i:%02i", broken_time->tm_hour, broken_time->tm_min, broken_time->tm_sec);

    Add(new cOsdItem(formatted_time, osUnknown, false));
    Add(new cOsdItem(msg->Message()));

    free(formatted_time);
  }
  Display();
}

eOSState cMlistMenu::ProcessKey(eKeys Key) {

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch(Key) {
      case kYellow:
          ClearList();
	  state = osContinue;
        break;
      default:
        state = osUnknown;
    }
  }
  return state;
}

void cMlistMenu::ClearList() {
  mlist->Clear();
  PopulateList();
}

// --------------------------- cPluginMlist ------------------------------------

class cPluginMlist : public cPlugin, public cStatus {
private:
  // message list
  cList<cMessage> mlist;
public:
  cPluginMlist(void);
  virtual ~cPluginMlist();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  
  // from cStatus
  virtual void OsdStatusMessage(const char *Message) { if (Message) mlist.Add(new cMessage(Message)); }
  // Message has been displayed in the status line of the menu
  // If Message is NULL, the status line has been cleared.
};

cPluginMlist::cPluginMlist(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginMlist::~cPluginMlist()
{
  // Clean up after yourself!
}

const char *cPluginMlist::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginMlist::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginMlist::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginMlist::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginMlist::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginMlist::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

const char *cPluginMlist::MainMenuEntry(void) 
 { 
   if (!MlistConfig.iHideMenuEntry)
    return tr(MAINMENUENTRY); 
   else
    return NULL; 
 }

cOsdObject *cPluginMlist::MainMenuAction(void)
{
  // Displays the list of messages
  cMlistMenu *mlistMenu = new cMlistMenu(&mlist);
  
  return mlistMenu;
}

cMenuSetupPage *cPluginMlist::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupMlist();
}

bool cPluginMlist::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "HideMenuEntry")){
   MlistConfig.iHideMenuEntry = atoi(Value);
   return true;
  } 
  else 
   return false;
}
bool cPluginMlist::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginMlist::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "LSTM\n"
    "   Print the Message History.",
    "DELM\n"
    "   Clear the Message History.",
    NULL
  };
										      
  return HelpPages;
}

cString cPluginMlist::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  if (strcasecmp(Command, "LSTM") == 0){
    cString reply = "";
    cString temp;
    if (mlist.Count() == 0)
      return cString::sprintf("Message History empty.");
    for (cMessage *msg = mlist.Last(); msg; msg = mlist.Prev(msg)) {
      struct tm *broken_time = localtime(msg->MessageTime());
      char *formatted_time;
      asprintf(&formatted_time, "%02i:%02i:%02i", broken_time->tm_hour, broken_time->tm_min, broken_time->tm_sec);
      reply = cString::sprintf("%s%s - %s\n", (const char *) reply, formatted_time, msg->Message());
      free(formatted_time);
    }
    return reply;
  }
  else if (strcasecmp(Command, "DELM") == 0){
    mlist.Clear();
    return cString::sprintf("Message History cleared.");
  }
  return NULL;
}

VDRPLUGINCREATOR(cPluginMlist); // Don't touch this!

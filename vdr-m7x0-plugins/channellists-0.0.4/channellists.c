/*
 * channellists.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/timers.h>
#include <vdr/interface.h>

#include "i18n.h"

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = trNOOP("Manage your channellists");
static const char *MAINMENUENTRY  = trNOOP("Channellists");

char *ConfigDir = NULL;
char *ChannelsConf = NULL;
char *UpdateScript = NULL;

char *ListToReload = NULL;

int HideMainMenuEntry = 0;
int ActionAfterSwitch = 0;
int RedButtonSwitch = 0;
int GreenButtonSwitch = 0;

// --- cMenuSetupChannellists -------------------------------------------------
class cMenuSetupChannellists : public cMenuSetupPage {
private:
  int newHideMainMenuEntry;
  int newActionAfterSwitch;
  int newRedButtonSwitch;
  int newGreenButtonSwitch;
  const char *AfterSwitchActions[4];
protected:
  virtual void Store(void);
public:
  cMenuSetupChannellists(void);
};

cMenuSetupChannellists::cMenuSetupChannellists(void) {
  newHideMainMenuEntry = HideMainMenuEntry;
  newActionAfterSwitch = ActionAfterSwitch;
  newRedButtonSwitch = RedButtonSwitch;
  newGreenButtonSwitch = GreenButtonSwitch;

  AfterSwitchActions[0] = trVDR("Channels");
  AfterSwitchActions[1] = tr("Menu");
  AfterSwitchActions[2] = tr("close");
  AfterSwitchActions[3] = tr("none");

  Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMainMenuEntry));
  Add(new cMenuEditStraItem(tr("Action after switch"),  &newActionAfterSwitch, 4, AfterSwitchActions));
  Add(new cMenuEditBoolItem(tr("Button Red"),           &newRedButtonSwitch,   tr("Rename"), tr("Script")));
  Add(new cMenuEditBoolItem(tr("Button Green"),         &newGreenButtonSwitch, tr("Copy"),   tr("Script")));
  }

void cMenuSetupChannellists::Store(void) {
  SetupStore("HideMainMenuEntry", HideMainMenuEntry = newHideMainMenuEntry);
  SetupStore("ActionAfterSwitch", ActionAfterSwitch = newActionAfterSwitch);
  SetupStore("RedButtonSwitch",   RedButtonSwitch   = newRedButtonSwitch);
  SetupStore("GreenButtonSwitch", GreenButtonSwitch = newGreenButtonSwitch);
  }

// time as string
cString DateTime(time_t t) {
  char buffer[32];
  if (t == 0) {
     time(&t);
     }
  struct tm tm_r;
  tm *tm = localtime_r(&t, &tm_r);
  snprintf(buffer, sizeof(buffer), "%02d-%02d_%02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_hour, tm->tm_min);
  return buffer;
  }


// --- cPluginChannellists ----------------------------------------------------
class cPluginChannellists : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  char *List[50];
  int ListNbr;
  cPluginChannellists(void);
  virtual ~cPluginChannellists();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return HideMainMenuEntry ? NULL : tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};


// --- Script ------------------------------------------------------------------
class cUpdateScriptThread : public cThread {
private:
protected:
  virtual void Action(void) {
     if (access(UpdateScript, 01) == 0) {
        dsyslog("Channellists: executing %s", UpdateScript);
        if (system(UpdateScript) == 0)
           dsyslog("Channellists: %s - finished", UpdateScript);
        }
     else
        LOG_ERROR_STR(UpdateScript);
     delete(this);
     };
public:
  ~cUpdateScriptThread() {};
};

class cExecuteCmdThread : public cThread {
private:
  char *ExecuteCmd;
protected:
  virtual void Action(void) {
     if (access(ExecuteCmd, 01) == 0) {
        dsyslog("Channellists: executing %s", ExecuteCmd);
        if (system(ExecuteCmd) == 0)
           dsyslog("Channellists: %s - finished", ExecuteCmd);
        }
     else
        LOG_ERROR_STR(ExecuteCmd);
     delete(this);
     };
public:
   cExecuteCmdThread(char *cmd) { asprintf(&ExecuteCmd, "%s", cmd); };
   cExecuteCmdThread(const char *cmd) { asprintf(&ExecuteCmd, "%s", cmd); };
  ~cExecuteCmdThread() { free(ExecuteCmd); };
};

// --- Timer ------------------------------------------------------------------
class TimerString : public cListObject {
private:
  cString _timer;
public:
  TimerString (cString timer) { _timer=timer; }
  cString GetTimerString() { return (_timer); }
};

class TimerList {
private:
  cList<TimerString>  myTimers;
public:
  TimerList() {};
  ~TimerList() {};
  void SaveTimer();
  void RestoreTimer();
};


// --- cSwitchMenu ------------------------------------------------------------
class cSwitchMenu : public cOsdMenu {
private:
  cPluginChannellists *switcher;
public:
  cSwitchMenu(cPluginChannellists &SwitcherPlug);
  virtual ~cSwitchMenu();
  void ChanOrg(void);
  void ErrorMsg(const char *MyErrorMsg);
  eOSState SwitchChannellist();
  eOSState RenameChannellist();
  eOSState CopyChannellist();
  eOSState DeleteChannellist();
  eOSState SaveChannellist();
  eOSState UpdateChannellist();
  eOSState FlushChannellist();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void SetHelp();
};

// --- cMenuEditChannellist --------------------------------------------------
class cMenuEditChannellist:public cOsdMenu {
private:
  char allowedChars[MaxFileName];
  char newname[MaxFileName];
  char oldname[MaxFileName];
  int  removeold;
  int  savecurrent;
public:
  cMenuEditChannellist(const char *ChannellistName, const char *MenuSelect);
  ~cMenuEditChannellist();
  void ErrorMsg(const char *MyErrorMsg);
  virtual eOSState ProcessKey(eKeys Key);
};


// --- cPluginChannellists ---------------------------------------------------

cPluginChannellists::cPluginChannellists(void) {
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  ListNbr=0;
  }

cPluginChannellists::~cPluginChannellists() {
  // Clean up after yourself!
  if (ConfigDir)
     free(ConfigDir);
  if (ChannelsConf)
     free(ChannelsConf);
  if (UpdateScript)
     free(UpdateScript);
  }

const char *cPluginChannellists::CommandLineHelp(void) {
  // Return a string that describes all known command line options.
  return "  -s SCRIPT,   --script=SCRIPT      path to update script (default: channellists-update.sh)\n";
  }

bool cPluginChannellists::ProcessArgs(int argc, char *argv[]) {
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
       { "script",      required_argument, NULL, 's' },
       { NULL }
     };

  int c;
  while ((c = getopt_long(argc, argv, "s:", long_options, NULL)) != -1) {
        switch (c) {
          case 's': UpdateScript = optarg;
                    break;
          default:  esyslog("Channellists: unknown Parameter: %c", c);
                    return false;
          }
        }
  return true;
  }

bool cPluginChannellists::Initialize(void) {
  // Initialize any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  return true;
  }

bool cPluginChannellists::Start(void) {
  // Start any background activities the plugin shall perform.
  if (!ConfigDir)
     ConfigDir = strdup(cPlugin::ConfigDirectory("channellists"));

  if (!ChannelsConf) {
     char *ToLoad;
     asprintf(&ToLoad,"%s", cPlugin::ConfigDirectory());
     char *tmp = strrchr(ToLoad,'/');
     *tmp = '\0';
     asprintf(&ChannelsConf, "%s/channels.conf",ToLoad);
     free (ToLoad);
     }

  if (!UpdateScript)
     asprintf(&UpdateScript, "channellists-update.sh");

  return true;
  }

void cPluginChannellists::Stop(void) {
  // Stop any background activities the plugin is performing.
  }

void cPluginChannellists::Housekeeping(void) {
  // Perform any cleanup or other regular tasks.
  }

void cPluginChannellists::MainThreadHook(void) {
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
  }

cString cPluginChannellists::Active(void) {
  // Return a message string if shutdown should be postponed
  return NULL;
  }

time_t cPluginChannellists::WakeupTime(void) {
  // Return custom wakeup time for shutdown script
  return 0;
  }

cOsdObject *cPluginChannellists::MainMenuAction(void) {
  // Perform the action when selected from the main VDR menu.
  return new cSwitchMenu(*this);
  }

cMenuSetupPage *cPluginChannellists::SetupMenu(void) {
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupChannellists();
  }

bool cPluginChannellists::SetupParse(const char *Name, const char *Value) {
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "HideMainMenuEntry"))  HideMainMenuEntry = atoi(Value);
  else if (!strcasecmp(Name, "ActionAfterSwitch"))  ActionAfterSwitch = atoi(Value);
  else if (!strcasecmp(Name, "RedButtonSwitch"))    RedButtonSwitch = atoi(Value);
  else if (!strcasecmp(Name, "GreenButtonSwitch"))  GreenButtonSwitch = atoi(Value);
  else
     return false;
  return true;
  }

struct Channellists_SetListToReload_v1_0 {
  char *NewListToReload;
  };

bool cPluginChannellists::Service(const char *Id, void *Data) {
  // Handle custom service requests from other plugins
  if (strcmp(Id, "Channellists-SetListToReload-v1.0") == 0) {
     if (Data == NULL)
//        cReloadList();
        return true;
/*
     cOsdMenu **menu = (cOsdMenu**) Data;
     if (menu)
        *menu = (cOsdMenu*) MainMenuAction();
*/
     ListToReload = ((Channellists_SetListToReload_v1_0*)Data)->NewListToReload;
//     cReloadList(ListToReload);
     return true;
     }
  return false;
  }

const char **cPluginChannellists::SVDRPHelpPages(void) {
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "LIST\n"
    "    Reload channellists osd.",
    "LOAD [/path/to/channels.conf]\n"
    "    Reload channels.conf.",
    NULL
    };
  return HelpPages;
  }

cString cPluginChannellists::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
  // Process SVDRP commands this plugin implements
  if (strcasecmp(Command, "LIST") == 0) { //   TODO need a function for reloading channellists osd
     return cString::sprintf("sorry, this is for testing only");
     }
  else if (strcasecmp(Command, "LOAD") == 0) {
     if ((*Option) && (strcmp(Option, ChannelsConf)!= 0)) {
        int c;
        FILE *in,*out;
        in=fopen(Option,"r");
        if (in) {
           Channels.SwitchTo(1);
           TimerList timers;
           timers.SaveTimer();
           out=fopen(ChannelsConf,"w");
           if (out) {
              while ((c=fgetc(in))!=EOF) {
                 fputc(c,out);
                 }
              fclose(out);
              }
           else {
              LOG_ERROR_STR(ChannelsConf);
              }
           fclose(in);
           Channels.Load(ChannelsConf);
           Channels.SwitchTo(1);
           timers.RestoreTimer();
           return cString::sprintf("channels.conf successfully reloaded");
           }
        else {
           LOG_ERROR_STR(Option);
           return cString::sprintf("error - see syslog for detailed information!");
           }
        }
     else {
//        Channels.SwitchTo(1);
        TimerList timers;
        timers.SaveTimer();
        Channels.Load(ChannelsConf);
        Channels.SwitchTo(1);
        timers.RestoreTimer();
        return cString::sprintf("channels.conf successfully reloaded");
        }
     }
  return NULL;
  }


// --- TimerList -------------------------------------------------------------
// Timer Save and Restore Methods
void TimerList::SaveTimer() {
  myTimers.Clear();
  for (cTimer *t = Timers.First(); t; t= Timers.Next(t)) {
     myTimers.Add(new TimerString(t->ToText(true)));
     }
  Timers.cList<cTimer>::Clear();
  }

// restore Timers, delete Timers with unknown channel
void TimerList::RestoreTimer() {
  Timers.cList<cTimer>::Clear();

  for (TimerString *timer = myTimers.First(); timer; timer= myTimers.Next(timer)) {
     cString t = timer->GetTimerString();
     cTimer *tim = new cTimer();
     tim->Parse(t);
     if (tim->Channel() != NULL)
        Timers.Add(tim);
     }
  Timers.SetModified();
  myTimers.Clear();
  }


// --- cMenuEditChannellist --------------------------------------------------
cMenuEditChannellist::cMenuEditChannellist(const char *ChannellistName, const char *MenuSelect) : cOsdMenu(tr(MenuSelect),12) {
  if (strcmp(MenuSelect, "Rename") == 0)
     removeold = 1;
  else
     removeold = 0;
  if (strcmp(MenuSelect, "Save") == 0)
     savecurrent = 1;
  else
     savecurrent = 0;

  strn0cpy(oldname, "", sizeof(oldname));
  strn0cpy(oldname, ChannellistName, sizeof(oldname));
  strn0cpy(newname, "", sizeof(newname));
  strn0cpy(newname, ChannellistName, sizeof(newname));

  strn0cpy(allowedChars, trVDR(FileNameChars), sizeof(allowedChars));
  char *pch;
  pch = strstr (allowedChars, ".");
  strncpy (pch,"_", 1);
  puts (allowedChars);

  Add(new cMenuEditStrItem(tr("Name"), newname, sizeof(newname), allowedChars));
  cRemote::Put(kRight);
  }

cMenuEditChannellist::~cMenuEditChannellist() {
  }

// osd error message
void cMenuEditChannellist::ErrorMsg(const char *MyErrorMsg) {
  Skins.Message(mtStatus, tr(MyErrorMsg));
  sleep(Setup.OSDMessageTime);
  Display();
  }

eOSState cMenuEditChannellist::ProcessKey(eKeys Key) {
  eOSState state=cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
        case kOk: {
           if ((strcmp(newname, "") == 0) || ((strcmp(newname, oldname) == 0) && (savecurrent == 0))) return osContinue;

           int c;
           char *ToLoad;
           char *ToReload;

           asprintf(&ToReload, "%s/channels.conf.%s", ConfigDir, newname);
           if (savecurrent)
              asprintf(&ToLoad, "%s", ChannelsConf);
           else
              asprintf(&ToLoad, "%s/channels.conf.%s", ConfigDir, oldname);

           FILE *in,*out;
           in=fopen(ToLoad,"r");
           if (in) {
              out=fopen(ToReload,"w");
              if (out) {
                 while ((c=fgetc(in))!=EOF) {
                    fputc(c,out);
                    }
                 fclose(out);
                 }
              else {
                 LOG_ERROR_STR(ToReload);
                 ErrorMsg(trNOOP("An error occurs, see syslog..."));
                 }
              fclose(in);
              if (removeold)
                 remove(ToLoad);
              state = osPlugin;
              }
           else {
              LOG_ERROR_STR(ToLoad);
              ErrorMsg(trNOOP("An error occurs, see syslog..."));
              state = osBack;
              }
           free(ToLoad);
           free(ToReload);
           break;
           }
        default: {
           state = osContinue;
           break;
           }
        }
//     state = osContinue;
     }
  return state;
}


// --- cSwitchMenu -----------------------------------------------------------

cSwitchMenu::cSwitchMenu(cPluginChannellists &SwitcherPlug) : cOsdMenu(tr("Channellists"),20) {
  SetHelp();
  SetHasHotkeys();
  DIR *d = opendir(ConfigDir);
  if (d) {
     switcher=&SwitcherPlug;
     switcher->ListNbr=0;
//     switcher->List[SwitcherPlug.ListNbr]="";
     struct dirent *e;
     while ((e = readdir(d)) != NULL) {
        if ((!strncmp("channels.conf.", e->d_name,14)) && (strcmp("channels.conf.", e->d_name) != 0)) {
           char *name;
           asprintf(&name, "%s", e->d_name);
           strtok(name,".");
           name=strtok(NULL,".");
           name=strtok(NULL,".");
           Add(new cOsdItem(name));
           asprintf(&name,"%s",name);
           switcher->List[SwitcherPlug.ListNbr]=name;
           switcher->ListNbr+=1;
//         free(name);
           }
        }
     closedir(d);
     }
  }

cSwitchMenu::~cSwitchMenu(void) {
  }

// osd error message
void cSwitchMenu::ErrorMsg(const char *MyErrorMsg) {
  Skins.Message(mtStatus,tr(MyErrorMsg));
  sleep(Setup.OSDMessageTime);
  Display();
  }

// switch to ChanOrg
void cSwitchMenu::ChanOrg() {
  Channellists_SetListToReload_v1_0* reloadlist = new Channellists_SetListToReload_v1_0;
//  reloadlist.NewReloadList = 1;
  cPlugin *Plugin = cPluginManager::GetPlugin("chanorg");
  if (Plugin) {
     Plugin->Service("ChanOrg-SetReloadList-v1.0", reloadlist);
     }
  delete reloadlist;
  }

// switch to selected channellist
eOSState cSwitchMenu::SwitchChannellist() {
  eOSState state = osContinue;
  if (Count() == 0) return state;
  if (Interface->Confirm(tr("Switch current channellist?"))) {
     int c;
     char *ToLoad;

     asprintf(&ToLoad, "%s/channels.conf.%s", ConfigDir, switcher->List[Current()]);

     FILE *in,*out;
     in=fopen(ToLoad,"r");
     if (in) {
        Channels.SwitchTo(1);
        TimerList timers;
        timers.SaveTimer();
        out=fopen(ChannelsConf,"w");
        if (out) {
           while ((c=fgetc(in))!=EOF) {
              fputc(c,out);
              }
           fclose(out);
           }
        else {
           LOG_ERROR_STR(ChannelsConf);
           ErrorMsg(trNOOP("An error occurs, see syslog..."));
           }
        fclose(in);
        Channels.Load(ChannelsConf);
        Channels.SwitchTo(1);
        timers.RestoreTimer();
        if (ActionAfterSwitch == 1)
           state = osBack;
        else if (ActionAfterSwitch == 2)
           state = osEnd;
        else if (ActionAfterSwitch == 3)
           state = osContinue;
        else
           state = osChannels;
        }
     else {
        state = osBack;
        LOG_ERROR_STR(ToLoad);
        ErrorMsg(trNOOP("An error occurs, see syslog..."));
        }
     free(ToLoad);
     }
  return state;
  }

// rename a channellist
eOSState cSwitchMenu::RenameChannellist() {
  if (HasSubMenu() || Count() == 0) return osContinue;
  return AddSubMenu(new cMenuEditChannellist(switcher->List[Current()], trNOOP("Rename")));
}

// copy a channellist
eOSState cSwitchMenu::CopyChannellist() {
  if (HasSubMenu() || Count() == 0) return osContinue;
  return AddSubMenu(new cMenuEditChannellist(switcher->List[Current()], trNOOP("Copy")));
  }

// delete selected channellist
eOSState cSwitchMenu::DeleteChannellist() {
  if (Count() == 0) return osContinue;
  if (Interface->Confirm(tr("Delete channellist?"))) {
     char *ToLoad;
     asprintf(&ToLoad, "%s/channels.conf.%s", ConfigDir, switcher->List[Current()]);
//   RemoveFileOrDir(cString(ToLoad, true));
     remove(ToLoad);
     free(ToLoad);
     return osPlugin;
     }
  return osContinue;
  }

// save current channellist
eOSState cSwitchMenu::SaveChannellist() {
  if (HasSubMenu()) return osContinue;
  time_t seconds;
  seconds = time (NULL);
  return AddSubMenu(new cMenuEditChannellist(*DateTime(seconds), trNOOP("Save")));
  }

// execute update script
eOSState cSwitchMenu::UpdateChannellist() {
  if (Interface->Confirm(tr("Execute update script?"))) {
/* old
     cUpdateScriptThread *update = new cUpdateScriptThread();
     update->Start();
*/
     cExecuteCmdThread *executecmd = new cExecuteCmdThread(UpdateScript);
     executecmd->Start();
     ErrorMsg(trNOOP("Waiting..."));
     sleep(Setup.OSDMessageTime);
     return osPlugin;
     }
  return osContinue;
  }

// reload list / plugin
eOSState cSwitchMenu::FlushChannellist() {
  return osPlugin;
  }


eOSState cSwitchMenu::ProcessKey(eKeys Key) {
  eOSState state=cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
        case kOk:     return SwitchChannellist();
        case kRed: {
                   if (RedButtonSwitch)
                      return UpdateChannellist();
                   else
                      return RenameChannellist();
                   }
        case kGreen: {
                   if (GreenButtonSwitch)
                      return UpdateChannellist();
                   else
                      return CopyChannellist();
                   }
        case kYellow: return DeleteChannellist();
        case kBlue:   return SaveChannellist();
        default: break;
        }
     state = osContinue;
     }
  return state;
  }

void cSwitchMenu::SetHelp(void) {
  cOsdMenu::SetHelp(RedButtonSwitch ? tr("Button$Script") : tr("Button$Rename"), GreenButtonSwitch ? tr("Button$Script") : tr("Button$Copy"), tr("Button$Delete"), tr("Button$Save"));
  }

VDRPLUGINCREATOR(cPluginChannellists); // Don't touch this!

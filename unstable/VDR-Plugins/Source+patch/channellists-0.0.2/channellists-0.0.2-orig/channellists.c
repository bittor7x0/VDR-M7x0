/*
 * channellists.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/timers.h>
#include <vdr/interface.h>

#include "i18n.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = trNOOP("Manage your channellists");
static const char *MAINMENUENTRY  = trNOOP("Channellists");

int HideMainMenuEntry = 0;

class cMenuSetupChannellists : public cMenuSetupPage {
private:
  int newHideMainMenuEntry;
protected:
  virtual void Store(void);
public:
  cMenuSetupChannellists(void);
  };

cMenuSetupChannellists::cMenuSetupChannellists(void) {
  newHideMainMenuEntry = HideMainMenuEntry;
  Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMainMenuEntry));
  }

void cMenuSetupChannellists::Store(void) {
  SetupStore("HideMainMenuEntry", HideMainMenuEntry = newHideMainMenuEntry);
  }

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
  void ErrorMsg(const char *MyErrorMsg);
  eOSState SwitchChannellist();
  eOSState RenameChannellist();
  eOSState CopyChannellist();
  eOSState DeleteChannellist();
  eOSState SaveChannellist();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void SetHelp();
};


// --- myMenuRenameRecording --------------------------------------------------
class cMenuEditChannellist:public cOsdMenu {
private:
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
  }

const char *cPluginChannellists::CommandLineHelp(void) {
  // Return a string that describes all known command line options.
  return NULL;
  }

bool cPluginChannellists::ProcessArgs(int argc, char *argv[]) {
  // Implement command line argument processing here if applicable.
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
  if (!strcasecmp(Name, "HideMainMenuEntry")) HideMainMenuEntry = atoi(Value);
  else
     return false;
  return true;
  }

bool cPluginChannellists::Service(const char *Id, void *Data) {
  // Handle custom service requests from other plugins
  if (strcmp(Id, "Channellists-Switcher-v1.0") == 0) {
     if (Data == NULL)
        return true;
     cOsdMenu **menu = (cOsdMenu**) Data;
     if (menu)
        *menu = (cOsdMenu*) MainMenuAction();
     return true;
     }
  return false;
  }

const char **cPluginChannellists::SVDRPHelpPages(void) {
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "RELOAD [/path/to/channels.conf]\n"
    "    Reload channels.conf.",
    NULL
    };
  return HelpPages;
  }

cString cPluginChannellists::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
  // Process SVDRP commands this plugin implements
  if (strcasecmp(Command, "RELOAD") == 0) {
     char *ToReload;
     asprintf(&ToReload,"%s", cPlugin::ConfigDirectory());
     char *tmp = strrchr(ToReload,'/');
     *tmp = '\0';
     asprintf(&ToReload,"%s/channels.conf",ToReload);

     if ((*Option) && (strcmp(Option, ToReload)!= 0)) {
        char *ToLoad;
        int c;
        asprintf(&ToLoad,"%s",Option);
        FILE *in,*out;
        in=fopen(ToLoad,"r");
        if (in) {
           Channels.SwitchTo(1);
           TimerList timers;
           timers.SaveTimer();
           out=fopen(ToReload,"w");
           if (out) {
              while ((c=fgetc(in))!=EOF) {
                 fputc(c,out);
                 }
              fclose(out);
              }
           else {
              LOG_ERROR_STR(ToReload);
              }
           fclose(in);
           Channels.Load(ToReload);
           Channels.SwitchTo(1);
           timers.RestoreTimer();
           free(ToLoad);
           free(ToReload);
           return cString::sprintf("channels.conf successfully reloaded");
           }
        else {
           LOG_ERROR_STR(ToLoad);
           free (ToLoad);
           return cString::sprintf("error - see syslog for detailed information!");
           }
        }
     else {
//        asprintf(&ToReload,"%s/../channels.conf",cPlugin::ConfigDirectory());
//        Channels.SwitchTo(1);
        TimerList timers;
        timers.SaveTimer();
        Channels.Load(ToReload);
        Channels.SwitchTo(1);
        timers.RestoreTimer();
        free (ToReload);
        return cString::sprintf("channels.conf successfully reloaded");
        }
     }
  return NULL;
  }


// --- TimerList -------------------------------------------------------------
void TimerList::SaveTimer() {
// Timer Save and Restore Methods
  myTimers.Clear();
  for (cTimer *t = Timers.First(); t; t= Timers.Next(t)) {
     myTimers.Add(new TimerString(t->ToText(true)));
     }
  Timers.cList<cTimer>::Clear();
  }

void TimerList::RestoreTimer() {
// restore Timers, delete Timers with unknown channel
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

  strn0cpy(oldname,"",sizeof(oldname));
  strn0cpy(oldname,ChannellistName,sizeof(oldname));
  strn0cpy(newname,"",sizeof(newname));
  strn0cpy(newname,ChannellistName,sizeof(newname));

  Add(new cMenuEditStrItem(tr("Name"), newname, sizeof(newname), tr(FileNameChars)));
  cRemote::Put(kRight);
  }

cMenuEditChannellist::~cMenuEditChannellist() {
  }

// osd error message
void cMenuEditChannellist::ErrorMsg(const char *MyErrorMsg) {
  Skins.Message(mtStatus,tr(MyErrorMsg));
  sleep(Setup.OSDMessageTime);
  Display();
  }

eOSState cMenuEditChannellist::ProcessKey(eKeys Key) {
  eOSState state=cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
        case kOk: {
           if ((strcmp(newname, "") == 0) || ((strcmp(newname, oldname) == 0) && (savecurrent == 0)))
              return osContinue;

           int c;
           char *ToLoad;
           char *ToReload;

           asprintf(&ToReload,"%s/channellists/channels.conf.%s", cPlugin::ConfigDirectory(), newname);
           if (savecurrent)
              asprintf(&ToLoad,"%s/../channels.conf", cPlugin::ConfigDirectory());
           else
              asprintf(&ToLoad,"%s/channellists/channels.conf.%s", cPlugin::ConfigDirectory(), oldname);

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
  char *dir;
  asprintf(&dir,"%s/channellists/", cPlugin::ConfigDirectory());
  DIR *d = opendir(dir);
  if (d) {
     switcher=&SwitcherPlug;
     switcher->ListNbr=0;
//     switcher->List[SwitcherPlug.ListNbr]="";
     struct dirent *e;
     while ((e = readdir(d)) != NULL) {
        if ((!strncmp("channels.conf.",e->d_name,14)) && (strcmp("channels.conf.", e->d_name) != 0)) {
           char *name;
           asprintf(&name,"%s",e->d_name);
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
  else {
     if ((mkdir(dir, ACCESSPERMS)) == -1) {
        LOG_ERROR_STR(dir);
        ErrorMsg(trNOOP("Could not create dir"));
        }
     }
  free(dir);
  }

cSwitchMenu::~cSwitchMenu(void) {
  }

// osd error message
void cSwitchMenu::ErrorMsg(const char *MyErrorMsg) {
  Skins.Message(mtStatus,tr(MyErrorMsg));
  sleep(Setup.OSDMessageTime);
  Display();
  }

// switch to selected channellist
eOSState cSwitchMenu::SwitchChannellist() {
  eOSState state = osContinue;
  if (Count() == 0)
     return state;

  if (Interface->Confirm(tr("Switch current channellist?"))) {
     int c;
     char *ToLoad;
     char *ToReload;

     asprintf(&ToLoad, "%s/channellists/channels.conf.%s", cPlugin::ConfigDirectory(), switcher->List[Current()]);
     asprintf(&ToReload, "%s/../channels.conf", cPlugin::ConfigDirectory());

     FILE *in,*out;
     in=fopen(ToLoad,"r");
     if (in) {
        Channels.SwitchTo(1);
        TimerList timers;
        timers.SaveTimer();
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
        Channels.Load(ToReload);
        Channels.SwitchTo(1);
        timers.RestoreTimer();
        state = osPlugin;
        }
     else {
        state = osBack;
        LOG_ERROR_STR(ToLoad);
        ErrorMsg(trNOOP("An error occurs, see syslog..."));
        }
     free(ToLoad);
     free(ToReload);
     }
  return state;
  }

// rename a channellist
eOSState cSwitchMenu::RenameChannellist() {
  if (HasSubMenu() || Count() == 0)
     return osContinue;

  return AddSubMenu(new cMenuEditChannellist(switcher->List[Current()], trNOOP("Rename")));
}

// copy a channellist
eOSState cSwitchMenu::CopyChannellist() {
  if (HasSubMenu() || Count() == 0)
     return osContinue;

  return AddSubMenu(new cMenuEditChannellist(switcher->List[Current()], trNOOP("Copy")));
  }

// delete selected channellist
eOSState cSwitchMenu::DeleteChannellist() {
  if (Count() == 0)
     return osContinue;

  if (Interface->Confirm(tr("Delete channellist?"))) {
     char *ToLoad;
     asprintf(&ToLoad, "%s/channellists/channels.conf.%s", cPlugin::ConfigDirectory(), switcher->List[Current()]);
//   RemoveFileOrDir(cString(ToLoad, true));
     remove(ToLoad);
     free(ToLoad);
     return osPlugin;
     }
  return osContinue;
  }

// save current channellist
eOSState cSwitchMenu::SaveChannellist() {
  if (HasSubMenu())
     return osContinue;

  return AddSubMenu(new cMenuEditChannellist("save", trNOOP("Save")));
  }

eOSState cSwitchMenu::ProcessKey(eKeys Key) {
  eOSState state=cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
        case kOk:     return SwitchChannellist();
        case kRed:    return RenameChannellist();
        case kGreen:  return CopyChannellist();
        case kYellow: return DeleteChannellist();
        case kBlue:   return SaveChannellist();
        default: break;
        }
     state = osContinue;
     }
  return state;
  }

void cSwitchMenu::SetHelp(void) {
  cOsdMenu::SetHelp(tr("Button$Rename"), tr("Button$Copy"), tr("Button$Delete"), tr("Button$Save"));
  }

VDRPLUGINCREATOR(cPluginChannellists); // Don't touch this!

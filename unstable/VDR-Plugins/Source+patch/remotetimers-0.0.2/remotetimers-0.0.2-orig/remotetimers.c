/*
 * remotetimers.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "setup.h"
#include "menu.h"
#include "i18n.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = trNOOP("Edit timers on remote VDR");
static const char *MAINMENUENTRY  = trNOOP("Remote Timers");

class cPluginRemotetimers : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginRemotetimers(void);
  virtual ~cPluginRemotetimers();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return trREMOTETIMERS(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual const char *MainMenuEntry(void) { return RemoteTimersSetup.hideMainMenuEntry ? NULL : trREMOTETIMERS(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginRemotetimers::cPluginRemotetimers(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRemotetimers::~cPluginRemotetimers()
{
  // Clean up after yourself!
}

const char *cPluginRemotetimers::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRemotetimers::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRemotetimers::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginRemotetimers::Start(void)
{
  // Start any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  return true;
}

void cPluginRemotetimers::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRemotetimers::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginRemotetimers::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginRemotetimers::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginRemotetimers::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new ::PluginRemoteTimers::cMenuMain(tr(MAINMENUENTRY));
}

cMenuSetupPage *cPluginRemotetimers::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cRemoteTimersMenuSetup();
}

bool cPluginRemotetimers::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return RemoteTimersSetup.Parse(Name, Value);;
}

bool cPluginRemotetimers::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  cOsdMenu **menu = (cOsdMenu**) Data;
  if (RemoteTimersSetup.replaceSchedule && strcmp(Id, "MainMenuHooksPatch-v1.0::osSchedule") == 0) {
	  if (menu) 
		  *menu = new ::PluginRemoteTimers::cMenuSchedule();
	  return true;
  }
  if (RemoteTimersSetup.replaceTimers && strcmp(Id, "MainMenuHooksPatch-v1.0::osTimers") == 0) {
	  if (menu) 
		  *menu = new ::PluginRemoteTimers::cMenuTimers();
	  return true;
  }
  return false;
}

const char **cPluginRemotetimers::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginRemotetimers::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginRemotetimers); // Don't touch this!

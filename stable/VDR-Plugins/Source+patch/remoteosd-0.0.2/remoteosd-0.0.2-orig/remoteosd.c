/*
 * remoteosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/skins.h>
#include <vdr/themes.h>
#include <strings.h>
#include <stdlib.h>
#include "i18n.h"
#include "menu.h"
#include "setup.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "Show menu of a remote VDR";
static const char *MAINMENUENTRY  = "Remote menu";

class cPluginRemoteOsd : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginRemoteOsd(void);
  virtual ~cPluginRemoteOsd();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return RemoteOsdSetup.hideMainMenuEntry ? NULL : tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginRemoteOsd::cPluginRemoteOsd(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRemoteOsd::~cPluginRemoteOsd()
{
  // Clean up after yourself!
}

const char *cPluginRemoteOsd::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRemoteOsd::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRemoteOsd::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginRemoteOsd::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  return true;
}

void cPluginRemoteOsd::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRemoteOsd::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginRemoteOsd::MainMenuAction(void)
{
  cRemoteOsdMenu *menu = new cRemoteOsdMenu(tr(MAINMENUENTRY));
  if (!menu->Open(RemoteOsdSetup.serverIp, (unsigned short) RemoteOsdSetup.serverPort, "MENU")) {
	  DELETENULL(menu);
	  Skins.Message(mtError, tr("Remote menu not available. Connection failed."));
  }
  return menu;
}

cMenuSetupPage *cPluginRemoteOsd::SetupMenu(void)
{
  return new cRemoteOsdMenuSetup();
}

bool cPluginRemoteOsd::SetupParse(const char *Name, const char *Value)
{
  if (!strcasecmp(Name, "HideMainMenuEntry"))
	  RemoteOsdSetup.hideMainMenuEntry = atoi(Value);
  else if (!strcasecmp(Name, "ServerIp"))
	  strn0cpy(RemoteOsdSetup.serverIp, Value, sizeof(RemoteOsdSetup.serverIp));
  else if (!strcasecmp(Name, "ServerPort"))
	  RemoteOsdSetup.serverPort = atoi(Value);
  else if (!strcasecmp(Name, "TuneServer"))
	  RemoteOsdSetup.tuneServer = atoi(Value);
  else if (!strcasecmp(Name, "MaxItems"))
	  RemoteOsdSetup.maxItems = atoi(Value);
  else if (!strcasecmp(Name, "RemoteTheme"))
	  strn0cpy(RemoteOsdSetup.remoteTheme, Value, sizeof(RemoteOsdSetup.remoteTheme));
  else
	  return false;
  return true;
}

bool cPluginRemoteOsd::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  cRemoteOsdMenu **menu = (cRemoteOsdMenu**) Data;
  if (RemoteOsdSetup.replaceSchedule && strcmp(Id, "MainMenuHooksPatch-v1.0::osSchedule") == 0) {
	  if (menu) {
		  *menu = new cRemoteOsdMenu(RemoteOsdSetup.serverIp);
		  if (!(*menu)->Open(RemoteOsdSetup.serverIp, (unsigned short) RemoteOsdSetup.serverPort, "SCHEDULE")) {
			  DELETENULL(*menu);
			  Skins.Message(mtError, tr("Remote menu not available. Connection failed."));
		  }
	  }
	  return true;
  }
  if (RemoteOsdSetup.replaceTimers && strcmp(Id, "MainMenuHooksPatch-v1.0::osTimers") == 0) {
	  if (menu) {
		  *menu = new cRemoteOsdMenu(RemoteOsdSetup.serverIp);
		  if (!(*menu)->Open(RemoteOsdSetup.serverIp, (unsigned short) RemoteOsdSetup.serverPort, "TIMERS")) {
			  DELETENULL(*menu);
			  Skins.Message(mtError, tr("Remote menu not available. Connection failed."));
		  }
	  }
	  return true;
  }
  return false;
}

const char **cPluginRemoteOsd::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
};

cString cPluginRemoteOsd::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginRemoteOsd); // Don't touch this!

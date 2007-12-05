/*
 * epgsync.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "i18n.h"
#include "setup.h"
#include "thread.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "Import EPG of an other VDR";
static const char *MAINMENUENTRY  = "Synchronize EPG";

class cPluginEpgsync : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cEpgSyncThread *EpgSyncThread;
public:
  cPluginEpgsync(void);
  virtual ~cPluginEpgsync();
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
  virtual const char *MainMenuEntry(void) { return EpgSyncSetup.hideMainMenuEntry ? NULL: tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginEpgsync::cPluginEpgsync(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  EpgSyncThread = NULL;
}

cPluginEpgsync::~cPluginEpgsync()
{
  // Clean up after yourself!
}

const char *cPluginEpgsync::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginEpgsync::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginEpgsync::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  EpgSyncThread = new cEpgSyncThread();
  return true;
}

bool cPluginEpgsync::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  if (EpgSyncSetup.syncOnStart) {
	EpgSyncThread->Start();
  }
  return true;
}

void cPluginEpgsync::Stop(void)
{
  // Stop any background activities the plugin shall perform.
  DELETENULL(EpgSyncThread);
}

void cPluginEpgsync::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginEpgsync::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginEpgsync::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginEpgsync::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  EpgSyncThread->Start();
  return NULL;
}

cMenuSetupPage *cPluginEpgsync::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cEpgSyncMenuSetup();
}

bool cPluginEpgsync::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return EpgSyncSetup.Parse(Name, Value);
}

bool cPluginEpgsync::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginEpgsync::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginEpgsync::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginEpgsync); // Don't touch this!

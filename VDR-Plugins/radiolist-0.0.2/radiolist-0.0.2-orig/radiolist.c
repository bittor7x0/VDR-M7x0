/*
 * radiolist.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>

#include "channel-menu.h"
#include "setup-radiolist.h"
#include "i18n.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "Channellist for radio channels";
static const char *MAINMENUENTRY  = "Radiochannels";

class cPluginRadiolist : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginRadiolist(void);
  virtual ~cPluginRadiolist();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
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
  };

cPluginRadiolist::cPluginRadiolist(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRadiolist::~cPluginRadiolist()
{
  // Clean up after yourself!
}

const char *cPluginRadiolist::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRadiolist::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRadiolist::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginRadiolist::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  
  return true;
}

void cPluginRadiolist::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRadiolist::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

const char *cPluginRadiolist::MainMenuEntry(void) {
    if (RadiolistSetup.HideMainMenu == 1)
	return NULL;
    else
        return tr(MAINMENUENTRY); 
}

cOsdObject *cPluginRadiolist::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.  
  return new cRadioListMenu();
}

cMenuSetupPage *cPluginRadiolist::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupRadiolist();
}

bool cPluginRadiolist::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return RadiolistSetup.SetupParse( Name, Value);
}

bool cPluginRadiolist::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRadiolist::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginRadiolist::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginRadiolist); // Don't touch this!

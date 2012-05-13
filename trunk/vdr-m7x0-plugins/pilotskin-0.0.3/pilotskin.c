/*
 * pilotskin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "pilotskinosd.h"
#include "pilotskinsetup.h"

#include <vdr/plugin.h>

#if VDRVERSNUM < 10507
#include "i18n.h"
#define trNOOP(s) (s)
#endif

static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = trNOOP("A zapping co-pilot");
static const char *MAINMENUENTRY  = trNOOP("Pilotskin");

class cPluginPilotskin : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginPilotskin(void);
  virtual ~cPluginPilotskin();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) {
		return config.hidemenu ? NULL : tr(MAINMENUENTRY);
	}
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

int PilotChannelNumber;

cPluginPilotskin::cPluginPilotskin(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  config.closeonswitch=1;
  config.hidemenu=0;
}

cPluginPilotskin::~cPluginPilotskin()
{
  // Clean up after yourself!
}

const char *cPluginPilotskin::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginPilotskin::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginPilotskin::Start(void)
{
  // Start any background activities the plugin shall perform.
#if VDRVERSNUM < 10507  
  RegisterI18n(Phrases);
#endif
  // Default values for setup
  return true;
}

void cPluginPilotskin::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginPilotskin::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cPilotskinOsd;
}

cMenuSetupPage *cPluginPilotskin::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cPilotskinSetup;
}

bool cPluginPilotskin::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(Name, "CloseOnSwitch"))         config.closeonswitch = atoi(Value);
  else if (!strcasecmp(Name, "HideMenu"))              config.hidemenu = atoi(Value);
  else
     return false;
  
  return true;
}

sPilotskinConfig config;

VDRPLUGINCREATOR(cPluginPilotskin); // Don't touch this!

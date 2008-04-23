/****************************************************************************
 * DESCRIPTION: 
 *             Setup a Plugin vor VDR
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 ****************************************************************************/

#include <vdr/plugin.h>
#include "setupmenu.h"
//not needed for open7x0 setup
//#include "setupsetup.h"
#include "i18n.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "open7x0-Setup (based on Minivdr-Setup)";
static const char *MAINMENUENTRY  = "Gigaset 7x0 Firmware Setup";

//holds setup configuration
//not needed for open7x0 setup
//cSetupSetup  setupSetup;

class cPluginSetup : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginSetup(void);
  virtual ~cPluginSetup();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginSetup::cPluginSetup(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginSetup::~cPluginSetup()
{
  // Clean up after yourself!
 
}

const char *cPluginSetup::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginSetup::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginSetup::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  debug("cPluginSetup::Initialize => Init");
  RegisterI18n(Phrases);
  return true;
}

bool cPluginSetup::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginSetup::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginSetup::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  isyslog("cPluginSetup => MainMenuAction");
  return (new cSetupMenu());
}

cMenuSetupPage *cPluginSetup::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  
  //not needed for open7x0 setup
  //return new cSetupSetupPage;
	return NULL;
}

bool cPluginSetup::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  //not needed for open7x0 setup
  //return setupSetup.SetupParse(Name, Value);
  return false;
}



VDRPLUGINCREATOR(cPluginSetup); // Don't touch this!

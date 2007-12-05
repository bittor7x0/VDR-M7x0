/*
 * pilotskin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "pilotskin.h"
#include "pilotskinosd.h"
#include "pilotskinsetup.h"
#include "i18n.h"

int PilotChannelNumber;

cPluginPilotskin::cPluginPilotskin(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  config.red=50;
  config.green=50;
  config.blue=200;
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
  RegisterI18n(Phrases);
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
  if (!strcasecmp(Name, "Red"))                        config.red = atoi(Value);
  else if (!strcasecmp(Name, "Green"))                 config.green = atoi(Value);
  else if (!strcasecmp(Name, "Blue"))                  config.blue = atoi(Value);
  else if (!strcasecmp(Name, "CloseOnSwitch"))         config.closeonswitch = atoi(Value);
  else if (!strcasecmp(Name, "HideMenu"))              config.hidemenu = atoi(Value);
  else
     return false;
  
  return true;
}

sPilotskinConfig config;

VDRPLUGINCREATOR(cPluginPilotskin); // Don't touch this!

/*
 * pilot.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "pilot.h"
#include "pilotosd.h"
#include "pilotsetup.h"
#include "i18n.h"
#include <vdr/device.h>

int PilotChannelNumber;

cPluginPilot::cPluginPilot(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  config.width=400;
  config.red=50;
  config.green=50;
  config.blue=200;
  config.alpha1=128;
  config.alpha2=255;
  config.originx=50;
  config.originy=350;
  config.alphaborder=255;
  config.closeonswitch=1;
  config.usedxr3=0;
}

cPluginPilot::~cPluginPilot()
{
  // Clean up after yourself!
}

const char *cPluginPilot::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginPilot::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginPilot::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  // Default values for setup
  return true;
}

void cPluginPilot::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginPilot::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  config.height=5 * (LINEHEIGHT + 4);
  return new cPilotOsd;
}

cMenuSetupPage *cPluginPilot::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cPilotSetup;
}

bool cPluginPilot::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "Width"))                 config.width = atoi(Value);
  else if (!strcasecmp(Name, "Red"))                   config.red = atoi(Value);
  else if (!strcasecmp(Name, "Green"))                 config.green = atoi(Value);
  else if (!strcasecmp(Name, "Blue"))                  config.blue = atoi(Value);
  else if (!strcasecmp(Name, "Alpha1"))                config.alpha1 = atoi(Value);
  else if (!strcasecmp(Name, "Alpha2"))                config.alpha2 = atoi(Value);
  else if (!strcasecmp(Name, "OriginX"))               config.originx = atoi(Value);
  else if (!strcasecmp(Name, "OriginY"))               config.originy = atoi(Value);
  else if (!strcasecmp(Name, "AlphaBorder"))           config.alphaborder = atoi(Value);
  else if (!strcasecmp(Name, "CloseOnSwitch"))         config.closeonswitch = atoi(Value);
  else if (!strcasecmp(Name, "UseDXR3"))               config.usedxr3 = atoi(Value);
  else if (!strcasecmp(Name, "HideMenu"))              config.hidemenu = atoi(Value);
  else
     return false;
  
  return true;
}

sPilotConfig config;

VDRPLUGINCREATOR(cPluginPilot); // Don't touch this!

/*
 * fepg.c: An EPG plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/interface.h>

#include "fepg.h"
#include "fepgOsd.h"
#include "menu.h"
#include "config.h"
#include "tools.h"
#include "screenMenu.h"
#include "colorMenu.h"
#include "i18n.h"


//////////////////////////////////////////////////////////////////////////////


static const char* VERSION        = "0.4.0";
static const char* MAINMENUENTRY  = "fEPG";


//////////////////////////////////////////////////////////////////////////////


cPluginFepg::cPluginFepg(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  
  // Default configuration values
  config.keyUp       = 0;
  config.keyDown     = 1;
  config.keyLeft     = 5;
  config.keyRight    = 6;
  config.keyInfo     = 8;
  config.keyPageUp   = 7;
  config.keyPageDown = 9;
  config.keyBack     = 4;
  config.keyOk       = 3;
  config.keyRecord   = 10;

  config.clr1Red   = 52;
  config.clr1Green = 79;
  config.clr1Blue  = 96;
  config.clr2Red   = 57;
  config.clr2Green = 96;
  config.clr2Blue  = 52;
  config.trans     = 255;
  
  config.mute = false;
  config.centerInfo = true;
  config.timeout = 1000; // 1 sec
  config.chanOrder = ASCENDING;
  config.timeFormat24h = true;
  config.hideMenuEntry = false;
  config.useSmallFont = false;
  config.TVMode = NTSC;
  config.sX1 = 60;
  config.sY1 = 20;
  config.sX2 = 660;  
  config.sY2 = 460;
  config.chanWidth  = 125;
  config.timeHeight = 40;
  config.numRows = 6;
  config.numCols = 3;
  config.timeInterval = 30;
  config.darkenBg = true;
  
  requestScreenMenu = false;
  requestColorMenu  = false;
}


//----------------------------------------------------------------------------

cPluginFepg::~cPluginFepg()
{
  // Clean up after yourself!
}


//----------------------------------------------------------------------------

const char* cPluginFepg::Version(void) { return VERSION; }


//----------------------------------------------------------------------------

const char* cPluginFepg::Description(void) { return tr("A Graphical EPG"); }


//----------------------------------------------------------------------------

const char *cPluginFepg::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}


//----------------------------------------------------------------------------

bool cPluginFepg::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}


//----------------------------------------------------------------------------

bool cPluginFepg::Start(void)
{
  // Start any background activities the plugin shall perform.
#if VDRVERSNUM < 10507  
  RegisterI18n(fepgPhrases);
#endif  
  return true;
}


//----------------------------------------------------------------------------

void cPluginFepg::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}


//----------------------------------------------------------------------------

const char* cPluginFepg::MainMenuEntry(void) 
{ 
  return config.hideMenuEntry ? NULL : MAINMENUENTRY; 
}


//----------------------------------------------------------------------------

cOsdObject *cPluginFepg::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.

  if      (requestScreenMenu) return new cScreenMenu;
  else if (requestColorMenu)  return new cColorMenu;
  else
    return new cFEpgOsd;
}


//----------------------------------------------------------------------------

cMenuSetupPage *cPluginFepg::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupFEpg;
}


//----------------------------------------------------------------------------

bool cPluginFepg::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "keyUp"))         config.keyUp      = atoi(Value);
  else if (!strcasecmp(Name, "keyDown"))       config.keyDown    = atoi(Value);
  else if (!strcasecmp(Name, "keyLeft"))       config.keyLeft    = atoi(Value);
  else if (!strcasecmp(Name, "keyRight"))      config.keyRight   = atoi(Value);
  else if (!strcasecmp(Name, "keyInfo"))       config.keyInfo    = atoi(Value);
  else if (!strcasecmp(Name, "keyPageUp"))     config.keyPageUp  = atoi(Value);
  else if (!strcasecmp(Name, "keyPageDown"))   config.keyPageDown= atoi(Value);
  else if (!strcasecmp(Name, "keyRecord"))     config.keyRecord  = atoi(Value);
  else if (!strcasecmp(Name, "keyBack"))       config.keyBack    = atoi(Value);
  else if (!strcasecmp(Name, "keyOk"))         config.keyOk      = atoi(Value);
  else if (!strcasecmp(Name, "mute"))          config.mute       = atoi(Value);
  else if (!strcasecmp(Name, "centerInfo"))    config.centerInfo = atoi(Value);
  else if (!strcasecmp(Name, "clr1Red"))       config.clr1Red    = atoi(Value);
  else if (!strcasecmp(Name, "clr1Green"))     config.clr1Green  = atoi(Value);
  else if (!strcasecmp(Name, "clr1Blue"))      config.clr1Blue   = atoi(Value);
  else if (!strcasecmp(Name, "clr2Red"))       config.clr2Red    = atoi(Value);
  else if (!strcasecmp(Name, "clr2Green"))     config.clr2Green  = atoi(Value);
  else if (!strcasecmp(Name, "clr2Blue"))      config.clr2Blue   = atoi(Value);
  else if (!strcasecmp(Name, "trans"))         config.trans      = atoi(Value);
  else if (!strcasecmp(Name, "timeout"))       config.timeout    = atoi(Value); 
  else if (!strcasecmp(Name, "chanOrder"))     config.chanOrder  = atoi(Value);
  else if (!strcasecmp(Name, "timeFormat24h")) config.timeFormat24h = atoi(Value);
  else if (!strcasecmp(Name, "hideMenuEntry")) config.hideMenuEntry = atoi(Value);
  else if (!strcasecmp(Name, "TVMode"))        config.TVMode = atoi(Value);
  else if (!strcasecmp(Name, "sX1"))           config.sX1 = atoi(Value);
  else if (!strcasecmp(Name, "sX2"))           config.sX2 = atoi(Value);
  else if (!strcasecmp(Name, "sY1"))           config.sY1 = atoi(Value);
  else if (!strcasecmp(Name, "sY2"))           config.sY2 = atoi(Value);
  else if (!strcasecmp(Name, "numRows"))       config.numRows = atoi(Value);
  else if (!strcasecmp(Name, "numCols"))       config.numCols = atoi(Value);
  else if (!strcasecmp(Name, "useSmallFont"))  config.useSmallFont = atoi(Value);
  else if (!strcasecmp(Name, "timeInterval"))  config.timeInterval = atoi(Value);
  else if (!strcasecmp(Name, "darkenBg"))      config.darkenBg = atoi(Value);
  else if (!strcasecmp(Name, "chanWidth"))     config.chanWidth = atoi(Value);
  else if (!strcasecmp(Name, "timeHeight"))    config.timeHeight = atoi(Value);
  else
    return false;
     
  return true;
}


//////////////////////////////////////////////////////////////////////////////


sFEpgConfig config;

bool requestScreenMenu;
bool requestColorMenu;


//////////////////////////////////////////////////////////////////////////////


VDRPLUGINCREATOR(cPluginFepg); // Don't touch this!


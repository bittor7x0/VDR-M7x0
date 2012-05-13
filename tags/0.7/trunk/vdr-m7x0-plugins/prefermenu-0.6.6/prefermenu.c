/*
 * prefermenu.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "prefermenu.h"
#include "preferosd.h"
#include "prefersetup.h"
#include "preferchannel.h"
#include "i18n.h"

extern cPreferedChannels PreferedChannelsList;
extern cPreferedChannels PreferedChannelsListDisplay;

cPluginPrefermenu::cPluginPrefermenu(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  config.width=300;
  config.originx=50;
  config.originy=300;
  config.closeonswitch=1;
  config.sortby=1;
  config.lines=7;
}

cPluginPrefermenu::~cPluginPrefermenu()
{
  // Clean up after yourself!
}

const char *cPluginPrefermenu::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginPrefermenu::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginPrefermenu::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  // Default values for setup
  return true;
}

void cPluginPrefermenu::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginPrefermenu::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  // Load prefered channels list
  if(!PreferedChannelsList.Load(AddDirectory(ConfigDirectory(),"prefermenu.conf"))) {
    // File load failed
    isyslog("prefermenu.conf file not found");
  }
  // Create the "displayed" channel list (which can be sorted)
  if(!PreferedChannelsListDisplay.Load(AddDirectory(ConfigDirectory(),"prefermenu.conf"))) {
    // Saving after loading allows to clean the file
    // File load failed
    // But just log once
  }
  // Clean up the file with current VDR channels.conf
  PreferedChannelsList.Save();
  config.height= (config.lines * (LINEHEIGHT+BORDERSIZE+2))+BORDERSIZE;
  return new cPreferOsd;
}

cMenuSetupPage *cPluginPrefermenu::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cPreferSetup;
}

bool cPluginPrefermenu::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "Width"))                 config.width = atoi(Value);
  else if (!strcasecmp(Name, "Lines"))                 config.lines = atoi(Value);
  else if (!strcasecmp(Name, "OriginX"))               config.originx = atoi(Value);
  else if (!strcasecmp(Name, "OriginY"))               config.originy = atoi(Value);
  else if (!strcasecmp(Name, "CloseOnSwitch"))         config.closeonswitch = atoi(Value);
  else if (!strcasecmp(Name, "SortBy"))                config.sortby = atoi(Value);
  else
     return false;
  
  return true;
}

sPreferConfig config;

VDRPLUGINCREATOR(cPluginPrefermenu); // Don't touch this!

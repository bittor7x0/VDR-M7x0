/*
 * zappilot.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "zappilotosd.h"
#include "zappilotsetup.h"

#include <vdr/plugin.h>

static const char *VERSION        = "0.0.5";
static const char *DESCRIPTION    = trNOOP("A zapping co-pilot");
static const char *MAINMENUENTRY  = trNOOP("ZapPilot");

class cPluginZappilot : public cPlugin
{
   private:
      // Add any member variables or functions you may need here.
   public:
      cPluginZappilot(void);
      virtual ~cPluginZappilot();
      virtual const char *Version(void) { return VERSION; }
      virtual const char *Description(void) { return tr(DESCRIPTION); }
      virtual const char *CommandLineHelp(void);
      virtual bool ProcessArgs(int argc, char *argv[]);
      virtual bool Start(void);
      virtual void Housekeeping(void);
      virtual const char *MainMenuEntry(void)
      {
         return config.hidemenu ? NULL : tr(MAINMENUENTRY);
      }
      virtual cOsdObject *MainMenuAction(void);
      virtual cMenuSetupPage *SetupMenu(void);
      virtual bool SetupParse(const char *Name, const char *Value);
};

int PilotChannelNumber;

cPluginZappilot::cPluginZappilot(void)
{
   // Initialize any member variables here.
   // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
   // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
   config.closeonswitch=1;
   config.hidemenu=0;
   config.fastbrowse=0;
   config.fastbrowsealt=0;
   config.switchtimer=0;
   config.switchminsbefore=1;
   config.pEPGSearch=NULL;
}


cPluginZappilot::~cPluginZappilot()
{
   // Clean up after yourself!
}


const char *cPluginZappilot::CommandLineHelp(void)
{
   // Return a string that describes all known command line options.
   return NULL;
}


bool cPluginZappilot::ProcessArgs(int argc, char *argv[])
{
   // Implement command line argument processing here if applicable.
   return true;
}


bool cPluginZappilot::Start(void)
{
   // Start any background activities the plugin shall perform.
   // Default values for setup
   config.pEPGSearch = cPluginManager::GetPlugin("epgsearch");
   if (!config.pEPGSearch)
   {
      config.switchtimer=0;
      esyslog("[ZapPilot] EPGSearch does not exist; switch timers are disabled!");
   }
   return true;
}


void cPluginZappilot::Housekeeping(void)
{
   // Perform any cleanup or other regular tasks.
}


cOsdObject *cPluginZappilot::MainMenuAction(void)
{
   // Perform the action when selected from the main VDR menu.
   return new cZappilotOsd;
}


cMenuSetupPage *cPluginZappilot::SetupMenu(void)
{
   // Return a setup menu in case the plugin supports one.
   return new cZappilotSetup;
}


bool cPluginZappilot::SetupParse(const char *Name, const char *Value)
{
   // Parse your own setup parameters and store their values.
   if (!strcasecmp(Name, "CloseOnSwitch")) config.closeonswitch = atoi(Value);
   else if (!strcasecmp(Name, "HideMenu")) config.hidemenu = atoi(Value);
   else if (!strcasecmp(Name, "FastBrowse")) config.fastbrowse = atoi(Value);
   else if (!strcasecmp(Name, "FastBrowseAlt")) config.fastbrowsealt = atoi(Value);
   else if (!strcasecmp(Name, "SwitchTimer")) config.switchtimer = atoi(Value);
   else if (!strcasecmp(Name, "SwitchMinsBefore")) config.switchminsbefore = atoi(Value);
   else
      return false;

   return true;
}


sZappilotConfig config;

// Don't touch this!
VDRPLUGINCREATOR(cPluginZappilot);

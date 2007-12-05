/*
 * zaphistory.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "zaphistory.h"
#include "setup-zaphistory.h"
#include "zaphistoryosd.h"
#include "i18n.h"

static const char *VERSION = "0.9.5";
static const char *MAINMENUENTRY = "Zapping history";
static const char *DESCRIPTION = "History of the last zapped channels";

const char *cPluginZaphistory::Version(void) {
    return VERSION;
}

const char *cPluginZaphistory::Description(void) {
    return tr(DESCRIPTION);
}

cPluginZaphistory::cPluginZaphistory(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  monitor = NULL;
}

cPluginZaphistory::~cPluginZaphistory()
{
  // Clean up after yourself!
}

const char *cPluginZaphistory::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginZaphistory::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginZaphistory::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

const char *cPluginZaphistory::MainMenuEntry(void) {
    if ( ZapHistorySetup.HideMainMenu == 1 )
	return NULL;
    
    return tr( tr(MAINMENUENTRY) );
}

bool cPluginZaphistory::Start(void)
{
  // Load stored history
  ZapHistory.Load( AddDirectory( ConfigDirectory(), "zaphistory.conf" ));
    
  monitor = new cZapHistoryMonitor();
  // Start any background activities the plugin shall perform.
  RegisterI18n( Phrases );
  
  return true;
}

void cPluginZaphistory::Stop(void)
{
  // Save history
  ZapHistory.UpdateWatchTime();
  ZapHistory.Save();
  // Stop any background activities the plugin shall perform.
  delete monitor;
}

void cPluginZaphistory::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginZaphistory::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cMenuZappedChannels();
}

cMenuSetupPage *cPluginZaphistory::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupZapHistory();
}

bool cPluginZaphistory::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return ZapHistorySetup.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cPluginZaphistory); // Don't touch this!

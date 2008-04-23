/*
 * $Id: pim.c,v 1.2 2006/05/11 20:00:34 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: pim.c,v $
 * Revision 1.2  2006/05/11 20:00:34  sundin
 * Introduced APIVERSION for VDR-1.4, version bumped to 0.0.6
 *
 * Revision 1.1.1.1  2005/02/17 05:45:53  sundin
 * initial import ver0.0.5
 *
 * Revision 1.2  2005/01/17 18:29:49  ksltuf
 * added finnish translations
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.5  2004/12/13 07:59:04  ksltuf
 * bugfix setup parsing; new option firstIsMonday
 *
 * Revision 1.4  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.3  2004/08/21 12:33:17  ksltuf
 * set version to 0.0.2
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include <string>

#include "pim.h"
#include "i18n.h"
#include "calManager.h"
#include "calEvent.h"
#include "eventListMenu.h"
#include "eventSetupMenu.h"
#include "pimOsdMenu.h"
#include "pimSetup.h"
#include "searchevents_thread.h"


// seconds the next timer must be in the future to assume manual start
#define MANUALSTART	600

cPluginPim *pluginPIM = NULL;

cPluginPim::cPluginPim(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  pluginPIM = this;
}

cPluginPim::~cPluginPim()
{
  // Clean up after yourself!
}

const char *cPluginPim::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginPim::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginPim::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  RegisterI18n(pimPhrases);
  return true;
}

bool cPluginPim::Start(void)
{
  // Start any background activities the plugin shall perform.
  time_t t = time(NULL);

  globalSetup.ConfigDir = strdup(cPlugin::ConfigDirectory());

  // no search action if not called by SVDRP if:
  // 1) no event reminder at all
  // 2) timer start assumed and not 'time mode daily'
  if ((globalSetup.reminderMode == 0) ||
      ((globalSetup.reminderMode != 3) && isTimerStartMode(t)))
  {
    isyslog("PIM: no event search, reason: %s",
            !globalSetup.reminderMode ? "disabled" : "timer start");
  }
  else
    cSearchEventsThread::Init();

  return true;
}

void cPluginPim::Stop(void)
{
  cSearchEventsThread::Exit();		// automatic search thread
  cSearchEventsThread::Exit(true);	// svdrp search thread
}

void cPluginPim::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginPim::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return  new cPimOsdMenu();
}

cMenuSetupPage *cPluginPim::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cEventSetupMenu();
}

bool cPluginPim::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if ( !strcasecmp(Name, "OsdOffsetX") )
    globalSetup.osdOffsetX = atoi(Value);
  else
  if ( !strcasecmp(Name, "OsdOffsetY") )
    globalSetup.osdOffsetY = atoi(Value);
  else
  if ( !strcasecmp(Name, "FirstIsMonday") )
    globalSetup.firstIsMonday = atoi(Value);
  else
  if ( !strcasecmp(Name, "ReminderMode") )
    globalSetup.reminderMode = atoi(Value);
  else
  if ( !strcasecmp(Name, "ActivityMode") )
    globalSetup.activityMode = atoi(Value);
  else
  if ( !strcasecmp(Name, "ReminderTime") )
    globalSetup.reminderTime = atoi(Value);
  else
  if ( !strcasecmp(Name, "RemindDelay") )
    globalSetup.remindDelay = atoi(Value);
  else
  if ( !strcasecmp(Name, "ShowCalendarWeek") )
    globalSetup.showCW = atoi(Value);
  else
  if ( !strcasecmp(Name, "PreviewLastNextWeek") )
    globalSetup.previewLastNextW = atoi(Value);
  else
  if ( !strcasecmp(Name, "InversEventColor") )
    globalSetup.inversEventColor = atoi(Value);
  else
    return false;

  return true;
}

const char **cPluginPim::SVDRPHelpPages(void)
{ 
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "CHECK\n"
    "    Display all next events on OSD (respect pimEvents.marks).",
    "DISP\n"
    "    Display all next events on OSD.",
    "REMOVE\n"
    "    Remove all old events.",
    NULL
    };
  return HelpPages;
}

cString cPluginPim::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  bool bRet = true;
  if (strcasecmp(Command, "CHECK") == 0) {
    bRet = cSearchEventsThread::Init(STA_svdrp_check);
  }
  else if (strcasecmp(Command, "DISP") == 0) {
    bRet = cSearchEventsThread::Init(STA_svdrp_display);
  }
  else if (strcasecmp(Command, "REMOVE") == 0) {	// delete all old events
    bRet = cSearchEventsThread::Init(STA_remove);
  }
  else
    return cString::sprintf("Unknown command [%s]", Command);

  return bRet ? cString::sprintf("Command [%s] done", Command) :
		cString("Another SVDRP command is still running");
}

// assumption of manual / automatic start of vdr
bool cPluginPim::isTimerStartMode(time_t Now)
{
  cTimer *timer = Timers.GetNextActiveTimer();
  if (!timer)
     return false;	// no timer at all

  time_t Delta = timer->StartTime() - Now;
  if (Delta > MANUALSTART)
     return false;	// assume manual start

  return true;
}

VDRPLUGINCREATOR(cPluginPim); // Don't touch this!

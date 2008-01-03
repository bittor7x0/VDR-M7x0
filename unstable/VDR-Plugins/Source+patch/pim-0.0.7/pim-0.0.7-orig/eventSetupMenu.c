/*
 * $Id: eventSetupMenu.c,v 1.1.1.1 2005/02/17 05:45:52 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: eventSetupMenu.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:52  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.4  2004/12/13 07:58:12  ksltuf
 * new setup option 'firstIsMonday'
 *
 * Revision 1.3  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include "eventSetupMenu.h"
#include "i18n.h"
#include "pimSetup.h"
#include "searchevents_thread.h"

#include <vdr/menuitems.h>


cEventSetupMenu::cEventSetupMenu(void)
{
  _osdOffsetX = globalSetup.osdOffsetX;
  _osdOffsetY = globalSetup.osdOffsetY;
  _firstIsMonday = globalSetup.firstIsMonday;
  _reminderMode = globalSetup.reminderMode;
  _reminderTime = globalSetup.reminderTime;
  _remindDelay = globalSetup.remindDelay;
  _activityMode = globalSetup.activityMode;
  _previewLastNextW = globalSetup.previewLastNextW;
  _inversEventColor = globalSetup.inversEventColor;
  _showCW = globalSetup.showCW;
  _last_remind = -1;

  Add(new cMenuEditIntItem(tr("OSD Offset hor."),  &_osdOffsetX,-150, 150));
  Add(new cMenuEditIntItem(tr("OSD Offset vert."), &_osdOffsetY,-150, 150));
  Add(new cMenuEditBoolItem(tr("Week starts with"), &_firstIsMonday, tr("Su"), tr("Mo")));
  Add(new cMenuEditBoolItem(tr("Show calendar weeks"), &_showCW, tr("off"), tr("on")));
  Add(new cMenuEditBoolItem(tr("Preview months"), &_previewLastNextW, tr("off"), tr("on")));
  Add(new cMenuEditBoolItem(tr("Use invers event color"), &_inversEventColor, tr("off"), tr("on")));

  reminderTexts[0] = tr("off");
  reminderTexts[1] = tr("on");
  reminderTexts[2] = tr("at time");
  reminderTexts[3] = tr("at time (daily)");
  Add(new cMenuEditStraItem(tr("Reminder mode"), &_reminderMode, 4, reminderTexts));
  Add(_timeItem = new cMenuEditTimeItem(tr("Reminder time"), &_reminderTime));
  Add(_delayItem = new cMenuEditIntItem(tr("Remind delay (seconds)"), &_remindDelay, 0, 300));
  Add(_activityItem = new cMenuEditStraItem(tr("Check user activity"), &_activityMode, 3, reminderTexts));

  allowRemindTime(_reminderMode);
  _last_remind = _reminderMode;
}

void cEventSetupMenu::allowRemindTime(int remind)
{
  if (_last_remind == remind) return;
  _timeItem->SetSelectable((remind >= 2));
  _delayItem->SetSelectable((remind != 0));
  _activityItem->SetSelectable((remind != 0));
  _last_remind = remind;
  RefreshCurrent();
  Display();
}

void cEventSetupMenu::Store(void)
{
  SetupStore("OsdOffsetX", _osdOffsetX);
  SetupStore("OsdOffsetY", _osdOffsetY);
  SetupStore("FirstIsMonday", _firstIsMonday);
  SetupStore("ReminderMode", _reminderMode);
  SetupStore("ActivityMode", _activityMode);
  SetupStore("ReminderTime", _reminderTime);
  SetupStore("RemindDelay", _remindDelay);
  SetupStore("ShowCalendarWeek", _showCW);
  SetupStore("PreviewLastNextWeek", _previewLastNextW);
  SetupStore("InversEventColor", _inversEventColor);

  // re-initialize search thread if the config changed
  bool reinit_search = ((globalSetup.reminderMode != _reminderMode) ||
			((_reminderMode >= 2) && (globalSetup.reminderTime != _reminderTime)));

  globalSetup.osdOffsetX = _osdOffsetX;
  globalSetup.osdOffsetY = _osdOffsetY;
  globalSetup.firstIsMonday = _firstIsMonday;
  globalSetup.reminderMode = _reminderMode;
  globalSetup.activityMode = _activityMode;
  globalSetup.reminderTime = _reminderTime;
  globalSetup.remindDelay = _remindDelay;
  globalSetup.showCW = _showCW;
  globalSetup.previewLastNextW = _previewLastNextW;
  globalSetup.inversEventColor = _inversEventColor;

  if (reinit_search) {
    cSearchEventsThread::Exit();
    if (globalSetup.reminderMode >= 2) {
      isyslog("PIM: restart search thread after re-configuration");
      cSearchEventsThread::Init(STA_auto, true);
    }
  }
}

eOSState cEventSetupMenu::ProcessKey(eKeys key)
{
  eOSState state = cOsdMenu::ProcessKey(key);

  if (state == osUnknown)
  {
    if (key == kOk) {
      Store();
      return osBack;
    }
    else
      return osContinue;
  }

  if ((state == osContinue) && (key != kNone))
    allowRemindTime(_reminderMode);

  return state;
}


/*
 * menu.h: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menu.h 1.85 2006/03/25 12:15:19 kls Exp $
 */

#ifndef _REMOTETIMERS_MENU__H
#define _REMOTETIMERS_MENU__H

#include "timers.h"
//#include "ci.h"
//#include "device.h"
#include <vdr/epg.h>
#include <vdr/osdbase.h>
//#include "dvbplayer.h"
#include <vdr/menuitems.h>
//#include "recorder.h"
#include <vdr/skins.h>

namespace PluginRemoteTimers {

class cMenuTimerItem : public cOsdItem {
private:
  cTimer *timer;
  bool remote;
public:
  cMenuTimerItem(cTimer *Timer, bool Remote = false);
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  bool Remote() { return remote; }
  cTimer *Timer(void) { return timer; }

  static int ParseUser(const cTimer &Timer);
  static void UpdateUser(cTimer &Timer, int User);
  };

class cMenuTimers : public cOsdMenu {
private:
  int helpKeys;
  int userFilter;
  cString currentTimerString;

  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState OnOff(void);
  eOSState Info(void);
  cMenuTimerItem *CurrentItem(void);
  cTimer *CurrentTimer(void);
  void SetHelpKeys(void);
  void Set(eRemoteTimersState State = rtsOk);
  void CheckState(eRemoteTimersState State, bool RefreshMsg = true);
public:
  cMenuTimers(void);
  virtual ~cMenuTimers();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuSchedule : public cOsdMenu {
private:
  cSchedulesLock schedulesLock;
  const cSchedules *schedules;
  bool now, next;
  int otherChannel;
  int helpKeys;
  int timerState;
  eOSState Number(void);
  eOSState Record(void);
  eOSState Switch(void);
  void PrepareScheduleAllThis(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleThisThis(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleThisAll(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleAllAll(const cEvent *Event, const cChannel *Channel);
  bool Update(void);
  void SetHelpKeys(void);
public:
  cMenuSchedule(void);
  virtual ~cMenuSchedule();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuEvent : public cOsdMenu {
private:
  const cEvent *event;
public:
  cMenuEvent(const cEvent *Event, bool CanSwitch = false, bool Buttons = false);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuMain : public cOsdMenu {
public:
  cMenuMain(const char *Title);
  virtual eOSState ProcessKey(eKeys Key);
  };

}; // namespace PluginRemoteTimers

#endif //_REMOTETIMERS_MENU__H

/*
 * menu.c: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menu.c 1.482 2008/03/16 11:15:28 kls Exp $
 */

#include "menu.h"
#include "setup.h"
#include "i18n.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>
//#include <ctype.h>
//#include <limits.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <vdr/channels.h>
//#include "config.h"
//#include "cutter.h"
//#include "eitscan.h"
//#include "i18n.h"
#include <vdr/interface.h>
//#include "plugin.h"
//#include "recording.h"
//#include "remote.h"
//#include "shutdown.h"
//#include "sources.h"
#include <vdr/status.h>
//#include "themes.h"
#include <vdr/timers.h>
//#include "transfer.h"
//#include "videodir.h"

#include <ctype.h>

namespace PluginRemoteTimers {

//#define MAXWAIT4EPGINFO   3 // seconds
//#define MODETIMEOUT       3 // seconds
//#define DISKSPACECHEK     5 // seconds between disk space checks
#define NEWTIMERLIMIT   120 // seconds until the start time of a new timer created from the Schedule menu,
                            // within which it will go directly into the "Edit timer" menu to allow
                            // further parameter settings

//#define MAXRECORDCONTROLS (MAXDEVICES * MAXRECEIVERS)
//#define MAXINSTANTRECTIME (24 * 60 - 1) // 23:59 hours
//#define MAXWAITFORCAMMENU  10 // seconds to wait for the CAM menu to open
//#define CAMMENURETYTIMEOUT  3 // seconds after which opening the CAM menu is retried
//#define CAMRESPONSETIMEOUT  5 // seconds to wait for a response from a CAM
//#define MINFREEDISK       300 // minimum free disk space (in MB) required to start recording
//#define NODISKSPACEDELTA  300 // seconds between "Not enough disk space to start recording!" messages

#define CHNUMWIDTH  (numdigits(Channels.MaxNumber()) + 1)

#define AUX_STARTTAG "<remotetimers>"
#define AUX_ENDTAG "</remotetimers>"
#define MSG_UNAVAILABLE trNOOP("Remote timers not available")

// --- cMenuEditRemoteTimer --------------------------------------------------------
class cMenuEditRemoteTimer : public cMenuEditTimer {
private:
  cTimer *timer;
  cString *timerString;
/*
  cTimer data;
  int channel;
  bool addIfConfirmed;
  cMenuEditDateItem *firstday;
  void SetFirstDayItem(void);
*/
  int remote;
  int tmpRemote;
  int user;
  int tmpUser;
  eOSState CheckState(eRemoteTimersState State);
public:
/*
  cMenuEditTimer(cTimer *Timer, bool New = false);
  virtual ~cMenuEditTimer();
*/
  virtual eOSState ProcessKey(eKeys Key);
  cMenuEditRemoteTimer(cTimer *Timer, cString *TimerString = NULL, bool New = false, bool Remote = false);
  };

cMenuEditRemoteTimer::cMenuEditRemoteTimer(cTimer *Timer, cString *TimerString, bool New, bool Remote)
:cMenuEditTimer(Timer, New), timer(Timer), timerString(TimerString)
{
  remote = tmpRemote = Remote;
  user = tmpUser = New ? RemoteTimersSetup.defaultUser : cMenuTimerItem::ParseUser(*timer);
  cOsdItem *item = new cMenuEditBoolItem(trREMOTETIMERS("Location"), &tmpRemote, trREMOTETIMERS("local"), trREMOTETIMERS("remote"));
  if (RemoteTimers.Offline())
      item->SetSelectable(false);
  Add(item, false, First());
  Add(new cMenuEditIntItem(trREMOTETIMERS("User ID"), &tmpUser, 0, 9), false, Get(8));
}

eOSState cMenuEditRemoteTimer::CheckState(eRemoteTimersState State)
{
  if (State > rtsRefresh) {
     Skins.Message(State == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(State)));
     return osContinue;
     }
  return osBack;
}

eOSState cMenuEditRemoteTimer::ProcessKey(eKeys Key)
{
  int TimerNumber = Timers.Count();
  eOSState state = cMenuEditTimer::ProcessKey(Key);
  if (state == osBack && Key == kOk) {
     // changes have been confirmed
     if (user != tmpUser)
        cMenuTimerItem::UpdateUser(*timer, tmpUser);
     if (TimerNumber == Timers.Count()) {
        // editing existing timer (remote timers are also added to Timers, first)
        if (remote == tmpRemote) {
           // timer was not moved
           if (tmpRemote)
              state = CheckState(RemoteTimers.Modify((cRemoteTimer*) timer));
           }
        else if (tmpRemote) {
           // move local to remote
           if (timer->Recording()) {
              Skins.Message(mtError, trREMOTETIMERS("Timer is recording - can't move it to server"));
              state = osContinue;
              }
           else {
              cRemoteTimer *rt = new cRemoteTimer();
              *(cTimer*) rt = *timer;
              if ((state = CheckState(RemoteTimers.New(rt))) == osBack) {
                 Timers.Del(timer);
                 timer = rt;
                 }
              else
                 delete rt;
              }
           }
        else {
           // move remote to local
           cTimer *lt = new cTimer();
#if VDRVERSNUM < 10403
           if (lt->Parse(timer->ToText())) {
#else
           *lt = *(cTimer*) timer;
#endif
           if ((state = CheckState(RemoteTimers.Delete((cRemoteTimer*) timer))) == osBack) {
              Timers.Add(lt);
              timer = lt;
              }
           else
              delete lt;
#if VDRVERSNUM < 10403
           }
           else
              delete lt;
#endif
           }
        }
     else {
        // local timer has been added
        if (tmpRemote) {
           // move to remote
           cRemoteTimer *rt = new cRemoteTimer();
           *(cTimer*) rt = *timer;
           if ((state = CheckState(RemoteTimers.New(rt))) == osBack) {
              Timers.Del(timer);
              timer = rt;
              }
           else
              delete rt;
           }
        }
     }
  // store timer string so we can highlight the correct item after a refresh
  if (state == osBack && timerString)
     *timerString = timer->ToText();
  return state;
}

// --- cMenuTimerItem --------------------------------------------------------

/*
class cMenuTimerItem : public cOsdItem {
private:
  cTimer *timer;
public:
  cMenuTimerItem(cTimer *Timer);
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  cTimer *Timer(void) { return timer; }
  };
*/

cMenuTimerItem::cMenuTimerItem(cTimer *Timer, bool Remote)
{
  timer = Timer;
  remote = Remote;
  Set();
}

int cMenuTimerItem::Compare(const cListObject &ListObject) const
{
  return timer->Compare(*((cMenuTimerItem *)&ListObject)->timer);
}

void cMenuTimerItem::Set(void)
{
  cString day, name("");
  if (timer->WeekDays())
#if VDRVERSNUM < 10503
     day = timer->PrintDay(0, timer->WeekDays());
#else
     day = timer->PrintDay(0, timer->WeekDays(), false);
#endif
  else if (timer->Day() - time(NULL) < 28 * SECSINDAY) {
     day = itoa(timer->GetMDay(timer->Day()));
     name = WeekDayName(timer->Day());
     }
  else {
     struct tm tm_r;
     time_t Day = timer->Day();
     localtime_r(&Day, &tm_r);
     char buffer[16];
     strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
     day = buffer;
     }
  // TRANSLATORS: Indicator for (R)emote or (L)ocal timer in timers list
  const char *RL = trREMOTETIMERS("RL");
  SetText(cString::sprintf("%c\t%c%d\t%s%s%s\t%02d:%02d\t%02d:%02d\t%s",
                    !(timer->HasFlags(tfActive)) ? ' ' : timer->FirstDay() ? '!' : timer->Recording() ? '#' : '>',
                    RL[remote ? 0 : 1],
                    timer->Channel()->Number(),
                    *name,
                    *name && **name ? " " : "",
                    *day,
                    timer->Start() / 100,
                    timer->Start() % 100,
                    timer->Stop() / 100,
                    timer->Stop() % 100,
                    timer->File()));
}

int cMenuTimerItem::ParseUser(const cTimer& Timer) {
  int user = 0;
  const char *p = Timer.Aux();
  if (p && (p = strstr(p, AUX_STARTTAG))) {
      if (sscanf(p, AUX_STARTTAG "%d" AUX_ENDTAG, &user) != 1)
          user = 0;
      }
  return user;
}

void cMenuTimerItem::UpdateUser(cTimer& Timer, int User) {
  cString origTimer = Timer.ToText();

  const char *head = origTimer;
  const char *tail = head + strlen(head);
  while (tail > head && isspace(*(tail - 1))) {
      tail--;
  }

  const char *start = strstr(head, AUX_STARTTAG);
  const char *end = start ? strstr(start, AUX_ENDTAG) : NULL;

  if (start) {
      end = end ? end + strlen(AUX_ENDTAG) : start + strlen(AUX_STARTTAG);
      }
  else {
      start = end = tail;
      }

  int lenPrefix = start - head;
  int lenPostfix = tail - end;

  char *modTimer;
  if (User == 0)
      asprintf(&modTimer, "%.*s%.*s", lenPrefix, head, lenPostfix, end);
  else
      asprintf(&modTimer, "%.*s" AUX_STARTTAG "%d" AUX_ENDTAG "%.*s", lenPrefix, head, User, lenPostfix, end);
  Timer.Parse(modTimer);
  free(modTimer);
}

// --- cMenuTimers -----------------------------------------------------------

/*
class cMenuTimers : public cOsdMenu {
private:
  int helpKeys;
  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState OnOff(void);
  eOSState Info(void);
  cTimer *CurrentTimer(void);
  void SetHelpKeys(void);
public:
  cMenuTimers(void);
  virtual ~cMenuTimers();
  virtual eOSState ProcessKey(eKeys Key);
  };
*/

cMenuTimers::cMenuTimers(void)
:cOsdMenu(tr("Timers"), 2, CHNUMWIDTH + 1, 10, 6, 6)
{
  helpKeys = -1;
  userFilter = RemoteTimersSetup.userFilter;
  if (!RemoteTimers.Connect())
      Skins.Message(mtWarning, tr(MSG_UNAVAILABLE));
  Set();
  /*
  for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
      timer->SetEventFromSchedule(); // make sure the event is current
      Add(new cMenuTimerItem(timer));
      }
  Sort();
  SetCurrent(First());
  SetHelpKeys();
  */
  Timers.IncBeingEdited();
}

cMenuTimers::~cMenuTimers()
{
  Timers.DecBeingEdited();
  RemoteTimers.Disconnect();
}

void cMenuTimers::Set(eRemoteTimersState Msg)
{
  int currentIndex = Current();
  Clear();
  eRemoteTimersState state = RemoteTimers.Offline() ? rtsOk : RemoteTimers.Refresh();
  if (state == rtsOk) {
      for (cRemoteTimer *timer = RemoteTimers.First(); timer; timer = RemoteTimers.Next(timer)) {
          int user = cMenuTimerItem::ParseUser(*timer);
          if (userFilter == 0 || user == 0 || userFilter == user) {
              timer->SetEventFromSchedule(); // make sure the event is current
              Add(new cMenuTimerItem(timer, true));
              }
          }
      }
  for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
      int user = cMenuTimerItem::ParseUser(*timer);
      if (userFilter == 0 || user == 0 || userFilter == user) {
          timer->SetEventFromSchedule(); // make sure the event is current
          Add(new cMenuTimerItem(timer));
          }
      }
  Sort();

  cOsdItem *currentItem = NULL;
  // if we have been editing a timer make it current
  if (*currentTimerString) {
     for (cOsdItem *item = First(); item && !currentItem; item = Next(item)) {
        if (strcmp(*currentTimerString, ((cMenuTimerItem *)item)->Timer()->ToText()) == 0)
           currentItem = item;
        }
     currentTimerString = NULL;
     }
  // make the previous position current
  if (!currentItem)
     currentItem = Get(currentIndex < Count() ? currentIndex : Count() - 1);
  // use first
  if (!currentItem)
     currentItem = First();

  SetCurrent(currentItem);
  SetHelpKeys();
  if (userFilter == 0)
      SetTitle(tr("Timers"));
  else
      SetTitle(cString::sprintf(trREMOTETIMERS("Timers - User %d"), userFilter));

  if (Msg > state)
      state = Msg;
  if (state != rtsOk)
      Skins.Message(state == rtsRefresh ? mtInfo : state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
}

void cMenuTimers::CheckState(eRemoteTimersState State, bool RefreshMsg)
{
  if (State == rtsRefresh && !RefreshMsg)
      Set(rtsOk);
  else if (State != rtsOk)
      Set(State);
}

cMenuTimerItem *cMenuTimers::CurrentItem(void)
{
  return (cMenuTimerItem *)Get(Current());
}

cTimer *cMenuTimers::CurrentTimer(void)
{
  cMenuTimerItem *item = (cMenuTimerItem *)Get(Current());
  return item ? item->Timer() : NULL;
}

void cMenuTimers::SetHelpKeys(void)
{
  int NewHelpKeys = 0;
  cTimer *timer = CurrentTimer();
  if (timer) {
     if (timer->Event())
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     helpKeys = NewHelpKeys;
     SetHelp(helpKeys > 0 ? tr("Button$On/Off") : NULL, tr("Button$New"), helpKeys > 0 ? tr("Button$Delete") : NULL, helpKeys == 2 ? tr("Button$Info") : NULL);
     }
}

eOSState cMenuTimers::OnOff(void)
{
  if (HasSubMenu())
     return osContinue;
  cMenuTimerItem *item = CurrentItem();
  cTimer *timer = CurrentTimer();
  if (timer) {
     timer->OnOff();
     timer->SetEventFromSchedule();
     if (item->Remote())
        CheckState(RemoteTimers.Modify((cRemoteTimer*)timer));
     RefreshCurrent();
     DisplayCurrent(true);
     if (timer->FirstDay())
        isyslog("timer %s first day set to %s", *timer->ToDescr(), *timer->PrintFirstDay());
     else
        isyslog("timer %s %sactivated", *timer->ToDescr(), timer->HasFlags(tfActive) ? "" : "de");
     if (!item->Remote())
        Timers.SetModified();
     }
  return osContinue;
}

eOSState cMenuTimers::Edit(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  isyslog("editing timer %s", *CurrentTimer()->ToDescr());
  return AddSubMenu(new cMenuEditRemoteTimer(CurrentTimer(), &currentTimerString, false, CurrentItem()->Remote()));
}

eOSState cMenuTimers::New(void)
{
  if (HasSubMenu())
     return osContinue;
  return AddSubMenu(new cMenuEditRemoteTimer(new cTimer, &currentTimerString, true, RemoteTimers.Offline() ? false : RemoteTimersSetup.addToRemote));
}

eOSState cMenuTimers::Delete(void)
{
  // Check if this timer is active:
  cMenuTimerItem *item = CurrentItem();
  cTimer *ti = CurrentTimer();
  if (ti) {
     if (Interface->Confirm(tr("Delete timer?"))) {
        if (ti->Recording() && !item->Remote()) {
           if (Interface->Confirm(tr("Timer still recording - really delete?"))) {
              ti->Skip();
              cRecordControls::Process(time(NULL));
              }
           else
              return osContinue;
           }
        isyslog("deleting timer %s", *ti->ToDescr());
        if (item->Remote())
           CheckState(RemoteTimers.Delete((cRemoteTimer*)ti), false);
        else {
           Timers.Del(ti);
           cOsdMenu::Del(Current());
           Timers.SetModified();
           }
        Display();
        }
     }
  return osContinue;
}

eOSState cMenuTimers::Info(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cTimer *ti = CurrentTimer();
  if (ti && ti->Event())
     return AddSubMenu(new cMenuEvent(ti->Event()));
  return osContinue;
}

eOSState cMenuTimers::ProcessKey(eKeys Key)
{
  int TimerNumber = HasSubMenu() ? Count() : -1;
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     return Edit();
       case kRed:    state = OnOff(); break; // must go through SetHelpKeys()!
       case kGreen:  return New();
       case kYellow: state = Delete(); break;
       case kInfo:
       case kBlue:   return Info();
                     break;
       case k0 ... k9:
                     userFilter = Key - k0;
                     Set();
                     Display();
                     return osContinue;
       default: break;
       }
     }
  if (TimerNumber >= 0 && !HasSubMenu()) {
  //if (TimerNumber >= 0 && !HasSubMenu() && Timers.Get(TimerNumber)) {
     // a newly created timer was confirmed with Ok
     //Add(new cMenuTimerItem(Timers.Get(TimerNumber)), true);
     Set();
     Display();
     }
  if (Key != kNone)
     SetHelpKeys();
  return state;
}

// --- cMenuEvent ------------------------------------------------------------

cMenuEvent::cMenuEvent(const cEvent *Event, bool CanSwitch, bool Buttons)
:cOsdMenu(tr("Event"))
{
  event = Event;
  if (event) {
     cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true);
     if (channel) {
        SetTitle(channel->Name());
        int TimerMatch = tmNone;
        Timers.GetMatch(event, &TimerMatch);
        if (TimerMatch != tmFull)
           RemoteTimers.GetMatch(event, &TimerMatch);
        if (Buttons)
           SetHelp(TimerMatch == tmFull ? tr("Button$Timer") : tr("Button$Record"), NULL, NULL, CanSwitch ? tr("Button$Switch") : NULL);
        }
     }
}

void cMenuEvent::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetEvent(event);
  if (event->Description())
     cStatus::MsgOsdTextItem(event->Description());
}

eOSState cMenuEvent::ProcessKey(eKeys Key)
{
  switch (Key) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft);
                  return osContinue;
    case kInfo:   return osBack;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kGreen:
       case kYellow: return osContinue;
       case kOk:     return osBack;
       default: break;
       }
     }
  return state;
}

// --- cMenuScheduleItem -----------------------------------------------------

class cMenuScheduleItem : public cOsdItem {
public:
  enum eScheduleSortMode { ssmAllThis, ssmThisThis, ssmThisAll, ssmAllAll }; // "which event(s) on which channel(s)"
private:
  static eScheduleSortMode sortMode;
public:
  const cEvent *event;
  const cChannel *channel;
  bool withDate;
  int timerMatch;
  cMenuScheduleItem(const cEvent *Event, cChannel *Channel = NULL, bool WithDate = false);
  static void SetSortMode(eScheduleSortMode SortMode) { sortMode = SortMode; }
  static void IncSortMode(void) { sortMode = eScheduleSortMode((sortMode == ssmAllAll) ? ssmAllThis : sortMode + 1); }
  static eScheduleSortMode SortMode(void) { return sortMode; }
  virtual int Compare(const cListObject &ListObject) const;
  bool Update(bool Force = false);
  };

cMenuScheduleItem::eScheduleSortMode cMenuScheduleItem::sortMode = ssmAllThis;

cMenuScheduleItem::cMenuScheduleItem(const cEvent *Event, cChannel *Channel, bool WithDate)
{
  event = Event;
  channel = Channel;
  withDate = WithDate;
  timerMatch = tmNone;
  Update(true);
}

int cMenuScheduleItem::Compare(const cListObject &ListObject) const
{
  cMenuScheduleItem *p = (cMenuScheduleItem *)&ListObject;
  int r = -1;
  if (sortMode != ssmAllThis)
     r = strcoll(event->Title(), p->event->Title());
  if (sortMode == ssmAllThis || r == 0)
     r = event->StartTime() - p->event->StartTime();
  return r;
}

static const char *TimerMatchChars = " tT";

bool cMenuScheduleItem::Update(bool Force)
{
  bool result = false;
  int OldTimerMatch = timerMatch;
  int remoteTimerMatch = tmNone;
  Timers.GetMatch(event, &timerMatch);
  RemoteTimers.GetMatch(event, &remoteTimerMatch);
  if (remoteTimerMatch > timerMatch)
     timerMatch = remoteTimerMatch;
  if (Force || timerMatch != OldTimerMatch) {
     cString buffer;
     char t = TimerMatchChars[timerMatch];
     char v = event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
     char r = event->SeenWithin(30) && event->IsRunning() ? '*' : ' ';
     const char *csn = channel ? channel->ShortName(true) : NULL;
     cString eds = event->GetDateString();
#if VDRVERSNUM < 10503
#define Utf8SymChars(a,b) b
#endif
     if (channel && withDate)
        buffer = cString::sprintf("%d\t%.*s\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, 6), csn, Utf8SymChars(csn, 6), *eds, *event->GetTimeString(), t, v, r, event->Title());
     else if (channel)
        buffer = cString::sprintf("%d\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, 6), csn, *event->GetTimeString(), t, v, r, event->Title());
     else
        buffer = cString::sprintf("%.*s\t%s\t%c%c%c\t%s", Utf8SymChars(eds, 6), *eds, *event->GetTimeString(), t, v, r, event->Title());
     SetText(buffer);
     result = true;
     }
  return result;
}

// --- cMenuWhatsOn ----------------------------------------------------------

class cMenuWhatsOn : public cOsdMenu {
private:
  bool now;
  int helpKeys;
  int timerState;
  eOSState Record(void);
  eOSState Switch(void);
  static int currentChannel;
  static const cEvent *scheduleEvent;
  bool Update(void);
  void SetHelpKeys(void);
public:
  cMenuWhatsOn(const cSchedules *Schedules, bool Now, int CurrentChannelNr);
  static int CurrentChannel(void) { return currentChannel; }
  static void SetCurrentChannel(int ChannelNr) { currentChannel = ChannelNr; }
  static const cEvent *ScheduleEvent(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

int cMenuWhatsOn::currentChannel = 0;
const cEvent *cMenuWhatsOn::scheduleEvent = NULL;

cMenuWhatsOn::cMenuWhatsOn(const cSchedules *Schedules, bool Now, int CurrentChannelNr)
:cOsdMenu(Now ? tr("What's on now?") : tr("What's on next?"), CHNUMWIDTH, 7, 6, 4)
{
  now = Now;
  helpKeys = -1;
  timerState = 0;
  Timers.Modified(timerState);
  for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
      if (!Channel->GroupSep()) {
         const cSchedule *Schedule = Schedules->GetSchedule(Channel);
         if (Schedule) {
            const cEvent *Event = Now ? Schedule->GetPresentEvent() : Schedule->GetFollowingEvent();
            if (Event)
               Add(new cMenuScheduleItem(Event, Channel), Channel->Number() == CurrentChannelNr);
            }
         }
      }
  currentChannel = CurrentChannelNr;
  Display();
  SetHelpKeys();
}

bool cMenuWhatsOn::Update(void)
{
  bool result = false;
  if (Timers.Modified(timerState)) {
     for (cOsdItem *item = First(); item; item = Next(item)) {
         if (((cMenuScheduleItem *)item)->Update())
            result = true;
         }
     }
  return result;
}

void cMenuWhatsOn::SetHelpKeys(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->timerMatch == tmFull)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], now ? tr("Button$Next") : tr("Button$Now"), tr("Button$Schedule"), tr("Button$Switch"));
     helpKeys = NewHelpKeys;
     }
}

const cEvent *cMenuWhatsOn::ScheduleEvent(void)
{
  const cEvent *ei = scheduleEvent;
  scheduleEvent = NULL;
  return ei;
}

eOSState cMenuWhatsOn::Switch(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     cChannel *channel = Channels.GetByChannelID(item->event->ChannelID(), true);
     if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
        return osEnd;
     }
  Skins.Message(mtError, tr("Can't switch channel!"));
  return osContinue;
}

eOSState cMenuWhatsOn::Record(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     if (item->timerMatch == tmFull) {
        int tm = tmNone;
        cTimer *timer = RemoteTimers.GetMatch(item->event, &tm);
        if (timer)
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, true));
        else {
           timer = Timers.GetMatch(item->event, &tm);
           if (timer)
              return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, false));
           }
        }
     bool isRemote = true;
     cTimer *timer = new cTimer(item->event);
     cTimer *t = RemoteTimers.GetTimer(timer);
     if (!t) {
        isRemote = false;
        t = Timers.GetTimer(timer);
        }
     if (t) {
        delete timer;
        timer = t;
        return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
        }
     else {
        isRemote = false;
        cMenuTimerItem::UpdateUser(*timer, RemoteTimersSetup.defaultUser);
        Timers.Add(timer);
        if (RemoteTimersSetup.addToRemote && !RemoteTimers.Offline()) {
           isRemote = true;
           cRemoteTimer *rt = new cRemoteTimer();
           *(cTimer*) rt = *timer;
           eRemoteTimersState state;
           if ((state = RemoteTimers.New(rt)) <= rtsRefresh) {
              Timers.Del(timer);
              timer = rt;
              }
           else {
              Skins.Message(state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
              Timers.Del(timer);
              delete rt;
              return osContinue;
              }
           }
        Timers.SetModified();
        isyslog("timer %s added (active)", *timer->ToDescr());
        if (timer->Matches(0, false, NEWTIMERLIMIT))
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     }
  return osContinue;
}

eOSState cMenuWhatsOn::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRecord:
       case kRed:    return Record();
       case kYellow: state = osBack;
                     // continue with kGreen
       case kGreen:  {
                       cMenuScheduleItem *mi = (cMenuScheduleItem *)Get(Current());
                       if (mi) {
                          scheduleEvent = mi->event;
                          currentChannel = mi->channel->Number();
                          }
                     }
                     break;
       case kBlue:   return Switch();
       case kInfo:
       case kOk:     if (Count())
                        return AddSubMenu(new cMenuEvent(((cMenuScheduleItem *)Get(Current()))->event, true, true));
                     break;
       default:      break;
       }
     }
  else if (!HasSubMenu()) {
     if (HadSubMenu && Update())
        Display();
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuSchedule ---------------------------------------------------------

/*
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
*/

cMenuSchedule::cMenuSchedule(void)
:cOsdMenu("")
{
  now = next = false;
  otherChannel = 0;
  helpKeys = -1;
  timerState = 0;
  Timers.Modified(timerState);
  cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
  cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (channel) {
     cMenuWhatsOn::SetCurrentChannel(channel->Number());
     schedules = cSchedules::Schedules(schedulesLock);
     PrepareScheduleAllThis(NULL, channel);
     SetHelpKeys();
     }
  if (!RemoteTimers.Connect() || RemoteTimers.Refresh() != rtsOk)
     Skins.Message(mtWarning, tr(MSG_UNAVAILABLE));
}

cMenuSchedule::~cMenuSchedule()
{
  cMenuWhatsOn::ScheduleEvent(); // makes sure any posted data is cleared
  RemoteTimers.Disconnect();
}

void cMenuSchedule::PrepareScheduleAllThis(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(7, 6, 4);
  SetTitle(cString::sprintf(tr("Schedule - %s"), Channel->Name()));
  if (schedules && Channel) {
     const cSchedule *Schedule = schedules->GetSchedule(Channel);
     if (Schedule) {
        const cEvent *PresentEvent = Event ? Event : Schedule->GetPresentEvent();
        time_t now = time(NULL) - Setup.EPGLinger * 60;
        for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
            if (ev->EndTime() > now || ev == PresentEvent)
               Add(new cMenuScheduleItem(ev), ev == PresentEvent);
            }
        }
     }
}

void cMenuSchedule::PrepareScheduleThisThis(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(7, 6, 4);
  SetTitle(cString::sprintf(tr("This event - %s"), Channel->Name()));
  if (schedules && Channel && Event) {
     const cSchedule *Schedule = schedules->GetSchedule(Channel);
     if (Schedule) {
        time_t now = time(NULL) - Setup.EPGLinger * 60;
        for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
            if ((ev->EndTime() > now || ev == Event) && !strcmp(ev->Title(), Event->Title()))
               Add(new cMenuScheduleItem(ev), ev == Event);
            }
        }
     }
}

void cMenuSchedule::PrepareScheduleThisAll(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(CHNUMWIDTH, 7, 7, 6, 4);
  SetTitle(tr("This event - all channels"));
  if (schedules && Event) {
     for (cChannel *ch = Channels.First(); ch; ch = Channels.Next(ch)) {
         const cSchedule *Schedule = schedules->GetSchedule(ch);
         if (Schedule) {
            time_t now = time(NULL) - Setup.EPGLinger * 60;
            for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
                if ((ev->EndTime() > now || ev == Event) && !strcmp(ev->Title(), Event->Title()))
                   Add(new cMenuScheduleItem(ev, ch, true), ev == Event && ch == Channel);
                }
            }
         }
     }
}

void cMenuSchedule::PrepareScheduleAllAll(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(CHNUMWIDTH, 7, 7, 6, 4);
  SetTitle(tr("All events - all channels"));
  if (schedules) {
     for (cChannel *ch = Channels.First(); ch; ch = Channels.Next(ch)) {
         const cSchedule *Schedule = schedules->GetSchedule(ch);
         if (Schedule) {
            time_t now = time(NULL) - Setup.EPGLinger * 60;
            for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
                if (ev->EndTime() > now || ev == Event)
                   Add(new cMenuScheduleItem(ev, ch, true), ev == Event && ch == Channel);
                }
            }
         }
     }
}

bool cMenuSchedule::Update(void)
{
  bool result = false;
  if (Timers.Modified(timerState)) {
     for (cOsdItem *item = First(); item; item = Next(item)) {
         if (((cMenuScheduleItem *)item)->Update())
            result = true;
         }
     }
  return result;
}

void cMenuSchedule::SetHelpKeys(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->timerMatch == tmFull)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], tr("Button$Now"), tr("Button$Next"));
     helpKeys = NewHelpKeys;
     }
}

eOSState cMenuSchedule::Number(void)
{
  cMenuScheduleItem::IncSortMode();
  cMenuScheduleItem *CurrentItem = (cMenuScheduleItem *)Get(Current());
  const cChannel *Channel = NULL;
  const cEvent *Event = NULL;
  if (CurrentItem) {
     Event = CurrentItem->event;
     Channel = Channels.GetByChannelID(Event->ChannelID(), true);
     }
  else
     Channel = Channels.GetByNumber(cDevice::CurrentChannel());
  switch (cMenuScheduleItem::SortMode()) {
    case cMenuScheduleItem::ssmAllThis:  PrepareScheduleAllThis(Event, Channel); break;
    case cMenuScheduleItem::ssmThisThis: PrepareScheduleThisThis(Event, Channel); break;
    case cMenuScheduleItem::ssmThisAll:  PrepareScheduleThisAll(Event, Channel); break;
    case cMenuScheduleItem::ssmAllAll:   PrepareScheduleAllAll(Event, Channel); break;
    }
  CurrentItem = (cMenuScheduleItem *)Get(Current());
  Sort();
  SetCurrent(CurrentItem);
  Display();
  return osContinue;
}

eOSState cMenuSchedule::Record(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     if (item->timerMatch == tmFull) {
        int tm = tmNone;
        cTimer *timer = RemoteTimers.GetMatch(item->event, &tm);
        if (timer)
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, true));
        else {
           timer = Timers.GetMatch(item->event, &tm);
           if (timer)
              return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, false));
           }
        }
     bool isRemote = true;
     cTimer *timer = new cTimer(item->event);
     cTimer *t = RemoteTimers.GetTimer(timer);
     if (!t) {
        isRemote = false;
        t = Timers.GetTimer(timer);
        }
     if (t) {
        delete timer;
        timer = t;
        return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
        }
     else {
        isRemote = false;
        cMenuTimerItem::UpdateUser(*timer, RemoteTimersSetup.defaultUser);
        Timers.Add(timer);
        if (RemoteTimersSetup.addToRemote && !RemoteTimers.Offline()) {
           isRemote = true;
           cRemoteTimer *rt = new cRemoteTimer();
           *(cTimer*) rt = *timer;
           eRemoteTimersState state;
           if ((state = RemoteTimers.New(rt)) <= rtsRefresh) {
              Timers.Del(timer);
              timer = rt;
              }
           else {
              Skins.Message(state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
              Timers.Del(timer);
              delete rt;
              return osContinue;
              }
           }
        Timers.SetModified();
        isyslog("timer %s added (active)", *timer->ToDescr());
        if (timer->Matches(0, false, NEWTIMERLIMIT))
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     }
  return osContinue;
}

eOSState cMenuSchedule::Switch(void)
{
  if (otherChannel) {
     if (Channels.SwitchTo(otherChannel))
        return osEnd;
     }
  Skins.Message(mtError, tr("Can't switch channel!"));
  return osContinue;
}

eOSState cMenuSchedule::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case k0:      return Number();
       case kRecord:
       case kRed:    return Record();
       case kGreen:  if (schedules) {
                        if (!now && !next) {
                           int ChannelNr = 0;
                           if (Count()) {
                              cChannel *channel = Channels.GetByChannelID(((cMenuScheduleItem *)Get(Current()))->event->ChannelID(), true);
                              if (channel)
                                 ChannelNr = channel->Number();
                              }
                           now = true;
                           return AddSubMenu(new cMenuWhatsOn(schedules, true, ChannelNr));
                           }
                        now = !now;
                        next = !next;
                        return AddSubMenu(new cMenuWhatsOn(schedules, now, cMenuWhatsOn::CurrentChannel()));
                        }
       case kYellow: if (schedules)
                        return AddSubMenu(new cMenuWhatsOn(schedules, false, cMenuWhatsOn::CurrentChannel()));
                     break;
       case kBlue:   if (Count() && otherChannel)
                        return Switch();
                     break;
       case kInfo:
       case kOk:     if (Count())
                        return AddSubMenu(new cMenuEvent(((cMenuScheduleItem *)Get(Current()))->event, otherChannel, true));
                     break;
       default:      break;
       }
     }
  else if (!HasSubMenu()) {
     now = next = false;
     const cEvent *ei = cMenuWhatsOn::ScheduleEvent();
     if (ei) {
        cChannel *channel = Channels.GetByChannelID(ei->ChannelID(), true);
        if (channel) {
           cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
           PrepareScheduleAllThis(NULL, channel);
           if (channel->Number() != cDevice::CurrentChannel()) {
              otherChannel = channel->Number();
              SetHelp(Count() ? tr("Button$Record") : NULL, tr("Button$Now"), tr("Button$Next"), tr("Button$Switch"));
              }
           Display();
           }
        }
     else if (HadSubMenu && Update())
        Display();
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuMain -------------------------------------------------------------

cMenuMain::cMenuMain(const char *Title)
:cOsdMenu(Title)
{
  SetHasHotkeys();
  Add(new cOsdItem(hk(tr("Schedule")),   osUser1));
  Add(new cOsdItem(hk(tr("Timers")),     osUser2));
}

eOSState cMenuMain::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  switch (state) {
    case osUser1:      return AddSubMenu(new cMenuSchedule);
    case osUser2:      return AddSubMenu(new cMenuTimers);
    default:           return state;
    }
}

}; // namespace PluginRemoteTimers

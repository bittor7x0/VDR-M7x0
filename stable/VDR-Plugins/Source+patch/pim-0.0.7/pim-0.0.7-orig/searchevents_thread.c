/*
 * $Id: searchevents_thread.c,v 1.0 2007/08/14 20:00:34 torsten Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: searchevents_thread.c,v $
 * Revision 1.0  2007/08/14 20:00:34 torsten
 * Initial version
 *
 */

#include <string>

#include "i18n.h"
#include "searchevents_thread.h"
#include "pimSetup.h"
#include "calManager.h"
#include "calEvents.h"

#include <vdr/plugin.h>
#include <vdr/tools.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

using namespace std;

// file to save if message was displayed today
#define PIMMARKS	"pimEvents.marks"

// assume that someone views tv after X remote keys are pressed
#define NUMBER_REMOTE_ACTIVE	5

cSearchEventsThread *searchAuto = NULL;
cSearchEventsThread *searchSvdrp = NULL;

cSearchEventsThread::cSearchEventsThread(void)
   : cThread("PIM: search events")
{
  cSearchEventsThread(STA_auto);
}

cSearchEventsThread::cSearchEventsThread(SearchThreadAction type, bool reconfig)
   : cThread("PIM: search events")
{
  m_Active = false;
  m_buffer = NULL;
  m_mark_file = NULL;
  m_activeCount = 0;
  m_checkActivity = false;
  m_action = type;
  m_reconfigured = reconfig;
}

cSearchEventsThread::~cSearchEventsThread()
{
  dsyslog("PIM: delete search events thread (%s)", (m_action == STA_auto) ? "auto" : "svdrp");
  if (m_buffer) free(m_buffer);
  if (m_mark_file) free(m_mark_file);
  if (m_marksList.Count() > 0) m_marksList.Clear();
  if (m_dispList.Count() > 0) m_dispList.Clear();

  if (m_action == STA_auto)
    searchAuto = NULL;
  else
    searchSvdrp = NULL;
}

bool cSearchEventsThread::Init(SearchThreadAction type, bool reconfig) 
{
  bool bRet = true;

  switch (type) {
    case STA_auto:
      if (searchAuto == NULL) {
	searchAuto = new cSearchEventsThread(type, reconfig);
	searchAuto->Start();
      }
      else bRet = false;
      break;

    default:
      if (searchSvdrp == NULL) {
	searchSvdrp = new cSearchEventsThread(type);
	searchSvdrp->Start();
      }
      else bRet = false;
      break;
  }

  if (!bRet) isyslog("PIM: another %s search thread is still running",
		     (type == STA_auto) ? "auto" : "svdrp");
  return bRet;
}

void cSearchEventsThread::Exit(bool svdrp)
{
  if (svdrp == true) {
    if (searchSvdrp != 0) {
      searchSvdrp->Stop();
      DELETENULL(searchSvdrp);
    }
  }
  else if (searchAuto != 0) {
    searchAuto->Stop();
    DELETENULL(searchAuto);
  }
}

void cSearchEventsThread::Stop(void)
{
  m_Active = false;
  Cancel(10);
}

char * cSearchEventsThread::getOSDtext(time_t t, int day, char * text)
{
  if (m_buffer) { free(m_buffer); m_buffer = NULL; }
  struct tm tm = *localtime(&t);
  int weekDay, diff = day - tm.tm_yday;
  char *d_desc = NULL;

  switch (diff)
  {
    case 0:	// today
      d_desc = (char *) tr("Today");
      break;

    case 1:	// tomorrow
      d_desc = (char *) tr("Tomorrow");
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:	// rest of a week (7 days round)
      weekDay = (tm.tm_wday + diff) % 7;
      switch (weekDay) {
	  case 0: d_desc = (char *) tr("Sunday"); break;
	  case 1: d_desc = (char *) tr("Monday"); break;
	  case 2: d_desc = (char *) tr("Tuesday"); break;
	  case 3: d_desc = (char *) tr("Wednesday"); break;
	  case 4: d_desc = (char *) tr("Thursday"); break;
	  case 5: d_desc = (char *) tr("Friday"); break;
	  case 6: d_desc = (char *) tr("Saturday"); break;
	  default: d_desc = "?"; break;
      }
      break;

    default:
      tm.tm_mday += diff;
      mktime(&tm);
      asprintf(&m_buffer,"%s (%02d.%02d.)", text, tm.tm_mday, tm.tm_mon + 1);
      break;
  }

  if (!m_buffer)
      asprintf(&m_buffer,"%s (%s)", text, d_desc);

  return m_buffer;
}

// read marks file in
void cSearchEventsThread::readMarkFile(void)
{
  struct stat sbuf;

  if (m_mark_file) free(m_mark_file);
  asprintf(&m_mark_file, "%s/%s", globalSetup.ConfigDir, PIMMARKS);
  if (stat(m_mark_file, &sbuf) == -1) {
    if (errno != ENOENT)
      esyslog ("PIM: cannot stat marks file [%s], errno %d (%s)", m_mark_file, errno, strerror(errno));
    return;
  }

  ifstream f;
  f.open(m_mark_file, ios::in);
  if (!f.is_open()) {
    esyslog ("PIM: cannot open marks file [%s] for reading", m_mark_file);
    return;
  }

  int d, nr = 1;
  char *p, line[100];
  while (!f.eof()) {
    f.getline(line, 100);
    p = strtok(line, ":");

    if (p) {
      d = atoi(p);
      p = strtok(NULL, ":");
    }

    if (p) m_marksList.Add(new cEventDisp(d, p));

    nr++;
  }

  f.close();
}

// sleep interuptable
void cSearchEventsThread::SleepSec(int secs)
{
  time_t t_end = time(NULL) + secs;
  while (Running() && (time(NULL) < t_end)) {
    cCondWait::SleepMs(1000);
  }
}

void cSearchEventsThread::Action(void)
{
  int day;
  m_Active = true;
  bool removed = false;
  bool nextSleep = false;

  do {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    if (m_action == STA_auto) {
      if (!m_checkActivity || nextSleep) {
	m_checkActivity = false;

	switch (globalSetup.reminderMode) {
	  case 0:	// disabled (should not happen, as catched in pim.c)
	    DELETENULL(searchAuto);
	    return;
	    break;

	  case 1:	// always on
	    if (globalSetup.activityMode == 1) {
	      dsyslog("PIM: wait for activity before checking for events");
	      m_Active = false;
	      m_activeCount = 0;
	      m_checkActivity = true;
	      return;	// return as we want to wait for some keys
	    }
	    break;

	  case 2:	// at special time
	  case 3:	// at special time (daily)
	    tm.tm_hour = globalSetup.reminderTime / 100;
	    tm.tm_min = globalSetup.reminderTime % 100;
	    tm.tm_sec = 0;
	    int secs = mktime(&tm) - t;

	    // sleep until next day
	    // in case of reconfiguration display only future events
	    if (nextSleep || m_reconfigured) {
	      nextSleep = m_reconfigured = false;
	      if (secs <= 0) {	// if event is gone, try next day
		tm.tm_mday++;
		secs = mktime(&tm) - t;
	      }
	    }

	    // wait until reminder time has reached and some keys are pressed
	    if (secs > 0) {
	      dsyslog("PIM: wait until %02d.%02d %02d:%02d o'clock before checking for events",
		      tm.tm_mday, tm.tm_mon+1, tm.tm_hour, tm.tm_min);
	      SleepSec(secs);
	    }

	    if (globalSetup.activityMode >= 1) {
	      dsyslog("PIM: wait for activity before checking for events");
	      m_Active = false;
	      m_activeCount = 0;
	      m_checkActivity = true;
	      return;	// return as we want to wait for some keys
	    }
	    break;
	}
      }
      else
	m_checkActivity = false;

      if (globalSetup.remindDelay > 0)
	SleepSec(globalSetup.remindDelay);	// wait some seconds before showing the events
    }

    char *s_action[] = { "auto", "svdrp_check", "svdrp_display", "svdrp_remove" };
    dsyslog("PIM: start action [%s], %d seconds delayed", s_action[m_action],
	    (m_action != STA_auto) ? 0 : globalSetup.remindDelay);

    cCalManager* mgr = cCalManager::getInstance();
    tEventIds ids;

    if (m_action == STA_remove) {
      ids = mgr->getAllOldEvents();
    }
    else {
      tm.tm_hour = 23;
      tm.tm_min = 59;
      tm.tm_sec = 59;
      t = mktime(&tm);
      ids = mgr->getEventsForDays(t);
    }

    if ((m_action == STA_auto) || (m_action == STA_svdrp_check))
      readMarkFile();

    bool removeIt = ((m_action == STA_auto) && (m_marksList.Count() > 0));
    tEventIds::const_iterator it = ids.begin();

    while (it != ids.end())
    {
      cCalEvent *ev = mgr->getEventById(*it);
      char *text = ev->getTextAndDay(t, &day);
      dsyslog("PIM: event found [%s] on day [%d]", text, day);
      if (m_action == STA_remove) {
	mgr->del(ev);
	removed = true;
      }
      else if ((m_action == STA_svdrp_display) ||
	       (m_marksList.Count() == 0) ||
	       !isMarked(day, text))
      {
	m_dispList.Add(new cEventDisp(day, text));
      }
      ++it;
    }

    // display OSD reminders
    if ((m_action != STA_remove) && (m_dispList.Count() > 0)) {
      m_dispList.Sort();		// sort by date first
      for (cEventDisp *ed = m_dispList.First(); ed; ed = m_dispList.Next(ed)) {
	// the 'synchronous' call to QueueMessage() with timeout is used here
	// to get exact knowledge if the message was displayed and to receive
	// if the user pressed a key.
	int aKey, showAgain = 10;		// try 10 times to display the message
	char *showText = getOSDtext(t, ed->yday, ed->str);
	do {
	  aKey = Skins.QueueMessage(mtInfo, showText, Setup.OSDMessageTime, Setup.OSDMessageTime + 10);
	  switch (aKey) {
	    case -1:
	      // the message wasn't displayed within the timeout, may be there is
	      // another OSD menu open. try again in a minute
	      dsyslog("PIM: queue of message [%s] timed out (other OSD open)", showText);
	      showAgain--;
	      SleepSec(60);
	      break;

	    case kOk:
	      // the message was displayed and the user pressed OK
	      // so mark this message to not display anymore (until event date is over)
	      if (m_action == STA_auto)
	        m_marksList.Add(new cEventDisp(ed->yday, ed->str, true));
	      // falltrough

	    case kNone:
	      // the message was displayed but the user didn't press a key
	      // falltrough

	    default:
	      // unknown state -> consider the message as 'processed'
	      showAgain = 0;
	      break;
	  }
	} while (showAgain && Running());
      }
      m_dispList.Clear();
    }

    // save actual marks (old gone events are dropped)
    if (m_action == STA_auto) {
      if (m_marksList.Count() > 0) {
	ofstream f;
	f.open(m_mark_file, ios::out);
	if (!f.is_open()) {
	  esyslog ("PIM: cannot open marks file [%s] for writing", m_mark_file);
	}
	else {
	  for (cEventDisp *ed = m_marksList.First(); ed; ed = m_marksList.Next(ed)) {
	    if (ed->use) {
	      f << ed->yday << ":" << ed->str << endl;
	      removeIt = false;
	    }
	  }
	  f.close();
	}
      }

      if (removeIt) {	// all marks are gone, file can be removed
	if (remove(m_mark_file) == -1)
	  esyslog ("PIM: failed to remove marks file [%s], errno %d (%s)", m_mark_file, errno, strerror(errno));
	else
	  dsyslog ("PIM: marks file [%s] removed", m_mark_file);
      }
    }
    else if (removed) {
      // save events because some old ones are removed
      mgr->save();
    }

    if (m_marksList.Count() > 0)
      m_marksList.Clear();

    if ((m_action == STA_auto) && (globalSetup.reminderMode == 3))
      nextSleep = true;

  } while (nextSleep && Running());

  DELETENULL((m_action == STA_auto) ? searchAuto : searchSvdrp);
}

// search if an event is marked to not be displayed
bool cSearchEventsThread::isMarked(int d, char *s)
{
  for (cEventDisp *ed = m_marksList.First(); ed; ed = m_marksList.Next(ed)) {
    if ((ed->yday == d) && !strcmp(ed->str, s)) {
      ed->use = true;
      dsyslog ("PIM: event [%s] on day [%d] marked to skip", s, d);
      return true;
    }
  }

  return false;
}

// only used for automatic actions
void cSearchEventsThread::actionCatched(void)
{
  if (!m_checkActivity || (m_activeCount > NUMBER_REMOTE_ACTIVE) || m_Active)
    return;		// disabled, time not reached or one instance still running

  dsyslog("PIM: actionCatched(%d)", m_activeCount + 1);
  if (m_activeCount < NUMBER_REMOTE_ACTIVE) {
    m_activeCount++;
    return;
  }

  m_activeCount++;
  Start();
}

void cSearchEventsThread::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{
  if (Device->CardIndex() == cDevice::PrimaryDevice()->CardIndex())
    actionCatched();
}

void cSearchEventsThread::Replaying(const cControl *Control, const char *Name, const char *FileName, bool On)
{
  actionCatched();
}

void cSearchEventsThread::SetVolume(int Volume, bool Absolute)
{
  actionCatched();
}

// ################## class cEventDisp ##################)

// compare 2 display events to sort by date
// Must return 0 if this object is equal to ListObject, a positive value
// if it is "greater", and a negative value if it is "smaller".
int cEventDisp::Compare(const cListObject &ListObject) const
{
  cEventDisp *ed_cmp = (cEventDisp *)&ListObject;
  return (yday - ed_cmp->yday);
}


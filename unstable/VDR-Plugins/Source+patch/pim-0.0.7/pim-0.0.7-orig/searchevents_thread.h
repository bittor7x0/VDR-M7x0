/*
 * $Id: searchevents_thread.h,v 1.0 2007/08/14 20:00:34 torsten Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: searchevents_thread.h,v $
 * Revision 1.0  2007/08/14 20:00:34 torsten
 * Initial version
 *
 */

#ifndef PIM_SEARCHEVENTS_THREAD_H
#define PIM_SEARCHEVENTS_THREAD_H

#include <vdr/thread.h>
#include <vdr/tools.h>
#include <vdr/status.h>


// event data for marks and display
class cEventDisp : public cListObject {
friend class cSearchEventsThread;
private:
  bool use;
  int yday;
  char *str;
  cEventDisp(int d, char *s, bool u = false) { use = u; yday = d; str = strdup(s); }
  ~cEventDisp(void) { if (str) free(str); }
  int Compare(const cListObject &ListObject) const;
};

// search thread action types
typedef enum {
    STA_auto,		// automatic search at startup
    STA_svdrp_check,	// search started by SVDRP
    STA_svdrp_display,	// search started by SVDRP ignoring marked events
    STA_remove		// remove events older then today
} SearchThreadAction;

class cSearchEventsThread: public cThread, public cStatus {
 private:
  bool m_Active;
  SearchThreadAction m_action;
  cList<cEventDisp> m_marksList;
  cList<cEventDisp> m_dispList;
  char * m_mark_file;
  char * m_buffer;
  int m_activeCount;
  bool m_checkActivity;
  bool m_reconfigured;

  bool checkReaded(char * text);
  void readMarkFile(void);
  char * getOSDtext(time_t t, int day, char * text);
  bool isMarked(int d, char *s);
  void actionCatched(void);
  void SleepSec(int secs);

 protected:
  virtual void Action(void);
  void Stop(void);
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
  virtual void Replaying(const cControl *Control, const char *Name, const char *FileName, bool On);
  virtual void SetVolume(int Volume, bool Absolute);

 public:
  cSearchEventsThread(void);
  cSearchEventsThread(SearchThreadAction type, bool reconfig = false);
  virtual ~cSearchEventsThread();

  static bool Init(SearchThreadAction type = STA_auto, bool reconfig = false);
  static void Exit(bool svdrp = false);
};

#endif


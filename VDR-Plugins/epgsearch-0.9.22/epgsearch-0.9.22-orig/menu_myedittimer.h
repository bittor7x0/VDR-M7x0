#ifndef __EPGSEARCH_MENUMYEDITTIMER_H
#define __EPGSEARCH_MENUMYEDITTIMER_H

#include <vdr/menuitems.h>
#include "timer_thread.h"

// --- cMenuMyEditTimer --------------------------------------------------------
class cMenuMyEditTimer : public cOsdMenu {
private:
  cTimer *timer;
  const cEvent* event;
  int channel;
  bool addIfConfirmed;
  int UserDefDaysOfWeek;
  cMenuEditStrItem* m_DirItem;

  uint flags;
  time_t day;
  int weekdays;
  int start;
  int stop;
  int priority;
  int lifetime;
  char file[MaxFileName];
  char directory[MaxFileName];
  int checkmode;
public:
  cMenuMyEditTimer(cTimer *Timer, bool New, const cEvent* event, const cChannel* forcechannel=NULL);
  virtual ~cMenuMyEditTimer();
  virtual eOSState ProcessKey(eKeys Key);
  void HandleSubtitle();
  void Set();
  void ReplaceDirVars();
  bool IsSingleEvent(void) const;
  void SplitFile();
  eOSState DeleteTimer();

  static const char* CheckModes[3];
};
#endif

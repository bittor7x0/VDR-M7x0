#ifndef __MENU_TIMERSDONE_H
#define __MENU_TIMERSDONE_H

#include "epgsearchext.h"
#include "timerdone.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>

// --- cMenuTimerDoneItem ----------------------------------------------------------
class cMenuTimerDoneItem : public cOsdItem {
 public:
    cTimerDone* timerDone;
    cMenuTimerDoneItem(cTimerDone* TimerDone);
    void Set(void);
    int Compare(const cListObject &ListObject) const;
};


class cMenuTimersDone : public cOsdMenu {
  private:
    cSearchExt* search;
    eOSState Delete(void);
    eOSState DeleteAll(void);
    bool showAll;
  protected:
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
    void UpdateTitle();
    eOSState Summary(void);
    cTimerDone* CurrentTimerDone(void);
 public:
    cMenuTimersDone(cSearchExt* search = NULL);
};

#endif

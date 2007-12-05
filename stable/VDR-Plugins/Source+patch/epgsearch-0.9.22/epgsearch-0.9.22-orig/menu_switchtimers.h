#ifndef __MENU_SWITCHTIMER_H
#define __MENU_SWITCHTIMER_H

#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include "switchtimer.h"

// --- cMenuSwitchTimerItem ----------------------------------------------------------
class cMenuSwitchTimerItem : public cOsdItem {
 public:
    cSwitchTimer* switchTimer;
    cMenuSwitchTimerItem(cSwitchTimer* SwitchTimer);
    void Set(void);
    int Compare(const cListObject &ListObject) const;
};


class cMenuSwitchTimers : public cOsdMenu {
  private:
    eOSState Delete(void);
    eOSState DeleteAll(void);
  protected:
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
    eOSState Summary(void);
    cSwitchTimer* CurrentSwitchTimer(void);
 public:
    cMenuSwitchTimers();
};

#endif

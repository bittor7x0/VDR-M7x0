#ifndef __EPGSEARCHMENUCONFL_H
#define __EPGSEARCHMENUCONFL_H

#include <vdr/menuitems.h>
#include "conflictcheck.h"
#include "menu_event.h"

// --- cMenuConflictCheckItem ------------------------------------------------------
class cMenuConflictCheckItem : public cOsdItem {
public: 
    cConflictCheckTime* checktime;
    cConflictCheckTimerObj* timerObj;
    cMenuConflictCheckItem(cConflictCheckTime* Ct, cConflictCheckTimerObj* TimerObj = NULL);
};

// --- cMenuConflictCheck ------------------------------------------------------
class cMenuConflictCheck : public cOsdMenu {
private:
    cConflictCheck conflictCheck;   
    bool showAll;
    int lastSel;
    virtual eOSState ProcessKey(eKeys Key);
    cConflictCheckTimerObj* CurrentTimerObj(void);
    void Update();
    bool BuildList();
 public:
    cMenuConflictCheck();
};

// --- cMenuConflictCheckDetailsItem ------------------------------------------------------
class cMenuConflictCheckDetailsItem : public cOsdItem {
    bool hasTimer;
public: 
    cConflictCheckTimerObj* timerObj;
    cMenuConflictCheckDetailsItem(cConflictCheckTimerObj* TimerObj = NULL);
    bool Update(bool Force = false);
};

// --- cMenuConflictCheckDetails ------------------------------------------------------
class cMenuConflictCheckDetails : public cOsdMenu {
private:
    cConflictCheck* conflictCheck;
    cConflictCheckTimerObj* timerObj;   
    cConflictCheckTime* checktime;
    cEventObjects eventObjects;

    virtual eOSState ProcessKey(eKeys Key);
    cConflictCheckTimerObj* CurrentTimerObj(void);
    eOSState Commands(eKeys Key);
    void SetHelpKeys();
    eOSState ToggleTimer(cConflictCheckTimerObj* TimerObj);
    eOSState DeleteTimer(cConflictCheckTimerObj* TimerObj);
    bool Update(bool Force = false);
    bool BuildList();
    eOSState ShowSummary();
    void UpdateCurrent();
 public:
    cMenuConflictCheckDetails(cConflictCheckTimerObj* TimerObj = NULL, cConflictCheck* ConflictCheck = NULL);
};

#endif

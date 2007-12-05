#ifndef __EPGSEARCH_MENUWHATSON_H
#define __EPGSEARCH_MENUWHATSON_H

#include <vdr/menuitems.h>
#include "epgsearchcfg.h"
#include "templatefile.h"
#include "menu_event.h"

// --- cMenuMyScheduleItem ------------------------------------------------------
class cMenuMyScheduleItem : public cOsdItem {
public:
  const cEvent *event;
  cChannel *channel;
  showMode mode;
  int timerMatch;
  bool inSwitchList;
  cMenuTemplate* menuTemplate;

  cMenuMyScheduleItem(const cEvent *Event, cChannel *Channel = NULL, showMode ShowMode = showNow, cMenuTemplate* menuTemplate = NULL);
  bool Update(bool Force = false);
};

// --- cMenuWhatsOnSearch ----------------------------------------------------------
class cMenuWhatsOnSearch : public cOsdMenu {
private:
    const cSchedules* schedules;
    eOSState Record(void);
    eOSState ExtendedSearch(void);
    static int currentChannel;
    cEventObjects eventObjects;
 public:
    static cChannel *scheduleChannel;
    static cList<cShowMode> showModes;
    static showMode currentShowMode;
    int shiftTime;
    time_t GetTimeT(int iTime);
    static showMode GetNextMode();
    cMenuWhatsOnSearch(const cSchedules *Schedules, int CurrentChannelNr);
    ~cMenuWhatsOnSearch();
    void LoadSchedules();
    static int CurrentChannel(void) { return currentChannel; }
    static void SetCurrentChannel(int ChannelNr) { currentChannel = ChannelNr; }
    static cChannel *ScheduleChannel(cChannel* forceChannel = NULL);
    virtual eOSState ProcessKey(eKeys Key); 
    virtual eOSState Switch(void);
    virtual eOSState Shift(int); 
    virtual eOSState Commands(eKeys Key);
    virtual eOSState ShowSummary();
    void SetHelpKeys();
    int GetTab(int Tab);
    bool Update(void);
    void CreateShowModes();
    static cShowMode* GetShowMode(showMode mode);
    void UpdateCurrent();
};

#endif

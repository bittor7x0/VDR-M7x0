#ifndef VDR_SEARCHTIMER_THREAD_H
#define VDR_SEARCHTIMER_THREAD_H

#include <vdr/thread.h>
#include "epgsearchext.h"
#include "epgsearch.h"
#include "mail.h"

// --- cRecordingObj --------------------------------------------------------
class cRecordingObj : public cListObject {
public:
    cRecording* recording;
    cSearchExt* search;
public:
    cRecordingObj(cRecording* r, cSearchExt* s) : recording(r), search(s) {}
    ~cRecordingObj() { recording = NULL;} // do not delete anything!
};

// --- cSearchTimerThread----------------------------------------------------
class cSearchTimerThread: public cThread {
 private:
    bool m_Active;
    time_t m_lastUpdate;
    cPluginEpgsearch* m_plugin;
    cMailUpdateNotifier mailNotifier;

 protected:
    virtual void Action(void);
    bool AddModTimer(cTimer* Timer, int, cSearchExt*, const cEvent*, int Prio, int Lifetime, char* Summary = NULL);
    void RemoveTimer(cTimer* Timer, const cEvent* Event = NULL);
    void Stop(void);
    bool NeedUpdate();
    bool TimerWasModified(cTimer* t);
 public:
    static cSearchResults announceList;
    static char* SummaryExtended(cSearchExt* searchExt, cTimer* Timer, const cEvent* pEvent);
    static cSearchTimerThread *m_Instance;
    static cTimer* GetTimer(cSearchExt *searchExt, const cEvent *pEvent, bool& bTimesMatchExactly);

    cSearchTimerThread(cPluginEpgsearch* thePlugin);
    virtual ~cSearchTimerThread();
    static void Init(cPluginEpgsearch* thePlugin, bool activatePermanently = false);
    static void Exit(void);
    void CheckExpiredRecs();
    void DelRecording(int index);
    void CheckManualTimers();
    void ModifyManualTimer(const cEvent* event, const cTimer* timer, int bstart, int bstop);
};

#endif 

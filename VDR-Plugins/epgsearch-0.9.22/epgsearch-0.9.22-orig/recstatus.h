#include <vdr/status.h>
#include "recdone.h"
#include "epgsearchtools.h"

// --- cRecDoneTimerObj --------------------------------------------------------
class cRecDoneTimerObj : public cTimerObj {
public:
    int deviceNr;
    cRecDone* recDone;
    time_t lastBreak;
public:
    cRecDoneTimerObj(cTimer* Timer, int DeviceNr) : cTimerObj(Timer), deviceNr(DeviceNr), recDone(NULL), lastBreak(0) {}
    ~cRecDoneTimerObj() { timer = NULL; recDone = NULL; } // do not delete anything!
};

class cRecStatusMonitor : public cStatus 
{
    cList<cRecDoneTimerObj> TimersRecording;
protected:
    virtual void Recording(const cDevice *Device, const char *Name, const char*, bool On);
 public:
    cRecStatusMonitor();
    int TimerRecDevice(cTimer*);
};

extern cRecStatusMonitor* gl_recStatusMonitor;

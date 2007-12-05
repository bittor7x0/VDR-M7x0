#ifndef __TIMERDONE_H
#define __TIMERDONE_H

#include <string>
#include <vdr/plugin.h>

using std::string;

// --- cTimerDone --------------------------------------------------------
class cTimerDone : public cListObject {
public:
    time_t start;        // Start time of the timer
    time_t stop;         // Stop time of the timer
    tChannelID channelID;    
    int searchID;
    string title;
    string shorttext;

    static char *buffer;
    
    cTimerDone();
    cTimerDone(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID);
    ~cTimerDone();
    bool operator== (const cTimerDone &arg) const;

    static bool Read(FILE *f);
    bool Parse(const char *s);
    const char *ToText(void) const;
    bool Save(FILE *f);
};

class cTimersDone : public cConfig<cTimerDone>, public cMutex {
 public:
    cTimerDone* InList(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID);
    void Update(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID, cTimerDone* timerdone);
    void ClearOutdated(void);
};

extern cTimersDone TimersDone;

#endif

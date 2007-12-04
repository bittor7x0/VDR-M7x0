#ifndef __NOANNOUCE_H
#define __NOANNOUCE_H

#include <string>
#include <vdr/plugin.h>

using std::string;

// --- cNoAnnouce --------------------------------------------------------
// an event that should not be announced again
class cNoAnnounce : public cListObject {
public:
    string title;             // Title of this event
    string shortText;         // Short description of this event
    time_t startTime;        // Start time of the timer
    time_t nextAnnounce;     // time of the next announce
    tChannelID channelID;    

    static char *buffer;
    
    cNoAnnounce();
    cNoAnnounce(const cEvent* Event, time_t NextAnnounce = 0);
    ~cNoAnnounce();
    bool operator== (const cNoAnnounce &arg) const;

    static bool Read(FILE *f);
    bool Parse(const char *s);
    const char *ToText(void) const;
    bool Save(FILE *f);
};

class cNoAnnounces : public cConfig<cNoAnnounce> {
 public:
    cNoAnnounce* InList(const cEvent* e);
    void ClearOutdated(void);
    void UpdateNextAnnounce(const cEvent* e, time_t NextAnnounce);
};

extern cNoAnnounces NoAnnounces;

#endif

#ifndef __EPGSEARCHDEFTIMERCHECKMETHOD_H
#define __EPGSEARCHDEFTIMERCHECKMETHOD_H

#include <vdr/menuitems.h>
#include <vdr/plugin.h>

#define UPD_CHDUR 1
#define UPD_EVENTID 2

class cDefTimerCheckMode : public cListObject {
 private:
    static char *buffer;
 public:
    tChannelID channelID;
    int mode;

    cDefTimerCheckMode() : mode(0) {}
    cDefTimerCheckMode(tChannelID ChannelID, int Mode) : channelID(ChannelID), mode(Mode) {}
    ~cDefTimerCheckMode(void);
    bool Parse(const char *s);
    const char *ToText(void) const;
    bool Save(FILE *f);
};

class cDefTimerCheckModes : public cConfig<cDefTimerCheckMode> {
 public:
    int GetMode(const cChannel* channel);
    void SetMode(const cChannel* channel, int mode);
};

extern cDefTimerCheckModes DefTimerCheckModes;

// --- cMenuDefTimerCheckMethod ---------------------------------------------------------

class cMenuDefTimerCheckMethod : public cOsdMenu {
    int* modes;
 public:
    cMenuDefTimerCheckMethod();
    ~cMenuDefTimerCheckMethod();
    void Set();
    eOSState ProcessKey(eKeys Key);

    static const char* CheckModes[3];
};

#endif

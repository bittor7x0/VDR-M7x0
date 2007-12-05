#ifndef __SWITCHTIMER_H
#define __SWITCHTIMER_H

#include <vdr/plugin.h>

class cSwitchTimer : public cListObject
{
public:
    const cEvent* event;
    int switchMinsBefore;
    int announceOnly;

    cSwitchTimer(void);
    cSwitchTimer(const cEvent* Event, int SwitchMinsBefore=1, int announceOnly=0);
    bool Parse(const char *s);
    const char *ToText(bool& ignore);
    bool Save(FILE *f);
};

class cSwitchTimers : public cConfig<cSwitchTimer>, public cMutex 
{
public:
    cSwitchTimers(void) {}
    ~cSwitchTimers(void) {}
    cSwitchTimer* InSwitchList(const cEvent* event);
    bool Exists(const cSwitchTimer* SwitchTimer);
};

extern cSwitchTimers SwitchTimers;

#endif

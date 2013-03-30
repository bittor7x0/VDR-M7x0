#ifndef __ZAPPILOTCONFIG_H
#define __ZAPPILOTCONFIG_H

#include <vdr/plugin.h>

struct sZappilotConfig
{
   int closeonswitch;
   int hidemenu;
   int fastbrowse;
   int fastbrowsealt;
   int switchtimer;
   int switchminsbefore;
   cPlugin* pEPGSearch;
};

extern sZappilotConfig config;

// #define DEBUG
#endif                           //__ZAPPILOTCONFIG_H

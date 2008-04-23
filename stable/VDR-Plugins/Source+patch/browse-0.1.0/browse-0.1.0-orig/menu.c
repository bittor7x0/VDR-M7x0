#include "menu.h"
#include "config.h"

//////////////////////////////////////////////////////////////////////////////


cMenuSetupBrowse::cMenuSetupBrowse()
{
  newCenterInfo    = config.centerInfo;
  newTimeFormat24h = config.timeFormat24h;
  newHideMenuEntry = config.hideMenuEntry;
  newNTSC = config.NTSC;
  
  Add(new cMenuEditBoolItem("Center Info Text", &newCenterInfo, "No", "Yes"));
  Add(new cMenuEditBoolItem("Hide Main Menu Entry", &newHideMenuEntry, "No", "Yes"));
  Add(new cMenuEditBoolItem("Time Format", &newTimeFormat24h, "12h", "24h"));
  Add(new cMenuEditBoolItem("Mode", &newNTSC, "PAL", "NTSC"));
}


//----------------------------------------------------------------------------

void cMenuSetupBrowse::Store(void)
{
  SetupStore("centerInfo",    config.centerInfo    = newCenterInfo);
  SetupStore("timeFormat24h", config.timeFormat24h = newTimeFormat24h);
  SetupStore("hideMenuEntry", config.hideMenuEntry = newHideMenuEntry);
  SetupStore("NTSC",          config.NTSC          = newNTSC);
}


//////////////////////////////////////////////////////////////////////////////


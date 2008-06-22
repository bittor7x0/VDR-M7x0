#include <vdr/plugin.h>
#include <vdr/interface.h>

#include "config.h"
#include "radioInfoSetupMenu.h"


///////////////////////////////////////////////////////////////////////////////


cRadioInfoSetupMenu::cRadioInfoSetupMenu(void)
{
  newMaxRetries  = config.maxRetries;
  newQuickDetect = config.quickDetect;
  newOsdDelay    = config.osdDelay;
  newScanDelay   = config.scanDelay;
   
  Add(new cMenuEditBoolItem("Use quick PID detection", &newQuickDetect, "No", "Yes"));
  Add(new cMenuEditIntItem("Max. retries for info",    &newMaxRetries, 1, 999)); 
  Add(new cMenuEditIntItem("OSD update delay (sec)",   &newOsdDelay, 0, 500)); 
  Add(new cMenuEditIntItem("Info update delay (sec)",  &newScanDelay, 0, 500)); 
}


//-----------------------------------------------------------------------------

void cRadioInfoSetupMenu::Store(void)
{
  SetupStore("maxRetries",  config.maxRetries  = newMaxRetries);
  SetupStore("quickDetect", config.quickDetect = newQuickDetect);
  SetupStore("osdDelay",    config.osdDelay    = newOsdDelay);
  SetupStore("scanDelay",   config.scanDelay   = newScanDelay);
}


///////////////////////////////////////////////////////////////////////////////


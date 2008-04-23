#include <vdr/plugin.h>
#include <vdr/interface.h>

#include "config.h"
#include "radioInfoSetupMenu.h"


cRadioInfoSetupMenu::cRadioInfoSetupMenu(void)
{
  newMAX_RETRIES  = config.MAX_RETRIES;
  newQUICK_DETECT = config.QUICK_DETECT;
  newDELAY        = config.DELAY;
  newSCAN_DELAY   = config.SCAN_DELAY;
   
  Add(new cMenuEditBoolItem("Use quick PID detection", &newQUICK_DETECT, "No", "Yes"));
  Add(new cMenuEditIntItem("Max. retries for info",    &newMAX_RETRIES, 1, 999)); 
  Add(new cMenuEditIntItem("OSD update delay (sec)",   &newDELAY, 0, 500)); 
  Add(new cMenuEditIntItem("Info update delay (sec)",  &newSCAN_DELAY, 0, 500)); 
}


void cRadioInfoSetupMenu::Store(void)
{
  SetupStore("MAX_RETRIES",  config.MAX_RETRIES  = newMAX_RETRIES);
  SetupStore("QUICK_DETECT", config.QUICK_DETECT = newQUICK_DETECT);
  SetupStore("DELAY",        config.DELAY        = newDELAY);
  SetupStore("SCAN_DELAY",   config.SCAN_DELAY   = newSCAN_DELAY);
}

#include <vdr/plugin.h>
#include <vdr/interface.h>

#include "menu.h"
#include "config.h"


char* keys[] = {
             "Up",
             "Down",
             "Menu",
             "Ok",
             "Back",
             "Left",
             "Right",
             "Red",
             "Green",
             "Yellow",
             "Blue",
             "Play",
             "Pause",
             "Stop",
             "Record",
             "FastFwd",
             "FastRew",
             "ChanUp",
             "ChanDn",
             "User1", "User2", "User3", "User4", "User5", "User6", "User7", "User8", "User9"};

//----------------------------------------------------------------------------

cMenuSetupFEpg::cMenuSetupFEpg(void)
{
  newKeyUp = config.keyUp;
  newKeyDown = config.keyDown;
  newKeyLeft = config.keyLeft;
  newKeyRight = config.keyRight;
  newKeyInfo = config.keyInfo;
  newKeyPageUp = config.keyPageUp;
  newKeyPageDown = config.keyPageDown;
  newKeyScreenShot = config.keyScreenShot;
  newKeyBack = config.keyBack;
  newKeyOk = config.keyOk;
  newKeyRecord = config.keyRecord;
  newMute = config.mute;
  newCenterInfo = config.centerInfo;
  newClr1Red = config.clr1Red;
  newClr1Green = config.clr1Green;
  newClr1Blue = config.clr1Blue;
  newClr2Red = config.clr2Red;
  newClr2Green = config.clr2Green;
  newClr2Blue = config.clr2Blue;
  newTrans = config.trans;
  newTimeout = config.timeout;
  
  
 
  Add(new cMenuEditStraItem("Channel Up", &newKeyUp, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Channel Down", &newKeyDown, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Scroll Right", &newKeyRight, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Scroll Left", &newKeyLeft, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Get Info", &newKeyInfo, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Page Up", &newKeyPageUp, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Page Down", &newKeyPageDown, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Exit", &newKeyBack, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Select", &newKeyOk, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Record", &newKeyRecord, NUM_OF_KEYS, keys));
  Add(new cMenuEditStraItem("Screenshot", &newKeyScreenShot, NUM_OF_KEYS, keys));
  Add(new cMenuEditBoolItem("Mute on open", &newMute, "No", "Yes"));
  Add(new cMenuEditBoolItem("Center Info Text", &newCenterInfo, "No", "Yes"));
  Add(new cMenuEditIntItem("Transparency", &newTrans, 0, 255));
  Add(new cMenuEditIntItem("Color 1: Red", &newClr1Red, 0, 255));
  Add(new cMenuEditIntItem("Color 1: Green", &newClr1Green, 0, 255));
  Add(new cMenuEditIntItem("Color 1: Blue", &newClr1Blue, 0, 255));
  Add(new cMenuEditIntItem("Color 2: Red", &newClr2Red, 0, 255));
  Add(new cMenuEditIntItem("Color 2: Green", &newClr2Green, 0, 255));
  Add(new cMenuEditIntItem("Color 2: Blue", &newClr2Blue, 0, 255));
  Add(new cMenuEditIntItem("Input Timeout (ms)", &newTimeout, 250, 3000));
  
}

//----------------------------------------------------------------------------

void cMenuSetupFEpg::Store(void)
{
  SetupStore("keyUp", config.keyUp = newKeyUp);
  SetupStore("keyDown", config.keyDown = newKeyDown);
  SetupStore("keyLeft", config.keyLeft = newKeyLeft);
  SetupStore("keyRight", config.keyRight = newKeyRight);
  SetupStore("keyInfo", config.keyInfo = newKeyInfo);
  SetupStore("keyPageUp", config.keyPageUp = newKeyPageUp);
  SetupStore("keyPageDown", config.keyPageDown = newKeyPageDown);
  SetupStore("keyScreenShot", config.keyScreenShot = newKeyScreenShot);
  SetupStore("keyBack", config.keyBack = newKeyBack);
  SetupStore("keyOk", config.keyOk = newKeyOk);
  SetupStore("keyRecord", config.keyRecord = newKeyRecord);
  
  SetupStore("mute", config.mute = newMute);
  SetupStore("centerInfo", config.centerInfo = newCenterInfo);
  SetupStore("clr1Red", config.clr1Red = newClr1Red);
  SetupStore("clr1Green", config.clr1Green = newClr1Green);
  SetupStore("clr1Blue", config.clr1Blue = newClr1Blue);
  SetupStore("clr2Red", config.clr2Red = newClr2Red);
  SetupStore("clr2Green", config.clr2Green = newClr2Green);
  SetupStore("clr2Blue", config.clr2Blue = newClr2Blue);
  SetupStore("trans", config.trans = newTrans);
  SetupStore("timeout", config.timeout = newTimeout);

}


//////////////////////////////////////////////////////////////////////////////

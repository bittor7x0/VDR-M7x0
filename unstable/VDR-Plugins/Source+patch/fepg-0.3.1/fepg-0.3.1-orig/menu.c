#include <vdr/plugin.h>
#include <vdr/interface.h>

#include "menu.h"
#include "config.h"


//////////////////////////////////////////////////////////////////////////////


static const int NUM_OF_KEYS = 28;

const char* fEpgKeys[NUM_OF_KEYS] = {
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


//////////////////////////////////////////////////////////////////////////////


eOSState cMenuSetupFEpg::cOpenScreenMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdItem::ProcessKey(Key);

  if (state == osUnknown) {
    if (NORMALKEY(Key) == kOk || NORMALKEY(Key) == kRight) {
      fprintf(stderr, "fEPG: Openning Screen Setup\n");
      requestScreenMenu = true; 
      cRemote::CallPlugin("fepg");
      parent->Store();
      return osEnd;
    }
  }
  
  return state;
}


//////////////////////////////////////////////////////////////////////////////


cMenuSetupFEpg::cMenuSetupFEpg(void)
{
  newKeyUp = config.keyUp;
  newKeyDown = config.keyDown;
  newKeyLeft = config.keyLeft;
  newKeyRight = config.keyRight;
  newKeyInfo = config.keyInfo;
  newKeyPageUp = config.keyPageUp;
  newKeyPageDown = config.keyPageDown;
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
  newChanOrder = config.chanOrder;
  newTimeFormat24h = config.timeFormat24h;
  newHideMenuEntry = config.hideMenuEntry;
  newTVMode = config.TVMode;
  newNumRows = config.numRows;
  newUseSmallFont = config.useSmallFont;
  
  AddCategory("General Options");
  Add(new cMenuEditBoolItem("Mute On Open", &newMute, "No", "Yes"));
  Add(new cMenuEditBoolItem("Center Info Text", &newCenterInfo, "No", "Yes"));
  Add(new cMenuEditBoolItem("Hide Main Menu Entry", &newHideMenuEntry, "No", "Yes"));
  Add(new cMenuEditBoolItem("Channel Order", &newChanOrder, "Descending", "Ascending"));
  Add(new cMenuEditIntItem("Input Timeout (ms)", &newTimeout, 250, 3000));
  Add(new cMenuEditBoolItem("Time Format", &newTimeFormat24h, "12h", "24h"));
  AddEmptyLine();
  
  AddCategory("Size and Position");
  Add(new cMenuEditBoolItem("TV Mode", &newTVMode, "PAL", "NTSC"));
  Add(new cMenuEditIntItem("Number of Rows", &newNumRows, 6, MAX_ROWS));
  Add(new cMenuEditBoolItem("Font Size", &newUseSmallFont, "Normal", "Small"));
  Add(new cOpenScreenMenu(this));
  AddEmptyLine();
  
  AddCategory("Key Configuration");
  Add(new cMenuEditStraItem("Channel Up", &newKeyUp, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Channel Down", &newKeyDown, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Scroll Right", &newKeyRight, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Scroll Left", &newKeyLeft, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Get Info", &newKeyInfo, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Page Up", &newKeyPageUp, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Page Down", &newKeyPageDown, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Exit", &newKeyBack, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Select", &newKeyOk, NUM_OF_KEYS, fEpgKeys));
  Add(new cMenuEditStraItem("Record", &newKeyRecord, NUM_OF_KEYS, fEpgKeys));
  AddEmptyLine();
  
  AddCategory("Color Configuration");
  Add(new cMenuEditIntItem("Opacity", &newTrans, 0, 255));
  Add(new cMenuEditIntItem("Color 1: Red", &newClr1Red, 0, 255));
  Add(new cMenuEditIntItem("Color 1: Green", &newClr1Green, 0, 255));
  Add(new cMenuEditIntItem("Color 1: Blue", &newClr1Blue, 0, 255));
  Add(new cMenuEditIntItem("Color 2: Red", &newClr2Red, 0, 255));
  Add(new cMenuEditIntItem("Color 2: Green", &newClr2Green, 0, 255));
  Add(new cMenuEditIntItem("Color 2: Blue", &newClr2Blue, 0, 255));
  
  
}

//----------------------------------------------------------------------------

void cMenuSetupFEpg::Store(void)
{
  SetupStore("keyUp",       config.keyUp = newKeyUp);
  SetupStore("keyDown",     config.keyDown = newKeyDown);
  SetupStore("keyLeft",     config.keyLeft = newKeyLeft);
  SetupStore("keyRight",    config.keyRight = newKeyRight);
  SetupStore("keyInfo",     config.keyInfo = newKeyInfo);
  SetupStore("keyPageUp",   config.keyPageUp = newKeyPageUp);
  SetupStore("keyPageDown", config.keyPageDown = newKeyPageDown);
  SetupStore("keyBack",     config.keyBack = newKeyBack);
  SetupStore("keyOk",       config.keyOk = newKeyOk);
  SetupStore("keyRecord",   config.keyRecord = newKeyRecord);
  
  SetupStore("mute",          config.mute = newMute);
  SetupStore("centerInfo",    config.centerInfo = newCenterInfo);
  SetupStore("clr1Red",       config.clr1Red = newClr1Red);
  SetupStore("clr1Green",     config.clr1Green = newClr1Green);
  SetupStore("clr1Blue",      config.clr1Blue = newClr1Blue);
  SetupStore("clr2Red",       config.clr2Red = newClr2Red);
  SetupStore("clr2Green",     config.clr2Green = newClr2Green);
  SetupStore("clr2Blue",      config.clr2Blue = newClr2Blue);
  SetupStore("trans",         config.trans = newTrans);
  SetupStore("timeout",       config.timeout = newTimeout);
  SetupStore("chanOrder",     config.chanOrder = newChanOrder);
  SetupStore("timeFormat24h", config.timeFormat24h = newTimeFormat24h);
  SetupStore("hideMenuEntry", config.hideMenuEntry = newHideMenuEntry);
  SetupStore("TVMode",        config.TVMode = newTVMode);
  SetupStore("numRows",       config.numRows = newNumRows);
  SetupStore("useSmallFont",       config.useSmallFont = newUseSmallFont);
}


//----------------------------------------------------------------------------
// Taken from Stremdev plugin
void cMenuSetupFEpg::AddCategory(const char *Title) 
{
  char *buffer = NULL;

  asprintf(&buffer, "--- %s -------------------------------------------------"
   		"---------------", Title );

  cOsdItem *item = new cOsdItem(buffer);
  free(buffer);

	item->SetSelectable(false);
  Add(item);
}


//----------------------------------------------------------------------------

void cMenuSetupFEpg::AddEmptyLine(void)
{
  cOsdItem *item = new cOsdItem("");
  item->SetSelectable(false);
  Add(item);
}


//////////////////////////////////////////////////////////////////////////////


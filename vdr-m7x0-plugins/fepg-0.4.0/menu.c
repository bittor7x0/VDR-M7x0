#include <vdr/plugin.h>
#include <vdr/interface.h>

#include "menu.h"
#include "config.h"
#include "colorMenu.h"


//////////////////////////////////////////////////////////////////////////////


class cMenuEditIntIntervalItem : public cMenuEditIntItem 
{
protected:
  int interval;
public:
  cMenuEditIntIntervalItem(const char* Name, int* Value, int Min, int Max, int Interval)
  : cMenuEditIntItem(Name, Value, Min, Max)
  {
    interval = Interval;
  }
  
  virtual eOSState ProcessKey(eKeys Key);
};


// Adapted from VDR's cMenuEditIntItem
eOSState cMenuEditIntIntervalItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) 
  {
    int newValue = *value;
    bool IsRepeat = Key & k_Repeat;
    Key = NORMALKEY(Key);
    switch (Key) {
      case kNone: break;
      case kLeft:
        newValue = *value - interval;
          if (!IsRepeat && newValue < min && max != INT_MAX)
            newValue = max;
        break;
      case kRight:
        newValue = *value + interval;
        if (!IsRepeat && newValue > max && min != INT_MIN)
          newValue = min;
        break;
      default:
        if (*value < min) { *value = min; Set(); }
        if (*value > max) { *value = max; Set(); }
        return state;
    }
    if (newValue != *value && min <= newValue && newValue <= max) {
      *value = newValue;
      Set();
    }
    state = osContinue;
  }
  return state;
}


//////////////////////////////////////////////////////////////////////////////


static const char* const fEpgKeys[] = 
{
  "Up", "Down", "Menu", "Ok", "Back", "Left", "Right", "Red", "Green", "Yellow",
  "Blue", "Play", "Pause", "Stop", "Record", "FastFwd", "FastRew", "ChanUp", "ChanDn",
  "User1", "User2", "User3", "User4", "User5", "User6", "User7", "User8", "User9"
};


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
  newTrans = config.trans;
  newTimeout = config.timeout;
  newChanOrder = config.chanOrder;
  newTimeFormat24h = config.timeFormat24h;
  newHideMenuEntry = config.hideMenuEntry;
  newTVMode = config.TVMode;
  newNumRows = config.numRows;
  newNumCols = config.numCols;
  newUseSmallFont = config.useSmallFont;
  newTimeInterval = config.timeInterval;
  newDarkenBg = config.darkenBg;
  
  
  AddCategory( tr("General Options") );
  Add(new cMenuEditBoolItem(tr("Mute On Open"), &newMute));
  Add(new cMenuEditBoolItem(tr("Center Info Text"), &newCenterInfo));
  Add(new cMenuEditBoolItem(tr("Hide Main Menu Entry"), &newHideMenuEntry));
  Add(new cMenuEditBoolItem(tr("Channel Order"), &newChanOrder, tr("descending"), tr("ascending")));
  Add(new cMenuEditIntItem(tr("Input Timeout (ms)"), &newTimeout, 250, 3000));
  Add(new cMenuEditBoolItem(tr("Time Format"), &newTimeFormat24h, tr("12h"), tr("24h")));
  Add(new cMenuEditIntIntervalItem(tr("Time Interval (min)"), &newTimeInterval, 10, 60, 5));
  AddEmptyLine();
  
  AddCategory( tr("Size and Position") );
  Add(new cMenuEditBoolItem(tr("TV Mode"), &newTVMode, "PAL", "NTSC"));
  Add(new cMenuEditIntItem(tr("Number of Rows"), &newNumRows, 6, MAX_ROWS));
  Add(new cMenuEditIntItem(tr("Number of Columns"), &newNumCols, 2, MAX_COLS));
  Add(new cMenuEditBoolItem(tr("Font Size"), &newUseSmallFont, tr("normal"), tr("small")));
  Add(openScreenMenu = new cOsdItem( tr("Setup Display...") ));
  AddEmptyLine();
 
  AddCategory( tr("Color Configuration") );
  Add(new cMenuEditIntItem(tr("Opacity"), &newTrans, 0, 255));
  Add(new cMenuEditBoolItem(tr("Darken background"), &newDarkenBg));
  Add(openColorMenu = new cOsdItem( tr("Open Color Setup...") ));
  AddEmptyLine();
    
  AddCategory( tr("Key Configuration") );
  int numOfKeys = sizeof(fEpgKeys)/sizeof(char*);
  Add(new cMenuEditStraItem(tr("Channel Up"),   &newKeyUp,       numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Channel Down"), &newKeyDown,     numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Scroll Right"), &newKeyRight,    numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Scroll Left"),  &newKeyLeft,     numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Get Info"),     &newKeyInfo,     numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Page Up"),      &newKeyPageUp,   numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Page Down"),    &newKeyPageDown, numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Exit"),         &newKeyBack,     numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Select"),       &newKeyOk,       numOfKeys, fEpgKeys));
  Add(new cMenuEditStraItem(tr("Record"),       &newKeyRecord,   numOfKeys, fEpgKeys));

}


//----------------------------------------------------------------------------

eOSState cMenuSetupFEpg::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
 
  cOsdItem* selected = Get(Current()); 
  
  switch (Key) {
    case kOk: 
      Store();
      
      if (selected == openScreenMenu) {
        requestScreenMenu = true;
        state = osEnd;
        cRemote::CallPlugin("fepg");
      }
      else if (selected == openColorMenu) {
        requestColorMenu = true;
        state = osEnd;
        cRemote::CallPlugin("fepg");
      }
      else {
        state = osBack;
      }  
      break;  
    default: break;
  }
  
  return state;  
}


//----------------------------------------------------------------------------

void cMenuSetupFEpg::Store(void)
{
  SetupStore("keyUp",       config.keyUp       = newKeyUp);
  SetupStore("keyDown",     config.keyDown     = newKeyDown);
  SetupStore("keyLeft",     config.keyLeft     = newKeyLeft);
  SetupStore("keyRight",    config.keyRight    = newKeyRight);
  SetupStore("keyInfo",     config.keyInfo     = newKeyInfo);
  SetupStore("keyPageUp",   config.keyPageUp   = newKeyPageUp);
  SetupStore("keyPageDown", config.keyPageDown = newKeyPageDown);
  SetupStore("keyBack",     config.keyBack     = newKeyBack);
  SetupStore("keyOk",       config.keyOk       = newKeyOk);
  SetupStore("keyRecord",   config.keyRecord   = newKeyRecord);
  
  SetupStore("mute",          config.mute = newMute);
  SetupStore("centerInfo",    config.centerInfo = newCenterInfo);
  SetupStore("trans",         config.trans = newTrans);
  SetupStore("timeout",       config.timeout = newTimeout);
  SetupStore("chanOrder",     config.chanOrder = newChanOrder);
  SetupStore("timeFormat24h", config.timeFormat24h = newTimeFormat24h);
  SetupStore("hideMenuEntry", config.hideMenuEntry = newHideMenuEntry);
  SetupStore("TVMode",        config.TVMode = newTVMode);
  SetupStore("numRows",       config.numRows = newNumRows);
  SetupStore("numCols",       config.numCols = newNumCols);
  SetupStore("useSmallFont",  config.useSmallFont = newUseSmallFont);
  SetupStore("timeInterval",  config.timeInterval = newTimeInterval);
  SetupStore("darkenBg",      config.darkenBg = newDarkenBg);
}


//----------------------------------------------------------------------------
// Taken from Streamdev plugin
void cMenuSetupFEpg::AddCategory(const char *Title) 
{
  char* buffer = NULL;

  asprintf(&buffer, "--- %s -------------------------------------------------"
   		              "---------------", Title );

  cOsdItem* item = new cOsdItem(buffer);
  free(buffer);

	item->SetSelectable(false);
  Add(item);
}


//----------------------------------------------------------------------------

void cMenuSetupFEpg::AddEmptyLine(void)
{
  cOsdItem* item = new cOsdItem("");
  item->SetSelectable(false);
  Add(item);
}


//////////////////////////////////////////////////////////////////////////////



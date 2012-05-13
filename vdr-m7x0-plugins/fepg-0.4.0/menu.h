#ifndef ___FEPGMENU_H
#define ___FEPGMENU_H

#include <vdr/plugin.h>
#include <vdr/interface.h>


//////////////////////////////////////////////////////////////////////////////


#define ASCENDING  1
#define DESCENDING 0

#define MAX_ROWS  10
#define MAX_COLS   6


//////////////////////////////////////////////////////////////////////////////


class cMenuSetupFEpg : public cMenuSetupPage 
{
public:
  cMenuSetupFEpg(void);
  
protected:
  virtual void Store(void);
  virtual eOSState ProcessKey(eKeys key);
    
  void AddCategory(const char *Title);
  void AddEmptyLine(void);
  
private:
  int newKeyUp;
  int newKeyDown;
  int newKeyLeft;
  int newKeyRight;
  int newKeyInfo;
  int newKeyPageUp;
  int newKeyPageDown;
  int newKeyBack;
  int newKeyOk;
  int newKeyRecord;
  
  int newMute;
  int newCenterInfo;
  
  int newTrans;
  
  int newTimeout;
  int newChanOrder;
  int newTimeFormat24h;
  int newHideMenuEntry;
  
  int newTVMode;
  int newNumRows;
  int newNumCols;
  int newUseSmallFont;
  int newTimeInterval;
  int newDarkenBg;
  
  cOsdItem* openScreenMenu;
  cOsdItem* openColorMenu;
};
  
  
//////////////////////////////////////////////////////////////////////////////


#endif //___FEPGMENU_H

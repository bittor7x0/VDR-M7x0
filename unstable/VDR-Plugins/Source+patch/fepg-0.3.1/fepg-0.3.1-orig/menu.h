#ifndef ___FEPGMENU_H
#define ___FEPGMENU_H

#include <vdr/plugin.h>
#include <vdr/interface.h>


//////////////////////////////////////////////////////////////////////////////


#define ASCENDING  1
#define DESCENDING 0

#define MAX_ROWS  10


//////////////////////////////////////////////////////////////////////////////


class cMenuSetupFEpg : public cMenuSetupPage 
{
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
  
  int newClr1Red;
  int newClr1Green;
  int newClr1Blue;
  int newClr2Red;
  int newClr2Green;
  int newClr2Blue;
  int newTrans;
  
  int newTimeout;
  int newChanOrder;
  int newTimeFormat24h;
  int newHideMenuEntry;
  
  int newTVMode;
  int newNumRows;
  int newUseSmallFont;
  
  class cOpenScreenMenu : public cOsdItem
  {
  private:
    cMenuSetupFEpg* parent;
  public:
    cOpenScreenMenu(cMenuSetupFEpg* p): cOsdItem("Open Screen Setup...") { parent = p; }
    virtual eOSState ProcessKey(eKeys Key);
  };
  
protected:
  virtual void Store(void);
  void AddCategory(const char *Title);
  void AddEmptyLine(void);
  
public:
  cMenuSetupFEpg(void);
  
};
  
  
//////////////////////////////////////////////////////////////////////////////


#endif //___FEPGMENU_H

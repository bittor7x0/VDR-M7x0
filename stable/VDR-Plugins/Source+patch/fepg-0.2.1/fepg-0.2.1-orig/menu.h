#ifndef ___FEPGMENU_H
#define ___FEPGMENU_H

#include <vdr/plugin.h>
#include <vdr/interface.h>



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
  int newKeyScreenShot;
  int newKeyBack;
  int newKeyOk;
  int newKeyRecord;
  
  static const int NUM_OF_KEYS = 27;
  
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

protected:
  virtual void Store(void);
public:
  cMenuSetupFEpg(void);
};
  
  
//////////////////////////////////////////////////////////////////////////////


#endif //___FEPGMENU_H

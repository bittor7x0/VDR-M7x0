#ifndef __BROWSEMENU_H
#define __BROWSEMENU_H

#include <vdr/plugin.h>
#include <vdr/interface.h>


//////////////////////////////////////////////////////////////////////////////


class cMenuSetupBrowse : public cMenuSetupPage 
{
private:
  int newCenterInfo;
  int newTimeFormat24h;
  int newHideMenuEntry;
  int newNTSC;
  
protected:
  virtual void Store(void);
 
public:
  cMenuSetupBrowse(void);
};



//////////////////////////////////////////////////////////////////////////////

#endif //__BROWSEMENU_H

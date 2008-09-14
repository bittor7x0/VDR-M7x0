#ifndef ___RADIOINFOSETUPMENU_H
#define ___RADIOINFOSETUPMENU_H

#include <vdr/plugin.h>


class cRadioInfoSetupMenu : public cMenuSetupPage 
{
private:
  int  newMAX_RETRIES;
  int  newQUICK_DETECT;
  int  newDELAY;
  int  newSCAN_DELAY; 
  
protected:
  virtual void Store(void);
  
public:
  cRadioInfoSetupMenu(void);
};


#endif //___RADIOINFOSETUPMENU_H

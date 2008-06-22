#ifndef ___RADIOINFOSETUPMENU_H
#define ___RADIOINFOSETUPMENU_H

#include <vdr/plugin.h>


///////////////////////////////////////////////////////////////////////////////


class cRadioInfoSetupMenu : public cMenuSetupPage 
{
private:
  int newMaxRetries;
  int newQuickDetect;
  int newOsdDelay;
  int newScanDelay; 
  
protected:
  virtual void Store(void);
  
public:
  cRadioInfoSetupMenu(void);
};


///////////////////////////////////////////////////////////////////////////////


#endif //___RADIOINFOSETUPMENU_H

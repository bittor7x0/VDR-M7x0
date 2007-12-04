#ifndef __PILOTSKINSETUP_H
#define __PILOTSKINSETUP_H

#include <vdr/plugin.h>

class cPilotskinSetup : public cMenuSetupPage 
{
private:
  int Red;
  int Green;
  int Blue;
  int CloseOnSwitch;
  int Hidemenu;
protected:
  virtual void Store(void);
public:
  cPilotskinSetup(void);
};

#endif //__PILOTSKINSETUP_H


#ifndef __PILOTSETUP_H
#define __PILOTSETUP_H

#include <vdr/plugin.h>

class cPilotSetup : public cMenuSetupPage 
{
private:
  int Red;
  int Green;
  int Blue;
  int Originx;
  int Originy;
  int Width;
  int Height;
  int Alpha1, Alpha2, AlphaBorder;
  int CloseOnSwitch;
  int UseDXR3;
protected:
  virtual void Store(void);
public:
  cPilotSetup(void);
};

#endif //__PILOTSETUP_H


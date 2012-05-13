#ifndef __PREFERSETUP_H
#define __PREFERSETUP_H

#include <vdr/plugin.h>

class cPreferSetup : public cMenuSetupPage 
{
private:
  //int Red;
  //int Green;
  //int Blue;
  int Originx;
  int Originy;
  int Width;
  int Height;
  //int Alpha1, Alpha2, AlphaBorder;
  int CloseOnSwitch;
  int SortBy;
  int Lines;
protected:
  virtual void Store(void);
public:
  cPreferSetup(void);
};

#endif //__PREFERSETUP_H


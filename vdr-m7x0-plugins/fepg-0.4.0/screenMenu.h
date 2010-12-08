#ifndef __FEPG_SCREEN_MENU_H
#define __FEPG_SCREEN_MENU_H

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include "config.h"


//////////////////////////////////////////////////////////////////////////////


class cScreenMenu : public cOsdObject 
{
private:
  int x1, x2, y1, y2;
  int chanWidth, timeHeight;
  int x1p, x2p, y1p, y2p;
  int width, height;
  cOsd* scrOsd;
  const cFont* font;
  
  static const int lt = 3;  // Line thickness
  static const int inc = 2; // Movement Increment
  
  void Draw(void);
  void Store(void);
  
public:
  cScreenMenu(void);
 ~cScreenMenu();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};


//////////////////////////////////////////////////////////////////////////////


#endif //__FEPG_SCREEN_MENU_H

#ifndef __FEPG_COLOR_MENU_H
#define __FEPG_COLOR_MENU_H

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include "config.h"


//////////////////////////////////////////////////////////////////////////////


class cColorMenu : public cOsdObject
{ 
public:
  cColorMenu(void);
 ~cColorMenu();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
   
private:
  int screenHeight;
  cOsd* scrOsd;
  const cFont* font;
  
  tColor color1;
  tColor color2;
  
  int sel;
  int colors[6];
  
  int inputNum;
  time_t inputTime;
  
  static const int lt = 3;  // Line thickness
  
  void Draw(void);
  void DrawColor(int x, int y, const char* s, int val, int i);
  void Store(void);  
};


//////////////////////////////////////////////////////////////////////////////


#endif //__FEPG_COLOR_MENU_H

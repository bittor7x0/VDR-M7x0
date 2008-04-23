#ifndef _OSDADJUST_SCREENMENU__H
#define _OSDADJUST_SCREENMENU__H

#include <vdr/plugin.h>
#include "config.h"

const char* OsdErrorToString(eOsdError err);

enum {            //AARRGGBB
  myTransparent50 = 0x7F000000,
  myTransparent   = 0x00000000,
  myGray        = 0xFF7F7F7F,
  myRed         = 0xFFFF0000,
  myGreen       = 0xFF00FF00,
  myBlue        = 0xFF0000FF,
  myYellow      = 0xFFFFFF00,
  myMagenta     = 0xFFFF00FF,
  myCyan        = 0xFF00FFFF,
  myBlack       = 0xFF000000,
  myGray10      = 0xFF1A1A1A,
  myGray20      = 0xFF333333,
  myGray25      = 0xFF404040,
  myGray30      = 0xFF4D4D4D,
  myGray40      = 0xFF666666,
  myGray50      = 0xFF7F7F7F,
  myGray60      = 0xFF999999,
  myGray70      = 0xFFB3B3B3,
  myGray75      = 0xFFBFBFBF,
  myGray80      = 0xFFCCCCCC,
  myGray90      = 0xFFE5E5E5,
  myWhite       = 0xFFFFFFFF,
};

class cScreenMenu : public cOsdObject {
private:
  bool wasMuted;            // Was the device muted when we entered the Plugin
  bool resetOsd;
  int stateOsd;
  int toggleOsd;
  int x0, x1, y0, y1;
  int x0p, x1p, y0p, y1p;
  int width, height;
  int barHeight, barWidth;
  int offset;
  cOsd *osd;
  const cFont *font;
  int lt;                   // Line thickness
  int inc;                  // Movement Increment
  int myBgColor;        // Background color
  int myFgColor;        // Foreground color
  void Draw(void);
  void DrawColor(void);
  void DrawCycle(int, int);
  void DrawGrid(int, int);
  void Bluge(void);
  void ColorBars(void);
  void GrayBars(void);
  void Osdtest256(void);
  void Store(void);
public:
  cScreenMenu(void);
  virtual ~cScreenMenu();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif // _OSDADJUST_SCREENMENU__H

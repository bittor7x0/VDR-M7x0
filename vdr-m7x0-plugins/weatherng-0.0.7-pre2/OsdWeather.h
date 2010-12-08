#ifndef __OSDWEATHER_H
#define __OSDWEATHER_H

#include <string>
#include <vdr/plugin.h>
#include <vdr/status.h>
#include "setup.h"


class cWetterOsd : public cOsdObject {
private:
  cOsd *osd;
  std::string file;
  int  day;
  int  Radar_left;
  int  Radar_top;
  int  Radar_width;
  int  Radar_height;
  int  row_y;
#ifdef HAVE_4MB
  bool hicolor;
#endif
  bool inverted;
  bool corner;
  bool fontsize;
  static cBitmap bmLeft_inv, bmRight_inv, bmDay_inv, bmNight_inv, bmLeft, bmRight, bmDay, bmNight;

public:
  cWetterOsd(void);
  ~cWetterOsd();
  int Satelite(void);
  void GetData(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__OSDWEATHER_H

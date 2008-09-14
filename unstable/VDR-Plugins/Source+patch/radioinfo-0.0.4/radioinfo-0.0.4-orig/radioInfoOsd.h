#ifndef __RADIOINFOOSD_H
#define __RADIOINFOOSD_H


#include <vdr/osd.h>
#include <vdr/osdbase.h>
#include "sRadioInfo.h"



class cRadioInfoOsd : public cOsdObject
{
private:
  cOsd* rOsd;
  cBitmap* header;
  cBitmap* canvas;
  char* name;
  int offsetX, offsetY;
  int dirX, dirY;
  cRadioInfoOsd** radioInfoOsd;
  sRadioInfo* radioInfo;
  
  void draw(void);
  void getEventName(void);

  long lastUpdate;
  
public:
  cRadioInfoOsd(cRadioInfoOsd** Rid, sRadioInfo* Ri);
  ~cRadioInfoOsd();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  virtual eOSState ProcessKey(eKeys Key, bool wantsOsd);


};




#endif //__RADIOINFOOSD_H

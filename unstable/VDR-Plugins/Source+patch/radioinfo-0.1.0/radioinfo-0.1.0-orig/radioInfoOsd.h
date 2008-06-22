#ifndef __RADIOINFOOSD_H
#define __RADIOINFOOSD_H


#include <vdr/osd.h>
#include <vdr/osdbase.h>
#include "radioInfoData.h"


///////////////////////////////////////////////////////////////////////////////


class cRadioInfoOsd : public cThread
{
private:
  cOsd* rOsd;
  cBitmap header;
  cBitmap canvas;
  const char* eventName;
  int offsetX, offsetY;
  int dirY;
  
  cRadioInfoData* radioInfoData;
  
  void Show(void);
  void Draw(void);
  void GetEventName(void);

protected:
  virtual void Action(void);  

public:
  cRadioInfoOsd(cRadioInfoData* Ri);
  ~cRadioInfoOsd();
};


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFOOSD_H

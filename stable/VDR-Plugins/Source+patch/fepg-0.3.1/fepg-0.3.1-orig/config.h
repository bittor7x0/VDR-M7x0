#ifndef __FEPGCONFIG_H
#define __FEPGCONFIG_H


//////////////////////////////////////////////////////////////////////////////


struct sFEpgConfig 
{
  int keyUp;
  int keyDown;
  int keyLeft;
  int keyRight;
  int keyInfo;
  int keyPageUp;
  int keyPageDown;
  int keyBack;
  int keyOk;
  int keyRecord;

  int mute;
  int centerInfo;
  
  int clr1Red;
  int clr1Green;
  int clr1Blue;

  int clr2Red;
  int clr2Green;
  int clr2Blue;
  int trans;  
  
  int timeout;
  int chanOrder;
  int timeFormat24h;
  int hideMenuEntry;
  
  int TVMode;
  
  int sX1, sX2, sY1, sY2;
  int numRows;
  int useSmallFont;
};


//////////////////////////////////////////////////////////////////////////////


extern sFEpgConfig config;

extern bool requestScreenMenu;


//////////////////////////////////////////////////////////////////////////////


#endif //__FEPGCONFIG_H

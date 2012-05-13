#ifndef __FEPGCONFIG_H
#define __FEPGCONFIG_H


//////////////////////////////////////////////////////////////////////////////


// Set to true if you are using North American providers, false otherwise.
#define BEV_DN true

#define PAL  0
#define NTSC 1


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
  int chanWidth, timeHeight;
  int numRows;
  int numCols;
  int useSmallFont;
  int darkenBg;
  int timeInterval;
};


//////////////////////////////////////////////////////////////////////////////


extern sFEpgConfig config;

extern bool requestScreenMenu;
extern bool requestColorMenu;


//////////////////////////////////////////////////////////////////////////////


#endif //__FEPGCONFIG_H

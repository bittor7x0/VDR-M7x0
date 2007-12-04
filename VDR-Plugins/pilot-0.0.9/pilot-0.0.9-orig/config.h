#ifndef __PILOTCONFIG_H
#define __PILOTCONFIG_H


struct sPilotConfig {
  int red;
  int green;
  int blue;
  int originx;
  int originy;
  int width;
  int height; //Not configurable through OSD
  int alpha1;
  int alpha2;
  int alphaborder;
  int closeonswitch;
  int usedxr3;
  int lines;
  int hidemenu;
};

extern sPilotConfig config;

// #define DEBUG

#endif //__PILOTCONFIG_H


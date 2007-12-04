#ifndef __PREFERCONFIG_H
#define __PREFERCONFIG_H

struct sPreferConfig {
  //int red;
  //int green;
  //int blue;
  int originx;
  int originy;
  int width;
  int height; //Not configurable through OSD
  //int alpha1;
  //int alpha2;
  //int alphaborder;
  int closeonswitch;
  int sortby;
  int lines;
};

extern sPreferConfig config;

#endif //__PREFERCONFIG_H


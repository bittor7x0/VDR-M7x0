#ifndef _OSDADJUST_CONFIG__H
#define _OSDADJUST_CONFIG__H

struct sOsdadjustConfig
{
  int AspectRatio;
  int VideoFormat;
  int Mute;
  int HideMenuEntry;
  int HideHelpText;
  int ShowGrid;
  int BgColor;
  int FgColor;
  int LineTn;         // Line  thickness;
  int MoveInc;        // Movement Increment
  const char *MainMenuEntry;
};

extern sOsdadjustConfig config;

#endif //_OSDADJUST_CONFIG__H

#ifndef __SRADIOINFO_H
#define __SRADIOINFO_H

struct sRadioInfo
{
  char title[256];
  char artist[256];
  char extra1[256];
  char extra2[256];
  char extra3[256];
  
  bool updating;
};

#endif //__SRADIOINFO_H

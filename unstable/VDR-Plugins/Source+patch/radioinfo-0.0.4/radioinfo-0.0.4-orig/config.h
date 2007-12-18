#ifndef __RADIOINFOCONFIG_H
#define __RADIOINFOCONFIG_H


struct sRadioInfoConfig 
{
  int  MAX_RETRIES;  // Number of tries before giving up (on info data)
  int  QUICK_DETECT; // Fast detection of Info PID 
  int  DELAY;        // OSD update delay
  int  SCAN_DELAY;   // number of seconds between info updates
};

extern sRadioInfoConfig config;



#endif //__RADIOINFOCONFIG_H

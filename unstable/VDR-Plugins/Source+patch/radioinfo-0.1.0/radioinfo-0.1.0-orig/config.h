#ifndef __RADIOINFOCONFIG_H
#define __RADIOINFOCONFIG_H


///////////////////////////////////////////////////////////////////////////////


struct sRadioInfoConfig 
{
  int  maxRetries;  // Number of tries before giving up (on info data)
  int  quickDetect; // Fast detection of Info PID 
  int  osdDelay;    // OSD update delay
  int  scanDelay;   // number of seconds between info updates
};


extern sRadioInfoConfig config;


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFOCONFIG_H

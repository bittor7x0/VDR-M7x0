#ifndef __RADIOINFODATA_H
#define __RADIOINFODATA_H

#include <vdr/thread.h>


///////////////////////////////////////////////////////////////////////////////


class cRadioInfoData
{
private:
  cMutex mutex;

public: 
  void Lock(void)   { mutex.Lock();   }
  void Unlock(void) { mutex.Unlock(); }
  void Reset(void);
   
  char title[128];
  char artist[128];
  char extra1[128];
  char extra2[128];
  char extra3[128];
};


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFODATA_H

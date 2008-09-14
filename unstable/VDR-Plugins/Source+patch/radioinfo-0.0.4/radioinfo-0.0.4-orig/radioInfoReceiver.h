#ifndef __RADIOINFORECEIVER_H
#define __RADIOINFORECEIVER_H


#include <vdr/receiver.h>
#include <vdr/thread.h>
#include "sRadioInfo.h"

class cRadioInfoReceiver : public cReceiver
{
private:
  sRadioInfo* radioInfo;
  long lastScan;
  int iPid;
  cDevice* attachedDevice;
   
public:
#if VDRVERSNUM >= 10500
  cRadioInfoReceiver(int Pid, tChannelID ChannelID, sRadioInfo* Ri);
#else
  cRadioInfoReceiver(int Pid, int Ca, sRadioInfo* Ri);
#endif
  
  void Attach(cDevice* device);
  void Detach(void);
  
protected:
  //virtual void Activate(bool On);
  virtual void Receive(uchar *Data, int Length);
   
};








#endif //__RADIOINFORECEIVER_H

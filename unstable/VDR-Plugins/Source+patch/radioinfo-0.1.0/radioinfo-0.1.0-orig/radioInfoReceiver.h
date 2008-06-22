#ifndef __RADIOINFORECEIVER_H
#define __RADIOINFORECEIVER_H


#include <vdr/receiver.h>
#include <vdr/thread.h>
#include "radioInfoData.h"


///////////////////////////////////////////////////////////////////////////////


class cRadioInfoReceiver : public cReceiver
{
private:
  cRadioInfoData* radioInfoData;
  time_t lastScan;
  cDevice* attachedDevice;
   
public:
  cRadioInfoReceiver(int Pid, tChannelID ChannelID, cRadioInfoData* Rid);
  
  void Attach(cDevice* device);
  void Detach(void);
  
protected:
  virtual void Receive(uchar *Data, int Length);
   
};


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFORECEIVER_H

#ifndef __RADIOINFOFILTER_H
#define __RADIOINFOFILTER_H

#include <vdr/filter.h>
#include <vdr/device.h>


///////////////////////////////////////////////////////////////////////////////


class cRadioInfoFilter : public cFilter
{
private: 
  cRadioInfoFilter(void);
  
  static cRadioInfoFilter* instance;
  cDevice* attachedDevice;
  int numOfRetries;
   
public:
  static cRadioInfoFilter* Instance(void);
  
  void Attach(cDevice* device);
  void Detach(void);
  
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);
  virtual void SetStatus(bool On);

};


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFOFILTER_H

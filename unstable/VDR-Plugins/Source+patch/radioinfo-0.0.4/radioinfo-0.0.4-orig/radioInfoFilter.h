#ifndef __RADIOINFOFILTER_H
#define __RADIOINFOFILTER_H

#include <vdr/filter.h>






class cRadioInfoFilter : public cFilter
{
private: 
  cDevice* attachedDevice;
  int numOfRetries;
   
public:
  cRadioInfoFilter();
  
  void Attach(cDevice* device);
  void Detach(void);
  
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);
  virtual void SetStatus(bool On);

friend class cPluginRadioinfo;
};








#endif //__RADIOINFOFILTER_H

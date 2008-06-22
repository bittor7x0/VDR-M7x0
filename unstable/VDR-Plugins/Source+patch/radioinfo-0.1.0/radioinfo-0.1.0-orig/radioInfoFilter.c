#include <vdr/plugin.h>
#include <vdr/filter.h>
#include  <vdr/device.h>

#include "libsi/section.h"
#include "libsi/descriptor.h"

#include "radioinfo.h"
#include "radioInfoFilter.h"
#include "tools.h"
#include "config.h"


///////////////////////////////////////////////////////////////////////////////


cRadioInfoFilter* cRadioInfoFilter::instance = NULL;


///////////////////////////////////////////////////////////////////////////////


cRadioInfoFilter::cRadioInfoFilter()
{
  DEBUG_MSG("RadioInfoFilter Created");

  numOfRetries = 0;
  attachedDevice = NULL;
  
  if (config.quickDetect)
  {
    DEBUG_MSG("RadioInfoFilter Fast PID Detection Enabled.");
    Set(0x20, 0x02); 
    Set(0x21, 0x02);
  } 
  else
  {
    DEBUG_MSG("RadioInfoFilter Thorough PID Detection Enabled.");
    Set(0x00, 0x00);
  }
}


//-----------------------------------------------------------------------------

cRadioInfoFilter* cRadioInfoFilter::Instance(void)
{
  if (!instance)
    instance = new cRadioInfoFilter();
    
  return instance;
}


//-----------------------------------------------------------------------------

void cRadioInfoFilter::Attach(cDevice* device)
{
  if (!attachedDevice) 
  {
    attachedDevice = device;
    if (attachedDevice) 
    {
      attachedDevice->AttachFilter(this);
      DEBUG_MSG("RadioInfoFilter Attached.");
    }
  }
}


//-----------------------------------------------------------------------------

void cRadioInfoFilter::Detach(void)
{
  if (attachedDevice) 
  {
    attachedDevice->Detach(this);
    attachedDevice = NULL;
    DEBUG_MSG("RadioInfoFilter Detached.");
  }
}
  

//-----------------------------------------------------------------------------

void cRadioInfoFilter::SetStatus(bool On)
{
  numOfRetries = 0;
  cFilter::SetStatus(On);
}


//-----------------------------------------------------------------------------  

void cRadioInfoFilter::Process(u_short Pid, u_char Tid, const u_char *Data, int Length)
{
  DEBUG_MSG("radioInfoFilter Receiving Data.");
  
  numOfRetries++;
  if (numOfRetries > config.maxRetries)
  {
    DEBUG_MSG("RadioInfoFilter unable to find info PID");
    SetStatus(false);
    cPluginRadioinfo::currentRadioInfo->FoundInfoPid(-1);
    return;
  }


  if (Pid == 0x00 && Tid == 0x00) 
  {
    SI::PAT pat(Data, false);
    if (!pat.CheckCRCAndParse()) return;          
    SI::PAT::Association assoc;
  
    for (SI::Loop::Iterator it; pat.associationLoop.getNext(assoc, it); ) 
    {
      if (!assoc.isNITPid()) 
      {
        if (Channels.GetByServiceID(Source(), Transponder(), assoc.getServiceId())) 
        {
          Add(assoc.getPid(), 0x02);
          break;
        }
      }
    }
  }
  else
  {
    SI::PMT pmt(Data, false);
    if (!pmt.CheckCRCAndParse())  return;
      
    cChannel* Channel = Channels.GetByServiceID(Source(), Transponder(), pmt.getServiceId());
    if (Channel && Channel == Channels.GetByNumber(cDevice::CurrentChannel()) ) 
    {
      SI::PMT::Stream stream;
        
      for (SI::Loop::Iterator it; pmt.streamLoop.getNext(stream, it); ) 
      {
        if (stream.getStreamType() == 5) 
        {
          DEBUG_MSG("Found Info PID %d", stream.getPid());
          cPluginRadioinfo::currentRadioInfo->FoundInfoPid(stream.getPid());
          SetStatus(false);
        }
      }
    }
  }
    
}


///////////////////////////////////////////////////////////////////////////////


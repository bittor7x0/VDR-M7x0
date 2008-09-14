#include <vdr/receiver.h>

#include "radioInfoReceiver.h"  
#include "sRadioInfo.h"
#include "tools.h"  
#include "radioinfo.h"
#include "config.h"


#if VDRVERSNUM >= 10500
cRadioInfoReceiver::cRadioInfoReceiver(int Pid, tChannelID ChannelID, sRadioInfo* Ri) : cReceiver(ChannelID, -1, Pid)
#else
cRadioInfoReceiver::cRadioInfoReceiver(int Pid, int Ca, sRadioInfo* Ri) : cReceiver(Ca, -1, Pid)
#endif
{
  DEBUG_MSG("RadioInfoReceiver Created.");
  radioInfo = Ri;
  lastScan = 0;
  iPid = Pid;
  attachedDevice = NULL;
}


void cRadioInfoReceiver::Attach(cDevice* device)
{
if (!attachedDevice) 
  {
    attachedDevice = device;
    if (attachedDevice) 
    {
      attachedDevice->AttachReceiver(this);
      DEBUG_MSG("RadioInfoReceiver Attached.");
    }
  }
}


void cRadioInfoReceiver::Detach(void)
{
  if (attachedDevice) 
  {
    attachedDevice->Detach(this);
    attachedDevice = NULL;
    DEBUG_MSG("RadioInfoReceiver Detached.");
  }
}


void cRadioInfoReceiver::Receive(uchar *Data, int Length) 
{
  if (time(NULL) - lastScan <= config.SCAN_DELAY)  return;
  
  DEBUG_MSG("Updating Info");
  
  /*
  fprintf(stderr,">>> DATA length=%d\n", Length);
  for (int i=13; i<Length && Data[i] != 0xFF; i++)
    fprintf(stderr,"%02X ", Data[i]);
    
  fprintf(stderr, "\n");
  
    fprintf(stderr,">>> CHAR\n");
  for (int i=13; i<Length && Data[i] != 0xFF; i++)
    fprintf(stderr,"%c", Data[i]);
    
  fprintf(stderr, "\n");
  */
  
  radioInfo->updating = true;
  
  if (Length < 13) return;
  
  int line=0; int p=0;
  int i=13; // Ignore 12 byte header
  bool end = false;
  
  while (i<Length && Data[i] != 0xFF && !end)
  {
    char c = Data[i];
    
    if (c == 0x00) { // End of data   
      c = '\0';
      end = true;
    }  
    
    if (c == 0x0A)  c = '\0'; // Line Break
    
    switch (line)
    {
      case 0:
        radioInfo->title[p] = c;
        break;
        
      case 1:
        radioInfo->artist[p] = c; 
        break;
         
      case 2:
        radioInfo->extra1[p] = c; 
        break;
         
      case 3:
        radioInfo->extra2[p] = c;
        break;
        
      case 4:
        radioInfo->extra3[p] = c;
        break;
    
    }
    
    p++;
    i++;
  
    if (c == '\0')
    {
      p = 0;
      line++;
    }
  }
  
  radioInfo->updating = false;
  
  lastScan = time(NULL); 
}





/********************************************************************************
 * scan.c
 * provides scanning through given tansponder lists
 * writen by reel-multimedia
 *
 * Contact to mhahn@reel-multimedia.com
 ********************************************************************************/


#include <time.h>
#include "m7x0_dvb/frontend.h"
#include <sys/ioctl.h>
#include <vdr/device.h>
#include <vdr/sources.h>
#include "channelscan.h"
#include "csmenu.h"
#include "scan.h"
#include <stdio.h>
#include "debug.h"

#define SCAN_DELAY 30
#define DVBS_LOCK_TIMEOUT 8000

//bool scanning_on_receiving_device = false;

using std::cout;

cScan::cScan()
{
  origUpdateChannels = Setup.UpdateChannels;
  printf ("DEBUG [channelscan]  %s  \n",__PRETTY_FUNCTION__);

  ::Setup.UpdateChannels = 4;
  //cMenuChannelscan::scanState = 0;
  cTransponders &transponders = cTransponders::GetInstance();
  sourceCode = transponders.SourceCode();


  newChannels = 0;
  fd_frontend= -1;
  cardnr = -1;
  transponderNr = 0;
  channelNumber = 0;
  frequency = 0;
  foundNum=0;
  nitScan = false;

  NFilter=NULL;
  PFilter=NULL;
  SFilter=NULL;
  EFilter=NULL;
}

//--------- Destructor ~cScan -----------------------------------

cScan::~cScan()
{
  Cancel(5000);

  printf ("DEBUG [channelscan]  %s  \n",__PRETTY_FUNCTION__);
  ::Setup.UpdateChannels =  origUpdateChannels;
  //worst case
  //cMenuChannelscan::scanning = false;
  if (cMenuChannelscan::scanState <= ssGetChannels)
    cMenuChannelscan::scanState = ssInterrupted;
  scanning_on_receiving_device = false;
  
  if (fd_frontend > 0)
     close(fd_frontend);

  if (NFilter) 
  {
    cDevice::GetDevice(cardnr)->Detach(NFilter);
    delete NFilter;
    NFilter=NULL;
  }

  if (PFilter)  
  {
    cDevice::GetDevice(cardnr)->Detach(PFilter);
    delete PFilter;
    PFilter=NULL;
  }
  if (SFilter)
  {
    cDevice::GetDevice(cardnr)->Detach(SFilter);
    delete SFilter;
    SFilter=NULL;
  }
  if (EFilter)
  {
    cDevice::GetDevice(cardnr)->Detach(EFilter);
    delete EFilter;
    EFilter=NULL;
  }
  printf ("DEBUG [channelscan]  %s end cTransponders::Destroy(); \n",__PRETTY_FUNCTION__);
}

void cScan::ShutDown()
{
  if (cMenuChannelscan::scanState <= ssGetChannels)
    cMenuChannelscan::scanState = ssInterrupted;
  scanning_on_receiving_device = false;
  Cancel(5000);
}

bool cScan::StartScanning(scanParameters *scp)
{
    if ( scp->frequency == 1|| scp->frequency == 2)
    {
        nitScan = true;
    }
    else 
    {
        nitScan = false;
    }
    //nitScan = scp->frequency == 2?true:false;
        
    cTransponders &transponders = cTransponders::GetInstance();
    // nit scan takes transponder lists from SI Network Information Table

    if (scp->frequency == 2)
    {
       cMenuChannelscan::scanState = ssGetTransponders;
    }
    else 
    {
       cMenuChannelscan::scanState = ssGetChannels;
    }

    if (cMenuChannelscan::scanState != ssGetTransponders && transponders.v_tp_.size()==0)
    {
        esyslog(ERR " Empty Transponderlist");
        cMenuChannelscan::scanState = ssNoTransponder;
        return false;
    }
    
    detailedSearch=scp->detail;
    scanMode=scp->symbolrate_mode;

    // Reset internal scan states
    lastLocked=1; // for safety
    srstat=-1;
    
    cDevice::PrimaryDevice()->StopReplay();

    cardnr = scp->card;
    DLOG (DBG " Stop Replay Card Number %d", cardnr);

    scanning_on_receiving_device=true;

    char buffer[265];
    snprintf(buffer,sizeof(buffer),"/dev/ost/frontend%d", cardnr);

    fd_frontend = open(buffer,O_RDONLY | O_NONBLOCK);
    if (fd_frontend<=0)
    {
       esyslog ("cant open device: %s ", buffer);
       cMenuChannelscan::scanState = ssDeviceFailure;
       return false;
    }
    Start();
    return true;
}

//-------------------------------------------------------------------------
uint32_t cScan::getSignal()
{
  uint32_t value = 0;
  CHECK(ioctl(fd_frontend, FE_READ_SIGNAL_STRENGTH, &value));
  return value;
}

uint32_t cScan::getSNR()
{
  uint32_t value = 0;
  CHECK(ioctl(fd_frontend, FE_READ_SNR, &value));
  return value;
}

uint32_t cScan::getBER()
{
  uint32_t value = 0;
  CHECK(ioctl(fd_frontend, FE_READ_BER, &value));
  return value;
}

uint16_t cScan::getStatus()
{
  fe_status_t value;
  CHECK(ioctl(fd_frontend, FE_READ_STATUS, &value));
  return value;
}
//-------------------------------------------------------------------------

void cScan::ScanServices()
{
  cMenuChannelscan::scanState = ssGetChannels;
#ifdef WITH_EIT 
  printf ("DEBUG [channelscan]: With EIT\n");
  EFilter = new cEitFilter();
#endif
  PFilter = new PatFilter();
  SFilter = new SdtFilter(PFilter);
  PFilter->SetSdtFilter(SFilter);

  device->AttachFilter(SFilter);
  device->AttachFilter(PFilter);
#ifdef WITH_EIT 
  device->AttachFilter(EFilter);
#endif

  time_t start=time(NULL);

  int foundSids=0;
  foundNum=totalNum=0;
  // Heuristic: Delay scan timeout if Sids or Services withs PIDs are found

  while(!PFilter->EndOfScan() && (
     //(time(NULL) - start < SCAN_DELAY && cMenuChannelscan::scanning) ||
     (time(NULL) - start < SCAN_DELAY && cMenuChannelscan::scanState == ssGetChannels || 
       (time(NULL)-PFilter->LastFoundTime() < SCAN_DELAY)))) {

        PFilter->GetFoundNum(foundNum,totalNum);
        if (totalNum && !foundSids) {
          start=time(NULL);
          foundSids=1;
        }
        usleep(200*1000);
    }

  if (cMenuChannelscan::scanState>=ssInterrupted && !PFilter->EndOfScan())
  {
    printf ("DEBUG [channelscan]: ScanServices aborted %d \n", cMenuChannelscan::scanState);
  }
  usleep(200*1000);
  PFilter->GetFoundNum(foundNum,totalNum);

  device->Detach(PFilter);
  device->Detach(SFilter);
#ifdef WITH_EIT 
  device->Detach(EFilter);
#endif

  if (PFilter) 
      delete PFilter;
  PFilter = NULL;
  if (SFilter)
     delete SFilter;
  SFilter = NULL;
#ifdef WITH_EIT 
  if (EFilter)
      delete EFilter;
  EFilter = NULL;
#endif
#ifdef M750S
  if (nitScan)
     ScanNitServices();
#endif
}

#ifdef M750S
//-------------------------------------------------------------------------
void cScan::ScanNitServices()
{

  printf (" DEBUG [cs]; ScanNITService \n");
  NFilter = new NitFilter();
  time_t start=time(NULL);
   
  device->AttachFilter(NFilter);

  // updates  status bar in cMenuScanActive 
  if ( cMenuChannelscan::scanState == ssGetTransponders)
    transponderNr = 0;
                                   
  while (!NFilter->EndOfScan() && 
          (time(NULL) - start < (SCAN_DELAY) && 
           ( cMenuChannelscan::scanState == ssGetTransponders ||
             cMenuChannelscan::scanState == ssGetChannels)))
  {
     if (NFilter->Found())
     { 
        start = time(NULL);
     }
     // we don`t know how much  tranponder we get 
     if (cMenuChannelscan::scanState == ssGetTransponders)
        transponderNr = (int)(time(NULL) - start); 

     usleep(200*1000);
  }
  device->Detach(NFilter);
  if (NFilter)
    delete NFilter;

  NFilter = NULL;

  // display 100% in status bar 
  //while (initalscan &&  transponderNr < 100 && cMenuChannelscan::scanning) 
  while (transponderNr < 100 && cMenuChannelscan::scanState == ssGetTransponders) 
  {
     usleep(200* 1000);
     transponderNr+=2;
  } 
  if (cMenuChannelscan::scanState == ssGetTransponders)
    transponderNr = 0;
  
  printf (" DEBUG [cs]; ScanNITService end \n");
}
//-------------------------------------------------------------------------

void cScan::ScanDVB_S(cTransponder *tp, cChannel *c)
{
  printf (" DEBUG [cs]; ScanDVB_S \n");
  tp->SetTransponderData(c, sourceCode);
  if (!device->SwitchChannel(c,false)) 
  {
     esyslog(ERR "SwitchChannel(%d)  failed", c->Frequency());
     printf(ERR "SwitchChannel(%d)  failed\n", c->Frequency());
#if HAVE_ROTOR
     esyslog(ERR "try Switch rotor ");
     printf(ERR " try Switch rotor \n");

     struct {
       cDevice* device;
       cChannel* channel;
     } data;

     data.device = device;
     data.channel = c; 

     cPlugin *p = cPluginManager::GetPlugin("rotor");
     if (p)
     {

        isyslog("Info [channelscan] Switch rotor \n");
        Plugin->Service("Rotor-SwitchChannel", &data);
        usleep(100*1000);
     }
#endif        
  }
  else if (cDevice::GetDevice(cardnr)->HasLock(DVBS_LOCK_TIMEOUT))
  {
     dsyslog("DEBUG SAT: SNR = %d : BER = %d : SIGNAL = %d)", getSNR(), getBER(), getSignal());
     if (cMenuChannelscan::scanState == ssGetTransponders) {
       ScanNitServices();
     }
     else if (cMenuChannelscan::scanState == ssGetChannels)
       ScanServices();
  }
  else 
  {
     isyslog (" debug [scan] ------------- NO LOCK at transponder %d  -------------\n",  c->Transponder());
  }
}
//-------------------------------------------------------------------------
#endif

// detail bit 0: wait longer
// detail bit 1: search also +-166kHz offset

void cScan::ScanDVB_T(cTransponder *tp, cChannel *c)
{
  int timeout=1000;
  int retries=0;
  int response,n,m;
  int frequency_orig=tp->Frequency();
  int offsets[3]={0,-166666,166666};

  tp->SetFrequency(frequency_orig);

  for(n=0;n<(detailedSearch&2?3:1);n++) 
  {

     //if (!cMenuChannelscan::scanning)
     if (cMenuChannelscan::scanState >= ssInterrupted)
        return;

     tp->SetFrequency(frequency_orig+offsets[n]);
     frequency = tp->Frequency();
     tp->SetTransponderData(c, sourceCode);
        
     if (!device->SwitchChannel(c,false))
       esyslog(ERR "SwitchChannel(c)  failed");

     DLOG("%i Tune %i \n",retries,frequency);
     usleep(1500*2000);
     if (lastLocked)
        sleep(2); // avoid false lock
     
     for(m=0;m<(detailedSearch&1?8:2);m++)
     {
        response=getStatus();
        DLOG("%i RESPONSE %x\n",retries,response);
        dsyslog("DEBUG TER: SNR = %d : BER = %d : SIGNAL = %d)", getSNR(), getBER(), getSignal());    
        
	if (response&0x10==0x10) {// Lock
          break;
        }

        if (response&15>2) // Almost locked, give it some more time
          sleep(1);

        sleep(1);
     }

     //if (!cMenuChannelscan::scanning)
     if (cMenuChannelscan::scanState >= ssInterrupted)
       break;
            
     if (device->HasLock(timeout))
     {
        DLOG(DBG "  ------ HAS LOCK ------");
	ScanServices();
	lastLocked=1;
        return;
     }
     lastLocked=0;
     retries++; 
  }
}
#ifdef M750S
//-------------------------------------------------------------------------
/*
  Scan algorithm for DVB-C: 
  Try 256QAM after 64QAM only if signal strength is high enough
  If initial mod is != 64QAM, then no 64QAM/256QAM auto detection is done

  Try 6900/6875/6111 until lock is achieved, then use found rate for all subsequent scans
  if scanMode is 2, only fixed SR is used.

  Wait additional 3s when last channel had lock to avoid false locking
 */

void cScan::ScanDVB_C(cTransponder *tp,cChannel *c)
{
  int timeout=500;
  int str1,n,m;
  int srtab[3]={6900,6875,6111};
  int fixedModulation=0;
    
  frequency = tp->Frequency();
  DLOG("Scan %f, Modulation %i SR %i\n",frequency/1e6,tp->Modulation(), tp->Symbolrate());
    
  if (tp->Modulation()!=QAM_64)
    fixedModulation=1;

    // SR try loop
  for (m=0;m<3;m++) {
    if (scanMode!=2) { // not manual
      if (srstat!=-1) {
        printf("Use auto SR %i\n",srtab[srstat]);
        tp->SetSymbolrate(srtab[srstat]);
      }
      else {
        printf("Use SR %i\n",srtab[m]);
        tp->SetSymbolrate(srtab[m]);
      }
    }

        // Reset modulation
    if (!fixedModulation)
      tp->SetModulation(QAM_64);

    // try 64QAM/256QAM
    for(n=0;n<2;n++) 
    { 
       //if (!cMenuChannelscan::scanning)
	    // return            
       tp->SetTransponderData(c, sourceCode);
            
       if (!device->SwitchChannel(c,false))
         esyslog(ERR "SwitchChannel(c)  failed");
            
       usleep(500*1000);
            
       str1=getSignal();
            
       if (lastLocked) {
         printf("Wait last lock\n");
         sleep(3);  // avoid false lock
       }
            
       if (device->HasLock(timeout))
       {
           //DLOG(DBG "  ------------- HAS LOCK -------------     ");
          //printf("LOCK @ %.1f\n",tp->Frequency()/1.0e6);
          ScanServices();
          lastLocked=1;
          if (scanMode==0 && srstat==-1)
            srstat=m; // remember SR for the remaining scan

          return;
       }
       lastLocked=0;
            
       if (!fixedModulation && n==0 && str1>0x4000) {
          //printf("QAM256 %x\n",str1);
         tp->SetModulation(QAM_256);
       }
       else break;
    }
        // leave SR try loop without useable signal strength (even in "try all" mode)
    if (str1<0x4000 || (scanMode==0 && srstat!=-1) || scanMode==2) 
            break;
  }
}
#endif // M750S

//---------  cMainMenu::Action()  -----------------------------------

void cScan::Action()
{
  // the one and only "scanning = true" !
  // cMenuChannelscan::scanning = true;

  device = cDevice::GetDevice(cardnr);
  std::auto_ptr<cChannel> c(new cChannel);

  cTransponders &transponders = cTransponders::GetInstance();

  transponderMap.clear();
  TblVersions.clear();


  if (cMenuChannelscan::scanState == ssGetTransponders)
  {
#ifdef M750S
    cTransponder *t = transponders.GetNITStartTransponder();
    if (t && device->ProvidesSource(cSource::stSat))
    {
        // fetch sat Transponders
        ScanDVB_S(t, c.get());
    }
#endif
       // if scanState had been changed meanwhile
    if (cMenuChannelscan::scanState == ssGetTransponders)
    {
       cMenuChannelscan::scanState = ssGetChannels;
       //transponders.v_tp_.clear(); 
       AddTransponders();
    }
    else 
    {
      return;
    }
  }
  constTpIter tp = transponders.v_tp_.begin();

  time_t startTime = time(NULL);
  
  while (Running() && cMenuChannelscan::scanState == ssGetChannels)
  {
  
    printf ("DEBUG [channelscan]: loop through  transponders ++++  size %d ++++++++\n", (int)transponders.v_tp_.size());
  
    unsigned int oldTransponderMapSize = transponderMap.size();
    while (tp != transponders.v_tp_.end())
    {
       //if (!cMenuChannelscan::scanning) break;
       if (cMenuChannelscan::scanState != ssGetChannels) break;
  
       // get counter
       transponderNr = tp - transponders.v_tp_.begin();
       channelNumber = (*tp)->ChannelNum();
       frequency = (*tp)->Frequency();
       printf ("\033[0;44m DEBUG [channelscan]: scan f: %d  \033[0m \n", frequency);
  
       if (device->ProvidesSource(cSource::stTerr))
         ScanDVB_T(*tp,c.get());
#ifdef M750S
       else if (device->ProvidesSource(cSource::stSat))
         ScanDVB_S(*tp,c.get());
       else if (device->ProvidesSource(cSource::stCable))
         ScanDVB_C(*tp,c.get());
#endif  
       ++tp;
    } // Scanning loop
  
    if (cMenuChannelscan::scanState <= ssGetChannels && transponderMap.size() > oldTransponderMapSize) 
    {
        size_t pos = transponders.v_tp_.size();
        AddTransponders();
        tp = transponders.v_tp_.begin()+pos;
    }
    else 
    {
      break;
    }
  }

  int duration = time(NULL) - startTime;
  DLOG(DBG " End of transponderlist. End of scan! Duratation %d ", duration);
  printf( " --- End of transponderlist. End of scan! Duratation %d ---\n", duration);


  if (cMenuChannelscan::scanState == ssGetChannels)
     cMenuChannelscan::scanState = ssSuccess;
  
  //else cMenuChannelscan::scanState = ssInterrupted;
  //cMenuChannelscan::scanning = false;

  DLOG(DBG " End of scan scanState: ssSuccess!");

  ClearMap();
}

//-------------------------------------------------------------------------

void cScan::AddTransponders()
{
  printf( " --- AddTransponders \n");
   cTransponders &transponders = cTransponders::GetInstance();
   for (tpMapIter itr = transponderMap.begin(); itr != transponderMap.end(); ++itr)
   {
      if (cMenuChannelscan::scanState == ssGetTransponders ||
           transponders.MissingTransponder(itr->first))
      {
          printf("add tp: %6d\n", itr->first);
          transponders.v_tp_.push_back(itr->second);
      }
   }
}

void cScan::ClearMap()
{
   cTransponders::GetInstance().Clear();

   while (!transponderMap.empty()) 
   {
      if (transponderMap.begin()->second) {
         printf("delete tp: %6d\n", (*transponderMap.begin()).first);
         //delete transponderMap.begin()->second;
      }
      else 
        printf(" tp: %6d deleted already\n", (*transponderMap.begin()).first);
          
      transponderMap.erase(transponderMap.begin());  
   }
}


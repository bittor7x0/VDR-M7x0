
#include "m7x0_dvb/frontend.h"
#include <map>
#include <vector>
#include <utility>

#include <vdr/tools.h>
#include <libsi/section.h>
#include <libsi/descriptor.h>

#include "csmenu.h"
#include "filter.h"

using std::vector;
using std::map;
using std::set;
using std::make_pair;


std::map<int, cTransponder *> transponderMap;
std::map<int, int> TblVersions;


// --- cCaDescriptor ---------------------------------------------------------

class cCaDescriptor : public cListObject {
private:
   int caSystem;
   bool stream;
   int length;
   uchar *data;
public:
   cCaDescriptor(int CaSystem, int CaPid, bool Stream, int Length, const uchar *Data);
   virtual ~cCaDescriptor();
   bool operator== (const cCaDescriptor &arg) const;
   int CaSystem(void) { return caSystem; }
   int Stream(void) { return stream; }
   int Length(void) const { return length; }
   const uchar *Data(void) const { return data; }
 };

cCaDescriptor::cCaDescriptor(int CaSystem, int CaPid, bool Stream, int Length, const uchar *Data)
{
   caSystem = CaSystem;
   stream = Stream;
   length = Length + 6;
   data = MALLOC(uchar, length);
   data[0] = SI::CaDescriptorTag;
   data[1] = length - 2;
   data[2] = (caSystem >> 8) & 0xFF;
   data[3] =  caSystem       & 0xFF;
   data[4] = ((CaPid   >> 8) & 0x1F) | 0xE0;
   data[5] =   CaPid         & 0xFF;
   if (Length)
     memcpy(&data[6], Data, Length);
}

cCaDescriptor::~cCaDescriptor()
{
  free(data);
}

bool cCaDescriptor::operator== (const cCaDescriptor &arg) const
{
  return length == arg.length && memcmp(data, arg.data, length) == 0;
}

// --- cCaDescriptors --------------------------------------------------------

class cCaDescriptors : public cListObject {
private:
  int source;
  int transponder;
  int serviceId;
  int numCaIds;
  int caIds[MAXCAIDS + 1];
  cList<cCaDescriptor> caDescriptors;
  void AddCaId(int CaId);
public:
  cCaDescriptors(int Source, int Transponder, int ServiceId);
  bool operator== (const cCaDescriptors &arg) const;
  bool Is(int Source, int Transponder, int ServiceId);
  bool Is(cCaDescriptors * CaDescriptors);
  bool Empty(void) { return caDescriptors.Count() == 0; }
  void AddCaDescriptor(SI::CaDescriptor *d, bool Stream);
  int GetCaDescriptors(const unsigned short *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag);
  const int *CaIds(void) { return caIds; }
  };

cCaDescriptors::cCaDescriptors(int Source, int Transponder, int ServiceId)
{
  source = Source;
  transponder = Transponder;
  serviceId = ServiceId;
  numCaIds = 0;
  caIds[0] = 0;
}

bool cCaDescriptors::operator== (const cCaDescriptors &arg) const
{
  cCaDescriptor *ca1 = caDescriptors.First();
  cCaDescriptor *ca2 = arg.caDescriptors.First();
  while (ca1 && ca2) {
     if (!(*ca1 == *ca2))
           return false;
      ca1 = caDescriptors.Next(ca1);
      ca2 = arg.caDescriptors.Next(ca2);
  }
  return !ca1 && !ca2;
}

bool cCaDescriptors::Is(int Source, int Transponder, int ServiceId)
{
  return source == Source && transponder == Transponder && serviceId == ServiceId;
}

bool cCaDescriptors::Is(cCaDescriptors *CaDescriptors)
{
  return Is(CaDescriptors->source, CaDescriptors->transponder, CaDescriptors->serviceId);
}

void cCaDescriptors::AddCaId(int CaId)
{
  if (numCaIds < MAXCAIDS) {
     for (int i = 0; i < numCaIds; i++) {
         if (caIds[i] == CaId)
            return;
     }
     caIds[numCaIds++] = CaId;
     caIds[numCaIds] = 0;
     }
}

void cCaDescriptors::AddCaDescriptor(SI::CaDescriptor *d, bool Stream)
{
  cCaDescriptor *nca = new cCaDescriptor(d->getCaType(), d->getCaPid(), Stream, d->privateData.getLength(), d->privateData.getData());
  for (cCaDescriptor *ca = caDescriptors.First(); ca; ca = caDescriptors.Next(ca)) {
      if (*ca == *nca) {
         delete nca;
         return;
       }
  }
  AddCaId(nca->CaSystem());
  caDescriptors.Add(nca);
}

int cCaDescriptors::GetCaDescriptors(const unsigned short *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag)
{
  if (!CaSystemIds || !*CaSystemIds)
     return 0;
  if (BufSize > 0 && Data) {
     int length = 0;
     int IsStream = -1;
     for (cCaDescriptor *d = caDescriptors.First(); d; d = caDescriptors.Next(d)) {
         const unsigned short *caids = CaSystemIds;
         do {
            if (*CaSystemIds == 0xFFFF || d->CaSystem() == *caids) {
               if (length + d->Length() <= BufSize) {
                  if (IsStream >= 0 && IsStream != d->Stream())
                     DLOG("CAM: different stream flag in CA descriptors");
                  IsStream = d->Stream();
                  memcpy(Data + length, d->Data(), d->Length());
                  length += d->Length();
                  }
               else
                  return -1;

             }
         } while (*++caids);
     }
     StreamFlag = IsStream == 1;
     return length;
     }
  return -1;
}

// --- cCaDescriptorHandler --------------------------------------------------

class cCaDescriptorHandler : public cList<cCaDescriptors> {
private:
  cMutex mutex;
public:
  int AddCaDescriptors(cCaDescriptors *CaDescriptors);
      // Returns 0 if this is an already known descriptor,
      // 1 if it is an all new descriptor with actual contents,
      // and 2 if an existing descriptor was changed.
  int GetCaDescriptors(int Source, int Transponder, int ServiceId, const unsigned short *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag);
  };

int cCaDescriptorHandler::AddCaDescriptors(cCaDescriptors *CaDescriptors)
{
  cMutexLock MutexLock(&mutex);
  for (cCaDescriptors *ca = First(); ca; ca = Next(ca)) {
      if (ca->Is(CaDescriptors)) {
         if (*ca == *CaDescriptors) {
            delete CaDescriptors;
            return 0;
            }
         Del(ca);
         Add(CaDescriptors);
         return 2;
         }
      }
  Add(CaDescriptors);
  return CaDescriptors->Empty() ? 0 : 1;
}

int cCaDescriptorHandler::GetCaDescriptors(int Source, int Transponder, int ServiceId, const unsigned short *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag)
{
  cMutexLock MutexLock(&mutex);
  StreamFlag = false;
  for (cCaDescriptors *ca = First(); ca; ca = Next(ca)) {
      if (ca->Is(Source, Transponder, ServiceId))
         return ca->GetCaDescriptors(CaSystemIds, BufSize, Data, StreamFlag);
      }
  return 0;
}

cCaDescriptorHandler CaDescriptorHandler;

int GetCaDescriptors(int Source, int Transponder, int ServiceId, const unsigned short *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag)
{
  return CaDescriptorHandler.GetCaDescriptors(Source, Transponder, ServiceId, CaSystemIds, BufSize, Data, StreamFlag);
}

// --- PatFilter ------------------------------------------------------------

PatFilter::PatFilter()
{
  sdtFilter=NULL;
  pmtIndex = 0;
  for (int i=0; i<MAXFILTERS; i++)
    pmtPid[i] = 0;
  numPmtEntries = 0;
  Set(0x00, 0x00);  // PAT 
  endofScan=false;
  SetStatus(false);
  pit=pnum=0;
  sdtfinished=false;
  for(int n=0;n<128;n++)
    pSid[n]=-1;
  pSidCnt=0;
  lastFound=0;
  waitingForGodot=0;
}

void PatFilter::SetSdtFilter(SdtFilter *SdtFilter)
{
  sdtFilter=SdtFilter;
  SetStatus(true);
}

void PatFilter::SetStatus(bool On)
{
  cFilter::SetStatus(On);
  pmtIndex = 0;
  for (int i=0; i<MAXFILTERS; i++)
    pmtPid[i] = 0;
  numPmtEntries = 0;
  num=0;
  numRunning=0;
  pit=pnum=0;
}

void PatFilter::Trigger(void)
{
  numPmtEntries = 0;
}

bool PatFilter::PmtVersionChanged(int PmtPid, int Sid, int Version)
{
  uint64_t v = Version;
  v <<= 32;
  uint64_t id = (PmtPid | (Sid << 16)) & 0x00000000FFFFFFFFLL;
  for (int i = 0; i < numPmtEntries; i++) {
      if ((pmtVersion[i] & 0x00000000FFFFFFFFLL) == id) {
         bool Changed = (pmtVersion[i] & 0x000000FF00000000LL) != v;
         if (Changed)
            pmtVersion[i] = id | v;
         return Changed;
         }
      }
  if (numPmtEntries < CMAXPMTENTRIES)
     pmtVersion[numPmtEntries++] = id | v;
  return true;
}

bool PatFilter::SidinSdt(int Sid)
{
  for (int i=0; i<sdtFilter->numSid; i++)
    if (sdtFilter->sid[i]==Sid && sdtFilter->usefulSid[i])
    {
      for (int j=0; j<MAXFILTERS; j++)
        if (Sids[j]==Sid)
          return false;
      return true;
    }
  return false;
}

void PatFilter::Process(u_short Pid, u_char Tid, const u_char *Data, int Length)
{
   if (lastFound<time(NULL) && waitingForGodot<10) {  // There's something on this channel, waiting a bit longer...
    waitingForGodot++;
    lastFound++;
  }

  if (Pid == 0x00) {
    if (Tid == 0x00) {
      for (int i=0; i<MAXFILTERS; i++) {
        if (pmtPid[i] && (time(NULL) - lastPmtScan[i]) > FILTERTIMEOUT) {
          Del(pmtPid[i], 0x02);
          pmtPid[i] =0; // Note for recycling, but do not remove from feed
          pSid[pSidCnt++]=Sids[i];
          num++;
          for (int k=0; i<sdtFilter->numSid; k++)
            if (sdtFilter->sid[k]==Sids[i])
            {
              sdtFilter->sid[k]=0;
              break;
            }
          Sids[i]=0;
          pmtIndex++;
        }
      }
      SI::PAT pat(Data, false);
      if (!pat.CheckCRCAndParse())
        return;
      SI::PAT::Association assoc;

      for (SI::Loop::Iterator it; pat.associationLoop.getNext(assoc, it); ) {
        int xSid=assoc.getServiceId();

        int sidfound=0;

        for(int n=0;n<pSidCnt;n++) {
          if (pSid[n]==xSid) {
            sidfound=1;
            break;
          }
        }
        
        if (!sidfound && !assoc.isNITPid() &&  SidinSdt(assoc.getServiceId())) {

          int Index = 0;
          int foundIndex=0;

          // Find free filter PID
          for(Index=0;Index<MAXFILTERS;Index++) {
            if (pmtPid[Index]==0) {
              foundIndex=1;
              break;
            }
          }
           
          if (foundIndex) {
            pmtPid[Index] = assoc.getPid();
            Sids[Index] = xSid;
            lastPmtScan[Index] = time(NULL);
            Add(pmtPid[Index], 0x02);
            pSid[pSidCnt++]=xSid;
          }
        }
      }
    }
     }
  else if (Tid == SI::TableIdPMT && Source() && Transponder()) {
     int Index=-1;
     for (int i=0; i<MAXFILTERS; i++)
       if (Pid==pmtPid[i])
          Index=i;
     SI::PMT pmt(Data, false);
     if (!pmt.CheckCRCAndParse())
        return;
     if (!Channels.Lock(true, 10)) {
        numPmtEntries = 0; // to make sure we try again
        return;
        }

     cChannel *Channel = Channels.GetByServiceID(Source(), Transponder(), pmt.getServiceId());
     if (Channel && Index!=-1) {
        SI::CaDescriptor *d;
        cCaDescriptors *CaDescriptors = new cCaDescriptors(Channel->Source(), Channel->Transponder(), Channel->Sid());
        // Scan the common loop:
        for (SI::Loop::Iterator it; (d = (SI::CaDescriptor*)pmt.commonDescriptors.getNext(it, SI::CaDescriptorTag)); ) {
            CaDescriptors->AddCaDescriptor(d, false);
            delete d;
            }
        // Scan the stream-specific loop:
        SI::PMT::Stream stream;
        int Vpid = 0;
        int Ppid = pmt.getPCRPid();
        int Vtype = 0;
        int Apids[MAXAPIDS + 1] = { 0 };
        int Dpids[MAXDPIDS + 1] = { 0 };
        int DPpids[MAXDPIDS + 1] = { 0 };
#if VDRVERSNUM >= 10332
        char ALangs[MAXAPIDS + 1][MAXLANGCODE2] = { "" };
        char DLangs[MAXDPIDS + 1][MAXLANGCODE2] = { "" };
#else
        char ALangs[MAXAPIDS + 1][4] = { "" };
        char DLangs[MAXDPIDS + 1][4] = { "" };
#endif
        int Tpid = 0;
        int NumApids = 0;
        int NumDpids = 0;
        for (SI::Loop::Iterator it; pmt.streamLoop.getNext(stream, it); ) {
            switch (stream.getStreamType()) {
              case 1: // STREAMTYPE_11172_VIDEO
              case 2: // STREAMTYPE_13818_VIDEO
              case 0x1B: // MPEG4 
                      Vpid = stream.getPid();
                      Vtype = stream.getStreamType();
                      break;
              case 3: // STREAMTYPE_11172_AUDIO
              case 4: // STREAMTYPE_13818_AUDIO
                      {
                      if (NumApids < MAXAPIDS) {
                         Apids[NumApids] = stream.getPid();
                         SI::Descriptor *d;
                         for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
                             switch (d->getDescriptorTag()) {
                               case SI::ISO639LanguageDescriptorTag: {
                                    SI::ISO639LanguageDescriptor *ld = (SI::ISO639LanguageDescriptor *)d;
                                    if (*ld->languageCode != '-') { // some use "---" to indicate "none"
                                       strn0cpy(ALangs[NumApids], I18nNormalizeLanguageCode(ld->languageCode), 4);
                                       ALangs[NumApids][4] = 0;
                                       }
                                    }
                                    break;
                               default: ;
                               }
                             delete d;
                             }
                         NumApids++;
                         }
                      }
                      break;
              case 5: // STREAMTYPE_13818_PRIVATE
              case 6: // STREAMTYPE_13818_PES_PRIVATE
              //XXX case 8: // STREAMTYPE_13818_DSMCC
                      {
                      int dpid = 0;
                      int dppid = 0;
                      char lang[4] = { 0 };
                      SI::Descriptor *d;
                      for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
                          switch (d->getDescriptorTag()) {
                            case SI::AC3DescriptorTag:
                            case SI::EnhancedAC3DescriptorTag:
                                 dpid = stream.getPid();
                                 dppid = d->getDescriptorTag();
                                 break;
                            case SI::TeletextDescriptorTag:
                                 Tpid = stream.getPid();
                                 break;
                            case SI::ISO639LanguageDescriptorTag: {
                                 SI::ISO639LanguageDescriptor *ld = (SI::ISO639LanguageDescriptor *)d;
                                 strn0cpy(lang, I18nNormalizeLanguageCode(ld->languageCode), 4);
                                 }
                                 break;
                            default: ;
                            }
                          delete d;
                          }
                      if (dpid) {
                         if (NumDpids < MAXDPIDS) {
                            Dpids[NumDpids] = dpid;
                            DPpids[NumDpids] = dppid;
                            strn0cpy(DLangs[NumDpids], lang, 4);
                            NumDpids++;
                            }
                         }
                      }
                      break;
              }
            for (SI::Loop::Iterator it; (d = (SI::CaDescriptor*)stream.streamDescriptors.getNext(it, SI::CaDescriptorTag)); ) {
                CaDescriptors->AddCaDescriptor(d, true);
                delete d;
                }
            }
        Channel->SetPids(Vpid, Vpid ? Ppid : 0, Apids, ALangs, Dpids, DLangs, Tpid, Vtype, DPpids);
        Channel->SetCaIds(CaDescriptors->CaIds());
        Channel->SetCaDescriptors(CaDescriptorHandler.AddCaDescriptors(CaDescriptors));

  Del(pmtPid[Index], 0x02);
  pmtPid[Index]=0;

  num++;
  numRunning++;
  lastFound=time(NULL);
        }
#if 1
     if (Index!=-1)
       lastPmtScan[Index] = 0; // this triggers the next scan
#endif
     Channels.Unlock();
     }
  if (sdtfinished && num>=sdtFilter->numUsefulSid) {
      endofScan=true;
  }
}

void PatFilter::GetFoundNum(int &current, int &total)
{
  current=numRunning;
  total=(sdtFilter?sdtFilter->numSid:0);
  if (total>1000 || total<0)
    total=0;
}

// --- cSdtFilter ------------------------------------------------------------

SdtFilter::SdtFilter(PatFilter *PatFilter)
{      
  patFilter = PatFilter;
  numSid=0;
  numUsefulSid=0;
  Set(0x11, 0x42);  // SDT
  AddServiceType = cMenuChannelscanSetup::SetupServiceType;
}

void SdtFilter::SetStatus(bool On)
{
  cFilter::SetStatus(On);
  sectionSyncer.Reset();
}

void SdtFilter::Process(u_short Pid, u_char Tid, const u_char *Data, int Length)
{

  if (!(Source() && Transponder()))
     return;
  SI::SDT sdt(Data, false);
  if (!sdt.CheckCRCAndParse())
     return;

  if (!sectionSyncer.Sync(sdt.getVersionNumber(), sdt.getSectionNumber(), sdt.getLastSectionNumber()))
     return;
  if (!Channels.Lock(true, 10))
     return;

  SI::SDT::Service SiSdtService;
  for (SI::Loop::Iterator it; sdt.serviceLoop.getNext(SiSdtService, it); ) {
     cChannel *channel = Channels.GetByChannelID(tChannelID(Source(), sdt.getOriginalNetworkId(), sdt.getTransportStreamId(), SiSdtService.getServiceId()));
      if (!channel)
         channel = Channels.GetByChannelID(tChannelID(Source(), 0, Transponder(), SiSdtService.getServiceId()));
      cLinkChannels *LinkChannels = NULL;
      SI::Descriptor *d;
      for (SI::Loop::Iterator it2; (d = SiSdtService.serviceDescriptors.getNext(it2)); ) {
          switch (d->getDescriptorTag()) {
            case SI::ServiceDescriptorTag: {
                 SI::ServiceDescriptor *sd = (SI::ServiceDescriptor *)d;
                    char NameBufDeb[1024];
                    char ShortNameBufDeb[1024];
                 sd->serviceName.getText(NameBufDeb, ShortNameBufDeb, sizeof(NameBufDeb), sizeof(ShortNameBufDeb));
                 switch (sd->getServiceType()) {
                   case 0x01: // digital television service
                   case 0x02: // digital radio sound service
                   case 0x03: // DVB Subtitles
                   case 0x04: // NVOD reference service
                   case 0x05: // NVOD time-shifted service
                   case 0x19: // digital HD television service
                   case 0x11: // digital television service MPEG-2 HD 
                   case 0xC3: // some french channels like kiosk
                        {
                        // Add only radio 
                          if ((sd->getServiceType()==1 || sd->getServiceType()==0x11 || sd->getServiceType()==0x19 || sd->getServiceType()==0xC3) && AddServiceType == 1) 
                          {
#ifdef DEBUG_CHANNELSCAN
                             printf(" Add nur Radio  aber nur TV Sender gefunden  SID skip %d \n",sd->getServiceType());  
#endif
                             break;
                          }
                          // Add only tv
                          if (sd->getServiceType()==2 && (AddServiceType==0))
                          {
#ifdef DEBUG_CHANNELSCAN
                             printf(" Add nur TV  aber nur RadioSender gefunden  SID skip %d \n",sd->getServiceType());  
#endif
                             break;
                          }
                          char NameBuf[1024];
                          char ShortNameBuf[1024];
                          char ProviderNameBuf[1024];
                          sd->serviceName.getText(NameBuf, ShortNameBuf, sizeof(NameBuf), sizeof(ShortNameBuf));
                          char *pn = compactspace(NameBuf);
                          char *ps = compactspace(ShortNameBuf);
                          sd->providerName.getText(ProviderNameBuf, sizeof(ProviderNameBuf));
                          char *pp = compactspace(ProviderNameBuf);

                          mutexNames.Lock();
                          switch(sd->getServiceType())
                          {
                             case 0x01:
                             case 0x11:
                             case 0x19:
                             case 0xC3:
                                     tvChannelNames.push_back(NameBuf); // if service wanted 
                                     #ifdef DEBUG_CHANNELSCAN 
                                     tvChannelList.push_back(NameBuf);
                                     #endif
                                     break;
                             case 0x02:
                                     radioChannelNames.push_back(NameBuf); // if service wanted 
                                     #ifdef DEBUG_CHANNELSCAN 
                                     radioChannelList.push_back(NameBuf);
                                     #endif
                                     break;
                             default: ;
                          }
                          mutexNames.Unlock();
                    
                          if ( SiSdtService.getRunningStatus()>SI::RunningStatusNotRunning) {
                            usefulSid[numSid]=1;
                            numUsefulSid++;
                          }
                          else 
                            usefulSid[numSid]=0;

                          sid[numSid++]=SiSdtService.getServiceId();
                    
                          if (channel) {
                            channel->SetId(sdt.getOriginalNetworkId(), sdt.getTransportStreamId(), SiSdtService.getServiceId(),channel->Rid());
                            //if (Setup.UpdateChannels >= 1)
                            channel->SetName(pn, ps, pp);
                               // Using SiSdtService.getFreeCaMode() is no good, because some
                               // tv stations set this flag even for non-encrypted channels :-(
                               // The special value 0xFFFF was supposed to mean "unknown encryption"
                               // and would have been overwritten with real CA values later:
                               // channel->SetCa(SiSdtService.getFreeCaMode() ? 0xFFFF : 0);
                               }
                             else if (*pn) {
                                channel = Channels.NewChannel(Channel(), pn, ps, pp, sdt.getOriginalNetworkId(), sdt.getTransportStreamId(), SiSdtService.getServiceId());
                               patFilter->Trigger();
                               if (SiSdtService.getServiceId() == 0x12) {
                               //eitFilter->Trigger();
                               }


                               }
                          } // end case Digital TV services 
                      }
                   }
         break;
         case SI::NVODReferenceDescriptorTag: {
                 SI::NVODReferenceDescriptor *nrd = (SI::NVODReferenceDescriptor *)d;
                 SI::NVODReferenceDescriptor::Service Service;
                 for (SI::Loop::Iterator it; nrd->serviceLoop.getNext(Service, it); ) {
                     cChannel *link = Channels.GetByChannelID(tChannelID(Source(), Service.getOriginalNetworkId(), Service.getTransportStream(), Service.getServiceId()));
                     if (!link) {
#if 0
           if (SiSdtService.getRunningStatus()>SI::RunningStatusNotRunning) {
               usefulSid[numSid]=1;
          numUsefulSid++;
           }
           else
#endif
               usefulSid[numSid]=0;

           sid[numSid++]=SiSdtService.getServiceId();
           link = Channels.NewChannel(Channel(), "NVOD", "", "", Service.getOriginalNetworkId(), Service.getTransportStream(), Service.getServiceId());
           patFilter->Trigger();
         }
                     if (link) {
                        if (!LinkChannels)
                           LinkChannels = new cLinkChannels;
                        LinkChannels->Add(new cLinkChannel(link));
                        }
                     }
                 }
                 break;
            default: ;
            }
          delete d;
          }
      if (LinkChannels) {
         if (channel)
            channel->SetLinkChannels(LinkChannels);
         else
            delete LinkChannels;
         }
     }
  Channels.Unlock();
  if (sdt.getSectionNumber() == sdt.getLastSectionNumber()) {
     patFilter->SdtFinished();
     SetStatus(false);
     }
}


// --- NitFilter  ---------------------------------------------------------

#ifdef DBG 
# undef DBG 
#endif 
#define DBG  " DEBUG [channelscan]: "



NitFilter::NitFilter(void)
{
#ifdef DEBUG_CHANNELSCAN
  printf(DBG " -- %s --  \n",  __PRETTY_FUNCTION__);
#endif
  numNits = 0;
  networkId = 0;
  lastCount = 0;
  found_ = endofScan = false;
  Set(0x10, 0x40);  // NIT
  //Set(0x10, 0x41);  // other NIT
  vector<int> tmp(64,0);
  sectionSeen_ = tmp;
}

void NitFilter::SetStatus(bool On)
{
  cFilter::SetStatus(On);
  numNits = 0;
  networkId = 0;
  sectionSyncer.Reset();
  lastCount = 0;
  found_ = endofScan = false;
  vector<int> tmp(64,0);
  sectionSeen_ = tmp;
}

NitFilter::~NitFilter()
{
#ifdef DEBUG_CHANNELSCAN
  printf ( " debug [nit] ------------- %s --------------- ", __PRETTY_FUNCTION__);
#endif
}
bool NitFilter::Found()
{
   return found_;

}
void NitFilter::Process(u_short Pid, u_char Tid, const u_char *Data, int Length)
{
  SI::NIT nit(Data, false);
  if (!nit.CheckCRCAndParse())
     return;

  // TODO 
  // if versionsMap(nit.getVersionNumber()) == false)
  //  return;

#ifdef DEBUG_CHANNELSCAN
  printf("DEBUG [nit]: ++ %s NIT ID  %d\n", __PRETTY_FUNCTION__, nit.getNetworkId());
  printf("DEBUG [nit]: ++ SectionNumber  %d\n", nit.getSectionNumber());
  printf("DEBUG [nit]: ++ LastSectionNumber  %d\n", nit.getLastSectionNumber());
  printf("DEBUG [nit]: ++ moreThanOneSection %d\n", nit.moreThanOneSection());
  printf("DEBUG [nit]: ++ num Nits  %d\n", numNits);
#endif

  // 
  // return if we have seen the Table already 
  int cnt = ++TblVersions[nit.getVersionNumber()];  
  if (cnt > nit.getLastSectionNumber()+1) 
  {
#ifdef DEBUG_CHANNELSCAN
      printf("DEBUG [nit]: ++  NIT Version %d found %d times \n", cnt, nit.getVersionNumber());
#endif
      endofScan = true;
      return; 
  }
  // Some broadcasters send more than one NIT, with no apparent way of telling which
  // one is the right one to use. This is an attempt to find the NIT that contains
  // the transponder it was transmitted on and use only that one:
  //int transportstreamId = SI::NIT::TransportStream::getTransportStreamId();
  found_ = endofScan = false;
  bool insert = false;     
  //if (!networkId) 
  if (true) {
     int ThisNIT = -1;
     for (int i = 0; i < numNits; i++) {
#ifdef DEBUG_CHANNELSCAN
         printf("DEBUG [nit]: ++ num Nits  %d\n", numNits);
#endif
         if (nits[i].networkId == nit.getNetworkId()) {
            if (nit.getSectionNumber() == 0) {
               // all NITs have passed by
               for (int j = 0; j < numNits; j++) {
#ifdef DEBUG_CHANNELSCAN
                   printf("DEBUG [nit]: nits[%d] has Transponder ? %s \n",j ,nits[j].hasTransponder?"YES":"NO");
#endif
                   if (nits[j].hasTransponder) {
                      networkId = nits[j].networkId;
#ifdef DEBUG_CHANNELSCAN
                      printf("take  NIT with network ID %d\n", networkId);
#endif
                      //XXX what if more than one NIT contains this transponder???
                      break;
                      }
                   }
               if (!networkId) {
#ifdef DEBUG_CHANNELSCAN
                  printf("none of the NITs contains transponder %d\n", Transponder());
#endif
                  return;
                  }
               }
            else {
#ifdef DEBUG_CHANNELSCAN
               printf(" ----------- ThisNIT: %d --------------  \n", i);
#endif
               ThisNIT = i;
               break;
               }
            }
         }
     if (!networkId && ThisNIT < 0 && numNits < MAXNITS) {
        if (nit.getSectionNumber() == 0) {
           *nits[numNits].name = 0;
           SI::Descriptor *d;
#ifdef DEBUG_CHANNELSCAN
           printf(" INFO [nit] ----------- loop common descriptors:  --------------  \n");
#endif
           for (SI::Loop::Iterator it; (d = nit.commonDescriptors.getNext(it)); ) {
               switch (d->getDescriptorTag()) {
                 case SI::NetworkNameDescriptorTag: {
                      SI::NetworkNameDescriptor *nnd = (SI::NetworkNameDescriptor *)d;
                      nnd->name.getText(nits[numNits].name, MAXNETWORKNAME);
#ifdef DEBUG_CHANNELSCAN
                      printf(" INFO [nit] ----------- Get Name %s   --------------  \n", nits[numNits].name);
#endif
                      }
                      break;
                 default: ;
                 }
               delete d;
               }
           nits[numNits].networkId = nit.getNetworkId();
           nits[numNits].hasTransponder = false;
#ifdef DEBUG_CHANNELSCAN
           printf(" INFO [nit] ---- NIT[%d] ID: %5d Proivider '%s'\n", numNits, nits[numNits].networkId, nits[numNits].name);
#endif
           ThisNIT = numNits;
           numNits++;
           }
        }
     }
#ifdef DEBUG_CHANNELSCAN
  else if (networkId != nit.getNetworkId()) {
      printf("found !!!! OTHER  NIT !!!!!  %d previos NIT %d \n", nit.getNetworkId(), networkId);
     //return; // ignore all other NITs
  }
#endif
  else if (!sectionSyncer.Sync(nit.getVersionNumber(), nit.getSectionNumber(), nit.getLastSectionNumber()))
     return;

  sectionSeen_[nit.getSectionNumber()]++;

#ifdef DEBUG_CHANNELSCAN
  printf("DEBUG [nit]: SectionNumber %d   \n", nit.getSectionNumber());
#endif

  SI::NIT::TransportStream ts;
  for (SI::Loop::Iterator it; nit.transportStreamLoop.getNext(ts, it); ) {
      insert = false;

      SI::Loop::Iterator it2;
      SI::FrequencyListDescriptor *fld = (SI::FrequencyListDescriptor *)ts.transportStreamDescriptors.getNext(it2, SI::FrequencyListDescriptorTag);
#ifdef M750S
      int NumFrequencies = fld ? fld->frequencies.getCount() + 1 : 1;
#endif
      if (fld) {
#ifdef M750S
         int Frequencies[NumFrequencies];
#endif
         int ct = fld->getCodingType();
         if (ct > 0) {
#ifdef M750S
            int n = 1;
#endif
            for (SI::Loop::Iterator it3; fld->frequencies.hasNext(it3); ) {
                int f = fld->frequencies.getNext(it3);
                switch (ct) {
                  case 1: f = BCD2INT(f) / 100; break;
                  case 2: f = BCD2INT(f) / 10; break;
                  case 3: f = f * 10;  break;
                  }
#ifdef M750S
                Frequencies[n++] = f;
#endif
                }
            }
#ifdef M750S
         else
            NumFrequencies = 1;
#endif
         }
      delete fld;

#ifdef M750S
      int getTransponderNum = 0;
      SI::Descriptor *d;

      for (SI::Loop::Iterator it2; (d = ts.transportStreamDescriptors.getNext(it2)); ) {
          switch (d->getDescriptorTag()) {
            case SI::SatelliteDeliverySystemDescriptorTag: {
                 SI::SatelliteDeliverySystemDescriptor *sd = (SI::SatelliteDeliverySystemDescriptor *)d;
                 //int Source = cSource::FromData(cSource::stSat, BCD2INT(sd->getOrbitalPosition()), sd->getWestEastFlag());
                 int Frequency = Frequencies[0] = BCD2INT(sd->getFrequency()) / 100;
                 getTransponderNum++;
                 static char Polarizations[] = { 'h', 'v', 'l', 'r' };
                 char Polarization = Polarizations[sd->getPolarization()];
                 static int CodeRates[] = { FEC_NONE, FEC_1_2, FEC_2_3, FEC_3_4, FEC_5_6, FEC_7_8, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_NONE };
                 int CodeRate = CodeRates[sd->getFecInner()];
                 int SymbolRate = BCD2INT(sd->getSymbolRate()) / 10;
                    for (int n = 0; n < NumFrequencies; n++) {
                        cChannel *Channel = new cChannel;
                        Channel->SetId(ts.getOriginalNetworkId(), ts.getTransportStreamId(), 0, 0);
#ifdef DEBUG_CHANNELSCAN
                        printf ("# %d Mhz  TSID %5d  orig NIT %6d \n",
                                 Frequencies[n], ts.getTransportStreamId(), ts.getOriginalNetworkId() ); 
#endif

                        cSatTransponder *t = new cSatTransponder(Frequency, Polarization, SymbolRate, CodeRate);
                        if (!t)  {
                            esyslog("FatalError new cSatTransponder %d failed",Frequency);
                        }
                        else 
                        {
                           mapRet ret = transponderMap.insert(make_pair(Frequency, t));
#ifdef DEBUG_CHANNELSCAN
                           if (ret.second)
                              printf (" New transponder f: %d  p: %c sr: %d  \n", Frequency, Polarization, SymbolRate);
                           else
#else
                           if (!ret.second)
#endif
                             delete t;
                        }
                    }
                 }
                 break;
            default: ;
            }

          delete d;
          }
#endif // M750S
      }
  found_ = insert;

#ifdef DEBUG_CHANNELSCAN
  printf("DEBUG [nit]:  End of ProcessLoop MapSize: %d  lastCount %d   \n", (int) transponderMap.size(), lastCount);
  printf("DEBUG [nit]:  -- NIT ID  %d\n", nit.getNetworkId());
  printf("DEBUG [nit]:  -- Version %d \n", nit.getVersionNumber());
  printf("DEBUG [nit]:  -- moreThanOneSection %d\n", nit.moreThanOneSection());
  printf("DEBUG [nit]:  -- SectionNumber  %d\n", nit.getSectionNumber());
  printf("DEBUG [nit]:  -- LastSectionNumber  %d\n", nit.getLastSectionNumber());
  printf("DEBUG [nit]:  -- moreThanOneSection %d\n", nit.moreThanOneSection());
#endif

  if (!nit.moreThanOneSection())
  {
     endofScan = true;
  }
  else 
  {
     endofScan = true;
#ifdef DEBUG_CHANNELSCAN
     printf("DEBUG [nit]:  -- LastSectionNumber  %d\n", nit.getLastSectionNumber());
#endif
     //for (int i = 0; i<sectionSeen.size();i++) 
     for (int i = 0; i<nit.getLastSectionNumber()+1;i++) 
     {
#ifdef DEBUG_CHANNELSCAN
       printf("DEBUG [nit]:  -- Seen[%d] %s\n",i, sectionSeen_[i]?"YES":"NO");
#endif
       if (sectionSeen_[i] == 0)
       {
          endofScan = false;
          break;
       }
     }
     //if (sectionSeen.size()<=1)     endofScan = false;
   }

  if (endofScan == true)
  {
#ifdef DEBUG_CHANNELSCAN
     printf ("DEBUG [channescan ]: filter.c  End of ProcessLoop newMap size: %d  \n", (int)transponderMap.size());
#endif
     vector<int> tmp(64,0);
     sectionSeen_ = tmp;
  }
  found_ = insert = false;
  lastCount = transponderMap.size();
#ifdef DEBUG_CHANNELSCAN
  printf ("DEBUG [channescan ]: set endofScan %s \n",endofScan?"TRUE":"FALSE");
#endif
}




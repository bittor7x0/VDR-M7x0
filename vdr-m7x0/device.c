/*
 * device.c: The basic device interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "device.h"
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "audio.h"
#include "channels.h"
#include "i18n.h"
#include "player.h"
#include "receiver.h"
#include "status.h"
#include "transfer.h"
#ifdef USE_PINPLUGIN
#include "childlock.h"
#endif
#include "tshift.h"

bool scanning_on_receiving_device = false;

// --- cPesAssembler ---------------------------------------------------------
//M7X0 BEGIN AK
#define REPLAY_MAX_UNUSABLE_DATA KILOBYTE(512)

class cPesAssembler {
private:
  uchar fragmentData[6 + 65535];
  int fragmentLength;
  const uchar *packetData;
  int packetLength;
  int streamId;
  int skippedBytes;
  uint32_t scanner;
  bool initialSync;

  bool ScanForStartCode(const uchar *&Data, int Length);
public:
  cPesAssembler(void);
  void Reset(void);
  const uchar *Packet(int &Length) const;
  int Put(const uchar *Data, int Length);
  // This has not been implemented. What should it do?
  // bool IsPes(void);
  };

cPesAssembler::cPesAssembler(void)
{
  Reset();
}

void cPesAssembler::Reset(void)
{
  packetData = NULL;
  packetLength = 0;
  fragmentData[0] = fragmentData[1] = 0;
  fragmentData[1] = 1;
  fragmentLength = 0;
  streamId = 0;
  scanner = 0xFFFFFFFF;
  skippedBytes = 0;
  initialSync = true;
}

const uchar *cPesAssembler::Packet(int &Length) const
{
  if ((streamId != 0) & (packetLength != 0) &
        ((fragmentLength == 0) | (packetLength == fragmentLength))) {
     Length = packetLength;
     return (!fragmentLength ? packetData : fragmentData);
     }
  return NULL;
}

bool cPesAssembler::ScanForStartCode(const uchar *&Data,int Length)
{
  const uchar *data = Data;

  // Normal case - No need to sync
  // I think this hit in 99% of cases.
  // If this hit, there cannot be a part startcode in scanner
  // since data[0] is 0
  if (Length >= 4 && (!(data[0] | data[1] | (data[2] - 1)) & (data[3] >= 0xBA)
         & (data[3] <= 0xEF))) {
     scanner = 0xFFFFFFFF;
     packetLength = 0;
     streamId = data[3];
     fragmentLength = 0;
     return true;
     }

  for (int i = 0; (i < 3) & (i < Length); i++) {
      if (((scanner & 0xFFFFFF) == 0x000001) & (data[i] >= 0xBA)
            & (data[i] <= 0xEF)) {
         scanner = 0xFFFFFFFF;
         packetLength = 0;
         streamId = data[i];
         fragmentLength = 3 - i;
         skippedBytes -= 3 - i;
         return true;
         }
      scanner = (scanner << 8) | data[i];
      }

  if (Length <= 3) {
     skippedBytes += Length;
     return false;
     }

  const uchar *const limit = data + Length - 1;
  data += 2;

  while (data < limit)
        if (data[0] > 1)
           data += 3;
        else if (!data[0])
           data++;
        else {
           if (!(data[-2] | data[-1])) {
              const uchar code = *++data;
              if (code >= 0xBA && code <= 0xEF) {
                 scanner = 0xFFFFFFFF;
                 packetLength = 0;
                 streamId = code;
                 fragmentLength = 0;
                 skippedBytes += data - 3 - Data;
                 Data = data - 3;
                 return true;
                 }
              }
           data += 3;
           }

  scanner = BE2HOST(get_unaligned((uint32_t *)(limit - 3)));
  skippedBytes += limit + 1 - Data;
  return false;
}

int cPesAssembler::Put(const uchar *Data, int Length)
{
  const uchar *data = Data;

  if ((streamId != 0) & (packetLength != 0) &
        ((fragmentLength == 0) | (packetLength == fragmentLength))) {
     streamId = 0;
     }

  if (!streamId && !ScanForStartCode(data, Length)) {
     if (skippedBytes >= REPLAY_MAX_UNUSABLE_DATA) {
        esyslog("ERROR: %d bytes of recording unusable - giving up!", skippedBytes);
        errno = EDEADLK; // Any ideas for a better errorcode
        return -1;
        }
     return Length;
     }

  if (skippedBytes) {
     if (!initialSync) {
        esyslog("WARNING: %d bytes of recording unusable!", skippedBytes);
        }
     skippedBytes = 0;
     initialSync = false;
     }

  Length -= data - Data;
  if (!packetLength) {
     if (fragmentLength + Length < 6) {
        memcpy(fragmentData + fragmentLength, data, Length);
        fragmentLength += Length;
        return (data - Data + Length);
        }

     int b1 = fragmentLength < 5 ? data[4-fragmentLength] : fragmentData[4];

     if (streamId != 0xBA) {
        int b2 = fragmentLength < 6 ? data[5-fragmentLength] : fragmentData[5];
        packetLength = 6 + ((b1 << 8) | b2);
        }
     else {
        // Mpeg 1 / 2 Pack Headers have different length
        if (b1 & 0xC0) { // MPEG2
           if (fragmentLength + Length < 14) {
              memcpy(fragmentData + fragmentLength, data, Length);
              fragmentLength += Length;
              return (data - Data + Length);
              }
           int b2 = fragmentLength < 14 ? data[13-fragmentLength] : fragmentData[13];
           packetLength = 14 + (b2 & 0x7); // 14 + Stuffing length
           }
        else  // MPEG1
           packetLength = 12;
        }
     }

  if ((fragmentLength != 0) | (Length < packetLength)) {
     int bite = std::min(packetLength - fragmentLength, Length);
     memcpy(fragmentData + fragmentLength, data, bite);
     fragmentLength += bite;
     return (data - Data + bite);
     }
  packetData = data;
  return (data - Data + packetLength);
}
//M7X0 END AK

// --- cDevice ---------------------------------------------------------------

// The default priority for non-primary devices:
#define DEFAULTPRIORITY  -1

int cDevice::numDevices = 0;
int cDevice::useDevice = 0;
int cDevice::nextCardIndex = 0;
int cDevice::currentChannel = 1;
cDevice *cDevice::device[MAXDEVICES] = { NULL };
cDevice *cDevice::primaryDevice = NULL;

cDevice::cDevice(void)
:patPmtParser(true)
{
  cardIndex = nextCardIndex++;
  dsyslog("new device number %d", CardIndex() + 1);

  SetDescription("receiver on device %d", CardIndex() + 1);

  mute = false;
  volume = Setup.CurrentVolume;

  sectionHandler = NULL;
  eitFilter = NULL;
  patFilter = NULL;
  sdtFilter = NULL;
  nitFilter = NULL;

  ciHandler = NULL;
  player = NULL;
#ifdef TS_PLAYER_BACKPORT
  isPlayingVideo = false;
#endif
  pesAssembler = new cPesAssembler;
  ClrAvailableTracks();
  currentAudioTrack = ttNone;
  currentAudioTrackMissingCount = 0;

//M7X0 BEGIN AK (last receiver is a special one for transfering)
  for (int i = 0; i <= MAXRECEIVERS; i++)
      receiver[i] = NULL;
//M7X0 END AK

  if (numDevices < MAXDEVICES)
     device[numDevices++] = this;
  else
     esyslog("ERROR: too many devices!");

//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  firstFrameEventGotten = false;
#endif
  actionLock = 0;
  otherLock = 0;
//M7X0 END AK
}

cDevice::~cDevice()
{
  Detach(player);
  DetachAllReceivers();
  delete ciHandler;
  delete nitFilter;
  delete sdtFilter;
  delete patFilter;
  delete eitFilter;
  delete sectionHandler;
  delete pesAssembler;
}

bool cDevice::WaitForAllDevicesReady(int Timeout)
{
  for (time_t t0 = time(NULL); time(NULL) - t0 < Timeout; ) {
      bool ready = true;
      for (int i = 0; i < numDevices; i++) {
          if (device[i] && !device[i]->Ready()) {
             ready = false;
             cCondWait::SleepMs(100);
             }
          }
      if (ready)
         return true;
      }
  return false;
}

void cDevice::SetUseDevice(int n)
{
  if (n < MAXDEVICES)
     useDevice |= (1 << n);
}

int cDevice::NextCardIndex(int n)
{
  if (n > 0) {
     nextCardIndex += n;
     if (nextCardIndex >= MAXDEVICES)
        esyslog("ERROR: nextCardIndex too big (%d)", nextCardIndex);
     }
  else if (n < 0)
     esyslog("ERROR: invalid value in NextCardIndex(%d)", n);
  return nextCardIndex;
}

int cDevice::DeviceNumber(void) const
{
  for (int i = 0; i < numDevices; i++) {
      if (device[i] == this)
         return i;
      }
  return -1;
}

void cDevice::MakePrimaryDevice(bool On)
{
}

bool cDevice::SetPrimaryDevice(int n)
{
  n--;
  if (0 <= n && n < numDevices && device[n]) {
     isyslog("setting primary device to %d", n + 1);
     if (primaryDevice)
        primaryDevice->MakePrimaryDevice(false);
     primaryDevice = device[n];
     primaryDevice->MakePrimaryDevice(true);
     if(Setup.VideoWithoutSignal){
        primaryDevice->SetTvMode(Setup.TvMode);
        primaryDevice->SetVideoFormat(eVideoFormat(Setup.VideoFormat));
        primaryDevice->SetVolumeDevice(Setup.CurrentVolume);
        }
     Setup.PrimaryDVB = n + 1;
     return true;
     }
  esyslog("ERROR: invalid primary device number: %d", n + 1);
  return false;
}

bool cDevice::HasDecoder(void) const
{
  return false;
}

void cDevice::CheckStreamAspect(bool)
{
    dsyslog("cDevice::CheckStreamAspect(bool): This should be never called");
}

void cDevice::SetTvSettings(bool)
{
    dsyslog("cDevice::SetTvSettings(bool): This should be never called");
}

void cDevice::SetTvMode(bool)
{
    dsyslog("cDevice::SetTvMode(bool): This should be never called");
}

void cDevice::SetVCRMode(bool)
{
    dsyslog("cDevice::SetVCRMode(bool): This should be never called");
}

cSpuDecoder *cDevice::GetSpuDecoder(void)
{
  return NULL;
}

cDevice *cDevice::ActualDevice(void)
{
  cDevice *d = cTransferControl::ReceiverDevice();
  if (!d)
     d = PrimaryDevice();
  return d;
}

cDevice *cDevice::GetDevice(int Index)
{
  return (0 <= Index && Index < numDevices) ? device[Index] : NULL;
}
//M7X0 BEGIN AK
cDevice *cDevice::GetDevice(const cChannel *Channel, int Priority, bool *NeedsDetachReceivers, bool forTransferer)
{
  cDevice *d = NULL;
  uint Impact = 0xFFFFFFFF; // we're looking for a device with the least impact
  for (int i = 0; i < numDevices; i++) {
      bool ndr;
      if (device[i]->ProvidesChannel(Channel, Priority, &ndr, forTransferer)) { // this device is basicly able to do the job
//M7X0 END AK
         // Put together an integer number that reflects the "impact" using
         // this device would have on the overall system. Each condition is represented
         // by one bit in the number (or several bits, if the condition is actually
         // a numeric value). The sequence in which the conditions are listed corresponds
         // to their individual severity, where the one listed first will make the most
         // difference, because it results in the most significant bit of the result.
         uint imp = 0;
         imp <<= 1; imp |= !device[i]->Receiving() || ndr;                         // use receiving devices if we don't need to detach existing receivers
         imp <<= 1; imp |= device[i]->Receiving();                                 // avoid devices that are receiving
         imp <<= 8; imp |= constrain(device[i]->Priority() + MAXPRIORITY, 0, 0xFF);// use the device with the lowest priority (+MAXPRIORITY to assure that values -99..99 can be used)
         imp <<= 1; imp |= device[i] == cTransferControl::ReceiverDevice();        // avoid the Transfer Mode receiver device
         imp <<= 8; imp |= constrain(device[i]->ProvidesCa(Channel), 0, 0xFF);     // use the device that provides the lowest number of conditional access methods
         imp <<= 1; imp |= ndr;                                                    // avoid detach receivers
         imp <<= 1; imp |= cTShiftControl::Impact(device[i],ndr);                  // avoid last TShift device
         imp <<= 1; imp |= device[i]->IsPrimaryDevice();                           // avoid the primary device
         imp <<= 1; imp |= device[i]->HasDecoder();                                // avoid full featured cards
         if (imp < Impact) {
            // This device has less impact than any previous one, so we take it.
            Impact = imp;
            d = device[i];
            if (NeedsDetachReceivers)
               *NeedsDetachReceivers = ndr;
            }
         }
      }
  return d;
}

void cDevice::Shutdown(void)
{
  primaryDevice = NULL;
  for (int i = 0; i < numDevices; i++) {
      delete device[i];
      device[i] = NULL;
      }
}

#ifdef WITH_LIBJPEG
uchar *cDevice::GrabImage(int &Size, bool Jpeg, int Quality, int SizeX, int SizeY)
{
  return NULL;
}

bool cDevice::GrabImageFile(const char *FileName, bool Jpeg, int Quality, int SizeX, int SizeY)
{
  int result = 0;
  int fd = open(FileName, O_WRONLY | O_CREAT | O_NOFOLLOW | O_TRUNC, DEFFILEMODE);
  if (fd >= 0) {
     int ImageSize;
     uchar *Image = GrabImage(ImageSize, Jpeg, Quality, SizeX, SizeY);
     if (Image) {
        if (safe_write(fd, Image, ImageSize) == ImageSize)
           isyslog("grabbed image to %s", FileName);
        else {
           LOG_ERROR_STR(FileName);
           result |= 1;
           }
        free(Image);
        }
     else
        result |= 1;
     close(fd);
     }
  else {
     LOG_ERROR_STR(FileName);
     result |= 1;
     }
  return result == 0;
}
#endif

void cDevice::SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat)
{
  cSpuDecoder *spuDecoder = GetSpuDecoder();
  if (spuDecoder) {
     if (Setup.VideoFormat)
        spuDecoder->setScaleMode(cSpuDecoder::eSpuNormal);
     else {
        switch (VideoDisplayFormat) {
               case vdfPanAndScan:
                    spuDecoder->setScaleMode(cSpuDecoder::eSpuPanAndScan);
                    break;
               case vdfLetterBox:
                    spuDecoder->setScaleMode(cSpuDecoder::eSpuLetterBox);
                    break;
               default: esyslog("ERROR: invalid value for VideoDisplayFormat '%d'", VideoDisplayFormat);
               }
        }
     }
}

void cDevice::SetVideoFormat(eVideoFormat VideoFormat)
{
    dsyslog("cDevice::SetVideoFormat(eVideoFormat VideoFormat): This should be never called");
}

eVideoSystem cDevice::GetVideoSystem(void)
{
  return vsPAL;
}

//#define PRINTPIDS(s) { char b[500]; char *q = b; q += sprintf(q, "%d %s ", CardIndex(), s); for (int i = 0; i < MAXPIDHANDLES; i++) q += sprintf(q, " %s%4d %d", i == ptOther ? "* " : "", pidHandles[i].pid, pidHandles[i].used); dsyslog("%s", b); }
#define PRINTPIDS(s)

bool cDevice::HasPid(int Pid) const
{
  for (int i = 0; i < MAXPIDHANDLES; i++) {
      if (pidHandles[i].pid == Pid)
         return true;
      }
  return false;
}
//M7x0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
int cDevice::ReceiverWantsFrameEvents(int Pid) {
  for (int i = 0; i < MAXRECEIVERS; i++)
      if (receiver[i] && receiver[i]->WantsFrameEvents() && (!Pid || receiver[i]->pids[0] == Pid))
         return i;
  return -1;
}
#endif
// PID handling changed, cause using "tap" mode is not intentional
// on the small m7x0 hardware.
// Live-PID handles should be used only for live viewing.
// If recording on the same Channel we need to open a new filter
bool cDevice::AddPid(int Pid, ePidType PidType)
{
  if (! (Pid || PidType == ptPcr) )
     return true;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (PidType < ptRecVideo) {
#else
  if (PidType < ptOther) {
#endif
     if (pidHandles[PidType].used) {
        esyslog("ERROR: Special PID slot %d on device %d already in use!", PidType, CardIndex() + 1);
        return false;
        }
     pidHandles[PidType].used=1;
     pidHandles[PidType].pid=Pid;

     if (!SetPid(&pidHandles[PidType],PidType,true)) {
        esyslog("ERROR: can't set PID %d on device %d", Pid, CardIndex() + 1);
        DelPid(Pid,PidType);
        return false;
        }

     if (ciHandler)
        ciHandler->SetPid(Pid, true);
     return true;
     }

  int n = -1;
  int a = -1;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  for (int i = ptRecVideo ; i < MAXPIDHANDLES && n == -1 ; i++)
#else
  for (int i = ptOther ; i < MAXPIDHANDLES && n == -1 ; i++)
#endif
      if (pidHandles[i].pid == Pid)
         n = i;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
      else if (a == -1 && i >= PidType && !pidHandles[i].used)
#else
      else if (a == -1 && !pidHandles[i].used)
#endif
         a = i;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (n != -1 && a != ptRecVideo) {
#else
  if (n != -1) {
#endif
     pidHandles[n].used++;
     return true;
     }

  if (a == -1) {
     esyslog("ERROR: no free slot for PID %d on device %d", Pid, CardIndex() + 1);
     return false;
     }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  int used = 1;
  if (a == ptRecVideo && n != -1) {
     used = pidHandles[n].used  + 1;
     pidHandles[n].used = 1;
     DelPid(Pid);
     }
#endif

  pidHandles[a].used=1;
  pidHandles[a].pid=Pid;

  if (!SetPid(&pidHandles[a],a,true)) {
     esyslog("ERROR: can't set PID %d on device %d", Pid, CardIndex() + 1);
     DelPid(Pid);
#ifdef USE_HW_VIDEO_FRAME_EVENTS
     if (used > 1) {
        esyslog("ERROR: Cannot set pid %d while shifting!", Pid);
        DetachAll(Pid);
        }
#endif
     return false;
     }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  pidHandles[a].used=used;
#endif
  if (ciHandler)
     ciHandler->SetPid(Pid, true);
  return true;

#if 0 // Original handling
  if (Pid || PidType == ptPcr) {
     int n = -1;
     int a = -1;
     if (PidType != ptPcr) { // PPID always has to be explicit
        for (int i = 0; i < MAXPIDHANDLES; i++) {
            if (i != ptPcr) {
               if (pidHandles[i].pid == Pid)
                  n = i;
               else if (a < 0 && i >= ptOther && !pidHandles[i].used)
                  a = i;
               }
            }
        }
     if (n >= 0) {
        // The Pid is already in use
        if (++pidHandles[n].used == 2 && n <= ptTeletext) {
           // It's a special PID that may have to be switched into "tap" mode
           PRINTPIDS("A");
           if (!SetPid(&pidHandles[n], n, true)) {
              esyslog("ERROR: can't set PID %d on device %d", Pid, CardIndex() + 1);
              if (PidType <= ptTeletext)
                 DetachAll(Pid);
              DelPid(Pid, PidType);
              return false;
              }
           if (ciHandler)
              ciHandler->SetPid(Pid, true);
           }
        PRINTPIDS("a");
        return true;
        }
     else if (PidType < ptOther) {
        // The Pid is not yet in use and it is a special one
        n = PidType;
        }
     else if (a >= 0) {
        // The Pid is not yet in use and we have a free slot
        n = a;
        }
     else {
        esyslog("ERROR: no free slot for PID %d on device %d", Pid, CardIndex() + 1);
        return false;
        }
     if (n >= 0) {
        pidHandles[n].pid = Pid;
        pidHandles[n].used = 1;
        PRINTPIDS("C");
        if (!SetPid(&pidHandles[n], n, true)) {
           esyslog("ERROR: can't set PID %d on device %d", Pid, CardIndex() + 1);
           if (PidType <= ptTeletext)
              DetachAll(Pid);
           DelPid(Pid, PidType);
           return false;
           }
        if (ciHandler)
           ciHandler->SetPid(Pid, true);
        }
     }
  return true;
#endif
}

void cDevice::DelPid(int Pid, ePidType PidType)
{
  if (!(Pid || PidType== ptPcr))
     return;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (PidType < ptRecVideo) {
#else
  if (PidType < ptOther) {
#endif
     if (!pidHandles[PidType].used)
        return;

     if (pidHandles[PidType].used > 1)
        esyslog("ERROR: Special PID slot %d on device %d more than once used!", PidType, CardIndex() + 1);

     pidHandles[PidType].used = 0;
     SetPid(&pidHandles[PidType], PidType, false);
     pidHandles[PidType].handle = -1;
     pidHandles[PidType].pid = 0;

     if (ciHandler)
        ciHandler->SetPid(Pid, false);

     return;
     }

  int n = -1;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  for (int i = ptRecVideo; i < MAXPIDHANDLES; i++)
#else
  for (int i = ptOther; i < MAXPIDHANDLES; i++)
#endif
      if (pidHandles[i].pid == Pid) {
         n = i;
         break;
         }


  if (n == -1 || !pidHandles[n].used)
     return;

  pidHandles[n].used--;

  if (!pidHandles[n].used) {

     SetPid(&pidHandles[n], n, false);

     pidHandles[n].handle = -1;
     pidHandles[n].pid = 0;

     if (ciHandler)
        ciHandler->SetPid(Pid, false);
     }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (n != ptRecVideo)
     return;

  int used = pidHandles[ptRecVideo].used;

  if (used) {
     int pidWantedBy = ReceiverWantsFrameEvents(Pid);

     if (pidWantedBy != -1)
        return;

     pidHandles[ptRecVideo].used = 0;
     SetPid(&pidHandles[ptRecVideo], n, false);

     pidHandles[n].handle = -1;
     pidHandles[n].pid = 0;

     if (ciHandler)
        ciHandler->SetPid(Pid, false);

     if (!AddPid(Pid)) {
        esyslog("ERROR: Cannot set pid %d while shifting!",Pid);
        DetachAll(Pid);
        return;
        }

     for (int i = ptOther; i < MAXPIDHANDLES; i++)
         if (pidHandles[i].pid == Pid) {
            pidHandles[i].used = used;
            break;
            }
     }

  firstFrameEventGotten = false;
  int frameEventsWantedBy = ReceiverWantsFrameEvents();

  if (frameEventsWantedBy == -1)
     return;

  Pid = receiver[frameEventsWantedBy]->pids[0];
  DelPid(Pid);

  if (!AddPid(Pid,ptRecVideo)) {
     esyslog("ERROR: Cannot set pid %d while shifting!",Pid);
     Detach(receiver[frameEventsWantedBy]);
     }
#endif
#if 0 // Original Pid-Handling
  if (Pid || PidType == ptPcr) {
     int n = -1;
     if (PidType == ptPcr)
        n = PidType; // PPID always has to be explicit
     else {
        for (int i = 0; i < MAXPIDHANDLES; i++) {
            if (pidHandles[i].pid == Pid) {
               n = i;
               break;
               }
            }
        }
     if (n >= 0 && pidHandles[n].used) {
        PRINTPIDS("D");
        if (--pidHandles[n].used < 2) {
           SetPid(&pidHandles[n], n, false);
           if (pidHandles[n].used == 0) {
              pidHandles[n].handle = -1;
              pidHandles[n].pid = 0;
              if (ciHandler)
                 ciHandler->SetPid(Pid, false);
              }
           }
        PRINTPIDS("E");
        }
     }
#endif
}
//M7x0 END AK
bool cDevice::SetPid(cPidHandle *Handle, int Type, bool On)
{
  return false;
}

void cDevice::StartSectionHandler(void)
{
  if (!sectionHandler) {
     sectionHandler = new cSectionHandler(this);
     AttachFilter(eitFilter = new cEitFilter);
     AttachFilter(patFilter = new cPatFilter);
     AttachFilter(sdtFilter = new cSdtFilter(patFilter));
     AttachFilter(nitFilter = new cNitFilter);
     }
}

void cDevice::StopSectionHandler(void)
{
  if (sectionHandler) {
     delete nitFilter;
     delete sdtFilter;
     delete patFilter;
     delete eitFilter;
     delete sectionHandler;
     nitFilter = NULL;
     sdtFilter = NULL;
     patFilter = NULL;
     eitFilter = NULL;
     sectionHandler = NULL;
     }
}

int cDevice::OpenFilter(u_short Pid, u_char Tid, u_char Mask)
{
  return -1;
}

void cDevice::AttachFilter(cFilter *Filter)
{
  if (sectionHandler)
     sectionHandler->Attach(Filter);
}

void cDevice::Detach(cFilter *Filter)
{
  if (sectionHandler)
     sectionHandler->Detach(Filter);
}

bool cDevice::ProvidesSource(int Source) const
{
  return false;
}

bool cDevice::ProvidesTransponder(const cChannel *Channel) const
{
  return false;
}

bool cDevice::ProvidesTransponderExclusively(const cChannel *Channel) const
{
  for (int i = 0; i < numDevices; i++) {
      if (device[i] && device[i] != this && device[i]->ProvidesTransponder(Channel))
         return false;
      }
  return true;
}
//M7X0 BEGIN AK
bool cDevice::ProvidesChannel(const cChannel *Channel, int Priority, bool *NeedsDetachReceivers, bool forTransferer) const
{
  return false;
}
//M7X0 END AK
bool cDevice::IsTunedToTransponder(const cChannel *Channel)
{
  return false;
}

bool cDevice::MaySwitchTransponder(void)
{
  return !Receiving(true) && !(pidHandles[ptAudio].pid || pidHandles[ptVideo].pid || pidHandles[ptDolby].pid);
}

bool cDevice::SwitchChannel(const cChannel *Channel, bool LiveView)
{
  if (LiveView) {
     isyslog("switching to channel %d", Channel->Number());
     if (getIaMode())
        cControl::Shutdown(); // prevents old channel from being shown too long if GetDevice() takes longer
     }
  for (int i = 3; i--;) {
      switch (SetChannel(Channel, LiveView)) {
        case scrOk:           return true;
        case scrNotAvailable: Skins.QueueMessage(mtInfo, tr("Channel not available!"));
                              return false;
        case scrNoTransfer:   Skins.QueueMessage(mtError, tr("Can't start Transfer Mode!"));
                              return false;
        case scrFailed:       break; // loop will retry
        default:              esyslog("ERROR: invalid return value from SetChannel");
        }
      esyslog("retrying");
      }
  return false;
}

//M7X0 BEGIN AK
bool cDevice::SwitchChannel(int Direction)
{
  bool result = false;
  Direction = sgn(Direction);
  if (Direction) {
     if (getIaMode())
        cControl::Shutdown(); // prevents old channel from being shown too long if GetDevice() takes longer
     int n = CurrentChannel() + Direction;
     int first = n;
     cChannel *channel;
     while ((channel = Channels.GetByNumber(n, Direction)) != NULL) {
           // try only channels which are currently available
#ifdef USE_PINPLUGIN
           if (!PinPatch::ChildLock::IsChannelProtected(channel))
#endif
           if (channel->Filtered() && (PrimaryDevice()->ProvidesChannel(channel, Setup.PrimaryLimit) || /* PrimaryDevice()->CanReplay() && */ GetDevice(channel, 0)))
              break;
           n = channel->Number() + Direction;
           }
     if (channel) {
        int d = n - first;
        if (abs(d) == 1)
           dsyslog("skipped channel %d", first);
        else if (d)
           dsyslog("skipped channels %d..%d", first, n - sgn(d));
        if (PrimaryDevice()->SwitchChannel(channel, true))
           result = true;
        }
     else if (n != first)
        Skins.QueueMessage(mtError, tr("Channel not available!"));
     }
  return result;
}

eSetChannelResult cDevice::SetChannel(const cChannel *Channel, bool LiveView)
{
#ifdef USE_PINPLUGIN
  if (LiveView && PinPatch::ChildLock::IsChannelProtected(Channel))
     return scrNotAvailable;
#endif

  if (LiveView)
     StopReplay();

  // If this card is switched to an other transponder, any receivers still
  // attached to it need to be automatically detached:
  bool NeedsDetachReceivers = false;
  bool provides=ProvidesChannel(Channel, Setup.PrimaryLimit, &NeedsDetachReceivers, true);

  // If this card can't receive this channel, we must not actually switch
  // the channel here, because that would irritate the driver when we
  // start replaying in Transfer Mode immediately after switching the channel:
//M7X0 BEGIN AK
  bool NeedsTransferMode = (LiveView && IsPrimaryDevice() && (Setup.TShift || !provides));
//M7X0 END AK
  eSetChannelResult Result = scrOk;

  // If this DVB card can't receive this channel, let's see if we can
  // use the card that actually can receive it and transfer data from there:

  if (NeedsTransferMode) {
//M7X0 BEGIN AK
     cDevice *CaDevice = GetDevice(Channel, 0, &NeedsDetachReceivers, true);
//M7X0 END AK
     // Lets make CaDevice Primary and handle the Channel
#if 0
	  if (CaDevice){
			SetPrimaryDevice(CaDevice->cardIndex+1);
			return CaDevice->SetChannel(Channel,LiveView);
	  }
#endif
     if (CaDevice && CanReplay()) {
        cStatus::MsgChannelSwitch(this, 0); // only report status if we are actually going to switch the channel
        if (CaDevice->SetChannel(Channel, false) == scrOk) { // calling SetChannel() directly, not SwitchChannel()!
           if (NeedsDetachReceivers)
              CaDevice->DetachAllReceivers();
	   if(Setup.TShift)
           	cControl::Launch(new cTShiftControl(CaDevice,Channel));
	   else
           	cControl::Launch(new cTransferControl(CaDevice,Channel->Ppid(), Channel->Vpid(), Channel->Apid(0),Channel->Tpid()));
           }
        else
           Result = scrNoTransfer;
        }
     else
        Result = scrNotAvailable;
     }
  else {
     Channels.Lock(false);
     cStatus::MsgChannelSwitch(this, 0); // only report status if we are actually going to switch the channel
     // Stop section handling:
     if (sectionHandler) {
        sectionHandler->SetStatus(false);
        sectionHandler->SetChannel(NULL);
        }
     // Tell the ciHandler about the channel switch and add all PIDs of this
     // channel to it, for possible later decryption:
     if (ciHandler) {
        ciHandler->SetSource(Channel->Source(), Channel->Transponder());
// Men at work - please stand clear! ;-)
#ifdef XXX_DO_MULTIPLE_CA_CHANNELS
        if (Channel->Ca() >= CA_ENCRYPTED_MIN) {
#endif
           ciHandler->AddPid(Channel->Sid(), Channel->Vpid(), 2);
           for (const int *Apid = Channel->Apids(); *Apid; Apid++)
               ciHandler->AddPid(Channel->Sid(), *Apid, 4);
           for (const int *Dpid = Channel->Dpids(); *Dpid; Dpid++)
               ciHandler->AddPid(Channel->Sid(), *Dpid, 0);
#ifdef XXX_DO_MULTIPLE_CA_CHANNELS
           bool CanDecrypt = ciHandler->CanDecrypt(Channel->Sid());//XXX
           dsyslog("CanDecrypt %d %d %d %s", CardIndex() + 1, CanDecrypt, Channel->Number(), Channel->Name());//XXX
           }
#endif
        }
     if (NeedsDetachReceivers)
        DetachAllReceivers();
     if (SetChannelDevice(Channel, LiveView)) {
        // Start section handling:
        if (sectionHandler) {
           sectionHandler->SetChannel(Channel);
           sectionHandler->SetStatus(true);
           }
        // Start decrypting any PIDs that might have been set in SetChannelDevice():
        if (ciHandler)
           ciHandler->StartDecrypting();
        }
     else
        Result = scrFailed;
     Channels.Unlock();
     }

  if (Result == scrOk) {
     if (LiveView && IsPrimaryDevice()) {
        currentChannel = Channel->Number();
        // Set the available audio tracks:
        ClrAvailableTracks();
        for (int i = 0; i < MAXAPIDS; i++)
            SetAvailableTrack(ttAudio, i, Channel->Apid(i), Channel->Alang(i));
        if (Setup.UseDolbyDigital) {
           for (int i = 0; i < MAXDPIDS; i++)
               SetAvailableTrack(ttDolby, i, Channel->Dpid(i), Channel->Dlang(i));
           }
        if (!NeedsTransferMode)
           EnsureAudioTrack(true);
        }
     cStatus::MsgChannelSwitch(this, Channel->Number()); // only report status if channel switch successfull
     }

  return Result;
}
//M7X0 END AK
void cDevice::ForceTransferMode(void)
{
  if (!cTransferControl::ReceiverDevice()) {
     cChannel *Channel = Channels.GetByNumber(CurrentChannel());
     if (Channel)
        SetChannelDevice(Channel, false); // this implicitly starts Transfer Mode
     }
}

bool cDevice::SetChannelDevice(const cChannel *Channel, bool LiveView)
{
  return false;
}

bool cDevice::HasLock(int TimeoutMs) const
{
  return true;
}

bool cDevice::HasProgramme(void) const
{
//M7X0 BEGIN AK
  return Replaying() || ((pidHandles[ptAudio].pid || pidHandles[ptVideo].pid) && IsPrimaryDevice());
//M7X0 END AK
}

int cDevice::GetAudioChannelDevice(void)
{
  return 0;
}

void cDevice::SetAudioChannelDevice(int AudioChannel)
{
}

void cDevice::SetVolumeDevice(int Volume)
{
}

void cDevice::SetDigitalAudioDevice(bool On)
{
}
//M7X0 BEGIN AK
void cDevice::SetAudioTrackDevice(eTrackType Type, const tTrackId *TrackId )
{
}
//M7X0 END AK
bool cDevice::ToggleMute(void)
{
  int OldVolume = volume;
  mute = !mute;
  //XXX why is it necessary to use different sequences???
  if (mute) {
     SetVolumeDevice(0);
     cStatus::MsgSetVolume(0, true);
     Audios.MuteAudio(mute); // Mute external audio after analog audio
     }
  else {
     Audios.MuteAudio(mute); // Enable external audio before analog audio
     SetVolume(OldVolume, true);
     }
  volume = OldVolume;
  return mute;
}

int cDevice::GetAudioChannel(void)
{
  int c = GetAudioChannelDevice();
  return (0 <= c && c <= 2) ? c : 0;
}

void cDevice::SetAudioChannel(int AudioChannel)
{
  if (0 <= AudioChannel && AudioChannel <= 2)
     SetAudioChannelDevice(AudioChannel);
}

void cDevice::SetVolume(int Volume, bool Absolute)
{
  int OldVolume = volume;
  volume = constrain(Absolute ? Volume : volume + Volume, 0, MAXVOLUME);
  Absolute |= mute;
  cStatus::MsgSetVolume(Absolute ? volume : volume - OldVolume, Absolute);
  if(!(mute && volume<OldVolume)) { // effectively change volume
    SetVolumeDevice(volume);
    if (mute) {
       mute = false;
       Audios.MuteAudio(mute);
       }
    }  
}

void cDevice::ClrAvailableTracks(bool DescriptionsOnly, bool IdsOnly)
{
  if (DescriptionsOnly) {
     for (int i = ttNone; i < ttMaxTrackTypes; i++)
         *availableTracks[i].description = 0;
     }
  else {
     if (IdsOnly) {
        for (int i = ttNone; i < ttMaxTrackTypes; i++)
            availableTracks[i].id = 0;
        }
     else
        memset(availableTracks, 0, sizeof(availableTracks));
     pre_1_3_19_PrivateStream = false;
     SetAudioChannel(0); // fall back to stereo
     currentAudioTrackMissingCount = 0;
     currentAudioTrack = ttNone;
     }
}

bool cDevice::SetAvailableTrack(eTrackType Type, int Index, uint16_t Id, const char *Language, const char *Description)
{
  eTrackType t = eTrackType(Type + Index);
  if ((Type == ttAudio && IS_AUDIO_TRACK(t)) ||
      (Type == ttDolby && IS_DOLBY_TRACK(t))) {
     if (Language)
        strn0cpy(availableTracks[t].language, Language, sizeof(availableTracks[t].language));
     if (Description)
        strn0cpy(availableTracks[t].description, Description, sizeof(availableTracks[t].description));
     if (Id) {
        availableTracks[t].id = Id; // setting 'id' last to avoid the need for extensive locking
        int numAudioTracks = NumAudioTracks();
        if (!availableTracks[currentAudioTrack].id && numAudioTracks && currentAudioTrackMissingCount++ > numAudioTracks * 10)
           EnsureAudioTrack();
        else if (t == currentAudioTrack)
           currentAudioTrackMissingCount = 0;
        }
     return true;
     }
  else
     esyslog("ERROR: SetAvailableTrack called with invalid Type/Index (%d/%d)", Type, Index);
  return false;
}

const tTrackId *cDevice::GetTrack(eTrackType Type)
{
//M7X0 BEGIN AK
// To avoid extensive lockings while replaying (locking takes very much time on m7x0, caused by hw not supporting atomic operations)
  if (Type == currentAudioTrack) {
     cMutexLock MutexLock(&mutexCurrentAudioTrack);
     return (ttNone < Type && Type < ttMaxTrackTypes) ? &availableTracks[Type] : NULL;
     }
//M7X0 END AK
  return (ttNone < Type && Type < ttMaxTrackTypes) ? &availableTracks[Type] : NULL;
}

int cDevice::NumAudioTracks(void) const
{
  int n = 0;
  for (int i = ttAudioFirst; i <= ttDolbyLast; i++) {
      if (availableTracks[i].id)
         n++;
      }
  return n;
}

bool cDevice::SetCurrentAudioTrack(eTrackType Type)
{
  if (ttNone < Type && Type <= ttDolbyLast) {
     cMutexLock MutexLock(&mutexCurrentAudioTrack);
     if (IS_DOLBY_TRACK(Type))
        SetDigitalAudioDevice(true);
     currentAudioTrack = Type;
     if (player)
        player->SetAudioTrack(currentAudioTrack, GetTrack(currentAudioTrack));
     else
        SetAudioTrackDevice(currentAudioTrack);
     if (IS_AUDIO_TRACK(Type))
        SetDigitalAudioDevice(false);
     return true;
     }
  return false;
}

void cDevice::EnsureAudioTrack(bool Force)
{
  if (Force || !availableTracks[currentAudioTrack].id) {
     eTrackType PreferredTrack = ttAudioFirst;
     int PreferredAudioChannel = 0;
     int LanguagePreference = -1;
     int StartCheck = Setup.CurrentDolby ? ttDolbyFirst : ttAudioFirst;
     int EndCheck = ttDolbyLast;
     for (int i = StartCheck; i <= EndCheck; i++) {
         const tTrackId *TrackId = GetTrack(eTrackType(i));
         int pos = 0;
         if (TrackId && TrackId->id && I18nIsPreferredLanguage(Setup.AudioLanguages, TrackId->language, LanguagePreference, &pos)) {
            PreferredTrack = eTrackType(i);
            PreferredAudioChannel = pos;
            }
         if (Setup.CurrentDolby && i == ttDolbyLast) {
            i = ttAudioFirst - 1;
            EndCheck = ttAudioLast;
            }
         }
     // Make sure we're set to an available audio track:
     const tTrackId *Track = GetTrack(GetCurrentAudioTrack());
     if (Force || !Track || !Track->id || PreferredTrack != GetCurrentAudioTrack()) {
        if (!Force) // only log this for automatic changes
           dsyslog("setting audio track to %d (%d)", PreferredTrack, PreferredAudioChannel);
        SetCurrentAudioTrack(PreferredTrack);
        SetAudioChannel(PreferredAudioChannel);
        }
     }
}

bool cDevice::CanReplay(void) const
{
  return HasDecoder();
}

bool cDevice::SetPlayMode(ePlayMode PlayMode)
{
  return false;
}

int64_t cDevice::GetSTC(void)
{
  return -1;
}

//M7X0 BEGIN AK
void cDevice::TrickSpeed(int Speed, bool UseFastForward)
{
}
//M7X0 END AK

void cDevice::Clear(void)
{
  Audios.ClearAudio();
}

void cDevice::Play(void)
{
  Audios.MuteAudio(mute);
}

void cDevice::Freeze(void)
{
  Audios.MuteAudio(true);
}

void cDevice::Mute(void)
{
  Audios.MuteAudio(true);
}

void cDevice::StillPicture(const uchar *Data, int Length)
{
  if (Data[0] == 0x47) {
     // TS data
     cTsToPes TsToPes;
     uchar *buf = NULL;
     int Size = 0;
     while (Length >= TS_SIZE) {
           int Pid = TsPid(Data);
           if (Pid == PATPID)
              patPmtParser.ParsePat(Data, TS_SIZE);
           else if (Pid == patPmtParser.PmtPid())
              patPmtParser.ParsePmt(Data, TS_SIZE);
           else if (Pid == patPmtParser.Vpid()) {
              if (TsPayloadStart(Data)) {
                 int l;
                 while (const uchar *p = TsToPes.GetPes(l)) {
                       int Offset = Size;
                       int NewSize = Size + l;
                       if (uchar *NewBuffer = (uchar *)realloc(buf, NewSize)) {
                          Size = NewSize;
                          buf = NewBuffer;
                          memcpy(buf + Offset, p, l);
                          }
                       else {
                          LOG_ERROR_STR("out of memory");
                          free(buf);
                          return;
                          }
                       }
                 TsToPes.Reset();
                 }
              TsToPes.PutTs(Data, TS_SIZE);
              }
           Length -= TS_SIZE;
           Data += TS_SIZE;
           }
     int l;
     while (const uchar *p = TsToPes.GetPes(l)) {
           int Offset = Size;
           int NewSize = Size + l;
           if (uchar *NewBuffer = (uchar *)realloc(buf, NewSize)) {
              Size = NewSize;
              buf = NewBuffer;
              memcpy(buf + Offset, p, l);
              }
           else {
              LOG_ERROR_STR("out of memory");
              free(buf);
              return;
              }
           }
     if (buf) {
        StillPicture(buf, Size);
        free(buf);
        }
     }
}

bool cDevice::Replaying(void) const
{
  return player != NULL;
}

bool cDevice::Transferring(void) const
{
  return dynamic_cast<cTransfer *>(player) != NULL;
}

bool cDevice::AttachPlayer(cPlayer *Player)
{
  if (CanReplay()) {
     if (player)
        Detach(player);
     pesAssembler->Reset();
     patPmtParser.Reset();
     player = Player;
     if (!Transferring())
        ClrAvailableTracks(false, true);
     SetPlayMode(player->playMode);
     player->device = this;
     player->Activate(true);
     return true;
     }
  return false;
}

void cDevice::Detach(cPlayer *Player)
{
  if (Player && player == Player) {
     player->Activate(false);
     player->device = NULL;
     player = NULL;
     SetPlayMode(pmNone);
     SetVideoDisplayFormat(eVideoDisplayFormat(Setup.VideoDisplayFormat));
#ifdef TS_PLAYER_BACKPORT
     PlayTs(NULL, 0);
#endif
     patPmtParser.Reset();
     Audios.ClearAudio();
#ifdef TS_PLAYER_BACKPORT
     isPlayingVideo = false;
#endif
     }
}

void cDevice::StopReplay(void)
{
  if (player) {
     Detach(player);
     if (IsPrimaryDevice())
        cControl::Shutdown();
     }
}

bool cDevice::Poll(cPoller &Poller, int TimeoutMs)
{
  return false;
}

bool cDevice::Flush(int TimeoutMs)
{
  return true;
}

int cDevice::PlayVideo(const uchar *Data, int Length)
{
  return -1;
}

int cDevice::PlayAudio(const uchar *Data, int Length, uchar Id)
{
  return -1;
}
// M7X0 BEGIN AK
int cDevice::PlayPesPacket(const uchar *Data, int Length, bool VideoOnly)
{
  const uchar streamId = Data[3];
  int written = 0;
  switch (streamId) {
    case 0xBE:          // padding stream, needed for MPEG1
    case 0xE0 ... 0xEF: // video
#ifdef TS_PLAYER_BACKPORT
         isPlayingVideo = true;
#endif
         do {
           int r = PlayVideo(Data + written, Length - written);
           if (r >= 0)
              written += r;
           else
              break;
           } while (written < Length);
         break;
    case 0xC0 ... 0xDF: { // audio
         SetAvailableTrack(ttAudio, streamId - 0xC0, streamId);
         eTrackType caud = currentAudioTrack;
         if (VideoOnly | (streamId != availableTracks[caud].id)) {
            written = Length;
            break;
            }

         Audios.PlayAudio(Data, Length, streamId);
         do {
           int r = PlayAudio(Data + written, Length - written,streamId);
           if (r >= 0)
              written += r;
           else
              break;
           } while (written < Length);
         break;
         }
    case 0xBD: { // private stream 1
         const int PayloadOffset = Data[8] + 9;
         uchar SubStreamId = Data[PayloadOffset];
         uchar SubStreamType = SubStreamId & 0xF0;
         uchar SubStreamIndex = SubStreamId & 0x1F;
         eTrackType thisTrack = ttNone;

         if (pre_1_3_19_PrivateStream) {
            SubStreamId = streamId;
            SubStreamType = 0x80;
            SubStreamIndex = 0;
            }

         switch (SubStreamType) {
           case 0x20:
           case 0x30: // SPU
                break;
           case 0x80:
                if (Setup.UseDolbyDigital)
                   thisTrack = ttDolby;
                break;
           case 0xA0:
                thisTrack = ttAudio;
                break;
           default:
                SubStreamId = streamId;
                //SubStreamType = 0x80; // Value stored to 'SubStreamType' is never read
                SubStreamIndex = 0;
                if (Setup.UseDolbyDigital)
                   thisTrack = ttDolby;
                dsyslog("switching to pre 1.3.19 Dolby Digital compatibility mode");
                ClrAvailableTracks();
                pre_1_3_19_PrivateStream = true;
           }
         if (thisTrack == ttNone) {
            written = Length;
            break;
            }
         SetAvailableTrack(thisTrack, SubStreamIndex, SubStreamId);
         eTrackType caud = currentAudioTrack;
         if (VideoOnly | (SubStreamId != availableTracks[caud].id)) {
            written = Length;
            break;
            }

         Audios.PlayAudio(Data, Length, SubStreamId);
         do {
           int r = PlayAudio(Data + written, Length - written, SubStreamId);
           if (r >= 0)
              written += r;
           else
              break;
           } while (written < Length);
         break;
         }
    default:
         written = Length;
    }

  if (written != Length) {
     if (written) {
        esyslog("ERROR: incomplete PES packet write!");
        }
     return -1;
     }
  return Length;
}

int cDevice::PlayPes(const uchar *Data, int Length, bool VideoOnly)
{
  if (!Data) {
     pesAssembler->Reset();
     return 0;
     }

  int Result = 0;
  while (Length > 0) {
        int used = pesAssembler->Put(Data,Length);
        if (used < 0)
           return used;
        Length -= used;
        Result += used;
        Data += used;

        int pLength;
        const uchar *pData = pesAssembler->Packet(pLength);
        if (!pData)
           continue;

        for (;;) {
        int w = PlayPesPacket(pData, pLength, VideoOnly);
        if (w < 0) {
           if ((errno == EAGAIN) && (Result != used))
              continue;
           return w;
           }
        break;
        }
        }
  return Result;
}
// M7X0 END AK

int cDevice::Ca(void) const
{
  int ca = 0;
//M7X0 BEGIN AK
  for (int i = 0; i <= MAXRECEIVERS; i++) {
      if (receiver[i] && (ca = receiver[i]->ca) != 0)
//M7X0 END AK
         break; // all receivers have the same ca
      }
  return ca;
}

#ifdef TS_PLAYER_BACKPORT
int cDevice::PlayTsVideo(const uchar *Data, int Length)
{
  // Video PES has no explicit length, so we can only determine the end of
  // a PES packet when the next TS packet that starts a payload comes in:
  if (TsPayloadStart(Data)) {
     int l;
     while (const uchar *p = tsToPesVideo.GetPes(l)) {
           int w = PlayVideo(p, l);
           if (w <= 0) {
              tsToPesVideo.SetRepeatLast();
              return w;
              }
           }
     tsToPesVideo.Reset();
     }
  tsToPesVideo.PutTs(Data, Length);
  return Length;
}

int cDevice::PlayTsAudio(const uchar *Data, int Length)
{
  // Audio PES always has an explicit length and consists of single packets:
  int l;
  if (const uchar *p = tsToPesAudio.GetPes(l)) {
     int w = PlayAudio(p, l, p[3]);
     if (w <= 0) {
        tsToPesAudio.SetRepeatLast();
        return w;
        }
     tsToPesAudio.Reset();
     }
  tsToPesAudio.PutTs(Data, Length);
  return Length;
}

/*
int cDevice::PlayTsSubtitle(const uchar *Data, int Length)
{
  if (!dvbSubtitleConverter)
     dvbSubtitleConverter = new cDvbSubtitleConverter;
  tsToPesSubtitle.PutTs(Data, Length);
  int l;
  if (const uchar *p = tsToPesSubtitle.GetPes(l)) {
     dvbSubtitleConverter->Convert(p, l);
     tsToPesSubtitle.Reset();
     }
  return Length;
}
*/

//TODO detect and report continuity errors?
int cDevice::PlayTs(const uchar *Data, int Length, bool VideoOnly)
{
  int Played = 0;
  if (Data == NULL) {
     tsToPesVideo.Reset();
     tsToPesAudio.Reset();
     //tsToPesSubtitle.Reset();
     }
  else if (Length < TS_SIZE) {
     esyslog("ERROR: skipped %d bytes of TS fragment", Length);
     return Length;
     }
  else {
     cMutexLock MutexLock(&mutexCurrentAudioTrack);
     while (Length >= TS_SIZE) {
           if (Data[0] != TS_SYNC_BYTE) {
              int Skipped = 1;
              while (Skipped < Length && (Data[Skipped] != TS_SYNC_BYTE || Length - Skipped > TS_SIZE && Data[Skipped + TS_SIZE] != TS_SYNC_BYTE))
                    Skipped++;
              esyslog("ERROR: skipped %d bytes to sync on start of TS packet", Skipped);
              return Played + Skipped;
              }
           int Pid = TsPid(Data);
           if (TsHasPayload(Data)) { // silently ignore TS packets w/o payload
              int PayloadOffset = TsPayloadOffset(Data);
              if (PayloadOffset < TS_SIZE) {
                 if (Pid == PATPID)
                    patPmtParser.ParsePat(Data, TS_SIZE);
                 else if (Pid == patPmtParser.PmtPid())
                    patPmtParser.ParsePmt(Data, TS_SIZE);
                 else if (Pid == patPmtParser.Vpid()) {
                    isPlayingVideo = true;
                    int w = PlayTsVideo(Data, TS_SIZE);
                    if (w < 0)
                       return Played ? Played : w;
                    if (w == 0)
                       break;
                    }
                 else if (Pid == availableTracks[currentAudioTrack].id) {
                    if (!VideoOnly /*|| HasIBPTrickSpeed()*/) {
                       int w = PlayTsAudio(Data, TS_SIZE);
                       if (w < 0)
                          return Played ? Played : w;
                       if (w == 0)
                          break;
                       Audios.PlayTsAudio(Data, TS_SIZE);
                       }
                    }
/*
                 else if (Pid == availableTracks[currentSubtitleTrack].id) {
                    if (!VideoOnly || HasIBPTrickSpeed())
                       PlayTsSubtitle(Data, TS_SIZE);
                    }
*/
                 }
              }
           else if (Pid == patPmtParser.Ppid()) {
              int w = PlayTsVideo(Data, TS_SIZE);
              if (w < 0)
                 return Played ? Played : w;
              if (w == 0)
                 break;
              }
           Played += TS_SIZE;
           Length -= TS_SIZE;
           Data += TS_SIZE;
           }
     }
  return Played;
}
#endif

int cDevice::Priority(void) const
{
  int priority = IsPrimaryDevice() ? Setup.PrimaryLimit - 1 : DEFAULTPRIORITY;
//M7X0 BEGIN AK
  for (int i = 0; i <= MAXRECEIVERS; i++) {
//M7X0 END AK
      if (receiver[i])
         priority = std::max(receiver[i]->priority, priority);
      }
  return priority;
}

bool cDevice::Ready(void)
{
  return true;
}

int cDevice::ProvidesCa(const cChannel *Channel) const
{
  int Ca = Channel->Ca();
  if (Ca == CardIndex() + 1)
     return 1; // exactly _this_ card was requested
  if (Ca && Ca <= CA_DVB_MAX)
     return 0; // a specific card was requested, but not _this_ one
  return !Ca; // by default every card can provide FTA
}

bool cDevice::Receiving(bool CheckAny) const
{
//M7X0 BEGIN AK
  for (int i = 0; i <= MAXRECEIVERS; i++) {
//M7X0 END AK
      if (receiver[i] && (CheckAny || receiver[i]->priority >= 0)) // cReceiver with priority < 0 doesn't count
         return true;
      }
  return false;
}

//M7X0 BEGIN AK
bool cDevice::FreeReceiverSlot(void) const
{
  for (int i = 0; i < MAXRECEIVERS; i++) {
      if (!receiver[i] || receiver[i]->priority < 0)
         return true;
      }
  return false;
}
// Seems to perform much better, if more than one ts packet (with the same PID)
// is deliviered in one loops
#define LOCKWAITTIME 10 // ms
void cDevice::FasterLockAction(void)
{
  actionLock = 1; // Signal wait for lock
  int readLock = otherLock;

  if (readLock == 0) {  // Other read at least wait
     actionLock = 2;    // Own lock
     readLock = otherLock;
     if (readLock != 3) {  // Other are in hardlocking ?
        return;
        }
     actionLock = 1;    // okay need hardlock
     }

  Lock();
  actionLock = 3;

  readLock = otherLock;
  while (readLock == 2) {
        cCondWait::SleepMs(LOCKWAITTIME);
        readLock = otherLock;
        }
}

void cDevice::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cDevice::FasterLockOther(void)
{
  otherLock = 1; // Signal wait for lock
  int readLock = actionLock;

  if (readLock == 0) {  // Other read at least wait
     otherLock = 2;    // Own lock
     readLock = actionLock;
     if (readLock != 3) {  // Other are in hardlocking ?
        return;
        }
     otherLock = 1;    // okay need hardlock
     }

  Lock();
  otherLock = 3;

  readLock = actionLock;
  while (readLock == 2) {
        cCondWait::SleepMs(LOCKWAITTIME);
        readLock = actionLock;
        }
}

void cDevice::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cDevice::Action(void)
{
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
  sTsDataHeader header;
  header.startsWithVideoFrame = tsVideoFrameUnknown;
#endif
  if (Running() && OpenDvr()) {
     int Length;
     uchar *b = NULL;
     while (Running()) {
           // Read data from the DVR device:
#ifdef USE_HW_VIDEO_FRAME_EVENTS
           if (GetTSPackets(b,Length,header.pid,header.startsWithVideoFrame)) {
#else
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
           if (GetTSPackets(b,Length,header.pid)) {
#else
           int Pid;
           if (GetTSPackets(b,Length,Pid)) {
#endif
#endif
              if (b) {
#ifdef USE_HW_VIDEO_FRAME_EVENTS
                 if (pidHandles[ptRecVideo].pid == header.pid) {
                    if (firstFrameEventGotten) {
                       if (header.startsWithVideoFrame == tsVideoFrameUnknown)
                          header.startsWithVideoFrame = tsVideoFrameNone;
                       }
                    else if (header.startsWithVideoFrame != tsVideoFrameUnknown)
                       firstFrameEventGotten = true;
                    }
#endif
                 // Distribute the packet(s) to all attached receivers:
                 //Lock();
                 FasterLockAction();
                 for (int i = 0; i < MAXRECEIVERS; i++) {
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
                     if (receiver[i] && receiver[i]->WantsPid(header.pid))
                        receiver[i]->Receive(b, Length, &header);
#else
                     if (receiver[i] && receiver[i]->WantsPid(Pid))
                        receiver[i]->Receive(b, Length);
#endif
                     }
                 //Unlock();
                 FasterUnlockAction();
                 }
              }
           else
              break;
           }
     CloseDvr();
     }
}
//M7X0 END AK

bool cDevice::OpenDvr(void)
{
  return false;
}

void cDevice::CloseDvr(void)
{
}
//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
bool cDevice::GetTSPackets(uchar *&Data, int &Length, int &Pid, eTsVideoFrame &videoFrame)
#else
bool cDevice::GetTSPackets(uchar *&Data, int &Length, int &Pid)
#endif
{
  return false;
}
//M7X0 END AK

bool cDevice::AttachReceiver(cReceiver *Receiver)
{
  if (!Receiver)
     return false;
  if (Receiver->device == this)
     return true;
// activate the following line if you need it - actually the driver should be fixed!
//M7X0 BEGIN AK
//#define WAIT_FOR_TUNER_LOCK
#ifdef WAIT_FOR_TUNER_LOCK
#define TUNER_LOCK_TIMEOUT 5000 // ms
  if (!HasLock(TUNER_LOCK_TIMEOUT)) {
     esyslog("ERROR: device %d has no lock, can't attach receiver!", CardIndex() + 1);
     return false;
     }
#endif
  cMutexLock MutexLock(&mutexReceiver);

  if (dynamic_cast<cTransfer *> (Receiver)) {
     if (receiver[MAXRECEIVERS]) {
        esyslog("ERROR: device %d has already transfering Receiver!", CardIndex() + 1);
        return false;
        }
     Receiver->device = this;
     Receiver->Activate(true);
     receiver[MAXRECEIVERS] = Receiver;
     return true;
     }

  if (!Running()) {
     otherLock = 0;
     actionLock = 0;
     }

  int i;
  for (i = 0; i < MAXRECEIVERS; i++) {
      if (!receiver[i])
         break;
      }

  if (i == MAXRECEIVERS && Receiver->priority >= 0) {
     for (i = 0; i < MAXRECEIVERS; i++) {
         if (receiver[i] && receiver[i]->priority < 0) {
            Detach(receiver[i]);
            break;
            }
         }
     }

  if (i == MAXRECEIVERS) {
     for (i = 0; i < MAXRECEIVERS; i++) {
         if (receiver[i] && receiver[i]->priority < Receiver->priority) {
            Detach(receiver[i]);
            break;
            }
         }
     }
  if (i == MAXRECEIVERS) {
     esyslog("ERROR: no free receiver slot!");
     return false;
     }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (!AddPid(Receiver->pids[0],Receiver->WantsFrameEvents()?ptRecVideo:ptOther))
     return false;

  for (int n = 1; n < Receiver->numPids; n++) {
#else
  for (int n = 0; n < Receiver->numPids; n++) {
#endif
      if (!AddPid(Receiver->pids[n])) {
         for ( ; n-- > 0; )
             DelPid(Receiver->pids[n]);
         return false;
         }
      }
  Receiver->Activate(true);
  //Lock();
  FasterLockOther();
  Receiver->device = this;
  receiver[i] = Receiver;
  //Unlock();
  FasterUnlockOther();

  if (!Running())
     Start();
  if (ciHandler)
     ciHandler->StartDecrypting();
  return true;
//M7X0 END AK
}

//M7X0 BEGIN AK
void cDevice::Detach(cReceiver *Receiver)
{
  if (!Receiver || Receiver->device != this)
     return;
  bool receiversLeft = false;

  cMutexLock MutexLock(&mutexReceiver);
  if (receiver[MAXRECEIVERS] == Receiver) {
     Receiver->Activate(false);
     receiver[MAXRECEIVERS] = NULL;
     Receiver->device = NULL;
     return;
     }

  for (int i = 0; i < MAXRECEIVERS; i++) {
      if (receiver[i] == Receiver) {
         //Lock();
         FasterLockOther();
         receiver[i] = NULL;
         Receiver->device = NULL;
         //Unlock();
         FasterUnlockOther();
         Receiver->Activate(false);
         for (int n = 0; n < Receiver->numPids; n++)
             DelPid(Receiver->pids[n]);
         }
      else if (receiver[i])
         receiversLeft = true;
      }
  if (ciHandler)
     ciHandler->StartDecrypting();
  if (!receiversLeft)
     Cancel(5);
}
//M7X0 END AK
void cDevice::DetachAll(int Pid)
{
  if (Pid) {
     cMutexLock MutexLock(&mutexReceiver);
     for (int i = 0; i < MAXRECEIVERS; i++) {
         cReceiver *Receiver = receiver[i];
         if (Receiver && Receiver->WantsPid(Pid))
            Detach(Receiver);
         }
     }
}

void cDevice::DetachAllReceivers(void)
{
  cMutexLock MutexLock(&mutexReceiver);
//M7X0 BEGIN AK
  for (int i = 0; i <= MAXRECEIVERS; i++) {
//M7X0 END AK
      if (receiver[i])
         Detach(receiver[i]);
      }
}

// --- cTSBuffer -------------------------------------------------------------

cTSBuffer::cTSBuffer(int File, int Size, int CardIndex)
{
  SetDescription("TS buffer on device %d", CardIndex);
  f = File;
  cardIndex = CardIndex;
  delivered = false;
  ringBuffer = new cRingBufferLinear(Size, TS_SIZE, false, "TS");
  ringBuffer->SetTimeouts(100, 100);
  Start();
}

cTSBuffer::~cTSBuffer()
{
  Cancel(3);
  delete ringBuffer;
}

void cTSBuffer::Action(void)
{
  if (ringBuffer) {
     bool firstRead = true;
     cPoller Poller(f);
     while (Running()) {
           if (firstRead || Poller.Poll(100)) {
              firstRead = false;
              int r = ringBuffer->Read(f);
              if (r < 0 && FATALERRNO) {
                 if (errno == EOVERFLOW)
                    esyslog("ERROR: driver buffer overflow on device %d", cardIndex);
                 else {
                    LOG_ERROR;
                    break;
                    }
                 }
              cCondWait::SleepMs(10); // avoids small chunks of data, which cause high CPU usage, esp. on ARM CPUs
              }
           }
     }
}

uchar *cTSBuffer::Get(void)
{
  int Count = 0;
  if (delivered) {
     ringBuffer->Del(TS_SIZE);
     delivered = false;
     }
  uchar *p = ringBuffer->Get(Count);
  if (p && Count >= TS_SIZE) {
     if (*p != TS_SYNC_BYTE) {
        for (int i = 1; i < Count; i++) {
            if (p[i] == TS_SYNC_BYTE) {
               Count = i;
               break;
               }
            }
        ringBuffer->Del(Count);
        esyslog("ERROR: skipped %d bytes to sync on TS packet on device %d", Count, cardIndex);
        return NULL;
        }
     delivered = true;
     return p;
     }
  return NULL;
}

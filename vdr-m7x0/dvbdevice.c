/*
 * dvbdevice.c: The DVB device tuner interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "dvbdevice.h"
#include <errno.h>
#include <limits.h>
#include <linux/videodev.h>
//M7X0 BEGIN AK
#include "m7x0_dvb/audio.h"
#include "m7x0_dvb/dmx.h"
#include "m7x0_dvb/dmx_ext.h"
#include "m7x0_dvb/frontend.h"
#include "m7x0_dvb/video.h"
//M7X0 END AK
#define __STDC_FORMAT_MACROS // Required for format specifiers
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "channels.h"
#ifdef M750S
#include "diseqc.h"
#endif
#include "dvbosd.h"
#include "eitscan.h"
#include "player.h"
#include "receiver.h"
#include "status.h"
#include "transfer.h"
#include "audio.h"

#define DO_REC_AND_PLAY_ON_PRIMARY_DEVICE 1
#define DO_MULTIPLE_RECORDINGS 1
//#define DO_MULTIPLE_CA_CHANNELS

//M7X0 BEGIN AK
#define DEV_VIDEO         "/dev/video"
#define DEV_DVB_ADAPTER   "/dev/ost/"
#define DEV_DVB_OSD       "/dev/fb"
//M7X0 END AK
#define DEV_DVB_FRONTEND  "frontend"
#define DEV_DVB_DVR       "dvr"
#define DEV_DVB_DEMUX     "demux"
#define DEV_DVB_VIDEO     "video"
#define DEV_DVB_AUDIO     "audio"
#define DEV_DVB_CA        "ca"

//M7X0 BEGIN AK
#define DVBS_TUNE_TIMEOUT  9000 //ms
#define DVBS_LOCK_TIMEOUT  2000 //ms
#define DVBC_TUNE_TIMEOUT  9000 //ms
#define DVBC_LOCK_TIMEOUT  2000 //ms
#define DVBT_TUNE_TIMEOUT  9000 //ms
#define DVBT_LOCK_TIMEOUT  2000 //ms


// Taken from gambler
// For 16:9/4:3 switching
#define AVSWCMD_TV_FBAS    0x80
#define AVSWCMD_TV_SVIDEO  0x81
#define AVSWCMD_TV_VCR     0x82
#define AVSWCMD_TV_OFF	   0x84
#define AVSWCMD_VCR_FBAS   0x85
#define AVSWCMD_VCR_SVIDEO 0x86
#define AVSWCMD_MODE_16_9  0x89
#define AVSWCMD_MODE_4_3   0x90

//M7X0 BEGIN AK
class cDvbName {
private:
  char buffer[PATH_MAX];
public:
  cDvbName(const char *Name, int n) {
//M7X0 BEGIN AK
    snprintf(buffer, sizeof(buffer), "%s%d", Name, n);
//M7X0 END AK
    }
  const char *operator*() { return buffer; }
  };

static int DvbOpen(const char *Name, int n, int Mode, bool ReportError = false)
{
  const char *FileName = *cDvbName(Name, n);
  int fd = open(FileName, Mode);
  if (fd < 0 && ReportError)
     LOG_ERROR_STR(FileName);
  return fd;
}

// --- cDvbTuner -------------------------------------------------------------

class cDvbTuner : public cThread {
private:
  enum eTunerStatus { tsIdle, tsSet, tsTuned, tsLocked };
  int fd_frontend;
  int cardIndex;
  int tuneTimeout;
  int lockTimeout;
  time_t lastTimeoutReport;
  fe_type_t frontendType;
  cCiHandler *ciHandler;
  cChannel channel;
  eTunerStatus tunerStatus;
  cMutex mutex;
  cCondVar locked;
  cCondVar newSet;
  bool GetFrontendStatus(fe_status_t &Status, int TimeoutMs = 0);
  bool SetFrontend(void);
  virtual void Action(void);
public:
  cDvbTuner(int Fd_Frontend, int CardIndex, fe_type_t FrontendType, cCiHandler *CiHandler);
  virtual ~cDvbTuner();
  bool IsTunedTo(const cChannel *Channel) const;
  void Set(const cChannel *Channel, bool Tune);
  bool Locked(int TimeoutMs = 0);
  };

cDvbTuner::cDvbTuner(int Fd_Frontend, int CardIndex, fe_type_t FrontendType, cCiHandler *CiHandler)
{
  fd_frontend = Fd_Frontend;
  cardIndex = CardIndex;
  frontendType = FrontendType;
  ciHandler = CiHandler;
  tuneTimeout = 0;
  lockTimeout = 0;
  lastTimeoutReport = 0;
  tunerStatus = tsIdle;
#ifdef M750S
  if (frontendType == FE_QPSK)
     CHECK(ioctl(fd_frontend, FE_SET_VOLTAGE, SEC_VOLTAGE_13)); // must explicitly turn on LNB power
#endif
  SetDescription("tuner on device %d", cardIndex + 1);
  Start();
}

cDvbTuner::~cDvbTuner()
{
  tunerStatus = tsIdle;
  newSet.Broadcast();
  locked.Broadcast();
  Cancel(3);
}

bool cDvbTuner::IsTunedTo(const cChannel *Channel) const
{
  return tunerStatus != tsIdle && channel.Source() == Channel->Source() && channel.Transponder() == Channel->Transponder();
}

void cDvbTuner::Set(const cChannel *Channel, bool Tune)
{
  cMutexLock MutexLock(&mutex);
  if (Tune)
     tunerStatus = tsSet;
  channel = *Channel;
  lastTimeoutReport = 0;
  newSet.Broadcast();
}

bool cDvbTuner::Locked(int TimeoutMs)
{
  bool isLocked = (tunerStatus >= tsLocked);
//M7X0 BEGIN AK
  if (TimeoutMs<0)
     TimeoutMs=(lockTimeout!=0)?lockTimeout:3000;
//M7X0 END
  if (isLocked || !TimeoutMs)
     return isLocked;

  cMutexLock MutexLock(&mutex);
  if (TimeoutMs && tunerStatus < tsLocked)
     locked.TimedWait(mutex, TimeoutMs);
  return tunerStatus >= tsLocked;
}

//M7X0 BEGIN AK
// m7x0 seems not to can handle FE_READ_STATUS very well.
// Seems to cause hangs while change switch and unneed(?) reinits
// and EBUSY Errors.
// Lets drop this call for while and see if problems are gone
bool cDvbTuner::GetFrontendStatus(fe_status_t &Status, int TimeoutMs)
{
  dvb_frontend_event Event;

  if (TimeoutMs) {
     cPoller Poller(fd_frontend);
     if (Poller.Poll(TimeoutMs)) {
        bool r = false;
        while (ioctl(fd_frontend, FE_GET_EVENT, &Event) == 0) {
              Status = Event.status;
              r = true;
              }
        if (r)
           return true;
        }
     }

  while (1) {
        int stat = ioctl(fd_frontend, FE_READ_STATUS, &Status);
        if (stat == 0)
           return true;
        if (stat < 0 && errno != EINTR && errno != EBUSY) // M7X0 returns in many cases EBUSY
           return false;

        cCondWait::SleepMs(3);
        }

  return false;
}
//M7X0 END AK

static unsigned int FrequencyToHz(unsigned int f)
{
  while (f && f < 1000000)
        f *= 1000;
  return f;
}

bool cDvbTuner::SetFrontend(void)
{
#ifdef M750S
  dvb_frontend_parameters Frontend;
  dvb_set_qpsk_parameters qpsk_Frontend;
#endif
  dvb_set_ofdm_parameters ofdm_Frontend;
  void *set_arg = &ofdm_Frontend;
  int set_call = FE_SET_OFDM;

  switch (frontendType) {
#ifdef M750S
    case FE_QPSK: { // DVB-S
//M7x0 BEGIN AK
         memset(&qpsk_Frontend, 0, sizeof(qpsk_Frontend));
         int frequency = channel.Frequency();
         tuneTimeout = DVBS_TUNE_TIMEOUT;
         lockTimeout = DVBS_LOCK_TIMEOUT;

         qpsk_Frontend.symbol_rate = channel.Srate();
         qpsk_Frontend.fec_inner = FEC_AUTO;
         set_call = FE_SET_QPSK;
         set_arg = &qpsk_Frontend;

         if (!Setup.DiSEqC) {
            if (frequency < Setup.LnbSLOF) {
               frequency -= Setup.LnbFrequLo;
               qpsk_Frontend.tone = SEC_TONE_OFF;
               }
            else {
               frequency -= Setup.LnbFrequHi;
               qpsk_Frontend.tone = SEC_TONE_ON;
               }
            char pol = channel.Polarization();
            qpsk_Frontend.voltage =  ((pol == 'v') | (pol == 'V') |
                                      (pol == 'r') | (pol == 'R')) ?
                                     SEC_VOLTAGE_13 : SEC_VOLTAGE_18;
            frequency = abs(frequency); // Allow for C-band, where the frequency is less than the LOF
            qpsk_Frontend.frequency = frequency * 1000UL;
            break;
            }

         cDiseqc *diseqc = Diseqcs.Get(channel.Source(), channel.Frequency(), channel.Polarization());
         if (diseqc) {
            int diseqc_idx = 0;
            cDiseqc::eDiseqcActions da;
            for (char *CurrentAction = NULL; (da = diseqc->Execute(&CurrentAction)) != cDiseqc::daNone; ) {
                switch (da) {
                  case cDiseqc::daNone:      break;
                  case cDiseqc::daToneOff:
                       qpsk_Frontend.tone = SEC_TONE_OFF;
                       break;
                  case cDiseqc::daToneOn:
                       qpsk_Frontend.tone = SEC_TONE_ON;
                       break;
                  case cDiseqc::daVoltage13:
                       qpsk_Frontend.voltage = SEC_VOLTAGE_13;
                       break;
                  case cDiseqc::daVoltage18:
                       qpsk_Frontend.voltage = SEC_VOLTAGE_18;
                       break;
                  case cDiseqc::daMiniA:
                       qpsk_Frontend.toneburst = SEC_MINI_A;
                       break;
                  case cDiseqc::daMiniB:
                       qpsk_Frontend.toneburst = SEC_MINI_B;
                       break;
                  case cDiseqc::daCodes: {
                       int n = 0;
                       uchar *codes = diseqc->Codes(n);
                       if ((codes != NULL) & (diseqc_idx < 5)) {
                          qpsk_Frontend.diseqc[diseqc_idx].diseqc_mode = 2;
                          qpsk_Frontend.diseqc[diseqc_idx].msg_len =
                             std::min(n, int(sizeof(qpsk_Frontend.diseqc[diseqc_idx].msg)));
                          memcpy(qpsk_Frontend.diseqc[diseqc_idx].msg, codes,
                                qpsk_Frontend.diseqc[diseqc_idx].msg_len);
                          diseqc_idx++;
                          }
                       }
                       break;
                  default: esyslog("ERROR: unknown diseqc command %d", da);
                  }
                }
            frequency -= diseqc->Lof();
            frequency = abs(frequency); // Allow for C-band, where the frequency is less than the LOF
            qpsk_Frontend.frequency = frequency * 1000UL;
            }
         else {
            esyslog("ERROR: no DiSEqC parameters found for channel %d", channel.Number());
            return false;
            }
         }
         break;
    case FE_QAM: { // DVB-C
         memset(&Frontend, 0, sizeof(Frontend));
         // Frequency and symbol rate:
         set_call = FE_SET_FRONTEND;
         set_arg = &Frontend;
         Frontend.frequency = FrequencyToHz(channel.Frequency());
#ifndef USE_TUNER_AUTOVALUES
         Frontend.inversion = fe_spectral_inversion_t(channel.Inversion());
#else
         Frontend.inversion = INVERSION_AUTO;
#endif
         Frontend.u.qam.symbol_rate = channel.Srate() * 1000UL;
#ifndef USE_TUNER_AUTOVALUES
         Frontend.u.qam.fec_inner = fe_code_rate_t(channel.CoderateH());
         Frontend.u.qam.modulation = fe_modulation_t(channel.Modulation());
#else
         Frontend.u.qam.fec_inner = FEC_AUTO;
         Frontend.u.qam.modulation = QAM_AUTO;
#endif

         tuneTimeout = DVBC_TUNE_TIMEOUT;
         lockTimeout = DVBC_LOCK_TIMEOUT;
         }
         break;
#endif // M750S
    case FE_OFDM: { // DVB-T
         memset(&ofdm_Frontend, 0, sizeof(ofdm_Frontend));
         // Frequency and timeout:
         ofdm_Frontend.frequency_khz = FrequencyToHz(channel.Frequency())/1000;
         tuneTimeout = DVBT_TUNE_TIMEOUT;
         lockTimeout = DVBT_LOCK_TIMEOUT;
         }
         break;
    default:
         esyslog("ERROR: attempt to set channel with unknown DVB frontend type");
         return false;
    }

  do {
     if (ioctl(fd_frontend, set_call, set_arg) == 0)
        return true;

     if (errno != EINTR && errno != EBUSY) {
        char __errorstr[256];
        strerror_r(errno,__errorstr,256);
        __errorstr[255] = 0;
        esyslog("ERROR: frontend %d: %s", cardIndex,__errorstr);
        return false;
        }
     cCondWait::SleepMs(3);
     } while (1);
//M7x0 END AK
  return true;
}

void cDvbTuner::Action(void)
{
  cTimeMs Timer;
  bool LostLock = false;
  fe_status_t Status = (fe_status_t)0;
  while (Running()) {
        fe_status_t NewStatus;
        if (GetFrontendStatus(NewStatus, tunerStatus != tsTuned ? 10 : tuneTimeout))
           Status = NewStatus;
        cMutexLock MutexLock(&mutex);
        switch (tunerStatus) {
          case tsIdle:
               break;
          /* coverity[unterminated_case] */
          case tsSet:
               tunerStatus = SetFrontend() ? tsTuned : tsIdle;
               Timer.Set(tuneTimeout);
               continue;
          /* coverity[unterminated_case] */
          case tsTuned:
               if (Timer.TimedOut()) {
                  tunerStatus = tsSet;
                  if (time(NULL) - lastTimeoutReport > 60) { // let's not get too many of these
                     if (channel.Number()) // no need to log this for transponders that are announced in the NIT but are not currently broadcasting
                        isyslog("frontend %d timed out while tuning to channel %d, tp %d", cardIndex, channel.Number(), channel.Transponder());
                     lastTimeoutReport = time(NULL);
                     }
                  continue;
                  }
          case tsLocked:
               if (Status & FE_REINIT) {
                  tunerStatus = tsSet;
                  isyslog("frontend %d was reinitialized", cardIndex);
                  lastTimeoutReport = 0;
                  continue;
                  }
               else if (Status & FE_HAS_LOCK) {
                  if (LostLock) {
                     isyslog("frontend %d regained lock on channel %d, tp %d", cardIndex, channel.Number(), channel.Transponder());
                     LostLock = false;
                     }
                  tunerStatus = tsLocked;
                  locked.Broadcast();
                  lastTimeoutReport = 0;
                  }
               else if (tunerStatus == tsLocked) {
                  LostLock = true;
                  isyslog("frontend %d lost lock on channel %d, tp %d", cardIndex, channel.Number(), channel.Transponder());
                  tunerStatus = tsTuned;
                  Timer.Set(lockTimeout);
                  lastTimeoutReport = 0;
                  continue;
                  }
               break;
          default: esyslog("ERROR: unknown tuner status %d", tunerStatus);
          }

        if (ciHandler)
           ciHandler->Process();
        if (tunerStatus != tsTuned)
           newSet.TimedWait(mutex, 1000);
        }
}
//M7X0 BEGIN AK

#define INITIAL_TRICKSPEED_BUFFER_SIZE KILOBYTE(256)
#define REPLAY_MAX_UNUSABLE_DATA KILOBYTE(512)
#define REPLAY_TIMEOUT 3

#ifndef TS_PLAYER_BACKPORT
// -----------------
// c7x0Ts

#define MAXPMTIDS 64

class c7x0TsReplayer {
private:
  cDvbDevice *dvbDevice;
  int fd_dvr;
  int fd_dmx_video;
  int fd_dmx_audio;
  int fd_dmx_pcr;
  int fd_dmx_teletext;

  struct {
    int patVersion;
    int readVersion;
    int curCCounter;
    int curSection;
    int numPmtIds;
    int pmtIds[MAXPMTIDS];
    } sPatInfo;
  uchar patBuf[1024 + 184];
  uchar pmtBuf[1024 + 184];
  int patBufLength;
  int pmtBufLength;
  int curPmtId;
  int pmtCCounter;
  int curPmtVersion;



  int videoStreamId;
  int pcrStreamId;
  int teletextStreamId;

  eTrackType curAudioTrack;
  int curAudioStreamId;

  uchar fragmentData[188];
  int fragmentLen;


  int skippedBytes;
  int unusableDataCount;
  time_t lastPlaytime;

  uchar *trickspeedData;
  int trickspeedLen;
  int trickspeedSize;
  bool inTrickspeed;
  bool inFastplay;
  enum {
       syncing,
       findFrame,
       scanFrame
       } trickspeedState;
  uint32_t trickspeedScanner;
  uchar pesHeader[9 + 255];
  int pesHeaderLength;
  int pesPacketLen;
  bool pesHeaderCompleted;
  int trickspeedCCounter;

  bool ReallocTrickspeedBuffer(const int Size);
  bool ScanDataTrickspeed(const uchar *&Data, const uchar *Limit);
  void OpenDvr(void);
  void CloseDvr(void);
  inline bool CheckTsHeader(const uchar *Data, bool &PayloadStart,
             int &PayloadOffset, int &CurCCounter, bool &OutOfSeq);

  inline bool CopySectionData(const uchar *Data, int Count, bool SectionStart,
                              uchar *SectionBuffer, int &SectionBufferLength);

  inline bool GetCommonSectionHeader(const uchar *Data, int Count,
            int ExpectedTableId, int &SectionLen, int &SectionVer,
                bool &Current, int &SectionNo, int &LastSectionNo);

  inline int HandlePmtElemInfo(const uchar *Data, int Length, int &Vpid,
                     int &Apid, int &Dpid, char *Alangs, char *Dlangs, int &Tpid);

  inline bool HandlePesTrickspeed(const uchar *&Data,
          const uchar *const Limit, bool payloadStart);

  void HandlePat(const uchar *Data);
  void HandlePmt(const uchar *Data);
  inline int CheckPmtIds(const int Pid);

  void CheckAndSetupAudio(bool idsChanged = false);
  void CheckAndSetupVideo(int PcrId, int VideoId, int TeletextId);
  int ScanDataForPacketStartCode(const uchar *&Data,const uchar *const Limit);
  int HandleTrickspeed(const uchar *Data);
#ifdef USE_PLAYER_WATCHDOG
  bool CheckTimeout(void);
#endif

  int WriteOutPacket(const uchar *Data, int Count);

public:
  void Reset(void);
  c7x0TsReplayer(cDvbDevice *dev);
  ~c7x0TsReplayer();
  int PlayTs(const uchar *Data, int Length);
  void TrickSpeed(const int Speed,const bool UseFastForward);
  void Clear();
  void Play();
  void Freeze();
  bool Poll(cPoller &Poller,const int TimeoutMs);
  bool Flush(const int TimeoutMs);
  void SetPids(int PmtPid, int VideoPid);
  int GetVideoPid(void) { return (videoStreamId > 0 ? videoStreamId : 0); }
  };


void c7x0TsReplayer::Reset(void)
{
  CloseDvr();

  videoStreamId = -1;
  pcrStreamId = -1;
  teletextStreamId = -1;

  sPatInfo.patVersion = -1;
  sPatInfo.readVersion = -1;
  sPatInfo.curCCounter = -1;
  sPatInfo.curSection = 0;
  sPatInfo.numPmtIds = 0;
  pmtCCounter = -1;
  patBufLength = 0;
  pmtBufLength = 0;
  curPmtId = -1;
  pmtCCounter = -1;
  curPmtVersion = -1;

  curAudioTrack = ttNone;
  curAudioStreamId = 0;

  fragmentLen = 0;
  skippedBytes = 0;
  unusableDataCount = 0;

  trickspeedScanner = 0xFFFFFFFF;
  inTrickspeed = false;
  inFastplay = false;
  trickspeedState = syncing;
  trickspeedLen = 0;
  trickspeedCCounter = -1;
  pesHeaderLength = 0;
  pesPacketLen = 0;
  pesHeaderCompleted = false;
  dvbDevice->ClrAvailableTracks();
}

c7x0TsReplayer::c7x0TsReplayer(cDvbDevice *dev)
{
  dvbDevice = dev;
  fd_dvr = -1;
  Reset();
  trickspeedSize = 0;
  trickspeedData = NULL;
  ReallocTrickspeedBuffer(INITIAL_TRICKSPEED_BUFFER_SIZE);
}

c7x0TsReplayer::~c7x0TsReplayer()
{
  CloseDvr();
  free(trickspeedData);
}

bool c7x0TsReplayer::ReallocTrickspeedBuffer(const int Size)
{
  if (Size <= trickspeedSize)
     return true;

  if (uchar *NewBuffer = (uchar *)realloc(trickspeedData, Size)) {
     trickspeedData = NewBuffer;
     trickspeedSize = Size;
     }
  else {
     esyslog("ERROR: out of memory");
     return false;
     }

  return true;
}

void c7x0TsReplayer::OpenDvr(void)
{
  if (fd_dvr >= 0)
     return;

  // None-Blocking-Mode not working as expected
  fd_dvr = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DVR, 0, O_WRONLY, true);


  if (fd_dvr <0 )
     return;

  //CHECK(ioctl(fd_dvr,DVR_SET_STREAM_TYPE, DVR_MPEG2_TS));

  CHECK(ioctl(dvbDevice->fd_audio,AUDIO_SET_AV_SYNC,dvbDevice->playMode == pmTsAudioVideo));

  fd_dmx_audio = -1;
  fd_dmx_video = -1;
  fd_dmx_pcr   = -1;
  fd_dmx_teletext   = -1;
  curAudioTrack = ttNone;
  CheckAndSetupVideo(pcrStreamId, videoStreamId, teletextStreamId);
  CheckAndSetupAudio();
  lastPlaytime = time(NULL);
}

void c7x0TsReplayer::CheckAndSetupVideo(int PcrId, int VideoId, int TeletextId)
{
  if (fd_dvr < 0)
     return;

  if (PcrId >= 0 && (PcrId != pcrStreamId || fd_dmx_pcr < 0)) {
     if (fd_dmx_pcr >= 0) {
        CHECK(ioctl(fd_dmx_pcr, DMX_STOP,1));
        }
     else {
        fd_dmx_pcr = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, 0, O_RDWR | O_NONBLOCK, true);
        if (fd_dmx_pcr < 0) {
           return;
           }
        }
     dmx_pes_filter_params pesFilterParams;
     memset(&pesFilterParams, 0, sizeof(pesFilterParams));

     pesFilterParams.pid      = PcrId;
     pesFilterParams.input    = DMX_IN_DVR;
     pesFilterParams.output   = DMX_OUT_DECODER0;
     pesFilterParams.pes_type = DMX_PES_PCR;
     pesFilterParams.flags    = DMX_IMMEDIATE_START;

     int r, i = 0, errnoSave;

     // Is this loop really nessesary any more.
     // In earllier Versions the driver returns with EBUSY sometimes
     do {
        if ((r = ioctl(fd_dmx_pcr,DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
           errnoSave = errno;
           CHECK(r);
           cCondWait::SleepMs(3);
           }
        else
           errnoSave = 0;
        i++;
        } while (errnoSave == EBUSY && i <= 100);

     if (errnoSave != 0) {
        close(fd_dmx_pcr);
        fd_dmx_pcr = -1;
        return;
        }
     pcrStreamId = PcrId;
     }

  if (VideoId > 0 && (VideoId != videoStreamId || fd_dmx_video < 0) && (dvbDevice->playMode == pmTsAudioVideo || dvbDevice->playMode == pmTsVideoOnly)) {
     if (fd_dmx_video >= 0) {
        CHECK(ioctl(fd_dmx_video, DMX_STOP,1));
        CHECK(ioctl(dvbDevice->fd_video,VIDEO_STOP,0));
        }
     else {
        fd_dmx_video = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, 0, O_RDWR | O_NONBLOCK, true);
        if (fd_dmx_video < 0) {
           return;
           }
        }
     dmx_pes_filter_params pesFilterParams;
     memset(&pesFilterParams, 0, sizeof(pesFilterParams));

     // Uglly the driver needs setting exacty this Value
     // Yet another BUG in m7x0-drivers
     CHECK(ioctl(fd_dmx_video, DMX_SET_BUFFER_SIZE,0x100000));

     pesFilterParams.pid      = VideoId;
     pesFilterParams.input    = DMX_IN_DVR;
     pesFilterParams.output   = DMX_OUT_DECODER0;
     pesFilterParams.pes_type = DMX_PES_VIDEO;
     pesFilterParams.flags    = DMX_IMMEDIATE_START;

     int r, i = 0, errnoSave;

     // Is this loop really nessesary any more.
     // In earllier Versions the driver returns with EBUSY sometimes
     do {
        if ((r = ioctl(fd_dmx_video,DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
           errnoSave = errno;
           CHECK(r);
           cCondWait::SleepMs(3);
           }
        else
           errnoSave = 0;
        i++;
        } while (errnoSave == EBUSY && i <= 100);

     if (errnoSave != 0) {
        close(fd_dmx_video);
        fd_dmx_video = -1;
        return;
        }

     videoStreamId = VideoId;
     CHECK(ioctl(dvbDevice->fd_video,VIDEO_PLAY,0));
     }
  if (TeletextId >= 0 && (TeletextId != teletextStreamId || fd_dmx_teletext < 0)) {
     if (fd_dmx_teletext >= 0) {
        CHECK(ioctl(fd_dmx_teletext, DMX_STOP,1));
        }
     else {
        fd_dmx_teletext = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, 0, O_RDWR | O_NONBLOCK, true);
        if (fd_dmx_teletext < 0) {
           return;
           }
        }
     dmx_pes_filter_params pesFilterParams;
     memset(&pesFilterParams, 0, sizeof(pesFilterParams));

     //CHECK(ioctl(fd_dmx_teletext, DMX_SET_BUFFER_SIZE,0x100000));

     pesFilterParams.pid      = TeletextId;
     pesFilterParams.input    = DMX_IN_DVR;
     pesFilterParams.output   = DMX_OUT_DECODER0;
     pesFilterParams.pes_type = DMX_PES_TELETEXT;
     pesFilterParams.flags    = DMX_IMMEDIATE_START;

     int r, i = 0, errnoSave;

     // Is this loop really nessesary any more.
     // In earllier Versions the driver returns with EBUSY sometimes
     do {
        if ((r = ioctl(fd_dmx_teletext,DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
           errnoSave = errno;
           CHECK(r);
           cCondWait::SleepMs(3);
           }
        else
           errnoSave = 0;
        i++;
        } while (errnoSave == EBUSY && i <= 100);

     if (errnoSave != 0) {
        close(fd_dmx_teletext);
        fd_dmx_teletext = -1;
        return;
        }
     teletextStreamId = TeletextId;
     }

}

void c7x0TsReplayer::CheckAndSetupAudio(bool idsChanged)
{
  if (fd_dvr < 0)
     return;

  eTrackType newAudioTrack = dvbDevice->GetCurrentAudioTrack();


  if (newAudioTrack != ttNone && dvbDevice->playMode != pmTsVideoOnly &&
                                      (newAudioTrack != curAudioTrack ||
                   (idsChanged && dvbDevice->GetTrack(newAudioTrack)->id
                                                  != curAudioStreamId))) {

     curAudioStreamId = dvbDevice->GetTrack(newAudioTrack)->id;

     if (fd_dmx_audio >= 0) {
        CHECK(ioctl(fd_dmx_audio, DMX_STOP,1));
        CHECK(ioctl(dvbDevice->fd_audio,AUDIO_STOP,0));
        }
     else {
        fd_dmx_audio = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, 0,
                                      O_RDWR | O_NONBLOCK, true);
        if (fd_dmx_audio < 0) {
           return;
           }
        }

     dmx_pes_filter_params pesFilterParams;
     memset(&pesFilterParams, 0, sizeof(pesFilterParams));
     CHECK(ioctl(fd_dmx_audio, DMX_SET_BUFFER_SIZE,0x1E000));
     pesFilterParams.input    = DMX_IN_DVR;
     pesFilterParams.pes_type = DMX_PES_AUDIO;
     pesFilterParams.flags    = DMX_IMMEDIATE_START;
     pesFilterParams.pid = curAudioStreamId;


     if (IS_DOLBY_TRACK(newAudioTrack)) {
        pesFilterParams.output   = DMX_OUT_DECODER1;
        }
     else {
        pesFilterParams.output   = DMX_OUT_DECODER0;
        }

      int r, i = 0, errnoSave;

     // Is this loop really nessesary any more.
     // In earllier Versions the driver returns with EBUSY sometimes
     do {
        if ((r = ioctl(fd_dmx_audio, DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
           errnoSave = errno;
           CHECK(r);
           cCondWait::SleepMs(3);
           }
        else
           errnoSave = 0;
        i++;
        } while (errnoSave == EBUSY && i <= 100);

     if (errnoSave != 0) {
        close(fd_dmx_audio);
        return;
        }

     CHECK(ioctl(dvbDevice->fd_audio,AUDIO_PLAY,0));
     curAudioTrack = newAudioTrack;
     }
}

void c7x0TsReplayer::CloseDvr()
{
  if (fd_dvr < 0)
     return;

  if (fd_dmx_audio >= 0) {
     CHECK(ioctl(fd_dmx_audio, DMX_STOP,1));
     close(fd_dmx_audio);
     fd_dmx_audio = -1;
     curAudioTrack = ttNone;
     }

  if (fd_dmx_video >= 0) {
     CHECK(ioctl(fd_dmx_video, DMX_STOP,1));
     close(fd_dmx_video);
     fd_dmx_video = -1;
     }

  if (fd_dmx_pcr >= 0) {
     CHECK(ioctl(fd_dmx_pcr, DMX_STOP,1));
     close(fd_dmx_pcr);
     fd_dmx_pcr = -1;
     }

  if (fd_dmx_teletext >= 0) {
     CHECK(ioctl(fd_dmx_teletext, DMX_STOP,1));
     close(fd_dmx_teletext);
     fd_dmx_teletext = -1;
     }

  close(fd_dvr);
  fd_dvr = -1;
  if (dvbDevice->playMode != pmTsVideoOnly)
     CHECK(ioctl(dvbDevice->fd_audio, AUDIO_STOP, 0))
  if (dvbDevice->playMode == pmTsAudioVideo || dvbDevice->playMode == pmTsVideoOnly)
     CHECK(ioctl(dvbDevice->fd_video, VIDEO_STOP, 0));
}

int c7x0TsReplayer::ScanDataForPacketStartCode(const uchar *&Data,const uchar *const Limit)
{
  const uchar *data = Data;
  const uchar *const limit = Limit;

  if (fragmentLen) {
     int c = std::min (188 - fragmentLen, limit - data);
     memcpy(fragmentData + fragmentLen, data, c);
     data += c;
     fragmentLen += c;

     Data = data;

     if (fragmentLen < 188)
        return -1;

     return (((fragmentData[1] << 8) | fragmentData[2]) & 0x1fff);
     }

  while (data < limit && data[0] != 0x47) {
        data++;
        }

  skippedBytes += data - Data;

  if (limit - data < 188) {
     int c =  limit - data;

     memcpy(fragmentData, data, c);
     fragmentLen = c;
     Data = limit;
     return -1;
     }

  Data = data;
  return (((data[1] << 8) | data[2]) & 0x1fff);
}

bool c7x0TsReplayer::CheckTsHeader(const uchar *Data, bool &PayloadStart,
                    int &PayloadOffset, int &CurCCounter, bool &OutOfSeq)
{

  PayloadStart = (Data[1] & 0x40);
  OutOfSeq = false;
  PayloadOffset = 4 + ((Data[3] & 0x20) ? Data[4] + 1 : 0);

  const int ccounter = Data[3] & 0xF;

  // Dublicate Packet or no Payload ?
  if (ccounter == CurCCounter || !(Data[3] & 0x10)) {
     PayloadOffset = 188;
     return true;
     }

  const int newccounter = (CurCCounter + 1) & 0xF;

  OutOfSeq = (newccounter != ccounter || CurCCounter == -1);
  CurCCounter = ccounter;


  if (Data[1] & 0x80)
     return false;

  if (PayloadOffset > 188) {
     esyslog("c7x0TsReplayer: Illegal adaption field length %d in TS-Packet", PayloadOffset - 5);
     PayloadOffset = 188;
     return false;
     }

  return true;
}

bool c7x0TsReplayer::CopySectionData(const uchar *Data, int Count,
                          bool SectionStart, uchar *SectionBuffer,
                                         int &SectionBufferLength)
{
  if (!SectionStart) {
     if (!SectionBufferLength)
        return false;

     memcpy (SectionBuffer + SectionBufferLength, Data, Count);
     SectionBufferLength += Count;
     return true;
     }

  const int pointer = Data[0] + 1;
  if (pointer >= Count) {
     esyslog("c7x0TsReplayer: Illegal pointer field %d in Section", pointer);
     return false;
     }

  if (!SectionBufferLength) {
     SectionBufferLength = Count - pointer;
     memcpy (SectionBuffer, Data + pointer, SectionBufferLength);
     return true;
     }

  Count--;
  Data++;
  memcpy (SectionBuffer + SectionBufferLength, Data, Count);
  SectionBufferLength += Count;
  return true;
}

bool c7x0TsReplayer::GetCommonSectionHeader(const uchar *Data, int Count,
                   int ExpectedTableId, int &SectionLen, int &SectionVer,
                         bool &Current, int &SectionNo, int &LastSectionNo)
{
  if (ExpectedTableId != Data[0]) {
     esyslog("c7x0TsReplayer: Illegal table id %hhd (expected %d) in section", Data[0], ExpectedTableId);
     return false;
     }

  if (Count < 12) {
     SectionLen = 12;
     return true;
     }

  SectionLen = (((Data[1] << 8) | Data[2]) & 0xFFF) + 3;
  if (SectionLen > 1024) {
     esyslog("c7x0TsReplayer: Illegal section length %d", SectionLen);
     return false;
     }

  SectionVer = (Data[5] >> 1) & 0x1F;
  Current = Data[5] & 0x1;
  SectionNo = Data[6];
  LastSectionNo = Data[7];
  return true;
}



void c7x0TsReplayer::HandlePat(const uchar *Data)
{

  bool sectionStart;
  int payloadOffset;
  bool outOfSeq;

  if (!CheckTsHeader(Data, sectionStart, payloadOffset,
                        sPatInfo.curCCounter, outOfSeq)) {
     patBufLength = 0;
     sPatInfo.curSection = 0;
     return;
     }

  if (payloadOffset >= 188)
     return;

  if (outOfSeq) {
     patBufLength = 0;
     sPatInfo.curSection = 0;
     }

  if (!CopySectionData(Data + payloadOffset, 188 - payloadOffset,
                              sectionStart, patBuf, patBufLength)) {
     patBufLength = 0;
     sPatInfo.curSection = 0;
     return;
     }


  while (true) {
        int sectionLen;
        int sectionVer;
        bool currentApp;
        int sectionNo;
        int lastSectionNo;

        if (!GetCommonSectionHeader(patBuf, patBufLength, 0, sectionLen,
                        sectionVer, currentApp, sectionNo, lastSectionNo)) {
           patBufLength = 0;
           sPatInfo.curSection = 0;
           return;
           }

        if (patBufLength < sectionLen)
           return;

        if (!currentApp || sPatInfo.patVersion == sectionVer ||
             (sectionNo && (sectionNo != sPatInfo.curSection ||
                        sPatInfo.readVersion != sectionVer)) ||
                       !SI::CRC32::isValid((const char *) patBuf, sectionLen)) {

           sPatInfo.curSection = 0;
           if (patBufLength > sectionLen && !patBuf[sectionLen]) {
              patBufLength -= sectionLen;
              memmove(patBuf,patBuf + sectionLen, patBufLength);
              continue;
              }

           patBufLength = 0;
           return;
           }

        sPatInfo.readVersion = sectionVer;

        if (!sectionNo) {
           sPatInfo.numPmtIds = 0;
           }

        int entryCount = (sectionLen - 12) >> 2;

        if (entryCount + sPatInfo.numPmtIds > MAXPMTIDS)
           entryCount = MAXPMTIDS - sPatInfo.numPmtIds;

        unsigned char *tmp = patBuf + 8;
        for (int i = 0; i < entryCount; i++) {
            if (tmp[i<<2]||tmp[(i<<2)+1]) {
               sPatInfo.pmtIds[sPatInfo.numPmtIds++] = ((tmp[(i<<2)+2]<<8)|tmp[(i<<2)+3])&0x1FFF;
               }
            }

        if (sectionNo == lastSectionNo) {
           sPatInfo.patVersion = sectionVer;
           sPatInfo.curSection = 0;
           int i;
           for (i = 0; i < sPatInfo.numPmtIds && sPatInfo.pmtIds[i] != curPmtId; i++)
               ;

           if (i == sPatInfo.numPmtIds) {
              curPmtId = -1;
              curPmtVersion = -1;
              pmtCCounter = -1;
              if (sPatInfo.numPmtIds == 1)
                 curPmtId = sPatInfo.pmtIds[0];
              }
           }
        else {
           sPatInfo.curSection = sectionNo + 1;
           }

        if (patBufLength > sectionLen && !patBuf[sectionLen]) {
           patBufLength -= sectionLen;
           memmove(patBuf,patBuf + sectionLen, patBufLength);
           continue;
           }

        patBufLength = 0;
        return;
        }

}

int c7x0TsReplayer::HandlePmtElemInfo(const uchar *Data, int Length,
      int &Vpid, int &Apid, int &Dpid, char *Alangs, char *Dlangs, int &Tpid)
{

  const int streamType = Data[0];
  const int elemPid = ((Data[1] << 8) | Data[2]) & 0x1FFF;
  const int elemInfoLen = (((Data[3] << 8) | Data[4]) & 0xFFF) + 5;

  if (elemInfoLen > Length) {
     esyslog("c7x0TsReplayer: Illegal elementary info length %d in PMT-Section", elemInfoLen);
     return -1;
     }

  Apid = Dpid = 0;
  Tpid = -1;

  char *curLangStr;
  bool AC3DescFound = false;
  switch (streamType) {
    case 1:
    case 2:
         Vpid = elemPid;
         return elemInfoLen;
    case 3:
    case 4:
         Apid = elemPid;
         curLangStr = Alangs;
         break;
    case 6:
         Dpid = elemPid;
         curLangStr = Dlangs;
         break;
    default:
         return elemInfoLen;
    }

  int offset = 5;
  int curLangCount = 0;

  while (offset < elemInfoLen) {
        const int descriptorTag = Data[offset++];
        const int descriptorLen = Data[offset++];

        if (descriptorLen + offset > elemInfoLen) {
           esyslog("c7x0TsReplayer: Illeagl descriptor length %d in PMT section", descriptorLen);
           break;
           }

        if (descriptorTag == 0x0A) { // Languages
           for (int i= 0; i < descriptorLen && curLangCount < 2; i += 4) {
               if (Data[offset + i] == '-')
                  continue;

               if (curLangCount > 0)
                  *curLangStr++ = '+';

               strn0cpy(curLangStr, I18nNormalizeLanguageCode(
                             (const char*) &Data[offset + i]), MAXLANGCODE1);

               curLangCount++;
               curLangStr += strlen(curLangStr);
               }
           }
        else if ((descriptorTag == 0x6A)||(descriptorTag == 0x7A)) { // AC3 Descriptor
           AC3DescFound = true;
           }
        else if (descriptorTag == 0x56) { // Teletext
           Tpid = Dpid;
           }

        offset += descriptorLen;
        }

  if (!AC3DescFound)
     Dpid = 0;
  return elemInfoLen;
}

void c7x0TsReplayer::HandlePmt(const uchar *Data)
{
  bool sectionStart;
  int payloadOffset;
  bool outOfSeq;

  if (!CheckTsHeader(Data, sectionStart, payloadOffset,
                                pmtCCounter, outOfSeq)) {
     pmtBufLength = 0;
     return;
     }

  if (payloadOffset >= 188)
     return;

  if (outOfSeq)
     pmtBufLength = 0;


  if (!CopySectionData(Data + payloadOffset, 188 - payloadOffset,
                              sectionStart, pmtBuf, pmtBufLength)) {
     pmtBufLength = 0;
     return;
     }

  while (true) {
        int sectionLen;
        int sectionVer;
        bool currentApp;
        int sectionNo;
        int lastSectionNo;

        if (!GetCommonSectionHeader(pmtBuf, pmtBufLength, 0x02, sectionLen,
                        sectionVer, currentApp, sectionNo, lastSectionNo)) {
           pmtBufLength = 0;
           return;
           }

        if (pmtBufLength < sectionLen)
           return;

        if (!currentApp || curPmtVersion == sectionVer ||
                 !SI::CRC32::isValid((const char *) pmtBuf, sectionLen)) {

           if (pmtBufLength > sectionLen && patBuf[sectionLen] == 0x02) {
              pmtBufLength -= sectionLen;
              memmove(pmtBuf, pmtBuf + sectionLen, pmtBufLength);
              continue;
              }

           pmtBufLength = 0;
           return;
           }

        const int PcrPid = ((pmtBuf[8] << 8) | pmtBuf[9]) & 0x1FFF;
        int VideoPid = 0;
        int AudioPids[MAXAPIDS + 1];
        int DolbyPids[MAXDPIDS + 1];
        char AudioLangs[MAXAPIDS + 1][MAXLANGCODE2];
        char DolbyLangs[MAXDPIDS + 1][MAXLANGCODE2];
        int NumAudioPids = 0;
        int NumDolbyPids = 0;
	int TeletextPid = -1;


        int offset = (((pmtBuf[10] << 8) | pmtBuf[11]) & 0xFFF) + 12;
        while (offset < sectionLen - 4) { // ES Info Loop
              int r = HandlePmtElemInfo(pmtBuf + offset, sectionLen - offset - 4,
                  VideoPid, AudioPids[NumAudioPids], DolbyPids[NumDolbyPids],
                          AudioLangs[NumAudioPids], DolbyLangs[NumDolbyPids], TeletextPid);

              if (r < 0) {
                 pmtBufLength = 0;
                 return;
                 }

              if (NumAudioPids < MAXAPIDS && AudioPids[NumAudioPids])
                 NumAudioPids++;

              if (NumDolbyPids < MAXDPIDS && DolbyPids[NumDolbyPids])
                 NumDolbyPids++;

              offset += r;
              }

        CheckAndSetupVideo(PcrPid, VideoPid, TeletextPid);
        dvbDevice->ClrAvailableTracks();
        for (int i = 0; i < NumAudioPids; i++)
            dvbDevice->SetAvailableTrack(ttAudio, i, AudioPids[i], AudioLangs[i]);

        if (Setup.UseDolbyDigital)
           for (int i = 0; i < NumDolbyPids; i++)
               dvbDevice->SetAvailableTrack(ttDolby, i, DolbyPids[i], DolbyLangs[i]);
        dvbDevice->EnsureAudioTrack();
        CheckAndSetupAudio(true);
        curPmtVersion = sectionVer;

        if (pmtBufLength > sectionLen && pmtBuf[sectionLen] == 0x02) {
           pmtBufLength -= sectionLen;
           memmove(pmtBuf, pmtBuf + sectionLen, pmtBufLength);
           continue;
           }

        pmtBufLength = 0;
        return;
        }
}


bool c7x0TsReplayer::ScanDataTrickspeed(const uchar *&Data, const uchar *Limit)
{
  const uchar *data = Data;
  const uchar *limit = Limit - 1;

  while (data < limit)
        if (data[0] > 1)
           data += 3;
        else if (!data[0])
           data++;
        else {
           if (!(data[-2] | data[-1])) {
              const uchar code = *++data;
              if (code == 0 || code == 0xB3 || code == 0xB8) {
                 Data = data;
                 return true;
                 }
              }
           data += 3;
           }

  return false;
}

bool c7x0TsReplayer::HandlePesTrickspeed(const uchar *&Data,
                                   const uchar *const Limit,
                                          bool payloadStart)
{
  if (payloadStart) {
     pesHeaderCompleted = false;
     pesHeaderLength = 0;
     pesPacketLen = 7; // Minimum
     }
  else if (pesPacketLen <= 0) {
     Data = Limit;
     return false;
     }

  if (pesHeaderCompleted) {
     if (pesHeaderLength >=4 && pesHeader[3] == 0xBE) { // Skip padding stream
        pesPacketLen -= Limit - Data;
        Data = Limit;
        }
     return true;
     }

  const uchar *data = Data;
  if (pesHeaderLength < 7) {
     int c = std::min (7 - pesHeaderLength, Limit - data);
     memcpy(pesHeader + pesHeaderLength,data, c);
     pesHeaderLength += c;
     data += c;
     if (pesHeaderLength < 7) {
        Data = data;
        return true;
        }
     }

  if ((pesHeader[0] | pesHeader[1] | (pesHeader[2] - 1)) ||
        (pesHeader[3] != 0xBE && (pesHeader[3] & 0xF0) != 0xE0)) {
     esyslog("c7x0TsReplayer: Illegal PES-Packet in Video-Stream for trick speed");
     pesHeaderLength = 0;
     pesPacketLen = 0;
     pesHeaderCompleted = true;
     Data = Limit;
     return false;
     }

  pesPacketLen = *((uint16_t *) (pesHeader + 4));

  // Packet Length not bound ? (only allow in TS Video Packets)
  // Define a maximum;
  if (!pesPacketLen)
        pesPacketLen = (1<<24);

  pesPacketLen += 6;

  if (pesHeader[3] == 0xBE) {
     pesPacketLen -= Limit - data;
     Data = Limit;
     pesHeaderCompleted = true;
     return true;
     }

  if (data >= Limit) {
     Data = Limit;
     return true;
     }

  if ((pesHeader[6] & 0xC0) == 0x80) { // MPEG2
     if (pesHeaderLength < 9) {
        int c = 9 - pesHeaderLength;

        if (Limit - data >= c) {
           c += data[c-1];
           if (Limit - data < c)
              c = Limit - data;
           }
        else
           c = Limit - data;

        memcpy(pesHeader + pesHeaderLength, data, c);
        pesHeaderLength += c;
        data += c;

        if (pesHeaderLength < 9) {
           Data = data;
           return true;
           }
        }

     const int headerLen = 9 + pesHeader[8];
     if (pesHeaderLength < headerLen) {
        if (data >= Limit) {
           Data = Limit;
           return true;
           }

        int c = std::min(headerLen - pesHeaderLength, Limit - data);
        memcpy (pesHeader + pesHeaderLength, data, c);
        pesHeaderLength += c;
        data += c;

        if (pesHeaderLength < headerLen) {
           Data = data;
           return true;
           }
        }

     pesHeaderCompleted = true;
     pesPacketLen -= pesHeaderLength;
     }
  else { // MPEG1
     // Stuffing Bytes
    int start = 6;
    while (start < pesHeaderLength && pesHeader[start] == 0xFF)
          start++;

    if (start == pesHeaderLength) {
       while (data < Limit && data[0] == 0xFF) {
             pesHeader[pesHeaderLength++] = *data++;
             start++;
             }

       if (data >= Limit) {
          Data = Limit;
          return true;
          }

       pesHeader[pesHeaderLength++] = *data++;
       }

    if ((pesHeader[start] & 0xC0) == 0x40) {
       start += 2;

       while (start >= pesHeaderLength && data < Limit)
                 pesHeader[pesHeaderLength++] = *data++;

       if (data >= Limit) {
          Data = Limit;
          return true;
          }
       }

    if ((pesHeader[start] & 0xF0) == 0x20) {
       start += 5;
       }
    else if ((pesHeader[start] & 0xF0) == 0x30) {
       start += 10;
       }
    else {
       start++;
       }

    int c = std::min(start - pesHeaderLength, Limit - data);
    if (c > 0) {
       memcpy(pesHeader + pesHeaderLength, data, c);
       pesHeaderLength += c;
       data += c;
       }

    if (pesHeaderLength < start) {
       Data = data;
       return true;
       }

    pesHeaderCompleted = true;
    pesPacketLen -= pesHeaderLength;
    }

  return true;
}

int c7x0TsReplayer::HandleTrickspeed(const uchar *Data)
{
  bool payloadStart;
  int tsPayloadOffset;
  bool outOfSeq;

  if (!CheckTsHeader(Data, payloadStart, tsPayloadOffset,
                            trickspeedCCounter, outOfSeq)){
     trickspeedLen = 0;
     trickspeedState = syncing;
     trickspeedScanner = 0xFFFFFFFF;
     pesPacketLen = 0;
     return 0;
     }

  if (tsPayloadOffset >= 188)
     return 0;

  if (outOfSeq) {
     // Data has errors -> discard last (in most cases partial) frame
     trickspeedLen = 0;
     trickspeedState = syncing;
     trickspeedScanner = 0xFFFFFFFF;
     pesPacketLen = 0;
     }

   // Always a full packet should be present in Data.
  const uchar *data = Data + tsPayloadOffset;
  const uchar *limit = Data + 188;

  if (!HandlePesTrickspeed(data, limit, payloadStart)) {
     trickspeedLen = 0;
     trickspeedState = syncing;
     trickspeedScanner = 0xFFFFFFFF;
     return 0;
     }

   if (data >= limit) {
      return 0;
      }

   pesPacketLen -= limit - data;

  // Driver expects an elementary video stream written to /dev/ost/video0
  // while in trickspeed.
  // At this point we would normally sync to a sequence header and
  // write out the data from data until limit. Pes-header is stripped up
  // above, the rest is video elementary data.
  // But hey as always there are a number of bugs in this part of the driver:
  // 1. Don't like writing startcodes aligned to start of buffer
  //   (isn't it funny - this is the easiest case)
  // 2. Don't like startcode a spilted over writes.
  // There are some others which I not yet was able to trace down.
  // Some parts of the video get replayed even if we are miles away for that
  // point. Seems so as if some frames get not played right.
  // The part below tries to workaround these issues.
  // As always let me write a hint:
  // IF YOU ARE IN CHARGE OF THE DRIVER SOURCE FIX THIS HORRIBLE BROKEN THING

  const uchar *data_save = data;
  const uchar *payload_start = data;

  for (int i = 0; i < 3 && data < limit; i++) {
      int code = data[0];
      if ((trickspeedScanner & 0xFFFFFF) == 0x000001) {
         if (trickspeedState == syncing && code == 0xB3) {
            putIntUnalignedBE(trickspeedData, 0x00000000); // First two 0: Padding for bug in driver
            trickspeedData[4] = 1;
            trickspeedLen = 5;
            payload_start = data;
            data += 3 - i;
            trickspeedState = findFrame;
            break;
            }
         if (trickspeedState == findFrame && !code) {
            trickspeedState = scanFrame;
            data += 3 - i;
            break;
            }
         if (trickspeedState == scanFrame && (!code || code == 0xB3 || code == 0xB8)) {
            trickspeedLen -= 3 - i;
            int r;
            if ((r = write(dvbDevice->fd_video, trickspeedData, trickspeedLen)) < 0) {
               LOG_ERROR;
               return r;
               }
            if (r != trickspeedLen) {
               esyslog("ERROR: Write of trickspeed data stripped off %d bytes",trickspeedLen - r);
               }

            putIntUnalignedBE(trickspeedData, 0x00000000); // First two 0: Padding for bug in driver
            trickspeedData[4] = 1;
            trickspeedLen = 5;
            payload_start = data;
            data += 3 - i;
            if (code) {
               trickspeedState = findFrame;
               }
            if (inFastplay && code != 0xB3) {
               trickspeedState = syncing;
               trickspeedLen = 0;
               }
            break;
            }
         }
      trickspeedScanner = (trickspeedScanner << 8) | code;
      data++;
      }

  data--;

  while (ScanDataTrickspeed(data, limit)) {
        int code = data[0];
        if (trickspeedState == scanFrame) {
           int n = data - 3 - payload_start;
           if (n) {
              if (!ReallocTrickspeedBuffer(n + trickspeedLen)) {
                 errno = ENOMEM;
                 return -1;
                 }
              memcpy(trickspeedData + trickspeedLen, payload_start, n);
              trickspeedLen += n;
              }
           int r;
           if ((r = write(dvbDevice->fd_video, trickspeedData, trickspeedLen)) < 0) {
              LOG_ERROR;
              return r;
              }
           if (r != trickspeedLen) {
              esyslog("ERROR: Write of trickspeed data stripped off %d bytes",trickspeedLen - r);
              }

           trickspeedData[0] = 0; // Padding for bug in driver
           trickspeedData[1] = 0; // Padding for bug in driver
           trickspeedLen = 2;
           payload_start = data - 3;
           data += 3;
           if (code) {
              trickspeedState = findFrame;
              }
           if (inFastplay && code != 0xB3) {
              trickspeedState = syncing;
              trickspeedLen = 0;
              }
           }
        else if (trickspeedState == findFrame && !code) {
           trickspeedState = scanFrame;
           data += 3;
           }
        else if (trickspeedState == syncing && code == 0xB3) {
           trickspeedData[0] = 0; // Padding for bug in driver
           trickspeedData[1] = 0; // Padding for bug in driver
           trickspeedLen = 2;
           payload_start = data - 3;
           data += 3;
           trickspeedState = findFrame;
           }
        }

  int n = std::min (4, limit - data_save);
  memcpy(&trickspeedScanner, limit - n, n);

  if (trickspeedState == syncing){
     unusableDataCount += limit - Data;
     return 0;
     }

  unusableDataCount = 0;
  n = limit - payload_start;
  if (n) {
     if (!ReallocTrickspeedBuffer(n + trickspeedLen)) {
        errno = ENOMEM;
        return -1;
        }
     memcpy(trickspeedData + trickspeedLen, payload_start, n);
     trickspeedLen += n;
     }
  return 0;
}

#ifdef USE_PLAYER_WATCHDOG
bool c7x0TsReplayer::CheckTimeout(void)
{
  time_t now = time(NULL);

  struct video_status videoStat;
  if (ioctl(dvbDevice->fd_video, VIDEO_GET_STATUS, &videoStat) >= 0 && videoStat.play_state == VIDEO_PLAYING)
     lastPlaytime = now;
  else if (now - lastPlaytime > REPLAY_TIMEOUT) {
     esyslog("ERROR: Playing timed out!");
     dsyslog("DEBUG: Video ID 0x%02X Audio ID 0x%02d", videoStreamId, curAudioStreamId);

     errno = EDEADLK;
     return true;
     }
  return false;
}
#endif

int c7x0TsReplayer::WriteOutPacket(const uchar *Data, int Count)
{

  int r;

  while (Count) {
        if ((r = write(fd_dvr,Data,Count)) < 0) {
           if ((errno == EBUSY) | (errno == EAGAIN)) {
              cCondWait::SleepMs(3);
              continue;
              }
           LOG_ERROR;
           return r;
           }
        Count -= r;
        Data += r;
        }

#ifdef USE_PLAYER_WATCHDOG
  if (CheckTimeout())
     return -1;
#endif
  return 0;
}

int c7x0TsReplayer::CheckPmtIds(const int Pid)
{

  for (int i = 0; i < sPatInfo.numPmtIds ; i++)
      if (sPatInfo.pmtIds[i] == Pid)
         return Pid;

  return -1;
}

int c7x0TsReplayer::PlayTs(const uchar *Data, int Length)
{
  if (!Data) {
     dsyslog("m7x0 Replayer: Discarding last Ts-Packet");
     fragmentLen = 0;
     return 0;
     }

  if (!inTrickspeed)
     OpenDvr();

  const uchar *data = Data;
  const uchar *const limit = Data + Length;
  int streamId;
  int writeOutCount = 0;
  const uchar *writeOutStart = NULL;

  while (data < limit) {
        // dsyslog
        if (fragmentLen == 188) {
           int r = WriteOutPacket(fragmentData,188);
           if (r < 0) {
	       LOG_ERROR;
               return r;
               }
           writeOutCount -= 188;
           fragmentLen = 0;
           }

        if ((streamId = ScanDataForPacketStartCode(data,limit)) == -1) {
           if (skippedBytes > REPLAY_MAX_UNUSABLE_DATA) {
              esyslog("ERROR: %d bytes of recording unusable while in initialization - giving up!", unusableDataCount);
              errno = EDEADLK; // Any ideas for a better errorcode
              return -1;
              }

           break;
           }

        if (skippedBytes) {
           if (writeOutCount) {
              int r = WriteOutPacket(writeOutStart,writeOutCount);
              if (r < 0) {
		 LOG_ERROR;
                 return r;
                 }
              writeOutCount = 0;
              }
           esyslog("ERROR: c7x0TsReplayer skipped %d bytes while syncing on Packet",skippedBytes);
           unusableDataCount += skippedBytes;
           skippedBytes = 0;
           }

        if (!writeOutCount)
           writeOutStart = data;

        if (!streamId) {
           if (writeOutCount) {
              int r = WriteOutPacket(writeOutStart,writeOutCount);
              if (r < 0) {
		 LOG_ERROR;
                 return r;
                 }
              writeOutCount = 0;
              }

           unusableDataCount = 0;
           if (fragmentLen) {
              HandlePat(fragmentData);
              fragmentLen = 0;
              }
           else {
              HandlePat(data);
              data += 188;
              }
           continue;
           }

        if (curPmtId == -1) {
           curPmtId = CheckPmtIds(streamId);
           curPmtVersion = -1;
           pmtCCounter = -1;
           }

        if (curPmtId == streamId) {
           if (writeOutCount) {
              int r = WriteOutPacket(writeOutStart,writeOutCount);
              if (r < 0) {
		 LOG_ERROR;
                 return r;
                 }
              writeOutCount = 0;
              }
           unusableDataCount = 0;

           if (fragmentLen) {
              HandlePmt(fragmentData);
              fragmentLen = 0;
              }
           else {
              HandlePmt(data);
              data += 188;
              }

           continue;
           }

        if (inTrickspeed) {
           writeOutCount = 0;
           if (streamId == videoStreamId) {
              unusableDataCount = 0;
              int r;
              if (fragmentLen) {
                 r = HandleTrickspeed(fragmentData);
                 fragmentLen = 0;
                 }
              else {
                 r = HandleTrickspeed(data);
                 data += 188;
                 }
              if (r < 0) {
                 return r;
                 }
              continue;
              }

           if (fragmentLen)
              fragmentLen = 0;
           else
              data += 188;

           unusableDataCount += 188;
           if (unusableDataCount > REPLAY_MAX_UNUSABLE_DATA) {
              esyslog("ERROR: %d bytes of recording unusable while in trickspeed - giving up!", unusableDataCount);
              errno = EDEADLK; // Any ideas for a better errorcode
              return -1;
              }
           continue;
           }

        if (streamId == videoStreamId || streamId == pcrStreamId || streamId == teletextStreamId) {
           if (!fragmentLen)
              data += 188;
           writeOutCount += 188;
           unusableDataCount = 0;
           continue;
           }

        CheckAndSetupAudio();
        if (streamId == curAudioStreamId) {
           if (!fragmentLen)
              data += 188;
           writeOutCount += 188;
           unusableDataCount = 0;
           continue;
           }

        if (writeOutCount) {
           int r = WriteOutPacket(writeOutStart,writeOutCount);
           if (r < 0) {
	      LOG_ERROR;
              return r;
              }
           writeOutCount = 0;
           }

        unusableDataCount += 188;
        if (unusableDataCount > REPLAY_MAX_UNUSABLE_DATA) {
           esyslog("ERROR: %d bytes of recording unusable - giving up!", unusableDataCount);
           errno = EDEADLK; // Any ideas for a better errorcode
           return -1;
           }

        if (!fragmentLen)
           data += 188;
        fragmentLen = 0;
        }

  if (writeOutCount) {
     int r = WriteOutPacket(writeOutStart,writeOutCount);
     if (r < 0) {
	LOG_ERROR;
        return r;
        }
     }
  return Length;
}

void c7x0TsReplayer::TrickSpeed(const int Speed,const bool UseFastForward)
{
  dsyslog("TrickSpeed called Speed %d %d!",Speed, UseFastForward);
  if (dvbDevice->playMode != pmTsAudioVideo && dvbDevice->playMode != pmTsVideoOnly)
     return;
  // Uglly needed otherwise continue won't work
  // Yet another bug in driver!?!
  CHECK(ioctl(dvbDevice->fd_audio, AUDIO_SET_MUTE, true));
  CHECK(ioctl(dvbDevice->fd_audio, AUDIO_STOP,0));
  CHECK(ioctl(dvbDevice->fd_video, VIDEO_STOP,0));
  CloseDvr();

  if (!inTrickspeed || !UseFastForward || (!inFastplay && UseFastForward)) {
     trickspeedState = syncing;
     trickspeedScanner = 0xFFFFFFFF;
     trickspeedLen = 0;
     trickspeedCCounter = -1;
     pesHeaderLength = 0;
     pesPacketLen = 0;
     pesHeaderCompleted = false;
     }
  if (Speed > 1)
     CHECK(ioctl(dvbDevice->fd_video, VIDEO_SLOWMOTION, Speed << 1))
  else
     CHECK(ioctl(dvbDevice->fd_video, VIDEO_FAST_FORWARD, 1))

  inTrickspeed = true;
  inFastplay = UseFastForward;
}

void c7x0TsReplayer::Play(void)
{
  dsyslog("Play called!");
  //CHECK(ioctl(dvbDevice->fd_audio, AUDIO_STOP,0));
  //CHECK(ioctl(dvbDevice->fd_video, VIDEO_STOP,0));
  //CloseDvr();
  inTrickspeed = false;
  //OpenDvr();
  CHECK(ioctl(dvbDevice->fd_audio, AUDIO_SET_MUTE, false));
  if (dvbDevice->playMode != pmTsAudioVideo && dvbDevice->playMode != pmTsVideoOnly)
     CHECK(ioctl(dvbDevice->fd_audio, AUDIO_CONTINUE, 0));

}

void c7x0TsReplayer::Clear(void)
{
  dsyslog("Clear called!");
  // CHECK(ioctl(dvbDevice->fd_audio, AUDIO_SET_MUTE, true));
  // CHECK(ioctl(dvbDevice->fd_audio, AUDIO_STOP,0));
  // CHECK(ioctl(dvbDevice->fd_video, VIDEO_STOP,0));
  CloseDvr();
  /* if (!inTrickspeed) {
     dsyslog("Clear called2!");
     OpenDvr();
     CHECK(ioctl(dvbDevice->fd_audio, AUDIO_SET_MUTE, false));
     }*/

}

void c7x0TsReplayer::Freeze(void)
{
  dsyslog("Freeze called!");
  if (dvbDevice->playMode == pmTsAudioVideo || dvbDevice->playMode == pmTsVideoOnly) {
     CloseDvr();
     CHECK(ioctl(dvbDevice->fd_video, VIDEO_STOP,0));
     CHECK(ioctl(dvbDevice->fd_audio, AUDIO_SET_MUTE, true));
     if (dvbDevice->playMode == pmTsAudioVideo)
        CHECK(ioctl(dvbDevice->fd_audio, AUDIO_STOP,0));
     }
  else
     CHECK(ioctl(dvbDevice->fd_audio, AUDIO_PAUSE, 0));
}

bool c7x0TsReplayer::Poll(cPoller &Poller,const int TimeoutMs)
{

  return true;
}

bool c7x0TsReplayer::Flush(const int TimeoutMs)
{
  dsyslog("Flush called!");
  return true;
}

void c7x0TsReplayer::SetPids(int PmtPid, int VideoPid)
{
  if (PmtPid && curPmtId != PmtPid) {
     curPmtId = PmtPid;
     pmtCCounter = -1;
     curPmtVersion = -1;
     }

  CheckAndSetupVideo(-1, VideoPid, -1);
  if (inTrickspeed && VideoPid > 0 && videoStreamId != VideoPid) {
     videoStreamId = VideoPid;
     trickspeedState = syncing;
     trickspeedScanner = 0xFFFFFFFF;
     trickspeedLen = 0;
     trickspeedCCounter = -1;
     pesHeaderLength = 0;
     pesPacketLen = 0;
     pesHeaderCompleted = false;
     }
}
#endif

// --- c7x0TSBuffer ----------------------------------------------------------
// This class is need cause m7x0 does not support poll()/select() calls on
// dvr-devices. The calls just returns immediatelly with POLLIN set.
// This is a ugly workaround, but I can't do anything about this.
// If someone who has access to the driver-source read this:
// FIX THIS HORRIBLE BROKEN DRIVERS !

#define DVR_RING_BUFFER_SIZE (1536*1024)
#define DVR_READ_TIMEOUT 2000 // ms
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
#define DVR_READ_RETRY 100 // ms
#else
#define DVR_READ_RETRY 300 // ms
#endif
#define DVR_MAX_EVENTS (DVR_RING_BUFFER_SIZE/TS_SIZE + 1)

class c7x0TSBuffer {
private:
  int maxFill;
  int cardIndex;
  int f;
  bool ok;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  // Frame Event
  struct eventBufferItem {
         enum eTsVideoFrame eventFrameType;
         uint32_t packetCount;
         uint32_t lostbytes;
         };

  eventBufferItem eventBuffer[DVR_MAX_EVENTS];
  int nextFreeEventBuffer;
  int firstBufferedEvent;

  enum eTsVideoFrame curEventFrameType;
  int curEventOffset;
  bool curEventAvail;
  bool eventsUsed;
  uint64_t readBytes;
#endif

  // Ringbuffer
  uchar *dvrRingBuffer;
  uchar ringBufferLeft[TS_SIZE];
  int ringBufferLeftLength;
  dvr_ring_buffer_state bufferState;

  void updateStats() { maxFill = std::max (maxFill, bufferState.fill); }
public:
  c7x0TSBuffer(int File, int CardIndex);
  ~c7x0TSBuffer();
  bool Okay(void) const { return ok; }
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  uchar *Get(int &Length, int &Pid,eTsVideoFrame &videoFrame);
  void UseFrameEvents(bool On);
#else
  uchar *Get(int &Length, int &Pid);
#endif

  };

#ifdef USE_HW_VIDEO_FRAME_EVENTS
void c7x0TSBuffer::UseFrameEvents(bool On)
{
  if (On == eventsUsed)
     return;

  if (On) {
     // Discard stalling events
     dvrEvent hwEvent;
     while (ioctl(f, DVR_GET_EVENT, &hwEvent) > 0)
           ;
     }
  nextFreeEventBuffer = 0;
  firstBufferedEvent = 0;
  eventsUsed = On;
}
#endif

c7x0TSBuffer::c7x0TSBuffer(int File, int CardIndex)
{
  maxFill = 0;
  f = File;
  cardIndex = CardIndex;
  ok = true;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  eventsUsed= false;

  readBytes = 0;
  curEventAvail = false;
  nextFreeEventBuffer = 0;
  firstBufferedEvent = 0;
#endif

  ringBufferLeftLength =0 ;
  memset(&bufferState,0,sizeof(struct dvr_ring_buffer_state));
  if (f < 0) {
     ok = false;
     return;
     }
  if ((dvrRingBuffer = (uchar *) mmap(NULL, DVR_RING_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0)) == MAP_FAILED) {
     LOG_ERROR;
     ok = false;
     }

}

c7x0TSBuffer::~c7x0TSBuffer()
{
  int lostbytes=0;
  if (!ok)
     return;
  CHECK(ioctl(f,DVR_GET_BYTESLOST,&lostbytes));
  munmap(dvrRingBuffer,DVR_RING_BUFFER_SIZE);
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  isyslog("M7X0 TS-Buffer on device %d has lost %d of %"PRId64" Bytes during Recording. Buffer Stats %d Bytes (%d%%)", cardIndex, lostbytes, readBytes + lostbytes, maxFill, (maxFill*100)/DVR_RING_BUFFER_SIZE);
#else
  isyslog("M7X0 TS-Buffer on device %d has lost %d during Recording. Buffer Stats %d Bytes (%d%%)", cardIndex, lostbytes, maxFill, (maxFill*100)/DVR_RING_BUFFER_SIZE);
#endif
}

#ifdef USE_HW_VIDEO_FRAME_EVENTS
uchar *c7x0TSBuffer::Get(int &Length,int &Pid, eTsVideoFrame &videoFrame)
#else
uchar *c7x0TSBuffer::Get(int &Length,int &Pid)
#endif
{
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  videoFrame = tsVideoFrameUnknown;
#endif
  if (!bufferState.readable) {
     int TimeoutMs=0;
     while (ioctl(f, DVR_GET_RING_BUFFER_STATE, &bufferState) != 0 && TimeoutMs < DVR_READ_TIMEOUT) {
           cCondWait::SleepMs(DVR_READ_RETRY);
           TimeoutMs += DVR_READ_RETRY;
           }

     if (!bufferState.readable)
        return NULL;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
     if (eventsUsed) {
        int r = 0;
        dvrEvent hwEvent;
        while ((r = ioctl(f, DVR_GET_EVENT, &hwEvent)) > 0) {

              CHECK(ioctl(f,DVR_GET_BYTESLOST,&eventBuffer[nextFreeEventBuffer].lostbytes));
              eventBuffer[nextFreeEventBuffer].packetCount=hwEvent.packet_count;

              if (hwEvent.trigger & (HW_EVENT_SEQUENCE_HEADER | HW_EVENT_IFRAME))
                 eventBuffer[nextFreeEventBuffer].eventFrameType = tsVideoFrameI;
              else if (hwEvent.trigger & HW_EVENT_PFRAME)
                 eventBuffer[nextFreeEventBuffer].eventFrameType = tsVideoFrameP;
              else if (hwEvent.trigger & HW_EVENT_BFRAME)
                 eventBuffer[nextFreeEventBuffer].eventFrameType = tsVideoFrameB;
              else {
                 esyslog("ERROR: Unknown Frame Event (Pid: 0x%x Trigger 0x%x Count 0x%x)!",hwEvent.source, hwEvent.trigger, hwEvent.packet_count);
                 continue;
                 }

              // dsyslog("DEBUG: Frame Event (Pid: 0x%x Trigger 0x%x Count 0x%x)!",hwEvent.source, hwEvent.trigger, hwEvent.packet_count);
              nextFreeEventBuffer++;
              if (nextFreeEventBuffer != DVR_MAX_EVENTS) {
                 if (nextFreeEventBuffer == firstBufferedEvent) {
                    esyslog("ERROR: Event buffer overflowed!");
                    nextFreeEventBuffer--;
                    }
                 }
              else {
                 if (firstBufferedEvent)
                    nextFreeEventBuffer = 0;
                 else {
                    esyslog("ERROR: Event buffer overflowed!");
                    nextFreeEventBuffer--;
                    }
                 }
              }

        if (r < 0)
           esyslog("ERROR: Read Event failed with %d (errno %d)",r, errno);
        }
#endif
     updateStats();
     }

  if (!ringBufferLeftLength && dvrRingBuffer[bufferState.offset] != TS_SYNC_BYTE) {
     int i;
     for (i = 1; i < bufferState.readable &&
                 dvrRingBuffer[bufferState.offset + i] != TS_SYNC_BYTE ; i++)
         ;

     bufferState.offset += i;
     bufferState.readable -= i;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
     readBytes += i;
     if (curEventAvail) {
        curEventOffset -= i;
        curEventAvail = (curEventOffset < 0);
        }
#endif
     esyslog("ERROR: skipped %d bytes to sync on TS packet on device %d", i, cardIndex);
     }

  // Frame Events are buggy by design. How should one know if the lost bytes are
  // befor or behind an event.
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  while (eventsUsed && !curEventAvail && firstBufferedEvent != nextFreeEventBuffer) {
        curEventOffset = (int) (eventBuffer[firstBufferedEvent].packetCount * 188ULL - readBytes - eventBuffer[firstBufferedEvent].lostbytes);

        curEventFrameType = eventBuffer[firstBufferedEvent].eventFrameType;

        firstBufferedEvent++;
        if (firstBufferedEvent == DVR_MAX_EVENTS)
           firstBufferedEvent = 0;

        if (curEventOffset >= 0) {
           curEventAvail = true;
           break;
           }

        esyslog ("ERROR: Event Offset < 0");
        }
#endif
#if 0
  dvrEvent hwEvent;
  while (eventsUsed && !eventAvail && ioctl(f, DVR_GET_EVENT, &hwEvent) == 1) {
        int lostbytes;
        CHECK(ioctl(f,DVR_GET_BYTESLOST,&lostbytes));
        eventOffset = (int) (hwEvent.packet_count * 188ULL - readBytes - lostbytes);

        eventAvail = (eventOffset >= 0); // Just in case, if syncing skips an event

        if (hwEvent.trigger & (HW_EVENT_SEQUENCE_HEADER | HW_EVENT_IFRAME))
           eventFrameType = tsVideoFrameI;
        else if (hwEvent.trigger & HW_EVENT_PFRAME)
           eventFrameType = tsVideoFrameP;
        else if (hwEvent.trigger & HW_EVENT_BFRAME)
           eventFrameType = tsVideoFrameB;
        else {
           esyslog("ERROR: Unknown Frame Event (Pid: 0x%x Trigger 0x%x Count 0x%x)!",hwEvent.source, hwEvent.trigger, hwEvent.packet_count);
           eventAvail = false;
           }
         dsyslog("DEBUG: Frame Event (Pid: 0x%x Trigger 0x%x Count 0x%x)!",hwEvent.source, hwEvent.trigger, hwEvent.packet_count);
        }

#endif
  if (ringBufferLeftLength || bufferState.readable < TS_SIZE) {
     int copy = std::min(TS_SIZE - ringBufferLeftLength, bufferState.readable);
     memcpy(ringBufferLeft+ringBufferLeftLength, dvrRingBuffer + bufferState.offset,copy);

     ringBufferLeftLength += copy;
     bufferState.offset += copy;
     bufferState.readable -= copy;

     if (ringBufferLeftLength == TS_SIZE) {
        ringBufferLeftLength = 0;
        Length = TS_SIZE;
        Pid =   (( *( (int *) ringBufferLeft)) >> 8) & 0x1fff;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
        if (curEventAvail) {
           if (curEventOffset >= TS_SIZE) {
              curEventOffset -= TS_SIZE;
              }
           else {
              if (curEventOffset != 0) {
                 esyslog("ERROR: Frame Event not aligned to TS-Pack bounds (read %"PRId64" diff %d)", readBytes, curEventOffset);
                 }
              videoFrame = curEventFrameType;
              curEventAvail = false;
              }
           }
        readBytes += TS_SIZE;
#endif
        return ringBufferLeft;
        }

     return NULL;
     }



  // Checks for same PID and TS_SYNC_BYTE
  uchar *p = dvrRingBuffer + bufferState.offset;

  uint32_t pid = getIntUnaligned(p)&0xff1fff00;
  int l;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  const int maxreadable = curEventAvail? std::min(curEventOffset - 1, bufferState.readable - TS_SIZE): bufferState.readable - TS_SIZE;
#else
  const int maxreadable =  bufferState.readable - TS_SIZE;
#endif

  for (l = TS_SIZE ; l <= maxreadable &&
        (get_unaligned((uint32_t *)(p+l))&0xff1fff00) == pid; l += TS_SIZE)
      ;

  bufferState.offset += l;
  bufferState.readable -= l;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (curEventAvail) {
     if (curEventOffset >= l) {
        curEventOffset -= l;
        }
     else {
        if (curEventOffset != 0) {
           esyslog("ERROR: Frame Event not aligned to TS-Pack bounds (read %"PRId64" diff %d)", readBytes, curEventOffset);
           }
        videoFrame = curEventFrameType;
        curEventAvail = false;
        }
     }
  readBytes += l;
#endif

  Length = l;
  Pid=(pid>>8)&0x1fff;
  return p;
}


//M7X0 END AK

// --- cDvbDevice ------------------------------------------------------------
int cDvbDevice::devVideoOffset = -1;
//M7X0 BEGIN AK
int cDvbDevice::setTransferModeForDolbyDigital = 3;
//M7X0 END AK
cDvbDevice::cDvbDevice(int n)
{
//M7X0 BEGIN AK
  audioChannel = 0;
  fd_playDvr = -1;
  fd_playDemux[0] = -1;
  fd_playDemux[1] = -1;
#ifndef TS_PLAYER_BACKPORT
  tsreplayer = NULL;
#endif
//M7X0 END AK
  dvbTuner = NULL;
  frontendType = fe_type_t(-1); // don't know how else to initialize this - there is no FE_UNKNOWN
  spuDecoder = NULL;
  digitalAudio = false;
  playMode = pmNone;

  // Devices that are present on all card types:

  int fd_frontend = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_FRONTEND, n, O_RDWR | O_NONBLOCK, true);

  // Devices that are only present on cards with decoders:
//M7X0 BEGIN AK
  fd_osd       = DvbOpen(DEV_DVB_OSD, 0, O_RDWR, true);
  fd_video     = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_VIDEO, n, O_RDWR | O_NONBLOCK, true);
  fd_video_v4l = DvbOpen(DEV_VIDEO, n, O_RDWR, true);
  fd_audio     = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_AUDIO, n, O_RDWR | O_NONBLOCK, true);
  // fd_stc      = DvbOpen(DEV_DVB_DEMUX,  n, O_RDWR);
  fd_stc       = -1; // STC doesn't work so don't open a device
//M7X0 END AK
  // The DVR device (will be opened and closed as needed):

  fd_dvr = -1;

  // The offset of the /dev/video devices:

  if (devVideoOffset < 0) { // the first one checks this
     FILE *f = NULL;
     char buffer[PATH_MAX];
     for (int ofs = 0; ofs < 100; ofs++) {
         snprintf(buffer, sizeof(buffer), "/proc/video/dev/video%d", ofs);
         if ((f = fopen(buffer, "r")) != NULL) {
            if (fgets(buffer, sizeof(buffer), f)) {
               if (strstr(buffer, "DVB Board")) { // found the _first_ DVB card
                  devVideoOffset = ofs;
                  dsyslog("video device offset is %d", devVideoOffset);
                  break;
                  }
               }
            else
               break;
            fclose(f);
            }
         else
            break;
         }
     if (devVideoOffset < 0)
        devVideoOffset = 0;
     if (f)
        fclose(f);
     }
  devVideoIndex = (devVideoOffset >= 0 && HasDecoder()) ? devVideoOffset++ : -1;

  if(Setup.VideoWithoutSignal)
     SetVideoFormat(eVideoFormat(Setup.VideoFormat));

  // We only check the devices that must be present - the others will be checked before accessing them://XXX

  if (fd_frontend >= 0) {
     dvb_frontend_info feinfo;
     if (ioctl(fd_frontend, FE_GET_INFO, &feinfo) >= 0) {
        frontendType = feinfo.type;
        ciHandler = cCiHandler::CreateCiHandler(*cDvbName(DEV_DVB_ADAPTER DEV_DVB_CA, n));
        dvbTuner = new cDvbTuner(fd_frontend, CardIndex(), frontendType, ciHandler);
        }
     else
        LOG_ERROR;
     }
  else
     esyslog("ERROR: can't open DVB device %d", n);

  StartSectionHandler();
}

cDvbDevice::~cDvbDevice()
{
//M7X0 BEGIN AK
#ifndef TS_PLAYER_BACKPORT
  if (tsreplayer != NULL)
     delete tsreplayer;
#endif
//M7X0 END AK
  StopSectionHandler();
  delete spuDecoder;
  delete dvbTuner;
  // We're not explicitly closing any device files here, since this sometimes
  // caused segfaults. Besides, the program is about to terminate anyway...
}

bool cDvbDevice::Probe(const char *FileName)
{
  if (access(FileName, F_OK) == 0) {
     int f = open(FileName, O_RDONLY);
     if (f >= 0) {
        close(f);
        return true;
        }
     else if (errno != ENODEV && errno != EINVAL)
        LOG_ERROR_STR(FileName);
     }
  else if (errno != ENOENT)
     LOG_ERROR_STR(FileName);
  return false;
}

bool cDvbDevice::Initialize(void)
{
  int found = 0;
  int i;
  for (i = 0; i < MAXDVBDEVICES; i++) {
      if (UseDevice(NextCardIndex())) {
         if (Probe(*cDvbName(DEV_DVB_ADAPTER DEV_DVB_FRONTEND, i))) {
            new cDvbDevice(i);
            found++;
            }
         else
            break;
         }
      else
         NextCardIndex(1); // skips this one
      }
  NextCardIndex(MAXDVBDEVICES - i); // skips the rest
  if (found > 0)
     isyslog("found %d video device%s", found, found > 1 ? "s" : "");
  else
     isyslog("no DVB device found");
  return found > 0;
}
//M7X0 BEGIN AK
void cDvbDevice::MakePrimaryDevice(bool On)
{
  if (HasDecoder()&&On&&!cOsdProvider::Available())
     new cDvbOsdProvider(fd_osd);

  if (!On) {
     TurnOffLiveMode(true);
     // Urgly workaround. But seems to be need in case of Primary-Switch
     // Maybe we find a better way.
     close(fd_video);
     close(fd_audio);
     fd_video    = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_VIDEO,  CardIndex(), O_RDWR | O_NONBLOCK);
     fd_audio    = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_AUDIO,  CardIndex(), O_RDWR | O_NONBLOCK);
     }
}
//M7X0 END AK

bool cDvbDevice::HasDecoder(void) const
{
  return fd_video >= 0 && fd_audio >= 0;
}

bool cDvbDevice::Ready(void)
{
  if (ciHandler) {
     ciHandler->Process();
     return ciHandler->Ready();
     }
  return true;
}

int cDvbDevice::ProvidesCa(const cChannel *Channel) const
{
  int NumCams = 0;
  if (ciHandler) {
     NumCams = ciHandler->NumCams();
     if (Channel->Ca() >= CA_ENCRYPTED_MIN) {
        unsigned short ids[MAXCAIDS + 1];
        for (int i = 0; i <= MAXCAIDS; i++) // '<=' copies the terminating 0!
            ids[i] = Channel->Ca(i);
        if (ciHandler->ProvidesCa(ids))
           return NumCams + 1;
        }
     }
  int result = cDevice::ProvidesCa(Channel);
  if (result > 0)
     result += NumCams;
  return result;
}

cSpuDecoder *cDvbDevice::GetSpuDecoder(void)
{
  if (!spuDecoder && IsPrimaryDevice())
     spuDecoder = new cDvbSpuDecoder();
  return spuDecoder;
}

//M7X0 BEGIN AK
#ifdef WITH_LIBJPEG
uchar *cDvbDevice::GrabImage(int &Size, bool Jpeg, int Quality, int SizeX, int SizeY)
{
  if (devVideoIndex < 0)
     return NULL;
  char buffer[PATH_MAX];
  snprintf(buffer, sizeof(buffer), "%s%d", DEV_VIDEO, devVideoIndex);
  int videoDev = open(buffer, O_RDWR);
  if (videoDev >= 0) {
     uchar *result = NULL;
     struct video_mbuf mbuf;
     if (ioctl(videoDev, VIDIOCGMBUF, &mbuf) == 0) {
        int msize = mbuf.size;
        unsigned char *mem = (unsigned char *)mmap(0, msize, PROT_READ | PROT_WRITE, MAP_SHARED, videoDev, 0);
        if (mem && mem != (unsigned char *)-1) {
           // set up the size and RGB
           struct video_capability vc;
           if (ioctl(videoDev, VIDIOCGCAP, &vc) == 0) {
              struct video_mmap vm;
              vm.frame = 0;
              if ((SizeX > 0) && (SizeX <= vc.maxwidth) &&
                  (SizeY > 0) && (SizeY <= vc.maxheight)) {
                 vm.width = SizeX;
                 vm.height = SizeY;
                 }
              else {
                 vm.width = vc.maxwidth;
                 vm.height = vc.maxheight;
                 }
              vm.format = VIDEO_PALETTE_RGB24;
              if (ioctl(videoDev, VIDIOCMCAPTURE, &vm) == 0 && ioctl(videoDev, VIDIOCSYNC, &vm.frame) == 0) {
                 // make RGB out of BGR:
                 int memsize = vm.width * vm.height;
                 unsigned char *mem1 = mem;
                 for (int i = 0; i < memsize; i++) {
                     unsigned char tmp = mem1[2];
                     mem1[2] = mem1[0];
                     mem1[0] = tmp;
                     mem1 += 3;
                     }

                 if (Quality < 0)
                    Quality = 100;

                 dsyslog("grabbing to %s %d %d %d", Jpeg ? "JPEG" : "PNM", Quality, vm.width, vm.height);
                 if (Jpeg) {
                    // convert to JPEG:
                    result = RgbToJpeg(mem, vm.width, vm.height, Size, Quality);
                    if (!result)
                       esyslog("ERROR: failed to convert image to JPEG");
                    }
                 else {
                    // convert to PNM:
                    char buf[32];
                    snprintf(buf, sizeof(buf), "P6\n%d\n%d\n255\n", vm.width, vm.height);
                    int l = strlen(buf);
                    int bytes = memsize * 3;
                    Size = l + bytes;
                    result = MALLOC(uchar, Size);
                    if (result) {
                       memcpy(result, buf, l);
                       memcpy(result + l, mem, bytes);
                       }
                    else
                       esyslog("ERROR: failed to convert image to PNM");
                    }
                 }
              }
           munmap(mem, msize);
           }
        else
           esyslog("ERROR: failed to memmap video device");
        }
     close(videoDev);
     return result;
     }
  else
     LOG_ERROR_STR(buffer);
  return NULL;
}
#endif

// Taken from gambler
/*
 * M7X0
 * wow realy wired we have to set the ioctl on /dev/videoX
 * and this takes effect on /dev/ost/videoX
 *
 *
 */
void cDvbDevice::SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat)
{

   if (HasDecoder()) {
      /*
       * we don't want to force 16/9 in letterbox as in orig code
       * try it this way
      *
      if (Setup.VideoFormat) {
         CHECK(ioctl(fd_video_v4l, M7X0_SET_TV_ASPECT_MODE, M7X0_VIDEO_LETTER_BOX));
        //CHECK(ioctl(fd_video_set, M7X0_VIDEO_SET_DISPLAY_FORMAT, M7X0_VIDEO_PAN_SCAN));
         dsyslog("DEBUG set setup mode: letterbox");
      }
      else { */
      switch (VideoDisplayFormat) {
        case vdfPanAndScan:
             CHECK(ioctl(fd_video_v4l, M7X0_SET_TV_ASPECT_MODE, M7X0_VIDEO_PAN_SCAN));
             dsyslog("DEBUG: set mode -> pan scan");
             break;
        case vdfLetterBox:
             CHECK(ioctl(fd_video_v4l, M7X0_SET_TV_ASPECT_MODE, M7X0_VIDEO_LETTER_BOX));
             dsyslog("DEBUG: set mode -> letterbox");
             break;
        default: esyslog("ERROR: unknown video display format %d", VideoDisplayFormat);
        }
      //}
   }

}
// Taken from gambler
void cDvbDevice::SetVideoFormat(eVideoFormat VideoFormat)
{
/*
 *
 * NOTE: m7x0 use avswitch for 16/9 4/3
 * but pan scan, letterbox stuff is done
 * on /dev/videoX
 *
 * thx to anonymous
 *
 * aspect auto format added
 */
   if (getIaMode() && HasDecoder()) {

     int avs = open("/dev/avswitch", O_WRONLY);
     if(avs == -1)
        esyslog("m7x0 can not open /dev/avswitch");

      switch(VideoFormat) {
        case vf16_9:
             dsyslog("DEBUG: set 16/9");
             CHECK(ioctl(fd_video_v4l, M7X0_SET_TV_ASPECT_RATIO, M7X0_VIDEO_FORMAT_16_9));
             CHECK(ioctl(avs, AVSWCMD_MODE_16_9, 0));
             break;
        case vf4_3:
             dsyslog("DEBUG: set 4/3");
             CHECK(ioctl(fd_video_v4l, M7X0_SET_TV_ASPECT_RATIO, M7X0_VIDEO_FORMAT_4_3));
             CHECK(ioctl(avs, AVSWCMD_MODE_4_3, 0));
             break;
        case vfauto:
             dsyslog("DEBUG: m7x0 auto aspect");
             CheckStreamAspect(1);
             break;
        default: esyslog("ERROR: unknown video format %d", VideoFormat);
      }
      close(avs);
      SetVideoDisplayFormat(eVideoDisplayFormat(Setup.VideoDisplayFormat));
   }
    //int debugget;
    //CHECK(ioctl(fd_video_v4l, M7X0_GET_TV_ASPECT_RATIO, &debugget));
    //dsyslog("DEBUG: current display format (3=16_9) -> %i", debugget);

//M7X0 END AK
}

//m7x0 auto aspect
void cDvbDevice::CheckStreamAspect(bool force)
{
  int asset, asget;
  CHECK(ioctl(fd_video, M7X0_GET_STREAM_ASPECT_RATIO, &asset));
  CHECK(ioctl(fd_video, M7X0_GET_TV_ASPECT_RATIO, &asget));
  //dsyslog("DEBUG: stream as -> %i", asset);
  if ((asset != asget && getIaMode()) || force ) {
     if (asset==3) {
        dsyslog("DEBUG: auto set 16/9");
        SetVideoFormat(eVideoFormat(1));
        }
     else {
        dsyslog("DEBUG: auto set 4/3");
        SetVideoFormat(eVideoFormat(0));
        }
    }
}

void cDvbDevice::SetTvSettings(bool settv){
    dsyslog("DEBUG: set tv settings-> %d", settv);
    if(settv){
      Audios.MuteAudio(IsMute());
      SetVolumeDevice(IsMute() ? 0 : CurrentVolume());
      SetTvMode(Setup.TvMode);
      SetVCRMode(Setup.VCRMode);
      SetVideoFormat(eVideoFormat(Setup.VideoFormat));
    }else{
      Audios.MuteAudio(true);
      int avs = open("/dev/avswitch", O_WRONLY);
      if(avs == -1)
        esyslog("m7x0 can not open /dev/avswitch");
      CHECK(ioctl(avs, AVSWCMD_TV_OFF, 0));
      CHECK(ioctl(avs, AVSWCMD_TV_VCR, 0));
      close(avs);
    }
}

//m7x0 TvMode fbas svideo
void cDvbDevice::SetTvMode(bool tvmode){
    dsyslog("DEBUG: set tv mode -> %d", tvmode);
    if(getIaMode()){
      int avs = open("/dev/avswitch", O_WRONLY);
      if(avs == -1)
        esyslog("m7x0 can not open /dev/avswitch");
      if(tvmode){
        CHECK(ioctl(avs, AVSWCMD_TV_SVIDEO, 0));
      }else{
        CHECK(ioctl(avs, AVSWCMD_TV_FBAS, 0));
      }
      close(avs);
    }
}

//m7x0 VCRMode fbas svideo
void cDvbDevice::SetVCRMode(bool vcrmode){
    dsyslog("DEBUG: set vcr mode -> %d", vcrmode);
    int avs = open("/dev/avswitch", O_WRONLY);
    if(avs == -1)
      esyslog("m7x0 can not open /dev/avswitch");
    if(vcrmode){
      CHECK(ioctl(avs, AVSWCMD_VCR_SVIDEO, 0));
    }else{
      CHECK(ioctl(avs, AVSWCMD_VCR_FBAS, 0));
    }
    close(avs);
}

eVideoSystem cDvbDevice::GetVideoSystem(void)
{
  eVideoSystem VideoSystem = vsPAL;
//M7X0 BEGIN AK
// Nothing in here works yet
//M7X0TODO: fix this
#if 0
  video_size_t vs;
  if (ioctl(fd_video, VIDEO_GET_SIZE, &vs) == 0) {
     if (vs.h == 480 || vs.h == 240)
        VideoSystem = vsNTSC;
     }
  else
     LOG_ERROR;
#endif
//M7X0 END AK
  return VideoSystem;
}

//M7X0 BEGIN AK
// If anybody who has access to the driver source reads this.
// FIX YOUR HORRIBLE BUGGY DRIVERS !!!  AC3 is a only bug
// Do not work really in all case.
// (not even in wavebox).



bool cDvbDevice::SetAudioBypass(bool On)
{
  if (setTransferModeForDolbyDigital != 1 && setTransferModeForDolbyDigital != 3)
    return false;
 // dsyslog("cDvbDevice: SetAudioBypass %d", On);

 // CHECK(ioctl(fd_audio,AUDIO_ENABLE_ANALOG,true));
  //CHECK(ioctl(fd_audio,AUDIO_ENABLE_ANALOG,!On));
  //CHECK(ioctl(fd_audio,AUDIO_ENABLE_AC3,On))
#if 0
  if (!setTransferModeForDolbyDigital)
     return false;
  return ioctl(fd_audio, AUDIO_SET_BYPASS_MODE, On) == 0;
#endif
  return true;
//M7XO END AK
}

//                            ptAudio        ptVideo        ptPcr        ptTeletext        ptDolby        ptOther
//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
dmx_pes_type_t PesTypes[] = { DMX_PES_AUDIO, DMX_PES_VIDEO, DMX_PES_PCR, DMX_PES_TELETEXT, DMX_PES_AUDIO, DMX_PES_VIDEO, DMX_PES_OTHER };
#else
dmx_pes_type_t PesTypes[] = { DMX_PES_AUDIO, DMX_PES_VIDEO, DMX_PES_PCR, DMX_PES_TELETEXT, DMX_PES_AUDIO, DMX_PES_OTHER };
#endif
//M7X0TODO: Get Dolby working
bool cDvbDevice::SetPid(cPidHandle *Handle, int Type, bool On)
{
  if (Handle->pid) {
     dmx_pes_filter_params pesFilterParams;
     memset(&pesFilterParams, 0, sizeof(pesFilterParams));
     if (On) {

        // For debuging only
        //dsyslog("DEBUG: Demux Setting PID: %u Type: %d", Handle->pid, Type);

        if (Handle->handle < 0) {
           Handle->handle = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, CardIndex(), O_RDWR | O_NONBLOCK, true);
           if (Handle->handle < 0) {
              LOG_ERROR;
              return false;
              }
           }

        int r,errnoSave, i = 0;
        if (Type == ptAudio || Type == ptDolby){
           // Uglly the driver needs setting exacty this Value
           // Yet another BUG in m7x0-drivers
           if ((r = ioctl(Handle->handle, DMX_SET_BUFFER_SIZE,0x1e000)) < 0) {
              CHECK(r);
              close(Handle->handle);
              Handle->handle = -1;
              return false;
              }
           }
        else if (Type == ptVideo)
           CHECK(ioctl(Handle->handle, DMX_SET_BUFFER_SIZE, 0x100000));

        pesFilterParams.pid     = Handle->pid;
        pesFilterParams.input   = DMX_IN_FRONTEND; //CardIndex() == 0 ? DMX_IN_FRONTEND0 : DMX_IN_FRONTEND1;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
        if (Type < ptRecVideo) {
#else
        if (Type < ptOther) {
#endif
           if (ActualDevice() == this)
              pesFilterParams.output = Type != ptDolby ? DMX_OUT_DECODER0 : DMX_OUT_DECODER1;
           else
              pesFilterParams.output = DMX_OUT_DECODER;
           }
        else
           pesFilterParams.output = DMX_OUT_TS_TAP;

        pesFilterParams.pes_type= PesTypes[Type < ptOther ? Type : ptOther];
        pesFilterParams.flags   = DMX_IMMEDIATE_START;

        // Is this loop really nessesary any more.
        // In earllier Versions the driver returns with EBUSY sometimes
        do {
           if ((r = ioctl(Handle->handle, DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
              errnoSave=errno;
              CHECK(r);
              cCondWait::SleepMs(10);
              }
           else
              errnoSave = 0;
           i++;
           } while (errnoSave==EBUSY && i<=100);

        if (errnoSave != 0){
           close(Handle->handle);
           Handle->handle = -1;
           return false;
           }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
        if (Type == ptRecVideo && tsBuffer)
           tsBuffer->UseFrameEvents(true);
#endif
        }
     else if (!Handle->used) {
        // For debuging only
        //dsyslog("DEBUG: Demux Stopping PID: %u Type: %d", Handle->pid, Type);
        CHECK(ioctl(Handle->handle, DMX_STOP));
        close(Handle->handle);
        Handle->handle = -1;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
        if (Type == ptRecVideo && tsBuffer)
           tsBuffer->UseFrameEvents(false);
#endif
        }
     }
  return true;
}
//M7X0 END AK

#define RB_NUM_SECTIONS 8 // default: 2 sections = 8192 bytes

int cDvbDevice::OpenFilter(u_short Pid, u_char Tid, u_char Mask)
{
   const char *FileName = *cDvbName(DEV_DVB_ADAPTER DEV_DVB_DEMUX, CardIndex());
  int f = open(FileName, O_RDWR | O_NONBLOCK);
  if (f >= 0) {
     if (Pid == EITPID) { // increase ringbuffer size for EIT
        if (ioctl(f, DMX_SET_BUFFER_SIZE, MAX_SECTION_SIZE * RB_NUM_SECTIONS) < 0)
           dsyslog("OpenFilter (pid=%d, tid=%02X): ioctl DMX_SET_BUFFER_SIZE failed", Pid, Tid);
        }
     dmx_sct_filter_params sctFilterParams;
     memset(&sctFilterParams, 0, sizeof(sctFilterParams));
     sctFilterParams.pid = Pid;
     sctFilterParams.timeout = 0;
//M7X0 BEGIN AK
// CRC checking won't work, driver delivers CRC-Invalid sections
// What the heck is this flag for as additional checking is need with or without it.
     sctFilterParams.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;

     sctFilterParams.filter.filter[0] = Tid;
     sctFilterParams.filter.mask[0] = Mask;
//M7X0 END AK
     if (ioctl(f, DMX_SET_FILTER, &sctFilterParams) >= 0)
        return f;
     else {
//M7x0 BEGIN AK
        char __errorstr[256];
        strerror_r(errno,__errorstr,256);
        __errorstr[255]=0;
        esyslog("ERROR: can't set filter (pid=%d, tid=%02X, mask=%02X): %s", Pid, Tid, Mask,__errorstr);
//M7x0 END AK
        close(f);
        }
     }
  else
     esyslog("ERROR: can't open filter handle on '%s'", FileName);
  return -1;
}

void cDvbDevice::TurnOffLiveMode(bool LiveView, bool DoBlank)
{
//M7X0 BEGIN AK

  // Turn off live PIDs:
// Live Pids get Special PID-slots so don't detach any receivers
#if 0
  DetachAll(pidHandles[ptAudio].pid);
  DetachAll(pidHandles[ptVideo].pid);
  DetachAll(pidHandles[ptPcr].pid);
  DetachAll(pidHandles[ptTeletext].pid);
#endif

  if (LiveView) {
     // Avoid noise while switching:
     CHECK(ioctl(fd_audio, AUDIO_STOP, 0));
     CHECK(ioctl(fd_video, VIDEO_STOP, 1));
     }
  if (DoBlank) {
  /* This more than ugly but driver is more than ugly as bad.
   * If switched to a radio channel, driver does not blank screen.
   * Last frame is shown until another video-channel is selected.
   * Even worse if started up with a radio channel video gets not
   * shown if a video channel is selected.
   * Have I mentioned the driver is buggy as hell, yet?
   * This is the only way I found to get it blanking */
     close(fd_video);
     close(fd_audio);
     fd_video    = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_VIDEO,  CardIndex(), O_RDWR | O_NONBLOCK);
     fd_audio    = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_AUDIO,  CardIndex(), O_RDWR | O_NONBLOCK);
     }
  DelPid(pidHandles[ptAudio].pid, ptAudio);
  DelPid(pidHandles[ptVideo].pid, ptVideo);
  DelPid(pidHandles[ptPcr].pid, ptPcr);
  DelPid(pidHandles[ptTeletext].pid, ptTeletext);
  DelPid(pidHandles[ptDolby].pid, ptDolby);

//M7X0 END AK
}

bool cDvbDevice::ProvidesSource(int Source) const
{
  int type = Source & cSource::st_Mask;
  return type == cSource::stNone
#ifdef M750S
      || (type == cSource::stCable && frontendType == FE_QAM)
      || (type == cSource::stSat   && frontendType == FE_QPSK)
#endif
      || (type == cSource::stTerr  && frontendType == FE_OFDM);
}

bool cDvbDevice::ProvidesTransponder(const cChannel *Channel) const
{
#ifdef M750S
  return ProvidesSource(Channel->Source()) && (!cSource::IsSat(Channel->Source()) || !Setup.DiSEqC || Diseqcs.Get(Channel->Source(), Channel->Frequency(), Channel->Polarization()));
#else
  return ProvidesSource(Channel->Source());
#endif
}
//M7X0 BEGIN AK
bool cDvbDevice::ProvidesChannel(const cChannel *Channel, int Priority, bool *NeedsDetachReceivers, bool forTransferer) const
{
  bool result = false;
  bool hasPriority = Priority < 0 || Priority > this->Priority();
  bool needsDetachReceivers = false;

  if (ProvidesSource(Channel->Source()) && ProvidesCa(Channel)) {
     result = hasPriority;
     if (Priority >= 0 && Receiving(true)) {
        if (dvbTuner->IsTunedTo(Channel) && (forTransferer || FreeReceiverSlot())) {
//M7X0 END AK
           if ((Channel->Vpid() && !HasPid(Channel->Vpid())) || (Channel->Apid(0) && !HasPid(Channel->Apid(0)))) {
#ifdef DO_MULTIPLE_RECORDINGS
#ifndef DO_MULTIPLE_CA_CHANNELS
              if (Ca() >= CA_ENCRYPTED_MIN || Channel->Ca() >= CA_ENCRYPTED_MIN)
                 needsDetachReceivers = Ca() != Channel->Ca();
              else
#endif
              if (!IsPrimaryDevice())
                 result = true;
#ifdef DO_REC_AND_PLAY_ON_PRIMARY_DEVICE
              else
                 result = Priority >= Setup.PrimaryLimit;
#endif
#endif
              }
           else
              result = !IsPrimaryDevice() || Priority >= Setup.PrimaryLimit;
           }
        else
           needsDetachReceivers = true;
        }
     }
  if (NeedsDetachReceivers)
     *NeedsDetachReceivers = needsDetachReceivers;
  return result;
}

bool cDvbDevice::IsTunedToTransponder(const cChannel *Channel)
{
  return dvbTuner->IsTunedTo(Channel);
}

//M7X0 BEGIN AK
bool cDvbDevice::SetChannelDevice(const cChannel *Channel, bool LiveView)
{
  bool DoTune = !dvbTuner->IsTunedTo(Channel);

  bool TurnOffLivePIDs = HasDecoder()
                         && (DoTune
                            || (!IsPrimaryDevice() && !Setup.TShift)
                            || LiveView // for a new live view the old PIDs need to be turned off
                            );


  bool TurnOnLivePIDs = HasDecoder() && LiveView;

  bool DoBlank = (LiveView && !pidHandles[ptVideo].pid && Channel->Vpid()) ||
                 (pidHandles[ptVideo].pid && (!LiveView || !Channel->Vpid()));


#ifndef DO_MULTIPLE_RECORDINGS
  TurnOffLivePIDs = TurnOnLivePIDs = true;
#endif

  // Turn off live PIDs if necessary:

  if (TurnOffLivePIDs) {
     if (TurnOnLivePIDs)
        CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, true));
     TurnOffLiveMode(LiveView, DoBlank);
     }

  // Set the tuner:

  dvbTuner->Set(Channel, DoTune);

  // If this channel switch was requested by the EITScanner we don't wait for
  // a lock and don't set any live PIDs (the EITScanner will wait for the lock
  // by itself before setting any filters):

  if (EITScanner.UsesDevice(this)) //XXX
     return true;

  // PID settings:

  if (TurnOnLivePIDs) {
     // Wait for tuner lock:
     // This seems to be need sometimes on M7x0
     if (!HasLock(-1))
        esyslog("ERROR: Cannot get Tuner-Lock!");
     SetAudioBypass(false);
     if (!(AddPid(Channel->Ppid(), ptPcr) && AddPid(Channel->Vpid(), ptVideo) && AddPid(Channel->Apid(0), ptAudio))) {
        esyslog("ERROR: failed to set PIDs for channel %d on device %d", Channel->Number(), CardIndex() + 1);
        return false;
        }

     if (IsPrimaryDevice())
        AddPid(Channel->Tpid(), ptTeletext);

     if (Channel->Vpid() > 0) {
        CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, 1));
        CHECK(ioctl(fd_video, VIDEO_PLAY,0));
        }
     else {
        CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, 0));
        }

     CHECK(ioctl(fd_audio, AUDIO_PLAY,0));
     CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, false));

     }
  return true;
}
//M7X0 END AK

bool cDvbDevice::HasLock(int TimeoutMs) const
{
  return dvbTuner ? dvbTuner->Locked(TimeoutMs) : false;
}
//M7X0 BEGIN AK
int cDvbDevice::GetAudioChannelDevice(void)
{
  return audioChannel;
}

void cDvbDevice::SetAudioChannelDevice(int AudioChannel)
{
  CHECK(ioctl(fd_audio, AUDIO_STOP));
  audioChannel=AudioChannel;
  CHECK(ioctl(fd_audio,AUDIO_STEERING, AudioChannel));
  CHECK(ioctl(fd_audio, AUDIO_PLAY));
}

void cDvbDevice::SetVolumeDevice(int Volume)
{
  if (HasDecoder()&&!digitalAudio) {
     audio_mixer_t am;
     // conversion for linear volume response:
     am.volume_left = am.volume_right = 2 * Volume - Volume * Volume / 255;
     CHECK(ioctl(fd_audio, AUDIO_SET_MIXER, &am));
     }
}

void cDvbDevice::SetDigitalAudioDevice(bool On)
{
  dsyslog("cDvbDevice: Set Digital Audio %d", On);
  if (digitalAudio != On) {

     digitalAudio = On;

     //CHECK(ioctl(fd_audio,AUDIO_ENABLE_AC3,On))
     if (On)
        //Is this right? Does this call select the decoder for AC3?
        CHECK(ioctl(fd_audio,AUDIO_ENABLE_AC3,true))
     else {
        // Maybe need for normal Analog mode (?)
        CHECK(ioctl(fd_audio,AUDIO_ENABLE_AC3,false));
        CHECK(ioctl(fd_audio,AUDIO_STEERING, audioChannel));
        }
     SetVolumeDevice(IsMute() ? 0 : CurrentVolume());
     }
}

void cDvbDevice::SetTransferModeForDolbyDigital(int Mode)
{
  dsyslog("cDvbDevice: Set Transfermode Digital Audio %d", Mode);
  setTransferModeForDolbyDigital = Mode;
}


void cDvbDevice::SetAudioTrackDevice(eTrackType Type, const tTrackId *TrackId)
{
  bool startStopAudio = true;
  if (!TrackId)
     TrackId = GetTrack(Type);
  else
     startStopAudio = false;
  if (TrackId && TrackId->id) {
     SetAudioBypass(false);
     if (IS_AUDIO_TRACK(Type) || (IS_DOLBY_TRACK(Type) && SetAudioBypass(true))) {
        if (pidHandles[ptAudio].pid && pidHandles[ptAudio].pid != TrackId->id) {
           if (startStopAudio)
              CHECK(ioctl(fd_audio,AUDIO_STOP))

           if (ciHandler)
              ciHandler->SetPid(pidHandles[ptAudio].pid, false);

           pidHandles[ptAudio].pid = TrackId->id;
           SetPid(&pidHandles[ptAudio],IS_DOLBY_TRACK(Type)?ptDolby:ptAudio, true);

           if (ciHandler) {
              ciHandler->SetPid(pidHandles[ptAudio].pid, true);
              ciHandler->StartDecrypting();
              }

           if (startStopAudio) {
              CHECK(ioctl(fd_audio,AUDIO_PLAY))

              if (IS_DOLBY_TRACK(Type) && (pidHandles[ptPcr].handle >= 0)) {
                 int diff, stat;
                 stat = ioctl(pidHandles[ptPcr].handle,DMX_GET_AUDIO_SYNC_DIFF,&diff);
                 dsyslog("cDvbDevice DEBUG: AC3 Audio Sync Diff ioctl-Status %d Difference %d", stat, diff);
                 }
              }
           }
        }
     else if (IS_DOLBY_TRACK(Type)) {
        if (setTransferModeForDolbyDigital == 0 || setTransferModeForDolbyDigital == 3)
           return;
        // Currently this works only in Transfer Mode
        ForceTransferMode();
        }
     }
}
void cDvbDevice::StartLiveView(bool On, bool AudioOnly) {
  if (playMode != pmTransferer && playMode != pmTransfererAudioOnly)
     return;
  if (On) {
     if (!AudioOnly) {
        CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, playMode == pmTransferer));
        CHECK(ioctl(fd_video, VIDEO_PLAY,0));
        }

     CHECK(ioctl(fd_audio, AUDIO_PLAY,0));
     return;
     }

  if (!AudioOnly)
     CHECK(ioctl(fd_video, VIDEO_STOP,1));

  CHECK(ioctl(fd_audio, AUDIO_STOP,0));
}
//M7X0 END AK

bool cDvbDevice::CanReplay(void) const
{
#ifndef DO_REC_AND_PLAY_ON_PRIMARY_DEVICE
  if (Receiving())
     return false;
#endif
  return cDevice::CanReplay();
}

//M7X0 BEGIN AK
//M7X0TODO: Get other playmodes working
bool cDvbDevice::SetPlayMode(ePlayMode PlayMode)
{
  // Sanity check
  if (playMode == PlayMode){
     dsyslog("WARNING: Play mode %d already set!", PlayMode);
     return true;
     }

  if (PlayMode != pmExtern_THIS_SHOULD_BE_AVOIDED && fd_video < 0 && fd_audio < 0) {
     // reopen the devices
     fd_video = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_VIDEO,  CardIndex(), O_RDWR | O_NONBLOCK);
     fd_audio = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_AUDIO,  CardIndex(), O_RDWR | O_NONBLOCK);
     SetVideoFormat(eVideoFormat(Setup.VideoFormat));
     }
  bool DoBlank = false;
  switch (PlayMode) {
    case pmNone:
         if (fd_playDemux[1] >= 0) {
            CHECK(ioctl(fd_playDemux[1], DMX_STOP,1));
            close(fd_playDemux[1]);
            fd_playDemux[1] = -1;
            playAudioId = 0;
            }
         if (fd_playDemux[0] >= 0) {
            CHECK(ioctl(fd_playDemux[0], DMX_STOP,1));
            close(fd_playDemux[0]);
            fd_playDemux[0] = -1;
            }
         if (fd_playDvr >= 0) {
            close(fd_playDvr);
            fd_playDvr = -1;
            }

         CHECK(ioctl(fd_audio, AUDIO_STOP, 0));
         CHECK(ioctl(fd_video, VIDEO_STOP, 0));

         if ((playMode == pmAudioOnly) | (playMode == pmAudioOnlyBlack))
            TurnOffLiveMode(true, true);

         if((playMode==pmTransfererAudioOnly)&&(Setup.TShift))
            TurnOffLiveMode(true, true);

#ifndef TS_PLAYER_BACKPORT
         if (tsreplayer != NULL) {
            delete tsreplayer;
            tsreplayer=NULL;
         }
#endif
         break;
    case pmAudioVideo:
    case pmVideoOnly: {
#ifndef TS_PLAYER_BACKPORT
         if (tsreplayer != NULL) {
            delete tsreplayer;
            tsreplayer=NULL;
         }
#endif
         TurnOffLiveMode(true, (!pidHandles[cDevice::ptVideo].pid&&!Setup.TShift) |
              (playMode == pmAudioOnly) | (playMode == pmAudioOnlyBlack));
         CHECK(ioctl(fd_audio, AUDIO_STOP,0));
         CHECK(ioctl(fd_video, VIDEO_STOP, 1));

         if (fd_playDvr >= 0)
            close(fd_playDvr);
         fd_playDvr = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DVR, 0, O_WRONLY, true);
         if (fd_playDvr < 0)
            return false;

         // CHECK(ioctl(fd_playDvr,DVR_SET_STREAM_TYPE, DVR_MPEG2_TS));
         if (fd_playDemux[0] >= 0)
            close(fd_playDemux[0]);

         fd_playDemux[0] = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, 0, O_RDWR | O_NONBLOCK, true);
         if (fd_playDemux[0] < 0) {
            close(fd_playDvr);
            fd_playDvr = -1;
            return false;
         }
         dmx_pes_filter_params pesFilterParams;
         memset(&pesFilterParams, 0, sizeof(pesFilterParams));

         // Uglly the driver needs setting exacty this Value
         // Yet another BUG in m7x0-drivers
         CHECK(ioctl(fd_playDemux[0], DMX_SET_BUFFER_SIZE,0x100000));
         pesFilterParams.pid      = 0xEF;
         playTsCcounter[0]        = 0;

         pesFilterParams.input    = DMX_IN_DVR;
         pesFilterParams.output   = DMX_OUT_DECODER0;
         pesFilterParams.pes_type = DMX_PES_VIDEO;
         pesFilterParams.flags    = DMX_IMMEDIATE_START;

         int r, i = 0, errnoSave;
         // Is this loop really nessesary any more.
         // In earllier Versions the driver returns with EBUSY sometimes
         do {
            if ((r = ioctl(fd_playDemux[0],DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
               errnoSave = errno;
               CHECK(r);
               cCondWait::SleepMs(3);
               }
            else
               errnoSave = 0;
            i++;
            } while ((errnoSave == EBUSY) & (i <= 100));

         if (errnoSave) {
            close(fd_playDemux[0]);
            fd_playDemux[0] = -1;
            close(fd_playDvr);
            fd_playDvr = -1;
            return false;
            }

         if (fd_playDemux[1] >= 0)
            close(fd_playDemux[1]);
         fd_playDemux[1] = -1;
         playAudioId = 0;
         playTrickSpeed = false;
         playBufferFill = 0;
         CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, PlayMode == pmAudioVideo));
         CHECK(ioctl(fd_video,VIDEO_PLAY,0));
         CHECK(ioctl(fd_audio,AUDIO_PLAY,0));

         break; }
    case pmAudioOnly:
    case pmAudioOnlyBlack:
#ifndef TS_PLAYER_BACKPORT
         if (tsreplayer != NULL) {
            delete tsreplayer;
            tsreplayer=NULL;
         }
#endif

         TurnOffLiveMode(true, true);
         CHECK(ioctl(fd_audio, AUDIO_STOP,0));
         CHECK(ioctl(fd_video, VIDEO_STOP, 1));
         playBufferFill = 0;
         playAudioId = 0;
         CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, false));
         CHECK(ioctl(fd_video,VIDEO_PLAY,0));
         CHECK(ioctl(fd_audio,AUDIO_PLAY,0));
         break;
    case pmTsAudioVideo:
    case pmTsAudioOnlyBlack:
         if (fd_playDemux[1] >= 0) {
            CHECK(ioctl(fd_playDemux[1], DMX_STOP,1));
            close(fd_playDemux[1]);
            fd_playDemux[1] = -1;
            playAudioId = 0;
            }
         if (fd_playDemux[0] >= 0) {
            CHECK(ioctl(fd_playDemux[0], DMX_STOP,1));
            close(fd_playDemux[0]);
            fd_playDemux[0] = -1;
            }
         if (fd_playDvr >= 0) {
            close(fd_playDvr);
            fd_playDvr = -1;
            }
         DoBlank =
#ifndef TS_PLAYER_BACKPORT
            !tsreplayer &&
#endif
           ((!pidHandles[cDevice::ptVideo].pid && PlayMode == pmTsAudioVideo) ||
            (pidHandles[cDevice::ptVideo].pid && PlayMode != pmTsAudioVideo));
         TurnOffLiveMode(true, DoBlank);
         CHECK(ioctl(fd_audio, AUDIO_STOP,0));
         CHECK(ioctl(fd_video, VIDEO_STOP, 1));
#ifndef TS_PLAYER_BACKPORT
         if (tsreplayer == NULL){
            tsreplayer = new c7x0TsReplayer(this);
            }
         else
            tsreplayer->Reset();
#endif
         break;
    case pmTsAudioOnly:
    case pmTsVideoOnly:
         if (fd_playDemux[1] >= 0) {
            CHECK(ioctl(fd_playDemux[1], DMX_STOP,1));
            close(fd_playDemux[1]);
            fd_playDemux[1] = -1;
            playAudioId = 0;
            }
         if (fd_playDemux[0] >= 0) {
            CHECK(ioctl(fd_playDemux[0], DMX_STOP,1));
            close(fd_playDemux[0]);
            fd_playDemux[0] = -1;
            }
         if (fd_playDvr >= 0) {
            close(fd_playDvr);
            fd_playDvr = -1;
            }
         CHECK(ioctl(fd_audio, AUDIO_STOP,0));
         CHECK(ioctl(fd_video, VIDEO_STOP, 1));
#ifndef TS_PLAYER_BACKPORT
         if (tsreplayer == NULL){
            tsreplayer = new c7x0TsReplayer(this);
            }
         else
            tsreplayer->Reset();
#endif
         break;
    case pmExtern_THIS_SHOULD_BE_AVOIDED:
         return false;
         break;

    case pmTransferer:
         TurnOffLiveMode(true,!pidHandles[cDevice::ptVideo].pid&&!Setup.TShift);
         break;
    case pmTransfererAudioOnly:
         TurnOffLiveMode(true,pidHandles[cDevice::ptVideo].pid||Setup.TShift);
         break;
    default: esyslog("ERROR: unknown play mode %d", PlayMode);
    }
  playMode = PlayMode;
  return true;
}


int64_t cDvbDevice::GetSTC(void)
{
#if 0
  if (fd_stc >= 0) {
     struct dmx_stc stc;
     stc.num = 0;
     if (ioctl(fd_stc, DMX_GET_STC, &stc) == -1) {
        esyslog("ERROR: stc %d: %m", CardIndex() + 1);
        return -1;
        }
     return stc.stc / stc.base;
     }
#endif
  return -1;
}

void cDvbDevice::TrickSpeed(int Speed, bool UseFastForward)
{
#ifndef TS_PLAYER_BACKPORT
  if (tsreplayer != NULL) {
     tsreplayer->TrickSpeed(Speed,UseFastForward);
     return;
     }
#endif

  if ((playMode != pmAudioVideo) & (playMode != pmVideoOnly))
     return;
  dsyslog("TrickSpeed called Speed %d %d!",Speed, UseFastForward);
  CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, true));
  CHECK(ioctl(fd_audio, AUDIO_STOP,0));
  CHECK(ioctl(fd_video, VIDEO_STOP,0));
  if (Speed > 1)
     CHECK(ioctl(fd_video, VIDEO_SLOWMOTION, Speed << 1))
  else
     CHECK(ioctl(fd_video, VIDEO_FAST_FORWARD, 1))
  playTrickSpeed = true;
  playBufferFill = 0;
}

void cDvbDevice::Clear(void)
{
  if (playMode < pmTsAudioVideo) {
     dsyslog("cDvbDevice::Clear(void) called!");
     CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, true));
     CHECK(ioctl(fd_audio, AUDIO_STOP,0));
     CHECK(ioctl(fd_video, VIDEO_STOP,0));
     CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, playMode == pmAudioVideo));
     CHECK(ioctl(fd_video, VIDEO_PLAY,0));
     CHECK(ioctl(fd_audio, AUDIO_PLAY,0));
     CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, false));
     playBufferFill = 0;
     playAudioId = 0;
     }
#ifndef TS_PLAYER_BACKPORT
  else if (tsreplayer != NULL)
     tsreplayer->Clear();
#endif
  cDevice::Clear();
}

void cDvbDevice::Play(void)
{
  if (playMode < pmTsAudioVideo) {
     dsyslog("cDvbDevice::Play(void) called!");
     CHECK(ioctl(fd_audio, AUDIO_STOP,0));
     CHECK(ioctl(fd_video, VIDEO_STOP,0));
     CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, playMode == pmAudioVideo));
     CHECK(ioctl(fd_video, VIDEO_PLAY,0));
     CHECK(ioctl(fd_audio, AUDIO_PLAY,0));
     CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, false));
     playTrickSpeed = false;
     playBufferFill = 0;
     playAudioId = 0;
     }
#ifndef TS_PLAYER_BACKPORT
  else if (tsreplayer != NULL)
     tsreplayer->Play();
#endif
  cDevice::Play();
}

void cDvbDevice::Freeze(void)
{
  if (playMode < pmTsAudioVideo) {
     dsyslog("cDvbDevice::Freeze(void) called!");
     CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, true));
     CHECK(ioctl(fd_audio, AUDIO_STOP,0));
     CHECK(ioctl(fd_video, VIDEO_STOP,0));
     }
#ifndef TS_PLAYER_BACKPORT
  else if (tsreplayer != NULL)
     tsreplayer->Freeze();
#endif
  cDevice::Freeze();
}

void cDvbDevice::Mute(void)
{
  if (fd_audio >= 0) {
     CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, false));
     CHECK(ioctl(fd_audio, AUDIO_SET_MUTE, true));
     }
  cDevice::Mute();
}

void cDvbDevice::StillPicture(const uchar *Data, int Length)
{
  if (!Data || Length < TS_SIZE)
     return;
  if (Data[0] == 0x47) {
     // TS data
     cDevice::StillPicture(Data, Length);
     }
  else if (Data[0] == 0x00 && Data[1] == 0x00 && Data[2] == 0x01 && (Data[3] & 0xF0) == 0xE0) {
     // PES data
     char *buf = MALLOC(char, Length);
     if (!buf)
        return;
     int i = 0;
     int blen = 0;
     while (i < Length - 6) {
           if (Data[i] == 0x00 && Data[i + 1] == 0x00 && Data[i + 2] == 0x01) {
              int len = Data[i + 4] * 256 + Data[i + 5];
              if ((Data[i + 3] & 0xF0) == 0xE0) { // video packet
                 // skip PES header
                 int offs = i + 6;
                 // skip header extension
                 if ((Data[i + 6] & 0xC0) == 0x80) {
                    // MPEG-2 PES header
                    if (Data[i + 8] >= Length)
                       break;
                    offs += 3;
                    offs += Data[i + 8];
                    len -= 3;
                    len -= Data[i + 8];
                    if (len < 0 || offs + len > Length)
                       break;
                    }
                 else {
                    // MPEG-1 PES header
                    while (offs < Length && len > 0 && Data[offs] == 0xFF) {
                          offs++;
                          len--;
                          }
                    if (offs <= Length - 2 && len >= 2 && (Data[offs] & 0xC0) == 0x40) {
                       offs += 2;
                       len -= 2;
                       }
                    if (offs <= Length - 5 && len >= 5 && (Data[offs] & 0xF0) == 0x20) {
                       offs += 5;
                       len -= 5;
                       }
                    else if (offs <= Length - 10 && len >= 10 && (Data[offs] & 0xF0) == 0x30) {
                       offs += 10;
                       len -= 10;
                       }
                    else if (offs < Length && len > 0) {
                       offs++;
                       len--;
                       }
                    }
                 if (blen + len > Length) // invalid PES length field
                    break;
                 memcpy(&buf[blen], &Data[offs], len);
                 i = offs + len;
                 blen += len;
                 }
              else if (Data[i + 3] >= 0xBD && Data[i + 3] <= 0xDF) // other PES packets
                 i += len + 6;
              else
                 i++;
              }
           else
              i++;
           }
     CHECK(ioctl(fd_video, VIDEO_FAST_FORWARD, 1))
     WriteAllOrNothing(fd_video,(const uchar*) buf,blen, 1000, 10);;
     free(buf);
     }
  else {
     // non-PES data
     CHECK(ioctl(fd_video, VIDEO_FAST_FORWARD, 1))
     WriteAllOrNothing(fd_video, Data,Length, 1000, 10);
     }
}

bool cDvbDevice::Poll(cPoller &Poller, int TimeoutMs)
{
#ifndef TS_PLAYER_BACKPORT
  if (tsreplayer!=NULL)
     return tsreplayer->Poll(Poller, TimeoutMs);
#endif
  return true;
}

bool cDvbDevice::Flush(int TimeoutMs)
{
  //TODO actually this function should wait until all buffered data has been processed by the card, but how?
#ifndef TS_PLAYER_BACKPORT
  if (tsreplayer!=NULL)
     return tsreplayer->Flush(TimeoutMs);
#endif
  dsyslog("cDvbDevice::Flush called");

  if ((playMode == pmAudioOnly) | (playMode == pmAudioOnlyBlack)) {
     const uchar *write_data = playBuffer;

     int retry_count = 0;
     while ((playBufferFill > 0) & (retry_count < 100)) {
           int r = write(fd_audio, write_data, playBufferFill);
           retry_count++;
           if (r < 0) {
              if ((errno == EBUSY) | (errno == EAGAIN)) {
                 cCondWait::SleepMs(3);
                 continue;
                 }
              if (errno == EINTR)
                 continue;
              break;
              }
           write_data += r;
           playBufferFill -= r;
           }

     if ((playBufferFill > 0) & (playBuffer != write_data))
        memmove(playBuffer, write_data, playBufferFill);
     }
  return true;
}

#ifndef TS_PLAYER_BACKPORT
int cDvbDevice::PlayTs(const uchar *Data, int Length)
{
  // cMutexLock MutexLock(&mutexCurrentAudioTrack);
  if (tsreplayer!=NULL)
     return tsreplayer->PlayTs(Data,Length);

  esyslog("PlayTs called without replayer");
  return -1;
}

void cDvbDevice::SetTsReplayPids(int pmtPid, int videoPid)
{
  if (tsreplayer!=NULL)
     tsreplayer->SetPids(pmtPid, videoPid);
  else
     esyslog("SetTsReplayPids called without replayer");
}

int cDvbDevice::GetTsReplayVideoPid(void)
{
  if (tsreplayer!=NULL)
     return tsreplayer->GetVideoPid();
  esyslog("GetTsReplayVideoPid called without replayer");
  return 0;
}
#endif

int cDvbDevice::PESPayload(const uchar *Data, int Length,
  int &TSOffset, int &TSLength)
{
  static int mpeg2_pts_dts_offset[4] = { 0, 0, 9, 9 };
  static int pts_dts_length[4] = { 0, 0, 5, 10 };
  if ((Data[6] & 0xC0) == 0x80) { // MPEG2
    int dts_pts_fl = Data[7] >> 6;
    TSOffset = mpeg2_pts_dts_offset[dts_pts_fl];
    TSLength = pts_dts_length[dts_pts_fl];
    return 9 + Data[8];
  }
  //MPEG 1
  int payl = 6;
  while (payl < Length && Data[payl] == 0xFF)
        payl++;
  if (payl < Length && (Data[payl] & 0xC0) == 0x40)
     payl += 2;

  if (payl >= Length) {
     esyslog("ERROR: Invalid MPEG1 PES Header!");
     return payl;
     }
  int dts_pts_fl = (Data[payl] & 0x30) >> 4;
  TSOffset = payl;
  TSLength = pts_dts_length[dts_pts_fl];
  payl += TSLength;
  if (!TSLength)
     payl++;
  return payl;
}

void cDvbDevice::PESPacket2TS(const uchar *Data, int Length, int Pid, int &CCounter)
{
  static uchar dts_pts_fl[11] =
    { 0, 0, 0, 0, 0, 0x80, 0, 0, 0, 0, 0xC0 };
  int payload_offset;
  int ts_offset;
  int ts_length;
  const uchar *ts_start;
  const uchar *data_start;
  int cc = CCounter;
  payload_offset = PESPayload(Data,Length,ts_offset,ts_length);
  playBufferFill = 0;
  data_start = Data;
  ts_start = Data + ts_offset;
  if ((Pid & 0xF0) == 0x80)
     payload_offset += 4;
  Data += payload_offset;
  Length -= payload_offset;
  if (Length <= 0)
     return;
  uchar *p = playBuffer;
  p[0] = 0x47;
  p[1] = 0x40;
  p[2] = Pid;
  p[3] = 0x10 | (cc & 0xf);
  cc++;
  int rest = 184 - 9 - ts_length;
  p+=4;
  if (Length < rest) {
     p[-1] = p[-1] | 0x30;
     p[0] = 183 - (Length + 9 + ts_length);
     p[1] = 0;
     memset(p + 2, 0xff, 182);
     p += 1 + p[0];
     rest = Length;
  }
  put_unaligned(get_unaligned((uint32_t *)data_start),(uint32_t *)p);
  put_unaligned(HOST2BE(uint16_t(Length + 3 + ts_length)), (uint16_t *)(p + 4));
  p[6] = 0x80;
  p[7] = dts_pts_fl[ts_length];
  p[8] = ts_length;
  p += 9;
  memcpy(p, ts_start, ts_length);
  p += ts_length;
  memcpy(p,Data,rest);
  p += rest;
  Data += rest;
  Length -= rest;
  int out_length = 188;

  while (Length >= 184) {
        p[0] = 0x47;
        p[1] = 0;
        p[2] = Pid;
        p[3] = 0x10 | (cc & 0xf);
        cc++;
        memcpy(p + 4 , Data, 184);
        Length -= 184;
        Data += 184;
        p += 188;
        out_length += 188;
        }

  if (Length > 0) {
     p[0] = 0x47;
     p[1] = 0;
     p[2] = Pid;
     p[3] = 0x30 | (cc & 0xf);
     cc++;
     p[4] = 183 - Length;
     p[5] = 0;
     memset(p + 6, 0xff, 182);
     p += 5 + p[4];
     memcpy(p, Data, Length);
     out_length += 188;
     }
  CCounter = cc;
  playBufferFill = out_length;
}

int cDvbDevice::PlayVideo(const uchar *Data, int Length)
{
  if ((Data[3] == 0xBE) || (playMode == pmAudioOnlyBlack)) // skip padding stream or video packets in audio mode
     return Length;

  if (!playTrickSpeed) {
     PESPacket2TS(Data, Length, 0xef, playTsCcounter[0]);
     int r = safe_write(fd_playDvr, playBuffer, playBufferFill);
     return r >= 0 ? Length : r;
     }

  int t1, t2;
  int pay_off = PESPayload(Data,Length,t1,t2);
  int pay_length = Length - pay_off;
  const uchar *p = Data + pay_off;
  if (((p[0] | p[1] | (p[2] - 1)) == 0) &
        ((p[3] == 0x00) | (p[3] == 0xB3) | (p[3] == 0xB8))) {
     if (playBufferFill > 0) {
        int r = safe_write(fd_video, playBuffer, playBufferFill);
        if (r < 0)
           return r;
        }
     *((uint32_t *) playBuffer) = 0;
     playBufferFill = 4;
  }

  if (playBufferFill + pay_length > KILOBYTE(256)) {
     esyslog("ERROR: Play buffer size exceeded in trick speed play."
             " Recording corrupt?");
     int bite = KILOBYTE(256) - playBufferFill;
     memcpy(playBuffer + playBufferFill, p, bite);
     int r = safe_write(fd_video, playBuffer, KILOBYTE(256));
     if (r < 0)
        return r;
     p += bite;
     pay_length -= bite;
     playBufferFill = 0;
     }

  memcpy(playBuffer + playBufferFill, p, pay_length);
  playBufferFill += pay_length;
  return Length;
}

int cDvbDevice::PlayAudio(const uchar *Data, int Length, uchar Id)
{
  if (playMode == pmAudioVideo) {
     if (Id != playAudioId) {
        dsyslog("Setting audio channel");
        CHECK(ioctl(fd_audio,AUDIO_STOP,0));
        CHECK(ioctl(fd_video,VIDEO_STOP,0));
        if (fd_playDemux[1] < 0) {
           fd_playDemux[1] = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DEMUX, 0, O_RDWR | O_NONBLOCK, true);
           if (fd_playDemux[1] < 0)
              return -1;
           }

        dmx_pes_filter_params pesFilterParams;
        memset(&pesFilterParams, 0, sizeof(pesFilterParams));

        // Uglly the driver needs setting exacty this Value
        // Yet another BUG in m7x0-drivers
        CHECK(ioctl(fd_playDemux[1], DMX_SET_BUFFER_SIZE,0x1E000));
        pesFilterParams.pid      = Id;
        playTsCcounter[1]        = 0;
        playAudioId              = Id;
        pesFilterParams.input    = DMX_IN_DVR;
        pesFilterParams.output   = (((Id & 0xF0) == 0x80) | (Id == 0xBD)) ?
                                   DMX_OUT_DECODER1 : DMX_OUT_DECODER0;
        pesFilterParams.pes_type = DMX_PES_AUDIO;
        pesFilterParams.flags    = DMX_IMMEDIATE_START;

        int i = 0; int r; int errnoSave;
        do {
           if ((r = ioctl(fd_playDemux[1],DMX_SET_PES_FILTER, &pesFilterParams)) < 0) {
              errnoSave = errno;
              CHECK(r);
              cCondWait::SleepMs(3);
              }
           else
              errnoSave = 0;
           i++;
           } while ((errnoSave == EBUSY) & (i <= 100));

        if (errnoSave)
           return -1;
        CHECK(ioctl(fd_audio, AUDIO_SET_AV_SYNC, playMode == pmAudioVideo));
        CHECK(ioctl(fd_video, VIDEO_PLAY,0));
        CHECK(ioctl(fd_audio,AUDIO_PLAY,0));
        }
     PESPacket2TS(Data, Length, Id, playTsCcounter[1]);
     int r = safe_write(fd_playDvr, playBuffer, playBufferFill);
     return r >= 0 ? Length : r;
     }

  return PlayAudioOnly(Data, Length, Id);
}

int cDvbDevice::PlayAudioOnly(const uchar *Data, int Length, uchar Id)
{
  int t1, t2;
  int pay_off = PESPayload(Data,Length,t1,t2);
  if (Id != playAudioId) {
     playBufferFill = 0;
     if ((Id == 0xbd) | ((Id & 0xf0) == 0x80)) {
        esyslog("Cannot play AC3 in audio only mode!");
        errno = EOPNOTSUPP;
        return -1;
        }
     CHECK(ioctl(fd_audio,AUDIO_STOP,0));
     if ((Id & 0xf0) == 0xa0) {
        if (pay_off + 7 > Length) {
           esyslog("Invalid LPCM packet");
           errno = EINVAL;
           return -1;
           }
        static const int bps_code[] = { 16, 20, 24, 32};
        static const int sr_code[] = { 48000, 96000, 44100, 32000};
        int bits_per_sample = bps_code[(Data[pay_off + 5] >> 6) & 0x3];
        int sample_rate = sr_code[(Data[pay_off + 5] >> 4) & 0x3];
        int channels = (Data[pay_off + 5] & 0xf) + 1;
        if ((bits_per_sample != 16) | (channels != 2)) {
           esyslog("Unsupported LPCM format");
           errno = EINVAL;
           return -1;
           }
        CHECK(ioctl(fd_audio,AUDIO_SET_STREAMTYPE,AUDIO_CAP_LPCM));
        CHECK(ioctl(fd_audio,AUDIO_SET_SAMPLE_RATE, sample_rate));
        }
     else {
        if (pay_off + 4 > Length) {
           esyslog("Invalid mpeg-audio packet");
           errno = EINVAL;
           return -1;
           }
        static const int sr_code[] = { 44100, 48000, 32000, 0};
        static const int stream_type[] = { AUDIO_CAP_MP1, AUDIO_CAP_MP2, AUDIO_CAP_MP3 };
        uint32_t header = BE2HOST(get_unaligned((uint32_t *)(Data + pay_off)));
        int id = 1 - ((header >> 19) & 1);
        int layer = 3 - ((header >> 17) & 3);
        int sample_rate = sr_code[(header >> 10) & 3] >> id;
        if ((((header >> 20) & 0xfff) != 0xfff) | (layer < 0) | (sample_rate <= 0)) {
           esyslog("Invalid mpeg-audio frame header at start of pes-packet");
           errno = EINVAL;
           return -1;
           }
        CHECK(ioctl(fd_audio,AUDIO_SET_STREAMTYPE, stream_type[layer]));
        CHECK(ioctl(fd_audio,AUDIO_SET_SAMPLE_RATE, sample_rate));
        }
     CHECK(ioctl(fd_audio,AUDIO_PLAY,0));
     playAudioId = Id;
     }

  if ((Id & 0xf0) == 0xa0)
     pay_off += 7;

  const int cur_data_length = Length - pay_off;
  const uchar *write_data = Data + pay_off;
  int write_length = cur_data_length;

  if (cur_data_length <= 0) {
     esyslog("Invalid audio packet");
     errno = EINVAL;
     return -1;
  }

  void *(*fp_memcpy)(void *,const void *, size_t) = &memcpy;
  if (playBufferFill) {
     if (playBufferFill + cur_data_length > KILOBYTE(2*65)) {
        esyslog("To much unplayed audio data");
        errno = EOVERFLOW;
        return -1;
        }
     memcpy(playBuffer + playBufferFill, write_data, cur_data_length);
     write_data = playBuffer;
     write_length += playBufferFill;
     fp_memcpy = &memmove;
     }

  int new_errno = 0;
  int ret = Length;
  if(write_length>=KILOBYTE(3))
  {
  int r;
  do {
     r = write(fd_audio, write_data, write_length);
     if (r < 0) {
        if ((errno != EAGAIN) & (errno != EBUSY) & (errno != EPERM) &
            (errno != EINTR))
           return -1;

        if (errno != EINTR) {
           if (errno == EPERM)
              dsyslog("Got EPERM while playing audio only");
           /* Don't retry to fast -- as it causes high cpu load */
           cCondWait::SleepMs(3);
           errno = EAGAIN;
           return -1;
           }
        }
     } while (r <= 0);

  write_data += r;
  write_length -= r;

  if (write_length >= cur_data_length) {
     write_length -= cur_data_length;
     new_errno = EAGAIN;
     ret = -1;
     }
  }

  playBufferFill = write_length;
  if (write_length > 0)
     fp_memcpy(playBuffer, write_data, write_length);

  errno = new_errno;
  return ret; 
}


// In orginal 2 MBs are used as buffer.
// Due to restrictions of mmaping on the dvr-device
// Ring-Buffer-Size is know fixed to 1536 KBs.
bool cDvbDevice::OpenDvr(void)
{
  CloseDvr();
  fd_dvr = DvbOpen(DEV_DVB_ADAPTER DEV_DVB_DVR, CardIndex(), O_RDWR | O_NONBLOCK , true);
  if (fd_dvr >= 0) {
     tsBuffer = new c7x0TSBuffer(fd_dvr, CardIndex() + 1);
     if (!tsBuffer || !tsBuffer->Okay()) {
        delete tsBuffer;
        tsBuffer = NULL;
        close(fd_dvr);
        fd_dvr = -1;
        }
     }
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (pidHandles[ptRecVideo].pid)
     tsBuffer->UseFrameEvents(true);
#endif

  return fd_dvr >= 0;
}
//M7X0 END AK

void cDvbDevice::CloseDvr(void)
{
  if (fd_dvr >= 0) {
     delete tsBuffer;
     tsBuffer = NULL;
     close(fd_dvr);
     fd_dvr = -1;
     }
}

//M7X0 BEGIN AK
// Seems to perform much better, if more than one ts packet (with the same PID)
// is deliviered in one call
#ifdef USE_HW_VIDEO_FRAME_EVENTS
bool cDvbDevice::GetTSPackets(uchar *&Data, int &Length, int &Pid, eTsVideoFrame &videoFrame)
#else
bool cDvbDevice::GetTSPackets(uchar *&Data, int &Length, int &Pid)
#endif
{
  if (tsBuffer) {
#ifdef USE_HW_VIDEO_FRAME_EVENTS
     Data = tsBuffer->Get(Length, Pid, videoFrame);
#else
     Data = tsBuffer->Get(Length, Pid);
#endif
     return true;
     }
  return false;
}
//M7X0 END AK

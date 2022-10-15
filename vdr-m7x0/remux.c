/*
 * remux.c: A streaming MPEG2 remultiplexer
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * The parts of this code that implement cTS2PES have been taken from
 * the Linux DVB driver's 'tuxplayer' example and were rewritten to suit
 * VDR's needs.
 *
 * The cRepacker family's code was originally written by Reinhard Nissl <rnissl@gmx.de>,
 * and adapted to the VDR coding style by kls@tvdr.de.
 *
 */

#include "remux.h"
#include <stdlib.h>
#include "channels.h"
#include "device.h"
#include "libsi/si.h"
#include "libsi/section.h"
#include "libsi/descriptor.h"
#include "shutdown.h"
#include "tools.h"

// Set these to 'true' for debug output:
static bool DebugPatPmt = false;

#define dbgpatpmt(a...) if (DebugPatPmt) fprintf(stderr, a)

//M7X0 BEGIN AK
ePesHeader AnalyzePesHeader(const uchar *Data, int Count, int &PesPayloadOffset, bool *ContinuationHeader)
{
  if (Count < 7)
     return phNeedMoreData; // too short

  if ((Data[6] & 0xC0) == 0x80) { // MPEG 2
     if (Count < 9)
        return phNeedMoreData; // too short

     PesPayloadOffset = 6 + 3 + Data[8];
     if (Count < PesPayloadOffset)
        return phNeedMoreData; // too short

     if (ContinuationHeader)
        *ContinuationHeader = (!(Data[7]&0xC0));
             // Better only check for PTS/DTS,
             // otherwise a header with i.e. orginal or copy flag set
             // replaces the ones with timestamps

     return phMPEG2; // MPEG 2
     }

  // check for MPEG 1 ...
  PesPayloadOffset = 6;

  // skip up to 16 stuffing bytes
  for (int i = 0; i < 16; i++) {
      if (Data[PesPayloadOffset] != 0xFF)
         break;

      if (Count <= ++PesPayloadOffset)
         return phNeedMoreData; // too short
      }

  // skip STD_buffer_scale/size
  if ((Data[PesPayloadOffset] & 0xC0) == 0x40) {
     PesPayloadOffset += 2;

     if (Count <= PesPayloadOffset)
        return phNeedMoreData; // too short
     }

  if (ContinuationHeader)
     *ContinuationHeader = false;

  if ((Data[PesPayloadOffset] & 0xF0) == 0x20) {
     // skip PTS only
     PesPayloadOffset += 5;
     }
  else if ((Data[PesPayloadOffset] & 0xF0) == 0x30) {
     // skip PTS and DTS
     PesPayloadOffset += 10;
     }
  else if (Data[PesPayloadOffset] == 0x0F) {
     // continuation header
     PesPayloadOffset++;

     if (ContinuationHeader)
        *ContinuationHeader = true;
     }
  else
     return phInvalid; // unknown

  if (Count < PesPayloadOffset)
     return phNeedMoreData; // too short

  return phMPEG1; // MPEG 1
}



// --- cRepacker -------------------------------------------------------------

#define MIN_LOG_INTERVAL 10 // min. # of seconds between two consecutive log messages of a cRepacker
#define LOG(a...) (LogAllowed() && (esyslog(a), true))

//XXX TODO: these should really be available in some driver header file!
#define PROG_STREAM_MAP  0xBC
#ifndef PRIVATE_STREAM1
#define PRIVATE_STREAM1  0xBD
#endif
#define PADDING_STREAM   0xBE
#ifndef PRIVATE_STREAM2
#define PRIVATE_STREAM2  0xBF
#endif
#define AUDIO_STREAM_S   0xC0
#define AUDIO_STREAM_E   0xDF
#define VIDEO_STREAM_S   0xE0
#define VIDEO_STREAM_E   0xEF
#define ECM_STREAM       0xF0
#define EMM_STREAM       0xF1
#define DSM_CC_STREAM    0xF2
#define ISO13522_STREAM  0xF3
#define PROG_STREAM_DIR  0xFF

//pts_dts flags
#define PTS_ONLY         0x80

#define TS_SIZE        188
#define PID_MASK_HI    0x1F
#define CONT_CNT_MASK  0x0F

// Flags:
#define PAY_LOAD       0x10
#define ADAPT_FIELD    0x20
#define PAY_START      0x40
#define TS_ERROR       0x80

#define MAX_PLENGTH  0xFFFF          // the maximum PES packet length (theoretically)
#define MMAX_PLENGTH (64*MAX_PLENGTH) // some stations send PES packets that are extremely large, e.g. DVB-T in Finland or HDTV 1920x1080

#define IPACKS 2048

// Start codes:
#define SC_SEQUENCE 0xB3  // "sequence header code"
#define SC_GROUP    0xB8  // "group start code"
#define SC_PICTURE  0x00  // "picture start code"

#define MAXNONUSEFULDATA (10*1024*1024)
#define MAXNUMUPTERRORS  10

class cRepacker {
private:
  int pid;
  int tsErrors;
  int ccErrors;
  int ccCounter;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  inline void HandleInputPesData(uchar *Data, int Count,const bool PacketStart, const eTsVideoFrame videoFrame) __attribute__ ((always_inline));
#else
  inline void HandleInputPesData(uchar *Data, int Count,const bool PacketStart) __attribute__ ((always_inline));
#endif
protected:
  bool initiallySyncing;
  int maxPacketSize;
  uint8_t streamType;
  uint8_t streamId;
  uint8_t subStreamId;

  int skippedBytes;
  int packetTodo;
  uchar *fragmentData; //[6 + 65535 + 4];
  int fragmentLen;
  uchar pesHeaderBackup[6 + 3 + 255];
  int pesHeaderBackupLen;
  uint32_t scanner;


  sPesResult curPacketDataHeader;
  cRingBufferResult *resultBuffer;


  ePesHeader mpegLevel;
  int inputPacketLength;
  int inputPacketDone;
  bool inputPesHeaderDataNeed;
  bool inputPacketIllegal;
  uchar inputPesHeaderBackup[6 + 3 + 255];
  int inputPesHeaderBackupLen;

  time_t lastLog;
  int suppressedLogMessages;
  bool LogAllowed(void);
  void DroppedData(const char *Reason, int Count) { LOG("%s (dropped %d bytes)", Reason, Count); }

  inline void CreatePesHeader(const bool ContinuationHeader, const bool UseCurrentHeader = false) __attribute__ ((always_inline));


  inline int Put(const uchar *const Data, const int Count,const int CapacityNeeded, const sPesResult *Header = NULL) __attribute__ ((always_inline));

  inline bool PushOutPacket(const uchar *const Data, const int Count) __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual void Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame) = 0;
#else
  virtual void Repack(uchar *Data, int Count, const bool PacketStart) = 0;
#endif
public:

  cRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId = 0);
  virtual ~cRepacker() {
     if (tsErrors || ccErrors)
        dsyslog("cRepacker got %d TS errors, %d TS continuity errors (Pid 0x%04X)", tsErrors, ccErrors,pid);
     delete[] fragmentData;
     }
  virtual void Reset(void) {
     initiallySyncing = true;
     tsErrors = 0;
     ccErrors = 0;
     ccCounter = -1;
     skippedBytes = 0;
     fragmentLen = 0;
     pesHeaderBackupLen = 0;
     mpegLevel = phInvalid;
     inputPacketLength = 0;
     inputPacketDone = 0;
     inputPesHeaderDataNeed = false;
     inputPacketIllegal = true;
     inputPesHeaderBackupLen = 0;
     }
  int Pid(void) { return pid; }
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  void ts_to_pes(uchar *Buf,const eTsVideoFrame videoFrame); // don't need count (=188)
#else
  void ts_to_pes(uchar *Buf); // don't need count (=188)
#endif
  void Clear(void) { Reset(); }
  };


cRepacker::cRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId)
{
  pid = Pid;
  resultBuffer = ResultBuffer;
  maxPacketSize = MaxPacketSize;
  streamId = StreamId;
  subStreamId = SubStreamId;
  suppressedLogMessages = 0;;
  lastLog = 0;
  fragmentData = new uchar[maxPacketSize + 4];

  if ((streamId & 0xF0) == VIDEO_STREAM_S)
     streamType = VIDEO_STREAM_S;
  else if (streamId >= AUDIO_STREAM_S && streamId <= AUDIO_STREAM_E)
     streamType = AUDIO_STREAM_S;
  else
     streamType = streamId;

  if (MaxPacketSize < 6 + 3 + 255 + 4 + 4 + 184)
     esyslog("ERROR: Repackers may not work correct if packet size is smaller than 456 Bytes");
  Reset();
}

bool cRepacker::LogAllowed(void)
{
  bool Allowed = time(NULL) - lastLog >= MIN_LOG_INTERVAL;
  lastLog = time(NULL);
  if (Allowed) {
     if (suppressedLogMessages) {
        esyslog("%d cRepacker messages suppressed", suppressedLogMessages);
        suppressedLogMessages = 0;
        }
     }
  else
     suppressedLogMessages++;
  return Allowed;
}

#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cRepacker::HandleInputPesData(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame)
#else
void cRepacker::HandleInputPesData(uchar *Data, int Count, const bool PacketStart)
#endif
{
  if (!PacketStart) {
     if (inputPacketIllegal)
        return;

     if (!inputPesHeaderDataNeed) {
#ifdef USE_HW_VIDEO_FRAME_EVENTS
        Repack(Data, Count, false, videoFrame);
#else
        Repack(Data, Count, false);
#endif
        inputPacketDone += Count;
        return;
        }
     }
  else {
     // We have some error cases here. In all case stream has errors:
     // 1. Some data of last packet are lost. There is nothing we can do about this
     // 2. We have lost the start of a new packet. In this case the last packet gets
     //    overfilled. We may break at end of packet, but we would lost much more data.
     //    This should be only a problem if the packet-start we missed should be skipped.
     if (inputPacketLength > 6) {
        if (inputPacketDone < inputPacketLength)
           esyslog("PES packet for stream 0x%hhX  shortened by %d bytes (expected: %d bytes)", streamId, inputPacketLength - inputPacketDone, inputPacketLength);
        if (inputPacketDone > inputPacketLength)
            esyslog("Overfull PES packet for stream 0x%hhX  found. Excedes by %d bytes (expected: %d bytes)", streamId, inputPacketDone - inputPacketLength, inputPacketLength);
        }

     if (inputPesHeaderBackupLen) {
        // This may happen if startcode get spilt across packet bounds.
        // Should be very rare.
        // N.B. All backuped headers contains timestamps
        if (pesHeaderBackupLen)
           esyslog("PES header backup for stream 0x%hhX not used!",streamId);
        pesHeaderBackupLen = 0;
        if (!inputPesHeaderDataNeed) {
           memcpy(pesHeaderBackup, inputPesHeaderBackup, inputPesHeaderBackupLen);
           pesHeaderBackupLen = inputPesHeaderBackupLen;
           }
        inputPesHeaderBackupLen = 0;
        }

     inputPesHeaderDataNeed = true;
     inputPacketLength = 0;
     inputPacketDone = 0;
     inputPacketIllegal = false;

     if (Count >= 7) {
        const int sid = Data[3];
        if ((Data[0] | Data[1] | (Data[2] - 1)) ||
            (streamType == VIDEO_STREAM_S && (sid & 0xF0) != VIDEO_STREAM_S) ||
            (streamType == AUDIO_STREAM_S && (sid & 0xE0) != AUDIO_STREAM_S) ||
            (streamType == PRIVATE_STREAM1 && sid != PRIVATE_STREAM1)) {
           esyslog("PES packet not starting or wrong type (0x%d) in stream 0x%hhX", sid, streamId);
           inputPacketIllegal = true;
           return;
           }

        inputPacketLength = ((Data[4] << 8) | Data[5]) + 6;

        int PayloadOffset = 0;
        bool ContinuationHeader = false;
        mpegLevel = AnalyzePesHeader(Data, Count, PayloadOffset, &ContinuationHeader);

        if (mpegLevel > phInvalid) {
           inputPesHeaderDataNeed = false;
           inputPacketDone = PayloadOffset;
           Count -= PayloadOffset;

           if (!ContinuationHeader) {
              inputPesHeaderBackupLen = PayloadOffset;
              memcpy(inputPesHeaderBackup, Data, PayloadOffset);
              inputPesHeaderBackup[3] = streamId;
              }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
           Repack(Data + PayloadOffset, Count, true, videoFrame);
#else
           Repack(Data + PayloadOffset, Count, true);
#endif

           inputPacketDone += Count;
           return;
           }

        if (mpegLevel == phInvalid) {
           inputPacketIllegal = true;
           esyslog("PES packet header illegal for stream 0x%hhX", streamId);
           return;
           }
        }

     inputPesHeaderBackupLen = Count;
     memcpy(inputPesHeaderBackup, Data, Count);
     return;
     }

  if (inputPesHeaderBackupLen < 7) {
     int c = std::min (7 - inputPesHeaderBackupLen, Count);
     memcpy(inputPesHeaderBackup + inputPesHeaderBackupLen, Data, c);
     inputPesHeaderBackupLen += c;
     Data += c;
     Count -= c;

     if (inputPesHeaderBackupLen < 7)
        return;
     }


  const int sid = inputPesHeaderBackup[3];
  if ((inputPesHeaderBackup[0] | inputPesHeaderBackup[1] | (inputPesHeaderBackup[2] - 1)) ||
      (streamType == VIDEO_STREAM_S && (sid & 0xF0) != VIDEO_STREAM_S) ||
      (streamType == AUDIO_STREAM_S && (sid & 0xE0) != AUDIO_STREAM_S) ||
      (streamType == PRIVATE_STREAM1 && sid != PRIVATE_STREAM1)) {
     esyslog("PES packet not starting or wrong type (0x%d) in stream 0x%hhX", sid, streamId);
     inputPacketIllegal = true;
     inputPesHeaderBackupLen = 0;
     return;
     }

  inputPesHeaderBackup[3] = streamId;
  inputPacketLength = ((inputPesHeaderBackup[4] << 8) | inputPesHeaderBackup[5]) + 6;

  int PayloadOffset = 0;
  bool ContinuationHeader = false;
  while ((mpegLevel = AnalyzePesHeader(inputPesHeaderBackup,
                    inputPesHeaderBackupLen, PayloadOffset,
                    &ContinuationHeader)) == phNeedMoreData && Count) {
        int c = std::max(std::min(PayloadOffset - inputPesHeaderBackupLen, Count),1);
        memcpy(inputPesHeaderBackup + inputPesHeaderBackupLen, Data, c);
        inputPesHeaderBackupLen += c;
        Count -= c;
        Data += c;
        }

  if (mpegLevel == phNeedMoreData)
     return;

  if (mpegLevel == phInvalid) {
     inputPacketIllegal = true;
     inputPesHeaderBackupLen = 0;
     esyslog("PES packet header illegal for stream 0x%hhX", streamId);
     return;
     }

  inputPesHeaderDataNeed = false;
  inputPacketDone = inputPesHeaderBackupLen;
  if (ContinuationHeader)
     inputPesHeaderBackupLen = 0;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  Repack(Data, Count, true, videoFrame);
#else
  Repack(Data, Count, true);
#endif
  inputPacketDone += Count;
}

void cRepacker::CreatePesHeader(const bool ContinuationHeader, const bool UseCurrentHeader)
{
  if (!ContinuationHeader && (pesHeaderBackupLen > 0  || UseCurrentHeader)) {
     // ISO 13818-1 says:
     // In the case of video, if a PTS is present in a PES packet header
     // it shall refer to the access unit containing the first picture start
     // code that commences in this PES packet. A picture start code commences
     // in PES packet if the first byte of the picture start code is present
     // in the PES packet.
     if (UseCurrentHeader) {
        memcpy(fragmentData, inputPesHeaderBackup, inputPesHeaderBackupLen);
        fragmentLen = inputPesHeaderBackupLen;
        inputPesHeaderBackupLen = 0;
        }
     else {
        memcpy(fragmentData, pesHeaderBackup, pesHeaderBackupLen);
        fragmentLen = pesHeaderBackupLen;
        }
     if (mpegLevel == phMPEG2)
        fragmentData[6] |= 0x04; // alignment-flag
     pesHeaderBackupLen = 0;
     }
  else {
     // ... otherwise create a continuation PES header
     *((int*) fragmentData) = 0x00000100 | streamId;

     if (mpegLevel == phMPEG1) {
        *((int*) (fragmentData + 4)) = 0x00000F00;
        fragmentLen = 7;
        }
     else {
        *((int*) (fragmentData + 4)) = ContinuationHeader?0x00008000:0x00008400; // alignment-flag
        fragmentData[8] = 0x00;
        fragmentLen = 9;
        }
     }
}

int cRepacker::Put(const uchar *const Data, const int Count, const int CapacityNeeded, const sPesResult *Header)
{
  if (CapacityNeeded >= Count && resultBuffer->Free() < CapacityNeeded) {
     esyslog("ERROR: possible result buffer overflow, dropped %d out of %d byte", CapacityNeeded, CapacityNeeded);
     return 0;
     }

  if (!resultBuffer->Put(Data, Count, Header)) {
     esyslog("ERROR: result buffer overflow, dropped %d out of %d byte", Count, Count);
     return 0;
     }
  return Count;

}

bool cRepacker::PushOutPacket(const uchar *const Data, const int Count)
{
  // enter packet length into PES header ...
  // determine PES packet payload
  int PacketLen = fragmentLen + Count - 6;
  fragmentData[ 4 ] = PacketLen >> 8;
  fragmentData[ 5 ] = PacketLen & 0xFF;
  // just skip packets with no payload
  int PesPayloadOffset = 0;
  if (AnalyzePesHeader(fragmentData, fragmentLen, PesPayloadOffset) <= phInvalid)
     LOG("cRepacker: invalid PES packet encountered in fragment buffer!");
  else if (6 + PacketLen <= PesPayloadOffset) {
     fragmentLen = 0;
     return true; // skip empty packet
     }
  // amount of data to put into result buffer: a negative Count value means
  // to strip off any partially contained start code.
  int Bite = fragmentLen + (Count >= 0 ? 0 : Count);
  // put data into result buffer

  int n = Put(fragmentData, Bite, 6 + PacketLen, &curPacketDataHeader);

  fragmentLen = 0;
  if (n != Bite)
     return false;

  // append further payload
  if (Count > 0) {
     // put data into result buffer
     n = Put(Data,Count, Count);
     if (n != Count)
        return false;
     }

  // we did it ;-)
  // ack ;)

  curPacketDataHeader.pictureType = NO_PICTURE;

  return true;
}
#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cRepacker::ts_to_pes(uchar *Buf, const eTsVideoFrame videoFrame) // don't need count (=188)
#else
void cRepacker::ts_to_pes(uchar *Buf) // don't need count (=188)
#endif
{

  if (Buf[1] & TS_ERROR)
     tsErrors++;

  if (!(Buf[3] & PAY_LOAD))
     return; // discard TS packet with adaption_field_control set to '00'.
             // Why not checking for Payload only?
             // Packets with adaption field only are not used anyway.
             // Makes thinks a little easier (AK)

  if ((Buf[3] ^ ccCounter) & CONT_CNT_MASK) {
     // This should check duplicates and packets which do not increase the counter.
     // But as the errors usually come in bursts this should be enough to
     // show you there is something wrong with signal quality.
     if (ccCounter != -1 && ((Buf[3] ^ (ccCounter + 1)) & CONT_CNT_MASK)) {
        ccErrors++;
        // Enable this if you are having problems with signal quality.
        // These are the errors I used to get with Nova-T when antenna
        // was not positioned correcly (not transport errors). //tvr
        //dsyslog("TS continuity error (%d)", ccCounter);
        }
     ccCounter = Buf[3] & CONT_CNT_MASK;
     }
  else
     return; // We don't want to copy dublicate Packets (in error we have already lost 16 Packets) AK


  int off = 0;

  if (Buf[3] & ADAPT_FIELD) {  // adaptation field?
     off = Buf[4] + 1;
     if (off + 4 > 187)
        return;
     }

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  HandleInputPesData(Buf + 4 + off, TS_SIZE - 4 - off, Buf[1] & PAY_START, videoFrame);
#else
  HandleInputPesData(Buf + 4 + off, TS_SIZE - 4 - off, Buf[1] & PAY_START);
#endif
}
// --- cVideoRepacker --------------------------------------------------------

#define PICTURE_STARTING 8


class cVideoRepacker : public cRepacker {
private:
  enum eState {
    syncing,
    findPicture,
    scanPicture
    };
  int state;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  eTsVideoFrame videoFrameSave;
#endif

  int picTypeOffset;
  inline void PushOutVideoPacket(const uchar *const Data, const int Count)  __attribute__ ((always_inline));

  inline void HandleStartCode(const uchar *const Data, const uchar *&Payload, const bool UseCurrentHeader)  __attribute__ ((always_inline));

  inline bool ScanDataForStartCode(const uchar *&Data, const uchar *const Limit)  __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual void Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame);
#else
  virtual void Repack(uchar *Data, int Count, const bool PacketStart);
#endif
public:

  cVideoRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId = 0);

  virtual void Reset(void);

  };


cVideoRepacker::cVideoRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId)
: cRepacker(Pid, ResultBuffer, MaxPacketSize, StreamId, SubStreamId)
{
  Reset();
}

void cVideoRepacker::Reset(void)
{
  cRepacker::Reset();
  scanner = 0xFFFFFFFF;
  state = syncing;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  videoFrameSave = tsVideoFrameUnknown;
#endif

  curPacketDataHeader.pesPacketType = pesPtVideo;
  curPacketDataHeader.pictureType = NO_PICTURE;
  picTypeOffset = -1;
}


void cVideoRepacker::PushOutVideoPacket(const uchar *const Data, const int Count)
{
  // Check for not yet known picture type and read it
  if (picTypeOffset != -1) {
     if (picTypeOffset < fragmentLen)
        curPacketDataHeader.pictureType = (fragmentData[picTypeOffset] >> 3) & 0x07;
     else {
        const int i = picTypeOffset - fragmentLen;
        if (i < Count)
           curPacketDataHeader.pictureType = (Data[i] >> 3) & 0x07;
        else  // For safety should normaly not happen (this is very very rare).
           curPacketDataHeader.pictureType = I_FRAME;
                           // Only with sequence or gop header precceding the frame
                           // this may happen. So its almost always an i-frame
                           // (p-frames are allowed follwing a seq-header)
        }
     picTypeOffset = -1;
     }
  PushOutPacket(Data, Count);
}

void cVideoRepacker::HandleStartCode(const uchar *const Data, const uchar *&Payload, const bool UseCurrentHeader)
{

  if (state == scanPicture) {
     // the start codes indicate that the current picture is done. So
     // push out the packet to start a new packet for the next picuture. If
     // the byte count get's negative then the current buffer ends in a
     // partitial start code that must be stripped off, as it shall be put
     // in the next packet.

     if (packetTodo + 3 < 0) {
        int bite = Data + packetTodo - Payload;
             // This may get negative, if we have copied to much in the last
             // run. In this case data gets striped off.
             // The overcommited Data may be at maximum 2 bytes long in this case.
             // This data is saved in scanner and put before start of this run,
             // so we need not copy anything in here.
        PushOutVideoPacket(Payload,bite);

        CreatePesHeader(true);
        Payload += bite;
        }

     PushOutVideoPacket(Payload, Data - 3 - Payload);
     // go on with syncing to the next picture
     state = syncing;
     }

  if (state == syncing) {
     if (initiallySyncing) // omit report for the typical initial case
        initiallySyncing = false;
     else if (skippedBytes > 3) // report that syncing dropped some bytes
        LOG("cVideoRepacker: skipped %d bytes to sync on next picture", skippedBytes - 3);
     skippedBytes = 0;

     CreatePesHeader(false, UseCurrentHeader);


     // Scanner is copied befor payload of this run. So startcode
     // is allways fully present.
     Payload = Data - 3;
     // as there is no length information available, assume the
     // maximum we can hold in one PES packet
     packetTodo = maxPacketSize - fragmentLen - 3;
     // go on with finding the picture data
     state = findPicture;
     }

  if (*Data == 0x00) {
     state = scanPicture; // We do not need searching for slices start codes this way
                          // We checking this anyway. IMO searching for slices has no
                          // advantage. AK

     // curPacketDataHeader.pictureTypes[0] = PICTURE_STARTING;
     picTypeOffset = maxPacketSize - packetTodo + 2;
     }
}



bool cVideoRepacker::ScanDataForStartCode(const uchar *&Data, const uchar *const Limit)
{
  // Scanner is saved at Data - 4, 4 bytes are always free,
  // TS-Header is at least 4 bytes long.
  const uchar *data = Data;
  const uchar *const limit = Limit - 1;
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

#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cVideoRepacker::Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame)
#else
void cVideoRepacker::Repack(uchar *Data, int Count, const bool PacketStart)
#endif
{
  const uchar *const limit = Data + Count;
  uint32_t save_scan;
  // Store scanner at Data -4, we have always 4 bytes free from ts-header
  save_scan = get_unaligned((uint32_t *)(Data - 4));
  put_unaligned(scanner, (uint32_t *)(Data - 4));
  scanner = get_unaligned((uint32_t *)(limit - 4));

  const uchar *data = Data - 1; // 1 of startcode may be in scanner

 // remember start of the data
  const uchar *payload = Data;
  // remember start of handled data
  const uchar *data_save = Data;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (videoFrameSave <= tsVideoFrameNone) {
     videoFrameSave = videoFrame;
     if (videoFrameSave == tsVideoFrameNone && state == findPicture)
        videoFrameSave = tsVideoFrameUnknown;
     }
#endif

  // Search for startcodes and handle them.
  while (
#ifdef USE_HW_VIDEO_FRAME_EVENTS
           videoFrameSave != tsVideoFrameNone &&
#endif
           ScanDataForStartCode(data, limit)) {

        packetTodo -= data - data_save;
        //dsyslog("DEBUG: In with Events %d %d",videoFrameSave, videoFrame);
        if (state == syncing)
           skippedBytes += data - data_save;

        HandleStartCode(data, payload, inputPesHeaderBackupLen && inputPacketDone - inputPesHeaderBackupLen + (data - Data) >= 3);

        data_save = data;
        data += 3; // Next 1 of startcode  is at least 3 bytes away.

#ifdef USE_HW_VIDEO_FRAME_EVENTS
        if (videoFrameSave != tsVideoFrameUnknown && state == scanPicture) {
           // curPacketDataHeader.pictureType = videoFrameSave - tsVideoFrameNone;
           // state = scanPicture;
           videoFrameSave = tsVideoFrameUnknown;
           //break;
           }
#endif
        }

  if (state == syncing) {
     put_unaligned(save_scan, ((uint32_t *)(Data - 4)));
     skippedBytes += limit - data_save;
     return;
     }

  packetTodo -= limit - data_save;


  // Check if we need to spilt the packet.
  // But be careful there may be a partial startcode at end
  // of packet which should not be spilted.
  // In this cases copy the partial one and strip overcommited
  // data off in the next runns (normaly the next).
  // Case (| := marks the splitpoint):
  // Todo : Scanner     | Possible follwing : Result
  // -----:---------------------------------:------
  //      : XX 00 00 01 | YY XX XX          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Wait
  //  -2  :      |                          : Wait
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  // -----:---------------------------------:------
  //      : XX XX 00 00 | 01 YY XX          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Wait
  //  -2  :      |                          : Push
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  // -----:---------------------------------:------
  //      : XX XX XX 00 | 00 01 YY          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Push
  //  -2  :      |                          : Push
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  if (packetTodo <= 0 && (packetTodo < -2 ||
                           ((scanner & 0x00FFFFFF) != 0x00000001  &&
        (packetTodo < -1 || (scanner & 0x0000FFFF) != 0x00000000) &&
        (packetTodo <  0 || (scanner & 0x000000FF) != 0x00000000)))) {

     int bite = limit + packetTodo - payload;
             // This may get negative, if we have copied to much in the last
             // run. In this case data gets striped off.
             // The overcommited Data may be at maximum 2 bytes long in this case.
             // This data is saved in scanner and put before start of this run,
             // so we need not copy anything in here.


     PushOutVideoPacket(payload, bite);

     CreatePesHeader(true);
     payload = limit + packetTodo;
     packetTodo += maxPacketSize - fragmentLen;
     }

  // the packet is done. Now store any remaining data into fragment buffer
  // It may contain part of a start code at it's end,
  // which will be removed when the next packet gets processed.
  const int bite = limit - payload;
  if (bite > 0) {
     memcpy(fragmentData + fragmentLen, payload, bite);
     fragmentLen += bite;
     }
  put_unaligned(save_scan, ((uint32_t *)(Data - 4)));
}

// --- cHDVideoRepacker --------------------------------------------------------


class cHDVideoRepacker : public cRepacker {
private:
  enum eState {
    syncing,
    findPicture,
    scanPicture
    };
  int state;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  eTsVideoFrame videoFrameSave;
#endif

  int picTypeOffset;
  inline void PushOutVideoPacket(const uchar *const Data, const int Count)  __attribute__ ((always_inline));

  inline void HandleStartCode(const uchar *const Data, const uchar *&Payload, const bool UseCurrentHeader)  __attribute__ ((always_inline));

  inline bool ScanDataForStartCode(const uchar *&Data, const uchar *const Limit)  __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual void Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame);
#else
  virtual void Repack(uchar *Data, int Count, const bool PacketStart);
#endif
public:

  cHDVideoRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId = 0);

  virtual void Reset(void);

  int currentField;
  };


cHDVideoRepacker::cHDVideoRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId)
: cRepacker(Pid, ResultBuffer, MaxPacketSize, StreamId, SubStreamId)
{
  Reset();
}

void cHDVideoRepacker::Reset(void)
{
  cRepacker::Reset();
  scanner = 0xFFFFFFFF;
  state = syncing;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  videoFrameSave = tsVideoFrameUnknown;
#endif

  curPacketDataHeader.pesPacketType = pesPtVideo;
  curPacketDataHeader.pictureType = NO_PICTURE;
  picTypeOffset = -1;
  currentField=0;
}


void cHDVideoRepacker::PushOutVideoPacket(const uchar *const Data, const int Count)
{
  // Check for not yet known picture type and read it
  if (picTypeOffset != -1) {
     int pictureType;
     if (picTypeOffset < fragmentLen)
        pictureType = (fragmentData[picTypeOffset] >> 5) + 1;
     else {
        const int i = picTypeOffset - fragmentLen;
        if (i < Count)
           pictureType = (Data[i] >> 5) + 1;
        else {
           dsyslog("picTypeOffset - fragmentLen >= count ???");
           pictureType = I_FRAME;
           }
        }
//TODO - Progresive video
     if ((currentField >= 0)||((pictureType == I_FRAME)&&(currentField > -25))) {
        curPacketDataHeader.pictureType = pictureType;
        currentField--;
        }
     else
        currentField++;
     picTypeOffset = -1;
     }
  PushOutPacket(Data, Count);
}

void cHDVideoRepacker::HandleStartCode(const uchar *const Data, const uchar *&Payload, const bool UseCurrentHeader)
{

  if (state == scanPicture) {
     // the start codes indicate that the current picture is done. So
     // push out the packet to start a new packet for the next picuture. If
     // the byte count get's negative then the current buffer ends in a
     // partitial start code that must be stripped off, as it shall be put
     // in the next packet.

     if (packetTodo + 3 < 0) {
        int bite = Data + packetTodo - Payload;
             // This may get negative, if we have copied to much in the last
             // run. In this case data gets striped off.
             // The overcommited Data may be at maximum 2 bytes long in this case.
             // This data is saved in scanner and put before start of this run,
             // so we need not copy anything in here.
        PushOutVideoPacket(Payload,bite);

        CreatePesHeader(true);
        Payload += bite;
        }

     PushOutVideoPacket(Payload, Data - 3 - Payload);
     // go on with syncing to the next picture
     state = syncing;
     }

  if (state == syncing) {
     if (initiallySyncing) // omit report for the typical initial case
        initiallySyncing = false;
     else if (skippedBytes > 3) // report that syncing dropped some bytes
        LOG("cHDVideoRepacker: skipped %d bytes to sync on next picture", skippedBytes - 3);
     skippedBytes = 0;

     CreatePesHeader(false, UseCurrentHeader);


     // Scanner is copied befor payload of this run. So startcode
     // is allways fully present.
     Payload = Data - 3;
     // as there is no length information available, assume the
     // maximum we can hold in one PES packet
     packetTodo = maxPacketSize - fragmentLen - 3;
     // go on with finding the picture data
     state = findPicture;
     }

  if (*Data == 0x09) {
     state = scanPicture;
     picTypeOffset = maxPacketSize - packetTodo + 1;
     }
}



bool cHDVideoRepacker::ScanDataForStartCode(const uchar *&Data, const uchar *const Limit)
{
  // Scanner is saved at Data - 4, 4 bytes are always free,
  // TS-Header is at least 4 bytes long.
  const uchar *data = Data;
  const uchar *const limit = Limit - 1;
  while (data < limit)
        if (data[0] > 1)
           data += 3;
        else if (!data[0])
           data++;
        else {
           if (!(data[-2] | data[-1])) {
              const uchar code = *++data;
              if (code == 9) {
                 Data = data;
                 return true;
                 }
              }
           data += 3;
           }

  return false;
}

#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cHDVideoRepacker::Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame)
#else
void cHDVideoRepacker::Repack(uchar *Data, int Count, const bool PacketStart)
#endif
{
  const uchar *const limit = Data + Count;
  uint32_t save_scan;
  // Store scanner at Data -4, we have always 4 bytes free from ts-header
  save_scan = get_unaligned((uint32_t *)(Data - 4));
  put_unaligned(scanner, (uint32_t *)(Data - 4));
  scanner = get_unaligned((uint32_t *)(limit - 4));

  const uchar *data = Data - 1; // 1 of startcode may be in scanner

 // remember start of the data
  const uchar *payload = Data;
  // remember start of handled data
  const uchar *data_save = Data;

#ifdef USE_HW_VIDEO_FRAME_EVENTS
  if (videoFrameSave <= tsVideoFrameNone) {
     videoFrameSave = videoFrame;
     if (videoFrameSave == tsVideoFrameNone && state == findPicture)
        videoFrameSave = tsVideoFrameUnknown;
     }
#endif

  // Search for startcodes and handle them.
  while (
#ifdef USE_HW_VIDEO_FRAME_EVENTS
           videoFrameSave != tsVideoFrameNone &&
#endif
           ScanDataForStartCode(data, limit)) {

        packetTodo -= data - data_save;
        //dsyslog("DEBUG: In with Events %d %d",videoFrameSave, videoFrame);
        if (state == syncing)
           skippedBytes += data - data_save;

        HandleStartCode(data, payload, inputPesHeaderBackupLen && inputPacketDone - inputPesHeaderBackupLen + (data - Data) >= 3);

        data_save = data;
        data += 3; // Next 1 of startcode  is at least 3 bytes away.

#ifdef USE_HW_VIDEO_FRAME_EVENTS
        if (videoFrameSave != tsVideoFrameUnknown && state == scanPicture) {
           // curPacketDataHeader.pictureType = videoFrameSave - tsVideoFrameNone;
           // state = scanPicture;
           videoFrameSave = tsVideoFrameUnknown;
           //break;
           }
#endif
        }

  if (state == syncing) {
     put_unaligned(save_scan, ((uint32_t *)(Data - 4)));
     skippedBytes += limit - data_save;
     return;
     }

  packetTodo -= limit - data_save;


  // Check if we need to spilt the packet.
  // But be careful there may be a partial startcode at end
  // of packet which should not be spilted.
  // In this cases copy the partial one and strip overcommited
  // data off in the next runns (normaly the next).
  // Case (| := marks the splitpoint):
  // Todo : Scanner     | Possible follwing : Result
  // -----:---------------------------------:------
  //      : XX 00 00 01 | YY XX XX          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Wait
  //  -2  :      |                          : Wait
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  // -----:---------------------------------:------
  //      : XX XX 00 00 | 01 YY XX          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Wait
  //  -2  :      |                          : Push
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  // -----:---------------------------------:------
  //      : XX XX XX 00 | 00 01 YY          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Push
  //  -2  :      |                          : Push
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  if (packetTodo <= 0 && (packetTodo < -2 ||
                           ((scanner & 0x00FFFFFF) != 0x00000001  &&
        (packetTodo < -1 || (scanner & 0x0000FFFF) != 0x00000000) &&
        (packetTodo <  0 || (scanner & 0x000000FF) != 0x00000000)))) {

     int bite = limit + packetTodo - payload;
             // This may get negative, if we have copied to much in the last
             // run. In this case data gets striped off.
             // The overcommited Data may be at maximum 2 bytes long in this case.
             // This data is saved in scanner and put before start of this run,
             // so we need not copy anything in here.


     PushOutVideoPacket(payload, bite);

     CreatePesHeader(true);
     payload = limit + packetTodo;
     packetTodo += maxPacketSize - fragmentLen;
     }

  // the packet is done. Now store any remaining data into fragment buffer
  // It may contain part of a start code at it's end,
  // which will be removed when the next packet gets processed.
  const int bite = limit - payload;
  if (bite > 0) {
     memcpy(fragmentData + fragmentLen, payload, bite);
     fragmentLen += bite;
     }
  put_unaligned(save_scan, ((uint32_t *)(Data - 4)));
}



// --- cAudioRepacker --------------------------------------------------------

class cAudioRepacker : public cRepacker {
private:
  enum eState {
    syncing,
    scanFrame
    };
  int state;
  int frameTodo;
  int frameSize;
  int samples;
  int keyFrame;

  inline bool IsValidAudioHeader(void) __attribute__ ((always_inline));
  inline bool ScanDataForStartCode (const uchar *&Data, const uchar *const Limit) __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual void Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame);
#else
  virtual void Repack(uchar *Data, int Count, const bool PacketStart);
#endif
public:

  cAudioRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId = 0);

  virtual void Reset(void);

  };

cAudioRepacker::cAudioRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId )
: cRepacker(Pid, ResultBuffer, MaxPacketSize, StreamId, SubStreamId)
{
  Reset();
}

void cAudioRepacker::Reset(void)
{
  cRepacker::Reset();
  scanner = 0;
  state = syncing;
  frameTodo = 0;
  frameSize = 0;
  samples=0;
  keyFrame=0;


  curPacketDataHeader.pesPacketType = pesPtAudio;
  curPacketDataHeader.pictureType = NO_PICTURE;

}


bool cAudioRepacker::IsValidAudioHeader(void)
{
  static const int keyFrames[2][3] = {  // packets*576 per keyFrame
        { 2646, 960, 640 }, // MPEG 1 / Layer 1,2,3
        { 1323, 576, 576 }  // MPEG 2 / Layer 1,2,3
  };

  static const int slots[2][3][3][14] = {
    {
      {
        {    8,   17,   26,   34,   43,   52,   60,   69,   78,   87,   95,  104,  113,  121 },
        {  104,  156,  182,  208,  261,  313,  365,  417,  522,  626,  731,  835, 1044, 1253 },
        {  104,  130,  156,  182,  208,  261,  313,  365,  417,  522,  626,  731,  835, 1044 }
      }, {
        {    8,   16,   24,   32,   40,   48,   56,   64,   72,   80,   88,   96,  104,  112 },
        {   96,  144,  168,  192,  240,  288,  336,  384,  480,  576,  672,  768,  960, 1152 },
        {   96,  120,  144,  168,  192,  240,  288,  336,  384,  480,  576,  672,  768,  960 }
      }, {
        {   12,   24,   36,   48,   60,   72,   84,   96,  108,  120,  132,  144,  156,  168 },
        {  144,  216,  252,  288,  360,  432,  504,  576,  720,  864, 1008, 1152, 1440, 1728 },
        {  144,  180,  216,  252,  288,  360,  432,  504,  576,  720,  864, 1008, 1152, 1440 }
      }
    }, {
      {
        {   17,   26,   30,   34,   43,   52,   60,   69,   78,   87,   95,  104,  121,  139 },
        {   52,  104,  156,  208,  261,  313,  365,  417,  522,  626,  731,  835,  940, 1044 },
        {   26,   52,   78,  104,  130,  156,  182,  208,  261,  313,  365,  417,  470,  522 }
      }, {
        {   16,   24,   28,   32,   40,   48,   56,   64,   72,   80,   88,   96,  112,  128 },
        {   48,   96,  144,  192,  240,  288,  336,  384,  480,  576,  672,  768,  864,  960 },
        {   24,   48,   72,   96,  120,  144,  168,  192,  240,  288,  336,  384,  432,  480 }
      }, {
        {   24,   36,   42,   48,   60,   72,   84,   96,  108,  120,  132,  144,  168,  192 },
        {   72,  144,  216,  288,  360,  432,  504,  576,  720,  864, 1008, 1152, 1296, 1440 },
        {   36,   72,  108,  144,  180,  216,  252,  288,  360,  432,  504,  576,  648,  720 }
      }
    }
  };

  if ((scanner & 0xFFF00000) != 0xFFF00000) // syncword
     return false;

  const int mpegIndex = 1 - ((scanner >> 19) & 1);

  const int layer = 3 - ((scanner >> 17) & 3);
  if (layer == 3)  // reserved
     return false;

  const int bitrate_index = ((scanner >> 12) & 0xF) - 1;
  if (bitrate_index == 0xF - 1)  // forbidden
     return 0;

  const int sampling_frequency = (scanner >> 10) & 3;
  if (sampling_frequency == 3) // reserved
     return false;

  const int padding_bit = (scanner >>  9) & 1;

  if ((scanner & 0x00000003) == 2) // emphasis 2 reserved
     return false;

  keyFrame=keyFrames[mpegIndex][layer];

  if (bitrate_index == -1) {
     frameSize = 0;
     return true;
     }

  frameSize = slots[mpegIndex][sampling_frequency][layer][bitrate_index] + padding_bit;

  if (layer == 0)
     frameSize *= 4;

  return true;

/*
slots is precalculated version of the normal calculation of frameSize as follows
for illeagle values -1 is used.

int cAudioRepacker::bitRates[2][3][16] = { // all values are specified as kbits/s
  {
    { 0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, -1 }, // MPEG 1, Layer I
    { 0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, -1 }, // MPEG 1, Layer II
    { 0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, -1 }  // MPEG 1, Layer III
  },
  {
    { 0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, -1 }, // MPEG 2, Layer I
    { 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, -1 }, // MPEG 2, Layer II/III
    { 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, -1 }  // MPEG 2, Layer II/III
  }
  };

int cAudioRepacker::samplingFrequencies[2][4] = { // all values are specified in Hz
  { 44100, 48000, 32000, -1 }, // MPEG 1
  { 22050, 24000, 16000, -1 }  // MPEG 2
  };

int cAudioRepacker::slotsMult[2][3] = {
  { 12000, 144000, 144000 }, // MPEG 1, Layer I, II, III
  { 12000, 144000,  72000 }  // MPEG 2, Layer I, II, III
  };

  const int syncword           = (scanner >> 20) & (0xFFF00000 >> 20);
  if (syncword != 0xFFF) // syncword
     return false;

  int id                 = (scanner >> 19) & (0x00080000 >> 19);
  if (id == 0 && mpegLevel == phMPEG1) // reserved in MPEG 1
     return false;

  int layer              = (scanner >> 17) & (0x00060000 >> 17);
  if (layer == 0) // reserved
     return false;

//int protection_bit     = (scanner >> 16) & (0x00010000 >> 16);

  int bitrate_index      = (scanner >> 12) & (0x0000F000 >> 12);
  if (bitrate_index == 0xF) // forbidden
     return false;

  int sampling_frequency = (scanner >> 10) & (0x00000C00 >> 10);
  if (sampling_frequency == 3) // reserved
     return false;

  int padding_bit        = (scanner >>  9) & (0x00000200 >>  9);
//int private_bit        = (scanner >>  8) & (0x00000100 >>  8);
//int mode               = (scanner >>  6) & (0x000000C0 >>  6);
//int mode_extension     = (scanner >>  4) & (0x00000030 >>  4);
//int copyright          = (scanner >>  3) & (0x00000008 >>  3);
//int orignal_copy       = (scanner >>  2) & (0x00000004 >>  2);

  const int emphasis           = (scanner & 0x00000003);
  if (emphasis == 2) // reserved
     return false;


  if (bitrate_index == 0)
     frameSize = 0;
  else {
     int mpegIndex = 1 - id;
     int layerIndex = 3 - layer;

     // Layer I (i. e., layerIndex == 0) has a larger slot size
     int slotSize = (layerIndex == 0) ? 4 : 1; // bytes

     int br = bitRates[mpegIndex][layerIndex][bitrate_index]; // bits/s
     int sf = samplingFrequencies[mpegIndex][sampling_frequency];

     int N = slotsMult[mpegIndex][layerIndex] * br / sf; // slots

     frameSize = (N + padding_bit) * slotSize; // bytes
     }
  return true;
  */
}

bool cAudioRepacker::ScanDataForStartCode (const uchar *&Data, const uchar *const Limit)
{
  const uchar *data = Data;
  if (!frameTodo && Limit - data >= 4) {
     scanner = getIntUnaligned(data);
     data += 4;
     if (IsValidAudioHeader()) {
        Data = data;
        return true;
        }
     }

  while (data < Limit ) {
        scanner <<= 8;
        scanner  |= *data;
        data++;
        if (IsValidAudioHeader()) {
           Data = data;
           return true;
           }
        }
   frameTodo = -1;
   // Data = data;
   return false;
}
#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cAudioRepacker::Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame)
#else
void cAudioRepacker::Repack(uchar *Data, int Count, const bool PacketStart)
#endif
{
  const uchar *data = Data;
  // remember start of the data
  const uchar *payload = Data;
  const uchar *data_save = Data;
  const uchar *const limit = Data + Count;

  while (data < limit) {
        if (frameSize) {
           const int bite = std::min(limit - data, std::min(frameTodo, packetTodo));
           packetTodo -= bite;
           frameTodo -= bite;

           if (packetTodo && frameTodo) {
              // All remaining data contains to current frame
              // And frame is not fully present
              memcpy(fragmentData + fragmentLen, data, bite);
              fragmentLen += bite;
              return;
              }

           PushOutPacket(data, bite);
           data += bite;
           payload = data;

           if (!frameTodo) {
              state = syncing;
              frameSize = 0;
              scanner = 0;
              continue;
              }

           CreatePesHeader(true);
           packetTodo = maxPacketSize - fragmentLen;
           continue;
           }

        data_save = data;
        if (!ScanDataForStartCode(data, limit))
           break; // No frame starting

        // Frame starting
        if (state == syncing)
           skippedBytes += data - data_save;
        else {
           packetTodo -= data - 4 - data_save;

           if (packetTodo < 0) {
              int bite = data - 4 + packetTodo - payload;
              const uchar *excessData = fragmentData + fragmentLen + bite;

              PushOutPacket(payload,bite);

              CreatePesHeader(true);

              while (bite++ < 0) {
                    // append the excess data here
                    fragmentData[fragmentLen++] = *excessData++;
                    }

              payload += bite - 1;
              }
           PushOutPacket(payload,data - 4 - payload);
           }


        if (initiallySyncing) // omit report for the typical initial case
           initiallySyncing = false;
        else if (skippedBytes > 4) // report that syncing dropped some bytes
            LOG("cAudioRepacker(0x%02hhX): skipped %d bytes to sync on next audio frame", streamId, skippedBytes - 4);
        skippedBytes = 0;

        CreatePesHeader(false,  inputPesHeaderBackupLen && inputPacketDone - inputPesHeaderBackupLen + (data - Data) >= 4);

        putIntUnaligned(fragmentData + fragmentLen, scanner);
        scanner = 0;
        fragmentLen += 4;
        packetTodo = maxPacketSize - fragmentLen;
        payload = data;

        frameTodo = frameSize -4;

	if(samples>=0)
	{
		curPacketDataHeader.pictureType=I_FRAME;
		samples+=576-keyFrame;
	}
	else
	{
		curPacketDataHeader.pictureType = NO_PICTURE;
		samples+=576;
	}

        state = scanFrame;
        }

  // collect number of skipped bytes while syncing
  if (state == syncing) {
     skippedBytes += limit - data_save;
     return;
     }

  packetTodo -= limit - data_save;

  // Check if we need to spilt the packet.
  // But be careful there may be a partial startcode at end
  // of packet which should not be spilted.
  // In this cases copy the partial one and strip overcommited
  // data off in the next runns (normaly the next).
  // Case (| := marks the splitpoint):
  // Todo : Scanner     | Possible follwing : Result
  // -----:---------------------------------:------
  //      : XX FF FY YY | YY XX XX          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Wait
  //  -2  :      |                          : Wait
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  // -----:---------------------------------:------
  //      : XX XX FF FY | YY YY XX          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Wait
  //  -2  :      |                          : Push
  //  -3  :   |                             : Push
  //   <  :                                 : Push
  // -----:---------------------------------:------
  //      : XX XX XX FF | FY YY YY          :
  //   0  :             |                   : Wait
  //  -1  :         |                       : Push
  //  -2  :      |                          : Push
  //  -3  :   |                             : Push
  //   <  :                                 : Push

  if (packetTodo <= 0 &&
       ((scanner & 0x00FFF000) != 0x00FFF000 || packetTodo < -2) &&
       ((scanner & 0x0000FFF0) != 0x0000FFF0 || packetTodo < -1) &&
       ((scanner & 0x000000FF) != 0x000000FF || packetTodo <  0)) {

     int bite = limit + packetTodo - payload;
     const uchar *excessData = fragmentData + fragmentLen + bite;
     PushOutPacket(payload, bite);

     CreatePesHeader(true);

     while (bite++ < 0) {
          // append the excess data here
           fragmentData[fragmentLen++] = *excessData++;
           packetTodo++;
           }
     payload += bite - 1;
     packetTodo += maxPacketSize - fragmentLen;
     }

  // The packet is done. Now store any remaining data into fragment buffer
  // if we are no longer syncing. Loop returns if we are syncing
  // It may contain part of an audio frame header at it's
  // end, which will be removed when the next packet gets processed.
  const int bite = limit - payload;
  if (bite > 0) {
     memcpy(fragmentData + fragmentLen, payload, bite);
     fragmentLen += bite;
     }

}

// --- cDolbyRepacker --------------------------------------------------------

class cDolbyRepacker : public cRepacker {
private:
  static const int frameSizes[];
  enum eState {
    syncing,
    scanFrame
    };
  int frameSize;
  int frameTodo;
  int state;

  inline void AppendSubStreamHeader(bool ContinuationFrame = false)  __attribute__ ((always_inline));
  inline bool ScanDataForStartCode(const uchar *&Data, const uchar *Limit)  __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual void Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame);
#else
  virtual void Repack(uchar *Data, int Count, const bool PacketStart);
#endif
public:

  cDolbyRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId = 0);

  virtual void Reset(void);

  };

// frameSizes are in bytes
const int cDolbyRepacker::frameSizes[] = {
  // fs = 48 kHz
   128,  128,  160,  160,  192,  192,  224,  224,  256,  256,  320,  320,  384,  384,  448,  448,
   512,  512,  640,  640,  768,  768,  896,  896, 1024, 1024, 1280, 1280, 1536, 1536, 1792, 1792,
  2048, 2048, 2304, 2304, 2560, 2560,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  // fs = 44.1 kHz
   138,  140,  174,  176,  208,  210,  242,  244,  278,  280,  348,  350,  416,  418,  486,  488,
   556,  558,  696,  698,  834,  836,  974,  976, 1114, 1116, 1392, 1394, 1670, 1672, 1950, 1952,
  2228, 2230, 2506, 2508, 2786, 2788,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  // fs = 32 kHz
   192,  192,  240,  240,  288,  288,  336,  336,  384,  384,  480,  480,  576,  576,  672,  672,
   768,  768,  960,  960, 1152, 1152, 1344, 1344, 1536, 1536, 1920, 1920, 2304, 2304, 2688, 2688,
  3072, 3072, 3456, 3456, 3840, 3840,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  //
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
  };


cDolbyRepacker::cDolbyRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId)
: cRepacker(Pid, ResultBuffer, MaxPacketSize, StreamId, SubStreamId)
{
  Reset();
}

void cDolbyRepacker::AppendSubStreamHeader(bool ContinuationFrame)
{
   fragmentData[fragmentLen++] = subStreamId;
   // number of ac3 frames "starting" in this packet (1 by design).
   fragmentData[fragmentLen++] = 0x01;
   // offset to start of first ac3 frame (0 means "no ac3 frame starting"
   // so 1 (by design) addresses the first byte after the next two bytes).
   fragmentData[fragmentLen++] = 0x00;
   fragmentData[fragmentLen++] = (ContinuationFrame ? 0x00 : 0x01);
}

bool cDolbyRepacker::ScanDataForStartCode(const uchar *&Data, const uchar *const Limit)
{
  const uchar *data = Data;
  if (!skippedBytes && Limit - data >= 5) {
        scanner = BE2HOST(get_unaligned((uint32_t *)data));
        data += 4;
        if ((scanner & 0xFFFF0000) == 0x0B770000 && (frameSize = frameSizes[*data])) {
           skippedBytes = 4;
           Data = data;
           return true;
           }
     }

  while (data < Limit) {
        if ((scanner & 0xFFFF0000) == 0x0B770000 && (frameSize = frameSizes[*data])) {
           skippedBytes += data - Data;
           Data = data;
           return true;
           }
        scanner <<= 8;
        scanner |= *data;
        data++;
        }

  skippedBytes += data - Data;
  // Data = data;
  return false;
}

void cDolbyRepacker::Reset(void)
{
  cRepacker::Reset();
  state = syncing;
  frameSize = 0;
  frameTodo = 0;
  scanner = 0;

  curPacketDataHeader.pesPacketType = pesPtDolby;
  curPacketDataHeader.pictureType = NO_PICTURE;
}

#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cDolbyRepacker::Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame)
#else
void cDolbyRepacker::Repack(uchar *Data, int Count, const bool PacketStart)
#endif
{
  // check for MPEG 2
  if (mpegLevel != phMPEG2) {
     DroppedData("cDolbyRepacker: MPEG 2 PES header expected", Count);
     return;
     }

  const uchar *data = Data;
  const uchar *const limit = Data + Count;

  while (data < limit) {
        // Have we a frame needs finishing?
        if (state == scanFrame) {
           const int bite = std::min(limit - data, std::min(frameTodo, packetTodo));
           packetTodo -= bite;
           frameTodo -= bite;

           if (frameTodo && packetTodo) {
              // All remaining data contains to current frame
              // And frame is not fully present
              memcpy(fragmentData + fragmentLen, data, bite);
              fragmentLen += bite;
              return;
              }

           PushOutPacket(data,bite);
           data += bite;
           if (!frameTodo) {
               state=syncing;
               scanner = 0;
               continue;
               }

           CreatePesHeader(true);
           AppendSubStreamHeader(true);
           packetTodo = maxPacketSize - fragmentLen;
           continue;
           }

        if (!ScanDataForStartCode(data,limit))
           return; // No frame start and we are syncing

        if (initiallySyncing) // omit report for the typical initial case
           initiallySyncing = false;
        else if (skippedBytes > 4) // report that syncing dropped some bytes
           LOG("cDolbyRepacker: skipped %d bytes to sync on next AC3 frame", skippedBytes - 4);
        skippedBytes = 0;

        CreatePesHeader(false, inputPesHeaderBackupLen && inputPacketDone - inputPesHeaderBackupLen - (data - Data) >= 4 );
        AppendSubStreamHeader(false);
        put_unaligned(HOST2BE(scanner), (uint32_t *)(fragmentData + fragmentLen));
        fragmentLen += 4;
        packetTodo = maxPacketSize - fragmentLen;
        frameTodo = frameSize - 4;


        curPacketDataHeader.pictureType = I_FRAME;

        state=scanFrame;
        }
}


// --- cDolbyPlusRepacker --------------------------------------------------------

class cDolbyPlusRepacker : public cRepacker {
private:
  enum eState {
    syncing,
    scanFrame
    };
  int frameSize;
  int frameTodo;
  int state;

  inline void AppendSubStreamHeader(bool ContinuationFrame = false)  __attribute__ ((always_inline));
  inline bool ScanDataForStartCode(const uchar *&Data, const uchar *Limit)  __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual void Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame);
#else
  virtual void Repack(uchar *Data, int Count, const bool PacketStart);
#endif
public:

  cDolbyPlusRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId = 0);

  virtual void Reset(void);

  };


cDolbyPlusRepacker::cDolbyPlusRepacker(int Pid, cRingBufferResult *ResultBuffer, int MaxPacketSize, uint8_t StreamId, uint8_t SubStreamId)
: cRepacker(Pid, ResultBuffer, MaxPacketSize, StreamId, SubStreamId)
{
  Reset();
}

void cDolbyPlusRepacker::AppendSubStreamHeader(bool ContinuationFrame)
{
   fragmentData[fragmentLen++] = subStreamId;
   // number of ac3 frames "starting" in this packet (1 by design).
   fragmentData[fragmentLen++] = 0x01;
   // offset to start of first ac3 frame (0 means "no ac3 frame starting"
   // so 1 (by design) addresses the first byte after the next two bytes).
   fragmentData[fragmentLen++] = 0x00;
   fragmentData[fragmentLen++] = (ContinuationFrame ? 0x00 : 0x01);
}

bool cDolbyPlusRepacker::ScanDataForStartCode(const uchar *&Data, const uchar *const Limit)
{
  const uchar *data = Data;
  if (!skippedBytes && Limit - data >= 4) {
        scanner = BE2HOST(get_unaligned((uint32_t *)data));
        data += 4;
        if ((scanner & 0xFFFF0000) == 0x0B770000) {
           frameSize=((scanner & 0x7FF)+1)*2;
           skippedBytes = 4;
           Data = data;
           return true;
           }
     }

  while (data < Limit) {
        if ((scanner & 0xFFFF0000) == 0x0B770000) {
           frameSize=((scanner & 0x7FF)+1)*2;
           skippedBytes += data - Data;
           Data = data;
           return true;
           }
        scanner <<= 8;
        scanner |= *data;
        data++;
        }

  skippedBytes += data - Data;
  // Data = data;
  return false;
}

void cDolbyPlusRepacker::Reset(void)
{
  cRepacker::Reset();
  state = syncing;
  frameSize = 0;
  frameTodo = 0;
  scanner = 0;

  curPacketDataHeader.pesPacketType = pesPtDolby;
  curPacketDataHeader.pictureType = NO_PICTURE;
}

#ifdef USE_HW_VIDEO_FRAME_EVENTS
void cDolbyPlusRepacker::Repack(uchar *Data, int Count, const bool PacketStart, const eTsVideoFrame videoFrame)
#else
void cDolbyPlusRepacker::Repack(uchar *Data, int Count, const bool PacketStart)
#endif
{
  // check for MPEG 2
  if (mpegLevel != phMPEG2) {
     DroppedData("cDolbyPlusRepacker: MPEG 2 PES header expected", Count);
     return;
     }

  const uchar *data = Data;
  const uchar *const limit = Data + Count;

  while (data < limit) {
        // Have we a frame needs finishing?
        if (state == scanFrame) {
           const int bite = std::min(limit - data, std::min(frameTodo, packetTodo));
           packetTodo -= bite;
           frameTodo -= bite;

           if (frameTodo && packetTodo) {
              // All remaining data contains to current frame
              // And frame is not fully present
              memcpy(fragmentData + fragmentLen, data, bite);
              fragmentLen += bite;
              return;
              }

           PushOutPacket(data,bite);
           data += bite;
           if (!frameTodo) {
               state=syncing;
               scanner = 0;
               continue;
               }

           CreatePesHeader(true);
           AppendSubStreamHeader(true);
           packetTodo = maxPacketSize - fragmentLen;
           continue;
           }

        if (!ScanDataForStartCode(data,limit))
           return; // No frame start and we are syncing

        if (initiallySyncing) // omit report for the typical initial case
           initiallySyncing = false;
        else if (skippedBytes > 4) // report that syncing dropped some bytes
           LOG("cDolbyPlusRepacker: skipped %d bytes to sync on next E-AC3 frame", skippedBytes - 4);
        skippedBytes = 0;

        CreatePesHeader(false, inputPesHeaderBackupLen && inputPacketDone - inputPesHeaderBackupLen - (data - Data) >= 4 );
        AppendSubStreamHeader(false);
        put_unaligned(HOST2BE(scanner), (uint32_t *)(fragmentData + fragmentLen));
        fragmentLen += 4;
        packetTodo = maxPacketSize - fragmentLen;
        frameTodo = frameSize - 4;


        curPacketDataHeader.pictureType = I_FRAME;

        state=scanFrame;
        }
}


// --- cRemux ----------------------------------------------------------------
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
#define RESULTBUFFERSIZE MEGABYTE(3) //(256)
#else
#define RESULTBUFFERSIZE KILOBYTE(768) //(256)
#endif
#define MAXRESTARTS 10

cRemux::cRemux(int VPid, const int *APids, const int *DPids, const int *SPids, bool ExitOnFailure, int VType, const int *DPPids)
{
  exitOnFailure = ExitOnFailure;
  isRadio = VPid == 0 || VPid == 1 || VPid == 0x1FFF;
  numUPTerrors = 0;
  synced = false;
  skipped = 0;
  numTracks = 0;
  resultSkipped = 0;


  resultBuffer = new cRingBufferResult(RESULTBUFFERSIZE, isRadio, false, "Result");

  resultBuffer->SetTimeouts(0, 500);
  resultBuffer->SetLimits(2 * IPACKS, 2 * RESULT_BUFFER_ALIGNMENT);
  if (VPid) {
    if (VType != 0x1B)
     repacker[numTracks++] = new cVideoRepacker(VPid, resultBuffer, IPACKS, 0xE0, 0x00);
    else
     repacker[numTracks++] = new cHDVideoRepacker(VPid, resultBuffer, IPACKS, 0xE0, 0x00);
    }

  if (APids) {
     int n = 0;
     while (*APids && numTracks < MAXTRACKS && n < MAXAPIDS)
           repacker[numTracks++] = new cAudioRepacker(*APids++, resultBuffer, IPACKS, 0xC0 + n++, 0x00);

     }
  if (DPids) {
     int n = 0;
     while (*DPids && numTracks < MAXTRACKS && n < MAXDPIDS)
        if ((DPPids) && (DPPids[n] == 0x7A))
           repacker[numTracks++] = new cDolbyPlusRepacker(*DPids++, resultBuffer, IPACKS, 0xBD, 0x80 + n++);
        else
           repacker[numTracks++] = new cDolbyRepacker(*DPids++, resultBuffer, IPACKS, 0xBD, 0x80 + n++);
     }
  /* future...
  if (SPids) {
     int n = 0;
     while (*SPids && numTracks < MAXTRACKS && n < MAXSPIDS)
           ts2pes[numTracks++] = new cTS2PES(*SPids++, resultBuffer, IPACKS, 0x00, 0x28 + n++);
     }
  */
}

cRemux::~cRemux()
{
  for (int t = 0; t < numTracks; t++)
      delete repacker[t];
  delete resultBuffer;
}

int cRemux::GetPid(const uchar *Data)
{
  return (((uint16_t)Data[0] & PID_MASK_HI) << 8) | (Data[1] & 0xFF);
}

int cRemux::GetPacketLength(const uchar *Data, int Count, int Offset)
{
  // Returns the length of the packet starting at Offset, or -1 if Count is
  // too small to contain the entire packet.
  int Length = (Offset + 5 < Count) ? (Data[Offset + 4] << 8) + Data[Offset + 5] + 6 : -1;
  if (Length > 0 && Offset + Length <= Count)
     return Length;
  return -1;
}


int cRemux::ScanVideoPacket(const uchar *Data, int Count, int Offset, uchar &PictureType)
{
  // Scans the video packet starting at Offset and returns its length.
  // If the return value is -1 the packet was not completely in the buffer.
  int Length = GetPacketLength(Data, Count, Offset);
  if (Length > 0) {
     int PesPayloadOffset = 0;
     if (AnalyzePesHeader(Data + Offset, Length, PesPayloadOffset) >= phMPEG1) {
        const uchar *p = Data + Offset + PesPayloadOffset + 2;
        const uchar *pLimit = Data + Offset + Length - 3;

        // cVideoRepacker ensures that a new PES packet is started for a new sequence,
        // group or picture which allows us to easily skip scanning through a huge
        // amount of video data.
        if (p < pLimit) {
           if (p[-2] || p[-1] || p[0] != 0x01)
              pLimit = 0; // skip scanning: packet doesn't start with 0x000001
           else {
              switch (p[1]) {
                case SC_SEQUENCE:
                case SC_GROUP:
                case SC_PICTURE:
                     break;
                default: // skip scanning: packet doesn't start a new sequence, group or picture
                     pLimit = 0;
                }
              }
           }

        while (p < pLimit && (p = (const uchar *)memchr(p, 0x01, pLimit - p))) {
              if (!p[-2] && !p[-1]) { // found 0x000001
                 switch (p[1]) {
                   case SC_PICTURE: PictureType = (p[3] >> 3) & 0x07;
                                    return Length;
                   default: ;
                   }
                 p += 4; // continue scanning after 0x01ssxxyy
                 }
              else
                 p += 3; // continue scanning after 0x01xxyy
              }
        }
     PictureType = NO_PICTURE;
     return Length;
     }
  return -1;
}

#define TS_SYNC_BYTE 0x47
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
int cRemux::Put(uchar *Data, int Count, const sTsDataHeader *const Header)
{
  int used = 0;

  int pid = Header->pid;

  int t;

  for (t = 0; t < numTracks; t++)
      if (repacker[t]->Pid() == pid) {
         for (;Count > 0; Count -= TS_SIZE, Data += TS_SIZE) {
             if (resultBuffer->Free() < 2 * IPACKS) {
                // A cTS2PES might write one full packet and also a small rest
                resultBuffer->DoRealign();
                break;
                }
             used += TS_SIZE;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
             repacker[t]->ts_to_pes(Data, Header->startsWithVideoFrame);
#else
             repacker[t]->ts_to_pes(Data);
#endif
             }
         break;
         }


  // Check if we're getting anywhere here:
  if (!synced && skipped >= 0) {
     if (skipped > MAXNONUSEFULDATA) {
        esyslog("ERROR: no useful data seen within %d byte of video stream", skipped);
        skipped = -1;
        if (exitOnFailure)
           ShutdownHandler.RequestEmergencyExit();
        }
     else
        skipped += used;
     }

  return used;
}

#else
int cRemux::Put(uchar *Data, int Count)
{
  int used = 0;

  // Make sure we are looking at a TS packet:

  while (Count > TS_SIZE) {
        if (Data[0] == TS_SYNC_BYTE && Data[TS_SIZE] == TS_SYNC_BYTE)
           break;
        Data++;
        Count--;
        used++;
        }
  if (used)
     esyslog("ERROR: skipped %d byte to sync on TS packet", used);

  // Convert incoming TS data into multiplexed PES:

  for (int i = 0; i < Count; i += TS_SIZE) {
      if (Count - i < TS_SIZE)
         break;
      if (Data[i] != TS_SYNC_BYTE)
         break;
      if (resultBuffer->Free() < 2 * IPACKS) {
         resultBuffer->DoRealign();
         break; // A cTS2PES might write one full packet and also a small rest
         }
      int pid = GetPid(Data + i + 1);
      if (Data[i + 3] & PAY_LOAD) { // got payload
         for (int t = 0; t < numTracks; t++) {
             if (repacker[t]->Pid() == pid) {
                repacker[t]->ts_to_pes(Data + i);
                break;
                }
             }
         }
      used += TS_SIZE;
      }

  // Check if we're getting anywhere here:
  if (!synced && skipped >= 0) {
     if (skipped > MAXNONUSEFULDATA) {
        esyslog("ERROR: no useful data seen within %d byte of video stream", skipped);
        skipped = -1;
        if (exitOnFailure)
           ShutdownHandler.RequestEmergencyExit();
        }
     else
        skipped += used;
     }

  return used;
}
#endif

uchar *cRemux::Get(int &Count,sPesResult *&Header, int &HeaderCount, int &FirstIFrame)
{
  sPesResult *header = NULL;
  int count = 0;
  int headerCount;
  uchar *p = resultBuffer->Get(count, header, headerCount);

  if (!p)
     return NULL;

  if (!synced && (headerCount == 0 || header[0].pictureType != I_FRAME || header[0].offset != 0)) {
     int i = 0;
     for (; i < headerCount && header[i].pictureType != I_FRAME; i++)
         ;

     if (i < headerCount)
        resultBuffer->Del(header[i].offset);
     else
        resultBuffer->Del(count);

     return NULL;
     }

  if (!synced && !isRadio)
     SetBrokenLink(p,count);
  synced = true;

  int firstIFrame = -1;
  for (int i=0; i < headerCount; i++)
      if (header[i].pictureType <= 0 ||  B_FRAME < header[i].pictureType) {
         esyslog("ERROR: unknown picture type '0x%02hhX'",  header[i].pictureType);
         if (++numUPTerrors > MAXNUMUPTERRORS && exitOnFailure)
            ShutdownHandler.RequestEmergencyExit();
         header[i].pictureType = NO_PICTURE;
         }
      else if (firstIFrame == -1 && header[i].pictureType == I_FRAME)
         firstIFrame = i;

  Count = count;
  Header = header;
  HeaderCount = headerCount;
  FirstIFrame = firstIFrame;
  return p;
}

//M7X0 END AK
void cRemux::Del(int Count)
{
  resultBuffer->Del(Count);
}

void cRemux::Clear(void)
{
  for (int t = 0; t < numTracks; t++)
      repacker[t]->Clear();
  resultBuffer->Clear();
  synced = false;
  skipped = 0;
  resultSkipped = 0;
}

bool cRemux::isTsHeader(uchar *Data)
{
  if (Data[0] == TS_SYNC_BYTE && Data[TS_SIZE] == TS_SYNC_BYTE /* && Data[1] == PAY_START && Data[TS_SIZE + 1] == PAY_START */) {
     isyslog("TS stream detected");
     return true;
     }

  isyslog("PES stream detected");
  return false;
}

bool cRemux::isAudioOrH264(uchar *Data, int Length, bool isTsData)
{
  uchar *data = Data;
  int length = Length;

  if (isTsData) {
     while(length > 3) {
        if((data[1] & 0x40) && ((data[1] & 0x1F) || (data[2]))) {
           int PES = 4;
           if(data[3] & 0x20)
              PES += data[4] + 1;
           if(PES + 2 < length) {
              if((!data[PES]) && (!data[PES + 1]) && (data[PES + 2] == 0x01)) {
                 data += PES;
                 length -= PES;
                 break;
                 }
              }
           }
        data += TS_SIZE;
        length -= TS_SIZE;
        }
     }

  // Detect audio stream
  if ((data[3] & 0xE0) == AUDIO_STREAM_S) {
     isyslog("Audio stream detected");
     return true;
     }

  // Detect H.264 video stream
  int PesPayloadOffset = 0;
  if ((data[3] & 0xF0) == VIDEO_STREAM_S && AnalyzePesHeader(data, length, PesPayloadOffset) >= phMPEG1) {
     if (PesPayloadOffset < length - 3) {
        if (data[PesPayloadOffset] == 0 && data[PesPayloadOffset + 1] == 0 && data[PesPayloadOffset + 2] == 1 && data[PesPayloadOffset + 3] == 0x09) {
           isyslog("H.264 video detected");
           return true;
           }
        if (PesPayloadOffset < length - 4)
           if (data[PesPayloadOffset] == 0 && data[PesPayloadOffset + 1] == 0 && data[PesPayloadOffset + 2] == 0 && data[PesPayloadOffset + 3] == 1 && data[PesPayloadOffset + 4] == 0x09) {
              isyslog("H.264 video detected");
              return true;
              }
        }
     }

  isyslog("Neither audio nor H.264 video stream detected");
  return false;
}

void cRemux::SetBrokenLink(uchar *Data, int Length)
{
  int PesPayloadOffset = 0;
  if (AnalyzePesHeader(Data, Length, PesPayloadOffset) >= phMPEG1 && (Data[3] & 0xF0) == VIDEO_STREAM_S) {
     for (int i = PesPayloadOffset; i < Length - 7; i++) {
         if (Data[i] == 0 && Data[i + 1] == 0 && Data[i + 2] == 1 && Data[i + 3] == 0xB8) {
            if (!(Data[i + 7] & 0x40)) // set flag only if GOP is not closed
               Data[i + 7] |= 0x20;
            return;
            }
         }
     dsyslog("SetBrokenLink: no GOP header found in video packet");
     }
  else
     dsyslog("SetBrokenLink: no video packet in frame");
}


// --- Some TS handling tools ------------------------------------------------

int64_t TsGetPts(const uchar *p, int l)
{
  // Find the first packet with a PTS and use it:
  while (l > 0) {
        const uchar *d = p;
        if (TsPayloadStart(d) && TsGetPayload(&d) && PesHasPts(d))
           return PesGetPts(d);
        p += TS_SIZE;
        l -= TS_SIZE;
        }
  return -1;
}

void TsSetTeiOnBrokenPackets(uchar *p, int l)
{
  bool Processed[MAXPID] = { false };
  while (l >= TS_SIZE) {
        if (*p != TS_SYNC_BYTE)
           break;
        int Pid = TsPid(p);
        if (!Processed[Pid]) {
           if (!TsPayloadStart(p))
              p[1] |= TS_ERROR;
           else {
              Processed[Pid] = true;
              int offs = TsPayloadOffset(p);
              cRemux::SetBrokenLink(p + offs, TS_SIZE - offs);
              }
           }
        l -= TS_SIZE;
        p += TS_SIZE;
        }
}

// --- cPatPmtParser ---------------------------------------------------------

cPatPmtParser::cPatPmtParser(bool UpdatePrimaryDevice)
{
  updatePrimaryDevice = UpdatePrimaryDevice;
  Reset();
}

void cPatPmtParser::Reset(void)
{
  pmtSize = 0;
  patVersion = pmtVersion = -1;
  pmtPid = -1;
  vpid = vtype = 0;
  ppid = 0;
}

void cPatPmtParser::ParsePat(const uchar *Data, int Length)
{
  // Unpack the TS packet:
  int PayloadOffset = TsPayloadOffset(Data);
  Data += PayloadOffset;
  Length -= PayloadOffset;
  // The PAT is always assumed to fit into a single TS packet
  if ((Length -= Data[0] + 1) <= 0)
     return;
  Data += Data[0] + 1; // process pointer_field
  SI::PAT Pat(Data, false);
  if (Pat.CheckCRCAndParse()) {
     dbgpatpmt("PAT: TSid = %d, c/n = %d, v = %d, s = %d, ls = %d\n", Pat.getTransportStreamId(), Pat.getCurrentNextIndicator(), Pat.getVersionNumber(), Pat.getSectionNumber(), Pat.getLastSectionNumber());
     if (patVersion == Pat.getVersionNumber())
        return;
     SI::PAT::Association assoc;
     for (SI::Loop::Iterator it; Pat.associationLoop.getNext(assoc, it); ) {
         dbgpatpmt("     isNITPid = %d\n", assoc.isNITPid());
         if (!assoc.isNITPid()) {
            pmtPid = assoc.getPid();
            dbgpatpmt("     service id = %d, pid = %d\n", assoc.getServiceId(), assoc.getPid());
            }
         }
     patVersion = Pat.getVersionNumber();
     }
  else
     esyslog("ERROR: can't parse PAT");
}

void cPatPmtParser::ParsePmt(const uchar *Data, int Length)
{
  // Unpack the TS packet:
  bool PayloadStart = TsPayloadStart(Data);
  int PayloadOffset = TsPayloadOffset(Data);
  Data += PayloadOffset;
  Length -= PayloadOffset;
  // The PMT may extend over several TS packets, so we need to assemble them
  if (PayloadStart) {
     pmtSize = 0;
     if ((Length -= Data[0] + 1) <= 0)
        return;
     Data += Data[0] + 1; // this is the first packet
     if (SectionLength(Data, Length) > Length) {
        if (Length <= int(sizeof(pmt))) {
           memcpy(pmt, Data, Length);
           pmtSize = Length;
           }
        else
           esyslog("ERROR: PMT packet length too big (%d byte)!", Length);
        return;
        }
     // the packet contains the entire PMT section, so we run into the actual parsing
     }
  else if (pmtSize > 0) {
     // this is a following packet, so we add it to the pmt storage
     if (Length <= int(sizeof(pmt)) - pmtSize) {
        memcpy(pmt + pmtSize, Data, Length);
        pmtSize += Length;
        }
     else {
        esyslog("ERROR: PMT section length too big (%d byte)!", pmtSize + Length);
        pmtSize = 0;
        }
     if (SectionLength(pmt, pmtSize) > pmtSize)
        return; // more packets to come
     // the PMT section is now complete, so we run into the actual parsing
     Data = pmt;
     }
  else
     return; // fragment of broken packet - ignore
  SI::PMT Pmt(Data, false);
  if (Pmt.CheckCRCAndParse()) {
     dbgpatpmt("PMT: sid = %d, c/n = %d, v = %d, s = %d, ls = %d\n", Pmt.getServiceId(), Pmt.getCurrentNextIndicator(), Pmt.getVersionNumber(), Pmt.getSectionNumber(), Pmt.getLastSectionNumber());
     dbgpatpmt("     pcr = %d\n", Pmt.getPCRPid());
     if (pmtVersion == Pmt.getVersionNumber())
        return;
     if (updatePrimaryDevice)
        cDevice::PrimaryDevice()->ClrAvailableTracks(false, true);
     int NumApids = 0;
     int NumDpids = 0;
     //int NumSpids = 0;
     vpid = vtype = 0;
     ppid = 0;
     apids[0] = 0;
     dpids[0] = 0;
     spids[0] = 0;
     atypes[0] = 0;
     dtypes[0] = 0;
     SI::PMT::Stream stream;
     for (SI::Loop::Iterator it; Pmt.streamLoop.getNext(stream, it); ) {
         dbgpatpmt("     stream type = %02X, pid = %d", stream.getStreamType(), stream.getPid());
         switch (stream.getStreamType()) {
           case 0x01: // STREAMTYPE_11172_VIDEO
           case 0x02: // STREAMTYPE_13818_VIDEO
           case 0x1B: // MPEG4
                      vpid = stream.getPid();
                      vtype = stream.getStreamType();
                      ppid = Pmt.getPCRPid();
                      break;
           case 0x03: // STREAMTYPE_11172_AUDIO
           case 0x04: // STREAMTYPE_13818_AUDIO
                      {
                      if (NumApids < MAXAPIDS) {
                         apids[NumApids] = stream.getPid();
                         atypes[NumApids] = stream.getStreamType();
                         *alangs[NumApids] = 0;
                         SI::Descriptor *d;
                         for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
                             switch (d->getDescriptorTag()) {
                               case SI::ISO639LanguageDescriptorTag: {
                                    SI::ISO639LanguageDescriptor *ld = (SI::ISO639LanguageDescriptor *)d;
                                    SI::ISO639LanguageDescriptor::Language l;
                                    char *s = alangs[NumApids];
                                    int n = 0;
                                    for (SI::Loop::Iterator it; ld->languageLoop.getNext(l, it); ) {
                                        if (*ld->languageCode != '-') { // some use "---" to indicate "none"
                                           dbgpatpmt(" '%s'", l.languageCode);
                                           if (n > 0)
                                              *s++ = '+';
                                           strn0cpy(s, I18nNormalizeLanguageCode(l.languageCode), MAXLANGCODE1);
                                           s += strlen(s);
                                           if (n++ > 1)
                                              break;
                                           }
                                        }
                                    }
                                    break;
                               default: ;
                               }
                             delete d;
                             }
                         if (updatePrimaryDevice)
                            cDevice::PrimaryDevice()->SetAvailableTrack(ttAudio, NumApids, apids[NumApids], alangs[NumApids]);
                         NumApids++;
                         apids[NumApids]= 0;
                         }
                      }
                      break;
           case 0x06: // STREAMTYPE_13818_PES_PRIVATE
                      {
                      int dpid = 0;
                      char lang[MAXLANGCODE1] = "";
                      SI::Descriptor *d;
                      for (SI::Loop::Iterator it; (d = stream.streamDescriptors.getNext(it)); ) {
                          switch (d->getDescriptorTag()) {
                            case SI::AC3DescriptorTag:
                            case SI::EnhancedAC3DescriptorTag:
                                 dbgpatpmt(" AC3");
                                 dpid = stream.getPid();
                                 break;
/*
                            case SI::SubtitlingDescriptorTag:
                                 dbgpatpmt(" subtitling");
                                 if (NumSpids < MAXSPIDS) {
                                    spids[NumSpids] = stream.getPid();
                                    *slangs[NumSpids] = 0;
                                    subtitlingTypes[NumSpids] = 0;
                                    compositionPageIds[NumSpids] = 0;
                                    ancillaryPageIds[NumSpids] = 0;
                                    SI::SubtitlingDescriptor *sd = (SI::SubtitlingDescriptor *)d;
                                    SI::SubtitlingDescriptor::Subtitling sub;
                                    char *s = slangs[NumSpids];
                                    int n = 0;
                                    for (SI::Loop::Iterator it; sd->subtitlingLoop.getNext(sub, it); ) {
                                        if (sub.languageCode[0]) {
                                           dbgpatpmt(" '%s'", sub.languageCode);
                                           subtitlingTypes[NumSpids] = sub.getSubtitlingType();
                                           compositionPageIds[NumSpids] = sub.getCompositionPageId();
                                           ancillaryPageIds[NumSpids] = sub.getAncillaryPageId();
                                           if (n > 0)
                                              *s++ = '+';
                                           strn0cpy(s, I18nNormalizeLanguageCode(sub.languageCode), MAXLANGCODE1);
                                           s += strlen(s);
                                           if (n++ > 1)
                                              break;
                                           }
                                        }
                                    if (updatePrimaryDevice)
                                       cDevice::PrimaryDevice()->SetAvailableTrack(ttSubtitle, NumSpids, spids[NumSpids], slangs[NumSpids]);
                                    NumSpids++;
                                    spids[NumSpids]= 0;
                                    }
                                 break;
*/
                            case SI::ISO639LanguageDescriptorTag: {
                                 SI::ISO639LanguageDescriptor *ld = (SI::ISO639LanguageDescriptor *)d;
                                 dbgpatpmt(" '%s'", ld->languageCode);
                                 strn0cpy(lang, I18nNormalizeLanguageCode(ld->languageCode), MAXLANGCODE1);
                                 }
                                 break;
                            default: ;
                            }
                          delete d;
                          }
                      if (dpid) {
                         if (NumDpids < MAXDPIDS) {
                            dpids[NumDpids] = dpid;
                            dtypes[NumDpids] = stream.getStreamType();
                            strn0cpy(dlangs[NumDpids], lang, sizeof(dlangs[NumDpids]));
                            if (updatePrimaryDevice && Setup.UseDolbyDigital)
                               cDevice::PrimaryDevice()->SetAvailableTrack(ttDolby, NumDpids, dpid, lang);
                            NumDpids++;
                            dpids[NumDpids]= 0;
                            }
                         }
                      }
                      break;
           default: ;
           }
         dbgpatpmt("\n");
         if (updatePrimaryDevice) {
            cDevice::PrimaryDevice()->EnsureAudioTrack(true);
            //cDevice::PrimaryDevice()->EnsureSubtitleTrack();
            }
         }
     pmtVersion = Pmt.getVersionNumber();
     }
  else
     esyslog("ERROR: can't parse PMT");
  pmtSize = 0;
}

bool cPatPmtParser::GetVersions(int &PatVersion, int &PmtVersion) const
{
  PatVersion = patVersion;
  PmtVersion = pmtVersion;
  return patVersion >= 0 && pmtVersion >= 0;
}

// --- cTsToPes --------------------------------------------------------------

cTsToPes::cTsToPes(void)
{
  data = NULL;
  size = 0;
  Reset();
}

cTsToPes::~cTsToPes()
{
  free(data);
}

void cTsToPes::PutTs(const uchar *Data, int Length)
{
  if (TsError(Data)) {
     Reset();
     return; // ignore packets with TEI set, and drop any PES data collected so far
     }
  if (TsPayloadStart(Data))
     Reset();
  else if (!size)
     return; // skip everything before the first payload start
  Length = TsGetPayload(&Data);
  if (length + Length > size) {
     int NewSize = std::max(KILOBYTE(2), length + Length);
     if (uchar *NewData = (uchar *)realloc(data, NewSize)) {
        data = NewData;
        size = NewSize;
        }
     else {
        esyslog("ERROR: out of memory");
        Reset();
        return;
        }
     }
  memcpy(data + length, Data, Length);
  length += Length;
}

#define MAXPESLENGTH 0xFFF0

const uchar *cTsToPes::GetPes(int &Length)
{
  if (repeatLast) {
     repeatLast = false;
     Length = lastLength;
     return lastData;
     }
  if (offset < length && PesLongEnough(length)) {
     if (!PesHasLength(data)) // this is a video PES packet with undefined length
        offset = 6; // trigger setting PES length for initial slice
     if (offset) {
        uchar *p = data + offset - 6;
        if (p != data) {
           p -= 3;
           memmove(p, data, 4);
           }
        int l = std::min(length - offset, MAXPESLENGTH);
        offset += l;
        if (p != data) {
           l += 3;
           p[6]  = 0x80;
           p[7]  = 0x00;
           p[8]  = 0x00;
           }
        p[4] = l / 256;
        p[5] = l & 0xFF;
        Length = l + 6;
        lastLength = Length;
        lastData = p;
        return p;
        }
     else {
        Length = PesLength(data);
        if (Length <= length) {
           offset = Length; // to make sure we break out in case of garbage data
           lastLength = Length;
           lastData = data;
           return data;
           }
        }
     }
  return NULL;
}

void cTsToPes::SetRepeatLast(void)
{
  repeatLast = true;
}

void cTsToPes::Reset(void)
{
  length = offset = 0;
  lastData = NULL;
  lastLength = 0;
  repeatLast = false;
}

/*
 * remux.h: A streaming MPEG2 remultiplexer
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __REMUX_H
#define __REMUX_H
#ifdef USE_HW_VIDEO_FRAME_EVENTS
#define USE_DATAHEADER_IN_RECEIVER 1
#endif
#include "channels.h"
#include "ringbuffer.h"
#include "tools.h"

enum ePesHeader {
  phNeedMoreData = -1,
  phInvalid = 0,
  phMPEG1 = 1,
  phMPEG2 = 2
  };

//M7X0 BEGIN AK
#ifdef PLUGIN_NAME_I18N
ePesHeader AnalyzePesHeader(const uchar *Data, int Count, int &PesPayloadOffset, bool *ContinuationHeader = NULL);
#else
inline ePesHeader AnalyzePesHeader(const uchar *Data, int Count, int &PesPayloadOffset, bool *ContinuationHeader = NULL) __attribute__ ((always_inline));
#endif
//M7X0 END AK

// Picture types:
#define NO_PICTURE 0
#define I_FRAME    1
#define P_FRAME    2
#define B_FRAME    3

#define MAXTRACKS 64

class cRepacker;

class cRemux {
private:
  bool exitOnFailure;
  bool isRadio;
  int numUPTerrors;
  bool synced;
  int skipped;
  cRepacker *repacker[MAXTRACKS];
  int numTracks;
//M7X0 BEGIN AK
  cRingBufferResult *resultBuffer;
  int clearTimeoutCounter;
//M7X0 END AK
  int resultSkipped;
  int GetPid(const uchar *Data);
public:
  cRemux(int VPid, const int *APids, const int *DPids, const int *SPids, bool ExitOnFailure = false, int VType = 0, const int *DPPids = NULL);
       ///< Creates a new remuxer for the given PIDs. VPid is the video PID, while
       ///< APids, DPids and SPids are pointers to zero terminated lists of audio,
       ///< dolby and subtitle PIDs (the pointers may be NULL if there is no such
       ///< PID). If ExitOnFailure is true, the remuxer will initiate an "emergency
       ///< exit" in case of problems with the data stream.
  ~cRemux();
  void SetTimeouts(int PutTimeout, int GetTimeout) { resultBuffer->SetTimeouts(PutTimeout, GetTimeout); }
       ///< By default cRemux assumes that Put() and Get() are called from different
       ///< threads, and uses a timeout in the Get() function in case there is no
       ///< data available. SetTimeouts() can be used to modify these timeouts.
       ///< Especially if Put() and Get() are called from the same thread, setting
       ///< both timeouts to 0 is recommended.
//M7X0 BEGIN AK
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
  int Put(uchar *Data, int Count, const sTsDataHeader *const Header);
#else
  int Put(uchar *Data, int Count);
       ///< Puts at most Count bytes of Data into the remuxer.
       ///< \return Returns the number of bytes actually consumed from Data.
#endif

  uchar* Get(int &Count, sPesResult *&Header, int &HeaderCount, int &FirstIFrame);
       ///< Gets all currently available data from the remuxer.
       ///< \return Count contains the number of bytes the result points to, and
       ///< PictureType (if not NULL) will contain one of NO_PICTURE, I_FRAME, P_FRAME
       ///< or B_FRAME.
//M7X0 END AK
  void Del(int Count);
       ///< Deletes Count bytes from the remuxer. Count must be the number returned
       ///< from a previous call to Get(). Several calls to Del() with fractions of
       ///< a previously returned Count may be made, but the total sum of all Count
       ///< values must be exactly what the previous Get() has returned.
  void Clear(void);
       ///< Clears the remuxer of all data it might still contain, keeping the PID
       ///< settings as they are.
  static bool isTsHeader(uchar *Data);
  static bool isAudioOrH264(uchar *Data, int Length, bool isTsData);
  static void SetBrokenLink(uchar *Data, int Length);
  static int GetPacketLength(const uchar *Data, int Count, int Offset);
  static int ScanVideoPacket(const uchar *Data, int Count, int Offset, uchar &PictureType);
  };

// Some TS handling tools.
// The following functions all take a pointer to one complete TS packet.

#define TS_SYNC_BYTE          0x47
#define TS_SIZE               188
#define TS_ERROR              0x80
#define TS_PAYLOAD_START      0x40
#define TS_TRANSPORT_PRIORITY 0x20
#define TS_PID_MASK_HI        0x1F
#define TS_SCRAMBLING_CONTROL 0xC0
#define TS_ADAPT_FIELD_EXISTS 0x20
#define TS_PAYLOAD_EXISTS     0x10
#define TS_CONT_CNT_MASK      0x0F
#define TS_ADAPT_DISCONT      0x80
#define TS_ADAPT_RANDOM_ACC   0x40 // would be perfect for detecting independent frames, but unfortunately not used by all broadcasters
#define TS_ADAPT_ELEM_PRIO    0x20
#define TS_ADAPT_PCR          0x10
#define TS_ADAPT_OPCR         0x08
#define TS_ADAPT_SPLICING     0x04
#define TS_ADAPT_TP_PRIVATE   0x02
#define TS_ADAPT_EXTENSION    0x01

#define MAXPID 0x2000 // for arrays that use a PID as the index

inline bool TsHasPayload(const uchar *p)
{
  return p[3] & TS_PAYLOAD_EXISTS;
}

inline bool TsHasAdaptationField(const uchar *p)
{
  return p[3] & TS_ADAPT_FIELD_EXISTS;
}

inline bool TsPayloadStart(const uchar *p)
{
  return p[1] & TS_PAYLOAD_START;
}

inline bool TsError(const uchar *p)
{
  return p[1] & TS_ERROR;
}

inline int TsPid(const uchar *p)
{
  return (p[1] & TS_PID_MASK_HI) * 256 + p[2];
}

inline bool TsIsScrambled(const uchar *p)
{
  return p[3] & TS_SCRAMBLING_CONTROL;
}

inline int TsPayloadOffset(const uchar *p)
{
  return (p[3] & TS_ADAPT_FIELD_EXISTS) ? p[4] + 5 : 4;
}

inline int TsGetPayload(const uchar **p)
{
  int o = TsPayloadOffset(*p);
  *p += o;
  return TS_SIZE - o;
}

inline int TsContinuityCounter(const uchar *p)
{
  return p[3] & TS_CONT_CNT_MASK;
}

inline int TsGetAdaptationField(const uchar *p)
{
  return TsHasAdaptationField(p) ? p[5] : 0x00;
}

// The following functions all take a pointer to a sequence of complete TS packets.

int64_t TsGetPts(const uchar *p, int l);
void TsSetTeiOnBrokenPackets(uchar *p, int l);

// Some PES handling tools:
// The following functions that take a pointer to PES data all assume that
// there is enough data so that PesLongEnough() returns true.

inline bool PesLongEnough(int Length)
{
  return Length >= 6;
}

inline bool PesHasLength(const uchar *p)
{
  return p[4] | p[5];
}

inline int PesLength(const uchar *p)
{
  return 6 + p[4] * 256 + p[5];
}

inline int PesPayloadOffset(const uchar *p)
{
  return 9 + p[8];
}

inline bool PesHasPts(const uchar *p)
{
  return (p[7] & 0x80) && p[8] >= 5;
}

inline int64_t PesGetPts(const uchar *p)
{
  return ((((int64_t)p[ 9]) & 0x0E) << 29) |
         (( (int64_t)p[10])         << 22) |
         ((((int64_t)p[11]) & 0xFE) << 14) |
         (( (int64_t)p[12])         <<  7) |
         ((((int64_t)p[13]) & 0xFE) >>  1);
}

#define MAX_SECTION_SIZE 4096 // maximum size of an SI section

// PAT/PMT Parser:

class cPatPmtParser {
private:
  uchar pmt[MAX_SECTION_SIZE];
  int pmtSize;
  int patVersion;
  int pmtVersion;
  int pmtPid;
  int vpid;
  int ppid;
  int vtype;
  int apids[MAXAPIDS + 1]; // list is zero-terminated
  int atypes[MAXAPIDS + 1]; // list is zero-terminated
  char alangs[MAXAPIDS][MAXLANGCODE2];
  int dpids[MAXDPIDS + 1]; // list is zero-terminated
  int dtypes[MAXDPIDS + 1]; // list is zero-terminated
  char dlangs[MAXDPIDS][MAXLANGCODE2];
  int spids[MAXSPIDS + 1]; // list is zero-terminated
  char slangs[MAXSPIDS][MAXLANGCODE2];
  uchar subtitlingTypes[MAXSPIDS];
  uint16_t compositionPageIds[MAXSPIDS];
  uint16_t ancillaryPageIds[MAXSPIDS];
  bool updatePrimaryDevice;
protected:
  int SectionLength(const uchar *Data, int Length) { return (Length >= 3) ? ((int(Data[1]) & 0x0F) << 8)| Data[2] : 0; }
public:
  cPatPmtParser(bool UpdatePrimaryDevice = false);
  void Reset(void);
       ///< Resets the parser. This function must be called whenever a new
       ///< stream is parsed.
  void ParsePat(const uchar *Data, int Length);
       ///< Parses the PAT data from the single TS packet in Data.
       ///< Length is always TS_SIZE.
  void ParsePmt(const uchar *Data, int Length);
       ///< Parses the PMT data from the single TS packet in Data.
       ///< Length is always TS_SIZE.
       ///< The PMT may consist of several TS packets, which
       ///< are delivered to the parser through several subsequent calls to
       ///< ParsePmt(). The whole PMT data will be processed once the last packet
       ///< has been received.
  bool GetVersions(int &PatVersion, int &PmtVersion) const;
       ///< Returns true if a valid PAT/PMT has been parsed and stores
       ///< the current version numbers in the given variables.
  int PmtPid(void) const { return pmtPid; }
       ///< Returns the PMT pid as defined by the current PAT.
       ///< If no PAT has been received yet, -1 will be returned.
  int Vpid(void) const { return vpid; }
       ///< Returns the video pid as defined by the current PMT, or 0 if no video
       ///< pid has been detected, yet.
  int Ppid(void) const { return ppid; }
       ///< Returns the PCR pid as defined by the current PMT, or 0 if no PCR
       ///< pid has been detected, yet.
  int Vtype(void) const { return vtype; }
       ///< Returns the video stream type as defined by the current PMT, or 0 if no video
       ///< stream type has been detected, yet.
  const int *Apids(void) const { return apids; }
  const int *Dpids(void) const { return dpids; }
  const int *Spids(void) const { return spids; }
  int Apid(int i) const { return (0 <= i && i < MAXAPIDS) ? apids[i] : 0; }
  int Dpid(int i) const { return (0 <= i && i < MAXDPIDS) ? dpids[i] : 0; }
  int Spid(int i) const { return (0 <= i && i < MAXSPIDS) ? spids[i] : 0; }
  int Atype(int i) const { return (0 <= i && i < MAXAPIDS) ? atypes[i] : 0; }
  int Dtype(int i) const { return (0 <= i && i < MAXDPIDS) ? dtypes[i] : 0; }
  const char *Alang(int i) const { return (0 <= i && i < MAXAPIDS) ? alangs[i] : ""; }
  const char *Dlang(int i) const { return (0 <= i && i < MAXDPIDS) ? dlangs[i] : ""; }
  const char *Slang(int i) const { return (0 <= i && i < MAXSPIDS) ? slangs[i] : ""; }
  uchar SubtitlingType(int i) const { return (0 <= i && i < MAXSPIDS) ? subtitlingTypes[i] : uchar(0); }
  uint16_t CompositionPageId(int i) const { return (0 <= i && i < MAXSPIDS) ? compositionPageIds[i] : uint16_t(0); }
  uint16_t AncillaryPageId(int i) const { return (0 <= i && i < MAXSPIDS) ? ancillaryPageIds[i] : uint16_t(0); }
  };

// TS to PES converter:
// Puts together the payload of several TS packets that form one PES
// packet.

class cTsToPes {
private:
  uchar *data;
  int size;
  int length;
  int offset;
  uchar *lastData;
  int lastLength;
  bool repeatLast;
public:
  cTsToPes(void);
  ~cTsToPes();
  void PutTs(const uchar *Data, int Length);
       ///< Puts the payload data of the single TS packet at Data into the converter.
       ///< Length is always 188.
       ///< If the given TS packet starts a new PES payload packet, the converter
       ///< will be automatically reset. Any packets before the first one that starts
       ///< a new PES payload packet will be ignored.
       ///< Once a TS packet has been put into a cTsToPes converter, all subsequent
       ///< packets until the next call to Reset() must belong to the same PID as
       ///< the first packet. There is no check whether this actually is the case, so
       ///< the caller is responsible for making sure this condition is met.
  const uchar *GetPes(int &Length);
       ///< Gets a pointer to the complete PES packet, or NULL if the packet
       ///< is not complete yet. If the packet is complete, Length will contain
       ///< the total packet length. The returned pointer is only valid until
       ///< the next call to PutTs() or Reset(), or until this object is destroyed.
       ///< Once GetPes() has returned a non-NULL value, it must be called
       ///< repeatedly, and the data processed, until it returns NULL. This
       ///< is because video packets may be larger than the data a single
       ///< PES packet with an actual length field can hold, and are therefore
       ///< split into several PES packets with smaller sizes.
       ///< Note that for video data GetPes() may only be called if the next
       ///< TS packet that will be given to PutTs() has the "payload start" flag
       ///< set, because this is the only way to determine the end of a video PES
       ///< packet.
  void SetRepeatLast(void);
       ///< Makes the next call to GetPes() return exactly the same data as the
       ///< last one (provided there was no call to Reset() in the meantime).
  void Reset(void);
       ///< Resets the converter. This needs to be called after a PES packet has
       ///< been fetched by a call to GetPes(), and before the next call to
       ///< PutTs().
  };

#endif // __REMUX_H

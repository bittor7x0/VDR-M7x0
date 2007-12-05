/*
 * remux.h: A streaming MPEG2 remultiplexer
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __REMUX_H
#define __REMUX_H
#ifdef USE_HW_VIDEO_FRAME_EVENTS
#define USE_DATAHEADER_IN_RECEIVER 1
#endif
#include <time.h> //XXX FIXME: DVB/linux/dvb/dmx.h should include <time.h> itself!!!
//M7X0 BEGIN AK
#include "m7x0_dvb/dmx.h"
//M7X0 END AK
#include "ringbuffer.h"
#include "tools.h"

enum ePesHeader {
  phNeedMoreData = -1,
  phInvalid = 0,
  phMPEG1 = 1,
  phMPEG2 = 2
  };

//M7X0 BEGIN AK
inline ePesHeader AnalyzePesHeader(const uchar *Data, int Count, int &PesPayloadOffset, bool *ContinuationHeader = NULL) __attribute__ ((always_inline));
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
  cRemux(int VPid, const int *APids, const int *DPids, const int *SPids, bool ExitOnFailure = false);
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
  static void SetBrokenLink(uchar *Data, int Length);
  static int GetPacketLength(const uchar *Data, int Count, int Offset);
  static int ScanVideoPacket(const uchar *Data, int Count, int Offset, uchar &PictureType);
  };

#endif // __REMUX_H

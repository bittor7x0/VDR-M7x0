/*
 * ringbuffer.h: A ring buffer
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include "thread.h"
#include "tools.h"
#include <stdint.h>

class cRingBuffer {
private:
  cCondWait readyForPut, readyForGet;
  int putTimeout;
  int getTimeout;
  int size;
  time_t lastOverflowReport;
  int overflowCount;
  int overflowBytes;
//M7X0 BEGIN AK
  volatile bool putWaiting;
  volatile bool getWaiting;
  uint64_t getWaitCounter;
  uint64_t putWaitCounter;
  uint64_t getSignalCounter;
  uint64_t putSignalCounter;
//M7X0 END AK
protected:
  tThreadId getThreadTid;
  int maxFill;//XXX
  int lastPercent;
  bool statistics;//XXX
//M7X0 BEGIN AK
  int minPutFree;
  int minGetAvail;
  char *description;
//M7X0 END AK
  void UpdatePercentage(int Fill);
  void WaitForPut(void);
  void WaitForGet(void);
  void EnablePut(void);
  void EnableGet(void);
  virtual void Clear(void) = 0;
  virtual int Available(void) = 0;
  virtual int Free(void) { return Size() - Available() - 1; }
  int Size(void) { return size; }
public:
//M7X0 BEGIN AK
  cRingBuffer(int Size, bool Statistics = false, const char *Description = NULL);
  void SetLimits(int PutFree, int GetAvail);
//M7X0 BEGIN AK
  virtual ~cRingBuffer();
  void SetTimeouts(int PutTimeout, int GetTimeout);
  void ReportOverflow(int Bytes);
  };

class cRingBufferLinear : public cRingBuffer {
//#define DEBUGRINGBUFFERS
#ifdef DEBUGRINGBUFFERS
private:
  int lastHead, lastTail;
  int lastPut, lastGet;
  static cRingBufferLinear *RBLS[];
  static void AddDebugRBL(cRingBufferLinear *RBL);
  static void DelDebugRBL(cRingBufferLinear *RBL);
public:
  static void PrintDebugRBL(void);
#endif
private:
  int margin, head, tail;
  int gotten;
  uchar *buffer;
//M7X0 BEGIN AK
  //char *description;
//M7X0 END AK
public:
  cRingBufferLinear(int Size, int Margin = 0, bool Statistics = false, const char *Description = NULL);
    ///< Creates a linear ring buffer.
    ///< The buffer will be able to hold at most Size-Margin-1 bytes of data, and will
    ///< be guaranteed to return at least Margin bytes in one consecutive block.
    ///< The optional Description is used for debugging only.
  virtual ~cRingBufferLinear();
  virtual int Available(void);
  virtual int Free(void) { return Size() - Available() - 1 - margin; }
  virtual void Clear(void);
    ///< Immediately clears the ring buffer.
  int Read(int FileHandle, int Max = 0);
    ///< Reads at most Max bytes from FileHandle and stores them in the
    ///< ring buffer. If Max is 0, reads as many bytes as possible.
    ///< Only one actual read() call is done.
    ///< \return Returns the number of bytes actually read and stored, or
    ///< an error value from the actual read() call.
  int Put(const uchar *Data, int Count);
    ///< Puts at most Count bytes of Data into the ring buffer.
    ///< \return Returns the number of bytes actually stored.
  uchar *Get(int &Count);
    ///< Gets data from the ring buffer.
    ///< The data will remain in the buffer until a call to Del() deletes it.
    ///< \return Returns a pointer to the data, and stores the number of bytes
    ///< actually available in Count. If the returned pointer is NULL, Count has no meaning.
  void Del(int Count);
    ///< Deletes at most Count bytes from the ring buffer.
    ///< Count must be less or equal to the number that was returned by a previous
    ///< call to Get().
  };

//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
#define USE_DATAHEADER_IN_RECEIVER 1
#endif

enum eTsVideoFrame { tsVideoFrameUnknown, tsVideoFrameNone, tsVideoFrameI, tsVideoFrameP, tsVideoFrameB };

struct sTsDataHeader {
  int pid;
  eTsVideoFrame startsWithVideoFrame;
  };

#ifdef USE_RECEIVER_RINGBUFFER

class cRingBufferReceiver : public cRingBuffer {
private:

  volatile int head;
  volatile int tail;
  volatile int end;
  int gotten;

  uchar *buffer;

  uchar *gottenData;
  int gottenCount;

public:
  cRingBufferReceiver(int Size, bool Statistics = false, const char *Description = NULL);

  virtual ~cRingBufferReceiver();
  virtual int Available(void) {
     const int diff = head - tail;
     return (diff >= 0) ? diff : Size() + diff;
     }
  virtual int Free(void) {
     const int diff = tail - head;
     return (diff > 0) ? diff : Size() + diff;
     }
  virtual void Clear(void);
    ///< Immediately clears the ring buffer.

  bool Put(const uchar *const Data, const int Count,const sTsDataHeader *const Header);

  sTsDataHeader *Get(uchar *&Data, int &Count);

  void Del(int Count);

  };
#endif

#define RESULT_BUFFER_ALIGNMENT KILOBYTE(128)
#define RESULT_BYTES_PER_HEADER KILOBYTE(2)
#define RESULT_INIT_GET_HEADERS 50

enum ePesPacketType {pesPtUnknown, pesPtVideo, pesPtAudio, pesPtDolby};
struct sPesResult {
  ePesPacketType pesPacketType;
  int offset;
  int pictureType; // should be I_FRAME for any Audio Packet (to support Radio mode)
  };

class cRingBufferResult : public cRingBuffer {
private:
  bool isRadio;
  volatile int head;
  volatile int tail;
  int gotten;
  int headersGotten;
  sPesResult *pesHeadersGotten;
  int pesHeadersGottenSize;

  volatile bool realign;
  uchar *buffer;
  sPesResult *pesHeader;
  int headersSize;
  volatile int firstHeader;
  volatile int lastHeader;

public:
  cRingBufferResult(int Size,bool IsRadio, bool Statistics = false,  const char *Description = NULL);

  virtual ~cRingBufferResult();
  virtual int Available(void) {
     const int diff = head - tail;
     return (diff >= 0) ? diff : Size() + diff;
     }
  virtual int Free(void) {
     const int diff = (tail & ~(RESULT_BUFFER_ALIGNMENT - 1)) - head;
     const int free = (diff > 0 ? diff : Size() + diff) - 1;
     const int headerDiff = firstHeader - lastHeader;
     const int headerFree = (headerDiff > 0 ? headerDiff : headerDiff + headersSize) - 1;
     return headerFree ? free : 0;
     }
  virtual void Clear(void);
    ///< Immediately clears the ring buffer.

  bool Put(const uchar *const Data, const int Count,const sPesResult *const Header = NULL);
  void DoRealign(void);
  uchar *Get(int &Count,sPesResult *&Headers, int &HeaderCount);
  void Del(int Count);
  };
//M7X0 END AK

enum eFrameType { ftUnknown, ftVideo, ftAudio, ftDolby };

class cFrame {
  friend class cRingBufferFrame;
private:
  cFrame *next;
  uchar *data;
  int count;
  eFrameType type;
  int index;
public:
  cFrame(const uchar *Data, int Count, eFrameType = ftUnknown, int Index = -1);
    ///< Creates a new cFrame object.
    ///< If Count is negative, the cFrame object will take ownership of the given
    ///< Data. Otherwise it will allocate Count bytes of memory and copy Data.
  ~cFrame();
  uchar *Data(void) const { return data; }
  int Count(void) const { return count; }
  eFrameType Type(void) const { return type; }
  int Index(void) const { return index; }
  };

class cRingBufferFrame : public cRingBuffer {
private:
  cMutex mutex;
  cFrame *head;
  int currentFill;
  void Delete(cFrame *Frame);
  void Lock(void) { mutex.Lock(); }
  void Unlock(void) { mutex.Unlock(); }
public:
  cRingBufferFrame(int Size, bool Statistics = false);
  virtual ~cRingBufferFrame();
  virtual int Available(void);
  virtual void Clear(void);
    // Immediately clears the ring buffer.
  bool Put(cFrame *Frame);
    // Puts the Frame into the ring buffer.
    // Returns true if this was possible.
  cFrame *Get(void);
    // Gets the next frame from the ring buffer.
    // The actual data still remains in the buffer until Drop() is called.
  void Drop(cFrame *Frame);
    // Drops the Frame that has just been fetched with Get().
  };

//M7x0 BEGIN AK
#define MAX_BUFFERED_FRAMES 10
class cFrameM7x0 {
  friend class cRingBufferFrameM7x0;
private:
  uchar *data1;
  uchar *data2;
  int count1;
  int count2;
  eFrameType type;
  int index;
  int fileNo;

  void Reset(void) {
    data1 = NULL;
    data2 = NULL;
    count1 = 0;
    count2 = 0;
    type = ftUnknown;
    index = -1;
    fileNo = 0;
    }
public:
  cFrameM7x0() { Reset(); }
  uchar *Data1(void) const { return data1; }
  int Count1(void) const { return count1; }
  uchar *Data2(void) const { return data2; }
  int Count2(void) const { return count2; }
  eFrameType Type(void) const { return type; }
  int Index(void) const { return index; }
  int FileNo(void) const { return fileNo; }
  };

class cFileName;
class cRingBufferFrameM7x0 : public cRingBuffer {
private:
  int alignment;

  int curFileNr;
  int curFileOffset;
  int readAHeadCount;

  uchar *buffer;
  volatile int head;
  volatile int tail;

  cFrameM7x0 frameBuffer[MAX_BUFFERED_FRAMES + 1];
  volatile int frameHead;
  volatile int frameTail;

  int ReadReal(cUnbufferedFile *file, uchar *buf, int count);
public:
  cRingBufferFrameM7x0(int Size, int Alignment, bool Statistics = false , const char *Description = NULL);
  virtual ~cRingBufferFrameM7x0();
  int Read(cUnbufferedFile *file, int FileNr, int Offset, int FrameSize, int Index);
  int Read(cFileName *fileName, int FileNr, int Offset, int Size, int Index);
  virtual int Available(void) {
    if (frameTail == frameHead)
       return 0;
    int diff = head - tail;
    return (diff >= 0 ? diff : Size() + diff);
    }
  virtual int Free(void) {
    int diff = tail - head;
    return (diff >= 0 ? diff : Size() + diff) - 1;
    }
  virtual void Clear(void) {
    curFileNr = -1;
    frameTail = frameHead = 0;
    tail = head = 0;
    readAHeadCount = 0;
    curFileOffset = 0;
    EnablePut();
    }
  cFrameM7x0 *Get(void);
  void Drop(cFrameM7x0 *frame = NULL);
  };

//M7x0 END AK
#endif // __RINGBUFFER_H

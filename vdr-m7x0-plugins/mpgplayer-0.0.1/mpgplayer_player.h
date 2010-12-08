/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */


#ifndef __MPGPLAYER_PLAYER_H
#define __MPGPLAYER_PLAYER_H

#include <vdr/player.h>
#include <vdr/thread.h>
#include "mpgplayer_recording.h"

class cMpgPlayer;

class cMpgFileName {
private:
  cMpgUnbufferedFile *file;
  int fileNumber;
  char *fileName, *pFileNumber;
  bool record;
  bool blocking;
#ifdef USE_DIRECT_IO
  bool direct;
#endif
public:
#ifdef USE_DIRECT_IO
  cMpgFileName(const char *FileName, bool Record, bool Blocking = false, bool Direct = false);
#else
  cMpgFileName(const char *FileName, bool Record, bool Blocking = false);
#endif
  ~cMpgFileName();
  const char *Name(void) { return fileName; }
  int Number(void) { return fileNumber; }
  cMpgUnbufferedFile *Open(void);
  void Close(void);
  int Unlink(void);
  cMpgUnbufferedFile *SetOffset(int Number, off64_t Offset = 0);
  cMpgUnbufferedFile *NextFile(void);
  };











//M7x0 BEGIN AK
#define MAX_BUFFERED_FRAMES 10
class cMpgFrameM7x0 {
  friend class cMpgRingBufferFrameM7x0;
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
  cMpgFrameM7x0() { Reset(); }
  uchar *Data1(void) const { return data1; }
  int Count1(void) const { return count1; }
  uchar *Data2(void) const { return data2; }
  int Count2(void) const { return count2; }
  eFrameType Type(void) const { return type; }
  int Index(void) const { return index; }
  int FileNo(void) const { return fileNo; }
  };












class cMpgRingBufferFrameM7x0 : public cRingBuffer {
private:
  int alignment;

  int curFileNr;
  int curFileOffset;
  int readAHeadCount;

  uchar *buffer;
  volatile int head;
  volatile int tail;

  cMpgFrameM7x0 frameBuffer[MAX_BUFFERED_FRAMES + 1];
  volatile int frameHead;
  volatile int frameTail;

  int ReadReal(cMpgUnbufferedFile *file, uchar *buf, int count);
public:
  cMpgRingBufferFrameM7x0(int Size, int Alignment, bool Statistics = false , const char *Description = NULL);
  virtual ~cMpgRingBufferFrameM7x0();
  int Read(cMpgUnbufferedFile *file, int FileNr, off64_t Offset, int FrameSize, int Index);
  int Read(cMpgFileName *fileName, int FileNr, off64_t Offset, int Size, int Index);
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
  cMpgFrameM7x0 *Get(void);
  void Drop(cMpgFrameM7x0 *frame = NULL);
  };


//M7X0 BEGIN AK
class cMpgNonBlockingFileReader : public cThread {
private:
  cMpgFileName *file;
  cMpgRingBufferFrameM7x0 *ringBuffer;
  int fileNr;
  off64_t offset;
  int index;
  int wanted;
  int length;
  bool hasData; 
  cCondWait newSet;
  cCondVar newDataCond;
  cMutex newDataMutex;

  volatile int actionLock;
  volatile int otherLock;

  inline void FasterLockAction(void) __attribute__ ((always_inline));
  inline void FasterUnlockAction(void) __attribute__ ((always_inline));
  inline void FasterLockOther(void) __attribute__ ((always_inline));
  inline void FasterUnlockOther(void) __attribute__ ((always_inline));
protected:
  void Action(void);
public:
  cMpgNonBlockingFileReader(cMpgRingBufferFrameM7x0 *RingBuffer);
  ~cMpgNonBlockingFileReader();
  void Clear(void);
  int Read(cMpgFileName *File, int FileNr, off64_t Offset, int Length, int Index);
  bool Reading(void) { return file; }
  bool WaitForDataMs(int msToWait);
  };



// --- cNonBlockingFileReader ------------------------------------------------

//M7x0 END AK






class cMpgPlayerControl : public cControl {
private:
  cMpgPlayer *player;
protected:
  cMpgRecording *Recording(void);

public:
  cMpgPlayerControl(const cMpgRecording *Rec);
       // Sets up a player for the given file.
  virtual ~cMpgPlayerControl();
  bool Active(void);
  void Stop(void);
       // Stops the current replay session (if any).
  void Pause(void);
       // Pauses the current replay session, or resumes a paused session.
  void Play(void);
       // Resumes normal replay mode.
  void Forward(void);
       // Runs the current replay session forward at a higher speed.
  void Backward(void);
       // Runs the current replay session backwards at a higher speed.

  int  SkipFrames(int Frames);
       // Returns the new index into the current replay session after skipping
       // the given number of frames (no actual repositioning is done!).
       // The sign of 'Frames' determines the direction in which to skip.
  void SkipSeconds(int Seconds);
       // Skips the given number of seconds in the current replay session.
       // The sign of 'Seconds' determines the direction in which to skip.
       // Use a very large negative value to go all the way back to the
       // beginning of the recording.
  bool GetIndex(int &Current, int &Total, bool SnapToIFrame = false);
       // Returns the current and total frame index, optionally snapped to the
       // nearest I-frame.

  bool GetIndex(int &Current, int &Total, int &CurrentTime, int &TotalTime);

  bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
       // Returns the current replay mode (if applicable).
       // 'Play' tells whether we are playing or pausing, 'Forward' tells whether
       // we are going forward or backward and 'Speed' is -1 if this is normal
       // play/pause mode, 0 if it is single speed fast/slow forward/back mode
       // and >0 if this is multi speed mode.
  void Goto(int Index, bool Still = false);
       // Positions to the given index and displays that frame as a still picture
       // if Still is true.
  };



class cMpgIndexFile {
private:
  struct tIndex { int offset; uchar type; uchar number; short reserved; };
  int f;
  char *fileName;
  int size, last;
  tIndex *index;
  cResumeFile resumeFile;
  cMutex mutex;
  bool CatchUp(int Index = -1);
public:
  cMpgIndexFile(const char *FileName, bool Record);
  ~cMpgIndexFile();
  bool Ok(void) { return index != NULL; }
  bool Write(uchar PictureType, uchar FileNumber, int FileOffset);
//M7X0 BEGIN AK
  bool Write(sPesResult *Picture, int PictureCount , uchar FileNumber, int FileOffset);
  int StripOffLast(void);
//M7X0 END AK
  bool Get(int Index, uchar *FileNumber, off64_t *FileOffset, uchar *PictureType = NULL, int *Length = NULL);
  int GetNextIFrame(int Index, bool Forward, uchar *FileNumber = NULL, off64_t *FileOffset = NULL, int *Length = NULL, bool StayOffEnd = false);
  int Get(uchar FileNumber, off64_t FileOffset);
  int Last(void) { CatchUp(); return last; }
  int GetResume(void) { return resumeFile.Read(); }
  bool StoreResume(int Index) { return resumeFile.Save(Index); }
  bool IsStillRecording(void);
  };

#endif //__MPGPLAYER_H

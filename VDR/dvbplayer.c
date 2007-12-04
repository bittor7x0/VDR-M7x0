/*
 * dvbplayer.c: The DVB player
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "dvbplayer.h"
#include <stdlib.h>
#include "recording.h"
#include "remux.h"
#include "ringbuffer.h"
#include "thread.h"
#include "tools.h"

// --- cBackTrace ------------------------------------------------------------

#define AVG_FRAME_SIZE 15000         // an assumption about the average frame size
#define DVB_BUF_SIZE   (2560 * 1024)  // an assumption about the dvb firmware buffer size
#define BACKTRACE_ENTRIES (DVB_BUF_SIZE / AVG_FRAME_SIZE + 20) // how many entries are needed to backtrace buffer contents

class cBackTrace {
private:
  int index[BACKTRACE_ENTRIES];
  int length[BACKTRACE_ENTRIES];
  int pos, num;
public:
  cBackTrace(void);
  void Clear(void);
  void Add(int Index, int Length);
  int Get(bool Forward);
  };

cBackTrace::cBackTrace(void)
{
  Clear();
}

void cBackTrace::Clear(void)
{
  pos = num = 0;
}

void cBackTrace::Add(int Index, int Length)
{
  index[pos] = Index;
  length[pos] = Length;
  if (++pos >= BACKTRACE_ENTRIES)
     pos = 0;
  if (num < BACKTRACE_ENTRIES)
     num++;
}

int cBackTrace::Get(bool Forward)
{
  int p = pos;
  int n = num;
  int l = DVB_BUF_SIZE + (Forward ? 0 : 256 * 1024); //XXX (256 * 1024) == DVB_BUF_SIZE ???
  int i = -1;

  while (n && l > 0) {
        if (--p < 0)
           p = BACKTRACE_ENTRIES - 1;
        i = index[p] - 1;
        l -= length[p];
        n--;
        }
  return i;
}

// --- cNonBlockingFileReader ------------------------------------------------
//M7X0 BEGIN AK
class cNonBlockingFileReader : public cThread {
private:
  cFileName *file;
  cRingBufferFrameM7x0 *ringBuffer;
  int fileNr;
  int offset;
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
  cNonBlockingFileReader(cRingBufferFrameM7x0 *RingBuffer);
  ~cNonBlockingFileReader();
  void Clear(void);
  int Read(cFileName *File, int FileNr, int Offset, int Length, int Index);
  bool Reading(void) { return file; }
  bool WaitForDataMs(int msToWait);
  };

cNonBlockingFileReader::cNonBlockingFileReader(cRingBufferFrameM7x0 *RingBuffer)
:cThread("non blocking file reader")
{
  actionLock = 0;
  otherLock = 0;

  ringBuffer = RingBuffer;
  file = NULL;
  fileNr = -1;
  offset = 0;
  index = -1;
  wanted = length = 0;
  hasData = false;
  Start();
}

cNonBlockingFileReader::~cNonBlockingFileReader()
{
  //newSet.Signal();
  Cancel(3);
}

#define LOCKWAITTIME 10 // ms
void cNonBlockingFileReader::FasterLockAction(void)
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

void cNonBlockingFileReader::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cNonBlockingFileReader::FasterLockOther(void)
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

void cNonBlockingFileReader::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cNonBlockingFileReader::Clear(void)
{
  FasterLockOther();
  file = NULL;
  hasData = false;
  fileNr = -1;
  offset = 0;
  index = -1;
  wanted = length = 0;
  FasterUnlockOther();

}

int cNonBlockingFileReader::Read(cFileName *File, int FileNr, int Offset, int Length, int Index)
{
  if (hasData && file) {
     if (fileNr != FileNr || offset != Offset || wanted != Length) {
        esyslog("ERROR: cNonBlockingFileReader::Read() called with different file offset!");
        errno = EINVAL;
        return -1;
        }
     file = NULL;
     return length;
     }
  if (!file) {
     fileNr = FileNr;
     offset = Offset;
     index = Index;
     wanted = Length;
     length = 0;
     hasData = false;
     file = File;
     //newSet.Signal();
     }
  errno = EAGAIN;
  return -1;
}

void cNonBlockingFileReader::Action(void)
{
  while (Running()) {
        FasterLockAction();
        if (!hasData && file) {
           int r = ringBuffer->Read(file, fileNr, offset, wanted, index);
           if (r >= 0) {
              length = r;
              //cMutexLock NewDataLock(&newDataMutex);
              hasData = true;
              //newDataCond.Broadcast();
              }
           else if (r < 0 && FATALERRNO) {
              LOG_ERROR;
              length = r; // this will forward the error status to the caller
              hasData = true;
              }
           FasterUnlockAction();
           }
        else {
           FasterUnlockAction();
           cCondWait::SleepMs(3);
           //newSet.Wait(1000);
           }
        }
}

bool cNonBlockingFileReader::WaitForDataMs(int msToWait)
{
  while (!hasData && msToWait > 0) {
        cCondWait::SleepMs(3);
        msToWait -= 3;
        }
  return hasData;
  /* if (hasData)
     return true;
  cMutexLock NewDataLock(&newDataMutex);
  return newDataCond.TimedWait(newDataMutex, msToWait);*/
}

// --- cDvbPlayer ------------------------------------------------------------

#define PLAYERBUFSIZE  MEGABYTE(1)
#define PLAYERBUFALIGNMENT KILOBYTE(64)
#define SLEEPWAITTIME 3  // ms
// The number of frames to back up when resuming an interrupted replay session:
#define RESUMEBACKUP (10 * FRAMESPERSEC)

class cDvbPlayer : public cPlayer, cThread {
private:
  enum ePlayModes { pmPlay, pmPause, pmSlow, pmFast, pmStill };
  enum ePlayDirs { pdForward, pdBackward };
  //static int Speeds[];
  cNonBlockingFileReader *nonBlockingFileReader;
  cRingBufferFrameM7x0 *ringBuffer;
  cBackTrace *backTrace;
  cFileName *fileName;
  cIndexFile *index;
  cUnbufferedFile *replayFile;
  bool eof;
  bool firstPacket;
  ePlayModes playMode;
  ePlayDirs playDir;
  int trickSpeed;

  int readIndex, writeIndex;
  uchar fileNr;
  int fileOffset;
  int readFrameLength;
  cFrameM7x0 *playFrame;

  volatile int actionLock;
  volatile int otherLock;

  inline void FasterLockAction(void) __attribute__ ((always_inline));
  inline void FasterUnlockAction(void) __attribute__ ((always_inline));
  inline void FasterLockOther(void) __attribute__ ((always_inline));
  inline void FasterUnlockOther(void) __attribute__ ((always_inline));

  void TrickSpeed(int Increment, bool lock = true);
  void Empty(void);
  bool NextFile(void);
  int Resume(void);
  bool Save(void);
protected:
  virtual void Activate(bool On);
  virtual void Action(void);
public:
  cDvbPlayer(const char *FileName);
  virtual ~cDvbPlayer();
  bool Active(void) { return cThread::Running(); }
  void Pause(void);
  void Play(void);
  void Forward(void);
  void Backward(void);
  int SkipFrames(int Frames);
  void SkipSeconds(int Seconds);
  void Goto(int Position, bool Still = false);
  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame = false);
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
  };


#define NORMAL_SPEED  0
#define MAX_SPEEDS    3
#define LOW_SPEEDS    0
cDvbPlayer::cDvbPlayer(const char *FileName)
:cThread("dvbplayer")
{
  nonBlockingFileReader = NULL;
  ringBuffer = NULL;
  backTrace = NULL;
  index = NULL;
  eof = false;
  firstPacket = true;
  playMode = pmPlay;
  playDir = pdForward;
  trickSpeed = NORMAL_SPEED;
  readIndex = writeIndex = -1;

  fileOffset = 0;
  readFrameLength = 0;

  playFrame = NULL;
  isyslog("replay %s", FileName);
//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
  fileName = new cFileName(FileName, false, true, true);
#else
  fileName = new cFileName(FileName, false);
#endif
//M7X0 END AK
  replayFile = fileName->Open();
  if (!replayFile)
     return;
  fileNr = fileName->Number();

  ringBuffer = new cRingBufferFrameM7x0(PLAYERBUFSIZE, PLAYERBUFALIGNMENT);
  ringBuffer->SetTimeouts(40,0);
  // Create the index file:
  index = new cIndexFile(FileName, false);
  if (!index)
     esyslog("ERROR: can't allocate index");
  else if (!index->Ok()) {
     delete index;
     index = NULL;
     }
  backTrace = new cBackTrace;
}

cDvbPlayer::~cDvbPlayer()
{
  Detach();
  Save();
  delete index;
  delete fileName;
  delete backTrace;
  delete ringBuffer;
}

void cDvbPlayer::FasterLockAction(void)
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

void cDvbPlayer::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cDvbPlayer::FasterLockOther(void)
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

void cDvbPlayer::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cDvbPlayer::TrickSpeed(int Increment, bool lock)
{
  int nts = trickSpeed + Increment;

  if (!nts) {
     trickSpeed = nts;
     if (playMode == pmFast)
        Play();
     else
        Pause();
     }
  else if (nts >= -MAX_SPEEDS && nts <= MAX_SPEEDS) {
     if (lock)
        FasterLockOther();
     int sp = nts - LOW_SPEEDS;
     sp = nts > 0 ? (sp >= 0 ? 1 : 1 << (-sp)) : 1 << (MAX_SPEEDS + nts + 1);
     if (playMode == pmSlow && playDir == pdBackward)
        sp <<= 2;
     DeviceTrickSpeed(sp,playMode==pmFast||playDir==pdBackward);
     trickSpeed = nts;
     if (lock)
        FasterUnlockOther();
     }
}

void cDvbPlayer::Empty(void)
{
  //LOCK_THREAD;
  if (nonBlockingFileReader)
     nonBlockingFileReader->Clear();
  if (playMode == pmFast || playMode == pmSlow || (readIndex = backTrace->Get(playDir == pdForward)) < 0)
     readIndex = writeIndex;
  writeIndex = readIndex;
  if ((readIndex = index->GetNextIFrame(readIndex + (playDir == pdBackward ? -1 : 1) ,playDir == pdBackward) - 1) < 0)
      readIndex = writeIndex;
  writeIndex = readIndex;
  playFrame = NULL;
  ringBuffer->Clear();
  backTrace->Clear();
  DeviceClear();
  firstPacket = true;
}

bool cDvbPlayer::NextFile(void)
{
  if (replayFile && eof) {
     replayFile = fileName->NextFile();
     fileNr = fileName->Number();
     fileOffset = 0;
     }
  eof = false;
  return replayFile != NULL;
}

int cDvbPlayer::Resume(void)
{
  if (index) {
     int Index = index->GetResume();
     if (Index >= 0) {
        if (index->Get(Index, &fileNr, &fileOffset, NULL, &readFrameLength))
           return Index;
        }
     }
  return -1;
}

bool cDvbPlayer::Save(void)
{
  if (index) {
     int Index = writeIndex;
     if (Index >= 0) {
        Index -= RESUMEBACKUP;
        if (Index > 0)
           Index = index->GetNextIFrame(Index, false);
        else
           Index = 0;
        if (Index >= 0)
           return index->StoreResume(Index);
        }
     }
  return false;
}

void cDvbPlayer::Activate(bool On)
{
  if (On) {
     actionLock = 0;
     otherLock = 0;
     if (replayFile)
        Start();
     }
  else
     Cancel(9);
}

void cDvbPlayer::Action(void)
{

  uchar *data1 = NULL;
  uchar *data2 = NULL;
  int count1 = 0;
  int count2 = 0;

  readIndex = Resume();

  if (readIndex >= 0) {
     isyslog("resuming replay at index %d (%s)", readIndex, *IndexToHMSF(readIndex, true));
     readIndex --; // Gets incremented in loop first
     }


  nonBlockingFileReader = new cNonBlockingFileReader(ringBuffer);

  bool Sleep = false;
  bool WaitingForData = false;

  cPoller Poller;

  while (Running() && (NextFile() || readIndex >= 0 || ringBuffer->Available() || !DeviceFlush(100))) {

        if (Sleep) {
           if (WaitingForData)
              nonBlockingFileReader->WaitForDataMs(SLEEPWAITTIME); // this keeps the CPU load low, but reacts immediately on new data
           else
              cCondWait::SleepMs(SLEEPWAITTIME); // this keeps the CPU load low
           Sleep = false;
           WaitingForData = false;
           }

        if (DevicePoll(Poller, 100)) {
           //LOCK_THREAD;
           FasterLockAction();

           if (playMode == pmStill || playMode == pmPause) {
              Sleep = true;
              FasterUnlockAction();
              continue;
              }

           if (replayFile || readIndex >= 0) {
              if (!nonBlockingFileReader->Reading()) {
                 if (playMode == pmFast || (playMode == pmSlow && playDir == pdBackward)) {

                    bool TimeShiftMode = index->IsStillRecording();
                    int Index = readIndex;
                    int skipIFrames = trickSpeed - LOW_SPEEDS;

                    while (true) {
                          Index = index->GetNextIFrame(Index, playDir == pdForward, &fileNr, &fileOffset, &readFrameLength, TimeShiftMode);
                          skipIFrames--;
                          if (skipIFrames <= 0 || Index < 0)
                             break;
                          }
                    if (Index < 0) {
                       if (!TimeShiftMode && playDir == pdForward) {
                          // hit end of recording: signal end of file but don't change playMode
                          readIndex = -1;
                          eof = true;
                          FasterUnlockAction();
                          continue;
                          }
                       // hit begin of recording: wait for device buffers to drain
                       // before changing play mode:
                       if (!DeviceFlush(100)) {
                          FasterUnlockAction();
                          continue;
                          }
                       // can't call Play() here, because those functions may only be
                       // called from the foreground thread - and we also don't need
                       // to empty the buffer here
                       DevicePlay();
                       backTrace->Clear();
                       playMode = pmPlay;
                       playDir = pdForward;
                       FasterUnlockAction();
                       continue;
                       }
                    readIndex = Index;
                    }
                 else if (index) {  // Normal play mode
                    readIndex++;
                    if (!index->Get(readIndex, &fileNr, &fileOffset, NULL, &readFrameLength)) {
                       readIndex = -1;
                       eof = true;
                       FasterUnlockAction();
                       continue;
                       }
                    }
                 else {
                    // allows replay even if the index file is missing
                    fileOffset += MAXFRAMESIZE;
                    readFrameLength = MAXFRAMESIZE;
                    }

                 if (readFrameLength == -1)
                    readFrameLength = MAXFRAMESIZE; // this means we read up to EOF (see cIndex)
                 else if (readFrameLength > MAXFRAMESIZE) {
                    esyslog("ERROR: frame larger than buffer (%d > %d)", readFrameLength, MAXFRAMESIZE);
                    readFrameLength = MAXFRAMESIZE;
                    }
                 }
              int r = nonBlockingFileReader->Read(fileName, fileNr, fileOffset, readFrameLength, readIndex);
              if (r < 0) {
                 if (errno == EAGAIN)
                    WaitingForData = true;
                 else if(FATALERRNO) {
                    LOG_ERROR;
                    FasterUnlockAction();
                    break;
                    }
                 }
              else if (r < readFrameLength)
                    eof = true;

              }

           if (!playFrame) {
              playFrame = ringBuffer->Get();
              data1 = NULL;
              }

           if (playFrame) {
              if (!data1) {
                 data1 = playFrame->Data1();
                 count1 = playFrame->Count1();
                 data2 = playFrame->Data2();
                 count2 = playFrame->Count2();
                 if (firstPacket) {
                    PlayPes(NULL, 0);
                    cRemux::SetBrokenLink(data1, count1);
                    firstPacket = false;
                    }
                 }

              int playedBytes = PlayPes(data1, count1, playMode != pmPlay);
              if (playedBytes < 0) {
                 if (FATALERRNO) {
                    LOG_ERROR;
                    FasterUnlockAction();
                    break;
                    }
                 FasterUnlockAction();
                 continue;
                 }

              data1 += playedBytes;
              count1 -= playedBytes;
              if (!count1 && count2) {
                 data1 = data2;
                 count1 = count2;
                 count2 = 0;

                 playedBytes = PlayPes(data1, count1, playMode != pmPlay);
                 if (playedBytes < 0) {
                    if (FATALERRNO) {
                       LOG_ERROR;
                       FasterUnlockAction();
                       break;
                       }
                    FasterUnlockAction();
                    continue;
                    }

                 data1 += playedBytes;
                 count1 -= playedBytes;
                 }

              if (!count1) {
                 writeIndex = playFrame->Index();
                 backTrace->Add(playFrame->Index(), playFrame->Count1() + playFrame->Count2());
                 ringBuffer->Drop(playFrame);
                 playFrame = NULL;
                 }
              }
           else
              Sleep = true;
           FasterUnlockAction();
           }
        }

  cNonBlockingFileReader *nbfr = nonBlockingFileReader;
  nonBlockingFileReader = NULL;
  delete nbfr;
}

void cDvbPlayer::Pause(void)
{
  if (playMode == pmPause || playMode == pmStill)
     Play();
  else {
     //LOCK_THREAD;
     FasterLockOther();
     // if (playMode == pmFast || (playMode == pmSlow && playDir == pdBackward))
     dsyslog("Pause : Read Index %d Write Index %d", readIndex , writeIndex);
     Empty();
     DeviceFreeze();
     playMode = pmPause;
     FasterUnlockOther();
     }
}

void cDvbPlayer::Play(void)
{
  if (playMode == pmPlay)
     Pause();
  else {
     //LOCK_THREAD;
     FasterLockOther();
     if (playMode == pmFast || playMode == pmSlow)
        Empty();

     dsyslog("Play: Read Index %d Write Index %d", readIndex , writeIndex);
     DevicePlay();
     playMode = pmPlay;
     playDir = pdForward;
     FasterUnlockOther();
     }
}

void cDvbPlayer::Forward(void)
{
  if (index) {
     switch (playMode) {
       case pmFast:
            if (Setup.MultiSpeedMode) {
               TrickSpeed(playDir == pdForward ? 1 : -1);
               break;
               }
            else if (playDir == pdForward) {
               Play();
               break;
               }
            // run into pmPlay
       case pmPlay: {
            //LOCK_THREAD;
            FasterLockOther();
            Empty();
            DeviceMute();
            playMode = pmFast;
            playDir = pdForward;
            trickSpeed = NORMAL_SPEED;
            TrickSpeed(Setup.MultiSpeedMode ? 1 : MAX_SPEEDS, false);
            FasterUnlockOther();
            }
            break;
       case pmSlow:
            if (Setup.MultiSpeedMode) {
               TrickSpeed(playDir == pdForward ? -1 : 1);
               break;
               }
            else if (playDir == pdForward) {
               Pause();
               break;
               }
            // run into pmPause
       case pmStill:
       case pmPause: {
            //LOCK_THREAD;
            FasterLockOther();
            DeviceMute();
            playDir = pdForward;
            playMode = pmSlow;
            trickSpeed = NORMAL_SPEED;
            TrickSpeed(Setup.MultiSpeedMode ? -1 : -MAX_SPEEDS, false);
            FasterUnlockOther();
            }
            break;
       }
     }
}

void cDvbPlayer::Backward(void)
{
  if (index) {
     switch (playMode) {
       case pmFast:
            if (Setup.MultiSpeedMode) {
               TrickSpeed(playDir == pdBackward ? 1 : -1);
               break;
               }
            else if (playDir == pdBackward) {
               Play();
               break;
               }
            // run into pmPlay
       case pmPlay: {
            //LOCK_THREAD;
            FasterLockOther();
            Empty();
            DeviceMute();
            playMode = pmFast;
            playDir = pdBackward;
            trickSpeed = NORMAL_SPEED;
            TrickSpeed(Setup.MultiSpeedMode ? 1 : MAX_SPEEDS, false);
            FasterUnlockOther();
            }
            break;
       case pmSlow:
            if (Setup.MultiSpeedMode) {
               TrickSpeed(playDir == pdBackward ? -1 : 1);
               break;
               }
            else if (playDir == pdBackward) {
               Pause();
               break;
               }
            // run into pmPause
       case pmStill:
       case pmPause: {
            //LOCK_THREAD;
            FasterLockOther();
            Empty();
            DeviceMute();
            playMode = pmSlow;
            playDir = pdBackward;
            trickSpeed = NORMAL_SPEED;
            TrickSpeed(Setup.MultiSpeedMode ? -1 : -MAX_SPEEDS, false);
            FasterUnlockOther();
            }
            break;
       }
     }
}

int cDvbPlayer::SkipFrames(int Frames)
{
  if (index && Frames) {
     int Current, Total;
     GetIndex(Current, Total, true);
     int OldCurrent = Current;
     // As GetNextIFrame() increments/decrements at least once, the
     // destination frame (= Current + Frames) must be adjusted by
     // -1/+1 respectively.
     Current = index->GetNextIFrame(Current + Frames + (Frames > 0 ? -1 : 1), Frames > 0);
     return Current >= 0 ? Current : OldCurrent;
     }
  return -1;
}

void cDvbPlayer::SkipSeconds(int Seconds)
{
  if (index && Seconds) {
     //LOCK_THREAD;
     if (playMode != pmPlay)
        Play();
     FasterLockOther();
     Empty();
     int Index = writeIndex;
     if (Index >= 0) {
        Index = max(Index + Seconds * FRAMESPERSEC, 0);
        if (Index > 0)
           Index = index->GetNextIFrame(Index, false, NULL, NULL, NULL, true);
        if (Index >= 0)
           readIndex = writeIndex = Index - 1; // Action() will first increment it!
        }
     FasterUnlockOther();
     }
}

void cDvbPlayer::Goto(int Index, bool Still)
{
  if (index) {
     //LOCK_THREAD;
     FasterLockOther();
     Empty();
     if (++Index <= 0)
        Index = 1; // not '0', to allow GetNextIFrame() below to work!
     uchar FileNumber;
     int FileOffset, Length;
     Index = index->GetNextIFrame(Index, false, &FileNumber, &FileOffset, &Length);
     if (Index >= 0 && Still) {
        int r = 0;
        for (;;) {
            r = ringBuffer->Read(fileName, FileNumber, FileOffset, Length, Index);
            if (r < 0 && FATALERRNO) {
               LOG_ERROR;
               break;
               }
            if (r >= 0 )
               break;
            }
        cFrameM7x0 *frame = ringBuffer->Get();
        Index++;
        if (r > 0 && frame) {
           uchar *b = frame->Data1();
           r = frame->Count1();
           if (frame->Count2()) {
              b = MALLOC(uchar,r + frame->Count2());
              if (b) {
                 memcpy(b, frame->Data1(), r);
                 memcpy(b + r, frame->Data2(), frame->Count2());
                 r += frame->Count2();
                 }
              else
                 r = 0;
              }
           if (playMode == pmPause)
              DevicePlay();
#if 0
           // append sequence end code to get the image shown immediately with softdevices
           if (r > 6 && (b[3] & 0xF0) == 0xE0) { // make sure to append it only to a video packet
              b[r++] = 0x00;
              b[r++] = 0x00;
              b[r++] = 0x01;
              b[r++] = b[3];
              if (b[6] & 0x80) { // MPEG 2
                 b[r++] = 0x00;
                 b[r++] = 0x07;
                 b[r++] = 0x80;
                 b[r++] = 0x00;
                 b[r++] = 0x00;
                 }
              else { // MPEG 1
                 b[r++] = 0x00;
                 b[r++] = 0x05;
                 b[r++] = 0x0F;
                 }
              b[r++] = 0x00;
              b[r++] = 0x00;
              b[r++] = 0x01;
              b[r++] = 0xB7;
              }
#endif
           if (r > 6)
              DeviceStillPicture(b, r);
           if (frame->Count2())
              free(b);
           }
        playMode = pmStill;
        }
     readIndex = writeIndex = Index - 1;
     FasterUnlockOther();
     }
}

bool cDvbPlayer::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  if (index) {
     if (playMode == pmStill)
        Current = max(readIndex, 0);
     else {
        Current = max(writeIndex, 0);
        if (playMode == pmPlay) {
           int backed = backTrace->Get(playDir == pdForward);
           if (backed >= 0)
              Current = backed;
           }
        if (SnapToIFrame) {
           int i1 = index->GetNextIFrame(Current + 1, false);
           int i2 = index->GetNextIFrame(Current, true);
           Current = (abs(Current - i1) <= abs(Current - i2)) ? i1 : i2;
           }
        }
     Total = index->Last();
     return true;
     }
  Current = Total = -1;
  return false;
}

bool cDvbPlayer::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play = (playMode == pmPlay || playMode == pmFast);
  Forward = (playDir == pdForward);
  if (playMode == pmFast || playMode == pmSlow)
     Speed = Setup.MultiSpeedMode ? abs(trickSpeed - NORMAL_SPEED) : 0;
  else
     Speed = -1;
  return true;
}

// --- cDvbPlayerControl -----------------------------------------------------

cDvbPlayerControl::cDvbPlayerControl(const char *FileName)
:cControl(player = new cDvbPlayer(FileName))
{
}

cDvbPlayerControl::~cDvbPlayerControl()
{
  Stop();
}

bool cDvbPlayerControl::Active(void)
{
  return player && player->Active();
}

void cDvbPlayerControl::Stop(void)
{
  delete player;
  player = NULL;
}

void cDvbPlayerControl::Pause(void)
{
  if (player)
     player->Pause();
}

void cDvbPlayerControl::Play(void)
{
  if (player)
     player->Play();
}

void cDvbPlayerControl::Forward(void)
{
  if (player)
     player->Forward();
}

void cDvbPlayerControl::Backward(void)
{
  if (player)
     player->Backward();
}

void cDvbPlayerControl::SkipSeconds(int Seconds)
{
  if (player)
     player->SkipSeconds(Seconds);
}

int cDvbPlayerControl::SkipFrames(int Frames)
{
  if (player)
     return player->SkipFrames(Frames);
  return -1;
}

bool cDvbPlayerControl::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  if (player) {
     player->GetIndex(Current, Total, SnapToIFrame);
     return true;
     }
  return false;
}

bool cDvbPlayerControl::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  return player && player->GetReplayMode(Play, Forward, Speed);
}

void cDvbPlayerControl::Goto(int Position, bool Still)
{
  if (player)
     player->Goto(Position, Still);
}

/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_player.c 110 2008-08-30 17:42:37Z andreas $
 *
 */

#include "cridplayer_player.h"
#include <stdlib.h>
#include <vdr/ringbuffer.h>
#include <vdr/thread.h>
#include <vdr/tools.h>

// --- cBackTrace ------------------------------------------------------------

#define CRID_AVG_GOP_SIZE 180000        // an assumption about the average gop size
#define CRID_DVB_BUF_SIZE (2560 * 1024)  // an assumption about the dvb firmware buffer size
#define CRID_BACKTRACE_ENTRIES (CRID_DVB_BUF_SIZE / CRID_AVG_GOP_SIZE + 20) // how many entries are needed to backtrace buffer contents

class cCridBackTrace {
private:
  int index[CRID_BACKTRACE_ENTRIES];
  int length[CRID_BACKTRACE_ENTRIES];
  int pos, num;
public:
  cCridBackTrace(void);
  void Clear(void);
  void Add(int Index, int Length);
  bool Get(bool Forward, int &idx);
  };

cCridBackTrace::cCridBackTrace(void)
{
  Clear();
}

void cCridBackTrace::Clear(void)
{
  pos = num = 0;
}

void cCridBackTrace::Add(int Index, int Length)
{
  index[pos] = Index;
  length[pos] = Length;
  if (++pos >= CRID_BACKTRACE_ENTRIES)
     pos = 0;
  if (num < CRID_BACKTRACE_ENTRIES)
     num++;
}

bool cCridBackTrace::Get(bool Forward, int &idx)
{
  int p = pos;
  int n = num;
  int l = CRID_DVB_BUF_SIZE + (Forward ? 0 : 256 * 1024); //XXX (256 * 1024) == DVB_BUF_SIZE ???
  int i = 0;
  bool r = false;
  while (n && l > 0) {
        if (--p < 0)
           p = CRID_BACKTRACE_ENTRIES - 1;
        i = index[p] - 1;
        l -= length[p];
        n--;
        r = true;
        }
  idx = i;
  return r;
}

// --- cNonBlockingFileReader ------------------------------------------------
//M7X0 BEGIN AK
class cCridFileReader : public cThread {
private:
  cUnbufferedFile *file;
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
  cCridFileReader(cRingBufferFrameM7x0 *RingBuffer);
  ~cCridFileReader();
  void Clear(void);
  int Read(cUnbufferedFile *File, int FileNr, int Offset, int Length, int Index);
  bool Reading(void) { return file; }
  bool WaitForDataMs(int msToWait);
  };

cCridFileReader::cCridFileReader(cRingBufferFrameM7x0 *RingBuffer)
: cThread("crid file reader")
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

cCridFileReader::~cCridFileReader()
{
  //newSet.Signal();
  Cancel(3);
}

#define LOCKWAITTIME 10 // ms
void cCridFileReader::FasterLockAction(void)
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

void cCridFileReader::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cCridFileReader::FasterLockOther(void)
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

void cCridFileReader::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cCridFileReader::Clear(void)
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

int cCridFileReader::Read(cUnbufferedFile *File, int FileNr, int Offset, int Length, int Index)
{
  if (hasData && file) {
     if (fileNr != FileNr || offset != Offset || wanted != Length) {
        esyslog("ERROR: cCridFileReader::Read() called with different file offset!");
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

void cCridFileReader::Action(void)
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

bool cCridFileReader::WaitForDataMs(int msToWait)
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

// --- cCridPlayer ------------------------------------------------------------

#define CRID_PLAYERBUFSIZE  MEGABYTE(1)
#define CRID_PLAYERBUFALIGNMENT KILOBYTE(64)
#define CRID_SLEEPWAITTIME 3  // ms
// The number of frames to back up when resuming an interrupted replay session:
#define CRID_RESUMEBACKUP (10 * 2)
#define CRID_MAXGOPSIZE KILOBYTE(512)

class cCridPlayer : public cPlayer, cThread {
private:
  enum ePlayModes { pmPlay, pmPause, pmSlow, pmFast, pmStill };
  enum ePlayDirs { pdForward, pdBackward };

  cCridFileReader *cridFileReader;
  cRingBufferFrameM7x0 *ringBuffer;
  cCridBackTrace *backTrace;
  cCridRecording *rec;
  cCridIndex *index;
  cCridMarks *marks;
  cUnbufferedFile *replayFile;
  bool eof;
  bool firstPacket;
  ePlayModes playMode;
  ePlayDirs playDir;
  int trickSpeed;

  int readIndex, writeIndex;
  int fileNr;
  int writeFileNr;
  int fileOffset;
  int readGopLength;
  int lastReadLength;
  cFrameM7x0 *playGop;

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
  uchar *GetIFrameElementary(cFrameM7x0 *frame, const int VideoPid,
                                         int &Offset, int &Length);
  bool SyncTs(const uchar *&Data, const uchar *const Limit);
  bool CheckTsHeader(const uchar *const Data,int &cCounter,
                    bool &payloadStart, int &payloadOffset);
  bool CheckPesHeader(const uchar *Data, int &Offset, uchar *Header,
                                    int &HeaderLength, int &PesDone,
                                    bool &HeadCompleted, bool Start);

protected:
  virtual void Activate(bool On);
  virtual void Action(void);
public:
  cCridPlayer(const cCridRecording *Rec);
  virtual ~cCridPlayer();
  bool Active(void) { return cThread::Running(); }
  void Pause(void);
  void Play(void);
  void Forward(void);
  void Backward(void);
  void SkipSeconds(int Seconds);
  void Goto(int Position, bool Still = false);
  void GotoTime(int32_t Pos, bool Still = false);
  cCridMarks *GetMarks(void) { return marks; }
  cCridRecording *Recording(void) { return rec;}

  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame = false) {
     esyslog("bool GetIndex(int&, int&, bool) called");
     return false;
     }
  bool GetIndex(int &Current, int &Total, int &CurrentTime, int &TotalTime, bool SnapToIFrame = false);
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
  };


#define NORMAL_SPEED  0
#define MAX_SPEEDS    3
#define LOW_SPEEDS    0
cCridPlayer::cCridPlayer(const cCridRecording *Rec)
:cPlayer(pmTsAudioVideo), cThread("crid player")
{
  rec = new cCridRecording(*Rec);
  cridFileReader = NULL;
  ringBuffer = NULL;
  backTrace = NULL;
  index = NULL;
  eof = false;
  firstPacket = true;
  playMode = pmPlay;
  playDir = pdForward;
  trickSpeed = NORMAL_SPEED;
  readIndex = writeIndex = INT_MIN;
  fileOffset = 0;
  readGopLength = 0;
  lastReadLength = 0;
  writeFileNr = -1;
  playGop = NULL;
  marks = NULL;

  // Create the index file:
  if (!rec) {
     esyslog("ERROR: can't allocate recording");
     return;
     }

  index = rec->GetIndex();
  if (!index) {
     esyslog("ERROR: can't allocate index");
     return;
     }

  if (!index->Read()) {
     esyslog("Cannot read mpeg filenames");
     delete index;
     index = NULL;
     return;
     }

  if (index->IndexOkay()) {
     marks = new cCridMarks(rec->GetPath(), rec->GetFilename(), index);
     if (marks)
        marks->Load();
     }
  ringBuffer = new cRingBufferFrameM7x0(CRID_PLAYERBUFSIZE, CRID_PLAYERBUFALIGNMENT);
  backTrace = new cCridBackTrace;
}

cCridPlayer::~cCridPlayer()
{
  Detach();
  Save();
  delete index;
  delete backTrace;
  delete ringBuffer;
  delete rec;
  delete marks;
}

void cCridPlayer::FasterLockAction(void)
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

void cCridPlayer::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cCridPlayer::FasterLockOther(void)
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

void cCridPlayer::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cCridPlayer::TrickSpeed(int Increment, bool lock)
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

void cCridPlayer::Empty(void)
{
  if (cridFileReader)
     cridFileReader->Clear();
  if (playMode == pmFast || playMode == pmSlow || backTrace->Get(playDir == pdForward, writeIndex))
     readIndex = writeIndex;
  writeIndex = readIndex;
  readGopLength = 0;
  playGop = NULL;
  ringBuffer->Clear();
  backTrace->Clear();
  DeviceClear();
  firstPacket = true;
}

bool cCridPlayer::NextFile(void)
{
  if (replayFile && eof) {
     replayFile = index->GetNextMpegFile();
     fileNr = index->GetMpegFileNumber();
     fileOffset = 0;
     }
  eof = false;
  return replayFile != NULL;
}

int cCridPlayer::Resume(void)
{
  if (index->IndexOkay())
     return rec->GetResume();
  return INT_MIN;
}

bool cCridPlayer::Save(void)
{
  if (marks)
     marks->Save();
  if (index && index->IndexOkay()) {
     int Index = writeIndex - CRID_RESUMEBACKUP;
     Index = Index > writeIndex ? INT_MIN : Index;
     return rec->StoreResume(Index);
     }

  return false;
}

void cCridPlayer::Activate(bool On)
{
  if (On) {
     actionLock = 0;
     otherLock = 0;
     if (index)
        Start();
     }
  else
     Cancel(9);
}

void cCridPlayer::Action(void)
{

  uchar *data1 = NULL;
  uchar *data2 = NULL;
  int count1 = 0;
  int count2 = 0;

  readIndex = Resume();

  if (readIndex > INT_MIN && index->IndexOkay()) {
     const int timestamp = index->GetTime(readIndex);
     isyslog("resuming replay at index %d (%02d:%02d)", readIndex,
                                 timestamp / 60 , timestamp % 60);
     }

  if (index->IndexOkay()) {
     readIndex = index->GetIndex(readIndex,fileNr,fileOffset,readGopLength);
     lastReadLength = min(CRID_MAXGOPSIZE, readGopLength);
     replayFile = index->GetMpegFile(fileNr);
     if (lastReadLength < 0) {
        esyslog("ERROR: illegale gop size %d", readGopLength);
        lastReadLength = readGopLength = CRID_MAXGOPSIZE;
        }
     }
  else {
    fileNr = 0;
    replayFile = index->GetMpegFile(0);
    }
  cridFileReader = new cCridFileReader(ringBuffer);

  bool Sleep = false;
  bool WaitingForData = false;
  cPoller Poller;

  while (Running() && (NextFile() || ringBuffer->Available() || !DeviceFlush(100))) {

        if (Sleep) {
           if (WaitingForData)
              cridFileReader->WaitForDataMs(CRID_SLEEPWAITTIME); // this keeps the CPU load low, but reacts immediately on new data
           else
              cCondWait::SleepMs(CRID_SLEEPWAITTIME); // this keeps the CPU load low
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

           if (replayFile) {
              if (!cridFileReader->Reading() && !readGopLength) {
                 if (playMode == pmFast || (playMode == pmSlow && playDir == pdBackward)) {

                    int skipIFrames = trickSpeed - LOW_SPEEDS;
                    if (skipIFrames <= 0)
                       skipIFrames = 1;

                    readIndex += playDir == pdForward ? skipIFrames : -skipIFrames;

                    int Index = index->GetIFrameIndex(readIndex, fileNr,
                                             fileOffset, readGopLength);

                    if (Index > readIndex) {
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
                       readIndex = Index;
                       FasterUnlockAction();
                       continue;
                       }
                    else if (Index < readIndex) {
                       // hit end of recording
                       replayFile = NULL;
                       FasterUnlockAction();
                       continue;
                       }
                    }
                 else if (index->IndexOkay()) {  // Normal play mode
                    readIndex++;
                    int Index = index->GetIndex(readIndex, fileNr, fileOffset,
                                                               readGopLength);
                    if (Index < readIndex) {
                       // hit end of recording
                       replayFile = NULL;
                       FasterUnlockAction();
                       continue;
                       }
                    readIndex = Index;
                    }
                 else {
                    // allows replay even if the index file is missing
                    fileOffset += CRID_MAXGOPSIZE;
                    readGopLength = CRID_MAXGOPSIZE;
                    }

                 lastReadLength = min(CRID_MAXGOPSIZE, readGopLength);
                 if (lastReadLength < 0) {
                    esyslog("ERROR: illegale gop size %d", readGopLength);
                    lastReadLength = readGopLength = CRID_MAXGOPSIZE;
                    }

                 replayFile = index->GetMpegFile(fileNr);
                 if (!replayFile) {
                    esyslog("ERROR: get not get mpeg file number %d",fileNr);
                    FasterUnlockAction();
                    continue;
                    }
                 }

              int r = cridFileReader->Read(replayFile, fileNr, fileOffset,
                                               lastReadLength, readIndex);
              if (r < 0) {
                 if (errno == EAGAIN)
                    WaitingForData = true;
                 else if(FATALERRNO) {
                    LOG_ERROR;
                    FasterUnlockAction();
                    break;
                    }
                 }
              else {
                  /* dsyslog("Read: File %d Offset 0x%X  0x%x  Read 0x%X Gop 0x%X Ind %d", fileNr, fileOffset,fileOffset + lastReadLength,
                                               lastReadLength,readGopLength, readIndex); */
                 fileOffset += lastReadLength;
                 readGopLength -= lastReadLength;
                 if (r < lastReadLength) {
                    readGopLength = 0;
                    eof = true;
                    }
                 lastReadLength = min(CRID_MAXGOPSIZE, readGopLength);
                 }
              }

           if (!playGop) {
              playGop = ringBuffer->Get();
              data1 = NULL;
              }

           if (playGop) {
              if (!data1) {
                 data1 = playGop->Data1();
                 count1 = playGop->Count1();
                 data2 = playGop->Data2();
                 count2 = playGop->Count2();
                 if (firstPacket) {
                    PlayTs(NULL, 0);
                    firstPacket = false;
                    }

                 if (writeFileNr != playGop->FileNo()) {
                    writeFileNr = playGop->FileNo();
                    int pmtPid = 0;
                    int videoPid = 0;
                    index->GetPids(writeFileNr,pmtPid, videoPid);
                    DeviceSetTsPids(pmtPid,videoPid);
                    }
                 }

              int playedBytes = PlayTs(data1, count1);
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

                 playedBytes = PlayTs(data1, count1);
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
                 writeIndex = playGop->Index();
                 backTrace->Add(playGop->Index(), playGop->Count1() + playGop->Count2());
                 ringBuffer->Drop(playGop);
                 playGop = NULL;
                 }
              }
           else
              Sleep = true;
           FasterUnlockAction();
           }
        }

  cCridFileReader *cfr = cridFileReader;
  cridFileReader = NULL;
  delete cfr;
}

void cCridPlayer::Pause(void)
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

void cCridPlayer::Play(void)
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

void cCridPlayer::Forward(void)
{
  if (index->IndexOkay()) {
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

void cCridPlayer::Backward(void)
{
  if (index->IndexOkay()) {
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


void cCridPlayer::SkipSeconds(int Seconds)
{
  if (index->IndexOkay() && Seconds) {
     //LOCK_THREAD;
     if (playMode != pmPlay)
        Play();
     FasterLockOther();
     Empty();
     index->GetIndexTimeRel(Seconds, writeIndex);
     readIndex = writeIndex;
     FasterUnlockOther();
     }
}

bool cCridPlayer::SyncTs(const uchar *&Data, const uchar *const Limit)
{
  const uchar *data = Data;
  while (data < Limit && data[0] != 0x47)
        data++;

  Data = data;
  if (data < Limit)
     return true;

  return false;
}

bool cCridPlayer::CheckTsHeader(const uchar *const Data,int &cCounter,
                                bool &payloadStart, int &payloadOffset)
{
  int newcCounter = Data[3] & 0xF;
  if (newcCounter == cCounter || !(Data[3] & 0x10)) {
     payloadOffset = 188;
     return true;
     }

  if (newcCounter != ((cCounter + 1) & 0xF) && cCounter != -1)
     return false;

  if (Data[1] & 0x80)
     return false;

  payloadOffset = 4 + ((Data[3] & 0x20) ? Data[4] + 1 : 0);

  if (payloadOffset >= 188)
     return false;

  cCounter = newcCounter;

  payloadStart = (Data[1] & 0x40);

  return true;
}

bool cCridPlayer::CheckPesHeader(const uchar *Data, int &Offset,
                 uchar *Header, int &HeaderLength, int &PesDone,
                                bool &HeaderCompleted, bool Start)
{
  if (!Start) {
     PesDone += 188 - Offset;
     if (!HeaderCompleted || Header[3] == 0xBE)
        Offset = 188;

     return true;
     }
  else {
     const int PesLength = BE2HOST(*((uint16_t *)(Header + 4)));
     if (HeaderCompleted && PesLength && PesDone != PesLength) {
        dsyslog("Illegal pes packet length in I-Frame");
        return false;
        }
     HeaderCompleted = false;
     HeaderLength = 0;
     PesDone = -6;
     }

  const uchar *data = Data + Offset;
  int length = 188 - Offset;

  if (HeaderLength < 7) {
     int c = min (7 - HeaderLength, length);
     memcpy(Header + HeaderLength, data, c);
     data += c;
     HeaderLength += c;
     length -= c;
     PesDone += c;

     if (HeaderLength < 7) {
        Offset = 188;
        return true;
        }
     }

  if (Header[0] != 0 || Header[1] != 0 || Header[2] != 1 ||
             (Header[3] != 0xBE && (Header[3] & 0xF0) != 0xE0)) {
     dsyslog("Illegal pes packet type in I-Frame");
     return false;
     }

  if (Header[3] == 0xBE) {
     HeaderCompleted = true;
     PesDone += length;
     Offset = 188;
     return true;
     }

  if ((Header[6] & 0xC0) == 0x80) {
     if (!length) {
        Offset = 188;
        return true;
        }

     if (HeaderLength < 9) {
        int c = min(9 - HeaderLength, length);
        memcpy(Header + HeaderLength, data, c);
        data += c;
        HeaderLength += c;
        length -= c;
        PesDone += c;

        if (HeaderLength < 9) {
           Offset = 188;
           return true;
           }
        }

     int c = Header[8];
     c -= HeaderLength - 9;
     if (c > length) {
        HeaderLength += length;
        PesDone += length;
        Offset = 188;
        return true;
        }

     length -= c;
     PesDone += c;
     HeaderLength += c;
     HeaderCompleted = true;
     Offset = 188 - length;
     return true;
     }

  int start = 6;
  while (start < HeaderLength && Header[start] == 0xFF)
        start++;

  if (start == HeaderLength) {
     int c = 0;
     while (c < length && data[c] == 0xFF)
           c++;

     memset(Header + HeaderLength, 0xFF, c);
     HeaderLength += c;
     length -= c;
     PesDone += c;
     data += c;
     if (!length) {
        Offset = 188;
        return true;
        }

     start = HeaderLength;
     Header[HeaderLength++] = *data++;
     length--;
     PesDone++;
     }

  if ((Header[start] & 0xC0) == 0x40) {
     start += 2;
     if (start >= HeaderLength) {
        int c = min (length, HeaderLength - start + 1);
        memcpy(Header + HeaderLength, data, c);
        data += c;
        HeaderLength += c;
        length -= c;
        PesDone += c;
        if (start >= HeaderLength) {
           Offset = 188;
           return true;
           }
        }
     }

  if ((Header[start] & 0xF0) == 0x20) {
     start += 5;
     }
  else if ((Header[start] & 0xF0) == 0x30) {
     start += 10;
     }
  else {
     start++;
     }

  int c = min(length, HeaderLength - start);
  HeaderLength += c;
  PesDone += c;
  length -= c;
  Offset = 188 - length;
  HeaderCompleted = HeaderLength == start;
  return true;
}

uchar *cCridPlayer::GetIFrameElementary(cFrameM7x0 *frame,
             const int VideoPid, int &Offset, int &Length)
{
  uchar *frameBuffer = MALLOC(uchar, frame->Count1() + frame->Count2());
  int frameBufferLength = 0;
  uchar *data = frame->Data1();
  uchar *limit = data + frame->Count1();

  uchar tsTmp[188];
  int tsTmpLength = 0;

  uchar pesHeader[255 + 9];
  int pesHeaderLength = 0;
  int pesDone = 0;
  bool pesHeaderCompleted = false;

  uchar *currentPacket = NULL;
  int cCounter = -1;

  while (data < limit) {
        currentPacket = NULL;

        if (tsTmpLength > 0) {
           int c = min (188 - tsTmpLength, limit - data);
           memcpy(tsTmp + tsTmpLength, data, c);
           tsTmpLength += c;
           data += c;

           if (tsTmpLength == 188)
               currentPacket = tsTmp;
           }
       else if (SyncTs((const uchar*&)data, (const uchar*)limit)) {
           int c = limit - data;
           if (c < 188) {
              memcpy(tsTmp, data, c);
              tsTmpLength = c;
              data += c;
              }
           else {
              currentPacket = data;
              data += 188;
              }
           }

        if (data == limit && frame->Count2() &&
                             limit - frame->Count1() == frame->Data1()) {
           data = frame->Data2();
           limit = data + frame->Count2();
           }

        if (!currentPacket)
           continue;

        if ((((currentPacket[1] << 8) | currentPacket[2]) & 0x1FFF) !=
                                                            VideoPid) {
           tsTmpLength = 0;
           continue;
           }

        bool payloadStart;
        int payloadOffset;
        if (!CheckTsHeader(currentPacket,cCounter, payloadStart,
                                                payloadOffset)) {
           dsyslog("Broken ts packet in I-Frame");
           frameBufferLength = 0;
           break;
           }

        if (payloadOffset >= 188) {
           tsTmpLength = 0;
           continue;
           }

        if (!CheckPesHeader(currentPacket,payloadOffset, pesHeader,
                            pesHeaderLength, pesDone, pesHeaderCompleted,
                            payloadStart)) {
           dsyslog("Broken pes packet in I-Frame");
           frameBufferLength = 0;
           break;
           }

        if (payloadOffset >= 188) {
           tsTmpLength = 0;
           continue;
           }

        memcpy(frameBuffer + frameBufferLength, currentPacket +
                            payloadOffset, 188 - payloadOffset);
        frameBufferLength += 188 - payloadOffset;
        tsTmpLength = 0;
        }

  if (frameBufferLength <= 4) {
     free(frameBuffer);
     esyslog("No I-Frame found!");
     return NULL;
     }

  const uchar *frameData = frameBuffer + 2;
  const uchar *frameStart = NULL;
  const uchar *frameLimit = frameBuffer + frameBufferLength - 1;
  int step = 0;
  while (frameData < frameLimit) {
        if (frameData[0] > 1)
           frameData += 3;
        else if (!frameData[0])
           frameData++;
        else {
           if (!(frameData[-2] | frameData[-1])) {
              const uchar code = *++frameData;
              if (code == 0 || code == 0xB3 || code == 0xB8) {
                 if (!frameStart) {
                    if (code == 0xB3)
                       frameStart = frameData - 3;
                    }
                 else {
                    if (step)
                       frameLimit = frameData - 3;
                    else if (!code)
                       step = 1;
                    }
                 }
              }
           frameData += 3;
           }
        }

  if (!frameStart || !step) {
     free(frameBuffer);
     esyslog("No I-Frame found!");
     return NULL;
     }

  Offset = frameStart - frameBuffer;
  Length = frameLimit - frameBuffer;
  return frameBuffer;
}

void cCridPlayer::Goto(int Index, bool Still)
{
  if (index->IndexOkay()) {
     //LOCK_THREAD;
     FasterLockOther();
     Empty();
     readIndex = writeIndex = Index;
     if (Still) {
        playMode = pmStill;
        int FileNo;
        int IFrameOffset;
        int IFrameSize;
        dsyslog("Still image at Index %d ", Index);
        Index = index->GetIFrameIndex(Index,FileNo,IFrameOffset,IFrameSize);
        dsyslog("Still image at Index %d FileNo %d Offset %d Size %d", Index, FileNo,IFrameOffset,IFrameSize);
        readIndex = writeIndex = Index;
        replayFile = index->GetMpegFile(FileNo);
        int r = 0;
        for (;;) {
            r = ringBuffer->Read(replayFile,FileNo,IFrameOffset, IFrameSize, Index);
            if (r < 0 && FATALERRNO) {
               LOG_ERROR;
               break;
               }
            if (r >= 0 )
               break;
            }
        cFrameM7x0 *frame = ringBuffer->Get();
        if (r <= 0 || !frame) {
           FasterUnlockOther();
           return;
           }

        if (frame->Index() != Index)
           dsyslog("Indexes mismatch %d %d", frame->Index(), Index);

        index->GetIndex(readIndex, fileNr, fileOffset, readGopLength);
        if (fileOffset != IFrameOffset) {
           readGopLength -= IFrameOffset;
           fileOffset = IFrameOffset;
           }
        fileOffset += r;
        readGopLength -= r;
        lastReadLength = min(CRID_MAXGOPSIZE, readGopLength);


        int VideoPid = (FileNo == writeFileNr ? DeviceGetVideoPid() : 0 );
        if (VideoPid <= 0) {
           int t;
           index->GetPids(FileNo, t, VideoPid);
           }

        int frameOffset;
        int frameLength;
        uchar *buf = GetIFrameElementary(frame, VideoPid, frameOffset,
                                                         frameLength);
        if (buf) {
           DeviceStillPicture(buf + frameOffset, frameLength);
           free(buf);
           }
        }

     FasterUnlockOther();
     }
}

void cCridPlayer::GotoTime(int32_t Pos, bool Still)
{
  if (index->IndexOkay()) {
     int idx = 0;
     index->GetIndexTimeAbs(Pos,idx);
     Goto(idx,Still);
     }
}
bool cCridPlayer::GetIndex(int &CurrentIndex, int &TotalIndex, int &CurrentTime, int &TotalTime, bool SnapToIFrame)
{
  if (index->IndexOkay()) {
     int CurIdx;
     if (playMode == pmStill)
        CurIdx = readIndex;
     else {
        CurIdx = writeIndex;
        if (playMode == pmPlay) {
           int backed;
           if (backTrace->Get(playDir == pdForward, backed))
              CurIdx = backed;
           }
        }
     CurrentIndex = index->GetRealIndex(CurIdx);
     const int TotalIdx = index->Last();
     TotalIndex = index->GetRealIndex(TotalIdx);
     CurrentTime = index->GetTime(CurIdx);
     TotalTime = index->GetTime(TotalIdx);
     return true;
     }
  CurrentIndex = TotalIndex = -1;
  return false;
}

bool cCridPlayer::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play = (playMode == pmPlay || playMode == pmFast);
  Forward = (playDir == pdForward);
  if (playMode == pmFast || playMode == pmSlow)
     Speed = Setup.MultiSpeedMode ? abs(trickSpeed - NORMAL_SPEED) : 0;
  else
     Speed = -1;
  return true;
}

// --- cCridPlayerControl -----------------------------------------------------

cCridPlayerControl::cCridPlayerControl(const cCridRecording *Rec)
:cControl(player = new cCridPlayer(Rec))
{
}

cCridPlayerControl::~cCridPlayerControl()
{
  Stop();
}

bool cCridPlayerControl::Active(void)
{
  return player && player->Active();
}

void cCridPlayerControl::Stop(void)
{
  delete player;
  player = NULL;
}

void cCridPlayerControl::Pause(void)
{
  if (player)
     player->Pause();
}

void cCridPlayerControl::Play(void)
{
  if (player)
     player->Play();
}

void cCridPlayerControl::Forward(void)
{
  if (player)
     player->Forward();
}

void cCridPlayerControl::Backward(void)
{
  if (player)
     player->Backward();
}

void cCridPlayerControl::SkipSeconds(int Seconds)
{
  if (player)
     player->SkipSeconds(Seconds);
}

bool cCridPlayerControl::GetIndex(int &CurrentIndex, int &TotalIndex, int &CurrentTime, int &TotalTime, bool SnapToIFrame)
{
  if (player) {
     player->GetIndex(CurrentIndex, TotalIndex, CurrentTime, TotalTime, SnapToIFrame);
     return true;
     }
  return false;
}

bool cCridPlayerControl::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  return player && player->GetReplayMode(Play, Forward, Speed);
}

void cCridPlayerControl::Goto(int Position, bool Still)
{
  if (player)
     player->Goto(Position, Still);
}

void cCridPlayerControl::GotoTime(int32_t Pos, bool Still)
{
  if (player)
     player->GotoTime(Pos, Still);
}
cCridRecording *cCridPlayerControl::Recording(void)
{
  if (player)
     return player->Recording();
  return NULL;
}

cCridMarks *cCridPlayerControl::GetMarks(void)
{
  if (player)
     return player->GetMarks();

  return NULL;
}

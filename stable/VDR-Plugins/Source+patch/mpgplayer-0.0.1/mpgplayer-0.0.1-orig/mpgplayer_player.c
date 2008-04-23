/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#include "mpgplayer_player.h"
#include "mpgplayer_tools.h"
#include <stdlib.h>
#include <vdr/recording.h>
#include <vdr/remux.h>
#include <vdr/ringbuffer.h>
#include <vdr/thread.h>
#include <vdr/tools.h>

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


// --- cMpgFileName -------------------------------------------------------------

#include <errno.h>
#include <unistd.h>
#include <vdr/videodir.h>

#define MAXFILESPERRECORDING 255
#define RECORDFILESUFFIX    "/%03d.vdr"
#define RECORDFILESUFFIXLEN 20 // some additional bytes for safety...

//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
cMpgFileName::cMpgFileName(const char *FileName, bool Record, bool Blocking, bool Direct)
#else
cMpgFileName::cMpgFileName(const char *FileName, bool Record, bool Blocking)
#endif
{
  file = NULL;
  fileNumber = 0;
  record = Record;
  blocking = Blocking;
#ifdef USE_DIRECT_IO
  direct = Direct;
#endif
  // Prepare the file name:
  fileName=strdup(FileName);
  SetOffset(1);
}

cMpgFileName::~cMpgFileName()
{
  Close();
  free(fileName);
}


cMpgUnbufferedFile *cMpgFileName::Open(void)
{
  if (!file) {

#ifdef USE_DIRECT_IO
     int BlockingFlag = direct ? O_DIRECT : (blocking ? 0 : O_NONBLOCK);
#else
     int BlockingFlag = blocking ? 0 : O_NONBLOCK;
#endif
//M7X0 END AK
     if (record) {
        dsyslog("recording to '%s'", fileName);
//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
 //       file = OpenVideoFile(fileName, O_WRONLY | O_CREAT | BlockingFlag);
#else
 //       file = OpenVideoFile(fileName, O_RDWR | O_CREAT | BlockingFlag);
#endif
//M7X0 END AK
        if (!file)
           LOG_ERROR_STR(fileName);
        }
     else {
        if (access(fileName, R_OK) == 0) 
        {
           // dsyslog("playing '%s'", fileName);
           file = cMpgUnbufferedFile::Create(fileName, O_RDONLY | BlockingFlag | O_LARGEFILE);
           if (!file)
           {  LOG_ERROR_STR(fileName);
              dsyslog("cannot play '%s'", fileName);
           }
        }
        else if (errno != ENOENT)
        { 
           LOG_ERROR_STR(fileName);
              dsyslog("cannot play 2 '%s'", fileName);
        }
        }
     }
  return file;
}

void cMpgFileName::Close(void)
{
  if (file) {
     if (CloseMpgVideoFile(file) < 0)
        LOG_ERROR_STR(fileName);
     file = NULL;
     }
}

//M7X0 BEGIN AK
int cMpgFileName::Unlink(void)
{
  Close();
  return unlink(fileName);
}
//M7X0 END AK
cMpgUnbufferedFile *cMpgFileName::SetOffset(int Number, off64_t Offset)
{

  Close();
     if (Open() >= 0) {
        if (Offset >= 0 && file && file->Seek(Offset, SEEK_SET) != Offset) {
           LOG_ERROR_STR(fileName);
           return NULL;
           }
        }
     return file;
}

cMpgUnbufferedFile *cMpgFileName::NextFile(void)
{
  return SetOffset(fileNumber + 1);
}





// --- cMpgNonBlockingFileReader ------------------------------------------------
//M7X0 BEGIN AK

cMpgNonBlockingFileReader::cMpgNonBlockingFileReader(cMpgRingBufferFrameM7x0 *RingBuffer)
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

cMpgNonBlockingFileReader::~cMpgNonBlockingFileReader()
{
  //newSet.Signal();
  Cancel(3);
}

#define LOCKWAITTIME 10 // ms
void cMpgNonBlockingFileReader::FasterLockAction(void)
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

void cMpgNonBlockingFileReader::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cMpgNonBlockingFileReader::FasterLockOther(void)
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

void cMpgNonBlockingFileReader::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cMpgNonBlockingFileReader::Clear(void)
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

int cMpgNonBlockingFileReader::Read(cMpgFileName *File, int FileNr, off64_t Offset, int Length, int Index)
{
  if (hasData && file) {
     if (fileNr != FileNr || offset != Offset || wanted != Length) {
        esyslog("ERROR: cMpgNonBlockingFileReader::Read() called with different file offset!");
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

void cMpgNonBlockingFileReader::Action(void)
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

bool cMpgNonBlockingFileReader::WaitForDataMs(int msToWait)
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

// --- cMpgPlayer ------------------------------------------------------------

#define PLAYERBUFSIZE  MEGABYTE(1)
#define PLAYERBUFALIGNMENT KILOBYTE(64)
#define SLEEPWAITTIME 3  // ms
// The number of frames to back up when resuming an interrupted replay session:
#define RESUMEBACKUP (10 * FRAMESPERSEC)

class cMpgPlayer : public cPlayer, cThread {
private:
  enum ePlayModes { pmPlay, pmPause, pmSlow, pmFast, pmStill };
  enum ePlayDirs { pdForward, pdBackward };
  //static int Speeds[];
  cMpgNonBlockingFileReader *nonBlockingFileReader;
  cMpgRingBufferFrameM7x0 *ringBuffer;
  cBackTrace *backTrace;
  cMpgFileName *fileName;
  cMpgRecording *rec;
  cMpgIndexFile *index;
  cMpgUnbufferedFile *replayFile;
  bool eof;
  bool firstPacket;
  ePlayModes playMode;
  ePlayDirs playDir;
  int trickSpeed;

  int readIndex, writeIndex;
  uchar fileNr;
  off64_t fileOffset;
  int readFrameLength;
  cMpgFrameM7x0 *playFrame;

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
  cMpgPlayer(const cMpgRecording *Rec);
  virtual ~cMpgPlayer();
  bool Active(void) { return cThread::Running(); }
  void Pause(void);
  void Play(void);
  void Forward(void);
  void Backward(void);
  int SkipFrames(int Frames);
  void SkipSeconds(int Seconds);
  void Goto(int Position, bool Still = false);
  virtual bool GetIndex(int &Current, int &Total, int &CurrentTime, int &TotalTime );
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
  cMpgRecording *Recording(void) { return rec;}
  };


#define NORMAL_SPEED  0
#define MAX_SPEEDS    3
#define LOW_SPEEDS    0
cMpgPlayer::cMpgPlayer(const cMpgRecording *Rec)
:cThread("mpgplayer")
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

  char *filename = MALLOC(char, strlen(Rec->GetPath())+strlen(Rec->GetFilename())+1);
  if (!filename)
  {  esyslog("ERROR: can't copy file name '%s%s'", Rec->GetPath(), Rec->GetFilename() );
     return;
  }
  strcpy(filename,Rec->GetPath());
  strcpy(filename+strlen(Rec->GetPath())+1,Rec->GetFilename());
  *(filename+strlen(Rec->GetPath()))='/';  
  isyslog("replay %s", filename);
//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
  fileName = new cMpgFileName(filename, false, true, true);
#else
  fileName = new cMpgFileName(filename, false);
#endif 
  free(filename);
//M7X0 END AK
  replayFile = fileName->Open();
  if (!replayFile)
     return;
  fileNr = fileName->Number();

  ringBuffer = new cMpgRingBufferFrameM7x0(PLAYERBUFSIZE, PLAYERBUFALIGNMENT);
  ringBuffer->SetTimeouts(40,0);
  // Create the index file:
  // index = new cMpgIndexFile(fileName->Name(), false);
  index=NULL;
  // if (!index)
  //   esyslog("ERROR: can't allocate index");
  // else if (!index->Ok()) {
  //   delete index;
  //   index = NULL;
  //    }
  backTrace = new cBackTrace;
}

cMpgPlayer::~cMpgPlayer()
{
  Detach();
  Save();
  delete index;
  delete fileName;
  delete backTrace;
  delete ringBuffer;
}

void cMpgPlayer::FasterLockAction(void)
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

void cMpgPlayer::FasterUnlockAction(void)
{
  if (actionLock == 3)
     Unlock();
  actionLock = 0;
}

void cMpgPlayer::FasterLockOther(void)
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

void cMpgPlayer::FasterUnlockOther(void)
{
  if (otherLock == 3)
     Unlock();
  otherLock = 0;
}

void cMpgPlayer::TrickSpeed(int Increment, bool lock)
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

void cMpgPlayer::Empty(void)
{
  //LOCK_THREAD;
  if (nonBlockingFileReader)
     nonBlockingFileReader->Clear();
  if (playMode == pmFast || playMode == pmSlow || (readIndex = backTrace->Get(playDir == pdForward)) < 0)
     readIndex = writeIndex;
  writeIndex = readIndex;
  if (index && ((readIndex = index->GetNextIFrame(readIndex + (playDir == pdBackward ? -1 : 1) ,playDir == pdBackward) - 1) < 0 ))
      readIndex = writeIndex;
  writeIndex = readIndex;
  playFrame = NULL;
  ringBuffer->Clear();
  backTrace->Clear();
  DeviceClear();
  firstPacket = true;
}

bool cMpgPlayer::NextFile(void)
{
  if (replayFile && eof) {
     replayFile = fileName->NextFile();
     fileNr = fileName->Number();
     fileOffset = 0;
     }
  eof = false;
  return replayFile != NULL;
}

int cMpgPlayer::Resume(void)
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

bool cMpgPlayer::Save(void)
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

void cMpgPlayer::Activate(bool On)
{
  if (On) {
     actionLock = 0;
     otherLock = 0;
     if (replayFile)
         Start(); // Action(); Start();
     }
  else
     Cancel(9);
}

void cMpgPlayer::Action(void)
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


  nonBlockingFileReader = new cMpgNonBlockingFileReader(ringBuffer);

  bool Sleep = false;
  bool WaitingForData = false;

  cPoller Poller;

  while (Running() && (NextFile() || readIndex >= 0 || ringBuffer->Available() || !DeviceFlush(100))) {

        // dsyslog("PlayerAction - while R%d N%d A%d", Running(), NextFile(), ringBuffer->Available() );

        if (Sleep) {
           // dsyslog("PlayerAction1: Sleep true %d", WaitingForData );
           if (WaitingForData)
               nonBlockingFileReader->WaitForDataMs(SLEEPWAITTIME); // this keeps the CPU load low, but reacts immediately on new data
           else
              cCondWait::SleepMs(SLEEPWAITTIME); // this keeps the CPU load low
           Sleep = false;
           WaitingForData = false;
           }

        if (DevicePoll(Poller, 100)) {

           // dsyslog("PlayerAction1: DevicePoll==true playMode=%d", playMode );

           //LOCK_THREAD;
           FasterLockAction();

           if (playMode == pmStill || playMode == pmPause) {
              Sleep = true;
              FasterUnlockAction();
              continue;
              }

           if (replayFile || readIndex >= 0) 
           {
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
              {     eof = true;
                    // dsyslog("PlayerAction1 - eof");

              }
              //else 
              //    dsyslog("PlayerAction1 - Reading: Offset: 0x%lX 0x%X 0x%X ", fileOffset, readFrameLength, r); 

           }


           if (!playFrame) 
           {  //dsyslog("PlayerAction2: getPlayFrame" );
              playFrame = ringBuffer->Get();
              data1 = NULL;
           }

           if (playFrame)
           {  //dsyslog("PlayerAction2: playPlayFrame %d %d %d %d", data1, count1, data2, count2 );
              if (!data1) 
              {  data1 = playFrame->Data1();
                 count1 = playFrame->Count1();
                 data2 = playFrame->Data2();
                 count2 = playFrame->Count2();
                 if (firstPacket) 
                 {  PlayPes(NULL, 0);
                    cRemux::SetBrokenLink(data1, count1);
                    firstPacket = false;
                 }
              }
              int playedBytes = PlayPes(data1, count1, playMode != pmPlay);
              if (playedBytes < 0)
              {  if (FATALERRNO)
                 {  LOG_ERROR;
                    FasterUnlockAction();
                    break;
                 }
                 FasterUnlockAction();
                 continue;
              }

              data1 += playedBytes;
              count1 -= playedBytes;
              if (!count1 && count2) 
              {  data1 = data2;
                 count1 = count2;
                 count2 = 0;

                 playedBytes = PlayPes(data1, count1, playMode != pmPlay);
                 if (playedBytes < 0)
                 {  if (FATALERRNO)
                    {  LOG_ERROR;
                       FasterUnlockAction();
                       break;
                    }
                    FasterUnlockAction();
                    continue;
                 }
                 data1 += playedBytes;
                 count1 -= playedBytes;
              }

              if (!count1) 
              {  writeIndex = playFrame->Index();
                 backTrace->Add(playFrame->Index(), playFrame->Count1() + playFrame->Count2());
                 ringBuffer->Drop(playFrame);
                 playFrame = NULL;
              }
           }  else
           {  //dsyslog("PlayerAction2: no PlayFrame" );
              Sleep = true;
           }
           FasterUnlockAction();
           }
           //else
           // dsyslog("PlayerAction: DevicePoll==false");

        }

  cMpgNonBlockingFileReader *nbfr = nonBlockingFileReader;
  nonBlockingFileReader = NULL;
  delete nbfr;
}

void cMpgPlayer::Pause(void)
{
  if (playMode == pmPause || playMode == pmStill)
     Play();
  else 
  {  //LOCK_THREAD;
     FasterLockOther();
     // if (playMode == pmFast || (playMode == pmSlow && playDir == pdBackward))
     Empty();
     DeviceFreeze();
     playMode = pmPause;
     FasterUnlockOther();
  }
}

void cMpgPlayer::Play(void)
{
  if (playMode == pmPlay)
     Pause();
  else {
     //LOCK_THREAD;
     FasterLockOther();
     if (playMode == pmFast || playMode == pmSlow)
        Empty();

     DevicePlay();
     playMode = pmPlay;
     playDir = pdForward;
     FasterUnlockOther();
     }
}

void cMpgPlayer::Forward(void)
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

void cMpgPlayer::Backward(void)
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

int cMpgPlayer::SkipFrames(int Frames)
{
  if (index && Frames) {
     int Current;
     // int Total;
     // GetIndex(Current, Total, true);
     int OldCurrent = Current;
     // As GetNextIFrame() increments/decrements at least once, the
     // destination frame (= Current + Frames) must be adjusted by
     // -1/+1 respectively.
     Current = index->GetNextIFrame(Current + Frames + (Frames > 0 ? -1 : 1), Frames > 0);
     return Current >= 0 ? Current : OldCurrent;
     }
  return -1;
}

void cMpgPlayer::SkipSeconds(int Seconds)
{
     FasterLockOther();
     Empty();
     fileOffset += MAXFRAMESIZE*Seconds;
     // dsyslog("SkipSeconds: Offset: 0x%lX %d", fileOffset, Seconds );
     if(fileOffset<0)
        fileOffset=0;
     FasterUnlockOther();
     // dsyslog("SkipSeconds 2: Offset: 0x%X", fileOffset );
  if (index && Seconds) {
     //LOCK_THREAD;
     if (playMode != pmPlay)
        Play();
     FasterLockOther();
     Empty();
     //index->GetIndexTimeRel(Seconds, writeIndex);
     readIndex = writeIndex;
     FasterUnlockOther();
     }
}

void cMpgPlayer::Goto(int Index, bool Still)
{
  if (index) {
     //LOCK_THREAD;
     FasterLockOther();
     Empty();
     if (++Index <= 0)
        Index = 1; // not '0', to allow GetNextIFrame() below to work!
     uchar FileNumber;
     int Length;
     off64_t FileOffset;
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
        cMpgFrameM7x0 *frame = ringBuffer->Get();
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

bool cMpgPlayer::GetIndex(int &Current, int &Total, int &CurrentTime, int &TotalTime)
{
  Current = fileOffset/MAXFRAMESIZE;
  struct stat64 st64;
  if (!FileStat64( fileName->Name(), &st64) )
  {  dsyslog("Cannot stat64 %s ... ", fileName->Name());
     Total=Current;
  }
  else
    Total=st64.st_size/MAXFRAMESIZE;
  //dsyslog("GetIndex() Current/Total: %d %d %s", Current, Total, fileName->Name() );
/****
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
//        if (SnapToIFrame)
          {
           int i1 = index->GetNextIFrame(Current + 1, false);
           int i2 = index->GetNextIFrame(Current, true);
           Current = (abs(Current - i1) <= abs(Current - i2)) ? i1 : i2;
           }
        }
     Total = index->Last();
     return true;
     }
**/
  return false;
}

bool cMpgPlayer::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play = (playMode == pmPlay || playMode == pmFast);
  Forward = (playDir == pdForward);
  if (playMode == pmFast || playMode == pmSlow)
     Speed = Setup.MultiSpeedMode ? abs(trickSpeed - NORMAL_SPEED) : 0;
  else
     Speed = -1;
  return true;
}

// --- cMpgPlayerControl -----------------------------------------------------

cMpgPlayerControl::cMpgPlayerControl(const cMpgRecording *Rec)
:cControl(player = new cMpgPlayer(Rec))
{
}

cMpgPlayerControl::~cMpgPlayerControl()
{
  Stop();
}

bool cMpgPlayerControl::Active(void)
{
  return player && player->Active();
}

void cMpgPlayerControl::Stop(void)
{
  delete player;
  player = NULL;
}

void cMpgPlayerControl::Pause(void)
{
  if (player)
     player->Pause();
}

void cMpgPlayerControl::Play(void)
{
  if (player)
     player->Play();
}

cMpgRecording *cMpgPlayerControl::Recording(void)
{
  if (player)
     return player->Recording();
  return NULL;
}

void cMpgPlayerControl::Forward(void)
{
  if (player)
     player->Forward();
}

void cMpgPlayerControl::Backward(void)
{
  if (player)
     player->Backward();
}

void cMpgPlayerControl::SkipSeconds(int Seconds)
{
  if (player)
     player->SkipSeconds(Seconds);
}

int cMpgPlayerControl::SkipFrames(int Frames)
{
  if (player)
     return player->SkipFrames(Frames);
  return -1;
}

bool cMpgPlayerControl::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  if (player) {
//     player->GetIndex(Current, Total, SnapToIFrame);
     return true;
     }
  return false;
}

bool cMpgPlayerControl::GetIndex(int &Current, int &Total, int &CurrentTime, int &TotalTime)
{
  if (player) {
     player->GetIndex(Current, Total, CurrentTime, TotalTime);
     return true;
     }
  return false;
}

bool cMpgPlayerControl::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  return player && player->GetReplayMode(Play, Forward, Speed);
}

void cMpgPlayerControl::Goto(int Position, bool Still)
{
  if (player)
     player->Goto(Position, Still);
}



//M7X0 BEGIN AK
// --- cMpgRingBufferFrameM7x0 ------------------------------------------------------
cMpgRingBufferFrameM7x0::cMpgRingBufferFrameM7x0(int Size, int Alignment, bool Statistics, const char *Description)
:cRingBuffer((Size + 2 * Alignment - 1) & ~(Alignment - 1), Statistics,Description)
{
  alignment = Alignment;
  void *tmp;
  if (posix_memalign(&tmp, Alignment, (Size + 2 * Alignment - 1) & ~(Alignment - 1)))
     esyslog("ERROR: can't allocate ring buffer (size=%d)", Size);

  buffer = (uchar *) tmp;

  head = 0;
  frameHead = 0;
  Clear();
}

cMpgRingBufferFrameM7x0::~cMpgRingBufferFrameM7x0()
{
  free(buffer);
}

int cMpgRingBufferFrameM7x0::ReadReal(cMpgUnbufferedFile *file, uchar *buf, int count)
{
  int retryCount = 0;
  int bytesRead = 0;
  while (bytesRead != count) {
        int r = file->Read(buf + bytesRead, count - bytesRead);

        if (r > 0) {
           retryCount = 0;
           bytesRead += r;
           continue;
        }

        if (!r)
           return bytesRead;

        if (FATALERRNO) {
           if (retryCount < 20 && errno == EIO) {
              esyslog("ERROR: Got an IO Error -- Retrying (%d)", retryCount);
              retryCount++;
              continue;
              }
           LOG_ERROR;
           return r;
           }

        }

  return bytesRead;
}


int cMpgRingBufferFrameM7x0::Read(cMpgFileName *fileName, int FileNr, off64_t Offset, int Size, int Index)
{
  cMpgUnbufferedFile *file = fileName->SetOffset(FileNr, -1);
  return (file ? Read(file, FileNr, Offset, Size, Index) : 0);
}

int cMpgRingBufferFrameM7x0::Read(cMpgUnbufferedFile *file, int FileNr, off64_t Offset, int FrameSize, int Index)
{

  off64_t ReadAHeadCount = readAHeadCount;
  int CurFileNr = curFileNr;
  off64_t CurFileOffset = curFileOffset;
  off64_t readDirectOffset;
  off64_t readFrameOffset;

  if (CurFileNr == FileNr && CurFileOffset == Offset) {
     readDirectOffset = (Offset + (alignment -1)) & ~(alignment -1);
     readFrameOffset = Offset - readDirectOffset;

     if (-readFrameOffset > ReadAHeadCount && FrameSize > ReadAHeadCount)
        FrameSize = ReadAHeadCount;

     }
  else {
     readDirectOffset = Offset & ~(alignment - 1);
     readFrameOffset = Offset & (alignment - 1);
     ReadAHeadCount = -readFrameOffset;
     }

  int readSize = (FrameSize + readFrameOffset + (alignment - 1)) & ~(alignment - 1);

  const int size = Size();
  int Head = head;

  int Tail = tail;
  int diff = Tail - Head;
  int free = (diff > 0 ? diff : size + diff) - 1;

  int FrameHead = frameHead;
  int FrameTail = frameTail;
  int framesDiff = FrameTail - FrameHead;
  int framesFree = (framesDiff > 0 ? framesDiff : MAX_BUFFERED_FRAMES + 1 + framesDiff) - 1;

  if (free < readSize || !framesFree) {
     EnableGet();
     WaitForPut();

     Tail = tail;
     diff = Tail - Head;
     free = (diff > 0 ? diff : size + diff) - 1;

     FrameTail = frameTail;
     framesDiff = FrameTail - FrameHead;
     framesFree = (framesDiff > 0 ? framesDiff : MAX_BUFFERED_FRAMES + 1 + framesDiff) - 1;

     if (free < readSize || !framesFree) {
        errno = EAGAIN;
        return -1;
        }
     }


  int oldHead = Head;
  int bytesRead = 0;

  if (readSize) {
     // dsyslog("RingBuffer: SeekOffset: 0x%lX", readDirectOffset); 
     if (file->Seek(readDirectOffset, SEEK_SET) != readDirectOffset) {
        LOG_ERROR;
        return 0;
        }

     int rest = size - Head;
     if (readSize >= rest) {
        int r = ReadReal(file, buffer + Head, rest);

        if (r < 0)
           return -1;

        readSize -= rest;
        bytesRead = r;
        Head = readSize;

        if (readSize && r == rest) {
           r = ReadReal(file, buffer, readSize);

           if (r < 0)
              return -1;
           bytesRead += r;
           }
        }
     else {
        int r = ReadReal(file, buffer + Head, readSize);

        if (r < 0)
           return -1;

        bytesRead = r;
        Head += readSize;
        }
     }


  int frameSizeRead = bytesRead + ReadAHeadCount;

  ReadAHeadCount = 0;

  if (frameSizeRead > FrameSize) {
     ReadAHeadCount = frameSizeRead - FrameSize;
     frameSizeRead = FrameSize;
     }
  else if (frameSizeRead <= 0)
     return 0;


  CurFileNr = FileNr;
  CurFileOffset = readDirectOffset + frameSizeRead + readFrameOffset;

  int data1 = oldHead + readFrameOffset;
  if (data1 < 0)
     data1 += size;

  int count1 = frameSizeRead;
  int count2 = 0;

  if (data1 + count1 > size) {
     count2 = data1 + count1 - size;
     count1 -= count2;
     }

  frameBuffer[FrameHead].data1 = buffer + data1;
  frameBuffer[FrameHead].data2 = buffer;
  frameBuffer[FrameHead].count1 = count1;
  frameBuffer[FrameHead].count2 = count2;
  frameBuffer[FrameHead].index = Index;
  frameBuffer[FrameHead].fileNo = FileNr;

  FrameHead++;

  if (FrameHead == MAX_BUFFERED_FRAMES + 1)
      FrameHead = 0;

  frameHead = FrameHead;
  head = Head;
  curFileNr = CurFileNr;
  curFileOffset = CurFileOffset;
  readAHeadCount = ReadAHeadCount;
  EnableGet();
  return frameSizeRead;
}


cMpgFrameM7x0 *cMpgRingBufferFrameM7x0::Get(void)
{
  int FrameHead = frameHead;
  const int FrameTail = frameTail;
  if (FrameHead == FrameTail) {
     EnablePut();
     WaitForGet();
     FrameHead = frameHead;
     if (FrameHead == FrameTail)
        return NULL;
     }

  return &frameBuffer[FrameTail];
}

void cMpgRingBufferFrameM7x0::Drop(cMpgFrameM7x0 *frame)
{
  const int FrameHead = frameHead;
  int FrameTail = frameTail;

  if (FrameHead == FrameTail){
     esyslog("ERROR: cMpgRingBufferFrameM7x0::Drop() no frame in Buffer!");
     return;
     }

  if (frame != NULL && &frameBuffer[FrameTail] != frame) {
     esyslog("ERROR: cMpgRingBufferFrameM7x0::Drop() attempt to drop wrong frame from ring buffer!");
     return;
     }

  int Tail = frameBuffer[FrameTail].data1 - buffer + frameBuffer[FrameTail].count1;

  if (Tail >= Size())
     Tail = frameBuffer[FrameTail].count2;

  FrameTail++;
  if (FrameTail == MAX_BUFFERED_FRAMES + 1)
     FrameTail = 0;

  tail = Tail;
  frameTail = FrameTail;
  EnablePut();
}


// --- cMpgIndexFile ------------------------------------------------------------

#define INDEXFILESUFFIX     ".idx"

// The number of frames to stay off the end in case of time shift:
#define INDEXSAFETYLIMIT 150 // frames

// The maximum time to wait before giving up while catching up on an index file:
#define MAXINDEXCATCHUP   8 // seconds

// The minimum age of an index file for considering it no longer to be written:
#define MININDEXAGE    3600 // seconds

cMpgIndexFile::cMpgIndexFile(const char *FileName, bool Record)
:resumeFile(FileName)
{
  f = -1;
  fileName = NULL;
  size = 0;
  last = -1;
  index = NULL;
  if (FileName) {
     fileName = MALLOC(char, strlen(FileName) + strlen(INDEXFILESUFFIX) );
     if (fileName) 
     {  strcpy(fileName, FileName);
        char *pFileExt = fileName + strlen(fileName);
        strcpy(pFileExt, INDEXFILESUFFIX);
        int delta = 0;
        if (access(fileName, R_OK) == 0) {
           struct stat buf;
           if (stat(fileName, &buf) == 0) {
              delta = buf.st_size % sizeof(tIndex);
              if (delta) {
                 delta = sizeof(tIndex) - delta;
                 esyslog("ERROR: invalid file size (%ld) in '%s'", buf.st_size, fileName);
                 }
              last = (buf.st_size + delta) / sizeof(tIndex) - 1;
              if (!Record && last >= 0) {
                 size = last + 1;
                 index = MALLOC(tIndex, size);
                 if (index) {
                    f = open(fileName, O_RDONLY);
                    if (f >= 0) {
                       if ((int)safe_read(f, index, buf.st_size) != buf.st_size) {
                          esyslog("ERROR: can't read from file '%s'", fileName);
                          free(index);
                          index = NULL;
                          close(f);
                          f = -1;
                          }
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
                       else {
                          for (int j=0; j <= last; j++)
                              index[j].offset = bswap_32(index[j].offset);
                          }
#endif
//M7X0 END AK
                       // we don't close f here, see CatchUp()!
                       }
                    else
                       LOG_ERROR_STR(fileName);

                    }
                 else
                    esyslog("ERROR: can't allocate %zd bytes for index '%s'", size * sizeof(tIndex), fileName);
                 }
              }
           else
              LOG_ERROR;
           }
        // else if (!Record)
        //   isyslog("missing index file %s", fileName);
        if (Record) {
           if ((f = open(fileName, O_RDWR | O_CREAT | O_APPEND, DEFFILEMODE)) >= 0) {
//M7X0 END AK
              if (delta) {
                 esyslog("ERROR: padding index file with %d '0' bytes", delta);
                 while (delta--)
                       writechar(f, 0);
                 }
              }
           else
              LOG_ERROR_STR(fileName);
           }
        }
     else
        esyslog("ERROR: can't copy file name '%s'", FileName);
     }
}

cMpgIndexFile::~cMpgIndexFile()
{
  if (f >= 0)
     close(f);
  free(fileName);
  free(index);
}

bool cMpgIndexFile::CatchUp(int Index)
{
  // returns true unless something really goes wrong, so that 'index' becomes NULL
  //dsyslog("MpgIndexFile::CatchUp1");
  if (index && f >= 0) {
     //dsyslog("MpgIndexFile::CatchUp2");
     cMutexLock MutexLock(&mutex);
     for (int i = 0; i <= MAXINDEXCATCHUP && (Index < 0 || Index >= last); i++) {
         struct stat buf;
         if (fstat(f, &buf) == 0) {
            if (time(NULL) - buf.st_mtime > MININDEXAGE) {
               // apparently the index file is not being written any more
               close(f);
               f = -1;
               break;
               }
            int newLast = buf.st_size / sizeof(tIndex) - 1;
            if (newLast > last) {
               if (size <= newLast) {
                  size *= 2;
                  if (size <= newLast)
                     size = newLast + 1;
                  }
               index = (tIndex *)realloc(index, size * sizeof(tIndex));
               if (index) {
                  int offset = (last + 1) * sizeof(tIndex);
                  int delta = (newLast - last) * sizeof(tIndex);
                  if (lseek(f, offset, SEEK_SET) == offset) {
                     if (safe_read(f, &index[last + 1], delta) != delta) {
                        esyslog("ERROR: can't read from index");
                        free(index);
                        index = NULL;
                        close(f);
                        f = -1;
                        break;
                        }
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
                     for (int j=last+1; j <= newLast; j++)
                         index[j].offset = bswap_32(index[j].offset);
#endif
//M7X0 END AK
                     last = newLast;
                     }
                  else
                     LOG_ERROR_STR(fileName);
                  }
               else
                  esyslog("ERROR: can't realloc() index");
               }
            }
         else
            LOG_ERROR_STR(fileName);
         if (Index < last - (i ? 2 * INDEXSAFETYLIMIT : 0) || Index > 10 * INDEXSAFETYLIMIT) // keep off the end in case of "Pause live video"
            break;
         cCondWait::SleepMs(1000);
         }
     }
  return index != NULL;
}

bool cMpgIndexFile::Write(uchar PictureType, uchar FileNumber, int FileOffset)
{
  if (f >= 0) {
     tIndex i = { FileOffset, PictureType, FileNumber, 0 };
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
     i.offset = bswap_32(i.offset);
#endif
//M7X0 END AK

     if (safe_write(f, &i, sizeof(i)) < 0) {
        LOG_ERROR_STR(fileName);
        close(f);
        f = -1;
        return false;
        }
     last++;
     }
  return f >= 0;
}

//M7X0 BEGIN AK
bool cMpgIndexFile::Write(sPesResult *Picture,  int PictureCount , uchar FileNumber, int FileOffset)
{
  if (f >= 0) {
     tIndex inds[PictureCount];
     int i;
     int count=0;
     for (i = 0;i < PictureCount; i++)
         if (Picture[i].pictureType) {

#if BYTE_ORDER == BIG_ENDIAN
            inds[count].offset = bswap_32(FileOffset + Picture[i].offset);
#else
            inds[count].offset = FileOffset + Picture[i].offset;
#endif

            inds[count].type = Picture[i].pictureType;
            inds[count].number = FileNumber;
            inds[count++].reserved = 0;
            }

     if (count) {
        if (safe_write(f, inds, sizeof(tIndex)*count) < 0) {
           LOG_ERROR_STR(fileName);
           close(f);
           f = -1;
          return false;
          }
        last+= count;
        }
     }
  return f >= 0;
}

int cMpgIndexFile::StripOffLast(void) {
  if (f < 0)
     return -1;

  off_t newEnd = lseek(f, - sizeof(tIndex), SEEK_END);
  if (newEnd < 0) {
     LOG_ERROR;
     return -1;
     }

  tIndex ind;
  if (safe_read(f, &ind, sizeof(tIndex)) < 0 ) {
     LOG_ERROR;
     return -1;
     }

  ftruncate(f, newEnd);
#if BYTE_ORDER == BIG_ENDIAN
  return bswap_32(ind.offset);
#else
  return ind.offset;
#endif
}
//M7X0 END AK
bool cMpgIndexFile::Get(int Index, uchar *FileNumber, off64_t *FileOffset, uchar *PictureType, int *Length)
{
  if (CatchUp(Index)) {
     if (Index >= 0 && Index < last) {
        *FileNumber = index[Index].number;
        *FileOffset = index[Index].offset;
        if (PictureType)
           *PictureType = index[Index].type;
        if (Length) {
           int fn = index[Index + 1].number;
           int fo = index[Index + 1].offset;
           if (fn == *FileNumber)
              *Length = fo - *FileOffset;
           else
              *Length = -1; // this means "everything up to EOF" (the buffer's Read function will act accordingly)
           }
        return true;
        }
     }
  return false;
}

int cMpgIndexFile::GetNextIFrame(int Index, bool Forward, uchar *FileNumber, off64_t *FileOffset, int *Length, bool StayOffEnd)
{
  //dsyslog("cMpgIndexFile::GetNextIFrame1");
  if (CatchUp()) {
     //dsyslog("cMpgIndexFile::GetNextIFrame2");
     int d = Forward ? 1 : -1;
     for (;;) {
         Index += d;
         if (Index >= 0 && Index < last - ((Forward && StayOffEnd) ? INDEXSAFETYLIMIT : 0)) {
            if (index[Index].type == I_FRAME) {
               if (FileNumber)
                  *FileNumber = index[Index].number;
               else
                  FileNumber = &index[Index].number;
               if (FileOffset)
                  *FileOffset = index[Index].offset;
               else
                ; //                  FileOffset = &index[Index].offset;
               if (Length) {
                  // all recordings end with a non-I_FRAME, so the following should be safe:
                  int fn = index[Index + 1].number;
                  int fo = index[Index + 1].offset;
                  if (fn == *FileNumber)
                     *Length = fo - *FileOffset;
                  else {
                     esyslog("ERROR: 'I' frame at end of file #%d", *FileNumber);
                     *Length = -1;
                     }
                  }
               return Index;
               }
            }
         else
            break;
         }
     }
  return -1;
}

int cMpgIndexFile::Get(uchar FileNumber, off64_t FileOffset)
{
  if (CatchUp()) {
     //TODO implement binary search!
     int i;
     for (i = 0; i < last; i++) {
         if (index[i].number > FileNumber || (index[i].number == FileNumber) && index[i].offset >= FileOffset)
            break;
         }
     return i;
     }
  return -1;
}

bool cMpgIndexFile::IsStillRecording()
{
  return f >= 0;
}

// --- cFileName -------------------------------------------------------------

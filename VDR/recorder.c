/*
 * recorder.c: The actual DVB recorder
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "recorder.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#define RECORDERBUFSIZE  MEGABYTE(4)

// The maximum time we wait before assuming that a recorded video data stream
// is broken:
#define MAXBROKENTIMEOUT 30 // seconds

#define MINFREEDISKSPACE    (512) // MB
#define DISKCHECKINTERVAL   100 // seconds

class cFileWriter : public cThread {
private:
  cRemux *remux;
  cFileName *fileName;
  cIndexFile *index;
  uchar pictureType;
  int fileSize;
  cUnbufferedFile *recordFile;
  time_t lastDiskSpaceCheck;
  bool RunningLowOnDiskSpace(void);
  bool NextFile(void);
protected:
  virtual void Action(void);
public:
  cFileWriter(const char *FileName, cRemux *Remux);
  virtual ~cFileWriter();
  };
//M7X0 BEGIN AK
cFileWriter::cFileWriter(const char *FileName, cRemux *Remux)
:cThread("file writer")
{
  fileName = NULL;
  remux = Remux;
  index = NULL;
  pictureType = NO_PICTURE;
  fileSize = 0;
  lastDiskSpaceCheck = time(NULL);
#ifdef USE_DIRECT_IO
  fileName = new cFileName(FileName, true, true, true);
#else
  fileName = new cFileName(FileName, true);
#endif
  recordFile = fileName->Open();
  if (!recordFile) {
     LOG_ERROR;
     return;
     }
  // Create the index file:
  index = new cIndexFile(FileName, true);
  if (!index)
     esyslog("ERROR: can't allocate index");
     // let's continue without index, so we'll at least have the recording
}

cFileWriter::~cFileWriter()
{
  Cancel(5);
  delete index;
  delete fileName;
}

bool cFileWriter::RunningLowOnDiskSpace(void)
{
  if (time(NULL) > lastDiskSpaceCheck + DISKCHECKINTERVAL) {
     int Free = FreeDiskSpaceMB(fileName->Name());
     lastDiskSpaceCheck = time(NULL);
     if (Free < MINFREEDISKSPACE) {
        dsyslog("low disk space (%d MB, limit is %d MB)", Free, MINFREEDISKSPACE);
        return true;
        }
     }
  return false;
}

bool cFileWriter::NextFile(void)
{
  if (recordFile && pictureType == I_FRAME) { // every file shall start with an I_FRAME
     if (fileSize > MEGABYTE(Setup.MaxVideoFileSize) || RunningLowOnDiskSpace()) {
        recordFile = fileName->NextFile();
        fileSize = 0;
        }
     }
  return recordFile != NULL;
}


void cFileWriter::Action(void)
{
  if (!recordFile) {
     esyslog("No recording file ... giving up!");
     return;
     }
#ifdef USE_WATCHDOG_IN_FILEWRITER
  time_t t = time(NULL);
#endif

  while (Running()) {
        int Count;
        sPesResult *Header;
        int HeaderCount;
        int FirstIFrame;
        const uchar *p = remux->Get(Count, Header, HeaderCount, FirstIFrame);
        if (p) {

           if (recordFile->Write(p, Count) < 0) {
              LOG_ERROR_STR(fileName->Name());
              break;
              }

           if (FirstIFrame != -1 && (!Running() || fileSize > MEGABYTE(Setup.MaxVideoFileSize) || RunningLowOnDiskSpace())) {
              if (index && FirstIFrame)
                 index->Write(Header, FirstIFrame, fileName->Number(), fileSize);

              recordFile->Truncate(fileSize + Header[FirstIFrame].offset);

              if (!(recordFile = fileName->NextFile())) {
                 LOG_ERROR;
                 esyslog("Cannot open next recording file '%s' ... giving up",fileName->Name());
                 break;
                 }

              fileSize = 0;
              remux->Del(Header[FirstIFrame].offset);

              }
           else {
              if (index && HeaderCount)
                 index->Write(Header, HeaderCount, fileName->Number(), fileSize);

              remux->Del(Count);
              fileSize += Count;
              }
           //cCondWait::SleepMs(50);
#ifdef USE_WATCHDOG_IN_FILEWRITER
           t = time(NULL);
#endif
           }
#ifdef USE_WATCHDOG_IN_FILEWRITER
        else if (time(NULL) - t > MAXBROKENTIMEOUT) {
           esyslog("ERROR: video data stream broken");
           cThread::EmergencyExit(true);
           t = time(NULL);
           }
#endif
        }

  if (fileSize && recordFile && index) {
     fileSize = index->StripOffLast();
     if (fileSize > 0)
        recordFile->Truncate(fileSize);
     }

  if (fileSize == 0) {
     fileName->Unlink();
     }

}

cRecorder::cRecorder(const char *FileName, int Ca, int Priority, int VPid, const int *APids, const int *DPids, const int *SPids)
:cReceiver(Ca, Priority, VPid, APids, Setup.UseDolbyDigital ? DPids : NULL, SPids)
#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
,cThread("recording")
#endif
{
  // Make sure the disk is up and running:
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  setFrameEventsWanted(VPid);
#endif
  SpinUpDisk(FileName);
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
  lostBytes = 0;
#else
#ifdef USE_RECEIVER_RINGBUFFER
  ringBuffer = new cRingBufferReceiver(RECORDERBUFSIZE, true, "Recorder");
#else
  ringBuffer = new cRingBufferLinear(RECORDERBUFSIZE, TS_SIZE * 2, true, "Recorder");
#endif
  ringBuffer->SetTimeouts(0, 500);
  ringBuffer->SetLimits(TS_SIZE, TS_SIZE * 1024);
#endif
  remux = new cRemux(VPid, APids, Setup.UseDolbyDigital ? DPids : NULL, SPids, true);
  writer = new cFileWriter(FileName, remux);
}

cRecorder::~cRecorder()
{
  Detach();
  delete writer;
  delete remux;
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
  isyslog("INFO: Recoder has lost %d bytes",lostBytes);
#else
  delete ringBuffer;
#endif
}

void cRecorder::Activate(bool On)
{
  if (On) {
     writer->Start();
#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
     Start();
#endif
     }
#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
  else
     Cancel(10);
#endif
}
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
void cRecorder::Receive(uchar *Data, int Length) {
  esyslog("ERROR: cRecorder::Receive(uchar*, int) should not be called any more!");
}
void cRecorder::Receive(uchar *Data, int Length, const sTsDataHeader *const Header)
{
  int p = remux->Put(Data, Length, Header);
  lostBytes += Length - p;
}
#else
#ifdef USE_RECEIVER_RINGBUFFER
void cRecorder::Receive(uchar *Data, int Length) {
  esyslog("ERROR: cRecorder::Receive(uchar*, int) should not be called any more!");
}
void cRecorder::Receive(uchar *Data, int Length, const sTsDataHeader *const Header)
{
  if (Running()) {
     if (!ringBuffer->Put(Data, Length, Header) && Running())
        ringBuffer->ReportOverflow(Length);

     }
}
void cRecorder::Action(void)
{
  const sTsDataHeader *Header;
  int Count;
  uchar *Data;
  while (Running()) {
        Header = ringBuffer->Get(Data, Count);
        if (Header) {
           Count = remux->Put(Data, Count, Header);
           if (Count)
              ringBuffer->Del(Count);
           else
              cCondWait::SleepMs(100); // avoid busy loop when resultBuffer is full in cRemux::Put()
           }
        }
}
#else
void cRecorder::Receive(uchar *Data, int Length) {
  if (Running()) {
     int p = ringBuffer->Put(Data, Length);
     if (p != Length && Running())
        ringBuffer->ReportOverflow(Length - p);

     }
}
void cRecorder::Action(void)
{
  while (Running()) {
        int r;
        uchar *b = ringBuffer->Get(r);
        if (b) {
           int Count = remux->Put(b, r);
           if (Count)
              ringBuffer->Del(Count);
           else
              cCondWait::SleepMs(100); // avoid busy loop when resultBuffer is full in cRemux::Put()
           }
        }
}
#endif
#endif
//M7X0 END AK

/*
 * cutter.c: The video cutting facilities
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cutter.c 205 2007-09-18 21:50:28Z andreas $
 */

#include "cutter.h"
#include "recording.h"
#include "remux.h"
#include "thread.h"
#include "videodir.h"


// --- cCuttingThread --------------------------------------------------------

#ifndef CUTTER_PRIORITY
#  define CUTTER_PRIORITY sched_get_priority_min(SCHED_OTHER)
#endif
#define CUTTER_TIMESLICE   100   // ms

class cCuttingThread : public cThread {
private:
  const char *error;
  cUnbufferedFile *fromFile, *toFile;
  cFileName *fromFileName, *toFileName;
  cIndexFile *fromIndex, *toIndex;
  cMarks fromMarks, toMarks;
protected:
  virtual void Action(void);
public:
  cCuttingThread(const char *FromFileName, const char *ToFileName);
  virtual ~cCuttingThread();
  const char *Error(void) { return error; }
  };

cCuttingThread::cCuttingThread(const char *FromFileName, const char *ToFileName)
:cThread("video cutting")
{
  error = NULL;
  fromFile = toFile = NULL;
  fromFileName = toFileName = NULL;
  fromIndex = toIndex = NULL;
  if (fromMarks.Load(FromFileName) && fromMarks.Count()) {
     fromFileName = new cFileName(FromFileName, false, true);
     toFileName = new cFileName(ToFileName, true, true);
     fromIndex = new cIndexFile(FromFileName, false);
     toIndex = new cIndexFile(ToFileName, true);
     toMarks.Load(ToFileName); // doesn't actually load marks, just sets the file name
     Start();
     }
  else
     esyslog("no editing marks found for %s", FromFileName);
}

cCuttingThread::~cCuttingThread()
{
  Cancel(3);
  delete fromFileName;
  delete toFileName;
  delete fromIndex;
  delete toIndex;
}

void cCuttingThread::Action(void)
{
  {
    sched_param tmp;
    tmp.sched_priority = CUTTER_PRIORITY;
    if(!pthread_setschedparam(pthread_self(), SCHED_OTHER, &tmp))
      printf("cCuttingThread::Action: cant set priority\n");
  }

  int bytes = 0;
  int __attribute__((unused)) burst_size = MEGABYTE(Setup.CutterMaxBandwidth) * CUTTER_TIMESLICE / 1000; // max bytes/timeslice 
  cTimeMs __attribute__((unused)) t;

//M7X0 BEGIN AK
  SetPriority(19);
//M7X0 END AK
  cMark *Mark = fromMarks.First();
  if (Mark) {
     fromFile = fromFileName->Open();
     toFile = toFileName->Open();
     if (!fromFile || !toFile)
        return;
     fromFile->SetReadAhead(MEGABYTE(20));
     int Index = Mark->position;
     Mark = fromMarks.Next(Mark);
     int FileSize = 0;
     int CurrentFileNumber = 0;
     int LastIFrame = 0;
     toMarks.Add(0);
     toMarks.Save();
     uchar buffer[MAXFRAMESIZE];
     bool LastMark = false;
     bool cutIn = true;
     while (Running()) {
           uchar FileNumber;
           int FileOffset, Length;
           uchar PictureType;

           // Make sure there is enough disk space:

           AssertFreeDiskSpace(-1);

           // Read one frame:

           if (fromIndex->Get(Index++, &FileNumber, &FileOffset, &PictureType, &Length)) {
              if (FileNumber != CurrentFileNumber) {
                 fromFile = fromFileName->SetOffset(FileNumber, FileOffset);
                 fromFile->SetReadAhead(MEGABYTE(20));
                 CurrentFileNumber = FileNumber;
                 }
              if (fromFile) {
                 int len = ReadFrame(fromFile, buffer,  Length, sizeof(buffer));
                 if (len < 0) {
                    error = "ReadFrame";
                    break;
                    }
                 if (len != Length) {
                    CurrentFileNumber = 0; // this re-syncs in case the frame was larger than the buffer
                    Length = len;
                    }
                 }
              else {
                 error = "fromFile";
                 break;
                 }
              }
           else
              break;

           // Write one frame:

           if (PictureType == I_FRAME) { // every file shall start with an I_FRAME
              if (LastMark) // edited version shall end before next I-frame
                 break;
              if (FileSize > MEGABYTE(Setup.MaxVideoFileSize)) {
                 toFile = toFileName->NextFile();
                 if (!toFile) {
                    error = "toFile 1";
                    break;
                    }
                 FileSize = 0;
                 }
              LastIFrame = 0;

              if (cutIn) {
                 cRemux::SetBrokenLink(buffer, Length);
                 cutIn = false;
                 }
              }
           if (toFile->Write(buffer, Length) < 0) {
              error = "safe_write";
              break;
              }
           if (!toIndex->Write(PictureType, toFileName->Number(), FileSize)) {
              error = "toIndex";
              break;
              }
           FileSize += Length;
           if (!LastIFrame)
              LastIFrame = toIndex->Last();

           // Check editing marks:

           if (Mark && Index >= Mark->position) {
              Mark = fromMarks.Next(Mark);
              toMarks.Add(LastIFrame);
              if (Mark)
                 toMarks.Add(toIndex->Last() + 1);
              toMarks.Save();
              if (Mark) {
                 Index = Mark->position;
                 Mark = fromMarks.Next(Mark);
                 CurrentFileNumber = 0; // triggers SetOffset before reading next frame
                 cutIn = true;
                 if (Setup.SplitEditedFiles) {
                    toFile = toFileName->NextFile();
                    if (!toFile) {
                       error = "toFile 2";
                       break;
                       }
                    FileSize = 0;
                    }
                 }
              else
                 LastMark = true;
              }

	   bytes += Length;
	   if(bytes >= burst_size) {
	     int elapsed = t.Elapsed();
	     int sleep = 0;
	     
	     if (Setup.CutterRelBandwidth > 0 &&  Setup.CutterRelBandwidth < 100) {
	       // stay under max. relative bandwidth

	       sleep = (elapsed * 100 / Setup.CutterRelBandwidth) - elapsed;
	       //if(sleep<=0 && elapsed<=2) sleep = 1; 
	       //if(sleep) esyslog("cutter: relative bandwidth limit, sleep %d ms (chunk %dk / %dms)", sleep, burst_size/1024, CUTTER_TIMESLICE);
	     }

	     // stay under max. absolute bandwidth
	     if(elapsed < CUTTER_TIMESLICE) {
	       sleep = max(CUTTER_TIMESLICE - elapsed, sleep);
	       //if(sleep) esyslog("cutter: absolute bandwidth limit, sleep %d ms (chunk %dk / %dms)", sleep, burst_size/1024, CUTTER_TIMESLICE);
	     }

	     if(sleep>0)
	       cCondWait::SleepMs(sleep);
	     t.Set();
	     bytes = 0;
	   }

           }
     Recordings.TouchUpdate();
     }
  else
     esyslog("no editing marks found!");
}

// --- cCutter ---------------------------------------------------------------

#define WAIT_BEFORE_NEXT_CUT   (10*1000)  // 10 seconds

#define CUTTER_QUEUE

class cStringListObject : public cListObject {
  public:
    cStringListObject(const char *s) { str = strdup(s); }
    ~cStringListObject() { free(str); }

    const char *Value() { return str; }
    operator const char * () { return str; }

  private:  
    char *str;
};

char *cCutter::editedVersionName = NULL;
cCuttingThread *cCutter::cuttingThread = NULL;
bool cCutter::error = false;
bool cCutter::ended = false;
cMutex *cCutter::cutterLock = new cMutex();

static uint64 /*cCutter::*/lastCuttingEndTime = 0;
static cList<cStringListObject> /**cCutter::*/cutterQueue /*= new cList<cStringListObject>*/;

bool cCutter::Start(const char *FileName)
{
  cMutexLock(cutterLock);

#ifdef CUTTER_QUEUE
  if(FileName) {
    /* Add file to queue.
     * If cutter is still active, next cutting will be started 
     * when vdr.c:main calls cCutter::Active and previous cutting has 
     * been stopped > 10 s before 
     */
    cutterQueue.Add(new cStringListObject(FileName));
  }

  if (cuttingThread) 
    return true;

  /* cut next file from queue */
  if(!(cutterQueue.First()))
    return false;
  FileName = cutterQueue.First()->Value();
#endif

  if (!cuttingThread) {
     error = false;
     ended = false;
     cRecording Recording(FileName);
     
     cMarks FromMarks;
     FromMarks.Load(FileName);
     cMark *First=FromMarks.First();
     if (First) Recording.SetStartTime(Recording.start+((First->position/FRAMESPERSEC+30)/60)*60);
     
     const char *evn = Recording.PrefixFileName('%');

     if(!(Recordings.GetByName(FileName))) {
       // Should _not_ remove any cutted recordings 
       // (original recording already deleted ?)
       // so, just pop item from queue and return.
       esyslog("can't cut non-existing recording %s", FileName);
#ifdef CUTTER_QUEUE
       cutterQueue.Del(cutterQueue.First());       
       return true; // might be already queued recording
#else
       return false;
#endif
     }
     if (evn && RemoveVideoFile(evn) && MakeDirs(evn, true)) {
        // XXX this can be removed once RenameVideoFile() follows symlinks (see videodir.c)
        // remove a possible deleted recording with the same name to avoid symlink mixups:
        char *s = strdup(evn);
        char *e = strrchr(s, '.');
        if (e) {
           if (strcmp(e, ".rec") == 0) {
              strcpy(e, ".del");
              RemoveVideoFile(s);
              }
           }
        free(s);
        // XXX
        editedVersionName = strdup(evn);
        Recording.WriteInfo();
        Recordings.AddByName(editedVersionName, false);
        cuttingThread = new cCuttingThread(FileName, editedVersionName);
        return true;
        }
     }
  return false;
}

void cCutter::Stop(void)
{
  cMutexLock(cutterLock);

  bool Interrupted = cuttingThread && cuttingThread->Active();
  const char *Error = cuttingThread ? cuttingThread->Error() : NULL;
  delete cuttingThread;
  cuttingThread = NULL;
  if ((Interrupted || Error) && editedVersionName) {
     if (Interrupted)
        isyslog("editing process has been interrupted");
     if (Error)
        esyslog("ERROR: '%s' during editing process", Error);
     RemoveVideoFile(editedVersionName); //XXX what if this file is currently being replayed?
     Recordings.DelByName(editedVersionName);
#ifdef CUTTER_QUEUE
     cutterQueue.Del(cutterQueue.First());
#endif
     }
#ifdef CUTTER_QUEUE
  lastCuttingEndTime = cTimeMs::Now();
#endif
}

bool cCutter::Active(void)
{
  cMutexLock(cutterLock);

  if (cuttingThread) {
     if (cuttingThread->Active())
        return true;
     error = cuttingThread->Error();
     Stop();
     if (!error)
        cRecordingUserCommand::InvokeCommand(RUC_EDITEDRECORDING, editedVersionName);
     free(editedVersionName);
     editedVersionName = NULL;
     ended = true;
     if(Setup.CutterAutoDelete){
        /* Remove original (if cutting was successful) */
        if(!error) {
          cRecording *recording = Recordings.GetByName(*cutterQueue.First());
          if (!recording) {
             esyslog("ERROR: Can't found '%s' after editing process", cutterQueue.First()->Value());
          } else {
             if (recording->Delete()) {
	        //Recordings.Del(recording);
                //cReplayControl::ClearLastReplayed(ri->FileName());
                Recordings.DelByName(recording->FileName());
             } else {
                esyslog("ERROR: Can't delete '%s' after editing process", cutterQueue.First()->Value());
             }
          }
        }
        lastCuttingEndTime = cTimeMs::Now();
     }
#ifdef CUTTER_QUEUE
     cutterQueue.Del(cutterQueue.First());
#endif
  }
#ifdef CUTTER_QUEUE
  if(!cuttingThread && cutterQueue.First()) {
    /* start next cutting from queue*/
    if(cTimeMs::Now() > lastCuttingEndTime + WAIT_BEFORE_NEXT_CUT)
      Start(NULL);
  }
#endif

  return false;
}

bool cCutter::Error(void)
{
  cMutexLock(cutterLock);
  bool result = error;
  error = false;
  return result;
}

bool cCutter::Ended(void)
{
  cMutexLock(cutterLock);
  bool result = ended;
  ended = false;
  return result;
}

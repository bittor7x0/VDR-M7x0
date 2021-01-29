/*
 * cutter.c: The video cutting facilities
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "cutter.h"
#include "recording.h"
#include "remux.h"
#include "thread.h"
#include "videodir.h"


// --- cCuttingThread --------------------------------------------------------

#define CUTTER_TIMESLICE   100   // ms

class cCuttingThread : public cThread {
private:
  const char *error;
  bool isPesRecording;
  cUnbufferedFile *fromFile, *toFile;
  cFileName *fromFileName, *toFileName;
  cIndexFile *fromIndex, *toIndex;
  cMarks fromMarks, toMarks;
  off_t maxVideoFileSize;
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
  cRecording Recording(FromFileName);
  isPesRecording = Recording.IsPesRecording();
  if (fromMarks.Load(FromFileName, isPesRecording) && fromMarks.Count()) {
     fromFileName = new cFileName(FromFileName, false, true, isPesRecording);
     toFileName = new cFileName(ToFileName, true, true, isPesRecording);
     fromIndex = new cIndexFile(FromFileName, false, isPesRecording);
     toIndex = new cIndexFile(ToFileName, true, isPesRecording);
     if (Setup.CutterWithMarks)
        toMarks.Load(ToFileName, isPesRecording); // doesn't actually load marks, just sets the file name
     maxVideoFileSize = MEGABYTE(Setup.MaxVideoFileSize);
     if (isPesRecording && maxVideoFileSize > MEGABYTE(MAXVIDEOFILESIZEPES))
        maxVideoFileSize = MEGABYTE(MAXVIDEOFILESIZEPES);
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
  cMark *Mark = fromMarks.First();
  if (Mark) {
     SetPriority(19);
     fromFile = fromFileName->Open();
     toFile = toFileName->Open();
     if (!fromFile || !toFile)
        return;
#ifdef USE_FADVISE
     fromFile->SetReadAhead(MEGABYTE(20));
#endif
     int Index = Mark->position;
     Mark = fromMarks.Next(Mark);
     off_t FileSize = 0;
     uint16_t CurrentFileNumber = 0;
     bool SkipThisSourceFile = false;
     int LastIFrame = 0;
     if (Setup.CutterWithMarks) {
        toMarks.Add(0);
        toMarks.Save();
        }
     uchar buffer[MAXFRAMESIZE];
     bool LastMark = false;
     bool cutIn = true;
     int bytes = 0;
     int burst_size = MEGABYTE(Setup.CutterMaxBandwidth) * CUTTER_TIMESLICE / 1000; // max bytes/timeslice
     cTimeMs t;
     while (Running()) {
           uint16_t FileNumber;
           off_t FileOffset;
           int Length;
           uchar PictureType;

           // Make sure there is enough disk space:

           AssertFreeDiskSpace(-1);

           // Read one frame:

           if (!fromIndex->Get(Index++, &FileNumber, &FileOffset, &PictureType, &Length)) {
              // Error, unless we're past last cut-in and there's no cut-out
              if (Mark || LastMark)
                 error = "index";
              break;
              }

           if (FileNumber != CurrentFileNumber) {
              fromFile = fromFileName->SetOffset(FileNumber, FileOffset);
#ifdef USE_FADVISE
              fromFile->SetReadAhead(MEGABYTE(20));
#endif
              CurrentFileNumber = FileNumber;
              if (SkipThisSourceFile) {
                 // At end of fast forward: Always skip to next file
                 toFile = toFileName->NextFile();
                 if (!toFile) {
                    error = "toFile 4";
                    break;
                    }
                 FileSize = 0;
                 SkipThisSourceFile = false;
                 }                 
              

              if (Setup.HardLinkCutter && FileOffset == 0) {
                 // We are at the beginning of a new source file.
                 // Do we need to copy the whole file?

                 // if !Mark && LastMark, then we're past the last cut-out and continue to next I-frame
                 // if !Mark && !LastMark, then there's just a cut-in, but no cut-out
                 // if Mark, then we're between a cut-in and a cut-out
                 
                 uint16_t MarkFileNumber;
                 off_t MarkFileOffset;
                 // Get file number of next cut mark
                 if (!Mark && !LastMark
                     || Mark
                        && fromIndex->Get(Mark->position, &MarkFileNumber, &MarkFileOffset)
                        && (MarkFileNumber != CurrentFileNumber)) {
                    // The current source file will be copied completely.
                    // Start new output file unless we did that already
                    if (FileSize != 0) {
                       toFile = toFileName->NextFile();
                       if (!toFile) {
                          error = "toFile 3";
                          break;
                          }
                       FileSize = 0;
                       }

                    // Safety check that file has zero size
                    struct stat buf;
                    if (stat(toFileName->Name(), &buf) == 0) {
                       if (buf.st_size != 0) {
                          esyslog("cCuttingThread: File %s exists and has nonzero size", toFileName->Name());
                          error = "nonzero file exist";
                          break;
                          }
                       }
                    else if (errno != ENOENT) {
                       esyslog("cCuttingThread: stat failed on %s", toFileName->Name());
                       error = "stat";
                       break;
                       }

                    // Clean the existing 0-byte file
                    toFileName->Close();
                    cString ActualToFileName(ReadLink(toFileName->Name()), true);
                    unlink(ActualToFileName);
                    unlink(toFileName->Name());

                    // Try to create a hard link
                    if (HardLinkVideoFile(fromFileName->Name(), toFileName->Name())) {
                       // Success. Skip all data transfer for this file
                       SkipThisSourceFile = true;
                       cutIn = false;
                       toFile = NULL; // was deleted by toFileName->Close()
                       } 
                    else {
                       // Fallback: Re-open the file if necessary
                       toFile = toFileName->Open();
                       }
                    }
                 } 
              }

           if (!SkipThisSourceFile) {
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
           // Write one frame:

           if (PictureType == I_FRAME) { // every file shall start with an I_FRAME
              if (LastMark) // edited version shall end before next I-frame
                 break;
              if (!SkipThisSourceFile && FileSize > toFileName->MaxFileSize()) {
                 toFile = toFileName->NextFile();
                 if (!toFile) {
                    error = "toFile 1";
                    break;
                    }
                 FileSize = 0;
                 }
              if (Setup.CutterWithMarks)
                 LastIFrame = 0;

              if (!SkipThisSourceFile && cutIn) {
                 if (isPesRecording)
                    cRemux::SetBrokenLink(buffer, Length);
                 else
                    TsSetTeiOnBrokenPackets(buffer, Length);
                 cutIn = false;
                 }
              }
           if (!SkipThisSourceFile && toFile->Write(buffer, Length) < 0) {
              error = "safe_write";
              break;
              }
           if (!toIndex->Write(PictureType, toFileName->Number(), FileSize)) {
              error = "toIndex";
              break;
              }
           FileSize += Length;
           if (Setup.CutterWithMarks && !LastIFrame)
                 LastIFrame = toIndex->Last();

           // Check editing marks:

           if (Mark && Index >= Mark->position) {
              Mark = fromMarks.Next(Mark);
              if (Setup.CutterWithMarks) {
                 toMarks.Add(LastIFrame);
                 if (Mark)
                    toMarks.Add(toIndex->Last() + 1);
                 toMarks.Save();
                 }
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
                 LastMark = true; // After last cut-out: Write on until next I-frame, then exit
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
	       sleep = std::max(CUTTER_TIMESLICE - elapsed, sleep);
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
     FromMarks.Load(FileName, Recording.IsPesRecording());
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

#define CUTTINGCHECKINTERVAL 500 // ms between checks for the active cutting process

bool CutRecording(const char *FileName)
{
  if (DirectoryOk(FileName)) {
     cRecording Recording(FileName);
     if (Recording.Name()) {
        cMarks Marks;
        if (Marks.Load(FileName, Recording.IsPesRecording()) && Marks.Count()) {
           if (cCutter::Start(FileName)) {
              while (cCutter::Active())
                    cCondWait::SleepMs(CUTTINGCHECKINTERVAL);
              return true;
              }
           else
              fprintf(stderr, "can't start editing process\n");
           }
        else
           fprintf(stderr, "'%s' has no editing marks\n", FileName);
        }
     else
        fprintf(stderr, "'%s' is not a recording\n", FileName);
     }
  else
     fprintf(stderr, "'%s' is not a directory\n", FileName);
  return false;
}

/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#ifndef __MPGPLAYER_CONTROLLER_H
#define __MPGPLAYER_CONTROLLER_H

#include "mpgplayer_recording.h"
#include "mpgplayer_tools.h"
#include "mpgplayer_setup.h"
#include "mpgplayer_menu.h"
#define MPG_POSTFIX_REGEX ".*\\.mpg$"
#define MPG_DELETED_REGEX ".*(mpg\\.(resume.*|marks|del)\\.vdr|\\.mpg)$"

class cMpgController {
private:
  cDirScanner *dirScanner;
  cMpgRecordings recordings;
  const cPathObject *pathSelected;
  cMpgSeriesRecording *seriesSelected;
  cMpgRecording *recordingSelected;
  char *playingRecording;
  bool playingRecordingDeleted;
public:
  cMpgController(void) {
     dirScanner = new cDirScanner(
                      MpgplayerSetup.BasePath,
                      MpgplayerSetup.LastPath,
                      MPG_POSTFIX_REGEX,
                      MpgplayerSetup.ShowHidden);
     if (!dirScanner)
        esyslog("cMpgController(void): Cannot alloc memory!");

     MpgplayerSetup.SetBasePath(dirScanner->GetBasePath());
     pathSelected = NULL;
     seriesSelected = NULL;
     recordingSelected = NULL;
     playingRecording = NULL;
     playingRecordingDeleted = false;
     }
  ~cMpgController() {
     delete dirScanner;
     free(playingRecording);
     }
  const cPathObject *GetPathSelected(void) const { return pathSelected; }
  cMpgSeriesRecording *GetSeriesSelected(void) const { return seriesSelected; }
  cMpgRecording *GetRecordingSelected(void) const { return recordingSelected; }
  void SetPathSelected(const cPathObject *path) { pathSelected = path; }
  void SetSeriesSelected(cMpgSeriesRecording *series) { seriesSelected = series; }
  void SetRecordingSelected(cMpgRecording *rec) { recordingSelected = rec;}
  const char *GetTitle(void) const { return dirScanner->DisplayPath(); }
  bool Read(void);
  bool ChDir(const cPathObject *path) { return dirScanner->ChDir(path->Name()); }
  const cList<cPathObject> *GetDirectories() const { return dirScanner->DirList(); }
  cMpgRecordings *GetRecordings(void) { return &recordings; }
  cMpgSeriesRecordings *GetSeriesRecordings(void) { return recordings.GetSeries(); }
  void Play(const cMpgRecording *Rec) {
     playingRecordingDeleted = false;
     cControl::Launch(new cMpgReplayControl(this, Rec));
     free(playingRecording);
     if (asprintf(&playingRecording,"%s/%s",Rec->GetPath(),Rec->GetFilename()) < 0)
        playingRecording = NULL;
     }
  void ReplayEnded(void) { free(playingRecording); playingRecording = NULL; }
  bool ReplayRecordingDeleted(void) { return  playingRecordingDeleted; }
  const char *GetCurrentPath(void) { return dirScanner->Path(); }
  void SetBasePath(const char *Path) {
     dirScanner->SetBasePath(Path);
     MpgplayerSetup.SetBasePath(dirScanner->GetBasePath());
     }
  void SetInitPath(const char *Path) {
     if (Path[0] == '/' && dirScanner->ChDir(Path)) {
        MpgplayerSetup.SetLastPath(Path);
        MpgplayerSetup.SetBasePath(dirScanner->GetBasePath());
        }
     }
  void SetShowHidden(bool ShowHidden) {
     dirScanner->SetReadHidden(ShowHidden);
     MpgplayerSetup.ShowHidden = ShowHidden;
     }
  bool Delete(cMpgSeriesRecording *series, cMpgRecording *rec) {
     if (!rec->Delete())
        return false;
     if (playingRecording) {
        char *tmp;
        if (asprintf(&tmp,"%s/%s",rec->GetPath(),rec->GetFilename()) >= 0) {
           if (!strcmp(playingRecording,tmp))
              playingRecordingDeleted = true;
           free(tmp);
           }
        }
     series->Del(rec);
     if (!series->Count()) {
        recordings.GetSeries()->Del(series);
        }
     return true;
     }
   bool Delete(cMpgRecording *rec) {
     if (!rec->Delete())
        return false;
     if (playingRecording) {
        char *tmp;
        if (asprintf(&tmp,"%s/%s",rec->GetPath(),rec->GetFilename()) >= 0) {
           if (!strcmp(playingRecording,tmp))
              playingRecordingDeleted = true;
           free(tmp);
           }
        }
     recordings.Del(rec);
     return true;
     }
  };

#define MPG_REMOVED_CHECK_INTERVAL 1800 // s
class cMpgRemoveDeleted : public cThread {
private:
  cDirScanner *dirScanner;
  cList<cPathObject> pathList;
  cList<cPathObject> *leftOverDeletedFiles;
  time_t lastRun;
  int32_t preGapTime;
  int32_t postGapTime;
  bool NeedsRun(void) { return !Active() && lastRun + MPG_REMOVED_CHECK_INTERVAL < time(NULL); }
  void HandleDirectories(void);
  bool GetLists(cList<cPathObject> *deletedFiles,
                                      cList<cMpgCridFile> *deletedRecordings,
                                            cList<cMpgCridFile> *recordings);
  bool GetDeletedFromFmpg(cList<cPathObject> *deletedFiles,
                                  cMpgFmpgFile *fmpgFile);
  bool GetDeletedRecordingFiles(cList<cPathObject> *deletedFiles,
                                      cList<cMpgCridFile> *deletedRecordings,
                                            cList<cMpgCridFile> *recordings);
  void DeleteFiles(cList<cPathObject> *deletedFiles);
protected:
  virtual void Action(void);
public:
  cMpgRemoveDeleted(void) : cThread("mpgplayer removed recordings") {
     dirScanner = new cDirScanner("/","/", MPG_DELETED_REGEX, true);
     lastRun = 0;
     leftOverDeletedFiles = new cList<cPathObject>;
     }
  virtual ~cMpgRemoveDeleted() { Cancel(10); delete dirScanner; delete leftOverDeletedFiles;}
  void StartRemoveOnNeed(void);
  };
#endif

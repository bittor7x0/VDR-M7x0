/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_controller.h 13 2007-03-06 07:09:16Z andreas $
 *
 */

#ifndef __CRIDPLAYER_CONTROLLER_H
#define __CRIDPLAYER_CONTROLLER_H

#include "cridplayer_recording.h"
#include "cridplayer_tools.h"
#include "cridplayer_setup.h"
#include "cridplayer_menu.h"
#define CRID_POSTFIX_REGEX ".*\\.crid$"
#define CRID_DELETED_REGEX ".*(crid\\.(resume.*|marks|del)\\.vdr|\\.crid)$"

class cCridController {
private:
  cDirScanner *dirScanner;
  cCridRecordings recordings;
  const cPathObject *pathSelected;
  cCridSeriesRecording *seriesSelected;
  cCridRecording *recordingSelected;
  char *playingRecording;
  bool playingRecordingDeleted;
public:
  cCridController(void) {
     dirScanner = new cDirScanner(
                      CridplayerSetup.BasePath,
                      CridplayerSetup.LastPath,
                      CRID_POSTFIX_REGEX,
                      CridplayerSetup.ShowHidden);
     if (!dirScanner)
        esyslog("cCridController(void): Cannot alloc memory!");

     CridplayerSetup.SetBasePath(dirScanner->GetBasePath());
     pathSelected = NULL;
     seriesSelected = NULL;
     recordingSelected = NULL;
     playingRecording = NULL;
     playingRecordingDeleted = false;
     }
  ~cCridController() {
     delete dirScanner;
     free(playingRecording);
     }
  const cPathObject *GetPathSelected(void) const { return pathSelected; }
  cCridSeriesRecording *GetSeriesSelected(void) const { return seriesSelected; }
  cCridRecording *GetRecordingSelected(void) const { return recordingSelected; }
  void SetPathSelected(const cPathObject *path) { pathSelected = path; }
  void SetSeriesSelected(cCridSeriesRecording *series) { seriesSelected = series; }
  void SetRecordingSelected(cCridRecording *rec) { recordingSelected = rec;}
  const char *GetTitle(void) const { return dirScanner->DisplayPath(); }
  bool Read(void);
  bool ChDir(const cPathObject *path) { return dirScanner->ChDir(path->Name()); }
  const cList<cPathObject> *GetDirectories() const { return dirScanner->DirList(); }
  cCridRecordings *GetRecordings(void) { return &recordings; }
  cCridSeriesRecordings *GetSeriesRecordings(void) { return recordings.GetSeries(); }
  void Play(const cCridRecording *Rec) {
     playingRecordingDeleted = false;
     cControl::Launch(new cCridReplayControl(this, Rec));
     free(playingRecording);
     if (asprintf(&playingRecording,"%s/%s",Rec->GetPath(),Rec->GetFilename()) < 0)
        playingRecording = NULL;
     }
  void ReplayEnded(void) { free(playingRecording); playingRecording = NULL; }
  bool ReplayRecordingDeleted(void) { return  playingRecordingDeleted; }
  const char *GetCurrentPath(void) { return dirScanner->Path(); }
  void SetBasePath(const char *Path) {
     dirScanner->SetBasePath(Path);
     CridplayerSetup.SetBasePath(dirScanner->GetBasePath());
     }
  void SetInitPath(const char *Path) {
     if (Path[0] == '/' && dirScanner->ChDir(Path)) {
        CridplayerSetup.SetLastPath(Path);
        CridplayerSetup.SetBasePath(dirScanner->GetBasePath());
        }
     }
  void SetShowHidden(bool ShowHidden) {
     dirScanner->SetReadHidden(ShowHidden);
     CridplayerSetup.ShowHidden = ShowHidden;
     }
  bool Delete(cCridSeriesRecording *series, cCridRecording *rec) {
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
   bool Delete(cCridRecording *rec) {
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

#define CRID_REMOVED_CHECK_INTERVAL 1800 // s
class cCridRemoveDeleted : public cThread {
private:
  cDirScanner *dirScanner;
  cList<cPathObject> pathList;
  cList<cPathObject> *leftOverDeletedFiles;
  time_t lastRun;
  int32_t preGapTime;
  int32_t postGapTime;
  bool NeedsRun(void) { return !Active() && lastRun + CRID_REMOVED_CHECK_INTERVAL < time(NULL); }
  void HandleDirectories(void);
  bool GetLists(cList<cPathObject> *deletedFiles,
                                      cList<cCridCridFile> *deletedRecordings,
                                            cList<cCridCridFile> *recordings);
  bool GetDeletedFromFmpg(cList<cPathObject> *deletedFiles,
                                  cCridFmpgFile *fmpgFile);
  bool GetDeletedRecordingFiles(cList<cPathObject> *deletedFiles,
                                      cList<cCridCridFile> *deletedRecordings,
                                            cList<cCridCridFile> *recordings);
  void DeleteFiles(cList<cPathObject> *deletedFiles);
protected:
  virtual void Action(void);
public:
  cCridRemoveDeleted(void) : cThread("cridplayer removed recordings") {
     dirScanner = new cDirScanner("/","/", CRID_DELETED_REGEX, true);
     lastRun = 0;
     leftOverDeletedFiles = new cList<cPathObject>;
     }
  virtual ~cCridRemoveDeleted() { Cancel(10); delete dirScanner; delete leftOverDeletedFiles;}
  void StartRemoveOnNeed(void);
  };
#endif

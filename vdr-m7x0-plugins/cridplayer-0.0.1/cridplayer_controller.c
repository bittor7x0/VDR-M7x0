/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_controller.c 79 2007-10-22 15:37:19Z andreas $
 *
 */

#include "cridplayer_controller.h"

// --- cCridController -------------------------------------------------------
bool cCridController::Read(void)
{
  if (dirScanner->IsRead())
     return true;

  if (!dirScanner->Read())
     return false;

  recordings.Clear();
  const cList<cPathObject> *cridFiles = dirScanner->FileList();
  if (!cridFiles->Count())
     return true;

  char *tmp = ReadLink(dirScanner->Path());
  if (!tmp)
     return false;
  for (const cPathObject *pobj = cridFiles->First(); pobj;
                          pobj = cridFiles->Next(pobj)) {
      recordings.Add(new cCridRecording(tmp, pobj->Name()));
      }
  free(tmp);
  recordings.Sort();
  return true;
}

// --- cCridRemoveDeleted ----------------------------------------------------

void cCridRemoveDeleted::HandleDirectories(void)
{
  const char *curPath = dirScanner->Path();
  const int curPathLen = strlen(curPath) + 1;
  char newPath[curPathLen + NAME_MAX + 1];

  memcpy(newPath, curPath, curPathLen - 1);
  newPath[curPathLen - 1] = '/';
  newPath[curPathLen + NAME_MAX] = 0;

  const cList<cPathObject> *dirs = dirScanner->DirList();

  for (const cPathObject *pobj = dirs->First(); pobj && Running();
                                          pobj = dirs->Next(pobj)) {
      if (!strcmp(pobj->Name(), "..") || pobj->LinkTo())
         continue;
      strcpy(newPath + curPathLen, pobj->Name());
      pathList.Add(new cPathObject(newPath, NULL, true));
      }
}

bool cCridRemoveDeleted::GetLists(cList<cPathObject> *deletedFiles,
                              cList<cCridCridFile> *deletedRecordings,
                              cList<cCridCridFile> *recordings)
{
  cList<cPathObject>  *fileList = dirScanner->MutableFileList();

  // Seperate crid-files, vdr files indicates deleting
  // and other vdr files. Save them in arrays
  cList<cPathObject> cridFiles;
  cList<cPathObject> delFiles;
  cList<cPathObject> otherFiles;

  cPathObject *pobj = fileList->First();
  while (pobj) {
        fileList->Del(pobj, false);

        const char *fileName = pobj->Name();
        const int fileNameLen = strlen(fileName);

        if (fileNameLen > 5 && !strcasecmp(fileName + fileNameLen - 5,
                                                             ".crid")) {
           cridFiles.Add(pobj);
           pobj->SetCompareLength(fileNameLen - 5);
           }
        else if (fileNameLen > 12 && !strcasecmp(fileName + fileNameLen - 12 ,
                                                           "crid.del.vdr")) {
           delFiles.Add(pobj);
           pobj->SetCompareLength(fileNameLen - 12);
           }
        else {
           otherFiles.Add(pobj);
           char tmp[fileNameLen + 1];
           memcpy(tmp, fileName, fileNameLen + 1);
           char *p = strrchr(tmp, '.');
           while (p && p - tmp >= 4 && strncasecmp(p - 4,"crid",4)) {
                 p[0] = 0;
                 p = strrchr(tmp, '.');
                 }
           if (!p || p - tmp < 4)
              return false;

           pobj->SetCompareLength(p - tmp - 4);
           }

        pobj = fileList->First();
        }

  cridFiles.Sort();
  delFiles.Sort();
  otherFiles.Sort();

  // Current path string is large enough holding
  // all full qualified filenames
  const char *curPath = dirScanner->Path();
  const int curPathLen = strlen(curPath) + 1;
  char newPath[curPathLen + NAME_MAX + 1];

  memcpy(newPath, curPath, curPathLen - 1);
  newPath[curPathLen - 1] = '/';
  newPath[curPathLen + NAME_MAX] = 0;

  // Are there recordings to delete ?
  if (delFiles.Count()) {
     // Yes! Okay we need a list of all deleted recordings
     // and a list of all other recordings to check if they
     // are intersecting.
     // File name array are sorted so we only run throught
     // the list once.
     cPathObject *delObj = delFiles.First();
     cPathObject *cridObj = cridFiles.First();
     while (delObj && cridObj) {
           const int cmp = delObj->Compare(*cridObj);

           if (cmp > 0) {
              // del file is greater than recording
              // so recordings is not deleted

              cCridCridFile *rec = new cCridCridFile(curPath,
                                            cridObj->Name());
              if (!rec) {
                 return false;
                 }

              recordings->Add(rec);
              if (!Running()) // For fast response reading may take awhile
                 return false;
              if (!rec->Read() || !rec->Playable()) {
                 dsyslog("Cannot check against Recording %s/%s ", curPath,
                                                         cridObj->Name());
                 return false;
                 }
              if (!Running()) // For fast response reading may take awhile
                 return false;

              cridObj = cridFiles.Next(cridObj);
              continue;
              }

           if (!cmp) {
              // del file is equal to recording
              // so recordings is deleted
              cCridCridFile *delRec = new cCridCridFile(curPath,
                                                cridObj->Name());
              if (!delRec) {
                 return false;
                 }

              deletedRecordings->Add(delRec);
              if (!Running()) // For fast response reading may take awhile
                 return false;
              if (!delRec->Read() || !delRec->Playable()) {
                 dsyslog("Cannot delete Recording %s/%s", curPath, cridObj->Name());
                 return false;
                 }
              if (!Running()) // For fast response reading may take awhile
                 return false;

              // The crid file is the first known deleted file of recording
              strcpy(newPath + curPathLen, cridObj->Name());
              cPathObject *pathObj = new cPathObject(newPath);
              if (!pathObj)
                 return false;

              deletedFiles->Add(pathObj);

              pathObj = cridFiles.Next(cridObj);
              cridFiles.Del(cridObj);
              cridObj = pathObj;
              // the deleted file should be deleted as well.
              // There may be only one delete file per recording
              // So fall through
              }

           // Delete file less than recording, which means
           // the corsponding recording is not present anymore
           strcpy(newPath + curPathLen, delObj->Name());
           deletedFiles->Add(new cPathObject(newPath));
           delObj = delFiles.Next(delObj);
           }

     // Handle left over delete files
     // the corsponding recording is not present anymore
     for (;delObj; delObj = delFiles.Next(delObj)) {
         strcpy(newPath + curPathLen, delObj->Name());
         deletedFiles->Add(new cPathObject(newPath));
         }

     // Handle left over recording files
     // for these is no delete file present.
     for (;cridObj; cridObj = cridFiles.Next(cridObj)) {
         cCridCridFile *rec = new cCridCridFile(curPath, cridObj->Name());
         if (!rec) {
            return false;
            }

         recordings->Add(rec);
         if (!Running()) // For fast response reading may take awhile
            return false;
         if (!rec->Read() || !rec->Playable()) {
            dsyslog("Cannot check against Recording %s/%s ", curPath,
                                                    cridObj->Name());
            return false;
            }
         if (!Running()) // For fast response reading may take awhile
            return false;
         }
     }

  // Check if there are *crid.*.vdr without recordings
  // and put them to delete list
  cPathObject *otherObj = otherFiles.First();
  cPathObject *cridObj = cridFiles.First();
  while (otherObj && cridObj) {
        const int cmp = otherObj->Compare(*cridObj);

        if (cmp > 0) {
           cridObj = cridFiles.Next(cridObj);
           continue;
           }
        if (!cmp) {
           otherObj = otherFiles.Next(otherObj);
           continue;
           }

        strcpy(newPath + curPathLen, otherObj->Name());
        deletedFiles->Add(new cPathObject(newPath));
        otherObj = otherFiles.Next(otherObj);
        }

  for (; otherObj; otherObj = otherFiles.Next(otherObj)) {
      strcpy(newPath + curPathLen, otherObj->Name());;
      deletedFiles->Add(new cPathObject(newPath));
      }

  return true;
}

bool cCridRemoveDeleted::GetDeletedFromFmpg(cList<cPathObject> *deletedFiles,
                                                     cCridFmpgFile *fmpgFile)
{
  if (!fmpgFile->Readable() || !fmpgFile->Read() || !fmpgFile->MpegCount()) {
     esyslog("Cannot read fmpg file '%s'", fmpgFile->Name());
     return false;
     }

  if (!Running()) {// For fast response reading may take awhile
     return false;
     }

  const bool CheckTimes = !fmpgFile->Unneed();
  if (CheckTimes && fmpgFile->MpegCount() <= 1) {
     dsyslog("Nothing deleted in fmpg '%s'", fmpgFile->Name());
     return true;
     }


  cPathObject *pathObj;

  int start = 0;
  int mpegCount = fmpgFile->MpegCount();
  const cCridMpegFile *mpegFiles = fmpgFile->MpegFiles();
  if (CheckTimes) {
     int32_t startTime = fmpgFile->NeedStartTime() - preGapTime;
     int32_t endTime = fmpgFile->NeedEndTime();

     int i;
     for (i = 0; i < mpegCount; i++) {
         if (mpegFiles[i].StartTimestamp() >= startTime)
            break;
         }

     i--;
     for (int j = 0; j < i; j++) {
         const int len = strlen(mpegFiles[j].Name());
         char tmpName[len + 6];

         memcpy(tmpName, mpegFiles[j].Name(), len + 1);
         pathObj = new cPathObject(tmpName);
         if (!pathObj)
            return false;

         deletedFiles->Add(pathObj);

         memcpy(tmpName + len, ".idx", 5);
         pathObj = new cPathObject(tmpName);
         if (!pathObj)
            return false;

         deletedFiles->Add(pathObj);

         memcpy(tmpName + len, ".midx", 6);
         pathObj = new cPathObject(tmpName);
         if (!pathObj)
            return false;

         deletedFiles->Add(pathObj);
         }

     start = mpegCount;
     if (endTime) {
        endTime += postGapTime;
        for (int j =  i + 1; j < mpegCount; j++) {
            if (mpegFiles[j].StartTimestamp() > endTime) {
               start = j;
               break;
               }
            }
        }
     }

  for (; start < mpegCount; start++) {
      const int len = strlen(mpegFiles[start].Name());
      char tmpName[len + 6];

      memcpy(tmpName, mpegFiles[start].Name(), len + 1);
      pathObj = new cPathObject(tmpName);
      if (!pathObj)
         return false;

      deletedFiles->Add(pathObj);

      memcpy(tmpName + len, ".idx", 5);
      pathObj = new cPathObject(tmpName);
      if (!pathObj)
         return false;

      deletedFiles->Add(pathObj);

      memcpy(tmpName + len, ".midx", 6);
      pathObj = new cPathObject(tmpName);
      if (!pathObj)
         return false;

      deletedFiles->Add(pathObj);
      }

  if (!CheckTimes) {
     pathObj = new cPathObject(fmpgFile->Name());
     if (!pathObj)
        return false;

     deletedFiles->Add(pathObj);
     if (fmpgFile->NewFormat()) {
        pathObj = new cPathObject(fmpgFile->Path(),NULL, true);
        if (!pathObj)
           return false;

        deletedFiles->Add(pathObj);
        }
     }
  return true;
}


bool cCridRemoveDeleted::GetDeletedRecordingFiles(cList<cPathObject> *deletedFiles,
                                    cList<cCridCridFile> *deletedRecordings,
                                           cList<cCridCridFile> *recordings)
{
  if (!deletedRecordings->Count()) {
     return true;
     }

  cList<cCridFmpgFile> deletedFmpgs;
  cCridCridFile *crid = deletedRecordings->First();
  while (crid) {
        const int fmpgCount = crid->FmpgCount();
        const cCridFmpgFile *fmpgFiles = crid->FmpgFiles();
        for (int i = 0; i < fmpgCount; i++) {
            cCridFmpgFile *fmpg = new cCridFmpgFile(fmpgFiles[i]);
            if (!fmpg)
               return false;
            deletedFmpgs.Add(fmpg);
            }
        deletedRecordings->Del(crid);
        crid = deletedRecordings->First();
        }

  deletedFmpgs.Sort();

  // Skip dublicate fmpgs
  for (cCridFmpgFile *f1 = deletedFmpgs.First(); f1;
                          f1 = deletedFmpgs.Next(f1)) {
      for (cCridFmpgFile *f2 = deletedFmpgs.Next(f1); f2 && !f1->Compare(*f2);
                                                f2 = deletedFmpgs.Next(f1)) {
          deletedFmpgs.Del(f2);
          }
      }

  // Check for intersection
  // Recordings may only intersect iff they have fmpgs-files in common
  if (recordings->Count()) {
     cList<cCridFmpgFile> normalFmpgs;

     crid = recordings->First();
     while (crid) {
        const int fmpgCount = crid->FmpgCount();
        const cCridFmpgFile *fmpgFiles = crid->FmpgFiles();
        for (int i = 0; i < fmpgCount; i++) {
            cCridFmpgFile *fmpg = new cCridFmpgFile(fmpgFiles[i]);
            if (!fmpg)
               return false;
            normalFmpgs.Add(fmpg);
            }
        recordings->Del(crid);
        crid = recordings->First();
        }

     normalFmpgs.Sort();

     // Don't skip dublicates cause intersection may be different
     cCridFmpgFile *fmpg = normalFmpgs.First();
     cCridFmpgFile *delFmpg = deletedFmpgs.First();
     while (fmpg && delFmpg) {
           const int cmp = delFmpg->Compare(*fmpg);

           if (cmp > 0) {
              // fmpg of normal recording does not intersect with
              // any deleted skip it
              normalFmpgs.Del(fmpg);
              fmpg = normalFmpgs.First();
              }
           else if (cmp < 0) {
              // fmpg of deleted recording does not intersect with
              // with _this_ fmpg of normal recording
              delFmpg = deletedFmpgs.Next(delFmpg);
              }
           else {
              // We have found intersection update needed time
              delFmpg->UpdateNeedTime(fmpg->StartTimestamp(),
                                      fmpg->EndTimestamp());
              // Only skip normal fmpg, deleted may intersect with
              // multiple other
              normalFmpgs.Del(fmpg);
              fmpg = normalFmpgs.First();
              }
           }
     }

  // Get all deleted files for fmpgs
  cCridFmpgFile *delFmpg = deletedFmpgs.First();
  while (delFmpg) {
        if (!GetDeletedFromFmpg(deletedFiles, delFmpg))
           return false;
        deletedFmpgs.Del(delFmpg);
        delFmpg = deletedFmpgs.First();
        }
  return true;
}

void cCridRemoveDeleted::DeleteFiles(cList<cPathObject> *deletedFiles)
{
  // Now lets really do it
  for (cPathObject *pathObj = deletedFiles->First(); pathObj; ) {
      if (pathObj->IsDir()) {
         isyslog("Cridplayer: removing directory '%s'", pathObj->Name());
#ifndef CRID_TEST_CLEANER
#error "crid cleaner nearly untested don't use it without testmode you may lose data"
         if (rmdir(pathObj->Name())) {
            if (errno != ENOENT && errno != ENOTDIR) {
               LOG_ERROR;

               if (deletedFiles != leftOverDeletedFiles) {
                  cPathObject *tmp = pathObj;
                  pathObj = deletedFiles->Next(pathObj);
                  deletedFiles->Del(tmp, false);
                  leftOverDeletedFiles->Add(tmp);
                  }
               else
                  pathObj = deletedFiles->Next(pathObj);

               continue;
               }
            }
#endif
         cPathObject *tmp = pathObj;
         pathObj = deletedFiles->Next(pathObj);
         deletedFiles->Del(tmp);
         continue;
         }

      isyslog("Cridplayer: removing file '%s'", pathObj->Name());
#ifndef CRID_TEST_CLEANER
#error "crid cleaner nearly untested don't use it without testmode you may lose data"
      if (unlink(pathObj->Name())) {
          if (errno != ENOENT) {
             LOG_ERROR;

             if (deletedFiles != leftOverDeletedFiles) {
                cPathObject *tmp = pathObj;
                pathObj = deletedFiles->Next(pathObj);
                deletedFiles->Del(tmp, false);
                leftOverDeletedFiles->Add(tmp);
                }
             else
                pathObj = deletedFiles->Next(pathObj);

             continue;
             }
         }
#endif
      cPathObject *tmp = pathObj;
      pathObj = deletedFiles->Next(pathObj);
      deletedFiles->Del(tmp);
      }
}
void cCridRemoveDeleted::Action(void)
{
  SetPriority(19);
  DeleteFiles(leftOverDeletedFiles);
  cPathObject *curPathObj = pathList.First();
  while (Running() && curPathObj){
        // dsyslog("Checking path %s", curPathObj->Name());
        if (!curPathObj->Name() || !dirScanner->ChDir(curPathObj->Name()) ||
                                                     !dirScanner->Read()) {
           dsyslog("Cannot cleanup directory %s skipping ...",
                                          curPathObj->Name());
           pathList.Del(curPathObj);
           curPathObj = pathList.First();
           continue;
           }

        HandleDirectories();
        if (!Running())
           break;
        cList<cPathObject> deletedFiles;
        cList<cCridCridFile> deletedRecordings;
        cList<cCridCridFile> recordings;
        if (!GetLists(&deletedFiles, &deletedRecordings, &recordings)) {
           dsyslog("Cannot cleanup directory %s skipping ...",
                                           curPathObj->Name());
           pathList.Del(curPathObj);
           curPathObj = pathList.First();
           continue;
           }

        if (!Running())
           break;

        if (!GetDeletedRecordingFiles(&deletedFiles, &deletedRecordings,
                                                            &recordings)) {
           dsyslog("Cannot cleanup directory %s skipping ...",
                                          curPathObj->Name());
           pathList.Del(curPathObj);
           curPathObj = pathList.First();
           continue;
           }

        if (!Running())
           break;

        DeleteFiles(&deletedFiles);
        pathList.Del(curPathObj);
        curPathObj = pathList.First();
        }
  dirScanner->Clear();
  pathList.Clear();
  lastRun = time(NULL);
}

void cCridRemoveDeleted::StartRemoveOnNeed(void)
{
  if (!NeedsRun())
     return;
  pathList.Clear();
  cPathObject *pathObj = new cPathObject(CridplayerSetup.BasePath, NULL, true);
  if (!pathObj)
     return;
  pathList.Add(pathObj);
  preGapTime = CridplayerSetup.RecordingPreGap;
  postGapTime = CridplayerSetup.RecordingPostGap;
  Start();
}

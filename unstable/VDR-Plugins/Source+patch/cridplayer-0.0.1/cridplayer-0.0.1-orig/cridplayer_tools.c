/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_tools.c 13 2007-03-06 07:09:16Z andreas $
 *
 */

#include "cridplayer_tools.h"
#include <string.h>
#include <stdlib.h>

static inline void Merge(void **oList, void **iList1, void **iList2,
           void **eList2, int (*Compare)(const void *, const void *)) {
  void **eList1 = iList2;

  while (iList1 < eList1 && iList2 < eList2) {
        const int cmp = Compare(iList1[0], iList2[0]);
        if (cmp < 0) {
           *oList++ = *iList1++;
           }
        else if (!cmp){
           *oList++ = *iList1++;
           *oList++ = *iList2++;
           }
        else {
           *oList++ = *iList2++;
           }
        }

  if (iList1 < eList1) {
     memcpy(oList, iList1, (eList1 - iList1) * sizeof(void *));
     }
  else if (iList2 < eList2) {
     memcpy(oList, iList2, (eList2 - iList2) * sizeof(void *));
     }
}

void MergeSort(void **List, int Count,
               int (*Compare)(const void *, const void *))
{
  if (Count <= 1)
     return;

  int runs[Count + 2];
  int runCount = 1;

  runs[0] = 0;

  for (int i = 1; i < Count; i++) {
      if (Compare(List[i - 1], List[i]) > 0)
         runs[runCount++] = i;
      }

  runs[runCount] = Count;
  runs[runCount + 1] = Count;

  void *tmpList[Count];
  void **oList = List;
  void **iList = tmpList;

  while (runCount > 1) {
        void **tmp = iList;
        iList = oList;
        oList = tmp;

        for (int i = 0; i < runCount; i += 2) {
            const int il1 = runs[i];
            const int il2 = runs[i + 1];
            const int el2 = runs[i + 2];
            runs[(i >> 1) + 1] = el2;
            Merge(&oList[il1], &iList[il1], &iList[il2], &iList[el2], Compare);
            }

        runCount = (runCount + 1) >> 1;
        runs[runCount + 1] = Count;
        }

  if (oList != List) {
     memcpy(List, oList, Count * sizeof(void *));
     }
}

// --- cDirScanner -----------------------------------------------------------
bool cDirScanner::ReadDir(void)
{
  Clear();
  cReadDir64 reader(curPath);
  if (!reader.Ok())
     return false;

  struct dirent64 *dirEntry;
  const int curPathLen = strlen(curPath) + 1;
  char tmpFileName[curPathLen + NAME_MAX + 1];

  memcpy(tmpFileName, curPath, curPathLen);
  tmpFileName[curPathLen - 1] = '/';


  while ((dirEntry = reader.Next())) {
        if (!readHidden) {
           if (dirEntry->d_name[0] == '.')
              continue;
           }
        else if (!strcmp(dirEntry->d_name,".") ||
                 !strcmp(dirEntry->d_name,".."))
           continue;

        memcpy(tmpFileName + curPathLen, dirEntry->d_name, NAME_MAX + 1);

        if (!FileReadable(tmpFileName))
           continue;

        struct stat64 st;
        if (!FileLStat64(tmpFileName, &st))
           continue;

        cPathObject *pobj = NULL;
        if (S_ISLNK(st.st_mode)) {
           if (!FileStat64(tmpFileName, &st))
              continue;
           if ((!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode)) ||
                (S_ISREG(st.st_mode) && fileMatch &&
                           regexec(fileMatch, dirEntry->d_name,
                                                   0, NULL, 0)))
              continue;
           char *tmp = ReadLink(tmpFileName);
           if (!tmp)
              continue;

           if (S_ISDIR(st.st_mode)) {
              const int len = strlen(basePath);
              if (strncmp(basePath, tmp, len) ||
                   (tmp[len] && tmp[len] != '/')) {
                 free(tmp);
                 continue;
                 }
              }

           pobj = new cPathObject(dirEntry->d_name, tmp, S_ISDIR(st.st_mode));
           free(tmp);
           }
        else {
           if ((!S_ISDIR(st.st_mode) && !S_ISREG(st.st_mode)) ||
                (S_ISREG(st.st_mode) && fileMatch &&
                           regexec(fileMatch, dirEntry->d_name,
                                                   0, NULL, 0)))
              continue;
           pobj = new cPathObject(dirEntry->d_name, NULL, S_ISDIR(st.st_mode));
           }

        if (!pobj) {
           esyslog("cDirScanner::ReadDir(): Cannot alloc memory");
           Clear();
           return false;
           }

        if (pobj->IsDir())
           dirList.Add(pobj);
        else
           fileList.Add(pobj);
        }

  if (sortDirs)
     dirList.Sort();

  if (strcmp(curPath,basePath)) {
     cPathObject *obj = new cPathObject("..", NULL, true);
     if (!obj) {
        esyslog("cDirScanner::ReadDir(): Cannot alloc memory");
        Clear();
        return false;
        }

     dirList.Ins(obj, dirList.First());
     }


  if (sortFiles)
     fileList.Sort();

  return true;
}


void cDirScanner::Clear(void)
{
  fileList.Clear();
  dirList.Clear();
  readOkay = false;
}

cDirScanner::cDirScanner(const char *BasePath, const char *InitPath, const char *RegexFiles, bool ReadHidden)
{
  if (Exists(BasePath)) {
     basePath = strdup(BasePath);
     }
  else {
     basePath = strdup("/");
     }

  if (Exists(InitPath)) {
     curPath = strdup(InitPath);
     }
  else {
     curPath = strdup(BasePath);
     }

 sortDirs = true;
 sortFiles = true;
 if (!RegexFiles) {
    fileMatch = NULL;
    }
 else {
    fileMatch =  MALLOC(regex_t, 1);
    if (fileMatch) {
       if (regcomp(fileMatch, RegexFiles, REG_EXTENDED | REG_ICASE | REG_NOSUB)) {
          free(fileMatch);
          fileMatch = NULL;
          esyslog("Cannot compile regular expression");
          }
       }
    else
       esyslog("Cannot alloc memory");
    }

  readHidden = ReadHidden;
  readOkay = false;
}

cDirScanner::~cDirScanner()
{
  Clear();
  if (fileMatch) {
     regfree(fileMatch);
     free(fileMatch);
     }
}

bool cDirScanner::Exists(const char *Path) const
{
  if (Path[0] == '/') {
     if (!FileReadable(Path))
        return false;

     struct stat64 st;
     if (!FileStat64(Path, &st) || !S_ISDIR(st.st_mode))
        return false;

     return true;
     }

  if (!curPath)
     return false;

  if (!strcmp(Path, "..")) {

     if (!strcmp(basePath,curPath))
        return false;

     if (!strcmp(curPath, "/"))
        return false;

     char *sep = strrchr(curPath,'/');
     if (curPath == sep)
        return true;

     if (!sep)
        return false;

     const int newPathLen = sep - curPath;
     char newPath[newPathLen + 1];
     memcpy(newPath, curPath, newPathLen);
     newPath[newPathLen] = 0;

     return Exists(newPath);
     }


  int curLen = strlen(curPath);
  const int newLen = strlen(Path);
  char newPath[curLen + 1 + newLen +1];

  if (curLen > 1)
     memcpy(newPath, curPath, curLen);
  else
     curLen = 0;
  newPath[curLen] = '/';
  memcpy(newPath + curLen + 1, Path, newLen + 1);


  return Exists(newPath);
}

bool cDirScanner::SetBasePath(const char *Path)
{
  if (Path[0] != '/' || !Exists(Path))
     return false;

  free(basePath);
  basePath = strdup(Path);
  const int len = strlen(basePath);
  if (len > 1 && (strncmp(basePath, curPath, len) ||
                 (curPath[len] && curPath[len] != '/'))) {
     free(curPath);
     curPath = strdup(basePath);
     }
  readOkay = false;
  return true;
}
bool cDirScanner::ChDir(const char *Path) {

  if (!Exists(Path))
     return false;

  if (Path[0] == '/') {
     const int len = strlen(basePath);
     if (len > 1 && (strncmp(basePath, Path, len) ||
                    (Path[len] && Path[len] != '/'))) {
        free(basePath);
        basePath = strdup("/");
        }
     free(curPath);
     curPath = strdup(Path);
     readOkay = false;
     return true;
     }

  if (!strcmp(Path, "..")) {
     char *sep = strrchr(curPath,'/');
     if (curPath == sep)
        sep++;
     *sep = 0;
     readOkay = false;
     return true;
     }

  const int curLen = strlen(curPath);
  const int newLen = strlen(Path);
  char *tmp = (char *) realloc(curPath, curLen +  1 + newLen + 1);
  if (!tmp) {
     esyslog("cDirScanner::ChDir: Cannot alloc Memory!");
     return false;
     }

  curPath = tmp;
  tmp += curLen;
  if (curLen != 1) {
     tmp[0] = '/';
     tmp++;
     }
  memcpy(tmp,Path,newLen + 1);

  readOkay = false;
  return true;
}

const char *cDirScanner::DisplayPath(void) const
{
  if (!curPath)
     return "";

  if (!strcmp(basePath,curPath))
     return "/";

  if (!strcmp(curPath,"/"))
     return "/";

  char *sep = strrchr(curPath,'/');

  if (!sep)
     return "";

  return sep + 1;
}

bool cDirScanner::Read(void)
{
  if (!ReadDir())
     return false;
  readOkay = true;
  return true;
}


// --- cReadDir64 --------------------------------------------------------------

cReadDir64::cReadDir64(const char *Directory)
{
  while (!(directory = opendir(Directory))) {
        if (FATALERRNO) {
           LOG_ERROR;
           break;
           }
        }
}

cReadDir64::~cReadDir64()
{
  if (directory)
     closedir(directory);
}

struct dirent64 *cReadDir64::Next(void)
{
  return readdir64_r(directory, &u.d, &result) == 0 ? result : NULL;
}


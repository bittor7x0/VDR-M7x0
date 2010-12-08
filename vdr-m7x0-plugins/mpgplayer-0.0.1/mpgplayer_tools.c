/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#include "mpgplayer_tools.h"
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
//              const int len = strlen(basePath);
//              if (false && strncmp(basePath, tmp, len) ||
//                   (tmp[len] && tmp[len] != '/')) {
//                 free(tmp);
//                 continue;
//                 }
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



// --- cMpgUnbufferedFile -------------------------------------------------------
//M7X0 BEGIN AK
// We have no kernel support for fadvise so disable this won't work anyway
//#define USE_FADVISE
#undef USE_FADVISE

cMpgUnbufferedFile::cMpgUnbufferedFile(void)
{
  fd = -1;
}

cMpgUnbufferedFile::~cMpgUnbufferedFile()
{
  Close();
}


int cMpgUnbufferedFile::Open(const char *FileName, int Flags, mode_t Mode)
{
  Close();
#ifdef USE_DIRECT_IO
  
  directIOused = Flags & O_DIRECT;
  if (directIOused) 
  {
     // dsyslog("directIOused==true"); 
     Flags &= ~O_NONBLOCK;
     fd = open(FileName, Flags, Mode);

     // Accordung to open man page, for direct io all user-buffers need to be
     // aligned to logical block size and read/write sizes have to multiplier
     // of blocksize. For offsets the alignment is need as well.
     if (fd >= 0) {
        struct stat64 fileStat;
        if (fstat64(fd, &fileStat) < 0) {
           LOG_ERROR;
           fileStat.st_blksize = KILOBYTE(64);
           }
        blockSize = fileStat.st_blksize;
        }
  }
  else
     fd = open(FileName, Flags, Mode);
#else
  fd = open(FileName, Flags, Mode);
#endif
  // dsyslog("cMpgUnbufferedFile::Open() fd=%d",fd); 
  curpos = 0;
#ifdef USE_FADVISE
  begin = lastpos = ahead = 0;
  cachedstart = 0;
  cachedend = 0;
  readahead = KILOBYTE(128);
  written = 0;
  totwritten = 0;
  if (fd >= 0)
     posix_fadvise(fd, 0, 0, POSIX_FADV_RANDOM); // we could use POSIX_FADV_SEQUENTIAL, but we do our own readahead, disabling the kernel one.
#endif
  return fd;
}

int cMpgUnbufferedFile::Close(void)
{
#ifdef USE_FADVISE
  if (fd >= 0) {
     if (totwritten)    // if we wrote anything make sure the data has hit the disk before
        fdatasync(fd);  // calling fadvise, as this is our last chance to un-cache it.
     posix_fadvise(fd, 0, 0, POSIX_FADV_DONTNEED);
     }
#endif

  int OldFd = fd;
  fd = -1;
  return close(OldFd);
}

// When replaying and going e.g. FF->PLAY the position jumps back 2..8M
// hence we do not want to drop recently accessed data at once.
// We try to handle the common cases such as PLAY->FF->PLAY, small
// jumps, moving editing marks etc.

#define FADVGRAN   KILOBYTE(4) // AKA fadvise-chunk-size; PAGE_SIZE or getpagesize(2) would also work.
#define READCHUNK  MEGABYTE(8)

int cMpgUnbufferedFile::FadviseDrop(off64_t Offset, off_t Len)
{
  // rounding up the window to make sure that not PAGE_SIZE-aligned data gets freed.
  return posix_fadvise(fd, Offset - (FADVGRAN - 1), Len + (FADVGRAN - 1) * 2, POSIX_FADV_DONTNEED);
}
#ifdef USE_DIRECT_IO
int cMpgUnbufferedFile::FallBackFromDirectIO(void)
{
  directIOused = false;
  int flags = fcntl(fd, F_GETFL);
  if (flags < 0)
     return flags;

  flags = flags & ~O_DIRECT;
  return fcntl(fd, F_SETFL, flags);
}
#endif

off64_t cMpgUnbufferedFile::Seek(off64_t Offset, int Whence)
{
  if (Whence == SEEK_SET && Offset == curpos)
     return curpos;
#ifdef USE_DIRECT_IO
  if (directIOused) {
     if (fd < 0) {
        errno = EINVAL;
        return -1;
        }
#ifdef CHECK_DIRECT_IO_BUFFERS
     if (Offset & (blockSize -1)) {
        esyslog("ERROR: Seek offset 0x%X not aligned to fs block size 0x%X "
                "while using direct io. Falling back ... !", Offset, blockSize);

        int r = FallBackFromDirectIO();
        if (r < 0)
           return r;

        curpos = lseek64(fd, Offset, Whence);
        return curpos;
        }
#endif
     curpos = lseek(fd, Offset, Whence);

     if (curpos < 0) {
        esyslog("ERROR: Cannot seek to offset 0x%lX fs block size 0x%X "
                "while using direct io. Falling back ... !", Offset, blockSize);

        int r = FallBackFromDirectIO();
        if (r < 0)
           return r;

        curpos = lseek64(fd, Offset, Whence);
        }

     return curpos;
     }
#endif

  curpos = lseek(fd, Offset, Whence);
  return curpos;
}

ssize_t cMpgUnbufferedFile::Read(void *Data, size_t Size)
{
  if (fd >= 0) {
#ifdef USE_FADVISE
     off64_t jumped = curpos-lastpos; // nonzero means we're not at the last offset
     if ((cachedstart < cachedend) && (curpos < cachedstart || curpos > cachedend)) {
        // current position is outside the cached window -- invalidate it.
        FadviseDrop(cachedstart, cachedend-cachedstart);
        cachedstart = curpos;
        cachedend = curpos;
        }
     cachedstart = min(cachedstart, curpos);
#endif
#ifdef USE_DIRECT_IO
     ssize_t bytesRead = 0;
     if (directIOused) {
#ifdef CHECK_DIRECT_IO_BUFFERS
        if ((((int)Data) & (blockSize - 1)) || (Size & (blockSize - 1))) {
           esyslog("ERROR: Read Data %p or Size 0x%X not aligned to fs "
               "block size 0x%X while using direct io. Falling back ... !",
                                                    Data, Size, blockSize);

           int r = FallBackFromDirectIO();
           if (r < 0)
              return r;
           }
#endif
        bytesRead = safe_read(fd, Data, Size);

        if (bytesRead < 0 && FATALERRNO) {
           dsyslog ("READ: Direct IO seems to be not supported falling back!");

           int r = FallBackFromDirectIO();
           if (r < 0)
              return r;

           bytesRead = safe_read(fd, Data, Size);
           }
        }
     else
        bytesRead = safe_read(fd, Data, Size);
#else
     ssize_t bytesRead = safe_read(fd, Data, Size);
#endif
     if (bytesRead > 0)
        curpos += bytesRead;
#ifdef USE_FADVISE
     if (bytesRead > 0) {
        //curpos += bytesRead;
        cachedend = max(cachedend, curpos);

        // Read ahead:
        // no jump? (allow small forward jump still inside readahead window).
        if (jumped >= 0 && jumped <= (off64_t)readahead) {
           // Trigger the readahead IO, but only if we've used at least
           // 1/2 of the previously requested area. This avoids calling
           // fadvise() after every read() call.
           if (ahead - curpos < (off_t)(readahead / 2)) {
              posix_fadvise(fd, curpos, readahead, POSIX_FADV_WILLNEED);
              ahead = curpos + readahead;
              cachedend = max(cachedend, ahead);
              }
           if (readahead < Size * 32) { // automagically tune readahead size.
              readahead = Size * 32;
              }
           }
        else
           ahead = curpos; // jumped -> we really don't want any readahead, otherwise e.g. fast-rewind gets in trouble.
        }

     if (cachedstart < cachedend) {
        if (curpos - cachedstart > READCHUNK * 2) {
           // current position has moved forward enough, shrink tail window.
           FadviseDrop(cachedstart, curpos - READCHUNK - cachedstart);
           cachedstart = curpos - READCHUNK;
           }
        else if (cachedend > ahead && cachedend - curpos > READCHUNK * 2) {
           // current position has moved back enough, shrink head window.
           FadviseDrop(curpos + READCHUNK, cachedend - (curpos + READCHUNK));
           cachedend = curpos + READCHUNK;
           }
        }
     lastpos = curpos;
#endif
     return bytesRead;
     }
  return -1;
}

cMpgUnbufferedFile *cMpgUnbufferedFile::Create(const char *FileName, int Flags, mode_t Mode)
{
  cMpgUnbufferedFile *File = new cMpgUnbufferedFile;
  if (File->Open(FileName, Flags, Mode) < 0) {
     delete File;
     File = NULL;
     }
  return File;
}

int CloseMpgVideoFile(cMpgUnbufferedFile *File)
{
  int Result = File->Close();
  delete File;
  return Result;
}
//M7X0 END AK

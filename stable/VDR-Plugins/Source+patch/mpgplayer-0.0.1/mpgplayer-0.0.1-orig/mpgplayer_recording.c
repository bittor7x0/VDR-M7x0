/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#include "mpgplayer_recording.h"
#include "mpgplayer_tools.h"
#include <string.h>
#include <libgen.h>
#include <vdr/tools.h>
#include <ctype.h>
#define SORTFORMAT "%s%08lX"

#define MPG_RESUME_FILE_FORMAT  "%s/%smpg.resume%s%s.vdr"
#define MPG_MARK_FORMART_STRING "%s/%smpg.marks.vdr"
#define MPG_DEL_SUFFIX "mpg.del.vdr"
#define MPG_DEL_SUFFIX_LEN 11
#define MPG_MARK_SUFFIX "mpg.marks.vdr"
#define MPG_MARK_SUFFIX_LEN 13

#define MPG_IDX_AVG_IFRAMES_PER_SEC 2
#define MPG_CATCHUP_TIME_LIMIT 5
#define MPG_CATCHUP_IDX_LIMIT 10
// --- cMpgMpegFile ---------------------------------------------------------

cMpgMpegFile::cMpgMpegFile()
{
  filename = NULL;
  pmtPid = 0;
  videoPid = 0;
  timestamp = -1;
  size = -1;
}

cMpgMpegFile::cMpgMpegFile(const char *Filename, int16_t PmtPid,
                             int16_t VideoPid, int32_t Timestamp)
{
  filename = strdup(Filename);
  pmtPid = PmtPid;
  videoPid = VideoPid;
  timestamp = Timestamp;
  size = -1;
}

void cMpgMpegFile::Set (const char *Filename, int16_t PmtPid,
                         int16_t VideoPid, int32_t Timestamp)
{
  free(filename);
  filename = strdup(Filename);
  pmtPid = PmtPid;
  videoPid = VideoPid;
  timestamp = Timestamp;
  size = -1;
}

cMpgMpegFile& cMpgMpegFile::operator= (const cMpgMpegFile &other)
{
  if (&other != this) {
     free(filename);
     filename = strdup(other.filename);
     pmtPid = other.pmtPid;
     videoPid = other.videoPid;
     timestamp = other.timestamp;
     size = other.size;
     }
  return *this;
}

bool cMpgMpegFile::Readable(void) const
{
  struct stat64 st64;
  if (FileReadable(filename) && FileStat64(filename, &st64) &&
                                      S_ISREG(st64.st_mode)) {
     size = st64.st_size;
     return true;
     }

  size = -1;
  return false;
}

// --- cMpgFmpgFile ---------------------------------------------------------
#define MPG_NEW_FORMAT_PREFIX ".rec/"
#define MPG_NEW_FORMAT_PREFIX_LEN 5
void cMpgFmpgFile::CheckAndSet(const char *Path, const char *Filename)
{
  const int pathlen = strlen(Path);
  const int filenamelen = strlen(Filename);
  char tmpPath[pathlen + 1 + MPG_NEW_FORMAT_PREFIX_LEN + (filenamelen << 1) + 2];

  memcpy(tmpPath, Path, pathlen);
  tmpPath[pathlen] = '/';
  memcpy(tmpPath + pathlen + 1, Filename, filenamelen + 1);

  struct stat64 st64;
  if (FileReadable(tmpPath) &&  FileStat64(tmpPath, &st64) &&
                                S_ISREG(st64.st_mode)) {
     path = strdup(Path);
     filename = strdup(Filename);
     fqFilename = strdup(tmpPath);
     size = 1<<8; // shelli simulate size for one entry (=files) // st.st_size;
     readable = true;
     newFormat = false;
     if (!path || !filename || !fqFilename) {
        esyslog("ERROR: Cannot alloc memory");
        readable = false;
        }
     return;
     }

  memcpy(tmpPath + pathlen + 1, MPG_NEW_FORMAT_PREFIX,
                           MPG_NEW_FORMAT_PREFIX_LEN);
  memcpy(tmpPath + pathlen + 1 + MPG_NEW_FORMAT_PREFIX_LEN, Filename,
                                                         filenamelen);
  tmpPath[pathlen + 1 + MPG_NEW_FORMAT_PREFIX_LEN + filenamelen] = '/';

  memcpy(tmpPath + pathlen + 1 + MPG_NEW_FORMAT_PREFIX_LEN + filenamelen + 1,
                                                   Filename, filenamelen + 1);

  if (FileReadable(tmpPath) &&  FileStat64(tmpPath, &st64) &&
                                S_ISREG(st64.st_mode)) {
     fqFilename = strdup(tmpPath);
     tmpPath[pathlen + 1 + MPG_NEW_FORMAT_PREFIX_LEN + filenamelen] = 0;
     path = strdup(tmpPath);
     filename = strdup(Filename);
     size = st64.st_size;
     readable = true;
     newFormat = true;
     if (!path || !filename || !fqFilename) {
        esyslog("ERROR: Cannot alloc memory");
        readable = false;
        }
     return;
     }

  readable = false;
  path = NULL;
  filename = NULL;
  fqFilename = NULL;
}

cMpgFmpgFile::cMpgFmpgFile()
{
  startTimestamp = 0;
  endTimestamp = 0;
  path = NULL;
  filename = NULL;
  fqFilename = NULL;
  mpegFiles = NULL;
  readable = false;
  needStartTimestamp = -1;
  needEndTimestamp = -1;
}

cMpgFmpgFile::cMpgFmpgFile(const cMpgFmpgFile &fmpgFile)
{
  path = NULL;
  if (fmpgFile.path)
     path = strdup(fmpgFile.path);

  filename = NULL;
  if (fmpgFile.filename)
     filename = strdup(fmpgFile.filename);

  fqFilename = NULL;
  if (fmpgFile.fqFilename)
     fqFilename = strdup(fmpgFile.fqFilename);

  startTimestamp = fmpgFile.startTimestamp;
  endTimestamp = fmpgFile.endTimestamp;
  needStartTimestamp = fmpgFile.needStartTimestamp;
  needEndTimestamp = fmpgFile.needEndTimestamp;
  readable = fmpgFile.readable;
  size = fmpgFile.size;
  newFormat = fmpgFile.newFormat;

  mpegFiles = NULL;

  const int mpegC = fmpgFile.MpegCount();
  if (mpegC) {
     mpegFiles = new cMpgMpegFile[mpegC];
     if (!mpegFiles) {
        esyslog("ERROR: Cannot alloc memory");
        return;
        }

     for (int i = 0; i < mpegC ; i ++)
         mpegFiles[i] = fmpgFile.mpegFiles[i];

     }
}

cMpgFmpgFile::cMpgFmpgFile(const char *Path, const char *Filename,
                      int32_t StartTimestamp, int32_t EndTimestamp)
{
  startTimestamp = StartTimestamp;
  endTimestamp = EndTimestamp;
  CheckAndSet(Path, Filename);
  mpegFiles = NULL;
  needStartTimestamp = -1;
  needEndTimestamp = -1;
}

cMpgFmpgFile& cMpgFmpgFile::operator= (const cMpgFmpgFile &fmpgFile)
{
  if (this != &fmpgFile) {
     free(path);
     path = NULL;
     if (fmpgFile.path)
        path = strdup(fmpgFile.path);

     free(filename);
     filename = NULL;
     if (fmpgFile.filename)
        filename = strdup(fmpgFile.filename);

     free(fqFilename);
     fqFilename = NULL;
     if (fmpgFile.fqFilename)
        fqFilename = strdup(fmpgFile.fqFilename);


     startTimestamp = fmpgFile.startTimestamp;
     endTimestamp = fmpgFile.endTimestamp;
     needStartTimestamp = fmpgFile.needStartTimestamp;
     needEndTimestamp = fmpgFile.needEndTimestamp;
     readable = fmpgFile.readable;
     size = fmpgFile.size;
     newFormat = fmpgFile.newFormat;

     delete[] mpegFiles;
     mpegFiles = NULL;

     const int mpegC = fmpgFile.MpegCount();
     if (mpegC) {
        mpegFiles = new cMpgMpegFile[mpegC];
        if (!mpegFiles) {
           esyslog("ERROR: Cannot alloc memory");
           return *this;
           }

        for (int i = 0; i < mpegC ; i ++)
            mpegFiles[i] = fmpgFile.mpegFiles[i];

        }
     }

  return *this;
}

bool cMpgFmpgFile::Set(const char *Path, const char *Filename,
                 int32_t StartTimestamp, int32_t EndTimestamp)
{
  free(path);   
  free(filename);
  free(fqFilename);
  startTimestamp = StartTimestamp;
  endTimestamp = EndTimestamp;
  CheckAndSet(Path, Filename);
  delete[] mpegFiles;
  mpegFiles = NULL;
  needStartTimestamp = -1;
  needEndTimestamp = -1;
  return readable;
}

cMpgFmpgFile::~cMpgFmpgFile()
{
  free(path);
  free(filename);
  free(fqFilename);
  delete[] mpegFiles;
}

bool cMpgFmpgFile::Read(void)
{
  struct sFmpgEntry {
    int64_t timestamp;
    int16_t pcrPid;
    int16_t videoPid;
    int16_t pmtPid;
    int16_t audioPid;
    char filename[256 - 16];
    };
  if (!readable)
     return false;
 
  const int pathlen = strlen(path) + 1;
  char tmpName[pathlen + 256];
  tmpName[pathlen + 255] = 0;
  memcpy(tmpName, path, pathlen);
  tmpName[pathlen - 1] = '/';

  mpegFiles = new cMpgMpegFile[1];

  if (!mpegFiles) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

    //TODO shelli fix pmtPid videoPid timestamp
  mpegFiles[0].Set( fqFilename,
                    0, // BE2HOST(fmpgEntry[i].pmtPid),
                    0, // BE2HOST(fmpgEntry[i].videoPid),
                    0 // (int32_t) (BE2HOST(fmpgEntry[i].timestamp) / 90000)
                  );

  return true;
}

void cMpgFmpgFile::UpdateNeedTime(int32_t start, int32_t end)
{
  if (needStartTimestamp == -1) {
     needStartTimestamp = start;
     needEndTimestamp = end;
     }
  else {
    needStartTimestamp =  min(needStartTimestamp , start);
    needEndTimestamp = needEndTimestamp && end ? max(needEndTimestamp, end) : 0;
    }
}
// --- cMpgCridFile ---------------------------------------------------------

cMpgCridFile::cMpgCridFile(const char *Path, const char *Filename)
{
  path = strdup(Path);
  readOkay = false;
  fmpgFiles = NULL;
  title = NULL;
  shortText = NULL;
  description = NULL;
  filename = strdup(Filename);

  const char *p = strrchr(Filename, '.');
  idFilenameLen = p ? p - Filename - 1 : 0;

  if (!path || !filename) {
     esyslog("ERROR: Cannot alloc memory!");
     return;
     }
}

cMpgCridFile::cMpgCridFile(const cMpgCridFile &mpgFile)
{
  path = strdup(mpgFile.path);
  filename = strdup(mpgFile.filename);
  idFilenameLen = mpgFile.idFilenameLen;
  fmpgFiles = NULL;
  title = NULL;
  shortText = NULL;
  description = NULL;

  readOkay = mpgFile.readOkay;
  if (mpgFile.title)
     title = strdup(mpgFile.title);

  if (mpgFile.shortText)
     shortText = strdup(mpgFile.shortText);

  if (mpgFile.description)
     description = strdup(mpgFile.description);


  recordState = mpgFile.recordState;
  startTimeEPG = mpgFile.startTimeEPG;
  endTimeEPG = mpgFile.endTimeEPG;
  recordType = mpgFile.recordType;
  seriesId = mpgFile.seriesId;
  protectionFlag = mpgFile.protectionFlag;

  if (mpgFile.fmpgCount) {
     fmpgFiles = new cMpgFmpgFile[mpgFile.fmpgCount];
     fmpgCount = 0;
     if (!fmpgFiles) {
        esyslog("ERROR: Cannot alloc memory");
        readOkay = false;
        return;
        }
     fmpgCount = mpgFile.fmpgCount;
     for (int i = 0; i < fmpgCount; i++) {
         fmpgFiles[i] = mpgFile.fmpgFiles[i];
         }
     }
}

cMpgCridFile::~cMpgCridFile()
{
  free(path);
  free(filename);
  delete[] fmpgFiles;
  free(title);
  free(shortText);
  free(description);
}

int cMpgCridFile::ParseFmpgInfo(const uchar *Data, int Length, int fmpgIndex)
{
  if (Length < 24)
     return -1;

  const int32_t filenameLength = BE2HOST(get_unaligned((int32_t *) Data));
  if (filenameLength + 24 > Length)
     return -1;

  char tmpFname[filenameLength + 1];
  memcpy(tmpFname, Data + 4, filenameLength);
  tmpFname[filenameLength] = 0;

  Data += filenameLength;

  int64_t tmp;
  tmp = BE2HOST(get_unaligned((int64_t *) (Data + 8)));
  tmp /= 90000;
  const int32_t startTime = (int32_t) tmp;

  tmp = BE2HOST(get_unaligned((int64_t *) (Data + 16)));
  tmp /= 90000;
  const int32_t endTime = (int32_t) tmp;

  if (!fmpgFiles[fmpgIndex].Set(path, tmpFname, startTime, endTime))
     return -1;
  return filenameLength + 24;
}

bool cMpgCridFile::Parse(char *Data, int Length)
{
  recordState = MPG_RECORD_STATE_OK;
  startTimeEPG = 0;
  endTimeEPG = 0;
  recordType = MPG_RECORD_TYPE_SINGLE_NORMAL;
  seriesId = 0;
  protectionFlag = true;

  free(title);
  title = MALLOC(char, strlen(filename)-4 + 1);
  if (!title) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  memcpy(title, filename, strlen(filename)-4);
  title[strlen(filename)-4] = 0;
  stripspace(title);

  fmpgCount = 1;    // shelli 1 crid file m740
  delete[] fmpgFiles;
  fmpgFiles = new cMpgFmpgFile[fmpgCount];
  if (!fmpgFiles) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  fmpgFiles[0].Set(path, filename, 0, 0);

  const char *tmpShortText="";  // shelli 
  int16_t tmpLength=strlen(tmpShortText);

  free(shortText);
  shortText = MALLOC(char,tmpLength + 1);
  if (!shortText) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  memcpy(shortText, tmpShortText, tmpLength);
  shortText[tmpLength] = 0;
  stripspace(shortText);

  free(description);
  description = Data;
  description[Length] = 0;
  stripspace(description);

  return true;
}

bool cMpgCridFile::Read(void)
{
  readOkay = false;
  if (!path || !filename)
     return false;

  int pathLen = strlen(path);
  int fileLen = strlen(filename);
  char fqName[pathLen + 1 + fileLen + 1];

  memcpy(fqName, path, pathLen);
  fqName[pathLen] = '/';
  memcpy(fqName + pathLen + 1, filename, fileLen + 1);

  dsyslog("Scanning %s ... ", fqName);
  struct stat64 st64;
  if (!FileStat64(fqName, &st64) || !S_ISREG(st64.st_mode)) 
  {  dsyslog("Cannot stat64 %s ... ", fqName);
     return false;
  }

  // shelli: read info file
  memcpy(fqName + pathLen  + 1 + fileLen - 3, "txt", 3);

  char *data = NULL;
  int bufLen = 0; 
  struct stat st;
  if (FileStat(fqName, &st) && S_ISREG(st.st_mode)) 
  {
        bufLen=  st.st_size;
        data=MALLOC(char, bufLen + 1);
        if (!data)
        {   esyslog("ERROR: Cannot alloc memory");
            return false;
        }

        int fd = open(fqName, O_RDONLY);
        if (fd < 0) 
        {   LOG_ERROR;
        }
        else
        {   if (bufLen != safe_read(fd, data, bufLen)) 
            {
                LOG_ERROR;
            }
            close(fd);
        }
  }
  else
  { data=MALLOC(char, 1);
    *data='\0';
  }

  if (!Parse(data,bufLen)) {
     esyslog("Cannot parse Crid");
     return false;
     }

  readOkay = true;
  return true;
}

bool cMpgCridFile::ReadFmpgs(void)
{
  if (!readOkay)
     return false;

  for (int i = 0; i < fmpgCount; i++) {
      if (!fmpgFiles[i].Read())
         return false;
      }

  return true;
}

// --- cMpgIndex ------------------------------------------------------------
void cMpgIndex::Clear(void)
{
  delete[] mpegFiles;
  mpegFiles = NULL;
  mpegFileCount = 0;
  free(index);
  index = NULL;
  lastIndex = 0;
}
#define MPG_IDX_SUFFIX ".idx"
#define MPG_IDX_SUFFIX_LEN 4
bool cMpgIndex::ReadIdxFile(int32_t timeOffset)
{
  struct sIdxEntry {
    int64_t timestamp;
    int64_t offset;
    int32_t isize;
    int32_t flags; // not used;
    };

  const int fileNo = mpegFileCount;
  const char *mpegName = mpegFiles[fileNo].Name();
  const int mpegNameLen = strlen(mpegName);
  char name[mpegNameLen + MPG_IDX_SUFFIX_LEN + 1];

  memcpy(name, mpegName, mpegNameLen);
  memcpy(name + mpegNameLen, MPG_IDX_SUFFIX, MPG_IDX_SUFFIX_LEN + 1);

  struct stat st;
  if (!FileStat(name, &st) || !S_ISREG(st.st_mode)) {
     esyslog("ERROR: cMpgIndex::ReadIdxFile(): Idx file %s not exists", name);
     return false;
     }

  int newIndexesCount = st.st_size / sizeof(sIdxEntry);
  if (newIndexesCount <= 0)
     return mpgFile->IsRunning();

  int fd = open(name, O_RDONLY);
  if (fd < 0) {
     esyslog("ERROR: cMpgIndex::ReadIdxFile(): Cannot open file %s", name);
     return false;
     }

  sIdxEntry idxEntry[newIndexesCount];

  const int size = newIndexesCount * sizeof(sIdxEntry);
  int r;
  if ((r = safe_read(fd,idxEntry,size)) < 0) {
     LOG_ERROR;
     close(fd);
     return false;
     }
  close(fd);

  if (r != size) {
     esyslog("ERROR: cMpgIndex::ReadIdxFile(): Skipped %d bytes while reading file %s", size - r, name);
     newIndexesCount = r / sizeof(sIdxEntry);
     if (newIndexesCount <= 0) {
         return false;
        }
     }

  sMpgIndex *tmp = (sMpgIndex *) realloc(index, sizeof(sMpgIndex) *
                                         (lastIndex + newIndexesCount));
  if (!tmp) {
     esyslog("ERROR: cMpgIndex::ReadIdxFile(): Cannot alloc memory");
     return false;
     }

  index = tmp;
  for (int i = 0; i < newIndexesCount; i++) {
      index[lastIndex].timestamp = ((int32_t) (BE2HOST(idxEntry[i].timestamp)
                                                       / 90000)) + timeOffset;
      index[lastIndex].offset = (int32_t) BE2HOST(idxEntry[i].offset);
      index[lastIndex].isize = BE2HOST(idxEntry[i].isize);
      index[lastIndex].fileNo = fileNo;
      lastIndex++;
      }

  return true;
}

bool cMpgIndex::Read(void)
{
  Clear();
  readOkay = false;
  indexOkay = true;
  if (!mpgFile->ReadOkay() || !mpgFile->ReadFmpgs()) {
     return false;
     }

  int tmpMpegCount = 0;
  const int fmpgCount = mpgFile->FmpgCount();
  if (!fmpgCount)
     return false;

  const cMpgFmpgFile *fmpgFiles = mpgFile->FmpgFiles();
  for (int i = 0; i < fmpgCount; i++) {
      tmpMpegCount += fmpgFiles[i].MpegCount();
      }

  mpegFiles = new cMpgMpegFile[tmpMpegCount];
  if (!mpegFiles) {
     esyslog("ERROR: Cannot alloc memory!");
     return false;
     }

  int32_t timeOffset = 0;
  for (int i = 0; i < fmpgCount; i++) {
      const int curMpegCount = fmpgFiles[i].MpegCount();
      const cMpgMpegFile *curMpegFiles = fmpgFiles[i].MpegFiles();

      for (int j = 0; j < curMpegCount; j++) {

          if (!curMpegFiles[j].Readable()) {
             esyslog("ERROR: cMpgIndex::Read(): Mpeg file %s not exists"
                                     " skipping", curMpegFiles[j].Name());
             continue;
             }

          mpegFiles[mpegFileCount] = curMpegFiles[j];
          const int32_t curTO = timeOffset + curMpegFiles[j].StartTimestamp();

          const bool idxOkay = ReadIdxFile(curTO);

          indexOkay = indexOkay && (idxOkay ||
                      curTO < fmpgFiles[i].StartTimestamp() + timeOffset ||
                      (fmpgFiles[i].EndTimestamp() &&
                       curTO > fmpgFiles[i].EndTimestamp() + timeOffset));
          mpegFileCount++;
          }

      if (i + 1 < fmpgCount) {
         const int32_t tmp = lastIndex > 0 ? index[lastIndex - 1].timestamp :
                                             timeOffset;

         if (fmpgFiles[i].EndTimestamp() > 0) {
            if (fmpgFiles[i].EndTimestamp() + timeOffset < tmp) {
               esyslog("ERROR: Can only drop end of last fmpg");
               timeOffset = tmp;
               }
            else {
               timeOffset += fmpgFiles[i].EndTimestamp();
               }
            }
         else {
            timeOffset = tmp;
            }
         }
      }

  if (!mpegFileCount) {
     esyslog("ERROR: cMpgIndex::Read(): No mpeg file found for recording!");
     Clear();
     return false;
     }

  readOkay = true;
  lastCatchup = time(NULL);
  startIndex = 0;
  preGapIndexes = 0;
  endIndex = lastIndex - 1;

  if (mpgFile->IsRunning() && endIndex <= MPG_CATCHUP_IDX_LIMIT) {
     return true;
     }

  int32_t firstTimestamp = 0;
  if (lastIndex) {
     firstTimestamp = index[0].timestamp;
     }
  int tmpStartIndex;
  int tmpPreGap;
  GetIndexTimeAbs(fmpgFiles[0].StartTimestamp() - firstTimestamp,
                                                  tmpStartIndex);
  GetIndexTimeAbs(fmpgFiles[0].StartTimestamp() - firstTimestamp -
                  MpgplayerSetup.RecordingPreGap, tmpPreGap);

  int tmpEndIndex;
  if (!mpgFile->IsRunning() && fmpgFiles[fmpgCount - 1].EndTimestamp())
     GetIndexTimeAbs(fmpgFiles[fmpgCount - 1].EndTimestamp() + timeOffset +
         MpgplayerSetup.RecordingPostGap + 1 - firstTimestamp, tmpEndIndex);
  else
     tmpEndIndex = lastIndex - 1;

  startIndex = tmpStartIndex;
  endIndex = tmpEndIndex;
  preGapIndexes = tmpStartIndex - tmpPreGap;

  return true;
}

bool cMpgIndex::CatchUp(int idx)
{
  if (mpgFile->IsRunning() && (idx > lastIndex - MPG_CATCHUP_IDX_LIMIT ||
                     time(NULL) - lastCatchup > MPG_CATCHUP_TIME_LIMIT)) {
     if (!mpgFile->Read()) {
        esyslog("ERROR: cMpgIndex::CatchUp(): Cannot reread mpg file");
        return false;
        }

     if (!mpgFile->Playable()) {
        esyslog("ERROR: cMpgIndex::CatchUp(): Recording not playable");
        return false;
        }
     readOkay = false;
     }

  if (readOkay)
     return true;

  Read();
  lastCatchup = time(NULL);
  return readOkay;
}

bool cMpgIndex::GetIndexTimeAbs(int32_t time, int &idx)
{
  if (!lastIndex) {
     idx = 0;
     return false;
     }

  if (time <= 0) {
     idx = -preGapIndexes;
     return time == 0;
     }

  CatchUp(0);
  int end = endIndex;
  int start = startIndex - preGapIndexes;

  int curIndex = time * MPG_IDX_AVG_IFRAMES_PER_SEC + start;
  time += index[start].timestamp;

  if (time >= index[end].timestamp) {
     idx = end - startIndex;
     return false;
     }

  if (curIndex >= end)
     curIndex = end - 1;

  while (start + 1 < end) {
        int32_t diff =  time - index[curIndex].timestamp;
        if (!diff) {
           start = end = curIndex;
           break;
           }

        if (diff < 0) {
           end = curIndex;
           curIndex += diff * MPG_IDX_AVG_IFRAMES_PER_SEC;
           if (curIndex <= start)
              curIndex = start + 1;
           }
        else {
           start = curIndex;
           curIndex += diff * MPG_IDX_AVG_IFRAMES_PER_SEC;
           if (curIndex >= end)
              curIndex = end - 1;
           }
        }

  curIndex = start - 1;
  while (curIndex >= startIndex - preGapIndexes &&
        index[curIndex].timestamp == index[start].timestamp) {
        start = curIndex;
        curIndex--;
        }

  if (index[start].timestamp == time) {
     idx = start - startIndex;
     return true;
     }

  end = start;
  while (end < endIndex && index[end].timestamp == index[start].timestamp)
        end++;

  if (index[end].timestamp - time < time - index[start].timestamp) {
     idx = end - startIndex;
     return true;
     }

  idx = start - startIndex;
  return true;
}

bool cMpgIndex::GetIndexTimeRel(int32_t time, int &idx)
{
  if (!lastIndex)
     return false;

  if (!time)
     return true;

  CatchUp(0);

  int curIndex = idx + startIndex;
  int end = endIndex;

  if (curIndex >= end)
     curIndex = end;

  int start = startIndex - preGapIndexes;
  if (curIndex < start)
     curIndex = start;

  if (time < 0)
     end = curIndex;
  else
     start = curIndex;


  int32_t tmp = index[curIndex].timestamp + time;
  curIndex += time * MPG_IDX_AVG_IFRAMES_PER_SEC;
  time = tmp;

  if (time >= index[endIndex].timestamp) {
     idx = endIndex - startIndex;
     return false;
     }

  if (time <= index[startIndex - preGapIndexes].timestamp) {
     idx = -preGapIndexes;
     return time == index[idx].timestamp;
     }


  if (curIndex >= end)
     curIndex = end - 1;
  if (curIndex <= start)
     curIndex = start + 1;

   while (start + 1 < end) {
        int32_t diff =  time - index[curIndex].timestamp;
        if (!diff) {
           start = end = curIndex;
           break;
           }

        if (diff < 0) {
           end = curIndex;
           curIndex += diff * MPG_IDX_AVG_IFRAMES_PER_SEC;
           if (curIndex <= start)
              curIndex = start + 1;
           }
        else {
           start = curIndex;
           curIndex += diff * MPG_IDX_AVG_IFRAMES_PER_SEC;
           if (curIndex >= end)
              curIndex = end - 1;
           }
        }

  curIndex = start - 1;
  while (curIndex >= startIndex - preGapIndexes &&
        index[curIndex].timestamp == index[start].timestamp) {
        start = curIndex;
        curIndex--;
        }

  if (index[start].timestamp == time) {
     idx = start - startIndex;
     return true;
     }

  end = start;
  while (end < endIndex && index[end].timestamp == index[start].timestamp)
        end++;

  if (index[end].timestamp - time < time - index[start].timestamp) {
     idx = end - startIndex;
     return true;
     }

  idx = start - startIndex;
  return true;
}

int cMpgIndex::GetIndex(int idx, int &FileNo, int32_t &Offset, int &Size)
{
  CatchUp(idx + startIndex);
  int curIndex = idx + startIndex;
  const int start = startIndex - preGapIndexes;

  if (curIndex < start)
     curIndex = start;

  if (curIndex > endIndex)
     curIndex = endIndex;

  int fileNo = index[curIndex].fileNo;
  int offset = index[curIndex].offset;
  int size;

  if (curIndex < lastIndex - 1 && fileNo == index[curIndex + 1].fileNo) {
     size = index[curIndex + 1].offset - offset;
     }
  else {
     size = mpegFiles[fileNo].Size() - offset;
     }

  if (curFileNo + 1 == fileNo && (!curIndex ||
                                  fileNo == index[curIndex - 1].fileNo + 1)) {
     size += offset;
     offset = 0;
     }

  FileNo = fileNo;
  Offset = offset;
  Size = size;
  return curIndex - startIndex;

}

int cMpgIndex::GetIFrameIndex(int idx, int &FileNo, int32_t &Offset, int &Size)
{
  CatchUp(idx + startIndex);
  int curIndex = idx + startIndex;
  const int start = startIndex - preGapIndexes;

  if (curIndex < start)
     curIndex = start;

  if (curIndex > endIndex)
     curIndex = endIndex;

  FileNo = index[curIndex].fileNo;
  Offset = index[curIndex].offset;
  Size = index[curIndex].isize;

  return curIndex - startIndex;
}

cMpgUnbufferedFile *cMpgIndex::GetMpegFile(int FileNo)
{
  if (FileNo == curFileNo && file)
     return file;

  if (FileNo >= mpegFileCount)
     return NULL;

  if (!file) {
     file = new cMpgUnbufferedFile();
     if (!file) {
        esyslog("ERROR: cMpgIndex::GetMpegFile(): Cannot create cMpgUnbufferedFile");
        return NULL;
        }
     }

#ifdef USE_DIRECT_IO
  if (file->Open(mpegFiles[FileNo].Name(), O_RDONLY | O_DIRECT) < 0) {
#else
  if (file->Open(mpegFiles[FileNo].Name(), O_RDONLY) < 0) {
#endif
     esyslog("ERROR: cMpgIndex::GetMpegFile(): Cannot open file %s",
                                           mpegFiles[FileNo].Name());
     return NULL;
     }

  isyslog("replay %s", mpegFiles[FileNo].Name());
  curFileNo = FileNo;
  return file;
}

bool cMpgIndex::GetPids(int FileNo, int &PmtPid, int &VideoPid)
{
  PmtPid = 0;
  VideoPid = 0;
  if (FileNo >= mpegFileCount)
     return false;
  PmtPid = mpegFiles[FileNo].PmtPid();
  VideoPid = mpegFiles[FileNo].VideoPid();
  return true;
}

int32_t cMpgIndex::GetTime(int idx)
{
  if (idx < -preGapIndexes)
     return 0;

  idx += startIndex;
  if (idx > endIndex)
     idx = endIndex;

  int start = startIndex - preGapIndexes;
  return index[idx].timestamp - index[start].timestamp;
}
// --- cMpgMark -----------------------------------------------------------------

cMpgMark::cMpgMark(int Position, int RealPosition, const char *Comment)
{
  position = Position;
  realPosition = RealPosition;
  comment = Comment ? strdup(Comment) : NULL;
}

cMpgMark::~cMpgMark()
{
  free(comment);
}

cString cMpgMark::ToText(void)
{
  char *buffer;
  asprintf(&buffer, "%d%s%s\n", realPosition, comment ? " " : "", comment ? comment : "");
  return cString(buffer, true);
}

bool cMpgMark::Parse(const char *s, cMpgIndex *Index)
{
  free(comment);
  comment = NULL;
  realPosition = strtol(s, NULL, 0);
  position = Index->GetRealIndex(realPosition);
  const char *p = strchr(s, ' ');
  if (p) {
     p = skipspace(p);
     if (*p)
        comment = strdup(p);
     }
  return true;
}

bool cMpgMark::Save(FILE *f)
{
  return fprintf(f, "%s", *ToText()) > 0;
}

// --- cMpgMarks ----------------------------------------------------------------

cMpgMarks::cMpgMarks(const char *path, const char *recFilename,
                       cMpgIndex *Index)
{
  filename = NULL;
  index = Index;
  const int pathLen = strlen(path);
  const char *p = strrchr(recFilename, '.');
  const int filenameLen = p ? p - recFilename : 0;
  filename = MALLOC(char, pathLen + 1 + filenameLen + MPG_MARK_SUFFIX_LEN + 1);

  if (!filename) {
     esyslog("ERROR: Cannot alloc memory!");
     return;
     }

  memcpy(filename, path, pathLen);
  filename[pathLen] = '/';
  memcpy(filename + pathLen + 1, recFilename, filenameLen);
  memcpy(filename + pathLen + 1 + filenameLen, MPG_MARK_SUFFIX,
                                      MPG_MARK_SUFFIX_LEN + 1);
}

bool cMpgMarks::Load()
{
  //Clear();
  if (filename && FileReadable(filename)) {
     bool result = true;
     isyslog("loading %s", filename);
     FILE *f = fopen(filename, "r");
     if (f) {
        char *s;
        int line = 0;
        cReadLine ReadLine;
        while ((s = ReadLine.Read(f)) != NULL) {
              line++;
              stripspace(s);
              if (!isempty(s)) {
                 cMpgMark *l = new cMpgMark;
                 if (l->Parse(s,index))
                    cConfig<cMark>::Add(l);
                 else {
                    esyslog("ERROR: error in %s, line %d", filename, line);
                    delete l;
                    result = false;
                    break;
                    }
                 }
              }
        }
     else {
        LOG_ERROR;
        result = false;
        }
     Sort();
     return result;
     }
  return false;
}

bool cMpgMarks::Save()
{
  bool result = true;
  cMpgMark *l = (cMpgMark *)this->First();
  if (l) {
     cSafeFile f(filename);
     if (f.Open()) {
        while (l) {
              if (!l->Save(f)) {
                 result = false;
                 break;
                 }
              l = (cMpgMark *)l->Next();
              }
        if (!f.Close())
           result = false;
        }
     else
       result = false;
     }
  return result;
}

void cMpgMarks::Sort(void)
{
  for (cMpgMark *m1 = (cMpgMark *)First(); m1; m1 = (cMpgMark *)Next(m1)) {
      for (cMpgMark *m2 = (cMpgMark *)Next(m1); m2; m2 = (cMpgMark *)Next(m2)) {

          while (m2 && m2->realPosition == m1->realPosition) {
                cMpgMark *m3 = (cMpgMark *)Next(m2);
                cConfig<cMark>::Del(m2);
                m2 = m3;
                }

          if (!m2)
             break;

          if (m2->position < m1->position) {
             swap(m1->position, m2->position);
             swap(m1->realPosition, m2->realPosition);
             swap(m1->comment, m2->comment);
             }
          }
      }
}


cMpgMark *cMpgMarks::Add(int Position)
{
  cMpgMark *m = (cMpgMark *)Get(Position);
  if (!m) {
     cConfig<cMark>::Add(m = new cMpgMark(Position, index->GetIndexFromReal(Position)));
     Sort();
     }
  return m;
}


// --- cMpgResumeFile -------------------------------------------------------

cMpgResumeFile::cMpgResumeFile(const char *Path,const char *Filename)
{
  filename = NULL;
  char *tmp = strdup(Filename);
  if (!tmp)
     return;
  char *p = strrchr(tmp,'.');
  if (!p) {
     free(tmp);
     return;
     }
  *p=0;
  if (asprintf(&filename, MPG_RESUME_FILE_FORMAT, Path, tmp,
                       Setup.ResumeID ? "." : "", Setup.ResumeID ?
                                   *itoa(Setup.ResumeID) : "") < 0) {
     esyslog("ERROR: can't allocate memory for resume file name");
     filename = NULL;
     }
  free(tmp);
}

cMpgResumeFile::~cMpgResumeFile()
{
  free(filename);
}

bool cMpgResumeFile::Read(int &Resume)
{
  int r = false;
  Resume = INT_MIN;

  if (filename) {
     struct stat st;
     if (stat(filename, &st) == 0) {
        if ((st.st_mode & S_IWUSR) == 0) // no write access, assume no resume
           return false;
        }
     int f = open(filename, O_RDONLY);
     if (f >= 0) {
        if (safe_read(f, &Resume, sizeof(Resume)) != sizeof(Resume)) {
           LOG_ERROR_STR(filename);
           }
        else {
           Resume = BE2HOST(Resume);
           r = true;
           }
        close(f);
        }
     else if (errno != ENOENT)
        LOG_ERROR_STR(filename);
     }
  return r;
}

bool cMpgResumeFile::Save(int Index)
{
  Index = HOST2BE(Index);
  if (filename) {
     int f = open(filename, O_WRONLY | O_CREAT | O_TRUNC, DEFFILEMODE);
     if (f >= 0) {
        if (safe_write(f, &Index, sizeof(Index)) < 0)
           LOG_ERROR_STR(filename);
        close(f);
        return true;
        }
     }
  return false;
}

void cMpgResumeFile::Reset(void)
{
  Save(INT_MIN);
}

// --- cMpgRecording --------------------------------------------------------

cMpgRecording::cMpgRecording(const char *Path, const char *Filename)
{
  mpgFile = new cMpgCridFile(Path,Filename);
  sortBuffer = NULL;
  event = NULL;
  resume = NULL;
}

cMpgRecording::cMpgRecording(const cMpgRecording &Rec) {
  mpgFile = new cMpgCridFile(*Rec.mpgFile);
  sortBuffer = NULL;
  event = NULL;
  resume = NULL;
}

cMpgRecording::~cMpgRecording()
{
  free(sortBuffer);
  delete event;
  delete resume;
  delete mpgFile;
}

bool cMpgRecording::ResumeOkay(void) const
{
  if (resume)
     return true;

  if (GetPath() && GetFilename())
     resume = new cMpgResumeFile(GetPath(), GetFilename());

  return resume;
}

bool cMpgRecording::EventOkay(void) const
{
  if (!event) {
     event = new cEvent(0);
     if (!event)
        return false;
     }
  event->SetTitle(Title());
  if (Title() && ShortText() && strcmp(Title(), ShortText()))
     event->SetShortText(ShortText());
  else
     event->SetShortText(NULL);

  event->SetDescription(Description());
  event->SetStartTime(StartTime());
  event->SetDuration(EndTime() - StartTime());
  return true;
}

bool cMpgRecording::Read(void)
{
  if (!mpgFile)
     return false;

  return mpgFile->Read();
}

const char *cMpgRecording::SortName(void) const
{
  if (!sortBuffer) {
     char *s;
     asprintf(&s,SORTFORMAT, Title(), StartTime());
     int l = strxfrm(NULL, s, 0) + 1;
     sortBuffer = MALLOC(char, l);
     if (!sortBuffer) {
        LOG_ERROR;
        return NULL;
        }
     strxfrm(sortBuffer, s, l);
     free(s);
     }
  return sortBuffer;
}

int cMpgRecording::Compare(const cListObject &ListObject) const
{
  const cMpgRecording *other = (cMpgRecording *) &ListObject;
  if (Series() && other->Series() && SeriesId() == other->SeriesId()) {
     return StartTime() - other->StartTime();
     }
  const char *s1 = SortName();
  const char *s2 = other->SortName();
  if (!s1)
     return -1;
  if (!s2)
     return 1;
  return strcmp(s1,s2);
}

bool cMpgRecording::IsDeleted(void) const
{
  const char *const path = GetPath();
  const char *const filename = GetFilename();
  if (!path|| !filename)
     return false;

  const int pathLen = strlen(path);
  const char *p = strrchr(filename, '.');
  if (!p)
     return false;

  const int filenameLen = p - filename;
  char tmpName[pathLen + 1 + filenameLen + MPG_DEL_SUFFIX_LEN + 1];
  memcpy(tmpName, path, pathLen);
  tmpName[pathLen] = '/';
  memcpy(tmpName + pathLen + 1, filename, filenameLen);
  memcpy(tmpName + pathLen + 1 + filenameLen, MPG_DEL_SUFFIX,
                                     MPG_DEL_SUFFIX_LEN + 1);


  struct stat st;
  if (!FileReadable(tmpName) || !FileStat(tmpName, &st) || !S_ISREG(st.st_mode))
     return false;

  return true;
}

bool cMpgRecording::Delete(void) const
{
  const char *const path = GetPath();
  const char *const filename = GetFilename();
  if (!path|| !filename)
     return false;

  const int pathLen = strlen(path);
  const char *p = strrchr(filename, '.');
  if (!p)
     return false;

  const int filenameLen = p - filename;
  char tmpName[pathLen + 1 + filenameLen + MPG_DEL_SUFFIX_LEN + 1];
  memcpy(tmpName, path, pathLen);
  tmpName[pathLen] = '/';
  memcpy(tmpName + pathLen + 1, filename, filenameLen);
  memcpy(tmpName + pathLen + 1 + filenameLen, MPG_DEL_SUFFIX,
                                     MPG_DEL_SUFFIX_LEN + 1);


  int fd = creat(tmpName, DEFFILEMODE);
  if (fd < 0) {
     esyslog("cMpgRecordings::Delete: Cannot create delete file!");
     return false;
     }

  close(fd);
  return true;
}

// --- cMpgSeriesRecording --------------------------------------------------


int cMpgSeriesRecording::Compare(const cListObject &ListObject) const
{
  const cMpgSeriesRecording *other = (cMpgSeriesRecording *) &ListObject;
  return strcoll(Title(),other->Title());
}

int cMpgSeriesRecording::NewCount(void) const
{
  int newCount = 0;
  for (cMpgRecording *rec = First(); rec;
              rec = cList<cMpgRecording>::Next(rec)) {
      if (rec->IsNew())
         newCount++;
      }
  return newCount;
}
// --- cMpgSeriesRecordings -------------------------------------------------

void cMpgSeriesRecordings::Add(cMpgRecording *rec)
{
  const int32_t seriesId = rec->SeriesId();
  cMpgSeriesRecording *curSeries = First();

  while (curSeries && curSeries->SeriesId() != seriesId)
        curSeries = Next(curSeries);

  if (!curSeries) {
     curSeries = new cMpgSeriesRecording();
     if (!curSeries) {
        esyslog("cMpgSeriesRecordings::Add: Cannot alloc Memory!");
        delete rec;
        return;
        }
     cList<cMpgSeriesRecording>::Add((cListObject *)curSeries);
     }

  curSeries->Add(rec);
}

void cMpgSeriesRecordings::Sort(void)
{
  cList<cMpgSeriesRecording>::Sort();
  for (cMpgSeriesRecording *curSeries = First(); curSeries;
                                 curSeries = Next(curSeries))
      curSeries->Sort();
}
// --- cMpgRecordings -------------------------------------------------
void cMpgRecordings::Add(cMpgRecording *rec)
{
  if (!rec) {
     esyslog("cMpgRecordings::Add(): Call with null pointer!");
     return;
     }

//   if (rec->IsDeleted()) {
//     esyslog("cMpgRecordings::Add(): Mpg '%s' is deleted skipping ...",
//                    rec->GetFilename() ? rec->GetFilename() : "<unknown>");
//     delete rec;
//     return;
//     }

  if (!rec->Read()) {
     esyslog("cMpgRecordings::Add(): Cannot read Mpg '%s'",
            rec->GetFilename() ? rec->GetFilename() : "<unknown>");
     delete rec;
     return;
     }

  if (!rec->Readable()) {
     esyslog("cMpgRecordings::Add(): Mpg '%s' not readable skipping ...",
                    rec->GetFilename() ? rec->GetFilename() : "<unknown>");
     delete rec;
     return;
     }

  if (rec->Series())
     seriesList.Add(rec);
  else
     cList<cMpgRecording>::Add(rec);
}


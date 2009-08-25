/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_recording.c 42 2007-04-30 14:25:31Z andreas $
 *
 */

#include "cridplayer_recording.h"
#include "cridplayer_tools.h"
#include <string.h>
#include <libgen.h>
#include <vdr/tools.h>
#include <ctype.h>
#define SORTFORMAT "%s%08lX"

#define CRID_RESUME_FILE_FORMAT  "%s/%scrid.resume%s%s.vdr"
#define CRID_MARK_FORMART_STRING "%s/%scrid.marks.vdr"
#define CRID_DEL_SUFFIX "crid.del.vdr"
#define CRID_DEL_SUFFIX_LEN 12
#define CRID_MARK_SUFFIX "crid.marks.vdr"
#define CRID_MARK_SUFFIX_LEN 14

#define CRID_IDX_AVG_IFRAMES_PER_SEC 2
#define CRID_CATCHUP_TIME_LIMIT 5
#define CRID_CATCHUP_IDX_LIMIT 10
// --- cCridMpegFile ---------------------------------------------------------

cCridMpegFile::cCridMpegFile()
{
  filename = NULL;
  pmtPid = 0;
  videoPid = 0;
  timestamp = -1;
  size = -1;
}

cCridMpegFile::cCridMpegFile(const char *Filename, int16_t PmtPid,
                             int16_t VideoPid, int32_t Timestamp)
{
  filename = strdup(Filename);
  pmtPid = PmtPid;
  videoPid = VideoPid;
  timestamp = Timestamp;
  size = -1;
}

void cCridMpegFile::Set (const char *Filename, int16_t PmtPid,
                         int16_t VideoPid, int32_t Timestamp)
{
  free(filename);
  filename = strdup(Filename);
  pmtPid = PmtPid;
  videoPid = VideoPid;
  timestamp = Timestamp;
  size = -1;
}

cCridMpegFile& cCridMpegFile::operator= (const cCridMpegFile &other)
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

bool cCridMpegFile::Readable(void) const
{
  struct stat st;
  if (FileReadable(filename) && FileStat(filename, &st) &&
                                      S_ISREG(st.st_mode)) {
     size = st.st_size;
     return true;
     }

  size = -1;
  return false;
}

// --- cCridFmpgFile ---------------------------------------------------------
#define CRID_NEW_FORMAT_PREFIX ".rec/"
#define CRID_NEW_FORMAT_PREFIX_LEN 5
void cCridFmpgFile::CheckAndSet(const char *Path, const char *Filename)
{
  const int pathlen = strlen(Path);
  const int filenamelen = strlen(Filename);
  char tmpPath[pathlen + 1 + CRID_NEW_FORMAT_PREFIX_LEN + (filenamelen << 1) + 2];

  memcpy(tmpPath, Path, pathlen);
  tmpPath[pathlen] = '/';
  memcpy(tmpPath + pathlen + 1, Filename, filenamelen + 1);

  struct stat st;
  if (FileReadable(tmpPath) &&  FileStat(tmpPath, &st) &&
                                S_ISREG(st.st_mode)) {
     path = strdup(Path);
     filename = strdup(Filename);
     fqFilename = strdup(tmpPath);
     size = st.st_size;
     readable = true;
     newFormat = false;
     if (!path || !filename || !fqFilename) {
        esyslog("ERROR: Cannot alloc memory");
        readable = false;
        }
     return;
     }

  memcpy(tmpPath + pathlen + 1, CRID_NEW_FORMAT_PREFIX,
                           CRID_NEW_FORMAT_PREFIX_LEN);
  memcpy(tmpPath + pathlen + 1 + CRID_NEW_FORMAT_PREFIX_LEN, Filename,
                                                         filenamelen);
  tmpPath[pathlen + 1 + CRID_NEW_FORMAT_PREFIX_LEN + filenamelen] = '/';

  memcpy(tmpPath + pathlen + 1 + CRID_NEW_FORMAT_PREFIX_LEN + filenamelen + 1,
                                                   Filename, filenamelen + 1);

  if (FileReadable(tmpPath) &&  FileStat(tmpPath, &st) &&
                                S_ISREG(st.st_mode)) {
     fqFilename = strdup(tmpPath);
     tmpPath[pathlen + 1 + CRID_NEW_FORMAT_PREFIX_LEN + filenamelen] = 0;
     path = strdup(tmpPath);
     filename = strdup(Filename);
     size = st.st_size;
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

cCridFmpgFile::cCridFmpgFile()
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

cCridFmpgFile::cCridFmpgFile(const cCridFmpgFile &fmpgFile)
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
     mpegFiles = new cCridMpegFile[mpegC];
     if (!mpegFiles) {
        esyslog("ERROR: Cannot alloc memory");
        return;
        }

     for (int i = 0; i < mpegC ; i ++)
         mpegFiles[i] = fmpgFile.mpegFiles[i];

     }
}

cCridFmpgFile::cCridFmpgFile(const char *Path, const char *Filename,
                      int32_t StartTimestamp, int32_t EndTimestamp)
{
  startTimestamp = StartTimestamp;
  endTimestamp = EndTimestamp;
  CheckAndSet(Path, Filename);
  mpegFiles = NULL;
  needStartTimestamp = -1;
  needEndTimestamp = -1;
}

cCridFmpgFile& cCridFmpgFile::operator= (const cCridFmpgFile &fmpgFile)
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
        mpegFiles = new cCridMpegFile[mpegC];
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

bool cCridFmpgFile::Set(const char *Path, const char *Filename,
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

cCridFmpgFile::~cCridFmpgFile()
{
  free(path);
  free(filename);
  free(fqFilename);
  delete[] mpegFiles;
}

bool cCridFmpgFile::Read(void)
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

  const int entriesCount = size >> 8;
  if (size & 0xFF) {
     esyslog("WARNING: Fmpg has unbound size!");
     }

  int fd = open(fqFilename, O_RDONLY);
  if (fd < 0) {
     esyslog("ERROR: Cannot open fmpg file '%s'", fqFilename);
     return false;
     }

  sFmpgEntry fmpgEntry[entriesCount];
  if (safe_read(fd, fmpgEntry, size & ~0xFF) != (size & ~0xFF)) {
     esyslog("ERROR: Cannot read fmpg file '%s'", fqFilename);
     close(fd);
     return false;
     }
  close(fd);

  const int pathlen = strlen(path) + 1;
  char tmpName[pathlen + 256];
  tmpName[pathlen + 255] = 0;
  memcpy(tmpName, path, pathlen);
  tmpName[pathlen - 1] = '/';

  mpegFiles = new cCridMpegFile[entriesCount];

  if (!mpegFiles) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  for (int i = 0; i < entriesCount; i++) {
      fmpgEntry[i].filename[256 - 16 - 1] = 0;
      char *p = strrchr(fmpgEntry[i].filename, '/');
      if (p)
         p++;
      else
         p = fmpgEntry[i].filename;
      memcpy(tmpName + pathlen, p, 256 - 16 - (p - fmpgEntry[i].filename));
      //dsyslog("mpeg %s",tmpName);
      mpegFiles[i].Set(tmpName, BE2HOST(fmpgEntry[i].pmtPid),
                                BE2HOST(fmpgEntry[i].videoPid),
                     (int32_t) (BE2HOST(fmpgEntry[i].timestamp) / 90000));

      }
  return true;
}

void cCridFmpgFile::UpdateNeedTime(int32_t start, int32_t end)
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
// --- cCridCridFile ---------------------------------------------------------

cCridCridFile::cCridCridFile(const char *Path, const char *Filename)
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

cCridCridFile::cCridCridFile(const cCridCridFile &cridFile)
{
  path = strdup(cridFile.path);
  filename = strdup(cridFile.filename);
  idFilenameLen = cridFile.idFilenameLen;
  fmpgFiles = NULL;
  title = NULL;
  shortText = NULL;
  description = NULL;

  readOkay = cridFile.readOkay;
  if (cridFile.title)
     title = strdup(cridFile.title);

  if (cridFile.shortText)
     shortText = strdup(cridFile.shortText);

  if (cridFile.description)
     description = strdup(cridFile.description);


  recordState = cridFile.recordState;
  startTimeEPG = cridFile.startTimeEPG;
  endTimeEPG = cridFile.endTimeEPG;
  recordType = cridFile.recordType;
  seriesId = cridFile.seriesId;
  protectionFlag = cridFile.protectionFlag;

  if (cridFile.fmpgCount) {
     fmpgFiles = new cCridFmpgFile[cridFile.fmpgCount];
     fmpgCount = 0;
     if (!fmpgFiles) {
        esyslog("ERROR: Cannot alloc memory");
        readOkay = false;
        return;
        }
     fmpgCount = cridFile.fmpgCount;
     for (int i = 0; i < fmpgCount; i++) {
         fmpgFiles[i] = cridFile.fmpgFiles[i];
         }
     }
}

cCridCridFile::~cCridCridFile()
{
  free(path);
  free(filename);
  delete[] fmpgFiles;
  free(title);
  free(shortText);
  free(description);
}

int cCridCridFile::ParseFmpgInfo(const uchar *Data, int Length, int fmpgIndex)
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

bool cCridCridFile::Parse(const uchar *Data, int Length)
{
  if (Length < 50)
     return false;

  recordState = BE2HOST(*((int32_t *) (Data + 12)));
  startTimeEPG = BE2HOST(*((int32_t *) (Data + 16)));
  endTimeEPG = BE2HOST(*((int32_t *) (Data + 20)));
  recordType = BE2HOST(*((int32_t *) (Data + 36)));
  seriesId = BE2HOST(*((int32_t *) (Data + 40)));
  protectionFlag = Data[45];

  int32_t tmpLength = BE2HOST(get_unaligned((int32_t *) (Data + 46)));
  int used = tmpLength + 54;
  if (Length < used)
     return false;

  Data += 50;
  while (tmpLength && (Data[0] < 0x20 || isspace(Data[0]))) {
        Data++;
        tmpLength--;
        }

  free(title);
  title = MALLOC(char, tmpLength + 1);
  if (!title) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  memcpy(title, Data, tmpLength);
  title[tmpLength] = 0;
  Data += tmpLength;
  stripspace(title);

  fmpgCount = BE2HOST(get_unaligned((int32_t *) Data));
  Data += 4;
  delete[] fmpgFiles;
  fmpgFiles = new cCridFmpgFile[fmpgCount];
  if (!fmpgFiles) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  for (int i = 0; i < fmpgCount; i++) {
      int r = ParseFmpgInfo(Data, Length - used, i);
      if (r < 0) {
         return false;
         }
      Data += r;
      used += r;
      }

  used += 4;
  if (Length < used)
     return false;

  tmpLength = BE2HOST(get_unaligned((int32_t *) Data));
  Data += 4;

  used += tmpLength;
  if (Length < used)
     return false;

  while (tmpLength && (Data[0] < 0x20 || isspace(Data[0]))) {
        Data++;
        tmpLength--;
        }

  free(shortText);
  shortText = MALLOC(char,tmpLength + 1);
  if (!shortText) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  memcpy(shortText, Data, tmpLength);
  shortText[tmpLength] = 0;
  Data += tmpLength;
  stripspace(shortText);

  used += 4;
  if (Length < used)
     return false;

  tmpLength = BE2HOST(get_unaligned((int32_t *) Data));
  Data += 4;

  used += tmpLength + 4;
  if (Length < used)
     return false;

  while (tmpLength && (Data[0] < 0x20 || isspace(Data[0]))) {
        Data++;
        tmpLength--;
        }

  free(description);
  description = MALLOC(char, tmpLength + 1);
  if (!description) {
     esyslog("ERROR: Cannot alloc memory");
     return false;
     }

  memcpy(description, Data, tmpLength);
  description[tmpLength] = 0;
  Data += tmpLength;
  stripspace(description);

  return true;
}

bool cCridCridFile::Read(void)
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

  dsyslog("Reading Crid %s ... ", fqName);
  struct stat st;
  if (!FileStat(fqName, &st) || !S_ISREG(st.st_mode)) {
     dsyslog("Cannot stat %s ... ", fqName);
     return false;
     }

  int fd = open(fqName, O_RDONLY);
  if (fd < 0) {
     LOG_ERROR;
     return false;
     }

  const int bufLen = st.st_size;
  uchar buf[bufLen];
  if (bufLen != safe_read(fd, buf, bufLen)) {
     LOG_ERROR;
     close(fd);
     return false;
     }
  close(fd);

  if (!Parse(buf,bufLen)) {
     esyslog("Cannot parse Crid");
     return false;
     }

  readOkay = true;
  return true;
}

bool cCridCridFile::ReadFmpgs(void)
{
  if (!readOkay)
     return false;

  for (int i = 0; i < fmpgCount; i++) {
      if (!fmpgFiles[i].Read())
         return false;
      }

  return true;
}

// --- cCridIndex ------------------------------------------------------------
void cCridIndex::Clear(void)
{
  delete[] mpegFiles;
  mpegFiles = NULL;
  mpegFileCount = 0;
  free(index);
  index = NULL;
  lastIndex = 0;
}
#define CRID_IDX_SUFFIX ".idx"
#define CRID_IDX_SUFFIX_LEN 4
bool cCridIndex::ReadIdxFile(int32_t timeOffset)
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
  char name[mpegNameLen + CRID_IDX_SUFFIX_LEN + 1];

  memcpy(name, mpegName, mpegNameLen);
  memcpy(name + mpegNameLen, CRID_IDX_SUFFIX, CRID_IDX_SUFFIX_LEN + 1);

  struct stat st;
  if (!FileStat(name, &st) || !S_ISREG(st.st_mode)) {
     esyslog("ERROR: cCridIndex::ReadIdxFile(): Idx file %s not exists", name);
     return false;
     }

  int newIndexesCount = st.st_size / sizeof(sIdxEntry);
  if (newIndexesCount <= 0)
     return cridFile->IsRunning();

  int fd = open(name, O_RDONLY);
  if (fd < 0) {
     esyslog("ERROR: cCridIndex::ReadIdxFile(): Cannot open file %s", name);
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
     esyslog("ERROR: cCridIndex::ReadIdxFile(): Skipped %d bytes while reading file %s", size - r, name);
     newIndexesCount = r / sizeof(sIdxEntry);
     if (newIndexesCount <= 0) {
         return false;
        }
     }

  sCridIndex *tmp = (sCridIndex *) realloc(index, sizeof(sCridIndex) *
                                         (lastIndex + newIndexesCount));
  if (!tmp) {
     esyslog("ERROR: cCridIndex::ReadIdxFile(): Cannot alloc memory");
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

bool cCridIndex::Read(void)
{
  Clear();
  readOkay = false;
  indexOkay = true;
  if (!cridFile->ReadOkay() || !cridFile->ReadFmpgs()) {
     return false;
     }

  int tmpMpegCount = 0;
  const int fmpgCount = cridFile->FmpgCount();
  if (!fmpgCount)
     return false;

  const cCridFmpgFile *fmpgFiles = cridFile->FmpgFiles();
  for (int i = 0; i < fmpgCount; i++) {
      tmpMpegCount += fmpgFiles[i].MpegCount();
      }

  mpegFiles = new cCridMpegFile[tmpMpegCount];
  if (!mpegFiles) {
     esyslog("ERROR: Cannot alloc memory!");
     return false;
     }

  int32_t timeOffset = 0;
  for (int i = 0; i < fmpgCount; i++) {
      const int curMpegCount = fmpgFiles[i].MpegCount();
      const cCridMpegFile *curMpegFiles = fmpgFiles[i].MpegFiles();

      for (int j = 0; j < curMpegCount; j++) {

          if (!curMpegFiles[j].Readable()) {
             esyslog("ERROR: cCridIndex::Read(): Mpeg file %s not exists"
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
     esyslog("ERROR: cCridIndex::Read(): No mpeg file found for recording!");
     Clear();
     return false;
     }

  readOkay = true;
  lastCatchup = time(NULL);
  startIndex = 0;
  preGapIndexes = 0;
  endIndex = lastIndex - 1;

  if (cridFile->IsRunning() && endIndex <= CRID_CATCHUP_IDX_LIMIT) {
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
                  CridplayerSetup.RecordingPreGap, tmpPreGap);

  int tmpEndIndex;
  if (!cridFile->IsRunning() && fmpgFiles[fmpgCount - 1].EndTimestamp())
     GetIndexTimeAbs(fmpgFiles[fmpgCount - 1].EndTimestamp() + timeOffset +
         CridplayerSetup.RecordingPostGap + 1 - firstTimestamp, tmpEndIndex);
  else
     tmpEndIndex = lastIndex - 1;

  startIndex = tmpStartIndex;
  endIndex = tmpEndIndex;
  preGapIndexes = tmpStartIndex - tmpPreGap;

  return true;
}

bool cCridIndex::CatchUp(int idx)
{
  if (cridFile->IsRunning() && (idx > lastIndex - CRID_CATCHUP_IDX_LIMIT ||
                     time(NULL) - lastCatchup > CRID_CATCHUP_TIME_LIMIT)) {
     if (!cridFile->Read()) {
        esyslog("ERROR: cCridIndex::CatchUp(): Cannot reread crid file");
        return false;
        }

     if (!cridFile->Playable()) {
        esyslog("ERROR: cCridIndex::CatchUp(): Recording not playable");
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

bool cCridIndex::GetIndexTimeAbs(int32_t time, int &idx)
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

  int curIndex = time * CRID_IDX_AVG_IFRAMES_PER_SEC + start;
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
           curIndex += diff * CRID_IDX_AVG_IFRAMES_PER_SEC;
           if (curIndex <= start)
              curIndex = start + 1;
           }
        else {
           start = curIndex;
           curIndex += diff * CRID_IDX_AVG_IFRAMES_PER_SEC;
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

bool cCridIndex::GetIndexTimeRel(int32_t time, int &idx)
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
  curIndex += time * CRID_IDX_AVG_IFRAMES_PER_SEC;
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
           curIndex += diff * CRID_IDX_AVG_IFRAMES_PER_SEC;
           if (curIndex <= start)
              curIndex = start + 1;
           }
        else {
           start = curIndex;
           curIndex += diff * CRID_IDX_AVG_IFRAMES_PER_SEC;
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

int cCridIndex::GetIndex(int idx, int &FileNo, int32_t &Offset, int &Size)
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

int cCridIndex::GetIFrameIndex(int idx, int &FileNo, int32_t &Offset, int &Size)
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

cUnbufferedFile *cCridIndex::GetMpegFile(int FileNo)
{
  if (FileNo == curFileNo && file)
     return file;

  if (FileNo >= mpegFileCount)
     return NULL;

  if (!file) {
     file = new cUnbufferedFile();
     if (!file) {
        esyslog("ERROR: cCridIndex::GetMpegFile(): Cannot create cUnbufferedFile");
        return NULL;
        }
     }

#ifdef USE_DIRECT_IO
  if (file->Open(mpegFiles[FileNo].Name(), O_RDONLY | O_DIRECT) < 0) {
#else
  if (file->Open(mpegFiles[FileNo].Name(), O_RDONLY) < 0) {
#endif
     esyslog("ERROR: cCridIndex::GetMpegFile(): Cannot open file %s",
                                           mpegFiles[FileNo].Name());
     return NULL;
     }

  isyslog("replay %s", mpegFiles[FileNo].Name());
  curFileNo = FileNo;
  return file;
}

bool cCridIndex::GetPids(int FileNo, int &PmtPid, int &VideoPid)
{
  PmtPid = 0;
  VideoPid = 0;
  if (FileNo >= mpegFileCount)
     return false;
  PmtPid = mpegFiles[FileNo].PmtPid();
  VideoPid = mpegFiles[FileNo].VideoPid();
  return true;
}

int32_t cCridIndex::GetTime(int idx)
{
  if (idx < -preGapIndexes)
     return 0;

  idx += startIndex;
  if (idx > endIndex)
     idx = endIndex;

  int start = startIndex - preGapIndexes;
  return index[idx].timestamp - index[start].timestamp;
}
// --- cCridMark -----------------------------------------------------------------

cCridMark::cCridMark(int Position, int RealPosition, const char *Comment)
{
  position = Position;
  realPosition = RealPosition;
  comment = Comment ? strdup(Comment) : NULL;
}

cCridMark::~cCridMark()
{
  free(comment);
}

cString cCridMark::ToText(void)
{
  char *buffer;
  asprintf(&buffer, "%d%s%s\n", realPosition, comment ? " " : "", comment ? comment : "");
  return cString(buffer, true);
}

bool cCridMark::Parse(const char *s, cCridIndex *Index)
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

bool cCridMark::Save(FILE *f)
{
  return fprintf(f, "%s", *ToText()) > 0;
}

// --- cCridMarks ----------------------------------------------------------------

cCridMarks::cCridMarks(const char *path, const char *recFilename,
                       cCridIndex *Index)
{
  filename = NULL;
  index = Index;
  const int pathLen = strlen(path);
  const char *p = strrchr(recFilename, '.');
  const int filenameLen = p ? p - recFilename : 0;
  filename = MALLOC(char, pathLen + 1 + filenameLen + CRID_MARK_SUFFIX_LEN + 1);

  if (!filename) {
     esyslog("ERROR: Cannot alloc memory!");
     return;
     }

  memcpy(filename, path, pathLen);
  filename[pathLen] = '/';
  memcpy(filename + pathLen + 1, recFilename, filenameLen);
  memcpy(filename + pathLen + 1 + filenameLen, CRID_MARK_SUFFIX,
                                      CRID_MARK_SUFFIX_LEN + 1);
}

bool cCridMarks::Load()
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
                 cCridMark *l = new cCridMark;
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

bool cCridMarks::Save()
{
  bool result = true;
  cCridMark *l = (cCridMark *)this->First();
  if (l) {
     cSafeFile f(filename);
     if (f.Open()) {
        while (l) {
              if (!l->Save(f)) {
                 result = false;
                 break;
                 }
              l = (cCridMark *)l->Next();
              }
        if (!f.Close())
           result = false;
        }
     else
       result = false;
     }
  return result;
}

void cCridMarks::Sort(void)
{
  for (cCridMark *m1 = (cCridMark *)First(); m1; m1 = (cCridMark *)Next(m1)) {
      for (cCridMark *m2 = (cCridMark *)Next(m1); m2; m2 = (cCridMark *)Next(m2)) {

          while (m2 && m2->realPosition == m1->realPosition) {
                cCridMark *m3 = (cCridMark *)Next(m2);
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


cCridMark *cCridMarks::Add(int Position)
{
  cCridMark *m = (cCridMark *)Get(Position);
  if (!m) {
     cConfig<cMark>::Add(m = new cCridMark(Position, index->GetIndexFromReal(Position)));
     Sort();
     }
  return m;
}


// --- cCridResumeFile -------------------------------------------------------

cCridResumeFile::cCridResumeFile(const char *Path,const char *Filename)
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
  if (asprintf(&filename, CRID_RESUME_FILE_FORMAT, Path, tmp,
                       Setup.ResumeID ? "." : "", Setup.ResumeID ?
                                   *itoa(Setup.ResumeID) : "") < 0) {
     esyslog("ERROR: can't allocate memory for resume file name");
     filename = NULL;
     }
  free(tmp);
}

cCridResumeFile::~cCridResumeFile()
{
  free(filename);
}

bool cCridResumeFile::Read(int &Resume)
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

bool cCridResumeFile::Save(int Index)
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

void cCridResumeFile::Reset(void)
{
  Save(INT_MIN);
}

// --- cCridRecording --------------------------------------------------------

cCridRecording::cCridRecording(const char *Path, const char *Filename)
{
  cridFile = new cCridCridFile(Path,Filename);
  sortBuffer = NULL;
  event = NULL;
  resume = NULL;
}

cCridRecording::cCridRecording(const cCridRecording &Rec) {
  cridFile = new cCridCridFile(*Rec.cridFile);
  sortBuffer = NULL;
  event = NULL;
  resume = NULL;
}

cCridRecording::~cCridRecording()
{
  free(sortBuffer);
  delete event;
  delete resume;
  delete cridFile;
}

bool cCridRecording::ResumeOkay(void) const
{
  if (resume)
     return true;

  if (GetPath() && GetFilename())
     resume = new cCridResumeFile(GetPath(), GetFilename());

  return resume;
}

bool cCridRecording::EventOkay(void) const
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

bool cCridRecording::Read(void)
{
  if (!cridFile)
     return false;

  return cridFile->Read();
}

const char *cCridRecording::SortName(void) const
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

int cCridRecording::Compare(const cListObject &ListObject) const
{
  const cCridRecording *other = (cCridRecording *) &ListObject;
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

bool cCridRecording::IsDeleted(void) const
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
  char tmpName[pathLen + 1 + filenameLen + CRID_DEL_SUFFIX_LEN + 1];
  memcpy(tmpName, path, pathLen);
  tmpName[pathLen] = '/';
  memcpy(tmpName + pathLen + 1, filename, filenameLen);
  memcpy(tmpName + pathLen + 1 + filenameLen, CRID_DEL_SUFFIX,
                                     CRID_DEL_SUFFIX_LEN + 1);


  struct stat st;
  if (!FileReadable(tmpName) || !FileStat(tmpName, &st) || !S_ISREG(st.st_mode))
     return false;

  return true;
}

bool cCridRecording::Delete(void) const
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
  char tmpName[pathLen + 1 + filenameLen + CRID_DEL_SUFFIX_LEN + 1];
  memcpy(tmpName, path, pathLen);
  tmpName[pathLen] = '/';
  memcpy(tmpName + pathLen + 1, filename, filenameLen);
  memcpy(tmpName + pathLen + 1 + filenameLen, CRID_DEL_SUFFIX,
                                     CRID_DEL_SUFFIX_LEN + 1);


  int fd = creat(tmpName, DEFFILEMODE);
  if (fd < 0) {
     esyslog("cCridRecordings::Delete: Cannot create delete file!");
     return false;
     }

  close(fd);
  return true;
}

// --- cCridSeriesRecording --------------------------------------------------


int cCridSeriesRecording::Compare(const cListObject &ListObject) const
{
  const cCridSeriesRecording *other = (cCridSeriesRecording *) &ListObject;
  return strcoll(Title(),other->Title());
}

int cCridSeriesRecording::NewCount(void) const
{
  int newCount = 0;
  for (cCridRecording *rec = First(); rec;
              rec = cList<cCridRecording>::Next(rec)) {
      if (rec->IsNew())
         newCount++;
      }
  return newCount;
}
// --- cCridSeriesRecordings -------------------------------------------------

void cCridSeriesRecordings::Add(cCridRecording *rec)
{
  const int32_t seriesId = rec->SeriesId();
  cCridSeriesRecording *curSeries = First();

  while (curSeries && curSeries->SeriesId() != seriesId)
        curSeries = Next(curSeries);

  if (!curSeries) {
     curSeries = new cCridSeriesRecording();
     if (!curSeries) {
        esyslog("cCridSeriesRecordings::Add: Cannot alloc Memory!");
        delete rec;
        return;
        }
     cList<cCridSeriesRecording>::Add((cListObject *)curSeries);
     }

  curSeries->Add(rec);
}

void cCridSeriesRecordings::Sort(void)
{
  cList<cCridSeriesRecording>::Sort();
  for (cCridSeriesRecording *curSeries = First(); curSeries;
                                 curSeries = Next(curSeries))
      curSeries->Sort();
}
// --- cCridRecordings -------------------------------------------------
void cCridRecordings::Add(cCridRecording *rec)
{
  if (!rec) {
     esyslog("cCridRecordings::Add(): Call with null pointer!");
     return;
     }

   if (rec->IsDeleted()) {
     esyslog("cCridRecordings::Add(): Crid '%s' is deleted skipping ...",
                    rec->GetFilename() ? rec->GetFilename() : "<unknown>");
     delete rec;
     return;
     }

  if (!rec->Read()) {
     esyslog("cCridRecordings::Add(): Cannot read Crid '%s'",
            rec->GetFilename() ? rec->GetFilename() : "<unknown>");
     delete rec;
     return;
     }

  if (!rec->Readable()) {
     esyslog("cCridRecordings::Add(): Crid '%s' not readable skipping ...",
                    rec->GetFilename() ? rec->GetFilename() : "<unknown>");
     delete rec;
     return;
     }

  if (rec->Series())
     seriesList.Add(rec);
  else
     cList<cCridRecording>::Add(rec);
}


/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_recording.h 13 2007-03-06 07:09:16Z andreas $
 *
 */

#ifndef __CRIDPLAYER_RECORDING_H
#define __CRIDPLAYER_RECORDING_H
#include <vdr/tools.h>
#include <vdr/epg.h>
#include <vdr/recording.h>
#include <stdlib.h>
#include "cridplayer_setup.h"


class cCridMpegFile {
private:
  char *filename;
  int16_t pmtPid;
  int16_t videoPid;
  int32_t timestamp;
  mutable int size;
public:
  inline cCridMpegFile();
  inline cCridMpegFile(const char *Filename, int16_t PmtPid, int16_t VideoPid,
                                                          int32_t Timesstamp);
  inline void Set(const char *Filename, int16_t PmtPid, int16_t VideoPid,
                                                      int32_t Timesstamp);
  ~cCridMpegFile() { free(filename); }
  inline cCridMpegFile& operator= (const cCridMpegFile &other);
  const char *Name(void) const { return filename; }
  int16_t PmtPid(void) const { return pmtPid; }
  int16_t VideoPid(void) const { return videoPid; }
  int32_t StartTimestamp(void) const { return timestamp; }
  int Size(void) const { return size; }
  bool Readable(void) const;
  };

class cCridFmpgFile : public cListObject {
private:
  char *path;
  char *filename;
  char *fqFilename;
  bool newFormat;
  int32_t startTimestamp;
  int32_t endTimestamp;
  int32_t needStartTimestamp;
  int32_t needEndTimestamp;
  int size;
  bool readable;
  void CheckAndSet(const char *Path, const char *Filename);
  cCridMpegFile *mpegFiles;
public:
  cCridFmpgFile();
  cCridFmpgFile(const cCridFmpgFile &fmpgFile);
  cCridFmpgFile(const char *Path, const char *Filename,
                       int32_t StartTimestamp, int32_t EndTimestamp);
  inline cCridFmpgFile& operator= (const cCridFmpgFile &fmpgFile);
  inline bool Set(const char *Path, const char *Filename,
                  int32_t StartTimestamp, int32_t EndTimestamp);
  virtual ~cCridFmpgFile();
  bool Readable(void) const { return readable; }
  const char *Path(void) const { return path; }
  const char *Name(void) const { return fqFilename; }
  bool NewFormat(void) const { return newFormat; }
  int32_t StartTimestamp(void) const { return startTimestamp; }
  int32_t EndTimestamp(void) const { return endTimestamp; }
  int32_t NeedStartTime(void) const { return needStartTimestamp; }
  int32_t NeedEndTime(void) const { return needEndTimestamp; }
  void UpdateNeedTime(int32_t start, int32_t end);
  bool Unneed(void) const { return needStartTimestamp == -1; }
  virtual int Compare(const cListObject &ListObject) const {
    cCridFmpgFile *fmpg = (cCridFmpgFile *)&ListObject;
    return strcmp(filename, fmpg->filename);
    }
  int MpegCount(void) const { return (mpegFiles ? size >> 8 : 0); }
  const cCridMpegFile *MpegFiles(void) const { return mpegFiles; }
  bool Read(void);
  };


#define CRID_RECORD_STATE_TIMER 1
#define CRID_RECORD_STATE_RUNNING 2
#define CRID_RECORD_STATE_OK 3
#define CRID_RECORD_STATE_ERROR 4
#define CRID_RECORD_STATE_INTERRUPTED 5
#define CRID_RECORD_STATE_CONFLICT_DELETED 8
#define CRID_RECORD_STATE_USER_DELETED 9

#define CRID_RECORD_TYPE_SINGLE_EPG 1
#define CRID_RECORD_TYPE_SINGLE_NORMAL 2
#define CRID_RECORD_TYPE_SERIES_EPG 4
#define CRID_RECORD_TYPE_SERIES_NORMAL 8
#define CRID_RECORD_TYPE_SERIES (CRID_RECORD_TYPE_SERIES_EPG | CRID_RECORD_TYPE_SERIES_NORMAL)

class cCridCridFile : public cListObject {
private:
  char *path;
  char *filename;
  int idFilenameLen;
  int32_t fmpgCount;
  cCridFmpgFile *fmpgFiles;
  int32_t recordState;
  time_t startTimeEPG;
  time_t endTimeEPG;
  int32_t recordType;
  int32_t seriesId;
  bool protectionFlag;
  char *title;
  char *shortText;
  char *description;
  bool readOkay;
  inline int ParseFmpgInfo(const uchar *Data, int Length, int fmpgIndex);
  inline bool Parse(const uchar *Data, int Length);
public:
  cCridCridFile(const char *Path, const char *Filename);
  cCridCridFile(const cCridCridFile &cridFile);
  virtual ~cCridCridFile();
  virtual int Compare(const cListObject &ListObject) const {
     cCridCridFile *crid = (cCridCridFile *)&ListObject;
     return (idFilenameLen == crid->idFilenameLen ?
             strncmp(filename,crid->filename, idFilenameLen) :
             idFilenameLen - crid->idFilenameLen);
    }
  bool Read(void);
  bool ReadFmpgs(void);
  bool ReadOkay(void) const { return readOkay; }
  bool Playable(void) const { return readOkay && recordState >= 2 &&
                                     recordState <= 5; }
  int32_t SeriesId(void) const { return seriesId; }
  time_t StartTime(void) const { return startTimeEPG; }
  time_t EndTime(void) const { return endTimeEPG; }
  const char *Title(void) const { return title; }
  const char *ShortText(void) const { return shortText; }
  const char *Description(void) const { return description; }
  int32_t State(void) const { return recordState; }
  int32_t Type(void) const { return recordType; }
  int32_t FmpgCount(void) const { return fmpgCount; }
  const cCridFmpgFile *FmpgFiles(void) { return fmpgFiles; }
  bool IsSeries(void) const { return recordType & CRID_RECORD_TYPE_SERIES; }
  bool IsRunning(void) const { return recordState == CRID_RECORD_STATE_RUNNING; }
  const char *Path(void) const { return path; }
  const char *Name(void) const { return filename; }
  };



struct sCridIndex {
  int32_t timestamp;
  int32_t offset;
  int32_t isize;
  int fileNo;
  };

class cCridIndex {
private:
  cCridCridFile *cridFile;
  cCridMpegFile *mpegFiles;
  int mpegFileCount;
  sCridIndex *index;
  int lastIndex;
  int endIndex;
  int startIndex;
  int preGapIndexes;
  bool readOkay;
  bool indexOkay;
  time_t lastCatchup;
  int curFileNo;
  cUnbufferedFile *file;
  void Clear(void);
  bool ReadIdxFile(int32_t timeOffset);
  bool CatchUp(int idx);
public:
  cCridIndex(cCridCridFile *CridFile) {
     cridFile = CridFile;
     mpegFiles = NULL;
     index = NULL;
     readOkay = false;
     indexOkay = false;
     lastCatchup = 0;
     curFileNo = -1;
     file = NULL;
     mpegFileCount = 0;
     }
  ~cCridIndex() { Clear();  delete file;}
  bool Read(void);
  bool GetIndexTimeAbs(int32_t time, int &idx);
  bool GetIndexTimeRel(int32_t time, int &idx);
  int GetIndex(int idx, int &FileNo, int32_t &Offset, int &Size);
  int GetIFrameIndex(int idx, int &FileNo, int32_t &Offset, int &Size);
  bool ReadOkay(void) { return readOkay; }
  bool IndexOkay(void) { return readOkay && indexOkay; }
  cUnbufferedFile *GetMpegFile(int FileNo);
  cUnbufferedFile *GetNextMpegFile(void) {
     return GetMpegFile(curFileNo + 1);
     }
  bool GetPids(int FileNo, int &PmtPid, int &VideoPid);
  int32_t GetTime(int idx);
  int32_t GetDuration(void) {
     return index[startIndex - preGapIndexes].timestamp -
                                index[endIndex].timestamp;
     }
  int GetMpegFileNumber(void) { return curFileNo; }
  int GetRealIndex(int idx)  { return min(max(idx + preGapIndexes, 0), endIndex - startIndex + preGapIndexes); }
  int GetIndexFromReal(int idx) { return min(max(idx - preGapIndexes, -preGapIndexes), endIndex - startIndex); }
  int Last(void) { return endIndex - startIndex; }
  };

class cCridMark : public cMark {
public:
  int realPosition;
  cCridMark(int Position = 0, int RealPosition = 0, const char *Comment = NULL);
  virtual ~cCridMark();
  cString ToText(void);
  bool Parse(const char *s, cCridIndex *Index);
  bool Save(FILE *f);
  };


class cCridMarks : public cMarks {
private:
  char *filename;
  cCridIndex *index;
public:
  cCridMarks(const char *path, const char *recFilename, cCridIndex *Index);
  virtual ~cCridMarks() { free(filename); }
  bool Load(void);
  bool Save(void);
  void Sort(void);
  cCridMark *Add(int Position);
  };


class cCridResumeFile {
private:
  char *filename;
public:
  cCridResumeFile(const char *Path,const char *Filename);
  ~cCridResumeFile();
  bool Read(int &Resume);
  bool Save(int Index);
  void Reset(void);
  };

class cCridRecording : public cListObject {
private:
  cCridCridFile *cridFile;
  mutable char *sortBuffer;
  mutable cEvent *event;
  mutable cCridResumeFile *resume;
  const char *SortName(void) const;
  bool ResumeOkay(void) const;
  bool EventOkay(void) const;
public:
  cCridRecording(const cCridRecording &Rec);
  cCridRecording(const char *Path, const char *Filename);
  virtual ~cCridRecording();
  bool Read(void);
  bool Series(void) const { return (cridFile ? cridFile->IsSeries() : false); }
  int32_t SeriesId(void) const { return (cridFile ? cridFile->SeriesId() : 0); }
  time_t StartTime(void) const { return (cridFile ? cridFile->StartTime() : 0); }
  time_t EndTime(void) const { return (cridFile ? cridFile->EndTime() : 0); }
  const char *Title(void) const { return (cridFile ? cridFile->Title() : NULL); }
  const char *ShortText(void) const { return (cridFile ? cridFile->ShortText() : NULL); }
  const char *Description(void) const { return (cridFile ? cridFile->Description() : NULL); }
  bool Readable(void) const { return (cridFile ? cridFile->Playable() : false); }
  bool IsNew (void) const { if (!ResumeOkay()) return false; int r; return !resume->Read(r) || r <= 0; }
  virtual int Compare(const cListObject &ListObject) const;
  const cEvent *Event(void) const { if (EventOkay()) return event; }
  void Rewind(void) { if (ResumeOkay()) resume->Reset(); }
  int GetResume(void) const { int r; return (ResumeOkay() && resume->Read(r) ? r : INT_MIN ); }
  bool StoreResume(int idx) const { return ResumeOkay() && resume->Save(idx); }
  cCridIndex *GetIndex(void) { return new cCridIndex(cridFile); }
  bool IsRunning(void) { return (cridFile ? cridFile->IsRunning() : false); }
  const char *GetPath(void) const { return (cridFile ? cridFile->Path() : NULL); }
  const char *GetFilename(void) const { return (cridFile ? cridFile->Name() : NULL); }
  bool IsDeleted(void) const;
  bool Delete(void) const;
  };

class cCridSeriesRecording : public cList<cCridRecording>, public cListObject {
public:
  int32_t SeriesId(void) const { if (First()) return First()->SeriesId(); return 0; }
  const char *Title(void) const { if (First()) return First()->Title(); return ""; }
  int NewCount(void) const;
  virtual int Compare(const cListObject &ListObject) const;
  };

class cCridSeriesRecordings : public cList<cCridSeriesRecording> {
public:
  void Add(cCridRecording *rec);
  void Sort(void);
  };

class cCridRecordings : public cList<cCridRecording> {
private:
  cCridSeriesRecordings seriesList;
public:
  void Add(cCridRecording *rec);
  virtual void Clear(void) { cList<cCridRecording>::Clear(); seriesList.Clear(); }
  void Sort(void) { cList<cCridRecording>::Sort(); seriesList.Sort(); }
  cCridSeriesRecordings *GetSeries(void) { return &seriesList; }
  };
#endif

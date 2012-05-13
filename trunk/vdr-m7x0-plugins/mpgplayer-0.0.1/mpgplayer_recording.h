/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#ifndef __MPGPLAYER_RECORDING_H
#define __MPGPLAYER_RECORDING_H
#include <vdr/tools.h>
#include <vdr/epg.h>
#include <vdr/recording.h>
#include <stdlib.h>
#include "mpgplayer_setup.h"
#include "mpgplayer_tools.h"


class cMpgMpegFile {
private:
  char *filename;
  int16_t pmtPid;
  int16_t videoPid;
  int32_t timestamp;
  mutable int size;
public:
  inline cMpgMpegFile();
  inline cMpgMpegFile(const char *Filename, int16_t PmtPid, int16_t VideoPid,
                                                          int32_t Timesstamp);
  inline void Set(const char *Filename, int16_t PmtPid, int16_t VideoPid,
                                                      int32_t Timesstamp);
  ~cMpgMpegFile() { free(filename); }
  inline cMpgMpegFile& operator= (const cMpgMpegFile &other);
  const char *Name(void) const { return filename; }
  int16_t PmtPid(void) const { return pmtPid; }
  int16_t VideoPid(void) const { return videoPid; }
  int32_t StartTimestamp(void) const { return timestamp; }
  int Size(void) const { return size; }
  bool Readable(void) const;
  };

class cMpgFmpgFile : public cListObject {
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
  cMpgMpegFile *mpegFiles;
public:
  cMpgFmpgFile();
  cMpgFmpgFile(const cMpgFmpgFile &fmpgFile);
  cMpgFmpgFile(const char *Path, const char *Filename,
                       int32_t StartTimestamp, int32_t EndTimestamp);
  inline cMpgFmpgFile& operator= (const cMpgFmpgFile &fmpgFile);
  inline bool Set(const char *Path, const char *Filename,
                  int32_t StartTimestamp, int32_t EndTimestamp);
  virtual ~cMpgFmpgFile();
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
    cMpgFmpgFile *fmpg = (cMpgFmpgFile *)&ListObject;
    return strcmp(filename, fmpg->filename);
    }
  int MpegCount(void) const { return (mpegFiles ? size >> 8 : 0); }
  const cMpgMpegFile *MpegFiles(void) const { return mpegFiles; }
  bool Read(void);
  };


#define MPG_RECORD_STATE_TIMER 1
#define MPG_RECORD_STATE_RUNNING 2
#define MPG_RECORD_STATE_OK 3
#define MPG_RECORD_STATE_ERROR 4
#define MPG_RECORD_STATE_INTERRUPTED 5
#define MPG_RECORD_STATE_CONFLICT_DELETED 8
#define MPG_RECORD_STATE_USER_DELETED 9

#define MPG_RECORD_TYPE_SINGLE_EPG 1
#define MPG_RECORD_TYPE_SINGLE_NORMAL 2
#define MPG_RECORD_TYPE_SERIES_EPG 4
#define MPG_RECORD_TYPE_SERIES_NORMAL 8
#define MPG_RECORD_TYPE_SERIES (MPG_RECORD_TYPE_SERIES_EPG | MPG_RECORD_TYPE_SERIES_NORMAL)

class cMpgCridFile : public cListObject {
private:
  char *path;
  char *filename;
  int idFilenameLen;
  int32_t fmpgCount;
  cMpgFmpgFile *fmpgFiles;
  int32_t recordState;
  time_t startTimeEPG;
  time_t endTimeEPG;
  int32_t recordType;
  int32_t   seriesId;
  bool protectionFlag;
  char *title;
  char *shortText;
  char *description;
  bool readOkay;
  inline int ParseFmpgInfo(const uchar *Data, int Length, int fmpgIndex);
  inline bool Parse(char *Data, int Length);
public:
  cMpgCridFile(const char *Path, const char *Filename);
  cMpgCridFile(const cMpgCridFile &mpgFile);
  virtual ~cMpgCridFile();
  virtual int Compare(const cListObject &ListObject) const {
     cMpgCridFile *mpg = (cMpgCridFile *)&ListObject;
     return (idFilenameLen == mpg->idFilenameLen ?
             strncmp(filename,mpg->filename, idFilenameLen) :
             idFilenameLen - mpg->idFilenameLen);
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
  const cMpgFmpgFile *FmpgFiles(void) { return fmpgFiles; }
  bool IsSeries(void) const { return recordType & MPG_RECORD_TYPE_SERIES; }
  bool IsRunning(void) const { return recordState == MPG_RECORD_STATE_RUNNING; }
  const char *Path(void) const { return path; }
  const char *Name(void) const { return filename; }
  };



struct sMpgIndex {
  int32_t timestamp;
  int64_t offset;
  int32_t isize;
  int fileNo;
  };

class cMpgIndex {
private:
  cMpgCridFile *mpgFile;
  cMpgMpegFile *mpegFiles;
  int mpegFileCount;
  sMpgIndex *index;
  int lastIndex;
  int endIndex;
  int startIndex;
  int preGapIndexes;
  bool readOkay;
  bool indexOkay;
  time_t lastCatchup;
  int curFileNo;
  cMpgUnbufferedFile *file;
  void Clear(void);
  bool ReadIdxFile(int32_t timeOffset);
  bool CatchUp(int idx);
public:
  cMpgIndex(cMpgCridFile *MpgFile) {
     mpgFile = MpgFile;
     mpegFiles = NULL;
     index = NULL;
     readOkay = false;
     indexOkay = false;
     lastCatchup = 0;
     curFileNo = -1;
     file = NULL;
     mpegFileCount = 0;
     }
  ~cMpgIndex() { Clear();  delete file;}
  bool Read(void);
  bool GetIndexTimeAbs(int32_t time, int &idx);
  bool GetIndexTimeRel(int32_t time, int &idx);
  int GetIndex(int idx, int &FileNo, int32_t &Offset, int &Size);
  int GetIFrameIndex(int idx, int &FileNo, int32_t &Offset, int &Size);
  bool ReadOkay(void) { return readOkay; }
  bool IndexOkay(void) { return readOkay && indexOkay; }
  cMpgUnbufferedFile *GetMpegFile(int FileNo);
  cMpgUnbufferedFile *GetNextMpegFile(void) {
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

class cMpgMark : public cMark {
public:
  int realPosition;
  cMpgMark(int Position = 0, int RealPosition = 0, const char *Comment = NULL);
  virtual ~cMpgMark();
  cString ToText(void);
  bool Parse(const char *s, cMpgIndex *Index);
  bool Save(FILE *f);
  };


class cMpgMarks : public cMarks {
private:
  char *filename;
  cMpgIndex *index;
public:
  cMpgMarks(const char *path, const char *recFilename, cMpgIndex *Index);
  virtual ~cMpgMarks() { free(filename); }
  bool Load(void);
  bool Save(void);
  void Sort(void);
  cMpgMark *Add(int Position);
  };


class cMpgResumeFile {
private:
  char *filename;
public:
  cMpgResumeFile(const char *Path,const char *Filename);
  ~cMpgResumeFile();
  bool Read(int &Resume);
  bool Save(int Index);
  void Reset(void);
  };

class cMpgRecording : public cListObject {
private:
  cMpgCridFile *mpgFile;
  mutable char *sortBuffer;
  mutable cEvent *event;
  mutable cMpgResumeFile *resume;
  const char *SortName(void) const;
  bool ResumeOkay(void) const;
  bool EventOkay(void) const;
public:
  cMpgRecording(const cMpgRecording &Rec);
  cMpgRecording(const char *Path, const char *Filename);
  virtual ~cMpgRecording();
  bool Read(void);
  bool Series(void) const { return (mpgFile ? mpgFile->IsSeries() : false); }
  int32_t SeriesId(void) const { return (mpgFile ? mpgFile->SeriesId() : 0); }
  time_t StartTime(void) const { return (mpgFile ? mpgFile->StartTime() : 0); }
  time_t EndTime(void) const { return (mpgFile ? mpgFile->EndTime() : 0); }
  const char *Title(void) const { return (mpgFile ? mpgFile->Title() : ""); }
  const char *ShortText(void) const { return (mpgFile ? mpgFile->ShortText() : NULL); }
  const char *Description(void) const { return (mpgFile ? mpgFile->Description() : NULL); }
  bool Readable(void) const { return (mpgFile ? mpgFile->Playable() : false); }
  bool IsNew (void) const { if (!ResumeOkay()) return false; int r; return !resume->Read(r) || r <= 0; }
  virtual int Compare(const cListObject &ListObject) const;
  const cEvent *Event(void) const { if (EventOkay()) return event; else return NULL; } 
  void Rewind(void) { if (ResumeOkay()) resume->Reset(); }
  int GetResume(void) const { int r; return (ResumeOkay() && resume->Read(r) ? r : INT_MIN ); }
  bool StoreResume(int idx) const { return ResumeOkay() && resume->Save(idx); }
  cMpgIndex *GetIndex(void) { return new cMpgIndex(mpgFile); }
  bool IsRunning(void) { return (mpgFile ? mpgFile->IsRunning() : false); }
  const char *GetPath(void) const { return (mpgFile ? mpgFile->Path() : NULL); }
  const char *GetFilename(void) const { return (mpgFile ? mpgFile->Name() : NULL); }
  bool IsDeleted(void) const;
  bool Delete(void) const;
  };

class cMpgSeriesRecording : public cList<cMpgRecording>, public cListObject {
public:
  int32_t SeriesId(void) const { if (First()) return First()->SeriesId(); return 0; }
  const char *Title(void) const { if (First()) return First()->Title(); return ""; }
  int NewCount(void) const;
  virtual int Compare(const cListObject &ListObject) const;
  };

class cMpgSeriesRecordings : public cList<cMpgSeriesRecording> {
public:
  void Add(cMpgRecording *rec);
  void Sort(void);
  };

class cMpgRecordings : public cList<cMpgRecording> {
private:
  cMpgSeriesRecordings seriesList;
public:
  void Add(cMpgRecording *rec);
  virtual void Clear(void) { cList<cMpgRecording>::Clear(); seriesList.Clear(); }
  void Sort(void) { cList<cMpgRecording>::Sort(); seriesList.Sort(); }
  cMpgSeriesRecordings *GetSeries(void) { return &seriesList; }
  };
#endif

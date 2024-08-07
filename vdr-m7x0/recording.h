/*
 * recording.h: Recording file handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __RECORDING_H
#define __RECORDING_H

#include <time.h>
#include "channels.h"
#include "config.h"
#include "device.h"
#include "epg.h"
#include "thread.h"
#include "timers.h"
#include "tools.h"
//M7X0 BEGIN AK
#include "ringbuffer.h"
#include <endian.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#include <byteswap.h>
#endif
//M7X0 BEGIN AK

#define FOLDERDELIMCHAR '~'
#define TIMERMACRO_TITLE    "TITLE"
#define TIMERMACRO_EPISODE  "EPISODE"

#define MAXFILESPERRECORDINGPES 255
#define RECORDFILESUFFIXPES     "/%03d.vdr"
#define MAXFILESPERRECORDINGTS  65535
#define RECORDFILESUFFIXTS      "/%05d.ts"
#define RECORDFILESUFFIXLEN 20 // some additional bytes for safety...

extern bool VfatFileSystem;

void RemoveDeletedRecordings(void);
void AssertFreeDiskSpace(int Priority = 0, bool Force = false);
     ///< The special Priority value -1 means that we shall get rid of any
     ///< deleted recordings faster than normal (because we're cutting).
     ///< If Force is true, the check will be done even if the timeout
     ///< hasn't expired yet.

class cResumeFile {
private:
  char *fileName;
  bool isPesRecording;
public:
  cResumeFile(const char *FileName, bool IsPesRecording);
  ~cResumeFile();
  int Read(void);
  bool Save(int Index);
  void Delete(void);
  };

class cRecordingInfo {
  friend class cRecording;
private:
  tChannelID channelID;
  char *channelName;
  const cEvent *event;
  cEvent *ownEvent;
  char *aux;
  int priority;
  int lifetime;
  char *fileName;
  cRecordingInfo(const cChannel *Channel = NULL, const cEvent *Event = NULL);
public:
  cRecordingInfo(const char *FileName);
  ~cRecordingInfo();
  tChannelID ChannelID(void) const { return channelID; }
  const char *ChannelName(void) const { return channelName; }
  const cEvent *GetEvent(void) const { return event; }
  const char *Title(void) const { return event->Title(); }
  const char *ShortText(void) const { return event->ShortText(); }
  const char *Description(void) const { return event->Description(); }
  const cComponents *Components(void) const { return event->Components(); }
  const char *Aux(void) const { return aux; }
  bool Read(FILE *f);
  bool Write(FILE *f, const char *Prefix = "") const;
  bool Read(void);
  bool Write(void) const;
  void SetData(const char *Title, const char *ShortText, const char *Description);
  void SetAux(const char *Aux);
  };

#define SORTRECORDINGSVERSNUM 3
#define MAXSORTMODES 4

class cRecording : public cListObject {
  friend class cRecordings;
private:
  mutable int resume;
  mutable char *titleBuffer;
  mutable char *sortBuffer[MAXSORTMODES];
  mutable char lastDirsFirst[MAXSORTMODES];
  mutable char *fileName;
  mutable char *name;
  mutable int fileSizeMB;
  int channel;
  int instanceId;
  bool isPesRecording;
  cRecordingInfo *info;
  static char *StripEpisodeName(char *s);
  char *SortName(void) const;
  int GetResume(void) const;
public:
  time_t start;
  int priority;
  int lifetime;
  time_t deleted;
  cRecording(cTimer *Timer, const cEvent *Event);
  cRecording(const char *FileName);
  virtual ~cRecording();
  virtual int Compare(const cListObject &ListObject) const;
  cString Folder(void) const;
       ///< Returns the name of the folder this recording is stored in (without the
       ///< video directory). For use in menus etc.
  cString BaseName(void) const;
       ///< Returns the base name of this recording (without the
       ///< video directory and folder). For use in menus etc.
  const char *Name(void) const { return name; }
       ///< Returns the full name of the recording (without the video directory.
       ///< For use in menus etc.
  const char *FileName(void) const;
       ///< Returns the full path name to the recording directory, including the
       ///< video directory and the actual '*.rec'. For disk file access use.
  const char *Title(char Delimiter = ' ', bool NewIndicator = false, int Level = -1, bool Original = true) const;
  cRecordingInfo *Info(void) const { return info; }
  void SetStartTime(time_t Start);
  const char *PrefixFileName(char Prefix);
  int HierarchyLevels(void) const;
  void ResetResume(void) const;
  bool IsNew(void) const { return GetResume() <= 0; }
  bool IsEdited(void) const;
  bool IsPesRecording(void) const { return isPesRecording; }
  bool WriteInfo(void);
  bool Delete(void);
       ///< Changes the file name so that it will no longer be visible in the "Recordings" menu
       ///< Returns false in case of error
  bool Remove(void);
       ///< Actually removes the file from the disk
       ///< Returns false in case of error
  bool Rename(const char *newName, int *newPriority, int *newLifetime);
       ///< Changes the file name, priority and lifetime
       ///< Returns false in case of error
  };

class cRecordings : public cList<cRecording>, public cThread {
private:
  static char *updateFileName;
  bool deleted;
  time_t lastUpdate;
  int state;
  int SortOrder;
  const char *UpdateFileName(void);
  void Refresh(bool Foreground = false);
  void ScanVideoDir(const char *DirName, bool Foreground = false, int LinkLevel = 0);
protected:
  void Action(void);
public:
  cRecordings(bool Deleted = false);
  virtual ~cRecordings();
  bool Load(void) { return Update(true); }
       ///< Loads the current list of recordings and returns true if there
       ///< is anything in it (for compatibility with older plugins - use
       ///< Update(true) instead).
  bool Update(bool Wait = false);
       ///< Triggers an update of the list of recordings, which will run
       ///< as a separate thread if Wait is false. If Wait is true, the
       ///< function returns only after the update has completed.
       ///< Returns true if Wait is true and there is anyting in the list
       ///< of recordings, false otherwise.
  void TouchUpdate(void);
       ///< Touches the '.update' file in the video directory, so that other
       ///< instances of VDR that access the same video directory can be triggered
       ///< to update their recordings list.
  bool NeedsUpdate(void);
  void ChangeState(void) { state++; }
  bool StateChanged(int &State);
  void ResetResume(const char *ResumeFileName = NULL);
  cRecording *GetByName(const char *FileName);
  void AddByName(const char *FileName, bool TriggerUpdate = true);
  void DelByName(const char *FileName);
  int TotalFileSizeMB(void); ///< Only for deleted recordings!
  void ToggleSortOrder(void) { SortOrder *= -1; }
  const int GetSortOrder(void) { return SortOrder; }
  };

extern cRecordings Recordings;
extern cRecordings DeletedRecordings;

class cMark : public cListObject {
public:
  int position;
  char *comment;
  cMark(int Position = 0, const char *Comment = NULL);
  virtual ~cMark();
  cString ToText(void);
  bool Parse(const char *s);
  bool Save(FILE *f);
  };

class cMarks : public cConfig<cMark> {
public:
  bool Load(const char *RecordingFileName, bool IsPesRecording = true);
  void Sort(void);
  cMark *Add(int Position);
  cMark *Get(int Position);
  cMark *GetPrev(int Position);
  cMark *GetNext(int Position);
  };

#define RUC_BEFORERECORDING "before"
#define RUC_AFTERRECORDING  "after"
#define RUC_EDITEDRECORDING "edited"

class cRecordingUserCommand {
private:
  static const char *command;
public:
  static void SetCommand(const char *Command) { command = Command; }
  static void InvokeCommand(const char *State, const char *RecordingFileName);
  };

//XXX+
#define FRAMESPERSEC 25

// The maximum size of a single frame (up to HDTV 1920x1080):
#define MAXFRAMESIZE  (KILOBYTE(512) / TS_SIZE * TS_SIZE) // multiple of TS_SIZE to avoid breaking up TS packets

// The maximum file size is limited by the range that can be covered
// with a 40 bit 'unsigned int', which is 1TB. The actual maximum value
// used is 6MB below the theoretical maximum, to have some safety (the
// actual file size may be slightly higher because we stop recording only
// before the next independent frame, to have a complete Group Of Pictures):
#define MAXVIDEOFILESIZETS  1048570 // MB
#define MAXVIDEOFILESIZEPES    2000 // MB
#define MINVIDEOFILESIZE          1 // MB
#define MAXVIDEOFILESIZEDEFAULT MAXVIDEOFILESIZEPES

#define MINRECORDINGSIZE      25 // GB
#define MAXRECORDINGSIZE     500 // GB
#define DEFAULTRECORDINGSIZE 100 // GB
// Dynamic recording size:
// Keep recording file size at Setup.MaxVideoFileSize for as long as possible,
// but switch to MAXVIDEOFILESIZE early enough, so that Setup.MaxRecordingSize
// will be reached, before recording to file 65535.vdr

struct __attribute__((packed)) tIndexPes {
  uint32_t offset;
  uchar type;
  uchar number;
  uint16_t reserved;
  };

struct __attribute__((packed)) tIndexTs {
  uint64_t offset:40; // up to 1TB per file (not using off_t here - must definitely be exactly 64 bit!)
  int reserved:7;     // reserved for future use
  int independent:1;  // marks frames that can be displayed by themselves (for trick modes)
  uint16_t number:16; // up to 64K files per recording
  };

class cIndexFile {
protected:
  union tIndex {
     tIndexTs ts;
     tIndexPes pes;
     void Set(bool IsPesRecording, uint64_t Offset, uchar Type, uint16_t Number) {
        if (IsPesRecording) {
           pes.offset = Offset;
           pes.type = Type;
           pes.number = Number;
           pes.reserved = 0;
           }
        else {
           ts.offset = Offset;
           ts.independent = (Type == 1);
           ts.number = Number;
           ts.reserved = 0;
           }
        }
     void SetOffset(bool IsPesRecording, uint64_t Offset) { IsPesRecording ? pes.offset = Offset : ts.offset = Offset; }
     void SetType(bool IsPesRecording, uchar Type) { IsPesRecording ? pes.type = Type : ts.independent = (Type == 1); }
     void SetNumber(bool IsPesRecording, uint16_t Number) { IsPesRecording ? pes.number = Number : ts.number = Number; }
     void SetReserved(bool IsPesRecording) { IsPesRecording ? pes.reserved = 0 : ts.reserved = 0; }
     uint64_t Offset(bool IsPesRecording) { return IsPesRecording ? pes.offset : ts.offset; }
     uchar Type(bool IsPesRecording) { return IsPesRecording ? pes.type : (ts.independent ? 1 : 2); }
     uint16_t Number(bool IsPesRecording) { return IsPesRecording ? pes.number : ts.number; }
  };
  int f;
private:
  char *fileName;
protected:
  int size, last;
  tIndex *index;
  bool isPesRecording;
private:
  cResumeFile resumeFile;
  cMutex mutex;
  static cString IndexFileName(const char *FileName, bool IsPesRecording);
protected:
virtual
  bool CatchUp(int Index = -1);
public:
  cIndexFile(const char *FileName, bool Record, bool IsPesRecording = true);
virtual
  ~cIndexFile();
  bool Ok(void) { return index != NULL; }
virtual
  bool Write(uchar PictureType, uint16_t FileNumber, off_t FileOffset);
//M7X0 BEGIN AK
virtual
  bool Write(sPesResult *Picture, int PictureCount , uint16_t FileNumber, off_t FileOffset);
virtual
  int StripOffToLastIFrame(uint16_t number);
//M7X0 END AK
virtual
  bool Get(int Index, uint16_t *FileNumber, off_t *FileOffset, uchar *PictureType = NULL, int *Length = NULL);
virtual
  int GetNextIFrame(int Index, bool Forward, uint16_t *FileNumber = NULL, off_t *FileOffset = NULL, int *Length = NULL, bool StayOffEnd = false);
virtual
  int Get(uint16_t FileNumber, off_t FileOffset);
  int Last(void) { CatchUp(); return last; }
virtual
  int GetResume(void) { return resumeFile.Read(); }
virtual
  bool StoreResume(int Index) { return resumeFile.Save(Index); }
virtual
  bool IsStillRecording(void);
  virtual cUnbufferedFile *NextFile(cFileName *FileName, bool Record);
  virtual int WaitIndex(int Index);
  static int GetLength(const char *FileName, bool IsPesRecording);
       ///< Calculates the recording length without reading the index.
  };

class cFileName {
private:
  cUnbufferedFile *file;
  uint16_t fileNumber;
  char *fileName, *pFileNumber;
  bool record;
  bool blocking;
  bool isPesRecording;
#ifdef USE_DIRECT_IO
  bool direct;
#endif
public:
#ifdef USE_DIRECT_IO
  cFileName(const char *FileName, bool Record, bool Blocking = false, bool Direct = false, bool IsPesRecording = true);
#else
  cFileName(const char *FileName, bool Record, bool Blocking = false, bool IsPesRecording = true);
#endif
  ~cFileName();
  const char *Name(void) { return fileName; }
  uint16_t Number(void) { return fileNumber; }
  bool GetLastPatPmtVersions(int &PatVersion, int &PmtVersion);
  cUnbufferedFile *Open(void);
  void Close(void);
  int Unlink(void);
  cUnbufferedFile *SetOffset(int Number, off_t Offset = 0); // yes, Number is int for easier internal calculating
  off_t MaxFileSize();
      // Dynamic file size for this file
  cUnbufferedFile *NextFile(void);
  };

cString IndexToHMSF(int Index, bool WithFrame = false);
      // Converts the given index to a string, optionally containing the frame number.
int HMSFToIndex(const char *HMSF);
      // Converts the given string (format: "hh:mm:ss.ff") to an index.
int SecondsToFrames(int Seconds); //XXX+ ->player???
      // Returns the number of frames corresponding to the given number of seconds.

int ReadFrame(cUnbufferedFile *f, uchar *b, int Length, int Max);

char *ExchangeChars(char *s, bool ToFileSystem);
      // Exchanges the characters in the given string to or from a file system
      // specific representation (depending on ToFileSystem). The given string will
      // be modified and may be reallocated if more space is needed. The return
      // value points to the resulting string, which may be different from s.

#endif //__RECORDING_H

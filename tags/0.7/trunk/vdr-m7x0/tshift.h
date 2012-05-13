/*
 * tshift.h: TShift mode
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __TSHIFT_H
#define __TSHIFT_H

#include "player.h"

#define MAXTSHIFT (MAXDEVICES * MAXRECEIVERS)
#define MAXFILESPERRECORDING 255
#define RECORDFILESUFFIX    "/%03d.vdr"
#define RECORDFILESUFFIXLEN 20 // some additional bytes for safety...
#define INDEXFILESUFFIX   "/index.vdr"
#define REMOVEOLDFILESAFETYLIMIT 50 // delete old files
#define MINFREEDISK       300 // minimum free disk space (in MB) required to start recording

enum TShiftPlayerMode{TShiftPlayerLive,TShiftPlayerDelay,TShiftPlayerPlay,TShiftPlayerPause};


class cTShiftRecorder;
class cTShiftTransferControl;

class cTShiftControl : public cControl {
private:
  static cTShiftRecorder *receivers[MAXTSHIFT];
  static cMutex controlLock;
  static cTShiftControl *currentControl;
  static time_t LastProcess;
  static bool nothingToDo;
  cTShiftTransferControl *controlTransfer;
  cTShiftRecorder *controlRecorder;
  int channelNumber;
  bool StartRecord(void);
  bool IsForwarding;
  int IsRecording(void);
  void StopRecord(void);
  bool StartTransfer(void);
  bool StartPlayer(void);
  bool ProcessKeyTShift(eKeys Key,int *LastChannel,cOsdObject *Menu);
  bool OnlyProcessKey(eKeys Key,int *LastChannel=NULL);
  time_t ControlLastProcess;
  time_t startRecorder;
  time_t beginChannel;
  time_t lastActivated;
  bool IsDelayed(void);
  bool IsTransfering(void);
  void StopChannel(void);

  bool Play(void);
  bool Pause(void);
  bool Delay(void);
  bool Stop(void);
  bool Forward(void);
  bool Backward(void);
  bool Forget(void);

public:
  cTShiftControl(cDevice *ReceiverDevice, const cChannel *Channel);
  virtual ~cTShiftControl();
  virtual void Hide(void){}
  static void InitializeTShift(void);
  static void ShutdownTShift(void);
  static void StartTShift(void);
  static eKeys FilterKey(eKeys Key,int *LastChannel,cOsdObject *Menu);
  static eKeys FilterKeyFromMenu(eKeys Key);
  static int GetTShiftInfo(int Channel,TShiftPlayerMode *Mode=NULL,int *Recording=NULL);
  static bool BufferInfo(int CurrentChannel,int *Total,int *Current);
  static bool ReplayInfo(int CurrentChannel,bool *Play,bool *Forward,int *Speed);
  static bool Impact(cDevice *Device,bool NeedDetach);
  };

class cTShiftIndexFile : public cIndexFile {
	int resume;
	int current;
	cMutex currentMutex;
	int first;
	cMutex firstMutex;
	char *contentFileName;
	char *contentFileNumber;
	int files[MAXFILESPERRECORDING];
	uchar nextFileToDelete;
	void RemoveOldFiles(int First);
	void SetFirst(int First);
	int maxSize;
	int lastFile;
	cMutex indexLock;
	bool ReadIndex(void);
	void WriteIndex(void);
	bool Grow(int needed,int FileNumber);
	int GetNextCurrent(int Current);
	int GetCurrent(int Current);
	void UnlockCurrent();
	void IncFirst(int Inc);
public:
	cUnbufferedFile *GetReplayFile(cFileName *FileName);
	cTShiftIndexFile *ActivatePlayer(int ResumeIndex);
	void DeactivatePlayer(void);
	int First(void){return first;}
protected:
	virtual bool CatchUp(int Index=-1){return index;}
public:
	cTShiftIndexFile(const char *FileName);
	virtual ~cTShiftIndexFile();
	virtual bool Write(uchar PictureType,uchar FileNumber,int FileOffset);
	virtual bool Write(sPesResult *Picture,int PictureCount,uchar FileNumber,int FileOffset);
	virtual int StripOffToLastIFrame(int number){return -1;}
	virtual bool Get(int Index,uchar *FileNumber,int *FileOffset,uchar *PictureType=NULL,int *Length=NULL);
	virtual int GetNextIFrame(int Index,bool Forward,uchar *FileNumber=NULL,int *FileOffset=NULL,int *Length=NULL,bool StayOffEnd=false);
	virtual int Get(uchar FileNumber,int FileOffset);
	virtual int GetResume(void);
	virtual bool StoreResume(int Index);
	virtual cUnbufferedFile *NextFile(cFileName *FileName,bool Record);
	virtual bool IsStillRecording(void){ return false;}
	virtual int WaitIndex(int Index);
	bool IsFileOK(void){return (f>=0); }
};


#endif //__TSHIFT_H

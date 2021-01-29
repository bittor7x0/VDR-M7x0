/*
 * tshift.c: Advanced timeshift by Posix.
 *
 * See the main source file 'vdr.c' for copyright information.
 * https://vdr-m7x0.foroactivo.com.es
 *
 */

#include <sys/ioctl.h>
#include <limits.h>
#include "tshift.h"
#include "m7x0_dvb/dmx_ext.h"
#include "videodir.h"
#include "recorder.h"
#include "recording.h"
#include "dvbplayer.h"
#include "transfer.h"
#include "remote.h"

extern const char *VideoDirectory;
#define MAXTSHIFTFILES (MAXTSHIFT+1)
#define BASEPRIORITY (-MAXPRIORITY)

class cTShiftFileName
{
	enum fileState{fileUnknown,fileDeleting,fileFree,fileInUse};
	static fileState files[MAXTSHIFTFILES];
public:
	static cString makeFileName(int Number);
	static cString newFileName(int *Number);
	static bool FreeFiles(bool Force);
	static void InitializeTShift(void);
	static void FreeFile(int Number);
};

cTShiftFileName::fileState cTShiftFileName::files[MAXTSHIFTFILES];

cString cTShiftFileName::newFileName(int *Number)
{
	int newDelete=-1;
	int f;
	for(f=0;f<MAXTSHIFTFILES;f++)
	{
		if(files[f]==fileFree)
			break;
		if((files[f]<fileFree)&&(newDelete<0))
			newDelete=f;
	}
	if((f==MAXTSHIFTFILES)&&(newDelete>=0))
		f=newDelete;
	cString result(makeFileName(f));
	if(f<MAXTSHIFTFILES)
	{
		if(files[f]<fileFree)		
			RemoveVideoFile(result);
		files[f]=fileInUse;
	}
	MakeDirs(result,true);
	*Number=f;
	return result;
}

cString cTShiftFileName::makeFileName(int Number)
{
	return cString::sprintf("%s/timeShift/%d",VideoDirectory,Number);
}

bool cTShiftFileName::FreeFiles(bool Force)
{
	bool result=false;
	for(int f=0;f<MAXTSHIFTFILES;f++)
		if(files[f]<fileFree)
		{
			RemoveVideoFile(makeFileName(f));
			files[f]=fileFree;
			if(!Force)
				return true;
			else
				result=true;
		}
	return result;
}


void cTShiftFileName::FreeFile(int Number)
{
	if(Number<MAXTSHIFTFILES)
		files[Number]=fileDeleting;
	else
		RemoveVideoFile(makeFileName(Number));
}

void cTShiftFileName::InitializeTShift()
{
	for(int f=0;f<MAXTSHIFTFILES;f++)
		files[f]=fileUnknown;
}

class cTShiftPlayerControl;

class cTShiftData
{
private:
	time_t zapChannel;
	TShiftPlayerMode mode;
	time_t lastChannel;
	int index;
	time_t GetZapChannel(void);
public:
	cTShiftData(time_t LastChannel);
	void BeginZapping(void);
	bool IsZapping(void);
	void ForgetZapping(void);
	time_t GetLastChannel(void);
	int GetIndex(void);
	TShiftPlayerMode GetMode(void);
	void SetMode(cTShiftPlayerControl *player);
	void SetMode(TShiftPlayerMode Mode,int Index=-1);
};

cTShiftData::cTShiftData(time_t LastChannel)
{
	lastChannel=LastChannel;
	if(lastChannel+Setup.ZapTimeout<time(NULL))
		zapChannel=0;
	else
		zapChannel=LastChannel;
	mode=TShiftPlayerLive;
	index=-1;
}

bool cTShiftData::IsZapping()
{
	return GetZapChannel();
}

time_t cTShiftData::GetZapChannel()
{
	if((zapChannel)&&(zapChannel+Setup.ZapTimeout<time(NULL)))
	{
		switch(mode)
		{
			case TShiftPlayerLive:
			case TShiftPlayerDelay:
				break;
			case TShiftPlayerPlay:
				index+=(zapChannel-lastChannel)*FRAMESPERSEC;
				break;
			case TShiftPlayerPause:
				mode=TShiftPlayerPlay;
				break;
		}
		lastChannel=zapChannel;
		zapChannel=0;
	}
	return zapChannel;
}

void cTShiftData::BeginZapping()
{
	zapChannel=time(NULL);
}

void cTShiftData::ForgetZapping()
{
	if(zapChannel)
		zapChannel=0;
}

time_t cTShiftData::GetLastChannel()
{
	time_t result=GetZapChannel();
	return result?result:lastChannel;
}

int cTShiftData::GetIndex()
{
	switch(mode)
	{
		case TShiftPlayerLive:
		case TShiftPlayerDelay:
			return INT_MAX;
		case TShiftPlayerPlay:
			return index+(time(NULL)-lastChannel)*FRAMESPERSEC;
		case TShiftPlayerPause:
			time_t result=GetZapChannel();
			if(result)
				return index+(time(NULL)-result)*FRAMESPERSEC;
			return index;				
	}
	return -1;
}

TShiftPlayerMode cTShiftData::GetMode()
{
	if(IsZapping()&&(mode==TShiftPlayerPause))
		return TShiftPlayerPlay;
	return mode;
}

void cTShiftData::SetMode(TShiftPlayerMode Mode,int Index)
{
	zapChannel=0;
	time_t now=time(NULL);
	if(Mode<TShiftPlayerPlay)
		index=-1;
	else
	{
		if(Index>=0)
			index=Index;
		else if(mode==TShiftPlayerPlay)
			index+=(now-lastChannel)*FRAMESPERSEC;
	}
	mode=Mode;
	lastChannel=now;
}
class cTShiftRecorder:public cRecorder
{
//File
	int file;
//Channel
	int channelNumber;
//Recorder
	cTShiftRecorder(const cChannel *Channel,time_t BeginChannel);
	cTShiftIndexFile *index;
//Recorder state
	bool haveLast;
	bool online;
	bool newRecord;
	void SetPriority(void);
//Player
	cTShiftPlayerControl *player;
	cTShiftData data;
public:
//Files
	static void InitializeTShift(void);
	static void ShutdownTShift(void);
	static bool ProcessTShift(void);
//Channel
	int GetChannelNumber(void){return channelNumber;}
//Recorder
	static cTShiftRecorder *New(const cChannel *Channel,time_t BeginChannel);
	virtual ~cTShiftRecorder();
	void Impact(cDevice *Device,time_t *LastThis,time_t *LastOther);
	cDevice *GetDevice(void);
	bool AutoDelete(void);
//Recorder state
	bool HaveRecord(void);
	int IsRecording(void);
//Player
	cPlayer *GetPlayer(void);
	bool BufferInfo(int *Total,int *Current,int *First);
	bool ReplayInfo(bool *Play,bool *Forward,int *Speed);

	void SetOnline(void);
	void PlayerStop(void);
	void PlayerPause(void);
	void PlayerPlay(void);
	void PlayerDelay(void);
	void PlayerForward(void);
	void PlayerBackward(void);
	TShiftPlayerMode GetMode();
	time_t GetLastChannel(void);
	bool IsPlaying(void);
};

class cTShiftPlayerControl:public cDvbPlayerControl
{
	cTShiftIndexFile *index;
	int lastIndex;
public:
	int GetIndex(void);
	TShiftPlayerMode GetMode(void);
	cTShiftPlayerControl(const char *FileName,int ResumeIndex,cTShiftIndexFile *newIndex);
	~cTShiftPlayerControl();
	virtual void Hide(void){}
	cPlayer *GetPlayer(void); 
};

class cTShiftTransferControl:public cTransferControl
{
public:
	cTShiftTransferControl(cDevice *ReceiverDevice, const cChannel *Channel);
	cPlayer *GetPlayer(bool Attach=false); 
}; 

void cTShiftData::SetMode(cTShiftPlayerControl *player)
{
	SetMode(player->GetMode(),player->GetIndex());
}


// --- cTShiftRecorder -------------------------------------------------------------

cTShiftRecorder::cTShiftRecorder(const cChannel *Channel,time_t BeginChannel)
:cRecorder(cTShiftFileName::newFileName(&file),0,BASEPRIORITY,Channel->Vpid(),Channel->Apids(),Channel->Dpids(),Channel->Spids(),true,0,Channel->DPpids()),data(BeginChannel)
{
  player=NULL;
  channelNumber=Channel->Number();
  haveLast=false;
  online=true;
  newRecord=true;
  index=(cTShiftIndexFile *)GetIndexFile();
}

cTShiftRecorder *cTShiftRecorder::New(const cChannel *Channel,time_t BeginChannel)
{
	cTShiftRecorder *result=new cTShiftRecorder(Channel,BeginChannel);
	if(!result)
		return NULL;
	if((!result->index)||(result->file==MAXTSHIFTFILES))
	{
		delete result;
		return NULL;
	}
	if(!result->index->IsFileOK())
	{
		delete result;
		return NULL;
	}
	return result;
}

bool cTShiftRecorder::AutoDelete(void)
{
	if(!Activated())
	{
		dsyslog("TShift: detached recorder autodeleted, channel %d",channelNumber);
		delete this;
		return true;
	}
	if(online)
	{
		if(data.IsZapping())
		{
			if(newRecord)
			{
				dsyslog("TShift: pre-zaptime recorder autodeleted, channel %d",channelNumber);
				delete this;
				return true;
			}
			else
				data.ForgetZapping();
		}
		else
		{
			if(player)
				data.SetMode(player);
			else
				data.SetMode(data.GetMode());
		}
		if(player)
		{
			delete player;
			player=NULL;
		}
		newRecord=false;
		online=false;
	}
	return false;
}

cTShiftRecorder::~cTShiftRecorder()
{
	if(player)
		delete player;
	Detach();
	cTShiftFileName::FreeFile(file);
}

void cTShiftRecorder::InitializeTShift()
{
	cTShiftFileName::InitializeTShift();
}

bool cTShiftRecorder::ProcessTShift()
{
	return cTShiftFileName::FreeFiles(false);
}

void cTShiftRecorder::ShutdownTShift()
{
	cTShiftFileName::FreeFiles(true);
}

bool cTShiftRecorder::HaveRecord()
{
	if(!haveLast)
		haveLast=(index->Last()>0);
	return haveLast;
}

bool cTShiftRecorder::IsPlaying()
{
	if(player)
		return player->Active();
	return false;
}

cPlayer* cTShiftRecorder::GetPlayer()
{
	if(!HaveRecord())
	{
		for(int f=1;(f<15)&&(Activated())&&(!HaveRecord());f++)
		{
			dsyslog("TShift: try #%d waiting recorder, channel %d",f,channelNumber);
			cCondWait::SleepMs(1000);				
		}
		if(!HaveRecord())
		{
			esyslog("TShift: timeout waiting recorder, channel %d",channelNumber);
			return NULL;
		}
	}
	if(player)
		delete player;
	player=new cTShiftPlayerControl(cTShiftFileName::makeFileName(file),data.GetIndex(),index);
	if(!index->Ok())
	{
		delete player;
		player=NULL;
		return NULL;
	}
	cPlayer *result=player->GetPlayer();
	if(!result)
	{
		delete player;
		player=NULL;
	}
	return result;
}

bool cTShiftRecorder::BufferInfo(int *Total,int *Current,int *First)
{
	if(!HaveRecord())
		return false;
	*Total=index->Last();
	*First=index->First();
	if(player)
		*Current=player->GetIndex();
	else
		*Current=data.GetIndex();
	if(*Current<*First)
		*Current=*First;
	else if (*Current>*Total)
		*Current=*Total;
	return true;
}

bool cTShiftRecorder::ReplayInfo(bool *Play,bool *Forward,int *Speed)
{
	if(!HaveRecord())
		return false;
	if(player)
		return player->GetReplayMode(*Play,*Forward,*Speed);
	*Play=true;
	*Forward=true;
	*Speed=-1;
	return true;
}

void cTShiftRecorder::SetOnline()
{
	online=true;
	data.BeginZapping();
}

void cTShiftRecorder::PlayerStop()
{
	if(player)
	{
		delete player;
		player=NULL;
	}
	data.SetMode(TShiftPlayerLive);
	SetPriority();
}

void cTShiftRecorder::PlayerDelay()
{
	if(player)
	{
		delete player;
		player=NULL;
	}
	data.SetMode(TShiftPlayerDelay);
	SetPriority();
}

void cTShiftRecorder::PlayerPlay()
{
	if(player)
	{
		player->Play();
		data.SetMode(player);
	}
	else
		data.SetMode(TShiftPlayerPlay);
	SetPriority();
}
void cTShiftRecorder::PlayerPause()
{
	if(player)
	{
		player->Pause();
		data.SetMode(player);
	}
	else
		data.SetMode(TShiftPlayerPause);
	SetPriority();
}
void cTShiftRecorder::PlayerBackward()
{
	if(player)
	{
		player->Backward();
		data.SetMode(player);
	}
	else
		data.SetMode(TShiftPlayerPlay);
	SetPriority();
}
void cTShiftRecorder::PlayerForward()
{
	if(player)
	{
		player->Forward();
		data.SetMode(player);
	}
	else
		data.SetMode(TShiftPlayerPlay);
	SetPriority();
}

void cTShiftRecorder::SetPriority()
{
	if(data.GetMode()==TShiftPlayerLive)
	{
		if(priority!=BASEPRIORITY)
		{
			priority=BASEPRIORITY;
			dsyslog("TShift: recorder priority=%d, channel %d",priority,channelNumber);
		}
	}
	else
	{
		if(priority!=Setup.TShiftPriority)
		{
			priority=Setup.TShiftPriority;
			dsyslog("TShift: recorder priority=%d, channel %d",priority,channelNumber);
		}
	}
}

TShiftPlayerMode cTShiftRecorder::GetMode()
{
	return data.GetMode();
}
time_t cTShiftRecorder::GetLastChannel(void)
{
	return data.GetLastChannel();
}

cDevice *cTShiftRecorder::GetDevice()
{
	return device;
}

void cTShiftRecorder::Impact(cDevice *Device,time_t *LastThis,time_t *LastOther)
{
	time_t lastChannel=GetLastChannel();
	if(device==Device)
	{
		if(*LastThis<lastChannel)
			*LastThis=lastChannel;
	}
	else
	{
		if(*LastOther<lastChannel)
			*LastOther=lastChannel;
	}
}

// --- cTShiftPlayerControl -------------------------------------------------------------

cTShiftPlayerControl::cTShiftPlayerControl(const char *FileName,int ResumeIndex,cTShiftIndexFile *newIndex)
:cDvbPlayerControl(FileName,newIndex->ActivatePlayer(ResumeIndex))
{
	index=newIndex;
	lastIndex=-1;
}

cTShiftPlayerControl::~cTShiftPlayerControl()
{
	index->DeactivatePlayer();
}
int cTShiftPlayerControl::GetIndex()
{
	int current;
	int total;
	if(!cDvbPlayerControl::GetIndex(current,total,false))
	{
		if(lastIndex>=0)
			return lastIndex;
		else
			return index->GetResume();
	}
	if(current<=0)
	{
		if(lastIndex>=0)
			return lastIndex;
		else
			return index->GetResume();
	}
	lastIndex=current;
	return current;
}

TShiftPlayerMode cTShiftPlayerControl::GetMode()
{
	bool Play=true;
	bool Forward;
	int Speed=1;
	if(cDvbPlayerControl::GetReplayMode(Play,Forward,Speed))
		if(!Play)
			return TShiftPlayerPause;
	return TShiftPlayerPlay;
}

cPlayer *cTShiftPlayerControl::GetPlayer()
{
	if(!cControl::player->IsAttached())
		if(!cDevice::PrimaryDevice()->AttachPlayer(cControl::player))
			return NULL;
	return cControl::player;
}

// --- cTShiftTransferControl -------------------------------------------------------------

cTShiftTransferControl::cTShiftTransferControl(cDevice *ReceiverDevice, const cChannel *Channel)
:cTransferControl(ReceiverDevice,Channel->Ppid(), Channel->Vpid(), Channel->Apid(0),Channel->Tpid())
{
}

cPlayer *cTShiftTransferControl::GetPlayer(bool Attach)
{
	if((Attach)&&(!cControl::player->IsAttached()))
		if(!cDevice::PrimaryDevice()->AttachPlayer(cControl::player))
			return NULL;
	return cControl::player;
} 

// --- cTShiftControl ------------------------------------------------------

cTShiftRecorder *cTShiftControl::receivers[MAXTSHIFT];
cMutex cTShiftControl::controlLock;
cTShiftControl *cTShiftControl::currentControl=NULL;
time_t cTShiftControl::LastProcess=0;
bool cTShiftControl::nothingToDo=true;


cTShiftControl::cTShiftControl(cDevice *ReceiverDevice, const cChannel *Channel)
:cControl(NULL,true)
{
  controlLock.Lock();
  delete currentControl;
  currentControl=this;
  ControlLastProcess=time(NULL);
  beginChannel=ControlLastProcess;
  lastActivated=0;
  IsForwarding=false;
  startRecorder=((Setup.TShift==1)&&(getIaMode()))?ControlLastProcess+Setup.TShiftStartRecord:0;
  channelNumber=Channel->Number();
  controlTransfer=NULL;
  controlRecorder=NULL;
  for(int f=0;f<MAXTSHIFT;f++)
	if(receivers[f])
		if(receivers[f]->GetChannelNumber()==channelNumber)
		{
			if(receivers[f]->AutoDelete())
				receivers[f]=NULL;
			else
			{ 			
				controlRecorder=receivers[f];
				receivers[f]=NULL;
				controlRecorder->SetOnline();
				startRecorder=ControlLastProcess+60;
			}
			break;
		}
  if(controlRecorder)
	if(controlRecorder->GetMode()>TShiftPlayerDelay)
		cControl::player=controlRecorder->GetPlayer();
  if(!cControl::player)
  {
	controlTransfer=new cTShiftTransferControl(ReceiverDevice,Channel);
	cControl::player=controlTransfer->GetPlayer();
  }
	
  nothingToDo=false;
  controlLock.Unlock();
}
void cTShiftControl::StopRecord()
{
	startRecorder=0;
	lastActivated=0;
	if(controlRecorder)
	{
		delete controlRecorder;
		controlRecorder=NULL;
	}
}
int cTShiftControl::IsRecording()
{
	if(!startRecorder)
		return -1;
	if(!controlRecorder)
		return 0;
	return (controlRecorder->Activated()?1:0);
}

bool cTShiftControl::StartRecord()
{
	time_t Now=time(NULL);
	startRecorder=Now+60;
	cChannel *channel=NULL;
	if(controlRecorder)
	{
		if(controlRecorder->Activated())
		{
			lastActivated=Now;
			return true;
		}
	}
	else
	{
		int FreeMB=0;
		VideoDiskSpace(&FreeMB);
		if(FreeMB<MINFREEDISK)
		{
			isyslog("TShift: not enough disk space to start recording, channel %d",channelNumber);
			return false;
		}
		Channels.Lock(false);
		channel=Channels.GetByNumber(channelNumber);
		if(!channel)
		{
			Channels.Unlock();
			StopRecord();
			esyslog("TShift: channel not found, channel %d",channelNumber);
			return false;
		}
		controlRecorder=cTShiftRecorder::New(channel,beginChannel);
		if(!controlRecorder)
		{
			Channels.Unlock();
			StopRecord();
			esyslog("TShift: recorder without file, channel %d",channelNumber);
			return false;
		}
	}
	cDevice *receiverDevice=cTransferControl::ReceiverDevice();
	if(receiverDevice)
		if(!receiverDevice->FreeReceiverSlot())
			receiverDevice=NULL;
	if(!receiverDevice)
	{
		if(!channel)
		{
			Channels.Lock(false);
			channel=Channels.GetByNumber(channelNumber);
		}
		if(!channel)
		{
			Channels.Unlock();
			esyslog("TShift: channel not found, channel %d",channelNumber);
		}
		else
		{
			receiverDevice=cDevice::GetDevice(channel,IsDelayed()?Setup.TShiftPriority:0,NULL,false);
			if(receiverDevice)
				if(!receiverDevice->IsTunedToTransponder(channel))
					if(!receiverDevice->SwitchChannel(channel,false))
						receiverDevice=NULL;
		}
	}
	if(channel)
		Channels.Unlock();
	if(!receiverDevice)
		isyslog("TShift: device not available, channel %d",channelNumber);
	else if(receiverDevice->AttachReceiver(controlRecorder))
	{
		isyslog("TShift: start online recording, channel %d",channelNumber);
		lastActivated=Now;
		return true;
	}
	startRecorder=Now+5;
	if(!lastActivated)
		lastActivated=Now;
	return true;
}

void cTShiftControl::InitializeTShift()
{
  int f=0;
  controlLock.Lock();
  for(;f<MAXTSHIFT;f++)
	receivers[f]=NULL;
  cTShiftRecorder::InitializeTShift();
  LastProcess=time(NULL);
  controlLock.Unlock();
}

void cTShiftControl::StartTShift()
{
  controlLock.Lock();
  LastProcess=time(NULL);
  nothingToDo=false;
  if(currentControl)
	if(!currentControl->startRecorder)
		currentControl->startRecorder=((Setup.TShift==1)&&(getIaMode()))?time(NULL)+Setup.TShiftStartRecord:0;
  controlLock.Unlock();
}

void cTShiftControl::ShutdownTShift()
{
  int f=0;
  controlLock.Lock();
  if(currentControl)
	currentControl->StopRecord();
  for(;f<MAXTSHIFT;f++)
  {
	if(receivers[f])
	{
		delete receivers[f];
		receivers[f]=NULL;
	}
  }
  LastProcess=time(NULL);
  cTShiftRecorder::ShutdownTShift();
  nothingToDo=true;
  controlLock.Unlock();
}

cTShiftControl::~cTShiftControl()
{
  controlLock.Lock();
  if(currentControl==this)
	currentControl=NULL;
  if(controlTransfer)
	delete controlTransfer; 
  if(controlRecorder)
  {
	if(controlRecorder->AutoDelete())
		isyslog("TShift: stop online recording, channel %d",channelNumber);
	else
	{
		if((Setup.TShiftDelayed==1)||((Setup.TShiftDelayed)&&(IsDelayed())))
		{
			int f=0;
			for(;f<MAXTSHIFT;f++)
				if(!receivers[f])
					break;
			if(f<MAXTSHIFT)
			{
				isyslog("TShift: start offline recording, channel %d",channelNumber);
				if((Setup.TShiftPause)&&(IsDelayed()))
					controlRecorder->PlayerPause();
				receivers[f]=controlRecorder;
				controlRecorder=NULL;
			}
			else
				esyslog("TShift: too much recordings, channel %d",channelNumber);
		}
		if(controlRecorder)
		{
			isyslog("TShift: stop online recording, channel %d",channelNumber);
			delete controlRecorder;
		}
	}
  }
  controlLock.Unlock();
}
bool cTShiftControl::IsDelayed()
{
	if(!controlRecorder)
		return false;
	return (controlRecorder->GetMode()>TShiftPlayerLive);
}
bool cTShiftControl::IsTransfering()
{
	return controlTransfer;
}

bool cTShiftControl::StartTransfer()
{
	if(IsTransfering())
		return true;
	Channels.Lock(false);
	cChannel *channel=Channels.GetByNumber(channelNumber);
	if(!channel)
	{
		Channels.Unlock();
		esyslog("TShift: channel not found, channel %d",channelNumber);
		return false;
	}
	cDevice *receiverDevice=controlRecorder?controlRecorder->GetDevice():NULL;
	if(!receiverDevice)
	{
		receiverDevice=cDevice::GetDevice(channel,0,NULL,true);
		if(receiverDevice)
			if(!receiverDevice->IsTunedToTransponder(channel))
				if(!receiverDevice->SwitchChannel(channel,false))
					receiverDevice=NULL;
	}
	if(receiverDevice)
		controlTransfer=new cTShiftTransferControl(receiverDevice,channel);
	Channels.Unlock();
	if(controlTransfer)
	{
		cPlayer *Player=controlTransfer->GetPlayer(true);
		if(Player)
		{
			cControl::player=Player;
			return true;
		}
		delete controlTransfer;
		controlTransfer=NULL;
		return false;
	}
	isyslog("TShift: device not available, channel %d",channelNumber);
	return false;
}
bool cTShiftControl::StartPlayer()
{
	if(!StartRecord())
		return false;
	if(controlRecorder->IsPlaying())
		return true;
	cPlayer *Player=controlRecorder->GetPlayer();
	if(!Player)
	{
		StopRecord();
		return false;
	}
	cControl::player=Player;
	delete controlTransfer;
	controlTransfer=NULL;
	return true;
}
bool cTShiftControl::Play(void)
{
	if(StartPlayer())
	{
		controlRecorder->PlayerPlay();
		return true;
	}
	return false;
}
bool cTShiftControl::Pause(void)
{
	if(StartPlayer())
	{
		controlRecorder->PlayerPause();
		return true;
	}
	return false;
}
bool cTShiftControl::Delay(void)
{
	if(StartTransfer())
	{
		controlRecorder->PlayerDelay();
		return true;
	}
	return false;
}
bool cTShiftControl::Stop(void)
{
	if(StartTransfer())
	{
		controlRecorder->PlayerStop();
		return true;
	}
	return false;
}
bool cTShiftControl::Forward(void)
{
	if(StartPlayer())
	{
		IsForwarding=true;
		controlRecorder->PlayerForward();
		return true;
	}
	return false;
}
bool cTShiftControl::Backward(void)
{
	if(StartPlayer())
	{
		controlRecorder->PlayerBackward();
		return true;
	}
	return false;
}
bool cTShiftControl::Forget(void)
{
	if(StartTransfer())
	{
		StopRecord();
		return true;
	}
	return false;
}

bool cTShiftControl::OnlyProcessKey(eKeys Key,int *LastChannel)
{
  switch(int(Key))
  {
    case kPlay:   if(LastChannel) *LastChannel=-1;
		   Play();
		   return true;
    case kPause:  if(LastChannel) *LastChannel=-1;
		   Pause();
		   return true;
    case kFastRew|k_Release:
	           if(Setup.MultiSpeedMode)
			return true;
    case kFastRew:if(LastChannel) *LastChannel=-1;
		   if(!StartRecord())
			return true;
		   if(!controlRecorder->HaveRecord())
		   	Delay();
		   else
			Backward();
		   return true;
    case kFastFwd|k_Release:
	           if(Setup.MultiSpeedMode)
			return true;
    case kFastFwd:if(LastChannel) *LastChannel=-1;
		   if(!StartRecord())
			return true;
		   if(IsTransfering())
			Delay();
		   else
			Forward();
		   return true;
    case kStop:   if(LastChannel) *LastChannel=-1;
		   if(!IsDelayed())
			Forget();
		   else if(!Stop())
			Skins.QueueMessage(mtInfo,tr("Channel not available!"));
		   return true;
    default:	   return false;
  }
  return false;
}
bool cTShiftControl::ProcessKeyTShift(eKeys Key,int *LastChannel,cOsdObject *Menu)
{
  if(!Menu)
	if(OnlyProcessKey(Key,LastChannel))
		return true;
  if(Key==kNone)
  {
	if(IsForwarding)
	{
		if((!IsTransfering())&&(controlRecorder))
		{
			if(!controlRecorder->IsPlaying())
			{
				*LastChannel=-1;
				if(!Delay())
					if(!Pause())
						StopChannel();
				return true;
			}
			else
			{
				bool Play,Forward;
				int Speed;
				if(controlRecorder->ReplayInfo(&Play,&Forward,&Speed))
				{
					if((!Forward)||(Speed<0))
						IsForwarding=false;
				}
				else
					IsForwarding=false;
			}
		}
		else
			IsForwarding=false;
	}
	time_t Now=time(NULL);
	if(startRecorder&&(startRecorder<Now))
		if(StartRecord())
			if(!controlRecorder->Activated())
			{
				int Total=0,Current=0,First=0;
				switch(controlRecorder->GetMode())
				{
					case TShiftPlayerLive:
					case TShiftPlayerDelay:
						if(Now-lastActivated>300)
						{
							if(Forget())
								startRecorder=Now+60;
							else
								StopChannel();
							return true;
						}
					case TShiftPlayerPause:
						startRecorder=Now+60;
						break;
					case TShiftPlayerPlay:
						controlRecorder->BufferInfo(&Total,&Current,&First);
						if(Total-Current<FRAMESPERSEC*10)
						{
							Skins.QueueMessage(mtInfo,tr("Channel not available!"));
							if(!Pause())
								StopChannel();
							return true;
						}
						if(Total-Current>FRAMESPERSEC*60)
						{
							Skins.QueueMessage(mtInfo,cString::sprintf(tr("Channel not available in less than %d minutes"),((Total-Current)/(FRAMESPERSEC*60))+1));
							startRecorder=Now+60;
						}
						break;
				}
			}
	if((ControlLastProcess+60<Now)&&(Setup.TShiftTimeout))
	{
		ControlLastProcess=Now;
		if(controlRecorder)
		{
			time_t LastActivity=cRemote::LastActivity();
			if(LastActivity<controlRecorder->GetLastChannel())
				LastActivity=controlRecorder->GetLastChannel();
			LastActivity+=Setup.TShiftTimeout*60;
			if(LastActivity<Now)
			{
				if(LastActivity+60<Now)
				{
					*LastChannel=-1;
					if(controlRecorder->GetMode()>=TShiftPlayerPlay)
					{
						isyslog("TShift: auto playerDelay online, channel %d",channelNumber);
						if(!Delay())
							StopChannel();
					}
					else
					{
						isyslog("TShift: auto StopRecord online, channel %d",channelNumber);
						if(!Forget())
							StopChannel();
					}
					return true;
				}
				if(controlRecorder->GetMode()>=TShiftPlayerPlay)
					Skins.QueueMessage(mtInfo,tr("Switching to live TV in 1 minute"));
				else
					Skins.QueueMessage(mtInfo,tr("Stopping buffer in 1 minute"));
			}
		}
	}
  }
  return false;
}
void cTShiftControl::StopChannel()
{
	int number=channelNumber;
	isyslog("TShift: StopChannel, channel %d",number);
	cDevice::PrimaryDevice()->StopReplay();
	if(!Channels.SwitchTo(number))
		if(!cDevice::SwitchChannel(1))
			cDevice::SwitchChannel(-1);
}
eKeys cTShiftControl::FilterKeyFromMenu(eKeys Key)
{
  if(nothingToDo)
	return Key;
  controlLock.Lock();
  if(currentControl)
  {
	if(currentControl->OnlyProcessKey(Key))
	{
		controlLock.Unlock();
		return kNone;
	}
  }
  controlLock.Unlock();
  return Key;
}
eKeys cTShiftControl::FilterKey(eKeys Key,int *LastChannel,cOsdObject *Menu)
{
  if(nothingToDo)
	return Key;
  controlLock.Lock();
  if(currentControl)
  {
	if(currentControl->ProcessKeyTShift(Key,LastChannel,Menu))
	{
		controlLock.Unlock();
		return kNone;
	}
  }
  if(Key==kNone)
  {
	if(cTShiftRecorder::ProcessTShift())
	{
		controlLock.Unlock();
		return kNone;
	}
	time_t Now=time(NULL);
	if(LastProcess+60<Now)
	{
		LastProcess=Now;
		nothingToDo=!currentControl;
		for(int f=0;f<MAXTSHIFT;f++)
			if(receivers[f])
			{
				nothingToDo=false;
				if(receivers[f]->AutoDelete())
				{
					receivers[f]=NULL;
					break;
				}
				else if((Setup.TShiftTimeout)&&(receivers[f]->GetLastChannel()+Setup.TShiftTimeout*60<Now))
				{
					isyslog("TShift: auto StopRecord offline, channel %d",receivers[f]->GetChannelNumber());
					delete receivers[f];
					receivers[f]=NULL;
					break;
				}
			}
	}
  }
  controlLock.Unlock();
  return Key;
}

int cTShiftControl::GetTShiftInfo(int Channel,TShiftPlayerMode *Mode,int *Recording)
{
  int result=0;
  if(Recording)
	*Recording=-1;
  controlLock.Lock();
  if(currentControl)
  {
	if(currentControl->IsDelayed())
		result=2;
	else if(currentControl->IsRecording()>0)
		result=1;
	if((Channel>0)&&(currentControl->channelNumber==Channel))
	{
		Channel=0;
		if(Recording)
			*Recording=currentControl->IsRecording()+1;
		if(Mode)
		{
			if(currentControl->controlRecorder)
				*Mode=currentControl->controlRecorder->GetMode();
			else
				*Mode=TShiftPlayerLive;
		}
		if(result)
			result+=2;
	}
  }
  for(int f=0;(f<MAXTSHIFT)&&((Channel>0)||(result<2))&&(result<3);f++)
  	if(receivers[f])
	{
		if((Channel>0)&&(receivers[f]->GetChannelNumber()==Channel))
		{
			Channel=0;
			if(receivers[f]->GetMode()>TShiftPlayerLive)
				result=4;
			else if(receivers[f]->Activated())
				result=3;
			if(Recording)
				*Recording=receivers[f]->Activated()?2:1;
			if(Mode)
				*Mode=receivers[f]->GetMode();
		}
		else
		{
			if(receivers[f]->GetMode()>TShiftPlayerLive)
				result=2;
			else if(receivers[f]->Activated())
				result=1;
		}
	}
  controlLock.Unlock();
  return result;
}
bool cTShiftControl::BufferInfo(int CurrentChannel,int *Total,int *Current)
{
  controlLock.Lock();
  if(currentControl)
	if((currentControl->controlRecorder)&&(currentControl->channelNumber==CurrentChannel))
	{
		int First;
		if(currentControl->controlRecorder->BufferInfo(Total,Current,&First))
		{
			if(*Current==*Total)
				(*Current)++;
			*Total-=First-1;
			*Current-=First;
			controlLock.Unlock();
			return true;
		}
	}
  controlLock.Unlock();
  return false;
}

bool cTShiftControl::ReplayInfo(int CurrentChannel,bool *Play,bool *Forward,int *Speed)
{
  controlLock.Lock();
  if(currentControl)
	if((currentControl->controlRecorder)&&(currentControl->channelNumber==CurrentChannel))
		if(currentControl->controlRecorder->ReplayInfo(Play,Forward,Speed))
		{
			controlLock.Unlock();
			return true;
		}
  controlLock.Unlock();
  return false;
}

bool cTShiftControl::Impact(cDevice *Device,bool NeedDetach)
{
  if((!NeedDetach)||(!Setup.TShift)||(Setup.TShiftDelayed!=1))
	return false;
  time_t LastThis=0;
  time_t LastOther=0;
  controlLock.Lock();
  if(currentControl)
	if(currentControl->controlRecorder)
	{
		currentControl->controlRecorder->Impact(Device,&LastThis,&LastOther);
		controlLock.Unlock();
		return LastThis>LastOther;
	}
  for(int f=0;f<MAXTSHIFT;f++)
  	if(receivers[f])
		receivers[f]->Impact(Device,&LastThis,&LastOther);
  controlLock.Unlock();
  return LastThis>LastOther;
}


bool cTShiftIndexFile::Grow(int needed,uint16_t FileNumber)
{
	lastFile=FileNumber;
	if((index)&&(size<maxSize)&&(last+needed>=size))
	{
		size+=600*FRAMESPERSEC;
		if(size>maxSize)
			size=maxSize;
		currentMutex.Lock();
		if(tIndex *newIndex=(tIndex *)realloc(index,size*sizeof(tIndex)))
		{
			index=newIndex;
			currentMutex.Unlock();
			dsyslog("TShift: cTShiftIndexFile realloc %d indexes,f=%d,l=%d,m=%d",size,first,last,maxSize);
		}
		else
		{
			esyslog("ERROR: out of memory");
			WriteIndex();
			currentMutex.Unlock();
			return false;
		}
	}
	if(last+needed-first>=maxSize)
		IncFirst(last+needed+1-first-maxSize);
	return index;
}


cUnbufferedFile *cTShiftIndexFile::GetReplayFile(cFileName *FileName)
{
	if((f>=0)&&(index)&&(FileName))
		return FileName->SetOffset(lastFile);
	return NULL;
}

cTShiftIndexFile *cTShiftIndexFile::ActivatePlayer(int ResumeIndex)
{
	indexLock.Lock();
	if((!index)&&(f>=0))
	{
		currentMutex.Lock();
		ReadIndex();
		currentMutex.Unlock();
		indexLock.Unlock();
		StoreResume(ResumeIndex);
		return this;
	}
	indexLock.Unlock();
	return NULL;
}
void cTShiftIndexFile::DeactivatePlayer()
{
	indexLock.Lock();
	if((index)&&(f>=0))
	{
		currentMutex.Lock();
		WriteIndex();
		currentMutex.Unlock();
	}
	indexLock.Unlock();
}
void cTShiftIndexFile::WriteIndex()
{
	if(f<0)
	{
		free(index);
		index=NULL;
		return;
	}

	if(lseek(f,0,SEEK_SET))
	{
		LOG_ERROR;
		close(f);
		f=-1;
		free(index);
		index=NULL;
		return;
	}
	int realSize=last+1;
	if(!realSize)
	{
		free(index);
		index=NULL;
		return;
	}
	if(realSize>maxSize)
		realSize=maxSize;
#if BYTE_ORDER == BIG_ENDIAN
	for(int j=0;j<realSize;j++)
		index[j].SetOffset(isPesRecording, bswap_32(index[j].Offset(isPesRecording)));
#endif
	realSize*=sizeof(tIndexPes);
	if(safe_write(f,index,realSize)!=realSize)
	{
		LOG_ERROR;
		close(f);
		f=-1;
	}
	free(index);
	index=NULL;
	if((f>=0)&&(last+1>=maxSize))
	{
		realSize=((last+1)%maxSize)*sizeof(tIndexPes);
		if(lseek(f,realSize,SEEK_SET)!=realSize)
		{
			LOG_ERROR;
			close(f);
			f=-1;
		}
	}
}
bool cTShiftIndexFile::ReadIndex()
{
	int realSize;
	if(last>=maxSize)
	{
		realSize=maxSize;
		size=maxSize;
	}
	else
	{
		realSize=last+1;
		size=realSize+600*FRAMESPERSEC;
		if(size>maxSize)
			size=maxSize;
	}
	index=MALLOC(tIndex,size);
	if(!index)
	{
		esyslog("TShift: can't allocate index, size %d",size);
		close(f);
		f=-1;
		return false;
	}
	if(realSize)
	{
		if(lseek(f,0,SEEK_SET))
		{
			LOG_ERROR;
			close(f);
			f=-1;
			free(index);
			index=NULL;
			return false;
		}
		int toRead=realSize*sizeof(tIndexPes);
		if(safe_read(f,index,toRead)!=toRead)
		{
			LOG_ERROR;
			close(f);
			f=-1;
			free(index);
			index=NULL;
			return false;
		}
#if BYTE_ORDER == BIG_ENDIAN
		for(int j=0;j<realSize;j++)
			index[j].SetOffset(isPesRecording, bswap_32(index[j].Offset(isPesRecording)));
#endif
	}
	return true;
}

cUnbufferedFile *cTShiftIndexFile::NextFile(cFileName *FileName, bool Record)
{
	if(!Record)
		return FileName->SetOffset((FileName->Number()%MAXFILESPERRECORDINGPES)+1);
	uint16_t FileNumber=FileName->Number();
	files[FileNumber-1]=last+1;
	FileNumber%=MAXFILESPERRECORDINGPES;
	uint16_t NextFile=(((FileNumber+1)%MAXFILESPERRECORDINGPES)+1)%MAXFILESPERRECORDINGPES;
	if(files[NextFile])
		SetFirst(files[NextFile]);
	if(!files[FileNumber])
		return FileName->SetOffset(FileNumber+1);
	esyslog("TShift: no more files");
	return NULL;
}

int cTShiftIndexFile::GetResume(void)
{
	if(resume>=last)
		resume=last-1;
	if(resume<first)
		resume=first;
	return resume;
}

bool cTShiftIndexFile::StoreResume(int Index)
{
	resume=Index;
	return true;
}

void cTShiftIndexFile::RemoveOldFiles(int First)
{
	while((files[nextFileToDelete])&&(First>=files[nextFileToDelete]+REMOVEOLDFILESAFETYLIMIT))
	{
		files[nextFileToDelete]=0;
		sprintf(contentFileNumber,RECORDFILESUFFIXPES,++nextFileToDelete);
		if(nextFileToDelete==MAXFILESPERRECORDINGPES)
			nextFileToDelete=0;
		dsyslog("TShift: removing %s",contentFileName);
		RemoveSingleVideoFile(contentFileName);
	}
}

int cTShiftIndexFile::GetNextCurrent(int Current)
{
	firstMutex.Lock();
	if(Current<first)
		Current=first;
	currentMutex.Lock();
	current=Current;
	firstMutex.Unlock();
	return current;
}
	
int cTShiftIndexFile::GetCurrent(int Current)
{
	firstMutex.Lock();
	if(Current<first)
	{
		firstMutex.Unlock();
		return -1;
	}
	currentMutex.Lock();
	current=Current;
	firstMutex.Unlock();
	return current;
}

void cTShiftIndexFile::UnlockCurrent()
{
	current=INT_MAX;
	currentMutex.Unlock();
}

void cTShiftIndexFile::IncFirst(int Inc)
{
	firstMutex.Lock();
	int First=first+Inc;
	if(First<=current)
	{
		first=First;
		firstMutex.Unlock();
		RemoveOldFiles(First);
		return;
	}
	currentMutex.Lock();
	first=First;
	firstMutex.Unlock();
	currentMutex.Unlock();
	RemoveOldFiles(First);
}
void cTShiftIndexFile::SetFirst(int First)
{
	firstMutex.Lock();
	if(First<=first)
	{
		firstMutex.Unlock();
		return;
	}
	if(First<=current)
	{
		first=First;
		firstMutex.Unlock();
		RemoveOldFiles(First);
		return;
	}
	currentMutex.Lock();
	first=First;
	firstMutex.Unlock();
	currentMutex.Unlock();
	RemoveOldFiles(First);
}

cTShiftIndexFile::cTShiftIndexFile(const char *FileName):cIndexFile(FileName,true,true)
{
	maxSize=Setup.TShiftBufferSize*60*FRAMESPERSEC;
	lastFile=0;
	current=INT_MAX;
	first=0;
	isPesRecording=true;
	resume=-1;
	memset(files,0,sizeof(int)*MAXFILESPERRECORDINGPES);
	nextFileToDelete=0;
	contentFileName=MALLOC(char,strlen(FileName)+RECORDFILESUFFIXLEN);
	if(!contentFileName)
	{
		LOG_ERROR;
		close(f);
		f=-1;
		return;
	}
	strcpy(contentFileName,FileName);
	contentFileNumber=contentFileName+strlen(FileName);
	if(f>=0)
	{
		int flags=fcntl(f,F_GETFL);
		if(flags<0)
		{
			LOG_ERROR;
			close(f);
			f=-1;
			return;
		}
		if(fcntl(f,F_SETFL,flags&~O_APPEND)<0)
		{
			LOG_ERROR;
			close(f);
			f=-1;
			return;
		}
	}
}
cTShiftIndexFile::~cTShiftIndexFile()
{
	free(contentFileName);
}
bool cTShiftIndexFile::Get(int Index, uint16_t *FileNumber, off_t *FileOffset, uchar *PictureType, int *Length)
{
	Index=GetCurrent(Index);
	if(Index<0)
		return false;
	if((index)&&(Index<last))
	{
		int realIndex=Index%maxSize;
		*FileNumber=index[realIndex].Number(isPesRecording);
		*FileOffset=index[realIndex].Offset(isPesRecording);
		if(PictureType)
			*PictureType=index[realIndex].Type(isPesRecording);
		if(Length)
		{
			int nextIndex=(realIndex+1)%maxSize;
			if(index[nextIndex].Number(isPesRecording)==*FileNumber)
				*Length=off_t(index[nextIndex].Offset(isPesRecording))-*FileOffset;
			else
				*Length=-1;
		}
		UnlockCurrent();
		return true;
	}
	UnlockCurrent();
	return false;
}

int cTShiftIndexFile::WaitIndex(int Index)
{
	if(Index<first)
		return GetNextIFrame(Index-1,true);
	return Index-1;
}


int cTShiftIndexFile::GetNextIFrame(int Index, bool Forward, uint16_t *FileNumber, off_t *FileOffset, int *Length, bool StayOffEnd)
{
	for(Index=(Forward?GetNextCurrent(Index+1):GetCurrent(Index-1));Index>=0;Index=(Forward?GetNextCurrent(Index+1):GetCurrent(Index-1)))
	{
		if((!index)||(Index>=last))
		{
			UnlockCurrent();
			return -1;
		}
		int realIndex=Index%maxSize;
		if(index[realIndex].Type(isPesRecording)==I_FRAME)
		{
			if(FileNumber)
				*FileNumber=index[realIndex].Number(isPesRecording);
			if(FileOffset)
				*FileOffset=index[realIndex].Offset(isPesRecording);
			if(Length)
			{
				int nextIndex=(realIndex+1)%maxSize;
				if(index[nextIndex].Number(isPesRecording)==index[realIndex].Number(isPesRecording))
					*Length=off_t(index[nextIndex].Offset(isPesRecording))-off_t(index[realIndex].Offset(isPesRecording));
				else
					*Length=-1;
			}
			UnlockCurrent();
			return Index;
		}
		UnlockCurrent();
	}
	return -1;
}

int cTShiftIndexFile::Get(uint16_t FileNumber, off_t FileOffset)
{
	uint16_t LastFile=lastFile;
	int i;
	for(i=0;i<last;i++)
	{
		i=GetNextCurrent(i);
		if(!index)
		{
			UnlockCurrent();
			return -1;
		}
		if(LastFile)
			if(index[i%maxSize].Number(isPesRecording)<=LastFile)
				LastFile=0;
		int realIndex=i%maxSize;
		if(((index[realIndex].Number(isPesRecording)>FileNumber)&&(LastFile<FileNumber))||((index[realIndex].Number(isPesRecording)==FileNumber)&&(off_t(index[realIndex].Offset(isPesRecording))>=FileOffset)))
			break;
		UnlockCurrent();
	}
	return i;
}

bool cTShiftIndexFile::Write(uchar PictureType, uint16_t FileNumber, off_t FileOffset)
{
	indexLock.Lock();
	tIndex i;
	i.Set(isPesRecording, FileOffset, PictureType, FileNumber);

	if(Grow(1,FileNumber))
	{
		memcpy(index+((last+1)%maxSize),&i,sizeof(tIndexPes));
		last++;
		indexLock.Unlock();
		return true;
	}
	if(f<0)
	{
		indexLock.Unlock();
		return false;
	}
#if BYTE_ORDER == BIG_ENDIAN
	i.SetOffset(isPesRecording, bswap_32(i.Offset(isPesRecording)));
#endif
	if((!((last+1)%maxSize))&&(last>=0))
		if(lseek(f,0,SEEK_SET))
		{
			LOG_ERROR;
			close(f);
			f=-1;
			indexLock.Unlock();
			return false;
		}
	if(safe_write(f,&i,sizeof(tIndexPes))<0)
	{
		LOG_ERROR;
		close(f);
		f=-1;
		indexLock.Unlock();
		return false;
	}
	last++;
	indexLock.Unlock();
	return true;
}

bool cTShiftIndexFile::Write(sPesResult *Picture,int PictureCount,uint16_t FileNumber,off_t FileOffset)
{
	tIndex inds[PictureCount];
	int count=0;
	indexLock.Lock();
	int realLeft=maxSize-((last+1)%maxSize);
	if(realLeft>=maxSize)
		realLeft=0;
	for(int i=0;i<PictureCount;i++)
	{
		if(Picture[i].pictureType)
		{
            inds[count].SetOffset(isPesRecording, FileOffset + Picture[i].offset);
            inds[count].SetType(isPesRecording, Picture[i].pictureType);
            inds[count].SetNumber(isPesRecording, FileNumber);
            inds[count++].SetReserved(isPesRecording);
		}
	}
	if(count)
	{
		if(Grow(count,FileNumber))
		{
			if(count>realLeft)
			{
				if(realLeft)
				{
					memcpy(index+((last+1)%maxSize),inds,realLeft*sizeof(tIndexPes));
					memcpy(index,inds+realLeft,(count-realLeft)*sizeof(tIndexPes));
				}
				else
				{
					memcpy(index,inds,count*sizeof(tIndexPes));
				}
			}
			else
			{
				memcpy(index+((last+1)%maxSize),inds,count*sizeof(tIndexPes));
			}
			last+=count;
			indexLock.Unlock();
			return true;
		}
		if(f<0)
		{
			indexLock.Unlock();
			return false;
		}
#if BYTE_ORDER == BIG_ENDIAN
		for(int i=0;i<count;i++)
			inds[i].SetOffset(isPesRecording, bswap_32(inds[i].Offset(isPesRecording)));
#endif
		if(count>realLeft)
		{
			if(realLeft)
			{
				if(safe_write(f,inds,sizeof(tIndexPes)*realLeft)<0)
				{
					LOG_ERROR;
					close(f);
					f=-1;
					indexLock.Unlock();
					return false;
				}
			}
			if(last>=0)
				if(lseek(f,0,SEEK_SET))
				{
					LOG_ERROR;
					close(f);
					f=-1;
					indexLock.Unlock();
					return false;
				}
		}
		else
			realLeft=0;
		if(safe_write(f,inds+realLeft,sizeof(tIndexPes)*(count-realLeft))<0)
		{
			LOG_ERROR;
			close(f);
			f=-1;
			indexLock.Unlock();
			return false;
		}
		last+=count;
	}
	indexLock.Unlock();
	return true;
}

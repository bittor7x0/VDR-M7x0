/*
 * recorder.c: The actual DVB recorder
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "recorder.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "shutdown.h"
#include "tshift.h"
#include "libsi/util.h"

#define RECORDERBUFSIZE  MEGABYTE(4)

// The maximum time we wait before assuming that a recorded video data stream
// is broken:
#define MAXBROKENTIMEOUT 30 // seconds

#define MINFREEDISKSPACE    (512) // MB
#define DISKCHECKINTERVAL   100 // seconds

class cFileWriter : public cThread {
private:
  cRemux *remux;
  cTSRecorder *recorder;
  cFileName *fileName;
  cIndexFile *index;
  uchar pictureType;
  off_t fileSize;
  cUnbufferedFile *recordFile;
  time_t lastDiskSpaceCheck;
  bool isPesRecording;
  bool RunningLowOnDiskSpace(void);
  bool NextFile(void);
protected:
  virtual void Action(void);
public:
  cFileWriter(const char *FileName, cRemux *Remux, bool IsTShift = false, cTSRecorder *Recorder = NULL);
  virtual ~cFileWriter();
  cIndexFile *GetIndexFile(void) { return index; }
  };
//M7X0 BEGIN AK
cFileWriter::cFileWriter(const char *FileName, cRemux *Remux, bool IsTShift, cTSRecorder *Recorder)
:cThread("file writer")
{
  fileName = NULL;
  remux = Remux;
  recorder = Recorder;
  index = NULL;
  pictureType = NO_PICTURE;
  fileSize = 0;
  lastDiskSpaceCheck = time(NULL);
  isPesRecording = recorder == NULL;
#ifdef USE_DIRECT_IO
  fileName = new cFileName(FileName, true, true, true, isPesRecording);
#else
  fileName = new cFileName(FileName, true, false, isPesRecording);
#endif
  recordFile = fileName->Open();
  if (!recordFile) {
     LOG_ERROR;
     return;
     }
  if (IsTShift)
     index=new cTShiftIndexFile(FileName);
  else
  // Create the index file:
  index = new cIndexFile(FileName, true, isPesRecording);
  if (!index)
     esyslog("ERROR: can't allocate index");
     // let's continue without index, so we'll at least have the recording
}

cFileWriter::~cFileWriter()
{
  Cancel(5);
  delete index;
  delete fileName;
}

bool cFileWriter::RunningLowOnDiskSpace(void)
{
  if (time(NULL) > lastDiskSpaceCheck + DISKCHECKINTERVAL) {
     int Free = FreeDiskSpaceMB(fileName->Name());
     lastDiskSpaceCheck = time(NULL);
     if (Free < MINFREEDISKSPACE) {
        dsyslog("low disk space (%d MB, limit is %d MB)", Free, MINFREEDISKSPACE);
        return true;
        }
     }
  return false;
}

bool cFileWriter::NextFile(void)
{
  if (recordFile && pictureType == I_FRAME) { // every file shall start with an I_FRAME
     if (fileSize > fileName->MaxFileSize() || RunningLowOnDiskSpace()) {
        recordFile = fileName->NextFile();
        fileSize = 0;
        }
     }
  return recordFile != NULL;
}


void cFileWriter::Action(void)
{
  if (!recordFile) {
     esyslog("No recording file ... giving up!");
     return;
     }
#ifdef USE_WATCHDOG_IN_FILEWRITER
  time_t t = time(NULL);
#endif

  while (Running()) {
        int Count;
        sPesResult *Header;
        int HeaderCount;
        int FirstIFrame;
        const uchar *p = NULL;
        if(recorder)
           p = recorder->Get(Count, Header, HeaderCount, FirstIFrame);
	else
           p = remux->Get(Count, Header, HeaderCount, FirstIFrame);
        if (!p) {
           if(recorder)
              cCondWait::SleepMs(300);
           }
        else {
           if (recordFile->Write(p, Count) < 0) {
              LOG_ERROR_STR(fileName->Name());
              break;
              }

           if (FirstIFrame != -1 && (fileSize > MEGABYTE(Setup.MaxVideoFileSize) | RunningLowOnDiskSpace())) {
              if ((index != NULL) & (FirstIFrame != 0))
                 index->Write(Header, FirstIFrame, fileName->Number(), fileSize);

              recordFile->Truncate(fileSize + Header[FirstIFrame].offset);

              if (index != NULL) {
                 if (!(recordFile = index->NextFile(fileName, true))) {
                    LOG_ERROR;
                    esyslog("Cannot open next recording file '%s' ... giving up",fileName->Name());
                    break;
                    }
                 }
              else
              if (!(recordFile = fileName->NextFile())) {
                 LOG_ERROR;
                 esyslog("Cannot open next recording file '%s' ... giving up",fileName->Name());
                 break;
                 }

              fileSize = 0;
              if(recorder)
                 recorder->Del(Header[FirstIFrame].offset);
              else
              remux->Del(Header[FirstIFrame].offset);

              }
           else {
              if ((index != NULL) & (HeaderCount != 0))
                 index->Write(Header, HeaderCount, fileName->Number(), fileSize);

              if(recorder)
                 recorder->Del(Count);
              else
              remux->Del(Count);
              fileSize += Count;
              }

#ifdef USE_WATCHDOG_IN_FILEWRITER
           t = time(NULL);
#endif
           }
#ifdef USE_WATCHDOG_IN_FILEWRITER
        else if (time(NULL) - t > MAXBROKENTIMEOUT) {
           esyslog("ERROR: video data stream broken");
           ShutdownHandler.RequestEmergencyExit();
           t = time(NULL);
           }
#endif
        }

  if (fileSize && recordFile && (index != NULL)) {
     fileSize = index->StripOffToLastIFrame(fileName->Number());
     if (fileSize > 0)
        recordFile->Truncate(fileSize);
     }

  if (fileSize == 0) {
     fileName->Unlink();
     }

}

cRecorder::cRecorder(const char *FileName, int Ca, int Priority, int VPid, const int *APids, const int *DPids, const int *SPids, bool IsTShift, int VType, const int *DPPids)
:cReceiver(Ca, Priority, VPid, APids, Setup.UseDolbyInRecordings ? DPids : NULL, SPids)
#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
,cThread("recording")
#endif
{
  // Make sure the disk is up and running:
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  setFrameEventsWanted(VPid);
#endif
  SpinUpDisk(FileName);
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
  lostBytes = 0;
#else
#ifdef USE_RECEIVER_RINGBUFFER
  ringBuffer = new cRingBufferReceiver(RECORDERBUFSIZE, true, "Recorder");
#else
  ringBuffer = new cRingBufferLinear(RECORDERBUFSIZE, TS_SIZE * 2, true, "Recorder");
#endif
  ringBuffer->SetTimeouts(0, 500);
  ringBuffer->SetLimits(TS_SIZE, TS_SIZE * 1024);
#endif
  remux = new cRemux(VPid, APids, Setup.UseDolbyInRecordings ? DPids : NULL, SPids, true, VType, DPPids);
  writer = new cFileWriter(FileName, remux, IsTShift);
}

cRecorder::~cRecorder()
{
  Detach();
  delete writer;
  delete remux;
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
  isyslog("INFO: Recoder has lost %d bytes",lostBytes);
#else
  delete ringBuffer;
#endif
}

void cRecorder::Activate(bool On)
{
  cReceiver::Activate(On);
  if (On) {
     writer->Start();
#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
     Start();
#endif
     }
#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
  else
     Cancel(10);
#endif
}
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
void cRecorder::Receive(uchar *Data, int Length) {
  esyslog("ERROR: cRecorder::Receive(uchar*, int) should not be called any more!");
}
void cRecorder::Receive(uchar *Data, int Length, const sTsDataHeader *const Header)
{
  int p = remux->Put(Data, Length, Header);
  lostBytes += Length - p;
}
#else
#ifdef USE_RECEIVER_RINGBUFFER
void cRecorder::Receive(uchar *Data, int Length) {
  esyslog("ERROR: cRecorder::Receive(uchar*, int) should not be called any more!");
}
void cRecorder::Receive(uchar *Data, int Length, const sTsDataHeader *const Header)
{
  if (Running()) {
     if (!ringBuffer->Put(Data, Length, Header) && Running())
        ringBuffer->ReportOverflow(Length);

     }
}
void cRecorder::Action(void)
{
  const sTsDataHeader *Header;
  int Count;
  uchar *Data;
  while (Running()) {
        Header = ringBuffer->Get(Data, Count);
        if (Header) {
           Count = remux->Put(Data, Count, Header);
           if (Count)
              ringBuffer->Del(Count);
           else
              cCondWait::SleepMs(100); // avoid busy loop when resultBuffer is full in cRemux::Put()
           }
        }
}
#else
void cRecorder::Receive(uchar *Data, int Length) {
  if (Running()) {
     int p = ringBuffer->Put(Data, Length);
     if (p != Length && Running())
        ringBuffer->ReportOverflow(Length - p);

     }
}
void cRecorder::Action(void)
{
  while (Running()) {
        int r;
        uchar *b = ringBuffer->Get(r);
        if (b) {
           int Count = remux->Put(b, r);
           if (Count)
              ringBuffer->Del(Count);
           else
              cCondWait::SleepMs(100); // avoid busy loop when resultBuffer is full in cRemux::Put()
           }
        }
}
#endif
#endif
//M7X0 END AK

cIndexFile *cRecorder::GetIndexFile(void)
{
	if(writer)
		return writer->GetIndexFile();
	return NULL;
}

#define RESULTBUFFERSIZE KILOBYTE(1024)
#define LAST_DIFF (1800)
cTSRecorder::cTSRecorder(const char *FileName,const cChannel *Channel,int Priority,bool IsTShift)
:cReceiver(Channel->Ca(),Priority,Channel->Vpid(Setup.UseHDInRecordings),Channel->Apids(),Setup.UseDolbyInRecordings?Channel->Dpids():NULL,Setup.UseSubtitlesInTSRecordings?Channel->Spids():NULL,Setup.UseTeletextInTSRecordings?Channel->Tpid():0,Channel->Ppid())
{
  // Make sure the disk is up and running:
  SpinUpDisk(FileName);
  lostBytes = 0;
  memset(bytesToSync,0,sizeof(int)*MAXRECEIVEPIDS);
  vPid=Channel->Vpid(Setup.UseHDInRecordings);
  vType=Channel->Vtype();
  if(!vType)
     vType=0x02;
  HaveTime=false;
  LastTime=0;
  LastDiff=LAST_DIFF;
  memset(packets,0,sizeof(sPesResult)*MAXRECEIVEPIDS);
  memset(firstPacket,0,sizeof(int)*MAXRECEIVEPIDS);
  MakePat(Channel);
  resultBuffer = new cRingBufferResult(RESULTBUFFERSIZE, false, true, "Result");
  writer = new cFileWriter(FileName, NULL, IsTShift, this);
}
cTSRecorder::~cTSRecorder()
{
  Detach();
  delete writer;
  delete resultBuffer;
  int f=0;
  for(;f<MAXRECEIVEPIDS;f++)
     lostBytes+=bytesToSync[f];
  isyslog("INFO: TSRecoder has lost %d bytes",lostBytes);
  if(!HaveTime)
     esyslog("TSRecoder, stream without PCR");
}
unsigned char *cTSRecorder::GetPat()
{
	TSPAT[3]=0x10|(((TSPAT[3]&0x0F)+1)&0x0F); //00-notScrambled=0, 01-noAdaptationOnlyPayload, XXXX-continuityCounter

	unsigned int crc=SI::CRC32::crc32((const char *)(TSPAT+5),12,0xFFFFFFFF);

	TSPAT[17]=(crc>>24); //crc-D1
	TSPAT[18]=(crc>>16)&0xFF; //crc-D7
	TSPAT[19]=(crc>>8)&0xFF; //crc-F4
	TSPAT[20]=crc&0xFF; //crc-B8

	TSPMS[3]=TSPAT[3]; //00-notScrambled=0, 01-noAdaptationOnlyPayload, XXXX-continuityCounter

	crc=SI::CRC32::crc32((const char *)(TSPMS+5),TSPMS[7]-1,0xFFFFFFFF);

	TSPMS[TSPMS[7]+4]=(crc>>24); //crc-D1
	TSPMS[TSPMS[7]+5]=(crc>>16)&0xFF; //crc-D7
	TSPMS[TSPMS[7]+6]=(crc>>8)&0xFF; //crc-F4
	TSPMS[TSPMS[7]+7]=crc&0xFF; //crc-B8
	return TSPAT;
}
void cTSRecorder::MakePat(const cChannel *Channel)
{
	memset(TSPAT,0xFF,TS_SIZE*2);
	TSPAT[0]=0x47;
	TSPAT[1]=0x40; //0-error, 1-payloadStart, 0-priority, 00000-PAT-Pid=0
	TSPAT[2]=0x00; //00000000-PAT-Pid=0
	TSPAT[3]=0x1F; //00-notScrambled=0, 01-noAdaptationOnlyPayload, 1111-continuityCounter
	TSPAT[4]=0x00; //Pointer field
	TSPAT[5]=0x00; //Table-Id-PAT
	TSPAT[6]=0xB0; //1-syntax, 0, 11-reserved, 00, 00-lenght-TS-header
	TSPAT[7]=0x0D; //lenght-TS-header
	TSPAT[8]=0x00; //TS-ID
	TSPAT[9]=0x01; //TS-ID  
	TSPAT[10]=0xC1; //11-reserved, 00000-version, 1-current/next
	TSPAT[11]=0x00; //section_number
	TSPAT[12]=0x00; //last_section_number
	TSPAT[13]=0x00; //program_number
	TSPAT[14]=0x01; //program_number
	TSPAT[15]=0xEF; //111-reserved, 01111 program_map_PID
	TSPAT[16]=0xFF; //11111111-program_map_PID
	//TSPAT[17]=0x36; //crc-
	//TSPAT[18]=0x90; //crc-
	//TSPAT[19]=0xE2; //crc-
	//TSPAT[20]=0x3D; //crc-

	TSPMS=TSPAT+TS_SIZE;
	TSPMS[0]=0x47;
	TSPMS[1]=0x4F; //0-error, 1-payloadStart, 0-priority, 01111-program_map_PID 
	TSPMS[2]=0xFF; //11111111-program_map_PID
	TSPMS[3]=0x1F; //00-notScrambled, 01-noAdaptationOnlyPayload, 1111-continuityCounter
	TSPMS[4]=0x00; //Pointer field
	TSPMS[5]=0x02; //Table-Id
	TSPMS[6]=0xB0; //1-syntax, 0, 11-reserved, 00, 00-lenght
	//TSPMS[7]=0x17; //lenght
	TSPMS[8]=0x00; //Program Number
	TSPMS[9]=0x01; //Program Number
	TSPMS[10]=0xC1; //11-reserved, 00000-version, 1-current/next
	TSPMS[11]=0x00; //section_number
	TSPMS[12]=0x00; //last_section_number
	//TSPMS[13]=0xE0|((PCRPid>>8)&0x1F); //111-Reserved XXXXX PCR_PID
	//TSPMS[14]=PCRPid&0xFF; //PCR_PID
	TSPMS[15]=0xF0; //1111-reserved, 0000 program_info_leght
	TSPMS[16]=0x00; //program_info_leght
	int current=17;
	PCRPid=Channel->Ppid();
	int numPids=0;
	//Video
	if(vPid) {
		if(!PCRPid)
			PCRPid=vPid;
	   	TSPMS[current++]=vType&0xff; //streamType  // 1B MPEG4 01/02 Video 03/04 Audio
		TSPMS[current++]=0xE0|((vPid>>8)&0x1F); //111-reserved XXXXX-Video-Pid
		TSPMS[current++]=vPid&0xFF; // Video-Pid
		TSPMS[current++]=0xF0; //1111-reserved, 0000-es_info_lenght
		TSPMS[current++]=0x00; //es_info_lenght
		packets[numPids++].pesPacketType=pesPtVideo;
		}
	const int *pid=Channel->Apids();
	if(pid)
		while((*pid)&&(numPids<MAXRECEIVEPIDS)) {
		//Audio MPEG
			if(!PCRPid)
				PCRPid=*pid;
			if(current<180){
				TSPMS[current++]=0x04; //streamType //04-Audio
				TSPMS[current++]=0xE0|(((*pid)>>8)&0x1F); //111-reserved XXXXX-Audio-Pid
				TSPMS[current++]=(*(pid++))&0xFF; //Audio-Pid
				TSPMS[current++]=0xF0; //1111-reserved, 0000-es_info_lenght
				TSPMS[current++]=0x00; //es_info_lenght
				}
			packets[numPids++].pesPacketType=pesPtAudio;
			}
	pid=Channel->Dpids();
	if((pid)&&(Setup.UseDolbyInRecordings)){
		int npid=0;
		while((*pid)&&(numPids<MAXRECEIVEPIDS)) {
		//Dolby Digital
			if(!PCRPid)
				PCRPid=*pid;
			if(current<177){
				TSPMS[current++]=0x06; //streamType //06-Private
				TSPMS[current++]=0xE0|(((*pid)>>8)&0x1F); //111-reserved XXXXX-Dolby-Pid
				TSPMS[current++]=(*(pid++))&0xFF; //Dolby-Pid
				TSPMS[current++]=0xF0; //1111-reserved, 0000-es_info_lenght
				TSPMS[current++]=0x03; //es_info_lenght
				TSPMS[current++]=Channel->DPpid(npid++); //Dolby-type AC3,EAC3
				TSPMS[current++]=0x01; //descriptor_length
				TSPMS[current++]=0x00; //AC-3 flags
				}
			packets[numPids++].pesPacketType=pesPtDolby;
			}
		}
	pid=Channel->Spids();
	if((pid)&&(Setup.UseSubtitlesInTSRecordings))
		while((*pid)&&(numPids<MAXRECEIVEPIDS)) {
		//Subtitle
			if(current<180){
				TSPMS[current++]=0x06; //streamType //06-Private
				TSPMS[current++]=0xE0|(((*pid)>>8)&0x1F); //111-reserved XXXXX-S-Pid
				TSPMS[current++]=(*(pid++))&0xFF; //S-Pid
				TSPMS[current++]=0xF0; //1111-reserved, 0000-es_info_lenght
				TSPMS[current++]=0x00; //es_info_lenght
				}
			packets[numPids++].pesPacketType=pesPtUnknown;
			}
	if((Setup.UseTeletextInTSRecordings)&&(Channel->Tpid())&&(numPids<MAXRECEIVEPIDS)) {
	//Teletext
		if(!PCRPid)
			PCRPid=Channel->Tpid();
		if(current<178){
			TSPMS[current++]=0x06; //streamType //06-Private
			TSPMS[current++]=0xE0|((Channel->Tpid()>>8)&0x1F); //111-reserved XXXXX-Teletext-Pid
			TSPMS[current++]=Channel->Tpid()&0xFF; //Teletext-Pid
			TSPMS[current++]=0xF0; //1111-reserved, 0000-es_info_lenght
			TSPMS[current++]=0x02; //es_info_lenght
			TSPMS[current++]=0x56; //teletext descriptor_tag
			TSPMS[current++]=0x00; //descriptor_length
/*			if(current<180){
				TSPMS[current-3]=0x07; //es_info_lenght
				TSPMS[current-1]=0x05; //descriptor_length
				TSPMS[current++]='s'; //language
				TSPMS[current++]='p'; //language
				TSPMS[current++]='a'; //language
				TSPMS[current++]=0x09; //00001 initial Teletext page // 001 Magazine number 1
				TSPMS[current++]=0x00; //0000 Page tens 0 // 0000 Page units 0
				if(current<180){
					TSPMS[current-8]=0x0C; //es_info_lenght
					TSPMS[current-6]=0x0A; //descriptor_length
					TSPMS[current++]='s'; //language
					TSPMS[current++]='p'; //language
					TSPMS[current++]='a'; //language
					TSPMS[current++]=0x10; //00010 Teletext subtitle page // 000 Magazine number 8
					TSPMS[current++]=0x88; //1000 Page tens 8 // 1000 Page units 8
					}
				}*/
			}
		packets[numPids++].pesPacketType=pesPtUnknown;
		}
	if((PCRPid)&&(numPids<MAXRECEIVEPIDS))
		packets[numPids++].pesPacketType=pesPtUnknown;
	TSPMS[7]=current-4; //lenght

	TSPMS[13]=0xE0|((PCRPid>>8)&0x1F); //111-Reserved XXXXX PCR_PID
	TSPMS[14]=PCRPid&0xFF; //PCR_PID

	//unsigned int crc=SI::CRC32::crc32((const char *)(TSPMS+5),22,0xFFFFFFFF);

	//TSPMS[27]=(crc>>24); //crc-D1
	//TSPMS[28]=(crc>>16)&0xFF; //crc-D7
	//TSPMS[29]=(crc>>8)&0xFF; //crc-F4
	//TSPMS[30]=crc&0xFF; //crc-B8

}
void cTSRecorder::Activate(bool On)
{
  cReceiver::Activate(On);
  if (On) {
     writer->Start();
     }
}
void cTSRecorder::Receive(uchar *Data, int Length) {
  esyslog("ERROR: cRecorder::Receive(uchar*, int) should not be called any more!");
}
void cTSRecorder::Receive(uchar *Data, int Length, const sTsDataHeader *const Header)
{
/* We have one or more TS packets with the same PID*/
	int numPacket=GetLastPid();
	if(!numPacket)
		packets[0].pictureType=NO_PICTURE;
	int ToWrite=0;
	int nextPictureType=NO_PICTURE;
	do {
		while(ToWrite<Length){
			if((Data[ToWrite+3]&0x20)&&(Data[ToWrite+4]>6)&&(Data[ToWrite+5]&0x10)){
				unsigned int currentTime=(Data[ToWrite+6]<<24)|(Data[ToWrite+7]<<16)|(Data[ToWrite+8]<<8)|Data[ToWrite+9];
				if(!HaveTime){
					HaveTime=true;
					dsyslog("numPacket=%d",numPacket);
					}
				else if(firstPacket[0]){
					if(currentTime>LastTime)
						LastDiff+=(currentTime-LastTime);
					else
						LastDiff+=(LastTime-currentTime);
					}
				LastTime=currentTime;
				}
			if(Data[ToWrite+1]&0x40){
				if(numPacket){
					if((firstPacket[0])&&(!firstPacket[numPacket])){
						firstPacket[numPacket]=1;
						if(bytesToSync[numPacket]) {
							isyslog("RecorderTS, skipped %d bytes to sync on PID %d",bytesToSync[numPacket],Header->pid);
							bytesToSync[numPacket]=0;
							}
						}
					}
				else if(LastDiff>=LAST_DIFF){
					if(packets[0].pesPacketType!=pesPtVideo)
						nextPictureType=I_FRAME;
					else{
						int PESPayload=4+ToWrite;
						if(Data[ToWrite+3]&0x20)
							PESPayload+=Data[ToWrite+4]+1;
						if(PESPayload<ToWrite+180){
							if((!Data[PESPayload])&&(!Data[PESPayload+1])&&(Data[PESPayload+2]==0x01)&&((Data[PESPayload+3]&0xF0)==0xE0)){
								PESPayload+=Data[PESPayload+8]+9;
								if(vType==0x1B){
									if(PESPayload<ToWrite+184){
										if((!Data[PESPayload])&&(!Data[PESPayload+1])){
											PESPayload+=2;
											while((!Data[PESPayload])&&(PESPayload<ToWrite+185))
												PESPayload++;
											if((Data[PESPayload]==0x01)&&(Data[PESPayload+1]==0x09)){
												nextPictureType=(Data[PESPayload+2]>>5)+1;
												break;
												}
											}
										}
									}
								else if(PESPayload<ToWrite+185){
									if((!Data[PESPayload])&&(!Data[PESPayload+1])&&(Data[PESPayload+2]==0x01)){
										switch(Data[PESPayload+3]){
											case 0x00:
												if(PESPayload<ToWrite+183)
													nextPictureType=(Data[PESPayload+5]>>3)&0x07;
												break;
											case 0xB8:
												packets[0].pictureType=I_FRAME;
												break;
											case 0xB3:
												int ToCheck=ToWrite+183;
												for(PESPayload+=12;PESPayload<ToCheck;PESPayload++){
													if((!Data[PESPayload])&&(!Data[PESPayload+1])&&(Data[PESPayload+2]==0x01)&&(!Data[PESPayload+3])){
														nextPictureType=(Data[PESPayload+5]>>3)&0x07;
														break;
														}
													}
												if(PESPayload==ToCheck){
													nextPictureType=I_FRAME;
													dsyslog("Sequence Header without Picture");
													}
											}
										if(nextPictureType!=NO_PICTURE)
											break;
										}
									}
								}
							}
						}
					}
				}
			ToWrite+=TS_SIZE;
			}
		if(ToWrite){
			if(packets[numPacket].pictureType==I_FRAME){
				if(!firstPacket[0]) {
					firstPacket[0]=1;
					if(bytesToSync[0]) {
						isyslog("RecorderTS, skipped %d bytes to sync on PID %d",bytesToSync[0],Header->pid);
						bytesToSync[0]=0;
						}
					}
				if(!resultBuffer->Put(GetPat(),TS_SIZE*2,packets))
					lostBytes+=TS_SIZE*2;
				packets[0].pictureType=NO_PICTURE;
				if(HaveTime)
					LastDiff-=LAST_DIFF;
				}
			else if(!firstPacket[numPacket]) {
					bytesToSync[numPacket]+=ToWrite;
					Data+=ToWrite;
					Length-=ToWrite;
					ToWrite=0;
				}
			else if((packets[numPacket].pictureType!=NO_PICTURE)&&(HaveTime))
				LastDiff-=LAST_DIFF;
			}
		if(ToWrite) {
			if(!resultBuffer->Put(Data,ToWrite,packets+numPacket))
				lostBytes+=ToWrite;
			Data+=ToWrite;
			Length-=ToWrite;
			}
		if(!numPacket){
			packets[0].pictureType=nextPictureType;
			nextPictureType=NO_PICTURE;
			}
		ToWrite=TS_SIZE;
		}while(Length);
}
void cTSRecorder::Del(int Count)
{
  resultBuffer->Del(Count);
}

uchar *cTSRecorder::Get(int &Count,sPesResult *&Header,int &HeaderCount,int &FirstIFrame)
{
  sPesResult *header=NULL;
  int count=0;
  int headerCount;
  uchar *p=resultBuffer->Get(count,header,headerCount);
  if (!p)
     return NULL;

  FirstIFrame=-1;
  for (int i=0; i < headerCount; i++)
      if((header[i].pictureType<=0)||(B_FRAME<header[i].pictureType)) {
         esyslog("ERROR: unknown picture type '0x%02hhX'",header[i].pictureType);
         header[i].pictureType=NO_PICTURE;
         }
      else if ((FirstIFrame==-1)&&(header[i].pictureType==I_FRAME))
         FirstIFrame=i;

  Count=count;
  Header=header;
  HeaderCount=headerCount;
  return p;
}
cIndexFile *cTSRecorder::GetIndexFile(void)
{
	if(writer)
		return writer->GetIndexFile();
	return NULL;
}


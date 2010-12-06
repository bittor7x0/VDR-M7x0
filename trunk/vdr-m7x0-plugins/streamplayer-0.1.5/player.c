#include "player.h"

#include <vdr/remux.h>
#include <vdr/ringbuffer.h>
#include "streamplayer.h"
#include "debug.h"


//
// ---------------------------------------
//


//
// cDataStreamRingBufferLinear
//

cDataStreamRingBufferLinear::cDataStreamRingBufferLinear(cDataStreamSource *src, int Size, int Margin, int Headstart)
	: cDataStreamFilter(src) {

	Buffer=new cRingBufferLinear(Size,Margin);
	full=false;
	MinCollect=Headstart;
}

cDataStreamRingBufferLinear::~cDataStreamRingBufferLinear() {
	delete Buffer;
}

bool cDataStreamRingBufferLinear::Poll() {
	uchar *pSrcBuffer;
	int nSrcBuffer;

	full=false;
	pSrcBuffer=cDataStreamFilter::Get(nSrcBuffer);

	if (pSrcBuffer==NULL || nSrcBuffer<=0) return false;

	int out=Buffer->Put(pSrcBuffer,nSrcBuffer);
	if (out<=0) {
		full=true;
		return false;
	}
	cDataStreamFilter::Del(out);
	return true;
}

uchar* cDataStreamRingBufferLinear::Get(int &Count) {
	// At the beginning, wait for buffer to fill up a bit.
	if (MinCollect && Buffer->Available()<MinCollect) return NULL;
	MinCollect=0;

	// Return some data
	return Buffer->Get(Count);
}
void cDataStreamRingBufferLinear::Del(int Count) {
	Buffer->Del(Count);
}

void cDataStreamRingBufferLinear::Reset() {
	cDataStreamFilter::Reset();
	Buffer->Clear();
}

//
// cDataStreamRingBufferThreaded
//


cDataStreamRingBufferThreaded::cDataStreamRingBufferThreaded(cDataStreamSource *Src, int Size, int Margin, int Headstart)
	: cDataStreamRingBufferLinear(Src,Size,Margin,Headstart) {

	Active=true;
	Start();
}

cDataStreamRingBufferThreaded::~cDataStreamRingBufferThreaded() {
	Stop();
}


void cDataStreamRingBufferThreaded::Action(void) {
	while (Active) {
		while (Poll()) ;

		usleep(3);
	}
}

void cDataStreamRingBufferThreaded::Stop() {
	Active=false;
	Cancel(2);
}


//
// cFilterPIDScan
//

cFilterPIDScan::cFilterPIDScan(cDataStreamSource *Src)
	:cDataStreamProcessor(Src) {
	Scanning=false;
	APid=0xffff;
	VPid=0xffff;
	nAPid=0;
	nVPid=0;
}

void cFilterPIDScan::ProcessData(uchar *data, int Count, int &Forwardable, int &Deletable) {
	Forwardable=0;
	Deletable=0;

	if (!Scanning) {
		Forwardable=Count;
		return;
	}

	while (Count>188 && (nAPid<=2 || nVPid<=2)) {
		if (data[0]!=0x47 || data[188]!=0x47) {
			// Move through buffer until next TS packet
			Forwardable++;
			data++;
			Count--;
		} else {
			// Found TS packet
			int xpid=(data[1]<<8 | data[2]);
			int pid=xpid&0x1fff;

			if (xpid&0x4000) {
				// Stream start (Payload Unit Start Indicator)

				uchar *d=data+4;
				// pointer to payload

				if (data[3]&0x20) d+=d[0]+1;
				// Skip adaption field

				if (data[3]&0x10) {
					// Payload present

					if (d[0]==0 && d[1]==0 && d[2]==1) {
						// PES packet start

						int sid=d[3];
						// Stream ID

						if (0xC0<=sid && sid<=0xDF) {
							if (pid==APid)
								nAPid++;
							else if (pid<APid) {
								APid=pid;
								nAPid=1;
							}
						} else if (0xE0<=sid && sid<=0xEF) {
							if (pid==VPid)
								nVPid++;
							else if (pid<VPid) {
								VPid=pid;
								nVPid=1;
							}
						}
					}
				}
			} // end if payload unit start

			// Mark this packet processed and move to next packet
			Count-=188;
			Forwardable+=188;
			data+=188;

		} // end if TS packet found
	} // end while


	if (nVPid>2 && nAPid>2) {
		// Switch to pass through from now on
		_dprintf("End PIDScan, found %i %ix %i %ix\n",(nVPid>0?VPid:0),nVPid,(nAPid>0?APid:0),nAPid);
		Scanning=false;
		Forwardable=Count;
	} else {
		if (Forwardable>0) _dprintf("PIDScan waiting for data, found %i %ix %i %ix\n",(nVPid>0?VPid:0),nVPid,(nAPid>0?APid:0),nAPid);
	}
}


//
// cFilterFinal
//

cFilterFinal::cFilterFinal(cDataStreamSource *Src, cFilterPIDScan *Scan)
	: cDataStreamFilter(Src) {
	scan=Scan;
}

uchar* cFilterFinal::Get(int &Count) {

	uchar *pData;
	int nData;

	// Nothing in the buffers, get more data for remuxing
	pData=cDataStreamFilter::Get(nData);
	// Get data from source

	if (!pData || nData<=0) {
		return NULL;
	}
	// no source data available, nothing to deliver

//	if (scan->Scanning) return NULL;
	// Still not sure about PIDs.

	Count=nData;
	return pData;

}

void cFilterFinal::Del(int Count) {
	cDataStreamFilter::Del(Count);
}

void cFilterFinal::Reset() {
	cDataStreamFilter::Reset();
}

//
// cFilterThroughput
//

cFilterThroughput::cFilterThroughput(cDataStreamSource *Src)
	: cDataStreamFilter(Src) {

	timeb t;
	ftime(&t);
	last=t.time;
	lastms=t.millitm;

	Bytes=0;
}

void cFilterThroughput::Del(int Count) {
	Bytes+=Count;
	cDataStreamFilter::Del(Count);
}

int cFilterThroughput::GetBytesPerSec() {
	int by;
	timeb t;
	int dt;

	ftime(&t);
	by=Bytes;
	dt=(t.time-last)*100+(t.millitm-lastms)/10;

	Bytes=0;
	last=t.time;
	lastms=t.millitm;

	if (dt==0) return 0;
	return by*100/dt;
}



//
// cStreamPlayer
//

cStreamPlayer::cStreamPlayer(void) /* : cPlayer(pmTsAudioVideo) */ {
	active=false;
	closedown=false;
	Throughput=NULL;
	firstPacket = true;
}

cStreamPlayer::~cStreamPlayer() {
	Detach();
	Stop();
}

void cStreamPlayer::Activate(bool On) {
	if (On){
		active = true;
		Start();
	} else {
		Stop();
	}
}

void cStreamPlayer::Stop(void) {
	int i=100;
	closedown=true;
	while (i>0 && active) {
		usleep(1000);
		i--;
	}
}

cDataStreamSource* cStreamPlayer::CreateFilterChain() {
	Source=NewSource();
	// The derived stream source

	Throughput=new cFilterThroughput(Source);
	// Proxy that mesasures data throughput before buffering

	StreamBuffer=new cDataStreamRingBufferLinear(
	    Throughput,
	    cPluginStreamplayer::Plugin->BufferSize*1024,
	    2*188,
	    cPluginStreamplayer::Plugin->BufferPreFill*1024
	);
	// Buffer incoming data to avoid problems

	ProcessingOut=NewPostBufferFilters(StreamBuffer);
	// Add custom filters by descendants

	PIDScanner=new cFilterPIDScan(ProcessingOut);
	// Scan TS stream for PID

	filterFinal=new cFilterFinal(PIDScanner,PIDScanner);
	// do final filtering

	return filterFinal;
}


void cStreamPlayer::DestroyFilterChain() {
	delete filterFinal;
	filterFinal=NULL;

	delete PIDScanner;
	PIDScanner=NULL;

	DeletePostBufferFilters(ProcessingOut);
	ProcessingOut=NULL;

	delete StreamBuffer;
	StreamBuffer=NULL;

	delete Throughput;
	Throughput=NULL;

	DeleteSource(Source);
	Source=NULL;
}

void cStreamPlayer::Action(void) {
	cDataStreamSource *DataOut;

	// Build up data filter chain
	DataOut=CreateFilterChain();
	// This delivers output for the player

	DevicePlay();
	while(active && !closedown) {
		cPoller Poller;
		int handle=0;

		int nPlayData;
		// Check what data is available in the filter chain
		uchar *pPlayData=DataOut->Get(nPlayData);

		// Get the file handle for which the data source waits, or 0 if buffer filled anyway.
		handle=Source->GetPollHandle();

		// If we wait for a handle, add it to poller
		if (handle) Poller.Add(handle,false);

		if (pPlayData!=NULL && nPlayData>0) {
			// We have data, poll for device write
			DevicePoll(Poller,100);
		} else if (handle) {
			// We have no data, but can wait for a file handle
			Poller.Poll(100);
		} else {
			// Nothing to do, nothing to wait for.
			// Burn some time
			// Keep CPU use low
			usleep(3);
		}

		// Try to fill incoming buffers from network
		while (StreamBuffer->Poll()) {
		}

		// Again, see what data is ready for the player
		pPlayData=DataOut->Get(nPlayData);

        if (firstPacket) {
           PlayPes(NULL, 0);
           firstPacket = false;
        }

        // If play buffer is filled, transmit to player.
		if (pPlayData!=NULL && nPlayData>0) {
			int playedBytes=PlayPes(pPlayData,nPlayData);

//			if (r>0) DataOut->Del(r);
            if (playedBytes > 0) {
            	DataOut->Del(playedBytes);
            } else {
            	_dprintf("ERROR played bytes %d", playedBytes);
            	if (FATALERRNO) {
            		LOG_ERROR;
                  break;
               }
               continue;
            }

		} else {
			// No data reached the play buffer.
			// Check if receive buffer is already filled up and stuck
			if (StreamBuffer->BufferFull()) {
				// Trash all present data
				DataOut->Reset();
			}
		}
	}

	// Clean up filter chain in reverse order
	DestroyFilterChain();

	active=false;
	closedown=false;
}



//
// cStreamControl
//

cStreamControl::cStreamControl(cStreamPlayer *Player)
	:cControl(player = Player) {
	OSDVisible=false;
	ErrorOSD=false;
	ErrorTime=0;
}

cStreamControl::~cStreamControl() {
	delete player;
	player=NULL;
	if (OSDVisible) Interface->Close();
}

void cStreamControl::Hide(void) {
	if (OSDVisible) {
		Interface->Close();

		OSDVisible=false;
	}
}

void cStreamControl::Show(void) {
	if (!OSDVisible) {
		Interface->Open(Setup.OSDwidth, 1);
		OSDVisible=true;
	}
}

void cStreamControl::Draw(void) {
	if (ErrorTime!=0) {
		// Error message blocks OSD

		if (time(NULL)-ErrorTime < 4) return;
		// keep message

		ErrorTime=0;
	}

	// Check for a new error message
	char *Text=GetError();
	if (Text) {
		if (!OSDVisible) {
			// Make OSD visible, but remember to close it afterwards
			ErrorOSD=true;
			Show();
		}
		// Show error
		Interface->Clear();
		Interface->Write(0,0,Text);
		Interface->Flush();
		ErrorTime=time(NULL);
		free(Text);
		return;
	}

	if (ErrorOSD) {
		ErrorOSD=false;
		Hide();
		return;
	}

	if (!OSDVisible) return;

	Interface->Clear();
	Interface->Write(0,0,player->GetStatus());
	Interface->Flush();
}

eOSState cStreamControl::ProcessKey(eKeys Key) {
	if (!Active()) return osEnd;

	switch (Key) {
	case kBack:
		return osEnd;
	case kOk:
		if (OSDVisible) {
			Hide();
		} else {
			Show();
			Draw();
		}
		break;
	case kStop:
		return osEnd;
	case kNone:
		Draw();
		break;
	default:;
	}

	return osUnknown;
}

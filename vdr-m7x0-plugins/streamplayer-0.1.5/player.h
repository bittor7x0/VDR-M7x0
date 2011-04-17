#ifndef __streamplayer_player_h__
#define __streamplayer_player_h__

#include <vdr/plugin.h>
#include <vdr/player.h>
#include <vdr/ringbuffer.h>
#include <vdr/remux.h>
#include <sys/timeb.h>
#include "datastreams.h"
#include "compatibility.h"


//
// cFilterThroughput
//

class cFilterThroughput : public cDataStreamFilter {
	// Proxy filter that measures incoming data rate
protected:
  	unsigned int Bytes;
  	time_t last;
  	unsigned short lastms;
public:
	cFilterThroughput(cDataStreamSource *Src);
	virtual void Del(int Count);
	int GetBytesPerSec();
};

//
// cDataStreamRingBufferLinear
//

class cDataStreamRingBufferLinear : public cDataStreamFilter {
	// Filter that buffers incoming data.
	// To fill the buffer, do frequent calls to Poll().

	cRingBufferLinear *Buffer;
	bool full;
	int MinCollect;

public:
	cDataStreamRingBufferLinear(cDataStreamSource *Src, int Size, int Margin, int Headstart=0);
	// Same options as cRingBufferLinear.
	// Headstart: After initialization, collect some data before giving data away.

	virtual ~cDataStreamRingBufferLinear();
	bool Poll();
	// Get data from source. Return false if no data available.

	bool BufferFull() const { return full; }
	int Available() { return Buffer->Available(); }
	virtual uchar* Get(int &Count);
	virtual void Del(int Count);
	virtual void Reset();
};

class cDataStreamRingBufferThreaded : public cDataStreamRingBufferLinear, public cThread {
	// Filter that buffers incoming data.
	// Buffer is filled by thread

public:
	cDataStreamRingBufferThreaded(cDataStreamSource *Src, int Size, int Margin, int Headstart=0);
	// Same options as cRingBufferLinear.
	// Headstart: After initialization, collect some data before giving data away.

	virtual ~cDataStreamRingBufferThreaded();

protected:
	bool Active;

	virtual void Action(void);
	void Stop();

};

//
// cFilterPIDScan
//

class cFilterPIDScan : public cDataStreamProcessor {
	// Primitive PID scanner. Scan for lowest PIDs with video and audio data.
	// Stop scanning after at least 3 video and audio packets seen on each PID.

protected:
	virtual void ProcessData(uchar *data, int Count, int &Forwardable, int &Deletable);

public:
	bool Scanning;
	int APid,VPid;
	int nAPid,nVPid;

	cFilterPIDScan(cDataStreamSource *Src);
};

//
// cFilterFinal
//

class cFilterFinal : public cDataStreamFilter {
	// Filter-wrapper for cRemux class

protected:
	cFilterPIDScan *scan;
public:
	cFilterFinal(cDataStreamSource *Src, cFilterPIDScan *Scan);
	virtual uchar* Get(int &Count);
	virtual void Del(int Count);
	virtual void Reset();
};


//
// cStreamPlayer
//


class cStreamPlayer : public cPlayer, cThread {
	// Abstract main player class.

protected:
  bool active;
  bool closedown;
  bool firstPacket;

  cDataStreamSource *Source;
  cFilterThroughput *Throughput;
  cDataStreamRingBufferLinear *StreamBuffer;
  cDataStreamSource *ProcessingOut;
  cFilterPIDScan *PIDScanner;
  cFilterFinal *filterFinal;

  cDataStreamSource* CreateFilterChain();
  void DestroyFilterChain();

  virtual void Activate(bool On);
  virtual void Action(void);

  virtual cDataStreamSource* NewSource()=0;
  virtual void DeleteSource(cDataStreamSource *src)=0;
  virtual cDataStreamSource* NewPostBufferFilters(cDataStreamSource *src) { return src; }
  virtual void DeletePostBufferFilters(cDataStreamSource *filter) { }

public:
  cStreamPlayer(void);
  virtual ~cStreamPlayer();
  bool Active(void) { return active; }
  void Stop(void);

  virtual char *GetStatus()=0;
};



class cStreamControl : public cControl {
	// Plugin control class

private:
  cStreamPlayer *player;
  bool OSDVisible;

  bool ErrorOSD;
  time_t ErrorTime;

public:
  cStreamControl(cStreamPlayer *Player);
  virtual ~cStreamControl();
  virtual void Hide(void);
  void Show(void);
  void Draw(void);
  bool Active(void) { return player && player->Active(); }
  virtual eOSState ProcessKey(eKeys Key);
};


#endif

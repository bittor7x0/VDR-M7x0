#ifndef VDR_STREAMDEV_LIVESTREAMER_H
#define VDR_STREAMDEV_LIVESTREAMER_H

#include <vdr/config.h>
#include <vdr/receiver.h>

#include "server/streamer.h"
#include "common.h"

class cTS2PSRemux;
class cTS2ESRemux;
class cExternRemux;
class cRemux;
class cStreamdevPatFilter;
class cStreamdevLiveReceiver;

// --- cStreamdevLiveStreamer -------------------------------------------------

class cStreamdevLiveStreamer: public cStreamdevStreamer {
private:
	int                     m_Priority;
	int                     m_Pids[MAXRECEIVEPIDS + 1];
	int                     m_NumPids;
	eStreamType             m_StreamType;
	const cChannel         *m_Channel;
	cDevice                *m_Device;
	cStreamdevLiveReceiver *m_Receiver;
	cStreamdevPatFilter    *m_PatFilter;
	cRemux                 *m_PESRemux;
	cTS2ESRemux            *m_ESRemux;
	cTS2PSRemux            *m_PSRemux;
	cExternRemux           *m_ExtRemux;

	void StartReceiver(void);
	bool HasPid(int Pid);

public:
	cStreamdevLiveStreamer(int Priority);
	virtual ~cStreamdevLiveStreamer();

	void SetDevice(cDevice *Device) { m_Device = Device; }
	bool SetPid(int Pid, bool On);
	bool SetPids(int Pid, const int *Pids1 = NULL, const int *Pids2 = NULL, const int *Pids3 = NULL);
	bool SetChannel(const cChannel *Channel, eStreamType StreamType, int Apid = 0);
	
	virtual int Put(const uchar *Data, int Count);
	virtual uchar *Get(int &Count);
	virtual void Del(int Count);

	virtual void Attach(void);
	virtual void Detach(void);

	// Statistical purposes:
	virtual std::string Report(void);
};


// --- cStreamdevFilterStreamer -------------------------------------------------

#  if VDRVERSNUM >= 10300

//#include <vdr/status.h>

class cStreamdevLiveFilter;

class cStreamdevFilterStreamer: public cStreamdevStreamer /*, public cStatus*/ {
private:
	cDevice                *m_Device;
	cStreamdevLiveFilter   *m_Filter;
	//const cChannel         *m_Channel;

public:
	cStreamdevFilterStreamer();
	virtual ~cStreamdevFilterStreamer();

	void SetDevice(cDevice *Device);
	//void SetChannel(const cChannel *Channel);
	bool SetFilter(u_short Pid, u_char Tid, u_char Mask, bool On);
	
	virtual void Attach(void);
	virtual void Detach(void);

	// cStatus message handlers
	//virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
};

#  endif // if VDRVERSNUM >= 10300

#endif // VDR_STREAMDEV_LIVESTREAMER_H

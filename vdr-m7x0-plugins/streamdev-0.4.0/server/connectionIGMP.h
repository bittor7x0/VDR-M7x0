/*
 *  $Id: connectionIGMP.h,v 1.1.2.2 2009/02/13 10:39:42 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SERVERS_CONNECTIONIGMP_H
#define VDR_STREAMDEV_SERVERS_CONNECTIONIGMP_H

#include "connection.h"
#include "server/livestreamer.h"

#include <tools/select.h>

#define MULTICAST_PRIV_MIN ((uint32_t) 0xefff0000)
#define MULTICAST_PRIV_MAX ((uint32_t) 0xeffffeff)

class cStreamdevLiveStreamer;

class cConnectionIGMP: public cServerConnection {
private:
	cStreamdevLiveStreamer           *m_LiveStreamer;
	int                               m_ClientPort;
	eStreamType                       m_StreamType;

public:
	cConnectionIGMP(const char* Name, int ClientPort, eStreamType StreamType);
	virtual ~cConnectionIGMP();

	bool Start(cChannel *Channel, in_addr_t Dst);
	void Stop();

	/* Not used here */
	virtual bool Command(char *Cmd) { return false; }

	virtual void Attach(void) { if (m_LiveStreamer != NULL) m_LiveStreamer->Attach(); }
	virtual void Detach(void) { if (m_LiveStreamer != NULL) m_LiveStreamer->Detach(); }

	virtual bool Abort(void) const;
};

inline bool cConnectionIGMP::Abort(void) const
{
	return !m_LiveStreamer || m_LiveStreamer->Abort();
}

#endif // VDR_STREAMDEV_SERVERS_CONNECTIONIGMP_H

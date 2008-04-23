/*
 * zaplist.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Channel entry for the zaphistory. Class wraps a VDR cChannel-instance and adds
 * additionally data like statistics
 */

#ifndef __ZAPHISTORYCHANNEL_H
#define __ZAPHISTORYCHANNEL_H

#include <vdr/channels.h>

#include "setup-zaphistory.h"

class cZapHistoryChannel : public cListObject {
    private:
	// VDRs Channel ID
	tChannelID channelID;
	
	time_t watchTime;
	long zapCount;
	int historyPos;
    public:
	cZapHistoryChannel();
	
	const cChannel *GetChannel();
	tChannelID GetChannelID();
	void SetChannelID(tChannelID id);
		
	long GetWatchTime();
	long GetZapCount();
	int GetHistoryPos();
		
	void IncreaseZapCount();
	void AddWatchTime( time_t t );
	void SetHistoryPos(int p);
	
	bool Parse(char *s);
	bool Save( FILE *f );
	
	void Reset();
	
	virtual int Compare(const cListObject &ListObject) const;
};

#endif

/*
 * zaplist.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __ZAPLIST_H
#define __ZAPLIST_H

#include <vdr/channels.h>
#include <vdr/status.h>

#include "setup-zaphistory.h"
#include "zaphistorychannel.h"

class cZapHistory;

extern cZapHistory ZapHistory;

class cZapHistory : public cConfig<cZapHistoryChannel> {
    private:
	cZapHistoryChannel *currentChannel;	
	time_t lastSwitch;
	
	cZapHistoryChannel* FindChannel( cChannel *channel );
	cZapHistorySortMode sortMode;
	
	void UpdateHistory( cZapHistoryChannel *zapChan);
    public:
	cZapHistory();
	void ChannelSwitch( cChannel *channel );
	void ViewInterrupted();
	void ViewContinue();
	void UpdateWatchTime();
	cZapHistorySortMode GetSortMode();
	void SetSortMode( cZapHistorySortMode mode );
	void Reset(cZapHistoryChannel *zapChan = NULL, bool statisticOnly = true);
};

#endif

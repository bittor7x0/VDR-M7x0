/*
 * zaplist.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/channels.h>

#include "zaplist.h"
#include "setup-zaphistory.h"

cZapHistory ZapHistory;

cZapHistoryChannel* cZapHistory::FindChannel(cChannel *channel) {
    if (channel == NULL)
	return NULL;

    cZapHistoryChannel *returnVal = NULL;

    // Look for channel
    for ( cZapHistoryChannel *zapChan = First(); zapChan; zapChan = Next( zapChan ) ) {
	if ( zapChan->GetChannelID() == channel->GetChannelID() ) {
	    returnVal = zapChan;
	    break;
	}
    }
    
    return returnVal;
}

void cZapHistory::Reset( cZapHistoryChannel *zapChan, bool statisticOnly ) {
    if (zapChan != NULL) { // Operate on ONE channel
	if (statisticOnly) { // reset statistics
	    zapChan->Reset();
	} else { // delete entry
	    if (zapChan == currentChannel)
		currentChannel = NULL;
	    Del(zapChan);
	}
    } else { // operate in complete history
	if (statisticOnly) { // reset statistics
	    for ( cZapHistoryChannel *zapChan = First(); zapChan; zapChan = Next( zapChan ) ) {
		zapChan->Reset();
	    }
	} else { // clear history
	    currentChannel = NULL;
	    
	    while (First() != NULL) {
		Del( First() );
	    }
	}
    }
    
    Save();
}

void cZapHistory::UpdateHistory(cZapHistoryChannel *zapChan) {
    for ( cZapHistoryChannel *chan = First(); chan; chan = Next( chan ) ) {
	chan->SetHistoryPos( chan->GetHistoryPos() + 1);
    }
    
    zapChan->IncreaseZapCount();
    zapChan->SetHistoryPos(1);
    UpdateWatchTime();
}

cZapHistory::cZapHistory() {
    currentChannel = NULL;
    lastSwitch = time(NULL);
    sortMode = historySort;
}

void cZapHistory::ViewInterrupted() {
    // Add view time to current channel before playback of st. is started
    UpdateWatchTime();
    
    lastSwitch = -1;
}

cZapHistorySortMode cZapHistory::GetSortMode() {
    return sortMode;
}

void cZapHistory::SetSortMode( cZapHistorySortMode mode ) {
    sortMode = mode;
}

void cZapHistory::UpdateWatchTime() {
    if (currentChannel == NULL || lastSwitch == -1)
	return;
    
    // Add wiew time since last switch
    currentChannel->AddWatchTime( time(NULL) - lastSwitch );
    
    lastSwitch = time(NULL);
}

void cZapHistory::ViewContinue() {
    lastSwitch = time(NULL);
}

void cZapHistory::ChannelSwitch( cChannel *channel ) {
    cZapHistoryChannel *zapChan = FindChannel( channel );

    // Channel not in history management
    if ( zapChan == NULL ) {
	// Create new history channel
	zapChan = new cZapHistoryChannel();
	zapChan->SetChannelID( channel->GetChannelID() );
	// Add channel to history
	Ins( zapChan );
	UpdateHistory(zapChan);
    } else if (currentChannel != zapChan) { // Channel already in history management and
	// a real switch (another channel)	
	UpdateHistory(zapChan);
    }
    
    currentChannel = zapChan;
    
    // Write history to file
    ZapHistory.Save();
}

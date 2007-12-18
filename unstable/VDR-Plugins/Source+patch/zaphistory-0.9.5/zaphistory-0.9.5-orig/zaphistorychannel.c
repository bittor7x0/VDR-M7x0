/*
 * zaphistorychannel.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/channels.h>

#include "zaplist.h"
#include "setup-zaphistory.h"
#include "zaphistorychannel.h"

cZapHistoryChannel::cZapHistoryChannel() {
    // init members
    //channelID = tChannelID.tChannelID();
    watchTime = 0;
    zapCount = 0;
}

// Getter/Setter for wrapped cChannel
const cChannel *cZapHistoryChannel::GetChannel() {
    return Channels.GetByChannelID(channelID);
}

tChannelID cZapHistoryChannel::GetChannelID() {
    return channelID;
}

void cZapHistoryChannel::SetChannelID(tChannelID id) {
    channelID = id;
}

// Reset statistics
void cZapHistoryChannel::Reset() {
    watchTime = 0;
    zapCount = 0;
}

// Getter for statistics
time_t cZapHistoryChannel::GetWatchTime() {
    return watchTime;
}

// Getter for statistics
long cZapHistoryChannel::GetZapCount() {
    return zapCount;
}

// Increases zap count
void cZapHistoryChannel::IncreaseZapCount() {
    zapCount++;
}

// Increases watch time
void cZapHistoryChannel::AddWatchTime( time_t t) {
    watchTime += t;
}

// Getter for position in history
int cZapHistoryChannel::GetHistoryPos() {
    return historyPos;
}

// Setter for position in history
void cZapHistoryChannel::SetHistoryPos(int p) {
    historyPos = p;
}

// Compares this instance to another cZapHistoryChannel
int cZapHistoryChannel::Compare(const cListObject &ListObject) const {
    // Comparison depends on current sort mode of history
    switch( ZapHistory.GetSortMode() ) {
	case historySort:
	    if ( historyPos == ((cZapHistoryChannel&) ListObject).GetHistoryPos() )
		return 0;
	    if ( historyPos > ((cZapHistoryChannel&) ListObject).GetHistoryPos() )
		return 1;
	
	    return -1;
	case zapcountSort:
	    if ( zapCount == ((cZapHistoryChannel&) ListObject).GetZapCount() )
		return 0;
	    if ( zapCount > ((cZapHistoryChannel&) ListObject).GetZapCount() )
		return -1;
	
	    return 1;
	case watchtimeSort:
	    if ( watchTime == ((cZapHistoryChannel&) ListObject).GetWatchTime() )
		return 0;
	    if ( watchTime > ((cZapHistoryChannel&) ListObject).GetWatchTime() )
		return -1;
	
	    return 1;
    }
    
    return 0;
}

bool cZapHistoryChannel::Parse( char *s ) {
    char *channelbuffer = NULL;
    
    // Parse line from zaphistory.conf
    if ( sscanf( s, "%a[^:]:%d:%ld:%ld", &channelbuffer, &historyPos, &zapCount, &watchTime) == 4) {
	// get channel ID
	channelID = tChannelID::FromString(channelbuffer);							
	// free buffer
	free(channelbuffer);

	// if invalid channel do not add to history
	if (GetChannel() == NULL)
	    return false;	

	// parsed successful	
	return true;	
    }
    
    // could not parse
    return false;
}

bool cZapHistoryChannel::Save( FILE *f ) {
    return fprintf(f, "%s:%d:%ld:%ld\n", *channelID.ToString(), GetHistoryPos(), GetZapCount(), GetWatchTime()  );
}

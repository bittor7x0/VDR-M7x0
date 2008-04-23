#ifndef __ZAPHISTORY_OSDITEM_H
#define __ZAPHISTORY_OSDITEM_H

/* cZapHistoryOsdItem

Base class for history menu items to display a channel
*/

#include <vdr/menuitems.h>
#include "zaphistorychannel.h"

// --- cMenuMyScheduleItem ------------------------------------------------------

class cZapHistoryOsdItem : public cOsdItem {
    protected:
	cZapHistoryChannel *zapChannel;

    public:  
	cZapHistoryOsdItem(cZapHistoryChannel *Channel);
	cZapHistoryChannel *Channel();
};

#endif

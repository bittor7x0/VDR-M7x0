/* * statistic_item.h: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * Provides a menuitem to display channel name and statistics
 */

#include "zaphistoryosd.h"
#include "zaphistory.h"
#include "zaplist.h"
#include "setup-zaphistory.h"
#include "zaphistory_osditem.h"

#include "epg_item.h"

#include <vdr/channels.h>
#include <vdr/menu.h>
#include <vdr/interface.h>

class cZapHistoryChannelItem : public cZapHistoryOsdItem {
    
    public:
	cZapHistoryChannelItem(cZapHistoryChannel *Channel);
	virtual void Set(void);
};

#ifndef __ZAPHISTORY_EPGITEM_H
#define __ZAPHISTORY_EPGITEM_H

/* cMenuMyScheduleItem

Provides a menu item with channel name and EPG information.
Basically taken from the epgsearch plugin
*/

#include <vdr/menuitems.h>
#include "zaphistorychannel.h"
#include "zaphistory_osditem.h"

// --- cMenuMyScheduleItem ------------------------------------------------------

class cMenuMyScheduleItem : public cZapHistoryOsdItem {
private:
  const cEvent *event;
  
  int progress;
  int timerMatch;

public:
  
  cMenuMyScheduleItem(const cEvent *Event, cZapHistoryChannel *Channel, int iProgress=0);
  bool Update(bool Force = false);
};

#endif

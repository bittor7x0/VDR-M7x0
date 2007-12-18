/* * statistic_item.h: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id:
 */

#include "zaphistoryosd.h"
#include "zaphistory.h"
#include "zaplist.h"
#include "setup-zaphistory.h"
#include "statistic_item.h"

#include "epg_item.h"

#include <vdr/channels.h>
#include <vdr/menu.h>
#include <vdr/interface.h>

cZapHistoryChannelItem::cZapHistoryChannelItem(cZapHistoryChannel *Channel):cZapHistoryOsdItem(Channel)
{
  if (Channel->GetChannel()->GroupSep())
     SetSelectable(false);
  Set();
}

void cZapHistoryChannelItem::Set(void)
{
  char *buffer = NULL;
  
  char *timeBuffer = NULL;
  char *zapCountBuffer = NULL;
  
  int hours = zapChannel->GetWatchTime() / 3600;
  int minutes = (int) ( (zapChannel->GetWatchTime() % 3600) / 60 );
  
  // build watch time string: e.g. --42:17
  if (hours >= 1000)
    asprintf( &timeBuffer, "%d:%02d" , hours, minutes );
  else if (hours >= 100)
    asprintf( &timeBuffer, "-%d:%02d" , hours, minutes );
  else if (hours >= 10)
    asprintf( &timeBuffer, "--%d:%02d" , hours, minutes );
  else
    asprintf( &timeBuffer, "---%d:%02d" , hours, minutes );

  // build counter string: e.g. --58
  if (zapChannel->GetZapCount() >= 1000)
    asprintf( &zapCountBuffer, "%ld", zapChannel->GetZapCount() );  
  else if (zapChannel->GetZapCount() >= 100)
    asprintf( &zapCountBuffer, "-%ld", zapChannel->GetZapCount() );  
  else if (zapChannel->GetZapCount() >= 10)
    asprintf( &zapCountBuffer, "--%ld", zapChannel->GetZapCount() );  
  else
    asprintf( &zapCountBuffer, "---%ld", zapChannel->GetZapCount() );  
  
  // build all data into one string
  asprintf(&buffer, "%s\t  %s\t  %s", zapChannel->GetChannel()->Name(), zapCountBuffer, timeBuffer );
  
  SetText(buffer, false);
}

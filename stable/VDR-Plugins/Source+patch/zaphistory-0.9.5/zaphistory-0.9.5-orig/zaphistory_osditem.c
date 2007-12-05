/*
 * zaphistory_osditem.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "zaphistory_osditem.h"

// --- cZapHistoryOsdItem ------------------------------------------------------
cZapHistoryOsdItem::cZapHistoryOsdItem(cZapHistoryChannel *Channel)
{
  zapChannel = Channel;
}

cZapHistoryChannel* cZapHistoryOsdItem::Channel() {
    return zapChannel;
}

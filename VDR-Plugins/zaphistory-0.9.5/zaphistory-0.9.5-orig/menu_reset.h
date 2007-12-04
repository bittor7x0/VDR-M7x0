/*
 * zap_reset.h: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * Provides a menu for reseting opertaions against the zaphistory
 */

#ifndef __MENU_RESET_H
#define __MENU_RESET_H

#include <vdr/osdbase.h>
#include "zaplist.h"
#include "setup-zaphistory.h"

class cMenuHistoryReset : public cOsdMenu {
private:
  void Setup(void);
  
  cZapHistoryChannel *zapChannel;
protected:
  eOSState Switch(void);
public:
  cMenuHistoryReset( cZapHistoryChannel *zapChan);
  virtual eOSState ProcessKey(eKeys Key);
};

#endif

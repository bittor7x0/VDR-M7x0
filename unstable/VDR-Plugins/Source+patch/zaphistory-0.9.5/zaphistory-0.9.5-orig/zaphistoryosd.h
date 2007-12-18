/*
 * zaphistoryosd.h: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * Provides the zaphistory menu
 */

#ifndef __ZAPHISTORYOSD_H
#define __ZAPHISTORYOSD_H

#include <vdr/osdbase.h>
#include "zaplist.h"
#include "setup-zaphistory.h"

class cMenuZappedChannels : public cOsdMenu {
private:
  void Setup(void);
  void SetupEPGView();
  void SetupStatisticView();
  void SortInfoItem();
  eOSState ShowInfo();
  
  cZapHistoryChannel *GetChannel(int Index);
  void Propagate(void);
  
  cZapHistoryView viewMode;
  
  bool inResetMode;
protected:
  eOSState Switch(void);
public:
  cMenuZappedChannels();
  virtual eOSState ProcessKey(eKeys Key);
};

#endif

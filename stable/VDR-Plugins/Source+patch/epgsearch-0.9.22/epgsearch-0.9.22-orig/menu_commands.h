#ifndef __EPGSEARCHCMD_H
#define __EPGSEARCHCMD_H

#include <vdr/menuitems.h>
#include "epgsearchext.h"

// --- cMenuSearchCommands ---------------------------------------------------------

class cMenuSearchCommands : public cOsdMenu {
private:
  cCommands commands;
  eOSState Execute(void);
  const cEvent* event;
  cSearchExt* search;
  bool directCall;
public:
  cMenuSearchCommands(const char *Title, const cEvent* Event, bool DirectCall = false, cSearchExt* Search = NULL);
  virtual ~cMenuSearchCommands();
  virtual eOSState ProcessKey(eKeys Key);
  virtual eOSState Record(void);
  virtual eOSState Switch(void);
  virtual eOSState ExtendedSearch(void);
  virtual eOSState CreateSearchTimer(void);
  virtual eOSState MarkAsRecorded(void);
  virtual eOSState AddToSwitchList(void);
  virtual eOSState CreateBlacklist(void);
  void LoadCommands();
};

#endif

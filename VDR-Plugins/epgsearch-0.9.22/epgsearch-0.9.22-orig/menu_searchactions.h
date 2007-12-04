#ifndef __EPGSEARCHACTIONS_H
#define __EPGSEARCHACTIONS_H

#include <vdr/menuitems.h>
#include "epgsearchext.h"

// --- cMenuSearchActions ---------------------------------------------------------

class cMenuSearchActions : public cOsdMenu {
private:
  cSearchExt* search;
  bool directCall;
  eOSState Search(void);
  eOSState OnOffSearchtimer(void);
  eOSState Execute();
public:
  cMenuSearchActions(cSearchExt* Search, bool DirectCall = false);
  virtual ~cMenuSearchActions();
  virtual eOSState ProcessKey(eKeys Key);
};

#endif

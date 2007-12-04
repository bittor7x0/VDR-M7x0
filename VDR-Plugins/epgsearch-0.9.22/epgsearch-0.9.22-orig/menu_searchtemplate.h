#ifndef __MENU_SEARCHTEMPLATE_H
#define __MENU_SEARCHTEMPLATE_H

#include <vdr/plugin.h>
#include "epgsearchext.h"
#include "blacklist.h"

class cMenuEPGSearchTemplate : public cOsdMenu {
  private:
    bool newSearch;
    eOSState New(void);
    eOSState Delete(void);
    eOSState DefaultTemplate(void);
    cSearchExt* search;
    cBlacklist* blacklist;
 protected:
    virtual eOSState ProcessKey(eKeys Key);
 public:
    cMenuEPGSearchTemplate(cSearchExt* Search, cBlacklist* Blacklist, bool New);
    cSearchExt* CurrentSearchTemplate(void);
};

#endif

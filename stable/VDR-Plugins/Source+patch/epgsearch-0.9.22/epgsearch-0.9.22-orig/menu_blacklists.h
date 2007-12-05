#ifndef __MENU_BLACKLISTS_H
#define __MENU_BLACKLISTS_H

#include <vdr/plugin.h>
#include "blacklist.h"

class cMenuBlacklists : public cOsdMenu {
  private:
    eOSState New(void);
    eOSState Delete(void);
 protected:
    virtual eOSState ProcessKey(eKeys Key);
 public:
    cMenuBlacklists();
    cBlacklist* CurrentBlacklist(void);
};

#endif

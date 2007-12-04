#ifndef __EPGSEARCHFAVORITES_H
#define __EPGSEARCHFAVORITES_H

#include "menu_searchresults.h"

class cMenuFavorites : public cMenuSearchResults {
 private:
    bool BuildList();
    virtual eOSState OnGreen();
    virtual eOSState OnYellow();
 public:
    cMenuFavorites();
    virtual eOSState ProcessKey(eKeys Key);
    virtual void SetHelpKeys(void);
};


#endif

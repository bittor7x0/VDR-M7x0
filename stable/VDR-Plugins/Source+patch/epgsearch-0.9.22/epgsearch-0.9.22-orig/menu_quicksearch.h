#ifndef __MENU_QUICKSEARCH_H
#define __MENU_QUICKSEARCH_H

#include "menu_searchedit.h"
#include "epgsearchtools.h"

// --- cMenuQuickSearch --------------------------------------------------------
class cMenuQuickSearch : public cMenuEditSearchExt {    
    int editmode;
    virtual void Set();
public:
    cMenuQuickSearch(cSearchExt*);
    virtual eOSState ProcessKey(eKeys Key);
  };
 
#endif

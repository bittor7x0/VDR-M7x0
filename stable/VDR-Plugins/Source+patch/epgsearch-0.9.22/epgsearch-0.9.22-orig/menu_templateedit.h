#ifndef __MENU_TEMPLATEEDIT_H
#define __MENU_TEMPLATEEDIT_H

#include "menu_searchedit.h"

// --- cMenuEditTemplate --------------------------------------------------------
class cMenuEditTemplate: public cMenuEditSearchExt {
 public:
  cMenuEditTemplate(cSearchExt *SearchExt, bool New = false) : cMenuEditSearchExt(SearchExt, New, true) 
      {
	  SetTitle(tr("Edit template"));
      }
  virtual eOSState ProcessKey(eKeys Key);
};

#endif
 

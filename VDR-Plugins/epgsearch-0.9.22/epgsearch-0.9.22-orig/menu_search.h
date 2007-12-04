#ifndef __MENU_SEARCH_H
#define __MENU_SEARCH_H

#include <vdr/plugin.h>
#include "epgsearchext.h"

class cMenuEPGSearchExt : public cOsdMenu {
  private:
	eOSState New(void);
	eOSState Delete(void);
	eOSState Actions(eKeys Key);
  protected:
    	virtual eOSState ProcessKey(eKeys Key);
  public:
    	cMenuEPGSearchExt();
	cSearchExt* CurrentSearchExt(void);
	void UpdateTitle();
};

#endif

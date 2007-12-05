#ifndef __MENU_RECSDONE_H
#define __MENU_RECSDONE_H

#include "epgsearchext.h"
#include "recdone.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>

// --- cMenuRecDoneItem ----------------------------------------------------------
class cMenuRecDoneItem : public cOsdItem {
 public:
    cRecDone* recDone;
    cMenuRecDoneItem(cRecDone* RecDone);
    void Set(void);
    int Compare(const cListObject &ListObject) const;
};


class cMenuRecsDone : public cOsdMenu {
  private:
    cSearchExt* search;
    eOSState Delete(void);
    eOSState DeleteAll(void);
    bool showAll;
  protected:
    void Set();
    virtual eOSState ProcessKey(eKeys Key);
    void UpdateTitle();
    eOSState Summary(void);
    cRecDone* CurrentRecDone(void);
 public:
    cMenuRecsDone(cSearchExt* search = NULL);
};

class cMenuTextDone : public cMenuText {
    cRecDone* recDone;
public:
  cMenuTextDone(const char *Title, cRecDone* RecDone, eDvbFont Font = fontOsd) 
      : cMenuText(Title, RecDone->description, Font), recDone(RecDone) 
      {
	  if (recDone->aux) SetHelp(NULL, NULL, NULL, tr("Button$Aux info"));
      }
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif

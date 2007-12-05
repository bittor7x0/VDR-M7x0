#include <vector>
#include "menu_search.h"
#include "epgsearchtools.h"
#include "epgsearchcfg.h"
#include "recdone.h"
#include "menu_searchedit.h"
#include "menu_searchactions.h"

// --- cMenuSearchExtItem ----------------------------------------------------------
class cMenuSearchExtItem : public cOsdItem {
  private:
public:
  cSearchExt* searchExt;
  cMenuSearchExtItem(cSearchExt* SearchExt);
  int Compare(const cListObject &ListObject) const;
  void Set(void);
  };

cMenuSearchExtItem::cMenuSearchExtItem(cSearchExt* SearchExt)
{
  searchExt = SearchExt;
  Set();
}

void cMenuSearchExtItem::Set(void)
{
  char *buffer = NULL;
  char *tmp[]  = { "", "", "", "", "" };

  if (searchExt->useAsSearchTimer)
      asprintf(&tmp[0], ">");

  if (searchExt->search && strlen(searchExt->search) > 0)
      asprintf(&tmp[1], "\t%s", searchExt->search);
  else
      asprintf(&tmp[1], "\t*");

  if (searchExt->useChannel == 1)
  {
      if (searchExt->channelMin != searchExt->channelMax)
	  asprintf(&tmp[2], "\t%d - %d", searchExt->channelMin->Number(),searchExt->channelMax->Number());
      else
	  asprintf(&tmp[2], "\t%.11s", (searchExt->useChannel?CHANNELNAME(searchExt->channelMin):""));
  }
  else if (searchExt->useChannel == 2)
      asprintf(&tmp[2], "\t%.11s", searchExt->channelGroup);
  else 
      asprintf(&tmp[2], "\t");
  

  if (searchExt->useTime)
  {
	  asprintf(&tmp[3], "\t%02d:%02d", searchExt->startTime / 100, searchExt->startTime % 100);
	  asprintf(&tmp[4], "\t%02d:%02d", searchExt->stopTime / 100, searchExt->stopTime % 100);
  }
  else
  {
	  asprintf(&tmp[3], "\t--:--");
	  asprintf(&tmp[4], "\t--:--");
  }
	
  asprintf(&buffer, "%s%s%s%s%s", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
  SetText(buffer, false);
}

int cMenuSearchExtItem::Compare(const cListObject &ListObject) const
{
  cMenuSearchExtItem *p = (cMenuSearchExtItem *)&ListObject;
  return strcasecmp(searchExt->search, p->searchExt->search);
}

// --- cMenuEPGSearchExt ----------------------------------------------------------
cMenuEPGSearchExt::cMenuEPGSearchExt()
:cOsdMenu("", 2, 20, 11, 6, 5)
{
    cMutexLock SearchExtsLock(&SearchExts);
    cSearchExt *SearchExt = SearchExts.First();
    while (SearchExt) {	
	Add(new cMenuSearchExtItem(SearchExt));
	SearchExt = SearchExts.Next(SearchExt);
    }

    UpdateTitle();
    SetHelp(tr("Button$Edit"), tr("Button$New"), tr("Button$Delete"), tr("Button$Actions"));
    Sort();
}

void cMenuEPGSearchExt::UpdateTitle()
{
    int total=0, active=0;
    cMutexLock SearchExtsLock(&SearchExts);
    cSearchExt *SearchExt = SearchExts.First();
    while (SearchExt) {	
	if (SearchExt->useAsSearchTimer) active++;
	SearchExt = SearchExts.Next(SearchExt);
	total++;
    }

    char *buffer = NULL;
    asprintf(&buffer, "%s (%d/%d %s)", tr("Search entries"), active, total, tr("active"));
    SetTitle(buffer);
    Display();
    free(buffer);
}

cSearchExt *cMenuEPGSearchExt::CurrentSearchExt(void)
{
    cMenuSearchExtItem *item = (cMenuSearchExtItem *)Get(Current());
    if (item && SearchExts.Exists(item->searchExt))
	return item->searchExt;
    return NULL;
}


eOSState cMenuEPGSearchExt::New(void)
{
  if (HasSubMenu())
    return osContinue;
  return AddSubMenu(new cMenuEditSearchExt(new cSearchExt, true));
}

eOSState cMenuEPGSearchExt::Delete(void)
{
    cSearchExt *curSearchExt = CurrentSearchExt();
    if (curSearchExt) {
	if (Interface->Confirm(tr("Edit$Delete search?"))) {
	    int DelID = curSearchExt->ID;
	    if (Interface->Confirm(tr("Delete all timers created from this search?")))
		curSearchExt->DeleteAllTimers();
	    LogFile.Log(1,"search timer %s (%d) deleted", curSearchExt->search, curSearchExt->ID);
	    cMutexLock SearchExtsLock(&SearchExts);
	    SearchExts.Del(curSearchExt);
	    SearchExts.Save();
	    RecsDone.RemoveSearchID(DelID);
	    cOsdMenu::Del(Current());
	    Display();
	    UpdateTitle();
	}
    }
    return osContinue;
}

eOSState cMenuEPGSearchExt::Actions(eKeys Key)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cSearchExt* search = CurrentSearchExt();
  if (search) {
      cMenuSearchActions *menu;
      eOSState state = AddSubMenu(menu = new cMenuSearchActions(search, true));
      if (Key != kNone)
	  state = menu->ProcessKey(Key);
      return state;
  }
  return osContinue;
}


eOSState cMenuEPGSearchExt::ProcessKey(eKeys Key)
{
  int SearchNumber = HasSubMenu() ? Count() : -1;
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key) {
	case k0:
	    if (HasSubMenu())
		return osContinue;	    
	    if (CurrentSearchExt())
		state = AddSubMenu(new cMenuSearchActions(CurrentSearchExt()));
	    else
		state = osContinue;
	    break;
	case k1...k9:
	    return Actions(Key);
      	case kOk:
	    state = Actions(k1);
	case kBlue:
	    if (HasSubMenu())
		return osContinue;	    
	    if (CurrentSearchExt())
		state = AddSubMenu(new cMenuSearchActions(CurrentSearchExt()));
	    else
		state = osContinue;
	    break;
	case kRed:
 	  if (HasSubMenu())
	    return osContinue;
	  if (CurrentSearchExt())
	      state = AddSubMenu(new cMenuEditSearchExt(CurrentSearchExt()));
	  else
	      state = osContinue;
	  break;
     	case kGreen: state = New(); break;
        case kYellow: state = Delete(); break;
      default: break;
    }
  }
  if (SearchNumber >= 0 && !HasSubMenu()) 
  {      
      cMutexLock SearchExtsLock(&SearchExts);
      cSearchExt* search = SearchExts.Get(SearchNumber);
      if (search)       // a newly created search was confirmed with Ok
	  Add(new cMenuSearchExtItem(search));
      else
	  search = CurrentSearchExt();
      // always update all entries, since channel group names may have changed and affect other searches
      Sort();
      for(int i=0; i<Count(); i++)
      {
	  cMenuSearchExtItem *item = (cMenuSearchExtItem *)Get(i);
	  if (item)
	  {
	      item->Set();
	      if (item->searchExt == search)
		  SetCurrent(item);
	  }
      }
      Display();
      UpdateTitle();
  }

  return state;
}

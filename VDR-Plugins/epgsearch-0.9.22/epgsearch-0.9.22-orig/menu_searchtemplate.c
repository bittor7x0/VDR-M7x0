#include <vector>
#include "menu_searchtemplate.h"
#include "epgsearchtools.h"
#include "epgsearchcfg.h"
#include "recdone.h"
#include "menu_templateedit.h"
#include "menu_searchactions.h"

// --- cMenuSearchTemplateItem ----------------------------------------------------------
class cMenuSearchTemplateItem : public cOsdItem {
  private:
public:
  cSearchExt* searchExt;
  cMenuSearchTemplateItem(cSearchExt* SearchExt);
  int Compare(const cListObject &ListObject) const;
  void Set(void);
  };

cMenuSearchTemplateItem::cMenuSearchTemplateItem(cSearchExt* SearchExt)
{
    searchExt = SearchExt;
    Set();
}

void cMenuSearchTemplateItem::Set(void)
{
  char *buffer = NULL;
  char *tmp[]  = { "", "", "", "", "" };

  if (searchExt->useAsSearchTimer)
      asprintf(&tmp[0], ">");
 
  if (searchExt->ID == EPGSearchConfig.DefSearchTemplateID)
      asprintf(&tmp[0], "*");

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

int cMenuSearchTemplateItem::Compare(const cListObject &ListObject) const
{
  cMenuSearchTemplateItem *p = (cMenuSearchTemplateItem *)&ListObject;
  return strcasecmp(searchExt->search, p->searchExt->search);
}

// --- cMenuEPGSearchTemplate ----------------------------------------------------------
cMenuEPGSearchTemplate::cMenuEPGSearchTemplate(cSearchExt* Search, cBlacklist* Blacklist, bool New)
:cOsdMenu(tr("Search templates"), 2, 20, 11, 6, 5)
{
    search = Search;
    blacklist = Blacklist;
    newSearch = New;
    cMutexLock SearchTemplatesLock(&SearchTemplates);
    cSearchExt *SearchExt = SearchTemplates.First();
    while (SearchExt) {	
	Add(new cMenuSearchTemplateItem(SearchExt));
	SearchExt = SearchTemplates.Next(SearchExt);
    }
    SetHelp(tr("Button$Edit"), tr("Button$New"), tr("Button$Delete"), tr("Button$Default"));
    Sort();
    Display();

}

cSearchExt *cMenuEPGSearchTemplate::CurrentSearchTemplate(void)
{
    cMenuSearchTemplateItem *item = (cMenuSearchTemplateItem *)Get(Current());
    return item ? item->searchExt : NULL;
}

eOSState cMenuEPGSearchTemplate::New(void)
{
  if (HasSubMenu())
    return osContinue;
  return AddSubMenu(new cMenuEditTemplate(new cSearchExt, true));
}

eOSState cMenuEPGSearchTemplate::Delete(void)
{
    cSearchExt *curSearchExt = CurrentSearchTemplate();
    if (curSearchExt) {
	if (Interface->Confirm(tr("Edit$Delete template?"))) {
	    cMutexLock SearchTemplatesLock(&SearchTemplates);
	    SearchTemplates.Del(curSearchExt);
	    SearchTemplates.Save();
	    cOsdMenu::Del(Current());
	    Display();
	}
    }
    return osContinue;
}

eOSState cMenuEPGSearchTemplate::DefaultTemplate(void)
{
  if (HasSubMenu())
    return osContinue;
  cSearchExt *curSearchExt = CurrentSearchTemplate();

  int current = Current();
  cMenuSearchTemplateItem *oldDefaultTemplateItem = NULL;
  for(int i=0; i<Count(); i++)
  {
      cMenuSearchTemplateItem *item = (cMenuSearchTemplateItem *)Get(i);
      if (item && item->searchExt && item->searchExt->ID == EPGSearchConfig.DefSearchTemplateID)
	  oldDefaultTemplateItem = item;
  }

  if (curSearchExt)
  {
      if (EPGSearchConfig.DefSearchTemplateID == curSearchExt->ID)
	  EPGSearchConfig.DefSearchTemplateID = -1;
      else
	  EPGSearchConfig.DefSearchTemplateID = curSearchExt->ID;
      cPluginManager::GetPlugin("epgsearch")->SetupStore("DefSearchTemplateID",  EPGSearchConfig.DefSearchTemplateID);

      SetCurrent(oldDefaultTemplateItem);
      RefreshCurrent();
      DisplayCurrent(true);

      SetCurrent(Get(current));
      RefreshCurrent();
      DisplayCurrent(true);

      Display();
  }
  return osContinue;
}

eOSState cMenuEPGSearchTemplate::ProcessKey(eKeys Key)
{
  int SearchNumber = HasSubMenu() ? Count() : -1;
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
      if (HasSubMenu())
	  return osContinue;
      switch (Key) {
	  case kOk:
	  {
	      if (search)
	      {
		  if (!newSearch && !Interface->Confirm(tr("Overwrite existing entries?")))
		      return osBack;
		  // duplicate template
		  cSearchExt* t =CurrentSearchTemplate();	      
		  if (!t) return osContinue;
		  // copy all except the name and id
		  search->CopyFromTemplate(t);
	      }
	      if (blacklist)
	      {
		  if (!newSearch && !Interface->Confirm(tr("Overwrite existing entries?")))
		      return osBack;
		  // duplicate template
		  cSearchExt* t =CurrentSearchTemplate();	      
		  if (!t) return osContinue;
		  // copy all except the name and id
		  blacklist->CopyFromTemplate(t);
	      }
	      return osBack;
	  }
	  case kRed:
	      if (CurrentSearchTemplate())
		  state = AddSubMenu(new cMenuEditTemplate(CurrentSearchTemplate()));
	      else
		  state = osContinue;
	      break;
	  case kGreen: state = New(); break;
	  case kYellow: state = Delete(); break;
	  case kBlue: state = DefaultTemplate(); break;
	  default: break;
      }
  }
  if (SearchNumber >= 0 && !HasSubMenu()) 
  {      
      cSearchExt* search = SearchTemplates.Get(SearchNumber);
      if (search)       // a newly created template was confirmed with Ok
	  Add(new cMenuSearchTemplateItem(search));
      else
	  search = CurrentSearchTemplate();
      // always update all entries, since channel group names may have changed and affect other searches
      Sort();
      for(int i=0; i<Count(); i++)
      {
	  cMenuSearchTemplateItem *item = (cMenuSearchTemplateItem *)Get(i);
	  if (item)
	  {
	      item->Set();
	      if (item->searchExt == search)
		  SetCurrent(item);
	  }
      }
      Display();
  }

  return state;
}

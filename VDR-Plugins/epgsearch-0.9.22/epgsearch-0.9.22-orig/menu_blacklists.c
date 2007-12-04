#include <vector>
#include "menu_blacklists.h"
#include "epgsearchtools.h"
#include "menu_blacklistedit.h"
#include "epgsearchcfg.h"
#include "menu_searchresults.h"

// --- cMenuBlacklistsItem ----------------------------------------------------------
class cMenuBlacklistsItem : public cOsdItem {
  private:
public:
  cBlacklist* blacklist;
  cMenuBlacklistsItem(cBlacklist* Blacklist);
  int Compare(const cListObject &ListObject) const;
  void Set(void);
  };

cMenuBlacklistsItem::cMenuBlacklistsItem(cBlacklist* Blacklist)
{
    blacklist = Blacklist;
    Set();
}

void cMenuBlacklistsItem::Set(void)
{
  char *buffer = NULL;
  char *tmp[]  = { "", "", "", "", "" };

  if (blacklist->search && strlen(blacklist->search) > 0)
      asprintf(&tmp[1], "%s", blacklist->search);
  else
      asprintf(&tmp[1], "*");

  if (blacklist->useChannel == 1)
  {
      if (blacklist->channelMin != blacklist->channelMax)
	  asprintf(&tmp[2], "\t%d - %d", blacklist->channelMin->Number(),blacklist->channelMax->Number());
      else
	  asprintf(&tmp[2], "\t%.11s", (blacklist->useChannel?CHANNELNAME(blacklist->channelMin):""));
  }
  else if (blacklist->useChannel == 2)
      asprintf(&tmp[2], "\t%.11s", blacklist->channelGroup);
  else 
      asprintf(&tmp[2], "\t");
  

  if (blacklist->useTime)
  {
	  asprintf(&tmp[3], "\t%02d:%02d", blacklist->startTime / 100, blacklist->startTime % 100);
	  asprintf(&tmp[4], "\t%02d:%02d", blacklist->stopTime / 100, blacklist->stopTime % 100);
  }
  else
  {
	  asprintf(&tmp[3], "\t--:--");
	  asprintf(&tmp[4], "\t--:--");
  }
	
  asprintf(&buffer, "%s%s%s%s%s", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]);
  SetText(buffer, false);
}

int cMenuBlacklistsItem::Compare(const cListObject &ListObject) const
{
  cMenuBlacklistsItem *p = (cMenuBlacklistsItem *)&ListObject;
  return strcasecmp(blacklist->search, p->blacklist->search);
}

// --- cMenuBlacklists ----------------------------------------------------------
cMenuBlacklists::cMenuBlacklists()
:cOsdMenu(tr("Blacklists"), 20, 11, 6, 5)
{
    cMutexLock BlacklistLock(&Blacklists);
    cBlacklist *Blacklist = Blacklists.First();
    while (Blacklist) {	
	Add(new cMenuBlacklistsItem(Blacklist));
	Blacklist = Blacklists.Next(Blacklist);
    }
    SetHelp(tr("Button$Edit"), tr("Button$New"), tr("Button$Delete"), NULL);
    Sort();
    Display();

}

cBlacklist *cMenuBlacklists::CurrentBlacklist(void)
{
    cMenuBlacklistsItem *item = (cMenuBlacklistsItem *)Get(Current());
    if (item && Blacklists.Exists(item->blacklist))
	return item->blacklist;
    return NULL;
}

eOSState cMenuBlacklists::New(void)
{
    if (HasSubMenu())
	return osContinue;
    return AddSubMenu(new cMenuBlacklistEdit(new cBlacklist, true));
}

eOSState cMenuBlacklists::Delete(void)
{
    cBlacklist *curBlacklist = CurrentBlacklist();
    if (curBlacklist) {
	if (Interface->Confirm(tr("Edit$Delete blacklist?"))) {
	    LogFile.Log(1,"blacklist %s (%d) deleted", curBlacklist->search, curBlacklist->ID);
	    SearchExts.RemoveBlacklistID(curBlacklist->ID);
	    cMutexLock BlacklistLock(&Blacklists);
	    Blacklists.Del(curBlacklist);
	    Blacklists.Save();
	    cOsdMenu::Del(Current());
	    Display();
	}
    }
    return osContinue;
}

eOSState cMenuBlacklists::ProcessKey(eKeys Key)
{
  int BlacklistNumber = HasSubMenu() ? Count() : -1;
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
      if (HasSubMenu())
	  return osContinue;
      switch (Key) {
	  case kOk:
	      return AddSubMenu(new cMenuSearchResultsForBlacklist(CurrentBlacklist()));
	      break;
	  case kRed:
	      if (CurrentBlacklist())
		  state = AddSubMenu(new cMenuBlacklistEdit(CurrentBlacklist()));
	      else
		  state = osContinue;
	      break;
	  case kGreen: state = New(); break;
	  case kYellow: state = Delete(); break;
	  default: break;
      }
  }
  if (BlacklistNumber >= 0 && !HasSubMenu()) 
  {      
      cMutexLock BlacklistLock(&Blacklists);
      cBlacklist* Blacklist = Blacklists.Get(BlacklistNumber);
      if (Blacklist)       // a newly created search was confirmed with Ok
	  Add(new cMenuBlacklistsItem(Blacklist));
      // always update all entries, since channel group names may have changed and affect other searches
      Sort();
      for(int i=0; i<Count(); i++)
      {
	  cMenuBlacklistsItem *item = (cMenuBlacklistsItem *)Get(i);
	  if (item)
	  {
	      item->Set();
	      if (item->blacklist == Blacklist)
		  SetCurrent(item);
	  }
      }
      Display();
  }

  return state;
}

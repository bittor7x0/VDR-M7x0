#include <set>
#include "menu_announcelist.h"
#include "epgsearchcfg.h"
#include "noannounce.h"

bool cMenuAnnounceList::showsDetails;

// --- cMenuAnnounceList -------------------------------------------------------
cMenuAnnounceList::cMenuAnnounceList(cSearchResults& SearchResults)
   :cMenuSearchResultsForList(SearchResults,  tr("%d new broadcast"), false)
{
   showsDetails = false;
}

void cMenuAnnounceList::SetHelpKeys()
{
   cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
   bool hasTimer = item && item->Selectable() && item->timerMatch == tmFull;

   if (toggleKeys==0)
      SetHelp((EPGSearchConfig.redkeymode==0?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), m_bSort? tr("Button$by channel"):tr("Button$by time"), modeYellow==showTitleEpisode?tr("Button$Episode"):tr("Button$Title"), tr("Button$Edit"));
   else
      SetHelp((EPGSearchConfig.redkeymode==1?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), m_bSort? tr("Button$by channel"):tr("Button$by time"), modeYellow==showTitleEpisode?tr("Button$Episode"):tr("Button$Title"), tr("Button$Edit"));
}

eOSState cMenuAnnounceList::ProcessKey(eKeys Key)
{
   eOSState state = cMenuSearchResultsForList::ProcessKey(Key);
   if (state == osUnknown)
   {
      switch (Key) {
         case kBlue:
         {
            cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
            if (item)
            {
               if (!HasSubMenu())
                  return AddSubMenu(new cMenuAnnounceDetails(item->event, item->search)); 
               else if (!showsDetails)
                  return Switch();
               else 
                  return osContinue;
            }
         }
         break;
         default:      
            break;
      }
   }
   return state;
}

// --- cMenuAnnounceDetails -------------------------------------------------------
cMenuAnnounceDetails::cMenuAnnounceDetails(const cEvent* Event, const cSearchExt* Search)
   :cOsdMenu("", 25), event(Event)
{
   cMenuAnnounceList::showsDetails = true;
   char* szTitle = NULL;
   if (event && !isempty(event->Title()))
   {
      asprintf(&szTitle, tr("announce details: %s"), event->Title());
      SetTitle(szTitle);
      free(szTitle);
   }
   search = Search;

   announceAgain = 1;
   announceWithNextUpdate = 1;
   announceAgainDay = 0;

   cNoAnnounce* noAnnounce = NoAnnounces.InList(event);
   if (noAnnounce)
   {
      if (noAnnounce->nextAnnounce > 0)
      {
         announceAgainDay = noAnnounce->nextAnnounce;
         announceWithNextUpdate = 0;
      }
      else
         announceAgain = 0;
   }
   Set();   
}

cMenuAnnounceDetails::~cMenuAnnounceDetails()
{
   cMenuAnnounceList::showsDetails = false;
}

void cMenuAnnounceDetails::Set()
{
   int current = Current();
   Clear();
  
   Add(new cMenuEditBoolItem(tr("announce again"), &announceAgain, tr("no"), tr("yes")));
   if (announceAgain)
   {
      Add(new cMenuEditBoolItem(IndentMenuItem(tr("with next update")), &announceWithNextUpdate, tr("no"), tr("yes")));      
      if (!announceWithNextUpdate)
         Add(new cMenuEditDateItem(IndentMenuItem(IndentMenuItem(tr("again from"))), &announceAgainDay, NULL));      
   }
   else
      announceAgainDay = 0;

   if (search)
   {
      cOsdItem* pInfoItem = new cOsdItem("");
      pInfoItem->SetSelectable(false);
      Add(pInfoItem);
      
      char* info = NULL;
      asprintf(&info, "%s: %s", tr("Search timer"), search->search);
      pInfoItem = new cOsdItem(info);
      free(info);
      pInfoItem->SetSelectable(false);
      Add(pInfoItem);
   }

   SetCurrent(Get(current));
   Display();
}

eOSState cMenuAnnounceDetails::ProcessKey(eKeys Key)
{
   int iTemp_announceAgain = announceAgain;
   int iTemp_announceWithNextUpdate = announceWithNextUpdate;

   eOSState state = cOsdMenu::ProcessKey(Key);

   if (iTemp_announceAgain != announceAgain ||
       iTemp_announceWithNextUpdate != announceWithNextUpdate)
   {
      Set();
      Display();
   }

   if (state == osUnknown) {
      switch (Key) {
         case kRed: 
         case kGreen:
         case kBlue:
         case kYellow:
         case kFastRew:
         case kFastFwd:
         case kRecord:
         case k0...k9:
            return osContinue;
         case kOk:
         {
            cNoAnnounce* noAnnounce = NoAnnounces.InList(event);
            if (!announceAgain || !announceWithNextUpdate)
            {
               if (!noAnnounce)
               {
                  noAnnounce = new cNoAnnounce(event, announceAgainDay);             
                  NoAnnounces.Add(noAnnounce);
               }
               else
                  NoAnnounces.UpdateNextAnnounce(event, announceAgainDay);
               NoAnnounces.ClearOutdated();
            }
            if (announceAgain && announceWithNextUpdate && noAnnounce)
               NoAnnounces.Del(noAnnounce);
            NoAnnounces.Save(); 
         }
         return osBack;
         default:      
            break;
      }
   }

   return state;
}

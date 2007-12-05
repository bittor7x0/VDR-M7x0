#ifndef __ANNOUNCELIST_H
#define __ANNOUNCELIST_H

#include "menu_searchresults.h"

// --- cMenuAnnounceList ------------------------------------------------------
class cMenuAnnounceList : public cMenuSearchResultsForList {
   friend class cMenuAnnounceDetails;

   virtual void SetHelpKeys(void);
   eOSState ProcessKey(eKeys Key);
  protected:
   static bool showsDetails;
  public:
   cMenuAnnounceList(cSearchResults& SearchResults);
};

// --- cMenuAnnounceDetails ------------------------------------------------------
class cMenuAnnounceDetails : public cOsdMenu {
   const cEvent* event;
   const cSearchExt* search;
   int announceAgain;
   int announceWithNextUpdate;
   time_t announceAgainDay;
   eOSState ProcessKey(eKeys Key);
   void Set();
  public:
   cMenuAnnounceDetails(const cEvent* Event, const cSearchExt* Search);
   ~cMenuAnnounceDetails();
};
#endif

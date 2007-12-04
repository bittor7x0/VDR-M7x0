#ifndef __EPGSEARCH_MENUMAIN_H
#define __EPGSEARCH_MENUMAIN_H

#include <vdr/menuitems.h>
#include "menu_event.h"

// --- cMenuSearchMain ---------------------------------------------------------

class cMenuSearchMain : public cOsdMenu {
  private:
   cSchedulesLock schedulesLock;
   const cSchedules *schedules;
   int otherChannel;
   int currentChannel;
   eOSState Record(void);
   eOSState ExtendedSearch(void);
   void PrepareSchedule(cChannel *Channel);
   eOSState Commands(eKeys Key);
   void SetHelpKeys();
   int GetTab(int Tab);
   int shiftTime;
   bool InWhatsOnMenu;
   bool InFavoritesMenu;
   cEventObjects eventObjects;
  public:
   cMenuSearchMain(void);
   virtual ~cMenuSearchMain();
   virtual eOSState ProcessKey(eKeys Key);
   eOSState Switch(void);
   eOSState Shift(int iMinutes);
   eOSState ShowSummary();
   bool Update(void);
};

#endif

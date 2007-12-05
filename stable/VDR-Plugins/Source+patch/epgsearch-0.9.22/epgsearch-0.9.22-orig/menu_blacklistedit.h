#ifndef __MENU_BLACKLISTEDIT_H
#define __MENU_BLACKLISTEDIT_H

#include "blacklist.h"

// --- cMenuBlacklistEdit --------------------------------------------------------
class cMenuBlacklistEdit: public cOsdMenu {
protected:
  cBlacklist *blacklist;
  cBlacklist data;
  int channelMin;
  int channelMax;
  bool addIfConfirmed;
  int UserDefDayOfWeek;
  int channelGroupNr;
  char* channelGroupName;
  char** menuitemsChGr;

  char *SearchModes[6];
  char *DaysOfWeek[8];
  char *UseChannelSel[4];

 public:
  cMenuBlacklistEdit(cBlacklist *Blacklist, bool New = false);
  virtual eOSState ProcessKey(eKeys Key);
  virtual ~cMenuBlacklistEdit();
  virtual void Set();
  void CreateMenuitemsChannelGroups();
};

#endif

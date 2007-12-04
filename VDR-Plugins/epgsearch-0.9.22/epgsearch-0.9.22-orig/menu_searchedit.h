#ifndef __MENU_SEARCHEDIT_H
#define __MENU_SEARCHEDIT_H

#include <vector>
#include <string>
#include "epgsearchext.h"


// --- cMenuEditSearchExt --------------------------------------------------------
class cMenuEditSearchExt : public cOsdMenu {
protected:
  cSearchExt *searchExt;
  cSearchExt data;
  int channelMin;
  int channelMax;
  bool addIfConfirmed;
  int UserDefDayOfWeek;
  int channelGroupNr;
  char* channelGroupName;
  char** menuitemsChGr;
  int* catarrayAvoidRepeats;
  cList<cBlacklistObject> blacklists;

  char *SearchModes[6];
  char *DaysOfWeek[8];
  char *UseChannelSel[4];
  char *SearchTimerModes[3];
  char *BlacklistModes[3];
  bool templateMode;

public:
  cMenuEditSearchExt(cSearchExt *SearchExt, bool New = false, bool Template = false);
  virtual ~cMenuEditSearchExt();
  virtual void Set();
  virtual eOSState ProcessKey(eKeys Key);
  };
 
// --- cMenuEditDaysOfWeek --------------------------------------------------------
class cMenuEditDaysOfWeek : public cOsdMenu {
private:
  int* pDaysOfWeek;
  int Days[7];
  int offset;
  bool negate;
public:
  cMenuEditDaysOfWeek(int* DaysOfWeek, int Offset=0, bool Negate = true);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cMenuSearchEditCompCats --------------------------------------------------------
class cMenuSearchEditCompCats : public cOsdMenu {
 private:
    int* search_catarrayAvoidRepeats;
    int* edit_catarrayAvoidRepeats;
public:
  cMenuSearchEditCompCats(int* catarrayAvoidRepeats);
  ~cMenuSearchEditCompCats();
  eOSState ProcessKey(eKeys Key);
  };

// --- cMenuBlacklistsSelection --------------------------------------------------------
class cMenuBlacklistsSelection : public cOsdMenu {
 private:
    int* blacklistsSel;
    cList<cBlacklistObject>* blacklists;
public:
    cMenuBlacklistsSelection(cList<cBlacklistObject>* pBlacklists);
    ~cMenuBlacklistsSelection();
    void Set();
    eOSState ProcessKey(eKeys Key);
};

// --- cMenuCatValuesSelect --------------------------------------------------------
class cMenuCatValuesSelect : public cOsdMenu {
 private:
    char* catValues;
    int catIndex;
    int searchMode;
    std::vector<bool> sel_cats;
public:
  cMenuCatValuesSelect(char* CatValues, int CatIndex, int SearchMode);
  void Set();
  eOSState ProcessKey(eKeys Key);
  };

#endif

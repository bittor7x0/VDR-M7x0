/*
 * ZapHistory plugin for VDR
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 * Setup data structure and setup menu
 *
 */

#ifndef __SETUP_ZapHistory_H
#define __SETUP_ZapHistory_H

#include <vdr/menuitems.h>

enum cZapHistoryView { historyView, statisticView };
enum cZapHistorySortMode { historySort, zapcountSort, watchtimeSort };

class cZapHistorySetup {
 public:
    int HideMainMenu;
    int EntryCount;
    cZapHistoryView DefaultView;
    cZapHistorySortMode DefaultSort;
    
    // Dummy fields to convert enum fields to int
    int DummyDefaultView;
    int DummyDefaultSort;
    
 public:
    cZapHistorySetup(void);
    void IntToEnum();
    bool SetupParse(const char *Name, const char *Value);
};

class cMenuSetupZapHistory : public cMenuSetupPage {
 private:
    const char* viewStrs[2];
    const char* sortStrs[3];
 protected:
    virtual void Store(void);
 public:
    cMenuSetupZapHistory(void);
};

extern cZapHistorySetup ZapHistorySetup;

#endif // __SETUP_ZapHistory_H

/*
 * ZapHistory Player plugin for VDR
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. Read the file COPYING for details.
 */

#include "zaphistory.h"
#include "setup-zaphistory.h"
#include "i18n.h"
#ifdef __QNXNTO__
#include <strings.h>
#include <vdr/channels.h>

#endif

cZapHistorySetup ZapHistorySetup;

// --- cZapHistorySetup -----------------------------------------------------------
cZapHistorySetup::cZapHistorySetup(void)
{
    // set default values
    HideMainMenu  = 0;
    DefaultView = historyView;
    DefaultSort = historySort;
    EntryCount = 10;
}

void cZapHistorySetup::IntToEnum() {
    // Convert int-dummy values to enum
    switch (DummyDefaultView) {
	case 0: DefaultView = historyView; break;
	case 1: DefaultView = statisticView; break;
    }
    
    switch (DummyDefaultSort) {
	case 0: DefaultSort = historySort; break;
	case 1: DefaultSort = zapcountSort; break;
	case 2: DefaultSort = watchtimeSort; break;
    }
}

bool cZapHistorySetup::SetupParse(const char *Name, const char *Value)
{
    // Parse your own setup parameters and store their values.
    if (!strcasecmp(Name, "HideMainMenu"))  HideMainMenu  = atoi(Value);
    else if (!strcasecmp(Name, "EntryCount"))  EntryCount  = atoi(Value);
    else if (!strcasecmp(Name, "DefaultView"))  { DummyDefaultView = atoi(Value); IntToEnum(); }
    else if (!strcasecmp(Name, "DefaultSort")) { DummyDefaultSort = atoi(Value); IntToEnum(); }
    else
	return false;
    return true;
}

// --- cMenuSetupZapHistory --------------------------------------------------------

cMenuSetupZapHistory::cMenuSetupZapHistory(void)
{
    // init sort option strings
    sortStrs[0] = tr("Latest view");
    sortStrs[1] = tr("Zap count");
    sortStrs[2] = tr("View time");
    
    // init view option strings
    viewStrs[0] = tr("Button$Schedule");
    viewStrs[1] = tr("Button$Statistic");

    // add menu items
    SetSection(tr("Zaphistory"));
    Add(new cMenuEditIntItem(tr("Number of entries"),  &ZapHistorySetup.EntryCount, 0, Channels.Count() ));
    Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"), &ZapHistorySetup.HideMainMenu));
    Add(new cMenuEditStraItem(tr("Default view"), &ZapHistorySetup.DummyDefaultView, 2, viewStrs));
    Add(new cMenuEditStraItem(tr("Default sort"), &ZapHistorySetup.DummyDefaultSort, 3, sortStrs));    
}

void cMenuSetupZapHistory::Store(void)
{
    // store setup
    ZapHistorySetup.IntToEnum();

    SetupStore("HideMainMenu",  ZapHistorySetup.HideMainMenu  );
    SetupStore("EntryCount",  ZapHistorySetup.EntryCount  );
    SetupStore("DefaultView",  ZapHistorySetup.DummyDefaultView  );
    SetupStore("DefaultSort",  ZapHistorySetup.DummyDefaultSort  );
}

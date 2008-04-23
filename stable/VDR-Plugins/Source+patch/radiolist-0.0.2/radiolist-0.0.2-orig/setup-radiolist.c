/*
 * Radiolist Player plugin for VDR
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. Read the file COPYING for details.
 */


#include "setup-radiolist.h"
#include "i18n.h"
#ifdef __QNXNTO__
#include <strings.h>
#endif

cRadiolistSetup RadiolistSetup;

// --- cRadiolistSetup -----------------------------------------------------------

cRadiolistSetup::cRadiolistSetup(void)
{
    HideMainMenu  = 0;
    DisplaySeperators = 0;
}

bool cRadiolistSetup::SetupParse(const char *Name, const char *Value)
{
    // Parse your own setup parameters and store their values.
    if (!strcasecmp(Name, "HideMainMenu"))  HideMainMenu  = atoi(Value);
    else if (!strcasecmp(Name, "DisplaySeperators"))  DisplaySeperators  = atoi(Value);
    else
	return false;
    return true;
}

// --- cMenuSetupRadiolist --------------------------------------------------------

cMenuSetupRadiolist::cMenuSetupRadiolist(void)
{
    data = RadiolistSetup;
    SetSection(tr("Radiolist"));
    Add(new cMenuEditBoolItem(tr("Hide Mainmenu Entry"),         &data.HideMainMenu));
    Add(new cMenuEditBoolItem(tr("Display Channel Seperators"),  &data.DisplaySeperators));
}

void cMenuSetupRadiolist::Store(void)
{
    RadiolistSetup = data;
    SetupStore("HideMainMenu",  RadiolistSetup.HideMainMenu  );
    SetupStore("DisplaySeperators",  RadiolistSetup.DisplaySeperators  );
}

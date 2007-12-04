#include <vector>
#include "menu_blacklistedit.h"
#include "changrp.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include "menu_dirselect.h"
#include "menu_searchedit.h"
#include "menu_searchtemplate.h"
#include "epgsearchcfg.h"

#include <math.h>

extern cChannelGroups ChannelGroups;
extern cSearchExtCats SearchExtCats;

extern const char AllowedChars[];

// --- cMenuBlacklistEdit --------------------------------------------------------
cMenuBlacklistEdit::cMenuBlacklistEdit(cBlacklist *Blacklist, bool New)
:cOsdMenu(tr("Edit blacklist"),32)
{
    SearchModes[0] = strdup(tr("phrase"));
    SearchModes[1] = strdup(tr("all words"));
    SearchModes[2] = strdup(tr("at least one word"));
    SearchModes[3] = strdup(tr("match exactly"));
    SearchModes[4] = strdup(tr("regular expression"));
    SearchModes[5] = strdup(tr("fuzzy"));
    
    DaysOfWeek[0] = strdup(WeekDayName(0));
    DaysOfWeek[1] = strdup(WeekDayName(1));
    DaysOfWeek[2] = strdup(WeekDayName(2));
    DaysOfWeek[3] = strdup(WeekDayName(3));
    DaysOfWeek[4] = strdup(WeekDayName(4));
    DaysOfWeek[5] = strdup(WeekDayName(5));
    DaysOfWeek[6] = strdup(WeekDayName(6));
    DaysOfWeek[7] = strdup(tr("user-defined"));
    
    UseChannelSel[0] = strdup(tr("no"));
    UseChannelSel[1] = strdup(tr("interval"));
    UseChannelSel[2] = strdup(tr("channel group"));
    UseChannelSel[3] = strdup(tr("only FTA"));

    if (New)
    {
	cSearchExt* SearchTempl = NULL; // copy the default settings, if we have a default template	
	cMutexLock SearchTemplatesLock(&SearchTemplates);
	cSearchExt *SearchExtTempl = SearchTemplates.First();
	while (SearchExtTempl) 
	{	
	    if (SearchExtTempl->ID == EPGSearchConfig.DefSearchTemplateID)
		SearchTempl = SearchExtTempl;
	    SearchExtTempl = SearchTemplates.Next(SearchExtTempl);
	}    
	if (SearchTempl)
	    Blacklist->CopyFromTemplate(SearchTempl);
    }

    blacklist = Blacklist;
    addIfConfirmed = New;

    if (blacklist) 
    {
	data = *blacklist;
	UserDefDayOfWeek = 0;
	if (blacklist->DayOfWeek < 0)
	{
	    UserDefDayOfWeek = blacklist->DayOfWeek;
	    data.DayOfWeek = 7;
	}

	menuitemsChGr = NULL;
	channelGroupName = NULL;

	channelMin = channelMax = cDevice::CurrentChannel();
	channelGroupNr = 0;  
	if (data.useChannel==1)
	{
	    channelMin = data.channelMin->Number();
	    channelMax = data.channelMax->Number();
	}
	if (data.useChannel==2)
	{
	    channelGroupNr = ChannelGroups.GetIndex(data.channelGroup);
	    if (channelGroupNr == -1)
	    {
		free(data.channelGroup);
		data.channelGroup = NULL;
		channelGroupNr = 0; // no selection
	    }
	    else
	    {
		channelGroupName = strdup(data.channelGroup);
		channelGroupNr++;
	    }
	}	
	Set();
    }
}

void cMenuBlacklistEdit::Set()
{
    int current = Current();
    Clear();
    
    Add(new cMenuEditStrItem( tr("Search term"), data.search, sizeof(data.search), tr(AllowedChars)));
    Add(new cMenuEditStraItem(tr("Search mode"),     &data.mode, 6, SearchModes));
    if (data.mode == 5) // fuzzy
	Add(new cMenuEditIntItem(IndentMenuItem(tr("Tolerance")), &data.fuzzyTolerance, 1, 9));
    Add(new cMenuEditBoolItem( tr("Match case"), &data.useCase, tr("no"), tr("yes")));
    Add(new cMenuEditBoolItem( tr("Use title"), &data.useTitle, tr("no"), tr("yes")));
    Add(new cMenuEditBoolItem( tr("Use subtitle"), &data.useSubtitle, tr("no"), tr("yes")));
    Add(new cMenuEditBoolItem( tr("Use description"), &data.useDescription, tr("no"), tr("yes")));  
    
    // show Categories only if we have them
    if (SearchExtCats.Count() > 0)
    {
	Add(new cMenuEditBoolItem( tr("Use extended EPG info"), &data.useExtEPGInfo, tr("no"), tr("yes")));
	if (data.useExtEPGInfo)
	{
	    cSearchExtCat *SearchExtCat = SearchExtCats.First();
	    int index = 0;
	    while (SearchExtCat) 
	    {
		Add(new cMenuEditStrItem( IndentMenuItem(SearchExtCat->menuname), data.catvalues[index], MaxFileName, tr(AllowedChars)));
		SearchExtCat = SearchExtCats.Next(SearchExtCat);
		index++;
	    }
	}
    }
    
    Add(new cMenuEditStraItem(tr("Use channel"), &data.useChannel, 4, UseChannelSel));
    if (data.useChannel==1)
    {
	Add(new cMenuEditChanItem(tr("  from channel"),      &channelMin));
	Add(new cMenuEditChanItem(tr("  to channel"),      &channelMax));
    }
    if (data.useChannel==2)
    {
	// create the char array for the menu display
	if (menuitemsChGr) delete [] menuitemsChGr;
	menuitemsChGr = ChannelGroups.CreateMenuitemsList();
	int oldchannelGroupNr = channelGroupNr;
	channelGroupNr = ChannelGroups.GetIndex(channelGroupName);
	if (channelGroupNr == -1)
	{
	    if (oldchannelGroupNr > 0 && oldchannelGroupNr <= ChannelGroups.Count()) // perhaps its name was changed
		channelGroupNr = oldchannelGroupNr;
	    else
		channelGroupNr = 0; // no selection
	}
	else
	    channelGroupNr++;
	Add(new cMenuEditStraItem(IndentMenuItem(tr("Channel group")), &channelGroupNr, ChannelGroups.Count()+1, menuitemsChGr));
    }
    
    Add(new cMenuEditBoolItem( tr("Use time"), &data.useTime, tr("no"), tr("yes")));
    if (data.useTime == true)
    {
	Add(new cMenuEditTimeItem(tr("  Start after"),        &data.startTime));
	Add(new cMenuEditTimeItem(tr("  Start before"),         &data.stopTime));
    }
    Add(new cMenuEditBoolItem( tr("Use duration"), &data.useDuration, tr("no"), tr("yes")));
    if (data.useDuration == true)
    {
	Add(new cMenuEditTimeItem(tr("  Min. duration"), &data.minDuration));
	Add(new cMenuEditTimeItem(tr("  Max. duration"), &data.maxDuration));
    }
    Add(new cMenuEditBoolItem( tr("Use day of week"), &data.useDayOfWeek, tr("no"), tr("yes")));
    if (data.useDayOfWeek)
    {
	if (data.DayOfWeek < 0)
	{
	    UserDefDayOfWeek = data.DayOfWeek;
	    data.DayOfWeek = 7;
	}	
	Add(new cMenuEditStraItem(tr("  Day of week"),     &data.DayOfWeek, 8, DaysOfWeek));
    }
    SetCurrent(Get(current));
}

cMenuBlacklistEdit::~cMenuBlacklistEdit()
{
    if (blacklist && addIfConfirmed)
	delete blacklist; // apparently it wasn't confirmed
    if (menuitemsChGr)
	free(menuitemsChGr);
    if (channelGroupName)
	free(channelGroupName);

    int i;
    for(i=0; i<=4; i++)
	free(SearchModes[i]);
    for(i=0; i<=7; i++)
	free(DaysOfWeek[i]);
    for(i=0; i<=2; i++)
	free(UseChannelSel[i]);
}

eOSState cMenuBlacklistEdit::ProcessKey(eKeys Key)
{
    bool bHadSubMenu = HasSubMenu();

    int iTemp_mode = data.mode;
    int iTemp_useTime = data.useTime;
    int iTemp_useChannel = data.useChannel;
    int iTemp_useDuration = data.useDuration;
    int iTemp_useDayOfWeek = data.useDayOfWeek;
    int iTemp_useExtEPGInfo = data.useExtEPGInfo;
    
    eOSState state = cOsdMenu::ProcessKey(Key);
    
    if (iTemp_mode != data.mode ||
	iTemp_useTime != data.useTime ||
	iTemp_useChannel != data.useChannel ||
	iTemp_useDuration != data.useDuration ||
	iTemp_useDayOfWeek != data.useDayOfWeek ||
	iTemp_useExtEPGInfo != data.useExtEPGInfo)
    {
	Set();
	Display();
    }
    const char* ItemText = Get(Current())->Text();
    
    if (!HasSubMenu())
    {
	if (strlen(ItemText)>0 && strstr(ItemText, tr("  from channel")) == ItemText && ((Key >= k0 &&  Key <= k9) || Key == kLeft || Key == kRight))
	{
	    channelMax = channelMin;
	    Set();
	    Display();
	}
    }

    int iOnUserDefDayItem = 0;
    int iOnUseChannelGroups = 0;
    int iOnChannelGroup = 0;
    int iOnTerm = 0;
    int iOnExtCatItemBrowsable = 0;
    int iCatIndex = -1;
    char* catname = NULL;

    if (!HasSubMenu() && strlen(ItemText)>0)
    {
	// check, if on an item of ext. EPG info
	int iOnExtCatItem = 0;
	cSearchExtCat *SearchExtCat = SearchExtCats.First();
	int index = 0;
	while (SearchExtCat) 
	{
	    if (strstr(ItemText, IndentMenuItem(SearchExtCat->menuname)) == ItemText)
	    {		
		iOnExtCatItem = 1;
		if (SearchExtCat->nvalues > 0)
		    iOnExtCatItemBrowsable = 1;
		iCatIndex = index;
		catname = SearchExtCat->menuname;
		break;		    
	    }
	    index++;
	    SearchExtCat = SearchExtCats.Next(SearchExtCat);
	}

	if (strstr(ItemText, tr("Search term")) == ItemText)
	{
	    if (!InEditMode(ItemText, tr("Search term"), data.search)) // show template for a new search
	    {
		SetHelp(NULL, NULL, NULL, tr("Button$Templates"));
		iOnTerm = 1;
	    }
	}
       	if (strstr(ItemText, tr("  Day of week")) == ItemText)
	{
	    if (data.DayOfWeek == 7)
	    {
		SetHelp(tr("Button$Edit"));
		iOnUserDefDayItem = 1;
	    }
	    else
		SetHelp(NULL);
	}
	else if (strstr(ItemText, tr("Use channel")) == ItemText && data.useChannel == 2)
	{
	    SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
	    iOnUseChannelGroups = 1;
	}
	else if (strstr(ItemText, IndentMenuItem(tr("Channel group"))) == ItemText)
	{
	    SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
	    iOnChannelGroup = 1;
	}
	else if (iOnExtCatItem)
	{
	    if (!InEditMode(ItemText, IndentMenuItem(catname), data.catvalues[iCatIndex]))	    
		SetHelp(NULL, NULL, NULL, iOnExtCatItemBrowsable?tr("Button$Select"):NULL);
	}
	else if (strstr(ItemText, tr("Search term")) != ItemText)
	    SetHelp(NULL, NULL, NULL, NULL);
    }
    if (state == osUnknown) {
	if (HasSubMenu())
	    return osContinue;
	switch (Key) {
	    case kOk: 
		if (data.useChannel==1) 
		{
		    cChannel *ch = Channels.GetByNumber(channelMin);
		    if (ch)
			data.channelMin = ch;
		    else 
		    {
			Skins.Message(mtError, tr("*** Invalid Channel ***"));
			break;
		    }
		    ch = Channels.GetByNumber(channelMax);
		    if (ch)
			data.channelMax = ch;
		    else 
		    {
			Skins.Message(mtError, tr("*** Invalid Channel ***"));
			break;
		    }
		    if (channelMin > channelMax)
		    {
			Skins.Message(mtError, tr("Please check channel criteria!"));
			return osContinue;
		    }
		}
		if (data.useChannel==2)
		    data.channelGroup = strdup(menuitemsChGr[channelGroupNr]);
		
		if (blacklist) 
		{
		    *blacklist = data;
		    if (data.DayOfWeek == 7)
			blacklist->DayOfWeek = UserDefDayOfWeek;
		    
		    if (addIfConfirmed)
		    {
			cMutexLock BlacklistLock(&Blacklists);
			blacklist->ID = Blacklists.GetNewID();
			Blacklists.Add(blacklist);
		    }
 
		    Blacklists.Save();
		    addIfConfirmed = false;
		}
		return osBack;
	    case kRed:
		if (iOnUserDefDayItem)
		    state = AddSubMenu(new cMenuEditDaysOfWeek(&UserDefDayOfWeek));
		break;
		
	    case kBlue:   
		if (iOnUseChannelGroups || iOnChannelGroup)
		{		    
		    if (channelGroupName)
			free(channelGroupName);
		    channelGroupName = strdup(menuitemsChGr[channelGroupNr]);
		    state = AddSubMenu(new cMenuChannelGroups(&channelGroupName));
		}
		if (iOnTerm)
		    state = AddSubMenu(new cMenuEPGSearchTemplate(NULL, &data, addIfConfirmed));
		if (iOnExtCatItemBrowsable)
		    state = AddSubMenu(new cMenuCatValuesSelect(data.catvalues[iCatIndex], iCatIndex, SearchExtCats.Get(iCatIndex)->searchmode));
		break;
	    case kGreen:
	    case kYellow: state = osContinue;
	    default: break;
	}
    }
    if ((iOnUseChannelGroups || iOnChannelGroup || iOnTerm) && bHadSubMenu && !HasSubMenu()) // return form submenu
    {
	if (iOnTerm)
	{
	    if (data.DayOfWeek < 0)
	    {
		UserDefDayOfWeek = data.DayOfWeek;
		data.DayOfWeek = 7;
	    }
	    if (data.useChannel == 2)
	    {
		channelGroupNr = ChannelGroups.GetIndex(data.channelGroup);
		channelGroupName = strdup(data.channelGroup);
	    }
	}
	Set();
	Display();
    }
    return state;
}


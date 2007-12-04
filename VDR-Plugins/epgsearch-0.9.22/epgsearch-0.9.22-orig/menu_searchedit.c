#include <vector>
#include "menu_searchedit.h"
#include "changrp.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include "menu_dirselect.h"
#include "menu_recsdone.h"
#include "menu_searchtemplate.h"
#include "epgsearchcfg.h"
#include "blacklist.h"
#include "menu_blacklists.h"
#include "epgsearch.h"
#include "searchtimer_thread.h"
#include <math.h>
#include "templatefile.h"

cChannelGroups ChannelGroups;
cSearchExtCats SearchExtCats;

// --- cMenuEditSearchExt --------------------------------------------------------
cMenuEditSearchExt::cMenuEditSearchExt(cSearchExt *SearchExt, bool New, bool Template)
:cOsdMenu(tr("Edit search"),32)
{
    templateMode = Template;

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

    SearchTimerModes[0] = strdup(tr("Record"));
    SearchTimerModes[1] = strdup(tr("Announce only"));
    SearchTimerModes[2] = strdup(tr("Switch only"));

    BlacklistModes[0] = strdup(tr("no"));
    BlacklistModes[1] = strdup(tr("Selection"));
    BlacklistModes[2] = strdup(tr("all"));

    if (!templateMode && New)
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
	    SearchExt->CopyFromTemplate(SearchTempl);
    }

    searchExt = SearchExt;
    addIfConfirmed = New;
    if (!templateMode)
	SetHelp(NULL, NULL, NULL, tr("Button$Templates"));

    if (searchExt) 
    {
	data = *searchExt;
	UserDefDayOfWeek = 0;
	if (searchExt->DayOfWeek < 0)
	{
	    UserDefDayOfWeek = searchExt->DayOfWeek;
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

	catarrayAvoidRepeats = NULL;
	if (SearchExtCats.Count() > 0)
	{
	    // fill an array, that stores yes/no for using categories in avoid repeats
	    catarrayAvoidRepeats = (int*) malloc(SearchExtCats.Count() * sizeof(int));
	    cSearchExtCat *SearchExtCat = SearchExtCats.First();
	    int index = 0;
	    while (SearchExtCat) 	    
	    {
		catarrayAvoidRepeats[index] = (SearchExt->catvaluesAvoidRepeat & (1<<index))?1:0;
		SearchExtCat = SearchExtCats.Next(SearchExtCat);
		index++;
	    }
	}
	blacklists.Clear();
	if (data.blacklistMode == blacklistsSelection)
	{	    
	    cBlacklistObject* blacklistObj = searchExt->blacklists.First();
	    while(blacklistObj)
	    {
		blacklists.Add(new cBlacklistObject(blacklistObj->blacklist));
		blacklistObj = searchExt->blacklists.Next(blacklistObj);
	    }
	}

	Set();
    }
}

void cMenuEditSearchExt::Set()
{
    int current = Current();
    Clear();
    
    if (templateMode)
	Add(new cMenuEditStrItem( tr("Template name"), data.search, sizeof(data.search), tr(AllowedChars)));
    else
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
    Add(new cMenuEditStraItem(tr("Use blacklists"), &data.blacklistMode, 3, BlacklistModes));

    if (EPGSearchConfig.showFavoritesMenu)
	Add(new cMenuEditBoolItem( tr("Use in favorites menu"), &data.useInFavorites, tr("no"), tr("yes")));

    int countSearchTemplates = 0;
    if ((countSearchTemplates = cTemplFile::CountSearchResultsTemplates()) > 1)
    {
	Add(new cMenuEditStraItem(tr("Result menu layout"), &data.menuTemplate, countSearchTemplates, cTemplFile::SearchTemplates));
    }

    Add(new cMenuEditBoolItem( tr("Use as search timer"), &data.useAsSearchTimer, tr("no"), tr("yes")));
    if (data.useAsSearchTimer)
    {
	Add(new cMenuEditStraItem(IndentMenuItem(tr("Action")), &data.action, 3, SearchTimerModes));
	if (data.action == searchTimerActionSwitchOnly)
	{
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Switch ... minutes before start")), &data.switchMinsBefore, 0, 99));
	}
	if (data.action == searchTimerActionRecord)
	{
	    Add(new cMenuEditBoolItem( tr("  Series recording"), &data.useEpisode, tr("no"), tr("yes")));
	    Add(new cMenuEditStrItem(IndentMenuItem(tr("Directory")), data.directory, sizeof(data.directory), tr(AllowedChars)));
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Delete recordings after ... days")), &data.delAfterDays, 0, 999));
	    if (data.delAfterDays > 0)
		Add(new cMenuEditIntItem(IndentMenuItem(IndentMenuItem(tr("Keep ... recordings"))), &data.recordingsKeep, 0, 999));
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Pause when ... recordings exist")), &data.pauseOnNrRecordings, 0, 999));
	    Add(new cMenuEditBoolItem(IndentMenuItem(tr("Avoid repeats")), &data.avoidRepeats, tr("no"), tr("yes")));
	    if (data.avoidRepeats)
	    {
		Add(new cMenuEditIntItem(IndentMenuItem(IndentMenuItem(tr("Allowed repeats"))), &data.allowedRepeats, 0, 99));
		if (data.allowedRepeats > 0)
		    Add(new cMenuEditIntItem(IndentMenuItem(IndentMenuItem(tr("Only repeats within ... days"))), &data.repeatsWithinDays, 0, 999));
		Add(new cMenuEditBoolItem(IndentMenuItem(IndentMenuItem(tr("Compare title"))), &data.compareTitle, tr("no"), tr("yes")));
		Add(new cMenuEditBoolItem(IndentMenuItem(IndentMenuItem(tr("Compare subtitle"))), &data.compareSubtitle, tr("no"), tr("yes")));
		Add(new cMenuEditBoolItem(IndentMenuItem(IndentMenuItem(tr("Compare summary"))), &data.compareSummary, tr("no"), tr("yes")));
		// show 'Compare categories' only if we have them
		if (SearchExtCats.Count() > 0)
		{
		    cSearchExtCat *SearchExtCat = SearchExtCats.First();
		    int iUsed = 0;
		    int index = 0;
		    while (SearchExtCat) 
		    {
			if (catarrayAvoidRepeats[index])
			    iUsed++;
			SearchExtCat = SearchExtCats.Next(SearchExtCat);
			index++;
		    }
		    
		    char* itemtext = NULL;
		    asprintf(&itemtext, "%s (%d/%d)", tr("Compare categories"), iUsed, SearchExtCats.Count());
		    Add(new cOsdItem(IndentMenuItem(IndentMenuItem(itemtext))));
		    free(itemtext);
		}
	    }
	    
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Priority")), &data.Priority, 0, MAXPRIORITY));
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Lifetime")), &data.Lifetime, 0, MAXLIFETIME));
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Setup.Recording$Margin at start (min)")), &data.MarginStart, -INT_MAX, INT_MAX));
	    Add(new cMenuEditIntItem(IndentMenuItem(tr("Setup.Recording$Margin at stop (min)")), &data.MarginStop, -INT_MAX, INT_MAX));
	    Add(new cMenuEditBoolItem(IndentMenuItem(tr("VPS")), &data.useVPS, tr("no"), tr("yes")));
	}
    }
    SetCurrent(Get(current));
}

cMenuEditSearchExt::~cMenuEditSearchExt()
{
    if (searchExt && addIfConfirmed)
	delete searchExt; // apparently it wasn't confirmed
    if (menuitemsChGr)
	free(menuitemsChGr);
    if (channelGroupName)
	free(channelGroupName);
    if (catarrayAvoidRepeats)
	free(catarrayAvoidRepeats);

    int i;
    for(i=0; i<=5; i++)
	free(SearchModes[i]);
    for(i=0; i<=7; i++)
	free(DaysOfWeek[i]);
    for(i=0; i<=2; i++)
	free(UseChannelSel[i]);
    for(i=0; i<=2; i++)
	free(SearchTimerModes[i]);
    for(i=0; i<=2; i++)
	free(BlacklistModes[i]);
}

eOSState cMenuEditSearchExt::ProcessKey(eKeys Key)
{
    bool bHadSubMenu = HasSubMenu();

    int iTemp_mode = data.mode;
    int iTemp_useTime = data.useTime;
    int iTemp_useChannel = data.useChannel;
    int iTemp_useDuration = data.useDuration;
    int iTemp_useDayOfWeek = data.useDayOfWeek;
    int iTemp_useAsSearchTimer = data.useAsSearchTimer;
    int iTemp_useExtEPGInfo = data.useExtEPGInfo;
    int iTemp_avoidRepeats = data.avoidRepeats;
    int iTemp_allowedRepeats = data.allowedRepeats;
    int iTemp_delAfterDays = data.delAfterDays;
    int iTemp_action = data.action;
    
    eOSState state = cOsdMenu::ProcessKey(Key);
    
    if (iTemp_mode != data.mode ||
	iTemp_useTime != data.useTime ||
	iTemp_useChannel != data.useChannel ||
	iTemp_useDuration != data.useDuration ||
	iTemp_useDayOfWeek != data.useDayOfWeek ||
	iTemp_useAsSearchTimer != data.useAsSearchTimer ||
	iTemp_useExtEPGInfo != data.useExtEPGInfo ||
	iTemp_avoidRepeats != data.avoidRepeats ||
	iTemp_allowedRepeats != data.allowedRepeats ||
	iTemp_delAfterDays != data.delAfterDays ||
	iTemp_action != data.action)
    {
	Set();
	Display();
	if ((iTemp_useAsSearchTimer != data.useAsSearchTimer || iTemp_action != data.action) && data.useAsSearchTimer) // if search timer menu is dropped then scroll down to display all contents
	{
	    int cur = Current();
	    PageDown();
	    SetCurrent(Get(cur));
	    Display();
	}
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
    int iOnDirectoryItem = 0;
    int iOnUseChannelGroups = 0;
    int iOnChannelGroup = 0;
    int iOnAvoidRepeats = 0;
    int iOnCompareCats = 0;
    int iOnTerm = 0;
    int iOnUseBlacklistsSelection = 0;
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
	else if (strstr(ItemText, IndentMenuItem(tr("Directory"))) == ItemText)
	{
	    if (!InEditMode(ItemText, IndentMenuItem(tr("Directory")), data.directory))
		SetHelp(NULL, NULL, NULL, tr("Button$Select"));
	    iOnDirectoryItem = 1;
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
	else if (strstr(ItemText, tr("Use blacklists")) == ItemText && data.blacklistMode == blacklistsSelection)
	{
	    SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
	    iOnUseBlacklistsSelection = 1;
	}
	else if (strstr(ItemText, IndentMenuItem(tr("Avoid repeats"))) == ItemText)
	{
	    SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
	    iOnAvoidRepeats = 1;
	}
	else if (strstr(ItemText, IndentMenuItem(IndentMenuItem(tr("Compare categories")))) == ItemText)
	{
	    SetHelp(NULL, NULL, NULL, tr("Button$Setup"));
	    iOnCompareCats = 1;
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
		
		if ((data.useTitle || data.useSubtitle || data.useDescription) && 
		    (int(strlen(data.search)) < 3) && 
		    !Interface->Confirm(tr("Edit$Search text too short - use anyway?")))
		    break;
				
		if (searchExt) 
		{
		    *searchExt = data;
		    if (data.DayOfWeek == 7)
			searchExt->DayOfWeek = UserDefDayOfWeek;
		    
		    // transfer cat selection for 'avoid repeats' back to search
		    cSearchExtCat *SearchExtCat = SearchExtCats.First();
		    int index = 0;
		    searchExt->catvaluesAvoidRepeat = 0;
		    while (SearchExtCat) 
		    {
			if (catarrayAvoidRepeats[index])
			    searchExt->catvaluesAvoidRepeat += (1<<index);
			SearchExtCat = SearchExtCats.Next(SearchExtCat);
			index++;
		    }

		    if (data.blacklistMode == blacklistsSelection)
		    {
			searchExt->blacklists.Clear();
			cBlacklistObject* blacklistObj = blacklists.First();
			while(blacklistObj)
			{
			    searchExt->blacklists.Add(new cBlacklistObject(blacklistObj->blacklist));
			    blacklistObj = blacklists.Next(blacklistObj);
			}
		    }
		    else
			searchExt->blacklists.Clear();

		    if (addIfConfirmed)
		    {
			cMutexLock SearchExtsLock(&SearchExts);
			searchExt->ID = SearchExts.GetNewID();
			SearchExts.Add(searchExt);
		    }
		    if (searchExt->useAsSearchTimer && !EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
		    {
			cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);
			Skins.Message(mtInfo, tr("Search timers activated in setup."));			
		    }
            
		    SearchExts.Save();
            addIfConfirmed = false;
		}
		return osBack;
	    case kRed:
		if (iOnUserDefDayItem)
		    state = AddSubMenu(new cMenuEditDaysOfWeek(&UserDefDayOfWeek));
		break;
		
	    case kBlue:   
		if (iOnDirectoryItem && !InEditMode(ItemText, IndentMenuItem(tr("Directory")), data.directory))
		    state = AddSubMenu(new cMenuDirSelect(data.directory));
		if (iOnUseChannelGroups || iOnChannelGroup)
		{		    
		    if (channelGroupName)
			free(channelGroupName);
		    channelGroupName = strdup(menuitemsChGr[channelGroupNr]);
		    state = AddSubMenu(new cMenuChannelGroups(&channelGroupName));
		}
		if (iOnAvoidRepeats)
		    state = AddSubMenu(new cMenuRecsDone(searchExt));
		if (iOnCompareCats)
		    state = AddSubMenu(new cMenuSearchEditCompCats(catarrayAvoidRepeats));
		if (iOnTerm)
		    state = AddSubMenu(new cMenuEPGSearchTemplate(&data, NULL, addIfConfirmed));
		if (iOnUseBlacklistsSelection)
		    state = AddSubMenu(new cMenuBlacklistsSelection(&blacklists));
		if (iOnExtCatItemBrowsable)
		    state = AddSubMenu(new cMenuCatValuesSelect(data.catvalues[iCatIndex], iCatIndex, SearchExtCats.Get(iCatIndex)->searchmode));
		break;
	    case kGreen:
	    case kYellow: state = osContinue;
	    default: break;
	}
    }
    if ((iOnUseChannelGroups || iOnChannelGroup || iOnCompareCats || iOnTerm) && bHadSubMenu && !HasSubMenu()) // return form submenu
    {
	if (iOnTerm)
	{
	    if (data.DayOfWeek < 0)
	    {
		UserDefDayOfWeek = data.DayOfWeek;
		data.DayOfWeek = 7;
	    }
	    if (data.useChannel == 1)
	    {
		channelMin = data.channelMin->Number();
		channelMax = data.channelMax->Number();
	    }
	    if (data.useChannel == 2)
	    {
		channelGroupNr = ChannelGroups.GetIndex(data.channelGroup);
		channelGroupName = strdup(data.channelGroup);
	    }
	    if (SearchExtCats.Count() > 0)
	    {
		cSearchExtCat *SearchExtCat = SearchExtCats.First();
		int index = 0;
		while (SearchExtCat) 	    
		{
		    catarrayAvoidRepeats[index] = (data.catvaluesAvoidRepeat & (1<<index))?1:0;
		    SearchExtCat = SearchExtCats.Next(SearchExtCat);
		    index++;
		}
	    }
	}
	Set();
	Display();
    }
    return state;
}

// --- cMenuEditDaysOfWeek --------------------------------------------------------
cMenuEditDaysOfWeek::cMenuEditDaysOfWeek(int* DaysOfWeek, int Offset, bool Negate)
:cOsdMenu(tr("Edit user-defined days of week"),30)
{
    int i=0;
    offset = Offset;
    negate = Negate;
    pDaysOfWeek = DaysOfWeek;

    if (negate)
	*pDaysOfWeek = -*pDaysOfWeek;

    for(i=0; i<7; i++)
	Days[(i+offset)%7]=((*pDaysOfWeek) & (int)pow(2,i))?1:0;
    for(i=0; i<7; i++)
	Add(new cMenuEditBoolItem( WeekDayName((i+1)%7), &Days[(i+1)%7], tr("no"), tr("yes")));

    SetHelp(NULL);
}

eOSState cMenuEditDaysOfWeek::ProcessKey(eKeys Key)
{
    int i=0;
    if (Key == kBack && negate)
	*pDaysOfWeek = -*pDaysOfWeek;		 

    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
	switch (Key) {
	    case kOk:
		*pDaysOfWeek = 0;
		for(i=0; i<7; i++)
		    *pDaysOfWeek += Days[i]?(int)pow(2,(i+7-offset)%7):0;
		if (negate)
		    *pDaysOfWeek = -*pDaysOfWeek;
		state = osBack;
		break;
	    default: break;
	}
    }
    return state;
}

// --- cMenuSearchEditCompCats --------------------------------------------------------
cMenuSearchEditCompCats::cMenuSearchEditCompCats(int* catarrayAvoidRepeats)
:cOsdMenu(tr("Compare categories"),30)
{
    search_catarrayAvoidRepeats = catarrayAvoidRepeats;
    edit_catarrayAvoidRepeats = (int*) malloc(SearchExtCats.Count() * sizeof(int));
    cSearchExtCat *SearchExtCat = SearchExtCats.First();
    int index = 0;
    while (SearchExtCat) 
    {
	edit_catarrayAvoidRepeats[index] = catarrayAvoidRepeats[index];
	char* menutext = NULL;
	asprintf(&menutext, "%s %s", tr("Compare"), SearchExtCat->menuname);
	Add(new cMenuEditBoolItem(menutext, &edit_catarrayAvoidRepeats[index], tr("no"), tr("yes")));
	free(menutext);
	SearchExtCat = SearchExtCats.Next(SearchExtCat);
	index++;
    }
}

cMenuSearchEditCompCats::~cMenuSearchEditCompCats()
{
    free(edit_catarrayAvoidRepeats);
}

eOSState cMenuSearchEditCompCats::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
	switch (Key) {
	    case kOk:
	    {
		cSearchExtCat *SearchExtCat = SearchExtCats.First();
		int index = 0;
		while (SearchExtCat) 
		{
		    search_catarrayAvoidRepeats[index] = edit_catarrayAvoidRepeats[index];
		    SearchExtCat = SearchExtCats.Next(SearchExtCat);
		    index++;
		}
	    }
	    state = osBack;
	    break;
	    default: break;
	}
    }
    return state;
}

// --- cMenuBlacklistsSelection --------------------------------------------------------
cMenuBlacklistsSelection::cMenuBlacklistsSelection(cList<cBlacklistObject>* pBlacklists)
:cOsdMenu(tr("Select blacklists"),30)
{
    blacklists = pBlacklists;
    blacklistsSel = new int[Blacklists.Count()];
    cMutexLock BlacklistLock(&Blacklists);
    cBlacklist* blacklist = Blacklists.First();
    int index = 0;
    while(blacklist)
    {
	blacklistsSel[index] = false;
	cBlacklistObject* blacklistObjSel = blacklists->First();
	while(blacklistObjSel)
	{
	    if (blacklistObjSel->blacklist->ID == blacklist->ID)
	    {
		blacklistsSel[index] = true;
		break;
	    }
	    blacklistObjSel = blacklists->Next(blacklistObjSel);
	}
	blacklist = Blacklists.Next(blacklist);
	index++;
    }

    SetHelp(tr("Button$Invert selection"), tr("Button$All yes"), tr("Button$All no"), tr("Button$Setup"));
    Set();
}

cMenuBlacklistsSelection::~cMenuBlacklistsSelection()
{
    if (blacklistsSel) delete [] blacklistsSel;
}

// --- cMenuBlacklistsSelectionItem ----------------------------------------------------------
class cMenuBlacklistsSelectionItem : public cMenuEditBoolItem {
    const char* name;
public:
  cMenuBlacklistsSelectionItem(const char *Name, int *Value, const char *FalseString = NULL, const char *TrueString = NULL):
      cMenuEditBoolItem(Name, Value, FalseString, TrueString) { name = Name; }
  int Compare(const cListObject &ListObject) const
      {
	  cMenuBlacklistsSelectionItem *p = (cMenuBlacklistsSelectionItem*)&ListObject;
	  return strcasecmp(name, p->name);
      }
};

void cMenuBlacklistsSelection::Set()
{
    int current = Current();
    Clear();
    cMutexLock BlacklistLock(&Blacklists);
    cBlacklist* blacklist = Blacklists.First();
    int index = 0;
    while(blacklist)
    {
	Add(new cMenuBlacklistsSelectionItem(blacklist->search, &blacklistsSel[index], tr("no"), tr("yes")));
	blacklist = Blacklists.Next(blacklist);
	index++;
    }
    SetCurrent(Get(current));
    Sort();
}

eOSState cMenuBlacklistsSelection::ProcessKey(eKeys Key)
{
    bool bHadSubMenu = HasSubMenu();

    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
	switch (Key) {
	    case kOk:
	    {
		cMutexLock BlacklistLock(&Blacklists);
		blacklists->Clear();
		cBlacklist* blacklist = Blacklists.First();
		int index = 0;
		while(blacklist)
		{
		    if (blacklistsSel[index++])
			blacklists->Add(new cBlacklistObject(blacklist));
		    blacklist = Blacklists.Next(blacklist);
		}
	    }
	    state = osBack;
	    break;
	    case kRed:
	    case kGreen:	      
	    case kYellow:
	    {
		cMutexLock BlacklistLock(&Blacklists);
		cBlacklist* blacklist = Blacklists.First();
		int index = 0;
		while(blacklist)
		{
		    blacklistsSel[index] = (Key == kGreen?1:(Key == kRed?1-blacklistsSel[index]:0));
		    blacklist = Blacklists.Next(blacklist);
		    index++;
		}
		Set();
		Display();
		return osContinue;
	    }
	    break;
	    case kBlue:
		state = AddSubMenu(new cMenuBlacklists);
		break;
	    default: break;
	}
    }
    if (bHadSubMenu && !HasSubMenu()) // return form submenu
    {
	Clear();
	delete [] blacklistsSel;
	blacklistsSel = new int[Blacklists.Count()];
	cMutexLock BlacklistLock(&Blacklists);
	cBlacklist* blacklist = Blacklists.First();
	int index = 0;
	while(blacklist)
	{
	    blacklistsSel[index] = false;
	    cBlacklistObject* blacklistObjSel = blacklists->First();
	    while(blacklistObjSel)
	    {
		if (blacklistObjSel->blacklist->ID == blacklist->ID)
		{
		    blacklistsSel[index] = true;
		    break;
		}
		blacklistObjSel = blacklists->Next(blacklistObjSel);
	    }
	    blacklist = Blacklists.Next(blacklist);
	    index++;
	}
	Set();
	Display();
    }

    return state;
}

// --- cMenuCatValuesSelect --------------------------------------------------------
cMenuCatValuesSelect::cMenuCatValuesSelect(char* CatValues, int CatIndex, int SearchMode)
:cOsdMenu(tr("Values for EPG category"), 1, 40)
{
    catValues = CatValues;
    catIndex = CatIndex;
    searchMode = SearchMode;
    cSearchExtCat* SearchExtCat = SearchExtCats.Get(catIndex);
    if (SearchExtCat)
    {
	sel_cats.assign(SearchExtCat->nvalues, false);
	for(int i=0; i<SearchExtCat->nvalues; i++)
	{
 	    char *pstrSearchToken, *pptr;
	    char *pstrSearch=strdup(catValues);
	    pstrSearchToken=strtok_r(pstrSearch, ",;|~", &pptr);
	    while(pstrSearchToken) 
	    {
		if(SearchExtCat->values[i] && strcmp(SearchExtCat->values[i], skipspace(pstrSearchToken))==0) 
		    sel_cats[i] = true;
		pstrSearchToken=strtok_r(NULL, ",;|~", &pptr);
	    }
	    free(pstrSearch);	    
	}
    }
    Set();
    SetHelp(tr("Button$On/Off"), NULL, NULL, tr("Button$Apply"));
}

void cMenuCatValuesSelect::Set()
{
    int current = Current();
    int selCount = 0;
    Clear();
    
    char* SearchMode = NULL;
    if (searchMode == 0)
	asprintf(&SearchMode, "%s: %s", tr("Search mode"), tr("phrase"));
    else if (searchMode == 1)
	asprintf(&SearchMode, "%s: %s", tr("Search mode"), tr("all words"));
    else if (searchMode == 2)
	asprintf(&SearchMode, "%s: %s", tr("Search mode"), tr("at least one word"));
    else if (searchMode == 3)
	asprintf(&SearchMode, "%s: %s", tr("Search mode"), tr("match exactly"));
    cOsdItem* sItem = new cOsdItem(SearchMode);
    Add(sItem);
    sItem->SetSelectable(false);
    free(SearchMode);

    cSearchExtCat* SearchExtCat = SearchExtCats.Get(catIndex);
    if (SearchExtCat)
    {
	for(int i=0; i<SearchExtCat->nvalues; i++)
	{
	    char* entry = NULL;
	    asprintf(&entry, "%c\t%s", sel_cats[i]?'*':' ',  SearchExtCat->values[i]);
	    if (sel_cats[i]) selCount++;
	    Add(new cOsdItem(entry));
	    free(entry);
	}
    }
    SetCurrent(Get(current));
    char *title = NULL;
    asprintf(&title, "%s (%d/%d)", tr("Values for EPG category"), selCount, SearchExtCat->nvalues);
    SetTitle(title);
    Display();
    free(title);

}

eOSState cMenuCatValuesSelect::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown) {
	switch (Key) {
	    case kOk:
	    case kRed:
		if (Current()>0)
		{
		    sel_cats[Current()-1] = !sel_cats[Current()-1];
		    Set();
		    Display();
		}
		break;
	    case kBlue:
	    {
		*catValues = 0;
		cSearchExtCat* SearchExtCat = SearchExtCats.Get(catIndex);
		if (SearchExtCat)
		{
		    for(int i=0; i<SearchExtCat->nvalues; i++)
		    {
			if (sel_cats[i])
			{
			    if (*catValues)
				strcat(catValues, ", ");
			    strcat(catValues, SearchExtCat->values[i]);
			}
		    }
		}
		state = osBack;
		break;
	    }
	    default: break;
	}
    }
    return state;
}


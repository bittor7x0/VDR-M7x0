/* zaphistoryosd.c: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id:
 */

#include "zaphistoryosd.h"
#include "zaphistory.h"
#include "zaplist.h"
#include "setup-zaphistory.h"
#include "statistic_item.h"
#include "menu_reset.h"
#include "epg_item.h"

#include <vdr/channels.h>
#include <vdr/menu.h>
#include <vdr/interface.h>

cMenuZappedChannels::cMenuZappedChannels()
:cOsdMenu(tr("Zapping History") )
{
  // Init members
  
  // Start with view mode from setup
  viewMode = ZapHistorySetup.DefaultView;
  
  // Start with sort mode from setup
  ZapHistory.SetSortMode( ZapHistorySetup.DefaultSort );
  
  inResetMode = false;
  
  // Build the menu
  Setup();
}

void cMenuZappedChannels::Setup() {
  Clear(); // Remove old menuitems
  ZapHistory.UpdateWatchTime(); // calculate watchtime in history and refresh channel instances
  
  // Sort history
  ZapHistory.Sort();
  
  switch(viewMode) { // build menu
    case historyView: SetupEPGView(); break;
    case statisticView: SetupStatisticView(); break;
  }
  
  Display();
}

// Builds a non-selectable menu item to display current sort mode
void cMenuZappedChannels::SortInfoItem() {
  char *buffer = NULL;
  const char *sortField = NULL;
  
  switch (ZapHistory.GetSortMode() ) {
    case historySort: sortField = tr("Latest view"); break;
    case zapcountSort: sortField = tr("Zap count"); break;
    case watchtimeSort: sortField = tr("View time"); break;
  }
  
  asprintf(&buffer, "--- %s '%s' -----------------------------------------------------", tr("Sorted by"), sortField );
  Add( new cOsdItem( buffer, osUnknown, false));
  free(buffer);
}

// Builds the EPG based view
void cMenuZappedChannels::SetupEPGView() {
  // Get schedules to retrieve EPG data
  cSchedulesLock *schedLock = new cSchedulesLock();
  const cSchedules *schedules = cSchedules::Schedules( *schedLock );

  // schedules not available
  if (schedules == NULL) {
    esyslog("ZAPHISTORY: could not get schedules - switching to statistic view");
    SetupStatisticView();
    return;
  }
    
  // Set tab-stops for menu columns
  SetCols( 10, 6, 6, 4);

  SortInfoItem();
  
  // Init history counter
  int historyPosition = 1;
  const cEvent *event = NULL;
  
  // Loop history objects
  for (cZapHistoryChannel *zapChannel = ZapHistory.First(); zapChannel; zapChannel = ZapHistory.Next(zapChannel)) {
    if (zapChannel->GetChannel() == NULL) {
    	esyslog("ZAPHISTORY: history item with channel=NULL - ignoring" );
	continue;
    }
  
    // As long as history counter as not greater that maxEntry from setup
    if (ZapHistorySetup.EntryCount == 0 || historyPosition <= ZapHistorySetup.EntryCount) {
	// get schedule for history item
#if VDRVERSNUM >= 10338
	const cSchedule *schedule = schedules->GetSchedule( zapChannel->GetChannel() );
#else
	const cSchedule *schedule = schedules->GetSchedule( zapChannel->GetChannelID() );
#endif
	if (schedule == NULL) {
	    event = NULL;
	} else {
	    event = schedule->GetEventAround(time(NULL));
	}

	// add menuitem
	Add( new cMenuMyScheduleItem( event, zapChannel, 1 ));
    } else // output complete
	break;
    historyPosition++;
  }
  
  // release schedules
  delete schedLock;
  
  // buttons
  SetHelp(tr("Button$Sort"), tr("Button$Statistic"),  tr("Button$Reset"), tr("Button$Info") );
}

// Builds the statistic view
void cMenuZappedChannels::SetupStatisticView(void)
{
  // Set tab-stops for menu columns
  SetCols( 25, 10, 10);

  SortInfoItem();

  char *buffer = NULL;
  asprintf(&buffer, "%s\t%s\t%s", tr("Channel"), tr("Zap count"), tr("View time") );

  Add( new cOsdItem( buffer, osUnknown, false));
  free(buffer);
  
  // Init history counter
  int historyPosition = 1;  
  // Loop history objects
  for (cZapHistoryChannel *zapChannel = ZapHistory.First(); zapChannel; zapChannel = ZapHistory.Next(zapChannel)) {
    if (ZapHistorySetup.EntryCount == 0 || historyPosition <= ZapHistorySetup.EntryCount) {
	// add menuitem    
        Add( new cZapHistoryChannelItem( zapChannel ));
    } else
	break;
	
    historyPosition++;
  }
  
  // buttons
  SetHelp(tr("Button$Sort"), tr("Button$Schedule"), tr("Button$Reset"), tr("Button$Info") );
}

// Returns the currently selcted channel
cZapHistoryChannel *cMenuZappedChannels::GetChannel(int Index)
{
  if (viewMode == historyView && Index < 1)
    return NULL;
  else if (viewMode == statisticView && Index < 2)
    return NULL;
    
    
  cZapHistoryOsdItem *selected = (cZapHistoryOsdItem*) Get(Index);
  
  if (selected == NULL)
    return NULL;
  
  return selected->Channel();
}

eOSState cMenuZappedChannels::Switch(void)
{
  cZapHistoryChannel *zapChan = GetChannel(Current());
  if (zapChan && zapChan->GetChannel() ) {
     return cDevice::PrimaryDevice()->SwitchChannel(zapChan->GetChannel(), true) ? osEnd : osContinue;
     return osEnd;
  }
  
  return osUnknown;
}

eOSState cMenuZappedChannels::ShowInfo() {
    if (HasSubMenu())
	return osUnknown;
	
    cZapHistoryChannel *channel = GetChannel(Current());
	    
    if (channel == NULL || channel->GetChannel() == NULL)
	return osUnknown;
      
    // Get schedules to retrieve EPG data
    cSchedulesLock *schedLock = new cSchedulesLock();
    const cSchedules *schedules = cSchedules::Schedules( *schedLock );

#if VDRVERSNUM >= 10338
    const cSchedule *schedule = schedules->GetSchedule( channel->GetChannel() );
#else
    const cSchedule *schedule = schedules->GetSchedule( channel->GetChannel()->GetChannelID() );
#endif
    delete schedLock;

    if (schedule == NULL)
	return osUnknown;

    return AddSubMenu( new cMenuEvent(schedule->GetEventAround(time(NULL)) ) );
    
}

eOSState cMenuZappedChannels::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
      case kBlue: // Show EPG info
	ShowInfo(); break;
	
      case kOk: // Switch channel 
        return Switch();
      
      case kGreen: // switch view mode
	switch( viewMode ) {
	    case historyView: viewMode = statisticView; break;
	    case statisticView: viewMode = historyView; break;
	}    
      
        Setup(); // rebuild menu
	break;
      
      case kYellow:
        AddSubMenu( new cMenuHistoryReset( GetChannel( Current() ) ) );
	inResetMode = true;
        break;
      case kRed: // sorting
        // TODO: menu
	switch( ZapHistory.GetSortMode() ) {
	    case historySort: ZapHistory.SetSortMode( zapcountSort ); break;
	    case zapcountSort: ZapHistory.SetSortMode( watchtimeSort ); break;	
	    case watchtimeSort: ZapHistory.SetSortMode( historySort ); break;	    
	}
	
	Setup();
	break;
	    
      default: break;
    }
  }

  // Rebuild after returning from Reset menu
  if (inResetMode && !HasSubMenu()) {
    Setup();
    inResetMode = false;
  }

  return state;
}

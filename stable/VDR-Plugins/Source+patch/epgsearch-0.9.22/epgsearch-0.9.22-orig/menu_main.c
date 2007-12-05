/* 
 * menu_main.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <vector>
#include "menu_main.h"
#include "menu_whatson.h"
#include "menu_myedittimer.h"
#include "epgsearchext.h"
#include "menu_event.h"
#include "menu_searchresults.h"
#include "menu_search.h" 
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include <vdr/menu.h>
#include "menu_conflictcheck.h"
#include "menu_favorites.h"
#include "menu_deftimercheckmethod.h"

int toggleKeys=0;
int exitToMainMenu = 0;
extern int gl_InfoConflict;

// --- cMenuSearchMain ---------------------------------------------------------
cMenuSearchMain::cMenuSearchMain(void)
:cOsdMenu("", GetTab(1), GetTab(2), GetTab(3), GetTab(4), GetTab(5))
{
    otherChannel = 0;
    toggleKeys = 0;
    shiftTime = 0;
    InWhatsOnMenu = false;
    InFavoritesMenu = false;
    cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
    if (channel) {
	cMenuWhatsOnSearch::SetCurrentChannel(channel->Number());
	schedules = cSchedules::Schedules(schedulesLock);
	PrepareSchedule(channel);
	SetHelpKeys();
	cMenuWhatsOnSearch::currentShowMode = showNow;
    }
    if (EPGSearchConfig.StartMenu == 1)
    {
	InWhatsOnMenu = true;
	AddSubMenu(new cMenuWhatsOnSearch(schedules, cDevice::CurrentChannel()));
    }
}

cMenuSearchMain::~cMenuSearchMain()
{
    cMenuWhatsOnSearch::ScheduleChannel(); // makes sure any posted data is cleared
}

int cMenuSearchMain::GetTab(int Tab)
{
    return cTemplFile::GetTemplateByName("MenuSchedule")->Tab(Tab-1);
}

void cMenuSearchMain::PrepareSchedule(cChannel *Channel)
{
    Clear();
    char *buffer = NULL;
    asprintf(&buffer, "%s - %s", tr("Schedule"), Channel->Name());
    SetTitle(buffer);
    free(buffer);

    cMenuTemplate* ScheduleTemplate = cTemplFile::GetTemplateByName("MenuSchedule");
    eventObjects.Clear();

   if (schedules) {
	const cSchedule *Schedule = schedules->GetSchedule(Channel->GetChannelID());
	currentChannel = Channel->Number();
	if (Schedule && Schedule->Events()->First()) 
	{
	    const cEvent *PresentEvent = Schedule->GetPresentEvent();
	    time_t now;
	    if (shiftTime == 0)
		now = time(NULL) - Setup.EPGLinger * 60;
	    else
		now = time(NULL) + shiftTime * 60;
	    time_t lastEventDate = Schedule->Events()->First()->StartTime();
	    for (const cEvent *Event = Schedule->Events()->First(); Event; Event = Schedule->Events()->Next(Event)) {
		if (Event->EndTime() > now || (shiftTime==0 && Event == PresentEvent))
		{
		    if (EPGSearchConfig.showDaySeparators)
		    {
			struct tm tm_rEvent;
			struct tm tm_rLastEvent;
			time_t EventDate = Event->StartTime();
			struct tm *t_event = localtime_r(&EventDate, &tm_rEvent);			
			struct tm *t_lastevent = localtime_r(&lastEventDate, &tm_rLastEvent);			
			if (t_event->tm_mday != t_lastevent->tm_mday)
			{
			    char* szSep = NULL;
			    asprintf(&szSep, "----------------------------------\t %s ----------------------------------------------------------------------------------------------", GETDATESTRING(Event));
			    cOsdItem* pSepItem = new cOsdItem(szSep);
			    free(szSep);
			    pSepItem->SetSelectable(false);
			    Add(pSepItem);
			}
			lastEventDate = EventDate;
		    }
		    Add(new cMenuMyScheduleItem(Event, NULL, showNow, ScheduleTemplate), Event == PresentEvent);
            eventObjects.Add(Event);
		}
            }
        }
    }
    if (shiftTime)
    {
	char *buffer = NULL;
	asprintf(&buffer, "%s (%s%dh %dm)", Channel->Name(), shiftTime>0?"+":"", shiftTime/60, shiftTime%60);
	SetTitle(buffer);
	free(buffer);
    }
}

bool cMenuSearchMain::Update(void)
{
  bool result = false;
  for (cOsdItem *item = First(); item; item = Next(item)) {
      if (item->Selectable() && ((cMenuMyScheduleItem *)item)->Update())
         result = true;
      }
  return result;
}

eOSState cMenuSearchMain::Record(void)
{
  cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
  if (item) {
      if (item->timerMatch == tmFull) 
      {
	  int tm = tmNone;
	  cTimer *timer = Timers.GetMatch(item->event, &tm);
	  if (timer)
	      if (EPGSearchConfig.useVDRTimerEditMenu)
		  return AddSubMenu(new cMenuEditTimer(timer));
	      else
		  return AddSubMenu(new cMenuMyEditTimer(timer, false, item->event, item->channel));
      }

     cTimer *timer = new cTimer(item->event);
     PrepareTimerFile(item->event, timer);
     cTimer *t = Timers.GetTimer(timer);
     if (EPGSearchConfig.onePressTimerCreation == 0 || t || !item->event || (!t && item->event && item->event->StartTime() - (Setup.MarginStart+2) * 60 < time(NULL))) 
     {
	 if (t)
	 {
	     delete timer;
	     timer = t;      
	 }
	 if (EPGSearchConfig.useVDRTimerEditMenu)
	     return AddSubMenu(new cMenuEditTimer(timer, !t));
	 else
	     return AddSubMenu(new cMenuMyEditTimer(timer, !t, item->event));     
     }
     else
     {
	 string fullaux = "";
	 if (item->event)
	 {
	     const cEvent* event = item->event;
	     int bstart = event->StartTime() - timer->StartTime();
	     int bstop = timer->StopTime() - event->EndTime();
	     string aux = "";
	     int checkmode = DefTimerCheckModes.GetMode(timer->Channel());
	     aux = UpdateAuxValue(aux, "channel", NumToString(timer->Channel()->Number()) + " - " + CHANNELNAME(timer->Channel()));
	     aux = UpdateAuxValue(aux, "update", checkmode);
	     aux = UpdateAuxValue(aux, "eventid", event->EventID());
	     aux = UpdateAuxValue(aux, "bstart", bstart);
	     aux = UpdateAuxValue(aux, "bstop", bstop);
	     fullaux = UpdateAuxValue(fullaux, "epgsearch", aux);
	 }
	 SetAux(timer, fullaux);
	 Timers.Add(timer);
	 timer->Matches();
	 Timers.SetModified();
	 LogFile.iSysLog("timer %s added (active)", *timer->ToDescr());

	 if (HasSubMenu())
	     CloseSubMenu();
	 if (Update())
	     Display();
	 SetHelpKeys();
     }
  }
  return osContinue;
}

eOSState cMenuSearchMain::Switch(void)
{
  cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
  if (item) {
     cChannel *channel = Channels.GetByChannelID(item->event->ChannelID(), true, true);
     if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
        return osEnd;
     }
  Skins.Message(mtInfo, tr("Can't switch channel!")); 
  return osContinue;
}

eOSState cMenuSearchMain::ExtendedSearch(void)
{
    return AddSubMenu(new cMenuEPGSearchExt());
}
 
eOSState cMenuSearchMain::Commands(eKeys Key)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
  if (mi && mi->event) {
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), mi->event, true));
        if (Key != kNone)
           state = menu->ProcessKey(Key);
        return state;
     }
  return osContinue;
}

eOSState cMenuSearchMain::ShowSummary()
{
   if (Count())
   {
      cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
      if (mi && mi->event) 
         return AddSubMenu(new cMenuEventSearch(mi->event, eventObjects, SurfModeTime));
   }
   return osContinue;
}

void cMenuSearchMain::SetHelpKeys()
{
    cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
    bool hasTimer = item && item->Selectable() && item->timerMatch == tmFull;

    if (toggleKeys==0)
	SetHelp((EPGSearchConfig.redkeymode==0?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), tr("Button$Now"), tr("Button$Next"), EPGSearchConfig.bluekeymode==0?tr("Button$Switch"):tr("Button$Search"));
    else
    {	
	const char* szGreenToggled = CHANNELNAME(Channels.GetByNumber(currentChannel-1,-1));
	const char* szYellowToggled = CHANNELNAME(Channels.GetByNumber(currentChannel+1,1));
	    
	SetHelp((EPGSearchConfig.redkeymode==1?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), (EPGSearchConfig.toggleGreenYellow==0?tr("Button$Now"):szGreenToggled), (EPGSearchConfig.toggleGreenYellow==0?tr("Button$Next"):szYellowToggled), EPGSearchConfig.bluekeymode==1?tr("Button$Switch"):tr("Button$Search"));
    }
}

eOSState cMenuSearchMain::Shift(int iMinutes)
{
    shiftTime += iMinutes;
    cChannel *channel = Channels.GetByNumber(currentChannel);
    PrepareSchedule(channel);
    Display();
    SetHelpKeys();
    return osContinue;
}

eOSState cMenuSearchMain::ProcessKey(eKeys Key)
{ 
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (exitToMainMenu == 1)
    {
	exitToMainMenu = 0;
	return osBack;
    }

   if (!HasSubMenu() && HadSubMenu)
   {
      // navigation in summary could have changed current item, so update it
      cEventObj* cureventObj = eventObjects.GetCurrent();
      if (cureventObj && cureventObj->Event())
         for (cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)First(); item; item = (cMenuMyScheduleItem *)Next(item)) 
            if (item->event == cureventObj->Event())
            {
               cureventObj->Select(false);
               SetCurrent(item);
               Display();
               break;
            }               
   }

    if (state == osUnknown) {
	switch (Key) {
	 case kFastRew:
	     if (HasSubMenu() && !InWhatsOnMenu && !InFavoritesMenu)
	     {
/*            if (Count())
            {
               CursorUp();
               return ShowSummary();
            }
*/	     }
	     else
            return Shift(-EPGSearchConfig.timeShiftValue);
	 case kFastFwd:
	     if (HasSubMenu() && !InWhatsOnMenu && !InFavoritesMenu)
	     {
/*		 if (Count())
		 {
		     CursorDown();
		     return ShowSummary();
		 }
*/	     }
	     else
		 return Shift(EPGSearchConfig.timeShiftValue);
	 case kRecord:
	 case kRed:
	     if(HasSubMenu()) {
		 state = Record();
		 break;
	     } 
	     if (Count())
	     {
		 if (EPGSearchConfig.redkeymode==toggleKeys)
		     state = Record();
		 else
		 {
		     cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
		     if (mi) {
			 if (mi->event) {
			     return AddSubMenu(new cMenuSearchCommands(tr("EPG Commands"),mi->event));
			 }
		     }
		 }
	     }
	     break;
	 case k0:
	     if(!HasSubMenu()) 
	     {
		 toggleKeys = 1 - toggleKeys;
		 SetHelpKeys();
	     }
	     state = osContinue;
	     break;
	 case k1...k9: 
	     if (!HasSubMenu())
		 return Commands(Key);
	     else
		 state = osContinue;
	     break;
	 case kGreen:  
	     if (schedules) 
	     {
		 if (HasSubMenu() && !InWhatsOnMenu && !InFavoritesMenu)
		 {
		     if (Count())
		     {
//			 CursorUp();
//			 return ShowSummary();
		     }
		 }
		 else if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0))
		 {
		     int ChannelNr = cDevice::CurrentChannel();
		     if (Count()) {
			 cMenuMyScheduleItem* Item = (cMenuMyScheduleItem *)Get(Current());
			 if (Item && Item->event)
			 {
			     cChannel *channel = Channels.GetByChannelID(Item->event->ChannelID(), true, true);
			     if (channel)
				 ChannelNr = channel->Number();
			 }
		     }
		     if (cMenuWhatsOnSearch::currentShowMode == showFavorites)
		     {
			 InFavoritesMenu = true;
			 return AddSubMenu(new cMenuFavorites());
		     }
		     else
		     {
			 InWhatsOnMenu = true;
			 return AddSubMenu(new cMenuWhatsOnSearch(schedules, ChannelNr));
		     }
		 }
		 else 
		 {
		     cChannel *channel = Channels.GetByNumber(currentChannel-1,-1);
		     
		     if (channel) {
			 PrepareSchedule(channel);
			 if (channel->Number() != cDevice::CurrentChannel()) {
			     otherChannel = channel->Number();
			 }
			 Display();
		     }
		     SetHelpKeys();
		     return osContinue;
		 }
	     }
	 case kYellow: 
	     if (schedules)
	     {
		 if (HasSubMenu())
		 {
		     if (Count())
		     {
//			 CursorDown();
//			 return ShowSummary();
		     }
		 }
		 else if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0))
		 {
		     cMenuWhatsOnSearch::currentShowMode = showNext;
		     InWhatsOnMenu = true;
		     return AddSubMenu(new cMenuWhatsOnSearch(schedules, cMenuWhatsOnSearch::CurrentChannel()));
		 }
		 else
		 {
		     cChannel *channel = Channels.GetByNumber(currentChannel+1,1);
		     if (channel) {
			 PrepareSchedule(channel);
			 if (channel->Number() != cDevice::CurrentChannel()) {
			     otherChannel = channel->Number();
			 }
			 Display();
		     }
		     SetHelpKeys();
		     return osContinue;
		 }
	     }
	     break;
	 case kBlue:
	     if (HasSubMenu())
		 return Switch();
	     if (EPGSearchConfig.bluekeymode==toggleKeys)
		 return Switch();
	     else
		 return ExtendedSearch();
	     break;
       case kOk: 
	   if (Count())
	       return ShowSummary();
	   break;
	 default:
	     break;
     }
  }
  if (!HasSubMenu()) {
      cChannel *ch = cMenuWhatsOnSearch::ScheduleChannel();
      InWhatsOnMenu = false;
      InFavoritesMenu = false;
      if (ch) {
	  PrepareSchedule(ch);
	  if (ch->Number() != cDevice::CurrentChannel()) {
	      otherChannel = ch->Number();
	  }
	  Display();
      }
      else if ((HadSubMenu || gl_TimerProgged) && Update())
      {
	  if (gl_TimerProgged) // when using epgsearch's timer edit menu, update is delayed because of SVDRP
	  {
	      gl_TimerProgged = 0;
	      SetHelpKeys();
	  }
	  Display();
      }
      if (Key != kNone)
	  SetHelpKeys();
      if (gl_InfoConflict)
      {
	  gl_InfoConflict = 0;
	  if (Interface->Confirm(tr("Timer conflict! Show?")))
	      return AddSubMenu(new cMenuConflictCheck());
      }      
  }
  return state;
}


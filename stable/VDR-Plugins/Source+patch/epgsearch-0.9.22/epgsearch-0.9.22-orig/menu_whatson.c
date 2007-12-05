/*
 * menu_whatson.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vector>
#include <string>
#include "uservars.h"
#include "menu_whatson.h"
#include "epgsearchext.h"
#include "menu_event.h"
#include "menu_myedittimer.h"
#include "menu_searchresults.h"
#include "menu_search.h" 
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include "switchtimer.h"
#include "epgsearchcats.h"
#include "conflictcheck.h"
#include <math.h>
#include <vdr/menu.h>
#include "menu_conflictcheck.h"
#include "templatefile.h"
#include "menu_deftimercheckmethod.h"

#define HOURS(x) ((x)/100)
#define MINUTES(x) ((x)%100)

#define SKIPHOURS 20
#define HOURS2SECS(x) (x*60*60)

extern int exitToMainMenu;
int gl_InfoConflict = 0;

// --- cMenuMyScheduleItem ------------------------------------------------------
cMenuMyScheduleItem::cMenuMyScheduleItem(const cEvent *Event, cChannel *Channel, showMode Mode, cMenuTemplate* MenuTemplate)
{
   event = Event;
   channel = Channel;
   mode = Mode;
   timerMatch = tmNone;
   inSwitchList = false;
   menuTemplate = MenuTemplate;
   Update(true);
}

bool cMenuMyScheduleItem::Update(bool Force)
{
   if (!menuTemplate)
      return false;

   const char* menutemplate = menuTemplate->MenuTemplate();
   if (!menutemplate || strlen(menutemplate) == 0)
      return false;

   bool result = false;

   int OldTimerMatch = timerMatch;
   bool OldInSwitchList = inSwitchList;
   bool hasMatch = false; 
   cTimer* timer = NULL;
   if (event) timer = Timers.GetMatch(event, &timerMatch);
   if (event) inSwitchList = (SwitchTimers.InSwitchList(event)!=NULL);
   if (timer) hasMatch = true;
    
   if (Force || timerMatch != OldTimerMatch || inSwitchList != OldInSwitchList) 
   {
      char szProgressPart[12] = "";
      char szProgressPartT2S[12] = "";
      if (channel)
      {
         if (event)
         {
            int frac = 0;
            if (mode == showNow)
               frac = (int)roundf( (float)(time(NULL) - event->StartTime()) / (float)(event->Duration()) * 8.0 );
            if (mode == showNext)
               frac = (int)roundf( (float)(1 - min(float(30*60), float(event->StartTime() - time(NULL))) / (30*60)) * 8.0 );
            frac = min(8,max(0, frac));
		
            char szProgressT2S[9] = "";
            for(int i = 0; i < frac; i++) szProgressT2S[i] = '|';
            szProgressT2S[frac]=0;
            sprintf(szProgressPartT2S, "%c%-8s%c", '[', szProgressT2S, ']');

            char szProgress[9] = {130,131,131,131,131,131,131,132,0};
            for(int i = 0; i < frac; i++) szProgress[i] = 127;
            sprintf(szProgressPart, "%-8s", szProgress);
         }
      }
      
      char t[2]="",v[2]="",r[2]="";
      char szStatus[4] = "";
      if (EPGSearchConfig.WarEagle)
      {
         sprintf(t, "%c", event && hasMatch ? (timerMatch == tmFull) ? ((timer && timer->Recording())?249:253) : 't' : ' ');
         sprintf(v, "%c", event && event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ');
         sprintf(r, "%c", event && event->IsRunning() ? 251 : ' ');
      }
      else
      {
         sprintf(t, "%c", event && hasMatch ? (timerMatch == tmFull) ? ((timer && timer->Recording())?'R':'T') : 't' : ' ');
         sprintf(v, "%c", event && event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ');
         sprintf(r, "%c", event && event->IsRunning() ? '*' : ' ');
      }

      if (event && inSwitchList)
      {
         cSwitchTimer* s = SwitchTimers.InSwitchList(event);
         sprintf(t, "%c", (s && s->announceOnly)?'s':'S');
      }
      sprintf(szStatus, "%s%s%s", t,v,r);

      char* buffer = strdup(menutemplate);
      char* tmp = strreplaceall(buffer, '|', "\t");
      free(buffer);
      buffer = tmp;

      buffer = strreplacei(buffer, "%title%", event?event->Title():tr(">>> no info! <<<"));
      if (channel)
      {
         char szChannelNr[6] = "";
         if (channel)
            sprintf(szChannelNr, "%d", channel->Number());
         buffer = strreplacei(buffer, "%chnr%", szChannelNr);
         buffer = strreplacei(buffer, "%chsh%", channel?channel->ShortName(true):"");
         buffer = strreplacei(buffer, "%chlng%", channel?channel->Name():"");
         buffer = strreplacei(buffer, "%progr%", szProgressPart);
         buffer = strreplacei(buffer, "%progrT2S%", szProgressPartT2S);
      }

      // parse the epxression and evaluate it
      cVarExpr varExpr(buffer);
      tmp = strdup(varExpr.Evaluate(event).c_str());
      free(buffer);
      buffer = tmp;

      buffer = strreplacei(buffer, "%status%", szStatus);
      buffer = strreplacei(buffer, "%t_status%", t);
      buffer = strreplacei(buffer, "%v_status%", v);
      buffer = strreplacei(buffer, "%r_status%", r);


      buffer = FixSeparators(buffer, '~');
      buffer = FixSeparators(buffer, ':');
      buffer = FixSeparators(buffer, '-');

      SetText(buffer, false);

      if (gl_InfoConflict == 0 && EPGSearchConfig.checkTimerConflAfterTimerProg && !Force && timer && timerMatch && timerMatch != OldTimerMatch)
      {
         cConflictCheck C;
         C.Check();
         if (C.TimerInConflict(timer))
            gl_InfoConflict = 1;	  
      }
      return true;      
   }
   return result;
}

// --- cMenuWhatsOnSearch ----------------------------------------------------------

int cMenuWhatsOnSearch::currentChannel = 0;
showMode cMenuWhatsOnSearch::currentShowMode = showNow;
cChannel *cMenuWhatsOnSearch::scheduleChannel = NULL;
extern const char *ShowModes[];
cList<cShowMode> cMenuWhatsOnSearch::showModes;

cMenuWhatsOnSearch::cMenuWhatsOnSearch(const cSchedules *Schedules, int CurrentChannelNr)
   :cOsdMenu("", GetTab(1), GetTab(2), GetTab(3), GetTab(4), GetTab(5))
{
   shiftTime = 0;
   schedules = Schedules;

   // do a font patch, if necessary
   if (strcasestr(cTemplFile::GetTemplateByName("MenuWhatsOnNow")->MenuTemplate(), "%progr%") ||
       strcasestr(cTemplFile::GetTemplateByName("MenuWhatsOnNext")->MenuTemplate(), "%progr%"))
   {
      if(Setup.UseSmallFont==2)  
         PatchFont(fontSml);
      else
         PatchFont(fontOsd);	
   }

   CreateShowModes();

   LoadSchedules();

   currentChannel = CurrentChannelNr;
    
   SetHelpKeys();
}

cMenuWhatsOnSearch::~cMenuWhatsOnSearch()
{
}

int cMenuWhatsOnSearch::GetTab(int Tab)
{
   if (currentShowMode == showNow)
      return cTemplFile::GetTemplateByName("MenuWhatsOnNow")->Tab(Tab-1);
   if (currentShowMode == showNext)
      return cTemplFile::GetTemplateByName("MenuWhatsOnNext")->Tab(Tab-1);
   if (currentShowMode > showNext)
      return cTemplFile::GetTemplateByName("MenuWhatsOnElse")->Tab(Tab-1);
   else
      return 0;
}

void cMenuWhatsOnSearch::LoadSchedules()
{
   Clear();
   eventObjects.Clear();
   
   time_t SeekTime;
   char* szTitle = NULL;
   cShowMode* mode = GetShowMode(currentShowMode);

   if (shiftTime != 0)
   {
      if (currentShowMode == showNow || currentShowMode == showNext)
         SeekTime = time(NULL);
      else
      {
         if (mode)
            SeekTime = GetTimeT(mode->GetTime());
         if (SeekTime < time(NULL)) SeekTime += HOURS2SECS(24);	
      }
      SeekTime += shiftTime*60;
	
      struct tm tm_r;
      time_t now = time(NULL);

      tm tm_seek = *localtime_r(&SeekTime, &tm_r);
      tm tm_now = *localtime_r(&now, &tm_r);
      if (tm_seek.tm_mday != tm_now.tm_mday)
         asprintf(&szTitle, "%s - %s", tr("Overview"), DAYDATETIME(SeekTime));
      else
         asprintf(&szTitle, "%s - %02d:%02d", tr("Overview"), tm_seek.tm_hour, tm_seek.tm_min);
   }
   else
   {
      if (mode)
      {
         SeekTime = GetTimeT(mode->GetTime());
         if (SeekTime < time(NULL) && currentShowMode != showNow && currentShowMode != showNext)
         {
            SeekTime += HOURS2SECS(24);	
            asprintf(&szTitle, "%s - %s (%s)", tr("Overview"), mode->GetDescription(), *WeekDayName(SeekTime));    
         }
         else
            asprintf(&szTitle, "%s - %s", tr("Overview"), mode->GetDescription());    
      }
   }
   SetTitle(szTitle);
   free(szTitle);

   cMenuTemplate* currentTemplate = NULL;
   if (currentShowMode == showNow)
      currentTemplate = cTemplFile::GetTemplateByName("MenuWhatsOnNow");
   if (currentShowMode == showNext)
      currentTemplate = cTemplFile::GetTemplateByName("MenuWhatsOnNext");
   if (currentShowMode > showNext)
      currentTemplate = cTemplFile::GetTemplateByName("MenuWhatsOnElse");

   int maxChannel = EPGSearchConfig.maxChannelMenuNow;
   if (currentChannel > maxChannel)
      maxChannel = 0;

   for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) 
   {	
      if (!Channel->GroupSep()) 
      {
         if (maxChannel && Channel->Number() > maxChannel) break;
         if (EPGSearchConfig.showRadioChannels == 0 && ISRADIO(Channel))
            continue;

         const cSchedule *Schedule = schedules->GetSchedule(Channel->GetChannelID());
         const cEvent *Event = NULL;
         if (Schedule) 
         {		
            if (shiftTime != 0)
               Event = Schedule->GetEventAround(SeekTime);
            else
            {
               switch(currentShowMode)
               {  
                  default:
                  case showNow: 
                     Event = Schedule->GetPresentEvent();
                     break;
                  case showNext:
                     Event = Schedule->GetFollowingEvent();
                     break;
                  case showUserMode1:
                  case showUserMode2:
                  case showUserMode3:
                  case showUserMode4:
                     Event = Schedule->GetEventAround(SeekTime);
                     break;
               }
            }
	     }
	     if (!EPGSearchConfig.showEmptyChannels && !Event)
            continue;

	     Add(new cMenuMyScheduleItem(Event, Channel, currentShowMode, currentTemplate), Channel->Number() == currentChannel);
         if (Event) eventObjects.Add(Event);            
      }
      else
      {
         if (EPGSearchConfig.showChannelGroups)
         {
            char* szGroup = NULL;
            asprintf(&szGroup, "----------------------------------------\t %s ----------------------------------------------------------------------------------------------", Channel->Name());
            cOsdItem* pGroupItem = new cOsdItem(szGroup);
            free(szGroup);
            pGroupItem->SetSelectable(false);
            Add(pGroupItem);
         }
      }
   }
}

time_t cMenuWhatsOnSearch::GetTimeT(int iTime)
{
   struct tm tm_r;
   time_t t = time(NULL);
   tm* tmnow = localtime_r(&t, &tm_r);
   tmnow->tm_hour = HOURS(iTime);
   tmnow->tm_min = MINUTES(iTime);
   return mktime(tmnow);
}

showMode cMenuWhatsOnSearch::GetNextMode()
{
   showMode nextShowMode = currentShowMode;
   cShowMode* Mode = GetShowMode(currentShowMode);
   if (Mode) 
   {
      cShowMode* ModeNext = showModes.Next(Mode);
      if (ModeNext == NULL) 
         nextShowMode = showNow;
      else
         nextShowMode = ModeNext->GetMode();
   }
   return nextShowMode;
}

void cMenuWhatsOnSearch::CreateShowModes()
{
   showModes.Clear();

   cShowMode* ModeNow = new cShowMode(showNow, tr("Button$Now"));
   showModes.Add(ModeNow);
   cShowMode* ModeNext = new cShowMode(showNext, tr("Button$Next"));
   showModes.Add(ModeNext);

   time_t now = time(NULL);
   for(int i=showUserMode1; i<showModeMax; i++)
   {
      if (!EPGSearchConfig.ShowModes[i].GetUsage())
         continue;

      time_t SeekTime = GetTimeT(EPGSearchConfig.ShowModes[i].GetTime());    
      if (SeekTime < now)
         SeekTime += HOURS2SECS(24);
      if (SeekTime - now > HOURS2SECS(SKIPHOURS))
         continue;
      cShowMode* Mode = new cShowMode((showMode)i, EPGSearchConfig.ShowModes[i].GetDescription(), 
                                      1, EPGSearchConfig.ShowModes[i].GetTime(), SeekTime);
      showModes.Add(Mode);
   }
   if (EPGSearchConfig.showFavoritesMenu)
   {
      cShowMode* ModeFav = new cShowMode(showFavorites, tr("Button$Favorites"));
      showModes.Add(ModeFav);
   }
   showModes.Sort();
}

cShowMode* cMenuWhatsOnSearch::GetShowMode(showMode mode)
{
   for (cShowMode *showMode = showModes.First(); showMode; showMode = showModes.Next(showMode)) 
      if (mode == showMode->GetMode())
         return showMode;
   return NULL;
}

cChannel *cMenuWhatsOnSearch::ScheduleChannel(cChannel *force_channel)
{
   cChannel *ch = force_channel?force_channel:scheduleChannel;
   scheduleChannel = NULL;
   return ch;
}


eOSState cMenuWhatsOnSearch::Switch(void)
{
   cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
   if (item && item->channel) {
      if (cDevice::PrimaryDevice()->SwitchChannel(item->channel, true))
         return osEnd;
   }
   Skins.Message(mtInfo, tr("Can't switch channel!")); 
   return osContinue;
}

eOSState cMenuWhatsOnSearch::Record(void)
{
   cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
   if (item) 
   {
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

      cTimer *timer = NULL;
      if (item->event)
      {
         timer = new cTimer(item->event);
         PrepareTimerFile(item->event, timer);
      }
      else
         timer = new cTimer(false, false, item->channel);
 
      cTimer *t = Timers.GetTimer(timer);
      if (EPGSearchConfig.onePressTimerCreation == 0 || t || !item->event || (!t && item->event && item->event->StartTime() - (Setup.MarginStart+2) * 60 < time(NULL))) 
      {
         if (t)
         {
            delete timer;
            timer = t;      
         }
         timer->SetFlags(tfActive);
         if (EPGSearchConfig.useVDRTimerEditMenu)
            return AddSubMenu(new cMenuEditTimer(timer, !t));
         else
            return AddSubMenu(new cMenuMyEditTimer(timer, !t, item->event, item->channel));
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

bool cMenuWhatsOnSearch::Update(void)
{
   bool result = false;
   for (cOsdItem *item = First(); item; item = Next(item)) {
      if (item->Selectable() && ((cMenuMyScheduleItem *)item)->Update())
         result = true;
   }
   return result;
}

eOSState cMenuWhatsOnSearch::Commands(eKeys Key)
{
   if (HasSubMenu() || Count() == 0)
      return osContinue;

   cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
   if (mi) 
   {
      if (!mi->event) 
      {
         if (Key == k3)
            return Switch();
         else if (Key == k2)
            return Record();
         else
            return osContinue;
      }
      cMenuSearchCommands *menu;
      eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), mi->event, true));
      if (Key != kNone)
         state = menu->ProcessKey(Key);
      return state;
   }
   return osContinue;
}

eOSState cMenuWhatsOnSearch::ExtendedSearch(void)
{
   return AddSubMenu(new cMenuEPGSearchExt());
}

void cMenuWhatsOnSearch::SetHelpKeys()
{
   cMenuMyScheduleItem *item = (cMenuMyScheduleItem *)Get(Current());
   bool hasTimer = item && item->Selectable() && item->timerMatch == tmFull;

   showMode nextShowMode = GetNextMode();    
   cShowMode* mode = GetShowMode(nextShowMode);
   char* szButtonGreen = NULL;
   if (mode) 
      szButtonGreen = mode->GetDescription();
   if (toggleKeys==0)
      SetHelp((EPGSearchConfig.redkeymode==0?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), 
              szButtonGreen, 
              tr("Button$Schedule"), 
              EPGSearchConfig.bluekeymode==0?(EPGSearchConfig.useOkForSwitch?tr("Button$Info"):tr("Button$Switch")):tr("Button$Search"));
   else
      SetHelp((EPGSearchConfig.redkeymode==1?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), 
              (EPGSearchConfig.toggleGreenYellow==0?szButtonGreen:"<<"), 
              (EPGSearchConfig.toggleGreenYellow==0?tr("Button$Schedule"):">>"), 
              EPGSearchConfig.bluekeymode==1?(EPGSearchConfig.useOkForSwitch?tr("Button$Info"):tr("Button$Switch")):tr("Button$Search"));
}

eOSState cMenuWhatsOnSearch::Shift(int iMinutes)
{
   shiftTime += iMinutes;
   cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
   int TempChannel = currentChannel;
   if (mi) 
   {
      currentChannel = mi->channel->Number();
      scheduleChannel = Channels.GetByNumber(currentChannel);
   }
   LoadSchedules();
   Display();
   currentChannel = TempChannel;
   SetHelpKeys();
   return osContinue;
}

eOSState cMenuWhatsOnSearch::ShowSummary()
{
   if (Count())
   {
      const cEvent *ei = ((cMenuMyScheduleItem *)Get(Current()))->event;
      if (ei) 
      {
         cChannel *channel = Channels.GetByChannelID(ei->ChannelID(), true, true);
         if (channel)
            return AddSubMenu(new cMenuEventSearch(ei, eventObjects, SurfModeChannel));
      }
   }
   return osContinue;
}

void cMenuWhatsOnSearch::UpdateCurrent()
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

eOSState cMenuWhatsOnSearch::ProcessKey(eKeys Key)
{
   exitToMainMenu = 0;
   if (!HasSubMenu() && Key == kBack)
   {
      exitToMainMenu = 1;
      return osBack;
   }

   bool HadSubMenu = HasSubMenu();
   eOSState state = cOsdMenu::ProcessKey(Key);

   if (!HasSubMenu() && HadSubMenu) // navigation in summary could have changed current item, so update it
      UpdateCurrent();

   if (state == osUnknown) {
      switch (Key) {
         case kFastRew:
            if(!HasSubMenu()) 	     
               return Shift(-EPGSearchConfig.timeShiftValue);
            break;
         case kFastFwd:
            if(!HasSubMenu()) 	     
               return Shift(EPGSearchConfig.timeShiftValue);
            break;
         case kRecord:
         case kRed:
            if(HasSubMenu()) {
               UpdateCurrent();
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
                     if (mi->event) 
                        return AddSubMenu(new cMenuSearchCommands(tr("EPG Commands"),mi->event));
                     else
                        return osContinue;
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
         case k1...k9: return Commands(Key);
         case kYellow: 
            if(!HasSubMenu()) 	     
            {
               if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0))
               {
                  cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
                  if (mi && mi->channel)
                  {
                     const cSchedule *Schedule = schedules->GetSchedule(mi->channel->GetChannelID());
                     if (Schedule)
                     {
                        time_t now = time(NULL);
                        bool hasFutureEvents = false;
                        for (const cEvent *e = Schedule->Events()->First(); e; e = Schedule->Events()->Next(e))
                           if (e->StartTime() > now)
                           {
                              hasFutureEvents = true;
                              break;
                           }
                        if (!hasFutureEvents)
                           return osContinue;
                     }
                     else
                        return osContinue;
                  }
                  state = osBack;
                  // continue with kGreen
               }
               else
                  return Shift(EPGSearchConfig.timeShiftValue);
            }            
         case kGreen:  
            if(!HasSubMenu()) 	     
            {
               if (toggleKeys == 0 || (toggleKeys == 1 && EPGSearchConfig.toggleGreenYellow == 0))
               {
                  if (Key == kYellow)
                     currentShowMode = showNow;
                  else
                     currentShowMode = GetNextMode();
                  cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
                  if (mi) 
                  {
                     currentChannel = mi->channel->Number();
                     scheduleChannel = Channels.GetByNumber(currentChannel);
                  }
               }
               else
                  return Shift(-EPGSearchConfig.timeShiftValue);
            }
            break;
         case kBlue:   
            if (HasSubMenu())
            {
               UpdateCurrent();
               return Switch();
            }
            if (EPGSearchConfig.bluekeymode==toggleKeys)
               return EPGSearchConfig.useOkForSwitch?ShowSummary():Switch();
            else
               return ExtendedSearch();
            break;
         case kOk:
         {
            cMenuMyScheduleItem *mi = (cMenuMyScheduleItem *)Get(Current());
            if (!mi->event) // no EPG, so simply switch to channel
               return Switch();
            else
               return EPGSearchConfig.useOkForSwitch?Switch():ShowSummary();
         }
         break;
         default:      break;
      }
   }
   if (!HasSubMenu()) 
   {
      if ((HadSubMenu || gl_TimerProgged) && Update())
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



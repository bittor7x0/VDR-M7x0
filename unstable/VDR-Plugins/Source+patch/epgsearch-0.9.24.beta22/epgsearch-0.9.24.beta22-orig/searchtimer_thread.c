/*
Copyright (C) 2004-2007 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#include <vector>
#include "searchtimer_thread.h"
#include "epgsearchcfg.h"
#include "epgsearchcats.h"
#include "epgsearchtools.h"
#include "changrp.h"
#include "switchtimer.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>
#include "switchtimer_thread.h"
#include "services.h"
#include "conflictcheck.h"
#include "uservars.h"
#include "svdrpclient.h"
#include "noannounce.h"
#include "timer_thread.h"
#include "timerdone.h"
#include "menu_deftimercheckmethod.h"
#include "timerstatus.h"
#include "epgsearch.h"

// priority for background thread
#define SEARCHTIMER_NICE 19
// delay before first thread activity
#define SEARCHTIMER_WAIT 20

#define DAYBUFFERSIZE 32

extern int updateForced;

cSearchTimerThread *cSearchTimerThread::m_Instance = NULL;
cSearchResults cSearchTimerThread::announceList;

cSearchTimerThread::cSearchTimerThread(cPluginEpgsearch* thePlugin)
   : cThread("EPGSearch: searchtimer")
{
   m_plugin = thePlugin;
   m_Active = false;
   m_lastUpdate = time(NULL);
}

cSearchTimerThread::~cSearchTimerThread() {
   if (m_Active) 
      Stop();
}

void cSearchTimerThread::Init(cPluginEpgsearch* thePlugin, bool activatePermanently) 
{
   if (activatePermanently)
   {
      EPGSearchConfig.useSearchTimers = 1;
      thePlugin->SetupStore("UseSearchTimers",  EPGSearchConfig.useSearchTimers);
   }
   if (!EPGSearchConfig.useSearchTimers)
      return;    
   if (m_Instance == NULL) {
      m_Instance = new cSearchTimerThread(thePlugin);
      m_Instance->Start();
   }
}

void cSearchTimerThread::Exit(void) {
   if (m_Instance != NULL) {
      m_Instance->Stop();
      DELETENULL(m_Instance);
   }
}

void cSearchTimerThread::Stop(void) {
   m_Active = false;
   Cancel(6);
}


cTimer *cSearchTimerThread::GetTimer(cSearchExt *searchExt, const cEvent *pEvent, bool& bTimesMatchExactly)
{
   cChannel *channel = Channels.GetByChannelID(pEvent->ChannelID(), true, true);
   if (!channel)
      return NULL;

   struct tm tm_r;

   bool UseVPS = searchExt->useVPS && pEvent->Vps() && Setup.UseVps;
   time_t eStart = pEvent->StartTime();
   time_t eStop = pEvent->EndTime();
   int eDuration = pEvent->Duration();

   int AllowedDiff = (eDuration < 10*60) ? (eDuration / 60): 10; // allowed start/stop difference

   int testVpsStart = 0;
   int testVpsStop = 0;
   time_t testVpsDay = 0;
   if (UseVPS)
   {
      eStart = pEvent->Vps();
      eStop = eStart + pEvent->Duration();
      cTimer VpsTestTimer(pEvent);
      testVpsStart = cTimer::TimeToInt(VpsTestTimer.Start());
      testVpsStop = cTimer::TimeToInt(VpsTestTimer.Stop());
      testVpsDay = VpsTestTimer.Day();
   }

   tm *tmStartEv = localtime_r(&eStart, &tm_r);
    
   for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti)) 
   {
      if (ti->Channel() != channel)
         continue;

      if (ti->WeekDays()) // ignore serial timers
         continue;

      // ignore manual timers if this search could modify them
      if (searchExt->action == searchTimerActionRecord && TriggeredFromSearchTimerID(ti) == -1) // manual timer
         continue;

      if (UseVPS && ti->HasFlags(tfVps))
      {
         if (testVpsDay != ti->Day()) continue;
         int timerVpsStart = cTimer::TimeToInt(ti->Start());
         int timerVpsStop = cTimer::TimeToInt(ti->Stop());

         if (abs(testVpsStart - timerVpsStart) > AllowedDiff * 60) continue;
         if (abs(testVpsStop - timerVpsStop) > AllowedDiff * 60) continue;

         bTimesMatchExactly = (testVpsStart == timerVpsStart && testVpsStop == timerVpsStop);
         return ti;
      }
      else
      {
         time_t tStart = ti->StartTime() + searchExt->MarginStart * 60;
         time_t tStop = ti->StopTime() - searchExt->MarginStop * 60;
         tm *tmStartTi = localtime_r(&tStart, &tm_r);
         if (tmStartEv->tm_mday != tmStartTi->tm_mday)
            continue;
	    
         bTimesMatchExactly = (tStart == eStart && tStop == eStop);
	
         if (abs(tStart - eStart) < AllowedDiff * 60 && abs(tStop - eStop) < AllowedDiff * 60) // accept a difference of max 10 min., but only if the event duration is more than 10 minutes
            return ti;
      }
   }
   return NULL;
}

bool cSearchTimerThread::TimerWasModified(cTimer* t)
{
   if (!t) return false;
   if (t->HasFlags(tfVps)) return false; // if timer uses VPS we ignore user changes

   char* start = GetAuxValue(t, "start");
   char* stop = GetAuxValue(t, "stop");
   bool bMod = false;
   if (start || stop)
   {
      time_t StartTime = time_t(atol(start));
      time_t StopTime = time_t(atol(stop));
      if (t->StartTime() != StartTime || t->StopTime() != StopTime)
         bMod = true;
   }
   if (start) free(start);
   if (stop) free(stop);
   return bMod;
}

void cSearchTimerThread::Action(void) 
{
   if (EPGSearchConfig.useExternalSVDRP && !cSVDRPClient::SVDRPSendCmd)
   {
      LogFile.eSysLog("ERROR - SVDRPSend script not specified or does not exist (use -f option)");
      return;
   }
   SetPriority(SEARCHTIMER_NICE);

   m_Active = true;
   // let VDR do its startup
   for(int wait = 0; wait < SEARCHTIMER_WAIT && m_Active; wait++)
      sleepSec(1);

   time_t nextUpdate = time(NULL);
   while (m_Active) 
   {
      time_t now = time(NULL);
      bool needUpdate = NeedUpdate();
      if (now >= nextUpdate || needUpdate)
      {
         if (Timers.BeingEdited()) 		
         {
            sleepSec(1);
            continue;
         }
         LogFile.iSysLog("search timer update started");

         UserVars.ResetCache(); // reset internal cache of user vars
         cTimerObjList* pOutdatedTimers = NULL;

         // for thread safeness we work with a copy of the current searches,
         // because SVDRP would not work if the main thread would be locked 
         cSearchExts* localSearchExts = SearchExts.Clone(); 
         cSearchExt *searchExt = localSearchExts->First();
         // reset announcelist
         announceList.Clear();
         while (searchExt && m_Active) 
         {
	   if (!searchExt->IsActiveAt(now))
            {
               searchExt = localSearchExts->Next(searchExt);
               continue;
            }
            pOutdatedTimers = searchExt->GetTimerList(pOutdatedTimers);

            cSearchResults* pSearchResults = searchExt->Run(-1, true);
            if (!pSearchResults)
            {
               searchExt = localSearchExts->Next(searchExt);
               continue;
            }
            pSearchResults->SortBy(CompareEventTime);

            if (searchExt->pauseOnNrRecordings > 0)
               searchExt->CheckExistingRecordings(pSearchResults);

            for (cSearchResult* pResultObj = pSearchResults->First(); 
                 pResultObj; 
                 pResultObj = pSearchResults->Next(pResultObj))
            {
               const cEvent* pEvent = pResultObj->event;
               if (!pEvent)
                  continue;
			    
               cChannel *channel = Channels.GetByChannelID(pEvent->ChannelID(), true, true);
               if (!channel)
                  continue;
			
               int index = 0;
               cTimer *timer = new cTimer(pEvent);

               // create the file 
               char* file = NULL;
               if ((file = searchExt->BuildFile(pEvent)) != NULL)
               {
                  while(strstr(file, "!^pipe^!")) file = strreplace(file, "!^pipe^!", "|"); // revert the translation of '|' in BuildFile 
                  timer->SetFile(file);
                  free(file);
               }
               int Priority = searchExt->Priority;
               int Lifetime = searchExt->Lifetime;
			
               // search for an already existing timer
               bool bTimesMatchExactly = false;
               cTimer *t = GetTimer(searchExt, pEvent, bTimesMatchExactly);

               char* Summary = NULL;
	       uint timerMod = tmNoChange;

               if (t) 
               { // already exists
                  pOutdatedTimers->DelTimer(t);

                  if (!t->HasFlags(tfActive))
                  { // do not update inactive timers
                     LogFile.Log(2,"timer for '%s~%s' (%s - %s, channel %d) not active - won't be touched", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                     delete timer;
                     continue;
                  }
			    
                  int triggerID = TriggeredFromSearchTimerID(t);
                  if (!pResultObj->needsTimer && !t->Recording()) // not needed
                  {
                     if (triggerID == searchExt->ID)
                     {
                        LogFile.Log(1,"delete timer for '%s~%s' (%s - %s, channel %d)", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                        RemoveTimer(t, pEvent);
                     }
                     else if (triggerID == -1) //manual timer 
                     {
                        LogFile.Log(2,"keep obsolete timer for '%s~%s' (%s - %s, channel %d) - was manually created", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                     }
                     delete timer;
                     continue;
                  }
                  if (TimerWasModified(t)) // don't touch timer modified by user 
                  {
                     LogFile.Log(2,"timer for '%s~%s' (%s - %s, channel %d) modified by user - won't be touched", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                     delete timer;
                     continue;
                  }
                  if (triggerID > -1 && triggerID != searchExt->ID)
                  {
                     LogFile.Log(2,"timer for '%s~%s' (%s - %s, channel %d) already created by search id %d - won't be touched", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent), triggerID);
                     delete timer;
                     continue;
                  }

                  char* pFile = NULL; // File is prepared for svdrp, so prepare t->File for comparision too
                  msprintf(&pFile, "%s", t->File());
                  pFile = strreplace(pFile, ':', '|');
                  pFile = strreplace(pFile, " ~", "~");
                  pFile = strreplace(pFile, "~ ", "~");
			    
                  Summary =  SummaryExtended(searchExt, t, pEvent);
			    
                  if (bTimesMatchExactly && strcmp(pFile, timer->File()) == 0
                      && (t->Aux() != NULL && strcmp(t->Aux(), Summary) == 0)
                     )
                  { // dir, title, episode name and summary have not changed
                     if (Summary) free(Summary);
                     delete timer;
                     free(pFile);
                     continue;
                  }
                  else
                  {
		    if (!bTimesMatchExactly) timerMod = (uint)timerMod | tmStartStop;
		    if (strcmp(pFile, timer->File()) != 0) timerMod |= tmFile;
		    if (t->Aux() != NULL && strcmp(t->Aux(), Summary) != 0)
		      {
			char* oldEventID = GetAuxValue(t, "eventid");
			char* newEventID = GetAuxValue(Summary, "eventid");
			if (oldEventID && newEventID && strcmp(oldEventID, newEventID) != 0)
			  timerMod |= tmAuxEventID;
			free(oldEventID);
			free(newEventID);
		      }

		    if (LogFile.Level() >= 3) // output reasons for a timer modification 
		      { 
                        if (timerMod & tmStartStop)
			  LogFile.Log(3,"timer for '%s~%s' (%s - %s, channel %d) : start/stop has changed", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                        if (timerMod & tmFile)
			  LogFile.Log(3,"timer for '%s~%s' (%s - %s, channel %d) : title and/or episdode has changed", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                        if (timerMod & tmAuxEventID)
			  LogFile.Log(3,"timer for '%s~%s' (%s - %s, channel %d) : aux info for event id has changed", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
		      }
		    index = t->Index()+1;
		    Priority = t->Priority();
		    Lifetime = t->Lifetime();
                  }
                  free(pFile);

                  if (t->Recording() && t->StopTime() == timer->StopTime()) 
                  {
                     // only update recording timers if stop time has changed, since all other settings can't be modified
                     LogFile.Log(2,"timer for '%s~%s' (%s - %s, channel %d) already recording - no changes possible", pEvent->Title()?pEvent->Title():"no title", pEvent->ShortText()?pEvent->ShortText():"no subtitle", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), ChannelNrFromEvent(pEvent));
                     delete timer;
                     continue;
                  }
               }			       
               else
               {
                  if (!pResultObj->needsTimer)
                  {
                     delete timer;
                     continue;
                  }
               }
		
               if (searchExt->action == searchTimerActionAnnounceOnly) 
               {
                  if (t || // timer already exists or
                      NoAnnounces.InList(pEvent) || // announcement not wanted anymore or
                      (EPGSearchConfig.noAnnounceWhileReplay && cDevice::PrimaryDevice()->Replaying()) // no announce while replay
                     ) 
                  {
                     if (Summary) free(Summary);
                     delete timer;
                     continue;
                  }
                  announceList.Add(new cSearchResult(pEvent, searchExt));

                  if (Summary) free(Summary);
                  delete timer;
                  continue;
               }

               if (searchExt->action == searchTimerActionSwitchOnly) // add to switch list
               {
                  time_t now = time(NULL);
                  if (now < pEvent->StartTime())
                  {
                     if (!SwitchTimers.InSwitchList(pEvent))
                     {			
                        cMutexLock SwitchTimersLock(&SwitchTimers);	
                        SwitchTimers.Add(new cSwitchTimer(pEvent, searchExt->switchMinsBefore, 0, 
							  searchExt->unmuteSoundOnSwitch));
                        SwitchTimers.Save();
                        cSwitchTimerThread::Init(); 
                     }
                  }
                  if (Summary) free(Summary);
                  delete timer;
                  continue;
               }
			
               if (AddModTimer(timer, index, searchExt, pEvent, Priority, Lifetime, Summary, timerMod))
               {
                  if (index == 0)
                     LogFile.Log(1,"added timer for '%s~%s' (%s - %s); search timer: '%s'", pEvent->Title(), pEvent->ShortText()?pEvent->ShortText():"", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);
                  else
                     LogFile.Log(1,"modified timer %d for '%s~%s' (%s - %s); search timer: '%s'", index, pEvent->Title(), pEvent->ShortText()?pEvent->ShortText():"", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);			    
               }
               if (Summary) free(Summary);
               delete timer;
            }
            searchExt = localSearchExts->Next(searchExt);
         }
         if (localSearchExts) delete localSearchExts;

         if (pOutdatedTimers)
         {
            if (pOutdatedTimers->Count() > 0)
            {
               LogFile.Log(1,"removing outdated timers");
               for(cTimerObj *tObj = pOutdatedTimers->First(); tObj; tObj = pOutdatedTimers->Next(tObj))
               {
                  cTimer* t = tObj->timer;
                  // timer could have been deleted meanwhile, so check if its still there
                  bool found = false;
                  for(cTimer* checkT = Timers.First(); checkT; checkT = Timers.Next(checkT)) 
                     if (checkT == t)
                     {
                        found = true;
                        break;
                     }
                  if (!found) continue;

                  if (TimerWasModified(t)) continue;
                  if (!t->Event()) continue; // if there is no event, we keep the timer, since EPG could have been cleared
                  if (time(NULL) > t->StopTime()) continue; // if this timer has (just) finished, let VDR do the cleanup
                  LogFile.Log(1,"delete timer for '%s' (%s, channel %s)", t->File(), DAYDATETIME(t->StartTime()), CHANNELNAME(t->Channel()));
                  RemoveTimer(t, t->Event());
               }
               LogFile.Log(1,"removing outdated timers - done");
            }
            delete pOutdatedTimers;
         }

         TimersDone.ClearOutdated();
         TimersDone.Save();

         if (announceList.Count() > 0)
         {
	   cString msgfmt = cString::sprintf(tr("%d new broadcast(s) found! Show them?"), announceList.Count());
	   if (SendMsg(msgfmt, true,7) == kOk)		   
	     {
	       m_plugin->showAnnounces = true;
	       cRemote::CallPlugin("epgsearch");	
	     }
         }

         LogFile.iSysLog("search timer update finished");

         // check for conflicts
         if (EPGSearchConfig.checkTimerConflictsAfterUpdate)
         {
            LogFile.iSysLog("check for timer conflicts");
            cConflictCheck conflictCheck;   
            conflictCheck.Check(); 

            if (conflictCheck.relevantConflicts > 0)
            {
               if (EPGSearchConfig.sendMailOnConflicts)
               {
                  cMailConflictNotifier mailNotifier;
                  mailNotifier.SendConflictNotifications(conflictCheck);
               }

               cString msgfmt = cString::sprintf(tr("%d timer conflict(s)! First at %s. Show them?"), 
						 conflictCheck.relevantConflicts,
						 *DateTime(conflictCheck.nextRelevantConflictDate));
               bool doMessage = EPGSearchConfig.noConflMsgWhileReplay == 0 || 
                  !cDevice::PrimaryDevice()->Replaying() || 
                  conflictCheck.nextRelevantConflictDate - now < 2*60*60;
               if (doMessage && SendMsg(msgfmt, true,7) == kOk)			
               {
                  m_plugin->showConflicts = true;
                  cRemote::CallPlugin("epgsearch");	
               }
            }
		
            LogFile.iSysLog("check for timer conflicts - done");	    
         }

         // delete expired recordings
         CheckExpiredRecs();

         // check for updates for manual timers
         CheckManualTimers();

         mailNotifier.SendUpdateNotifications();

         if (updateForced == 2)
            SendMsg(tr("Search timer update done!"));

         // reset service call flag
         updateForced = 0;

         m_lastUpdate = time(NULL);
         nextUpdate = long(m_lastUpdate/60)*60 + (EPGSearchConfig.UpdateIntervall * 60);
      }
      sleepSec(2); // to avoid high system load if time%30==0 
      while (m_Active && !NeedUpdate() && time(NULL)%30 != 0) // sync heart beat to a multiple of 5secs
         sleepSec(1);
   };
   LogFile.iSysLog("Leaving search timer thread");
}

bool cSearchTimerThread::NeedUpdate()
{
   return  (m_lastUpdate <= LastModifiedTime(AddDirectory(CONFIGDIR, ".epgsearchupdate")) || updateForced>0);
}

char* cSearchTimerThread::SummaryExtended(cSearchExt* searchExt, cTimer* Timer, const cEvent* pEvent)
{
   bool UseVPS = searchExt->useVPS && pEvent->Vps() && Setup.UseVps;
   time_t eStart;
   if (!UseVPS)
      eStart = pEvent->StartTime();
   else
      eStart = pEvent->Vps();
   time_t eStop;
   if (!UseVPS)
      eStop = pEvent->EndTime();
   else
      eStop = pEvent->Vps() + pEvent->Duration();
   time_t start = eStart - (UseVPS?0:(searchExt->MarginStart * 60));
   time_t stop  = eStop + (UseVPS?0:(searchExt->MarginStop * 60));

   char* addSummaryFooter = NULL;
   msprintf(&addSummaryFooter, "<channel>%d - %s</channel><searchtimer>%s</searchtimer><start>%ld</start><stop>%ld</stop><s-id>%d</s-id><eventid>%ld</eventid>", 
            Timer->Channel()->Number(), CHANNELNAME(Timer->Channel()),
            searchExt->search, 
            start, 
            stop, 
            searchExt->ID,
            (long) pEvent->EventID());

   const char* aux = Timer->Aux();
   // remove epgsearch entries
   char* tmpaux = NULL;
   if (!isempty(aux))
   {
      tmpaux = strdup(aux);
      char* begin = strstr(aux, "<epgsearch>");
      char* end = strstr(aux, "</epgsearch>");
      if (begin && end)
      {
         if (begin == aux) strcpy(tmpaux, ""); else strn0cpy(tmpaux, aux, begin-aux+1);
         strcat(tmpaux, end + strlen("</epgsearch>"));
      }
   }

   char* tmpSummary = NULL;
   msprintf(&tmpSummary, "<epgsearch>%s</epgsearch>%s", addSummaryFooter, tmpaux?tmpaux:"");
   free(addSummaryFooter);
   if (tmpaux) free(tmpaux);
   return tmpSummary;
}

bool cSearchTimerThread::AddModTimer(cTimer* Timer, int index, cSearchExt* searchExt, const cEvent* pEvent, int Prio, int Lifetime, char* Summary, uint timerMod) 
{
   char *cmdbuf = NULL;

   static char bufStart[25];
   static char bufEnd[25];

   struct tm tm_r;    time_t eStart = pEvent->StartTime();
   time_t eStop = pEvent->EndTime();
   time_t start = eStart - (searchExt->MarginStart * 60);
   time_t stop  = eStop + (searchExt->MarginStop * 60);
   int Flags = Timer->Flags();
   if (searchExt->useVPS && pEvent->Vps() && Setup.UseVps)
   {
      start = pEvent->Vps();
      stop = start + pEvent->Duration();
   }
   else
      Flags = 1; // don't use VPS, if not set in this search

   // already done the same timer?
   if (!EPGSearchConfig.TimerProgRepeat && index == 0 && TimersDone.InList(start, stop, pEvent, -1))
   {
      LogFile.Log(2,"skip timer for '%s~%s' (%s - %s); search timer: '%s' - already done", pEvent->Title(), pEvent->ShortText()?pEvent->ShortText():"", GETDATESTRING(pEvent), GETTIMESTRING(pEvent), searchExt->search);
      return false;
   }

   strftime(bufStart, sizeof(bufStart), "%H%M", localtime_r(&start, &tm_r));
   strftime(bufEnd, sizeof(bufEnd), "%H%M", localtime_r(&stop, &tm_r));

   // add additonal info
   char* tmpSummary = NULL;
   if (Summary)
   {
      tmpSummary = strdup(Summary);
      strreplace(tmpSummary, '\n', '|');
   }
   else 
      tmpSummary = SummaryExtended(searchExt, Timer, pEvent);

   if (index==0)
      msprintf(&cmdbuf, "NEWT %d:%d:%s:%s:%s:%d:%d:%s:%s", 
               Flags,
               Timer->Channel()->Number(),
#if VDRVERSNUM < 10503
               *Timer->PrintDay(start, Timer->WeekDays()),
#else
               *Timer->PrintDay(start, Timer->WeekDays(), true),
#endif
               bufStart,
               bufEnd,
               Prio, 
               Lifetime, 
               Timer->File(),
               tmpSummary?tmpSummary:"");
   else
      msprintf(&cmdbuf, "MODT %d %d:%d:%s:%s:%s:%d:%d:%s:%s", 
               index,
               Flags,
               Timer->Channel()->Number(),
#if VDRVERSNUM < 10503
               *Timer->PrintDay(start, Timer->WeekDays()),
#else
               *Timer->PrintDay(start, Timer->WeekDays(), true),
#endif
               bufStart,
               bufEnd,
               Prio, 
               Lifetime, 
               Timer->File(),
               tmpSummary?tmpSummary:"");

   SendViaSVDRP(cmdbuf);
   if (gl_timerStatusMonitor) gl_timerStatusMonitor->SetConflictCheckAdvised(); 

   cTimerDone* timerdone = new cTimerDone(start, stop, pEvent, searchExt->ID);
   if (index==0)
      TimersDone.Add(timerdone);
   else
      TimersDone.Update(start, stop, pEvent, searchExt->ID, timerdone);

   if (EPGSearchConfig.sendMailOnSearchtimers)
   {
      if (index==0) // new
         mailNotifier.AddNewTimerNotification(pEvent->EventID(), pEvent->ChannelID()); 
      else
	mailNotifier.AddModTimerNotification(pEvent->EventID(), pEvent->ChannelID(), timerMod); 
   }
   free(cmdbuf);
   if (tmpSummary) free(tmpSummary);

   return true;
}

void cSearchTimerThread::RemoveTimer(cTimer* t, const cEvent* e)
{
   if (!t) return;
   if (EPGSearchConfig.sendMailOnSearchtimers)
      mailNotifier.AddRemoveTimerNotification(t,e); 
   if (!EPGSearchConfig.TimerProgRepeat)
   {
      cTimerDone * TimerDone = TimersDone.InList(t->StartTime(), t->StopTime(), e, -1);
      if (TimerDone)
      {
         cMutexLock TimersDoneLock(&TimersDone);
         TimersDone.Del(TimerDone);
         TimersDone.Save();
      }
   }
   DelTimer(t->Index()+1);
}

void cSearchTimerThread::DelRecording(int index)
{
  cString cmdbuf = cString::sprintf("DELR %d", index);    
  LogFile.Log(2, "delete recording %d", index);
  SendViaSVDRP(cmdbuf);
}

void cSearchTimerThread::CheckExpiredRecs()
{
   LogFile.Log(1, "check for expired recordings started");
   cThreadLock RecordingsLock(&Recordings);
   cList<cRecordingObj> DelRecordings;
   for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) 
   {
      LogFile.Log(3, "check recording %s from %s for expiration", recording->Name(), DAYDATETIME(recording->start));
      if (recording->start == 0)
      {
         LogFile.Log(2, "oops, recording %s has no start time, skipped", recording->Name());
         continue;
      }
      if (recording->IsEdited()) continue;

      if (!recording->Info()) continue;
      char* searchID = GetAuxValue(recording, "s-id");	
      char* searchName = GetAuxValue(recording, "searchtimer");	
      if (!searchName)
         searchName = GetAuxValue(recording, "search timer");	

      if (!searchID || !searchName)
      {
         if (searchID) free(searchID);
         if (searchName) free(searchName);
         continue;
      }
      cSearchExt* search = SearchExts.GetSearchFromID(atoi(searchID));
      if (!search || strcmp(search->search, searchName) != 0) 
      {
         if (searchID) free(searchID);
         if (searchName) free(searchName);
         continue;
      }
      LogFile.Log(3, "recording triggered from search timer %s", search->search);
      if (search->delAfterDays == 0) continue;
      time_t now = time(NULL);
      int daysBetween = int(double((now - recording->start)) / (60*60*24));
      if (daysBetween  >= search->delAfterDays)
         DelRecordings.Add(new cRecordingObj(recording, search));
      else
         LogFile.Log(3, "recording will expire in %d days, search timer %s", search->delAfterDays - daysBetween, search->search);
   }    
   for (cRecordingObj *recordingObj = DelRecordings.First(); recordingObj; recordingObj = DelRecordings.Next(recordingObj)) 
   {
      cRecording* recording = recordingObj->recording;
      cSearchExt* search = recordingObj->search;
      if (search->recordingsKeep > 0 && search->recordingsKeep >= search->GetCountRecordings())
      {
         LogFile.Log(1, "recording '%s' from %s expired, but will be kept, search timer %s", recording->Name(), DAYDATETIME(recording->start), recordingObj->search->search);
         continue;
      }
      LogFile.Log(1, "delete expired recording '%s' from %s, search timer %s", recording->Name(), DAYDATETIME(recording->start), recordingObj->search->search);
      cRecordControl *rc = cRecordControls::GetRecordControl(recording->FileName());
      if (!rc) 
      {
         if (!recording->Delete())
            LogFile.Log(1, "error deleting recording!");
         else
            ::Recordings.DelByName(recording->FileName());
      }
      else
         LogFile.Log(1, "recording already in use by a timer!");
   }
   LogFile.Log(1, "check for expired recordings finished");
}

void cSearchTimerThread::ModifyManualTimer(const cEvent* event, const cTimer* timer, int bstart, int bstop)
{
   LogFile.Log(1,"modified manual timer %d for '%s~%s' (%s - %s)", timer->Index()+1, event->Title(), event->ShortText()?event->ShortText():"", GETDATESTRING(event), GETTIMESTRING(event));

   time_t start = event->StartTime() - bstart;
   time_t stop = event->EndTime() + bstop;
   struct tm tm_r_start;
   struct tm tm_r_stop;
   localtime_r(&start, &tm_r_start);
   localtime_r(&stop, &tm_r_stop);
   char daybuffer[DAYBUFFERSIZE];
   char startbuffer[DAYBUFFERSIZE];
   char stopbuffer[DAYBUFFERSIZE];
   strftime(daybuffer, DAYBUFFERSIZE, "%Y-%m-%d", &tm_r_start);
   strftime(startbuffer, DAYBUFFERSIZE, "%H%M", &tm_r_start);
   strftime(stopbuffer, DAYBUFFERSIZE, "%H%M", &tm_r_stop);
    
   char* cmdbuf = NULL;
   msprintf(&cmdbuf, "MODT %d %d:%d:%s:%s:%s:%d:%d:%s:%s", 
            timer->Index()+1,
            timer->Flags(),
            timer->Channel()->Number(),
            daybuffer,
            startbuffer,
            stopbuffer,
            timer->Priority(), 
            timer->Lifetime(), 
            timer->File(),
            timer->Aux());
    
   if (EPGSearchConfig.sendMailOnSearchtimers)
      mailNotifier.AddModTimerNotification(event->EventID(), event->ChannelID()); 

   cTimerThread timerThread;
   timerThread.Init(cmdbuf);			   
   free(cmdbuf);
}

void cSearchTimerThread::CheckManualTimers()
{
   LogFile.Log(1, "manual timer check started");

   cSchedulesLock schedulesLock;
   const cSchedules *schedules;
   schedules = cSchedules::Schedules(schedulesLock);

   for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti)) 
   {
      if (TriggeredFromSearchTimerID(ti) != -1) continue; // manual timer?
        
      char* szbstart = GetAuxValue(ti, "bstart");
      int bstart = szbstart? atoi(szbstart) : 0;
      free(szbstart);
      char* szbstop = GetAuxValue(ti, "bstop");
      int bstop = szbstop? atoi(szbstop) : 0;
      free(szbstop);

      // how to check? 
      char* updateMethod = GetAuxValue(ti, "update");
      if (updateMethod && atoi(updateMethod) == UPD_EVENTID) // by event ID?
      {
         // get the channels schedule
         const cSchedule* schedule = schedules->GetSchedule(ti->Channel());
         if (schedule)
         {
            tEventID eventID = 0;
            char* szEventID = GetAuxValue(ti, "eventid");
            if (szEventID)
               eventID = atol(szEventID); 
            LogFile.Log(3,"checking manual timer %d by event ID %u", ti->Index()+1, eventID);
            const cEvent* event = schedule->GetEvent(eventID);
            if (event)
            {
               if (event->StartTime() - bstart != ti->StartTime() || event->EndTime() + bstop != ti->StopTime())
                  ModifyManualTimer(event, ti, bstart, bstop);
            }
            else
               LogFile.Log(1,"ooops - no event found with id %u for manual timer %d", eventID, ti->Index()+1);

            if (szEventID) free(szEventID);
         }
      }
      if (updateMethod && atoi(updateMethod) == UPD_CHDUR) // by channel and time?
      {
         // get the channels schedule
         const cSchedule* schedule = schedules->GetSchedule(ti->Channel());
         if (schedule)
         {
            // collect all events touching the old timer margins
            cSearchResults eventlist;
            for (cEvent *testevent = schedule->Events()->First(); testevent; testevent = schedule->Events()->Next(testevent))
            {
               if (testevent->StartTime() < ti->StopTime() && testevent->EndTime() > ti->StartTime())
                  eventlist.Add(new cSearchResult(testevent, (const cSearchExt*)NULL));
            }
            LogFile.Log(3,"checking manual timer %d by channel and time, found %d candidates", ti->Index()+1, eventlist.Count());
            if (eventlist.Count() > 0)
            {
               // choose the event with the best match by duration
               long origlen = (ti->StopTime() - bstop) - (ti->StartTime() + bstart); 
               double maxweight = 0;
               const cEvent* event = eventlist.First()->event;
               for (cSearchResult* pResultObj = eventlist.First();  pResultObj; pResultObj = eventlist.Next(pResultObj))
               {
                  const cEvent* testevent = pResultObj->event;
                  time_t start = (testevent->StartTime() < ti->StartTime()) ? ti->StartTime() : testevent->StartTime(); 
                  time_t stop =  (testevent->EndTime() > ti->StopTime()) ? ti->StopTime() : testevent->EndTime();
                  double weight = double(stop - start) / double(testevent->EndTime() - testevent->StartTime());
                  LogFile.Log(3,"candidate '%s~%s' (%s - %s) timer match: %f, duration match: %f", testevent->Title(), testevent->ShortText()?testevent->ShortText():"", GETDATESTRING(testevent), GETTIMESTRING(testevent), weight, (double(testevent->EndTime() - testevent->StartTime()) / origlen));
                  if (weight > maxweight && (double(testevent->EndTime() - testevent->StartTime()) / origlen) >= 0.9)
                  {
                     maxweight = weight;
                     event = testevent;
                  }
               }
               LogFile.Log(3,"selected candidate is '%s~%s' (%s - %s)", event->Title(), event->ShortText()?event->ShortText():"", GETDATESTRING(event), GETTIMESTRING(event));
               if ((maxweight > 0 && event->StartTime() - bstart != ti->StartTime()) || (event->EndTime() + bstop != ti->StopTime()))
                  ModifyManualTimer(event, ti, bstart, bstop);		    
	       else if (maxweight == 0)
		 LogFile.Log(3,"selected candidate is too bad");
            }
            else
               LogFile.Log(1,"ooops - no events found touching manual timer %d", ti->Index()+1);
         }
         if (updateMethod) free(updateMethod);
      }    
   }
   LogFile.Log(1, "manual timer check finished");
}


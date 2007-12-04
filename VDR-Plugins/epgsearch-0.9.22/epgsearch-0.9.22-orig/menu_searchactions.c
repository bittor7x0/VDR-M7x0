/*
 * menu_searchactions.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vector>
#include "menu_commands.h"
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include "menu_searchactions.h"
#include "menu_searchresults.h"
#include "menu_recsdone.h"
#include "menu_timersdone.h"
#include "menu_switchtimers.h"
#include "menu_blacklists.h"
#include "epgsearchtools.h"
#include "menu_conflictcheck.h"
#include "epgsearchcfg.h"
#include "searchtimer_thread.h"

#define ACTION_COUNTER 10
extern int updateForced;
// --- cMenuSearchActions ---------------------------------------------------------

cMenuSearchActions::cMenuSearchActions(cSearchExt* Search, bool DirectCall)
:cOsdMenu(tr("Search actions"))
{
    directCall = DirectCall;
    SetHasHotkeys();
    
    search = Search;

    Add(new cOsdItem(hk(tr("Execute search"))));
    Add(new cOsdItem(hk(tr("Use as search timer on/off"))));
    Add(new cOsdItem(hk(tr("Trigger search timer update"))));
    Add(new cOsdItem(hk(tr("Show recordings done"))));
    Add(new cOsdItem(hk(tr("Show timers created"))));
    Add(new cOsdItem(hk(tr("Use as template"))));
    Add(new cOsdItem(hk(tr("Show switch list"))));
    Add(new cOsdItem(hk(tr("Show blacklists"))));
    Add(new cOsdItem(hk(tr("Delete created timers?"))));
    Add(new cOsdItem(hk(tr("Timer conflict check"))));
}

cMenuSearchActions::~cMenuSearchActions()
{
}


eOSState cMenuSearchActions::Search(void)
{
    cMenuTemplate* MenuTemplate = NULL;
    if (search && search->menuTemplate > 0)
	MenuTemplate = cTemplFile::GetSearchTemplateByPos(search->menuTemplate);
    if (!MenuTemplate)
	MenuTemplate = cTemplFile::GetTemplateByName("MenuSearchResults");
    return AddSubMenu(new cMenuSearchResultsForSearch(search, MenuTemplate));
}

eOSState cMenuSearchActions::OnOffSearchtimer(void)
{
   if (search) 
   {
      search->useAsSearchTimer = search->useAsSearchTimer?0:1;
	  SearchExts.Save();
	  if (!search->useAsSearchTimer && Interface->Confirm(tr("Disable associated timers too?")))
         search->OnOffTimers(false);
      if (search->useAsSearchTimer && Interface->Confirm(tr("Activate associated timers too?")))
      {
         search->OnOffTimers(true);
         if (!EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
         {
            cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);
            Skins.Message(mtInfo, tr("Search timers activated in setup."));			
         }
      }      
  }
  return osBack;
}


eOSState cMenuSearchActions::Execute()
{
   int current = Current();

   if (current <= ACTION_COUNTER-1)
   {
      if (current == 0)
         return Search();
      if (current == 1)
         return OnOffSearchtimer(); 
      if (current == 2)
      {
         if (!EPGSearchConfig.useSearchTimers) // enable search timer thread if necessary
         {
            cSearchTimerThread::Init((cPluginEpgsearch*) cPluginManager::GetPlugin("epgsearch"), true);
            Skins.Message(mtInfo, tr("Search timers activated in setup."));			
         }
         if (Interface->Confirm(tr("Run search timer update?")))
            updateForced = 2; // with message about completion
         return osBack;
      }	
      if (current == 3)
         return AddSubMenu(new cMenuRecsDone(search));
      if (current == 4)
         return AddSubMenu(new cMenuTimersDone(search));
      if (current == 5)
      {
         if (!Interface->Confirm(tr("Copy this entry to templates?")))
            return osBack;
         cSearchExt* templateObj = new cSearchExt;
         templateObj->CopyFromTemplate(search);
         strcpy(templateObj->search, search->search);
         cMutexLock SearchTemplatesLock(&SearchTemplates);
         templateObj->ID = SearchTemplates.GetNewID();
         SearchTemplates.Add(templateObj);
         SearchTemplates.Save();
         return osBack;
      }
      if (current == 6)
         return AddSubMenu(new cMenuSwitchTimers());
      if (current == 7)
         return AddSubMenu(new cMenuBlacklists());
      if (current == 8)
      {
         if (!Interface->Confirm(tr("Delete all timers created from this search?")))
            return osBack;
	 search->DeleteAllTimers();
	 return osBack;
     }
     if (current == 9)	 
	 return AddSubMenu(new cMenuConflictCheck());
  }
  return osContinue;
}

eOSState cMenuSearchActions::ProcessKey(eKeys Key)
{
   bool hadSubmenu = HasSubMenu();
   if (directCall && Key == k1 && !HasSubMenu())
      return Search();

   eOSState state = cOsdMenu::ProcessKey(Key);

   // jump back to calling menu, if a command was called directly with key '1' .. '9'
   if (directCall && hadSubmenu && !HasSubMenu())
      return osBack;

   if (state == osUnknown) {
      switch (Key) {
         case kGreen:
         case kYellow:
         case kBlue:
            return osContinue;
         case kOk:  
            if (!HasSubMenu())
               return Execute();
         default:   break;
      }
   }
   return state;
}

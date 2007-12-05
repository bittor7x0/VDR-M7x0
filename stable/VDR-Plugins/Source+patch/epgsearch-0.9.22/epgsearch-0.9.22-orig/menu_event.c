/*
 * menu_event.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vector>
#include <vdr/status.h>
#include "menu_event.h"
#include "menu_commands.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"

// --- cMenuEventSearch ------------------------------------------------------------
cMenuEventSearch::cMenuEventSearch(const cEvent* Event, cEventObjects& EventObjects,  MenuEventSurfMode SurfMode)
   :cOsdMenu(tr("Event")),
   eventObjects(EventObjects),
   surfMode(SurfMode)
{
   
   eventObjects.SetCurrent(Event);
   szGreen = szYellow = NULL;
   Set();
}

cEventObj* cMenuEventSearch::GetPrev(const cEvent* Event)
{
   cEventObj* prevEventObj = NULL;
   cEventObjects::iterator i;
   for(i = eventObjects.begin(); i != eventObjects.end(); ++i)
   {
      if (*i && (*i)->Event() == Event)
         return prevEventObj;
      else
         prevEventObj = *i;
   }
   return NULL;
}

cEventObj* cMenuEventSearch::GetNext(const cEvent* Event)
{
   cEventObjects::iterator i;
   for(i = eventObjects.begin(); i != eventObjects.end(); ++i)
      if (*i && (*i)->Event() == Event)
      {
         cEventObjects::iterator nexti = i;
         nexti++;
         return nexti != eventObjects.end() ? *nexti : NULL; 
      }

   return NULL;
}

void cMenuEventSearch::Set()
{
   cEventObj* eventObj = eventObjects.GetCurrent();
   if (!eventObj) return;
   event = eventObj->Event();
   if (!event) return;

   if (szGreen) free(szGreen);
   if (szYellow) free(szYellow);
   szGreen = szYellow = NULL;

   if (event) 
   {
      cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true, true);
      bool canSwitch = false;
      if (channel) 
      {
         SetTitle(channel->Name());
         canSwitch = channel->Number() != cDevice::CurrentChannel();
      }

      cEventObj* eventObjPrev = GetPrev(event);
      cEventObj* eventObjNext = GetNext(event);

      if (surfMode == SurfModeUnknown)
         SetHelp(tr("Button$Record"), eventObjPrev ? "<<":NULL, eventObjNext ? ">>":NULL, canSwitch ? tr("Button$Switch") : NULL);
      else if (surfMode == SurfModeTime)
      {
         if (eventObjPrev && eventObjPrev->Event()) szGreen = strdup(GETTIMESTRING(eventObjPrev->Event()));
         if (eventObjNext && eventObjNext->Event()) szYellow = strdup(GETTIMESTRING(eventObjNext->Event()));
         SetHelp(tr("Button$Record"), szGreen, szYellow, canSwitch ? tr("Button$Switch") : NULL);
      }
      else if (surfMode == SurfModeChannel)
      {
         if (eventObjPrev && eventObjPrev->Event()) 
            szGreen = strdup(CHANNELNAME(Channels.GetByChannelID(eventObjPrev->Event()->ChannelID(), true, true)));
         if (eventObjNext && eventObjNext->Event()) 
            szYellow = strdup(CHANNELNAME(Channels.GetByChannelID(eventObjNext->Event()->ChannelID(), true, true)));
         SetHelp(tr("Button$Record"), szGreen, szYellow, canSwitch ? tr("Button$Switch") : NULL);
      }
   }
}

cMenuEventSearch::~cMenuEventSearch()
{
   if (szGreen) free(szGreen);
   if (szYellow) free(szYellow);
}

void cMenuEventSearch::Display(void)
{
  cOsdMenu::Display();
  if (event)
  {
      DisplayMenu()->SetEvent(event);
      cStatus::MsgOsdTextItem(event->Description());
  }
}

eOSState cMenuEventSearch::Commands(eKeys Key)
{
    if (HasSubMenu())
	return osContinue;
    if (event) {
        cMenuSearchCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuSearchCommands(tr("EPG Commands"), event, true));
        if (Key != kNone)
	    state = menu->ProcessKey(Key);
        return state;
    }
    return osContinue;
}

eOSState cMenuEventSearch::ProcessKey(eKeys Key)
{
   if (!HasSubMenu())
   {
      switch (Key) {
         case kUp|k_Repeat:
         case kUp:
         case kDown|k_Repeat:
         case kDown:
         case kLeft|k_Repeat:
         case kLeft:
         case kRight|k_Repeat:
         case kRight:
            DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
            cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp);
            return osContinue;
         case k1...k9: 
            if (!HasSubMenu())
               return Commands(Key);
            else
               return osContinue;
            break;
         default: break;
      }
   }

   eOSState state = cOsdMenu::ProcessKey(Key);
  
   if (state == osUnknown) {
      switch (Key) {
         case kOk:     return osBack;
         case kGreen:
         case kFastRew:
         {
            cEventObj* eventObjPrev = GetPrev(event);
            if (eventObjPrev && eventObjPrev->Event())
            {
               eventObjects.SetCurrent(eventObjPrev->Event());
               Set();
               Display();
            }
            state = osContinue;
         }
         break;
         case kFastFwd:
         case kYellow:
         {
            cEventObj* eventObjNext = GetNext(event);
            if (eventObjNext && eventObjNext->Event())
            {
               eventObjects.SetCurrent(eventObjNext->Event());
               Set();
               Display();
            }
            state = osContinue;
         }
         break;
         default: break;
      }
   }
   return state;
}


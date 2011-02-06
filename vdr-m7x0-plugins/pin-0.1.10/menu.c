/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/interface.h>

#include "menu.h"
#include "pin.h"

//***************************************************************************
// Pin Menu
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cPinMenu::cPinMenu(const char* title, cLockItems* theChannels,
                   cLockedBroadcasts* theBroadcasts, cLockItems* thePlugins,
                   cLockItems* theMenuItems)
   : cOsdMenu(title)
{
   cLockItem* setupItem;
   int setupLocked = no;

   lockedChannels = theChannels;
   lockedBroadcasts = theBroadcasts;
   lockedPlugins = thePlugins;
   lockedMenuItems = theMenuItems;

   if ((setupItem = lockedMenuItems->FindByName(tr("Setup"))))
      setupLocked = setupItem->GetActive();

   if (!setupLocked)
      if ((setupItem = lockedPlugins->FindByName("setup")))
         setupLocked = setupItem->GetActive();

   SetHasHotkeys();

   cOsdMenu::Add(new cPinMenuItem(hk(tr("Add current channel to protection list")), miAddCurrentChannel));
   cOsdMenu::Add(new cPinMenuItem(hk(tr("Add current broadcast to protection list")), miAddCurrentBroadcast));
   cOsdMenu::Add(new cPinMenuItem(hk(tr("Edit channel locks")), miEditChannelList));
   cOsdMenu::Add(new cPinMenuItem(hk(tr("Edit broadcast locks")), miEditBroadcastList));
   cOsdMenu::Add(new cPinMenuItem(hk(tr("Edit plugin locks")), miEditPluginList));
   cOsdMenu::Add(new cPinMenuItem(hk(tr("Edit menu locks")), miEditMenuItemList));
   cOsdMenu::Add(new cPinMenuItem(setupLocked ? hk(tr("Unlock setup")) : hk(tr("Lock setup")), miProtectSetup));
   cOsdMenu::Add(new cPinMenuItem(hk(tr("Activate childlock")), miLock));

	Display();
}

cPinMenu::~cPinMenu()
{
   // Aenderungen speichern

   lockedChannels->Save();
   lockedBroadcasts->Save();
   lockedPlugins->Save();
   lockedMenuItems->Save();
}

//***************************************************************************
// Process Key
//***************************************************************************

eOSState cPinMenu::ProcessKey(eKeys key)
{
   eOSState state = cOsdMenu::ProcessKey(key);

   switch (state)
   {
      case miLock:
      {
         if (*cPinPlugin::pinCode)
         {
            cOsd::pinValid = false;
            Skins.Message(mtInfo, tr("Activated pin protection"));
         }
         else
         {
            Skins.Message(mtInfo, tr("Please setup a pin code first!"));
         }

         return osEnd;
      }

      case miAddCurrentChannel:   addCurrentChannel();      break;
      case miAddCurrentBroadcast: addCurrentBroadcast();    break;

      case miEditChannelList:
         state = AddSubMenu(new cLockMenu(tr("Protected channels"),
                                          lockedChannels, ltChannels));
         break;
      case miEditBroadcastList:
         state = AddSubMenu(new cLockMenu(tr("Protected broadcasts"),
                                          (cLockItems*)lockedBroadcasts, ltBroadcasts));
         break;
      case miEditPluginList:
         state = AddSubMenu(new cLockMenu(tr("Plugin protections"),
                                          lockedPlugins, ltPlugins));
         break;
      case miEditMenuItemList:
         state = AddSubMenu(new cLockMenu(tr("Menu protections"),
                                          lockedMenuItems, ltMenuItems));
         break;
      case miProtectSetup:
      {
         char* buf;
         int locked = protectSetup();
         cPinMenuItem* item = (cPinMenuItem*)Get(7-1);
         int hotKey = atoi(item->Text());

         asprintf(&buf, " %2d  %s", hotKey, locked ? tr("Unlock setup") : tr("Lock setup"));
         item->SetText(buf, /*copy*/ true);
         free(buf);

         Display();
         break;
      }

      default: ;
   }

   return state;
}

//***************************************************************************
// Protect Setup
//***************************************************************************

int cPinMenu::protectSetup()
{
   cLockItem* setupItemPlugin;
   cLockItem* setupItemMenu;
   char* buf;
   int locked;

   if (!(setupItemMenu = lockedMenuItems->FindByName(tr("Setup"))))
      lockedMenuItems->Add(setupItemMenu = new cLockItem(tr("Setup")));

   if (!(setupItemPlugin = lockedPlugins->FindByName("setup")))
      lockedPlugins->Add(setupItemPlugin = new cLockItem("setup"));

   locked = !(setupItemPlugin->GetActive() && setupItemMenu->GetActive());

   setupItemPlugin->SetActive(locked);
   setupItemMenu->SetActive(locked);

   asprintf(&buf, "setup %s", locked ? tr("locked") : tr("unlocked"));
   Skins.Message(mtInfo, buf);
   free(buf);

   return locked;
}

//***************************************************************************
// Add Current Channel
//***************************************************************************

int cPinMenu::addCurrentChannel()
{
   char* buf;
   cChannel* channel = Channels.GetByNumber(cDevice::CurrentChannel());

   if (!channel || channel->GroupSep())
      return ignore;

   if (lockedChannels->FindByName(channel->Name()))
   {
      asprintf(&buf, "%s - %s", channel->Name(), tr("already in list"));
      Skins.Message(mtInfo, buf);
      free(buf);

      return ignore;
   }

   asprintf(&buf, "%s - %s", channel->Name(), tr("added to protection list"));
   Skins.Message(mtInfo, buf);
   free(buf);

   // append channel to lock list

   lockedChannels->Add(new cLockItem(channel->Name()));

   return success;
}

//***************************************************************************
// Add Current Broadcast
//***************************************************************************

int cPinMenu::addCurrentBroadcast()
{
   cSchedulesLock schedLock;
   const cSchedules* scheds;
   const cSchedule *sched;
   const cEvent* event;
   char* buf;

   cChannel* channel = Channels.GetByNumber(cDevice::CurrentChannel());

   if (channel && !channel->GroupSep())
   {
      if (!(scheds = cSchedules::Schedules(schedLock)))
         return done;

      if (!(sched = scheds->GetSchedule(channel->GetChannelID())))
         return done;

      if (!(event = sched->GetPresentEvent()))
         return done;

      // Info

      asprintf(&buf, "%s - %s", event->Title(), tr("added to protection list"));
      Skins.Message(mtInfo, buf);
      free(buf);

      // append event to lock list

      lockedBroadcasts->Add(new cLockedBroadcast(event->Title()));
   }

   return success;
}

//***************************************************************************
// Pin Menu Item
//***************************************************************************
//***************************************************************************
// Process Key
//***************************************************************************

cPinMenuItem::cPinMenuItem(const char* title, int osState)
   : cOsdItem(title, (eOSState)osState)
{

}

//***************************************************************************
// Lock Menu
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cLockMenu::cLockMenu(const char* title, cLockItems* theItems, ListType theType)
   : cOsdMenu(title)
{
   cLockItem* item;

   items = theItems;
   type = theType;

   for (item = items->First(); item; item = items->Next(item))
      cOsdMenu::Add(new cLockMenuItem(item));

   if (type == ltPlugins)
      SetHelp(tr("Edit"), 0, 0, tr("On/Off"));
   else
      SetHelp(tr("Edit"), tr("Add"), tr("Delete"), tr("On/Off"));
   
   Display();
}

cLockMenu::~cLockMenu()
{
}

//***************************************************************************
// Process Key
//***************************************************************************

eOSState cLockMenu::ProcessKey(eKeys key)
{
   eOSState state = cOsdMenu::ProcessKey(key);

   if (state == osUnknown)
   {
      switch (key)
      {
         case kOk:
         case kRed:                          // Edit
         {
            if (!items->Count() || !Get(Current()))
               break;

            if (items->GetListType() == cPinPlugin::ltBroadcasts)
               state = AddSubMenu(new cBroadcastEditMenu((cLockedBroadcast*)
                                                         (((cLockMenuItem*)Get(Current()))->GetLockItem())));
            else
               state = AddSubMenu(new cLockEditMenu(((cLockMenuItem*)Get(Current()))->GetLockItem()));

            break;
         }

         case kGreen:                         // New
         {
            cLockItem* item;

            if (type != ltPlugins)
            {
               if (items->GetListType() == cPinPlugin::ltBroadcasts)
                  items->Add(item = new cLockedBroadcast("- new -"));
               else
                  items->Add(item = new cLockItem("- new -"));

               cOsdMenu::Add(new cLockMenuItem(item));

               Display();
            }

            break;
         }

         case kYellow:                        // Delete
         {
            if (type != ltPlugins)
            {
               if (!items->Count() || !Get(Current()))
                  break;

               if (Interface->Confirm(tr("Remove entry?")))
               {
                  // remove from list

                  items->Del(((cLockMenuItem*)Get(Current()))->GetLockItem());
                  cOsdMenu::Del(Current());
               }

               Display();
            }

            break;
         }

         case kBlue:                        // Toggle
         {
            cLockItem* p = ((cLockMenuItem*)Get(Current()))->GetLockItem();

            p->SetActive(!p->GetActive());
            ((cLockMenuItem*)Get(Current()))->Set();

            Display();

            break;
         }

         default:  break;
      }
   }

   return state;
}

//***************************************************************************
// Lock Menu Item
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cLockMenuItem::cLockMenuItem(cLockItem* aItem)
   : cOsdItem()
{
   item = aItem;
   Set();
}

cLockMenuItem::~cLockMenuItem()
{
}

//***************************************************************************
// Set
//***************************************************************************

void cLockMenuItem::Set()
{
   char* tmp;
   asprintf(&tmp, "%c %s", item->GetActive() ? '§' : ' ', GetTitle());
   SetText(tmp, /*copy*/ true);
   free(tmp);
}

//***************************************************************************
// Get Title
//***************************************************************************

const char* cLockMenuItem::GetTitle()
{ 
   if (item->GetTitle())
      return item->GetTitle();

   return item->GetName();
}

//***************************************************************************
// Lock Edit Menu
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cLockEditMenu::cLockEditMenu(cLockItem* aItem)
{
   static const char* trSearchModes[cLockedBroadcast::smCount] = { 0 };

   item = aItem;
   strncpy(name, item->GetName(), sizeNameMax);
   name[sizeNameMax] = 0;
   strncpy(pattern, aItem->GetPattern(), sizePatternMax);
   pattern[sizePatternMax] = 0;
   active = item->GetActive();
   searchMode = aItem->SearchMode();
   
   // translate search modes

   if (!trSearchModes[0])
      for (int i = 0; i < cLockedBroadcast::smCount; i++)
         trSearchModes[i] = tr(cLockedBroadcast::searchModes[i]);

   Add(new cMenuEditStrItem(tr("Name"), name, sizeNameMax, editCharacters));
   Add(new cMenuEditBoolItem(tr("Lock active"), &active));
   Add(new cMenuEditStrItem(tr("Keyword"), pattern, sizePatternMax, editCharacters));
	Add(new cMenuEditStraItem(tr("Search mode"), &searchMode, cLockedBroadcast::smCount, trSearchModes));

   if (item->HasRange())
   {
      long l;

      l = item->GetStart();
      start = (l/60/60) * 100 + l/60%60;
      l = item->GetEnd();
      end = (l/60/60) * 100 + l/60%60;

      Add(new cMenuEditTimeItem(tr("Start"), &start));
      Add(new cMenuEditTimeItem(tr("Stop"), &end));
   }

   SetTitle(item->GetTitle() ? item->GetTitle() : item->GetName());
}

//***************************************************************************
// Store
//***************************************************************************

void cLockEditMenu::Store(void)
{
   item->SetName(name);
   item->SetActive(active);
   item->SetPattern(pattern);
   item->SetSearchMode(searchMode);

   if (item->HasRange())
   {
      item->SetStart((start/100)*60*60 + (start%100)*60);
      item->SetEnd((end/100)*60*60 + (end%100)*60);
   }
}

//***************************************************************************
// Broadcast Edit Menu
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cBroadcastEditMenu::cBroadcastEditMenu(cLockedBroadcast* aItem)
   : cLockEditMenu((cLockItem*)aItem)
{
   // notching yet ...
}

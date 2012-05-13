/*
 * menu.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/interface.h>
#include <vdr/osdbase.h>
#include <vdr/plugin.h>

#include "locks.h"
#include "pin.h"

#define editCharacters " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-/()[]{}ßöäüÖÄÜ.,;#<>@0123456789"

//***************************************************************************
// Pin Menu Item
//***************************************************************************

class cPinMenuItem : public cOsdItem
{
   public:
   
      cPinMenuItem(const char* title, int osState);
};

//***************************************************************************
// Pin Menu
//***************************************************************************

class cPinMenu : public cOsdMenu, public PinService
{
   public:

      enum Item
      {
         miLock               = os_User,
         miAddCurrentChannel,
         miDelChannel,
         miEditChannelList,
         miEditBroadcastList,
         miAddCurrentBroadcast,
         miEditPluginList,
         miEditMenuItemList,
         miProtectSetup
      };
   
      cPinMenu(const char* title, cLockItems* theChannels, 
               cLockedBroadcasts* theBroadcasts, cLockItems* thePlugins,
               cLockItems* theMenuItems);
      virtual ~cPinMenu();
      
      virtual eOSState ProcessKey(eKeys key);

      int addCurrentChannel();
      int addCurrentBroadcast();
      int protectSetup();

  protected:

      cLockItems* lockedChannels;
      cLockedBroadcasts* lockedBroadcasts;
      cLockItems* lockedPlugins;
      cLockItems* lockedMenuItems;
};

//***************************************************************************
// Channel Menu Item
//***************************************************************************

class cLockMenuItem : public cOsdItem
{
      
   public:
   
      cLockMenuItem(cLockItem* aItem);
       virtual ~cLockMenuItem();

      const char* GetTitle();
      cLockItem* GetLockItem()       { return item; }
      virtual void Set();

   protected:

      cLockItem* item;
};

//***************************************************************************
// Channel Menu
//***************************************************************************

class cLockMenu : public cOsdMenu, public PinService
{
   public:
 
      cLockMenu(const char* title, cLockItems* theItems, ListType theType);
      virtual ~cLockMenu();
      
      eOSState ProcessKey(eKeys key);

   protected:

      cLockItems* items;
      ListType type;
};

//***************************************************************************
// Channel Edit Menu
//***************************************************************************

class cLockEditMenu : public cMenuSetupPage
{
   public:

      enum Misc
      {
         sizeNameMax    = 50,
         sizePatternMax = 50,
         sizeTitleMax   = 50
      };

      cLockEditMenu(cLockItem* aItem);
      virtual ~cLockEditMenu() {};

      virtual void Store(void);

   protected:

      cLockItem* item;
      char name[sizeNameMax+TB];
      int active;
      int start;
      int end;
      char pattern[sizePatternMax+TB];
      int searchMode;
};

//***************************************************************************
// Broadcast Edit Menu
//***************************************************************************

class cBroadcastEditMenu : public cLockEditMenu
{
   public:

      cBroadcastEditMenu(cLockedBroadcast* aBroadcast);
};

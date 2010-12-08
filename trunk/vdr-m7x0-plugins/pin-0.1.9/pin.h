/*
 * A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * File: pin.h 
 *
 * ---------------------------------------------------------------------
 * Version - History
 * ---------------------------------------------------------------------
 *
 * build  version  date       comment
 *

 * #34    0.1.9    19.05.07,  added:  setup option for log-level
                                      More frensh translations from pat, thanks!
                                      'no translation found' message now appear 
                                      only once for each text 
                                      (added internal cache for menu item translations)
 * #33    0.1.8    12.02.07,  added:  Autofill of list with menu-entry 
 * #32    0.1.7    31.01.07,  added:  Hiding protected plugins.
                              bugfix: Fixed problem with hotkey handling in pin menu
                              added:  Protection of menu entries.
                                      Hiding protected menu entries.
                                      New search mode 'start with'.
                                      Extended search modes (exact, regular expression, includes, start with) 
                                        now available for all items (channels, plugins, menuitems).
                                      Hiding protected items now configuable.
                                      added patch for epgsearch 0.9.20
 * #31    0.1.6a   29.01.07,  bugfix: Fixed syntax error in vdr patch (reported by zulu)
 * #30    0.1.6    28.01.07,  added:  Hiding protected recorings in recordings menu
 * #29    0.1.5b   12.01.07,  bugfix: Fixed error message when closing setup.
                                      Fixed translation in timer menue, thanks to Marc (zulu)
                              added:  Patch for vdr 1.4.5
                              change: Patch for epgsearch changed 
 * #28    0.1.5a   07.01.07,  change: pin in setup dialog now hidden again.
 * #27    0.1.5    06.01.07,  bugfix: Fixed protection leak when replaying via 
                                      the 'Play' key, reported by Christian Reiser.
                              change: Edit of timers fsk setting only protected if the 
                                      timer 'not' already locked
 * #26    0.1.4    23.11.06,  added:  Frensh translations from pat, thanks!
 * #25    0.1.3    23.11.06,  change: Pin input dialog now calls status class
                                      (thereby it's shown on graphTFT, ...)
                              added:  Edit of timers fsk setting only when unlocked ;)
                              bugfix: Some small fixes
 * #24    0.1.2    09.11.06,  change: limited pin to 5 digits
                              change: many namings and translations
                              bugfix: fixed handling of daylightsavingtime.
                              change: ask for inital pincode setup when entering menue
                              added:  EXCLUDEPLUGINS to makefile, here can defined which 
                                      plugins can't protected
                              added:  setup-plugin lock to pin menu (for fast and easy access)
                              added:  button blue now toggle protection in list
                              added:  patch for the timer menu of epgsearch
                                      the changes of thes release mostly suggested by Andreas Ritter
 * #23    0.1.1    11.09.06,  added:  Redesign of fskcheck communication (some code rework)
                              bugfix: Fixed translation bug in patch (Danke Frank!)
                              bugfix: Take care of daylightsavingtime.
                              added:  Changed patch due to epgsearch plugin requirements.
 * #22    0.1.0    09.09.06,  change: No protection until a pin code was configured 
                                      (requested by Andreas Ritter)
                              change: Enter pin menu directly if pin was 
                                      accepted (configurable) (requested by Andreas Ritter)
                              added:  Translation of script messages by 
                                      plugin (as far as found in translation list)
                                      Frensh translations from pat, thanks!
                              bugix:  Fixed calculation of 'inactivity' time
                              change: Some code cleanup
 * #21    0.0.21   10.06.06,  change: Removed green and yellow button in plugin-lock menu
 * #20    0.0.20   30.05.06,  bugfix: Fixed crash in empty lock menus (reported by Andreas Ritter)
                                      Fixed permissions of config path (reported by wilderigel)
 * #19    0.0.19   20.05.06,  change: Using skin for pin request dialog
 * #18    0.0.18   04.05.06,  change: Code redesign, pooled similar classes, ..
 *                            added:  Paragraph-Sign to mark activated locks in pin menu lists
 * #17    0.0.17   04.05.06,  added:  plugin protection
 *                            bugfix: fixed problem with recording subdirectories 
 *                                    (bug was reported by Viking, thanks!)
 * #16    0.0.16   01.05.06,  bugfix: fixed permissions ot system message queues.
 *                            added:  num keys navigation for pin menu
 *                            added:  Missing translation for 'Recording 
 *                                    protected, enter pin code first'.
 *                                    All this detected and reported by Michael Nausch, many thanks!
 * #15    0.0.15   29.04.06,  added:  french translations (thanks to Pat!)
 *                            change: Makefile due to VDR's new APIVERSION feature
 * #14    0.0.14   08.04.06,  added:  protection feature for scripting
 * #13    0.0.13   26.03.06,  bugfix: autoProtectionMode now respect start time
 *                                    of event (needed since channels not 
 *                                    protected every time)
 * #12    0.0.12   20.03.06,  change: now using aux field in timer config patch
 *                            bugfix: autoProtectionMode will now restored from config
 *                                    (Bug reported by rookie1, thanks!)
 *                            bugfix: autoProtectionMode now working even with vdr 1.3.44
 *                                    (Bug reported by rookie1, thanks!)
 * #11    0.0.11   26.02.06,  bugfix: create missing config directory at startup.
 *                            bugfix: keys 1-5 in pin menu now working.
 *                            added:  hungary translations (thanks to Albert Papp!)
 *                            change: silend skip of channels works now again 
 *                                    (fixed problems due to changes of VDR 1.3.40)
 * #10    0.0.10   28.01.06,  added:  regular expressions for brodcasts locks.
 *                            added:  russian translations (appropriated by Monchenko Vladimir, thanks!)
 *                            bugfix: only 3 chars of pin in setup menu (reported by Monchenko Vladimir, thanks!)
 * #9     0.0.9    26.01.06,  added:  lock for brodcasts.
 * #8     0.0.8    17.01.06,  added:  channel lock time range.
 * #7     0.0.7    15.01.06,  added:  channel lock list and improved configuration
 * #6     0.0.6f   13.01.06,  change: removed pin request on demand
 *        0.0.6e   23.12.05,  bugfix: syslog message at channel check
 *        0.0.6d   18.12.05,  added:  dynamic handling of menu entries
 *        0.0.6c   18.12.05,  added:  internationalisation
 *        0.0.6b   18.12.05,  change: reset time now in minutes
 *        0.0.6a   17.12.05,  added:  reset time for pin code
 *        0.0.6    17.12.05,  added:  protection flag to timer config
 * #5     0.0.5b   11.12.05,  bugfix: some bugfixes, added handling for recording
 *        0.0.5a   10.12.05,  added:  pin request on demand, even for replay of recorings
 *        0.0.5    10.12.05,  added:  pin request on demand
 * #4     0.0.4    08.12.05,  added:  channel protection
 * #3     0.0.3    06.12.05,  added:  protection of recording sub-directorys
 * #2     0.0.2    01.12.05,  bugfixes
 * #1     0.0.1    11.04.05,  initial release
 * 
 * ---------------------------------------------------------------------
 * Todos
 * ---------------------------------------------------------------------
 *
 * - broadcast locks via event description and/or epg text
 *
 * ---------------------------------------------------------------------
 */

#ifndef __PIN_H__
#define __PIN_H__

//***************************************************************************
// Includes
//***************************************************************************

#include <vdr/status.h>
#include <vdr/plugin.h>

#include "locks.h"
#include "def.h"
#include "i18n.h"
#include "talk.h"


//***************************************************************************
// Constants
//***************************************************************************

static const char *VERSION        = "0.1.9";
static const char *DESCRIPTION    = "Childlock plugin";
static const char *MAINMENUENTRY  = "Childlock";

#define PROTECTION_FILE "protection.fsk"
// #define __DEBUG__

//***************************************************************************
// Pin Service
//***************************************************************************

class PinService
{
   public:

      enum Size
      {
         sizePinCode = 5
      };

      enum ListType
      {
         ltUnknown = na,
         ltChannels,
         ltBroadcasts,
         ltPlugins,
         ltMenuItems
      };

      class Translation : public cListObject
      {
         public:

            char in[200+TB];
            char out[200+TB];
      };
};

class Translations : public cList<PinService::Translation>, PinService
{
   public:

      void append(const char* in, const char* out);
      const char* lookup(const char* in);

};

//***************************************************************************
// Message Reciver
//***************************************************************************

class MessageReceiver : public cThread
{
   public:
 
      MessageReceiver();
      ~MessageReceiver();

      int StartReceiver();
      int StopReceiver();

   protected:

      void Action();
      int wait();

      // data

      Talk* talk;
      int pid;
      bool active;
};

//***************************************************************************
// Pin Status Monitor
//***************************************************************************

class cPinStatusMonitor : public cStatus 
{
   protected:

      // interface

      virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
      virtual bool ChannelProtected(const cDevice *Device, const cChannel* Channel);
      virtual bool ReplayProtected(const cRecording* Recording, const char* Name, 
                                   const char* Base, bool isDirectory, int menuView = false);
      virtual void RecordingFile(const char* FileName);     
      virtual void TimerCreation(cTimer* Timer, const cEvent *Event);
      virtual bool PluginProtected(cPlugin* Plugin, int menuView = false);
      virtual bool MenuItemProtected(const char* Name, int menuView = false);
      virtual void UserAction(const eKeys key, const cOsdObject* Interact);

      // internal stuff

      const cEvent* GetEventOf(const cChannel* Channel);

      Translations translations;
};

//***************************************************************************
// Pin
//***************************************************************************

class cPin : public cOsdObject, public PinService
{
   public:

      cPin(void);
      virtual ~cPin();
      virtual void Show(void);
      virtual eOSState ProcessKey(eKeys Key);
      void clear();

   private:

      cOsd* osd;
      int textWidth;
      const char* pinTxt;
      char code[sizePinCode+TB];
      cSkinDisplayMessage* display;
      int pinSetup;
};

//***************************************************************************
// Pin Plugin
//***************************************************************************

class cPinPlugin : public cPlugin, public PinService
{
  public:
      
      enum AutoProtectionMode
      {
         apmUnknown = na,
         
         apmAlways,
         apmIntelligent,
         apmNever,

         apmCount
      };

      cPinPlugin(void);
      virtual ~cPinPlugin();

      const char* CommandLineHelp(void)        { return 0; }
      bool ProcessArgs(int argc, char *argv[]) { return true; }
      bool Start(void);
      void Stop(void);
      void Housekeeping(void)                  {}

      virtual cMenuSetupPage *SetupMenu(void);
      virtual bool SetupParse(const char *Name, const char *Value);
      const char* Version(void)                { return VERSION; }
      const char* Description(void)            { return DESCRIPTION; }
      virtual bool Initialize(void);
      virtual const char* MainMenuEntry(void)  { return tr(MAINMENUENTRY); }
      virtual cOsdObject* MainMenuAction(void);

      void addChannel();
      void delChannel();
      int channelProtected(const char* name, long startTime = 0);
      int pluginProtected(const char* name);
      int menuItemProtected(const char* text);
      int menuItemAppend(const char* text);
      int broadcastProtected(const char* title);
      void checkActivity();
      int initPluginList();
      void StorePin();

      static cPinPlugin* getObject()            { return object; }

      // static config items

      static char pinCode[sizePinCode+TB];
      static bool skipChannelSilent;
      static bool hideProtectedMenus;
      static bool hideProtectedPlugins;
      static bool hideProtectedRecordings;
      static int pinResetTime;
      static bool autoMenuOpen;
      static int autoProtectionMode;

      // other static stuff

      static const char* autoProtectionModes[apmCount+1];

  protected:

      // data

      cLockItems lockedChannels;
      cLockedBroadcasts lockedBroadcasts;
      cLockItems lockedPlugins;
      cLockItems lockedMenuItems;
      time_t lastAction;
      cPinStatusMonitor* statusMonitor;
      MessageReceiver* receiver;

      static cPinPlugin* object;    // the object
};

//***************************************************************************
#endif // __PIN_H__

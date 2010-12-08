/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: undelete.h 0.6 2006/03/15 22:12:06 hflor Exp $
 */

#ifndef __UNDELETE_H
#define __UNDELETE_H

#include "vdrtools.h"
#include <vdr/config.h>
#include <vdr/recording.h>
#include <vdr/plugin.h>

#define freenull(T) { if (T) { free(T); T = NULL; } }
#if VDRVERSNUM > 10307
  #define MAXOSDTEXTWIDTH 45
  #define AKTOSDTEXTWIDTH 45
  #define ERROR(E)  Skins.Message(mtError, E)
  #define INFO(I)   Skins.Message(mtInfo, I)
  #define STATUS(S) Skins.Message(mtStatus, S)
#elif VDRVERSNUM == 10307
  #error "cMenuText is in VDR 1.3.7 not in menu.h"
#else
  #define MAXOSDTEXTWIDTH MAXOSDWIDTH
  #define AKTOSDTEXTWIDTH Setup.OSDwidth
  #define ERROR(E)  Interface->Error(E)
  #define INFO(I)   Interface->Info(I)
  #ifdef HAVE_ELCHI
    #define STATUS(S) { Interface->Status(S, Setup.Theme == themeVanilla ? clrBlack : (eDvbColor)fginfofont, Setup.Theme == themeVanilla ? clrCyan : clrInfoLine); Interface->Flush(); }
  #else
    #define STATUS(S) { Interface->Status(S); Interface->Flush(); }
  #endif
#endif

#ifdef UND_Debug
extern int cMenuRecordingSelectItem_nextID;
extern int cMenuRecordingSelect_nextID;
extern int cMenuUndelete_nextID;
extern int cMenuSetupUndelete_nextID;
extern int cMenuDispKey_nextID;
#endif

#ifdef UND_Debug1
  #define d1syslog(p, t, a...)  dsyslog("%s: %-50s ID=%05d " t, plugin_name, p, objID, a)
  #define d1syslogi(p, t, a...) dsyslog("%s: %-50s          " t, plugin_name, p, a)
  #define d1listlog(p)          for (cOsdItem *item = First(); item; item = Next(item)) \
                                  d1syslog(p, "Index=%02d Osditem=%s", item->Index() + 1, item->Text())
#else
  #define d1syslog(p, t, a...)  {}
  #define d1syslogi(p, t, a...) {}
  #define d1listlog(p)          {}
#endif
#ifdef UND_Debug2
  #define d2syslog(p, t, a...)  dsyslog("%s: %-50s ID=%05d " t, plugin_name, p, objID, a)
  #define d2syslogi(p, t, a...) dsyslog("%s: %-50s          " t, plugin_name, p, a)
#else
  #define d2syslog(p, t, a...)  {}
  #define d2syslogi(p, t, a...) {}
#endif
#ifdef UND_Debug3
  #define d3syslog(p, t, a...)  dsyslog("%s: %-50s ID=%05d " t, plugin_name, p, objID, a)
  #define d3ProcessKey1(p)      bool noneKey = Key == kNone; \
                                if (!noneKey) \
                                  d3syslog(p, "Key=%s", KeyName(Key))
  #define d3ProcessKey2(p)      if (!noneKey || (state != osUnknown && state != osContinue)) \
                                { \
                                  d3syslog(p, "OSState=%s", OSStateName(state)); \
                                  noneKey = false; \
                                }
  #define d3ProcessKey3(p)      if (!noneKey || (state != osUnknown && state != osContinue)) \
                                  d3syslog(p, "returned OSState=%s", OSStateName(state))
#else
  #define d3syslog(p, t, a...)  {}
  #define d3ProcessKey1(p)      {}
  #define d3ProcessKey2(p)      {}
  #define d3ProcessKey3(p)      {}
#endif

#define kSalvageRec (kNone + 110)
#define kPurgeRec   (kNone + 111)
#define kDelLine    (kNone + 112)
#define kHelpKeys   (kNone + 113)

// Global variables that control the overall behaviour:

struct tParamInt {
  bool c;               // read command line
  bool r;               // read config file
  bool h;               // hide in setup menu
  int d;                // default
  int u;                // used
  int s;                // config file
  int o;                // old value for setup menu
  };

struct tParamChar {
  bool c;               // read command line
  bool r;               // read config file
  bool h;               // hide in setup menu
  char d;               // default
  char u;               // used
  char s;               // config file
  char o;               // old value for setup menu
  };

struct tParamFile {
  bool c;               // read command line
  bool r;               // read config file
  bool h;               // hide in setup menu
  char d[MaxFileName];  // default
  char u[MaxFileName];  // used
  char s[MaxFileName];  // config file
  char o[MaxFileName];  // old value for setup menu
  char e[MaxFileName];  // expanded name (environment)
  };

// var pos1
extern tParamInt   commandline_preference;
extern tParamInt   confirmpurge;
extern tParamInt   confirmpurgedir;
extern tParamInt   confirmpurgeall;
extern tParamInt   confirmsalvage;
extern tParamInt   confirmsalvagedir;
extern tParamInt   confirmsalvageall;
extern tParamInt   firstsalvage;
extern tParamInt   functionkey[];
extern tParamFile  mainmenu_name;
extern tParamInt   mainmenu_visible;
extern tParamInt   numkey;
extern tParamInt   okkey;
extern tParamInt   functionline_b_e[];
extern tParamInt   functionline_num[];
extern tParamInt   verbose;

extern char        plugin_name[];
extern cPlugin     *plugin;
extern bool        PurgeRecording;
extern bool        SalvageRecording;
extern cRecordings DeletedRecordings;
extern bool        MenuIsOpen;
extern char        *SVDRP_Process;
extern char        *WorkFilename;
extern int         KeyState;
extern const char  *FunctionLine[];
#define MaxFunctionName 11
extern const char  *FunctionName[];
extern const char  *FunctionHotKey[];
#define MaxKeysName 28
extern char        *KeysName[];

void ExpandEnvironment(tParamFile *FileStruc);

class cRemoveThread : public cThread {
protected:
  virtual void Action(void);
public:
  bool ActiveWithCancel(void);
  };

extern cRemoveThread oRemoveThread;

#endif //__UNDELETE_H

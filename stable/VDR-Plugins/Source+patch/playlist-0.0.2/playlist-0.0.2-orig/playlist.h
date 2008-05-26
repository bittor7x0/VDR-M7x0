/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: playlist.h 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#ifndef __PLAYLIST_H
#define __PLAYLIST_H

#include <vdr/config.h>
#include <vdr/recording.h>
#include <vdr/plugin.h>

#define FREENULL(T) { if (T) { free(T); T = NULL; } }
#if VDRVERSNUM >= 10307
#define MAXOSDTEXTWIDTH 45
#define AKTOSDTEXTWIDTH 45
#define ERROR(E)  Skins.Message(mtError, E)
#define INFO(I)   Skins.Message(mtInfo, I)
#define STATUS(S) Skins.Message(mtStatus, S)
#else
#define MAXOSDTEXTWIDTH MAXOSDWIDTH
#define AKTOSDTEXTWIDTH Setup.OSDwidth
#define ERROR(E)  Interface->Error(E)
#define INFO(I)   Interface->Info(I)
#endif

// Global variables that control the overall behaviour:

enum eStartOption {
  Start_DisplayPL = 0,
  Start_DisplayLast,
  Start_PlayLast,
  Start_NewEmpty,
  Start_max };

enum eNoYesDefault {
  NoYesDefault_no = 0,
  NoYesDefault_yes,
  NoYesDefault_defaultPlugin,
  NoYesDefault_defaultPlaylist,
  NoYesDefault_defaultRecordDir,
  NoYesDefault_max };

enum ePlayOptions {
  PlayOptions_firstpos = 0,
  PlayOptions_firstnew,
  PlayOptions_lastplay,
  PlayOptions_selectpos,
  PlayOptions_question,
  PlayOptions_defaultPlugin,
  PlayOptions_max };

enum eOptions {
  Option_confirmdeleterecord = 0,
  Option_confirmstartnewrecord,
  Option_deletefromlist,
  Option_deleterecord,
  Option_jumpmark,
  Option_playoption1,
  Option_playoption2,
  Option_playonlynew,
  Option_searchnewrecord,
  Option_searchrecordsub,
  Option_max };

#define MAXOptionLines 20

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

extern tParamChar  charentry;
extern tParamChar  charfolder;
extern tParamChar  charrecord;
extern tParamInt   commandline_preference;
extern tParamFile  lastplaylist;
extern tParamFile  mainmenu_name;
extern tParamInt   mainmenu_visible;
extern tParamInt   confirmdelplentry;
extern tParamInt   okstartplay;
extern tParamInt   deleterecords;
extern tParamFile  pathplaylists;
extern tParamInt   startoption;
extern tParamInt   storeplaylist;
extern tParamInt   timeoutreadrecords;
extern tParamInt   verbose;
extern tParamInt   PL_options[];

extern char        plugin_name[];
extern char        DisplayStatus[];
extern const char  *NoYesDefault[];
extern const char  *StartOptions[];
extern const char  *PlayOptions[];
extern char        *FileNameCharsAllowed;
extern char        *AllCharsAllowed;
extern char        *LastSelectedRecord;
extern bool        ControlMenuIsOpen;
#if VDRVERSNUM < 10311
extern cRecordings Recordings;
extern time_t      LoadTime_Recordings;
#endif

void ExpandEnvironment(tParamFile *FileStruc);

// --- cPluginPlaylist ----------------------------------------------------------

class cPluginPlaylist : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  int OSDLanguage;
  void TestAndSetOSDLanguage(void);
  bool ProcessArg(int argc, char *argv[]);

public:
  cPluginPlaylist(void);
  virtual ~cPluginPlaylist();
  virtual const char *Version(void);
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  const char *ExpandPath(const char *Filename, bool CreateDir = true);
  };

extern cPluginPlaylist *PluginPlaylist;

#endif //__PLAYLIST_H

/*
 * config.h: Configuration file handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "i18n.h"
#include "tools.h"

// VDR's own version number:

#define VDRVERSION  "1.4.7"
#define VDRVERSNUM   10407  // Version * 10000 + Major * 100 + Minor

// The plugin API's version number:

#define APIVERSION  "1.4.5"
#define APIVERSNUM   10405  // Version * 10000 + Major * 100 + Minor

// When loading plugins, VDR searches them by their APIVERSION, which
// may be smaller than VDRVERSION in case there have been no changes to
// VDR header files since the last APIVERSION. This allows compiled
// plugins to work with newer versions of the core VDR as long as no
// VDR header files have changed.

#define EPGHANDLER 1
#define LIEMIKUUTIO  130

// The MainMenuHook Patch's version number:
#define MAINMENUHOOKSVERSION "1.0.1"
#define MAINMENUHOOKSVERSNUM 10001  // Version * 10000 + Major * 100 + Minor

#define MAXPRIORITY 99
#define MINPRIORITY (-MAXPRIORITY)
#define MAXLIFETIME 99

#define DVLVIDPREFER_MAX 12

#define MINOSDWIDTH  480
#define MAXOSDWIDTH  672
#define MINOSDHEIGHT 324
#define MAXOSDHEIGHT 567

#define MaxFileName NAME_MAX + 1 // obsolete - use NAME_MAX directly instead!
                                 // NAME_MAX does not include '\0'
#define MaxSkinName 16
#define MaxThemeName 16

typedef uint32_t in_addr_t; //XXX from /usr/include/netinet/in.h (apparently this is not defined on systems with glibc < 2.2)

class cSVDRPhost : public cListObject {
private:
  struct in_addr addr;
  in_addr_t mask;
public:
  cSVDRPhost(void);
  bool Parse(const char *s);
  bool IsLocalhost(void);
  bool Accepts(in_addr_t Address);
  };

template<class T> class cConfig : public cList<T> {
private:
  char *fileName;
  bool allowComments;
  void Clear(void)
  {
    free(fileName);
    fileName = NULL;
    cList<T>::Clear();
  }
public:
  cConfig(void) { fileName = NULL; }
  virtual ~cConfig() { free(fileName); }
  const char *FileName(void) { return fileName; }
  bool Load(const char *FileName = NULL, bool AllowComments = false, bool MustExist = false)
  {
    cConfig<T>::Clear();
    if (FileName) {
       free(fileName);
       fileName = strdup(FileName);
       allowComments = AllowComments;
       }
    bool result = !MustExist;
    if (fileName && access(fileName, F_OK) == 0) {
       isyslog("loading %s", fileName);
       FILE *f = fopen(fileName, "r");
       if (f) {
          char *s;
          int line = 0;
          cReadLine ReadLine;
          result = true;
          while ((s = ReadLine.Read(f)) != NULL) {
                line++;
                if (allowComments) {
                   char *p = strchr(s, '#');
                   if (p)
                      *p = 0;
                   }
                stripspace(s);
                if (!isempty(s)) {
                   T *l = new T;
                   if (l->Parse(s))
                      this->Add(l);
                   else {
                      esyslog("ERROR: error in %s, line %d", fileName, line);
                      delete l;
                      result = false;
                      }
                   }
                }
          fclose(f);
          }
       else {
          LOG_ERROR_STR(fileName);
          result = false;
          }
       }
    if (!result)
       fprintf(stderr, "vdr: error while reading '%s'\n", fileName);
    return result;
  }
  bool Save(void) const
  {
    bool result = true;
    T *l = (T *)this->First();
    cSafeFile f(fileName);
    if (f.Open()) {
       while (l) {
             if (!l->Save(f)) {
                result = false;
                break;
                }
             l = (T *)l->Next();
             }
       if (!f.Close())
          result = false;
       }
    else
       result = false;
    return result;
  }
  };

class cNestedItem : public cListObject {
private:
  char *text;
  cList<cNestedItem> *subItems;
public:
  cNestedItem(const char *Text, bool WithSubItems = false);
  virtual ~cNestedItem();
  virtual int Compare(const cListObject &ListObject) const;
  const char *Text(void) const { return text; }
  cList<cNestedItem> *SubItems(void) { return subItems; }
  void AddSubItem(cNestedItem *Item);
  void SetText(const char *Text);
  void SetSubItems(bool On);
  };

class cNestedItemList : public cList<cNestedItem> {
private:
  char *fileName;
  bool Parse(FILE *f, cList<cNestedItem> *List, int &Line);
  bool Write(FILE *f, cList<cNestedItem> *List, int Indent = 0);
public:
  cNestedItemList(void);
  virtual ~cNestedItemList();
  void Clear(void);
  bool Load(const char *FileName);
  bool Save(void);
  };

class cSVDRPhosts : public cConfig<cSVDRPhost> {
public:
  bool LocalhostOnly(void);
  bool Acceptable(in_addr_t Address);
  };

extern cNestedItemList Folders;
extern cNestedItemList Commands;
extern cNestedItemList RecordingCommands;
extern cNestedItemList TimerCommands;
extern cSVDRPhosts SVDRPhosts;

class cSetupLine : public cListObject {
private:
  char *plugin;
  char *name;
  char *value;
public:
  cSetupLine(void);
  cSetupLine(const char *Name, const char *Value, const char *Plugin = NULL);
  virtual ~cSetupLine();
  virtual int Compare(const cListObject &ListObject) const;
  const char *Plugin(void) { return plugin; }
  const char *Name(void) { return name; }
  const char *Value(void) { return value; }
  bool Parse(char *s);
  bool Save(FILE *f);
  };

class cSetup : public cConfig<cSetupLine> {
  friend class cPlugin; // needs to be able to call Store()
  friend class cSVDRP;
private:
  void StoreLanguages(const char *Name, int *Values);
  bool ParseLanguages(const char *Value, int *Values);
  bool Parse(const char *Name, const char *Value);
  cSetupLine *Get(const char *Name, const char *Plugin = NULL);
  void Store(const char *Name, const char *Value, const char *Plugin = NULL, bool AllowMultiple = false);
  void Store(const char *Name, int Value, const char *Plugin = NULL);
public:
  // Also adjust cMenuSetup (menu.c) when adding parameters here!
  int __BeginData__;
  int OSDLanguage;
  char OSDSkin[MaxSkinName];
  char OSDTheme[MaxThemeName];
  int WarEagleIcons;
  int PrimaryDVB;
  int ShowInfoOnChSwitch;
  int TimeoutRequChInfo;
  int MenuScrollPage;
  int MenuScrollWrap;
  int MenuButtonCloses;
  int MarkInstantRecord;
  char NameInstantRecord[NAME_MAX + 1];
  int InstantRecordTime;
#ifdef M750S
  int LnbSLOF;
  int LnbFrequLo;
  int LnbFrequHi;
  int DiSEqC;
#endif
  int SetSystemTime;
  int TimeSource;
  int TimeTransponder;
  int MarginStart, MarginStop;
  int AudioLanguages[I18nNumLanguages + 1];
  int EPGLanguages[I18nNumLanguages + 1];
  int EPGScanTimeout;
  int EPGBugfixLevel;
  int EPGLinger;
//M7X0 BEGIN AK
  int EPGDefaultMode;
  int EPGInvDefaultMode;
  int EPGChannelNoLimit;
//M7X0 END AK
  int SVDRPTimeout;
  int ZapTimeout;
  int ChannelEntryTimeout;
  int PrimaryLimit;
  int DefaultPriority, DefaultLifetime;
  int PausePriority, PauseLifetime;
  int PauseKeyHandling;
  int UseSubtitle;
  int UseVps;
  int VpsMargin;
  int RecordingDirs;
  int FoldersInTimerMenu;
  int HotStandby;
  int TvMode;
  int VCRMode;
  int VideoDisplayFormat;
  int VideoFormat;
  int VideoWithoutSignal;
  int UpdateChannels;
  int UseDolbyDigital;
  int UseDolbyInRecordings;
  int UseHDInRecordings;
  int UseTSInHD;
  int UseTSInSD;
  int UseTSInAudio;
  int UseTeletextInTSRecordings;
  int UseSubtitlesInTSRecordings;
  int ChannelInfoPos;
  int ChannelInfoTime;
  int OSDLeft, OSDTop, OSDWidth, OSDHeight;
  int OSDMessageTime;
  int UseSmallFont;
  int MaxVideoFileSize;
  int MaxRecordingSize;
  int SplitEditedFiles;
  int DelTimeshiftRec;
  int HardLinkCutter;
  int MinEventTimeout, MinUserInactivity;
  int MultiSpeedMode;
  int ShowReplayMode;
  int ResumeID;
  int CurrentChannel;
  int CurrentVolume;
  int CurrentDolby;
  int ChannelFilter;
  int InitialChannel;
  int InitialVolume;
  int TShift;
  int TShiftBufferSize;
  int TShiftStartRecord;
  int TShiftDelayed;
  int TShiftPriority;
  int TShiftPause;
  int TShiftTimeout;
  int LRChannelGroups;
  int LRForwardRewind;
  int ShowRecDate, ShowRecTime, ShowRecLength, ShowProgressBar, MenuCmdPosition;
  int RecordingsSortMode;
  int RecordingsSortDirsFirst;
  int CutterAutoDelete;
  int CutterMaxBandwidth;
  int CutterRelBandwidth;
  int CutterWithMarks;
  int JumpSeconds, JumpSecondsRepeat, JumpFramesRepeat;
#ifdef USE_DIRECT_IO
  int ReplayUseDirectIO;
#endif
  int ShutdownMessage;
  int AbortWhenPluginFails;
  int EmergencyExit;
  int LircRepeatDelay;
  int LircRepeatFreq;
  int LircRepeatTimeout;
  int CapitalizeFilenames;
  int UseVidPrefer; // 0 = VDR's default, 1 = use
  int nVidPrefer;
  int VidPreferPrio[DVLVIDPREFER_MAX];
  int VidPreferSize[DVLVIDPREFER_MAX];
  int __EndData__;
  cSetup(void);
  cSetup& operator= (const cSetup &s);
  bool Load(const char *FileName);
  bool Save(void);
  };

extern cSetup Setup;

#endif //__CONFIG_H

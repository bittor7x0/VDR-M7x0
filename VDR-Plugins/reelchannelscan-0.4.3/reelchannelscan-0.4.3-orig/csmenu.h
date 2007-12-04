/********************************************************************************
 * csmenu.h channelscan menu part
 * writen by markus Hahn @ reel-multimedia
 *
 * See REDME for copyright information and
 * how to reach the authors.
 *
 ********************************************************************************/

#ifndef __CSMENU_H
#define __CSMENU_H


#include <string>
#include <vector>

#include <unistd.h>

#include <vdr/osdbase.h>
#include <vdr/channels.h>
#include <vdr/menuitems.h>
#include <vdr/thread.h>
#include <vdr/sources.h>
#include <vdr/config.h>

#include "scan.h"
#include "transponders.h"
#include "dirfiles.h"




extern cMutex mutexNames;
extern std::vector<std::string> tvChannelNames;
extern std::vector<std::string> radioChannelNames;
extern std::vector<std::string> dataChannelNames;

#ifdef DEBUG_CHANNELSCAN
extern std::vector<std::string> tvChannelList;
extern std::vector<std::string> radioChannelList;
extern std::vector<std::string> dataChannelList;
#endif 

// --- cMenuChannelscan  ----------------------------------------------------

class cMenuChannelscan : public cOsdMenu  {
private:
  cSetup data;
  int searchMode;
  int autoSearch;
  int source;
  int frequency;
  int symbolrate;
  char *srcTexts[4];
  int srcTypes[4];
  int srcTuners;
  const char *fecTexts[10];
  const char *modTexts[10];
  const char *sRateItem[3];
  const char *searchTexts[3];
  const char *sSRScanMode[3];
  const char *sBwItem[3];

  int fecStat;
  int modStat;
  int searchStat;
  char polarization;
  int bandwidth;
  int detailedScan;
  int srScanMode;

  scanParameters scp;

  int loopIndex;
  std::vector<int> loopSources;
  static int sourceStat;
  int sourceType;
  //static int autoSearchStat;
  static int channelNumber;
  bool expertSettings;

  int sRate6875;
  int sRateManual;
  int qam256;
   
  int lnbs;
  int currentChannel;

  void Set();
  void InitLnbs();
  void TunerDetection();
  void SetInfoBar();
public:
  cMenuChannelscan(int CurrentChannelNr);
  ~cMenuChannelscan();
  virtual eOSState ProcessKey(eKeys Key);
  void SwitchChannel();
  void DiseqShow();
  void Store();
  void AddBlankLineItem(int line);

  //static volatile bool scanning;
  static volatile int scanState;
  ///< internal scan state used to display correct messages in menues  
  
};

// --- cMenuScanActive ----------------------------------------------------


class cMenuScanActive : public cOsdMenu {
private:
  int oldChannelNumbers;
  int oldUpdateChannels;
  bool nitScan_;
  int transponderNum_;
  std::auto_ptr<cScan> Scan;
  scanParameters *scp;
  void ErrorMessage();
public:
  cMenuScanActive(scanParameters *sp);
  ~cMenuScanActive();
  eOSState ProcessKey(eKeys Key);
  void AddBlankLineItem(int lines);
  void DeleteDummy();
  void Setup();
  virtual bool NoAutoClose(void) { return true; }
};
// --- cMenuScanActiveItem ----------------------------------------------------

class cMenuScanActiveItem : public cOsdItem {
private:
  char *tvChannel;
  char *radioChannel;
public:
  cMenuScanActiveItem();
  cMenuScanActiveItem(const char *TvChannel,const char *RadioChannel);
  ~cMenuScanActiveItem();
  void SetValues();
};

// taken from menu.c
// --- cMenuSetupBase ---------------------------------------------------------
class cMenuSetupBase : public cMenuSetupPage {
protected:
  cSetup data;
  virtual void Store(void);
public:
  cMenuSetupBase(void);
};


// --- cMenuSetupLNB ---------------------------------------------------------

class cMenuSetupLNB : public cMenuSetupBase {
private:
  void Setup(void);
  void SetHelpKeys(void);
  cSource *source;

  const char *useDiSEqcTexts[4];
  const char *lofTexts[5];
  bool diseqcConfRead;
  int diseqObjNumber;
  int lnbNumber;
  int currentchannel;
  int waitMs;
  int repeat;

public:
  cMenuSetupLNB(void);
  virtual eOSState ProcessKey(eKeys Key);
};

#ifndef REELVDR
// --- Class cMenuEditSrcItem  ----------------------------------------------------

class cMenuEditSrcItem : public cMenuEditIntItem {
private:
  const cSource *source;
protected:
  virtual void Set(void);
public:
  cMenuEditSrcItem(const char *Name, int *Value);
  eOSState ProcessKey(eKeys Key);
};
#endif

// --- Class cMenuScanInfoItem   ----------------------------------------------------
class cMenuScanInfoItem : public cOsdItem {
public:
  cMenuScanInfoItem(const std::string& pos, int f, char c, int sr, int fecIndex);
  const char *FECToStr(int Index);
};

// --- Class cMenuStatusBar   ----------------------------------------------------
class cMenuStatusBar : public cOsdItem {
  int total;
  int part;
public:
  cMenuStatusBar(int total, int current, int channelNr, bool BarOnly = false);
  int LiteralCentStat(char **str);
};

// --- Class cMenuStatusBar   ----------------------------------------------------
class cMenuInfoItem : public cOsdItem {
public:
  cMenuInfoItem(const char *text, const char *textValue = NULL);
  cMenuInfoItem(const char *text, int intValue);
  cMenuInfoItem(const char *text, bool boolValue);
};

#ifdef REELVDR
// --- Class cMenuChannelsItem   ----------------------------------------------------

class cMenuChannelsItem : public cOsdItem {
private:
  cDirectoryEntry *entry_;
  bool isDir_;
public:
  cMenuChannelsItem(cDirectoryEntry *Entry);
  ~cMenuChannelsItem();
  const char *FileName(void) { return entry_->FileName(); }
  bool IsDirectory(void) { return entry_->IsDirectory(); }
};

// --- cMenuSelectChannelList  ----------------------------------------------------

class cMenuSelectChannelList : public cOsdMenu  {
private:
  int helpKeys_;
  int level_;
  int current_;
  int hasTimers_;
  std::string oldDir_;
  int DirState();
  bool ChangeChannelList();
  eOSState Open(bool Back = false);
public:
  virtual eOSState ProcessKey(eKeys Key);
  cMenuSelectChannelList();
  ~cMenuSelectChannelList(); 
  const char **ChannelLists();
  void Set();
  eOSState Store();
  eOSState Delete();
  void DumpDir();
};
#endif // REELVDR 

#endif // __CSMENU_H


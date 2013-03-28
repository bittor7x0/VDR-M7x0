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
//#include "dirfiles.h"




extern cMutex mutexNames;
extern std::vector<std::string> tvChannelNames;
extern std::vector<std::string> radioChannelNames;

#ifdef DEBUG_CHANNELSCAN
extern std::vector<std::string> tvChannelList;
extern std::vector<std::string> radioChannelList;
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
  const char *serviceTypeTexts[3];
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
#ifdef M750S   
  int lnbs;
#endif
  int currentChannel;


  void Set();
#ifdef M750S
  void InitLnbs();
#endif
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

  static const char *serviceTypeTxts[3];
  
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

#ifdef M750S
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
#endif

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
  cMenuStatusBar(int total, int current, int channelNr, int mode, bool BarOnly = false);
  int LiteralCentStat(char **str);
};

// --- Class cMenuStatusBar   ----------------------------------------------------
class cMenuInfoItem : public cOsdItem {
public:
  cMenuInfoItem(const char *text, const char *textValue = NULL);
  cMenuInfoItem(const char *text, int intValue);
  cMenuInfoItem(const char *text, bool boolValue);
};

#endif // __CSMENU_H


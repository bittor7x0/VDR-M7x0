/***********************************************************
 * Header file holding Class Definitions from <VDR>/menu.c *
 *                                                         *
 * This file is generated automaticly!                     *
 * Do not make changes!                                    *
 *                                                         *
 ***********************************************************/

// Headers included by <VDR>/menu.c
#include "menu.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "channels.h"
#include "config.h"
#include "cutter.h"
#include "eitscan.h"
#include "i18n.h"
#include "interface.h"
#include "plugin.h"
#include "recording.h"
#include "remote.h"
#include "sources.h"
#include "status.h"
#include "themes.h"
#include "timers.h"
#include "transfer.h"
#include "videodir.h"

// local headers needed
#include "setupsystemmenu.h"
#include "setupmenu.h"
#include "debug.h"
// ************************************
// SetupClasses defined in <VDR>/menu.c
// ************************************

class cMenuSetupBase : public cMenuSetupPage {
protected:
  cSetup data;
  virtual void Store(void);
public:
  cMenuSetupBase(void);
  };


class cMenuSetupOSD : public cMenuSetupBase {
private:
  const char *useSmallFontTexts[3];
  int numSkins;
  int originalSkinIndex;
  int skinIndex;
  const char **skinDescriptions;
  cThemes themes;
  int themeIndex;
  virtual void Set(void);
public:
  cMenuSetupOSD(void);
  virtual ~cMenuSetupOSD();
  virtual eOSState ProcessKey(eKeys Key);
  };


class cMenuSetupEPG : public cMenuSetupBase {
private:
  int originalNumLanguages;
  int numLanguages;
  void Setup(void);
public:
  cMenuSetupEPG(void);
  virtual eOSState ProcessKey(eKeys Key);
  };


class cMenuSetupDVB : public cMenuSetupBase {
private:
  int originalNumAudioLanguages;
  int numAudioLanguages;
  void Setup(void);
  const char *videoDisplayFormatTexts[3];
  const char *videoFormatTexts[3];
  const char *updateChannelsTexts[6];
public:
  cMenuSetupDVB(void);
  virtual eOSState ProcessKey(eKeys Key);
  };


class cMenuSetupLNB : public cMenuSetupBase {
private:
  void Setup(void);
public:
  cMenuSetupLNB(void);
  virtual eOSState ProcessKey(eKeys Key);
  };


class cMenuSetupCICAM : public cMenuSetupBase {
private:
  eOSState Menu(void);
  eOSState Reset(void);
public:
  cMenuSetupCICAM(void);
  virtual eOSState ProcessKey(eKeys Key);
  };


class cMenuSetupRecord : public cMenuSetupBase {
public:
  cMenuSetupRecord(void);
  };


class cMenuSetupReplay : public cMenuSetupBase {
protected:
  virtual void Store(void);
public:
  cMenuSetupReplay(void);
  };


class cMenuSetupMisc : public cMenuSetupBase {
public:
  cMenuSetupMisc(void);
  };


class cMenuSetupPlugins : public cMenuSetupBase {
public:
  cMenuSetupPlugins(void);
  virtual eOSState ProcessKey(eKeys Key);
  };


//############################################################################
// cSetupSystemMenu
//############################################################################
cSetupSystemMenu::cSetupSystemMenu()
{
}
cSetupSystemMenu::~cSetupSystemMenu()
{
}
// Get SystemMenu
cOsdMenu *cSetupSystemMenu::GetSystemMenu(const char *sysCommand, Config  *config)
{
 cOsdMenu *menu=NULL;
 if( sysCommand != NULL)
 {
    if( strcmp(sysCommand, "ActPlugins") == 0)
        menu =new cSetupPluginMenu(config);
//    else
//      if(strcmp(sysCommand, "VDRMenu") == 0)
//        menu = new cSetupVdrMenu(tr("Menu Edit"));
    else
      if(strcmp(sysCommand, "OSD") == 0)
         menu = new cMenuSetupOSD;
    else
      if(strcmp(sysCommand, "EPG") == 0)
         menu = new cMenuSetupEPG;
    else
      if(strcmp(sysCommand, "DVB") == 0)
         menu = new cMenuSetupDVB;
    else
      if(strcmp(sysCommand, "LNB") == 0)
         menu = new cMenuSetupLNB;
    else
      if(strcmp(sysCommand, "CICAM") == 0)
         menu = new cMenuSetupCICAM;
    else
      if(strcmp(sysCommand, "Record") == 0)
         menu = new cMenuSetupRecord;
    else
      if(strcmp(sysCommand, "Replay") == 0)
         menu = new cMenuSetupReplay;
    else
      if(strcmp(sysCommand, "Misc") == 0)
         menu = new cMenuSetupMisc;
    else
      if(strcmp(sysCommand, "Plugins") == 0)
         menu = new cMenuSetupPlugins;
   else
      DEBUG3("%s: <menu System=%s> not known\n", DBG_PREFIX, sysCommand);
  }
  return (menu);
}

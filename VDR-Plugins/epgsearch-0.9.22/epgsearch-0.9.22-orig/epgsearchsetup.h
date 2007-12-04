#ifndef __EPGSEARCH_H
#define __EPGSEARCH_H

#include <vdr/plugin.h>
#include "epgsearchcfg.h"
#include <vector>

class cPluginEpgsearch;

class cMenuSetupSubMenu : public cOsdMenu {
  protected:
    cEPGSearchConfig* data;
    std::vector<const char*> helpTexts;
    eOSState Help();
    void AddHelp(const char* helpText);
    virtual eOSState ProcessKey(eKeys Key);
    virtual void Set(void) = 0;
  public:
    cMenuSetupSubMenu(const char* Title, cEPGSearchConfig* Data);
  };


class cMenuSetupGeneral : public cMenuSetupSubMenu 
{
 protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
 public:
    cMenuSetupGeneral(cEPGSearchConfig* Data);
};

class cMenuSetupEPGMenus : public cMenuSetupSubMenu 
{
 protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
    void SetHelpKeys();
 public:
    cMenuSetupEPGMenus(cEPGSearchConfig* Data);
};

class cMenuSetupUserdefTimes : public cMenuSetupSubMenu
{
 protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
 public:
    cMenuSetupUserdefTimes(cEPGSearchConfig* Data);
};

class cMenuSetupTimers : public cMenuSetupSubMenu
{
 protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
    void SetHelpKeys();
 public:
    cMenuSetupTimers(cEPGSearchConfig* Data);
};

class cMenuSetupSearchtimers : public cMenuSetupSubMenu 
{
 protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
    void SetHelpKeys();
 public:
    cMenuSetupSearchtimers(cEPGSearchConfig* Data);
};

class cMenuSetupTimerConflicts : public cMenuSetupSubMenu 
{
 protected:
    virtual eOSState ProcessKey(eKeys Key);
    void Set(void);
 public:
    cMenuSetupTimerConflicts(cEPGSearchConfig* Data);
};

class cMenuSetupMailNotification : public cMenuSetupSubMenu 
{
   char tmpMailAuthPass[MaxFileName];      
   void SetHelpKeys();
  protected:
   virtual eOSState ProcessKey(eKeys Key);
   void Set(void);
  public:
   cMenuSetupMailNotification(cEPGSearchConfig* Data);
   eOSState TestMailAccount();
   
   static const char *HostNameChars;
   static const char *UserNameChars;
   static const char *PasswordChars;
   static const char *MailBoxChars;
};

class cMenuEPGSearchSetup : public cMenuSetupPage {
  private:
    virtual void Setup(void);
    cEPGSearchConfig data;
    int delaySearchTimerThreadMode;
  protected:
    virtual eOSState ProcessKey(eKeys Key);
    virtual void Store(void);
    void Set(void);
  public:
    cMenuEPGSearchSetup(void);
  };

#endif

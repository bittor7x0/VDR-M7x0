#ifndef __RADIOINFO_H
#define __RADIOINFO_H


#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/receiver.h>

#include "radioInfoReceiver.h" 
#include "radioInfoFilter.h"
#include "radioInfoOsd.h"
#include "sRadioInfo.h"




class cPluginRadioinfo : public cPlugin, cStatus 
{
private:
  // Add any member variables or functions you may need here.
  
  cRadioInfoOsd* radioInfoOsd;
  cRadioInfoReceiver* radioInfoReceiver;
  cRadioInfoFilter* radioInfoFilter;
  sRadioInfo* radioInfo;
    
  void showOsd(void);
  void hideOsd(void);
  void resetInfo(void);
  
  
  bool wantsOsd;
  
  cChannel* currentChannel;
  cDevice*  currentDevice;
  
public:
  cPluginRadioinfo(void);
  virtual ~cPluginRadioinfo();
  virtual const char *Version(void);
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  
  void foundInfoPid(int Pid);
  
protected:
  virtual void ChannelSwitch(const cDevice* Device, int ChannelNumber);
  virtual void OsdClear(void);
  
  
  friend class cRadioInfoOsd;
  friend class cRadioFilter;
  };



extern cPluginRadioinfo* currentRadioInfo;




#endif //__RADIOINFO_H

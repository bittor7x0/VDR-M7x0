#ifndef __RADIOINFO_H
#define __RADIOINFO_H


#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/receiver.h>

#include "radioInfoReceiver.h" 
#include "radioInfoOsd.h"
#include "radioInfoData.h"


///////////////////////////////////////////////////////////////////////////////


class cPluginRadioinfo : public cPlugin, cStatus 
{
private:
  cRadioInfoOsd* radioInfoOsd;
  cRadioInfoReceiver* radioInfoReceiver;
  cRadioInfoData radioInfoData;
  
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

  void FoundInfoPid(int Pid);
  static cPluginRadioinfo* currentRadioInfo;
   
protected:
  virtual void ChannelSwitch(const cDevice* Device, int ChannelNumber);
  
  //friend class cRadioInfoOsd;
};


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFO_H

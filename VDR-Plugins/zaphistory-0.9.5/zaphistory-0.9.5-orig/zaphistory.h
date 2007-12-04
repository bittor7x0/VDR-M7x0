/*
 * zaphistory.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __ZAPHISTORY_H
#define __ZAPHISTORY_H

#include <vdr/plugin.h>
#include "zaplist.h"
#include "monitor.h"

class cPluginZaphistory : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cZapHistoryMonitor *monitor;
public:
  cPluginZaphistory(void);
  virtual ~cPluginZaphistory();
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
  };

#endif

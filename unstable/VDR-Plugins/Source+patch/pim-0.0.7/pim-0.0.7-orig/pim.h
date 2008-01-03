/*
 * $Id: pim.h,v 1.0 2007/08/14 20:00:34 torsten Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: pim.h,v $
 * Revision 1.0 2007/08/14 20:00:34 torsten
 * Initial version
 *
 */

#ifndef VDR_PIM_H
#define VDR_PIM_H

#include <vdr/plugin.h>

static const char *VERSION        = "0.0.7";
static const char *DESCRIPTION    = "Simple Personal Information Manager";
static const char *MAINMENUENTRY  = "Calendar";

class cPluginPim : public cPlugin {
public:
  cPluginPim(void);
  virtual ~cPluginPim();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);

private:
  bool isTimerStartMode(time_t Now);
};

extern cPluginPim *pluginPIM;

#endif


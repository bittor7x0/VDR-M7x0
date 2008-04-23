#ifndef __PILOT_H
#define __PILOT_H

#include <vdr/plugin.h>

static const char *VERSION        = "0.0.9";
static const char *DESCRIPTION    = "A zapping co-pilot";
static const char *MAINMENUENTRY  = "Pilot";

class cPluginPilot : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginPilot(void);
  virtual ~cPluginPilot();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

#endif //__PILOT_H

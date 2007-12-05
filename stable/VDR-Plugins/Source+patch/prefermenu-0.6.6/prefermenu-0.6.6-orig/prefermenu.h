#ifndef __PREFERMENU_H
#define __PREFERMENU_H

#include <vdr/plugin.h>

static const char *VERSION        = "0.6.6";
static const char *DESCRIPTION    = "Prefer Channel Menu";
static const char *MAINMENUENTRY  = "Prefermenu";

class cPluginPrefermenu : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginPrefermenu(void);
  virtual ~cPluginPrefermenu();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

#endif //__PREFERMENU_H

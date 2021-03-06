#ifndef __AIDE_H
#define __AIDE_H

#include <vdr/plugin.h>

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = "VDR Help";
static const char *MAINMENUENTRY  = "Help";

// --- cPluginLocker -------------------------------------------------------------

class cPluginAide : public cPlugin {
private:
public:
  cPluginAide(void);
  virtual ~cPluginAide(void);
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  //virtual cMenuSetupPage *SetupMenu(void);
  //virtual bool SetupParse(const char *Name,const char *Value);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  //void Store(void);
};

extern cPluginAide PlugAide;

#endif //__AIDE_H

#ifndef __PLUGSUBMENU_H
#define __PLUGSUBMENU_H

#include <vdr/plugin.h>
#include <vdr/submenu.h>
#include <vdr/i18n.h>


static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "SubMenu Maker";

class cPluginSubMenu : public cPlugin{
private:
public:
  cPluginSubMenu(void);
  virtual ~cPluginSubMenu(void);
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name,const char *Value);
};

#endif //__PLUGSUBMENU_H

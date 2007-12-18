#ifndef __CHANORG_H
#define __CHANORG_H

#include <vdr/plugin.h>
#include <vdr/i18n.h>

static const char *VERSION        = "0.0.6";
static const char *DESCRIPTION    = "Channels Organizer";
static const char *MAINMENUENTRY  = "Channels Organizer";

// --- cPluginLocker -------------------------------------------------------------

class cPluginChanOrg : public cPlugin {
private:
public:
  cChannels OrgChans;
  cTimers Tmr;
  char *TmrChan[255];
  int LastAffect;
  cPluginChanOrg(void);
  virtual ~cPluginChanOrg(void);
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  bool ShowOsd;
  bool ReDraw;
  const char *ShowOsdFile;
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  void Debug(void);
};

extern cPluginChanOrg ChanOrg;

#endif //__AIDE_H

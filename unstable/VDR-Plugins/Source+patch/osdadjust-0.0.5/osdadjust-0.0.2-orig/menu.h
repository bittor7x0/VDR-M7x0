#ifndef _OSDADJUST_MENU__H
#define _OSDADJUST_MENU__H

#include <vdr/plugin.h>
#include "config.h"
#include "i18n.h"


class cMenuSetupOsdadjust : public cMenuSetupPage {
private:
  int newAspectRatio;
  int newVideoFormat;
  int newHideMenuEntry;
  int newHideHelpText;
  int newShowGrid;
  int newMute;
  int newBgColor;
  int newFgColor;
  int newLineTn;
  int newMoveInc;
  const char *aColors[11];
  const char *aRatios[3];
  const char *aFormat[3];
  class cOpenScreenMenu : public cOsdItem {
  private:
    cMenuSetupOsdadjust* parent;
  public:
    cOpenScreenMenu(cMenuSetupOsdadjust* p) : cOsdItem(tr(config.MainMenuEntry)) { parent = p; }
    virtual eOSState ProcessKey(eKeys Key);
  };
protected:
  virtual void Store(void);
  void AddCategory(const char *Title);
  void AddEmptyLine(void);
public:
  cMenuSetupOsdadjust(void);
};

#endif //_OSDADJUST_MENU__H

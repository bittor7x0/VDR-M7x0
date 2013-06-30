/*
 * setup.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "config.h"
#include <vdr/plugin.h>

class cPluginSkinEnigmaSetup : public cMenuSetupPage {
private:
  cEnigmaConfig data;

  virtual void Setup(void);
protected:
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Store(void);
public:
    cPluginSkinEnigmaSetup(void);
};

class cMenuSetupSubMenu : public cOsdMenu {
protected:
  cEnigmaConfig *data;
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Set(void) = 0;
  void AddCategory(const char *Title);
public:
  cMenuSetupSubMenu(const char *Title, cEnigmaConfig *Data);
};

class cMenuSetupGeneral : public cMenuSetupSubMenu {
private:
  const char *showRemainingTexts[4];
  const char *showRecSizeTexts[3];
  const char *statusLineModeTexts[3];
  const char *showScrollbarTexts[3];
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupGeneral(cEnigmaConfig *Data);
};

class cMenuSetupLogos : public cMenuSetupSubMenu {
private:
  const char *showSymbolsTexts[4];
  const char *resizeImagesTexts[3];
#ifdef USE_PLUGIN_MAILBOX
  const char *showMailIconTexts[3];
#endif
  const char *showStatusSymbolsTexts[3];
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupLogos(cEnigmaConfig *Data);
};

class cMenuSetupAnimText : public cMenuSetupSubMenu {
private:
  const char *scrollModeTexts[2];
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupAnimText(cEnigmaConfig *Data);
};

class cMenuSetupTTF : public cOsdMenu {
private:
  FontInfo *data;
  int nFont;
  int nWidth;
  int nSize;
  cStringList *fontList;
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
  void Store(void);
public:
  cMenuSetupTTF(FontInfo *fontinfo, cStringList* fontList);
};

class cMenuSetupFonts : public cMenuSetupSubMenu {
private:
  cStringList fontNames;
  cStringList fontMonoNames;

protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupFonts(cEnigmaConfig *Data);
  virtual ~cMenuSetupFonts(void);
};

#ifdef USE_PLUGIN_EPGSEARCH
class cMenuSetupEpgSearch : public cMenuSetupSubMenu {
private:
  const char *useSubtitleRerunTexts[3];
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupEpgSearch(cEnigmaConfig *Data);
};
#endif

// vim:et:sw=2:ts=2:

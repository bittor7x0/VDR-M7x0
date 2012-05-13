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
  void AddCategory(const char *Title);
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
public:
  cMenuSetupSubMenu(const char *Title, cEnigmaConfig *Data);
};

class cMenuSetupGeneral : public cMenuSetupSubMenu {
private:
  const char *showRemainingTexts[3];
  const char *showRecSizeTexts[3];
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
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupLogos(cEnigmaConfig *Data);
};

#ifndef DISABLE_ANIMATED_TEXT
class cMenuSetupAnimText : public cMenuSetupSubMenu {
private:
  const char *scrollModeTexts[2];
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupAnimText(cEnigmaConfig *Data);
};
#endif

#ifdef HAVE_FREETYPE
class cMenuSetupTTF : public cOsdMenu {
private:
  FontInfo *data;
  int nFont;
  int nWidth;
  int nSize;
  const char **availTTFs;
  int nMaxTTFs;
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
  void Store(void);
public:
  cMenuSetupTTF(FontInfo *fontinfo);
};

#endif

class cMenuSetupFonts : public cMenuSetupSubMenu {
protected:
  virtual eOSState ProcessKey(eKeys Key);
  void Set(void);
public:
  cMenuSetupFonts(cEnigmaConfig *Data);
  virtual ~cMenuSetupFonts(void);
};

#ifdef SKINENIGMA_HAVE_EPGSEARCH
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

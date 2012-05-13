/*
 * config.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINENIGMA_CONFIG_H
#define __SKINENIGMA_CONFIG_H

#include "common.h"

#ifdef HAVE_IMAGEMAGICK
#define NUM_IMAGEEXTENSIONTEXTS 3
extern const char *imageExtensionTexts[NUM_IMAGEEXTENSIONTEXTS];
#else
#define NUM_IMAGEEXTENSIONTEXTS 0
#endif

#ifdef HAVE_FREETYPE
# include "font.h"
#endif

#include <vdr/skins.h>
#include <vdr/font.h>

#ifndef MAXFONTNAME
#define MAXFONTNAME 64
#endif
#ifndef MAXFONTSIZE
#define MAXFONTSIZE 64
#endif

struct EnigmaOsdSize
{
  int x;
  int y;
  int w;
  int h;
};

#define FONT_TRUETYPE 0

enum
{
  FONT_OSDTITLE,
  FONT_MESSAGE,
  FONT_DATE,
  FONT_HELPKEYS,
  FONT_CITITLE,
  FONT_CISUBTITLE,
  FONT_CILANGUAGE,
  FONT_LISTITEM,
  FONT_INFOTIMERHEADLINE,
  FONT_INFOTIMERTEXT,
  FONT_INFOWARNHEADLINE,
  FONT_INFOWARNTEXT,
  FONT_DETAILSTITLE,
  FONT_DETAILSSUBTITLE,
  FONT_DETAILSDATE,
  FONT_DETAILSTEXT,
  FONT_REPLAYTIMES,
  FONT_FIXED,
  FONT_NUMFONTS
};

struct FontInfo
{
  int VdrId;
  char Name[MAXFONTNAME + 1];
  int Width;
  int Size;
  int Default;
};

struct FontConfig
{
  int Id;
  char *KeyId;
  char *KeyName;
};

extern FontConfig allFontConfig[FONT_NUMFONTS];

struct cEnigmaConfig
{
private:
  char logoDir[255];
  char strImagesDir[255];
#ifdef HAVE_FREETYPE
  char strFontsDir[255];
#endif
public:
  cEnigmaConfig();
  ~cEnigmaConfig();
  void SetLogoDir(const char *logodirP);
  char *GetLogoDir(void) { return logoDir; }
  void SetImagesDir(const char *dir);
  char *GetImagesDir(void) { return strImagesDir; }
#ifdef HAVE_FREETYPE
  void SetFontsDir(const char *dir);
  char *GetFontsDir(void) { return strFontsDir; }
#endif
  const char *GetImageExtension(void);
  const cFont *GetFont(int id, const cFont *pFontCur = NULL);
  void SetFont(int id, const char *font);
  void SetFont(int id, int vdrId);
  void GetOsdSize(struct EnigmaOsdSize *size);
  int showAuxInfo;
  int showLogo;
  int showVps;
  int showSymbols;
  int showSymbolsMenu;
  int showSymbolsReplay;
  int showSymbolsMsgs;
  int showSymbolsAudio;
  int showListSymbols;
  int showProgressbar;
  int cacheSize;
  int useChannelId;
  int showInfo;
  int showRemaining;
  int showMarker;
  int singleArea;
  int singleArea8Bpp;
  int showFlags;
  int numReruns;
  int useSubtitleRerun;
  int showTimerConflicts;
  int showRecSize;
  int showImages;
  int resizeImages;
  int showMailIcon;
  int imageWidth;
  int imageHeight;
  int imageExtension;
  int fullTitleWidth;
  int useTextEffects;
  int scrollDelay;
  int scrollPause;
  int scrollMode;
  int blinkPause;
  int scrollInfo;
  int scrollListItem;
  int scrollOther;
  int scrollTitle;
  int dynOsd;
  FontInfo allFonts[FONT_NUMFONTS];
};

extern cEnigmaConfig EnigmaConfig;

#endif // __SKINENIGMA_CONFIG_H

// vim:et:sw=2:ts=2:

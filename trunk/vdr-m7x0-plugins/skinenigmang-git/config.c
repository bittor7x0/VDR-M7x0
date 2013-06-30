/*
 * config.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "config.h"
#include "tools.h"

#ifdef USE_PLUGIN_AVARDS
#include "services/avards.h"
#endif

#include <stdlib.h>
#include <vdr/font.h>
#include <vdr/plugin.h>

#ifdef HAVE_IMAGEMAGICK
const char *imageExtensionTexts[NUM_IMAGEEXTENSIONTEXTS] = { "xpm", "jpg", "png" };
#endif

cEnigmaConfig EnigmaConfig;

cGraphtftFont FontCache;

FontConfig allFontConfig[FONT_NUMFONTS] =
{
  { FONT_OSDTITLE,          "FontOsdTitle",          "FontOsdTitleName" },
  { FONT_MESSAGE,           "FontMessage",           "FontMessageName" },
  { FONT_DATE,              "FontDate",              "FontDateName" },
  { FONT_HELPKEYS,          "FontHelpKeys",          "FontHelpKeysName" },
  { FONT_CITITLE,           "FontCiTitle",           "FontCiTitleName" },
  { FONT_CISUBTITLE,        "FontCiSubtitle",        "FontCiSubtitleName" },
  { FONT_CILANGUAGE,        "FontCiLanguage",        "FontCiLanguageName" },
  { FONT_LISTITEM,          "FontListItem",          "FontListItemName" },
  { FONT_INFOTIMERHEADLINE, "FontInfoTimerHeadline", "FontInfoTimerHeadlineName" },
  { FONT_INFOTIMERTEXT,     "FontInfoTimerText",     "FontInfoTimerTextName" },
  { FONT_INFOWARNHEADLINE,  "FontInfoWarnHeadline",  "FontInfoWarnHeadlineName" },
  { FONT_INFOWARNTEXT,      "FontInfoWarnText",      "FontInfoWarnTextName" },
  { FONT_DETAILSTITLE,      "FontDetailsTitle",      "FontDetailsTitleName" },
  { FONT_DETAILSSUBTITLE,   "FontDetailsSubtitle",   "FontDetailsSubtitleName" },
  { FONT_DETAILSDATE,       "FontDetailsDate",       "FontDetailsDateName" },
  { FONT_DETAILSTEXT,       "FontDetailsText",       "FontDetailsTextName" },
  { FONT_REPLAYTIMES,       "FontReplayTimes",       "FontReplayTimesName" },
  { FONT_FIXED,             "FontFixed",             "FontFixedName" }
};

cEnigmaConfig::cEnigmaConfig() : showAuxInfo(1), showLogo(1), showVps(1), showSymbols(1),
                                 showSymbolsMenu(0), showSymbolsReplay(1), showSymbolsMsgs(1), showSymbolsAudio(1), showColSymbolsDetails(0),
                                 showListSymbols(1), showProgressbar(1), cacheSize(50),
                                 useChannelId(0), showInfo(1), minInfoWidth(144), showRemaining(0), showMarker(1),
                                 singleArea(1), singleArea8Bpp(1), showFlags(1), numReruns(5),
                                 useSubtitleRerun(1), showTimerConflicts(1), showRecSize(2),
                                 showImages(0), resizeImages(0), showMailIcon(0),
                                 imageWidth(120), imageHeight(80), imageExtension(0), fullTitleWidth(0),
                                 useTextEffects(0), scrollDelay(50), scrollPause(1500), scrollMode(0),
                                 blinkPause(1000), scrollInfo(1), scrollListItem(1), scrollOther(1), scrollTitle(1),
                                 dynOsd(0), statusLineMode(0), showWssSymbols(0), showStatusSymbols(0), showScrollbar(1),
                                 showSignalInfo(0), showCatGenCon(0), showCaMode(0), drawRoundCorners(1), channelLogoWidth(80), channelLogoHeight(80),
                                 signalInfoWidth(74), progressBarWidth(124)
{
  memset(logoDir, 0, sizeof(logoDir));
  memset(strImagesDir, 0, sizeof(strImagesDir));

  for (int id = 0; id < FONT_NUMFONTS; id++) {
    switch (id) {
      case FONT_CISUBTITLE:
      case FONT_CILANGUAGE:
      case FONT_DETAILSSUBTITLE:
        allFonts[id].VdrId = fontSml + 1;
        allFonts[id].Default = fontSml;
        break;
      case FONT_FIXED:
        allFonts[id].VdrId = fontFix + 1;
        allFonts[id].Default = fontFix;
        break;
      default:
        allFonts[id].VdrId = fontOsd + 1;
        allFonts[id].Default = fontOsd;
    }
    memset(allFonts[id].Name, 0, sizeof(allFonts[id].Name));
    allFonts[id].Width = 100;
    allFonts[id].Size = 26;
  }
}

cEnigmaConfig::~cEnigmaConfig()
{
}

void cEnigmaConfig::SetLogoDir(const char *logodirP)
{
  if (logodirP) {
    debug("cEnigmaConfig::SetLogoDir(%s)", logodirP);
    strncpy(logoDir, logodirP, sizeof(logoDir));
  }
}

void cEnigmaConfig::SetImagesDir(const char *dir)
{
  if (dir) {
    debug("cEnigmaConfig::SetImagesDir(%s)", dir);
    strncpy(strImagesDir, dir, sizeof(strImagesDir));
  }
}

const char *cEnigmaConfig::GetImageExtension(void)
{
#ifdef HAVE_IMAGEMAGICK
  return (0 <= imageExtension && imageExtension < NUM_IMAGEEXTENSIONTEXTS) ? imageExtensionTexts[imageExtension] : imageExtensionTexts[0];
#else
  return "xpm";
#endif
}

const cFont *cEnigmaConfig::GetFont(int id)
{
  const cFont *res = NULL;
  eDvbFont nVdrId = (eDvbFont)allFonts[id].VdrId;
  if (::Setup.UseSmallFont == 1) { // if "Use small font" == "skin dependent"
    if (nVdrId == FONT_TRUETYPE) {
      if (!isempty(allFonts[id].Name)) {
        res = FontCache.GetFont(allFonts[id].Name, allFonts[id].Size, allFonts[id].Width);
      }
    } else {
      if (nVdrId > FONT_TRUETYPE)
        nVdrId = (eDvbFont)(nVdrId - 1);
    }
  } else {
    if (allFonts[id].Default == fontFix)
      nVdrId = fontFix;
    if (::Setup.UseSmallFont == 2)
      nVdrId = fontSml;
    else
      nVdrId = fontOsd;
  }

  if (res == NULL) {
    switch (nVdrId) {
      case fontOsd: res = FontCache.GetFont(Setup.FontOsd, Setup.FontOsdSize); break;
      case fontSml: res = FontCache.GetFont(Setup.FontSml, Setup.FontSmlSize); break;
      case fontFix: res = FontCache.GetFont(Setup.FontFix, Setup.FontFixSize); break;
    }
  }

  if (res)
    return res;
  else
    return cFont::GetFont(nVdrId);
}

void cEnigmaConfig::SetFont(int id, const char *font)
{
  if (id >= 0 && id < FONT_NUMFONTS && font) {
    const char *tmp = strrchr(font, ':');
    if (tmp) {
      strncpy(allFonts[id].Name, font, std::min((int)sizeof(allFonts[id].Name), (int)(tmp - font)));
      allFonts[id].Size = atoi(tmp + 1);
      tmp = strchr(tmp + 1, ',');
      if (tmp) {
        allFonts[id].Width = atoi(tmp + 1);
      }
    } else {
      strncpy(allFonts[id].Name, font, sizeof(allFonts[id].Name));
    }
  }
}

void cEnigmaConfig::SetFont(int id, int vdrId)
{
  if (id >= 0 && id < FONT_NUMFONTS && vdrId >= 0) {
    allFonts[id].VdrId = vdrId;
  }
}

void cEnigmaConfig::GetOsdSize(struct EnigmaOsdSize *size)
{
  if (size == NULL)
    return;

  size->y = Setup.OSDTop;
  size->x = Setup.OSDLeft;
  size->w = Setup.OSDWidth;
  size->h = Setup.OSDHeight;

  if (dynOsd) {
    size->y = cOsd::OsdTop();
    size->x = cOsd::OsdLeft();
    size->w = cOsd::OsdWidth();
    size->h = cOsd::OsdHeight();
  }

  if (size->w < MINOSDWIDTH)
    size->w = MINOSDWIDTH;
  if (size->h < MINOSDHEIGHT)
    size->h = MINOSDHEIGHT;

  debug("cEnigmaConfig::GetOsdSize() x=%d(%d) y=%d(%d) w=%d(%d) h=%d(%d)", size->x, Setup.OSDLeft, size->y, Setup.OSDTop, size->w, Setup.OSDWidth, size->h, Setup.OSDHeight);
}
// vim:et:sw=2:ts=2:

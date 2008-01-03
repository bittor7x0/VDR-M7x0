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
#include "../avards/avards_services.h"
#endif

#include <stdlib.h>
#include <vdr/font.h>
#include <vdr/plugin.h>

#ifdef HAVE_IMAGEMAGICK
const char *imageExtensionTexts[NUM_IMAGEEXTENSIONTEXTS] = { "xpm", "jpg", "png" };
#endif

cEnigmaConfig EnigmaConfig;

#ifdef HAVE_FREETYPE
# if VDRVERSNUM != 10503
cGraphtftFont FontCache;
# endif
#endif

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
                                 showSymbolsMenu(1), showSymbolsReplay(1), showSymbolsMsgs(1), showSymbolsAudio(1),
                                 showListSymbols(1), showProgressbar(1), cacheSize(50),
                                 useChannelId(0), showInfo(1), showRemaining(0), showMarker(1),
                                 singleArea(1), singleArea8Bpp(1), showFlags(1), numReruns(5),
                                 useSubtitleRerun(1), showTimerConflicts(1), showRecSize(2),
                                 showImages(0), resizeImages(0), showMailIcon(0),
                                 imageWidth(120), imageHeight(80), imageExtension(0), fullTitleWidth(0),
                                 useTextEffects(0), scrollDelay(50), scrollPause(1500), scrollMode(0),
                                 blinkPause(1000), scrollInfo(1), scrollListItem(1), scrollOther(1), scrollTitle(1),
                                 dynOsd(0)
{
  memset(logoDir, 0, sizeof(logoDir));
  memset(strImagesDir, 0, sizeof(strImagesDir));
#ifdef HAVE_FREETYPE
  memset(strFontsDir, 0, sizeof(strFontsDir));
#endif

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

#ifdef HAVE_FREETYPE
void cEnigmaConfig::SetFontsDir(const char *dir)
{
  if (dir) {
    debug("cEnigmaConfig::SetFontsDir(%s)", dir);
    strncpy(strFontsDir, dir, sizeof(strFontsDir));
  }
}
#endif

const cFont *cEnigmaConfig::GetFont(int id, const cFont *pFontCur)
{
  const cFont *res = NULL;
  if (::Setup.UseSmallFont == 1) { // if "Use small font" == "skin dependent"
    if (allFonts[id].VdrId == FONT_TRUETYPE) {
      if (pFontCur) // TTFs can't get patched, so it's always save to return the previous pointer
        return pFontCur;
      if (!isempty(allFonts[id].Name)) {
#ifdef HAVE_FREETYPE
        char *cachename;
        asprintf(&cachename, "%s_%d_%d_%d", allFonts[id].Name, allFonts[id].Size, allFonts[id].Width, Setup.OSDLanguage);
        if (FontCache.Load(string(strFontsDir) + "/" + string(allFonts[id].Name), cachename, allFonts[id].Size, Setup.OSDLanguage, allFonts[id].Width)) {
          res = FontCache.GetFont(cachename);
        } else {
          error("ERROR: EnigmaNG: Couldn't load font %s:%d", allFonts[id].Name, allFonts[id].Size);
        }
        free(cachename);
#else
        error("ERROR: EnigmaNG: Font engine not enabled at compile time!");
#endif
      }
    } else if (allFonts[id].VdrId > FONT_TRUETYPE) {
      res = cFont::GetFont((eDvbFont)(allFonts[id].VdrId - 1));
    } else {
      res = cFont::GetFont((eDvbFont)allFonts[id].VdrId);
    }
  }

  if (res == NULL)
    res = cFont::GetFont((eDvbFont)allFonts[id].Default);

  if (res)
    return res;
  else
    return cFont::GetFont(fontOsd);
}

void cEnigmaConfig::SetFont(int id, const char *font)
{
  if (id >= 0 && id < FONT_NUMFONTS && font) {
    char *tmp = strrchr(font, ':');
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

#if VDRVERSNUM >= 10504
  if (dynOsd) {
    size->y = cOsd::OsdTop();
    size->x = cOsd::OsdLeft();
    size->w = cOsd::OsdWidth();
    size->h = cOsd::OsdHeight();
  }
#else    
# ifdef USE_PLUGIN_AVARDS
  if (dynOsd) {
    cPlugin *p = cPluginManager::GetPlugin("avards");
    if (p) {
      avards_MaxOSDsize_v1_0 OSDsize;
      if (p->Service(AVARDS_MAXOSDSIZE_SERVICE_STRING_ID, &OSDsize)) {
        size->y = OSDsize.top;
        size->x = OSDsize.left;
        size->w = OSDsize.width;
        size->h = OSDsize.height;
      }
    }
  }
# endif
#endif //VDRVERSNUM >= 10504
}
// vim:et:sw=2:ts=2:

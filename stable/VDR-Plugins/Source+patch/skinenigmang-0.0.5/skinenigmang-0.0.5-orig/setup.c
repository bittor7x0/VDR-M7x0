/*
 * setup.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "config.h"
#include "i18n.h"
#include "logo.h"
#include "setup.h"
#include "tools.h"

#ifdef HAVE_FREETYPE
#include "texteffects.h"
#endif

static const char *allVdrFonts[] = {
#ifdef HAVE_FREETYPE
  "TrueType Font",
#else
  "No TTF support!",
#endif
  "Default OSD Font",
  "Default Fixed Size Font",
  "Default Small Font",
  SKINENIGMA_FONTS
};

// cPluginSkinEnigmaSetup
cPluginSkinEnigmaSetup::cPluginSkinEnigmaSetup(void)
{
  // create setup menu
  debug("cPluginSkinEnigmaSetup()");

  data = EnigmaConfig;
  Setup();
}

void cPluginSkinEnigmaSetup::AddCategory(const char *Title) {
  char *buffer = NULL;

  asprintf(&buffer, "---\t%s ----------------------------------------------------------------------------------", Title );

  cOsdItem *item = new cOsdItem(buffer);
  free(buffer);

  if (item) {
    item->SetSelectable(false);
    Add(item);
  }
}

void cPluginSkinEnigmaSetup::Setup(void)
{
  // update setup display
  int current = Current();

  Clear();

  Add(new cOsdItem(tr("General")));
  Add(new cOsdItem(tr("Logos & Symbols")));
#ifndef DISABLE_ANIMATED_TEXT
  Add(new cOsdItem(tr("Animated Text")));
#endif
  if (::Setup.UseSmallFont == 1) { // only if "Use small font" = "skin dependent"
    Add(new cOsdItem(tr("Fonts")));
  }
#ifdef SKINENIGMA_HAVE_EPGSEARCH
  Add(new cOsdItem(tr("EPGSearch")));
#endif

  SetCurrent(Get(current));
  Display();
  SetHelp(NULL, NULL, NULL, tr("Button$Open"));
}

void cPluginSkinEnigmaSetup::Store(void)
{
  // store setup data
  debug("cPluginSkinEnigmaSetup::Store()");

  EnigmaConfig = data;
  SetupStore("SingleArea8Bpp", EnigmaConfig.singleArea8Bpp);
  SetupStore("ShowAuxInfo", EnigmaConfig.showAuxInfo);
  SetupStore("ShowRemaining", EnigmaConfig.showRemaining);
  SetupStore("ShowProgressBar", EnigmaConfig.showProgressbar);
  SetupStore("ShowListSymbols", EnigmaConfig.showListSymbols);
  SetupStore("ShowSymbols", EnigmaConfig.showSymbols);
  SetupStore("ShowSymbolsMenu", EnigmaConfig.showSymbolsMenu);
  SetupStore("ShowSymbolsReplay", EnigmaConfig.showSymbolsReplay);
  SetupStore("ShowSymbolsMsgs", EnigmaConfig.showSymbolsMsgs);
  SetupStore("ShowSymbolsAudio", EnigmaConfig.showSymbolsAudio);
  SetupStore("ShowLogo", EnigmaConfig.showLogo);
  SetupStore("ShowInfo", EnigmaConfig.showInfo);
  SetupStore("ShowVPS", EnigmaConfig.showVps);
  SetupStore("ShowFlags", EnigmaConfig.showFlags);
  SetupStore("ShowMarker", EnigmaConfig.showMarker);
  SetupStore("CacheSize", EnigmaConfig.cacheSize);
  SetupStore("UseChannelId", EnigmaConfig.useChannelId);
  SetupStore("NumReruns", EnigmaConfig.numReruns);
  SetupStore("UseSubtitleRerun", EnigmaConfig.useSubtitleRerun);
  SetupStore("ShowTimerConflicts", EnigmaConfig.showTimerConflicts);
  SetupStore("ShowRecSize", EnigmaConfig.showRecSize);
  SetupStore("ShowImages", EnigmaConfig.showImages);
  SetupStore("ResizeImages", EnigmaConfig.resizeImages);
  SetupStore("ShowMailIcon", EnigmaConfig.showMailIcon);
  SetupStore("ImageWidth", EnigmaConfig.imageWidth);
  SetupStore("ImageHeight", EnigmaConfig.imageHeight);
  SetupStore("ImageExtension", EnigmaConfig.imageExtension);
  SetupStore("FullTitleWidth", EnigmaConfig.fullTitleWidth);
  SetupStore("UseTextEffects", EnigmaConfig.useTextEffects);
  SetupStore("ScrollDelay", EnigmaConfig.scrollDelay);
  SetupStore("ScrollPause", EnigmaConfig.scrollPause);
  SetupStore("ScrollMode", EnigmaConfig.scrollMode);
  SetupStore("BlinkPause", EnigmaConfig.blinkPause);
  SetupStore("ScrollInfo", EnigmaConfig.scrollInfo);
  SetupStore("ScrollListItem", EnigmaConfig.scrollListItem);
  SetupStore("ScrollOther", EnigmaConfig.scrollOther);
  SetupStore("ScrollTitle", EnigmaConfig.scrollTitle);
  SetupStore("DynOSD", EnigmaConfig.dynOsd);

  char tmp[sizeof(EnigmaConfig.allFonts[0].Name) + 8];
  for (int id = 0; id < FONT_NUMFONTS; id++) {
    SetupStore(allFontConfig[id].KeyId, EnigmaConfig.allFonts[id].VdrId);
    if (EnigmaConfig.allFonts[id].Name[0] != 0) {
      sprintf(tmp, "%s:%d,%d", EnigmaConfig.allFonts[id].Name, EnigmaConfig.allFonts[id].Size, EnigmaConfig.allFonts[id].Width);
      SetupStore(allFontConfig[id].KeyName, tmp);
    }
  }

  // resize logo cache
  EnigmaLogoCache.Resize(EnigmaConfig.cacheSize);
}

eOSState cPluginSkinEnigmaSetup::ProcessKey(eKeys Key)
{
  bool hadSubMenu = HasSubMenu();
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (hadSubMenu && Key == kOk)
    Store();

  if (!HasSubMenu() && (state == osUnknown || Key == kOk)) {
    if ((Key == kOk && !hadSubMenu) || Key == kBlue) {
      const char* ItemText = Get(Current())->Text();
      if (strcmp(ItemText, tr("General")) == 0)
        state = AddSubMenu(new cMenuSetupGeneral(&data));
      else if (strcmp(ItemText, tr("Logos & Symbols")) == 0)
        state = AddSubMenu(new cMenuSetupLogos(&data));
#ifndef DISABLE_ANIMATED_TEXT
      else if (strcmp(ItemText, tr("Animated Text")) == 0)
        state = AddSubMenu(new cMenuSetupAnimText(&data));
#endif
      else if (strcmp(ItemText, tr("Fonts")) == 0)
        state = AddSubMenu(new cMenuSetupFonts(&data));
#ifdef SKINENIGMA_HAVE_EPGSEARCH
      else if (strcmp(ItemText, tr("EPGSearch")) == 0)
        state = AddSubMenu(new cMenuSetupEpgSearch(&data));
#endif
    }
  }

  return state;
}

// Setup: SubMenu
cMenuSetupSubMenu::cMenuSetupSubMenu(const char* Title, cEnigmaConfig* Data) : cOsdMenu(Title, 30)
{
  data = Data;
}

eOSState cMenuSetupSubMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        return osBack;
      default:
        break;
    }
  }

  return state;
}

// Setup: General
cMenuSetupGeneral::cMenuSetupGeneral(cEnigmaConfig* Data) : cMenuSetupSubMenu(tr("General"), Data)
{
  showRemainingTexts[0] = tr("elapsed");
  showRemainingTexts[1] = tr("remaining");
  showRemainingTexts[2] = tr("percent");

  showRecSizeTexts[0] = tr("never");
  showRecSizeTexts[1] = tr("use size.vdr only");
  showRecSizeTexts[2] = tr("always");

  Set();
}

eOSState cMenuSetupGeneral::ProcessKey(eKeys Key)
{
  return cMenuSetupSubMenu::ProcessKey(Key);
}

void cMenuSetupGeneral::Set(void)
{
  int current = Current();
  Clear();

  Add(new cMenuEditBoolItem(tr("Try 8bpp single area"), &data->singleArea8Bpp));
  Add(new cMenuEditBoolItem(tr("Show info area in main menu"), &data->showInfo));
  Add(new cMenuEditBoolItem(tr("Show auxiliary information"), &data->showAuxInfo, tr("top"), tr("bottom")));
  Add(new cMenuEditStraItem(tr("Show recording's size"), &data->showRecSize, 3, showRecSizeTexts));
  Add(new cMenuEditBoolItem(tr("Full title width"), &data->fullTitleWidth));
  Add(new cMenuEditStraItem(tr("Show remaining/elapsed time"), &data->showRemaining, 3, showRemainingTexts));
  Add(new cMenuEditBoolItem(tr("Show VPS"), &data->showVps));
  Add(new cMenuEditBoolItem(tr("Show progressbar"), &data->showProgressbar));
#ifdef USE_PLUGIN_AVARDS
  Add(new cMenuEditBoolItem(tr("Dynamic OSD size"), &data->dynOsd));
#endif

  SetCurrent(Get(current));
  Display();
  SetHelp(NULL, NULL, NULL, NULL);
}

// Setup: Logos & Symbols
cMenuSetupLogos::cMenuSetupLogos(cEnigmaConfig* Data) : cMenuSetupSubMenu(tr("Logos & Symbols"), Data)
{
#ifndef SKINENIGMA_NO_MENULOGO
  resizeImagesTexts[0] = tr("pixel algo");
  resizeImagesTexts[1] = tr("ratio algo");
  resizeImagesTexts[2] = tr("zoom image");
#endif

#ifdef USE_PLUGIN_MAILBOX
  showMailIconTexts[0] = tr("never");
  showMailIconTexts[1] = tr("only if new mail present");
  showMailIconTexts[2] = tr("always");
#endif

  Set();
}

eOSState cMenuSetupLogos::ProcessKey(eKeys Key)
{
  int oldShowLogo = data->showLogo;
  int oldShowSymbols = data->showSymbols;
  int oldShowSymbolsMenu = data->showSymbolsMenu;
  int oldShowImages = data->showImages;

  eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

  if ((state == osUnknown) && (Key == kRed)) {
    Skins.Message(mtInfo, tr("Flushing channel logo cache..."));
    EnigmaLogoCache.Flush();
    Skins.Message(mtInfo, NULL);
    state = osContinue;
  }

  if (Key != kNone &&
      ((data->showLogo != oldShowLogo) 
       || (data->showSymbols != oldShowSymbols) 
       || (data->showSymbolsMenu != oldShowSymbolsMenu) 
       || (data->showImages != oldShowImages)
      )) {
    Set();
  }

  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        return osBack;
      default:
        break;
    }
  }

  return state;
}

void cMenuSetupLogos::Set(void)
{
  int current = Current();
  Clear();

  Add(new cMenuEditBoolItem(tr("Show symbols"), &data->showSymbols));    //TODO? symbols -> icons

  if (data->showSymbols) {
#ifndef SKINENIGMA_NO_MENULOGO
    Add(new cMenuEditBoolItem(tr("  Show symbols in menu"), &data->showSymbolsMenu));
#endif
    Add(new cMenuEditBoolItem(tr("  Show symbols in replay"), &data->showSymbolsReplay));
    Add(new cMenuEditBoolItem(tr("  Show symbols in messages"), &data->showSymbolsMsgs));
    Add(new cMenuEditBoolItem(tr("  Show symbols in audio"), &data->showSymbolsAudio));
  }

  Add(new cMenuEditBoolItem(tr("Show symbols in lists"), &data->showListSymbols));
  Add(new cMenuEditBoolItem(tr("Show marker in lists"), &data->showMarker));
  Add(new cMenuEditBoolItem(tr("Show flags"), &data->showFlags));

#ifdef USE_PLUGIN_MAILBOX
  cPlugin *MailBoxPlugin = cPluginManager::GetPlugin("mailbox");
  if (MailBoxPlugin && MailBoxPlugin->Service("MailBox-HasNewMail-1.0", 0)) {
    Add(new cMenuEditStraItem(tr("Show mail icon"), &data->showMailIcon, 3, showMailIconTexts));
  }
#endif

#ifndef SKINENIGMA_NO_MENULOGO
  if (data->showSymbols && data->showSymbolsMenu) {
    Add(new cMenuEditBoolItem(tr("Show event/recording images"), &data->showImages));
    if (data->showImages) {
#ifdef HAVE_IMAGEMAGICK
      Add(new cMenuEditStraItem(tr("  Resize images"), &data->resizeImages, 3, resizeImagesTexts));
#endif
      Add(new cMenuEditIntItem(tr("  Image width" ), &data->imageWidth, 80, 180));
     Add(new cMenuEditIntItem(tr("  Image height" ), &data->imageHeight, 80, 144));
#ifdef HAVE_IMAGEMAGICK
     Add(new cMenuEditStraItem(tr("  Image format"), &data->imageExtension, 3, imageExtensionTexts));
#endif
    }
  }
#endif //SKINENIGMA_NO_MENULOGO

  Add(new cMenuEditBoolItem(tr("Show channel logos"), &data->showLogo));
  if (data->showLogo) {
    Add(new cMenuEditBoolItem(tr("  Identify channel by"), &data->useChannelId, tr("name"), tr("data")));
  }
  if (data->showLogo || data->showSymbols) {
    Add(new cMenuEditIntItem(tr("Channel logo cache size"), &data->cacheSize, 0, 1000));
  }

  SetCurrent(Get(current));
  Display();
  SetHelp(tr("Button$Flush cache"), NULL, NULL, NULL);
}

#ifndef DISABLE_ANIMATED_TEXT
// Setup: Animated Text
cMenuSetupAnimText::cMenuSetupAnimText(cEnigmaConfig* Data) : cMenuSetupSubMenu(tr("Animated Text"), Data)
{
  scrollModeTexts[0] = tr("left and right");
  scrollModeTexts[1] = tr("to the left");

  Set();
}

eOSState cMenuSetupAnimText::ProcessKey(eKeys Key)
{
  int oldUseTextEffects = data->useTextEffects;

  eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

  if (Key != kNone && oldUseTextEffects != data->useTextEffects)
    Set();

  return state;
}

void cMenuSetupAnimText::Set(void)
{
  int current = Current();
  Clear();

  Add(new cMenuEditBoolItem(tr("Enable"), &data->useTextEffects));
  if (data->useTextEffects) {
    Add(new cMenuEditBoolItem(tr("  Scroll OSD title"), &data->scrollTitle));
    Add(new cMenuEditBoolItem(tr("  Scroll info area"), &data->scrollInfo));
    Add(new cMenuEditBoolItem(tr("  Scroll active list items"), &data->scrollListItem));
    Add(new cMenuEditBoolItem(tr("  Scroll other items"), &data->scrollOther));
    Add(new cMenuEditStraItem(tr("  Scoll behaviour"), &data->scrollMode, 2, scrollModeTexts));
    Add(new cMenuEditIntItem(tr("  Scroll delay (ms)"), &data->scrollDelay, 50, 1000));
    Add(new cMenuEditIntItem(tr("  Scroll pause (ms)"), &data->scrollPause, 500, 2000));
    Add(new cMenuEditIntItem(tr("  Blink pause (ms)"), &data->blinkPause, 500, 2000));
  }

  SetCurrent(Get(current));
  Display();
  SetHelp(NULL, NULL, NULL, NULL);
}
#endif

#ifdef HAVE_FREETYPE
// Setup: TTF
cMenuSetupTTF::cMenuSetupTTF(FontInfo* Data) : cOsdMenu(tr("TrueType Font"), 10)
{
  data = Data;
  availTTFs = EnigmaTextEffects.GetAvailTTFs();
  if (availTTFs && data) {
    nMaxTTFs = EnigmaTextEffects.GetNumAvailTTFs();
    nFont = 0;
    for (int i = 0; i < nMaxTTFs; i++) {
      if (availTTFs[i]) {
        if (strcmp(availTTFs[i], data->Name) == 0) {
          nFont = i;
          break;
        }
      }
    }
    nWidth = data->Width;
    nSize = data->Size;
  }
  SetHelp(NULL, NULL, NULL, NULL);
  Set();
}

void cMenuSetupTTF::Set(void)
{
  int current = Current();
  Clear();

  if (availTTFs) {
    Add(new cMenuEditStraItem(tr("Name"), &nFont, nMaxTTFs, availTTFs));
    Add(new cMenuEditIntItem(tr("Size"), &nSize, 10, MAXFONTSIZE));
#if VDRVERSNUM < 10503 || VDRVERSNUM >= 10505
    //VDR >1.5.2 cant's set TTF width
    Add(new cMenuEditIntItem(tr("Width"), &nWidth, 80, 120));
#endif
    SetCurrent(Get(current));
  } else {
    cOsdItem *item = new cOsdItem(tr("No TrueType fonts installed!"));

    if (item) {
      item->SetSelectable(false);
      Add(item);
    }
  }

  Display();
}

eOSState cMenuSetupTTF::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
      case kOk:
        Store();
        state = osBack;
        break;
      default:
        break;
    }
  }

  return state;
}

void cMenuSetupTTF::Store(void)
{
  if (data && availTTFs) {
    strncpy(data->Name, availTTFs[nFont], sizeof(data->Name));
    data->Width = nWidth;
    data->Size = nSize;
  }
}
#endif

// Setup: Fonts
cMenuSetupFonts::cMenuSetupFonts(cEnigmaConfig* Data) : cMenuSetupSubMenu(tr("Fonts"), Data)
{
#ifdef HAVE_FREETYPE
  allVdrFonts[0] = tr("TrueType Font");
#else
  allVdrFonts[0] = tr("No TTF support!");
#endif
  allVdrFonts[1] = tr("Default OSD Font");
  allVdrFonts[2] = tr("Default Fixed Size Font");
  allVdrFonts[3] = tr("Default Small Font");

  Set();
}

cMenuSetupFonts::~cMenuSetupFonts(void)
{
}

eOSState cMenuSetupFonts::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

#ifdef HAVE_FREETYPE
  if (state == osUnknown && Key == kBlue && data->allFonts[Current()].VdrId == FONT_TRUETYPE) {
    state = AddSubMenu(new cMenuSetupTTF(&data->allFonts[Current()]));
  } else {
    if (!HasSubMenu() && data->allFonts[Current()].VdrId == FONT_TRUETYPE)
      SetHelp(NULL, NULL, NULL, tr("Button$Set"));
    else
      SetHelp(NULL, NULL, NULL, NULL);
  }
#endif

  return state;
}

void cMenuSetupFonts::Set(void)
{
  int current = Current();
  Clear();

  int numAvailFonts = sizeof(allVdrFonts)/sizeof(char*);
  Add(new cMenuEditStraItem(tr("OSD title"), &data->allFonts[FONT_OSDTITLE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Messages"), &data->allFonts[FONT_MESSAGE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Date"), &data->allFonts[FONT_DATE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Help keys"), &data->allFonts[FONT_HELPKEYS].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Channelinfo: title"), &data->allFonts[FONT_CITITLE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Channelinfo: subtitle"), &data->allFonts[FONT_CISUBTITLE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Channelinfo: language"), &data->allFonts[FONT_CILANGUAGE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("List items"), &data->allFonts[FONT_LISTITEM].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Info area: timers title"), &data->allFonts[FONT_INFOTIMERHEADLINE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Info area: timers text"), &data->allFonts[FONT_INFOTIMERTEXT].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Info area: warning title"), &data->allFonts[FONT_INFOWARNHEADLINE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Info area: warning text"), &data->allFonts[FONT_INFOWARNTEXT].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Details: title"), &data->allFonts[FONT_DETAILSTITLE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Details: subtitle"), &data->allFonts[FONT_DETAILSSUBTITLE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Details: date"), &data->allFonts[FONT_DETAILSDATE].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Details: text"), &data->allFonts[FONT_DETAILSTEXT].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Replay: times"), &data->allFonts[FONT_REPLAYTIMES].VdrId, numAvailFonts, allVdrFonts));
  Add(new cMenuEditStraItem(tr("Fixed Font"), &data->allFonts[FONT_FIXED].VdrId, numAvailFonts, allVdrFonts));

  SetCurrent(Get(current));
  Display();
#ifdef HAVE_FREETYPE
  if (data->allFonts[Current()].VdrId == FONT_TRUETYPE)
    SetHelp(NULL, NULL, NULL, tr("Button$Set"));
  else
#endif
    SetHelp(NULL, NULL, NULL, NULL);
}

#ifdef SKINENIGMA_HAVE_EPGSEARCH
// Setup: EpgSearch
cMenuSetupEpgSearch::cMenuSetupEpgSearch(cEnigmaConfig* Data) : cMenuSetupSubMenu(tr("EPGSearch"), Data)
{
  useSubtitleRerunTexts[0] = tr("never");
  useSubtitleRerunTexts[1] = tr("if exists");
  useSubtitleRerunTexts[2] = tr("always");

  Set();
}

eOSState cMenuSetupEpgSearch::ProcessKey(eKeys Key)
{
  return cMenuSetupSubMenu::ProcessKey(Key);
}

void cMenuSetupEpgSearch::Set(void)
{
  int current = Current();
  Clear();

  Add(new cMenuEditIntItem(tr("Number of Reruns"), &data->numReruns, 0, 10));
  Add(new cMenuEditStraItem(tr("Use Subtitle for reruns"), &data->useSubtitleRerun, 3, useSubtitleRerunTexts));
  if (data->showInfo) {
    Add(new cMenuEditBoolItem(tr("Show timer conflicts"), &data->showTimerConflicts));
  }

  SetCurrent(Get(current));
  Display();
  SetHelp(NULL, NULL, NULL, NULL);
}
#endif
// vim:et:sw=2:ts=2:

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

#ifdef USE_PLUGIN_EPGSEARCH
#include "services/epgsearch.h"
#endif

#ifdef USE_PLUGIN_AVARDS
#include "services/avards.h"
#endif

#ifdef USE_PLUGIN_MAILBOX
#include "services/mailbox.h"
#endif

#include "texteffects.h"

static const char *allVdrFonts[] = {
#ifdef HAVE_FREETYPE
  trNOOP("TrueType Font"),
#else
  trNOOP("No TTF support!"),
#endif
  trNOOP("Default OSD Font"),
  trNOOP("Default Fixed Size Font"),
  trNOOP("Default Small Font")
};

// cPluginSkinEnigmaSetup
cPluginSkinEnigmaSetup::cPluginSkinEnigmaSetup(void)
{
  // create setup menu
  debug("cPluginSkinEnigmaSetup()");

  data = EnigmaConfig;
  Setup();
}

void cMenuSetupSubMenu::AddCategory(const char *Title) {
  char *buffer = NULL;

  if (-1 != asprintf(&buffer, "--- %s ---", Title )) {

    cOsdItem *item = new cOsdItem(buffer);
    free(buffer);

    if (item) {
      item->SetSelectable(false);
      Add(item);
    }
  }
}

void cPluginSkinEnigmaSetup::Setup(void)
{
  // update setup display
  int currentItem = Current();

  Clear();

  Add(new cOsdItem(tr("General")));
  Add(new cOsdItem(tr("Logos & Symbols")));
  Add(new cOsdItem(tr("Animated Text")));
  if (::Setup.UseSmallFont == 1) { // only if "Use small font" = "skin dependent"
    Add(new cOsdItem(tr("Fonts")));
  }
#ifdef USE_PLUGIN_EPGSEARCH
  if (cPluginManager::GetPlugin("epgsearch") != NULL) {
    Add(new cOsdItem(tr("EPGSearch")));
  } //TODO? else display "EPGsearch plugin not found"
#endif

  SetCurrent(Get(currentItem));
  Display();
  SetHelp(NULL, NULL, NULL, trVDR("Button$Open"));
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
  SetupStore("ShowColSymbolsDetails", EnigmaConfig.showColSymbolsDetails);
  SetupStore("ShowLogo", EnigmaConfig.showLogo);
  SetupStore("ShowInfo", EnigmaConfig.showInfo);
  SetupStore("MinInfoWidth", EnigmaConfig.minInfoWidth);
  SetupStore("ShowVPS", EnigmaConfig.showVps);
  SetupStore("ShowFlags", EnigmaConfig.showFlags);
  SetupStore("ShowCatGenCon", EnigmaConfig.showCatGenCon);
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
  SetupStore("StatusLineMode", EnigmaConfig.statusLineMode);
  SetupStore("ShowWssSymbols", EnigmaConfig.showWssSymbols);
  SetupStore("ShowStatusSymbols", EnigmaConfig.showStatusSymbols);
  SetupStore("ShowScrollbar", EnigmaConfig.showScrollbar);
  SetupStore("ShowSignalInfo", EnigmaConfig.showSignalInfo);
  SetupStore("ShowCaMode", EnigmaConfig.showCaMode);
  SetupStore("DrawRoundCorners", EnigmaConfig.drawRoundCorners);
  SetupStore("ChannelLogoWidth", EnigmaConfig.channelLogoWidth);
  SetupStore("ChannelLogoHeight", EnigmaConfig.channelLogoHeight);
  SetupStore("SignalInfoWidth", EnigmaConfig.signalInfoWidth);
  SetupStore("ProgressBarWidth", EnigmaConfig.progressBarWidth);

  char tmp[sizeof(EnigmaConfig.allFonts[0].Name) + 8];
  for (int id = 0; id < FONT_NUMFONTS; id++) {
    SetupStore(allFontConfig[id].KeyId, EnigmaConfig.allFonts[id].VdrId);
    if (EnigmaConfig.allFonts[id].Name[0] != 0) {
      snprintf(tmp, sizeof(tmp), "%s:%d,%d", EnigmaConfig.allFonts[id].Name, EnigmaConfig.allFonts[id].Size, EnigmaConfig.allFonts[id].Width);
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
      else if (strcmp(ItemText, tr("Animated Text")) == 0)
        state = AddSubMenu(new cMenuSetupAnimText(&data));
      else if (strcmp(ItemText, tr("Fonts")) == 0)
        state = AddSubMenu(new cMenuSetupFonts(&data));
#ifdef USE_PLUGIN_EPGSEARCH
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
  showRemainingTexts[3] = tr("elapsed/remaining");

  showRecSizeTexts[0] = tr("never");
  showRecSizeTexts[1] = tr("use size.vdr only");
  showRecSizeTexts[2] = tr("always");

  statusLineModeTexts[0] = tr("Last line");
  statusLineModeTexts[1] = tr("Help buttons");
  statusLineModeTexts[2] = tr("Free last line");

  showScrollbarTexts[0] = trVDR("no");
  showScrollbarTexts[1] = trVDR("yes");
  showScrollbarTexts[2] = tr("if required");
  Set();
}

eOSState cMenuSetupGeneral::ProcessKey(eKeys Key)
{
  int oldShowInfo = data->showInfo;
#ifndef DISABLE_SIGNALINFO
  int oldShowSignalInfo = data->showSignalInfo;
#endif

  eOSState state = cMenuSetupSubMenu::ProcessKey(Key);

  if (Key != kNone &&
      ((data->showInfo != oldShowInfo)
#ifndef DISABLE_SIGNALINFO
       || (data->showSignalInfo != oldShowSignalInfo)
#endif
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

void cMenuSetupGeneral::Set(void)
{
  int currentItem = Current();
  Clear();
#ifndef VDRO7OVERSION
  Add(new cMenuEditBoolItem(tr("Try 8bpp single area"), &data->singleArea8Bpp));
#endif
  Add(new cMenuEditBoolItem(tr("Round corners"), &data->drawRoundCorners));
  Add(new cMenuEditBoolItem(tr("Full title width"), &data->fullTitleWidth)); // channel info / replay / tracks
  Add(new cMenuEditBoolItem(tr("Show VPS"), &data->showVps)); // channel info / epg details
#if VDRVERSNUM >= 10711
  Add(new cMenuEditBoolItem(tr("Dynamic OSD size"), &data->dynOsd));
#endif
  AddCategory(tr("Menu OSD"));
  Add(new cMenuEditBoolItem(tr("Show info area in main menu"), &data->showInfo));
  if (data->showInfo)
    Add(new cMenuEditIntItem(tr("  Min width of info area"), &data->minInfoWidth, 80, Setup.OSDWidth));
  Add(new cMenuEditStraItem(tr("Show messages in menu on"), &data->statusLineMode, 3, statusLineModeTexts));
  Add(new cMenuEditStraItem(tr("Show scrollbar in menu"), &data->showScrollbar, 3, showScrollbarTexts));
  AddCategory(tr("EPG & Recording Details OSD"));
  Add(new cMenuEditBoolItem(tr("Show auxiliary information"), &data->showAuxInfo, trVDR("top"), trVDR("bottom")));
  Add(new cMenuEditStraItem(tr("Show recording's size"), &data->showRecSize, 3, showRecSizeTexts));
  AddCategory(tr("Channel Info OSD"));
  Add(new cMenuEditStraItem(tr("Show remaining/elapsed time"), &data->showRemaining, 4, showRemainingTexts));
  Add(new cMenuEditIntItem(tr("Width of progress bar"), &data->progressBarWidth, 74, Setup.OSDWidth));
#ifndef DISABLE_SIGNALINFO
  Add(new cMenuEditBoolItem(tr("Show signal info"), &data->showSignalInfo));
  if (data->showSignalInfo)
    Add(new cMenuEditIntItem(tr("  Width of signal info"), &data->signalInfoWidth, 74, Setup.OSDWidth));
#endif //DISABLE_SIGNALINFO
  Add(new cMenuEditBoolItem(tr("Show CA system as text"), &data->showCaMode));
  Add(new cMenuEditBoolItem(tr("Show Category - Genre / Contents"), &data->showCatGenCon));

  SetCurrent(Get(currentItem));
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

  showStatusSymbolsTexts[0] = trVDR("no");
  showStatusSymbolsTexts[1] = trVDR("yes");
  showStatusSymbolsTexts[2] = tr("active only");

  Set();
}

eOSState cMenuSetupLogos::ProcessKey(eKeys Key)
{
  int oldShowLogo = data->showLogo;
  int oldShowSymbols = data->showSymbols;
  int oldShowStatusSymbols = data->showStatusSymbols;
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
       || (data->showStatusSymbols != oldShowStatusSymbols) 
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
  int currentItem = Current();
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

  Add(new cMenuEditBoolItem(tr("Colored status symbols in EPG details"), &data->showColSymbolsDetails));
  Add(new cMenuEditBoolItem(tr("Show symbols in lists"), &data->showListSymbols));
  Add(new cMenuEditBoolItem(tr("Show progressbar in lists"), &data->showProgressbar));
  Add(new cMenuEditBoolItem(tr("Show marker in lists"), &data->showMarker));
  Add(new cMenuEditStraItem(tr("Show status symbols"), &data->showStatusSymbols, 3, showStatusSymbolsTexts));
  if (data->showStatusSymbols) {
    Add(new cMenuEditBoolItem(tr("  Show flags"), &data->showFlags));
#ifdef USE_PLUGIN_AVARDS
    cPlugin *pAvardsPlugin = cPluginManager::GetPlugin("avards");
    if (pAvardsPlugin != NULL) {
      if (pAvardsPlugin->Service(AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID)) {
        Add(new cMenuEditBoolItem(tr("  Show WSS mode symbols"), &data->showWssSymbols));
      } else {
        Add(new cOsdItem(AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID " service not found!", osUnknown, false));
      }
    }
#endif

#ifdef USE_PLUGIN_MAILBOX
    cPlugin *MailBoxPlugin = cPluginManager::GetPlugin("mailbox");
    if (MailBoxPlugin) {
      if (MailBoxPlugin->Service(MailBox_HasNewMail_v1_0_NAME, 0)) {
        Add(new cMenuEditStraItem(tr("  Show mail icon"), &data->showMailIcon, 3, showMailIconTexts));
      } else {
        Add(new cOsdItem(MailBox_HasNewMail_v1_0_NAME " service not found!", osUnknown, false));
      }
    } //TODO? else display "Plugin not found"
#endif
  }

#ifndef SKINENIGMA_NO_MENULOGO
  if (data->showSymbols && data->showSymbolsMenu) {
    Add(new cMenuEditBoolItem(tr("Show event/recording images"), &data->showImages));
    if (data->showImages) {
#ifdef HAVE_IMAGEMAGICK
      Add(new cMenuEditStraItem(tr("  Resize images"), &data->resizeImages, 3, resizeImagesTexts));
#endif
      Add(new cMenuEditIntItem(tr("  Image width" ), &data->imageWidth, 80, 360));
      Add(new cMenuEditIntItem(tr("  Image height" ), &data->imageHeight, 80, 288));
#ifdef HAVE_IMAGEMAGICK
      Add(new cMenuEditStraItem(tr("  Image format"), &data->imageExtension, 3, imageExtensionTexts));
#endif
    }
  }
#endif //SKINENIGMA_NO_MENULOGO

  Add(new cMenuEditBoolItem(tr("Show channel logos"), &data->showLogo));
  if (data->showLogo) {
    Add(new cMenuEditIntItem(tr("  Image width" ), &data->channelLogoWidth, 64, 360));
    Add(new cMenuEditIntItem(tr("  Image height" ), &data->channelLogoHeight, 48, 288));
    Add(new cMenuEditBoolItem(tr("  Identify channel by"), &data->useChannelId, tr("name"), tr("data")));
  }
  if (data->showLogo || data->showSymbols) {
    Add(new cMenuEditIntItem(tr("Channel logo cache size"), &data->cacheSize, 0, 4000));
  }

  SetCurrent(Get(currentItem));
  Display();
  SetHelp(tr("Button$Flush cache"), NULL, NULL, NULL);
}

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
  int currentItem = Current();
  Clear();

  Add(new cMenuEditBoolItem(tr("Enable"), &data->useTextEffects));
  if (data->useTextEffects) {
    Add(new cMenuEditBoolItem(tr("  Scroll OSD title"), &data->scrollTitle));
    Add(new cMenuEditBoolItem(tr("  Scroll info area"), &data->scrollInfo));
    Add(new cMenuEditBoolItem(tr("  Scroll active list items"), &data->scrollListItem));
    Add(new cMenuEditBoolItem(tr("  Scroll other items"), &data->scrollOther));
    Add(new cMenuEditStraItem(tr("  Scroll behaviour"), &data->scrollMode, 2, scrollModeTexts));
    Add(new cMenuEditIntItem(tr("  Scroll delay (ms)"), &data->scrollDelay, 3, 1000));
    Add(new cMenuEditIntItem(tr("  Scroll pause (ms)"), &data->scrollPause, 500, 2000));
    Add(new cMenuEditIntItem(tr("  Blink pause (ms)"), &data->blinkPause, 500, 2000));
  }

  SetCurrent(Get(currentItem));
  Display();
  SetHelp(NULL, NULL, NULL, NULL);
}

#ifdef HAVE_FREETYPE
// Setup: TTF
cMenuSetupTTF::cMenuSetupTTF(FontInfo* Data, cStringList* fonts) : cOsdMenu(tr("TrueType Font"), 10)
{
  data = Data;
  if (data && fonts) {
    fontList = fonts;
    nFont = std::max(0, fontList->Find(data->Name));
    nWidth = data->Width;
    nSize = data->Size;
  }

  SetHelp(NULL, NULL, NULL, NULL);
  Set();
}

void cMenuSetupTTF::Set(void)
{
  int currentItem = Current();
  Clear();

  if (fontList->Size() > 0) {
    Add(new cMenuEditStraItem(tr("Name"), &nFont, fontList->Size(), &(*fontList)[0]));
    Add(new cMenuEditIntItem(tr("Size"), &nSize, 10, MAXFONTSIZE));
    Add(new cMenuEditIntItem(tr("Width"), &nWidth, 50, 150));
    SetCurrent(Get(currentItem));
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
  if (data) {
    Utf8Strn0Cpy(data->Name, (*fontList)[nFont], sizeof(data->Name));
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

#ifdef HAVE_FREETYPE
  cFont::GetAvailableFontNames(&fontMonoNames, true);
  cFont::GetAvailableFontNames(&fontNames);
  fontNames.Insert(strdup(DefaultFontSml));
  fontNames.Insert(strdup(DefaultFontOsd));
  fontMonoNames.Insert(strdup(DefaultFontFix));
#endif

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
    state = AddSubMenu(new cMenuSetupTTF(&data->allFonts[Current()], strncmp(Get(Current())->Text(), tr("Fixed Font"), strlen(tr("Fixed Font"))) == 0 ? &fontMonoNames : &fontNames));
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
  int currentItem = Current();
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

  SetCurrent(Get(currentItem));
  Display();
#ifdef HAVE_FREETYPE
  if (data->allFonts[Current()].VdrId == FONT_TRUETYPE)
    SetHelp(NULL, NULL, NULL, tr("Button$Set"));
  else
#endif
    SetHelp(NULL, NULL, NULL, NULL);
}

#ifdef USE_PLUGIN_EPGSEARCH
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
  int currentItem = Current();
  Clear();

  cPlugin *pEPGsearchPlugin = cPluginManager::GetPlugin("epgsearch");
  if (pEPGsearchPlugin != NULL) {
    if (pEPGsearchPlugin->Service(EPGSEARCH_SEARCHRESULTS_SERVICE_STRING_ID)) {
      Add(new cMenuEditIntItem(tr("Number of Reruns"), &data->numReruns, 0, 10));
      Add(new cMenuEditStraItem(tr("Use Subtitle for reruns"), &data->useSubtitleRerun, 3, useSubtitleRerunTexts));
    } else {
      Add(new cOsdItem(EPGSEARCH_SEARCHRESULTS_SERVICE_STRING_ID " service not found!", osUnknown, false));
    }
    if (data->showInfo) {
      if (pEPGsearchPlugin->Service(EPGSEARCH_LASTCONFLICTINFO_SERVICE_STRING_ID)) {
        Add(new cMenuEditBoolItem(tr("Show timer conflicts"), &data->showTimerConflicts));
      } else {
        Add(new cOsdItem(EPGSEARCH_LASTCONFLICTINFO_SERVICE_STRING_ID " service not found!", osUnknown, false));
      }
    }
  }

  SetCurrent(Get(currentItem));
  Display();
  SetHelp(NULL, NULL, NULL, NULL);
}
#endif
// vim:et:sw=2:ts=2:

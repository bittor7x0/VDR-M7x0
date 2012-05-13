/*
 * skinenigmang.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "config.h"
#include "enigma.h"
#include "i18n.h"
#include "logo.h"
#include "setup.h"
#include "status.h"

#include <getopt.h>
#include <vdr/plugin.h>

#if defined(APIVERSNUM) && APIVERSNUM < 10400
#error "VDR-1.4.0 API version or greater is required!"
#endif

#if VDRVERSNUM == 10503
#warning "YOU NEED A PATCHED VDR 1.5.3 OR EnigmaNG WILL CRASH!"
#endif

static const char VERSION[] = "0.0.5";
static const char DESCRIPTION[] = "EnigmaNG skin";

class cPluginSkinEnigma : public cPlugin {
private:
  bool fLogodirSet;
  bool fImagesDirSet;
#ifdef HAVE_FREETYPE
  bool fFontsDirSet;
#endif

public:
  cPluginSkinEnigma(void);
  virtual ~cPluginSkinEnigma();
  virtual const char *Version(void) {
    return VERSION;
  } virtual const char *Description(void) {
    return tr(DESCRIPTION);
  }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void) {
  }
  virtual cString Active(void) {
    return NULL;
  }
  virtual const char *MainMenuEntry(void) {
    return NULL;
  }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginSkinEnigma::cPluginSkinEnigma(void)
{
  // initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  fLogodirSet = false;
  fImagesDirSet = false;
#ifdef HAVE_FREETYPE
  fFontsDirSet = false;
#endif
}

cPluginSkinEnigma::~cPluginSkinEnigma()
{
  // clean up after yourself!
}

const char *cPluginSkinEnigma::CommandLineHelp(void)
{ 
  // return a string that describes all known command line options.
  return 
#ifdef HAVE_FREETYPE
         "  -f <FONTSDIR>, --fonts=<FONTSDIR> Set directory where truetype fonts are stored\n"
#endif
         "  -i <IMAGESDIR>, --epgimages=<IMAGESDIR> Set directory where epgimages are stored\n"
         "  -l <LOGODIR>, --logodir=<LOGODIR>       Set directory where logos are stored.\n";
}

bool cPluginSkinEnigma::ProcessArgs(int argc, char *argv[])
{
  // implement command line argument processing here if applicable.
  static const struct option long_options[] = {
#ifdef HAVE_FREETYPE
    { "fonts", required_argument, NULL, 'f' },
#endif
    { "epgimages", required_argument, NULL, 'i' },
    { "logodir", required_argument, NULL, 'l' },
    { NULL }
  };

  int c;
  while ((c = getopt_long(argc, argv, "i:f:l:", long_options, NULL)) != -1) {
    switch (c) {
#ifdef HAVE_FREETYPE
      case 'f':
        EnigmaConfig.SetFontsDir(optarg);
        fFontsDirSet = true;
        break;
#endif
      case 'i':
        EnigmaConfig.SetImagesDir(optarg);
        fImagesDirSet = true;
        break;
      case 'l':
        EnigmaConfig.SetLogoDir(optarg);
        fLogodirSet = true;
        break;
      default:
        return false;
    }
  }
  return true;
}

bool cPluginSkinEnigma::Initialize(void)
{
  // initialize any background activities the plugin shall perform.
  debug("cPluginSkinEnigma::Initialize()");
  return true;
}

bool cPluginSkinEnigma::Start(void)
{
  // start any background activities the plugin shall perform.
  debug("cPluginSkinEnigma::Start()");
  RegisterI18n(Phrases);
  if (!fLogodirSet) {
    // set logo directory
    EnigmaConfig.SetLogoDir(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
    fLogodirSet = true;
  }
  if (!fImagesDirSet) {
    // set epgimages directory
    char *dir = NULL;
    asprintf(&dir, "%s/epgimages", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
    if (dir) {
      EnigmaConfig.SetImagesDir(dir);
      fImagesDirSet = true;
      free(dir);
    }
  }
#ifdef HAVE_FREETYPE
  if (!fFontsDirSet) {
    // set fonts directory
    char *dir = NULL;
    asprintf(&dir, "%s/fonts", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
    if (dir) {
      EnigmaConfig.SetFontsDir(dir);
      fFontsDirSet = true;
      free(dir);
    }
  }
#endif

  // resize logo cache
  EnigmaLogoCache.Resize(EnigmaConfig.cacheSize);
  // create skin
  new cSkinEnigma;
  return true;
}

void cPluginSkinEnigma::Stop(void)
{
  // stop any background activities the plugin shall perform.
  debug("cPluginSkinEnigma::Stop()");
}

void cPluginSkinEnigma::Housekeeping(void)
{
  // perform any cleanup or other regular tasks.
}

cOsdObject *cPluginSkinEnigma::MainMenuAction(void)
{
  // perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginSkinEnigma::SetupMenu(void)
{
  // return a setup menu in case the plugin supports one.
  debug("cPluginSkinEnigma::SetupMenu()");
  return new cPluginSkinEnigmaSetup();
}

bool cPluginSkinEnigma::SetupParse(const char *Name, const char *Value)
{
  // parse your own setup parameters and store their values.
//  debug("cPluginSkinEnigma::SetupParse()");

       if (!strcasecmp(Name, "SingleArea8Bpp"))            EnigmaConfig.singleArea8Bpp = atoi(Value);
  else if (!strcasecmp(Name, "ShowAuxInfo"))               EnigmaConfig.showAuxInfo = atoi(Value);
  else if (!strcasecmp(Name, "ShowProgressBar"))           EnigmaConfig.showProgressbar = atoi(Value);
  else if (!strcasecmp(Name, "ShowRemaining"))             EnigmaConfig.showRemaining = atoi(Value);
  else if (!strcasecmp(Name, "ShowListSymbols"))           EnigmaConfig.showListSymbols = atoi(Value);
  else if (!strcasecmp(Name, "ShowSymbols"))               EnigmaConfig.showSymbols = atoi(Value);
  else if (!strcasecmp(Name, "ShowSymbolsMenu"))           EnigmaConfig.showSymbolsMenu = atoi(Value);
  else if (!strcasecmp(Name, "ShowSymbolsReplay"))         EnigmaConfig.showSymbolsReplay = atoi(Value);
  else if (!strcasecmp(Name, "ShowSymbolsMsgs"))           EnigmaConfig.showSymbolsMsgs = atoi(Value);
  else if (!strcasecmp(Name, "ShowSymbolsAudio"))          EnigmaConfig.showSymbolsAudio = atoi(Value);
  else if (!strcasecmp(Name, "ShowLogo"))                  EnigmaConfig.showLogo = atoi(Value);
  else if (!strcasecmp(Name, "ShowInfo"))                  EnigmaConfig.showInfo = atoi(Value);
  else if (!strcasecmp(Name, "ShowMarker"))                EnigmaConfig.showMarker = atoi(Value);
  else if (!strcasecmp(Name, "ShowVPS"))                   EnigmaConfig.showVps = atoi(Value);
  else if (!strcasecmp(Name, "ShowFlags"))                 EnigmaConfig.showFlags = atoi(Value);
  else if (!strcasecmp(Name, "CacheSize"))                 EnigmaConfig.cacheSize = atoi(Value);
  else if (!strcasecmp(Name, "UseChannelId"))              EnigmaConfig.useChannelId = atoi(Value);
  else if (!strcasecmp(Name, "NumReruns"))                 EnigmaConfig.numReruns = atoi(Value);
  else if (!strcasecmp(Name, "UseSubtitleRerun"))          EnigmaConfig.useSubtitleRerun = atoi(Value);
  else if (!strcasecmp(Name, "ShowTimerConflicts"))        EnigmaConfig.showTimerConflicts = atoi(Value);
  else if (!strcasecmp(Name, "ShowRecSize"))               EnigmaConfig.showRecSize = atoi(Value);
  else if (!strcasecmp(Name, "ShowImages"))                EnigmaConfig.showImages = atoi(Value);
  else if (!strcasecmp(Name, "ResizeImages"))              EnigmaConfig.resizeImages = atoi(Value);
  else if (!strcasecmp(Name, "ShowMailIcon"))              EnigmaConfig.showMailIcon = atoi(Value);
  else if (!strcasecmp(Name, "ImageWidth"))                EnigmaConfig.imageWidth = atoi(Value);
  else if (!strcasecmp(Name, "ImageHeight"))               EnigmaConfig.imageHeight = atoi(Value);
  else if (!strcasecmp(Name, "ImageExtension"))            EnigmaConfig.imageExtension = atoi(Value);
  else if (!strcasecmp(Name, "FullTitleWidth"))            EnigmaConfig.fullTitleWidth = atoi(Value);
  else if (!strcasecmp(Name, "UseTextEffects"))            EnigmaConfig.useTextEffects = atoi(Value);
  else if (!strcasecmp(Name, "ScrollDelay"))               EnigmaConfig.scrollDelay = atoi(Value);
  else if (!strcasecmp(Name, "ScrollPause"))               EnigmaConfig.scrollPause = atoi(Value);
  else if (!strcasecmp(Name, "ScrollMode"))                EnigmaConfig.scrollMode = atoi(Value);
  else if (!strcasecmp(Name, "BlinkPause"))                EnigmaConfig.blinkPause = atoi(Value);
  else if (!strcasecmp(Name, "ScrollInfo"))                EnigmaConfig.scrollInfo = atoi(Value);
  else if (!strcasecmp(Name, "ScrollListItem"))            EnigmaConfig.scrollListItem = atoi(Value);
  else if (!strcasecmp(Name, "ScrollOther"))               EnigmaConfig.scrollOther = atoi(Value);
  else if (!strcasecmp(Name, "ScrollTitle"))               EnigmaConfig.scrollTitle = atoi(Value);
  else if (!strcasecmp(Name, "FontOsdTitle"))              EnigmaConfig.SetFont(FONT_OSDTITLE, atoi(Value));
  else if (!strcasecmp(Name, "FontOsdTitleName"))          EnigmaConfig.SetFont(FONT_OSDTITLE, Value);
  else if (!strcasecmp(Name, "FontMessage"))               EnigmaConfig.SetFont(FONT_MESSAGE, atoi(Value));
  else if (!strcasecmp(Name, "FontMessageName"))           EnigmaConfig.SetFont(FONT_MESSAGE, Value);
  else if (!strcasecmp(Name, "FontDate"))                  EnigmaConfig.SetFont(FONT_DATE, atoi(Value));
  else if (!strcasecmp(Name, "FontDateName"))              EnigmaConfig.SetFont(FONT_DATE, Value);
  else if (!strcasecmp(Name, "FontHelpKeys"))              EnigmaConfig.SetFont(FONT_HELPKEYS, atoi(Value));
  else if (!strcasecmp(Name, "FontHelpKeysName"))          EnigmaConfig.SetFont(FONT_HELPKEYS, Value);
  else if (!strcasecmp(Name, "FontCiTitle"))               EnigmaConfig.SetFont(FONT_CITITLE, atoi(Value));
  else if (!strcasecmp(Name, "FontCiTitleName"))           EnigmaConfig.SetFont(FONT_CITITLE, Value);
  else if (!strcasecmp(Name, "FontCiSubtitle"))            EnigmaConfig.SetFont(FONT_CISUBTITLE, atoi(Value));
  else if (!strcasecmp(Name, "FontCiSubtitleName"))        EnigmaConfig.SetFont(FONT_CISUBTITLE, Value);
  else if (!strcasecmp(Name, "FontCiLanguage"))            EnigmaConfig.SetFont(FONT_CILANGUAGE, atoi(Value));
  else if (!strcasecmp(Name, "FontCiLanguageName"))        EnigmaConfig.SetFont(FONT_CILANGUAGE, Value);
  else if (!strcasecmp(Name, "FontListItem"))              EnigmaConfig.SetFont(FONT_LISTITEM, atoi(Value));
  else if (!strcasecmp(Name, "FontListItemName"))          EnigmaConfig.SetFont(FONT_LISTITEM, Value);
  else if (!strcasecmp(Name, "FontInfoTimerHeadline"))     EnigmaConfig.SetFont(FONT_INFOTIMERHEADLINE, atoi(Value));
  else if (!strcasecmp(Name, "FontInfoTimerHeadlineName")) EnigmaConfig.SetFont(FONT_INFOTIMERHEADLINE, Value);
  else if (!strcasecmp(Name, "FontInfoTimerText"))         EnigmaConfig.SetFont(FONT_INFOTIMERTEXT, atoi(Value));
  else if (!strcasecmp(Name, "FontInfoTimerTextName"))     EnigmaConfig.SetFont(FONT_INFOTIMERTEXT, Value);
  else if (!strcasecmp(Name, "FontInfoWarnHeadline"))      EnigmaConfig.SetFont(FONT_INFOWARNHEADLINE, atoi(Value));
  else if (!strcasecmp(Name, "FontInfoWarnHeadlineName"))  EnigmaConfig.SetFont(FONT_INFOWARNHEADLINE, Value);
  else if (!strcasecmp(Name, "FontInfoWarnText"))          EnigmaConfig.SetFont(FONT_INFOWARNTEXT, atoi(Value));
  else if (!strcasecmp(Name, "FontInfoWarnTextName"))      EnigmaConfig.SetFont(FONT_INFOWARNTEXT, Value);
  else if (!strcasecmp(Name, "FontDetailsTitle"))          EnigmaConfig.SetFont(FONT_DETAILSTITLE, atoi(Value));
  else if (!strcasecmp(Name, "FontDetailsTitleName"))      EnigmaConfig.SetFont(FONT_DETAILSTITLE, Value);
  else if (!strcasecmp(Name, "FontDetailsSubtitle"))       EnigmaConfig.SetFont(FONT_DETAILSSUBTITLE, atoi(Value));
  else if (!strcasecmp(Name, "FontDetailsSubtitleName"))   EnigmaConfig.SetFont(FONT_DETAILSSUBTITLE, Value);
  else if (!strcasecmp(Name, "FontDetailsDate"))           EnigmaConfig.SetFont(FONT_DETAILSDATE, atoi(Value));
  else if (!strcasecmp(Name, "FontDetailsDateName"))       EnigmaConfig.SetFont(FONT_DETAILSDATE, Value);
  else if (!strcasecmp(Name, "FontDetailsText"))           EnigmaConfig.SetFont(FONT_DETAILSTEXT, atoi(Value));
  else if (!strcasecmp(Name, "FontDetailsTextName"))       EnigmaConfig.SetFont(FONT_DETAILSTEXT, Value);
  else if (!strcasecmp(Name, "FontReplayTimes"))           EnigmaConfig.SetFont(FONT_REPLAYTIMES, atoi(Value));
  else if (!strcasecmp(Name, "FontReplayTimesName"))       EnigmaConfig.SetFont(FONT_REPLAYTIMES, Value);
  else if (!strcasecmp(Name, "FontFixed"))                 EnigmaConfig.SetFont(FONT_FIXED, atoi(Value));
  else if (!strcasecmp(Name, "FontFixedName"))             EnigmaConfig.SetFont(FONT_FIXED, Value);
  else if (!strcasecmp(Name, "DynOSD"))                    EnigmaConfig.dynOsd = atoi(Value);
  else return false;

  return true;
}

bool cPluginSkinEnigma::Service(const char *Id, void *Data)
{
  // handle custom service requests from other plugins
  return false;
}

const char **cPluginSkinEnigma::SVDRPHelpPages(void)
{
  // return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginSkinEnigma::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginSkinEnigma);    // don't touch this!
// vim:et:sw=2:ts=2:

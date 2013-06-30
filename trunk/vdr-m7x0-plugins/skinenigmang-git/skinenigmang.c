/*
 * skinenigmang.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "config.h"
#include "enigma.h"
#include "logo.h"
#include "setup.h"
#include "status.h"

#include <getopt.h>
#include <vdr/plugin.h>

#if defined(APIVERSNUM) && APIVERSNUM < 10600
#error "VDR-1.6.0 API version or greater is required!"
#endif


static const char VERSION[] = "0.1.2";
static const char DESCRIPTION[] = trNOOP("EnigmaNG skin");

class cPluginSkinEnigma : public cPlugin {
private:
  bool fLogodirSet;
  bool fImagesDirSet;

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
}

cPluginSkinEnigma::~cPluginSkinEnigma()
{
  // clean up after yourself!
}

const char *cPluginSkinEnigma::CommandLineHelp(void)
{ 
  // return a string that describes all known command line options.
  return 
         "  -i <IMAGESDIR>, --epgimages=<IMAGESDIR> Set directory where epgimages are stored\n"
         "  -l <LOGODIR>, --logodir=<LOGODIR>       Set directory where logos are stored.\n";
}

bool cPluginSkinEnigma::ProcessArgs(int argc, char *argv[])
{
  // implement command line argument processing here if applicable.
  static const struct option long_options[] = {
    { "epgimages", required_argument, NULL, 'i' },
    { "logodir", required_argument, NULL, 'l' },
    { 0, 0, 0, 0 }
  };

  int c;
  while ((c = getopt_long(argc, argv, "i:f:l:", long_options, NULL)) != -1) {
    switch (c) {
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

  if (!fLogodirSet) {
    // set logo directory
    EnigmaConfig.SetLogoDir(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N));
    fLogodirSet = true;
  }
  if (!fImagesDirSet) {
    // set epgimages directory
    char *dir = NULL;
    if (-1 != asprintf(&dir, "%s/epgimages", cPlugin::ConfigDirectory(PLUGIN_NAME_I18N))) {
      EnigmaConfig.SetImagesDir(dir);
      fImagesDirSet = true;
      free(dir);
    }
  }

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

bool cPluginSkinEnigma::SetupParse(const char *OptionName, const char *Value)
{
  // parse your own setup parameters and store their values.
//  debug("cPluginSkinEnigma::SetupParse()");

       if (!strcasecmp(OptionName, "SingleArea8Bpp"))            EnigmaConfig.singleArea8Bpp = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowAuxInfo"))               EnigmaConfig.showAuxInfo = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowProgressBar"))           EnigmaConfig.showProgressbar = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowRemaining"))             EnigmaConfig.showRemaining = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowListSymbols"))           EnigmaConfig.showListSymbols = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowSymbols"))               EnigmaConfig.showSymbols = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowSymbolsMenu"))           EnigmaConfig.showSymbolsMenu = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowSymbolsReplay"))         EnigmaConfig.showSymbolsReplay = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowSymbolsMsgs"))           EnigmaConfig.showSymbolsMsgs = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowSymbolsAudio"))          EnigmaConfig.showSymbolsAudio = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowColSymbolsDetails"))     EnigmaConfig.showColSymbolsDetails = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowLogo"))                  EnigmaConfig.showLogo = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowInfo"))                  EnigmaConfig.showInfo = atoi(Value);
  else if (!strcasecmp(OptionName, "MinInfoWidth"))              EnigmaConfig.minInfoWidth = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowMarker"))                EnigmaConfig.showMarker = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowVPS"))                   EnigmaConfig.showVps = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowFlags"))                 EnigmaConfig.showFlags = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowCatGenCon"))             EnigmaConfig.showCatGenCon = atoi(Value);
  else if (!strcasecmp(OptionName, "CacheSize"))                 EnigmaConfig.cacheSize = atoi(Value);
  else if (!strcasecmp(OptionName, "UseChannelId"))              EnigmaConfig.useChannelId = atoi(Value);
  else if (!strcasecmp(OptionName, "NumReruns"))                 EnigmaConfig.numReruns = atoi(Value);
  else if (!strcasecmp(OptionName, "UseSubtitleRerun"))          EnigmaConfig.useSubtitleRerun = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowTimerConflicts"))        EnigmaConfig.showTimerConflicts = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowRecSize"))               EnigmaConfig.showRecSize = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowImages"))                EnigmaConfig.showImages = atoi(Value);
  else if (!strcasecmp(OptionName, "ResizeImages"))              EnigmaConfig.resizeImages = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowMailIcon"))              EnigmaConfig.showMailIcon = atoi(Value);
  else if (!strcasecmp(OptionName, "ImageWidth"))                EnigmaConfig.imageWidth = atoi(Value);
  else if (!strcasecmp(OptionName, "ImageHeight"))               EnigmaConfig.imageHeight = atoi(Value);
  else if (!strcasecmp(OptionName, "ImageExtension"))            EnigmaConfig.imageExtension = atoi(Value);
  else if (!strcasecmp(OptionName, "FullTitleWidth"))            EnigmaConfig.fullTitleWidth = atoi(Value);
  else if (!strcasecmp(OptionName, "UseTextEffects"))            EnigmaConfig.useTextEffects = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollDelay"))               EnigmaConfig.scrollDelay = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollPause"))               EnigmaConfig.scrollPause = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollMode"))                EnigmaConfig.scrollMode = atoi(Value);
  else if (!strcasecmp(OptionName, "BlinkPause"))                EnigmaConfig.blinkPause = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollInfo"))                EnigmaConfig.scrollInfo = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollListItem"))            EnigmaConfig.scrollListItem = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollOther"))               EnigmaConfig.scrollOther = atoi(Value);
  else if (!strcasecmp(OptionName, "ScrollTitle"))               EnigmaConfig.scrollTitle = atoi(Value);
  else if (!strcasecmp(OptionName, "FontOsdTitle"))              EnigmaConfig.SetFont(FONT_OSDTITLE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontOsdTitleName"))          EnigmaConfig.SetFont(FONT_OSDTITLE, Value);
  else if (!strcasecmp(OptionName, "FontMessage"))               EnigmaConfig.SetFont(FONT_MESSAGE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontMessageName"))           EnigmaConfig.SetFont(FONT_MESSAGE, Value);
  else if (!strcasecmp(OptionName, "FontDate"))                  EnigmaConfig.SetFont(FONT_DATE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontDateName"))              EnigmaConfig.SetFont(FONT_DATE, Value);
  else if (!strcasecmp(OptionName, "FontHelpKeys"))              EnigmaConfig.SetFont(FONT_HELPKEYS, atoi(Value));
  else if (!strcasecmp(OptionName, "FontHelpKeysName"))          EnigmaConfig.SetFont(FONT_HELPKEYS, Value);
  else if (!strcasecmp(OptionName, "FontCiTitle"))               EnigmaConfig.SetFont(FONT_CITITLE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontCiTitleName"))           EnigmaConfig.SetFont(FONT_CITITLE, Value);
  else if (!strcasecmp(OptionName, "FontCiSubtitle"))            EnigmaConfig.SetFont(FONT_CISUBTITLE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontCiSubtitleName"))        EnigmaConfig.SetFont(FONT_CISUBTITLE, Value);
  else if (!strcasecmp(OptionName, "FontCiLanguage"))            EnigmaConfig.SetFont(FONT_CILANGUAGE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontCiLanguageName"))        EnigmaConfig.SetFont(FONT_CILANGUAGE, Value);
  else if (!strcasecmp(OptionName, "FontListItem"))              EnigmaConfig.SetFont(FONT_LISTITEM, atoi(Value));
  else if (!strcasecmp(OptionName, "FontListItemName"))          EnigmaConfig.SetFont(FONT_LISTITEM, Value);
  else if (!strcasecmp(OptionName, "FontInfoTimerHeadline"))     EnigmaConfig.SetFont(FONT_INFOTIMERHEADLINE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontInfoTimerHeadlineName")) EnigmaConfig.SetFont(FONT_INFOTIMERHEADLINE, Value);
  else if (!strcasecmp(OptionName, "FontInfoTimerText"))         EnigmaConfig.SetFont(FONT_INFOTIMERTEXT, atoi(Value));
  else if (!strcasecmp(OptionName, "FontInfoTimerTextName"))     EnigmaConfig.SetFont(FONT_INFOTIMERTEXT, Value);
  else if (!strcasecmp(OptionName, "FontInfoWarnHeadline"))      EnigmaConfig.SetFont(FONT_INFOWARNHEADLINE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontInfoWarnHeadlineName"))  EnigmaConfig.SetFont(FONT_INFOWARNHEADLINE, Value);
  else if (!strcasecmp(OptionName, "FontInfoWarnText"))          EnigmaConfig.SetFont(FONT_INFOWARNTEXT, atoi(Value));
  else if (!strcasecmp(OptionName, "FontInfoWarnTextName"))      EnigmaConfig.SetFont(FONT_INFOWARNTEXT, Value);
  else if (!strcasecmp(OptionName, "FontDetailsTitle"))          EnigmaConfig.SetFont(FONT_DETAILSTITLE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontDetailsTitleName"))      EnigmaConfig.SetFont(FONT_DETAILSTITLE, Value);
  else if (!strcasecmp(OptionName, "FontDetailsSubtitle"))       EnigmaConfig.SetFont(FONT_DETAILSSUBTITLE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontDetailsSubtitleName"))   EnigmaConfig.SetFont(FONT_DETAILSSUBTITLE, Value);
  else if (!strcasecmp(OptionName, "FontDetailsDate"))           EnigmaConfig.SetFont(FONT_DETAILSDATE, atoi(Value));
  else if (!strcasecmp(OptionName, "FontDetailsDateName"))       EnigmaConfig.SetFont(FONT_DETAILSDATE, Value);
  else if (!strcasecmp(OptionName, "FontDetailsText"))           EnigmaConfig.SetFont(FONT_DETAILSTEXT, atoi(Value));
  else if (!strcasecmp(OptionName, "FontDetailsTextName"))       EnigmaConfig.SetFont(FONT_DETAILSTEXT, Value);
  else if (!strcasecmp(OptionName, "FontReplayTimes"))           EnigmaConfig.SetFont(FONT_REPLAYTIMES, atoi(Value));
  else if (!strcasecmp(OptionName, "FontReplayTimesName"))       EnigmaConfig.SetFont(FONT_REPLAYTIMES, Value);
  else if (!strcasecmp(OptionName, "FontFixed"))                 EnigmaConfig.SetFont(FONT_FIXED, atoi(Value));
  else if (!strcasecmp(OptionName, "FontFixedName"))             EnigmaConfig.SetFont(FONT_FIXED, Value);
  else if (!strcasecmp(OptionName, "DynOSD"))                    EnigmaConfig.dynOsd = atoi(Value);
  else if (!strcasecmp(OptionName, "StatusLineMode"))            EnigmaConfig.statusLineMode = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowWssSymbols"))            EnigmaConfig.showWssSymbols = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowStatusSymbols"))         EnigmaConfig.showStatusSymbols = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowScrollbar"))             EnigmaConfig.showScrollbar = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowSignalInfo"))            EnigmaConfig.showSignalInfo = atoi(Value);
  else if (!strcasecmp(OptionName, "ShowCaMode"))                EnigmaConfig.showCaMode = atoi(Value);
  else if (!strcasecmp(OptionName, "DrawRoundCorners"))          EnigmaConfig.drawRoundCorners = atoi(Value);
  else if (!strcasecmp(OptionName, "ChannelLogoWidth"))          EnigmaConfig.channelLogoWidth = atoi(Value);
  else if (!strcasecmp(OptionName, "ChannelLogoHeight"))         EnigmaConfig.channelLogoHeight = atoi(Value);
  else if (!strcasecmp(OptionName, "SignalInfoWidth"))           EnigmaConfig.signalInfoWidth = atoi(Value);
  else if (!strcasecmp(OptionName, "ProgressBarWidth"))          EnigmaConfig.progressBarWidth = atoi(Value);
  else return false;

  return true;
}

bool cPluginSkinEnigma::Service(const char * /* Id */, void * /* Data */)
{
  // handle custom service requests from other plugins
  return false;
}

const char **cPluginSkinEnigma::SVDRPHelpPages(void)
{
  // return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginSkinEnigma::SVDRPCommand(const char * /* Command */, const char * /* Option */, int & /* ReplyCode */)
{
  // process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginSkinEnigma);    // don't touch this!
// vim:et:sw=2:ts=2:

/*
 * enigma.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "config.h"
#include "enigma.h"
#include "logo.h"
#include "i18n.h"
#include "status.h"
#include "texteffects.h"

#include <math.h>
#include <ctype.h>
#include <sstream>
#include <iomanip>
#include <locale.h>

#include <vdr/device.h>
#include <vdr/timers.h>
#include <vdr/menu.h>
#include <vdr/font.h>
#include <vdr/osd.h>
#include <vdr/themes.h>
#include <vdr/plugin.h>

#ifdef SKINENIGMA_HAVE_EPGSEARCH
#include "../epgsearch/services.h"
#endif

#ifdef USE_PLUGIN_AVARDS
#include "../avards/avards_services.h"
#endif

#include "tools.h"

#define MAX_AUDIO_BITMAPS      3
#define MAX_SPEED_BITMAPS      10
#define MAX_TRICKSPEED_BITMAPS 4


#include "symbols/small/eventparttimer.xpm"
#include "symbols/small/eventtimer.xpm"
#include "symbols/small/eventvps.xpm"
#include "symbols/small/eventrunning.xpm"
#include "symbols/small/eventrecording.xpm"
#include "symbols/small/recordingnew.xpm"

// small symbols (e.g. for channelinfo)
#include "symbols/small/audio.xpm"
#include "symbols/small/audioleft.xpm"
#include "symbols/small/audioright.xpm"
#include "symbols/small/dolbydigital.xpm"
#include "symbols/small/encrypted.xpm"
#include "symbols/small/teletext.xpm"
#include "symbols/small/vps.xpm"
#include "symbols/small/radio.xpm"
#include "symbols/small/recording.xpm"
#include "symbols/small/timer.xpm"
#include "symbols/small/run.xpm"
#ifdef USE_PLUGIN_MAILBOX
#include "symbols/small/mail.xpm"
#endif

static const char *strWeekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static cBitmap bmEventPartTimer(eventparttimer_xpm);
static cBitmap bmEventTimer(eventtimer_xpm);
static cBitmap bmEventVPS(eventvps_xpm);
static cBitmap bmEventRunning(eventrunning_xpm);
static cBitmap bmEventRecording(eventrecording_xpm);
static cBitmap bmRecordingNew(recordingnew_xpm);

// channel info
static cBitmap bmAudio[MAX_AUDIO_BITMAPS] = {
  cBitmap(audio_xpm),
  cBitmap(audioleft_xpm),
  cBitmap(audioright_xpm)
};
static cBitmap bmDolbyDigital(dolbydigital_xpm);
static cBitmap bmEncrypted(encrypted_xpm);
static cBitmap bmRadio(radio_xpm);
static cBitmap bmRecording(recording_xpm);
static cBitmap bmTeletext(teletext_xpm);
static cBitmap bmVPS(vps_xpm);
static cBitmap bmRun(run_xpm);
static cBitmap bmTimer(timer_xpm);
#ifdef USE_PLUGIN_MAILBOX
static cBitmap bmMail(mail_xpm);
#endif

// audio menu
static const char *strAudio_large[MAX_AUDIO_BITMAPS] = {
  "icons/audio/audioStereo",
  "icons/audio/audioLeft",
  "icons/audio/audioRight"
};

// replay
static const char *strSlowForward_large[MAX_TRICKSPEED_BITMAPS] = {
  "icons/replay/slowForward",
  "icons/replay/slowForward1",
  "icons/replay/slowForward2",
  "icons/replay/slowForward3"
};
static const char *strFastForward_large[MAX_SPEED_BITMAPS] = {
  "icons/replay/fastForward",
  "icons/replay/fastForward1",
  "icons/replay/fastForward2",
  "icons/replay/fastForward3",
  "icons/replay/fastForward4",
  "icons/replay/fastForward5",
  "icons/replay/fastForward6",
  "icons/replay/fastForward7",
  "icons/replay/fastForward8",
  "icons/replay/fastForward9"
};
static const char *strSlowRewind_large[MAX_TRICKSPEED_BITMAPS] = {
  "icons/replay/slowRewind",
  "icons/replay/slowRewind1",
  "icons/replay/slowRewind2",
  "icons/replay/slowRewind3"
};
static const char *strFastRewind_large[MAX_SPEED_BITMAPS] = {
  "icons/replay/fastRewind",
  "icons/replay/fastRewind1",
  "icons/replay/fastRewind2",
  "icons/replay/fastRewind3",
  "icons/replay/fastRewind4",
  "icons/replay/fastRewind5",
  "icons/replay/fastRewind6",
  "icons/replay/fastRewind7",
  "icons/replay/fastRewind8",
  "icons/replay/fastRewind9"
};

static cTheme Theme;

// Background colors
THEME_CLR(Theme, clrBackground, 0xE5DEE5FA);
THEME_CLR(Theme, clrAltBackground, 0xE5B2BBD6);
THEME_CLR(Theme, clrTitleBg, 0xE54158BC);
THEME_CLR(Theme, clrLogoBg, 0xE58EA4E3);
THEME_CLR(Theme, clrBottomBg, 0xE51D2F7D);
THEME_CLR(Theme, clrBotProgBarBg, 0xFF808080);
THEME_CLR(Theme, clrBotProgBarFg, 0xFFFFFFFF);
// Text colors
THEME_CLR(Theme, clrMenuTxtFg, 0xFF000000);
THEME_CLR(Theme, clrTitleFg, 0xFFFFFFFF);
THEME_CLR(Theme, clrTitleShadow, 0xFF000000);
// Symbols
THEME_CLR(Theme, clrSymbolActive, 0xFFC4C400);
THEME_CLR(Theme, clrSymbolInactive, 0xFF808080);
THEME_CLR(Theme, clrSymbolRecord, 0xFFC40000);
THEME_CLR(Theme, clrSymbolRecordBg, 0xFFFFFFFF);
THEME_CLR(Theme, clrSymbolTimerActive, 0xFF0000C4);
THEME_CLR(Theme, clrSymbolVpsActive, 0xFFC4C400);
THEME_CLR(Theme, clrSymbolRecActive, 0xFFC40000);
THEME_CLR(Theme, clrSymbolRunActive, 0xFF00C400);
// Help buttons
THEME_CLR(Theme, clrButtonRedFg, 0xFFFFFFFF);
THEME_CLR(Theme, clrButtonRedBg, 0xE5C40000);
THEME_CLR(Theme, clrButtonGreenFg, 0xFF000000);
THEME_CLR(Theme, clrButtonGreenBg, 0xE500C400);
THEME_CLR(Theme, clrButtonYellowFg, 0xFF000000);
THEME_CLR(Theme, clrButtonYellowBg, 0xE5C4C400);
THEME_CLR(Theme, clrButtonBlueFg, 0xFFFFFFFF);
THEME_CLR(Theme, clrButtonBlueBg, 0xE50000C4);
// Messages
THEME_CLR(Theme, clrMessageStatusFg, 0xFF000000);
THEME_CLR(Theme, clrMessageStatusBg, 0xE5D7FFD1);
THEME_CLR(Theme, clrMessageInfoFg, 0xFF000000);
THEME_CLR(Theme, clrMessageInfoBg, 0xE5D1DDFF);
THEME_CLR(Theme, clrMessageWarningFg, 0xFF000000);
THEME_CLR(Theme, clrMessageWarningBg, 0xE5F9FFD1);
THEME_CLR(Theme, clrMessageErrorFg, 0xFF000000);
THEME_CLR(Theme, clrMessageErrorBg, 0xE5FFD1D1);
// Volume bar
THEME_CLR(Theme, clrVolumeBar, 0xFF33CC33);
THEME_CLR(Theme, clrVolumeBarMute, 0xFFFF0000);
// Menu list items
THEME_CLR(Theme, clrMenuItemSelectableFg, 0xFF000000);
THEME_CLR(Theme, clrMenuItemNotSelectableFg, 0xE54158BC);
// Replay OSD
THEME_CLR(Theme, clrReplayCurrent, 0xFF1D2F7D);
THEME_CLR(Theme, clrReplayTotal, 0xFF1D2F7D);
THEME_CLR(Theme, clrReplayModeJump, 0xFF1D2F7D);
THEME_CLR(Theme, clrReplayBarAreaBg, 0xE5DEE5FA);
THEME_CLR(Theme, clrReplayProgressSeen, 0xFF8EA4E3);
THEME_CLR(Theme, clrReplayProgressRest, 0xE5DEE5FA);
THEME_CLR(Theme, clrReplayProgressSelected, 0xFF4158BC);
THEME_CLR(Theme, clrReplayProgressMark, 0xFF4158BC);
THEME_CLR(Theme, clrReplayProgressCurrent, 0xFFFF0000);

#define MIN_DATEWIDTH 144
#define TinyGap 1
#define SmallGap 2
#define Gap 4
#define BigGap 8
#define Roundness 10

#define TitleDecoGap SmallGap
#define TitleDecoGap2 SmallGap
#define TitleDecoHeight SmallGap
#define TitleDeco (TitleDecoGap + TitleDecoHeight + TitleDecoGap2)
#define LogoDecoGap SmallGap
#define LogoDecoGap2 Gap
#define LogoDecoWidth SmallGap
#define MarkerGap 6
#define ListProgressBarGap Gap
#define ListProgressBarBorder TinyGap
#define ScrollbarHeight 20
#define ScrollbarWidth 16


// --- cSkinEnigmaDisplayChannel --------------------------------------------

class cSkinEnigmaDisplayChannel : public cSkinDisplayChannel, public cSkinEnigmaOsd {
private:
  cOsd *osd;
  bool fShowLogo;
  char *strLastDate;
#ifndef DISABLE_ANIMATED_TEXT
  bool fScrollTitle;
  bool fLocked;
  int idTitle;
#endif

  const cFont *pFontOsdTitle;
  const cFont *pFontDate;
  const cFont *pFontTitle;
  const cFont *pFontSubtitle;
  const cFont *pFontLanguage;
  const cFont *pFontMessage;

  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom, xLogoDecoLeft, xLogoDecoRight, xLogoPos, yLogoPos;
  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xEventNowLeft, xEventNowRight, yEventNowTop, yEventNowBottom;
  int xEventNextLeft, xEventNextRight, yEventNextTop, yEventNextBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;
  int xFirstSymbol, xDateLeft;

  bool HasChannelTimerRecording(const cChannel *Channel);
  void DrawAreas(void);
  void DrawGroupInfo(const cChannel *Channel, int Number);
  void DrawChannelInfo(const cChannel *Channel, int Number);
  cString GetChannelName(const cChannel *Channel);
  cString GetChannelNumber(const cChannel *Channel, int Number);
public:
  cSkinEnigmaDisplayChannel(bool WithInfo);
  virtual ~ cSkinEnigmaDisplayChannel();
  virtual void SetChannel(const cChannel *Channel, int Number);
  virtual void SetEvents(const cEvent *Present, const cEvent *Following);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  virtual void DrawTitle(const char *Title);
};

cSkinEnigmaDisplayChannel::cSkinEnigmaDisplayChannel(bool WithInfo)
{
  debug("cSkinEnigmaDisplayChannel::cSkinEnigmaDisplayChannel(%d)", WithInfo);

  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontTitle = EnigmaConfig.GetFont(FONT_CITITLE);
  pFontSubtitle = EnigmaConfig.GetFont(FONT_CISUBTITLE);
  pFontLanguage = EnigmaConfig.GetFont(FONT_CILANGUAGE);
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);

  fShowLogo = EnigmaConfig.showLogo;
  xFirstSymbol = 0;
  strLastDate = NULL;
#ifndef DISABLE_ANIMATED_TEXT
  fScrollTitle = EnigmaConfig.useTextEffects && EnigmaConfig.scrollTitle;
  idTitle = -1;
#endif

  int MessageHeight = 2 * SmallGap + pFontMessage->Height() + 2 * SmallGap;
  int LogoSize = std::max(pFontTitle->Height() * 2 + pFontSubtitle->Height() * 2 + SmallGap, ChannelLogoHeight);
  LogoSize += (LogoSize % 2 ? 1 : 0);
  // title bar & logo area
  xLogoLeft = 0;
  xLogoRight = xLogoLeft + LogoSize;
  xLogoDecoLeft = xLogoRight + LogoDecoGap;
  xLogoDecoRight = xLogoDecoLeft + LogoDecoWidth;
  xTitleLeft = (fShowLogo && !EnigmaConfig.fullTitleWidth ? xLogoDecoRight + LogoDecoGap2 : xLogoLeft);
  xTitleRight = xTitleLeft + ((OsdSize.w - xTitleLeft) & ~0x07); // width must be multiple of 8
  yTitleTop = 0;
  yTitleBottom = yTitleTop + std::max(pFontOsdTitle->Height(), pFontDate->Height());
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  yLogoTop = yTitleDecoBottom + TitleDecoGap2;
  yLogoBottom = yLogoTop + LogoSize;
  xLogoPos = xLogoLeft + (LogoSize - ChannelLogoHeight) / 2;
  yLogoPos = yLogoTop + (LogoSize - ChannelLogoHeight) / 2;
  // current event area
  xEventNowLeft = (fShowLogo ? xLogoDecoRight + LogoDecoGap2 : xTitleLeft);
  xEventNowRight = xTitleRight;
  yEventNowTop = yLogoTop;
  yEventNowBottom = yEventNowTop + pFontTitle->Height() + pFontSubtitle->Height();
  // next event area
  xEventNextLeft = xEventNowLeft;
  xEventNextRight = xEventNowRight;
  yEventNextTop = yEventNowBottom + SmallGap;
  yEventNextBottom = yEventNextTop + pFontTitle->Height() + pFontSubtitle->Height();
  // progress bar area
  xBottomLeft = xTitleLeft;
  xBottomRight = xTitleRight;
  yBottomTop = yEventNextBottom + SmallGap;
  yBottomBottom = yBottomTop + pFontLanguage->Height();
  // message area
  xMessageLeft = xEventNowLeft;
  xMessageRight = xTitleRight;
  yMessageTop = yLogoTop + (LogoSize - MessageHeight) / 2;
  yMessageBottom = yMessageTop + MessageHeight;
  // date area
  cString date = DayDateTime();
  int w = pFontDate->Width(date);
  xDateLeft = xTitleRight - Roundness - w - SmallGap;

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + (Setup.ChannelInfoPos ? 0 : (OsdSize.h - yBottomBottom)) );
  tArea Areas[] = { {0, 0, xBottomRight - 1, yBottomBottom - 1, fShowLogo || EnigmaConfig.showFlags ? 8 : 4} };
  if ((Areas[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayChannel: using %dbpp single area", Areas[0].bpp);
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    // clear all
    osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  } else {
    debug("cSkinEnigmaDisplayChannel: using multiple areas");
    if (fShowLogo) {
      tArea Areas[] = { {xLogoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, 4},
                        {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2},
                        {xEventNowLeft, yEventNowTop, xEventNowRight - 1, yEventNextBottom - 1, 4},
                        {xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, 4}
      };
      eOsdError rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayChannel: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
    } else {
      tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2},
                        {xEventNowLeft, yEventNowTop, xEventNowRight - 1, yEventNextBottom - 1, 4},
                        {xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, 4}
      };
      eOsdError rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayChannel: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
    }
  }

#ifndef DISABLE_ANIMATED_TEXT
  fLocked = false;
  if (fScrollTitle) {
    TE_START(osd);
    fLocked = true;
  }
#endif
}

cSkinEnigmaDisplayChannel::~cSkinEnigmaDisplayChannel()
{
  debug("cSkinEnigmaDisplayChannel::~cSkinEnigmaDisplayChannel()");

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle) {
    if (!fLocked) TE_LOCK;
    TE_STOP;
  }
#endif
  free(strLastDate);
  delete osd;
}

bool cSkinEnigmaDisplayChannel::HasChannelTimerRecording(const cChannel *Channel)
{
  // try to find current channel from timers
  for (cTimer * t = Timers.First(); t; t = Timers.Next(t)) {
    if ((t->Channel() == Channel) && t->Recording())
      return true;
  }
  return false;
}

void cSkinEnigmaDisplayChannel::DrawAreas(void)
{
  // draw titlebar
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1,
                     Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1,
                     yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  // draw rounded left corner of title bar
  osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                   yTitleTop + Roundness - 1, clrTransparent, -2);
  // draw rounded right corner of title bar
  osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1,
                   yTitleTop + Roundness - 1, clrTransparent, -1);
  // draw current event area
  osd->DrawRectangle(xEventNowLeft, yEventNowTop, xEventNowRight - 1,
                     yEventNowBottom - 1, Theme.Color(clrBackground));
  // draw next event area
  osd->DrawRectangle(xEventNextLeft, yEventNextTop, xEventNextRight - 1,
                     yEventNextBottom - 1, Theme.Color(clrAltBackground));
  // draw progress bar area
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1,
                     yBottomBottom - 1, Theme.Color(clrBottomBg));
  osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                   xBottomLeft + Roundness, yBottomBottom - 1, clrTransparent,
                   -3);
  osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                   xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);
}

void cSkinEnigmaDisplayChannel::DrawGroupInfo(const cChannel *Channel, int Number)
{
  DrawAreas();

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle) {
    int xName = (fShowLogo && EnigmaConfig.fullTitleWidth ? xEventNowLeft : xTitleLeft + Roundness + pFontOsdTitle->Width("0000-") + Gap);
    idTitle = TE_TITLE(osd, idTitle, GetChannelName(Channel), xDateLeft - SmallGap - xName, this);
  } else
#endif
    DrawTitle(GetChannelName(Channel));
}

void cSkinEnigmaDisplayChannel::DrawTitle(const char *Title)
{
  //Must be TE_LOCKed by caller

  int xName = (fShowLogo && EnigmaConfig.fullTitleWidth ? xEventNowLeft : xTitleLeft + Roundness + pFontOsdTitle->Width("0000-") + Gap);
  // draw titlebar
  osd->DrawRectangle(xName, yTitleTop, xDateLeft - SmallGap - 1, yTitleBottom - 1,
                     Theme.Color(clrTitleBg));
  osd->DrawRectangle(xName, yTitleDecoTop, xDateLeft - SmallGap - 1,
                     yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  if (Title) {
    int y = yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2;
    // draw channel group name
    osd->DrawText(xName + 3, y + 3, Title,
                  Theme.Color(clrTitleShadow), clrTransparent, pFontOsdTitle,
                  xDateLeft - SmallGap - xName - 3, yTitleBottom - y - 3);
    osd->DrawText(xName, y, Title,
                  Theme.Color(clrTitleFg), clrTransparent, pFontOsdTitle,
                  xDateLeft - SmallGap - xName - 3, yTitleBottom - y);
  }
}

void cSkinEnigmaDisplayChannel::DrawChannelInfo(const cChannel *Channel, int Number)
{
  DrawAreas();

  int xNumber = xTitleLeft + Roundness;
  int xName = xNumber + pFontOsdTitle->Width("0000-") + Gap;
  if (fShowLogo && EnigmaConfig.fullTitleWidth) {
    xNumber = xTitleLeft + Roundness;
    xName = xEventNowLeft;
  }

  // draw channel number
  osd->DrawText(xNumber + 3, yTitleTop + 3, GetChannelNumber(Channel, Number),
                Theme.Color(clrTitleShadow), clrTransparent, pFontOsdTitle,
                xName - xNumber - Gap - 3, yTitleBottom - yTitleTop - 3, taCenter);
  osd->DrawText(xNumber, yTitleTop, GetChannelNumber(Channel, Number),
                Theme.Color(clrTitleFg), clrTransparent, pFontOsdTitle,
                xName - xNumber - Gap, yTitleBottom - yTitleTop, taCenter);

  // draw channel name
#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle) {
    int xName = (fShowLogo && EnigmaConfig.fullTitleWidth ? xEventNowLeft : xTitleLeft + Roundness + pFontOsdTitle->Width("0000-") + Gap);
    idTitle = TE_TITLE(osd, idTitle, GetChannelName(Channel), xDateLeft - SmallGap - xName, this);
  } else
#endif
    DrawTitle(GetChannelName(Channel));

  // draw symbols
  // right edge of logo
  int xs = xBottomRight - Roundness;
  // bottom edge of logo
  int ys = yBottomTop + (yBottomBottom - yBottomTop - SymbolHeight) / 2;
  bool isvps = false;
  if (EnigmaConfig.showVps) {
    // check if vps
    // get schedule
    cSchedulesLock SchedulesLock;
    const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock);
    if (Schedules) {
      const cSchedule *Schedule = Schedules->GetSchedule(Channel);
      if (Schedule) {
        // get present event
        const cEvent *Event = Schedule->GetPresentEvent();
        // check if present event has vps
        if (Event && Event->Vps()) {
          isvps = true;
        }
      }
    }
  }

  bool fFlagFound = false;
  if (EnigmaConfig.showFlags && !isempty(Channel->Alang(0))) {
    char *strFile = NULL;
    asprintf(&strFile, "flags/%s", Channel->Alang(0));
    if (EnigmaLogoCache.LoadSymbol(strFile)) {
      xs -= (EnigmaLogoCache.Get().Width() + SmallGap);
      osd->DrawBitmap(xs, ys, EnigmaLogoCache.Get());
      fFlagFound = true;
    }
    free(strFile);
  }
  if (!fFlagFound) {
    xs -= pFontLanguage->Width(Channel->Alang(0));
    osd->DrawText(xs, yBottomTop + SmallGap , Channel->Alang(0),
                  Theme.Color(clrSymbolActive), Theme.Color(clrBottomBg), pFontLanguage,
                  pFontLanguage->Width(Channel->Alang(0)), yBottomBottom - SmallGap);
  }
  // draw audio symbol according to current audio channel
  int AudioMode = cDevice::PrimaryDevice()->GetAudioChannel();
  if (!(AudioMode >= 0 && AudioMode < MAX_AUDIO_BITMAPS))
    AudioMode = 0;
  xs -= (bmAudio[AudioMode].Width() + SmallGap);
  osd->DrawBitmap(xs, ys, bmAudio[AudioMode],
                  Theme.Color(clrBottomBg), Theme.Color(Channel->Apid(1) ? clrSymbolActive : clrSymbolInactive));
  // draw dolby digital symbol
  xs -= (bmDolbyDigital.Width() + SmallGap);
  osd->DrawBitmap(xs, ys, bmDolbyDigital,
                  Theme.Color(clrBottomBg), Theme.Color(Channel->Dpid(0) ? clrSymbolActive : clrSymbolInactive));
  if (EnigmaConfig.showVps) {
    // draw vps symbol
    xs -= (bmVPS.Width() + SmallGap);
    osd->DrawBitmap(xs, ys, bmVPS,
                    Theme.Color(clrBottomBg), Theme.Color(isvps ? clrSymbolActive : clrSymbolInactive));
  }
  // draw radio symbol if no video PID; otherwise draw teletext symbol
  if (Channel->Vpid()) {
    xs -= (bmTeletext.Width() + SmallGap);
    osd->DrawBitmap(xs, ys, bmTeletext,
                    Theme.Color(clrBottomBg), Theme.Color(Channel->Tpid() ? clrSymbolActive : clrSymbolInactive));
  } else {
    xs -= (bmRadio.Width() + SmallGap);
    osd->DrawBitmap(xs, ys, bmRadio,
                    Theme.Color(clrBottomBg), Theme.Color(Channel->Apid(0) ? clrSymbolActive : clrSymbolInactive));
  }
  // draw encryption symbol
  xs -= (bmEncrypted.Width() + SmallGap);
  osd->DrawBitmap(xs, ys, bmEncrypted,
                  Theme.Color(clrBottomBg), Theme.Color(Channel->Ca() ? clrSymbolActive : clrSymbolInactive));
  // draw recording symbol
  xs -= (bmRecording.Width() + SmallGap);
  if (cRecordControls::Active()) {
    bool fRecording = HasChannelTimerRecording(Channel);
    osd->DrawBitmap(xs, ys, bmRecording,
                    Theme.Color(fRecording ? clrSymbolRecordBg : clrBottomBg),
                    Theme.Color(fRecording ? clrSymbolRecord : clrSymbolActive));
  } else
    osd->DrawBitmap(xs, ys, bmRecording,
                    Theme.Color(clrBottomBg), Theme.Color(clrSymbolInactive));

#ifdef USE_PLUGIN_MAILBOX
  if (EnigmaConfig.showMailIcon) {
    cPlugin *MailBoxPlugin = cPluginManager::GetPlugin("mailbox");
    bool fHasNewMail = false;
    if (MailBoxPlugin && MailBoxPlugin->Service("MailBox-HasNewMail-1.0", &fHasNewMail)) {
      if (fHasNewMail || EnigmaConfig.showMailIcon > 1) {
        xs -= (bmMail.Width() + SmallGap);
        osd->DrawBitmap(xs, ys, bmMail, Theme.Color(clrBottomBg),
                        Theme.Color(fHasNewMail ? clrSymbolActive : clrSymbolInactive));
      }
    }
  }
#endif

#ifdef USE_PLUGIN_AVARDS
  cPlugin *AvardsPlugin = cPluginManager::GetPlugin("avards");
  if (AvardsPlugin) {
    avards_CurrentWssMode_v1_0 avardsData;
    if (AvardsPlugin->Service("avards-CurrentWSSMode-v1.0", &avardsData)) {
      if (avardsData.ModeString) { //TODO: icons?
        xs -= (pFontLanguage->Width(avardsData.ModeString) + SmallGap);
        osd->DrawText(xs, yBottomTop + SmallGap , avardsData.ModeString,
                      Theme.Color(clrSymbolActive), Theme.Color(clrBottomBg), pFontLanguage,
                      pFontLanguage->Width(avardsData.ModeString), yBottomBottom - SmallGap);
      }
    }
  }
#endif
  xFirstSymbol = xs - Gap;
}

cString cSkinEnigmaDisplayChannel::GetChannelName(const cChannel *Channel)
{
  char buffer[256];
  // check if channel exists
  if (Channel) {
    snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
  } else {
    snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));
  }
  return buffer;
}

cString cSkinEnigmaDisplayChannel::GetChannelNumber(const cChannel *Channel, int Number)
{
  char buffer[256];
  // check if real channel exists
  if (Channel && !Channel->GroupSep()) {
    snprintf(buffer, sizeof(buffer), "%d%s", Channel->Number(), Number ? "-" : "");
  } else if (Number) {
    // no channel but number
    snprintf(buffer, sizeof(buffer), "%d-", Number);
  } else {
    // no channel and no number
    snprintf(buffer, sizeof(buffer), " ");
  }
  return buffer;
}

void cSkinEnigmaDisplayChannel::SetChannel(const cChannel *Channel, int Number)
{
  debug("cSkinEnigmaDisplayChannel::SetChannel()");

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle && !fLocked)
    TE_LOCK;
#endif

  xFirstSymbol = 0;
  free(strLastDate);
  strLastDate = NULL;

  if (Channel) {
    // clear all
    osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);

    if (fShowLogo) {
      // draw logo area
      osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
      osd->DrawRectangle(xLogoDecoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));

      if (EnigmaLogoCache.LoadChannelLogo(Channel)) {
        osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - ChannelLogoWidth) / 2,
                        yLogoTop + (yLogoBottom - yLogoTop - ChannelLogoHeight) / 2,
                        EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                        Theme.Color(clrLogoBg), true);
      }
    }

    if (Channel->GroupSep())
      DrawGroupInfo(Channel, Number);
    else
      DrawChannelInfo(Channel, Number);
  }

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle && !fLocked)
    TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayChannel::SetEvents(const cEvent *Present,
                                          const cEvent *Following)
{
  debug("cSkinEnigmaDisplayChannel::SetEvents()");

  int xTimeLeft = xEventNowLeft + Gap;
  int xTimeWidth = pFontTitle->Width("00:00");
  int lineHeightTitle = pFontTitle->Height();
  int lineHeightSubtitle = pFontSubtitle->Height();

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle && !fLocked)
    TE_LOCK;
#endif

  // check epg datas
  const cEvent *e = Present;    // Current event
  if (e) {
    char sLen[6];
    char sNow[6];
    int total = e->Duration();
    snprintf(sLen, sizeof(sLen), "%d'", total / 60);

    int now = (time(NULL) - e->StartTime());
    if ((now < total) && ((now / 60) > 0)) {
      switch (EnigmaConfig.showRemaining) {
        case 0:
          snprintf(sNow, sizeof(sNow), "+%d'", now / 60);
          break;
        case 1: 
          snprintf(sNow, sizeof(sNow), "-%d'", (int)ceil((total - now) / 60.0));
          break;
        case 2:
          snprintf(sNow, sizeof(sNow), "%ld%%", lrint((ceil((float)now) / total * 100.0)));
          break;
        default:
          error("Invalid value for ShowRemaining: %d", EnigmaConfig.showRemaining);
      }
    }

    int xDurationLeft = xEventNowRight - Gap - std::max(pFontTitle->Width(sLen), pFontSubtitle->Width(sNow));
    int xDurationWidth = xEventNowRight - Gap - xDurationLeft;
    int xTextLeft = xTimeLeft + xTimeWidth + BigGap;
    int xTextWidth = xDurationLeft - xTextLeft - BigGap;

    // draw start time
    osd->DrawText(xTimeLeft, yEventNowTop, e->GetTimeString(),
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                  pFontTitle, xTimeWidth);
    // draw title
    osd->DrawText(xTextLeft, yEventNowTop, e->Title(),
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                  pFontTitle, xTextWidth);
    // draw duration
    osd->DrawText(xDurationLeft, yEventNowTop, sLen,
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                  pFontTitle, xDurationWidth, lineHeightTitle, taRight);
    if (e->HasTimer()) {
      if (e->IsRunning())
        osd->DrawBitmap(xTimeLeft + (xTimeWidth - bmRecording.Width()) / 2,
                        yEventNowTop + lineHeightTitle, bmRecording,
                        Theme.Color(clrSymbolRecActive),
                        Theme.Color(clrBackground));
      else
        osd->DrawBitmap(xTimeLeft + (xTimeWidth - bmTimer.Width()) / 2,
                        yEventNowTop + lineHeightTitle, bmTimer,
                        Theme.Color(clrSymbolTimerActive),
                        Theme.Color(clrBackground));
    }
    // draw shorttext
    osd->DrawText(xTextLeft, yEventNowTop + lineHeightTitle, e->ShortText(),
                  Theme.Color(clrMenuItemNotSelectableFg),
                  Theme.Color(clrBackground), pFontSubtitle, xTextWidth);

    // draw duration
    if ((now < total) && ((now / 60) > 0)) {
      osd->DrawText(xDurationLeft, yEventNowTop + lineHeightTitle, sNow,
                    Theme.Color(clrMenuItemNotSelectableFg),
                    Theme.Color(clrBackground), pFontSubtitle, xDurationWidth,
                    lineHeightSubtitle, taRight);
    }
    // draw timebar
    int xBarLeft = xBottomLeft + Roundness;
    int xBarWidth = (xFirstSymbol > xBarLeft ? (xFirstSymbol - Gap - xBarLeft) : 124);
    int x = xBarLeft + SmallGap + (int)(ceil((float)(now) / (float)(total) * (float)(xBarWidth - Gap - SmallGap)));
    x = std::min(x, xBarLeft + Gap + xBarWidth - SmallGap - 1);
    osd->DrawRectangle(xBarLeft, yBottomTop + SmallGap + SmallGap,
                       xBarLeft + Gap + xBarWidth - 1,
                       yBottomBottom - SmallGap - SmallGap - 1,
                       Theme.Color(clrBotProgBarBg));
    osd->DrawRectangle(xBarLeft + SmallGap,
                       yBottomTop + SmallGap + SmallGap + SmallGap, x,
                       yBottomBottom - SmallGap - SmallGap - SmallGap - 1,
                       Theme.Color(clrBotProgBarFg));
  }

  e = Following;                // Next event
  if (e) {
    char sLen[6];
    snprintf(sLen, sizeof(sLen), "%d'", e->Duration() / 60);

    int xDurationLeft = xEventNowRight - Gap - pFontTitle->Width(sLen);
    int xDurationWidth = xEventNowRight - Gap - xDurationLeft;
    int xTextLeft = xTimeLeft + xTimeWidth + BigGap;
    int xTextWidth = xDurationLeft - xTextLeft - BigGap;

    // draw start time
    osd->DrawText(xTimeLeft, yEventNextTop, e->GetTimeString(),
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
                  pFontTitle, xTimeWidth);
    // draw title
    osd->DrawText(xTextLeft, yEventNextTop, e->Title(),
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
                  pFontTitle, xTextWidth);
    // draw duration
    osd->DrawText(xDurationLeft, yEventNextTop, sLen,
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
                  pFontTitle, xDurationWidth, lineHeightTitle, taRight);
    if (e->HasTimer())
      osd->DrawBitmap(xTimeLeft + (xTimeWidth - bmTimer.Width()) / 2,
                      yEventNextTop + lineHeightTitle, bmTimer,
                      Theme.Color(clrSymbolTimerActive), Theme.Color(clrAltBackground));
    // draw shorttext
    osd->DrawText(xTextLeft, yEventNextTop + lineHeightTitle, e->ShortText(),
                  Theme.Color(clrMenuItemNotSelectableFg),
                  Theme.Color(clrAltBackground), pFontSubtitle, xTextWidth);
  }

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle && !fLocked)
    TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
  debug("cSkinEnigmaDisplayChannel::SetMessage()");

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle && !fLocked)
    TE_LOCK;
#endif

  // check if message
  if (Text) {
    // save osd region
    osd->SaveRegion(xMessageLeft, yMessageTop, xMessageRight - 1, yMessageBottom - 1);
    // draw message
    osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1,
                       yMessageBottom - 1, clrTransparent);
    osd->DrawRectangle(xMessageLeft, yMessageTop + SmallGap, xMessageRight - 1,
                       yMessageBottom - SmallGap - 1,
                       Theme.Color(clrButtonRedBg));
    osd->DrawText(xMessageLeft, yMessageTop + 2 * SmallGap, Text,
                  Theme.Color(clrMessageStatusFg + 2 * Type),
                  Theme.Color(clrMessageStatusBg + 2 * Type),
                  pFontMessage,
                  xMessageRight - xMessageLeft,
                  yMessageBottom - 2 * SmallGap - yMessageTop - 2 * SmallGap,
                  taCenter);
  } else {
    // restore saved osd region
    osd->RestoreRegion();
  }

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle && !fLocked)
    TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayChannel::Flush(void)
{
//  debug("cSkinEnigmaDisplayChannel::Flush()");

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked && fScrollTitle)
    TE_LOCK;
#endif

  cString date = DayDateTime();
  if ((strLastDate == NULL) || strcmp(strLastDate, (const char*)date) != 0) {
    free(strLastDate);
    strLastDate = strdup((const char*)date);
    // update date string
    int w = pFontDate->Width(date);
    osd->DrawText(xDateLeft, yTitleTop, date,
                  Theme.Color(clrTitleFg), Theme.Color(clrTitleBg),
                  pFontDate, w, yTitleBottom - yTitleTop, taCenter);
  }
  osd->Flush();

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle) {
    TE_UNLOCK;
    if (fLocked) {
      fLocked = false;
      TE_WAKEUP;
    }
  }
#endif
}

// --- cSkinEnigmaDisplayMenu -----------------------------------------------

class cSkinEnigmaDisplayMenu : public cSkinDisplayMenu, cSkinEnigmaOsd {
private:
  cOsd *osd;
  const cFont *pFontList;
  const cFont *pFontOsdTitle;

  char *strTitle;
  char *strLastDate;
  char *strTheme;
  bool isMainMenu;
  bool fShowLogo;
  bool fShowLogoDefault;
  bool fShowInfo;
  bool fSetupAreasDone;

  int xBodyLeft, xBodyRight, yBodyTop, yBodyBottom;
  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xButtonsLeft, xButtonsRight, yButtonsTop, yButtonsBottom;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;
  int xDateLeft, xDateRight, yDateTop, yDateBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom;
  int xInfoLeft, xInfoRight, yInfoTop, yInfoBottom;

  int lineHeight;
  int xItemLeft;
  int xItemRight;
  int yItemTop;

  int nMessagesShown;
  int nNumImageColors;

#ifndef DISABLE_ANIMATED_TEXT
  int nOldIndex;
  int idListItem[MaxTabs];
  int idTitle;
  bool fLocked;
  bool fScrollTitle;
  bool fScrollInfo;
  bool fScrollListItem;
  bool fScrollOther;
#endif

  void SetScrollbar(void);
  void SetupAreas(void);
  void SetColors(void);
  int DrawFlag(int x, int y, const tComponent *p);
  const char *GetPluginMainMenuName(const char *plugin);
  int ReadSizeVdr(const char *strPath);
  bool HasTabbedText(const char *s, int Tab);
  int getDateWidth(const cFont *pFontDate);

public:
  cSkinEnigmaDisplayMenu();
  virtual ~cSkinEnigmaDisplayMenu();
  virtual void Scroll(bool Up, bool Page);
  virtual int MaxItems(void);
  virtual void Clear(void);
  virtual void SetTitle(const char *Title);
  virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
  virtual void SetEvent(const cEvent *Event);
  virtual void SetRecording(const cRecording *Recording);
  virtual void SetText(const char *Text, bool FixedFont);
  virtual int GetTextAreaWidth(void) const;
  virtual const cFont *GetTextAreaFont(bool FixedFont) const;
  virtual void Flush(void);
  virtual void DrawTitle(const char *Title);
};

cSkinEnigmaDisplayMenu::cSkinEnigmaDisplayMenu(void)
{
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  fSetupAreasDone = false;
  setlocale(LC_TIME, tr("en_US"));
  osd = NULL;
  pFontList = EnigmaConfig.GetFont(FONT_LISTITEM);
  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);

  const cFont *pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  const cFont *pFontDetailsTitle = EnigmaConfig.GetFont(FONT_DETAILSTITLE);
  const cFont *pFontDetailsSubtitle = EnigmaConfig.GetFont(FONT_DETAILSSUBTITLE);
  const cFont *pFontDetailsDate = EnigmaConfig.GetFont(FONT_DETAILSDATE);
  
  strTitle = NULL;
  strLastDate = NULL;
  strTheme = strdup(Theme.Name());
  isMainMenu = false;
  fShowLogo = false;
#ifdef SKINENIGMA_NO_MENULOGO
  fShowLogoDefault = false;
#else
  fShowLogoDefault = EnigmaConfig.showSymbols && (EnigmaConfig.showSymbolsMenu || EnigmaConfig.showImages);
#endif
  fShowInfo = false;
  nMessagesShown = 0;
  nNumImageColors = 2;
#ifndef DISABLE_ANIMATED_TEXT
  for (int i = 0; i < MaxTabs; i++)
    idListItem[i] = -1;
  nOldIndex = -1;
  idTitle = -1;
  fScrollTitle = EnigmaConfig.useTextEffects && EnigmaConfig.scrollTitle;
  fScrollInfo = EnigmaConfig.useTextEffects && EnigmaConfig.scrollInfo;
  fScrollListItem = EnigmaConfig.useTextEffects && EnigmaConfig.scrollListItem;
  fScrollOther = EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther;
#endif

  int LogoHeight = std::max(std::max(pFontOsdTitle->Height(), pFontDate->Height()) + TitleDeco + pFontDetailsTitle->Height() + Gap + pFontDetailsSubtitle->Height(),
                            std::max(3 * pFontDate->Height(), 
                                     (EnigmaConfig.showImages ? std::max(EnigmaConfig.imageHeight, IconHeight) : IconHeight)
                                    )
                           );
  int LogoWidth = EnigmaConfig.showImages ? std::max(IconWidth, EnigmaConfig.imageWidth) : IconWidth;
  int RightColWidth = 0;
  if (fShowLogoDefault) {
    int nMainDateWidth = getDateWidth(pFontDate) + SmallGap + LogoWidth;
    cString date = DayDateTime();
    int nSubDateWidth = pFontDate->Width(date);
    RightColWidth = (SmallGap + Gap + std::max(nMainDateWidth, nSubDateWidth) + Gap) & ~0x07; // must be multiple of 8
  } else {
    cString date = DayDateTime();
    RightColWidth = (SmallGap + Gap + std::max(MIN_DATEWIDTH + LogoWidth, pFontDate->Width(date)) + Gap) & ~0x07; // must be multiple of 8
  }

  const cFont *pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);
  int MessageHeight = 2 * SmallGap + pFontMessage->Height() + 2 * SmallGap;

  // title bar
  xTitleLeft = 0;
  xTitleRight = OsdSize.w - RightColWidth;
  yTitleTop = 0;
  yTitleBottom = std::max(pFontOsdTitle->Height(), pFontDate->Height());
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  // help buttons
  xButtonsLeft = xTitleLeft;
  xButtonsRight = OsdSize.w;
  yButtonsTop = OsdSize.h - EnigmaConfig.GetFont(FONT_HELPKEYS)->Height();
  yButtonsBottom = OsdSize.h;
  // content area with items
  xBodyLeft = xTitleLeft;
  xBodyRight = xTitleRight;
  yBodyTop = yTitleDecoBottom + TitleDecoGap2;
  yBodyBottom = yButtonsTop - SmallGap;
  // message area
  xMessageLeft = xBodyLeft;
  xMessageRight = OsdSize.w;
  yMessageBottom = yButtonsTop - SmallGap;
  yMessageTop = yMessageBottom - MessageHeight + SmallGap;      // No extra SmallGap at bottom as there's already the Gap above the buttons
  // logo box
  xLogoLeft = OsdSize.w - LogoWidth;
  xLogoRight = OsdSize.w;
  yLogoTop = yTitleTop;
  yLogoBottom = yLogoTop + LogoHeight + SmallGap;
  // info box
  xInfoLeft = OsdSize.w - RightColWidth;
  xInfoRight = OsdSize.w;
  yInfoTop = yLogoBottom;
  yInfoBottom = yBodyBottom;
  // date box
  xDateLeft = xTitleRight;
  xDateRight = OsdSize.w;
  yDateTop = yTitleTop;
  yDateBottom = yLogoBottom;

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y);

  tArea Areas[] = { {xTitleLeft,   yTitleTop, xMessageRight - 1, yButtonsBottom - 1, 8} };
  if (EnigmaConfig.singleArea8Bpp && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayMenu: using %dbpp single area", Areas[0].bpp);
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    nNumImageColors = 230; //TODO: find correct number of colors
  } else {
    debug("cSkinEnigmaDisplayMenu: using multiple areas");
    tArea Areas[] = { {xTitleLeft,   yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2}, //title area
                      {xBodyLeft,    yBodyTop, xBodyRight - 1, yInfoTop + pFontDetailsDate->Height() - 1, 2}, //body area (beside date/logo/symbols area)
                      {xDateLeft,    yDateTop, xLogoRight - 1, yInfoTop - 1, 4}, //date/logo area
                      {xInfoLeft,    yInfoTop, xInfoRight - 1, yInfoTop + pFontDetailsDate->Height() - 1, 4}, //area for symbols in event/recording info
                      {xBodyLeft,    yInfoTop + pFontDetailsDate->Height(), xInfoRight - 1, yMessageTop - 1, 2}, // body/info area (below symbols area)
                      {xMessageLeft, yMessageTop, xButtonsRight - 1, yButtonsBottom - 1, 4} //buttons/message area
    };
 
    eOsdError rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
    if (rc == oeOk)
      osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    else {
      error("cSkinEnigmaDisplayMenu: CanHandleAreas() [1] returned %d", rc);
      delete osd;
      osd = NULL;
      throw 1;
      return;
    }

    nNumImageColors = 13; // "16 available colors" - "clrTransparent" - "clrLogoBg" - "clrMenuTxtFg"
  }

  lineHeight = pFontList->Height();
  xItemLeft = xBodyLeft + (EnigmaConfig.showMarker ? lineHeight : SmallGap);
  xItemRight = (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - (EnigmaConfig.showMarker ? lineHeight : SmallGap);
  int numItems = (yBodyBottom - yBodyTop) / lineHeight;
  yItemTop = yBodyTop + (yBodyBottom - yBodyTop - numItems * lineHeight) / 2;

#ifndef DISABLE_ANIMATED_TEXT
  fLocked = false;
  if (fScrollTitle || fScrollInfo || fScrollListItem || fScrollOther) {
    TE_START(osd);
    fLocked = true;
  }
#endif
}

void cSkinEnigmaDisplayMenu::SetColors(void)
{
  debug("cSkinEnigmaDisplayMenu::SetColors()");

  if (osd->GetBitmap(1) == NULL) { //single area
    cBitmap *bitmap = osd->GetBitmap(0);
    if (bitmap) {
      bitmap->Reset();
      bitmap->SetColor( 0, clrTransparent);
      bitmap->SetColor( 1, Theme.Color(clrTitleBg));
      bitmap->SetColor( 2, Theme.Color(clrTitleFg));
      bitmap->SetColor( 3, Theme.Color(clrTitleShadow));
      bitmap->SetColor( 4, Theme.Color(clrLogoBg));
      bitmap->SetColor( 5, Theme.Color(clrBackground));
      bitmap->SetColor( 6, Theme.Color(clrAltBackground));
      bitmap->SetColor( 7, Theme.Color(clrMenuTxtFg));
      bitmap->SetColor( 8, Theme.Color(clrBottomBg));
      bitmap->SetColor( 9, Theme.Color(clrButtonRedBg));
      bitmap->SetColor(10, Theme.Color(clrButtonRedFg));
      bitmap->SetColor(11, Theme.Color(clrButtonGreenBg));
      bitmap->SetColor(12, Theme.Color(clrButtonGreenFg));
      bitmap->SetColor(13, Theme.Color(clrButtonYellowBg));
      bitmap->SetColor(14, Theme.Color(clrButtonYellowFg));
      bitmap->SetColor(15, Theme.Color(clrButtonBlueBg));
      bitmap->SetColor(16, Theme.Color(clrButtonBlueFg));
    }
    return;
  }

  cBitmap *bitmap = osd->GetBitmap(0);
  if (bitmap) { //title area
    bitmap->Reset();
    bitmap->SetColor(0, clrTransparent);
    bitmap->SetColor(1, Theme.Color(clrTitleBg));
    bitmap->SetColor(2, Theme.Color(clrTitleFg));
    bitmap->SetColor(3, Theme.Color(clrTitleShadow));
  }
  bitmap = osd->GetBitmap(1);
  if (bitmap) { //body area (beside date/logo/ area)
    bitmap->Reset();
    bitmap->SetColor(0, Theme.Color(clrBackground));
    bitmap->SetColor(1, Theme.Color(clrAltBackground));
    bitmap->SetColor(2, Theme.Color(clrMenuItemSelectableFg));
    bitmap->SetColor(3, Theme.Color(clrMenuItemNotSelectableFg));
  }
  bitmap = osd->GetBitmap(2);
  if (bitmap) { //date/logo area
    bitmap->Reset();
    if (fShowLogo) {
      bitmap->SetColor(0, clrTransparent);
      bitmap->SetColor(1, Theme.Color(clrLogoBg));
      bitmap->SetColor(2, Theme.Color(clrMenuTxtFg));
    } else {
      bitmap->SetColor(0, Theme.Color(clrBackground));
      bitmap->SetColor(1, Theme.Color(clrAltBackground));
      bitmap->SetColor(2, Theme.Color(clrMenuItemSelectableFg));
      bitmap->SetColor(3, Theme.Color(clrMenuItemNotSelectableFg));
      bitmap->SetColor(4, clrTransparent);
      bitmap->SetColor(5, Theme.Color(clrLogoBg));
      bitmap->SetColor(6, Theme.Color(clrMenuTxtFg));
    }
  }
  bitmap = osd->GetBitmap(3);
  if (bitmap) { //area for symbols in event/recording info
    bitmap->Reset();
    bitmap->SetColor(0, Theme.Color(clrBackground));
    bitmap->SetColor(1, Theme.Color(clrAltBackground));
    bitmap->SetColor(2, Theme.Color(clrMenuItemSelectableFg));
    bitmap->SetColor(3, Theme.Color(clrMenuItemNotSelectableFg));
  }
  bitmap = osd->GetBitmap(4);
  if (bitmap) { //body/info area (below symbols area)
    bitmap->Reset();
    bitmap->SetColor(0, Theme.Color(clrBackground));
    bitmap->SetColor(1, Theme.Color(clrAltBackground));
    bitmap->SetColor(2, Theme.Color(clrMenuItemSelectableFg));
    bitmap->SetColor(3, Theme.Color(clrMenuItemNotSelectableFg));
  }
  bitmap = osd->GetBitmap(5);
  if (bitmap) { //buttons/message area
    bitmap->Reset();
    bitmap->SetColor( 0, Theme.Color(clrBackground));
    bitmap->SetColor( 1, Theme.Color(clrAltBackground));
    bitmap->SetColor( 2, Theme.Color(clrMenuItemSelectableFg));
    bitmap->SetColor( 3, Theme.Color(clrMenuItemNotSelectableFg));
    bitmap->SetColor( 4, Theme.Color(clrBottomBg));
    bitmap->SetColor( 5, Theme.Color(clrButtonRedBg));
    bitmap->SetColor( 6, Theme.Color(clrButtonRedFg));
    bitmap->SetColor( 7, Theme.Color(clrButtonGreenBg));
    bitmap->SetColor( 8, Theme.Color(clrButtonGreenFg));
    bitmap->SetColor( 9, Theme.Color(clrButtonYellowBg));
    bitmap->SetColor(10, Theme.Color(clrButtonYellowFg));
    bitmap->SetColor(11, Theme.Color(clrButtonBlueBg));
    bitmap->SetColor(12, Theme.Color(clrButtonBlueFg));
    bitmap->SetColor(13, clrTransparent);
    // color 14 reserved for "clrMessageStatusFg + 2 * Type"
    // color 15 reserved for "clrMessageStatusBg + 2 * Type"
  }
}

void cSkinEnigmaDisplayMenu::SetupAreas(void)
{
  //Must be TE_LOCKed by caller

  debug("cSkinEnigmaDisplayMenu::SetupAreas() %d %d %d", isMainMenu, fShowLogo, fShowInfo);
  fSetupAreasDone = true;
  SetColors();

#ifndef DISABLE_ANIMATED_TEXT
  EnigmaTextEffects.Clear();
  idTitle = -1;
  nOldIndex = -1;
  for (int i = 0; i < MaxTabs; i++)
    idListItem[i] = -1;
#endif

  xItemRight = (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - (EnigmaConfig.showMarker ? lineHeight : SmallGap);

  // clear transparent areas (without date/logo borders)
  osd->DrawRectangle(xBodyLeft, yTitleDecoBottom, xDateLeft - 1, yBodyTop - 1, clrTransparent);
  osd->DrawRectangle(xBodyLeft, yMessageBottom, xInfoRight - 1, yButtonsTop - 1, clrTransparent);

#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle) {
    idTitle = TE_TITLE(osd, idTitle, strTitle, xTitleRight - xTitleLeft - Roundness, this);
  } else
#endif
    DrawTitle(strTitle);

  // draw date area
  if (fShowLogo) {
    osd->DrawRectangle(xDateLeft + SmallGap, yDateTop, xLogoRight - 1, yDateBottom - SmallGap - 1, Theme.Color(clrLogoBg));
    //draw borders
    osd->DrawRectangle(xDateLeft, yDateTop, xDateLeft + SmallGap - 1, yLogoBottom - SmallGap - 1, clrTransparent);
    osd->DrawRectangle(xDateLeft, yLogoBottom - SmallGap, xInfoRight - 1, yLogoBottom - 1, clrTransparent);
  } else {
    osd->DrawRectangle(xDateLeft + SmallGap, yDateTop, xLogoRight - 1, yTitleDecoBottom - 1, Theme.Color(clrLogoBg));
    //draw borders
    osd->DrawRectangle(xDateLeft, yDateTop, xDateLeft + SmallGap - 1, yTitleDecoBottom - 1, clrTransparent);
    osd->DrawRectangle(xDateLeft, yTitleDecoBottom, xInfoRight - 1, yBodyTop - 1, clrTransparent);
  }

  // draw body area
  osd->DrawRectangle(xBodyLeft, yBodyTop, xBodyRight - 1, yBodyBottom - 1, Theme.Color(clrBackground));

  // draw info area
  if (fShowInfo) {
#ifdef CLEAR_BUG_WORKAROUND
    osd->DrawRectangle(xInfoLeft, fShowLogo ? yInfoTop : yBodyTop, xInfoRight - 2, yInfoBottom - 1, Theme.Color(clrAltBackground));
#else
    osd->DrawRectangle(xInfoLeft, fShowLogo ? yInfoTop : yBodyTop, xInfoRight - 1, yInfoBottom - 1, Theme.Color(clrAltBackground));
#endif

    int x = xInfoLeft + Gap;
    int y = fShowLogo ? yInfoTop : yBodyTop;
    int w = xInfoRight - x;
#ifdef CLEAR_BUG_WORKAROUND
    w--;
#endif
    int yMaxHeight = yInfoBottom;

#ifdef SKINENIGMA_HAVE_EPGSEARCH
    if (EnigmaConfig.showTimerConflicts) {
      cPlugin *p = cPluginManager::GetPlugin("epgsearch");
      if (p) {
        Epgsearch_lastconflictinfo_v1_0 *serviceData = new Epgsearch_lastconflictinfo_v1_0;
        if (serviceData) {
          serviceData->nextConflict = 0;
          serviceData->relevantConflicts = 0;
          serviceData->totalConflicts = 0;

          p->Service("Epgsearch-lastconflictinfo-v1.0", serviceData);
          if (serviceData->relevantConflicts > 0) {
            const cFont *pFontInfoWarnHeadline = EnigmaConfig.GetFont(FONT_INFOWARNHEADLINE);
            const cFont *pFontInfoWarnText = EnigmaConfig.GetFont(FONT_INFOWARNTEXT);
            int yWarning = yMaxHeight -
                           pFontInfoWarnHeadline->Height() -
                           2 * pFontInfoWarnText->Height(); // Needed space for warning
            yMaxHeight = yWarning;
            osd->DrawRectangle(xInfoLeft, yWarning, xInfoRight - 1, yWarning + SmallGap + 1, Theme.Color(clrBackground));
            yWarning += pFontInfoWarnText->Height() / 2;
#ifndef DISABLE_ANIMATED_TEXT
            TE_BLINK(osd, -1, xInfoLeft, yWarning, tr("WARNING"),
                     Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                     FONT_INFOWARNHEADLINE,
                     w, pFontInfoWarnHeadline->Height(), taCenter);
#else
            osd->DrawText(xInfoLeft, yWarning, tr("WARNING"),
                     Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                     pFontInfoWarnHeadline,
                     w, pFontInfoWarnHeadline->Height(), taCenter);
#endif
            yWarning += (int)(1.5 * pFontInfoWarnHeadline->Height());

            char *info;
            asprintf(&info, "%d %s", serviceData->relevantConflicts, (serviceData->relevantConflicts == 1 ? tr("Timer conflict") : tr("Timer conflicts")));
            osd->DrawText(x, yWarning, info, Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                          pFontInfoWarnText,
                          w, pFontInfoWarnText->Height(), taCenter);
            yWarning += pFontInfoWarnText->Height();
            free(info);
          }
          delete serviceData;
        }
      }
    }
#endif //SKINENIGMA_HAVE_EPGSEARCH

    if (Timers.GetNextActiveTimer()) {
      const cFont *pFontInfoTimerHeadline = EnigmaConfig.GetFont(FONT_INFOTIMERHEADLINE);
      const cFont *pFontInfoTimerText = EnigmaConfig.GetFont(FONT_INFOTIMERTEXT);
      int h = pFontInfoTimerHeadline->Height();
      // Show next active timers
      y += h / 2;
      osd->DrawText(x, y, tr("TIMERS"), Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                    pFontInfoTimerHeadline,
                    w, pFontInfoTimerHeadline->Height(), taCenter);
      y += h + h / 2;
      EnigmaStatus.UpdateActiveTimers();

      h = pFontInfoTimerText->Height();
      for (tTimer *timer = EnigmaStatus.mTimers.First(); timer; timer = EnigmaStatus.mTimers.Next(timer)) {
        if (y + 2 * h + SmallGap + 1 + h / 2 > yMaxHeight)
          break; // don't overwrite warning or help buttons

        if (timer->isRecording) {
        osd->DrawBitmap(x, y + (h - bmRecording.Height()) / 2, bmRecording, Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground));
        }

#ifndef DISABLE_ANIMATED_TEXT
        if (fScrollInfo)
          TE_MARQUEE(osd, -1, x + (timer->isRecording ? (bmRecording.Width() + Gap) : 0),
                     y, timer->title.c_str(),
                     Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                     FONT_INFOTIMERTEXT, w, h);
        else
#endif
          osd->DrawText(x + (timer->isRecording ? (bmRecording.Width() + Gap) : 0),
                        y, timer->title.c_str(),
                        Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                        pFontInfoTimerText, w, h);
        y += h;
        char* info = NULL;
        if (timer->isRecording) {
          asprintf(&info, "- %s / %s", *TimeString(timer->stopTime), timer->channelName.c_str());
        } else {
          asprintf(&info, "%d. %s / %s", timer->startDay, *TimeString(timer->startTime), timer->channelName.c_str());
        }
        osd->DrawText(x, y, info, Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground), pFontInfoTimerText, w, h);
        free(info);
        y += h;
        osd->DrawRectangle(x, y + SmallGap, xInfoRight - Gap - 1, y + SmallGap + 1, Theme.Color(clrMenuItemSelectableFg));
        y += SmallGap + 1 + h / 2;
      }
    }

#ifdef CLEAR_BUG_WORKAROUND
    osd->DrawRectangle(xInfoRight - 1, (fShowLogo ? yInfoTop : yBodyTop) - SmallGap, xInfoRight - 1, yInfoBottom - 1, Theme.Color(clrMenuItemNotSelectableFg));
#endif
  } else { // !fShowInfo
    osd->DrawRectangle(xInfoLeft, fShowLogo ? yInfoTop : yBodyTop, xInfoRight - 1, yInfoBottom - 1, Theme.Color(clrBackground));
  }
}

cSkinEnigmaDisplayMenu::~cSkinEnigmaDisplayMenu()
{
#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle || fScrollInfo || fScrollListItem || fScrollOther) {
    if (!fLocked) TE_LOCK;
    TE_STOP;
  }
#endif
  free(strTheme);
  free(strTitle);
  free(strLastDate);
  delete osd;
}

void cSkinEnigmaDisplayMenu::SetScrollbar(void)
{
  //Must be TE_LOCKed by caller

  // check if scrollbar is needed
  if (textScroller.CanScroll()) {
    int yt = textScroller.Top();
    int yb = yt + textScroller.Height();
    int st = yt + ScrollbarHeight + Gap;
    int sb = yb - ScrollbarHeight - Gap;
    int tt = st + (sb - st) * textScroller.Offset() / textScroller.Total();
    int tb = tt + (sb - st) * textScroller.Shown() / textScroller.Total();
    int xl = textScroller.Width() + SmallGap;
    // arrow up
    osd->DrawRectangle(xl, yt, xl + ScrollbarWidth, yt + SmallGap,
                       textScroller.CanScrollUp() ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));    
    osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, yt + SmallGap, xl + ScrollbarWidth, yt + ScrollbarHeight,
                       textScroller.CanScrollUp() ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));
    // draw background of scrollbar
    osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, st, xl + ScrollbarWidth, sb, Theme.Color(clrAltBackground));
    // draw visible area of scrollbar
    osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, tt, xl + ScrollbarWidth, tb, Theme.Color(clrMenuTxtFg));
    // arrow down
    osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, yb - ScrollbarHeight, xl + ScrollbarWidth, yb - SmallGap,
                       textScroller.CanScrollDown() ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));
    osd->DrawRectangle(xl, yb - SmallGap, xl + ScrollbarWidth, yb,
                       textScroller.CanScrollDown() ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));
  }
}

void cSkinEnigmaDisplayMenu::Scroll(bool Up, bool Page)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  cSkinDisplayMenu::Scroll(Up, Page);
  SetScrollbar();
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

int cSkinEnigmaDisplayMenu::MaxItems(void)
{
  // max number of items
  return (yBodyBottom - yBodyTop) / lineHeight;
}

void cSkinEnigmaDisplayMenu::Clear(void)
{
  debug("cSkinEnigmaDisplayMenu::Clear() %d %d %d", isMainMenu, fShowLogo, fShowInfo);

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
  pFontList = EnigmaConfig.GetFont(FONT_LISTITEM, pFontList); //TODO? get current font which might have been patched meanwhile
  nOldIndex = -1;
  for (int i = MaxTabs - 1; i >= 0; i--) {
    if (idListItem[i] >= 0) {
      EnigmaTextEffects.ResetText(idListItem[i], Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrBackground), !fLocked);
      idListItem[i] = -1;
    }
  }
#endif

  textScroller.Reset();

  if (strcmp(strTheme, Theme.Name()) != 0) {
    free(strTheme);
    strTheme = strdup(Theme.Name());

    SetupAreas();
  } else {
    // clear items area
    if (fShowLogo || fShowInfo) {
      osd->DrawRectangle(xBodyLeft, yBodyTop, xBodyRight - 1, yBodyBottom - 1, Theme.Color(clrBackground));
      //TODO? clear logo and/or info area?
    } else {
      osd->DrawRectangle(xBodyLeft, yBodyTop, xInfoRight - 1, yInfoBottom - 1, Theme.Color(clrBackground));
    }
  }
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif

}

void cSkinEnigmaDisplayMenu::SetTitle(const char *Title)
{
  debug("cSkinEnigmaDisplayMenu::SetTitle(%s)", Title);

  bool fTitleChanged = false;
  if (Title && strTitle) {
    if (strcmp(Title, strTitle) != 0) {
      fTitleChanged = true;
      free(strTitle);
      strTitle = strdup(Title);
    }
  } else {
    free(strTitle);
    if (Title)
      strTitle = strdup(Title);
    else
      strTitle = NULL;
  }

  char *strTitlePrefix = NULL;
  asprintf(&strTitlePrefix, "%s  -  ", tr("VDR"));

  if ((Title == NULL) || (isMainMenu && strncmp(strTitlePrefix, Title, strlen(strTitlePrefix)) == 0)) {
#ifndef DISABLE_ANIMATED_TEXT
    if (!fLocked) TE_LOCK;
    if (fScrollTitle) {
      idTitle = TE_TITLE(osd, idTitle, strTitle, xTitleRight - xTitleLeft - Roundness, this);
    } else
#endif
      DrawTitle(Title);
#ifndef DISABLE_ANIMATED_TEXT
    if (!fLocked) TE_UNLOCK;
#endif
  } else {
    bool old_isMainMenu = isMainMenu;
    bool old_fShowLogo = fShowLogo;
    bool old_fShowInfo = fShowInfo;

    if (strTitle == NULL || strncmp(strTitlePrefix, strTitle, strlen(strTitlePrefix)) == 0) {
      isMainMenu = true;
      fShowLogo = fShowLogoDefault ? EnigmaConfig.showSymbolsMenu : false;
      fShowInfo = EnigmaConfig.showInfo;
    } else {
      isMainMenu = false;
      fShowLogo = false;
      fShowInfo = false;
    }

#ifndef DISABLE_ANIMATED_TEXT
    if (fTitleChanged) {
      if (!fLocked) TE_LOCK;
      EnigmaTextEffects.Clear();
      idTitle = -1;
      nOldIndex = -1;
      for (int i = 0; i < MaxTabs; i++)
        idListItem[i] = -1;
      if (!fLocked) TE_UNLOCK;
    }
#endif

    if (!fSetupAreasDone
        || old_isMainMenu != isMainMenu
        || old_fShowLogo != fShowLogo
        || old_fShowInfo != fShowInfo) {

#ifndef DISABLE_ANIMATED_TEXT
      if (!fLocked) TE_LOCK;
#endif
      SetupAreas();
#ifndef DISABLE_ANIMATED_TEXT
      if (!fLocked) TE_UNLOCK;
#endif
    } else {
#ifndef DISABLE_ANIMATED_TEXT
      if (!fLocked) TE_LOCK;
      if (fScrollTitle) {
        idTitle = TE_TITLE(osd, idTitle, strTitle, xTitleRight - xTitleLeft - Roundness, this);
      } else
#endif
        DrawTitle(Title);
#ifndef DISABLE_ANIMATED_TEXT
      if (!fLocked) TE_UNLOCK;
#endif
    }
  }
  free (strTitlePrefix);

  free(strLastDate);
  strLastDate = NULL;
}

void cSkinEnigmaDisplayMenu::DrawTitle(const char *Title)
{
  //Must be TE_LOCKed by caller

  // draw titlebar
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleBottom, xTitleRight - 1, yTitleDecoTop - 1, clrTransparent);
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1, yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  // draw rounded left corner of title bar
  osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1, yTitleTop + Roundness - 1, clrTransparent, -2);

  if (Title) {
    pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE, pFontOsdTitle); //TODO? get current font which might have been patched meanwhile
    int y = yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2;
    // draw title with shadow
    osd->DrawText(xTitleLeft + Roundness + 3, y + 3, Title,
                  Theme.Color(clrTitleShadow), clrTransparent,
                  pFontOsdTitle,
                  xTitleRight - xTitleLeft - Roundness - 3,
                  yTitleBottom - y - 3);
    osd->DrawText(xTitleLeft + Roundness, y, Title,
                  Theme.Color(clrTitleFg), clrTransparent,
                  pFontOsdTitle,
                  xTitleRight - xTitleLeft - Roundness - 3,
                  yTitleBottom - y);
  }
}

void cSkinEnigmaDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
  debug("cSkinEnigmaDisplayMenu::SetButtons(%s, %s, %s, %s)", Red, Green, Yellow, Blue);

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  const cFont *pFontHelpKeys = EnigmaConfig.GetFont(FONT_HELPKEYS);
  int w = (xButtonsRight - xButtonsLeft) / 4;
  int t3 = xButtonsLeft + xButtonsRight - xButtonsLeft - w;
  int t2 = t3 - w;
  int t1 = t2 - w;
  int t0 = xButtonsLeft;

  // draw color buttons
  if (Red)
    osd->DrawText(t0, yButtonsTop, Red, Theme.Color(clrButtonRedFg),
                  Theme.Color(clrButtonRedBg), pFontHelpKeys, t1 - t0 + 1,
                  yButtonsBottom - yButtonsTop, taCenter);
  else {
    osd->DrawRectangle(t0, yButtonsTop, Green ? t1 - 1 : t1, yButtonsBottom - 1, Theme.Color(clrBottomBg));
    osd->DrawEllipse(t0, yButtonsBottom - Roundness, t0 + Roundness - 1, yButtonsBottom - 1, clrTransparent, -3);
  }
  if (Green)
    osd->DrawText(t1, yButtonsTop, Green, Theme.Color(clrButtonGreenFg),
                  Theme.Color(clrButtonGreenBg), pFontHelpKeys, w,
                  yButtonsBottom - yButtonsTop, taCenter);
  else
    osd->DrawRectangle(t1, yButtonsTop, Yellow ? t1 + w : t2 - 1, yButtonsBottom - 1, Theme.Color(clrBottomBg));

  if (Yellow)
    osd->DrawText(t2, yButtonsTop, Yellow, Theme.Color(clrButtonYellowFg),
                  Theme.Color(clrButtonYellowBg), pFontHelpKeys, w,
                  yButtonsBottom - yButtonsTop, taCenter);
  else
    osd->DrawRectangle(t2, yButtonsTop, Blue ? t2 + w : t3 - 1, yButtonsBottom - 1, Theme.Color(clrBottomBg));
  if (Blue)
    osd->DrawText(t3, yButtonsTop, Blue, Theme.Color(clrButtonBlueFg),
                  Theme.Color(clrButtonBlueBg), pFontHelpKeys, w,
                  yButtonsBottom - yButtonsTop, taCenter);
  else {
    osd->DrawRectangle(t3, yButtonsTop, xButtonsRight, yButtonsBottom, Theme.Color(clrBottomBg));
    osd->DrawEllipse(xButtonsRight - Roundness, yButtonsBottom - Roundness,
                     xButtonsRight - 1, yButtonsBottom - 1, clrTransparent, -4);
  }
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayMenu::SetMessage(eMessageType Type, const char *Text)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  // check if message
  if (Text) {
#ifndef DISABLE_ANIMATED_TEXT
    EnigmaTextEffects.PauseEffects(yMessageTop);
#endif
    // save osd region
    if (nMessagesShown == 0)
      osd->SaveRegion(xMessageLeft, yMessageTop, xMessageRight - 1, yMessageBottom - 1);

    const cFont *pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);
    // draw message
    osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1,
                       yMessageBottom - 1, clrTransparent);
    osd->DrawRectangle(xMessageLeft, yMessageTop + SmallGap, xMessageRight - 1,
                       yMessageBottom - 1, Theme.Color(clrButtonRedBg));
    osd->DrawText(xMessageLeft, yMessageTop + 2 * SmallGap, Text,
                  Theme.Color(clrMessageStatusFg + 2 * Type),
                  Theme.Color(clrMessageStatusBg + 2 * Type),
                  pFontMessage,
                  xMessageRight - xMessageLeft,
                  yMessageBottom - SmallGap - yMessageTop - 2 * SmallGap,
                  taCenter);
    nMessagesShown++;
  } else {
    if (nMessagesShown > 0)
      nMessagesShown--;
    // restore saved osd region
    if (nMessagesShown == 0)
      osd->RestoreRegion();
#ifndef DISABLE_ANIMATED_TEXT
    EnigmaTextEffects.PauseEffects();
#endif
  }
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

bool cSkinEnigmaDisplayMenu::HasTabbedText(const char *s, int Tab)
{
  if (!s)
    return false;

  const char *b = strchrnul(s, '\t');
  while (*b && Tab-- > 0) {
    b = strchrnul(b + 1, '\t');
  }
  if (!*b)
    return (Tab <= 0) ? true : false;
  return true;
}

void cSkinEnigmaDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable)
{
  debug("cSkinEnigmaDisplayMenu::SetItem(%s, %d, %d, %d)", Text, Index, Current, Selectable);

  int y = yItemTop + Index * lineHeight;
  if (nMessagesShown > 0 && y >= yMessageTop)
    return; //Don't draw above messages

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  tColor ColorFg, ColorBg;
  // select colors
  if (Current) {
    ColorFg = Theme.Color(clrMenuItemSelectableFg);
    ColorBg = Theme.Color(clrAltBackground);
  } else {
    if (Selectable) {
      ColorFg = Theme.Color(clrMenuItemSelectableFg);
      ColorBg = Theme.Color(clrBackground);
    } else {
      ColorFg = Theme.Color(clrMenuItemNotSelectableFg);
      ColorBg = Theme.Color(clrBackground);
    }
  }

  pFontList = EnigmaConfig.GetFont(FONT_LISTITEM, pFontList); //TODO? get current font which might have been patched meanwhile
#ifndef DISABLE_ANIMATED_TEXT
  if (!Current && Index == nOldIndex) {
    for (int i = MaxTabs - 1; i >= 0; i--) {
      if (idListItem[i] >= 0) {
        EnigmaTextEffects.ResetText(idListItem[i], ColorFg, ColorBg);
        idListItem[i] = -1;
      }
    }
  } else if (Current) {
    nOldIndex = Index;
  }
#endif

  // this should prevent menu flickering
  osd->DrawRectangle(xItemLeft, y + lineHeight / 2, xItemLeft + 1, y + lineHeight / 2 + 1, ColorBg);
  osd->DrawRectangle(xItemRight - 2, y + lineHeight / 2, xItemRight - 1, y + lineHeight / 2 + 1, ColorBg);

  if (EnigmaConfig.showMarker) {
    osd->DrawRectangle(xBodyLeft, y, xItemLeft - 1, y + lineHeight - 1, ColorBg);
    osd->DrawEllipse(xBodyLeft + MarkerGap, y + MarkerGap, xBodyLeft + lineHeight - MarkerGap, y + lineHeight - MarkerGap, Current ? ColorFg : ColorBg);
    osd->DrawRectangle(xItemRight, y, (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - 1, y + lineHeight - 1, ColorBg);
  }
  // draw item
  for (int i = 0; i < MaxTabs; i++) {
    const char *s = GetTabbedText(Text, i);
    if (s) {
      char buffer[9];
      int xt = xItemLeft + Tab(i);
      bool iseventinfo = false;
      bool isnewrecording = false;
      bool isprogressbar = false;
      int now = 0, total = 0;

      // check if event info symbol: "tTV*" "R"
      if (EnigmaConfig.showListSymbols) {
        // check if event info characters
        if (strlen(s) == 3 && strchr(" tTR", s[0])
            && strchr(" V", s[1]) && strchr(" *", s[2])) {
          // update status
          iseventinfo = true;
        }

        // check if new recording: "01.01.06*", "10:10*"
        if (!iseventinfo &&
            (strlen(s) == 6 && s[5] == '*' && s[2] == ':' && isdigit(*s)
             && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4)))
            || (strlen(s) == 9 && s[8] == '*' && s[5] == '.' && s[2] == '.'
                && isdigit(*s) && isdigit(*(s + 1)) && isdigit(*(s + 3))
                && isdigit(*(s + 4)) && isdigit(*(s + 6))
                && isdigit(*(s + 7)))) {
          // update status
          isnewrecording = true;
          // make a copy
          strncpy(buffer, s, strlen(s));
          // remove the '*' character
          buffer[strlen(s) - 1] = '\0';
        }
      }

      // check if progress bar: "[|||||||   ]"
      if (!iseventinfo && !isnewrecording && EnigmaConfig.showProgressbar &&
          (strlen(s) > 5 && s[0] == '[' && s[strlen(s) - 1] == ']')) {
        const char *p = s + 1;
        // update status
        isprogressbar = true;
        for (; *p != ']'; ++p) {
          // check if progressbar characters
          if (*p == ' ' || *p == '|') {
            // update counters
            ++total;
            if (*p == '|')
              ++now;
          } else {
            // wrong character detected; not a progressbar
            isprogressbar = false;
            break;
          }
        }
      }

      if (iseventinfo) {
        int evx = xt + Gap;
        const char *p = s;
        // draw background
        osd->DrawRectangle(xt, y, xItemRight, y + lineHeight - 1, ColorBg);
        // draw symbols
        for (; *p; ++p) {
          switch (*p) {
            case 't':
              // partial timer event
              osd->DrawBitmap(evx, y + (lineHeight - bmEventPartTimer.Height()) / 2, bmEventPartTimer, ColorFg, ColorBg);
              evx += bmEventPartTimer.Width();
              break;
            case 'T':
              // timer event
              osd->DrawBitmap(evx, y + (lineHeight - bmEventTimer.Height()) / 2, bmEventTimer, ColorFg, ColorBg);
              evx += bmEventTimer.Width();
              break;
            case 'R':
              // recording event (epgsearch)
              osd->DrawBitmap(evx, y + (lineHeight - bmEventRecording.Height()) / 2, bmEventRecording, ColorFg, ColorBg);
              evx += bmEventRecording.Width();
              break;
            case 'V':
              // vps event
              osd->DrawBitmap(evx, y + (lineHeight - bmEventVPS.Height()) / 2, bmEventVPS, ColorFg, ColorBg);
              evx += bmEventVPS.Width();
              break;
            case '*':
              // running event
              osd->DrawBitmap(evx, y + (lineHeight - bmEventRunning.Height()) / 2, bmEventRunning, ColorFg, ColorBg);
              evx += bmEventRunning.Width();
              break;
            case ' ':
            default:
              // let's ignore space character
              break;
          }
        }
      } else if (isnewrecording) {
        // draw text
        osd->DrawText(xt, y, buffer, ColorFg, ColorBg, pFontList, xItemRight - xt);
        // draw symbol
        osd->DrawBitmap(xt + pFontList->Width(buffer), y + (lineHeight - bmRecordingNew.Height()) / 2, bmRecordingNew, ColorFg, ColorBg);
      } else if (isprogressbar) {
        // define x coordinates of progressbar
        int px0 = xt;
        int px1 = (Selectable ? (Tab(i + 1) ? Tab(i + 1) : xItemRight) : xItemRight) - 1;
        int px = px0 + std::max((int)((float) now * (float) (px1 - px0) / (float) total), ListProgressBarBorder);
        // define y coordinates of progressbar
        int py0 = y + ListProgressBarGap;
        int py1 = y + lineHeight - ListProgressBarGap;
        // draw background
        osd->DrawRectangle(px0, y, /*TODO? px1 */ xItemRight - 1, y + lineHeight - 1, ColorBg);
        // draw progressbar
        osd->DrawRectangle(px0,    py0, px,  py1, ColorFg);
        osd->DrawRectangle(px + 1, py0, px1, py0 + ListProgressBarBorder, ColorFg);
        osd->DrawRectangle(px + 1, py1 - ListProgressBarBorder, px1, py1, ColorFg);
        osd->DrawRectangle(px1 - ListProgressBarBorder, py0, px1, py1, ColorFg);
      } else {
        int w = (Tab(i + 1) && HasTabbedText(Text, i + 1) ? (xItemLeft + Tab(i + 1)) : xItemRight) - xt;
        //TODO? int w = xItemRight - xt;
        // draw text
        if (Current) {
#ifndef DISABLE_ANIMATED_TEXT
          if (fScrollListItem) {
//            if (i > 0)
//TODO?              EnigmaTextEffects.UpdateTextWidth(idListItem[i - 1], Tab(i) - Tab(i - 1));
            idListItem[i] = TE_MARQUEE(osd, idListItem[i], xt, y, s, ColorFg, ColorBg, FONT_LISTITEM, w, nMessagesShown ? std::min(yMessageTop - y, lineHeight) : 0 );
          } else
#endif
            osd->DrawText(xt, y, s, ColorFg, ColorBg, pFontList, w, nMessagesShown ? std::min(yMessageTop - y, lineHeight) : 0 );
        } else
          osd->DrawText(xt, y, s, ColorFg, ColorBg, pFontList, w, nMessagesShown ? std::min(yMessageTop - y, lineHeight) : 0 );
      }
    }
    if (!Tab(i + 1))
      break;
  }
  //set editable width
  SetEditableWidth(xItemRight - Tab(1) - xItemLeft);

#ifndef SKINENIGMA_NO_MENULOGO
  if (Current && isMainMenu && fShowLogo && Text) {
    char *ItemText, *ItemText2;
    int n = strtoul(Text, &ItemText, 10);
    if (n != 0)
      ItemText2 = ItemText = skipspace(ItemText);
    else
      ItemText2 = skipspace(ItemText);

    bool fFoundLogo = false;
    if (strcmp(ItemText, tr("Schedule")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/schedule");
    else if (strcmp(ItemText, tr("Channels")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/channels");
    else if (strcmp(ItemText, tr("Timers")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("timerinfo")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/timers");
    else if (strcmp(ItemText, tr("Recordings")) == 0
             || strcmp(ItemText, tr("Recording info")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("extrecmenu")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/recordings");
    else if (strcmp(ItemText, tr("Setup")) == 0
             || strcmp(ItemText2, tr("Setup")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/setup");
    else if (strcmp(ItemText, tr("Commands")) == 0
             || strcmp(ItemText2, tr("Commands")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/commands");
    else if (strcmp(ItemText, tr(" Stop replaying")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/stop");
    else if (strcmp(ItemText, tr(" Cancel editing")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/cancel");
    else if (strcmp(ItemText2, GetPluginMainMenuName("audiorecorder")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/audiorecorder");
    else if (strcmp(ItemText2, GetPluginMainMenuName("burn")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/burn");
    else if (strcmp(ItemText2, GetPluginMainMenuName("cdda")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/cdda");
    else if (strcmp(ItemText2, GetPluginMainMenuName("chanorg")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/chanorg");
    else if (strcmp(ItemText2, GetPluginMainMenuName("channelscan")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/channelscan");
    else if (strcmp(ItemText2, GetPluginMainMenuName("digicam")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/digicam");
    else if (strcmp(ItemText2, GetPluginMainMenuName("director")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/director");
    else if (strcmp(ItemText2, GetPluginMainMenuName("dvd")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/dvd");
    else if (strcmp(ItemText2, GetPluginMainMenuName("dvdselect")) == 0
           || strcmp(ItemText2, GetPluginMainMenuName("dvdswitch")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/dvdselect");
    else if (strcmp(ItemText2, GetPluginMainMenuName("dxr3")) == 0
           || strcmp(ItemText2, GetPluginMainMenuName("softdevice")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/device");
    else if (strcmp(ItemText2, GetPluginMainMenuName("epgsearch")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("epgsearchonly")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("conflictcheckonly")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("nordlichtsepg")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("quickepgsearch")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/epgsearch");
    else if (strcmp(ItemText2, GetPluginMainMenuName("externalplayer")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/externalplayer");
    else if (strcmp(ItemText2, GetPluginMainMenuName("femon")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/femon");
    else if (strcmp(ItemText2, GetPluginMainMenuName("filebrowser")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/filebrowser");
    else if (strcmp(ItemText2, GetPluginMainMenuName("fussball")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("sport")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/fussball");
    else if (strcmp(ItemText2, GetPluginMainMenuName("games")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/games");
    else if (strcmp(ItemText2, GetPluginMainMenuName("image")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("osdimage")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/image");
    else if (strcmp(ItemText2, GetPluginMainMenuName("mp3")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("mp3ng")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("muggle")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("music")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/mp3");
    else if (strcmp(ItemText2, GetPluginMainMenuName("mplayer")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/mplayer");
    else if (strcmp(ItemText2, GetPluginMainMenuName("newsticker")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/newsticker");
    else if (strcmp(ItemText2, GetPluginMainMenuName("osdpip")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/osdpip");
    else if (strcmp(ItemText2, GetPluginMainMenuName("pin")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/pin");
    else if (strcmp(ItemText2, GetPluginMainMenuName("radio")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/radio");
    else if (strcmp(ItemText2, GetPluginMainMenuName("rotor")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/rotor");
    else if (strcmp(ItemText2, GetPluginMainMenuName("solitaire")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/solitaire");
    else if (strcmp(ItemText2, GetPluginMainMenuName("streamdev-client")) == 0
           || strcmp(ItemText2, GetPluginMainMenuName("streamdev-server")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/streamdev");
    else if (strcmp(ItemText2, GetPluginMainMenuName("sudoku")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/sudoku");
    else if (strcmp(ItemText2, GetPluginMainMenuName("teletext")) == 0
           || strcmp(ItemText2, GetPluginMainMenuName("osdteletext")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/teletext");
    else if (strcmp(ItemText2, GetPluginMainMenuName("tvonscreen")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/tvonscreen");
    else if (strcmp(ItemText2, GetPluginMainMenuName("vcd")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/vcd");
    else if (strcmp(ItemText2, GetPluginMainMenuName("vdrc")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/vdrc");
    else if (strcmp(ItemText2, GetPluginMainMenuName("vdrcd")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("mediad")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("mediamanager")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/vdrcd");
    else if (strcmp(ItemText2, GetPluginMainMenuName("vdrrip")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/vdrrip");
    else if (strcmp(ItemText2, GetPluginMainMenuName("weather")) == 0
           || strcmp(ItemText2, GetPluginMainMenuName("weatherng")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/weather");
    else if (strcmp(ItemText2, GetPluginMainMenuName("webepg")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/webepg");
    else if (strcmp(ItemText2, GetPluginMainMenuName("xineliboutput")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/xineliboutput");
    else
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/vdr");

    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoTop - 1, Theme.Color(clrLogoBg));
    if (fFoundLogo) {
      osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                      yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                      EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                      Theme.Color(clrLogoBg), false, true);
    }
  }
#endif

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

const char *cSkinEnigmaDisplayMenu::GetPluginMainMenuName(const char *plugin)
{
  cPlugin *p = cPluginManager::GetPlugin(plugin);
  if (p) {
    const char *entry = p->MainMenuEntry();
    if (entry)
      return entry;
  }
  return plugin;
}

int cSkinEnigmaDisplayMenu::DrawFlag(int x, int y, const tComponent *p)
{
  //Must be TE_LOCKed by caller

  if (p == NULL)
    return 0;

  if ((p->stream == 2) && p->language) {
    std::string flag("flags/");
    flag += p->language;
    /*TODO
    if (p->description) {
      
    }
    */
    if (EnigmaLogoCache.LoadSymbol(flag.c_str())) {
      osd->DrawBitmap(x, y - EnigmaLogoCache.Get().Height(), EnigmaLogoCache.Get());
      return EnigmaLogoCache.Get().Width();
    }
  }

  return 0;
}

void cSkinEnigmaDisplayMenu::SetEvent(const cEvent *Event)
{
  // check if event
  if (!Event)
    return;

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  const cFont *pFontDetailsTitle = EnigmaConfig.GetFont(FONT_DETAILSTITLE);
  const cFont *pFontDetailsSubtitle = EnigmaConfig.GetFont(FONT_DETAILSSUBTITLE);
  const cFont *pFontDetailsDate = EnigmaConfig.GetFont(FONT_DETAILSDATE);
  const cFont *pFontDetailsText = EnigmaConfig.GetFont(FONT_DETAILSTEXT);

  isMainMenu = false;
  fShowInfo = false;
  fShowLogo = fShowLogoDefault ? EnigmaConfig.showImages : false;
  SetupAreas();

  time_t now = time(NULL);
  // draw recording date string
  std::stringstream sstrDate;
  sstrDate << *Event->GetDateString()
           << "  " << *Event->GetTimeString()
           << " - " << *Event->GetEndTimeString()
           << " (";
  if (now > Event->StartTime())
    sstrDate << (now - Event->StartTime()) / 60 << '/';
  sstrDate << Event->Duration() / 60 << tr("min") << ")";

  int y = yDateBottom + (pFontDetailsDate->Height() - bmVPS.Height()) / 2;
  int xs = xDateRight - Gap;
  // check if event has VPS 
  if (EnigmaConfig.showVps && Event->Vps()) {
    // draw VPS symbol
    if (Event->Vps() != Event->StartTime()) {
      char vps[6];
      struct tm tm_r;
      time_t t_vps = Event->Vps();
      strftime(vps, sizeof(vps), "%H:%M", localtime_r(&t_vps, &tm_r));
      xs -= pFontDetailsDate->Width(vps);
      osd->DrawText(xs, yDateBottom, vps,
                    Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                    pFontDetailsDate, pFontDetailsDate->Width(vps), pFontDetailsDate->Height());
      xs -= TinyGap;
    }
    xs -= bmVPS.Width();
    osd->DrawBitmap(xs, y, bmVPS, Theme.Color(clrSymbolVpsActive), Theme.Color(clrSymbolInactive));
    xs -= Gap;
  }
  // check if event is running
  if (Event->IsRunning()) {
    // draw running symbol
    xs -= (bmRun.Width() + Gap);
    osd->DrawBitmap(xs, y, bmRun, Theme.Color(clrSymbolRunActive), Theme.Color(clrBackground));
    xs -= Gap;
  }
  // check if event has timer
  if (Event->HasTimer()) {
    if (Event->IsRunning()) {
      // draw recording symbol
      xs -= bmRecording.Width();
      osd->DrawBitmap(xs, y, bmRecording, Theme.Color(clrSymbolRecActive), Theme.Color(clrBackground));
    } else {
      // draw timer symbol
      xs -= bmTimer.Width();
      osd->DrawBitmap(xs, y, bmTimer, Theme.Color(clrSymbolTimerActive), Theme.Color(clrBackground));
    }
    xs -= Gap;
  }
  std::string stringInfo;
  const cComponents *Components = Event->Components();
  if (Components) {
    std::stringstream sstrInfo;
    for (int i = 0; i < Components->NumComponents(); i++) {
      const tComponent *p = Components->Component(i);
      if (p && (p->stream == 2) && p->language) {
        if (p->description) {
          sstrInfo << p->description
                   << " (" << p->language << "), ";
        } else {
          sstrInfo << p->language << ", ";
        }
//        DrawFlag(p); //TODO
      }
    }
    // strip out the last delimiter
    if (!sstrInfo.str().empty())
      stringInfo = tr("Languages");
      stringInfo += ": "; 
      stringInfo += sstrInfo.str().substr(0, sstrInfo.str().length() - 2);
  }
  int yHeadlineBottom = yDateBottom;
  int xHeadlineRight  = fShowLogo ? xDateLeft : xInfoRight;
  osd->DrawRectangle(xBodyLeft, yBodyTop, xHeadlineRight - 1,
                     yHeadlineBottom - 1, Theme.Color(clrAltBackground));
  int th = pFontDetailsTitle->Height() + (!isempty(Event->Description()) && !isempty(Event->ShortText()) ? Gap + pFontDetailsSubtitle->Height() : 0);
  y = yBodyTop + (yHeadlineBottom - yBodyTop - th) / 2;

  // draw recording title
#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollOther)
    TE_MARQUEE(osd, -1, xBodyLeft + Gap, y, Event->Title(),
               Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
               FONT_DETAILSTITLE, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsTitle->Height());
  else
#endif
    osd->DrawText(xBodyLeft + Gap, y, Event->Title(),
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
                  pFontDetailsTitle, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsTitle->Height());

  osd->DrawText(xBodyLeft + Gap, yHeadlineBottom, sstrDate.str().c_str(),
                Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                pFontDetailsDate, xs - xBodyLeft - Gap - 1, pFontDetailsDate->Height());

  // draw recording short text and description
  const char *strDescr = NULL;
  if (isempty(Event->Description())) {
    // check if short text
    if (!isempty(Event->ShortText())) {
      // draw short text as description, if no description available
      strDescr = Event->ShortText();
    }
  } else {
    // check if short text
    if (!isempty(Event->ShortText())) {
      // draw below Event->Title
      y += pFontDetailsTitle->Height() + Gap;

      // draw short text
#ifndef DISABLE_ANIMATED_TEXT
      if (fScrollOther)
        TE_MARQUEE(osd, -1, xBodyLeft + Gap, y, Event->ShortText(),
                   Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrAltBackground),
                   FONT_DETAILSSUBTITLE, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsSubtitle->Height());
      else
#endif
        osd->DrawText(xBodyLeft + Gap, y, Event->ShortText(),
                      Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrAltBackground),
                      pFontDetailsSubtitle, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsSubtitle->Height());
    }
    // draw description
    strDescr = Event->Description();
  }

  std::string stringReruns;
#ifdef SKINENIGMA_HAVE_EPGSEARCH
  // try to find a rerun of the show using epgsearch-service
  if (EnigmaConfig.numReruns > 0 && !isempty(Event->Title())) {
    std::stringstream sstrReruns;
    Epgsearch_searchresults_v1_0 data;
    std::string strQuery = Event->Title();
    if (EnigmaConfig.useSubtitleRerun > 0) {
      if (EnigmaConfig.useSubtitleRerun == 2 || !isempty(Event->ShortText()))
        strQuery += "~";
      if (!isempty(Event->ShortText()))
        strQuery += Event->ShortText();
      data.useSubTitle = true;
    } else {
      data.useSubTitle = false;
    }
    data.query = (char *)strQuery.c_str();
    data.mode = 0;
    data.channelNr = 0;
    data.useTitle = true;
    data.useDescription = false;
    if (cPluginManager::CallFirstService("Epgsearch-searchresults-v1.0", &data)) {
      cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
      if (list && (list->Count() > 1)) {
        //TODO: current event is shown as rerun 
        sstrReruns << tr("RERUNS OF THIS SHOW") << ':' << std::endl;
        int i = 0;
        for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < EnigmaConfig.numReruns; r = list->Next(r)) {
          i++;
          sstrReruns << "- "
                     << *DayDateTime(r->event->StartTime())
                     << " " << Channels.GetByChannelID(r->event->ChannelID())->ShortName(true)
                     << ":  " << r->event->Title();
          if (!isempty(r->event->ShortText()))
            sstrReruns << "~" << r->event->ShortText();
          sstrReruns << std::endl;
        }
        delete list;
      }
    }
    stringReruns = sstrReruns.str();
  }
#endif // SKINENIGMA_HAVE_EPGSEARCH

  const char *strFirst = NULL;
  const char *strSecond = NULL;
  const char *strThird = stringReruns.empty() ? NULL : stringReruns.c_str();
  if (EnigmaConfig.showAuxInfo) {
    strFirst = strDescr;
    strSecond = stringInfo.empty() ? NULL : stringInfo.c_str();
  } else {
    strFirst = stringInfo.empty() ? NULL : stringInfo.c_str();
    strSecond = strDescr;
  }
  if (strFirst || strSecond || strSecond) {
    y = yHeadlineBottom + SmallGap + 2 * pFontDetailsDate->Height();
    char *mytext;
    asprintf(&mytext, "%s%s%s%s%s", strFirst ? strFirst : "",
                                    strSecond ? "\n\n" : "", strSecond ? strSecond : "",
                                    (strFirst || strSecond) && strThird ? "\n\n" : "", strThird ? strThird : "");
    textScroller.Set(osd, xBodyLeft + Gap, y,
                     xInfoRight - SmallGap - ScrollbarWidth - SmallGap - Gap - xBodyLeft,
                     yBodyBottom - y,
                     mytext, pFontDetailsText,
                     Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
    SetScrollbar();
    free(mytext);
  }

#ifndef SKINENIGMA_NO_MENULOGO
  if (fShowLogo) {
    // draw logo
    osd->DrawRectangle(xDateLeft + SmallGap, yDateTop, xDateRight - 1, yDateBottom - SmallGap - 1, Theme.Color(clrLogoBg));
    if (!(EnigmaConfig.showImages && EnigmaLogoCache.DrawEventImage(Event, xLogoLeft, yLogoTop, xLogoRight - xLogoLeft, yLogoBottom - yLogoTop, nNumImageColors, osd->GetBitmap(2) ? osd->GetBitmap(2) : osd->GetBitmap(0)))) {
      if (EnigmaLogoCache.LoadIcon("icons/menu/schedule"))
        osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                        yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                        EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1), Theme.Color(clrLogoBg));
    }
    // draw borders
    osd->DrawRectangle(xDateLeft, yDateTop, xDateLeft + SmallGap - 1, yDateBottom - 1, clrTransparent);
    osd->DrawRectangle(xDateLeft, yLogoBottom - SmallGap, xDateRight, yLogoBottom - 1, clrTransparent);
  }
#endif
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

int cSkinEnigmaDisplayMenu::ReadSizeVdr(const char *strPath)
{
  int dirSize = -1;
  char buffer[20];
  char *strFilename = NULL;
  asprintf(&strFilename, "%s/size.vdr", strPath);
  if (strFilename) {
    struct stat st;
    if (stat(strFilename, &st) == 0) {
      int fd = open(strFilename, O_RDONLY);
      if (fd >= 0) {
        if (safe_read(fd, &buffer, sizeof(buffer)) >= 0) {
          dirSize = atoi(buffer);
        }
        close(fd);
      }
    }
    free(strFilename);
  }
  return dirSize;
}

void cSkinEnigmaDisplayMenu::SetRecording(const cRecording *Recording)
{
  // check if recording
  if (!Recording)
    return;

  const cRecordingInfo *Info = Recording->Info();
  if (Info == NULL) {
    //TODO: draw error message
    return;
  }

#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  const cFont *pFontDetailsTitle = EnigmaConfig.GetFont(FONT_DETAILSTITLE);
  const cFont *pFontDetailsSubtitle = EnigmaConfig.GetFont(FONT_DETAILSSUBTITLE);
  const cFont *pFontDetailsDate = EnigmaConfig.GetFont(FONT_DETAILSDATE);
  const cFont *pFontDetailsText = EnigmaConfig.GetFont(FONT_DETAILSTEXT);

  isMainMenu = false;
  fShowInfo = false;
  fShowLogo = fShowLogoDefault ? EnigmaConfig.showImages : false;;
  SetupAreas();

  // draw recording date string
  std::stringstream sstrDate;
  sstrDate << *DateString(Recording->start)
           << "  " << *TimeString(Recording->start);

  // draw additional information
  std::stringstream sstrInfo;
  int dirSize = -1;
  if (EnigmaConfig.showRecSize > 0) {
    if ((dirSize = ReadSizeVdr(Recording->FileName())) < 0 && EnigmaConfig.showRecSize == 2) {
      dirSize = DirSizeMB(Recording->FileName());
    }
  }
  cChannel *channel = Channels.GetByChannelID(((cRecordingInfo *)Info)->ChannelID());
  if (channel)
    sstrInfo << tr("Channel") << ": " << channel->Number() << " - " << channel->Name() << std::endl;
  if (dirSize >= 0)
    sstrInfo << tr("Size") << ": " << std::setprecision(3) << (dirSize > 1023 ? dirSize / 1024.0 : dirSize) << (dirSize > 1023 ? "GB\n" : "MB\n");
  sstrInfo << tr("Priority") << ": " << Recording->priority << std::endl
           << tr("Lifetime") << ": " << Recording->lifetime << std::endl;
  if (Info->Aux()) {
    sstrInfo << std::endl << tr("Auxiliary information") << ":\n"
             << parseaux(Info->Aux());
  }
  int xs = xDateRight - Gap;
  const cComponents *Components = Info->Components();
  if (Components) {
    //TODO: show flaggs?
    std::stringstream info;
    for (int i = 0; i < Components->NumComponents(); i++) {
      const tComponent *p = Components->Component(i);
      if ((p->stream == 2) && p->language) {
        if (p->description) {
          info << p->description
               << " (" << p->language << "), ";
        } else {
          info << p->language << ", ";
        }
      }
    }
    // strip out the last delimiter
    if (!info.str().empty()) {
      sstrInfo << tr("Languages") << ": "
               << info.str().substr(0, info.str().length() - 2);
    }
  }

  int yHeadlineBottom = yDateBottom;
  int xHeadlineRight  = fShowLogo ? xDateLeft : xInfoRight;
  const char *Title = Info->Title();
  if (isempty(Title))
    Title = Recording->Name();
  osd->DrawRectangle(xBodyLeft, yBodyTop, xHeadlineRight - 1, yHeadlineBottom - 1, Theme.Color(clrAltBackground));
  int th = pFontDetailsTitle->Height() + (!isempty(Info->Description()) && !isempty(Info->ShortText()) ? Gap + pFontDetailsSubtitle->Height() : 0);
  int y = yBodyTop + (yHeadlineBottom - yBodyTop - th) / 2;

  // draw recording title
#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollOther)
    TE_MARQUEE(osd, -1, xBodyLeft + Gap, y, Title, 
               Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
               FONT_DETAILSTITLE, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsTitle->Height());
  else
#endif
    osd->DrawText(xBodyLeft + Gap, y, Title, 
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
                  pFontDetailsTitle, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsTitle->Height());

  osd->DrawText(xBodyLeft + Gap, yHeadlineBottom, sstrDate.str().c_str(),
                Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                pFontDetailsDate, xs - xBodyLeft - Gap - 1, pFontDetailsDate->Height());
  // draw recording short text and description
  const char* strDescr = NULL;
  if (isempty(Info->Description())) {
    // check if short text
    if (!isempty(Info->ShortText())) {
      // draw short text as description, if no description available
      strDescr = Info->ShortText();
    }
  } else {
    // check if short text
    if (!isempty(Info->ShortText())) {
      // draw below Title
      y += pFontDetailsTitle->Height() + Gap;

      // draw short text
#ifndef DISABLE_ANIMATED_TEXT
      if (fScrollOther)
        TE_MARQUEE(osd, -1, xBodyLeft + Gap, y, Info->ShortText(),
                   Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrAltBackground),
                   FONT_DETAILSSUBTITLE, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsSubtitle->Height());
      else
#endif
        osd->DrawText(xBodyLeft + Gap, y, Info->ShortText(),
                      Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrAltBackground),
                      pFontDetailsSubtitle, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsSubtitle->Height());
    }
    // draw description
    strDescr = Info->Description();
  }

  std::string stringInfo = sstrInfo.str();
  const char *strInfo = stringInfo.empty() ? NULL : stringInfo.c_str();
  if (strDescr || strInfo) {
    y = yHeadlineBottom + pFontDetailsDate->Height() + BigGap;
    char *mytext;
    if (EnigmaConfig.showAuxInfo)
      asprintf(&mytext, "%s%s%s", strDescr ? strDescr : "", strInfo && strDescr ? "\n\n" : "", strInfo ? strInfo : "");
    else
      asprintf(&mytext, "%s%s%s", strInfo ? strInfo : "", strInfo && strDescr ? "\n\n" : "", strDescr ? strDescr : "");
    textScroller.Set(osd, xBodyLeft + Gap, y,
                     xInfoRight - SmallGap - ScrollbarWidth - SmallGap - Gap - xBodyLeft,
                     yBodyBottom - y, mytext, pFontDetailsText,
                     Theme.Color(clrMenuTxtFg),
                     Theme.Color(clrBackground));
    SetScrollbar();
    free(mytext);
  }

#ifndef SKINENIGMA_NO_MENULOGO
  if (fShowLogo) {
    osd->DrawRectangle(xDateLeft + SmallGap, yDateTop, xDateRight - 1, yDateBottom - SmallGap - 1, Theme.Color(clrLogoBg));
    if (!(EnigmaConfig.showImages && EnigmaLogoCache.DrawRecordingImage(Recording, xLogoLeft, yLogoTop, xLogoRight - xLogoLeft, yLogoBottom - yLogoTop, nNumImageColors, osd->GetBitmap(2) ? osd->GetBitmap(2) : osd->GetBitmap(0)))) {
      // draw logo
      if (EnigmaLogoCache.LoadIcon("icons/menu/recordings"))
        osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                        yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                        EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1), Theme.Color(clrLogoBg));
    }
    // draw borders
    osd->DrawRectangle(xDateLeft, yDateTop, xDateLeft + SmallGap - 1, yDateBottom - 1, clrTransparent);
    osd->DrawRectangle(xDateLeft, yLogoBottom - SmallGap, xDateRight, yLogoBottom - 1, clrTransparent);
  }
#endif
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayMenu::SetText(const char *Text, bool FixedFont)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  // draw text
  textScroller.Set(osd, xBodyLeft + Gap, yBodyTop + Gap,
                   GetTextAreaWidth(),
                   yBodyBottom - Gap - yBodyTop - Gap, Text,
                   GetTextAreaFont(FixedFont),
                   Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
  SetScrollbar();
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

int cSkinEnigmaDisplayMenu::GetTextAreaWidth(void) const
{
  // max text area width
  return (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - Gap - SmallGap - ScrollbarWidth - SmallGap;
}

const cFont *cSkinEnigmaDisplayMenu::GetTextAreaFont(bool FixedFont) const
{
  // text area font
  return FixedFont ? EnigmaConfig.GetFont(FONT_FIXED) : EnigmaConfig.GetFont(FONT_DETAILSTEXT);
}

int cSkinEnigmaDisplayMenu::getDateWidth(const cFont *pFontDate)
{ // only called from constructor, so pFontDate should be OK
  int w = MIN_DATEWIDTH;
  struct tm tm_r;
  time_t t = time(NULL);
  tm *tm = localtime_r(&t, &tm_r);

  int nWeekday = tm->tm_wday;
  if (0 <= nWeekday && nWeekday < 7)
    w = std::max(w, pFontDate->Width(strWeekdays[nWeekday]));

  char temp[32];
  strftime(temp, sizeof(temp), "%d.%m.%Y", tm);
  w = std::max(w, pFontDate->Width(temp));

  cString time = TimeString(t);
  w = std::max(w, pFontDate->Width(time));

  return w;
}

void cSkinEnigmaDisplayMenu::Flush(void)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
debug("cSkinEnigmaDisplayMenu::Flush()");

  if (fShowLogo) {
    time_t t = time(NULL);
    cString time = TimeString(t);
    if ((strLastDate == NULL) || strcmp(strLastDate, (const char*)time) != 0) {
      free(strLastDate);
      strLastDate = strdup((const char*)time);

      const cFont *pFontDate = EnigmaConfig.GetFont(FONT_DATE);
      int x = xDateLeft + SmallGap;
      int w = xLogoLeft - x;
      int ys = yDateTop + (yDateBottom - SmallGap - yDateTop - 3 * pFontDate->Height()) / 2;

      char temp[32];
      struct tm tm_r;
      tm *tm = localtime_r(&t, &tm_r);

      int nWeekday = tm->tm_wday;
      if (0 <= nWeekday && nWeekday < 7) {
        osd->DrawText(x, ys, tr(strWeekdays[nWeekday]), Theme.Color(clrMenuTxtFg),
                      Theme.Color(clrLogoBg), pFontDate, w,
                      pFontDate->Height(), taCenter);
      }
      /*TODO: old
      strftime(temp, sizeof(temp), "%A", tm);
      osd->DrawText(x, ys, temp, Theme.Color(clrMenuTxtFg),
                    Theme.Color(clrLogoBg), pFontDate, w,
                    pFontDate->Height(), taCenter);
      */
      ys += pFontDate->Height();

      strftime(temp, sizeof(temp), "%d.%m.%Y", tm);
      osd->DrawText(x, ys, temp, Theme.Color(clrMenuTxtFg),
                    Theme.Color(clrLogoBg), pFontDate, w,
                    pFontDate->Height(), taCenter);
      ys += pFontDate->Height();

      osd->DrawText(x, ys, time, Theme.Color(clrMenuTxtFg),
                    Theme.Color(clrLogoBg), pFontDate, w,
                    pFontDate->Height(), taCenter);
    }
  } else {
    cString date = DayDateTime();
    if ((strLastDate == NULL) || strcmp(strLastDate, (const char*)date) != 0) {
      free(strLastDate);
      strLastDate = strdup((const char*)date);
      const cFont *pFontDate = EnigmaConfig.GetFont(FONT_DATE);
      osd->DrawText(xDateLeft + SmallGap, yDateTop, date, Theme.Color(clrMenuTxtFg),
                    Theme.Color(clrLogoBg), pFontDate, xDateRight - xDateLeft - SmallGap,
                    yTitleDecoBottom - yDateTop, taCenter);
    }
  }
  osd->Flush();
#ifndef DISABLE_ANIMATED_TEXT
  TE_UNLOCK;
  if (fLocked) {
    fLocked = false;
    TE_WAKEUP;
  }
#endif
}

// --- cSkinEnigmaDisplayReplay ---------------------------------------------

class cSkinEnigmaDisplayReplay : public cSkinDisplayReplay, public cSkinEnigmaOsd {
private:
  cOsd *osd;
  const cFont *pFontOsdTitle;
  const cFont *pFontReplayTimes;
  const cFont *pFontDate;
  const cFont *pFontMessage;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom;
  int xProgressLeft, xProgressRight, yProgressTop, yProgressBottom;
  int xTimeLeft, xTimeRight, yTimeTop, yTimeBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;

  char *strLastDate;
  bool modeonly;
  int nJumpWidth;
  int nCurrentWidth;
  bool fShowSymbol;
#ifndef DISABLE_ANIMATED_TEXT
  bool fLocked;
  bool fScrollTitle;
  int idTitle;
#endif

public:
  cSkinEnigmaDisplayReplay(bool ModeOnly);
  virtual ~ cSkinEnigmaDisplayReplay();
  virtual void SetTitle(const char *Title);
  virtual void SetMode(bool Play, bool Forward, int Speed);
  virtual void SetProgress(int Current, int Total);
  virtual void SetCurrent(const char *Current);
  virtual void SetTotal(const char *Total);
  virtual void SetJump(const char *Jump);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  virtual void DrawTitle(const char *s);
};

cSkinEnigmaDisplayReplay::cSkinEnigmaDisplayReplay(bool ModeOnly)
{
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontReplayTimes = EnigmaConfig.GetFont(FONT_REPLAYTIMES);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);

  strLastDate = NULL;

  int MessageHeight = 2 * SmallGap + pFontMessage->Height() + 2 * SmallGap;

  modeonly = ModeOnly;
  nJumpWidth = 0;
  nCurrentWidth = 0;
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsReplay;
#ifndef DISABLE_ANIMATED_TEXT
  fScrollTitle = !modeonly && EnigmaConfig.useTextEffects && EnigmaConfig.scrollTitle;
  idTitle = -1;
#endif

  int LogoSize = Gap + IconHeight + Gap;
  LogoSize += (LogoSize % 2 ? 1 : 0);
  xTitleLeft = 0;
  xTitleRight = OsdSize.w;
  yTitleTop = 0;
  yTitleBottom = yTitleTop + pFontOsdTitle->Height();
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  xLogoLeft = xTitleLeft;
  xLogoRight = xLogoLeft + LogoSize;
  yLogoTop = yTitleDecoBottom + TitleDecoGap2;
  yLogoBottom = yLogoTop + LogoSize;
  xTimeLeft = xLogoRight + LogoDecoGap2;
  xTimeRight = xTitleRight;
  xTimeLeft += (xTimeRight - xTimeLeft) - ((xTimeRight - xTimeLeft) & ~0x07);
  yTimeTop = yLogoBottom - SmallGap - pFontReplayTimes->Height() - SmallGap;
  yTimeBottom = yLogoBottom;
  xProgressLeft = xTimeLeft;
  xProgressRight = xTitleRight;
  yProgressTop = yLogoTop;
  yProgressBottom = yTimeTop - SmallGap;
  xBottomLeft = xTitleLeft;
  xBottomRight = xTitleRight;
  yBottomTop = yLogoBottom + SmallGap;
  yBottomBottom = yBottomTop + pFontDate->Height();
  xMessageLeft = xProgressLeft;
  xMessageRight = xProgressRight;
  yMessageTop = yLogoTop + (LogoSize - MessageHeight) / 2;
  yMessageBottom = yMessageTop + MessageHeight;

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + OsdSize.h - yBottomBottom);
  tArea Areas[] = { {xTitleLeft, yTitleTop, xBottomRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((Areas[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayReplay: using %dbpp single area", Areas[0].bpp);
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  } else {
    debug("cSkinEnigmaDisplayReplay: using multiple areas");
    tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2},
                      {xLogoLeft, yLogoTop, xLogoRight + 1, yLogoBottom - 1, 4}, //TODO? "+1" because of wrong colors with "-1" 
                      {xProgressLeft, yProgressTop, xTimeRight - 1, yTimeBottom - 1, 4},
                      {xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, 2}
    };
    int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
    if (rc == oeOk)
      osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
    else {
      error("cSkinEnigmaDisplayReplay: CanHandleAreas() returned %d", rc);
      delete osd;
      osd = NULL;
      throw 1;
      return;
    }
  }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  if (modeonly) {
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, fShowSymbol ? Theme.Color(clrLogoBg) : clrTransparent);
  } else {
    // draw title area
    osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1,
                       yTitleBottom - 1, Theme.Color(clrTitleBg));
    osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                     yTitleTop + Roundness - 1, clrTransparent, -2);
    osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight,
                     yTitleTop + Roundness, clrTransparent, -1);
    osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1,
                       yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    // draw progress bar area
    osd->DrawRectangle(xProgressLeft, yProgressTop, xProgressRight - 1,
                       yProgressBottom - 1, Theme.Color(clrReplayBarAreaBg));
    // draw time area
    osd->DrawRectangle(xTimeLeft, yTimeTop, xTimeRight - 1, yTimeBottom - 1, Theme.Color(clrAltBackground));
    // draw bottom area
    osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1,
                       yBottomBottom - 1, Theme.Color(clrBottomBg));
    osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                     xBottomLeft + Roundness - 1, yBottomBottom - 1,
                     clrTransparent, -3);
    osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                     xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);
  }

#ifndef DISABLE_ANIMATED_TEXT
  fLocked = false;
  if (fScrollTitle) {
    TE_START(osd);
    fLocked = true;
  }
#endif
}

cSkinEnigmaDisplayReplay::~cSkinEnigmaDisplayReplay()
{
#ifndef DISABLE_ANIMATED_TEXT
  if (fScrollTitle) {
    if (!fLocked) TE_LOCK;
    TE_STOP;
  }
#endif
  free(strLastDate);
  delete osd;
}

void cSkinEnigmaDisplayReplay::SetTitle(const char *Title)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
  if (fScrollTitle) {
    idTitle = TE_TITLE(osd, idTitle, Title, xTitleRight - Roundness - xTitleLeft - Roundness, this);
  } else
#endif
    DrawTitle(Title);
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::DrawTitle(const char *Title)
{
  //Must be TE_LOCKed by caller

  // draw title area
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1,
                     yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                   yTitleTop + Roundness - 1, clrTransparent, -2);
  osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight,
                   yTitleTop + Roundness, clrTransparent, -1);
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1,
                     yTitleDecoBottom - 1, Theme.Color(clrTitleBg));

  if (Title) {
    debug("REPLAY TITLE: %s", Title);
  // draw titlebar
    osd->DrawText(xTitleLeft + Roundness + 3, yTitleTop + 3, Title,
                  Theme.Color(clrTitleShadow), clrTransparent,
                  pFontOsdTitle,
                  xTitleRight - Roundness - xTitleLeft - Roundness - 3,
                  yTitleBottom - yTitleTop - 3);
    osd->DrawText(xTitleLeft + Roundness, yTitleTop, Title,
                  Theme.Color(clrTitleFg), clrTransparent,
                  pFontOsdTitle,
                  xTitleRight - Roundness - xTitleLeft - Roundness - 3,
                  yTitleBottom - yTitleTop);
  }
}

void cSkinEnigmaDisplayReplay::SetMode(bool Play, bool Forward, int Speed)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif

  bool fFoundLogo = false;
  if (Speed < -1)
    Speed = -1;

  if (modeonly) {
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, clrTransparent);
    osd->DrawEllipse(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  } else {
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  }

  if (fShowSymbol) {
    char *logo = NULL;
    asprintf(&logo, "icons/menu/%s", EnigmaStatus.ReplayModeName());
    if (EnigmaLogoCache.LoadIcon(logo)) {
      osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                      yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                      EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                      clrTransparent, true, true);
    }
    free(logo);
  }

  if (Speed == -1)
    fFoundLogo = EnigmaLogoCache.LoadIcon(Play ? "icons/replay/play" : "icons/replay/pause");
  else if (Play) {
    if (Speed > MAX_SPEED_BITMAPS - 1) {
      error("MAX SPEED %d > 9", Speed);
      Speed = MAX_SPEED_BITMAPS - 1;
    }
    fFoundLogo = EnigmaLogoCache.LoadIcon(Forward ? strFastForward_large[Speed] : strFastRewind_large[Speed]);
  } else {                      // trick speed
    if (Speed > MAX_TRICKSPEED_BITMAPS - 1) {
      error("MAX SPEED %d > 3", Speed);
      Speed = MAX_TRICKSPEED_BITMAPS - 1;
    }
    fFoundLogo = EnigmaLogoCache.LoadIcon(Forward ? strSlowForward_large[Speed] : strSlowRewind_large[Speed]);
  }
  if (fFoundLogo)
    osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                    yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                    EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                    clrTransparent, false, true);
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::SetProgress(int Current, int Total)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  // create progressbar
  cProgressBar pb(xProgressRight - xProgressLeft - 2 * BigGap,
                  yProgressBottom - yProgressTop - 2 * BigGap, Current, Total,
                  marks, Theme.Color(clrReplayProgressSeen),
                  Theme.Color(clrReplayProgressRest),
                  Theme.Color(clrReplayProgressSelected),
                  Theme.Color(clrReplayProgressMark),
                  Theme.Color(clrReplayProgressCurrent));
  // draw progressbar
  osd->DrawBitmap(xProgressLeft + BigGap, yProgressTop + BigGap, pb);
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::SetCurrent(const char *Current)
{
  if (!Current)
    return;

  // draw current time
  int w = pFontReplayTimes->Width(Current);
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  osd->DrawText(xTimeLeft + BigGap, yTimeTop, Current,
                Theme.Color(clrReplayCurrent), Theme.Color(clrAltBackground), pFontReplayTimes,
                w, yTimeBottom - yTimeTop, taLeft);
  if (nCurrentWidth > w)
    osd->DrawRectangle(xTimeLeft + BigGap + w, yTimeTop, xTimeLeft + BigGap + nCurrentWidth - 1, yTimeBottom - 1, Theme.Color(clrAltBackground));
  nCurrentWidth = w;
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::SetTotal(const char *Total)
{
  if (!Total)
    return;

  // draw total time
  int w = pFontReplayTimes->Width(Total);
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  osd->DrawText(xTimeRight - BigGap - w, yTimeTop, Total,
                Theme.Color(clrReplayTotal), Theme.Color(clrAltBackground), pFontReplayTimes, w,
                yTimeBottom - yTimeTop, taRight);
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::SetJump(const char *Jump)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  if (Jump) {
    // draw jump prompt
    nJumpWidth = pFontReplayTimes->Width(Jump);
    osd->DrawText(xTimeLeft + (xTimeRight - xTimeLeft - nJumpWidth) / 2,
                  yTimeTop, Jump, Theme.Color(clrReplayModeJump),
                  Theme.Color(clrAltBackground), pFontReplayTimes, nJumpWidth,
                  yTimeBottom - yTimeTop, taLeft);
  } else {
    // erase old prompt
    osd->DrawRectangle(xTimeLeft + (xTimeRight - xTimeLeft - nJumpWidth) / 2,
                       yTimeTop,
                       xTimeLeft + (xTimeRight - xTimeLeft - nJumpWidth) / 2 +
                       nJumpWidth - 1, yTimeBottom - 1, Theme.Color(clrAltBackground));
  }
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::SetMessage(eMessageType Type, const char *Text)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  if (Text) {
    // save current osd
    osd->SaveRegion(xMessageLeft, yMessageTop, xMessageRight, yMessageBottom);
    // draw message
    osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1,
                       yMessageBottom - 1, clrTransparent);
    osd->DrawRectangle(xMessageLeft, yMessageTop + SmallGap, xMessageRight - 1,
                       yMessageBottom - SmallGap - 1,
                       Theme.Color(clrButtonRedBg));
    osd->DrawText(xMessageLeft, yMessageTop + 2 * SmallGap, Text,
                  Theme.Color(clrMessageStatusFg + 2 * Type),
                  Theme.Color(clrMessageStatusBg + 2 * Type),
                  pFontMessage,
                  xMessageRight - xMessageLeft,
                  yMessageBottom - 2 * SmallGap - yMessageTop - 2 * SmallGap,
                  taCenter);
  } else {
    // restore saved osd
    osd->RestoreRegion();
  }
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayReplay::Flush(void)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  // update date
  if (!modeonly) {
    cString date = DayDateTime();
    if ((strLastDate == NULL) || strcmp(strLastDate, (const char*)date) != 0) {
      free(strLastDate);
      strLastDate = strdup((const char*)date);
      osd->DrawText(xBottomLeft + Roundness, yBottomTop, date,
                    Theme.Color(clrTitleFg), Theme.Color(clrBottomBg),
                    pFontDate,
                    xBottomRight - Roundness - xBottomLeft - Roundness - 1,
                    yBottomBottom - yBottomTop - 1, taRight);
    }
  }
  osd->Flush();
#ifndef DISABLE_ANIMATED_TEXT
  TE_UNLOCK;
  if (fLocked) {
    fLocked = false;
    TE_WAKEUP;
  }
#endif
}

// --- cSkinEnigmaDisplayVolume ---------------------------------------------

class cSkinEnigmaDisplayVolume : public cSkinDisplayVolume {
private:
  cOsd *osd;
  const cFont *pFontOsdTitle;
  const cFont *pFontDate;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom, xLogoDecoLeft, xLogoDecoRight;
  int xBodyLeft, xBodyRight, yBodyTop, yBodyBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;

  time_t lastTime;
  bool fShowSymbol;

public:
  cSkinEnigmaDisplayVolume();
  virtual ~ cSkinEnigmaDisplayVolume();
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
};

cSkinEnigmaDisplayVolume::cSkinEnigmaDisplayVolume()
{
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);

  lastTime = 0;
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsAudio;

  int LogoSize = std::max(pFontOsdTitle->Height() +
                          cFont::GetFont(fontOsd)->Height() +
                          pFontDate->Height() +
                          TitleDeco + SmallGap, IconHeight);
  LogoSize += (LogoSize % 2 ? 1 : 0);
  xLogoLeft = 0;
  xLogoRight = LogoSize;
  xLogoDecoLeft = xLogoRight + LogoDecoGap;
  xLogoDecoRight = xLogoDecoLeft + LogoDecoWidth;
  yLogoTop = 0;
  yLogoBottom = yLogoTop + LogoSize;
  xTitleLeft = fShowSymbol ? (xLogoDecoRight + LogoDecoGap2) : 0;
  xTitleRight = OsdSize.w;
  yTitleTop = yLogoTop;
  yTitleBottom = yTitleTop + pFontOsdTitle->Height();
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  xBottomLeft = xTitleLeft;
  xBottomRight = OsdSize.w;
  yBottomTop = yLogoBottom - pFontDate->Height();
  yBottomBottom = yLogoBottom;
  xBodyLeft = xTitleLeft;
  xBodyRight = xTitleRight;
  yBodyTop = yTitleDecoBottom + TitleDecoGap2;
  yBodyBottom = yBottomTop - SmallGap;

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + OsdSize.h - yBottomBottom);
  tArea Areas[] = { {xLogoLeft, yLogoTop, xTitleRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((Areas[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayVolume: using %dbpp single area", Areas[0].bpp);
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  } else {
    debug("cSkinEnigmaDisplayVolume: using multiple areas");
    cBitmap *bitmap = NULL;
    if (fShowSymbol) {
      tArea Areas[] = { {xLogoLeft, yLogoTop, xLogoRight + LogoDecoGap + LogoDecoWidth - 1, yLogoBottom - 1, 4},
                        {xTitleLeft, yTitleTop, xTitleRight - 1, yBottomBottom - 1, 4}
      };
      int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayVolume: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
      bitmap = osd->GetBitmap(1);
    } else {
      tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yBottomBottom - 1, 4}
      };
      int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayVolume: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
      bitmap = osd->GetBitmap(0);
    }
    if (bitmap) {
      // set colors
      bitmap->Reset();
      bitmap->SetColor(0, Theme.Color(clrTransparent));
      bitmap->SetColor(1, Theme.Color(clrBackground));
      bitmap->SetColor(2, Theme.Color(clrTitleBg));
      bitmap->SetColor(3, Theme.Color(clrBottomBg));
      bitmap->SetColor(4, Theme.Color(clrVolumeBar));
      bitmap->SetColor(5, Theme.Color(clrVolumeBarMute));
      bitmap->SetColor(6, Theme.Color(clrTitleFg));
      bitmap->SetColor(7, Theme.Color(clrTitleShadow));
    }
  }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  if (fShowSymbol) {
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    osd->DrawRectangle(xLogoDecoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  }
  // draw title
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1, yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1, yTitleTop + Roundness - 1, clrTransparent, -1);
  // draw body area
  osd->DrawRectangle(xBodyLeft, yBodyTop, xBodyRight - 1, yBodyBottom - 1, Theme.Color(clrBackground));
  // draw bottom area
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, Theme.Color(clrBottomBg));
  osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness, xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);

  if (!fShowSymbol) {
    osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1, yTitleTop + Roundness - 1, clrTransparent, -2);
    osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness, xBottomLeft + Roundness, yBottomBottom - 1, clrTransparent, -3);
  }
}

cSkinEnigmaDisplayVolume::~cSkinEnigmaDisplayVolume()
{
  delete osd;
}

void cSkinEnigmaDisplayVolume::SetVolume(int Current, int Total, bool Mute)
{
  tColor ColorBar;
  const char *Prompt;
  bool fFoundLogo = false;

  // select behaviour
  if (Mute) {
    ColorBar = Theme.Color(clrVolumeBarMute);
    Prompt = tr("Mute");
    if (fShowSymbol)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/volume/muteOn");
  } else {
    ColorBar = Theme.Color(clrVolumeBar);
    Prompt = tr("Volume");
    if (fShowSymbol)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/volume/muteOff");
  }
  // logo
  if (fShowSymbol && fFoundLogo)
    osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                    yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                    EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                    Theme.Color(clrLogoBg));
  // current volume
  int vol = xBodyLeft + Gap + (xBodyRight - Gap - xBodyLeft - Gap) * Current / Total;
  // draw titlebar
  osd->DrawRectangle(xTitleLeft + (fShowSymbol ? Gap : Roundness), yTitleTop, xTitleRight - Roundness - 1,
                     yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawText(xTitleLeft + (fShowSymbol ? Gap : Roundness) + 3, yTitleTop + 3, Prompt,
                Theme.Color(clrTitleShadow), clrTransparent, pFontOsdTitle,
                xTitleRight - Roundness - xTitleLeft, pFontOsdTitle->Height(), taCenter);
  osd->DrawText(xTitleLeft + (fShowSymbol ? Gap : Roundness), yTitleTop, Prompt,
                Theme.Color(clrTitleFg), clrTransparent, pFontOsdTitle,
                xTitleRight - Roundness - xTitleLeft, pFontOsdTitle->Height(), taCenter);
  // draw volumebar
  osd->DrawRectangle(xBodyLeft + Gap, yBodyTop + Gap, xBodyRight - Gap - 1,
                     yBodyBottom - Gap - 1, Theme.Color(clrBackground));
  osd->DrawRectangle(xBodyLeft + Gap, yBodyTop + Gap, vol - 1,
                     yBodyBottom - Gap - 1, ColorBar);
}

void cSkinEnigmaDisplayVolume::Flush(void)
{
  time_t now = time(NULL);
  if (now != lastTime) {
    lastTime = now;
    cString date = DayDateTime();
    osd->DrawText(xBottomLeft + (fShowSymbol ? 0 : Roundness), yBottomTop, date, Theme.Color(clrTitleFg),
                  Theme.Color(clrBottomBg), pFontDate,
                  xBottomRight - (fShowSymbol ? Gap : (2 * Roundness)) - xBottomLeft - 1,
                  yBottomBottom - yBottomTop - 1, taRight);
  }
  osd->Flush();
}

// --- cSkinEnigmaDisplayTracks ---------------------------------------------

class cSkinEnigmaDisplayTracks : public cSkinDisplayTracks {
private:
  cOsd *osd;
  const cFont *pFontOsdTitle;
  const cFont *pFontDate;
  const cFont *pFontListItem;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom;
  int xListLeft, xListRight, yListTop, yListBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;

  int lineHeight;
  int currentIndex;
  bool fShowSymbol;
  time_t lastTime;

  void SetItem(const char *Text, int Index, bool Current);
public:
  cSkinEnigmaDisplayTracks(const char *Title, int NumTracks,
                           const char *const *Tracks);
  virtual ~ cSkinEnigmaDisplayTracks();
  virtual void SetTrack(int Index, const char *const *Tracks);
  virtual void SetAudioChannel(int AudioChannel);
  virtual void Flush(void);
};

cSkinEnigmaDisplayTracks::cSkinEnigmaDisplayTracks(const char *Title, int NumTracks, const char *const *Tracks)
{
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontListItem = EnigmaConfig.GetFont(FONT_LISTITEM);

  lastTime = 0;
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsAudio;

  lineHeight = pFontListItem->Height();
  int LogoSize = IconHeight;
  LogoSize += (LogoSize % 2 ? 1 : 0);
  currentIndex = -1;
  int ItemsWidth = 0;
  for (int i = 0; i < NumTracks; i++)
    ItemsWidth = std::max(ItemsWidth, pFontListItem->Width(Tracks[i]));
  ItemsWidth += (EnigmaConfig.showMarker ? lineHeight : SmallGap) + SmallGap;
  ItemsWidth = std::max(ItemsWidth, LogoSize);
  int width = ItemsWidth;
  if (fShowSymbol)
    width += LogoSize + LogoDecoGap2;
  width = std::max(width, pFontDate->Width(DayDateTime()) + 2 * Roundness);
  width = std::max(width, pFontOsdTitle->Width(Title) + 2 * Roundness + (fShowSymbol ? 0 : (bmAudio[0].Width() + SmallGap)));

  xTitleLeft = 0;
  xTitleRight = OsdSize.w;
  int d = xTitleRight - xTitleLeft;
  if (d > width) {
    d = (d - width) & ~0x07; // must be multiple of 8
    xTitleRight -= d;
  }

  yTitleTop = 0;
  yTitleBottom = pFontOsdTitle->Height();
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  xLogoLeft = xTitleLeft;
  xLogoRight = xLogoLeft + LogoSize;
  yLogoTop = yTitleDecoBottom + TitleDecoGap2;
  yLogoBottom = yLogoTop + std::max(LogoSize, NumTracks * lineHeight);
  xListLeft = fShowSymbol ? (xLogoRight + LogoDecoGap2) : 0;
  xListRight = xTitleRight;
  yListTop = yLogoTop;
  yListBottom = yLogoBottom;
  xBottomLeft = xTitleLeft;
  xBottomRight = xTitleRight;
  yBottomTop = yListBottom + SmallGap;
  yBottomBottom = yBottomTop + pFontDate->Height();

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + OsdSize.h - yBottomBottom);
  tArea Areas[] = { {xTitleLeft, yTitleTop, xBottomRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((Areas[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayTracks: using %dbpp single area", Areas[0].bpp);
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  } else {
    debug("cSkinEnigmaDisplayTracks: using multiple areas");
    if (fShowSymbol) {
      tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom- 1, 2},
                         {xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, 4},
                         {xListLeft, yListTop, xListRight - 1, yListBottom - 1, 2},
                         {xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, 2}
      };
      int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayTracks: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
    } else {
      tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom- 1, 2},
                         {xListLeft, yListTop, xListRight - 1, yListBottom - 1, 2},
                         {xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, 2}
      };
      int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayTracks: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
    }
  }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  // draw titlebar
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1, yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawText(xTitleLeft + Roundness + 3, yTitleTop + 3, Title,
                Theme.Color(clrTitleShadow), clrTransparent,
                pFontOsdTitle,
                xTitleRight - Roundness - xTitleLeft - Roundness,
                yTitleBottom - yTitleTop - 3, fShowSymbol ? taCenter : taLeft);
  osd->DrawText(xTitleLeft + Roundness, yTitleTop, Title,
                Theme.Color(clrTitleFg), clrTransparent,
                pFontOsdTitle,
                xTitleRight - Roundness - xTitleLeft - Roundness, yTitleBottom - yTitleTop,
                fShowSymbol ? taCenter : taLeft);
  // draw rounded left corner of titlebar
  osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                   yTitleTop + Roundness - 1, clrTransparent, -2);
  // draw rounded right corner of titlebar
  osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1,
                   yTitleTop + Roundness - 1, clrTransparent, -1);
  if (fShowSymbol) {
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  }
  // draw list area
  osd->DrawRectangle(xListLeft, yListTop, xListRight - 1, yListBottom - 1, Theme.Color(clrBackground));
  // draw bottom
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, Theme.Color(clrBottomBg));
  osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                   xBottomLeft + Roundness, yBottomBottom - 1, clrTransparent, -3);
  osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                   xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);
  // fill up audio tracks
  SetAudioChannel(cDevice::PrimaryDevice()->GetAudioChannel());
  for (int i = 0; i < NumTracks; i++)
    SetItem(Tracks[i], i, false);
}

cSkinEnigmaDisplayTracks::~cSkinEnigmaDisplayTracks()
{
  delete osd;
}

void cSkinEnigmaDisplayTracks::SetItem(const char *Text, int Index, bool Current)
{
  int xItemLeft = xListLeft + (EnigmaConfig.showMarker ? lineHeight : SmallGap);
  int y = yListTop + Index * lineHeight;
  tColor ColorFg, ColorBg;
  if (Current) {
    ColorFg = Theme.Color(clrMenuItemSelectableFg);
    ColorBg = Theme.Color(clrAltBackground);
    currentIndex = Index;
  } else {
    ColorFg = Theme.Color(clrMenuItemSelectableFg);
    ColorBg = Theme.Color(clrBackground);
  }
  // draw track id
  osd->DrawRectangle(xListLeft, y, xListRight, y + lineHeight, ColorBg);
  if (EnigmaConfig.showMarker) {
    osd->DrawEllipse(xListLeft + MarkerGap, y + MarkerGap,
                     xListLeft + lineHeight - MarkerGap,
                     y + lineHeight - MarkerGap,
                     Current ? ColorFg : ColorBg);
  }
  osd->DrawText(xItemLeft, y, Text, ColorFg, ColorBg, pFontListItem, xListRight - xItemLeft - SmallGap, lineHeight);
}

void cSkinEnigmaDisplayTracks::SetAudioChannel(int AudioChannel)
{
  if (fShowSymbol) {
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    if (!(AudioChannel >= 0 && AudioChannel < MAX_AUDIO_BITMAPS))
      AudioChannel = 0;
    if (EnigmaLogoCache.LoadIcon(strAudio_large[AudioChannel]))
      osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                      yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                      EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                      Theme.Color(clrLogoBg));
  } else {
    if (!(AudioChannel >= 0 && AudioChannel < MAX_AUDIO_BITMAPS))
      AudioChannel = 0;
    osd->DrawBitmap(xTitleRight - Roundness - bmAudio[AudioChannel].Width(),
                    yTitleTop + (yTitleBottom - yTitleTop - bmAudio[AudioChannel].Height()) / 2,
                    bmAudio[AudioChannel], Theme.Color(clrTitleFg),
                    Theme.Color(clrTitleBg));
  }
}

void cSkinEnigmaDisplayTracks::SetTrack(int Index, const char *const *Tracks)
{
  if (currentIndex >= 0)
    SetItem(Tracks[currentIndex], currentIndex, false);
  SetItem(Tracks[Index], Index, true);
}

void cSkinEnigmaDisplayTracks::Flush(void)
{
  time_t now = time(NULL);
  if (now != lastTime) {
    lastTime = now;
    cString date = DayDateTime();
    osd->DrawText(xBottomLeft + Roundness, yBottomTop, date,
                  Theme.Color(clrTitleFg), Theme.Color(clrBottomBg),
                  pFontDate,
                  xBottomRight - Roundness - xBottomLeft - Roundness - 1,
                  yBottomBottom - yBottomTop - 1, taRight);
  }
  osd->Flush();
}

// --- cSkinEnigmaDisplayMessage --------------------------------------------

class cSkinEnigmaDisplayMessage : public cSkinDisplayMessage {
private:
  cOsd *osd;
  const cFont *pFontMessage;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom, xLogoDecoLeft, xLogoDecoRight;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;

  bool fShowSymbol;
#ifndef DISABLE_ANIMATED_TEXT
  int idMessage;
  bool fLocked;
#endif

public:
  cSkinEnigmaDisplayMessage();
  virtual ~ cSkinEnigmaDisplayMessage();
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
};

cSkinEnigmaDisplayMessage::cSkinEnigmaDisplayMessage()
{
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  const cFont *pFontOsdTitle;
  const cFont *pFontDate;
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);
  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);

#ifndef DISABLE_ANIMATED_TEXT
  idMessage = -1;
#endif
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsMsgs;

  int LogoSize = std::max(pFontOsdTitle->Height() + TitleDeco +
                          pFontMessage->Height() +
                          5 * SmallGap +
                          pFontDate->Height(), IconHeight);
  LogoSize += (LogoSize % 2 ? 1 : 0);

  xLogoLeft = 0;
  xLogoRight = xLogoLeft + LogoSize;
  xLogoDecoLeft = xLogoRight + LogoDecoGap;
  xLogoDecoRight = xLogoDecoLeft + LogoDecoWidth;
  yLogoTop = 0;
  yLogoBottom = yLogoTop + LogoSize;
  xTitleLeft = fShowSymbol ? xLogoDecoRight + LogoDecoGap2 : 0;
  xTitleRight = OsdSize.w;
  yTitleTop = yLogoTop;
  yTitleBottom = yTitleTop + pFontOsdTitle->Height();
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  xBottomLeft = xTitleLeft;
  xBottomRight = xTitleRight;
  yBottomTop = yLogoBottom - pFontDate->Height();
  yBottomBottom = yLogoBottom;
  xMessageLeft = xTitleLeft;
  xMessageRight = xTitleRight;
  yMessageTop = yTitleDecoBottom + TitleDecoGap2;
  yMessageBottom = yBottomTop - SmallGap;

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + OsdSize.h - yBottomBottom);
  tArea Areas[] = { {xLogoLeft, yLogoTop, xBottomRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((Areas[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayMessage: using %dbpp single area", Areas[0].bpp);
    osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  } else {
    debug("cSkinEnigmaDisplayMessage: using multiple areas");
    if (fShowSymbol) {
      tArea Areas[] = { {xLogoLeft, yLogoTop, xLogoRight + LogoDecoGap + LogoDecoWidth - 1, yLogoBottom - 1, 4},
                        {xTitleLeft, yTitleTop, xBottomRight - 1, yBottomBottom - 1, 4} };
      int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayMessage: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
    } else {
      tArea Areas[] = { {xTitleLeft, yTitleTop, xBottomRight - 1, yBottomBottom - 1, 4} };
      int rc = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
      if (rc == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
      else {
        error("cSkinEnigmaDisplayMessage: CanHandleAreas() returned %d", rc);
        delete osd;
        osd = NULL;
        throw 1;
        return;
      }
    }
  }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);

#ifndef DISABLE_ANIMATED_TEXT
  fLocked = false;
  if (EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther) {
    TE_START(osd);
    fLocked = true;
  }
#endif
}

cSkinEnigmaDisplayMessage::~cSkinEnigmaDisplayMessage()
{
  debug("cSkinEnigmaDisplayMessage::~cSkinEnigmaDisplayMessage");

#ifndef DISABLE_ANIMATED_TEXT
  if (EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther) {
    if (!fLocked) TE_LOCK;
    TE_STOP;
  }
#endif
  delete osd;
}

void cSkinEnigmaDisplayMessage::SetMessage(eMessageType Type, const char *Text)
{
  debug("cSkinEnigmaDisplayMessage::SetMessage");
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked && EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther)
    TE_LOCK;
#endif
  if (fShowSymbol) {
    // draw logo
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    osd->DrawRectangle(xLogoDecoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    if (EnigmaLogoCache.LoadIcon("icons/message/info"))
      osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                      yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2, EnigmaLogoCache.Get(),
                      EnigmaLogoCache.Get().Color(1), Theme.Color(clrLogoBg));
  }
  // draw title
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1, yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1, yTitleTop + Roundness - 1, clrTransparent, -1);
  // draw centered message text
  osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1, yMessageBottom - 1, clrTransparent);
  osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1,
                     yMessageBottom - 1, Theme.Color(clrButtonRedBg));
#ifndef DISABLE_ANIMATED_TEXT
  if (EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther)
    idMessage = TE_MARQUEE(osd, idMessage, xMessageLeft, yMessageTop + 2 * SmallGap, Text,
                           Theme.Color(clrMessageStatusFg + 2 * Type),
                           Theme.Color(clrMessageStatusBg + 2 * Type),
                           FONT_MESSAGE,
                           xMessageRight - xMessageLeft,
                           yMessageBottom - 2 * SmallGap - yMessageTop - 2 * SmallGap, taCenter);
  else
#endif
    osd->DrawText(xMessageLeft, yMessageTop + SmallGap, Text,
                  Theme.Color(clrMessageStatusFg + 2 * Type),
                  Theme.Color(clrMessageStatusBg + 2 * Type),
                  pFontMessage,
                  xMessageRight - xMessageLeft - 1,
                  yMessageBottom - SmallGap - yMessageTop - SmallGap - 1, taCenter);
  // draw bottom
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, Theme.Color(clrBottomBg));
  osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                   xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);

  if (!(fShowSymbol)) {
    // draw ellipse if not showing logo
    osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1, yTitleTop + Roundness - 1, clrTransparent, -2);
    osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                     xBottomLeft + Roundness - 1, yBottomBottom - 1, clrTransparent, -3);
  }
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked && EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther)
    TE_UNLOCK;
#endif
}

void cSkinEnigmaDisplayMessage::Flush(void)
{
#ifndef DISABLE_ANIMATED_TEXT
  if (!fLocked) TE_LOCK;
#endif
  osd->Flush();
#ifndef DISABLE_ANIMATED_TEXT
  TE_UNLOCK;
  if (fLocked) {
    fLocked = false;
    TE_WAKEUP;
  }
#endif
}

// --- cSkinEnigma ----------------------------------------------------------

cSkinEnigma::cSkinEnigma() : cSkin("EnigmaNG", &::Theme)
{
  // Get the "classic" skin to be used as fallback skin if any of the OSD
  // menu fails to open.
  skinFallback = Skins.First();
  for (cSkin *Skin = Skins.First(); Skin; Skin = Skins.Next(Skin)) {
    if (strcmp(Skin->Name(), "classic") == 0) {
      skinFallback = Skin;
      break;
    }
  }
}

const char *cSkinEnigma::Description(void)
{
  return tr("EnigmaNG");
}

cSkinDisplayChannel *cSkinEnigma::DisplayChannel(bool WithInfo)
{
  try {
    return new cSkinEnigmaDisplayChannel(WithInfo);
  } catch(...) {
    return skinFallback->DisplayChannel(WithInfo);
  }
}

cSkinDisplayMenu *cSkinEnigma::DisplayMenu(void)
{
  try {
    return new cSkinEnigmaDisplayMenu;
  } catch (...) {
    return skinFallback->DisplayMenu();
  }
}

cSkinDisplayReplay *cSkinEnigma::DisplayReplay(bool ModeOnly)
{
  try {
    return new cSkinEnigmaDisplayReplay(ModeOnly);
  } catch (...) {
    return skinFallback->DisplayReplay(ModeOnly);
  }
}

cSkinDisplayVolume *cSkinEnigma::DisplayVolume(void)
{
  try {
    return new cSkinEnigmaDisplayVolume;
  } catch (...) {
    return skinFallback->DisplayVolume();
  }
}

cSkinDisplayTracks *cSkinEnigma::DisplayTracks(const char *Title, int NumTracks, const char *const *Tracks)
{
  try {
    return new cSkinEnigmaDisplayTracks(Title, NumTracks, Tracks);
  } catch (...) {
    return skinFallback->DisplayTracks(Title, NumTracks, Tracks);
  }
}

cSkinDisplayMessage *cSkinEnigma::DisplayMessage(void)
{
  try {
    return new cSkinEnigmaDisplayMessage;
  } catch (...) {
    return skinFallback->DisplayMessage();
  }
}

// vim:et:sw=2:ts=2:

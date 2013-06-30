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
#include <vdr/videodir.h>

#ifndef DISABLE_SIGNALINFO
#include <sys/ioctl.h>
#include <linux/dvb/frontend.h>
#endif //DISABLE_SIGNALINFO

#ifdef USE_PLUGIN_EPGSEARCH
#include "services/epgsearch.h"
#endif

#ifdef USE_PLUGIN_AVARDS
#include "services/avards.h"
#endif

#ifdef USE_PLUGIN_MAILBOX
#include "services/mailbox.h"
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
#include "symbols/small/subtitle.xpm"
#include "symbols/small/vps.xpm"
#include "symbols/small/radio.xpm"
#include "symbols/small/recording.xpm"
#include "symbols/small/timer.xpm"
#include "symbols/small/run.xpm"
#ifdef USE_PLUGIN_MAILBOX
#include "symbols/small/mail.xpm"
#endif
#ifdef USE_PLUGIN_AVARDS
#define MAX_AVARDS_MODES 6
#include "symbols/small/v_unknown.xpm"
#include "symbols/small/v_4_3.xpm"
#include "symbols/small/v_l14_9.xpm"
#include "symbols/small/v_l16_9.xpm"
#include "symbols/small/v_l_16_9.xpm"
#include "symbols/small/v_16_9.xpm"
#endif


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
static cBitmap bmSubtitle(subtitle_xpm);
static cBitmap bmVPS(vps_xpm);
static cBitmap bmRun(run_xpm);
static cBitmap bmTimer(timer_xpm);
#ifdef USE_PLUGIN_MAILBOX
static cBitmap bmMail(mail_xpm);
#endif
#ifdef USE_PLUGIN_AVARDS
static cBitmap bmWSS[MAX_AVARDS_MODES] = {
  cBitmap(v_unknown_xpm),
  cBitmap(v_4_3_xpm),
  cBitmap(v_l14_9_xpm),
  cBitmap(v_l16_9_xpm),
  cBitmap(v_l_16_9_xpm),
  cBitmap(v_16_9_xpm)
};
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
#ifndef DISABLE_SIGNALINFO
// Channel info
THEME_CLR(Theme, clrSignalHighFg, 0xFF11BB10);
THEME_CLR(Theme, clrSignalMediumFg, 0xFFC4C400);
THEME_CLR(Theme, clrSignalLowFg, 0xFFCC0000);
#endif //DISABLE_SIGNALINFO


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
#define ListHBorder Gap
#define ScrollbarHeight 20
#define ScrollbarWidth 16


// --- cSkinEnigmaDisplayChannel --------------------------------------------

class cSkinEnigmaDisplayChannel : public cSkinDisplayChannel, public cSkinEnigmaBaseOsd {
private:
  bool fShowLogo;
  bool fWithInfo;
  char *strLastDate;
  int nMessagesShown;
  bool fScrollTitle;
  bool fScrollOther;
  bool fLocked;
  bool fLockNeeded;
  int idTitle;
  int idEvTitle;
  int idEvSubTitle;
  int idEvCat;
  int nBPP;

  const cFont *pFontOsdTitle;
  const cFont *pFontDate;
  const cFont *pFontTitle;
  const cFont *pFontSubtitle;
  const cFont *pFontLanguage;
  const cFont *pFontMessage;

  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom, xLogoDecoLeft, xLogoDecoRight;
  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xEventNowLeft, xEventNowRight, yEventNowTop, yEventNowBottom;
  int xEventNextLeft, xEventNextRight, yEventNextTop, yEventNextBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;
  int xFirstSymbol, xDateLeft, xIndent;
#ifndef DISABLE_SIGNALINFO
  int xSignalBarLeft, nStrBarWidth, nSnrBarWidth;
#if VDRVERSNUM < 10719
  int m_Frontend;
#endif
  cTimeMs UpdateSignalTimer;
#endif //DISABLE_SIGNALINFO

  void DrawAreas(void);
  void DrawGroupInfo(const cChannel *Channel, int Number);
  void DrawChannelInfo(const cChannel *Channel, int Number);
  void DrawSymbols(const cChannel *Channel);
#ifndef DISABLE_SIGNALINFO
  int GetSignal(int &str, int &snr, fe_status_t &status);
  void UpdateSignal(void);
#endif //DISABLE_SIGNALINFO
  cString GetChannelName(const cChannel *Channel);
  cString GetChannelNumber(const cChannel *Channel, int Number);
  int FindCatTextAndLen(const cEvent* e, int& xTextWidth, std::string& cat);
public:
  cSkinEnigmaDisplayChannel(bool WithInfo);
  virtual ~ cSkinEnigmaDisplayChannel();
  virtual void SetChannel(const cChannel *Channel, int Number);
  virtual void SetEvents(const cEvent *Present, const cEvent *Following);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
};

cSkinEnigmaDisplayChannel::cSkinEnigmaDisplayChannel(bool WithInfo)
{
  debug("cSkinEnigmaDisplayChannel::cSkinEnigmaDisplayChannel(%d)", WithInfo);

  fWithInfo = WithInfo;
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  xIndent = EnigmaConfig.drawRoundCorners ? Roundness : SmallGap;

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontTitle = EnigmaConfig.GetFont(FONT_CITITLE);
  pFontSubtitle = EnigmaConfig.GetFont(FONT_CISUBTITLE);
  pFontLanguage = EnigmaConfig.GetFont(FONT_CILANGUAGE);
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);

  fShowLogo = EnigmaConfig.showLogo;
  xFirstSymbol = 0;
#ifndef DISABLE_SIGNALINFO
  nStrBarWidth = 10000;
  nSnrBarWidth = 10000;
#if VDRVERSNUM < 10719
  m_Frontend = -1;
#endif
  UpdateSignalTimer.Set();
#endif //DISABLE_SIGNALINFO
  nMessagesShown = 0;
  strLastDate = NULL;
  fScrollTitle = EnigmaConfig.useTextEffects && EnigmaConfig.scrollTitle;
  fScrollOther = EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther;
  idTitle = -1;
  idEvTitle = -1;
  idEvSubTitle = -1;
  idEvCat = -1;
  nBPP = 1;

  int MessageHeight = 2 * SmallGap + pFontMessage->Height() + 2 * SmallGap;
  int LogoHeight = 0;
  if (fWithInfo)
    LogoHeight = std::max(pFontTitle->Height() * 2 + pFontSubtitle->Height() * 2 + SmallGap, EnigmaConfig.channelLogoHeight);
  else
    LogoHeight = std::max(std::max(pFontOsdTitle->Height(), pFontDate->Height()) + TitleDecoGap + TitleDecoHeight + TitleDecoGap2 + MessageHeight + pFontLanguage->Height() + SmallGap, EnigmaConfig.channelLogoHeight);

  // title bar & logo area
  xLogoLeft = 0;
  xLogoRight = xLogoLeft + EnigmaConfig.channelLogoWidth;
  xLogoDecoLeft = xLogoRight + LogoDecoGap;
  xLogoDecoRight = xLogoDecoLeft + LogoDecoWidth;
  xTitleLeft = (fShowLogo && (!EnigmaConfig.fullTitleWidth || !fWithInfo) ? xLogoDecoRight + LogoDecoGap2 : xLogoLeft);
  xTitleRight = xTitleLeft + ((OsdSize.w - xTitleLeft) & ~0x07); // width must be multiple of 8
  yTitleTop = 0;
  yTitleBottom = yTitleTop + std::max(pFontOsdTitle->Height(), pFontDate->Height());
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  yLogoTop = (fWithInfo ? yTitleDecoBottom + TitleDecoGap2 : yTitleTop);
  yLogoBottom = yLogoTop + LogoHeight;
  if (fWithInfo) {
    // current event area
    xEventNowLeft = (fShowLogo ? xLogoDecoRight + LogoDecoGap2 : xTitleLeft);
    xEventNowRight = xTitleRight;
    yEventNowTop = yLogoTop;
    yEventNowBottom = yEventNowTop + (LogoHeight - SmallGap) / 2;
    // next event area
    xEventNextLeft = xEventNowLeft;
    xEventNextRight = xEventNowRight;
    yEventNextTop = yEventNowBottom + SmallGap;
    yEventNextBottom = yLogoBottom;
  } else {
    xEventNowLeft   = xEventNextLeft   = (fShowLogo ? xLogoDecoRight + LogoDecoGap2 : xTitleLeft);
    xEventNowRight  = xEventNextRight  = xTitleRight;
    yEventNowTop    = yEventNextTop    = yTitleDecoBottom + TitleDecoGap2;
    yEventNowBottom = yEventNextBottom = yLogoBottom - pFontLanguage->Height() - SmallGap;
  }
  // progress bar area
  xBottomLeft = xTitleLeft;
  xBottomRight = xTitleRight;
  yBottomTop = yEventNextBottom + SmallGap;
  yBottomBottom = yBottomTop + pFontLanguage->Height();
  // message area
  xMessageLeft = xEventNowLeft;
  xMessageRight = xTitleRight;
  yMessageTop = yLogoTop + (LogoHeight - MessageHeight) / 2;
  yMessageBottom = yMessageTop + MessageHeight;
  // date area
  cString date = DayDateTime();
  int w = pFontDate->Width(date);
  xDateLeft = xTitleRight - xIndent - w - SmallGap;
#ifndef DISABLE_SIGNALINFO
  xSignalBarLeft = EnigmaConfig.showSignalInfo ? (xBottomRight - xIndent - EnigmaConfig.signalInfoWidth) : -1;
#endif

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + (Setup.ChannelInfoPos ? 0 : (OsdSize.h - yBottomBottom)) );
  tArea SingleArea[] = { {0, 0, xBottomRight - 1, yBottomBottom - 1, fShowLogo || EnigmaConfig.showFlags ? 8 : 4} };
  if ((SingleArea[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayChannel: using %dbpp single area", SingleArea[0].bpp);
    osd->SetAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea));
    // clear all
    osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
    if (SingleArea[0].bpp >=8 && Setup.AntiAlias)
      nBPP = 8;
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

  fLockNeeded = (fScrollTitle || fScrollOther);
  fLocked = false;
  TE_START(osd);
  if (fLockNeeded) {
    fLocked = true;
  }
}

cSkinEnigmaDisplayChannel::~cSkinEnigmaDisplayChannel()
{
  debug("cSkinEnigmaDisplayChannel::~cSkinEnigmaDisplayChannel()");

  if (fLockNeeded && !fLocked) TE_LOCK;
  TE_STOP;

#ifndef DISABLE_SIGNALINFO
#if VDRVERSNUM < 10719
  if (m_Frontend >= 0)
    close(m_Frontend);
#endif
#endif //DISABLE_SIGNALINFO

  free(strLastDate);
  delete osd;
}

void cSkinEnigmaDisplayChannel::DrawAreas(void)
{
  // draw titlebar
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1,
                     Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1,
                     yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  if (fWithInfo) {
    // draw current event area
    osd->DrawRectangle(xEventNowLeft, yEventNowTop, xEventNowRight - 1,
                       yEventNowBottom - 1, Theme.Color(clrBackground));
    // draw next event area
    osd->DrawRectangle(xEventNextLeft, yEventNextTop, xEventNextRight - 1,
                       yEventNextBottom - 1, Theme.Color(clrAltBackground));
  } else {
    osd->DrawRectangle(xEventNowLeft, yEventNowTop, xEventNowRight - 1,
                       yEventNextBottom - 1, Theme.Color(clrBackground));
  }
  // draw progress bar area
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1,
                     yBottomBottom - 1, Theme.Color(clrBottomBg));

  if (EnigmaConfig.drawRoundCorners) {
    // draw rounded corners
    if (fWithInfo) {
      osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                       yTitleTop + Roundness - 1, clrTransparent, -2);
      osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                       xBottomLeft + Roundness, yBottomBottom - 1, clrTransparent, -3);
    }
    osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1,
                     yTitleTop + Roundness - 1, clrTransparent, -1);
    osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                     xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);
  }
}

void cSkinEnigmaDisplayChannel::DrawGroupInfo(const cChannel *Channel, int /* Number */)
{
  DrawAreas();

  int xName = (fShowLogo && EnigmaConfig.fullTitleWidth ? xEventNowLeft : xTitleLeft + xIndent + pFontOsdTitle->Width("0000-") + Gap);
  idTitle = TE_MARQUEE(osd, idTitle, fScrollTitle, xName, yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2, GetChannelName(Channel), Theme.Color(clrTitleFg), Theme.Color(clrTitleBg), pFontOsdTitle, nBPP, xDateLeft - SmallGap - xName);
}

void cSkinEnigmaDisplayChannel::DrawChannelInfo(const cChannel *Channel, int Number)
{
  DrawAreas();

  int xNumber = xTitleLeft + xIndent;
  int xName = xNumber + pFontOsdTitle->Width("0000-") + Gap;
  if (fShowLogo && EnigmaConfig.fullTitleWidth && fWithInfo) {
    xNumber = xTitleLeft + xIndent;
    xName = xEventNowLeft;
  }

  // draw channel number
  osd->DrawText(xNumber, yTitleTop, GetChannelNumber(Channel, Number),
                Theme.Color(clrTitleFg), clrTransparent, pFontOsdTitle,
                xName - xNumber - Gap, yTitleBottom - yTitleTop, taCenter);

  // draw channel name
  idTitle = TE_MARQUEE(osd, idTitle, fScrollTitle, xName, yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2, GetChannelName(Channel), Theme.Color(clrTitleFg), Theme.Color(clrTitleBg), pFontOsdTitle, nBPP, xDateLeft - SmallGap - xName);

  if (fWithInfo && EnigmaConfig.showStatusSymbols)
    DrawSymbols(Channel);
}

void cSkinEnigmaDisplayChannel::DrawSymbols(const cChannel *Channel)
{
  // draw symbols
  // right edge of logo
  int xs = xBottomRight - xIndent;
  // bottom edge of logo
  int ys = yBottomTop + (yBottomBottom - yBottomTop - SymbolHeight) / 2;

#ifndef DISABLE_SIGNALINFO
  if (xSignalBarLeft >= 0)
    xs = xSignalBarLeft - Gap;
#endif //DISABLE_SIGNALINFO

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

  // draw audio symbol according to current audio channel
  int AudioMode = cDevice::PrimaryDevice()->GetAudioChannel();
  if (!(AudioMode >= 0 && AudioMode < MAX_AUDIO_BITMAPS))
    AudioMode = 0;
  xs -= (bmAudio[AudioMode].Width() + SmallGap);
  osd->DrawBitmap(xs, ys, bmAudio[AudioMode],
                  Theme.Color(clrBottomBg), Theme.Color(Channel->Apid(1) ? clrSymbolActive : clrSymbolInactive));

  if (Channel->Dpid(0) || EnigmaConfig.showStatusSymbols == 1) {
    // draw dolby digital symbol
    xs -= (bmDolbyDigital.Width() + SmallGap);
    osd->DrawBitmap(xs, ys, bmDolbyDigital,
                    Theme.Color(clrBottomBg), Theme.Color(Channel->Dpid(0) ? clrSymbolActive : clrSymbolInactive));
  }

  if (Channel->Spid(0) || EnigmaConfig.showStatusSymbols == 1) {
    // draw subtitle symbol
    xs -= (bmSubtitle.Width() + SmallGap);
    osd->DrawBitmap(xs, ys, bmSubtitle,
                    Theme.Color(clrBottomBg), Theme.Color(Channel->Spid(0) ? clrSymbolActive : clrSymbolInactive));
  }

  if ((isvps || EnigmaConfig.showStatusSymbols == 1) && EnigmaConfig.showVps) {
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

  if (EnigmaConfig.showCaMode == 0) {
    if (Channel->Ca() >= 0x0100 || EnigmaConfig.showStatusSymbols == 1) {
      // draw encryption symbol
      xs -= (bmEncrypted.Width() + SmallGap);
      osd->DrawBitmap(xs, ys, bmEncrypted,
                      Theme.Color(clrBottomBg), Theme.Color(Channel->Ca() >= 0x0100 ? clrSymbolActive : clrSymbolInactive));
    }
  } else {
    const char *strCA = NULL;
    switch (Channel->Ca()) {
      case 0x0000:            /* Free-To-Air */        strCA = tr("Free-To-Air"); break;
      case 0x0001 ... 0x000F: /* Reserved */           break;
      case 0x0100 ... 0x01FF: /* Canal Plus */         strCA = "Seca"; break;
      case 0x0500 ... 0x05FF: /* France Telecom */     strCA = "Viaccess"; break;
      case 0x0600 ... 0x06FF: /* Irdeto */             strCA = "Irdeto"; break;
      case 0x0900 ... 0x09FF: /* News Datacom */       strCA = "NDS"; break;
      case 0x0B00 ... 0x0BFF: /* Norwegian Telekom */  strCA = "Conax"; break;
      case 0x0D00 ... 0x0DFF: /* Philips */            strCA = "Cryptoworks"; break;
      case 0x0E00 ... 0x0EFF: /* Scientific Atlanta */ strCA = "Powervu"; break;
      case 0x1200 ... 0x12FF: /* BellVu Express */     strCA = "Nagravision"; break;
      case 0x1700 ... 0x17FF: /* BetaTechnik */        strCA = "Betacrypt"; break;
      case 0x1800 ... 0x18FF: /* Kudelski SA */        strCA = "Nagravision"; break;
      case 0x2200 ... 0x22FF: /* Scopus */             strCA = "CodiCrypt"; break;
      case 0x2600           : /* EBU */                strCA = "BISS"; break;
      case 0x4A20 ... 0x4A2F: /* AlphaCrypt */         strCA = "AlphaCrypt"; break;
      case 0x4A60 ... 0x4A6F: /* @Sky */               strCA = "Skycrypt"; break;
      case 0x4A70 ... 0x4A7F: /* Dreamcrypt */         strCA = "DreamCrypt"; break;
      case 0x4A80 ... 0x4A8F: /* THALESCrypt */        strCA = "ThalesCrypt"; break;
      case 0x4AA0 ... 0x4AAF: /* SIDSA */              strCA = "KeyFly"; break;
      case 0x4AD0 ... 0x4AD1: /* XCrypt Inc */         strCA = "XCrypt"; break;
      case 0x4AE0 ... 0x4AE1: /* Digi Raum Electronics */          strCA = "DRE-Crypt"; break;
      default:                                         strCA = tr("encrypted"); break;
    }
    if (strCA) {
      xs -= (pFontLanguage->Width(strCA) + SmallGap);
      osd->DrawText(xs, yBottomTop + SmallGap , strCA,
                    Theme.Color(clrSymbolActive), Theme.Color(clrBottomBg), pFontLanguage,
                    pFontLanguage->Width(strCA), yBottomBottom - SmallGap);
    }
  }

  xFirstSymbol = DrawStatusSymbols(xBottomLeft + xIndent + EnigmaConfig.progressBarWidth + Gap, xs, yBottomTop, yBottomBottom, Channel) - Gap;
}

#ifndef DISABLE_SIGNALINFO
#define FRONTEND_DEVICE "/dev/dvb/adapter%d/frontend%d"

int cSkinEnigmaDisplayChannel::GetSignal(int &str, int &snr, fe_status_t & /* status */) {
#if VDRVERSNUM < 10719
  if (m_Frontend < 0) {
    str = 0;
    snr = 0;

    int const cardIndex = cDevice::ActualDevice()->CardIndex();
    static char dev[256];

    sprintf(dev, FRONTEND_DEVICE, cardIndex, 0);
    m_Frontend = open(dev, O_RDONLY | O_NONBLOCK);
    if (m_Frontend < 0)
      return -1;
  } else if (UpdateSignalTimer.Elapsed() < 500) {
    return 0;
  }

  ::ioctl(m_Frontend, FE_READ_SIGNAL_STRENGTH, &str);
  ::ioctl(m_Frontend, FE_READ_SNR, &snr);
#else
  if (UpdateSignalTimer.Elapsed() < 500) {
    return 0;
  }
  cDevice* dev = cDevice::ActualDevice();
  str = dev->SignalStrength();
  snr = dev->SignalQuality();
#endif
  UpdateSignalTimer.Set();

  return 0;
}

void cSkinEnigmaDisplayChannel::UpdateSignal() {
  if (xSignalBarLeft <= 0)
    return;

  int str = -1;
  int snr = -1;
  fe_status_t status;
  if (GetSignal(str, snr, status) < 0)
    return;

  if (snr < 0 && str < 0)
    return;

  int bw = EnigmaConfig.signalInfoWidth;
  int xSignalBarRight = xSignalBarLeft + bw;

#if VDRVERSNUM < 10719
  str = str * bw / 0xFFFF;
  snr = snr * bw / 0xFFFF;
#else
  str = str * bw / 100;
  snr = snr * bw / 100;
#endif

  if (str != nStrBarWidth || snr != nSnrBarWidth) {
    nStrBarWidth = str;
    nSnrBarWidth = snr;

    int h = int((yBottomBottom - Gap - yBottomTop - Gap - Gap ) / 2);
    int yStr = yBottomTop + Gap;
    int ySnr = yStr + h + Gap;

    // Draw Background
    osd->DrawRectangle(xSignalBarLeft, yStr, xSignalBarRight - 1, yStr + h , Theme.Color(clrBotProgBarBg));
    osd->DrawRectangle(xSignalBarLeft, ySnr, xSignalBarRight - 1, ySnr + h , Theme.Color(clrBotProgBarBg));

    // Draw Foreground
    int signalFgColor = Theme.Color(clrSignalHighFg);
    if (str <= 0.5 * bw) // low signal : RED
      signalFgColor = Theme.Color(clrSignalLowFg);
    else if (str <= 0.6 * bw) // medium signal : ORANGE
      signalFgColor = Theme.Color(clrSignalMediumFg);

    if (str)
      osd->DrawRectangle(xSignalBarLeft, yStr , xSignalBarLeft + str - 1, yStr + h , signalFgColor);

    signalFgColor = Theme.Color(clrSignalHighFg);
    if (snr <= 0.5 * bw) // low signal : RED
      signalFgColor = Theme.Color(clrSignalLowFg);
    else if (snr <= 0.6 * bw) // medium signal : ORANGE
      signalFgColor = Theme.Color(clrSignalMediumFg);

    if (snr)
      osd->DrawRectangle(xSignalBarLeft, ySnr , xSignalBarLeft + snr - 1, ySnr + h , signalFgColor);
  }
}
#endif //DISABLE_SIGNALINFO

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

int cSkinEnigmaDisplayChannel::FindCatTextAndLen(const cEvent* e, int& xTextWidth, std::string& cat)
{
  int xCatWidth = 0;
  cat = ExtractAttribute(e->Description(), EVENT_CATEGORY);
  std::string gen = ExtractAttribute(e->Description(), EVENT_GENRE);
  if (!cat.empty() && !gen.empty())
    cat += " - ";
  cat += gen;

#if VDRVERSNUM >= 10711
  if (cat.empty()) {
    std::string strInfo;
    bool fFirst = true;
    for (int i = 0; e->Contents(i); i++) {
      const char *s = e->ContentToString(e->Contents(i));
      if (!isempty(s)) {
        if (!fFirst)
          strInfo += " ";
        fFirst = false;
        strInfo += s;
      }
    }
    cat += strInfo;
  }
#endif

  if (!cat.empty()) {
    if (pFontSubtitle->Width(e->ShortText()) > xTextWidth * .5)
      xCatWidth = std::min((int)(xTextWidth * .3), pFontLanguage->Width(cat.c_str()));
    else
      xCatWidth = std::min((int)(xTextWidth * .5), pFontLanguage->Width(cat.c_str()));
    xTextWidth -= xCatWidth;
  }

  return xCatWidth;

}

void cSkinEnigmaDisplayChannel::SetChannel(const cChannel *Channel, int Number)
{
  debug("cSkinEnigmaDisplayChannel::SetChannel()");

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }

#ifndef DISABLE_SIGNALINFO
#if VDRVERSNUM < 10719
  if (m_Frontend >= 0) {
    close(m_Frontend);
    m_Frontend = -1;
  }
#endif
#endif //DISABLE_SIGNALINFO

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
        osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                        yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                        EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                        Theme.Color(clrLogoBg), true);
      }
    }

    if (Channel->GroupSep())
      DrawGroupInfo(Channel, Number);
    else
      DrawChannelInfo(Channel, Number);
  }

  if (fLockNeeded && !fLocked)
    TE_UNLOCK;
}

void cSkinEnigmaDisplayChannel::SetEvents(const cEvent *Present,
                                          const cEvent *Following)
{ //TODO: update symbols
  debug("cSkinEnigmaDisplayChannel::SetEvents(%p, %p)", Present, Following);

  if (!fWithInfo)
    return;

  int xTimeLeft = xEventNowLeft + Gap;
  int wPresent = Present ? pFontTitle->Width(Present->GetTimeString()) : 0;
  int wFollowing = Following ? pFontTitle->Width(Following->GetTimeString()) : 0;
  int xTimeWidth = std::max(wPresent, wFollowing);
  if (xTimeWidth == 0)
    xTimeWidth = pFontTitle->Width("00:00");
  int lineHeightTitle = pFontTitle->Height();
  int lineHeightSubtitle = pFontSubtitle->Height();

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }

  EnigmaTextEffects.ResetText(idEvTitle, Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground), false);
  EnigmaTextEffects.ResetText(idEvSubTitle, Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrBackground), false);
  EnigmaTextEffects.ResetText(idEvCat, Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrBackground), false);
  idEvTitle = idEvSubTitle = idEvCat = -1;

  // check epg datas
  const cEvent *e = Present;    // Current event
  int wDur = 0;
  if (e) {
    char sLen[6];
    char sNow[12];
    int total = e->Duration();
    snprintf(sLen, sizeof(sLen), "%d'", total / 60);

    int now = std::max((int)(time(NULL) - e->StartTime()), 0);
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
        case 3:
          snprintf(sNow, sizeof(sNow), "+%d'/-%d'", now / 60, (int)ceil((total - now) / 60.0));
          break;
        default:
          error("Invalid value for ShowRemaining: %d", EnigmaConfig.showRemaining);
      }
    }
    wDur = std::max(pFontTitle->Width(sLen), pFontSubtitle->Width(sNow));

    int xDurationLeft = xEventNowRight - Gap - wDur;
    int xDurationWidth = xEventNowRight - Gap - xDurationLeft;
    int xTextLeft = xTimeLeft + xTimeWidth + BigGap;
    int xTextWidth = xDurationLeft - xTextLeft - BigGap;

    // draw start time
    osd->DrawText(xTimeLeft, yEventNowTop, e->GetTimeString(),
                  Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                  pFontTitle, xTimeWidth);
    // draw title
    idEvTitle = TE_MARQUEE(osd, idEvTitle, fScrollOther, xTextLeft, yEventNowTop, e->Title(),
                           Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground),
                           pFontTitle, nBPP, xTextWidth, pFontTitle->Height());

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

    if (EnigmaConfig.showCatGenCon) {
      std::string cat;
      int xCatWidth = FindCatTextAndLen(e, xTextWidth, cat);

      // draw category
      idEvCat = TE_MARQUEE(osd, idEvCat, fScrollOther, xTextLeft + xTextWidth + SmallGap, yEventNowTop + lineHeightTitle, cat.c_str(),
                                Theme.Color(clrMenuItemNotSelectableFg),
                                Theme.Color(clrBackground), pFontSubtitle, nBPP, xCatWidth - SmallGap, pFontSubtitle->Height(), taRight);
    }

    // draw shorttext
    idEvSubTitle = TE_MARQUEE(osd, idEvSubTitle, fScrollOther, xTextLeft, yEventNowTop + lineHeightTitle, e->ShortText(),
                              Theme.Color(clrMenuItemNotSelectableFg),
                              Theme.Color(clrBackground), pFontSubtitle, nBPP, xTextWidth, pFontSubtitle->Height());

    // draw duration
    if ((now < total) && ((now / 60) > 0)) {
      osd->DrawText(xDurationLeft, yEventNowTop + lineHeightTitle, sNow,
                    Theme.Color(clrMenuItemNotSelectableFg),
                    Theme.Color(clrBackground), pFontSubtitle, xDurationWidth,
                    lineHeightSubtitle, taRight);
    }
    // draw timebar
    int xBarLeft = xBottomLeft + xIndent;
    int xBarWidth = EnigmaConfig.progressBarWidth;
    int x = xBarLeft + SmallGap + (int)(ceil((float)(now) / (float)(total) * (float)(xBarWidth - SmallGap - SmallGap)));
    x = std::min(x, xBarLeft + xBarWidth - SmallGap - 1);
    osd->DrawRectangle(xBarLeft, yBottomTop + Gap,
                       xBarLeft + xBarWidth - 1,
                       yBottomBottom - Gap - 1,
                       Theme.Color(clrBotProgBarBg));
    osd->DrawRectangle(xBarLeft + SmallGap,
                       yBottomTop + Gap + SmallGap, x,
                       yBottomBottom - Gap - SmallGap - 1,
                       Theme.Color(clrBotProgBarFg));
  }

  e = Following;                // Next event
  if (e) {
    char sLen[6];
    snprintf(sLen, sizeof(sLen), "%d'", e->Duration() / 60);

    int xDurationLeft = xEventNowRight - Gap - wDur;
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

    if (EnigmaConfig.showCatGenCon) {
      std::string cat;
      int xCatWidth = FindCatTextAndLen(e, xTextWidth, cat);

      // draw category
      osd->DrawText(xTextLeft + xTextWidth + SmallGap, yEventNextTop + lineHeightTitle, cat.c_str(),
                    Theme.Color(clrMenuItemNotSelectableFg),
                    Theme.Color(clrAltBackground), pFontSubtitle, xCatWidth - SmallGap, 0, taRight);
    }

    // draw shorttext
    osd->DrawText(xTextLeft, yEventNextTop + lineHeightTitle, e->ShortText(),
                  Theme.Color(clrMenuItemNotSelectableFg),
                  Theme.Color(clrAltBackground), pFontSubtitle, xTextWidth);
  }

  if (fLockNeeded && !fLocked)
    TE_UNLOCK;
}

void cSkinEnigmaDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
  debug("cSkinEnigmaDisplayChannel::SetMessage()");

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }

  // check if message
  if (Text) {
    int top = (fWithInfo ? yMessageTop : yEventNowTop);
    int bottom = (fWithInfo ? yMessageBottom : yEventNextBottom);

    EnigmaTextEffects.PauseEffects(top);

    // save osd region
    if (nMessagesShown == 0)
      osd->SaveRegion(xMessageLeft, top, xMessageRight - 1, bottom - 1);
    // draw message
    osd->DrawRectangle(xMessageLeft, top, xMessageRight - 1,
                       bottom - 1, clrTransparent);
    osd->DrawRectangle(xMessageLeft, top + SmallGap, xMessageRight - 1,
                       bottom - SmallGap - 1,
                       Theme.Color(clrButtonRedBg));
    osd->DrawText(xMessageLeft, top + 2 * SmallGap, Text,
                  Theme.Color(clrMessageStatusFg + 2 * Type),
                  Theme.Color(clrMessageStatusBg + 2 * Type),
                  pFontMessage,
                  xMessageRight - xMessageLeft,
                  bottom - 2 * SmallGap - top - 2 * SmallGap,
                  taCenter);
    nMessagesShown++;
  } else {
    if (nMessagesShown > 0)
      nMessagesShown--;
    // restore saved osd region
    if (nMessagesShown == 0)
      osd->RestoreRegion();

    EnigmaTextEffects.PauseEffects();
  }

  if (fLockNeeded && !fLocked)
    TE_UNLOCK;
}

void cSkinEnigmaDisplayChannel::Flush(void)
{
//  debug("cSkinEnigmaDisplayChannel::Flush()");

  if (fLockNeeded && !fLocked)
    TE_LOCK;

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
#ifndef DISABLE_SIGNALINFO
  UpdateSignal();
#endif //DISABLE_SIGNALINFO
  osd->Flush();

  if (fLockNeeded) {
    TE_UNLOCK;
    if (fLocked) {
      fLocked = false;
      TE_WAKEUP;
    }
  }
}

// --- cSkinEnigmaDisplayMenu -----------------------------------------------

class cSkinEnigmaDisplayMenu : public cSkinDisplayMenu, public cSkinEnigmaBaseOsd {
protected:
  int Tab(int n) { return (n >= 0 && n < MaxTabs) ? mytabs[n] : 0; }

private:
  int mytabs[MaxTabs];

  const cFont *pFontList;
  const cFont *pFontOsdTitle;
  const cFont *pFontHelpKeys;
  const cFont *pFontDate;
  const cFont *pFontDetailsTitle;
  const cFont *pFontDetailsSubtitle;
  const cFont *pFontDetailsDate;
  const cFont *pFontDetailsText;
  const cFont *pFontMessage;
  const cFont *pFontInfoWarnHeadline;
  const cFont *pFontInfoWarnText;
  const cFont *pFontInfoTimerHeadline;
  const cFont *pFontInfoTimerText;
  const cFont *pFontFixed;

#if VDRVERSNUM >= 10728
  int lastDiskUsageState;
#endif
  char *strTitle;
  char *strLastDate;
  char *strTheme;
  char *strLastText;
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

  int lineHeight, nMarkerGap, xIndent;
  int xItemLeft, xItemRight, yItemTop;

  int nMessagesShown;
  int nNumImageColors;
  int nBPP;

  int nOldIndex;
  int idListItem[MaxTabs];
  int idTitle;
  bool fLocked;
  bool fLockNeeded;
  bool fScrollTitle;
  bool fScrollInfo;
  bool fScrollListItem;
  bool fScrollOther;
  bool fScrollbarShown;

  void DrawTitle(void);
  void DrawScrollbar(int Total, int Offset, int Shown, int Top, int Left, int Height, bool CanScrollUp, bool CanScrollDown);
  void SetTextScrollbar(void);
  void SetupAreas(void);
  void SetFonts(void);
  void SetColors(void);
  int DrawFlag(int x, int y, const tComponent *p);
  const char *GetPluginMainMenuName(const char *plugin);
  int ReadSizeVdr(const char *strPath);
  bool HasTabbedText(const char *s, int Tab);
  int getDateWidth(void);

public:
  cSkinEnigmaDisplayMenu();
  virtual ~cSkinEnigmaDisplayMenu();
  virtual void SetTabs(int Tab1, int Tab2 = 0, int Tab3 = 0, int Tab4 = 0, int Tab5 = 0);
  virtual void Scroll(bool Up, bool Page);
  virtual int MaxItems(void);
  virtual void Clear(void);
  virtual void SetTitle(const char *Title);
  virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
  virtual void SetScrollbar(int Total, int Offset);
  virtual void SetEvent(const cEvent *Event);
  virtual void SetRecording(const cRecording *Recording);
  virtual void SetText(const char *Text, bool FixedFont);
  virtual int GetTextAreaWidth(void) const;
  virtual const cFont *GetTextAreaFont(bool FixedFont) const;
  virtual void Flush(void);
};

cSkinEnigmaDisplayMenu::cSkinEnigmaDisplayMenu(void)
{
#if VDRVERSNUM >= 10728
  lastDiskUsageState = -1;
#endif
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  xIndent = EnigmaConfig.drawRoundCorners ? Roundness : SmallGap;
  fSetupAreasDone = false;
  osd = NULL;
  SetFonts();
  
  strTitle = NULL;
  strLastDate = NULL;
  strLastText = NULL;
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
  nBPP = 1;
  for (int i = 0; i < MaxTabs; i++)
    idListItem[i] = -1;
  nOldIndex = -1;
  idTitle = -1;
  fScrollTitle = EnigmaConfig.useTextEffects && EnigmaConfig.scrollTitle;
  fScrollInfo = EnigmaConfig.useTextEffects && EnigmaConfig.scrollInfo;
  fScrollListItem = EnigmaConfig.useTextEffects && EnigmaConfig.scrollListItem;
  fScrollOther = EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther;
  fScrollbarShown = false;

  int LogoHeight = std::max(std::max(pFontOsdTitle->Height(), pFontDate->Height()) + TitleDeco + pFontDetailsTitle->Height() + Gap + pFontDetailsSubtitle->Height(),
                            std::max(3 * pFontDate->Height(), 
                                     (EnigmaConfig.showImages ? std::max(EnigmaConfig.imageHeight, IconHeight) : IconHeight)
                                    )
                           );
  int LogoWidth = EnigmaConfig.showImages ? std::max(IconWidth, EnigmaConfig.imageWidth) : IconWidth;
  int RightColWidth = 0;
  if (fShowLogoDefault) {
    int nMainDateWidth = getDateWidth() + SmallGap + LogoWidth;
    cString date = DayDateTime();
    int nSubDateWidth = pFontDate->Width(date);
    RightColWidth = SmallGap + Gap + std::max(nMainDateWidth, nSubDateWidth) + Gap;
  } else {
    cString date = DayDateTime();
    RightColWidth = SmallGap + Gap + std::max(MIN_DATEWIDTH + LogoWidth, pFontDate->Width(date)) + Gap;
  }
  if (EnigmaConfig.showInfo)
    RightColWidth = std::max(EnigmaConfig.minInfoWidth, RightColWidth);
  RightColWidth = RightColWidth & ~0x07; // must be multiple of 8

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
  if (EnigmaConfig.statusLineMode == 1) {
    //message on help butons
    yButtonsTop = OsdSize.h - std::max(SmallGap + pFontMessage->Height() + SmallGap, pFontHelpKeys->Height());
  } else {
    yButtonsTop = OsdSize.h - pFontHelpKeys->Height();
  }
  yButtonsBottom = OsdSize.h;
  // content area with items
  xBodyLeft = xTitleLeft;
  xBodyRight = xTitleRight;
  yBodyTop = yTitleDecoBottom + TitleDecoGap2;
  yBodyBottom = yButtonsTop - SmallGap;
  // message area
  xMessageLeft = xBodyLeft;
  xMessageRight = OsdSize.w;
  if (EnigmaConfig.statusLineMode == 1) {
    //on help buttons
    yMessageBottom = yButtonsBottom;
    yMessageTop = yButtonsTop;
  } else {
    yMessageBottom = yButtonsTop - SmallGap;
    yMessageTop = yMessageBottom - MessageHeight + SmallGap;      // No extra SmallGap at bottom as there's already the Gap above the buttons
  }
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

  tArea SingleArea[] = { {xTitleLeft,   yTitleTop, xMessageRight - 1, yButtonsBottom - 1, 8} };
  if (EnigmaConfig.singleArea8Bpp && osd->CanHandleAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayMenu: using %dbpp single area", SingleArea[0].bpp);
    osd->SetAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea));
    nNumImageColors = 230; //TODO: find correct number of colors
    if (SingleArea[0].bpp >=8 && Setup.AntiAlias)
      nBPP = 8;
  } else {
    if (fShowLogoDefault || EnigmaConfig.showColSymbolsDetails) {
      debug("cSkinEnigmaDisplayMenu: using 6 areas");
      tArea Areas[] = { {xTitleLeft,   yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2}, //title area
                        {xBodyLeft,    yBodyTop, xBodyRight - 1, yInfoTop + pFontDetailsDate->Height() - 1, 2}, //body area (beside date/logo/symbols area)
                        {xDateLeft,    yDateTop, xLogoRight - 1, yInfoTop - 1, 4}, //date/logo area
                        {xInfoLeft,    yInfoTop, xInfoRight - 1, yInfoTop + pFontDetailsDate->Height() - 1, 4}, //area for symbols in event/recording info
                        {xBodyLeft,    yInfoTop + pFontDetailsDate->Height(), xInfoRight - 1, (EnigmaConfig.statusLineMode == 1 ? yBodyBottom : yMessageTop) - 1, 2}, // body/info area (below symbols area)
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
    } else {
      debug("cSkinEnigmaDisplayMenu: using 4 areas");
      tArea Areas[] = { {xTitleLeft,   yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2}, //title area
                        {xBodyLeft,    yBodyTop, xInfoRight - 1, (EnigmaConfig.statusLineMode == 1 ? yBodyBottom : yMessageTop) - 1, 2}, // body/info area
                        {xDateLeft,    yDateTop, xLogoRight - 1, yTitleDecoBottom - 1, 4}, //date area
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
    }

    nNumImageColors = 13; // "16 available colors" - "clrTransparent" - "clrLogoBg" - "clrMenuTxtFg"
  }

  lineHeight = pFontList->Height();
  nMarkerGap = min(MarkerGap, lineHeight / 2 - 1); //lineHeight - 2 * MarkerGap
  xItemLeft = xBodyLeft + (EnigmaConfig.showMarker ? lineHeight : ListHBorder);
  xItemRight = (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - ListHBorder - SmallGap - SmallGap - SmallGap;
  int numItems = MaxItems();
  yItemTop = yBodyTop + ((EnigmaConfig.statusLineMode == 2 ? yMessageTop : yBodyBottom) - yBodyTop - numItems * lineHeight) / 2;

  fLockNeeded = (fScrollTitle || fScrollInfo || fScrollListItem || fScrollOther);
  fLocked = false;
  TE_START(osd);
  if (fLockNeeded) {
    fLocked = true;
  }
}

void cSkinEnigmaDisplayMenu::SetFonts(void)
{
  debug("cSkinEnigmaDisplayMenu::SetFonts()");

  pFontList = EnigmaConfig.GetFont(FONT_LISTITEM);
  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontHelpKeys = EnigmaConfig.GetFont(FONT_HELPKEYS);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontDetailsTitle = EnigmaConfig.GetFont(FONT_DETAILSTITLE);
  pFontDetailsSubtitle = EnigmaConfig.GetFont(FONT_DETAILSSUBTITLE);
  pFontDetailsDate = EnigmaConfig.GetFont(FONT_DETAILSDATE);
  pFontDetailsText = EnigmaConfig.GetFont(FONT_DETAILSTEXT);
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);
  pFontInfoWarnHeadline = EnigmaConfig.GetFont(FONT_INFOWARNHEADLINE);
  pFontInfoWarnText = EnigmaConfig.GetFont(FONT_INFOWARNTEXT);
  pFontInfoTimerHeadline = EnigmaConfig.GetFont(FONT_INFOTIMERHEADLINE);
  pFontInfoTimerText = EnigmaConfig.GetFont(FONT_INFOTIMERTEXT);
  pFontFixed = EnigmaConfig.GetFont(FONT_FIXED);
}

void cSkinEnigmaDisplayMenu::SetColors(void)
{
  debug("cSkinEnigmaDisplayMenu::SetColors()");

  if (osd->GetBitmap(1) == NULL) { //single area
    return;
  }

  cBitmap *bitmap = osd->GetBitmap(0);
  if (bitmap) { //title area
    bitmap->Reset();
    bitmap->SetColor(0, clrTransparent);
    bitmap->SetColor(1, Theme.Color(clrTitleBg));
    bitmap->SetColor(2, Theme.Color(clrTitleFg));
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

  if (osd->GetBitmap(4)) { // multiple body areas
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
  } else { // single body area
    bitmap = osd->GetBitmap(2);
  }

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

  EnigmaTextEffects.Clear();
  idTitle = -1;
  nOldIndex = -1;
  for (int i = 0; i < MaxTabs; i++)
    idListItem[i] = -1;

  xItemRight = (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - ListHBorder - SmallGap - SmallGap - SmallGap;

  // clear transparent areas (without date/logo borders)
  osd->DrawRectangle(xBodyLeft, yTitleDecoBottom, xDateLeft - 1, yBodyTop - 1, clrTransparent);
  osd->DrawRectangle(xBodyLeft, yMessageBottom, xInfoRight - 1, yButtonsTop - 1, clrTransparent);

  // draw titlebar
  osd->DrawRectangle(xTitleLeft, yTitleTop, xTitleRight - 1, yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawRectangle(xTitleLeft, yTitleBottom, xTitleRight - 1, yTitleDecoTop - 1, clrTransparent);
  osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1, yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
  if (EnigmaConfig.drawRoundCorners) {
    // draw rounded left corner of title bar
    osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1, yTitleTop + Roundness - 1, clrTransparent, -2);
  }
  idTitle = TE_MARQUEE(osd, idTitle, fScrollTitle, xTitleLeft + xIndent, yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2, strTitle, Theme.Color(clrTitleFg), Theme.Color(clrTitleBg), pFontOsdTitle, nBPP, xTitleRight - xTitleLeft - xIndent - 1);

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

#ifdef USE_PLUGIN_EPGSEARCH
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
            int yWarning = yMaxHeight -
                           pFontInfoWarnHeadline->Height() -
                           2 * pFontInfoWarnText->Height(); // Needed space for warning
            yMaxHeight = yWarning;
            osd->DrawRectangle(xInfoLeft, yWarning, xInfoRight - 1, yWarning + SmallGap + 1, Theme.Color(clrBackground));
            yWarning += pFontInfoWarnText->Height() / 2;
            TE_BLINK(osd, -1, true, xInfoLeft, yWarning, tr("WARNING"),
                     Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                     pFontInfoWarnHeadline, nBPP,
                     w, pFontInfoWarnHeadline->Height(), taCenter);
            yWarning += (int)(1.5 * pFontInfoWarnHeadline->Height());

            char *info = NULL;
            if (-1 != asprintf(&info, "%d %s", serviceData->relevantConflicts, (serviceData->relevantConflicts == 1 ? tr("Timer conflict") : tr("Timer conflicts")))) {
              osd->DrawText(x, yWarning, info, Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                            pFontInfoWarnText,
                            w, pFontInfoWarnText->Height(), taCenter);
              yWarning += pFontInfoWarnText->Height();
              free(info);
            }
          }
          delete serviceData;
        }
      }
    }
#endif //USE_PLUGIN_EPGSEARCH

    if (Timers.GetNextActiveTimer()) {
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

        TE_MARQUEE(osd, -1, fScrollInfo, x + (timer->isRecording ? (bmRecording.Width() + Gap) : 0),
                   y, timer->title.c_str(),
                   Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground),
                   pFontInfoTimerText, nBPP, w - (timer->isRecording ? (bmRecording.Width() + Gap) : 0), h);
        y += h;
        char* info = NULL;
        int rc = -1;
        if (timer->isRecording) {
          rc = asprintf(&info, "- %s / %s", *TimeString(timer->stopTime), timer->channelName.c_str());
        } else {
          rc = asprintf(&info, "%d. %s / %s", timer->startDay, *TimeString(timer->startTime), timer->channelName.c_str());
        }
        if (-1 != rc) {
          osd->DrawText(x, y, info, Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrAltBackground), pFontInfoTimerText, w, h);
          free(info);
          y += h;
          osd->DrawRectangle(x, y + SmallGap, xInfoRight - Gap - 1, y + SmallGap + 1, Theme.Color(clrMenuItemSelectableFg));
          y += SmallGap + 1 + h / 2;
        }
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
  if (fLockNeeded && !fLocked) TE_LOCK;
  TE_STOP;

  free(strTheme);
  free(strTitle);
  free(strLastDate);
  free(strLastText);
  delete osd;
}

void cSkinEnigmaDisplayMenu::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5)
{
  int w = pFontList->Width('0');
  mytabs[0] = 0;
  mytabs[1] = Tab1 ? mytabs[0] + Tab1 : 0;
  mytabs[2] = Tab2 ? mytabs[1] + Tab2 : 0;
  mytabs[3] = Tab3 ? mytabs[2] + Tab3 : 0;
  mytabs[4] = Tab4 ? mytabs[3] + Tab4 : 0;
  mytabs[5] = Tab5 ? mytabs[4] + Tab5 : 0;
  if (Tab2) {
    for (int i = 1; i < MaxTabs; i++)
      mytabs[i] *= w;//XXX average character width of font used for items - see also skincurses.c!!!
  } else if (Tab1) {
    int temp1 = mytabs[0] + (int)(0.7 * ((fShowInfo ? xBodyRight : xInfoRight)  - xBodyLeft));
    int temp2 = mytabs[1] * w;
    mytabs[1] = std::min(temp1, temp2);
  }
}

void cSkinEnigmaDisplayMenu::SetTextScrollbar(void)
{
  //Must be TE_LOCKed by caller

  // check if scrollbar is needed
  if (textScroller.CanScrollUp() || textScroller.CanScrollDown())
    DrawScrollbar(textScroller.Total(), textScroller.Offset(), textScroller.Shown(), textScroller.Top() - SmallGap - 1, textScroller.Width(), textScroller.Height() + SmallGap + SmallGap + 1, textScroller.CanScrollUp(), textScroller.CanScrollDown());
}

void cSkinEnigmaDisplayMenu::SetScrollbar(int Total, int Offset)
{
  debug("cSkinEnigmaDisplayMenu::SetScrollbar(%d, %d)", Total, Offset);

  if (EnigmaConfig.showScrollbar == 0 || (EnigmaConfig.showScrollbar == 2 && Total <= MaxItems())) {
    fScrollbarShown = false;
    return;
  }

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  fScrollbarShown = true;
  DrawScrollbar(Total, Offset, MaxItems(), yItemTop - SmallGap - 1, xItemRight - ScrollbarWidth + SmallGap, MaxItems() * lineHeight + SmallGap + SmallGap + 1, Offset > 0, Offset + MaxItems() < Total);

  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayMenu::DrawScrollbar(int Total, int Offset, int Shown, int Top, int Left, int Height, bool CanScrollUp, bool CanScrollDown)
{
  //Must be TE_LOCKed by caller

  if (Total <= 0 || Total <= Shown) {
    Total = Shown = 1;
  }

  int yt = Top;
  int yb = yt + Height;
  int st = yt + ScrollbarHeight + Gap;
  int sb = yb - ScrollbarHeight - Gap;
  int tt = st + (sb - st) * Offset / Total;
  int tb = tt + (sb - st) * Shown / Total;
  int xl = Left + SmallGap;
  // arrow up
  osd->DrawRectangle(xl, yt, xl + ScrollbarWidth, yt + SmallGap,
                     CanScrollUp ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));    
  osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, yt + SmallGap, xl + ScrollbarWidth, yt + ScrollbarHeight,
                     CanScrollUp ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));
  // draw background of scrollbar
  osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, st, xl + ScrollbarWidth, sb, Theme.Color(clrAltBackground));
  // draw visible area of scrollbar
  osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, tt, xl + ScrollbarWidth, tb, Theme.Color(clrMenuTxtFg));
  // arrow down
  osd->DrawRectangle(xl + ScrollbarWidth - SmallGap, yb - ScrollbarHeight, xl + ScrollbarWidth, yb - SmallGap,
                     CanScrollDown ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));
  osd->DrawRectangle(xl, yb - SmallGap, xl + ScrollbarWidth, yb,
                     CanScrollDown ? Theme.Color(clrMenuTxtFg) : Theme.Color(clrAltBackground));
}

void cSkinEnigmaDisplayMenu::Scroll(bool Up, bool Page)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  cSkinDisplayMenu::Scroll(Up, Page);
  SetTextScrollbar();
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

int cSkinEnigmaDisplayMenu::MaxItems(void)
{
  // max number of items
  return ((EnigmaConfig.statusLineMode == 2 ? yMessageTop : yBodyBottom) - yBodyTop) / lineHeight;
}

void cSkinEnigmaDisplayMenu::Clear(void)
{
  debug("cSkinEnigmaDisplayMenu::Clear() %d %d %d", isMainMenu, fShowLogo, fShowInfo);

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  nOldIndex = -1;
  for (int i = MaxTabs - 1; i >= 0; i--) {
    if (idListItem[i] >= 0) {
      EnigmaTextEffects.ResetText(idListItem[i], Theme.Color(clrMenuItemSelectableFg), Theme.Color(clrBackground), !fLocked);
      idListItem[i] = -1;
    }
  }
  free(strLastText);
  strLastText = NULL;

  textScroller.Reset();

  //TODO? SetFonts();

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
  if (fLockNeeded && !fLocked) TE_UNLOCK;

}

void cSkinEnigmaDisplayMenu::DrawTitle(void)
{
#if VDRVERSNUM >= 10728
  bool WithDisk = MenuCategory() == mcMain || MenuCategory() == mcRecording;
  idTitle = TE_MARQUEE(osd, idTitle, fScrollTitle,
                       xTitleLeft + xIndent, yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2,
                       WithDisk ? (const char*)cString::sprintf("%s  -  %s", strTitle, *cVideoDiskUsage::String()) : strTitle,
                       Theme.Color(clrTitleFg), Theme.Color(clrTitleBg),
                       pFontOsdTitle, nBPP, xTitleRight - xTitleLeft - xIndent - 1);
#else
  idTitle = TE_MARQUEE(osd, idTitle, fScrollTitle,
                       xTitleLeft + xIndent, yTitleTop + (yTitleBottom - yTitleTop - pFontOsdTitle->Height()) / 2,
                       strTitle,
                       Theme.Color(clrTitleFg), Theme.Color(clrTitleBg),
                       pFontOsdTitle, nBPP, xTitleRight - xTitleLeft - xIndent - 1);
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

#if VDRVERSNUM >= 10728
#define IS_MAINMENU (MenuCategory() == mcMain)
#else
  char *strTitlePrefix = NULL;
  if (-1 == asprintf(&strTitlePrefix, "%s  -  ", trVDR("VDR")))
    return;
#define IS_MAINMENU (strncmp(strTitlePrefix, Title, strlen(strTitlePrefix)) == 0)
#endif

  if ((Title == NULL) || (isMainMenu && IS_MAINMENU)) {
    if (fLockNeeded && !fLocked) {
      fLocked = true;
      TE_LOCK;
    }
    DrawTitle();
    if (fLockNeeded && !fLocked) TE_UNLOCK;
  } else {
    bool old_isMainMenu = isMainMenu;
    bool old_fShowLogo = fShowLogo;
    bool old_fShowInfo = fShowInfo;

    if (strTitle == NULL || IS_MAINMENU) {
      isMainMenu = true;
      fShowLogo = fShowLogoDefault ? EnigmaConfig.showSymbolsMenu : false;
      fShowInfo = EnigmaConfig.showInfo;
    } else {
      isMainMenu = false;
      fShowLogo = false;
      fShowInfo = false;
    }

    if (fTitleChanged) {
      if (fLockNeeded && !fLocked) {
        fLocked = true;
        TE_LOCK;
      }
      EnigmaTextEffects.Clear();
      idTitle = -1;
      nOldIndex = -1;
      for (int i = 0; i < MaxTabs; i++)
        idListItem[i] = -1;
      if (fLockNeeded && !fLocked) TE_UNLOCK;
    }

    if (!fSetupAreasDone
        || old_isMainMenu != isMainMenu
        || old_fShowLogo != fShowLogo
        || old_fShowInfo != fShowInfo) {

      if (fLockNeeded && !fLocked) {
        fLocked = true;
        TE_LOCK;
      }
      SetupAreas();
      if (fLockNeeded && !fLocked) TE_UNLOCK;
    } else {
      if (fLockNeeded && !fLocked) {
        fLocked = true;
        TE_LOCK;
      }
      DrawTitle();
      if (fLockNeeded && !fLocked) TE_UNLOCK;
    }
  }
#if VDRVERSNUM < 10728
  free (strTitlePrefix);
#endif

  free(strLastDate);
  strLastDate = NULL;
}

void cSkinEnigmaDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
  debug("cSkinEnigmaDisplayMenu::SetButtons(%s, %s, %s, %s)", Red, Green, Yellow, Blue);

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
#if VDRVERSNUM < 10730
  int n0 = 0;
  int n1 = 1;
  int n2= 2;
  int n3 = 3;
#else
  int n0 = Setup.ColorKey0;
  int n1 = Setup.ColorKey1;
  int n2 = Setup.ColorKey2;
  int n3 = Setup.ColorKey3;
#endif
  const char *lutText[] = { Red, Green, Yellow, Blue };
  tColor lutFg[] = { clrButtonRedFg, clrButtonGreenFg, clrButtonYellowFg, clrButtonBlueFg };
  tColor lutBg[] = { clrButtonRedBg, clrButtonGreenBg, clrButtonYellowBg, clrButtonBlueBg };
  int w = (xButtonsRight - xButtonsLeft) / 4;
  int t3 = xButtonsLeft + xButtonsRight - xButtonsLeft - w;
  int t2 = t3 - w;
  int t1 = t2 - w;
  int t0 = xButtonsLeft;

  // draw color buttons
  if (lutText[n0])
    osd->DrawText(t0, yButtonsTop, lutText[n0], Theme.Color(lutFg[n0]),
                  Theme.Color(lutBg[n0]), pFontHelpKeys, t1 - t0 + 1,
                  yButtonsBottom - yButtonsTop, taCenter);
  else {
    osd->DrawRectangle(t0, yButtonsTop, lutText[n1] ? t1 - 1 : t1, yButtonsBottom - 1, Theme.Color(clrBottomBg));
    if (EnigmaConfig.drawRoundCorners) {
      osd->DrawEllipse(t0, yButtonsBottom - Roundness, t0 + Roundness - 1, yButtonsBottom - 1, clrTransparent, -3);
    }
  }
  if (lutText[n1])
    osd->DrawText(t1, yButtonsTop, lutText[n1], Theme.Color(lutFg[n1]),
                  Theme.Color(lutBg[n1]), pFontHelpKeys, w,
                  yButtonsBottom - yButtonsTop, taCenter);
  else
    osd->DrawRectangle(t1, yButtonsTop, lutText[n2] ? t1 + w : t2 - 1, yButtonsBottom - 1, Theme.Color(clrBottomBg));

  if (lutText[n2])
    osd->DrawText(t2, yButtonsTop, lutText[n2], Theme.Color(lutFg[n2]),
                  Theme.Color(lutBg[n2]), pFontHelpKeys, w,
                  yButtonsBottom - yButtonsTop, taCenter);
  else
    osd->DrawRectangle(t2, yButtonsTop, lutText[n3] ? t2 + w : t3 - 1, yButtonsBottom - 1, Theme.Color(clrBottomBg));

  if (lutText[n3])
    osd->DrawText(t3, yButtonsTop, lutText[n3], Theme.Color(lutFg[n3]),
                  Theme.Color(lutBg[n3]), pFontHelpKeys, w,
                  yButtonsBottom - yButtonsTop, taCenter);
  else {
    osd->DrawRectangle(t3, yButtonsTop, xButtonsRight, yButtonsBottom, Theme.Color(clrBottomBg));
    if (EnigmaConfig.drawRoundCorners) {
      osd->DrawEllipse(xButtonsRight - Roundness, yButtonsBottom - Roundness,
                       xButtonsRight - 1, yButtonsBottom - 1, clrTransparent, -4);
    }
  }
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayMenu::SetMessage(eMessageType Type, const char *Text)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  // check if message
  if (Text) {
    EnigmaTextEffects.PauseEffects(yMessageTop);
    // save osd region
    if (nMessagesShown == 0)
      osd->SaveRegion(xMessageLeft, yMessageTop, xMessageRight - 1, yMessageBottom - 1);

    // draw message
    if (EnigmaConfig.statusLineMode == 1) {
      //on help buttons
      osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1,
                         yMessageBottom - 1, Theme.Color(clrButtonRedBg));
      osd->DrawText(xMessageLeft, yMessageTop + SmallGap, Text,
                    Theme.Color(clrMessageStatusFg + 2 * Type),
                    Theme.Color(clrMessageStatusBg + 2 * Type),
                    pFontMessage,
                    xMessageRight - xMessageLeft,
                    yMessageBottom - SmallGap - yMessageTop - SmallGap,
                    taCenter);
    } else {
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
    }
    nMessagesShown++;
  } else {
    if (nMessagesShown > 0)
      nMessagesShown--;
    // restore saved osd region
    if (nMessagesShown == 0)
      osd->RestoreRegion();
    EnigmaTextEffects.PauseEffects();
  }
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

bool cSkinEnigmaDisplayMenu::HasTabbedText(const char *s, int NumOfTabs)
{
  if (!s)
    return false;

  const char *b = strchrnul(s, '\t');
  while (*b && NumOfTabs-- > 0) {
    b = strchrnul(b + 1, '\t');
  }
  if (!*b)
    return (NumOfTabs <= 0) ? true : false;
  return true;
}

void cSkinEnigmaDisplayMenu::SetItem(const char *Text, int Index, bool isCurrent, bool Selectable)
{
  debug("cSkinEnigmaDisplayMenu::SetItem(%s, %d, %d, %d)", Text, Index, isCurrent, Selectable);

  int y = yItemTop + Index * lineHeight;
  if (nMessagesShown > 0 && y >= yMessageTop)
    return; //Don't draw above messages

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  tColor ColorFg, ColorBg;
  // select colors
  if (isCurrent) {
    if ((Index == nOldIndex) && strLastText && Text && !strcmp(strLastText, Text) ) {
      return;
    }

    free(strLastText);
    strLastText = strdup(Text);
    nOldIndex = Index;

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

    if (nOldIndex == Index) {
      for (int i = MaxTabs - 1; i >= 0; i--) {
        if (idListItem[i] >= 0) {
          EnigmaTextEffects.ResetText(idListItem[i], ColorFg, ColorBg);
          idListItem[i] = -1;
        }
      }
      free(strLastText);
      strLastText = NULL;
      nOldIndex = -1;
    }
  }

  if (!Selectable && EnigmaConfig.showListSymbols && (strncmp(Text, "---", 3) == 0)) {
    // headline?
    const char *c = Text + 3;
    while (*c == '-')
      c++;
    if (*c == ' ' || *c == '\t') {
      // it's a headline
      while (*c == ' ' || *c == '\t') // find start of headline text
        c++;

      // find end of headline text
      char *c2 = (char*)(c + strlen(c) - 1);
      while (*c2 == '-')
        c2--;
      while (*c2 == ' ' || *c2 == '\t')
        c2--;
      *(c2 + 1) = 0;

      int xt = xItemLeft + Tab(0);
      int w = Gap + pFontList->Width(c) + Gap;
      int x = std::max(0, (xItemRight - xt - w) / 2);
      int y2 = y + lineHeight;
      if (nMessagesShown && yMessageTop > y2)
        y2 = yMessageTop;
      int yBar = y + (y2 - y - SmallGap) / 2;
      osd->DrawRectangle(xt, yBar, x - 1, yBar + SmallGap - 1, ColorFg);
      osd->DrawText(x, y, c, ColorFg, ColorBg, pFontList, w, y2 - y, taCenter);
      osd->DrawRectangle(x + w, yBar, xItemRight - 1, yBar + SmallGap - 1, ColorFg);

      return; // headline displayed -> done
    }
  }

  // this should prevent menu flickering
  osd->DrawPixel(xItemLeft, y, ColorBg);
  osd->DrawPixel(xItemRight - 2, y, ColorBg);
  osd->DrawPixel(xItemLeft, y, ColorFg);
  osd->DrawPixel(xItemRight - 2, y, ColorFg);

  osd->DrawRectangle(xBodyLeft, y, xItemLeft - 1, y + lineHeight - 1, ColorBg);
  if (EnigmaConfig.showMarker) {
    osd->DrawEllipse(xBodyLeft + nMarkerGap, y + nMarkerGap, xBodyLeft + lineHeight - nMarkerGap, y + lineHeight - nMarkerGap, isCurrent ? ColorFg : ColorBg);
  }

  if (EnigmaConfig.showScrollbar == 0 || (!fScrollbarShown && EnigmaConfig.showScrollbar == 2))
    osd->DrawRectangle(xItemRight, y, (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - 1, y + lineHeight - 1, ColorBg);

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
      if (EnigmaConfig.showListSymbols && Selectable) {
        // check if event info characters
        if (strlen(s) == 3 && strchr(" tTR", s[0])
            && strchr(" V", s[1]) && strchr(" *", s[2])) {
          // update status
          iseventinfo = true;
        }

        // check if new recording: "01.01.06*", "10:10*"
        if ((!iseventinfo &&
            (strlen(s) == 6 && s[5] == '*' && s[2] == ':' && isdigit(*s)
             && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4))))
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
        // draw text
        if (isCurrent) {
          idListItem[i] = TE_MARQUEE(osd, idListItem[i], fScrollListItem, xt, y, s, ColorFg, ColorBg, pFontList, nBPP, w, nMessagesShown ? std::min(yMessageTop - y, lineHeight) : 0 );
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
  if (isCurrent && isMainMenu && fShowLogo && Text) {
    char *ItemText, *ItemText2;
    int n = strtoul(Text, &ItemText, 10);
    if (n != 0)
      ItemText2 = ItemText = skipspace(ItemText);
    else
      ItemText2 = skipspace(ItemText);

    bool fFoundLogo = false;
    if (strcmp(ItemText, trVDR("Schedule")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/schedule");
    else if (strcmp(ItemText, trVDR("Channels")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/channels");
    else if (strcmp(ItemText, trVDR("Timers")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("timerinfo")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/timers");
    else if (strcmp(ItemText, trVDR("Recordings")) == 0
             || strcmp(ItemText, trVDR("Recording info")) == 0
             || strcmp(ItemText2, GetPluginMainMenuName("extrecmenu")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/recordings");
    else if (strcmp(ItemText, trVDR("Setup")) == 0
             || strcmp(ItemText2, trVDR("Setup")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/setup");
    else if (strcmp(ItemText, trVDR("Commands")) == 0
             || strcmp(ItemText2, trVDR("Commands")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/commands");
    else if (strcmp(ItemText, trVDR(" Stop replaying")) == 0)
      fFoundLogo = EnigmaLogoCache.LoadIcon("icons/menu/stop");
    else if (strcmp(ItemText, trVDR(" Cancel editing")) == 0)
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

  if (fLockNeeded && !fLocked) TE_UNLOCK;
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

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }

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
    osd->DrawBitmap(xs, y, bmVPS, EnigmaConfig.showColSymbolsDetails ? Theme.Color(clrSymbolVpsActive) : Theme.Color(clrMenuTxtFg), EnigmaConfig.showColSymbolsDetails ? Theme.Color(clrSymbolInactive) : Theme.Color(clrBackground));
    xs -= Gap;
  }
  // check if event is running
  if (Event->IsRunning()) {
    // draw running symbol
    xs -= (bmRun.Width() + Gap);
    osd->DrawBitmap(xs, y, bmRun, EnigmaConfig.showColSymbolsDetails ? Theme.Color(clrSymbolRunActive) : Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
    xs -= Gap;
  }
  // check if event has timer
  if (Event->HasTimer()) {
    if (Event->IsRunning()) {
      // draw recording symbol
      xs -= bmRecording.Width();
      osd->DrawBitmap(xs, y, bmRecording, EnigmaConfig.showColSymbolsDetails ? Theme.Color(clrSymbolRecActive) : Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
    } else {
      // draw timer symbol
      xs -= bmTimer.Width();
      osd->DrawBitmap(xs, y, bmTimer, EnigmaConfig.showColSymbolsDetails ? Theme.Color(clrSymbolTimerActive) : Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
    }
    xs -= Gap;
  }
  std::stringstream sstrInfo;
#if VDRVERSNUM >= 10711
  bool fFirst = true;
  for (int i = 0; Event->Contents(i); i++) {
    const char *s = Event->ContentToString(Event->Contents(i));
    if (!isempty(s)) {
      if (fFirst) {
        fFirst = false;
        sstrInfo << tr("Content: ");
      } else {
        sstrInfo << ", ";
      }
      sstrInfo << s;
    }
  }
  if (!fFirst)
    sstrInfo << std::endl;

  if (Event->ParentalRating()) {
    sstrInfo << *Event->GetParentalRatingString() << std::endl;
  }
#endif

  const cComponents *Components = Event->Components();
  if (Components) {
    //TODO: show flaggs?
    std::stringstream infoVideo, infoAudio, infoSubtitle;
    for (int i = 0; i < Components->NumComponents(); i++) {
      const tComponent *p = Components->Component(i);
//      sstrInfo << "* "
//               << (int)p->stream << " " << (int)p->type << " (" << (p->language ? p->language : "n/a") << ") (" << (p->description ? p->description : "n/a") << ")\n";

      if (p->stream == 1) { // video
        if (p->description) {
          infoVideo << p->description
                    << " (" << p->language << "), ";
        } else {
          infoVideo << p->language << ", ";
        }
      } else if ((p->stream == 2) && p->language) { // audio
        if (p->description) {
          infoAudio << p->description
                    << " (" << p->language << "), ";
        } else {
          infoAudio << p->language << ", ";
        }
      } else if ((p->stream == 3) && p->language) { // subtitle
        if (p->description) {
          infoSubtitle << p->description
                       << " (" << p->language << "), ";
//        } else {  //TODO: no description = no subtitle?
//          infoSubtitle << p->language << ", ";
        }
      }
    }
    if (!infoVideo.str().empty()) {
      sstrInfo << std::endl << tr("Video") << ": "
               << infoVideo.str().substr(0, infoVideo.str().length() - 2); // strip out the last delimiter
    }
    if (!infoAudio.str().empty()) {
      sstrInfo << std::endl << tr("Audio") << ": "
               << infoAudio.str().substr(0, infoAudio.str().length() - 2); // strip out the last delimiter
    }
    if (!infoSubtitle.str().empty()) {
      sstrInfo << std::endl << tr("Subtitles") << ": "
               << infoSubtitle.str().substr(0, infoSubtitle.str().length() - 2); // strip out the last delimiter
    }
  }

  std::string stringInfo = sstrInfo.str();
  int yHeadlineBottom = yDateBottom;
  int xHeadlineRight  = fShowLogo ? xDateLeft : xInfoRight;
  osd->DrawRectangle(xBodyLeft, yBodyTop, xHeadlineRight - 1,
                     yHeadlineBottom - 1, Theme.Color(clrAltBackground));
  int th = pFontDetailsTitle->Height() + (!isempty(Event->Description()) && !isempty(Event->ShortText()) ? Gap + pFontDetailsSubtitle->Height() : 0);
  y = yBodyTop + (yHeadlineBottom - yBodyTop - th) / 2;

  // draw recording title
  TE_MARQUEE(osd, -1, fScrollOther, xBodyLeft + Gap, y, Event->Title(),
             Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
             pFontDetailsTitle, nBPP, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsTitle->Height());

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
      TE_MARQUEE(osd, -1, fScrollOther, xBodyLeft + Gap, y, Event->ShortText(),
                 Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrAltBackground),
                 pFontDetailsSubtitle, nBPP, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsSubtitle->Height());
    }
    // draw description
    strDescr = Event->Description();
  }

  std::string stringReruns;
#ifdef USE_PLUGIN_EPGSEARCH
  cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
  // try to find a rerun of the show using epgsearch-service
  if (epgSearchPlugin && EnigmaConfig.numReruns > 0 && !isempty(Event->Title())) {
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
    if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data)) {
      cList<Epgsearch_searchresults_v1_0::cServiceSearchResult>* list = data.pResultList;
      if (list && (list->Count() > 1)) {
        //TODO: current event is shown as rerun 
        sstrReruns << tr("RERUNS OF THIS SHOW") << ':' << std::endl;
        int i = 0;
        for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = list->First(); r && i < EnigmaConfig.numReruns; r = list->Next(r)) {
          i++;
          sstrReruns << "- "
                     << *DayDateTime(r->event->StartTime());
          cChannel *channel = Channels.GetByChannelID(r->event->ChannelID(), true, true);
          if (channel)
            sstrReruns << " " << channel->ShortName(true);
          sstrReruns << ":  " << r->event->Title();
          if (!isempty(r->event->ShortText()))
            sstrReruns << "~" << r->event->ShortText();
          sstrReruns << std::endl;
        }
        delete list;
      }
    }
    stringReruns = sstrReruns.str();
  }
#endif // USE_PLUGIN_EPGSEARCH

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
  if (strFirst || strSecond || strThird) {
    y = yHeadlineBottom + SmallGap + 2 * pFontDetailsDate->Height();
    char *mytext = NULL;
    if (-1 != asprintf(&mytext, "%s%s%s%s%s", strFirst ? strFirst : "",
                                    strSecond ? "\n\n" : "", strSecond ? strSecond : "",
                                    (strFirst || strSecond) && strThird ? "\n\n" : "", strThird ? strThird : "")) {
      textScroller.Set(osd, xBodyLeft + Gap, y + Gap,
                       xInfoRight - ListHBorder - SmallGap - SmallGap - SmallGap - ScrollbarWidth + SmallGap - xBodyLeft,
                       yBodyBottom - Gap - y - Gap,
                       mytext, pFontDetailsText,
                       Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
      SetTextScrollbar();
      free(mytext);
    }
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
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

int cSkinEnigmaDisplayMenu::ReadSizeVdr(const char *strPath)
{
  int dirSize = -1;
  char buffer[20];
  char *strFilename = NULL;
  if (-1 != asprintf(&strFilename, "%s/size.vdr", strPath)) {
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

  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }

  isMainMenu = false;
  fShowInfo = false;
  fShowLogo = fShowLogoDefault ? EnigmaConfig.showImages : false;;
  SetupAreas();

  // draw recording date string
  std::stringstream sstrDate;
#if VDRVERSNUM >= 10721
  sstrDate << *DateString(Recording->Start())
           << "  " << *TimeString(Recording->Start());
#else
  sstrDate << *DateString(Recording->start)
           << "  " << *TimeString(Recording->start);
#endif

  unsigned long long nRecSize = -1;
  unsigned long long nFileSize[1000];
  nFileSize[0] = 0;
  if (EnigmaConfig.showRecSize == 2) {
    int i = 0;
    struct stat filebuf;
    cString filename;
    int rc = 0;
    do {
#if VDRVERSNUM >= 10703
      if (Recording->IsPesRecording())
        filename = cString::sprintf("%s/%03d.vdr", Recording->FileName(), ++i);
      else
        filename = cString::sprintf("%s/%05d.ts", Recording->FileName(), ++i);
#else
      filename = cString::sprintf("%s/%03d.vdr", Recording->FileName(), ++i);
#endif
      rc = stat(filename, &filebuf);
      if (rc == 0)
        nFileSize[i] = nFileSize[i-1] + filebuf.st_size;
      else
        if (ENOENT != errno) {
          error("error determining file size of \"%s\" %d (%s)", (const char *)filename, errno, strerror(errno));
          nRecSize = -1;
        }
    } while (i <= 999 && !rc);
    nRecSize = nFileSize[i-1];
  }

  cMarks marks;
#if VDRVERSNUM >= 10703
  bool fHasMarks = marks.Load(Recording->FileName(), Recording->FramesPerSecond(), Recording->IsPesRecording()) && marks.Count();
  cIndexFile *index = new cIndexFile(Recording->FileName(), false, Recording->IsPesRecording());
#else
  bool fHasMarks = marks.Load(Recording->FileName()) && marks.Count();
  cIndexFile *index = new cIndexFile(Recording->FileName(), false);
#endif

  int nCutLength = 0;
  long nCutInFrame = 0;
  unsigned long long nRecSizeCut = nRecSize < 0 ? -1 : 0;
  unsigned long long nCutInOffset = 0;
  if (fHasMarks && index) {
#if VDRVERSNUM >= 10703
    uint16_t FileNumber;
    off_t FileOffset;
#else
    uchar FileNumber;
    int FileOffset;
#endif

    bool fCutIn = true;
    cMark *mark = marks.First();
    while (mark) {
#if VDRVERSNUM >= 10721
      int pos = mark->Position();
#else
      int pos = mark->position;
#endif
      index->Get(pos, &FileNumber, &FileOffset); //TODO: will disc spin up?
      if (fCutIn) {
        nCutInFrame = pos;
        fCutIn = false;
        if (nRecSize >= 0)
          nCutInOffset = nFileSize[FileNumber-1] + FileOffset;
      } else {
        nCutLength += pos - nCutInFrame;
        fCutIn = true;
        if (nRecSize >= 0)
          nRecSizeCut += nFileSize[FileNumber-1] + FileOffset - nCutInOffset;
      }
      cMark *nextmark = marks.Next(mark);
      mark = nextmark;
    }

    if (!fCutIn) {
      nCutLength += index->Last() - nCutInFrame;
      index->Get(index->Last() - 1, &FileNumber, &FileOffset);
      if (nRecSize >= 0)
        nRecSizeCut += nFileSize[FileNumber-1] + FileOffset - nCutInOffset;
    }
  }

  // draw additional information
  std::stringstream sstrInfo;
#if VDRVERSNUM >= 10711
  bool fFirst = true;
  for (int i = 0; Info->GetEvent()->Contents(i); i++) {
    const char *s = Info->GetEvent()->ContentToString(Info->GetEvent()->Contents(i));
    if (!isempty(s)) {
      if (fFirst) {
        fFirst = false;
        sstrInfo << tr("Content: ");
      } else {
        sstrInfo << ", ";
      }
      sstrInfo << s;
    }
  }
  if (!fFirst)
    sstrInfo << std::endl;

  if (Info->GetEvent()->ParentalRating()) {
    sstrInfo << *Info->GetEvent()->GetParentalRatingString() << std::endl;
  }
#endif

  cChannel *channel = Channels.GetByChannelID(Info->ChannelID());
  if (channel)
    sstrInfo << trVDR("Channel") << ": " << channel->Number() << " - " << channel->Name() << std::endl;
  if (EnigmaConfig.showRecSize > 0) {
    if (nRecSize < 0) {
      if ((nRecSize = ReadSizeVdr(Recording->FileName())) < 0 && EnigmaConfig.showRecSize == 2) {
        nRecSize = DirSizeMB(Recording->FileName());
      }
    }
    if (nRecSize >= 0) {
      cString strRecSize = "";
      if (fHasMarks) {
        if (nRecSize > MEGABYTE(1023))
          strRecSize = cString::sprintf("%s: %.2f GB (%s: %.2f GB)", tr("Size"), (float)nRecSize / MEGABYTE(1024), tr("cut"), (float)nRecSizeCut / MEGABYTE(1024));
        else
          strRecSize = cString::sprintf("%s: %lld MB (%s: %lld MB)", tr("Size"), nRecSize / MEGABYTE(1), tr("cut"), nRecSizeCut / MEGABYTE(1));
      } else {
        if (nRecSize > MEGABYTE(1023))
          strRecSize = cString::sprintf("%s: %.2f GB", tr("Size"), (float)nRecSize / MEGABYTE(1024));
        else
          strRecSize = cString::sprintf("%s: %lld MB", tr("Size"), nRecSize / MEGABYTE(1));
      }

      sstrInfo << (const char*)strRecSize << std::endl;
    }
  }

  if (index) {
    int nLastIndex = index->Last();
    if (nLastIndex) {
      cString strLength;
      if (fHasMarks) {
#if VDRVERSNUM >= 10703
        strLength = cString::sprintf("%s: %s (%s %s)", tr("Length"), *IndexToHMSF(nLastIndex, false, Recording->FramesPerSecond()), tr("cut"), *IndexToHMSF(nCutLength, false, Recording->FramesPerSecond()));
#else
        strLength = cString::sprintf("%s: %s (%s %s)", tr("Length"), *IndexToHMSF(nLastIndex), tr("cut"), *IndexToHMSF(nCutLength));
#endif
      } else {
#if VDRVERSNUM >= 10703
        strLength = cString::sprintf("%s: %s", tr("Length"), *IndexToHMSF(nLastIndex, false, Recording->FramesPerSecond()));
#else
        strLength = cString::sprintf("%s: %s", tr("Length"), *IndexToHMSF(nLastIndex));
#endif
      }
      sstrInfo << (const char*)strLength << std::endl;

#if VDRVERSNUM >= 10703
      cString strBitrate = cString::sprintf("%s: %s\n%s: %.2f MBit/s (Video+Audio)", tr("Format"), Recording->IsPesRecording() ? "PES" : "TS", tr("Est. bitrate"), (float)nRecSize / nLastIndex * Recording->FramesPerSecond() * 8 / MEGABYTE(1));
#else
      cString strBitrate = cString::sprintf("%s: %.2f MBit/s (Video+Audio)", tr("Est. bitrate"), (float)nRecSize / nLastIndex * FRAMESPERSEC * 8 / MEGABYTE(1));
#endif
      sstrInfo << (const char*)strBitrate << std::endl;
    }
  }
  delete index;

#if VDRVERSNUM >= 10721
  sstrInfo << trVDR("Priority") << ": " << Recording->Priority() << std::endl
           << trVDR("Lifetime") << ": " << Recording->Lifetime() << std::endl;
#else
  sstrInfo << trVDR("Priority") << ": " << Recording->priority << std::endl
           << trVDR("Lifetime") << ": " << Recording->lifetime << std::endl;
#endif
  if (Info->Aux()) {
    sstrInfo << std::endl << tr("Auxiliary information") << ":\n"
             << parseaux(Info->Aux());
  }
  int xs = xDateRight - Gap;
  const cComponents *Components = Info->Components();
  if (Components) {
    //TODO: show flaggs?
    std::stringstream infoVideo, infoAudio, infoSubtitle;
    for (int i = 0; i < Components->NumComponents(); i++) {
      const tComponent *p = Components->Component(i);
//      sstrInfo << "* "
//               << (int)p->stream << " " << (int)p->type << " (" << (p->language ? p->language : "n/a") << ") (" << (p->description ? p->description : "n/a") << ")\n";

      if (p->stream == 1 && p->language) { // video
        if (p->description) {
          infoVideo << p->description
                    << " (" << p->language << "), ";
        } else {
          infoVideo << p->language << ", ";
        }
      } else if ((p->stream == 2) && p->language) { // audio
        if (p->description) {
          infoAudio << p->description
                    << " (" << p->language << "), ";
        } else {
          infoAudio << p->language << ", ";
        }
      } else if ((p->stream == 3) && p->language) { // subtitle
        if (p->description) {
          infoSubtitle << p->description
                       << " (" << p->language << "), ";
//        } else {  //TODO: no description = no subtitle?
//          infoSubtitle << p->language << ", ";
        }
      }
    }
    if (!infoVideo.str().empty()) {
      sstrInfo << std::endl << tr("Video") << ": "
               << infoVideo.str().substr(0, infoVideo.str().length() - 2); // strip out the last delimiter
    }
    if (!infoAudio.str().empty()) {
      sstrInfo << std::endl << tr("Audio") << ": "
               << infoAudio.str().substr(0, infoAudio.str().length() - 2); // strip out the last delimiter
    }
    if (!infoSubtitle.str().empty()) {
      sstrInfo << std::endl << tr("Subtitles") << ": "
               << infoSubtitle.str().substr(0, infoSubtitle.str().length() - 2); // strip out the last delimiter
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
  TE_MARQUEE(osd, -1, fScrollOther, xBodyLeft + Gap, y, Title, 
             Theme.Color(clrMenuTxtFg), Theme.Color(clrAltBackground),
             pFontDetailsTitle, nBPP, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsTitle->Height());

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
      TE_MARQUEE(osd, -1, fScrollOther, xBodyLeft + Gap, y, Info->ShortText(),
                 Theme.Color(clrMenuItemNotSelectableFg), Theme.Color(clrAltBackground),
                 pFontDetailsSubtitle, nBPP, xHeadlineRight - xBodyLeft - Gap - 1, pFontDetailsSubtitle->Height());
    }
    // draw description
    strDescr = Info->Description();
  }

  std::string stringInfo = sstrInfo.str();
  const char *strInfo = stringInfo.empty() ? NULL : stringInfo.c_str();
  if (strDescr || strInfo) {
    y = yHeadlineBottom + pFontDetailsDate->Height() + BigGap;
    char *mytext = NULL;
    int rc = -1;
    if (EnigmaConfig.showAuxInfo)
      rc = asprintf(&mytext, "%s%s%s", strDescr ? strDescr : "", strInfo && strDescr ? "\n\n" : "", strInfo ? strInfo : "");
    else
      rc = asprintf(&mytext, "%s%s%s", strInfo ? strInfo : "", strInfo && strDescr ? "\n\n" : "", strDescr ? strDescr : "");
    if (-1 != rc) {
      textScroller.Set(osd, xBodyLeft + Gap, y + Gap,
                       xInfoRight - ListHBorder - SmallGap - SmallGap - SmallGap - ScrollbarWidth + SmallGap - xBodyLeft,
                       yBodyBottom - Gap - y - Gap, mytext, pFontDetailsText,
                       Theme.Color(clrMenuTxtFg),
                       Theme.Color(clrBackground));
      SetTextScrollbar();
      free(mytext);
    }
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
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayMenu::SetText(const char *Text, bool FixedFont)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  // draw text
  textScroller.Set(osd, xBodyLeft + Gap, yBodyTop + Gap,
                   GetTextAreaWidth(),
                   yBodyBottom - Gap - yBodyTop - Gap, Text,
                   GetTextAreaFont(FixedFont),
                   Theme.Color(clrMenuTxtFg), Theme.Color(clrBackground));
  SetTextScrollbar();
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

int cSkinEnigmaDisplayMenu::GetTextAreaWidth(void) const
{
  // max text area width
  return (fShowLogo || fShowInfo ? xBodyRight : xInfoRight) - ListHBorder - SmallGap - SmallGap - SmallGap - ScrollbarWidth + SmallGap;
}

const cFont *cSkinEnigmaDisplayMenu::GetTextAreaFont(bool FixedFont) const
{
  // text area font
  return FixedFont ? pFontFixed : pFontDetailsText;
}

int cSkinEnigmaDisplayMenu::getDateWidth(void)
{ // only called from constructor, so pFontDate should be OK
  int w = MIN_DATEWIDTH;
  struct tm tm_r;
  time_t t = time(NULL);
  tm *tm = localtime_r(&t, &tm_r);

  int nWeekday = tm->tm_wday;
  if (0 <= nWeekday && nWeekday < 7)
    w = std::max(w, pFontDate->Width(WeekDayNameFull(nWeekday)));

  char temp[32];
  strftime(temp, sizeof(temp), "%d.%m.%Y", tm);
  w = std::max(w, pFontDate->Width(temp));

  cString time = TimeString(t);
  w = std::max(w, pFontDate->Width(time));

  return w;
}

void cSkinEnigmaDisplayMenu::Flush(void)
{
  if (fLockNeeded && !fLocked) TE_LOCK;
//debug("cSkinEnigmaDisplayMenu::Flush()");

#if VDRVERSNUM >= 10728
  if (cVideoDiskUsage::HasChanged(lastDiskUsageState))
    DrawTitle();
#endif

  if (fShowLogo) {
    time_t t = time(NULL);
    cString time = TimeString(t);
    if ((strLastDate == NULL) || strcmp(strLastDate, (const char*)time) != 0) {
      free(strLastDate);
      strLastDate = strdup((const char*)time);

      int x = xDateLeft + SmallGap;
      int w = xLogoLeft - x;
      int ys = yDateTop + (yDateBottom - SmallGap - yDateTop - 3 * pFontDate->Height()) / 2;

      char temp[32];
      struct tm tm_r;
      tm *tm = localtime_r(&t, &tm_r);

      int nWeekday = tm->tm_wday;
      if (0 <= nWeekday && nWeekday < 7) {
        osd->DrawText(x, ys,WeekDayNameFull(nWeekday), Theme.Color(clrMenuTxtFg),
                      Theme.Color(clrLogoBg), pFontDate, w,
                      pFontDate->Height(), taCenter);
      }
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
      osd->DrawText(xDateLeft + SmallGap, yDateTop, date, Theme.Color(clrMenuTxtFg),
                    Theme.Color(clrLogoBg), pFontDate, xDateRight - xDateLeft - SmallGap,
                    yTitleDecoBottom - yDateTop, taCenter);
    }
  }
  osd->Flush();
  if (fLockNeeded) {
    TE_UNLOCK;
    if (fLocked) {
      fLocked = false;
      TE_WAKEUP;
    }
  }
}

// --- cSkinEnigmaDisplayReplay ---------------------------------------------

class cSkinEnigmaDisplayReplay : public cSkinDisplayReplay, public cSkinEnigmaBaseOsd {
private:
  const cFont *pFontOsdTitle;
  const cFont *pFontReplayTimes;
  const cFont *pFontDate;
  const cFont *pFontLanguage;
  const cFont *pFontMessage;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, xLogoDecoLeft, xLogoDecoRight, yLogoTop, yLogoBottom;
  int xProgressLeft, xProgressRight, yProgressTop, yProgressBottom;
  int xTimeLeft, xTimeRight, yTimeTop, yTimeBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;
  int xFirstSymbol, xIndent;

  char *strLastDate;
  bool modeonly;
  int nJumpWidth;
  int nCurrentWidth;
  bool fShowSymbol;
  bool fLocked;
  bool fLockNeeded;
  bool fScrollTitle;
  int idTitle;
  int nBPP;

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
};

cSkinEnigmaDisplayReplay::cSkinEnigmaDisplayReplay(bool ModeOnly)
{
  struct EnigmaOsdSize OsdSize;
  EnigmaConfig.GetOsdSize(&OsdSize);

  xIndent = EnigmaConfig.drawRoundCorners ? Roundness : SmallGap;

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontReplayTimes = EnigmaConfig.GetFont(FONT_REPLAYTIMES);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontLanguage = EnigmaConfig.GetFont(FONT_CILANGUAGE);
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);

  strLastDate = NULL;

  int MessageHeight = 2 * SmallGap + pFontMessage->Height() + 2 * SmallGap;

  modeonly = ModeOnly;
  nJumpWidth = 0;
  nCurrentWidth = 0;
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsReplay;
  fScrollTitle = !modeonly && EnigmaConfig.useTextEffects && EnigmaConfig.scrollTitle;
  idTitle = -1;
  nBPP = 1;

  int LogoSize = Gap + IconHeight + Gap;
  LogoSize += (LogoSize % 2 ? 1 : 0);
  yTitleTop = 0;
  yTitleBottom = yTitleTop + pFontOsdTitle->Height();
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  xLogoLeft = 0;
  xLogoRight = xLogoLeft + LogoSize;
  xLogoDecoLeft = xLogoRight + LogoDecoGap;
  xLogoDecoRight = xLogoDecoLeft + LogoDecoWidth;
  xTitleLeft = EnigmaConfig.fullTitleWidth ? 0 : xLogoDecoRight + LogoDecoGap2;
  xTitleRight = OsdSize.w;
  yLogoTop = yTitleDecoBottom + TitleDecoGap2;
  yLogoBottom = yLogoTop + LogoSize;
  xTimeLeft = xLogoDecoRight + LogoDecoGap2;
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
  yBottomBottom = yBottomTop + std::max(pFontDate->Height(), pFontLanguage->Height());
  xMessageLeft = xProgressLeft;
  xMessageRight = xProgressRight;
  yMessageTop = yLogoTop + (LogoSize - MessageHeight) / 2;
  yMessageBottom = yMessageTop + MessageHeight;
  xFirstSymbol = xBottomRight - xIndent;

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + OsdSize.h - yBottomBottom);
  tArea SingleArea[] = { {std::min(xTitleLeft, xLogoLeft), yTitleTop, xBottomRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((SingleArea[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayReplay: using %dbpp single area", SingleArea[0].bpp);
    osd->SetAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea));
    if (SingleArea[0].bpp >=8 && Setup.AntiAlias)
      nBPP = 8;
  } else {
    debug("cSkinEnigmaDisplayReplay: using multiple areas");
    tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom - 1, 2},
                      {xLogoLeft, yLogoTop, xLogoDecoRight + 1, yLogoBottom - 1, 4}, //TODO? "+1" because of wrong colors with "-1" 
                      {xProgressLeft, yProgressTop, xTimeRight - 1, yTimeBottom - 1, 4},
                      {xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, 4}
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
    osd->DrawRectangle(xTitleLeft, yTitleDecoTop, xTitleRight - 1,
                       yTitleDecoBottom - 1, Theme.Color(clrTitleBg));
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    osd->DrawRectangle(xLogoDecoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    // draw progress bar area
    osd->DrawRectangle(xProgressLeft, yProgressTop, xProgressRight - 1,
                       yProgressBottom - 1, Theme.Color(clrReplayBarAreaBg));
    // draw time area
    osd->DrawRectangle(xTimeLeft, yTimeTop, xTimeRight - 1, yTimeBottom - 1, Theme.Color(clrAltBackground));
    // draw bottom area
    osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1,
                       yBottomBottom - 1, Theme.Color(clrBottomBg));

    if (EnigmaConfig.drawRoundCorners) {
      // draw rounded corners
      osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                       yTitleTop + Roundness - 1, clrTransparent, -2);
      osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight,
                       yTitleTop + Roundness, clrTransparent, -1);
      osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                       xBottomLeft + Roundness - 1, yBottomBottom - 1,
                       clrTransparent, -3);
      osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                       xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);
    }

    xFirstSymbol = DrawStatusSymbols(0, xFirstSymbol, yBottomTop, yBottomBottom) - Gap;
  }

  fLockNeeded = fScrollTitle;
  fLocked = false;
  TE_START(osd);
  if (fLockNeeded) {
    fLocked = true;
  }
}

cSkinEnigmaDisplayReplay::~cSkinEnigmaDisplayReplay()
{
  if (fLockNeeded && !fLocked) TE_LOCK;
  TE_STOP;
  free(strLastDate);
  delete osd;
}

void cSkinEnigmaDisplayReplay::SetTitle(const char *Title)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  idTitle = TE_MARQUEE(osd, idTitle, fScrollTitle, xTitleLeft + xIndent, yTitleTop, Title, Theme.Color(clrTitleFg), Theme.Color(clrTitleBg), pFontOsdTitle, nBPP, xTitleRight - xIndent - xTitleLeft - xIndent - 1);
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::SetMode(bool Play, bool Forward, int Speed)
{
  if (fLockNeeded && !fLocked) TE_LOCK;

  bool fFoundLogo = false;
  if (Speed < -1)
    Speed = -1;

  if (modeonly) {
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, clrTransparent);
    osd->DrawEllipse(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  } else {
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    osd->DrawRectangle(xLogoDecoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  }

  if (fShowSymbol) {
    char *logo = NULL;
    if (-1 != asprintf(&logo, "icons/menu/%s", EnigmaStatus.ReplayModeName())) {
      if (EnigmaLogoCache.LoadIcon(logo)) {
        osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                        yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                        EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                        clrTransparent, true, true);
      }
      free(logo);
    }
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
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::SetProgress(int isCurrent, int Total)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  // create progressbar
  cProgressBar pb(xProgressRight - xProgressLeft - 2 * BigGap,
                  yProgressBottom - yProgressTop - 2 * BigGap, isCurrent, Total,
                  marks, Theme.Color(clrReplayProgressSeen),
                  Theme.Color(clrReplayProgressRest),
                  Theme.Color(clrReplayProgressSelected),
                  Theme.Color(clrReplayProgressMark),
                  Theme.Color(clrReplayProgressCurrent));
  // draw progressbar
  osd->DrawBitmap(xProgressLeft + BigGap, yProgressTop + BigGap, pb);
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::SetCurrent(const char *CurrentTime)
{
  if (!CurrentTime)
    return;

  // draw current time
  int w = pFontReplayTimes->Width(CurrentTime);
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  osd->DrawText(xTimeLeft + BigGap, yTimeTop, CurrentTime,
                Theme.Color(clrReplayCurrent), Theme.Color(clrAltBackground), pFontReplayTimes,
                w, yTimeBottom - yTimeTop, taLeft);
  if (nCurrentWidth > w)
    osd->DrawRectangle(xTimeLeft + BigGap + w, yTimeTop, xTimeLeft + BigGap + nCurrentWidth - 1, yTimeBottom - 1, Theme.Color(clrAltBackground));
  nCurrentWidth = w;
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::SetTotal(const char *Total)
{
  if (!Total)
    return;

  // draw total time
  int w = pFontReplayTimes->Width(Total);
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
  osd->DrawText(xTimeRight - BigGap - w, yTimeTop, Total,
                Theme.Color(clrReplayTotal), Theme.Color(clrAltBackground), pFontReplayTimes, w,
                yTimeBottom - yTimeTop, taRight);
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::SetJump(const char *Jump)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
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
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::SetMessage(eMessageType Type, const char *Text)
{
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
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
  if (fLockNeeded && !fLocked) TE_UNLOCK;
}

void cSkinEnigmaDisplayReplay::Flush(void)
{
  if (fLockNeeded && !fLocked) TE_LOCK;
  // update date
  if (!modeonly) {
    cString date = DayDateTime();
    if ((strLastDate == NULL) || strcmp(strLastDate, (const char*)date) != 0) {
      free(strLastDate);
      strLastDate = strdup((const char*)date);
      osd->DrawText(xBottomLeft + xIndent, yBottomTop, date,
                    Theme.Color(clrTitleFg), Theme.Color(clrBottomBg),
                    pFontDate,
                    xFirstSymbol - xBottomLeft - xIndent - 1,
                    yBottomBottom - yBottomTop - 1, taLeft);
    }
  }
  osd->Flush();
  if (fLockNeeded) {
    TE_UNLOCK;
    if (fLocked) {
      fLocked = false;
      TE_WAKEUP;
    }
  }
}

// --- cSkinEnigmaDisplayVolume ---------------------------------------------

class cSkinEnigmaDisplayVolume : public cSkinDisplayVolume, public cSkinEnigmaBaseOsd {
private:
  const cFont *pFontOsdTitle;
  const cFont *pFontDate;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom, xLogoDecoLeft, xLogoDecoRight;
  int xBodyLeft, xBodyRight, yBodyTop, yBodyBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;
  int xIndent;

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

  xIndent = EnigmaConfig.drawRoundCorners ? Roundness : SmallGap;

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
  tArea SingleArea[] = { {xLogoLeft, yLogoTop, xTitleRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((SingleArea[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayVolume: using %dbpp single area", SingleArea[0].bpp);
    osd->SetAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea));
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
  // draw body area
  osd->DrawRectangle(xBodyLeft, yBodyTop, xBodyRight - 1, yBodyBottom - 1, Theme.Color(clrBackground));
  // draw bottom area
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, Theme.Color(clrBottomBg));

  if (EnigmaConfig.drawRoundCorners) {
    // draw rounded corners
    osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1, yTitleTop + Roundness - 1, clrTransparent, -1);
    osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness, xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);

    if (!fShowSymbol) {
      osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1, yTitleTop + Roundness - 1, clrTransparent, -2);
      osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness, xBottomLeft + Roundness, yBottomBottom - 1, clrTransparent, -3);
    }
  }
}

cSkinEnigmaDisplayVolume::~cSkinEnigmaDisplayVolume()
{
  delete osd;
}

void cSkinEnigmaDisplayVolume::SetVolume(int CurrentVol, int Total, bool Mute)
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
  int vol = xBodyLeft + Gap + (xBodyRight - Gap - xBodyLeft - Gap) * CurrentVol / Total;
  // draw titlebar
  osd->DrawRectangle(xTitleLeft + (fShowSymbol ? Gap : xIndent), yTitleTop, xTitleRight - xIndent - 1,
                     yTitleBottom - 1, Theme.Color(clrTitleBg));
  osd->DrawText(xTitleLeft + (fShowSymbol ? Gap : xIndent), yTitleTop, Prompt,
                Theme.Color(clrTitleFg), clrTransparent, pFontOsdTitle,
                xTitleRight - xIndent - xTitleLeft, pFontOsdTitle->Height(), taCenter);
  // draw volumebar
  osd->DrawRectangle(xBodyLeft + Gap, yBodyTop + Gap, xBodyRight - Gap - 1,
                     yBodyBottom - Gap - 1, Theme.Color(clrBackground));
  osd->DrawRectangle(xBodyLeft + Gap, yBodyTop + Gap, vol - 1,
                     yBodyBottom - Gap - 1, ColorBar);
}

void cSkinEnigmaDisplayVolume::Flush(void)
{
  osd->Flush();
}

// --- cSkinEnigmaDisplayTracks ---------------------------------------------

class cSkinEnigmaDisplayTracks : public cSkinDisplayTracks, public cSkinEnigmaBaseOsd {
private:
  const cFont *pFontOsdTitle;
  const cFont *pFontDate;
  const cFont *pFontListItem;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, xLogoDecoLeft, xLogoDecoRight, yLogoTop, yLogoBottom;
  int xListLeft, xListRight, yListTop, yListBottom, xItemLeft, xItemRight;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;

  int xIndent;
  int lineHeight;
  int nMarkerGap;
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

  xIndent = EnigmaConfig.drawRoundCorners ? Roundness : SmallGap;

  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);
  pFontListItem = EnigmaConfig.GetFont(FONT_LISTITEM);

  lastTime = 0;
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsAudio;

  bool fShowSymbolNow = false; // if symbols are requested try to find subtitle XPM and force to display it at the end of this function, else disable symbols
  if (fShowSymbol && strcmp(Title, trVDR("Button$Subtitles")) == 0) {
      if (EnigmaLogoCache.LoadIcon("icons/subtitle/subtitle"))
        fShowSymbolNow = true;
      else
        fShowSymbol = false;
  }

  lineHeight = pFontListItem->Height();
  nMarkerGap = min(MarkerGap, lineHeight / 2 - 1); //lineHeight - 2 * MarkerGap
  int LogoSize = IconHeight;
  LogoSize += (LogoSize % 2 ? 1 : 0);
  currentIndex = -1;
  int ItemsWidth = 0;
  //Get required space for the list items
  for (int i = 0; i < NumTracks; i++)
    ItemsWidth = std::max(ItemsWidth, pFontListItem->Width(Tracks[i]));
  //Add required space for marker or border
  ItemsWidth += (EnigmaConfig.showMarker ? lineHeight : ListHBorder) + ListHBorder;
  //If OSD title only covers the list then its content must fit too
  if (!EnigmaConfig.fullTitleWidth)
    ItemsWidth = std::max(ItemsWidth, pFontOsdTitle->Width(Title) + 2 * xIndent + (fShowSymbol ? 0 : (bmAudio[0].Width() + SmallGap)));
  //If the symbol is shown the list's width should be at least as wide as the symbol
  if (fShowSymbol)
    ItemsWidth = std::max(ItemsWidth, LogoSize);
  //Now let's calculate the OSD's full width
  if (EnigmaConfig.fullTitleWidth) {
    //If the symbol is shown, add its width
    if (fShowSymbol)
      ItemsWidth += LogoSize + LogoDecoGap + LogoDecoWidth + LogoDecoGap2;
    //The width must be wide enough for the OSD title
    ItemsWidth = std::max(ItemsWidth, pFontOsdTitle->Width(Title) + 2 * xIndent + (fShowSymbol ? 0 : (bmAudio[0].Width() + SmallGap)));
  }

  yTitleTop = 0;
  yTitleBottom = pFontOsdTitle->Height();
  yTitleDecoTop = yTitleBottom + TitleDecoGap;
  yTitleDecoBottom = yTitleDecoTop + TitleDecoHeight;
  xLogoLeft = 0;
  xLogoRight = xLogoLeft + LogoSize;
  xLogoDecoLeft = xLogoRight + LogoDecoGap;
  xLogoDecoRight = xLogoDecoLeft + LogoDecoWidth;
  yLogoTop = yTitleDecoBottom + TitleDecoGap2;
  yLogoBottom = yLogoTop + std::max(LogoSize, NumTracks * lineHeight);
  xTitleLeft = fShowSymbol ? (EnigmaConfig.fullTitleWidth ? xLogoLeft : xLogoDecoRight + LogoDecoGap2) : 0;
  xTitleRight = xTitleLeft + FixWidth(ItemsWidth, 2);
  xListLeft = fShowSymbol ? (xLogoDecoRight + LogoDecoGap2) : 0;
  xListRight = xTitleRight;
  yListTop = yLogoTop;
  yListBottom = yLogoBottom;
  xItemLeft = xListLeft + (EnigmaConfig.showMarker ? lineHeight : ListHBorder);
  xItemRight = xListRight - ListHBorder;
  xBottomLeft = xTitleLeft;
  xBottomRight = xTitleRight;
  yBottomTop = yListBottom + SmallGap;
  yBottomBottom = yBottomTop + pFontDate->Height();

  // create osd
  osd = cOsdProvider::NewOsd(OsdSize.x, OsdSize.y + OsdSize.h - yBottomBottom);
  tArea SingleArea[] = { {fShowSymbol ? xLogoLeft : xTitleLeft, yTitleTop, xBottomRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((SingleArea[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayTracks: using %dbpp single area", SingleArea[0].bpp);
    osd->SetAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea));
  } else {
    debug("cSkinEnigmaDisplayTracks: using multiple areas");
    if (fShowSymbol) {
      tArea Areas[] = { {xTitleLeft, yTitleTop, xTitleRight - 1, yTitleDecoBottom- 1, 2},
                         {xLogoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, 4},
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
  osd->DrawText(xTitleLeft + xIndent, yTitleTop, Title,
                Theme.Color(clrTitleFg), clrTransparent,
                pFontOsdTitle,
                xTitleRight - xIndent - xTitleLeft - xIndent, yTitleBottom - yTitleTop,
                fShowSymbol ? taCenter : taLeft);
  if (fShowSymbol) {
    // draw logo area
    osd->DrawRectangle(xLogoLeft, yLogoTop, xLogoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
    osd->DrawRectangle(xLogoDecoLeft, yLogoTop, xLogoDecoRight - 1, yLogoBottom - 1, Theme.Color(clrLogoBg));
  }
  // draw list area
  osd->DrawRectangle(xListLeft, yListTop, xListRight - 1, yListBottom - 1, Theme.Color(clrBackground));
  // draw bottom
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, Theme.Color(clrBottomBg));

  if (EnigmaConfig.drawRoundCorners) {
    // draw rounded corners
    osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1,
                     yTitleTop + Roundness - 1, clrTransparent, -2);
    osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1,
                     yTitleTop + Roundness - 1, clrTransparent, -1);
    osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                     xBottomLeft + Roundness, yBottomBottom - 1, clrTransparent, -3);
    osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                     xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);
  }

  // fill up audio tracks
  for (int i = 0; i < NumTracks; i++)
    SetItem(Tracks[i], i, false);

  if (fShowSymbolNow) { // subtitle logo
    osd->DrawBitmap(xLogoLeft + (xLogoRight - xLogoLeft - EnigmaLogoCache.Get().Width()) / 2,
                    yLogoTop + (yLogoBottom - yLogoTop - EnigmaLogoCache.Get().Height()) / 2,
                    EnigmaLogoCache.Get(), EnigmaLogoCache.Get().Color(1),
                    Theme.Color(clrLogoBg));
  }
}

cSkinEnigmaDisplayTracks::~cSkinEnigmaDisplayTracks()
{
  delete osd;
}

void cSkinEnigmaDisplayTracks::SetItem(const char *Text, int Index, bool isCurrent)
{
  int y = yListTop + Index * lineHeight;
  tColor ColorFg, ColorBg;
  if (isCurrent) {
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
    osd->DrawEllipse(xListLeft + nMarkerGap, y + nMarkerGap,
                     xListLeft + lineHeight - nMarkerGap,
                     y + lineHeight - nMarkerGap,
                     isCurrent ? ColorFg : ColorBg);
  }
  osd->DrawText(xItemLeft, y, Text, ColorFg, ColorBg, pFontListItem, xItemRight - xItemLeft, lineHeight);
  osd->DrawRectangle(xItemRight, y, xListRight - 1, y + lineHeight - 1, ColorBg);
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
    osd->DrawBitmap(xTitleRight - xIndent - bmAudio[AudioChannel].Width(),
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
  osd->Flush();
}

// --- cSkinEnigmaDisplayMessage --------------------------------------------

class cSkinEnigmaDisplayMessage : public cSkinDisplayMessage, public cSkinEnigmaBaseOsd {
private:
  const cFont *pFontMessage;

  int xTitleLeft, xTitleRight, yTitleTop, yTitleBottom, yTitleDecoTop, yTitleDecoBottom;
  int xLogoLeft, xLogoRight, yLogoTop, yLogoBottom, xLogoDecoLeft, xLogoDecoRight;
  int xMessageLeft, xMessageRight, yMessageTop, yMessageBottom;
  int xBottomLeft, xBottomRight, yBottomTop, yBottomBottom;
  int xIndent;

  bool fShowSymbol;
  int idMessage;
  bool fScrollOther;
  bool fLocked;
  bool fLockNeeded;
  int nBPP;

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

  xIndent = EnigmaConfig.drawRoundCorners ? Roundness : SmallGap;

  const cFont *pFontOsdTitle;
  const cFont *pFontDate;
  pFontMessage = EnigmaConfig.GetFont(FONT_MESSAGE);
  pFontOsdTitle = EnigmaConfig.GetFont(FONT_OSDTITLE);
  pFontDate = EnigmaConfig.GetFont(FONT_DATE);

  fScrollOther = EnigmaConfig.useTextEffects && EnigmaConfig.scrollOther;
  idMessage = -1;
  fShowSymbol = EnigmaConfig.showSymbols && EnigmaConfig.showSymbolsMsgs;
  nBPP = 1;

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
  tArea SingleArea[] = { {xLogoLeft, yLogoTop, xBottomRight - 1, yBottomBottom - 1, fShowSymbol ? 8 : 4} };
  if ((SingleArea[0].bpp < 8 || EnigmaConfig.singleArea8Bpp) && osd->CanHandleAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea)) == oeOk) {
    debug("cSkinEnigmaDisplayMessage: using %dbpp single area", SingleArea[0].bpp);
    osd->SetAreas(SingleArea, sizeof(SingleArea) / sizeof(tArea));
    if (SingleArea[0].bpp >=8 && Setup.AntiAlias)
      nBPP = 8;
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

  fLockNeeded = fScrollOther;
  fLocked = false;
  TE_START(osd);
  if (fLockNeeded) {
    fLocked = true;
  }
}

cSkinEnigmaDisplayMessage::~cSkinEnigmaDisplayMessage()
{
  debug("cSkinEnigmaDisplayMessage::~cSkinEnigmaDisplayMessage");

  if (fLockNeeded && !fLocked) TE_LOCK;
  TE_STOP;
  delete osd;
}

void cSkinEnigmaDisplayMessage::SetMessage(eMessageType Type, const char *Text)
{
  debug("cSkinEnigmaDisplayMessage::SetMessage");
  if (fLockNeeded && !fLocked) {
    fLocked = true;
    TE_LOCK;
  }
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
  // draw centered message text
  osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1, yMessageBottom - 1, clrTransparent);
  osd->DrawRectangle(xMessageLeft, yMessageTop, xMessageRight - 1,
                     yMessageBottom - 1, Theme.Color(clrButtonRedBg));
  idMessage = TE_MARQUEE(osd, idMessage, fScrollOther, xMessageLeft, yMessageTop + 2 * SmallGap, Text,
                         Theme.Color(clrMessageStatusFg + 2 * Type),
                         Theme.Color(clrMessageStatusBg + 2 * Type),
                         pFontMessage, nBPP, 
                         xMessageRight - xMessageLeft,
                         yMessageBottom - 2 * SmallGap - yMessageTop - 2 * SmallGap, taCenter);
  // draw bottom
  osd->DrawRectangle(xBottomLeft, yBottomTop, xBottomRight - 1, yBottomBottom - 1, Theme.Color(clrBottomBg));

  if (EnigmaConfig.drawRoundCorners) {
    // draw rounded corners
    osd->DrawEllipse(xTitleRight - Roundness, yTitleTop, xTitleRight - 1, yTitleTop + Roundness - 1, clrTransparent, -1);
    osd->DrawEllipse(xBottomRight - Roundness, yBottomBottom - Roundness,
                     xBottomRight - 1, yBottomBottom - 1, clrTransparent, -4);

    if (!(fShowSymbol)) {
      // draw rounded corners at the left side if not showing logo
      osd->DrawEllipse(xTitleLeft, yTitleTop, xTitleLeft + Roundness - 1, yTitleTop + Roundness - 1, clrTransparent, -2);
      osd->DrawEllipse(xBottomLeft, yBottomBottom - Roundness,
                       xBottomLeft + Roundness - 1, yBottomBottom - 1, clrTransparent, -3);
    }
  }
  if (fLockNeeded && !fLocked)
    TE_UNLOCK;
}

void cSkinEnigmaDisplayMessage::Flush(void)
{
  if (fLockNeeded && !fLocked) TE_LOCK;

  osd->Flush();

  if (fLockNeeded) {
    TE_UNLOCK;
    if (fLocked) {
      fLocked = false;
      TE_WAKEUP;
    }
  }
}

// --- cSkinEnigmaOsd ----------------------------------------------------------

bool cSkinEnigmaBaseOsd::HasChannelTimerRecording(const cChannel *Channel)
{
  // try to find current channel from timers
  for (cTimer * t = Timers.First(); t; t = Timers.Next(t)) {
    if ((t->Channel() == Channel) && t->Recording())
      return true;
  }
  return false;
}

int cSkinEnigmaBaseOsd::DrawStatusSymbols(int /* x0 */, int xs, int top, int bottom, const cChannel *Channel /* = NULL */)
{
  if (!EnigmaConfig.showStatusSymbols)
    return xs;

  int ys = top + (bottom - top - SymbolHeight) / 2;
  const cFont *pFontLanguage = EnigmaConfig.GetFont(FONT_CILANGUAGE);

  cDevice *Device = cDevice::PrimaryDevice();
  eTrackType TrackType = Device->GetCurrentAudioTrack();
  const tTrackId *Track = Device->GetTrack(TrackType);
  if (Track) {
    int AudioMode = Device->GetAudioChannel();
    if (!(AudioMode >= 0 && AudioMode < MAX_AUDIO_BITMAPS))
      AudioMode = 0;

    char *strFile = NULL;
    if (-1 != asprintf(&strFile, "flags/%s%s", Track->language, IS_DOLBY_TRACK(TrackType) ? "DD" : (AudioMode > 0 ? "MONO" : ""))) {
      bool fFlagFound = false;
      if (EnigmaConfig.showFlags) {
        if (EnigmaLogoCache.LoadSymbol(strFile)) {
          xs -= (EnigmaLogoCache.Get().Width() + SmallGap);
          osd->DrawBitmap(xs, ys, EnigmaLogoCache.Get());
          fFlagFound = true;
        }
      }
    
      if (!fFlagFound) {
        const char *strLang = Track->description ? Track->description : (strFile + strlen("flags/")); //don't show "flags/" in OSD
        xs -= (pFontLanguage->Width(strLang) + SmallGap);
        osd->DrawText(xs, top + SmallGap , strLang,
                      Theme.Color(clrSymbolActive), Theme.Color(clrBottomBg), pFontLanguage,
                      pFontLanguage->Width(strLang), bottom - SmallGap);
      }
      free(strFile);
    }
  }

  // draw recording symbol
  if (cRecordControls::Active()) {
    xs -= (bmRecording.Width() + SmallGap);
    //TODO? get current channel on primary device: Channel = Device->CurrentChannel()
    bool fRecording = Channel && HasChannelTimerRecording(Channel);
    osd->DrawBitmap(xs, ys, bmRecording,
                    Theme.Color(fRecording ? clrSymbolRecordBg : clrBottomBg),
                    Theme.Color(fRecording ? clrSymbolRecord : clrSymbolActive));
  } else if (EnigmaConfig.showStatusSymbols == 1) {
    xs -= (bmRecording.Width() + SmallGap);
    osd->DrawBitmap(xs, ys, bmRecording,
                    Theme.Color(clrBottomBg), Theme.Color(clrSymbolInactive));
  }

#ifdef USE_PLUGIN_MAILBOX
  if (EnigmaConfig.showMailIcon) {
    cPlugin *MailBoxPlugin = cPluginManager::GetPlugin("mailbox");
    bool fHasNewMail = false;
    if (MailBoxPlugin && MailBoxPlugin->Service(MailBox_HasNewMail_v1_0_NAME, &fHasNewMail)) {
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
    avards_CurrentVideoFormat_v1_0 avardsData;
    if (AvardsPlugin->Service(AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID, &avardsData)) {
      if (EnigmaConfig.showWssSymbols) {
        if (avardsData.Mode > 0) {
          if (avardsData.Mode < MAX_AVARDS_MODES) {
            xs -= (bmWSS[avardsData.Mode].Width() + SmallGap);
            osd->DrawBitmap(xs, ys, bmWSS[avardsData.Mode], Theme.Color(clrBottomBg), Theme.Color(clrSymbolActive));
          } else {
            esyslog("ERROR: Unknown Avards mode %d", avardsData.Mode);
          }
        }
      } else {
        if (avardsData.Mode > 0 && avardsData.ModeString) {
          xs -= (pFontLanguage->Width(avardsData.ModeString) + SmallGap);
          osd->DrawText(xs, top + SmallGap , avardsData.ModeString,
                        Theme.Color(clrSymbolActive), Theme.Color(clrBottomBg), pFontLanguage,
                        pFontLanguage->Width(avardsData.ModeString), bottom - SmallGap);
        }
      }
    }
  }
#endif

  return xs;
}

int cSkinEnigmaBaseOsd::FixWidth(int w, int bpp, bool enlarge /* = true */)
{
  int a = 8 / bpp;
  int r = w & (a - 1);
  if (r == 0)
    return w;

  return enlarge ? (w + a -r) : (w - r);
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

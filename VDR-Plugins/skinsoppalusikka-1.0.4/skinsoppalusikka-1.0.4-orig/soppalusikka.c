/*
 * skinsoppalusikka.c: The 'soppalusikka' VDR skin
 *
 * $Id: $
 */

#include "common.h"
#include "tools.h"
#include "config.h"
#include "logo.h"
#include "i18n.h"
#include "soppalusikka.h"
#include <math.h>
#include <ctype.h>
#include <vdr/device.h>
#include <vdr/timers.h>
#include <vdr/menu.h>
#include <vdr/font.h>
#include <vdr/osd.h>
#include <vdr/themes.h>
#include <vdr/plugin.h>

#include "symbols/arrowdown.xpm"
#include "symbols/arrowup.xpm"
#include "symbols/audio.xpm"
#include "symbols/audioleft.xpm"
#include "symbols/audioright.xpm"
#include "symbols/dolbydigital.xpm"
#include "symbols/encrypted.xpm"
#include "symbols/ffwd.xpm"
#include "symbols/ffwd1.xpm"
#include "symbols/ffwd2.xpm"
#include "symbols/ffwd3.xpm"
#include "symbols/frew.xpm"
#include "symbols/frew1.xpm"
#include "symbols/frew2.xpm"
#include "symbols/frew3.xpm"
#include "symbols/pause.xpm"
#include "symbols/play.xpm"
#include "symbols/radio.xpm"
#include "symbols/recording.xpm"
#include "symbols/running.xpm"
#include "symbols/sfwd.xpm"
#include "symbols/sfwd1.xpm"
#include "symbols/sfwd2.xpm"
#include "symbols/sfwd3.xpm"
#include "symbols/srew.xpm"
#include "symbols/srew1.xpm"
#include "symbols/srew2.xpm"
#include "symbols/srew3.xpm"
#include "symbols/teletext.xpm"
#include "symbols/vps.xpm"
#include "symbols/eventparttimer.xpm"
#include "symbols/eventtimer.xpm"
#include "symbols/eventvps.xpm"
#include "symbols/eventrunning.xpm"
#include "symbols/eventrecording.xpm"
#include "symbols/recordingnew.xpm"

static cBitmap bmArrowDown(arrowdown_xpm);
static cBitmap bmArrowUp(arrowup_xpm);
static cBitmap bmAudio(audio_xpm);
static cBitmap bmAudioLeft(audioleft_xpm);
static cBitmap bmAudioRight(audioright_xpm);
static cBitmap bmDolbyDigital(dolbydigital_xpm);
static cBitmap bmEncrypted(encrypted_xpm);
static cBitmap bmFastForward(ffwd_xpm);
static cBitmap bmFastForward1(ffwd1_xpm);
static cBitmap bmFastForward2(ffwd2_xpm);
static cBitmap bmFastForward3(ffwd3_xpm);
static cBitmap bmFastReverse(frew_xpm);
static cBitmap bmFastReverse1(frew1_xpm);
static cBitmap bmFastReverse2(frew2_xpm);
static cBitmap bmFastReverse3(frew3_xpm);
static cBitmap bmPause(pause_xpm);
static cBitmap bmPlay(play_xpm);
static cBitmap bmRadio(radio_xpm);
static cBitmap bmRecording(recording_xpm);
static cBitmap bmRunning(running_xpm);
static cBitmap bmSlowForward(sfwd_xpm);
static cBitmap bmSlowForward1(sfwd1_xpm);
static cBitmap bmSlowForward2(sfwd2_xpm);
static cBitmap bmSlowForward3(sfwd3_xpm);
static cBitmap bmSlowReverse(srew_xpm);
static cBitmap bmSlowReverse1(srew1_xpm);
static cBitmap bmSlowReverse2(srew2_xpm);
static cBitmap bmSlowReverse3(srew3_xpm);
static cBitmap bmTeletext(teletext_xpm);
static cBitmap bmVPS(vps_xpm);
static cBitmap bmEventPartTimer(eventparttimer_xpm);
static cBitmap bmEventTimer(eventtimer_xpm);
static cBitmap bmEventVPS(eventvps_xpm);
static cBitmap bmEventRunning(eventrunning_xpm);
static cBitmap bmEventRecording(eventrecording_xpm);
static cBitmap bmRecordingNew(recordingnew_xpm);

static cTheme Theme;

THEME_CLR(Theme, clrBackground,             0xC8000066);
THEME_CLR(Theme, clrButtonRedFg,            0xFFFFFFFF);
THEME_CLR(Theme, clrButtonRedBg,            0xC8CC1111);
THEME_CLR(Theme, clrButtonGreenFg,          0xFF000000);
THEME_CLR(Theme, clrButtonGreenBg,          0xC822BB22);
THEME_CLR(Theme, clrButtonYellowFg,         0xFF000000);
THEME_CLR(Theme, clrButtonYellowBg,         0xC8EEEE22);
THEME_CLR(Theme, clrButtonBlueFg,           0xFFFFFFFF);
THEME_CLR(Theme, clrButtonBlueBg,           0xC82233CC);
THEME_CLR(Theme, clrMessageStatusFg,        0xFFFFEE00);
THEME_CLR(Theme, clrMessageStatusBg,        0xC8000066);
THEME_CLR(Theme, clrMessageInfoFg,          0xFFFFEE00);
THEME_CLR(Theme, clrMessageInfoBg,          0xC8000066);
THEME_CLR(Theme, clrMessageWarningFg,       0xFFFFEE00);
THEME_CLR(Theme, clrMessageWarningBg,       0xC8000066);
THEME_CLR(Theme, clrMessageErrorFg,         0xFFFFEE00);
THEME_CLR(Theme, clrMessageErrorBg,         0xC8000066);
THEME_CLR(Theme, clrVolumePromptFg,         0xFF000000);
THEME_CLR(Theme, clrVolumePromptBg,         0xFFFFFFFF);
THEME_CLR(Theme, clrVolumeBarUpper,         0xC8000066);
THEME_CLR(Theme, clrVolumeBarLower,         0xFF33CC33);
THEME_CLR(Theme, clrVolumeBarMute,          0xFFFF0000);
THEME_CLR(Theme, clrChannelNameFg,          0xFF000000);
THEME_CLR(Theme, clrChannelNameBg,          0xFFFFFFFF);
THEME_CLR(Theme, clrChannelNumberDateFg,    0xFF000000);
THEME_CLR(Theme, clrChannelNumberDateBg,    0xFFFFFFFF);
THEME_CLR(Theme, clrChannelEpgTimeFg,       0xFFDDDDDD);
THEME_CLR(Theme, clrChannelEpgTimeBg,       0xC8CC1111);
THEME_CLR(Theme, clrChannelEpgTitle,        0xFF00FFFF);
THEME_CLR(Theme, clrChannelEpgShortText,    0xFFCCBB22);
THEME_CLR(Theme, clrChannelEpgDuration,     0xFF7F7F7F);
THEME_CLR(Theme, clrChannelTimeBarFg,       0xFF7F7F7F);
THEME_CLR(Theme, clrChannelTimeBarBg,       0xFF000000);
THEME_CLR(Theme, clrChannelSymbolActive,    0xFFCCBB22);
THEME_CLR(Theme, clrChannelSymbolInactive,  0xFF7F7F7F);
THEME_CLR(Theme, clrChannelSymbolRecord,    0xFFFF0000);
THEME_CLR(Theme, clrMenuTitleFg,            0xFF000000);
THEME_CLR(Theme, clrMenuTitleBg,            0xC833AAEE);
THEME_CLR(Theme, clrMenuItemCurrentFg,      0xFF000000);
THEME_CLR(Theme, clrMenuItemCurrentBg,      0xC833AAEE);
THEME_CLR(Theme, clrMenuItemSelectable,     0xFFFFFFFF);
THEME_CLR(Theme, clrMenuEventTime,          0xFFFFFFFF);
THEME_CLR(Theme, clrMenuEventTitle,         0xFFCCBB22);
THEME_CLR(Theme, clrMenuEventShortText,     0xFFCCBB22);
THEME_CLR(Theme, clrMenuEventDescription,   0xFF00FFFF);
THEME_CLR(Theme, clrMenuEventSymbolFg,      0xFFCCBB22);
THEME_CLR(Theme, clrMenuEventSymbolBg,      0xC8000066);
THEME_CLR(Theme, clrMenuScrollbarTotal,     0xC8000066);
THEME_CLR(Theme, clrMenuScrollbarShown,     0xFFCCBB22);
THEME_CLR(Theme, clrMenuText,               0xFFCCBB22);
THEME_CLR(Theme, clrReplayTitleFg,          0xFF000000);
THEME_CLR(Theme, clrReplayTitleBg,          0xFFFFFFFF);
THEME_CLR(Theme, clrReplayCurrent,          0xFFFFFFFF);
THEME_CLR(Theme, clrReplayTotal,            0xFFFFFFFF);
THEME_CLR(Theme, clrReplayModeJump,         0xFFFFFFFF);
THEME_CLR(Theme, clrReplaySymbolActive,     0xFFCCBB22);
THEME_CLR(Theme, clrReplaySymbolInactive,   0xFF7F7F7F);
THEME_CLR(Theme, clrReplayProgressSeen,     0xFF33CC33);
THEME_CLR(Theme, clrReplayProgressRest,     0xFFFFFFFF);
THEME_CLR(Theme, clrReplayProgressSelected, 0xFFFF0000);
THEME_CLR(Theme, clrReplayProgressMark,     0xFF000000);
THEME_CLR(Theme, clrReplayProgressCurrent,  0xFFFF0000);

#define TinyGap 1
#define SmallGap 2
#define Gap 4
#define BigGap 8
#define Roundness 10

// --- cSkinSoppalusikkaDisplayChannel --------------------------------------------

class cSkinSoppalusikkaDisplayChannel : public cSkinDisplayChannel {
private:
  cOsd *osd;
  bool islogo;
  int x0, x1;
  int xt0, xt1, xt2, xt3, xt4, xt5, xt6, xt7, xt8, xt9;
  int xb0, xb1, xb2, xb3, xb4, xb5, xb6;
  int y0, y1;
  int yt0, yt1, yt2, yt3, yt4;
  int yb0, yb1, yb2, yb3, yb4, yb5;
  bool HasChannelTimerRecording(const cChannel *Channel);
  void ResetTopAreaCoordinates(bool islogo = false);
  void DrawTopArea(const cChannel *Channel = NULL);
  void DrawBottomArea(void);
  cString GetChannelName(const cChannel *Channel);
  cString GetChannelNumber(const cChannel *Channel, int Number);
public:
  cSkinSoppalusikkaDisplayChannel(bool WithInfo);
  virtual ~cSkinSoppalusikkaDisplayChannel();
  virtual void SetChannel(const cChannel *Channel, int Number);
  virtual void SetEvents(const cEvent *Present, const cEvent *Following);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayChannel::cSkinSoppalusikkaDisplayChannel(bool WithInfo)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int lineHeight = font->Height();
  islogo = false;
  // general coordinates
  x0 = 0;
  x1 = Setup.OSDWidth;
  // top area x-coordinates
  xt9 = x1 - BigGap;
  xt0 = x0 + BigGap;
  xt1 = xt0;
  xt2 = xt1 + Roundness;
  xt3 = xt2 + font->Width("0000-");
  xt8 = xt9 - Roundness;
  xt7 = xt8 - bmTeletext.Width() - bmAudio.Width() - bmDolbyDigital.Width() - bmEncrypted.Width() - bmRecording.Width() - 7 * BigGap;
  if (SoppalusikkaConfig.showvps) {
     xt7 -= bmVPS.Width();
     }
  xt6 = xt7 - Roundness;
  xt5 = xt6 - 5 * BigGap;
  xt4 = xt5 - Roundness;
  // bottom area x-coordinates
  xb6 = x1 - BigGap;
  xb0 = x0 + BigGap;
  xb1 = xb0 + Roundness;
  xb2 = xb1 + font->Width("00:00") + BigGap;
  xb3 = xb2 + 3 * Gap;
  xb4 = xb3 + BigGap;
  xb5 = xb6 - Roundness;
  // top area y-coordinates
  y0 = 0;
  yt0 = y0 + BigGap;
  yt1 = yt0 + Roundness;
  yt2 = yt0 + lineHeight;
  yt3 = yt2 + SmallGap;
  yt4 = yt3 + lineHeight;
  // bottom area y-coordinates
  yb0 = yt4;
  yb1 = yb0 + lineHeight;
  yb2 = yb1 + lineHeight;
  yb3 = yb2 + lineHeight;
  yb5 = yb3 + lineHeight;
  yb4 = yb5 - Roundness;
  y1 = yb5;
  // create osd
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + (Setup.ChannelInfoPos ? 0 : Setup.OSDHeight - y1));
  // try to use single 8bpp area
  tArea Areas[] = { { x0, y0, x1 - 1, y1 - 1, 8 } };
  if (SoppalusikkaConfig.usesinglearea && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     if (SoppalusikkaConfig.showlogo) {
        tArea Areas[] = { { x0, y0, x0 + ChannelLogoWidth - 1, y0 + ChannelLogoHeight - 1, 4 },
                          { x0 + ChannelLogoWidth, y0, x1 - 1, y0 + ChannelLogoHeight - 1, 4 },
                          { x0, y0 + ChannelLogoHeight, x1 - 1, y1 - 1, 4 }
                        };
        if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        }
     else {
        tArea Areas[] = { { x0, y0, x1 - 1, y1 - 1, 4 } };
        if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        }
     }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  // draw top area
  DrawTopArea(NULL);
  // draw bottom area
  DrawBottomArea();
}

cSkinSoppalusikkaDisplayChannel::~cSkinSoppalusikkaDisplayChannel()
{
  delete osd;
}

bool cSkinSoppalusikkaDisplayChannel::HasChannelTimerRecording(const cChannel *Channel)
{
  // try to find current channel from timers
  for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
      if ((t->Channel() == Channel) && t->Recording())
         return true;
      }
  return false;
}

void cSkinSoppalusikkaDisplayChannel::ResetTopAreaCoordinates(bool islogo)
{
  const cFont *font = cFont::GetFont(fontOsd);
  // x-coordinates
  xt1 = (islogo ? x0 + ChannelLogoWidth + TinyGap : xt0);
  xt2 = xt1 + Roundness;
  xt3 = xt2 + font->Width("0000-");
}

void cSkinSoppalusikkaDisplayChannel::DrawTopArea(const cChannel *Channel)
{
  // draw logo stuff
  if (SoppalusikkaConfig.showlogo && Channel && SoppalusikkaLogoCache.Load(SoppalusikkaConfig.usechannelid ? *Channel->GetChannelID().ToString() : Channel->Name())) {
     // load channel logo
     islogo = true;
     // set top area for logo
     ResetTopAreaCoordinates(islogo);
     // draw missing bling-blings
     osd->DrawRectangle(x0, y0, x0 + TinyGap + ChannelLogoWidth - 1, y0 + SmallGap + ChannelLogoHeight - 1, Theme.Color(clrBackground));
     osd->DrawRectangle(xt0, y0 + SmallGap + ChannelLogoHeight, xt1 - 1, yt4 - 1, Theme.Color(clrChannelNameBg));
     // draw logo
     osd->DrawBitmap(x0, y0, SoppalusikkaLogoCache.Get(), 0, 0, true);
     }
  else {
     // clear possible logo leftovers
     if (islogo) {
        osd->DrawRectangle(x0, y0, xt1 - 1, yt4 - 1, clrTransparent);
        }
     // no logo
     islogo = false;
     // set top area for logoless
     ResetTopAreaCoordinates(islogo);
     }
  // draw clock row
  osd->DrawRectangle(xt1, yt0, xt4 - 1, yt2 - 1, Theme.Color(clrChannelNumberDateBg));
  osd->DrawRectangle(xt4, yt0, xt9 - 1, yt2 - 1, Theme.Color(clrBackground));
  // draw rounded left corner of clock row
  if (!islogo)
     osd->DrawEllipse(xt1, yt0, xt2 - 1, yt1 - 1, clrTransparent, -2);
  // draw rounded right edge of clock row
  osd->DrawEllipse(xt4, yt0, xt5, yt2 - 1, Theme.Color(clrChannelNumberDateBg), 5);
  // draw rounded right corner of clock row
  osd->DrawEllipse(xt8, yt0, xt9 - 1, yt1 - 1, clrTransparent, -1);
  // draw separator
  osd->DrawRectangle(xt1, yt2, xt9 - 1, yt3 - 1, Theme.Color(clrBackground));
  // draw channel row
  osd->DrawRectangle(xt1, yt3, xt6 - 1, yt4 - 1, Theme.Color(clrChannelNameBg));
  osd->DrawRectangle(xt6, yt3, xt9 - 1, yt4 - 1, Theme.Color(clrBackground));
  // draw rounded right edge of channel row
  osd->DrawEllipse(xt6, yt3, xt7, yt4 - 1, Theme.Color(clrChannelNameBg), 5);
}

void cSkinSoppalusikkaDisplayChannel::DrawBottomArea(void)
{
  // draw time area
  osd->DrawRectangle(xb0, yb0, xb2 - 1, yb5 - 1, Theme.Color(clrChannelEpgTimeBg));
  // draw rounded left corner
  osd->DrawEllipse(xb0, yb4, xb1 - 1, yb5 - 1, clrTransparent, -3);
  // draw timebar area
  osd->DrawRectangle(xb2, yb0, xb3 - 1, yb5 - 1, Theme.Color(clrChannelTimeBarBg));
  // draw event area
  osd->DrawRectangle(xb3, yb0, xb6 - 1, yb5 - 1, Theme.Color(clrBackground));
  // draw rounded right corner
  osd->DrawEllipse(xb5, yb4, xb6 - 1, yb5 - 1, clrTransparent, -4);
}

cString cSkinSoppalusikkaDisplayChannel::GetChannelName(const cChannel *Channel)
{
  char buffer[256];
  // check if channel exists
  if (Channel) {
     snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
     }
  else {
     snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));
     }
  return buffer;
}

cString cSkinSoppalusikkaDisplayChannel::GetChannelNumber(const cChannel *Channel, int Number)
{
  char buffer[256];
  // check if real channel exists
  if (Channel && !Channel->GroupSep()) {
     snprintf(buffer, sizeof(buffer), "%d%s", Channel->Number(), Number ? "-" : "");
     }
  else if (Number) {
     // no channel but number
     snprintf(buffer, sizeof(buffer), "%d-", Number);
     }
  else {
     // no channel and no number
     snprintf(buffer, sizeof(buffer), " ");
     }
  return buffer;
}

void cSkinSoppalusikkaDisplayChannel::SetChannel(const cChannel *Channel, int Number)
{
  // draw top area
  DrawTopArea(Channel);
  // draw channel number
  osd->DrawText(xt2, yt0, GetChannelNumber(Channel, Number), Theme.Color(clrChannelNumberDateFg), Theme.Color(clrChannelNumberDateBg), cFont::GetFont(fontSml), xt3 - xt2, yt2 - yt0);
  // draw channel name
  osd->DrawText(xt2, yt3, GetChannelName(Channel), Theme.Color(clrChannelNameFg), Theme.Color(clrChannelNameBg), cFont::GetFont(fontSml), xt6 - xt2, yt4 - yt3);
  // draw symbols
  if (Channel && !Channel->GroupSep()) {
     int xs = xt8;
     bool isvps = false;
     // check if vps
     if (SoppalusikkaConfig.showvps) {
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
     // draw radio symbol if no video PID; otherwise draw teletext symbol
     if (Channel->Vpid()) {
        xs -= bmTeletext.Width();
        osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmTeletext.Height()) / 2, bmTeletext, Theme.Color(Channel->Tpid() ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
        }
     else {
        xs -= bmRadio.Width();
        osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmRadio.Height()) / 2, bmRadio, Theme.Color(Channel->Apid(0) ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
        }
     // draw audio symbol according to current audio channel
     switch (cDevice::PrimaryDevice()->GetAudioChannel()) {
       default:
       case 0: /* stereo */
            xs -= (bmAudio.Width() + BigGap);
            osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmAudio.Height()) / 2, bmAudio, Theme.Color(Channel->Apid(1) ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
            break;
       case 1: /* mono left */
            xs -= (bmAudioLeft.Width() + BigGap);
            osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmAudioLeft.Height()) / 2, bmAudioLeft, Theme.Color(Channel->Apid(1) ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
            break;
       case 2: /* mono right */
            xs -= (bmAudioRight.Width() + 2 * BigGap);
            osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmAudioRight.Height()) / 2, bmAudioRight, Theme.Color(Channel->Apid(1) ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
            break;
        }
     // draw dolby digital symbol
     xs -= (bmDolbyDigital.Width() + BigGap);
     osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmDolbyDigital.Height()) / 2, bmDolbyDigital, Theme.Color(Channel->Dpid(0) ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
     // draw encryption symbol
     xs -= (bmEncrypted.Width() + BigGap);
     osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmEncrypted.Height()) / 2, bmEncrypted, Theme.Color(Channel->Ca() ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
     // draw vps symbol
     if (SoppalusikkaConfig.showvps) {
        xs -= (bmVPS.Width() + BigGap);
        osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmVPS.Height()) / 2, bmVPS, Theme.Color(isvps ? clrChannelSymbolActive : clrChannelSymbolInactive), Theme.Color(clrBackground));
        }
     // draw recording symbol
     xs -= (bmRecording.Width() + BigGap);
     osd->DrawBitmap(xs, yt0 + (yt4 - yt0 - bmRecording.Height()) / 2, bmRecording, Theme.Color(cRecordControls::Active() ? (HasChannelTimerRecording(Channel) ? clrChannelSymbolRecord : clrChannelSymbolActive) : clrChannelSymbolInactive), Theme.Color(clrBackground));
     }
}

void cSkinSoppalusikkaDisplayChannel::SetEvents(const cEvent *Present, const cEvent *Following)
{
  // draw bottom area
  DrawBottomArea();
  // check epg datas
  const cEvent *e = Present;
  if (e) {
     char *s;
     int total = e->Duration();
     int now = (time(NULL) - e->StartTime());
     if ((now < total) && ((now / 60) > 0)) {
        asprintf(&s, "  %d / %d %s", now / 60, total / 60, tr("min"));
        }
     else {
        asprintf(&s, "  %d %s", total / 60, tr("min"));
        }
     // draw start time
     osd->DrawText(xb1, yb0, e->GetTimeString(), Theme.Color(clrChannelEpgTimeFg), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontOsd), xb2 - xb1, yb1 - yb0);
     // draw title
     osd->DrawText(xb4, yb0, e->Title(), Theme.Color(clrChannelEpgTitle), Theme.Color(clrBackground), cFont::GetFont(fontOsd), xb5 - xb4, yb1 - yb0);
     // draw duration
     osd->DrawText(xb5 - cFont::GetFont(fontSml)->Width(s), yb0, s, Theme.Color(clrChannelEpgDuration), Theme.Color(clrBackground), cFont::GetFont(fontSml), cFont::GetFont(fontSml)->Width(s), yb1 - yb0);
     // draw vps time
     if (SoppalusikkaConfig.showvps && e->Vps() && (e->Vps() != e->StartTime())) {
        /* difference between start time and vps time in minutes */
        int delta = (e->StartTime() - e->Vps()) / 60;
        /* check if difference is less than 10 hours */
        if (abs(delta) < 600) {
           char *vps;
           /* relative vps time formats: "+0:30" "-1:30" */
           asprintf(&vps, "%c%01d:%02d", delta < 0 ? '-' : '+', abs(delta) / 60, abs(delta) % 60);
           osd->DrawText(xb1, yb1, vps, Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2 - xb1 - Gap, yb2 - yb1, taRight);
           free(vps);
           }
        else {
           /* absolute vps time format: "18:45" */
           osd->DrawText(xb1, yb1, TimeString(e->Vps()), Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2 - xb1 - Gap, yb2 - yb1, taRight);
           }
        }
     // draw shorttext
     osd->DrawText(xb4, yb1, e->ShortText(), Theme.Color(clrChannelEpgShortText), Theme.Color(clrBackground), cFont::GetFont(fontSml), xb5 - xb4 - Gap, yb2 - yb1);
     // draw timebar
     int yc = yb0 + (int)(roundf((float)(now) / (float)(total) * (float)(yb5 - yb0)));
     yc = min(yc, yb5);
     osd->DrawRectangle(xb2 + SmallGap, yb0, xb3 - SmallGap - 1, yc - 1, Theme.Color(clrChannelTimeBarFg));
     free(s);
     }
  e = Following;
  if (e) {
     char *s;
     asprintf(&s, "  %d %s", e->Duration() / 60, tr("min"));
     // draw start time
     osd->DrawText(xb1, yb2, e->GetTimeString(), Theme.Color(clrChannelEpgTimeFg), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontOsd), xb2 - xb1, yb3 - yb2);
     // draw title
     osd->DrawText(xb4, yb2, e->Title(), Theme.Color(clrChannelEpgTitle), Theme.Color(clrBackground), cFont::GetFont(fontOsd), xb5 - xb4, yb3 - yb2);
     // draw duration
     osd->DrawText(xb5 - cFont::GetFont(fontSml)->Width(s), yb2, s, Theme.Color(clrChannelEpgDuration), Theme.Color(clrBackground), cFont::GetFont(fontSml), cFont::GetFont(fontSml)->Width(s), yb3 - yb2);
     // draw vps time - only if skin dependent small fonts
     if (SoppalusikkaConfig.showvps && e->Vps() && (e->Vps() != e->StartTime())) {
        /* difference between start time and vps time in minutes */
        int delta = (e->StartTime() - e->Vps()) / 60;
        /* check if difference is less than 10 hours */
        if (abs(delta) < 600) {
           char *vps;
           /* relative vps time formats: "+0:30" "-1:30" */
           asprintf(&vps, "%c%d:%02d", delta < 0 ? '-' : '+', abs(delta) / 60, abs(delta) % 60);
           osd->DrawText(xb1, yb3, vps, Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2 - xb1 - Gap, yb5 - yb3, taRight);
           free(vps);
           }
        else {
           /* absolute vps time format: "18:45" */
           osd->DrawText(xb1, yb3, TimeString(e->Vps()), Theme.Color(clrChannelEpgShortText), Theme.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2 - xb1 - Gap, yb5 - yb3, taRight);
           }
        }
     // draw shorttext
     osd->DrawText(xb4, yb3, e->ShortText(), Theme.Color(clrChannelEpgShortText), Theme.Color(clrBackground), cFont::GetFont(fontSml), xb5 - xb4, yb5 - yb3);
     free(s);
     }
}

void cSkinSoppalusikkaDisplayChannel::SetMessage(eMessageType Type, const char *Text)
{
  // check if message
  if (Text) {
     // save osd region
     osd->SaveRegion(xb0, yb3, xb6 - 1, yb5 - 1);
     // draw message area
     osd->DrawRectangle(xb0, yb3, xb6 - 1, yb5 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
     // draw message
     osd->DrawText(xb1, yb3, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), cFont::GetFont(fontOsd), xb5 - xb1, yb5 - yb3, taCenter);
     // draw rounded left corner
     osd->DrawEllipse(xb0, yb4, xb1 - 1, yb5 - 1, clrTransparent, -3);
     // draw rounded right corner
     osd->DrawEllipse(xb5, yb4, xb6 - 1, yb5 - 1, clrTransparent, -4);
     }
  else {
     // restore saved osd region
     osd->RestoreRegion();
     }
}

void cSkinSoppalusikkaDisplayChannel::Flush(void)
{
  cString date = DayDateTime();
  // draw updated date string
  osd->DrawText(xt3, yt0, date, Theme.Color(clrChannelNumberDateFg), Theme.Color(clrChannelNumberDateBg), cFont::GetFont(fontSml), xt4 - xt3, yt1 - yt0, taRight);
  osd->Flush();
}

// --- cSkinSoppalusikkaDisplayMenu -----------------------------------------------

class cSkinSoppalusikkaDisplayMenu : public cSkinDisplayMenu {
private:
  cOsd *osd;
  int x0, x1, x2, x3;
  int y0, y1, y2, y3, y4, y5, y6, y7, y8;
  int lineHeight;
  void SetScrollbar(void);
public:
  cSkinSoppalusikkaDisplayMenu();
  virtual ~cSkinSoppalusikkaDisplayMenu();
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
  };

cSkinSoppalusikkaDisplayMenu::cSkinSoppalusikkaDisplayMenu(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeight = font->Height();
  x0 = 0;
  x1 = x0 + Roundness;
  x3 = Setup.OSDWidth;
  x2 = x3 - Roundness;
  y0 = 0;
  y1 = y0 + Roundness;
  y2 = y0 + lineHeight;
  y3 = y2 + SmallGap;
  y4 = y3 + lineHeight;
  y8 = Setup.OSDHeight;
  y7 = y8 - Roundness;
  y6 = y8 - lineHeight;
  y5 = y6 - lineHeight;
  // create osd
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop);
  // try to use single 8bpp area
  tArea Areas[] = { { x0, y0, x3 - 1, y8 - 1, 8 } };
  if (SoppalusikkaConfig.usesinglearea && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     // clear all
     osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
     }
  else {
     tArea Areas[] = { { x0, y0, x3 - 1, y8 - 1, 4 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        // clear all
        osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
        }
     else {
        tArea Areas[] = { { x0, y0, x3 - 1, y2 - 1, 2 },
                          { x0, y2, x3 - 1, y5 - 1, 2 },
                           { x0, y5, x3 - 1, y6 - 1, 2 },
                           { x0, y6, x3 - 1, y8 - 1, 4 }
                        };
        if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        // clear all
        osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
        // fill up items area palette to prevent palette overflow
        osd->GetBitmap(1)->Reset();
        osd->GetBitmap(1)->SetColor(0, Theme.Color(clrBackground));
        osd->GetBitmap(1)->SetColor(1, Theme.Color(clrMenuItemSelectable));
        osd->GetBitmap(1)->SetColor(2, Theme.Color(clrMenuItemCurrentBg));
        osd->GetBitmap(1)->SetColor(3, Theme.Color(clrMenuItemCurrentFg));
        }
     }
  // draw titlebar
  osd->DrawRectangle(x0, y0, x3 - 1, y2 - 1, Theme.Color(clrMenuTitleBg));
  // draw rounded left corner of title bar
  osd->DrawEllipse(x0, y0, x1 - 1, y1 - 1, clrTransparent, -2);
  // draw rounded right corner of title bar
  osd->DrawEllipse(x2, y0, x3 - 1, y1 - 1, clrTransparent, -1);
  // draw gap + items + message area
  osd->DrawRectangle(x0, y2, x3 - 1, y6 - 1, Theme.Color(clrBackground));
  // draw colorbar
  osd->DrawRectangle(x0, y6, x3 - 1, y8 - 1, Theme.Color(clrBackground));
  osd->DrawRectangle(x0, y6, x1 - 1, y8 - 1, Theme.Color(clrButtonRedBg));
  osd->DrawRectangle(x2, y6, x3 - 1, y8 - 1, Theme.Color(clrButtonBlueBg));
  SetButtons(NULL);
  // draw rounded left corner of colorbar
  osd->DrawEllipse(x0, y7, x1 - 1, y8 - 1, clrTransparent, -3);
  // draw rounded right corner of colorbar
  osd->DrawEllipse(x2, y7, x3 - 1, y8 - 1, clrTransparent, -4);
}

cSkinSoppalusikkaDisplayMenu::~cSkinSoppalusikkaDisplayMenu()
{
  delete osd;
}

void cSkinSoppalusikkaDisplayMenu::SetScrollbar(void)
{
  // check if scrollbar is needed
  if (textScroller.CanScroll()) {
     int h = bmArrowUp.Height();
     int w = bmArrowUp.Width();
     int yt = textScroller.Top();
     int yb = yt + textScroller.Height();
     int st = yt + h + Gap;
     int sb = yb - h - Gap;
     int tt = st + (sb - st) * textScroller.Offset() / textScroller.Total();
     int tb = tt + (sb - st) * textScroller.Shown() / textScroller.Total();
     int xl = x3 - w - Gap;
      // draw up symbol
     osd->DrawBitmap(xl, yt, bmArrowUp, Theme.Color(clrMenuScrollbarShown), Theme.Color(clrBackground));
     // draw background of scrollbar
     osd->DrawRectangle(xl, st, xl + w, sb, Theme.Color(clrMenuScrollbarTotal));
     // draw visible area of scrollbar
     osd->DrawRectangle(xl, tt, xl + w, tb, Theme.Color(clrMenuScrollbarShown));
      // draw down symbol
     osd->DrawBitmap(xl, yb - h, bmArrowDown, Theme.Color(clrMenuScrollbarShown), Theme.Color(clrBackground));
     }
}

void cSkinSoppalusikkaDisplayMenu::Scroll(bool Up, bool Page)
{
  cSkinDisplayMenu::Scroll(Up, Page);
  SetScrollbar();
}

int cSkinSoppalusikkaDisplayMenu::MaxItems(void)
{
  // max number of items
  return (y5 - y3) / lineHeight;
}

void cSkinSoppalusikkaDisplayMenu::Clear(void)
{
  textScroller.Reset();
  // clear items area
  osd->DrawRectangle(x0, y2, x3 - 1, y6 - 1, Theme.Color(clrBackground));
}

void cSkinSoppalusikkaDisplayMenu::SetTitle(const char *Title)
{
  // draw title
  osd->DrawText(x1, y0, Title, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), cFont::GetFont(fontSml), x2 - x1, y2 - y0);
}

void cSkinSoppalusikkaDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{
  const cFont *font = cFont::GetFont(fontSml);
  int t0 = x1;
  int t4 = x2;
  int t2 = t0 + (t4 - t0) / 2;
  int t1 = t0 + (t2 - t0) / 2;
  int t3 = t2 + (t4 - t2) / 2;
  // draw color buttons
  osd->DrawText(t0, y6, Red,    Theme.Color(clrButtonRedFg),    Theme.Color(clrButtonRedBg),    font, t1 - t0, y8 - y6, taCenter);
  osd->DrawText(t1, y6, Green,  Theme.Color(clrButtonGreenFg),  Theme.Color(clrButtonGreenBg),  font, t2 - t1, y8 - y6, taCenter);
  osd->DrawText(t2, y6, Yellow, Theme.Color(clrButtonYellowFg), Theme.Color(clrButtonYellowBg), font, t3 - t2, y8 - y6, taCenter);
  osd->DrawText(t3, y6, Blue,   Theme.Color(clrButtonBlueFg),   Theme.Color(clrButtonBlueBg),   font, t4 - t3, y8 - y6, taCenter);
}

void cSkinSoppalusikkaDisplayMenu::SetMessage(eMessageType Type, const char *Text)
{
  // check if message
  if (Text) {
     // draw message
     osd->DrawText(x1, y5, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), cFont::GetFont(fontOsd), x2 - x1, y6 - y5, taCenter);
     }
  else {
     // clear message area
     osd->DrawRectangle(x1, y5, x2 - 1, y6 - 1, Theme.Color(clrBackground));
     }
}

void cSkinSoppalusikkaDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable)
{
  int y = y3 + Index * lineHeight;
  tColor ColorFg, ColorBg;
  // select colors
  if (Current) {
     ColorFg = Theme.Color(clrMenuItemCurrentFg);
     ColorBg = Theme.Color(clrMenuItemCurrentBg);
     }
  else {
     ColorFg = Theme.Color(Selectable ? clrMenuItemSelectable : clrMenuItemCurrentBg);
     ColorBg = Theme.Color(clrBackground);
     }
  const cFont *font = cFont::GetFont(fontOsd);
  // this should prevent menu flickering
  osd->DrawRectangle(x0, y + lineHeight / 2, x0 + 1, y + lineHeight / 2 + 1, Theme.Color(clrMenuItemSelectable));
  // draw rounded left corner
  osd->DrawEllipse(x0, y, x1 - 1, y + lineHeight - 1, ColorBg, 7);
  // draw item
  for (int i = 0; i < MaxTabs; i++) {
      const char *s = GetTabbedText(Text, i);
      if (s) {
         char buffer[9];
         int xt = x1 + Tab(i);
         bool iseventinfo = false;
         bool isnewrecording = false;
         bool isprogressbar = false;
         int now = 0, total = 0;
         // check if event info symbol: "tTV*" "R"
         if (SoppalusikkaConfig.showsymbols) {
            // check if event info characters
            if (strlen(s) == 3 && ischaracter(s[0], " tTR") && ischaracter(s[1], " V") && ischaracter(s[2], " *")) {
               // update status
               iseventinfo = true;
               }
            }
         // check if new recording: "01.01.06*", "10:10*"
         if (!iseventinfo && SoppalusikkaConfig.showsymbols &&
            (strlen(s) == 6 && s[5] == '*' && s[2] == ':' && isdigit(*s) && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4))) ||
            (strlen(s) == 9 && s[8] == '*' && s[5] == '.' && s[2] == '.' && isdigit(*s) && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4)) && isdigit(*(s + 6)) && isdigit(*(s + 7)))) {
            // update status
            isnewrecording = true;
            // make a copy
            strncpy(buffer, s, strlen(s));
            // remove the '*' character
            buffer[strlen(s) - 1] = '\0';
            }
         // check if progress bar: "[|||||||   ]"
         if (!iseventinfo && !isnewrecording && SoppalusikkaConfig.showprogressbar &&
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
                   }
                else {
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
            osd->DrawRectangle(xt, y, x2, y + lineHeight - 1, ColorBg);
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
            }
         else if (isnewrecording) {
            // draw text
            osd->DrawText(xt, y, buffer, ColorFg, ColorBg, font, x2 - xt);
            // draw symbol
            osd->DrawBitmap(xt + font->Width(buffer), y + (lineHeight - bmRecordingNew.Height()) / 2, bmRecordingNew, ColorFg, ColorBg);
            }
         else if (isprogressbar) {
            // define x coordinates of progressbar
            int px0 = xt;
            int px1 = px0 + SmallGap;
            int px2 = px1 + SmallGap;
            int px5;
            if (Tab(i+1))
                px5 = px0 + Tab(i+1) - Tab(i);
            else
                px5 = px0 + x2 - x1 - Tab(i);
            int px4 = px5 - SmallGap;
            int px3 = px4 - SmallGap;
            int px = px2 + (int)((float)now * (float)(px3 - px2) / (float)total);
            // define y coordinates of progressbar
            int py0 = y + Gap;
            int py1 = py0 + SmallGap;
            int py2 = py1 + Gap;
            int py5 = y + lineHeight - Gap;
            int py4 = py5 - SmallGap;
            int py3 = py4 - Gap;
            // draw background
            osd->DrawRectangle(xt, y, x2, y + lineHeight - 1, ColorBg);
            // draw progressbar
            osd->DrawRectangle(px0, py0, px1, py5, ColorFg);
            osd->DrawRectangle(px4, py0, px5, py5, ColorFg);
            osd->DrawRectangle(px1, py0, px4, py1, ColorFg);
            osd->DrawRectangle(px1, py4, px4, py5, ColorFg);
            osd->DrawRectangle(px2, py2, px,  py3, ColorFg);
            }
         else {
            // draw text
            osd->DrawText(xt, y, s, ColorFg, ColorBg, font, x2 - xt);
            }
         }
      if (!Tab(i + 1))
         break;
      }
  // draw rounded right corner
  osd->DrawEllipse(x2, y, x3 - 1, y + lineHeight - 1, ColorBg, 5);
  //set editable width
  SetEditableWidth(x2 - x1 - Tab(1));
}

void cSkinSoppalusikkaDisplayMenu::SetEvent(const cEvent *Event)
{
  // check if event
  if (!Event)
     return;
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smlfont = cFont::GetFont(fontSml);
  cTextScroller ts;
  char t[64];
  int y = y3;
  int xs = x1;
  int wsb = bmArrowUp.Width() + 2 * Gap;
  // check if event has timer
  if (Event->HasTimer()) {
     // draw timer symbol
     osd->DrawBitmap(xs, y + (y4 - y) / bmRecording.Height(), bmRecording, Theme.Color(clrMenuEventSymbolFg), Theme.Color(clrMenuEventSymbolBg));
     xs += (bmRecording.Width() + Roundness);
     }
  // check if event is running
  if (Event->IsRunning()) {
     // draw running symbol
     osd->DrawBitmap(xs, y + (y4 - y) / bmRunning.Height(), bmRunning, Theme.Color(clrMenuEventSymbolFg), Theme.Color(clrMenuEventSymbolBg));
     xs += (bmRunning.Width() + Roundness);
     }
  // check if event has VPS 
  if (Event->Vps()) {
     // draw VPS symbol
     osd->DrawBitmap(xs, y + (y4 - y) / bmVPS.Height(), bmVPS, Theme.Color(clrMenuEventSymbolFg), Theme.Color(clrMenuEventSymbolBg));
     xs += (bmVPS.Width() + Roundness);
     }
  y = y4;
  // draw event date / duration string
  snprintf(t, sizeof(t), "%s  %s - %s (%d %s)", *Event->GetDateString(), *Event->GetTimeString(), *Event->GetEndTimeString(), Event->Duration() / 60, tr("min"));
  ts.Set(osd, x1, y, x2 - x1, y5 - y, t, font, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
  y += ts.Height();
  // check if event has VPS
  if (Event->Vps() && Event->Vps() != Event->StartTime()) {
     char *buffer;
     asprintf(&buffer, "%s: %s", tr("VPS"), *Event->GetVpsString());
     ts.Set(osd, x1, y, x2 - x1, y5 - y, buffer, smlfont, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
     y += ts.Height();
     free(buffer);
     }
  // draw recording languages
  const cComponents *Components = Event->Components();
  if (Components) {
     char *info;
     asprintf(&info, "%s: ", tr("Languages"));
     for (int i = 0; i < Components->NumComponents(); i++) {
         const tComponent *p = Components->Component(i);
         if ((p->stream == 2) && p->language) {
            if (p->description) {
               info = strcatrealloc(info, p->description);
               info = strcatrealloc(info, " (");
               info = strcatrealloc(info, p->language);
               info = strcatrealloc(info, "), ");
               }
            else {
               info = strcatrealloc(info, p->language);
               info = strcatrealloc(info, ", ");
               }
            }
         }
     // strip out the last delimiter
     if (endswith(info, ", ")) {
        char *s = info + strlen(info) - 2;                                                                                                                                        
        *s = 0;
        }
     ts.Set(osd, x1, y, x2 - x1, y5 - y, info, smlfont, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
     y += ts.Height();
     free(info);
     }
  y += smlfont->Height();
  // draw event title
  ts.Set(osd, x1, y, x2 - x1, y5 - y, Event->Title(), font, Theme.Color(clrMenuEventTitle), Theme.Color(clrBackground));
  y += ts.Height();
  // draw recording short text and description
  if (isempty(Event->Description())) {
     y += smlfont->Height();
     // check if short text
     if (!isempty(Event->ShortText())) {
        // draw short text as description, if no description available
        textScroller.Set(osd, x1, y, x3 - x1 - wsb, y5 - y, Event->ShortText(), font, Theme.Color(clrMenuEventDescription), Theme.Color(clrBackground));
        SetScrollbar();
        }
     }
  else {
     // check if short text
     if (!isempty(Event->ShortText())) {
        // draw short text
        ts.Set(osd, x1, y, x2 - x1, y5 - y, Event->ShortText(), smlfont, Theme.Color(clrMenuEventShortText), Theme.Color(clrBackground));
        y += ts.Height();
        }
     y += smlfont->Height();
     // draw description
     textScroller.Set(osd, x1, y, x3 - x1 - wsb, y5 - y, Event->Description(), font, Theme.Color(clrMenuEventDescription), Theme.Color(clrBackground));
     SetScrollbar();
     }
}

void cSkinSoppalusikkaDisplayMenu::SetRecording(const cRecording *Recording)
{
  // check if recording
  if (!Recording)
     return;
  const cRecordingInfo *Info = Recording->Info();
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smlfont = cFont::GetFont(fontSml);
  int y = y3;
  int wsb = bmArrowUp.Width() + 2 * Gap;
  cTextScroller ts;
  char t[32];
  // draw recording date string
  snprintf(t, sizeof(t), "%s  %s", *DateString(Recording->start), *TimeString(Recording->start));
  ts.Set(osd, x1, y, x2 - x1, y5 - y, t, font, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
  y += ts.Height();
  // draw additional information
  char *info;
  asprintf(&info, "%s: %d %s: %d", tr("Priority"), Recording->priority, tr("Lifetime"), Recording->lifetime);
  if (SoppalusikkaConfig.showauxinfo && Info->Aux()) {
     char *aux = strdup(Info->Aux());
     info = strcatrealloc(info, "\n");
     info = strcatrealloc(info, tr("Auxiliary information"));
     info = strcatrealloc(info, ": ");
     info = strcatrealloc(info, parseaux(aux));
     free(aux);
     }
  const cComponents *Components = Info->Components();
  if (Components) {
     info = strcatrealloc(info, "\n");
     info = strcatrealloc(info, tr("Languages"));
     info = strcatrealloc(info, ": ");
     for (int i = 0; i < Components->NumComponents(); i++) {
         const tComponent *p = Components->Component(i);
         if ((p->stream == 2) && p->language) {
            if (p->description) {
               info = strcatrealloc(info, p->description);
               info = strcatrealloc(info, " (");
               info = strcatrealloc(info, p->language);
               info = strcatrealloc(info, "), ");
               }
            else {
               info = strcatrealloc(info, p->language);
               info = strcatrealloc(info, ", ");
               }
            }
         }
     // strip out the last delimiter
     if (endswith(info, ", ")) {
        char *s = info + strlen(info) - 2;
        *s = 0;
        }
     }
  ts.Set(osd, x1, y, x2 - x1, y5 - y, info, smlfont, Theme.Color(clrMenuEventTime), Theme.Color(clrBackground));
  y += ts.Height();
  free(info);
  y += smlfont->Height();
  // draw recording title
  const char *Title = Info->Title();
  if (isempty(Title))
     Title = Recording->Name();
  ts.Set(osd, x1, y, x2 - x1, y5 - y, Title, font, Theme.Color(clrMenuEventTitle), Theme.Color(clrBackground));
  y += ts.Height();
  // draw recording short text and description
  if (isempty(Info->Description())) {
     y += smlfont->Height();
     // check if short text
     if (!isempty(Info->ShortText())) {
        // draw short text as description, if no description available
        textScroller.Set(osd, x1, y, x3 - x1 - wsb, y5 - y, Info->ShortText(), font, Theme.Color(clrMenuEventDescription), Theme.Color(clrBackground));
        SetScrollbar();
        }
     }
  else {
     // check if short text
     if (!isempty(Info->ShortText())) {
        // draw short text
        ts.Set(osd, x1, y, x2 - x1, y5 - y, Info->ShortText(), smlfont, Theme.Color(clrMenuEventShortText), Theme.Color(clrBackground));
        y += ts.Height();
        }
     y += smlfont->Height();
     // draw description
     textScroller.Set(osd, x1, y, x3 - x1 - wsb, y5 - y, Info->Description(), font, Theme.Color(clrMenuEventDescription), Theme.Color(clrBackground));
     SetScrollbar();
     }
}

void cSkinSoppalusikkaDisplayMenu::SetText(const char *Text, bool FixedFont)
{
  int wsb = bmArrowUp.Width() + 2 * Gap;
  // draw text
  textScroller.Set(osd, x1, y3, x3 - x1 - wsb, y5 - y3, Text, GetTextAreaFont(FixedFont), Theme.Color(clrMenuText), Theme.Color(clrBackground));
  SetScrollbar();
}

int cSkinSoppalusikkaDisplayMenu::GetTextAreaWidth(void) const
{
  // max text area width
  return x2 - x1;
}

const cFont *cSkinSoppalusikkaDisplayMenu::GetTextAreaFont(bool FixedFont) const
{
  // text area font
  return cFont::GetFont(FixedFont ? fontFix : fontOsd);
}

void cSkinSoppalusikkaDisplayMenu::Flush(void)
{
  cString date = DayDateTime();
  const cFont *font = cFont::GetFont(fontSml);
  int w = font->Width("Wmm 07.07 07:07");
  int xl = x2 - w;
  // update date string on titlebar
  osd->DrawText(x2 - w, y0, date, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), font, w, y2 - y0, taRight);
  // draw recording symbol
  xl -= bmRecording.Width();
  osd->DrawBitmap(xl, y0 + (y2 - y0 - bmRecording.Height()) / 2, bmRecording, Theme.Color(clrMenuTitleBg), Theme.Color(cRecordControls::Active() ? clrMenuTitleFg : clrMenuTitleBg));
  osd->Flush();
}

// --- cSkinSoppalusikkaDisplayReplay ---------------------------------------------

class cSkinSoppalusikkaDisplayReplay : public cSkinDisplayReplay {
private:
  cOsd *osd;
  int x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
  int y0, y1, y2, y3, y4, y5;
  int lineHeight;
  bool drawdate;
  bool modeonly;
public:
  cSkinSoppalusikkaDisplayReplay(bool ModeOnly);
  virtual ~cSkinSoppalusikkaDisplayReplay();
  virtual void SetTitle(const char *Title);
  virtual void SetMode(bool Play, bool Forward, int Speed);
  virtual void SetProgress(int Current, int Total);
  virtual void SetCurrent(const char *Current);
  virtual void SetTotal(const char *Total);
  virtual void SetJump(const char *Jump);
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayReplay::cSkinSoppalusikkaDisplayReplay(bool ModeOnly)
{
  int sw = bmFastReverse.Width() + bmSlowReverse.Width() + bmPlay.Width() + bmPause.Width() + bmSlowForward.Width() + bmFastForward.Width();
  lineHeight = cFont::GetFont(fontOsd)->Height();
  drawdate = true;
  modeonly = ModeOnly;
  x0 = 0;
  x1 = x0 + BigGap;
  x2 = x1 + Roundness;
  x3 = x2 + sw + 5 * Gap;
  x4 = x3 + 4 * BigGap;
  x10 = Setup.OSDWidth;
  x9 = x10 - BigGap;
  x8 = x9 - Roundness;
  x6 = x8 - sw - 5 * Gap;
  x7 = x6 + (x8 - x6) / 2;
  x5 = x6 - 4 * BigGap;
  y0 = 0;
  y1 = y0 + Roundness;
  y2 = y0 + lineHeight + Gap;
  y3 = y2 + lineHeight;
  y5 = y3 + lineHeight;
  y4 = y5 - Roundness;
  // create osd
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - y5);
  // try to use single 8bpp area
  tArea Areas[] = { { x0, y0, x10 - 1, y5 - 1, 8 } };
  if (SoppalusikkaConfig.usesinglearea && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0, y0, x10 - 1, y5 - 1, 4 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  // select mode
  if (modeonly) {
     // draw statusbar
     osd->DrawRectangle(x6, y3, x8 - 1, y5 - 1, Theme.Color(clrBackground));
     // draw rounded left corner
     osd->DrawEllipse(x6 - Roundness - 1, y3, x6 - 1, y5 - 1, Theme.Color(clrBackground), 7);
     // draw rounded right corner
     osd->DrawEllipse(x8, y3, x9 - 1, y5 - 1, Theme.Color(clrBackground), 5);
     }
  else {
     // draw titlebar
     osd->DrawRectangle(x0, y0, x5 - 1, y2 - 1, Theme.Color(clrReplayTitleFg));
     // draw rounded left corner of timebar
     osd->DrawSlope(x5, y0, x6 - 1, y2 - 1, Theme.Color(clrBackground), 0);
     // draw timebar
     osd->DrawRectangle(x6, y0, x9 - 1, y2 - 1, Theme.Color(clrBackground));
     osd->DrawText(x7 - Roundness / 2, y0, "/", Theme.Color(clrReplayCurrent), Theme.Color(clrBackground), cFont::GetFont(fontSml), Roundness, y2 - y0, taCenter);
     // draw rounded right corner of timebar
     osd->DrawEllipse(x8, y0, x9 - 1, y1 - 1, clrTransparent, -1);
     // draw progressbar
     osd->DrawRectangle(x1, y2, x9 - 1, y3 - 1, Theme.Color(clrBackground));
     // draw statusbar
     osd->DrawRectangle(x1, y3, x3 - 1, y5 - 1, Theme.Color(clrBackground));
     osd->DrawRectangle(x6, y3, x9 - 1, y5 - 1, Theme.Color(clrBackground));
     // draw rounded center area of statusbar
     osd->DrawSlope(x3, y3, x4 - 1, y5 - 1, Theme.Color(clrBackground), 1);
     osd->DrawSlope(x5, y3, x6 - 1, y5 - 1, Theme.Color(clrBackground), 3);
     // draw rounded left corner of statusbar
     osd->DrawEllipse(x1, y4, x2 - 1, y5 - 1, clrTransparent, -3);
     // draw rounded right corner of statusbar
     osd->DrawEllipse(x8, y4, x9 - 1, y5 - 1, clrTransparent, -4);
     }
}

cSkinSoppalusikkaDisplayReplay::~cSkinSoppalusikkaDisplayReplay()
{
  delete osd;
}

void cSkinSoppalusikkaDisplayReplay::SetTitle(const char *Title)
{
  // draw titlebar
  osd->DrawText(x0 + SmallGap, y0 + SmallGap, Title, Theme.Color(clrReplayTitleFg), Theme.Color(clrReplayTitleBg), cFont::GetFont(fontSml), x5 - x0 - 2 * SmallGap, y2 - y0 - 2 * SmallGap);
}

void cSkinSoppalusikkaDisplayReplay::SetMode(bool Play, bool Forward, int Speed)
{
  cBitmap *bm = NULL;
  // start drawing from right corner
  int xs = x8;
  // clamp parameters
  if (Speed < -1)
     Speed = -1;
  if (Speed > 3)
     Speed = 3;
  // check if current mode is fast forward
  if ((Speed > -1) && Play && Forward) {
     // select symbol
     switch (Speed) {
       case 0:
            bm = &bmFastForward;
            break;
       case 1:
            bm = &bmFastForward1;
            break;
       case 2:
            bm = &bmFastForward2;
            break;
       default:
       case 3:
            bm = &bmFastForward3;
            break;
       }
     // draw active symbol
     xs -= bm->Width();
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bm->Height()) / 2, *bm, Theme.Color(clrReplaySymbolActive), Theme.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= bmFastForward.Width();
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmFastForward.Height()) / 2, bmFastForward, Theme.Color(clrReplaySymbolInactive), Theme.Color(clrBackground));
     }
  // check if current mode is slow forward
  if ((Speed > -1) && !Play && Forward) {
     // select symbol
     switch (Speed) {
       case 0:
            bm = &bmSlowForward;
            break;
       case 1:
            bm = &bmSlowForward1;
            break;
       case 2:
            bm = &bmSlowForward2;
            break;
       default:
       case 3:
            bm = &bmSlowForward3;
            break;
       }
     // draw active symbol
     xs -= bm->Width() + Gap;
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bm->Height()) / 2, *bm, Theme.Color(clrReplaySymbolActive), Theme.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= bmSlowForward.Width() + Gap;
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmSlowForward.Height()) / 2, bmSlowForward, Theme.Color(clrReplaySymbolInactive), Theme.Color(clrBackground));
     }
  // check if current mode is pause
  xs -= bmPause.Width() + Gap;
  if ((Speed == -1) && !Play) {
     // draw active symbol
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmPause.Height()) / 2, bmPause, Theme.Color(clrReplaySymbolActive), Theme.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmPause.Height()) / 2, bmPause, Theme.Color(clrReplaySymbolInactive), Theme.Color(clrBackground));
     }
  // check if current mode is play
  xs -= bmPlay.Width() + Gap;
  if ((Speed == -1) && Play) {
     // draw active symbol
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmPlay.Height()) / 2, bmPlay, Theme.Color(clrReplaySymbolActive), Theme.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmPlay.Height()) / 2, bmPlay, Theme.Color(clrReplaySymbolInactive), Theme.Color(clrBackground));
     }
  // check if current mode is slow reverse
  if ((Speed > -1) && !Play && !Forward) {
     // select symbol
     switch (Speed) {
       case 0:
            bm = &bmSlowReverse;
            break;
       case 1:
            bm = &bmSlowReverse1;
            break;
       case 2:
            bm = &bmSlowReverse2;
            break;
       default:
       case 3:
            bm = &bmSlowReverse3;
            break;
       }
     // draw active symbol
     xs -= bm->Width() + Gap;
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bm->Height()) / 2, *bm, Theme.Color(clrReplaySymbolActive), Theme.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= bmSlowReverse.Width() + Gap;
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmSlowReverse.Height()) / 2, bmSlowReverse, Theme.Color(clrReplaySymbolInactive), Theme.Color(clrBackground));
     }
  // check if current mode is fast reverse
  if ((Speed > -1) && Play && !Forward) {
     // select symbol
     switch (Speed) {
       case 0:
            bm = &bmFastReverse;
            break;
       case 1:
            bm = &bmFastReverse1;
            break;
       case 2:
            bm = &bmFastReverse2;
            break;
       default:
       case 3:
            bm = &bmFastReverse3;
            break;
       }
     // draw active symbol
     xs -= bm->Width() + Gap;
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bm->Height()) / 2, *bm, Theme.Color(clrReplaySymbolActive), Theme.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= bmFastReverse.Width() + Gap;
     osd->DrawBitmap(xs, y3 + (y5 - y3 - bmFastReverse.Height()) / 2, bmFastReverse, Theme.Color(clrReplaySymbolInactive), Theme.Color(clrBackground));
     }
}

void cSkinSoppalusikkaDisplayReplay::SetProgress(int Current, int Total)
{
  // create progressbar
  cProgressBar pb(x9 - x1 - 2 * SmallGap, y3 - y2 - 2 * SmallGap, Current, Total, marks, Theme.Color(clrReplayProgressSeen), Theme.Color(clrReplayProgressRest), Theme.Color(clrReplayProgressSelected), Theme.Color(clrReplayProgressMark), Theme.Color(clrReplayProgressCurrent));
  // draw progressbar
  osd->DrawBitmap(x1 + SmallGap, y2 + SmallGap, pb);
}

void cSkinSoppalusikkaDisplayReplay::SetCurrent(const char *Current)
{
  // draw current time
  osd->DrawText(x6, y0, Current, Theme.Color(clrReplayCurrent), Theme.Color(clrBackground), cFont::GetFont(fontSml), x7 - x6 - BigGap, y2 - y0, taRight);
}

void cSkinSoppalusikkaDisplayReplay::SetTotal(const char *Total)
{
  // draw total time
  osd->DrawText(x7 + BigGap, y0, Total, Theme.Color(clrReplayTotal), Theme.Color(clrBackground), cFont::GetFont(fontSml), x8 - x7 - BigGap, y2 - y0, taLeft);
}

void cSkinSoppalusikkaDisplayReplay::SetJump(const char *Jump)
{
  // check if prompt
  if (Jump) {
     // disallow date updating
     drawdate = false;
     }
  else {
     // allow date updating
     drawdate = true;
     }
  // draw jump prompt
  osd->DrawText(x2, y3, Jump, Theme.Color(clrReplayModeJump), Theme.Color(clrBackground), cFont::GetFont(fontOsd), x3 - x2, y5 - y3, taCenter);
}

void cSkinSoppalusikkaDisplayReplay::SetMessage(eMessageType Type, const char *Text)
{
  // check if new message
  if (Text) {
     // disallow date updating
     drawdate = false;
     // save current osd
     osd->SaveRegion(x0, y3, x10 - 1, y5 - 1);
     // draw statusbar
     osd->DrawRectangle(x0, y3, x1 - 1, y5 - 1, clrTransparent);
     osd->DrawRectangle(x1, y3, x9 - 1, y5 - 1, Theme.Color(clrMessageStatusBg + 2 * Type));
     osd->DrawRectangle(x9, y3, x10 - 1, y5 - 1, clrTransparent);
     // draw rounded left corner
     osd->DrawEllipse(x1, y3, x2 - 1, y5 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 7);
     // draw message
     osd->DrawText(x2, y3, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), cFont::GetFont(fontOsd), x8 - x2, y5 - y3, taCenter);
     // draw rounded right corner
     osd->DrawEllipse(x8, y3, x9 - 1, y5 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 5);
     }
  else {
     // restore saved osd
     osd->RestoreRegion();
     // allow date updating
     drawdate = true;
     }
}

void cSkinSoppalusikkaDisplayReplay::Flush(void)
{
  // update date
  if (drawdate && !modeonly) {
     cString date = DayDateTime();
     osd->DrawText(x2, y3, date, Theme.Color(clrReplayModeJump), Theme.Color(clrBackground), cFont::GetFont(fontSml), x3 - x2, y5 - y3, taCenter);
     }
  osd->Flush();
}

// --- cSkinSoppalusikkaDisplayVolume ---------------------------------------------

class cSkinSoppalusikkaDisplayVolume : public cSkinDisplayVolume {
private:
  cOsd *osd;
  int x0, x1, x2, x3, x4, x5;
  int y0, y1, y2, y3;
  int lineHeight;
public:
  cSkinSoppalusikkaDisplayVolume();
  virtual ~cSkinSoppalusikkaDisplayVolume();
  virtual void SetVolume(int Current, int Total, bool Mute);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayVolume::cSkinSoppalusikkaDisplayVolume()
{
  lineHeight = cFont::GetFont(fontSml)->Height();
  x0 = 0;
  x1 = x0 + BigGap;
  x2 = x1 + Roundness;
  x5 = Setup.OSDWidth;
  x4 = x5 - BigGap;
  x3 = x4 - Roundness;
  y0 = 0;
  y1 = y0 + lineHeight;
  y2 = y1 + Gap;
  y3 = y2 + lineHeight;
  // create osd
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - y3);
  // try to use single 8bpp area
  tArea Areas[] = { { x0, y0, x5 - 1, y3 - 1, 8 } };
  if (SoppalusikkaConfig.usesinglearea && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0, x0, x5 - 1, y3 - 1, 4 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
}

cSkinSoppalusikkaDisplayVolume::~cSkinSoppalusikkaDisplayVolume()
{
  delete osd;
}

void cSkinSoppalusikkaDisplayVolume::SetVolume(int Current, int Total, bool Mute)
{
  tColor ColorBar;
  const char *Prompt;
  const cFont *font = cFont::GetFont(fontSml);

  // select behaviour
  if (Mute) {
     ColorBar = Theme.Color(clrVolumeBarMute);
     Prompt = tr("Mute");
     }
  else {
     ColorBar = Theme.Color(clrVolumeBarLower);
     Prompt = tr("Volume");
     }
  // width of active titlebar
  int wfg = x0 + font->Width(Prompt) + 2 * Gap;
  int wbg = wfg + 2 * Gap;
  // current volume
  int vol = x2 + (x3 - x2) * Current / Total;
  // draw titlebar
  osd->DrawRectangle(x0, y0, wbg - 1, y2 - 1, Theme.Color(clrVolumePromptFg));
  osd->DrawRectangle(wbg, y0, x4 - 1, y2 - 1, clrTransparent);
  osd->DrawText(x0 + Gap, y0 + Gap, Prompt, Theme.Color(clrVolumePromptFg), Theme.Color(clrVolumePromptBg), font, wfg, lineHeight, taCenter);
  // draw rounded left corner
  osd->DrawEllipse(x1, y2, x2 - 1, y3 - 1, Theme.Color(clrBackground), 3);
  // draw volumebar
  osd->DrawRectangle(x2, y2, x3 - 1,  y3 - 1, Theme.Color(clrBackground));
  osd->DrawRectangle(x2, y2 + Gap, vol, y3 - Gap - 1, ColorBar);
  osd->DrawRectangle(vol + 1, y2 + Gap, x3 - 1, y3 - Gap - 1, Theme.Color(clrVolumeBarUpper));
  // draw rounded right corner
  osd->DrawEllipse(x3, y2, x4 - 1, y3 - 1, Theme.Color(clrBackground), 5);
}

void cSkinSoppalusikkaDisplayVolume::Flush(void)
{
  osd->Flush();
}

// --- cSkinSoppalusikkaDisplayTracks ---------------------------------------------

class cSkinSoppalusikkaDisplayTracks : public cSkinDisplayTracks {
private:
  cOsd *osd;
  int x0, x1, x2, x3, x4, x5, x6;
  int y0, y1, y2, y3, y4, y5, y6, y7;
  int lineHeight;
  int currentIndex;
  void SetItem(const char *Text, int Index, bool Current);
public:
  cSkinSoppalusikkaDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
  virtual ~cSkinSoppalusikkaDisplayTracks();
  virtual void SetTrack(int Index, const char * const *Tracks);
  virtual void SetAudioChannel(int AudioChannel);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayTracks::cSkinSoppalusikkaDisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
  lineHeight = cFont::GetFont(fontOsd)->Height();
  currentIndex = -1;
  const cFont *font = cFont::GetFont(fontSml);
  int ItemsWidth = font->Width(Title);
  for (int i = 0; i < NumTracks; i++)
      ItemsWidth = max(ItemsWidth, font->Width(Tracks[i]));
  ItemsWidth = max(ItemsWidth, font->Width(tr("Stereo")));
  ItemsWidth = max(ItemsWidth, font->Width(tr("Left")));
  ItemsWidth = max(ItemsWidth, font->Width(tr("Right")));
  ItemsWidth += 2 * BigGap + 2 * bmAudio.Width();
  x0 = 0;
  x1 = x0 + Roundness;
  x2 = x1 + ItemsWidth;
  x3 = x2 + BigGap;
  x4 = x3 + bmAudio.Width();
  x5 = x4 + Roundness;
  x6 = Setup.OSDWidth;
  y0 = 0;
  y1 = y0 + Roundness;
  y3 = y0 + lineHeight;
  y2 = y3 - Gap;
  y4 = y3 + NumTracks * lineHeight;
  y5 = y4 + Gap;
  y7 = y5 + lineHeight;
  y6 = y7 - Roundness;
  // create osd
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - y7);
  // try to use single 8bpp area
  tArea Areas[] = { { x0, y0, x6 - 1, y7 - 1, 8 } };
  if (SoppalusikkaConfig.usesinglearea && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0, y0, x6 - 1, y7 - 1, 4 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
  // draw titlebar
  osd->DrawRectangle(x0, y0, x4 - 1, y2 - 1, Theme.Color(clrMenuTitleBg));
  osd->DrawText(x1, y0, Title, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), font, x5 - x1, y2 - y0);
  // draw rounded left corner of titlebar
  osd->DrawEllipse(x0, y0, x1 - 1, y1 - 1, clrTransparent, -2);
  // draw rounded right corner of titlebar
  osd->DrawEllipse(x4, y0, x5 - 1, y1 - 1, clrTransparent, -1);
  // draw tracks area
  osd->DrawRectangle(x0, y2, x5 - 1, y5 - 1, Theme.Color(clrBackground));
  // draw bottombar
  osd->DrawRectangle(x0, y5, x5 - 1, y7 - 1, Theme.Color(clrMenuTitleBg));
  // draw rounded left corner of titlebar
  osd->DrawEllipse(x0, y6, x1 - 1, y7 - 1, clrTransparent, -3);
  // draw rounded right corner of titlebar
  osd->DrawEllipse(x4, y6, x5 - 1, y7 - 1, clrTransparent, -4);
  // fill up audio tracks
  SetAudioChannel(cDevice::PrimaryDevice()->GetAudioChannel());
  for (int i = 0; i < NumTracks; i++)
      SetItem(Tracks[i], i, false);
}

cSkinSoppalusikkaDisplayTracks::~cSkinSoppalusikkaDisplayTracks()
{
  delete osd;
}

void cSkinSoppalusikkaDisplayTracks::SetItem(const char *Text, int Index, bool Current)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int y = y3 + Index * lineHeight;
  tColor ColorFg, ColorBg;
  if (Current) {
     ColorFg = Theme.Color(clrMenuItemCurrentFg);
     ColorBg = Theme.Color(clrMenuItemCurrentBg);
     currentIndex = Index;
     }
  else {
     ColorFg = Theme.Color(clrMenuItemSelectable);
     ColorBg = Theme.Color(clrBackground);
     }
  // draw track id
  osd->DrawText(x1, y, Text, ColorFg, ColorBg, font, x4 - x1, lineHeight);
  // draw rounded left corner
  osd->DrawEllipse(x0 + SmallGap, y, x1 - 1, y + lineHeight - 1, ColorBg, 7);
  // draw rounded right corner
  osd->DrawEllipse(x4, y, x5 - SmallGap - 1, y + lineHeight - 1, ColorBg, 5);
}

void cSkinSoppalusikkaDisplayTracks::SetAudioChannel(int AudioChannel)
{
  const cFont *font = cFont::GetFont(fontSml);
  switch (AudioChannel) {
    case 0:
         /* stereo */
         osd->DrawBitmap(x3, y0 + (y2 - bmAudio.Height()) / 2, bmAudio, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg));
         osd->DrawText(x1, y5, tr("Stereo"), Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), font, x4 - x1, y7 - y5, taCenter);
         break;
    case 1:
         /* mono left */
         osd->DrawBitmap(x3, y0 + (y2 - bmAudio.Height()) / 2, bmAudioLeft, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg));
         osd->DrawText(x1, y5, tr("Left"), Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), font, x4 - x1, y7 - y5, taCenter);
         break;
    case 2:
         /* mono right */
         osd->DrawBitmap(x3, y0 + (y2 - bmAudio.Height()) / 2, bmAudioRight, Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg));
         osd->DrawText(x1, y5, tr("Right"), Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg), font, x4 - x1, y7 - y5, taCenter);
         break;
    }
}

void cSkinSoppalusikkaDisplayTracks::SetTrack(int Index, const char * const *Tracks)
{
  if (currentIndex >= 0)
     SetItem(Tracks[currentIndex], currentIndex, false);
  SetItem(Tracks[Index], Index, true);
}

void cSkinSoppalusikkaDisplayTracks::Flush(void)
{
  osd->Flush();
}

// --- cSkinSoppalusikkaDisplayMessage --------------------------------------------

class cSkinSoppalusikkaDisplayMessage : public cSkinDisplayMessage {
private:
  cOsd *osd;
  int x0, x1, x2, x3;
  int y0, y1;
  int lineHeight;
public:
  cSkinSoppalusikkaDisplayMessage();
  virtual ~cSkinSoppalusikkaDisplayMessage();
  virtual void SetMessage(eMessageType Type, const char *Text);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayMessage::cSkinSoppalusikkaDisplayMessage()
{
  lineHeight = cFont::GetFont(fontOsd)->Height();
  x0 = 0;
  x1 = x0 + Roundness;
  x3 = Setup.OSDWidth;
  x2 = x3 - Roundness;
  y0 = 0;
  y1 = y0 + lineHeight;
  // create osd
  osd = cOsdProvider::NewOsd(Setup.OSDLeft, Setup.OSDTop + Setup.OSDHeight - y1);
  // try to use single 8bpp area
  tArea Areas[] = { { x0, y0, x3 - 1, y1 - 1, 8 } };
  if (SoppalusikkaConfig.usesinglearea && osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0, y0, x3 - 1, y1 - 1, 2 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osd->DrawRectangle(0, 0, osd->Width(), osd->Height(), clrTransparent);
}

cSkinSoppalusikkaDisplayMessage::~cSkinSoppalusikkaDisplayMessage()
{
  delete osd;
}

void cSkinSoppalusikkaDisplayMessage::SetMessage(eMessageType Type, const char *Text)
{
  // draw rounded left corner
  osd->DrawEllipse(x0, y0, x1 - 1, y1 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 7);
  // draw centered message text
  osd->DrawText(x1, y0, Text, Theme.Color(clrMessageStatusFg + 2 * Type), Theme.Color(clrMessageStatusBg + 2 * Type), cFont::GetFont(fontOsd), x2 - x1, y1 - y0, taCenter);
  // draw rounded right corner
  osd->DrawEllipse(x2, y0, x3 - 1, y1 - 1, Theme.Color(clrMessageStatusBg + 2 * Type), 5);
}

void cSkinSoppalusikkaDisplayMessage::Flush(void)
{
  osd->Flush();
}

// --- cSkinSoppalusikka ----------------------------------------------------------

cSkinSoppalusikka::cSkinSoppalusikka()
:cSkin("soppalusikka", &::Theme)
{
}

const char *cSkinSoppalusikka::Description(void)
{
  return tr("Soppalusikka");
}

cSkinDisplayChannel *cSkinSoppalusikka::DisplayChannel(bool WithInfo)
{
  return new cSkinSoppalusikkaDisplayChannel(WithInfo);
}

cSkinDisplayMenu *cSkinSoppalusikka::DisplayMenu(void)
{
  return new cSkinSoppalusikkaDisplayMenu;
}

cSkinDisplayReplay *cSkinSoppalusikka::DisplayReplay(bool ModeOnly)
{
  return new cSkinSoppalusikkaDisplayReplay(ModeOnly);
}

cSkinDisplayVolume *cSkinSoppalusikka::DisplayVolume(void)
{
  return new cSkinSoppalusikkaDisplayVolume;
}

cSkinDisplayTracks *cSkinSoppalusikka::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
  return new cSkinSoppalusikkaDisplayTracks(Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cSkinSoppalusikka::DisplayMessage(void)
{
  return new cSkinSoppalusikkaDisplayMessage;
}

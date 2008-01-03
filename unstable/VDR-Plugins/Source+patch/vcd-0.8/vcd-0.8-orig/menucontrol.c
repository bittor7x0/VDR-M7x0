/*
 * menucontrol.c: Menu control of VCD replay
 *
 * VCD Player plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 f the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 */


#include <stdio.h>
#include <typeinfo>

#include <vdr/interface.h>
#include <vdr/status.h>
#include "i18n.h"
#include "menu.h"
#include "functions.h"
#include "setup.h"
#include "menucontrol.h"


#define MODETIMEOUT       3 // seconds


// --- cMenuSpiControl -------------------------------------------------------

int cMenuSpiControl::spi = 0;
cVcd *cMenuSpiControl::vcd = 0;

cMenuSpiControl::cMenuSpiControl(void)
: cVcdViewerControl(spi, vcd)
{
  displayReplay = NULL;
  visible = modeOnly = false;
}

cMenuSpiControl::~cMenuSpiControl()
{
  cStatus::MsgReplaying(this, NULL, NULL, true);
  Stop();
}

void cMenuSpiControl::SetItem(int Spi, cVcd *Vcd)
{
  cControl *control = cControl::Control();
  if (control && typeid(*control)==typeid(cMenuVcdControl))
     static_cast<cMenuVcdControl*>(control)->Stop();
  if (control && typeid(*control)==typeid(cMenuSpiControl)
        && strcmp(Vcd->DeviceName(),static_cast<cMenuSpiControl*>(control)->DeviceName())==0) {
     static_cast<cMenuSpiControl*>(control)->GotoItem(Spi);
     return;
     }

  spi = Spi;
  vcd = Vcd;
  cControl::Launch(new cMenuSpiControl);
}

void cMenuSpiControl::Hide(void)
{
  if (visible) {
     delete displayReplay;
#if APIVERSNUM >= 10500
     SetNeedsFastResponse(false);
#else
     needsFastResponse = false;
#endif
     visible = false;
     modeOnly = false;
     }
}

eOSState cMenuSpiControl::ProcessKey(eKeys Key)
{
  if (!Active())
     return osEnd;
  switch (Key) {
     // Positioning:
     case k4:       SkipItems(-1); break;
     case k5:       ToggleStillRes(); break;
     case k6:       SkipItems(1); break;
     case kBlue:    Hide();
                    Stop();
                    vcd->Close();
                    return osEnd;
     default: {
        switch (Key) {
           // Editing: (?)
           default: {
              switch (Key) {
                 // Menu control:
                 case kOk:   break;
                 case kBack: Hide();
                             Stop();
                             vcd->Close();
                             return osEnd;
                 default:    return osUnknown;
                 }
              }
           }
        }
     }
  return osContinue;
}


// --- cMenuVcdControl ------------------------------------------------------

int cMenuVcdControl::track = 0;
cVcd *cMenuVcdControl::vcd = 0;
char *cMenuVcdControl::title = 0;

cMenuVcdControl::cMenuVcdControl(void)
: cVcdPlayerControl(track, vcd)
{
  displayReplay = NULL;
  visible = modeOnly = shown = displayFrames = false;
  lastCurrent = lastTotal = -1;
  lastPlay = lastForward = false;
  lastSpeed = -1;
  timeoutShow = 0;
  timeSearchActive = false;
  cStatus::MsgReplaying(this, "VCD", NULL, true);
}

cMenuVcdControl::~cMenuVcdControl()
{
  Hide();
  cStatus::MsgReplaying(this, NULL, NULL, false);
  Stop();
}

void cMenuVcdControl::SetTrack(int Track, cVcd *Vcd)
{
  cControl *control = cControl::Control();
  if (control && typeid(*control)==typeid(cMenuSpiControl))
     static_cast<cMenuSpiControl*>(control)->Stop();
  if (control && typeid(*control)==typeid(cMenuVcdControl)
        && strcmp(Vcd->DeviceName(),static_cast<cMenuVcdControl*>(control)->DeviceName())==0) {
     static_cast<cMenuVcdControl*>(control)->GotoTrack(Track);
     return;
     }

  free(title);
  track = Track;
  vcd = Vcd;
  title = NULL;
  cControl::Launch(new cMenuVcdControl);
}

const int cMenuVcdControl::LastReplayed(void)
{
  return track;
}

void cMenuVcdControl::ShowTimed(int Seconds)
{
  if (modeOnly)
     Hide();
  if (!visible) {
     shown = ShowProgress(true);
     timeoutShow = (shown && Seconds > 0) ? time(NULL) + Seconds : 0;
     }
}

void cMenuVcdControl::Show(void)
{
  ShowTimed();
}

void cMenuVcdControl::Hide(void)
{
  if (visible) {
     delete displayReplay;
#if APIVERSNUM >= 10500
     SetNeedsFastResponse(false);
#else
     needsFastResponse = false;
#endif
     visible = false;
     modeOnly = false;
     lastPlay = lastForward = false;
     lastSpeed = -1;
     timeSearchActive = false;
     }
}

void cMenuVcdControl::ShowMode(void)
{
  if (visible || Setup.ShowReplayMode && !cOsd::IsOpen()) {
     bool Play, Forward;
     int Speed;
     if (GetReplayMode(Play, Forward, Speed) && (!visible || Play != lastPlay || Forward != lastForward || Speed != lastSpeed)) {
        bool NormalPlay = (Play && Speed == -1);

        if (!visible) {
           if (NormalPlay)
              return; // no need to do indicate ">" unless there was a different mode displayed before
           visible = modeOnly = true;
           displayReplay = Skins.Current()->DisplayReplay(modeOnly);
           }

        if (modeOnly && !timeoutShow && NormalPlay)
           timeoutShow = time(NULL) + MODETIMEOUT;
        displayReplay->SetMode(Play, Forward, Speed);
        lastPlay = Play;
        lastForward = Forward;
        lastSpeed = Speed;
        }
     }
}

bool cMenuVcdControl::ShowProgress(bool Initial)
{
  int Current, Total;

  if (GetIndex(Current, Total) && Total > 0) {
     if (!visible) {
        displayReplay = Skins.Current()->DisplayReplay(modeOnly);
#if APIVERSNUM >= 10500
        SetNeedsFastResponse(false);
#else
        needsFastResponse = false;
#endif
	visible = true;
        }
     if (Initial) {
        lastCurrent = lastTotal = -1;
        }
     if (title) 
        displayReplay->SetTitle(title);
     if (Total != lastTotal) {
        displayReplay->SetTotal(IndexToHMSF(Total));
        if (!Initial)
           displayReplay->Flush();
        }
     if (Current != lastCurrent || Total != lastTotal) {
        displayReplay->SetProgress(Current, Total);
        if (!Initial)
           displayReplay->Flush();
        displayReplay->SetCurrent(IndexToHMSF(Current, displayFrames));
        displayReplay->Flush();
        lastCurrent = Current;
        }
     lastTotal = Total;
     ShowMode();
     return true;
     }
  return false;
}

void cMenuVcdControl::TimeSearchDisplay(void)
{
  char buf[64];
  strcpy(buf, tr("Jump: "));
  int len = strlen(buf);
  char h10 = '0' + (timeSearchTime >> 24);
  char h1  = '0' + ((timeSearchTime & 0x00FF0000) >> 16);
  char m10 = '0' + ((timeSearchTime & 0x0000FF00) >> 8);
  char m1  = '0' + (timeSearchTime & 0x000000FF);
  char ch10 = timeSearchPos > 3 ? h10 : '-';
  char ch1  = timeSearchPos > 2 ? h1  : '-';
  char cm10 = timeSearchPos > 1 ? m10 : '-';
  char cm1  = timeSearchPos > 0 ? m1  : '-';
  sprintf(buf + len, "%c%c:%c%c", ch10, ch1, cm10, cm1);
  displayReplay->SetJump(buf);
}

void cMenuVcdControl::TimeSearchProcess(eKeys Key)
{
#define STAY_SECONDS_OFF_END 10
  int Seconds = (timeSearchTime >> 24) * 36000 + ((timeSearchTime & 0x00FF0000) >> 16) * 3600 + ((timeSearchTime & 0x0000FF00) >> 8) * 600 + (timeSearchTime & 0x000000FF) * 60;
  int Current = (lastCurrent / FRAMESPERSEC);
  int Total = (lastTotal / FRAMESPERSEC);
  switch (Key) {
    case k0 ... k9:
         if (timeSearchPos < 4) {
            timeSearchTime <<= 8;
            timeSearchTime |= Key - k0;
            timeSearchPos++;
            TimeSearchDisplay();
            }
         break;
    case kFastRew:
    case kLeft:
    case kFastFwd:
    case kRight: {
         int dir = ((Key == kRight || Key == kFastFwd) ? 1 : -1);
         if (dir > 0)
            Seconds = min(Total - Current - STAY_SECONDS_OFF_END, Seconds);
         SkipSeconds(Seconds * dir);
         timeSearchActive = false;
         }
         break;
    case kPlay:
    case kUp:
    case kPause:
    case kDown:
         Seconds = min(Total - STAY_SECONDS_OFF_END, Seconds);
         Goto(Seconds, Key == kDown || Key == kPause);
         timeSearchActive = false;
         break;
    default:
         timeSearchActive = false;
         break;
    }
  if (!timeSearchActive) {
     if (timeSearchHide)
        Hide();
     else
        displayReplay->SetJump(NULL);
     ShowMode();
     }
}

void cMenuVcdControl::TimeSearch(void)
{
  timeSearchTime = timeSearchPos = 0;
  timeSearchHide = false;
  if (modeOnly)
     Hide();
  if (!visible) {
     Show();
     if (visible)
        timeSearchHide = true;
     else
        return;
     }
  timeoutShow = 0;
  TimeSearchDisplay();
  timeSearchActive = true;
}

eOSState cMenuVcdControl::ProcessKey(eKeys Key)
{
  if (!Active())
     return osEnd;
  if (visible) {
     if (timeoutShow && time(0) > timeoutShow) {
        Hide();
        ShowMode();
        timeoutShow = 0;
        }
     else if (modeOnly)
        ShowMode();
     else
        shown = ShowProgress(!shown) || shown;
     }
  bool DisplayedFrames = displayFrames;
  displayFrames = false;
  if (timeSearchActive && Key != kNone) {
     TimeSearchProcess(Key);
     return osContinue;
     }
  switch (Key) {
     case k1: SkipEntries(-1); break;
     case k3: SkipEntries(1); break;
     case k4: SkipTracks(-1); break;
     case k5: SkipTracks(0); break;
     case k6: SkipTracks(1); break;
     default: break;
     }
  asprintf(&title, "VideoCD Track %02d", GetTrack());
  bool DoShowMode = true;
  switch (Key) {
     // Positioning:
     case kPlay:
     case kUp:      Play(); break;
     case kPause:
     case kDown:    Pause(); break;
     case kFastRew|k_Release:
     case kLeft|k_Release:
                    if (Setup.MultiSpeedMode) break;
     case kFastRew:
     case kLeft:    Backward(); break;
     case kFastFwd|k_Release:
     case kRight|k_Release:
                    if (Setup.MultiSpeedMode) break;
     case kFastFwd:
     case kRight:   Forward(); break;
     case kRed:     TimeSearch(); break;
     case kGreen|k_Repeat:
     case kGreen:   SkipSeconds(-60); break;
     case kYellow|k_Repeat:
     case kYellow:  SkipSeconds(60); break;
     case kStop:
     case kBlue:    Hide();
                    Stop();
                    vcd->Close();
                    return osEnd;
     default: {
        DoShowMode = false;
        switch (Key) {
           // Editing: (?)

           default: {
              displayFrames = DisplayedFrames;
              switch (Key) {
                 // Menu control:
                 case kOk:   if (visible && !modeOnly) {
                                Hide();
                                DoShowMode = true;
                                }
                             else
                                Show();
                             break;
                 case kBack: Hide();
                             Stop();
                             vcd->Close();
                             return osEnd;
                 default:    return osUnknown;
                 }
              }
           }
        }
     }
  if (DoShowMode)
     ShowMode();
  return osContinue;
}


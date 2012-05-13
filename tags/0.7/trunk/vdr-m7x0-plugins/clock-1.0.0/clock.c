/*
 * clock.c: A plugin for the Video Disk Recorder
 *
 * Clock Plugin for VDR. See http://vdr.aistleitner.info for download.
 * Copyright (C) 2007  Mario Aistleitner <vdr@aistleitner.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/font.h>
#include <vdr/remote.h>
#include <math.h>
#include "enhancedbitmap.h"
#include "i18n.h"

#include "images/clock.xpm"
#include "images/tux.xpm"
#include "images/tuxpart.xpm"

static const char *VERSION        = "1.0.0";
static const char *DESCRIPTION    = "A Simple Clock";
static const char *MAINMENUENTRY  = "Clock";

// Global variables

int       TimeWidth  = Setup.OSDWidth;
int       TimeHeight = Setup.OSDHeight;
int       XOffset    = 100;
int       YOffset    = 100;
int       Size       = 2;
int       Alarm      = false;
int       AlarmTime  = -1;
int       ShowSec    = true;
int       ShowAlarm  = true;
int       FullScreen = false;
int       Type       = 0;
int       SW         = 1;
int       SL         = 99;
int       MW         = 5;
int       ML         = 90;
int       HW         = 10;
int       HL         = 50;
double    SecWidth   = ((double)SW) / 100;
double    SecLength  = ((double)SL) / 100;
double    MinWidth   = ((double)MW) / 100;
double    MinLength  = ((double)ML) / 100;
double    HourWidth  = ((double)HW) / 100;
double    HourLength = ((double)HL) / 100;
int       ColorBG    = 0;
int       ColorFG    = 1;
int       ColorHour  = 1;
int       ColorMin   = 1;
int       ColorSec   = 3;
int       TeaTime    = 5;
const char      *TimeColors[]  = {"Transparent",
                            "White",
	                    "Black",
			    "Red",
			    "Green",
			    "Yellow",
			    "Blue",
                           };
const char      *ClockType[] = {"Digital",
			  "Analog",
			  "Analog modern",
			  "MorphOS",
			  "TuxClock",
			  "Internet time",
			  "Tea clock",
                         };



// --- cOSDClock -------------------------------------------------------------

class cOSDClock : public cOsdObject,  public cThread {
private:
  cOsd *osd;
  eKeys LastKey;
  bool shutdown;
  tColor ColorH, ColorM, ColorS, ColorForeGround, ColorBackGround;
  int r;
  void DrawArrow(double width, double len, tColor col, double ang, cEnhancedBitmap &img);
  void DrawScale(int r, tColor col, cEnhancedBitmap &Analog);
  void DrawModernSec(int r, int sec, tColor col, cEnhancedBitmap &Analog);
  void DrawPoint(double x, double y, int r, tColor col, cEnhancedBitmap &Analog);
  tColor getColor(int color);
  static cBitmap bmClock;
  static cBitmap bmTux;
  static cBitmap bmTuxpart;
protected:
  virtual void Action(void);
public:
  cOSDClock(void);
  virtual ~cOSDClock(void);
  void Show(void) {Start(); }
  eOSState ProcessKey(eKeys Key);
};


cBitmap cOSDClock::bmClock(clock_xpm);
cBitmap cOSDClock::bmTux(tux_xpm);
cBitmap cOSDClock::bmTuxpart(tuxpart_xpm);

cOSDClock::cOSDClock()
:cOsdObject(true), cThread()
{
  osd = NULL;
  LastKey = kNone;
  if (Type == 3) Size = 3;
  if (Type ==4) {
    r = 40;
    XOffset = Setup.OSDWidth - 144;
    }
  else r = Size*16;
}

cOSDClock::~cOSDClock()
{
  if (Running())
     Cancel(3);

  while (!shutdown) {
     if (AlarmTime == -1)
        Alarm = false;
        usleep(100000);
     }

  if (osd)
     delete osd;

  cRemote::Put(LastKey);
}

void cOSDClock::DrawArrow(double width, double len, tColor col, double ang, cEnhancedBitmap &img)
{
  double s=sin(ang);
  double c=cos(ang);
  img.FillTriangle((int)(r-r*width*c),(int)(r-r*width*s),(int)(r+r*width*c),(int)(r+r*width*s),(int)(r+r*len*s),(int)(r-r*len*c),col);
  img.FillTriangle((int)(r-r*width*c),(int)(r-r*width*s),(int)(r+r*width*c),(int)(r+r*width*s),(int)(r-r*width*s),(int)(r+r*width*c),col);
}

void cOSDClock::DrawPoint(double x, double y, int r, tColor col, cEnhancedBitmap &Analog)
{
  double ang = 2*3.1415/20;
  for (int i = 0; i < 20; i++)
    {
      double s=sin(ang * i);
      double c=cos(ang * i);
      Analog.DrawLine((int)x, (int)y, (int)(x + r*0.02*s), (int)(y + r*0.02*c), col);
    }
}


void cOSDClock::DrawScale(int r, tColor col, cEnhancedBitmap &Analog)
{
  double ang = 2*3.1415*5/60;
  for (int i = 0; i < 12; i++)
    {
      double s=sin(ang * i);
      double c=cos(ang * i);
      DrawPoint(r+r*s, r+r*c, r, col, Analog);
     if (i % 3 == 0)
	{
	  for (int j = 0; j < 10; j++)
	    {
	      DrawPoint(r+r*s-0.1*r*s*j/10, r+r*c-0.1*r*c*j/10, r, col, Analog);
	    }
	}
    }
}


void cOSDClock::DrawModernSec(int r, int sec, tColor col, cEnhancedBitmap &Analog)
{

  double ang = 2*3.1415/60;
  for (int i = 45; i < sec+46; i++)
    {
      double c=sin(ang * i);
      double s=cos(ang * i);
      DrawPoint(r+r*s, r+r*c, r, col, Analog);;
    }
}

tColor cOSDClock::getColor(int color)
{
  switch (color)
    {
    case 0  : return clrTransparent;
    case 1  : return clrWhite;
    case 2  : return clrBlack;
    case 3  : return clrRed;
    case 4  : return clrGreen;
    case 5  : return clrYellow;
    case 6  : return clrBlue;
    default : return clrBlack;
    }
  return clrBlack;
}

void cOSDClock::Action(void)
{
  int ITime, Min, Hour, Sec, ActualTime, TimeLeft = 0;
  int SaveSec = -1;
  time_t Result;
  char TimeString[250];
  double SecAng, SecAng2,MinAng,HourAng;
  struct tm *CurTime;
  shutdown = false;
  const cFont *font = cFont::GetFont(fontOsd);

  if (FullScreen)
      r = 12 * (TimeHeight-1);
  int ClockSize = 2 * r + 1;
  int ClockHeight = ClockSize;
  ClockSize = int(ClockSize / 4)*4;

  cEnhancedBitmap Analog(ClockSize+1,ClockSize+1,6);
  cEnhancedBitmap AnalogHigh(ClockSize+1,ClockSize+1,8);
  cEnhancedBitmap TuxMap(154, 184 ,8);
  cEnhancedBitmap Tea(5, 5, 6);

  ColorBackGround = getColor(ColorBG);
  ColorForeGround = getColor(ColorFG);
  ColorH          = getColor(ColorHour);
  ColorM          = getColor(ColorHour);
  ColorS          = getColor(ColorSec);

  Analog.SetColor(0,ColorBackGround);
  Analog.SetColor(1,ColorForeGround);
  Analog.SetColor(2,ColorH);
  Analog.SetColor(3,ColorM);
  Analog.SetColor(4,ColorS);

  if (Type == 0 || Type > 4 ){
     ClockHeight = font->Height() +4;
     ClockSize = font->Width("XX:XX:XX")+4;
     }
  if (Type == 4) {
     ClockHeight = 178;
     ClockSize = 148;
     Analog.SetSize(80, 80);
     }

  if (osd) delete osd;
  osd = cOsdProvider::NewOsd(XOffset, YOffset);
  tArea Area = { 0, 0, ClockSize+3, ClockHeight+5, 8 };
  osd->SetAreas(&Area, 1);

  while (Running())
    {
      Result = time(NULL);
      CurTime = localtime(&Result);
      Hour = CurTime->tm_hour;
      Min = CurTime->tm_min;
      Sec = CurTime->tm_sec;
      if (Sec != SaveSec){
      if (Type == 1)
	{
	  SecAng=2*3.1415*Sec/60;
	  SecAng2=2*3.1415*(Sec-1)/60;
	  MinAng=2*3.1415*((double)Min+((double)Sec)/60)/60;
	  HourAng=2*3.1415*((double)Hour+((double)Min+((double)Sec)/60)/60)/12;
	  Analog.DrawLine(0,0,1,1, ColorBackGround);
	  Analog.DrawRectangle(0, 0, ClockSize-1, ClockHeight-1, ColorBackGround);
	  Analog.DrawEllipse(0, 0, ClockSize, ClockSize, clrGray50);
	  DrawScale(r, ColorForeGround, Analog);
	  osd->DrawBitmap(0, 0, Analog);

	  DrawArrow(HourWidth,HourLength,ColorH,HourAng,Analog);
	  DrawArrow(MinWidth,MinLength,ColorM,MinAng,Analog);
	  if (ShowSec)
	    DrawArrow(SecWidth,SecLength,ColorS,SecAng,Analog);
	  if (FullScreen)
	    osd->DrawBitmap(0, 0, Analog);
	  else
	    osd->DrawBitmap(0, 0, Analog);
	  if (ShowSec)
	    {
	      DrawArrow(SecWidth,SecLength,ColorBackGround,SecAng,Analog);
	      DrawArrow(SecWidth,SecLength,ColorBackGround,SecAng2,Analog);
	    }
	  DrawArrow(MinWidth,MinLength,ColorBackGround,MinAng,Analog);
	  DrawArrow(HourWidth,HourLength,ColorBackGround,HourAng,Analog);
	  osd->Flush();
	  Analog.Clean();
	}
       if (Type == 2)
	{
	  SecAng=2*3.1415*Sec/60;
	  SecAng2=2*3.1415*(Sec-1)/60;
	  MinAng=2*3.1415*((double)Min+((double)Sec)/60)/60;
	  HourAng=2*3.1415*((double)Hour+((double)Min+((double)Sec)/60)/60)/12;
	  Analog.DrawRectangle(0, 0, ClockSize, ClockHeight, 0x00000000);
	  Analog.DrawEllipse(0, 0, ClockSize, ClockHeight, clrGray50);
	  DrawScale(r, ColorForeGround, Analog);
	  DrawArrow(HourWidth*0.7,HourLength,ColorH,HourAng,Analog);
	  DrawArrow(MinWidth*0.7,MinLength,ColorM,MinAng,Analog);
	  if (ShowSec)
	    DrawModernSec(r, Sec, clrYellow, Analog);
          if (FullScreen)
	    osd->DrawBitmap(0, 0, Analog);
	  else
	    osd->DrawBitmap(0, 0, Analog);
	  DrawArrow(MinWidth*0.7,MinLength,ColorBackGround,MinAng,Analog);
	  DrawArrow(HourWidth*0.7,HourLength,ColorBackGround,HourAng,Analog);
	  if (ShowSec)
	    DrawModernSec(r, Sec, ColorBackGround, Analog);
	  osd->Flush();
	  Analog.Clean();
	}
      else if (Type == 3)
	{
	  SecAng=2*3.1415*Sec/60;
	  SecAng2=2*3.1415*(Sec-1)/60;
	  MinAng=2*3.1415*((double)Min+((double)Sec)/60)/60;
	  HourAng=2*3.1415*((double)Hour+((double)Min+((double)Sec)/60)/60)/12;
	  AnalogHigh.DrawRectangle(0, 0, ClockSize-1, ClockHeight-1, 0x00000000);
	  bmClock.SetColor(0, 0x00000000);
	  AnalogHigh.DrawBitmap(9, 10, bmClock);
	  osd->DrawBitmap(0, 0, AnalogHigh);
	  DrawArrow(HourWidth*0.7,HourLength*0.8,ColorH,HourAng,AnalogHigh);
	  DrawArrow(MinWidth*0.8,MinLength*0.7,ColorM,MinAng,AnalogHigh);
	  if (ShowSec)
	    DrawArrow(SecWidth,SecLength*0.7,ColorS,SecAng,AnalogHigh);
	  if (FullScreen)
	    osd->DrawBitmap(0, 0, AnalogHigh);
	  else
	    osd->DrawBitmap(0, 0, AnalogHigh);
	  if (ShowSec)
	    {
	      DrawArrow(SecWidth,SecLength*0.7,ColorBackGround,SecAng,AnalogHigh);
	      DrawArrow(SecWidth,SecLength*0.7,ColorBackGround,SecAng2,AnalogHigh);
	    }
	  DrawArrow(MinWidth*0.8,MinLength*0.7,ColorBackGround,MinAng,AnalogHigh);
	  DrawArrow(HourWidth*0.7,HourLength*0.8,ColorBackGround,HourAng,AnalogHigh);
	  osd->Flush();
	  AnalogHigh.Clean();
	}
      else if (Type == 4)
	{
	  SecAng=2*3.1415*Sec/60;
	  SecAng2=2*3.1415*(Sec-1)/60;
	  MinAng=2*3.1415*((double)Min+((double)Sec)/60)/60;
	  HourAng=2*3.1415*((double)Hour+((double)Min+((double)Sec)/60)/60)/12;
	  bmTux.SetColor(12, 0x00000000);
	  TuxMap.DrawBitmap(0, 0, bmTux);
	  Analog.DrawBitmap(1, 0, bmTuxpart);
	  DrawScale(r, clrRed, Analog);
	  //osd->DrawBitmap(0, 0, TuxMap);
	  DrawArrow(HourWidth,HourLength,clrBlack,HourAng,Analog);
	  DrawArrow(MinWidth,MinLength,clrBlack,MinAng,Analog);
	  DrawArrow(SecWidth,SecLength,clrRed,SecAng,Analog);
	  TuxMap.DrawBitmap(38, 83, Analog);
          osd->DrawBitmap(0, 0, TuxMap);
	  osd->Flush();
	  TuxMap.Clean();
	  Analog.Clean();
	}
      else if (Type == 0)
	{
	  if (ShowSec)
	    sprintf(TimeString, " %02d:%02d:%02d ", Hour, Min, Sec);
	  else
	    sprintf(TimeString, " %02d:%02d ", Hour, Min);
	  osd->DrawRectangle(0, 0, ClockSize-1, ClockHeight-1, ColorBackGround);
	  osd->DrawText(0, 0, TimeString, ColorForeGround, ColorBackGround, font, ClockSize-1, font->Height(),taCenter);
	  osd->Flush();
	}
      else if (Type == 5)
	{
	  ITime = (int)((Hour * 3600 + Min * 60 + Sec)/86.4);
	  sprintf(TimeString, " @%04d ", ITime);
	  osd->DrawRectangle(0, 0, ClockSize-1, font->Height(), ColorBackGround);
	  osd->DrawText(0, 0, TimeString, ColorForeGround, ColorBackGround, font, ClockSize-1, font->Height(),taCenter);
	  osd->Flush();
	}
      else if (Type == 6)
	{
	  if (Alarm)
	    {
	      AlarmTime = -1;
	      osd->DrawRectangle(0, 0, ClockSize-1, font->Height(), ColorBackGround);
	      osd->DrawText(0, 0 , tr("It's tea time"), ColorForeGround, ColorBackGround, font, ClockSize-1, font->Height(),taCenter);
	      osd->Flush();
	      Tea.Clean();
	    }
	  else
	    {
	      ActualTime = Hour * 3600 + Min * 60 + Sec;
	      if (AlarmTime == -1)
		AlarmTime = (ActualTime + TeaTime * 60);
	      if (TimeLeft > 4000)
		AlarmTime = AlarmTime - 86400;
	      TimeLeft = AlarmTime - ActualTime;
	      if (TimeLeft <= 0)
		{
		  Alarm = true;
		  Min = 0;
		  Sec = 0;
		}
	      else
		{
		  Min = TimeLeft / 60;
		  Sec = TimeLeft % 60;
		}
	      sprintf(TimeString, " %02d:%02d ", Min, Sec);
	     if (ShowAlarm){
	      osd->DrawRectangle(0, 0, ClockSize-1, font->Height(), ColorBackGround);
	      osd->DrawText(0, 0, TimeString, ColorForeGround, ColorBackGround, font, ClockSize-1, font->Height(),taCenter);
	      osd->Flush();
	      }
	    }
	}
      }
      SaveSec = Sec;
      usleep(250000);
    } // while Running()
  shutdown = true;
}

eOSState cOSDClock::ProcessKey(eKeys Key)
{
  if (Key != kNone) {
     if (AlarmTime == -1)
        Alarm = false;
     if (Key != kBack)
        LastKey = Key;
     if (Key != kChanUp && Key != kChanDn && Key != kVolUp && Key != kVolDn && Key != kMute)
        return (osEnd);
     }
  return (osContinue);
}


// --- cMenuSetupClock -------------------------------------------------------

class cMenuSetupClock : public cMenuSetupPage {
private:
  char Color[16][32];
  int newSize;
  int newShowSec;
  int newShowAlarm;
  int newFullScreen;
  int newType;
  int newHW;
  int newHL;
  int newMW;
  int newML;
  int newSW;
  int newSL;
  int newColorBG;
  int newColorFG;
  int newColorHour;
  int newColorMin;
  int newColorSec;
  int newXOffset;
  int newYOffset;
  int newTeaTime;
  int x;
  int y;
protected:
  virtual void Store(void);
public:
  cMenuSetupClock(void);
  };

cMenuSetupClock::cMenuSetupClock(void)
{
  cSetup data;
  newSize       = Size;
  newShowSec    = ShowSec;
  newShowAlarm  = ShowAlarm;
  newFullScreen = FullScreen;
  newType       = Type;
  newHW         = HW;
  newHL         = HL;
  newMW         = MW;
  newML         = ML;
  newSW         = SW;
  newSL         = SL;
  newColorBG    = ColorBG;
  newColorFG    = ColorFG;
  newColorHour  = ColorHour;
  newColorMin   = ColorMin;
  newColorSec   = ColorSec;
  newXOffset    = XOffset;
  newYOffset    = YOffset;
  newTeaTime    = TeaTime;

  if (!Type)
    {
      y = 2;
      if (ShowSec)
	x = 8;
      else
	x = 6;
    }
  else
    {
      x = 3 * Size;
      y = 0;

    }

  int current = Current();
  Add(new cMenuEditIntItem(  tr("x Position"),                   &newXOffset,    0, Setup.OSDWidth - x));
  Add(new cMenuEditIntItem(  tr("y Position"),                   &newYOffset,    0, Setup.OSDHeight - y));
  Add(new cMenuEditStraItem( tr("Type of the clock"),            &newType,       7, ClockType));
  if (newType != 3){
  Add(new cMenuEditStraItem( tr("Background color"),         &newColorBG,    7, TimeColors));
  Add(new cMenuEditStraItem( tr("Foreground color"),         &newColorFG,    7, TimeColors));
  }
  if (newType == 0)
    {
      Add(new cMenuEditBoolItem( tr("Show Seconds"),             &newShowSec              ));
    }
  else if (newType == 1 || newType == 2)
    {
      Add(new cMenuEditBoolItem( tr("Show Seconds"),             &newShowSec              ));
      Add(new cMenuEditBoolItem( tr("FullScreen"),               &newFullScreen           ));
      Add(new cMenuEditIntItem(  tr("Analog clock size"),        &newSize,       0,15     ));
      Add(new cMenuEditStraItem( tr("Arrow color"),              &newColorHour,  7, TimeColors));
      Add(new cMenuEditIntItem(  tr("Length of hour arrow [%]"), &newHL,         0, 100   ));
      Add(new cMenuEditIntItem(  tr("Width of hour arrow [%]"),  &newHW,         0, 100   ));
      Add(new cMenuEditIntItem(  tr("Length of min arrow [%]"),  &newML,         0, 100   ));
      Add(new cMenuEditIntItem(  tr("Width of min arrow [%]"),   &newMW,         0, 100   ));
      if (newType == 1){
        Add(new cMenuEditStraItem( tr("Seconds arrow color"),      &newColorSec,   7, TimeColors));
        Add(new cMenuEditIntItem(  tr("Length of sec arrow [%]"),  &newSL,         0, 100   ));
        Add(new cMenuEditIntItem(  tr("Width of sec arrow [%]"),   &newSW,         0, 100   ));
	}
    }
  if (newType == 4)
    {
      Add(new cMenuEditIntItem( tr("Time till tea"),             &newTeaTime,    0, 60    ));
      Add(new cMenuEditBoolItem( tr("Show Countdown"),           &newShowAlarm            ));
    }
  SetCurrent(Get(current));
  Display();
}

void cMenuSetupClock::Store(void)
{
  SetupStore("XOffset",    XOffset    = newXOffset);
  SetupStore("YOffset",    YOffset    = newYOffset);
  SetupStore("ShowSec",    ShowSec    = newShowSec);
  SetupStore("ShowAlarm",  ShowAlarm  = newShowAlarm);
  SetupStore("FullScreen", FullScreen = newFullScreen);
  SetupStore("Type",       Type       = newType);
  SetupStore("ColorBG",    ColorBG    = newColorBG);
  SetupStore("ColorFG",    ColorFG    = newColorFG);
  SetupStore("ColorHour",  ColorHour  = newColorHour);
  SetupStore("ColorSec",   ColorSec   = newColorSec);
  SetupStore("ColorMin",   ColorMin   = newColorMin);
  SetupStore("Size",       Size       = newSize);
  SetupStore("HW",         HW         = newHW);
  SetupStore("HL",         HL         = newHL);
  SetupStore("MW",         MW         = newMW);
  SetupStore("ML",         ML         = newML);
  SetupStore("SW",         SW         = newSW);
  SetupStore("SL",         SL         = newSL);
  SetupStore("TeaTime",    TeaTime    = newTeaTime);
}
// --- End cMenuSetupClock


// --- cPluginClock ---------------------------------------------------

class cPluginClock : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginClock(void);
  virtual ~cPluginClock();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginClock::cPluginClock(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginClock::~cPluginClock()
{
  // Clean up after yourself!
}

const char *cPluginClock::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginClock::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginClock::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginClock::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  return true;
}

void cPluginClock::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginClock::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cOSDClock();
}

cMenuSetupPage *cPluginClock::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupClock;
}


bool cPluginClock::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.

  if      (!strcasecmp(Name, "XOffset"))          XOffset    = atoi(Value);
  else if (!strcasecmp(Name, "YOffset"))          YOffset    = atoi(Value);
  else if (!strcasecmp(Name, "ShowSec"))          ShowSec    = atoi(Value);
  else if (!strcasecmp(Name, "ShowAlarm"))        ShowAlarm  = atoi(Value);
  else if (!strcasecmp(Name, "FullScreen"))       FullScreen = atoi(Value);
  else if (!strcasecmp(Name, "Type"))             Type       = atoi(Value);
  else if (!strcasecmp(Name, "Size"))             Size       = atoi(Value);
  else if (!strcasecmp(Name, "ColorBG"))          ColorBG    = atoi(Value);
  else if (!strcasecmp(Name, "ColorFG"))          ColorFG    = atoi(Value);
  else if (!strcasecmp(Name, "ColorHour"))        ColorHour  = atoi(Value);
  else if (!strcasecmp(Name, "ColorMin"))         ColorMin   = atoi(Value);
  else if (!strcasecmp(Name, "ColorSec"))         ColorSec   = atoi(Value);
  else if (!strcasecmp(Name, "TeaTime"))          TeaTime    = atoi(Value);
  else if (!strcasecmp(Name, "HW"))               HW         = atoi(Value);
  else if (!strcasecmp(Name, "HL"))               HL         = atoi(Value);
  else if (!strcasecmp(Name, "MW"))               MW         = atoi(Value);
  else if (!strcasecmp(Name, "ML"))               ML         = atoi(Value);
  else if (!strcasecmp(Name, "SW"))               SW         = atoi(Value);
  else if (!strcasecmp(Name, "SL"))               SL         = atoi(Value);
  else
     return false;
  return true;
}

VDRPLUGINCREATOR(cPluginClock); // Don't touch this!

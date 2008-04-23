#ifndef __BROWSE_H
#define __BROWSE_H


#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/osd.h>
#include <vdr/font.h>

#include <time.h>
#include <stdlib.h>

#include "config.h"


//////////////////////////////////////////////////////////////////////////////


// Set to true if you are using North American providers, false otherwise.
#define BEV_DN true

#define SCR_WIDTH 720
#define HEIGHT 120


class cBrowse : public cOsdObject 
{
private:
  void DrawTitle(const char*);
  void Draw();

  void ShowInfoWindow(const cEvent*);
    static const tColor clrClear      = (tColor) 0x00000000;
  static const tColor green         = (tColor) 0xFF016E08;
  static const tColor lightgrey     = (tColor) 0xFFAAAAAA; 
  static const tColor clrBackground = (tColor) 0x7F000000;
  
  int screenHeight;
  int width;
  int TOP_X;
  int TOP_Y;
  bool now;
  bool infoOpen;
  int currentChannel; 
  int inputNum; 
  long inputTime;
  cOsd* osd;
  const cFont* fontSmall;
  const cFont* fontBig;

  const cEvent* event;
  

public:
  cBrowse(void);
  ~cBrowse();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
 };


//////////////////////////////////////////////////////////////////////////////


class cPluginBrowse : public cPlugin 
{
private:
  // Add any member variables or functions you may need here.
  static const char *VERSION;
  static const char *DESCRIPTION;
  static const char *MAINMENUENTRY;
public:
  cPluginBrowse(void);
  virtual ~cPluginBrowse();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *MainMenuEntry(void); 
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);};


//////////////////////////////////////////////////////////////////////////////

#define EXPSIZE 7
static const char* titleDelims[EXPSIZE] = { " Series.", " Movie.", " News.", " Education.", " Sports.", " Series/Special.", " Music/Art." };


const cChannel* GetNextChannel(const cChannel* c);
const cChannel* GetPrevChannel(const cChannel* c);


//----------------------------------------------------------------------------

void TimeToString(char* buf, const struct tm* t)
{
  if (config.timeFormat24h)
    strftime(buf, 8, "%H:%M", t);
  else
    strftime(buf, 9, "%I:%M %p", t);
    
  if (buf[0] == '0')
    for (size_t i=0; i<strlen(buf); i++)
      buf[i] = buf[i+1]; 
}


//----------------------------------------------------------------------------

void TimeToString(char* buf, const time_t t)
{
  struct tm tm_r;
  TimeToString(buf, localtime_r(&t, &tm_r));
}


//----------------------------------------------------------------------------

char* DurationToString(time_t t)
{
  int tmin = (t - (t % 60)) / 60;
  int min = tmin % 60;
  int hrs = (tmin - min) / 60;
   
  char* time;
  asprintf(&time, "%d:%02d", hrs, min);
  return time;
}


//----------------------------------------------------------------------------

const char* OsdErrorToString(eOsdError err)
{
  const char* e = "Unknown";
  switch (err)
  {
    case oeOk:              e = "Ok"; break;
    case oeTooManyAreas:    e = "Too Many Areas"; break;
    case oeTooManyColors:   e = "Too Many Colors"; break;
    case oeBppNotSupported: e = "Bpp Not Supported"; break;
    case oeAreasOverlap:    e = "Areas Overlap"; break;
    case oeWrongAlignment:  e = "Wrong Alignment"; break;
    case oeOutOfMemory:     e = "Out Of Memory"; break;
    case oeWrongAreaSize:   e = "Wrong Area Size"; break;
    case oeUnknown: break;
  }
  return e;
}


//////////////////////////////////////////////////////////////////////////////

#endif //__BROWSE_H

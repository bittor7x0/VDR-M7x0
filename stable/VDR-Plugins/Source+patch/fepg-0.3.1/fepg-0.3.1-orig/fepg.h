#ifndef __FEPG_H
#define __FEPG_H

#include <vdr/plugin.h>
#include <vdr/interface.h>

#include <stdlib.h>
#include <time.h>

#include "menu.h"


//////////////////////////////////////////////////////////////////////////////


// Set to true if you are using North American providers, false otherwise.
#define BEV_DN true

#define NAME_W 125 // Width of the cells containing channel names & numbers
#define TIME_H 40  // Height of the time cells
#define NUM_COL 3 // Number of columns
  
  
//////////////////////////////////////////////////////////////////////////////


class cFEpg : public cOsdObject 
{
private: 
  void Draw(void); // Draws the epg
  void ShowInfoWindow(const cEvent*);
  void ShowRecordDialog(const cEvent* e);
  void UpdateScreen(void); // Redraws the screen
  
  int Width(const char*);
  void Text(int, int, const char*, tColor, tColor);
  void Fill(int, int, int, int, tColor);
  void DarkenBg(void);
  
  int scrHeight, scrWidth;
  
  static const int brd = 3;
  
  // Colors
  tColor bgColor;
  tColor green;
  tColor lightgrey;

  cBitmap* bitmap; // Do all drawing on this
  int width;  // Width of display
  int height; // Height of display (not counting the time bar at the top)
  int cWidth; // Width of a cell
  
  const cChannel* currentChannel; // The first channel shown in the EPG
  int displayedChannels[MAX_ROWS]; // A list of the channels numbers currently shown 

  time_t firstTime;
  time_t currentTime;
  
  int oldMin;

  int curRow, curCol; // Currently selected Cell, horizontal and vertical values in EPG table
  
  cOsd* osd;
  const cFont* font; // The used font
  const cEvent* curEvent; // Currently selected event
  const cEvent* eventMtx[MAX_ROWS][NUM_COL]; // Pointers to all the events currently shown on screen

  bool infoOpen; // Is the info window open?
  bool recOpen;  // IS the record window open?
  bool wasMuted;  // Was the device muted when we entered the EPG
 
  int inputNum;   // Holds numerical inputs for channel selection
  time_t inputTime; // The last time a numerical value was inputed
  
  eOSState RecProcessKey(eKeys Key);
  eOSState InfoProcessKey(eKeys Key);
  
public:
  cFEpg(void);
  ~cFEpg();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};


const cChannel* getChannelDown(const cChannel* c);
const cChannel* getChannelUp(const cChannel* c);
const cChannel* getNextChannel(const cChannel* c);
const cChannel* getPrevChannel(const cChannel* c);


//////////////////////////////////////////////////////////////////////////////

#define EXPSIZE 7
static const char* titleDelims[EXPSIZE] = { " Series.", " Movie.", " News.", " Education.", " Sports.", " Series/Special.", " Music/Art." };


static const eKeys ekeys[] = {
             kUp,
             kDown,
             kMenu,
             kOk,
             kBack,
             kLeft,
             kRight,
             kRed,
             kGreen,
             kYellow,
             kBlue,
             kPlay,
             kPause,
             kStop,
             kRecord,
             kFastFwd,
             kFastRew,
             kChanUp,
             kChanDn,
             kUser1, kUser2, kUser3, kUser4, kUser5, kUser6, kUser7, kUser8, kUser9};
             
             
//////////////////////////////////////////////////////////////////////////////


#endif //___FEPGMENU_H

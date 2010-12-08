#ifndef __FEPG_OSD_H
#define __FEPG_OSD_H

#include <vdr/plugin.h>
#include <vdr/interface.h>

#include <stdlib.h>
#include <time.h>

#include "menu.h"
#include "config.h"


//////////////////////////////////////////////////////////////////////////////


class cFEpgOsd : public cOsdObject 
{
public:
  cFEpgOsd(void);
 ~cFEpgOsd();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  
private:
  void Draw(void); // Draws the epg
  void ShowInfoWindow(const cEvent*);
  void ShowRecordDialog(const cEvent*);
  void UpdateScreen(void); // Redraws the screen
  
  void DrawEvent(const cEvent* event, time_t startTime, time_t endTime, int row);
  
  int Width(const char* str) const;
  void Text(int x, int y, const char* str, tColor fg, tColor bg);
  void Fill(int x1, int y1, int x2, int y2, tColor c);
  void Darken(int x1, int y1, int x2, int x2, int d = 2, tColor c = clrBlack);
  void DarkenBg(void);
  
  void UpdateSelTime(void);
  
  int scrHeight, scrWidth;
  
  static const int brd = 2;

  // Colors
  tColor gridClr;
  tColor chanClr;
  tColor timeClr;
  tColor textClr;
  tColor lineClr;
  tColor infoClr;
  tColor selClr;
  tColor selTextClr;

  cBitmap* bitmap; // Do all drawing on this
  int width;       // Width of display
  int height;      // Height of display (not counting the time bar at the top)
  int rowHeight;
  int colWidth;    // Width of a column
   
  const cChannel* currentChannel; // The first channel shown in the EPG 
  int selChanNum; // Number of the channel corresponding to the selected row
  time_t* times;  // Array of size config.numCols+1. The time values displayed over each column, 
                  // the last value is not displayed and only used for calculations
  
  time_t firstTime;
  time_t currentTime;

  time_t timeWeight;
  time_t curStartTime, curEndTime; // Used to determined current selected event
                                   // do no necessarily represent start/end time of an event
  
  int oldMin;

  int currentRow;
  
  cOsd* osd;
  const cFont* font; // The used font
  const cEvent* curEvent; // Currently selected event

  bool infoOpen; // Is the info window open?
  bool recOpen;  // Is the record window open?
  bool warnOpen; // Is a warning being displayed?
  bool wasMuted; // Was the device muted when we entered the EPG
 
  int inputNum;     // Holds numerical inputs for channel selection
  time_t inputTime; // The last time a numerical value was inputed
  
  eOSState RecProcessKey(eKeys Key);
  eOSState InfoProcessKey(eKeys Key);
  
};


//////////////////////////////////////////////////////////////////////////////


const cChannel* getChannelDown(const cChannel* c);
const cChannel* getChannelUp(const cChannel* c);
const cChannel* getNextChannel(const cChannel* c);
const cChannel* getPrevChannel(const cChannel* c);

             
//////////////////////////////////////////////////////////////////////////////


#endif //__FEPG_OSD_H

#ifndef ___FEPG_H
#define ___FEPG_H

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <time.h>


using namespace std;


// Set to true if you are using Bell ExpressVu, false otherwise.
static const bool BEV = true;


  
/// cFEpg ////////////////////////////////////////////////////////////////////

class cFEpg : public cOsdObject 
{
private:
  // Draws the epg
  void draw(int start_sid, char direction);
  
  // Draws the info window
  void info(const cEvent*);

  // Converts a string into several string with at most given width
  void WrapText(string s, vector<string>&, int width, bool);
  int Width(const char*);
   
  void Text(int, int, const char*, tColor, tColor);
  void Fill(int, int, int, int, tColor);
  
  void record(const cEvent* e);

  // Redraws the screen
  void updateScreen(void);

  static const int SCR_WIDTH  = 719; // Total screen width
  static const int SCR_HEIGHT = 479; // Total screen height
  static const int NAME_W = 125; // Width of the cells containing channel names & numbers
  static const int DX = 60;  // Space between edge of screen and display (horizontal)
  static const int DY = 20;  // Space between edge of screen and display (vertical)
  static const int DYO = 65; // Space between top of screen and the first channel display
  static const int NUM = 6;  // Number of channels shown at one time

  // Colors
  tColor bgColor;
  tColor green;
  tColor lightgrey;

  cBitmap* bitmap; // Do all drawing on this
  int width;  // Width of display
  int height; // Height of display (not counting the time bar at the top)
  int cWidth; // Width of a cell
  
  int currentChannel; // The first channel shown in the EPG
  int displayedChannels[NUM]; // A list of the channels numbers currently shown 

  struct tm* firstTime; // The time shown in the first column of the EPG
  struct tm* currentTime;
  int oldMin;

  int curCellh, curCellv; // Currently selected Cell, horizontal and vertical values in EPG table
  
  cOsd* osd;
  const cFont* font; // The used font
  const cEvent* curEvent; // Currently selected event
  const cEvent* eventMtx[NUM][3]; // Pointers to all the events currently shown on screen

  bool info_open; // Is the info window open?
  bool rec_open;  // IS the record window open?
  bool wasMuted;  // Was the device muted when we entered the EPG
 
  int inputNum;   // Holds numerical inputs for channel selection
  long inputTime; // The last time a numerical value was inputed
  
  
public:
  cFEpg(void);
  ~cFEpg();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
};


//////////////////////////////////////////////////////////////////////////////

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

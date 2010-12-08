#ifndef __PILOTSKINOSD_H
#define __PILOTSKINOSD_H

#include <vdr/osd.h>
#include <vdr/menuitems.h>
#include <vdr/skins.h>
#include <ctype.h>

#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <time.h>
#include "config.h"


class cPilotskinOsd : public cOsdObject {
private:

  cOsd *osd;
  cSkinDisplayChannel *displayChannel;
  class cPilMenuEvent *displayEvent;
  class cMenuEditTimer *displayTimer;
  int lastTime;
  int number;
  int group;
  char ChanName[255];
  char ChanNumber[255];
  int extraInfo;
  const cEvent *Present;
  const cEvent *Following;
  const cEvent **pArray;
  const char *textPresentInfo;
  const char *textFollowingInfo;
  const char *titlePresentInfo;
  const char *titleFollowingInfo;
  const char *subtitlePresentInfo;
  const char *subtitleFollowingInfo;
  int currentEvent;
  int offset;
  int lines;
  int type;
  
public:
  cPilotskinOsd(void);
  ~cPilotskinOsd();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);

  void CursorUp();
  void CursorDown();
  void CursorOK();
  
  void UpdateEPGInfo(int NowPrevNext);
  void DrawMenu(int delta, int highlight);
  void DisplayBitmap(void); 
  void DisplayInfo(int delta);
  void DisplayExtraInfo(void);
  void DisplayTimer(void);
  void DisplayChannel(const cChannel *Channel);
};

// ----------------------------------------------- //
// --------------   cPilMenuEvent   -------------- //
// ----------------------------------------------- //

class cPilMenuEvent : public cMenuEvent {
public:
	cPilMenuEvent(const class cEvent *EventInfo);
	void printMsg(const char *);
        // Autotimer a implementer -> true
	void helpLine(bool full=false);
};

#endif //__PILOTSKINOSD_H

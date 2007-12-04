#ifndef __PILOTOSD_H
#define __PILOTOSD_H

#include <vdr/osd.h>
#include <vdr/menuitems.h>
#include <ctype.h>
#include "pilotbmp.h"

static const cFont *font = cFont::GetFont(fontOsd);
static const int LINEHEIGHT = font->Height();
#define NBLINES 4

class cPilotOsd : public cOsdObject {
private:
  void DrawSymbol(int x, int y, char *SymbolName[], tColor color);
  cOsd *osd;
  int pal[16];
  int lastTime;
  int number;
  int group;
  char ChanName[255];
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
  cPilotOsd(void);
  ~cPilotOsd();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  
  void SendPalette(void);
  void SetColor(int Index, int Red, int Green, int Blue, int Alpha);
  void ClearBitmap(void);

  void CursorUp();
  void CursorDown();
  void CursorOK();
  
  void UpdateEPGInfo(int NowPrevNext);
  void DrawMenu(int delta, int highlight);
  void DisplayBitmap(void); 
  void DisplayInfo(int delta);
  void DisplayExtraInfo(void);
  void DisplayChannel(const cChannel *Channel);
  char *WrapText(const char *Text, int Width, int *Height);
};

#endif //__PILOTOSD_H

/*
 * $Id: pimOsdMenu.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: pimOsdMenu.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#ifndef __PIMOSDMENU_H_
#define __PIMOSDMENU_H_

#include "calendar.h"
#include "eventListMenu.h"
#include "eventEditMenu.h"

#if VDRVERSNUM >= 10307

#include <vdr/osd.h>
typedef tColor tMyColor;

#else

#include <vdr/osdbase.h>
typedef eDvbColor tMyColor;

#endif


class cPimOsdMenu : public cOsdObject
{
 public:
  
  cPimOsdMenu ();
  ~cPimOsdMenu ();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys key);
  

 private:

  cCalendar _cal;

  int myW, myH;
  int stepX, stepY;
  int cursorX, cursorY;
  int offsetX, offsetY;
  int startX, startY;
  int maxX, maxY;

  tMyColor colFg, colBg;

  cEventListMenu *list;
  eDvbFont _font;

  // make osd-stuff version-independet

#if VDRVERSNUM >= 10307

  cOsd *osd;

  void drawRectangle(int x1, int y1, int x2, int y2, tMyColor Color)
  {
    osd->DrawRectangle(x1, y1, x2, y2, Color);
  }

  void drawText(int x,int y,const char *s,tMyColor ColorFg,tMyColor ColorBg)
  {
    osd->DrawText(x, y, s, ColorFg, ColorBg, cFont::GetFont(_font));
  }

#else

  cOsdBase *osd;

  void drawRectangle(int x1, int y1, int x2, int y2, tMyColor Color)
  {
    osd->Fill(x1, y1, x2, y2, Color);
  }

  void drawText(int x,int y,const char *s,tMyColor ColorFg,tMyColor ColorBg)
  {
    osd->Text(x, y, s, ColorFg, ColorBg);
  }

#endif

  void setColorsForDay (int day, int month, int year, int dayOfWeek,
			bool cursor = false);

  void showCalendar(cCalendar& c);
  void showEventsForDay(cCalendar& c);
  void showAllEvents();
  void showCursor(eKeys key);
  bool isToday(int day);
  void setFont(eDvbFont f) { _font = f; };
  int GetCalWeekNo(int d, int m, int y);
  void setCursorToday(void);
};

#endif

/*
 * $Id: pimOsdMenu.c,v 1.1.1.1 2005/02/17 05:45:53 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: pimOsdMenu.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:53  sundin
 * initial import ver0.0.5
 *
 * Revision 1.2  2005/01/17 18:30:28  ksltuf
 * fix for gcc 3.4
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.5  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.4  2004/08/09 17:45:24  ksltuf
 * repeat events for cursor
 *
 * Revision 1.3  2004/07/05 19:26:41  ksltuf
 * removed unused variable weekFont
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include "pimOsdMenu.h"
#include "pimSetup.h"
#include "calEvent.h"
#include "calEvents.h"
#include "calendar.h"
#include "calManager.h"
#include "i18n.h"

#include <vdr/config.h>
#include <vdr/osd.h>
#include <vdr/osdbase.h>

#include <stdlib.h>

// colors for month preview
#define my_clrLightGrey  0xFFD0D0D0
#define my_clrLightGreen 0xFFC0F0C0

cPimOsdMenu::cPimOsdMenu()
{
  osd = NULL;
  list = NULL;

  cursorX = 0;
  cursorY = 0;

#if VDRVERSNUM >= 10307
  int maxWidth  = Setup.OSDWidth;
  int maxHeight = Setup.OSDHeight;
#else
  int maxWidth  = Setup.OSDwidth  * cOsd::CellWidth();
  int maxHeight = Setup.OSDheight * cOsd::LineHeight();
#endif

  // size of our OSD
  myW = min (356, maxWidth);
  myH = min (450, maxHeight);

  // start point (left upper corner) of our osd on the screen
  startX = (maxWidth  - myW) / 2 + globalSetup.osdOffsetX;
  startY = (maxHeight - myH) / 2 + globalSetup.osdOffsetY;

  // step size for calendar columns and rows
  stepX = myW / (globalSetup.showCW ? 8 : 7); // 8 rows if we want to draw week number
  stepY = myH / 9;

  // start point for drawing text (calendar entrys)
  offsetX = 10 + (globalSetup.showCW ? stepX : 0);	// shift by stepX if week number should be drawn
  offsetY = 10 + 2 * stepY;  // skip headers (3 lines)

  maxX = 6; // day per week 0 ... 6 , i.e. columns
  maxY = 5; // max. weeks per month  0 ... 5 , i.e. rows

  setCursorToday();
}


cPimOsdMenu::~cPimOsdMenu()
{
  if (osd)  DELETENULL(osd);
  if (list) DELETENULL(list);
}


// set today as start point for cursor
// pre-condition: calendar must contain the actual month
// (no check for this here)
void cPimOsdMenu::setCursorToday(void)
{
  int d = _cal.getTodayDay();
  for (int i=0; i<=maxY; i++) {
    for (int j=0; j<=maxX; j++) {	
      if (_cal[i][j] == d) {
	cursorX = j;
	cursorY = i;
	return;
      }
    }
  }
}


// set foreground and background color for one day
void cPimOsdMenu::setColorsForDay(int day, int month, int year,
				  int dayOfWeek, bool cursor) 
{
  if (day == 0) {	// no day displayed
    colFg = colBg = clrWhite;
    return;
  }

  if (day < 0) {	// last or next month
    if (day < -20) {	// last month
      month--;
      if (month == 0) {
	month = 12;
	year--;
      }
    }
    else {		// next month
      month++;
      if (month > 12) {
	month = 1;
	year++;
      }
    }
  }

  tMyColor fg = clrBlack, bg = clrWhite;
  if ((day > 0) && isToday(day)) {
    fg = clrMagenta;	// today
    bg = cursor ? clrBlack : clrWhite;
  }
  else if ( cCalManager::getInstance()->hasEvent(abs(day), month, year, dayOfWeek) ) {
    fg = (day > 0) ? clrGreen : my_clrLightGreen;	// day with event
    bg = cursor ? clrBlack : clrWhite;
  }
  else if (day < 0) {
    colFg = my_clrLightGrey;	// last or next month
    colBg = clrWhite;
    return;
  }
  else {
    // all other days (no invers colors for weekend)
    colFg = cursor ? clrWhite : (dayOfWeek==6 || dayOfWeek==7) ? clrRed : clrBlack;
    colBg = cursor ? clrBlack : clrWhite;
    return;
  }

  // set the colors
  colFg = globalSetup.inversEventColor ? bg : fg;
  colBg = globalSetup.inversEventColor ? fg : bg;
}


void cPimOsdMenu::showCursor(eKeys key)
{
  char buf[30];

  // old position
  int xPos = offsetX + cursorX*stepX;
  int yPos = offsetY + cursorY*stepY;

  // get old entry
  int old_day = _cal[cursorY][cursorX];
  if (old_day != 0)
     sprintf (buf, "%2d ", abs(old_day));
  else
     sprintf (buf, "   ");

  // get old color
  setColorsForDay(old_day, _cal.getMonth(), _cal.getYear(), cursorX+1);

  // draw old entry
  drawText(xPos, yPos, buf, colFg, colBg);

  switch (key)
  {
    case kUp:
	     cursorY--; break;
    case kDown:
	     cursorY++; break;
    case kLeft:
	     cursorX--; break;
    case kRight:
	     cursorX++; break;
    case kOk:
	     setColorsForDay(old_day, _cal.getMonth(), _cal.getYear(), cursorX+1, true);
	     drawText(xPos, yPos, buf, colFg, colBg);
	     osd->Flush();
	     // falltrough
    default:
	     return;
  }

  // overflow
  if (cursorX < 0)    { cursorX = maxX; cursorY--;} 
  if (cursorX > maxX) { cursorX = 0; cursorY++;}
  if (cursorY < 0)    cursorY = maxY;
  if (cursorY > maxY) cursorY = 0;

  if ((_cal[cursorY][cursorX] <= 0 && cursorY == maxY && key == kRight) ||
      // this happens after overflow, if last day in month is a sunday
      (cursorY == 0 && cursorX == 0 && key == kRight) ||
      // this happens after overflow, if next month days are previewed
      ((old_day > 0) && (_cal[cursorY][cursorX] < 0) && (key == kDown)))
  {
    // last day in last line -> show first day of next month
    cursorX = cursorY = 0;
    _cal.increaseMonth();
    showCalendar (_cal);
    return;
  }
  else if ((_cal[cursorY][cursorX] <= 0 && cursorY == 0 && key == kLeft) ||
	   // this happens after overflow, if first day in month is a monday
	   (cursorY == maxY && cursorX == maxX && key == kLeft) ||
	   // this happens after overflow, if previous month days are previewed
	   ((old_day > 0) && (_cal[cursorY][cursorX] < 0) && (key == kUp)))
  {
    // first day in first line -> show last day of previous month
    cursorX = maxX;
    cursorY = maxY;
    _cal.decreaseMonth();
    showCalendar (_cal);
    return;
  }
  else if (_cal[cursorY][cursorX] <= 0 && key != kOk)
  {
    // empty entry -> repeat event
    showCursor(key);
    return;
  }
  else
  {
    // new position
    xPos = offsetX + cursorX*stepX;
    yPos = offsetY + cursorY*stepY;

    // show new entry
    sprintf (buf, "%2d ", _cal[cursorY][cursorX]);
    setColorsForDay(_cal[cursorY][cursorX], _cal.getMonth(),
		    _cal.getYear(), cursorX+1, true);
    drawText(xPos, yPos, buf, colFg, colBg);
  }

  osd->Flush();
}


void cPimOsdMenu::showEventsForDay(cCalendar& c)
{
  // get entry
  int day = c[cursorY][cursorX];

  if (day == 0)
     return;

  if ( cCalManager::getInstance()->hasEvent(day, c.getMonth(), 
			                    c.getYear(), cursorX+1) )
  {
     // close the main calendar osd and show a submenu containing
     // all events for the marked day
     osd->Flush();
     DELETENULL(osd);

     list = new cEventListMenu(day, c.getMonth(), c.getYear(), cursorX+1);
     list->Display();
  }
}
  

void cPimOsdMenu::showAllEvents()
{

  // close the main calendar osd and show a submenu containing
  // all events for the marked day
  osd->Flush();
  DELETENULL(osd);

  list = new cEventListMenu();
  list->Display();
}
  
//------------------------------------------------------------
// Die deutschen Kalenderwochen berechnen sich wie folgt:
// Der 1. Januar eines Jahres gehoert erst dann zur ersten
// Kalenderwoche, wenn dieser Tag auf einen Montag,
// Dienstag, Mittwoch oder Donnerstag faellt.
// Falls der 1.Januar ein Freitag, Samstag oder Sonntag ist,
// zaehlt er, und ggf. auch der 2. und 3. Januar, noch zur 
// letzten Kalenderwoche des vorherigen Jahres.
//
// Weiterhin koennen der 29., 30. und 31.12. eines Jahres schon
// zur Kalenderwoche 1 des neuen Jahres gehoeren.
// Das ist genau dann der Fall, wenn der 31.12. auf einen
// Montag, Dienstag oder Mittwoch faellt.
//
// Bemerkungen zur Berechnung der Wochennummer nach DIN 1355:
//
// Der Montag ist der erste Tag der Woche
// Der 4. Januar liegt immer in der ersten Kalenderwoche.
// Der 28. Dezember liegt immer in der letzten Kalenderwoche.
// aus diesen Definitionen folgt:
//
// Eine Woche gehoert zu demjenigen Kalenderjahr, in dem 3
// oder mehr Tage der Woche liegen.
// Der Donnerstag einer Woche liegt immer in demjenigen
// Kalenderjahr, dem die Woche zugerechnet wird.
//------------------------------------------------------------
// If first day of week is configured as sunday, the same
// algorithm is applied.
// TODO: adapt for foreign countries if needed.
//------------------------------------------------------------
int cPimOsdMenu::GetCalWeekNo(int d, int m, int y)
{
  struct tm tm = { 0, 0, 12, d, m-1, y-1900, 0, 0, -1 };

  mktime(&tm);
  int wday = globalSetup.firstIsMonday ? ((tm.tm_wday == 0) ? 6 : (tm.tm_wday - 1)) : tm.tm_wday;
  int yday = tm.tm_yday + 7 - wday;
  int CalWeekNo = yday / 7;

  if ((yday - (CalWeekNo * 7)) > 3)
    CalWeekNo++;

  if ((CalWeekNo == 0) || (CalWeekNo == 53)) {
    if(CalWeekNo == 0) tm.tm_year--;	// get CW of last week last year
    tm.tm_mday = 31;
    tm.tm_mon = 11;
    mktime(&tm);
    wday = globalSetup.firstIsMonday ? ((tm.tm_wday == 0) ? 6 : (tm.tm_wday - 1)) : tm.tm_wday;

    if (CalWeekNo == 53) {
      if (wday <= 3)
        CalWeekNo = 1;
    }
    else {	// CalWeekNo == 0
      yday = tm.tm_yday + 7 - wday;
      CalWeekNo = yday / 7;
      if ((yday - (CalWeekNo * 7)) > 3)
        CalWeekNo++;
    }
  }

  return CalWeekNo;
}

void cPimOsdMenu::showCalendar(cCalendar& c)
{
  char buf[30];

#if VDRVERSNUM >= 10300
  const cFont *f = cFont::GetFont(fontOsd);
#else
  cFont *f = new cFont(fontOsd);
#endif

  setFont(fontOsd);

  int xPos = offsetX;
  int yPos = offsetY;

  // background
  drawRectangle(5, 5,       myW-5, stepY,     clrBlack);
  drawRectangle(5, stepY,   myW-5, stepY*2,   clrYellow);
  drawRectangle(5, stepY*2, myW-5, stepY*8,   clrWhite);
  if (globalSetup.showCW)
    drawRectangle(5, stepY, stepX, stepY*8,   clrYellow);

  // footer
  int xb = (myW-10) / 4;
  int w, h;

  // red button
  w = f->Width(tr("Today"));
  h = f->Height(tr("Today"));
  drawRectangle(5, stepY*8, 5+xb, stepY*9-5, clrRed);
  drawText(5+(xb-w)/2, stepY*8 + (stepY-h)/2, tr("Today"), clrWhite, clrRed);

  // green button
  w = f->Width("-");
  h = f->Height("-");
  drawRectangle(5+xb, stepY*8, 5+2*xb, stepY*9-5, clrGreen);
  drawText(5+xb+(xb-w)/2, stepY*8 + (stepY-h)/2, "-", clrBlack, clrGreen);

  // yellow button
  w = f->Width("+");
  h = f->Height("+");
  drawRectangle(5+2*xb, stepY*8, 5+3*xb, stepY*9-5, clrYellow);
  drawText(5+2*xb+(xb-w)/2, stepY*8 + (stepY-h)/2, "+", clrBlack, clrYellow);

  // blue button
  w = f->Width(tr("List"));
  h = f->Height(tr("List"));
  drawRectangle(5+3*xb, stepY*8, myW-5, stepY*9-5, clrBlue);
  drawText(5+3*xb+(xb-w)/2, stepY*8 + (stepY-h)/2, tr("List"),clrWhite,clrBlue);

  // draw the header with month and year
  sprintf (buf, "%d   %s", c.getYear(), c.getMonthAsText()); 
  drawText (offsetX + 5 - (globalSetup.showCW ? stepX : 0),
	    offsetY - (2 * stepY), buf, clrYellow, clrBlack);

  // weekdays in header
  yPos = offsetY - stepY;
  for (int j=0; j <= maxX; j++) {
    // calculate position
    xPos = offsetX + j*stepX;
    sprintf (buf, "%s ", c.getDayOfWeekAsText(j+1));
    drawText (xPos,yPos,buf,clrBlack,clrYellow);
  }

  // setup calendar week
  int cw = 0;
  if (globalSetup.showCW) {
    // draw calender week header
    drawText(offsetX - stepX - 2, yPos, tr("CW"), clrBlack, clrYellow);
    cw = GetCalWeekNo(1, c.getMonth(), c.getYear());
  }

  int new_cw = cw;
  for (int i=0; i <= maxY; i++) {
    yPos = offsetY + i*stepY;
    if (globalSetup.showCW) {
      // correct CW if at start / end of year
      cw = (new_cw >= 53) ? GetCalWeekNo(1 + (i * 7), c.getMonth(), c.getYear()) : new_cw;
      sprintf(buf, "%2d", cw);
      new_cw = cw + 1;
      // calculate position for week name
      xPos = offsetX - stepX;
      if ((c[i][0] != 0) || !i)
	drawText(xPos, yPos, buf, clrBlack, clrYellow);
    }
    for (int j=0; j <= maxX; j++) {	
      // calculate position
      xPos = offsetX + j*stepX;
      setColorsForDay(c[i][j], c.getMonth(), c.getYear(), j+1);

      // skip empty days
      if (c[i][j] == 0)
	strcpy (buf, "  ");
      else
	sprintf (buf, "%2d", abs(c[i][j]));
      drawText(xPos, yPos, buf, colFg, colBg);
    }
  }

  // Set cursor to valid day if field is empty.
  // At first line shift to the right else to the left side to cover this.
  if (_cal[cursorY][cursorX] <= 0)
    showCursor(cursorY ? kLeft : kRight);
  else
    showCursor(kOk);

  osd->Flush();
}


bool cPimOsdMenu::isToday(const int day)
{
  return ( day == _cal.getTodayDay() &&
           _cal.getMonth() == _cal.getTodayMonth() &&
           _cal.getYear()  == _cal.getTodayYear() );
}


void cPimOsdMenu::Show(void)
{
#if VDRVERSNUM >= 10307
  osd = cOsdProvider::NewOsd(startX,startY);
  if (osd)
  {
    tArea Area = { 1, 1, myW, myH, 4 };

    if (osd->CanHandleAreas(&Area, 1) == oeOk)
       osd->SetAreas(&Area, 1);
    else
       esyslog ("PIM: OSD Open failed");
  }
#else
  osd = cOsd::OpenRaw (startX, startY);
  if (osd)
  {
    osd->Create(1, 1, myW, myH, 4);
    osd->AddColor(clrBlack);
    osd->AddColor(clrRed);
    osd->AddColor(clrGreen);
    osd->AddColor(clrBlack);
    osd->AddColor(clrYellow);
    osd->AddColor(clrBlue);
    osd->AddColor(clrMagenta);
  }
#endif

  showCalendar(_cal);
}


eOSState cPimOsdMenu::ProcessKey(eKeys key)
{
  if (!osd && key == kBack)
  {
     // back was pressed in sub-menu -> if it has no sub-menu itself
     // then close it and show calendar-osd again
     if (list && !list->HasSubMenu())
     {
       DELETENULL(list);
       Show();
       return osContinue;
     }
  }

  if (list)
  {
    // give the control to our sub-menu
    return list->ProcessKey(key);
  }

  // process other events
  eOSState state = cOsdObject::ProcessKey(key);

  if (osd)
  switch (key & 0x7fff)   // repeat events
  {
	  case kOk:
		  // any events for this day?
		  showEventsForDay (_cal);
		  break;

	  case kRed:
		  // show actual month
		  _cal.reset();
		  setCursorToday();
		  showCalendar (_cal);
		  break;

	  case kGreen:
		  // show previous month
		  _cal.decreaseMonth();
		  showCalendar (_cal);
		  break;

	  case kYellow:
		  // show next month
		  _cal.increaseMonth();
		  showCalendar (_cal);
		  break;

	  case kBlue:
		  // show a list of all known events
		  showAllEvents();
		  break;

	  case kUp:
	  case kDown:	  
	  case kLeft:	  
	  case kRight:
	  	  // move the cursor
		  showCursor(key);
		  break;

	  case kBack:
		  // close calendar-osd
		  state=osEnd;

          default:
		  break;
  }

  return state;
}


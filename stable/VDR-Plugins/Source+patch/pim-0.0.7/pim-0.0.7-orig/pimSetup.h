/*
 * $Id: pimSetup.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: pimSetup.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.3  2004/12/13 07:58:12  ksltuf
 * new setup option 'firstIsMonday'
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#ifndef __PIMSETUP_H_
#define __PIMSETUP_H_

#define MAX_CAL_CHARS 20

class cPimSetup
{
 public:

  // for the osd-window
  int osdOffsetX;
  int osdOffsetY;

  // indicates if cal returns monday (true) or sunday (false) as
  // first day of week
  bool firstIsMonday;

  // remind for events:
  // 0 - off at all
  // 1 - remind always at vdr start
  // 2 - remind at special time (after user activity)
  int reminderMode;

  // remind just now or after user activity
  // 0 - activity checking always off
  // 1 - remind always after user activity
  // 2 - remind after user activity only in time mode
  int activityMode;

  // specail reminder time
  int reminderTime;

  // wait some seconds before remind
  int remindDelay;

  // display calendar week (true) else not (false)
  bool showCW;

  // show last / next month days on month display in grey
  bool previewLastNextW;

  // use iners colors for days with events (foreground and backgroung color switched)
  bool inversEventColor;

  // config directory
  char* ConfigDir;

  cPimSetup(void);
  ~cPimSetup(void);
};

extern cPimSetup globalSetup;

#endif


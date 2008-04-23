/*
 * $Id: eventSetupMenu.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: eventSetupMenu.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.4  2004/12/13 07:58:12  ksltuf
 * new setup option 'firstIsMonday'
 *
 * Revision 1.3  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#ifndef __EVENTSETUPMENU_H_
#define __EVENTSETUTMENU_H_

#include "pimSetup.h"

#include <vdr/plugin.h>
#include <vdr/menuitems.h>

class cEventSetupMenu : public cMenuSetupPage
{
 public:
   cEventSetupMenu(void);

 protected:
   virtual void Store(void);

 private:
   int _osdOffsetX;
   int _osdOffsetY;
   int _firstIsMonday;
   int _reminderMode;
   int _activityMode;
   int _showCW;
   int _previewLastNextW;
   int _inversEventColor;
   int _reminderTime;
   int _remindDelay;
   int _last_remind;

   const char *reminderTexts[4];

   cMenuEditTimeItem * _timeItem;
   cMenuEditIntItem * _delayItem;
   cMenuEditStraItem * _activityItem;

   eOSState ProcessKey(eKeys key);
   void allowRemindTime(int remind);
};

#endif

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef __SETUP_H
#define __SETUP_H


//There are two places to be kept in sync with these enums:
//TeletextBrowser::TranslateKey and 
//the constants in cPluginTeletextosd::initTexts
enum eTeletextAction { Zoom, HalfPage, SwitchChannel,
                       DarkScreen, /*SuspendReceiving,*/ LastAction }; //and 100-899 => jump to page

enum ActionKeys {
ActionKeyRed,
ActionKeyGreen,
ActionKeyYellow,
ActionKeyBlue,
ActionKeyPlay,
//ActionKeyPause,
ActionKeyStop,
//ActionKeyRecord,
ActionKeyFastFwd,
ActionKeyFastRew,

LastActionKey
};

/* 
kRed
kGreen
kYellow
kBlue
kPlay
kPause
kStop
kRecord
kFastFwd*/

//Default values are set in menu.c, setup menu, parsing and storing can be found in osdteletext.c
class TeletextSetup {
public:
   TeletextSetup();
   int mapKeyToAction[10]; //4 color keys + kPlay, kPause etc.
   unsigned int configuredClrBackground;
   int showClock;
   int suspendReceiving;
   int autoUpdatePage;
   int OSDheight;
   int OSDwidth;
   int OSDHAlign;
   int OSDVAlign;
   int inactivityTimeout;
};

extern TeletextSetup ttSetup;

#endif

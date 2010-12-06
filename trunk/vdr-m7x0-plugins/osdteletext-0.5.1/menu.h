/***************************************************************************
 *       Copyright (c) 2003,2004 by Marcel Wiesweg                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OSDTELETEXT_MENU__H
#define OSDTELETEXT_MENU__H

#include <time.h>

#include <vdr/osd.h>
#include <vdr/osdbase.h>
#include <vdr/status.h>

#include "txtrecv.h"
#include "setup.h"

extern int Stretch;

class ChannelStatus : public cStatus {
public:
   ChannelStatus();
protected:
   virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
};


class TeletextBrowser : public cOsdObject {
public:
   TeletextBrowser(cTxtStatus *txtSt);
   ~TeletextBrowser();
   void Show(void);
   static void ChannelSwitched(int ChannelNumber);
   virtual eOSState ProcessKey(eKeys Key);
protected:
   enum Direction { DirectionForward, DirectionBackward };
   void SetNumber(int i);
   void ShowPage();
   void UpdateClock();
   bool DecodePage();
   void ChangePageRelative(Direction direction);
   void ChangeSubPageRelative(Direction direction);
   bool CheckPage();
   void ShowAskForChannel();
   bool CheckFirstSubPage(int startWith);
   void SetPreviousPage(int oldPage, int oldSubPage, int newPage);
   bool CheckIsValidChannel(int number);
   int  PageCheckSum();
   void ShowPageNumber();
   void ExecuteAction(eTeletextAction e);
   int nextValidPageNumber(int start, Direction direction);
   char fileName[PATH_MAX];
   char page[40][24];
   int cursorPos;
   eTeletextAction TranslateKey(eKeys Key);
   bool pageFound;
   bool selectingChannel;
   bool needClearMessage;
   int selectingChannelNumber;
   int checkSum;
   cTxtStatus *txtStatus;
   bool suspendedReceiving;
   int previousPage;
   int previousSubPage;
   int pageBeforeNumberInput;
   time_t lastActivity;
   int inactivityTimeout;
   static int currentPage;
   static int currentSubPage;
   static tChannelID channel;
   static int currentChannelNumber;
   static TeletextBrowser* self;
};


#endif


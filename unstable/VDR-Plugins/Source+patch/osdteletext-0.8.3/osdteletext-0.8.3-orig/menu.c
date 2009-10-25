/***************************************************************************
 *       Copyright (c) 2003,2004 by Marcel Wiesweg                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
  
#include <map>
#include <time.h>
    
#include <vdr/interface.h>
#include <vdr/config.h>

#include "menu.h"
#include "display.h"
#include "setup.h"
#include "txtrecv.h"

#define GET_HUNDREDS(x) ( ( (x) - ((x)%256) ) /256 )
#define GET_TENS(x)  ( (( (x) - ((x)%16) )%256 ) /16 )
#define GET_ONES(x)   ( (x)%16 )

#define GET_HUNDREDS_DECIMAL(x) ( ( (x) - ((x)%100) ) /100 )
#define GET_TENS_DECIMAL(x)  ( (( (x) - ((x)%10) )%100 ) /10 )
#define GET_ONES_DECIMAL(x)   ( (x)%10 )

#define PSEUDO_HEX_TO_DECIMAL(x) ( (GET_HUNDREDS_DECIMAL(x))*256 + (GET_TENS_DECIMAL(x))*16 + (GET_ONES_DECIMAL(x)) )

using namespace std;
   
int Stretch = true;
typedef map<int,int> IntMap;
IntMap channelPageMap;

//static variables
int TeletextBrowser::currentPage=0x100; //Believe it or not, the teletext numbers are somehow hexadecimal
int TeletextBrowser::currentSubPage=0;
tChannelID TeletextBrowser::channel;
int TeletextBrowser::currentChannelNumber=0;
TeletextBrowser* TeletextBrowser::self=0;


TeletextBrowser::TeletextBrowser(cTxtStatus *txtSt) {
   cursorPos=0;
   pageFound=true;
   selectingChannel=false;
   needClearMessage=false;
   selectingChannelNumber=-1;
   self=this;
   txtStatus=txtSt;
   //if (txtStatus)
    //  txtStatus->ForceReceiving(true);
   suspendedReceiving=false;  
   previousPage=currentPage;
   previousSubPage=currentSubPage;
   pageBeforeNumberInput=currentPage;
   lastActivity=time(NULL);
   inactivityTimeout=-1;
}


TeletextBrowser::~TeletextBrowser() {
   Display::Delete();

   self=0;
   /*if (txtStatus) {
      if (suspendedReceiving)
         txtStatus->ForceSuspending(false);
      txtStatus->ForceReceiving(false);
   }*/
}

void TeletextBrowser::Show(void) {
    Display::SetMode(Display::mode);
    ShowPage();
}

bool TeletextBrowser::CheckIsValidChannel(int number) {
   return (Channels.GetByNumber(number) != 0);
}

void TeletextBrowser::ChannelSwitched(int ChannelNumber) {
   cChannel *chan=Channels.GetByNumber(ChannelNumber);
   
   if (!chan)
      return;
      
   tChannelID chid=chan->GetChannelID();
   if (chid==channel || chid==tChannelID::InvalidID)
      return;
      
   channel=chid;
   
   //store page number of current channel
   IntMap::iterator it;
   channelPageMap[currentChannelNumber] = currentPage;
   currentChannelNumber=ChannelNumber;
   
   currentPage=0x100;
   currentSubPage=0;
      
   //see if last page number on this channel was stored
   it=channelPageMap.find(ChannelNumber);
   if (it != channelPageMap.end()) { //found
      currentPage=(*it).second;
   }
   
   //on the one hand this must work in background mode, when the plugin is not active.
   //on the other hand, if active, the page should be shown.
   //so this self-Pointer.
   if (self) {
      self->ShowPage();
   }
}


eOSState TeletextBrowser::ProcessKey(eKeys Key) {
   if (Key != kNone)
      lastActivity = time(NULL);
   
   switch (Key) {
      case k1: SetNumber(1);break;
      case k2: SetNumber(2);break;
      case k3: SetNumber(3);break;
      case k4: SetNumber(4);break;
      case k5: SetNumber(5);break;
      case k6: SetNumber(6);break;
      case k7: SetNumber(7);break;
      case k8: SetNumber(8);break;
      case k9: SetNumber(9);break;
      case k0: 
         //same behavior for 0 as VDR does it with channels
         if ((cursorPos==0)  && (!selectingChannel)) {
            //swap variables
            int tempPage=currentPage;
            int tempSubPage=currentSubPage;
            currentPage=previousPage;
            currentSubPage=previousSubPage;
            previousPage=tempPage;
            previousSubPage=tempSubPage;
            ShowPage();
         } else
            SetNumber(0);
         break;
      case kOk: 
         if (selectingChannel) {
            selectingChannel=false;
            Display::ClearMessage();
            if (selectingChannelNumber>0) {
               if (CheckIsValidChannel(selectingChannelNumber))
                  ChannelSwitched(selectingChannelNumber);
               else {
                  needClearMessage=true;
                  Display::DrawMessage(trVDR("*** Invalid Channel ***"));
               }
            } else {
               ShowPage();
            }
         }
         break;        
      case kBack: return osEnd; 
      case kNone: //approx. every second
         //checking if page changed
         if ( pageFound && ttSetup.autoUpdatePage && cursorPos==0 && !selectingChannel && (PageCheckSum() != checkSum) ) {
            ShowPage();
         //check if page was previously not found and is available now
         } else if (!pageFound && CheckFirstSubPage(0)) {
            ShowPage();
         } else {
            if (needClearMessage) {
               needClearMessage=false;
               Display::ClearMessage();
            }
            //updating clock
            UpdateClock();
         }
         //check for activity timeout
         if (ttSetup.inactivityTimeout && (time(NULL) - lastActivity > ttSetup.inactivityTimeout*60))
            return osEnd;
         break;
      case kUp:
         if (selectingChannel) {
             selectingChannel=false;
             Display::ClearMessage();
         }

         if (cursorPos != 0) {
            //fully revert cursor
            SetNumber(-3);
         }
         ChangePageRelative(DirectionForward);
         Display::ShowUpperHalf();
         ShowPage();
         break;
      case kDown:
         if (selectingChannel) {
             selectingChannel=false;
             Display::ClearMessage();
         }
         if (cursorPos != 0) {
            //fully reset
            SetNumber(-3);
         }
         ChangePageRelative(DirectionBackward);
         Display::ShowUpperHalf();
         ShowPage();
         break;       
      case kRight:
         if (selectingChannel) {
             selectingChannel=false;
             Display::ClearMessage();
         }
         if (cursorPos != 0) {
            //fully reset
            SetNumber(-3);
         }
         ChangeSubPageRelative(DirectionForward);
         Display::ShowUpperHalf();
         ShowPage();
         break;       
      case kLeft:
         if (selectingChannel) {
             selectingChannel=false;
             Display::ClearMessage();
         }
         if (cursorPos != 0) {
            //revert cursor
            SetNumber(-1);
            break;
         }
         ChangeSubPageRelative(DirectionBackward);
         Display::ShowUpperHalf();
         ShowPage();
         break; 
         
              
      case kRed: 
      case kGreen: 
      case kBlue:
      case kYellow:
      //case kUser1:case kUser2:case kUser3:case kUser4:case kUser5:
      //case kUser6:case kUser7:case kUser8:case kUser9:
      case kPlay:case kPause:case kStop: case kRecord:case kFastFwd:case kFastRew:
         if (cursorPos != 0) {
            //fully reset
            SetNumber(-3);
         }
         ExecuteAction(TranslateKey(Key));
         break;             
      default: break;
   }
   return osContinue;
}

void TeletextBrowser::ExecuteAction(eTeletextAction e) {
         switch (e) {
            case Zoom:
               if (selectingChannel) {
                   selectingChannel=false;
                   Display::ClearMessage();
               }
               
               switch (Display::GetZoom()) {
               case cDisplay::Zoom_Off:
                  Display::SetZoom(cDisplay::Zoom_Upper);
                  break;
               case cDisplay::Zoom_Upper:
                  Display::SetZoom(cDisplay::Zoom_Lower);
                  break;
               case cDisplay::Zoom_Lower:
                  Display::SetZoom(cDisplay::Zoom_Off);
                  break;
               }
               
               break;
            case HalfPage:
               if (selectingChannel) {
                   selectingChannel=false;
                   Display::ClearMessage();
               }
                  
               switch (Display::mode) {
               case Display::HalfUpper:
                  Display::SetMode(Display::HalfLower);
                  break;
               case Display::HalfLower:
                  Display::SetMode(Display::Full);
                  break;
               case Display::Full:
                  Display::SetMode(Display::HalfUpper);
                  break;
               }
               ShowPage();
               break;
            case SwitchChannel:
               selectingChannelNumber=0;
               selectingChannel=true;
               ShowAskForChannel();
               break;
            /*case SuspendReceiving:
               if (!txtStatus)
                  break;
               //if (suspendedReceiving)
                //  txtStatus->ForceSuspending(false);
               //else
                //  txtStatus->ForceSuspending(true);
               //suspendedReceiving=(!suspendedReceiving);
               break;*/
            case DarkScreen:
               ChangeBackground();
               break;
            default:
               //In osdteletext.c, numbers are thought to be decimal, the setup page
               //entries will display them in this way. It is a lot easier to do the
               //conversion to hexadecimal here.
               //This means, we convert the number to what it would be if the string
               //had been parsed with hexadecimal base.
               int pageNr=PSEUDO_HEX_TO_DECIMAL((int)e);
               if (0x100<=pageNr<=0x899) {
                  if (selectingChannel) {
                      selectingChannel=false;
                      Display::ClearMessage();
                  }
                  SetPreviousPage(currentPage, currentSubPage, pageNr);
                  currentPage=pageNr;
                  cursorPos=0;
                  currentSubPage=0;
                  
                  Display::ShowUpperHalf();
                  ShowPage();
               }
               break;
         }
}

// 3-state toggling between configured->transparent->black.
// If configured is black or transparent, do 2-state transparent->black only.
void TeletextBrowser::ChangeBackground()
{
   tColor clrConfig = (tColor)ttSetup.configuredClrBackground;
   tColor clrCurrent = Display::GetBackgroundColor();

   if (clrCurrent == clrConfig)
      if (clrConfig == clrTransparent)
         Display::SetBackgroundColor(clrBlack);
      else
         Display::SetBackgroundColor(clrTransparent);
   else if (clrCurrent == clrBlack)
      if (clrConfig == clrBlack)
         Display::SetBackgroundColor(clrTransparent);
      else
         Display::SetBackgroundColor(clrConfig);
   else // clrCurrent == clrTransparent
      Display::SetBackgroundColor(clrBlack);
}

eTeletextAction TeletextBrowser::TranslateKey(eKeys Key) {
   switch(Key) {
      case kRed:     return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyRed];
      case kGreen:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyGreen];
      case kYellow:  return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyYellow];
      case kBlue:    return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyBlue];
      case kPlay:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyPlay];
      //case kPause:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyPause];
      case kStop:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyStop];
      //case kRecord:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyRecord];
      case kFastFwd:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyFastFwd];
      case kFastRew:   return (eTeletextAction)ttSetup.mapKeyToAction[ActionKeyFastRew];
      default: return (eTeletextAction)100; //just to keep gcc quiet
   }
}


void TeletextBrowser::SetNumber(int i) {
   //cursorPos means insertion after, 0<=cursorPos<=2
   if (selectingChannel) {
      selectingChannelNumber = selectingChannelNumber*10+i;  
      ShowAskForChannel();
      return;
   }
   
   //i<0 means revert cursor position
   if (i<0) {
      for (;i<0;i++) {
         switch (cursorPos) {
         case 0:
            return;
         case 1:
            currentPage = currentPage-256*GET_HUNDREDS(currentPage)+256*GET_HUNDREDS(pageBeforeNumberInput);
            break;
         case 2:
            currentPage = currentPage-16*GET_TENS(currentPage)+16*GET_TENS(pageBeforeNumberInput);
            break;
         }
         cursorPos--;
      }
      ShowPageNumber();
      return;
   }

   
   static int tempPage;
   switch (cursorPos) {
   case 0:
      if (i<1) i=1;
      //accept no 9 when cursorPos==0
      if (i>8) i=8;
      tempPage= currentPage;
      pageBeforeNumberInput = currentPage;
      currentPage = currentPage-256*GET_HUNDREDS(currentPage)+256*i;
      break;
   case 1:
      if (i<0) i=0;
      if (i>9) i=9;
      currentPage = currentPage-16*GET_TENS(currentPage)+16*i;
      break;
   case 2:
      if (i<0) i=0;
      if (i>9) i=9;
      currentPage = currentPage-GET_ONES(currentPage)+i;
      pageBeforeNumberInput = currentPage;
      SetPreviousPage(tempPage, currentSubPage, currentPage);
      break;
   }
   pageFound=true; //so that "page ... not found" is not displayed, but e.g. 1**-00
   if (++cursorPos>2) {
      cursorPos=0;
      CheckFirstSubPage(0);
      Display::ShowUpperHalf();
      ShowPage();
   } else {
      ShowPageNumber();
   }
}

//returns whether x, when written in hexadecimal form,
//will only contain the digits 0...9 and not A...F
//in the first three digits.
static inline bool onlyDecimalDigits(int x) {
   return ((  x      & 0xE) < 0xA) &&
          (( (x>>4)  & 0xE) < 0xA) &&
          (( (x>>8)  & 0xE) < 0xA);
}

//after 199 comes 1A0, but if these pages exist, they contain no useful data, so filter them out
int TeletextBrowser::nextValidPageNumber(int start, Direction direction) {
   do {
      switch (direction) {
      case DirectionForward:
         start++;
         break;
      case DirectionBackward:
         start--;
         break;
      }
   } while (!onlyDecimalDigits(start));
   return start;
}

void TeletextBrowser::ChangePageRelative(Direction direction)
{
   int oldpage = currentPage;
   int oldSubPage = currentSubPage;

   do  {
      /*if (back)
         currentPage--;
      else
         currentPage++;*/
      currentPage=nextValidPageNumber(currentPage, direction);
      if (currentPage>0x899) currentPage=0x100;
      if (currentPage<0x100) currentPage=0x899;
      // sub page is always 0 if you change the page
      if (CheckFirstSubPage(0)) {
         SetPreviousPage(oldpage, oldSubPage, currentPage);
         return;
      }
   } while (currentPage != oldpage);

   return;
}

void TeletextBrowser::ChangeSubPageRelative(Direction direction)
{
   int oldsubpage = currentSubPage;

   do  {
      /*if (back)
         currentSubPage--;
      else
         currentSubPage++;*/
      currentSubPage=nextValidPageNumber(currentSubPage, direction);
      if (currentSubPage > 0x99) currentSubPage=0;
      if (currentSubPage < 0) currentSubPage=0x99;

      if (CheckPage())
         return;
   } while (currentSubPage != oldsubpage);

   return;
}

bool TeletextBrowser::CheckFirstSubPage(int startWith) {
   int oldsubpage = currentSubPage;

   do  {
      if (CheckPage())
         return true;
      //currentSubPage++;
      currentSubPage=nextValidPageNumber(currentSubPage, DirectionForward);
      
      if (currentSubPage > 0x99) currentSubPage=0;
      if (currentSubPage < 0) currentSubPage=0x99;

   } while (currentSubPage != oldsubpage);
   
   return false;
}

bool TeletextBrowser::CheckPage()
{
   StorageHandle fd;
   
   Storage *s=Storage::instance();
   if (!(fd=s->openForReading(PageID(channel, currentPage, currentSubPage), false)) )
      return false;

   s->close(fd);
   return true;
}

//sets the previousPage variables if and only if new page is different from old page
void TeletextBrowser::SetPreviousPage(int oldPage, int oldSubPage, int newPage)  {
   if (oldPage != newPage) {
      previousPage=oldPage;
      previousSubPage=oldSubPage;
   }
}




void TeletextBrowser::ShowPage() {
   if ((pageFound=DecodePage())) {
      if (ttSetup.autoUpdatePage)
         checkSum=PageCheckSum();
   }
}

void TeletextBrowser::ShowPageNumber() {
   char str[8];
   sprintf(str, "%3x-%02x", currentPage, currentSubPage);
   if (cursorPos>0) {
      str[2]='*';
      if (cursorPos==1)
         str[1]='*';
   }
   Display::DrawPageId(str);
}

void TeletextBrowser::ShowAskForChannel() {
   if (selectingChannel) {
      cString str = cString::sprintf(selectingChannelNumber > 0 ? "%s%d" : "%s", tr("Channel (press OK): "), selectingChannelNumber);
      Display::DrawMessage(str);
   }
}

//this is taken and adapted from the teletext plugin since it uses its data
bool TeletextBrowser::DecodePage() {
   // Load the page and decodes it
   unsigned char cache[40*24+12];
   StorageHandle fd;
   // Take a look if there is a xxx-00 page
   Storage *s=Storage::instance();
   if (currentSubPage==0) {
      if ( !(fd=s->openForReading(PageID(channel, currentPage,currentSubPage), false)) ) {
         // There is no subpage 0 so look if there is subpage 1
         currentSubPage++;
         // Generate file string
      } else {
         // yes file exists
         s->close(fd);
      }
   }
   
   if ( (fd=s->openForReading(PageID(channel, currentPage, currentSubPage), true)) )
   {
      s->read(cache,sizeof cache,fd); // Read full page data
      s->close(fd);
      
      Display::HoldFlush();
      Display::ClearMessage();
      Display::RenderTeletextCode(cache);
      ShowPageNumber();
      UpdateClock();
      Display::ReleaseFlush();
   } else {
      // page doesn't exist
      currentSubPage--;

      Display::HoldFlush();
      ShowPageNumber();
      char str[80];
      snprintf(str,80, "%s %3x-%02x %s",tr("Page"),currentPage, currentSubPage,tr("not found"));
      Display::DrawMessage(str);
      Display::ReleaseFlush();

      return false;
   }
   return true;
}



int TeletextBrowser::PageCheckSum() {
   int retSum=0;
   StorageHandle fd;
   
   CheckFirstSubPage(currentSubPage);
   
   Storage *s=Storage::instance();
   if ((fd=s->openForReading(PageID(channel, currentPage, currentSubPage), false)) ) {
      uchar cache[960];
      s->read(cache, 12, fd); //skip
      s->read(cache, sizeof(cache), fd);
      s->close(fd);
      memset(cache+12, 0, 8); //it seems that there the clock is transmitted, ignore changes
      for (uint i=0;i<sizeof(cache); i++)
         retSum+=cache[i];
   }
   return retSum;
}

void TeletextBrowser::UpdateClock() {
   if ( ttSetup.showClock )
      Display::DrawClock();
}

TeletextSetup ttSetup;

TeletextSetup::TeletextSetup() {
   //Set default values for setup options
   
   configuredClrBackground=clrGray50;
   
   //init key bindings
   for (int i=0;i<10;i++)
      mapKeyToAction[0]=(eTeletextAction)0;
   mapKeyToAction[3]=Zoom;
   mapKeyToAction[2]=HalfPage;   
   mapKeyToAction[0]=SwitchChannel;
   
   showClock=true;
   suspendReceiving=false;
   autoUpdatePage=true;
   //OSDHeight+width default values given in Start()
   OSDHAlign=50;
   OSDVAlign=50;
   
   //use the value set for VDR's min user inactivity.
   //Initially this value could be changed via the plugin's setup, but I removed that
   //because there is no advantage, but a possible problem when VDR's value is change
   //after the plugin has stored its own value.
   inactivityTimeout=Setup.MinUserInactivity;
}


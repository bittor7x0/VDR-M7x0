/*
 * pilotosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include "pilotosd.h"
#include "config.h"
#include <vdr/config.h>
#include <vdr/device.h>
#include "symbols/up.xpm"
#include "symbols/down.xpm"

extern int PilotChannelNumber;

cPilotOsd::cPilotOsd(void)
{
  osd = NULL;
  group = -1;
  number = 0;
  lastTime = cTimeMs::Now();
  extraInfo = false;
  offset = 0;
  lines = 0;
  Present = NULL;
  Following = NULL;
  pArray = NULL;
  currentEvent = 0;
}
 
cPilotOsd::~cPilotOsd(void)
{
  delete osd;
}
void cPilotOsd::DrawSymbol(int x, int y, char *SymbolName[], tColor ColorFg)
{
  cBitmap bm(SymbolName);
  osd->DrawBitmap(x, y, bm, ColorFg, pal[0]);
}

void cPilotOsd::DisplayChannel(const cChannel *Channel)
{
  int BufSize = 255;
  if (Channel) {
     if (Channel->GroupSep())
        snprintf(ChanName, BufSize, "* %s *", Channel->Name());
     else
        snprintf(ChanName, BufSize, "%d%s %s", Channel->Number(), number ? "-" : "", Channel->Name());
     }
  else if (number)
     snprintf(ChanName, BufSize, "%d-", number);
  else
     snprintf(ChanName, BufSize, "%s", " *** Invalid Channel *** ");
  // Draw background for channel name
  osd->DrawRectangle(2, 2, config.width-2, LINEHEIGHT-2, pal[3]);
  // Draw channel number and name
  osd->DrawText(10, 3, ChanName,pal[2],pal[3],font);
  DisplayBitmap();
}

static int CompareEventTime(const void *p1, const void *p2)
{
  return (int)((*(cEvent **)p1)->StartTime() - (*(cEvent **)p2)->StartTime());
}

void cPilotOsd::UpdateEPGInfo(int NowNextPrev)
{
    // Start to reset all previous info
    textPresentInfo = NULL;
    textFollowingInfo = NULL;
    titlePresentInfo = NULL;
    titleFollowingInfo = NULL;
    subtitlePresentInfo = NULL;
    subtitleFollowingInfo = NULL;
    cSchedulesLock SchedulesLock;
    const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock);
    if (Schedules) {
      const cSchedule *Schedule = Schedules->GetSchedule(Channels.GetByNumber(PilotChannelNumber)->GetChannelID());
      if (Schedule) {
        // Get Present event
        time_t now = time(NULL);
        int a=0;
        int num = ( Schedule->Events() )->Count();
	switch (NowNextPrev) {
	  case 1: // Now
            // Free previous Array
            Present = NULL;
            Following = NULL;
            if (pArray) {
              free(pArray);
              pArray = NULL;
            }
            pArray = MALLOC(const cEvent *, num);
            if (pArray) {
              int numreal = 0;
              for (int a = 0; a < num; a++) {
                const cEvent *EventInfo = ( Schedule->Events() )->Get(a);
                // if (EventInfo->GetTime() + EventInfo->GetDuration() > now)
                    pArray[numreal++] = EventInfo;
              }
              qsort(pArray, numreal, sizeof(cEvent *), CompareEventTime);
            }
            // Find the current event
            while ( (a < num) && ( (pArray[a])->StartTime() + (pArray[a])->Duration() < now ) ) {
              a++;
            }
            currentEvent = a;
            if (a<num) {
              Present = pArray[currentEvent];
              if (currentEvent + 1 < num)
                Following = pArray[currentEvent + 1];
            }
            break;
	  case 2: // Next
            if (currentEvent + 1 < num) {
              Present = NULL;
              Following = NULL;
              currentEvent++;
              Present = pArray[currentEvent];
              if (currentEvent + 1 < num)
                Following = pArray[currentEvent + 1];
            }
            break;
	  case 3: // Prev
            if (currentEvent != 0) {
              Following = Present;
              if (currentEvent  > 0) {
                currentEvent--;
                Present = pArray[currentEvent];
              }
            }
            break;
	}
        // Get Event details
        if (Present) {
          // printf("[%d] PRESENT:", currentEvent);
          titlePresentInfo = Present->Title();
          subtitlePresentInfo = Present->ShortText();
          textPresentInfo = Present->Description();
        }
        // Get Following event
        if (Following) {
          // printf("[%d] FOLLOWING:", currentEvent+1);
          titleFollowingInfo = Following->Title();
          subtitleFollowingInfo = Following->ShortText();
          textFollowingInfo = Following->Description();
	  // printf("  --->%s\n", Following->GetTitle());
        }
      }
    }
}

void cPilotOsd::Show()
{
  // find the actual current
  cChannel *Channel = Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
  if (Channel) {
    PilotChannelNumber = Channel->Number();
    sprintf(ChanName, 
            "%d %-16.16s", 
	    (Channels.GetByNumber(PilotChannelNumber))->Number(), 
	    (Channels.GetByNumber(PilotChannelNumber))->Name()); 
  osd=cOsdProvider::NewOsd(config.originx, config.originy);
    if (osd) {

    tArea Area = { 0, 0, config.width-1, config.height-1, 4};
    osd->SetAreas(&Area, 1);
      SendPalette();
      UpdateEPGInfo(1);
      DrawMenu(0,0);
      DisplayBitmap();
    }     
  } else {
    isyslog("Can't open pilot window: no current channel on primary device\n");
  }
}

eOSState cPilotOsd::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key) {
      case kBack:
         if (extraInfo == false) {
           return osEnd;
	 } else {
           // Display normal info
           extraInfo = false;
           DisplayBitmap();
           DrawMenu(0,0);
           DisplayBitmap();
           return osContinue;
         }
      case k0:
         if (extraInfo == false) {
           if (number == 0) {
              // keep the "Toggle channels" function working
              // FIXME Well, it doesn't :)
//              cRemote::Put(Key);
              return osContinue;
              }
         }
      case k1 ... k9:
         if (extraInfo == false) {
           if (number >= 0) {
              number = number * 10 + Key - k0;
              if (number > 0) {
                 cChannel *channel = Channels.GetByNumber(number);
	         // Display the current channel
                 DisplayChannel(channel);
                 lastTime = cTimeMs::Now();
                 }
              }
	 }
         break;
      case kLeft|k_Repeat:
      case kLeft:
      case kRight|k_Repeat:
      case kRight:
	 if (extraInfo == false) {
           if (group < 0) {
              cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
              if (channel)
                 group = channel->Index();
              }
           if (group >= 0) {
              int SaveGroup = group;
              if (NORMALKEY(Key) == kRight) {
                 group = Channels.GetNextGroup(group);
               } else {
                 group = Channels.GetPrevGroup(group < 1 ? 1 : group);
               }
              if (group < 0)
                 group = SaveGroup;
              cChannel *channel = Channels.Get(group);
              if (channel) {
                 //DisplayChannel(channel);
                 if (!channel->GroupSep())
                   group = -1;
		 else {
                   const char *groupName;
		   groupName = channel->Name();
                   channel = (cChannel *)channel->Next();
                   while (channel->GroupSep()) {
                     channel = (cChannel *)channel->Next();
                   }
		   PilotChannelNumber = channel->Number();
                   sprintf(ChanName, "%d %s/%-16.16s", 
                      (Channels.GetByNumber(PilotChannelNumber))->Number(),
		      groupName,
                      (Channels.GetByNumber(PilotChannelNumber))->Name());
                   UpdateEPGInfo(1);
                   DrawMenu(-16,1);
                   DisplayBitmap();
                 }
              }
           }
	    lastTime = cTimeMs::Now();
	}
        break;
      case kUp|k_Repeat:
      case kUp: {
	 if (extraInfo == false) {
           CursorUp();
           cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
           if (channel)
              group = channel->Index()-1;
	 } else {
           // Scroll extra info up
	   if (offset > 0) {
	     offset--;
             extraInfo = true;
             DisplayExtraInfo();
             DisplayBitmap();
             return osContinue;
	   }
         }
         break;
        }
      case kDown|k_Repeat:
      case kDown: {
	 if (extraInfo == false) {
           CursorDown();
           cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
           if (channel)
              group = channel->Index()-1;
	 } else {
           // Scroll extra info dozn
	   if (NBLINES + offset < lines) {
	     offset++;
             extraInfo = true;
             DisplayExtraInfo();
             DisplayBitmap();
             return osContinue;
           }
         }
         break;
         }
      case kNone:
	 if (extraInfo == false) {
	    if (number && (int)cTimeMs::Now() - lastTime > 1000) {
              if (Channels.GetByNumber(number)) {
                PilotChannelNumber = number;
                sprintf(ChanName, 
			"%d %-16.16s", 
			(Channels.GetByNumber(PilotChannelNumber))->Number(), 
			(Channels.GetByNumber(PilotChannelNumber))->Name()); 
		lastTime = cTimeMs::Now();
	        number = 0;
                cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
                if (channel)
                  group = channel->Index()-1;
	      } else {
                number = 0;
		 lastTime = cTimeMs::Now();
                return osContinue;
              }
              DisplayBitmap();
              UpdateEPGInfo(1);
              DrawMenu(0,0);
              DisplayBitmap();
              return osContinue;
            }
	  }
         break;
      case kOk:   
	 if (extraInfo == false) {
	   if (group>=-1) {
             number = 0;
             PilotChannelNumber = Channels.Get(Channels.GetNextNormal(group))->Number();
             sprintf(ChanName, 
                     "%d %-16.16s", 
		     (Channels.GetByNumber(PilotChannelNumber))->Number(), 
		     (Channels.GetByNumber(PilotChannelNumber))->Name()); 
             CursorOK();
             if (config.closeonswitch) return osEnd;
             else break;
           }
	} else {
          extraInfo = false;
          CursorOK();
          if (config.closeonswitch) return osEnd;
          else break;
	}
      case kRed: {
	 if ((extraInfo == false) && (!isempty(textPresentInfo))) {
           // Display extra infos
           extraInfo = true;
           offset = 0;
	   type = 1; // PRESENT
           DisplayExtraInfo();
           DisplayBitmap();
           return osContinue;
         } else {
           // Display normal info
           extraInfo = false;
           DisplayBitmap();
           DrawMenu(0,0);
           DisplayBitmap();
           return osContinue;
	 }
      }
      break;
      case kGreen: {
	 if ((extraInfo == false) && (!isempty(textFollowingInfo))) {
           // Display extra infos
           extraInfo = true;
           offset = 0;
	   type = 2; // PRESENT
           DisplayExtraInfo();
           DisplayBitmap();
           return osContinue;
         } else {
           // Display normal info
           extraInfo = false;
           DisplayBitmap();
           DrawMenu(0,0);
           DisplayBitmap();
           return osContinue;
	 }
      }
      break;
      case kYellow|k_Repeat:
      case kYellow: {
	 if (extraInfo == false) {
           // Scroll back in time
           UpdateEPGInfo(3);
           DrawMenu(0,0);
           DrawSymbol(config.width - 25, 3+LINEHEIGHT, up_xpm, pal[7]);
           DisplayBitmap();
           usleep(100000);
           DrawSymbol(config.width - 25, 3+LINEHEIGHT, up_xpm, pal[4]);
           DisplayBitmap();
           return osContinue;
	 }
      }
      break;
      case kBlue|k_Repeat:
      case kBlue: {
	 if (extraInfo == false) {
           // Scroll back in time
           UpdateEPGInfo(2);
           DrawMenu(0,0);
           DrawSymbol(config.width - 25, config.height - 2 - LINEHEIGHT, down_xpm, pal[3]);
           DisplayBitmap();
           usleep(100000);
           DrawSymbol(config.width - 25, config.height - 2 - LINEHEIGHT, down_xpm, pal[5]);
           DisplayBitmap();
           return osContinue;
	 }
      }
      break;
      default:
         return state;
    };
    if (extraInfo == false) {
      if ((int)cTimeMs::Now() - lastTime < 5000) {
        DisplayBitmap();
        DrawMenu(0,0);
        DisplayBitmap();
        return osContinue;
      }
    }
  }
  return osContinue;
}

void cPilotOsd::SendPalette() {
  if (config.usedxr3 == 0) {
    // AARRGGBB starting from VDR 1.3.7
    pal[10]= 0xFFCC0000; // 70% Red
    pal[11]= 0x00CC00FF; // 70% Green
    pal[12]= 0xFF00FCFC;
    pal[13]= 0xFF00FCFC;
    pal[14]= 0xFF00FCFC;
    pal[15]= 0xFF00FCFC;
    // Main window color
    SetColor(1,   config.blue, config.green, config.red,  config.alpha1);
    // Line color
    SetColor(2,   0,   0,   0, config.alphaborder);
    // Channel name background, Time and titles color
    SetColor(3, 255, 255, 255, config.alpha1);
    // Yellow button
    SetColor(4,   0, 255, 255, config.alpha1);
    // Blue button
    SetColor(5, 255,   0,   0, config.alpha1);
    // Green button color
    SetColor(6,   0, 255,   0, config.alpha1);
    // Dark Yellow
    SetColor(7,   0,  50,  50, config.alpha1);
    // Darker color for second half background
    SetColor(8, (config.blue>50) ? config.blue-50 : 0, 
		(config.green>50) ? config.green-50 : 0,
		(config.red>50) ? config.red-50 : 0,
		config.alpha1);
    // Subtitle text
    SetColor(9,   0, 255, 255, config.alpha1);
  } else {
    // DXR3 workaround
    pal[0]= clrTransparent; 
    // Main window color
//    pal[1]= clrBackground; 
    // Line color
    pal[2]= clrBlack; 
    // Channel name background, Time and titles color
    pal[3]= clrWhite; 
    // Yellow button
    pal[4]= clrYellow; 
    // Blue button
    pal[5]= clrBlue; 
    // Green button color
    pal[6]= clrGreen; 
    // Black
    pal[7]= clrBlack; 
    // Darker color for second half background
    pal[8]= clrBlack; 
    // Subtitle text
    pal[9]= clrRed; 
    // Red button color
    pal[10]= clrRed;
    // AARRGGBB starting from VDR 1.3.7
    pal[11]= 0xFF00FCFC;
    pal[12]= 0xFF00FCFC;
    pal[13]= 0xFF00FCFC;
    pal[14]= 0xFF00FCFC;
    pal[15]= 0xFF00FCFC;
  }
}

void cPilotOsd::SetColor(int Index, int Red, int Green, int Blue, int Alpha) {
  if ((Index >= 0) && (Index <= 15) && (Red >= 0) && (Red <= 255) && (Blue >= 0) && (Blue <= 255) && (Green >= 0) && (Green <= 255)&& (Alpha >= 0) && (Alpha <= 255)) {
    pal[Index] = 0;
    pal[Index] += Alpha;
    pal[Index] <<= 8;
    pal[Index] += Blue;
    pal[Index] <<= 8;
    pal[Index] += Green;
    pal[Index] <<= 8;
    pal[Index] += Red;
  }
}

void cPilotOsd::ClearBitmap() {
  osd->DrawRectangle(0, 0, config.width, config.height, pal[0]);
}

void cPilotOsd::CursorUp() {
  int n = PilotChannelNumber + 1;
  cChannel *channel;
  channel = Channels.GetByNumber(n, 1);
  if (channel) {
    n = channel->Number();
    PilotChannelNumber = n;
    sprintf(ChanName, 
            "%d %-16.16s", 
	    (Channels.GetByNumber(PilotChannelNumber))->Number(), 
	    (Channels.GetByNumber(PilotChannelNumber))->Name()); 
    UpdateEPGInfo(1);
    DrawMenu(-16,1);
    DisplayBitmap();
  }
} 

void cPilotOsd::CursorDown() {
  int n = PilotChannelNumber - 1;
  cChannel *channel;
  channel = Channels.GetByNumber(n, -1);
  if (channel) {
    n = channel->Number();
    PilotChannelNumber = n;
    sprintf(ChanName, 
            "%d %-16.16s", 
	    (Channels.GetByNumber(PilotChannelNumber))->Number(), 
	    (Channels.GetByNumber(PilotChannelNumber))->Name()); 
    UpdateEPGInfo(1);
    DrawMenu(+16,1);
    DisplayBitmap();
  }
}

void cPilotOsd::CursorOK() {
  DrawMenu(0,2);
  DisplayBitmap();
  cChannel *Channel =  Channels.GetByNumber(PilotChannelNumber);
  if (Channel){
    cDevice::PrimaryDevice()->SwitchChannel(Channel, true);
  }
}

void cPilotOsd::DrawMenu(int delta, int highlight) {

  ClearBitmap();
  int height = config.height;

  // Draw main window
  osd->DrawRectangle(0, 0, config.width, height, pal[1]);
  osd->DrawRectangle(0, 0, config.width, LINEHEIGHT, pal[3]);
  osd->DrawRectangle(0, (3+LINEHEIGHT)*3-10, config.width, height, pal[8]);
  for(int i=0; i<config.width; i++)
  {
    // Draw Main rectangle and horizontal lines
    osd->DrawPixel(i, 0,                        pal[2]);
    osd->DrawPixel(i, 1,                        pal[2]);
    osd->DrawPixel(i, height-2,                 pal[2]);
    osd->DrawPixel(i, height-1,                 pal[2]);
  }

  // Draw vertical lines
  for(int i=0; i<height; i++)
  {
    osd->DrawPixel(0,       i,        pal[2]);
    osd->DrawPixel(1,       i,        pal[2]);
    osd->DrawPixel(config.width-2, i, pal[2]);
    osd->DrawPixel(config.width-1, i, pal[2]);
  }
  
  DisplayInfo(delta);
}

void cPilotOsd::DisplayBitmap() {
  osd->Flush();
}

// to display channel Infos ... 
void cPilotOsd::DisplayInfo(int delta) {
  
  const int t = 170;
  bool withInfo=true;
  if (withInfo) {
    // Draw channel number and name
  osd->DrawText(10, 3, ChanName, pal[2],pal[3],font);
    if (!isempty(titlePresentInfo)) {
  osd->DrawText(10, 3+LINEHEIGHT, Present->GetTimeString(), pal[3],pal[1],font);
  osd->DrawText(t, (3+LINEHEIGHT)*1, titlePresentInfo, pal[3],pal[1],font);
    }
    if (!isempty(textPresentInfo)) {
      // Draw Red button, on the side for non-DXR3 users and upper left to DXR3 users
      if (config.usedxr3==0)
        osd->DrawRectangle(t-16, (3+LINEHEIGHT)*1 + 8, t-6, (3+LINEHEIGHT)*1+18, pal[10]);
      else
        osd->DrawRectangle(config.width-46, 3+8, config.width-36, 3+18, pal[10]);
    }
    if (!isempty(subtitlePresentInfo)) {
      osd->DrawText(t, (3+LINEHEIGHT)*2-5, subtitlePresentInfo, pal[3], pal[1],font);
    }
    if (!isempty(titleFollowingInfo)) {
      char time[10]="";
      sprintf(time, "> %s", *Following->GetTimeString());
      osd->DrawText(70, 3+LINEHEIGHT, time, pal[3], pal[1],font);
      osd->DrawText(t, (3+LINEHEIGHT)*3, titleFollowingInfo, pal[3], pal[8],font);
    }
    if (!isempty(textFollowingInfo)) {
      // Draw Green button, on the side for non-DXR3 users and upper left to DXR3 users
      if (config.usedxr3==0)
        osd->DrawRectangle(t-16, (3+LINEHEIGHT)*3 + 8, t-6, (3+LINEHEIGHT)*3+18, pal[6]);
      else
        osd->DrawRectangle(config.width-26, 3+8, config.width-16, 3+18, pal[6]);
    }
    if (!isempty(subtitleFollowingInfo)) {
      osd->DrawText(t, (3+LINEHEIGHT)*4-5, subtitleFollowingInfo, pal[9],pal[8],font);
    }
  }

  // Display scrolling arrows
  osd->DrawRectangle(config.width - 28, 4+LINEHEIGHT, config.width-4, config.height-2, pal[1]);
  osd->DrawRectangle(config.width - 28, 2+LINEHEIGHT, config.width-2, (3+LINEHEIGHT)*2-2, pal[8]);
  osd->DrawRectangle(config.width - 28, config.height-2-LINEHEIGHT, config.width-4, config.height-2, pal[8]);
  // Draw arrows
  DrawSymbol(config.width - 25, 3+LINEHEIGHT, up_xpm, pal[4]);
  DrawSymbol(config.width - 25, config.height - 2 - LINEHEIGHT, down_xpm, pal[5]);
  for(int y=3+LINEHEIGHT; y < config.height-2; y++){
    osd->DrawPixel(config.width - 29, y, pal[2]);
    osd->DrawPixel(config.width - 30, y, pal[2]);
  }

  // Time bar
  if ( (Present) && (Present->StartTime() < time(NULL)) && ( time(NULL) <= Present->StartTime() + Present->Duration()) ) {
    int width;
    int height;
    float percent = 0;

    width=135;
    height=LINEHEIGHT/2;
    if (Present->Duration() != 0)
      percent =  (float)(time(NULL) - Present->StartTime()) / (float)(Present->Duration());
    if (percent < 0) percent = 0;
    if (percent > 1.0) percent = 1.0;
    // Draw the time bar
    for(int x=2; x<width-2; x++){
      osd->DrawPixel(10+x, (3+LINEHEIGHT)*2, pal[2]);
      osd->DrawPixel(10+x, (3+LINEHEIGHT)*2+1, pal[2]);
      osd->DrawPixel(10+x, (3+LINEHEIGHT)*2+height, pal[2]);
      osd->DrawPixel(10+x, (3+LINEHEIGHT)*2+height-1, pal[2]);
      for(int y=2; y<height-2; y++){
        if( x > (int)(percent*(float)width)) {
          // Elapsed part
          osd->DrawPixel(10+x, (3+LINEHEIGHT)*2+y, pal[3]);
        } else {
          // Remaining part
          osd->DrawPixel(10+x, (3+LINEHEIGHT)*2+y, pal[1]);
        }
      }
    }
    for(int y=(3+LINEHEIGHT)*2; y<(3+LINEHEIGHT)*2+height; y++){
      osd->DrawPixel(10, y, pal[2]);
      osd->DrawPixel(10+1, y, pal[2]);
      osd->DrawPixel(10+width, y, pal[2]);
      osd->DrawPixel(10+width-1, y, pal[2]);
    }
  }
}

void cPilotOsd::DisplayExtraInfo() {
  int height = config.height;
  char *text = NULL;
  int l = 0;
  char *t = NULL;
  
  // Clear channel name 
  osd->DrawRectangle(2, 2, config.width-2, LINEHEIGHT-2,pal[3]);

// Display Title
if (type == 1)
    osd->DrawText(10, 3, titlePresentInfo, pal[2], pal[3],font);
  else if (type == 2)
    osd->DrawText(10, 3, titleFollowingInfo, pal[2], pal[3],font);
  else 
    osd->DrawText(10, 3, "-", pal[2], pal[3],font);


  // Clean channel info
  osd->DrawRectangle(2, LINEHEIGHT+2, config.width-2, height-2, pal[1]);
  // Display close window symbol
  osd->DrawText(config.width - 25, 2, "X", pal[10],pal[1],font);
  if (type == 1) 
    text = WrapText(textPresentInfo, config.width - 60, &lines);
  else if (type == 2)
    text = WrapText(textFollowingInfo, config.width - 60, &lines);
  else 
    osd->DrawText(10, 3, "-", pal[2], pal[1],font);
  t = text;
  while (*t) {
        char *n = strchr(t, '\n');
        if (l >= offset) {
           if (n)
              *n = 0;
           osd->DrawText(15, (3+LINEHEIGHT)*(1 + l - offset), t, pal[3], pal[1],font);
           if (n)
              *n = '\n';
           else
              break;
           }
        if (!n)
           break;
        t = n + 1;
        if (++l >= NBLINES + offset)
           break;
        }
  // Draw lift arrows
  if (offset > 0)   DrawSymbol(config.width - 25, 3+LINEHEIGHT, up_xpm, pal[9]);
  if (NBLINES + offset < lines) DrawSymbol(config.width - 25, height - 2 - LINEHEIGHT, down_xpm, pal[9]);
}

char * cPilotOsd::WrapText(const char *Text, int Width, int *Height) {
  int Lines = 1;
  char *t = strdup(Text);
  char *Blank = NULL;
  char *Delim = NULL;
  int w = 0;

  // Width *= cOsd::CellWidth();

  while (*t && t[strlen(t) - 1] == '\n')
        t[strlen(t) - 1] = 0; // skips trailing newlines

  for (char *p = t; *p; ) {
      if (*p == '|')
         *p = '\n';
      if (*p == '\n') {
         Lines++;
         w = 0;
         Blank = Delim = NULL;
         p++;
         continue;
         }
      else if (isspace(*p))
         Blank = p;
      int cw = font->Width(*p);
      if (w + cw > Width) {
         if (Blank) {
            *Blank = '\n';
            p = Blank;
            continue;
            }
         else {
            // Here's the ugly part, where we don't have any whitespace to
            // punch in a newline, so we need to make room for it:
            if (Delim)
               p = Delim + 1; // let's fall back to the most recent delimiter
            char *s = MALLOC(char, strlen(t) + 2); // The additional '\n' plus the terminating '\0'
            int l = p - t;
            strncpy(s, t, l);
            s[l] = '\n';
            strcpy(s + l + 1, p);
            free(t);
            t = s;
            p = t + l;
            continue;
            }
         }
      else
         w += cw;
      if (strchr("-.,:;!?_", *p)) {
         Delim = p;
         Blank = NULL;
         }
      p++;
      }

  *Height = Lines;
  return t;
}

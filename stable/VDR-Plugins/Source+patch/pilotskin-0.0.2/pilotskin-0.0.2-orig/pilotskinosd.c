/*
 * pilotskinosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include "pilotskinosd.h"
#include "config.h"
#include <vdr/device.h>
#ifndef VDRVERSNUM
#include <vdr/config.h>
#endif


extern int PilotChannelNumber;

cPilotskinOsd::cPilotskinOsd(void)
:cOsdObject(true)
{
  osd = cOsdProvider::NewOsd(0, 0);
  //displayChannel = Skins.Current()->DisplayChannel(true);
  displayChannel = NULL;
  displayEvent = NULL;
  displayTimer = NULL;
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
 
cPilotskinOsd::~cPilotskinOsd(void)
{
  if (displayChannel)
  {
     delete displayChannel;
     displayChannel = NULL;
  }
  if (displayEvent)
  {
     delete displayEvent;
     displayEvent = NULL;
  }
  if (displayTimer)
  {
     delete displayTimer;
     displayTimer = NULL;
  }
  delete osd;
}

void cPilotskinOsd::DisplayChannel(const cChannel *Channel)
{
  int BufSize = 255;
  if (Channel) {
     if (Channel->GroupSep())
        snprintf(ChanName, BufSize, "* %s *", Channel->Name());
     else {
        snprintf(ChanNumber, BufSize, "%d%s", Channel->Number(), number ? "-" : "");
        snprintf(ChanName, BufSize, "%s", Channel->Name());
     }
   
  }
  DrawMenu(0,0);
  DisplayBitmap();
}

static int CompareEventTime(const void *p1, const void *p2)
{
  return (int)((*(cEvent **)p1)->StartTime() - (*(cEvent **)p2)->StartTime());
}

void cPilotskinOsd::UpdateEPGInfo(int NowNextPrev)
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

void cPilotskinOsd::Show()
{
  // find the actual current
   cChannel *Channel =  Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());

   if (Channel)
    {
      PilotChannelNumber = Channel->Number();
    }
  UpdateEPGInfo(1);
  DrawMenu(0,0);
  DisplayBitmap();
}

eOSState cPilotskinOsd::ProcessKey(eKeys Key)
{
 eOSState state;
 if (displayTimer)
  {
     state=displayTimer->ProcessKey(Key);
     if (state==osBack)
      {
           extraInfo = false;
	   DisplayBitmap();
           DrawMenu(0,0);
           DisplayBitmap();
           return osContinue;
      }
     else if (state == osUnknown)
      {
        switch (Key & ~k_Repeat)
         {
          case kOk:
          case kBack:
          case kRed:
           extraInfo = false;
	   DisplayBitmap();
           DrawMenu(0,0);
           DisplayBitmap();
          break;
         }
      }
   return osContinue;
  }
 else if (displayChannel || displayEvent)
 {
  state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key) {
      case kUser1 ... kUser9:
	 if ((NORMALKEY(Key) == kUser1 && config.red==1)
	   || (NORMALKEY(Key) == kUser2 && config.red==2)
	    || (NORMALKEY(Key) == kUser3 && config.red==3)
	     || (NORMALKEY(Key) == kUser4 && config.red==4)
	      || (NORMALKEY(Key) == kUser5 && config.red==5)
	       || (NORMALKEY(Key) == kUser6 && config.red==6))
	 {
           return osEnd;
         }
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
              //cRemote::Put(Key);
              return osContinue;
              }
         }
      case k1 ... k9:
         if (extraInfo == false) {
           if (number >= 0) {
              if (number > Channels.MaxNumber())
                 number = Key - k0;
              else
                 number = number * 10 + Key - k0;
              if (number > 0) {
                 cChannel *channel = Channels.GetByNumber(number);
                 if (channel) {
                   PilotChannelNumber = channel->Number();
                   UpdateEPGInfo(1);
                   displayChannel->SetChannel(channel, number);
                   displayChannel->SetEvents(NULL, NULL);
                   displayChannel->Flush();
                 }
                 else {
                   displayChannel->SetChannel(channel, number);
                   displayChannel->SetEvents(NULL, NULL);
                   displayChannel->Flush();
                 }
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
                   UpdateEPGInfo(1);
                   DrawMenu(-16,1);
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
	 }
         break;
       }
      case kNone:
	 if (extraInfo == false) {
	    //if (number && (int)cTimeMs::Now() - lastTime > 1000) {
            if (number && (int)cTimeMs::Now() - lastTime > 1000) {
              if (Channels.GetByNumber(number)) {
                PilotChannelNumber = number;
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
              UpdateEPGInfo(1);
              DrawMenu(0,0);
              return osContinue;
            }
	  }
         break;
      case kOk:   
	 if (extraInfo == false) {
	   // cn if (group>=-1) {
           //  number = 0;
           //  PilotChannelNumber = Channels.Get(Channels.GetNextNormal(group))->Number();
             CursorOK();
             if (config.closeonswitch) return osEnd;
             else break;
           //}
	} else {
          extraInfo = false;
          CursorOK();
          if (config.closeonswitch) return osEnd;
          else break;
	}
      case kRed: {
	 if (Present)
	  {
	     extraInfo = true;
	     DisplayTimer();
          }
	 return osContinue;
      }
      break;
      case kGreen: {
	 if (extraInfo == false)  {
           // Display extra infos
           extraInfo = true;
           offset = 0;
	   type = 2; // PRESENT
           DisplayExtraInfo();
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
           return osContinue;
	 }
      }
      break;
      default:
         return state;
    };
    if (extraInfo == false) {
      if ((int)cTimeMs::Now() - lastTime < 5000) {
        DrawMenu(0,0);
        return osContinue;
      }
    }
  }
 }
 return osContinue;
}

void cPilotskinOsd::CursorUp() {
  int n = PilotChannelNumber + 1;
  cChannel *channel;
  channel = Channels.GetByNumber(n, 1);
  if (channel) {
    n = channel->Number();
    PilotChannelNumber = n;
    UpdateEPGInfo(1);
    DrawMenu(-16,1);
  }
} 

void cPilotskinOsd::CursorDown() {
  int n = PilotChannelNumber - 1;
  cChannel *channel;
  channel = Channels.GetByNumber(n, -1);
  if (channel) {
    n = channel->Number();
    PilotChannelNumber = n;
    UpdateEPGInfo(1);
    DrawMenu(+16,1);
  }
}

void cPilotskinOsd::CursorOK() {
  DrawMenu(0,2);
  cChannel *Channel =  Channels.GetByNumber(PilotChannelNumber);
  if (Channel){
    cDevice::PrimaryDevice()->SwitchChannel(Channel, true);
  }
}

void cPilotskinOsd::DrawMenu(int delta, int highlight) {

  DisplayInfo(delta);

}

void cPilotskinOsd::DisplayBitmap() {
  // osd->Flush();
  if (displayChannel)
  {
     displayChannel->Flush();
  }
  else if (displayEvent)
  {
     displayEvent->Display();
  }
  else if (displayTimer)
  {
     displayTimer->Display();
  }
}

// to display channel Infos ... 
void cPilotskinOsd::DisplayInfo(int delta) {
  if (osd) {
     delete osd;
     osd = NULL;
     displayChannel = Skins.Current()->DisplayChannel(true);
   }

   if (displayEvent) {
      delete displayEvent;
      displayEvent = NULL;
      displayChannel = Skins.Current()->DisplayChannel(true);
   }
   else if (displayTimer) {
      delete displayTimer;
      displayTimer = NULL;
      displayChannel = Skins.Current()->DisplayChannel(true);
   }
   cChannel *channel = Channels.GetByNumber(PilotChannelNumber);

      if (!isempty(titlePresentInfo)) {
         displayChannel->SetChannel(channel, 0);
         displayChannel->SetEvents(Present, Following);
      }
      else {
         displayChannel->SetChannel(channel, 0);
         displayChannel->SetEvents(NULL, NULL);
      }
}

void cPilotskinOsd::DisplayExtraInfo() {
  if (displayChannel) {
      delete displayChannel;
      displayChannel = NULL;
  } else if (displayEvent) {
      delete displayEvent;
      displayEvent = NULL;
  }
  displayEvent = new cPilMenuEvent(Present);
  displayEvent->Display();
}

void cPilotskinOsd::DisplayTimer() {
  
  if (displayChannel) {
      delete displayChannel;
      displayChannel = NULL;
  }
  else if (displayEvent) {
      delete displayEvent;
      displayEvent = NULL;
  }

  cTimer *timer = new cTimer(Present);
  cTimer *t = Timers.GetTimer(timer);
  if (t) 
  {
      delete timer;
      timer = t;
  }
  displayTimer=new cMenuEditTimer(timer, !t);
  displayTimer->Display();
}


// ----------------------------------------------------- //
// -------------   cPilMenuEvent    -------------------- //
// ----------------------------------------------------- //


cPilMenuEvent::cPilMenuEvent(const class cEvent *EventInfo)
 : cMenuEvent(EventInfo,false)
{
	helpLine();
}

void cPilMenuEvent::printMsg(const char *s)
{
	SetStatus(s);
}
void cPilMenuEvent::helpLine(bool full)
{
	SetHelp( tr("Button$Record"), tr("Button$Close"), full?tr("Search"):NULL, full?tr("AutoTimer"):NULL);
}

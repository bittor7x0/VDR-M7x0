/*
 * zappilotosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/device.h>
#include <vdr/osdbase.h>
#include "zappilotosd.h"
#include "config.h"

extern int PilotChannelNumber;

// Data structure for service "Epgsearch-switchtimer-v1.0"
struct Epgsearch_switchtimer_v1_0
{
   // in
   const cEvent* event;
   int mode;                     // mode (0=query existance, 1=add/modify, 2=delete)
   // in/out
   int switchMinsBefore;
   int announceOnly;
   // out
   bool success;                 // result
};

cZappilotOsd::cZappilotOsd(void):cOsdObject(true)
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


cZappilotOsd::~cZappilotOsd(void)
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


void cZappilotOsd::DisplayChannel(const cChannel *Channel)
{
   int BufSize = 255;
   if (Channel)
   {
      if (Channel->GroupSep())
         snprintf(ChanName, BufSize, "* %s *", Channel->Name());
      else
      {
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


void cZappilotOsd::AddDelSwitchTimer(const cEvent *event)
{
   bool SwitchTimerExits = false;
   if (config.pEPGSearch && event)
   {
      Epgsearch_switchtimer_v1_0* serviceData = new Epgsearch_switchtimer_v1_0;
      serviceData->event = event;
      serviceData->mode = 0;
      if (config.pEPGSearch->Service("Epgsearch-switchtimer-v1.0", serviceData))
      {
         SwitchTimerExits=serviceData->success;
         delete serviceData;
      }
      else
      {
         esyslog("[ZapPilot] EPGSearch does not support Epgsearch-switchtimer-v1.0 service!");
         delete serviceData;

      }
      if (!SwitchTimerExits)
      {
         serviceData = new Epgsearch_switchtimer_v1_0;
         serviceData->event = event;
         serviceData->mode = 1;
         serviceData->switchMinsBefore = config.switchminsbefore;
         serviceData->announceOnly = 0;
         if (config.pEPGSearch->Service("Epgsearch-switchtimer-v1.0", serviceData))
         {
            if (serviceData->success)
            {
               Skins.Message(mtInfo, tr("Switch timer added!"));
               delete serviceData;
            }
         }
         else
         {
            esyslog("[ZapPilot] EPGSearch does not support Epgsearch-switchtimer-v1.0 service!");
            delete serviceData;
            return;
         }
      }
      else
      {
         serviceData = new Epgsearch_switchtimer_v1_0;
         serviceData->event = event;
         serviceData->mode = 2;
         serviceData->switchMinsBefore = 1;
         serviceData->announceOnly = false;
         if (config.pEPGSearch->Service("Epgsearch-switchtimer-v1.0", serviceData))
         {
            if (serviceData->success)
            {
               Skins.Message(mtInfo, tr("Switch timer deleted!"));
               delete serviceData;
            }
         }
         else
         {
            esyslog("[ZapPilot] EPGSearch does not support Epgsearch-switchtimer-v1.0 service!");
            delete serviceData;
            return;
         }
      }
   }
   else
   {
      esyslog("[ZapPilot] EPGSearch does not exist; switch timer is not possible!");
   }
}


void cZappilotOsd::UpdateEPGInfo(int NowNextPrev)
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
   if (Schedules)
   {
      const cSchedule *Schedule = Schedules->GetSchedule(Channels.GetByNumber(PilotChannelNumber)->GetChannelID());
      if (Schedule)
      {
         // Get Present event
         time_t now = time(NULL);
         int a=0;
         int num = ( Schedule->Events() )->Count();
         switch (NowNextPrev)
         {
            case 1:              // Now
               // Free previous Array
               Present = NULL;
               Following = NULL;
               if (pArray)
               {
                  free(pArray);
                  pArray = NULL;
               }
               pArray = MALLOC(const cEvent *, num);
               if (pArray)
               {
                  int numreal = 0;
                  for (int a = 0; a < num; a++)
                  {
                     const cEvent *EventInfo = ( Schedule->Events() )->Get(a);
                     // if (EventInfo->GetTime() + EventInfo->GetDuration() > now)
                     pArray[numreal++] = EventInfo;
                  }
                  qsort(pArray, numreal, sizeof(cEvent *), CompareEventTime);
               }
               // Find the current event
               while ( (a < num) && ( (pArray[a])->StartTime() + (pArray[a])->Duration() < now ) )
               {
                  a++;
               }
               currentEvent = a;
               if (a<num)
               {
                  Present = pArray[currentEvent];
                  if (currentEvent + 1 < num)
                     Following = pArray[currentEvent + 1];
               }
               break;
            case 2:              // Next
               if (currentEvent + 1 < num)
               {
                  Present = NULL;
                  Following = NULL;
                  currentEvent++;
                  Present = pArray[currentEvent];
                  if (currentEvent + 1 < num)
                     Following = pArray[currentEvent + 1];
               }
               break;
            case 3:              // Prev
               if (currentEvent != 0)
               {
                  Following = Present;
                  if (currentEvent  > 0)
                  {
                     currentEvent--;
                     Present = pArray[currentEvent];
                  }
               }
               break;
         }
         // Get Event details
         if (Present)
         {
            // printf("[%d] PRESENT:", currentEvent);
            titlePresentInfo = Present->Title();
            subtitlePresentInfo = Present->ShortText();
            textPresentInfo = Present->Description();
         }
         // Get Following event
         if (Following)
         {
            // printf("[%d] FOLLOWING:", currentEvent+1);
            titleFollowingInfo = Following->Title();
            subtitleFollowingInfo = Following->ShortText();
            textFollowingInfo = Following->Description();
            // printf("  --->%s\n", Following->GetTitle());
         }
      }
   }
}


void cZappilotOsd::Show()
{
   // find the actual current
   cChannel *Channel =  Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
   currentChannel = Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());

   if (Channel)
   {
      PilotChannelNumber = Channel->Number();
   }
   UpdateEPGInfo(1);
   DrawMenu(0,0);
   DisplayBitmap();
}


eOSState cZappilotOsd::ProcessKey(eKeys Key)
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
   else if (displayChannel )
   {
      switch (Key)
      {
         case kUser1 ... kUser9:
         {
            return osEnd;
         }
         case kBack:
         {
            return osEnd;
         }
         case k0:
         {
            if (number == 0 && config.switchtimer)
            {
               AddDelSwitchTimer(Present);
               return osContinue;
            }
         }
         case k1 ... k9:
            if (number >= 0)
            {
               if (number > Channels.MaxNumber())
                  number = Key - k0;
               else
                  number = number * 10 + Key - k0;
               if (number > 0)
               {
                  cChannel *channel = Channels.GetByNumber(number);
                  if (channel)
                  {
                     PilotChannelNumber = channel->Number();
                     UpdateEPGInfo(1);
                     displayChannel->SetChannel(channel, number);
                     displayChannel->SetEvents(NULL, NULL);
                     displayChannel->Flush();
                  }
                  else
                  {
                     displayChannel->SetChannel(channel, number);
                     displayChannel->SetEvents(NULL, NULL);
                     displayChannel->Flush();
                  }
                  lastTime = cTimeMs::Now();
               }
            }
            break;
         case kLeft|k_Repeat:
         case kLeft:
            if (config.fastbrowse)
            {
               CursorDown();
               cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
               if (channel)
                  group = channel->Index()-1;
               break;
            }
            else if (config.fastbrowsealt)
            {
               // Scroll back in time
               UpdateEPGInfo(3);
               DrawMenu(0,0);
               return osContinue;
            }
         case kRight|k_Repeat:
         case kRight:
            if (config.fastbrowse)
            {
               CursorUp();
               cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
               if (channel)
                  group = channel->Index()-1;
               break;
            }
            else if (config.fastbrowsealt)
            {
               // Scroll forward in time
               UpdateEPGInfo(2);
               DrawMenu(0,0);
               return osContinue;
            }
            else
            {
               if (group < 0)
               {
                  cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
                  if (channel)
                     group = channel->Index();
               }
               if (group >= 0)
               {
                  int SaveGroup = group;
                  if (NORMALKEY(Key) == kRight)
                  {
                     group = Channels.GetNextGroup(group);
                  }
                  else
                  {
                     group = Channels.GetPrevGroup(group < 1 ? 1 : group);
                  }
                  if (group < 0)
                     group = SaveGroup;
                  cChannel *channel = Channels.Get(group);
                  if (channel)
                  {
                     //DisplayChannel(channel);
                     if (!channel->GroupSep())
                        group = -1;
                     else
                     {
                        const char *groupName;
                        groupName = channel->Name();
                        channel = (cChannel *)channel->Next();
                        while (channel->GroupSep())
                        {
                           channel = (cChannel *)channel->Next();
                        }
                        PilotChannelNumber = channel->Number();
                        UpdateEPGInfo(1);
                        DrawMenu(-16,1);
                     }
                  }
               }
               lastTime = cTimeMs::Now();
               break;
            }
         case kUp|k_Repeat:
         case kUp:
            if (config.fastbrowse)
            {
               // Scroll back in time
               UpdateEPGInfo(3);
               DrawMenu(0,0);
               return osContinue;
            }
            else
            {
               CursorUp();
               cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
               if (channel)
                  group = channel->Index()-1;
               break;
            }
         case kDown|k_Repeat:
         case kDown:
            if (config.fastbrowse)
            {
               // Scroll forward in time
               UpdateEPGInfo(2);
               DrawMenu(0,0);
               return osContinue;
            }
            else
            {
               CursorDown();
               cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
               if (channel)
                  group = channel->Index()-1;
               break;
            }
         case kNone:
            if (number && (int)cTimeMs::Now() - lastTime > 1000)
            {
               if (Channels.GetByNumber(number))
               {
                  PilotChannelNumber = number;
                  lastTime = cTimeMs::Now();
                  number = 0;
                  cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
                  if (channel)
                     group = channel->Index()-1;
               }
               else
               {
                  number = 0;
                  lastTime = cTimeMs::Now();
                  return osContinue;
               }
               UpdateEPGInfo(1);
               DrawMenu(0,0);
               return osContinue;
            }
            break;
         case kOk:
         {
            // cn if (group>=-1) {
            //  number = 0;
            //  PilotChannelNumber = Channels.Get(Channels.GetNextNormal(group))->Number();
            CursorOK();
            if (config.closeonswitch) return osEnd;
            else break;
         }
         case kRed:
         {
            if (Present)
            {
               extraInfo = true;
               DisplayTimer();
            }
            return osContinue;
         }
         break;
         case kInfo:
         case kGreen:
         {
            if (Present)
            {
               // Display extra infos
               extraInfo = true;
               offset = 0;
               type = 2;         // PRESENT
               DisplayExtraInfo();
               return osContinue;
            }
         }
         break;
         case kYellow|k_Repeat:
         case kYellow:
         {
            if (!config.fastbrowse)
            {
               // Scroll back in time
               UpdateEPGInfo(3);
               DrawMenu(0,0);
               return osContinue;
               break;
            }
         }
         case kBlue|k_Repeat:
         case kBlue:
            if (!config.fastbrowse)
            {
               // Scroll back in time
               UpdateEPGInfo(2);
               DrawMenu(0,0);
               return osContinue;
               break;
            }
            else
            {
               if (group < 0)
               {
                  cChannel *channel = Channels.GetByNumber(PilotChannelNumber);
                  if (channel)
                     group = channel->Index();
               }
               if (group >= 0)
               {
                  int SaveGroup = group;
                  if (NORMALKEY(Key) == kBlue)
                  {
                     group = Channels.GetNextGroup(group);
                  }
                  else
                  {
                     group = Channels.GetPrevGroup(group < 1 ? 1 : group);
                  }
                  if (group < 0)
                     group = SaveGroup;
                  cChannel *channel = Channels.Get(group);
                  if (channel)
                  {
                     //DisplayChannel(channel);
                     if (!channel->GroupSep())
                        group = -1;
                     else
                     {
                        const char *groupName;
                        groupName = channel->Name();
                        channel = (cChannel *)channel->Next();
                        while (channel->GroupSep())
                        {
                           channel = (cChannel *)channel->Next();
                        }
                        PilotChannelNumber = channel->Number();
                        UpdateEPGInfo(1);
                        DrawMenu(-16,1);
                     }
                  }
               }
               lastTime = cTimeMs::Now();
               break;
            }
         default:
            return state;
      };
      if ((int)cTimeMs::Now() - lastTime < 5000)
      {
         DrawMenu(0,0);
         return osContinue;
      }
   }
   else if (displayEvent)
   {
      switch (Key)
      {
         case kUser1 ... kUser9:
         {
            return osEnd;
         }
         case kBack:
         {
            // Display normal info
            extraInfo = false;
            DisplayBitmap();
            DrawMenu(0,0);
            DisplayBitmap();
            return osContinue;
         }
         case kOk:
         {
            extraInfo = false;
            CursorOK();
            if (config.closeonswitch) return osEnd;
            else break;
         }
         case kRed:
         {
            if (Present)
            {
               extraInfo = true;
               DisplayTimer();
            }
            return osContinue;
            break;
         }
         case kInfo:
         case kGreen:
         {
            if (Present)
            {
               // Display normal info
               extraInfo = false;
               DisplayBitmap();
               DrawMenu(0,0);
               DisplayBitmap();
               return osContinue;
            }
            break;
         }
         default:
            state = displayEvent->ProcessKey(Key);
            return state;
      };
   }
   return osContinue;
}


void cZappilotOsd::CursorUp()
{
   int n = PilotChannelNumber + 1;
   cChannel *channel;
   channel = Channels.GetByNumber(n, 1);
   if (channel)
   {
      n = channel->Number();
      PilotChannelNumber = n;
      UpdateEPGInfo(1);
      DrawMenu(-16,1);
   }
}


void cZappilotOsd::CursorDown()
{
   int n = PilotChannelNumber - 1;
   cChannel *channel;
   channel = Channels.GetByNumber(n, -1);
   if (channel)
   {
      n = channel->Number();
      PilotChannelNumber = n;
      UpdateEPGInfo(1);
      DrawMenu(+16,1);
   }
}


void cZappilotOsd::CursorOK()
{
   DrawMenu(0,2);
   if (currentChannel != Channels.GetByNumber(PilotChannelNumber))
   {
     cChannel *Channel =  Channels.GetByNumber(PilotChannelNumber);
     if (Channel)
     {
        cDevice::PrimaryDevice()->SwitchChannel(Channel, true);
     }
   }
}


void cZappilotOsd::DrawMenu(int delta, int highlight)
{
   DisplayInfo(delta);
}


void cZappilotOsd::DisplayBitmap()
{
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
void cZappilotOsd::DisplayInfo(int delta)
{
   if (osd)
   {
      delete osd;
      osd = NULL;
      displayChannel = Skins.Current()->DisplayChannel(true);
   }

   if (displayEvent)
   {
      delete displayEvent;
      displayEvent = NULL;
      displayChannel = Skins.Current()->DisplayChannel(true);
   }
   else if (displayTimer)
   {
      delete displayTimer;
      displayTimer = NULL;
      displayChannel = Skins.Current()->DisplayChannel(true);
   }
   cChannel *channel = Channels.GetByNumber(PilotChannelNumber);

   if (!isempty(titlePresentInfo))
   {
      displayChannel->SetChannel(channel, 0);
      displayChannel->SetEvents(Present, Following);
   }
   else
   {
      displayChannel->SetChannel(channel, 0);
      displayChannel->SetEvents(NULL, NULL);
   }
}


void cZappilotOsd::DisplayExtraInfo()
{
   if (displayChannel)
   {
      delete displayChannel;
      displayChannel = NULL;
   }
   else if (displayEvent)
   {
      delete displayEvent;
      displayEvent = NULL;
   }

   displayEvent = new cMenuEvent(Present,false,true);
   displayEvent->Display();
}


void cZappilotOsd::DisplayTimer()
{

   if (displayChannel)
   {
      delete displayChannel;
      displayChannel = NULL;
   }
   else if (displayEvent)
   {
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

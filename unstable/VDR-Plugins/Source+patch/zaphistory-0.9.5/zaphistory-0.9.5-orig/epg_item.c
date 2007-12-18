/*
 * epg_item.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "epg_item.h"
#include <math.h>
#include <vdr/menu.h>

#define HOURS(x) ((x)/100)
#define MINUTES(x) ((x)%100)

#define SHORTTEXT(EVENT) \
  ((EVENT) && !isempty((EVENT)->ShortText())) ? " ~ ":"", \
  ((EVENT) && !isempty((EVENT)->ShortText())) ? (EVENT)->ShortText():""

//extern int exitToMainMenu;

// --- cMenuMyScheduleItem ------------------------------------------------------
cMenuMyScheduleItem::cMenuMyScheduleItem(const cEvent *Event, cZapHistoryChannel *Channel, int Progress)
    :cZapHistoryOsdItem(Channel)
{
  event = Event;
  progress = Progress;
  timerMatch = 0;
  Update(true);
}

bool cMenuMyScheduleItem::Update(bool Force)
{
  bool result = false;
  const cChannel* channel = zapChannel->GetChannel();
  
  int OldTimerMatch = timerMatch;
  bool hasMatch = false;
  
  if (event)
    hasMatch = Timers.GetMatch(event, &timerMatch);

  if (Force || timerMatch != OldTimerMatch) 
  {
      char szChannelpart[20] = "";
      if (channel)
	  sprintf(szChannelpart, "%s\t", channel->Name() );
      
      char szProgressPart[12] = "";
      if (progress > 0 && channel)
      {
	  strcpy(szProgressPart, "\t");
	  if (event)
	  {
	      char szProgress[9] = "";
	      int frac = (int)roundf( (float)(time(NULL) - event->StartTime()) / (float)(event->Duration()) * 8.0 );
	      frac = min(8,max(0, frac));
	      
	      for(int i = 0; i < frac; i++)
		  szProgress[i] = (progress == 1 ? '|' : 127);
	      szProgress[frac]=0;
	      sprintf(szProgressPart, "%c%-8s%c\t", progress==1?'[':128, szProgress, progress==1?']':129);
	  }
      }
      
      char t = event && hasMatch ? (timerMatch == tmFull) ? 'T' : 't' : ' ';
      char v = event && event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
      char r = event && event->IsRunning() ? '*' : ' ';
      
      char szEventDescr[100] = "";
      snprintf(szEventDescr, 100, "%s%s%s", 
	      event?event->Title():tr(">>> no info! <<<"), 
	      SHORTTEXT(event) );

      char *buffer = NULL;
      if (channel) // menu "What's on"
	  asprintf(&buffer, "%s%s\t%s %c%c%c \t%s", 
		   szChannelpart,
		   event?*(event->GetTimeString() ):"", 
		   szProgressPart,
		   t, v, r, 
		   szEventDescr);
		   
      SetText(buffer, false);
      return true;
  }
  return result;
}

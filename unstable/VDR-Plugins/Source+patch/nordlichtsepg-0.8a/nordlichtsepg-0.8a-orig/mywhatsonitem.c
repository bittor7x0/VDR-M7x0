/*
 * See the README file for copyright information and how to reach the author.
 */

#include <math.h>
#include "mywhatsonitem.h"

myWhatsOnItem::myWhatsOnItem(const cEvent *Event,cChannel *Channel,bool Next)
{
 event=Event;
 channel=Channel;
 next=Next;
 
 Set();
}

void myWhatsOnItem::Set()
{
 int i;
 char *buffer=NULL,m=' ';
 char ProgressBar[11]={130,131,131,131,131,131,131,131,131,132};
 if(enigmatheme)
  strcpy(ProgressBar,"[        ]");
 timer=NULL;
 
 // look for timers
 for(cTimer *ti=Timers.First();ti;ti=Timers.Next(ti))
 {
  if(ti->Matches(t)&&(ti->Channel()==channel))
  {
   timer=ti;
   m=timer->Recording()?char(128):char(129);
  }
 }

 if(event)
 {
  // calculate progress bar
  int progress=(int)roundf((float)(time(NULL)-event->StartTime())/(float)(event->Duration())*10.0);

  if(progress<0)
   progress=0;
  else
   if(progress>9)
    progress=10;

  // assemble menu item text
  if(enigmatheme)
  {
   for(i=1;i<progress-1;i++)ProgressBar[i]='|';
   if(showchannelnumbers)
    asprintf(&buffer,"%c\t%d\t%-10s \t%s\t %s",m,channel->Number(),channel->ShortName(true),(!(event->RunningStatus()==4)&&next)?*event->GetTimeString():ProgressBar,event->Title());
   else
    asprintf(&buffer,"%c\t%-10s \t%s\t %s",m,channel->ShortName(true),(!(event->RunningStatus()==4)&&next)?*event->GetTimeString():ProgressBar,event->Title());
  }
  else
  {
   for(i=0;i<progress;i++)ProgressBar[i]=127;
   if(showchannelnumbers)
    asprintf(&buffer,"%c\t%d\t%-10s\t %s\t%s",m,channel->Number(),channel->ShortName(true),(!(event->RunningStatus()==4)&&next)?*event->GetTimeString():ProgressBar,event->Title());
   else
    asprintf(&buffer,"%c\t%-10s\t %s\t%s",m,channel->ShortName(true),(!(event->RunningStatus()==4)&&next)?*event->GetTimeString():ProgressBar,event->Title());
  }
 }
 else
 {
  if(enigmatheme)
  {
   if(showchannelnumbers)
    asprintf(&buffer,"%c\t%d\t%-10s\t \t (%s)",m,channel->Number(),channel->ShortName(true),tr("no info"));
   else
    asprintf(&buffer,"%c\t%-10s\t \t (%s)",m,channel->ShortName(true),tr("no info"));
  }
  else
  {
   if(showchannelnumbers)
    asprintf(&buffer,"%c\t%d\t%-10s\t \t(%s)",m,channel->Number(),channel->ShortName(true),tr("no info"));
   else
    asprintf(&buffer,"%c\t%-10s\t \t(%s)",m,channel->ShortName(true),tr("no info"));
  }
 }

 SetText(buffer,false);
}

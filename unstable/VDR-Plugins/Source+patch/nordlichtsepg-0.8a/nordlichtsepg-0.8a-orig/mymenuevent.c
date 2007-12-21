/*
 * See the README file for copyright information and how to reach the author.
 */

#include "mymenuevent.h"

extern int keeposd;
extern time_t t;
char Title[32];

myMenuEvent::myMenuEvent(myWhatsOnItem *Item):cOsdMenu(tr("Event"))
{
 item=Item;
 event=item->event;
 timer=item->timer;
 channel=item->channel;

 SetTitle(channel->Name());
 if(timer)
  SetHelp(tr("Button$Edit T."),tr("Button$Delete T."),NULL,tr("Button$Switch"));
 else
  SetHelp(tr("Button$Record"),NULL,NULL,tr("Button$Switch"));
}

void myMenuEvent::Display()
{
 cOsdMenu::Display();

 if(event)
 {
  DisplayMenu()->SetEvent(event);
  cStatus::MsgOsdTextItem(event->Description());
 }
 else
  DisplayMenu()->SetText(tr("no info"),0);
}

eOSState myMenuEvent::Switch()
{
 const cChannel *channel=item->channel;
 if(channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
  return keeposd?osContinue:osEnd;
 Skins.Message(mtError, tr("Can't switch channel!"));
 return osContinue;
}

eOSState myMenuEvent::Record()
{
 // if a timer exists, edit it
 if(timer)
  return AddSubMenu(new cMenuEditTimer(timer));
 else
 {
  // we have to create a new one
  if(event)
  {
   timer=new cTimer(event);
   Timers.Add(timer);
   timer->Matches();
   Timers.SetModified();
  }
  else
  {
   // this is a hack to create a timer for channels without events
   char *buffer;
   int starthh,startmm,stophh,stopmm;
   
   struct tm tm_t;
   localtime_r(&t,&tm_t);
   
   sscanf(*TimeString(t),"%2d:%2d",&starthh,&startmm);
   sscanf(*TimeString(t+Setup.InstantRecordTime*60),"%2d:%2d",&stophh,&stopmm);
   
   asprintf(&buffer,"1:%d:%d:%d%d:%d%d:%d:%d:%s",item->channel->Number(),tm_t.tm_mday,starthh,startmm,stophh,stopmm,Setup.DefaultPriority,Setup.DefaultLifetime,item->channel->Name());
   
   timer=new cTimer(false,false);
   timer->Parse(buffer);
   free(buffer);
   
   // we start a cMenuEditTimer because there are no reasonable values
   return AddSubMenu(new cMenuEditTimer(timer,true));
  }
 }
 SetHelp(tr("Button$Edit T."),tr("Button$Delete T."),NULL,tr("Button$Switch"));
 // update item text
 item->Set();
 return osContinue;
}

eOSState myMenuEvent::Delete()
{
 if(Interface->Confirm(tr("Delete timer?")))
 {
  if(timer->Recording())
  {
   if(Interface->Confirm(tr("Timer still recording - really delete?")))
   {
    timer->Skip();
    cRecordControls::Process(time(NULL));
   }
   else
    return osContinue;
  }
  Timers.Del(timer);
  Timers.SetModified();
  timer=NULL;

  SetHelp(tr("Button$Record"),NULL,NULL,tr("Button$Switch"));
 }
 // update item text
 item->Set();
 return osContinue;
}

eOSState myMenuEvent::ProcessKey(eKeys Key)
{
 if(!HasSubMenu())
 {
  switch(Key)
  {
   case kUp|k_Repeat:
   case kUp:
   case kDown|k_Repeat:
   case kDown:
   case kLeft|k_Repeat:
   case kLeft:
   case kRight|k_Repeat:
   case kRight:
    DisplayMenu()->Scroll(NORMALKEY(Key)==kUp||NORMALKEY(Key)==kLeft,NORMALKEY(Key)==kLeft||NORMALKEY(Key)==kRight);
    cStatus::MsgOsdTextItem(NULL,NORMALKEY(Key)==kUp);
    return osContinue;
   default: break;
  }
 }
 
 eOSState state=cOsdMenu::ProcessKey(Key);

 if(state==osUnknown)
 {
  switch(Key)
  {
   case kRecord:
   case kRed:return Record();
   case kGreen:if(timer)return Delete();
   case kYellow:return osContinue;
   case kBlue:return Switch();
   case kOk:return osBack;
   default:break;
  }
 }
 return state;
}

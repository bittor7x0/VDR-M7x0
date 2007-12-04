/*
 * See the README file for copyright information and how to reach the author.
 */

#include "nordlichtsepg.h"
#include "mymenuevent.h"
#include "patchfont.h"

#define CHNUMWIDTH (numdigits(Channels.MaxNumber()))

static const char *VERSION        = "0.8a";
static const char *DESCRIPTION    = "Extended EPG";
static const char *MAINMENUENTRY  = "Nordlicht's EPG";

// setup values
int hidemainmenu=false;
int bookmark=2015;
int Step=30;
int keeposd=0;
int showchannelgroups=0;
int hideencryptedchannels=0;
int enigmatheme=0;
int showchannelnumbers=0;
int hideradiochannels=0;
int switchwithok=0;

time_t t;

// --- myMenuSetup ------------------------------------------------------------
class myMenuSetup:public cMenuSetupPage
{
 protected:
  virtual void Store()
  {
   SetupStore("hidemainmenu",hidemainmenu);
   SetupStore("bookmark",bookmark);
   SetupStore("step",Step);
   SetupStore("keeposd",keeposd);
   SetupStore("showchannelgroups",showchannelgroups);
   SetupStore("hideencryptedchannels",hideencryptedchannels);
   SetupStore("hideradiochannels",hideradiochannels);
   SetupStore("enigmatheme",enigmatheme);
   SetupStore("showchannelnumbers",showchannelnumbers);
   SetupStore("switchwithok",switchwithok);
  }
 public:
  myMenuSetup()
  {
   Add(new cMenuEditIntItem(tr("Step width (min)"),&Step));
   Add(new cMenuEditTimeItem(tr("Jump to"),&bookmark));
   Add(new cMenuEditBoolItem(tr("Switch with Ok"),&switchwithok));
   Add(new cOsdItem(tr("Appearance"),osUnknown,false));
   Add(new cMenuEditBoolItem(tr("Hide main menu entry"),&hidemainmenu));
   Add(new cMenuEditBoolItem(tr("Keep display after switching"),&keeposd));
   Add(new cMenuEditBoolItem(tr("Show channel numbers"),&showchannelnumbers));
   Add(new cMenuEditBoolItem(tr("Show channel groups"),&showchannelgroups));
   Add(new cMenuEditBoolItem(tr("Hide encrypted channels"),&hideencryptedchannels));
   Add(new cMenuEditBoolItem(tr("Hide radio channels"),&hideradiochannels));
   Add(new cMenuEditBoolItem(tr("Is Enigma theme used"),&enigmatheme));
  }
};

// --- myMenuWhatsOn ----------------------------------------------------------
class myMenuWhatsOn:public cOsdMenu
{
 private:
  bool next,jumpto;
  char tmp[16];
  int currentChannel,hh,mm,index;
  const cSchedules *schedules;
  cSchedulesLock schedulesLock;

  void LoadSchedules(int shift);
  void GoToDay(int day);
  void JumpTo(int hh,int mm);
  eOSState Switch();
  int Tab(){if(Setup.UseSmallFont==2)return 5;else return 6;}
 public:
  myMenuWhatsOn();
  ~myMenuWhatsOn();
  virtual eOSState ProcessKey(eKeys Key);
};

myMenuWhatsOn::myMenuWhatsOn():cOsdMenu("")
{
 jumpto=false;

 if(Setup.UseSmallFont==2)
 {
  if(showchannelnumbers)
   cOsdMenu::SetCols(2,CHNUMWIDTH,10,5);
  else
   cOsdMenu::SetCols(2,10,5);

  PatchFont(fontSml);
 }
 else
 {
  if(showchannelnumbers)
   cOsdMenu::SetCols(2,CHNUMWIDTH,10,6);
  else
   cOsdMenu::SetCols(2,10,6);

  PatchFont(fontOsd);
 }

 t=time(NULL);
 next=false;

 currentChannel=cDevice::CurrentChannel();

 LoadSchedules(0);
}

myMenuWhatsOn::~myMenuWhatsOn()
{
 if(Setup.UseSmallFont==2)cFont::SetFont(fontSml);
 else cFont::SetFont(fontOsd);
}

void myMenuWhatsOn::LoadSchedules(int shift)
{
 t+=shift*Step*60;

 myWhatsOnItem *mi=(myWhatsOnItem *)Get(Current());
 if(mi)currentChannel = mi->channel->Number();

 Clear();
 
 schedules=cSchedules::Schedules(schedulesLock);
 for(cChannel *Channel=Channels.First();Channel;Channel=Channels.Next(Channel))
 {
  if(!(!Channel->Vpid()&&Channel->Apid(0)&&hideradiochannels||Channel->Ca()&&hideencryptedchannels))
  {
   if(!Channel->GroupSep())
   {
    const cSchedule *Schedule=schedules->GetSchedule(Channel->GetChannelID());
    // if there is no cSchedule we can't get infos
    if(Schedule)
    { 
     const cEvent *Event=next?Schedule->GetEventAround(t):Schedule->GetPresentEvent();
     Add(new myWhatsOnItem(Event,Channel,next),Channel->Number()==currentChannel);
    }
    else
     Add(new myWhatsOnItem(NULL,Channel,next),Channel->Number()==currentChannel);
   }
   else
    if(showchannelgroups)
     Add(new cOsdItem(Channel->Name(),osUnknown,false));
  }
 }
 
 sprintf(tmp,"%d:%d",bookmark/100,bookmark%100);

 SetTitle(next?*DayDateTime(t):tr("What's on now?"));

 if(switchwithok)
  SetHelp(next?tr("Button$Now"):NULL,"<<",">>",tr("Button$Info"));
 else
  SetHelp(bookmark?tmp:NULL,"<<",">>",next?tr("Button$Now"):tr("Button$Switch"));

 Display();
}

eOSState myMenuWhatsOn::Switch()
{
 myWhatsOnItem *item = (myWhatsOnItem *)Get(Current());
 if(item)
 {
  const cChannel *channel=item->channel;
  if(channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
   return keeposd?osContinue:osEnd;
 }
 Skins.Message(mtError, tr("Can't switch channel!"));
 return osContinue;
}

void myMenuWhatsOn::GoToDay(int day)
{
 struct tm tm1,tm2;
 time_t tmp_t=time(NULL);
 
 localtime_r(&t,&tm1);
 localtime_r(&tmp_t,&tm2);
 tm2.tm_hour=tm1.tm_hour;
 tm2.tm_min=tm1.tm_min;
 tm2.tm_mday+=day;

 t=mktime(&tm2);
 
 next=1;
 LoadSchedules(0);
}

void myMenuWhatsOn::JumpTo(int hour,int min)
{
 struct tm tmp_tm;
 localtime_r(&t,&tmp_tm);

 if(tmp_tm.tm_hour>hour||tmp_tm.tm_min>min&&tmp_tm.tm_hour>=hour)
  tmp_tm.tm_mday++;
 
 tmp_tm.tm_hour=hour;
 tmp_tm.tm_min=min;
 
 t=mktime(&tmp_tm);
 
 next=true;
 LoadSchedules(0);
}

eOSState myMenuWhatsOn::ProcessKey(eKeys Key)
{
 eOSState state;
 
 if(jumpto)
 {
  switch(Key)
  {
   case k0...k9: {
                  int number=Key-k0;
                  switch(index)
                  {
                   // first digit
                   case 0: if(number<=2)
                           {
                            hh=number*10;
                            sprintf(tmp,"%s: %d-:--",tr("Jump to"),number);
                            index++;
                           }
                           break;
                   // second digit
                   case 1: if(hh<=10||hh==20&&number<=3)
                           {
                            hh+=number;
                            sprintf(tmp,"%s: %02d:--",tr("Jump to"),hh);
                            index++;
                           }
                           break;
                   // third digit
                   case 2: if(number<=5)
                           {
                            mm=number*10;
                            sprintf(tmp,"%s: %02d:%d-",tr("Jump to"),hh,number);
                            index++;
                           }
                           break;
                   // fourth digit
                   case 3: mm+=number;
                           sprintf(tmp,"%s: %02d:%02d",tr("Jump to"),hh,mm);
                           index++;
                           break;
                   default: break;
                  }
                  if(index==4)
                  {
                   JumpTo(hh,mm);
                   jumpto=0;
                  }
                  else
                  {
                   SetTitle(tmp);
                   Display();
                  }
                  break;
                 }
    case kOk: JumpTo(hh,mm);
              jumpto=0;
              break;
    case kBack: jumpto=0;
                SetTitle(next?*DayDateTime(t):tr("What's on now?"));
                Display();
                break;
    default: break;
  }
  state=osContinue;
 }
 else
 {
  state=cOsdMenu::ProcessKey(Key);

  if(state==osUnknown)
  {
   if(switchwithok)
   {
    switch(Key)
    {
     case k1...k7: GoToDay(Key-k0);
                   break;
     // toggle asking for a time to jump to
     case k0: hh=mm=index=0;
              sprintf(tmp,"%s: --:--",tr("Jump to"));
              SetTitle(tmp);
              Display();
              jumpto=1;
              break;
     case kRed: if(next)
                {
                 next=false;
                 t=time(NULL);
                 LoadSchedules(0);
                }
                break;
     case kGreen: next=true;
                  LoadSchedules(-1);
                  break;
     case kYellow: next=true;
                   LoadSchedules(1);
                   break;
     case kBlue: if(Count())
                  return AddSubMenu(new myMenuEvent((myWhatsOnItem*)Get(Current())));
                 break;
     case kOk: return Switch();
     default: break;
    }
   }
   else
   {
    switch(Key)
    {
     case k1...k7: GoToDay(Key-k0);
                   break;
     // toggle asking for a time to jump to
     case k0: hh=mm=index=0;
              sprintf(tmp,"%s: --:--",tr("Jump to"));
              SetTitle(tmp);
              Display();
              jumpto=1;
              break;
     case kRed: if(bookmark)
                 JumpTo(bookmark/100,bookmark%100);
                break;
     case kGreen: next=true;
                  LoadSchedules(-1);
                  break;
     case kYellow: next=true;
                   LoadSchedules(1);
                   break;
     case kBlue: if(next)
                 {
                  next=false;
                  t=time(NULL);
                  LoadSchedules(0);
                 }
                 else
                  return Switch();
                 break;
     case kOk: if(Count())
                return AddSubMenu(new myMenuEvent((myWhatsOnItem*)Get(Current())));
               break;
     default: break;
    }
   }
  }
 }
 return state;
}

// --- cPluginNordlichtsepg ---------------------------------------------------
class cPluginNordlichtsepg:public cPlugin
{
private:
public:
 cPluginNordlichtsepg(void);
 virtual ~cPluginNordlichtsepg();
 virtual const char *Version(void) { return VERSION; }
 virtual const char *Description(void) { return tr(DESCRIPTION); }
 virtual const char *CommandLineHelp(void);
 virtual bool ProcessArgs(int argc, char *argv[]);
 virtual bool Initialize(void);
 virtual bool Start(void);
 virtual void Stop(void);
 virtual void Housekeeping(void);
 virtual const char *MainMenuEntry(void) {return hidemainmenu?NULL:tr(MAINMENUENTRY);}
 virtual cOsdObject *MainMenuAction(void);
 virtual cMenuSetupPage *SetupMenu(void);
 virtual bool SetupParse(const char *Name, const char *Value);
 virtual bool Service(const char *Id, void *Data = NULL){return false;};
 virtual const char **SVDRPHelpPages(void){return NULL;};
 virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode){return NULL;};
};

cPluginNordlichtsepg::cPluginNordlichtsepg(void)
{
}

cPluginNordlichtsepg::~cPluginNordlichtsepg()
{
}

const char *cPluginNordlichtsepg::CommandLineHelp(void)
{
 return NULL;
}

bool cPluginNordlichtsepg::ProcessArgs(int argc, char *argv[])
{
 return true;
}

bool cPluginNordlichtsepg::Initialize(void)
{
 RegisterI18n(Phrases);
 return true;
}

bool cPluginNordlichtsepg::Start(void)
{
 return true;
}

void cPluginNordlichtsepg::Stop(void)
{
}

void cPluginNordlichtsepg::Housekeeping(void)
{
}

cOsdObject *cPluginNordlichtsepg::MainMenuAction(void)
{
 return new myMenuWhatsOn;
}

cMenuSetupPage *cPluginNordlichtsepg::SetupMenu(void)
{
 return new myMenuSetup;
}

bool cPluginNordlichtsepg::SetupParse(const char *Name, const char *Value)
{
 if(!strcmp("hidemainmenu",Name))
  hidemainmenu=atoi(Value);
 else
  if(!strcmp("bookmark",Name))
   bookmark=atoi(Value);
  else
   if(!strcmp("step",Name))
    Step=atoi(Value);
   else
    if(!strcmp("keeposd",Name))
     keeposd=atoi(Value);
    else
     if(!strcmp("showchannelgroups",Name))
      showchannelgroups=atoi(Value);
     else
      if(!strcmp("hideencryptedchannels",Name))
       hideencryptedchannels=atoi(Value);
      else
       if(!strcmp("enigmatheme",Name))
        enigmatheme=atoi(Value);
       else
        if(!strcmp("showchannelnumbers",Name))
         showchannelnumbers=atoi(Value);
        else
         if(!strcmp("hideradiochannels",Name))
          hideradiochannels=atoi(Value);
         else
          if(!strcmp("switchwithok",Name))
           switchwithok=atoi(Value);
          else
           return false;
 return true;
}

VDRPLUGINCREATOR(cPluginNordlichtsepg); // Don't touch this!

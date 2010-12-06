/*
 * blink.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/tools.h>
#include <stdlib.h>
#include "i18n.h"
#include "led.h"

#ifdef DEBUG
   #define debug(x...) dsyslog("blink: " x);
   #define error(x...) esyslog("blink: " x);
#else
   #define debug(x...) ;
   #define error(x...) esyslog("blink: " x);
#endif

static const char *VERSION        = "0.0.6";
static const char *DESCRIPTION    = "Control the LEDs of M7x0";
//We do not need a entry in the main menu
//static const char *MAINMENUENTRY  = "Blink";
int i_LedOn = 2; //Led in on mode
int i_LedOnL = 0; //Led in on mode
int i_LedOnH = 40; //Led in on mode
int i_LedOnT = 1; //Led in on mode
#define LEDONF 22 //Flags for dim
int i_LedHotsby = 1; //led in hot standbay mode
int i_LedHotsbyL = 0; //led in hot standbay mode
int i_LedHotsbyH = 40; //led in hot standbay mode
int i_LedHotsbyT = 1; //led in hot standbay mode
#define LEDHOTSBYF 22 //Flags for dim
int i_LedRec = 1; //Led in rec mode
int i_LedRecL = 0; //Led in rec mode
int i_LedRecH = 40; //Led in rec mode
int i_LedRecT = 1; //Led in rec mode
#define LEDRECF 22 //Flags for dim
int i_LedDelay = 3; //Min wait time between two status blinks
//dim mode for shutdown, config only in scr
#define LEDSL 0 //Low for shutbown dim
#define LEDSH 40 //High for shutdown dim
#define LEDST 1 //Time for shutdown dim
#define LEDSF 17 //Flags for shutdown dim

int isBlinkHotsby = 0; //Bink for every running recording in Hotstanby


const char *ast_LedModes[4];
// --------------------------------- Menu Setup blink --------------------------------------------
class cMenuSetupBlink : public cMenuSetupPage{
private:
   int i_LedOn1;
   int i_LedOn2;
   int i_LedHotsby1;
   int i_LedHotsby2;
   int i_LedRec1;
   int i_LedRec2;
protected:
   virtual eOSState ProcessKey(eKeys Key);
   void Set(void);
public:
   cMenuSetupBlink(void);
   virtual void Store(void);
};

cMenuSetupBlink::cMenuSetupBlink(void){

   ast_LedModes[0]=tr("off");
   ast_LedModes[1]=tr("middle on");
   ast_LedModes[2]=tr("blink");
   ast_LedModes[3]=tr("outer on");

   //Spilt T into tow parameter for display
   if (i_LedOnH>40){
      i_LedOn1=40;
      i_LedOn2=i_LedOnH-40;
   }else{
      i_LedOn1=i_LedOnH;
      i_LedOn2=0;
   }
   if (i_LedHotsbyH>40){
      i_LedHotsby1=40;
      i_LedHotsby2=i_LedHotsbyH-40;
   }else{
      i_LedHotsby1=i_LedHotsbyH;
      i_LedHotsby2=0;
   }
   if (i_LedRecH>40){
      i_LedRec1=40;
      i_LedRec2=i_LedRecH-40;
   }else{
      i_LedRec1=i_LedRecH;
      i_LedRec2=0;
   }

   Set();
}

void cMenuSetupBlink::Set(void){
   int current = Current();
   Clear();

   //Build setup entries
   Add(new cMenuEditIntItem(tr("Delay between led activation"), &i_LedDelay, 0, 20));
   Add(new cMenuEditStraItem(tr("Led on mode"), &i_LedOn, 4, ast_LedModes));
   if (i_LedOn==2){
      Add(new cMenuEditIntItem(tr("==> Led low value"), &i_LedOnL, 0, 39));
      Add(new cMenuEditIntItem(tr("==> Led high value"), &i_LedOn1, 0, 40));
      Add(new cMenuEditIntItem(tr("==> Led delay"), &i_LedOnT, 1, 20));
      if (i_LedOn1==40){
         Add(new cMenuEditIntItem(tr("==> Led on delay"), &i_LedOn2, 0, 200));
      }
   }
#ifdef HOTSBY
   Add(new cMenuEditStraItem(tr("Led hotstandby mode"), &i_LedHotsby, 4, ast_LedModes));
   if (i_LedHotsby==2){
      Add(new cMenuEditIntItem(tr("==> Led low value"), &i_LedHotsbyL, 0, 39));
      Add(new cMenuEditIntItem(tr("==> Led high value"), &i_LedHotsby1, 0, 40));
      Add(new cMenuEditIntItem(tr("==> Led delay"), &i_LedHotsbyT, 1, 20));
      if (i_LedHotsby1==40){
         Add(new cMenuEditIntItem(tr("==> Led on delay"), &i_LedHotsby2, 0, 200));
      }
   }
#endif
   Add(new cMenuEditStraItem(tr("Led rec mode"), &i_LedRec, 4, ast_LedModes));
   if (i_LedRec==2){
      Add(new cMenuEditIntItem(tr("==> Led low value"), &i_LedRecL, 0, 39));
      Add(new cMenuEditIntItem(tr("==> Led high value"), &i_LedRec1, 0, 40));
      Add(new cMenuEditIntItem(tr("==> Led delay"), &i_LedRecT, 1, 20));
      if (i_LedRec1==40){
         Add(new cMenuEditIntItem(tr("==> Led on delay"), &i_LedRec2, 0, 200));
      }
   }
   Add(new cMenuEditBoolItem(tr("Blink recordings in hotstandby"), &isBlinkHotsby,  tr("No"), tr("Yes")));

   SetCurrent(Get(current));
   Display();
}

eOSState cMenuSetupBlink::ProcessKey(eKeys Key){
   int iTemp_LedOn = i_LedOn;
   int iTemp_LedOn1=i_LedOn1;
   int iTemp_LedRec = i_LedRec;
   int iTemp_LedRec1 = i_LedRec1;
   int iTemp_LedHotsby = i_LedHotsby;
   int iTemp_LedHotsby1 = i_LedHotsby1;
   int NeedRefresh = 0;

   eOSState state = cMenuSetupPage::ProcessKey(Key);

   if (iTemp_LedRec != i_LedRec || iTemp_LedOn != i_LedOn || iTemp_LedHotsby != i_LedHotsby){
      NeedRefresh=1;
   }

   if (i_LedHotsbyL>i_LedHotsby1){
      i_LedHotsby1=i_LedHotsbyL+1;
      NeedRefresh=1;
   }
   if ((i_LedHotsby1<40 && iTemp_LedHotsby1==40)||(i_LedHotsby1==40 && iTemp_LedHotsby1<40)){
      i_LedHotsby2=0;
      NeedRefresh=1;
   }
   if (i_LedOnL>i_LedOn1){
      i_LedOn1=i_LedOnL+1;
      NeedRefresh=1;
   }
   if ((i_LedOn1<40 && iTemp_LedOn1==40)||(i_LedOn1==40 && iTemp_LedOn1<40)){
      i_LedOn2=0;
      NeedRefresh=1;
   }
   if (i_LedRecL>i_LedRec1){
      i_LedRec1=i_LedRecL+1;
      NeedRefresh=1;
   }
   if ((i_LedRec1<40 && iTemp_LedRec1==40)||(i_LedRec1==40 && iTemp_LedRec1<40)){
      i_LedRec2=0;
      NeedRefresh=1;
   }

   if (NeedRefresh==1){
      Set();
      Display();
   }

   i_LedOnH=i_LedOn1+i_LedOn2;
   i_LedHotsbyH=i_LedHotsby1+i_LedHotsby2;
   i_LedRecH=i_LedRec1+i_LedRec2;

   if (state == osUnknown){
      switch (Key){
         case kOk:
            return osBack;
         default: break;
      }
   }

   return state;
}

void cMenuSetupBlink::Store(void){
   SetupStore("LedOn", i_LedOn);
   SetupStore("LedOnL", i_LedOnL);
   SetupStore("LedOnH", i_LedOnH);
   SetupStore("LedOnT", i_LedOnT);
   SetupStore("LedHotsby", i_LedHotsby);
   SetupStore("LedHotsbyL", i_LedHotsbyL);
   SetupStore("LedHotsbyH", i_LedHotsbyH);
   SetupStore("LedHotsbyT", i_LedHotsbyT);
   SetupStore("LedRec", i_LedRec);
   SetupStore("LedRecL", i_LedRecL);
   SetupStore("LedRecH", i_LedRecH);
   SetupStore("LedRecT", i_LedRecT);
   SetupStore("LedDelay", i_LedDelay);
   SetupStore("BlinkHotsby", isBlinkHotsby);

   debug("Led on: %i, Led off: %i, Led Rec: %i, BlinkHotsby: %i",i_LedOn,i_LedHotsby,i_LedRec,isBlinkHotsby);
   debug("onL: %i, onH: %i, onT: %i, offL: %i, offH: %i, offT: %i",i_LedOnL,i_LedOnH,i_LedOnT,i_LedHotsbyL,i_LedHotsbyH,i_LedHotsbyT)
}


//--------------------------- Background Monitor Classs ---------------------------------------------
class cBackgroundMonitor : public cThread{
private:
   virtual void Action(void);
   void setStatusLed(void);
   void waitBlinkEnd(void);
   void setLed(int i_Led, int i_Low, int i_High, int i_Delay, int i_Flag);
   bool isActive;
public:
   cBackgroundMonitor(void);
   ~cBackgroundMonitor();
};

cBackgroundMonitor::cBackgroundMonitor(void):cThread("blink"){
}

cBackgroundMonitor::~cBackgroundMonitor(){
   isActive=false;
}

void cBackgroundMonitor::setStatusLed(void){
#ifdef HOTSBY
   if(getIaMode()==1){//Box is on
      setLed(i_LedOn,i_LedOnL,i_LedOnH,i_LedOnT,LEDONF);
   }else{//Box is in hotstanby
      setLed(i_LedHotsby,i_LedHotsbyL,i_LedHotsbyH,i_LedHotsbyT,LEDHOTSBYF);
   }
#else
   setLed(i_LedOn,i_LedOnL,i_LedOnH,i_LedOnT,LEDONF);
#endif
}

void cBackgroundMonitor::waitBlinkEnd(void){

   int i_led_dim_low=0;
   int i_led_dim_high=0;
   int i_led_dim_dur=0;
   int i_led_flags=0;

   if (pic_get_dim_led(1, &i_led_dim_low, &i_led_dim_high, &i_led_dim_dur, &i_led_flags)){
      i_led_dim_low=0;
      i_led_dim_high=0;
      i_led_dim_dur=0;
      i_led_flags=0;
   }
// debug("LED Flags: %i, %i",i_led_flags,(i_led_flags & 16));

   while ((i_led_flags & 16)==16 && isActive && Running())
  {
      if (pic_get_dim_led(1, &i_led_dim_low, &i_led_dim_high, &i_led_dim_dur, &i_led_flags)){
         i_led_dim_low=0;
         i_led_dim_high=0;
         i_led_dim_dur=0;
         i_led_flags=0;
      }
//    debug("LED Flags: %i, %i",i_led_flags,(i_led_flags & 16));
      cCondWait::SleepMs(333);//333ms
   }
}

void cBackgroundMonitor::setLed(int i_Led, int i_Low, int i_High, int i_Delay, int i_Flag){
   if (i_Led==0){
//    debug("Box off, set Led off")
      pic_set_led(1,OUTER_LED,0);
      pic_set_led(1,MIDDLE_LED,0);
   }else if(i_Led==1){
//    debug("Box off, set middle Led on")
      pic_set_led(1,OUTER_LED,0);
      pic_set_led(1,MIDDLE_LED,1);
   }else if(i_Led==2){
//    debug("Box off, blink Led")
      pic_set_led(1,MIDDLE_LED,0);
      pic_set_dim_led(1,i_Low,i_High,i_Delay,i_Flag);
   }else if(i_Led==3){
//    debug("Box of, set outer Led on")
      pic_set_led(1,MIDDLE_LED,0);
      pic_set_led(1,OUTER_LED,1);
   }
}
void cBackgroundMonitor::Action(void){
   int i_numRec = 0;

#ifdef HOTSBY
   bool oldIaMode=getIaMode();
#endif

   isActive=true;

   //Set all LED to off
   debug("Switch LED off");
   setLed(0,0,0,0,0);

   while (isActive && Running()){
      //Check if we need to blink for recordings
#ifdef HOTSBY
      if (i_LedRec!=0 && (getIaMode()==1 || isBlinkHotsby)){
#else
      if (i_LedRec!=0){
#endif
         i_numRec=0;
         for (cTimer * t = Timers.First(); t && Running() && isActive; t = Timers.Next(t)){
            if (t->Recording()){
               i_numRec++;
            }
         }
//       debug("found %i recordings",i_numRec);
         if (i_numRec>0){
            for (int i=0;i<i_numRec && Running() && isActive;i++ ){
                  //Set all LED to selected mode
                  if (i_LedRec==2){//dim
#ifdef HOTSBY
                     if ((i_LedOn==3 && getIaMode()==1) || (i_LedHotsby==3 && getIaMode()==0)){//dim up
#else
                     if (i_LedOn==3){//dim up
#endif
                        setLed(i_LedRec,i_LedRecL,i_LedRecH,i_LedRecT,LEDRECF & 251);
                     }else{//dim down
                        setLed(i_LedRec,i_LedRecL,i_LedRecH,i_LedRecT,LEDRECF | 4);
                     }
                     waitBlinkEnd();
                  }else{//No dim
                     setLed(i_LedRec,i_LedRecL,i_LedRecH,i_LedRecT,LEDRECF);
                     //wait 200ms
                     cCondWait::SleepMs(200);
                     //Set LED to off
                     setLed(0,0,0,0,0);
                  }
                  //wait 200ms
                  cCondWait::SleepMs(200);
            }
         }
      }
      //Set status LED
      setStatusLed();

      //check if LED is blinking and wait for it to finish
      waitBlinkEnd();

      //Wait for x seconds
#ifdef HOTSBY
      oldIaMode=getIaMode();
#endif
      for(int i=0; i<=i_LedDelay && isActive && Running();i++){
         cCondWait::SleepMs(1000);//1 seconds
#ifdef HOTSBY
         //Check if the iamode changed to on
         if (getIaMode()!=oldIaMode){
            break;
         }
         oldIaMode=getIaMode();
#endif
      }
   }//End while(isActive)

   //Switch to dim for shutdown
   setLed(0,0,0,0,0);
   setLed(2,LEDSL,LEDSH,LEDST,LEDSF);

}


//--------------------------   Plugin Class --------------------------------------------------
class cPluginBlink : public cPlugin{
private:
  // Add any member variables or functions you may need here.
   cBackgroundMonitor *monitor;
public:
  cPluginBlink(void);
  virtual ~cPluginBlink();
  virtual const char *Version(void){ return VERSION; }
  virtual const char *Description(void){ return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
//We do not need a entry in the main menu
//  virtual const char *MainMenuEntry(void){ return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginBlink::cPluginBlink(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginBlink::~cPluginBlink()
{
  // Clean up after yourself!
}

const char *cPluginBlink::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginBlink::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginBlink::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginBlink::Start(void)
{
  // Start any background activities the plugin shall perform.
   RegisterI18n(Phrases);

   //Start drive monitor
   monitor = new cBackgroundMonitor;
   monitor->Start();

    return true;
}

void cPluginBlink::Stop(void)
{
  // Stop any background activities the plugin shall perform.

   //Delete drive monitor
   delete monitor;

}

void cPluginBlink::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginBlink::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginBlink::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginBlink::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginBlink::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupBlink;
}

bool cPluginBlink::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
   if(!strcasecmp(Name, "LedOn")){
      i_LedOn =  atoi(Value);
   }else if(!strcasecmp(Name, "LedOnL")){
      i_LedOnL =  atoi(Value);
   }else if(!strcasecmp(Name, "LedOnH")){
      i_LedOnH =  atoi(Value);
   }else if(!strcasecmp(Name, "LedOnT")){
      i_LedOnT =  atoi(Value);
   }else if (!strcasecmp(Name, "LedHotsby")){
      i_LedHotsby =  atoi(Value);
   }else if (!strcasecmp(Name, "LedHotsbyL")){
      i_LedHotsbyL =  atoi(Value);
   }else if (!strcasecmp(Name, "LedHotsbyH")){
      i_LedHotsbyH =  atoi(Value);
   }else if (!strcasecmp(Name, "LedHotsbyT")){
      i_LedHotsbyT =  atoi(Value);
   }else    if(!strcasecmp(Name, "LedRec")){
      i_LedRec =  atoi(Value);
   }else if(!strcasecmp(Name, "LedRecL")){
      i_LedRecL =  atoi(Value);
   }else if(!strcasecmp(Name, "LedRecH")){
      i_LedRecH =  atoi(Value);
   }else if(!strcasecmp(Name, "LedRecT")){
      i_LedRecT =  atoi(Value);
   }else if(!strcasecmp(Name, "LedDelay")){
      i_LedDelay =  atoi(Value);
   }else if (!strcasecmp(Name, "BlinkHotsby")){
      isBlinkHotsby = atoi(Value);
   }else{
      return false;
   }
   return true;
}

bool cPluginBlink::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginBlink::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginBlink::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginBlink); // Don't touch this!

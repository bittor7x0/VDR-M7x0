/*
 * fasttimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include <vdr/i18n.h>
#include <vdr/timers.h>
#include <vdr/channels.h>
#include <vdr/epg.h>


// --- cMenuEditDateItem2 ---------------------------------------------------

class cMenuEditDateItem2 : public cMenuEditDateItem {
  private:
    time_t *value2;

  public:
    cMenuEditDateItem2(const char *Name, time_t *Value);
    virtual eOSState ProcessKey(eKeys Key);
};

cMenuEditDateItem2::cMenuEditDateItem2(const char *Name, time_t *Value) :
  cMenuEditDateItem(Name, Value)
{
  value2 = Value;
}

eOSState cMenuEditDateItem2::ProcessKey(eKeys Key)
{
  if (Key >= k0 && Key <= k9 && value2) {
    time_t currtime = time(NULL);
    struct tm tm_r;
    struct tm *now = localtime_r(&currtime, &tm_r);

    int day1   = now->tm_mday % 10;
    int day10  = now->tm_mday / 10;
    int newday = Key - k0;

    if(newday >= day1)
      now->tm_mday = 10*day10 + newday;
    else
      now->tm_mday = 10*(day10+1) + newday;
    // mktime will fix overflows

    now->tm_sec = 58;   // cEditDateItem shows nothing if time is from past ...
    now->tm_min = 59;
    now->tm_hour = 23;

    now->tm_mday--;  // ProcessKey(kRight) will increment this
    *value2 = mktime(now);

    Key = kRight;
  }

  return cMenuEditDateItem::ProcessKey(Key);
}

// --- cQuickTimerMenu ------------------------------------------------------

class cQuickTimerMenu : public cOsdMenu {
  private:
    int channel;
    mutable time_t day;
    int start;

    cSchedulesLock schedulesLock;
    const cSchedules *schedules;
    const cSchedule *schedule;
    const cEvent *event;

    void CreateTimer(void);

  public:
    cQuickTimerMenu();
    virtual eOSState ProcessKey(eKeys Key);
};

#define posDATE      0
#define posCHANNEL   1
#define posTIME      2
#define posTITLE     3
#define posEMPTY     4
#define posLASTTIMER 5

cQuickTimerMenu::cQuickTimerMenu() :
  cOsdMenu(tr("Create timer"), 12)
{
  channel = cDevice::CurrentChannel();
  day   = cTimer::SetTime(time(NULL), 24*3600-2);
  start = 0;
  event = NULL;

  schedules = cSchedules::Schedules(schedulesLock);
  schedule  = NULL;

  Add(new cMenuEditDateItem2(tr("Day"),         &day));
  Add(new cMenuEditChanItem(tr("Channel"),      &channel));
  Add(new cMenuEditTimeItem(tr("Start"),        &start));
  Add(new cOsdItem("-- Event not found from EPG --"));

  cOsdItem *tmp;
  Add(tmp = new cOsdItem(""));
  tmp->SetSelectable(false);

  SetCurrent(Get(posDATE));

  SetHelp(NULL, NULL, NULL, NULL);
}

void cQuickTimerMenu::CreateTimer()
{
  if(event && event->Title()) {
    cTimer *timer = new cTimer(event);
    cTimer *t = Timers.GetTimer(timer);
    if (t) {
      Skins.Message(mtInfo, tr("Timer for event already exists"));
      delete timer;
    } else if(!timer) {
      Skins.Message(mtInfo, tr("Error creating timer"));
    } else {
      Timers.Add(timer);
      timer->Matches();
      Timers.SetModified();
      Timers.Save();
      isyslog("timer %s added (%s)", *timer->ToDescr(), 
	      timer->HasFlags(tfActive) ? "active" : "inactive");
      
      Skins.Message(mtInfo, tr("Timer created"));

      // Add new timer to list of created timers
      char *str = NULL;
      const char *d = Get(posDATE)->Text();
      const char *t = Get(posTIME)->Text();
      const char *c = Get(posCHANNEL)->Text();
      const char *n = Get(posTITLE)->Text();
      d = d ? strrchr(d, ' ')  : NULL;
      t = t ? strrchr(t, '\t') : NULL;
      c = c ? strrchr(c, '\t') : NULL;
      d = d ? d + 1 : NULL;
      t = t ? t + 1 : NULL;
      c = c ? c + 1 : NULL;
      t = t ? strreplace(strdup(t),'-','0') : NULL;
      asprintf(&str, "%s %s %s\t%s", d?d:"", t?t:"", c?c:"", n?n:"");
      cOsdItem *tmp;
      Add(tmp = new cOsdItem(str), false, Get(posEMPTY));
      tmp->SetSelectable(false);
      free(str);
      free((char*)t);
      
      event = NULL;
    }
    Get(posTIME)->SetFresh(true);
    SetCurrent(Get(posCHANNEL));
    Display();		       
  }
}

eOSState cQuickTimerMenu::ProcessKey(eKeys Key)
{
  int old_channel = channel;

  eOSState state = cOsdMenu::ProcessKey(Key);

  // get current schedule
  if(old_channel != channel || !schedule) {    
    cChannel *Channel = Channels.GetByNumber(channel);
    if(Channel && schedules)
      schedule = schedules->GetSchedule(Channel->GetChannelID());
    else
      schedule = NULL;
  }

  // auto-jump to next field ?
  if((state == osContinue && Key>=k0 && Key<=k9) || (Key == kOk)) {    
    if(Current() == posDATE) {
      CursorDown();
      state = osContinue;
    } else if(Current() == posCHANNEL) {
      CursorDown();
      state = osContinue;
    } else if(Current() == posTIME) {
      const char *txt = Get(posTIME)->Text();
      if(txt && txt[strlen(txt)-1] != '-')
	CursorDown();
    }
  }

  // update title
  if (schedule && schedule->Events() && state == osContinue) {

    time_t Time = cTimer::SetTime(day, (start%100)*60 + (start/100)*3600);

    const cEvent *pe = NULL;
    for (cEvent *p = schedule->Events()->First(); p; 
	 p = schedule->Events()->Next(p)) {
      if(p->StartTime() <= Time)
	pe = p;
      else
	break;
    }
    if(event != pe) {
      if(!pe)
	Get(posTITLE)->SetText("-- Event not found from EPG --");
      else if(!pe->Title()) 
	Get(posTITLE)->SetText("(null)");
      else
	Get(posTITLE)->SetText(pe->Title());
      SetHelp(pe ? tr("Record") : NULL, NULL, NULL, NULL);
      Display();
    }
    event = pe;
  }  

  if (state == osUnknown) {
    switch (Key) {
      case kOk: 
      case kRed:    CreateTimer();
                    // fall thru
      case kGreen:
      case kYellow:
      case kBlue:   return osContinue;
      default:      break;
    }
  }

  return state;
}


// --- cPluginQuicktimer -----------------------------------------------------

#include <vdr/plugin.h>

static const char *VERSION        = "0.1.2";
static const char *DESCRIPTION    = "Create new timers";
static const char *MAINMENUENTRY  = "New timer";

class cPluginQuicktimer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginQuicktimer(void) {};
  virtual ~cPluginQuicktimer() {};
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void) { return NULL; }
  virtual bool ProcessArgs(int argc, char *argv[]) { return true; }
  virtual bool Initialize(void) { return true; }
  virtual bool Start(void) { return true; }
  virtual void Stop(void) {}
  virtual void Housekeeping(void) {}
  virtual const char *MainMenuEntry(void)  { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void) { return new cQuickTimerMenu(); }
  virtual cMenuSetupPage *SetupMenu(void) { return NULL; }
  virtual bool SetupParse(const char *Name, const char *Value) {return false;}
};

VDRPLUGINCREATOR(cPluginQuicktimer); // Don't touch this!

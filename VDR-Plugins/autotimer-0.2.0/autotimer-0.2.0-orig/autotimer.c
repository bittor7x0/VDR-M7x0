/*
 * autotimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

//
// autotimer.c
//

#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/timers.h>

#define FULLSCAN_INTERVAL   (24*60*60)  // 24 hours
#define QUICKSCAN_INTERVAL     (10*60)  // 10 min

#define tfAutoTimer         ((eTimerFlags)0x0800)


#include "rules.h"
//#include "recdb.h"


bool g_autotimer_Silent = false;
#define ISYSLOG(a...) void( (!g_autotimer_Silent) ? isyslog(a) : void() )


static void CreateTimer(cEvent *event, const cString& Name, int Priority, 
			bool CheckDuplicates = false)
{
  if(!event) {
    esyslog("autotimer: CreateTimer called with NULL event !");
    return;
  }

  const cString date = event->GetDateString();
  const cString time = event->GetTimeString();
  const char *title  = event->Title();
  
  /* create new timer */
  cTimer *timer = new cTimer(event);
  if(!timer) {
    esyslog("autotimer: Error creating timer for %s %s %s (%x %x v%d)",
	    *date, *time, title,
	    event->TableID(), event->EventID(), event->Version());
    return;
  }
  
  /* check if timer already exists */
  cTimer *t = Timers.GetTimer(timer);
  if (t) {
    ISYSLOG("autotimer: Timer for %s %s %s already exists, tfAutoTimer=%s tfActive=%s%s(tbl:0x%x id:%x v.%d)",
	    *date, *time, title, 
	    t->HasFlags(tfAutoTimer) ? "yes" : "no",
	    t->HasFlags(tfActive)    ? "yes" : "no",
	    t->HasFlags(tfRecording) ? " RECORDING" : "",
	    event->TableID(), event->EventID(), event->Version());
    if(t->HasFlags(tfAutoTimer) && !t->HasFlags(tfRecording)) {
      if(strcmp(t->File(), *Name)) {
	ISYSLOG("           updating file %s -> %s", t->File(), *Name);
	t->SetFile(*Name);
      }
    }
    delete timer;
    return;
  }


  //
  // TODO: 
  // keep record and check if program has already been recorded (re-transmission) ...
  // Store: Title, ShortText, Description --- channel ???
  // - detect failed/missed recordings ?
  // - when creating timers, create only for first event
  // - when event has been recorded, store info to db
  // - when creating timer, check if it has been already recorded or has another active timer 
  // --> if recording is missed, recording is retried automatically by new timer next time event is transmitted
  //
  //
    
  // aux field (used autotimer rule)
#if 0
  cstring s = timer->ToString();
  char *str = *s, *s2, *aux=NULL, *ns = NULL;
  if(s2=strstr(s, ":\n")) {
    *s2 = NULL;
    asprintf(&aux, ":%s\n", "autotimer: *** TODO ***");
    ns = malloc(strlen(s)+strlen(aux)+1);
    strcpy(ns, s);
    strcat(ns, aux);
    timer->Parse(ns);
      ISYSLOG("autotimer: setting aux to %s", aux);
  }
#else
//# warning todo
#endif

  // timer priority
  if(Priority >= 0)
    timer->SetPriority(Priority);
  // recording folder
  timer->SetFile(*Name);
  // autotimer flag
  timer->SetFlags(tfAutoTimer);
  
  Timers.Add(timer);
  timer->Matches();
  Timers.SetModified();
  Timers.Save();
  ISYSLOG("autotimer: Created timer %s %s %s prio %d (file %s) (%x %x v%d)", 
	  *date, *time, title, timer->Priority(), timer->File(),
	  event->TableID(), event->EventID(), event->Version());
}

static int64_t ScanEpg(bool Quick)
{
  time_t  now = time(NULL);
  time_t  FirstEnd = now + QUICKSCAN_INTERVAL;
  int     ch = 1, Priority = -1;
  cString Name;

  /* load and parse rule(s) */  
  cString SpecFile = AddDirectory(cPlugin::ConfigDirectory(), "autotimer.spec");
  cRule *Rule = cRule::RuleFromFile(*SpecFile);
  if(!Rule) {
    esyslog("autotimer: Can't load and parse %s", *SpecFile);
    return Quick ? QUICKSCAN_INTERVAL : FULLSCAN_INTERVAL;
  }

  /* lock schedule access */
  cSchedulesLock schedulesLock;
  const cSchedules *schedules = cSchedules::Schedules(schedulesLock);

  /* check EPG events against rule(s) */
  if(schedules) {
    cChannel *Channel = Channels.GetByNumber(ch);
    while(Channel) {
      const cSchedule *schedule = schedules->GetSchedule(Channel->GetChannelID());
      if(schedule && schedule->Events()) {
	int n = 0;
	for (cEvent *ev = schedule->Events()->First(); ev && (!Quick || n < 5);
	     ev = schedule->Events()->Next(ev), n++) {
	  if(Rule->Matches(ev, Name, Priority)) {
	    if(ev->EndTime() > now)
	      CreateTimer(ev, Name, Priority);
	    //else
	    //  isyslog("autotimer: skip %s %s,%s (end time %ld ; now %ld ; diff %ld",
	    //          ev->Title(), *ev->GetDateString(), *ev->GetTimeString(),
	    //          ev->EndTime(), now, ev->EndTime() - now);
	  }
	  if((n==4 || !schedule->Events()->Next(ev)) && ev->EndTime() < FirstEnd) {
	    if(ev->EndTime() > now)
	      FirstEnd = ev->EndTime();
	  }
	}
      }
      Channel = Channels.GetByNumber(++ch);
    }   
  }

  delete Rule;

  return FirstEnd - time(NULL);
}

#if 0
class cAtSetupMenu : public cMenuSetupPage
{
 public:
  cAtSetupMenu() {
    Add(new cOsdItem("Force full EPG scan now", osUser1));
  }
  virtual eOSState ProcessKey(eKeys Key);
};

eOSState cAtSetupMenu::ProcessKey(eKeys Key)
{
  m_plugin->ForceFullScan();
}
#endif

//--- cPluginAutotimer -------------------------------------------

static char *TimeToStr(time_t sec)
{
  static char buffer[64];
  struct tm tm_r;

  localtime_r(&sec, &tm_r);
  strftime(buffer, 63, "%Y-%m-%d %T", &tm_r);

  return buffer;
}

static const char *VERSION        = "0.2.0";
static const char *DESCRIPTION    = "Automatic timers from EPG";
static const char *MAINMENUENTRY  = NULL;//"Autotimer EPG scan";

class cPluginAutotimer : public cPlugin, cStatus {
private:
  // Add any member variables or functions you may need here.

  int64_t NextFullScan;
  int64_t NextQuickScan;

public:
  cPluginAutotimer(void);
  virtual ~cPluginAutotimer();

  // cPlugin interface
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);

  // cStatus interface
  virtual void Recording(const cDevice *Device, const char *Name, const char *FileName, bool On);
};

cPluginAutotimer::cPluginAutotimer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!

  NextFullScan  = time(NULL) + FULLSCAN_INTERVAL;
  NextQuickScan = time(NULL) + QUICKSCAN_INTERVAL;
}

cPluginAutotimer::~cPluginAutotimer()
{
  // Clean up after yourself!
}

const char *cPluginAutotimer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "Options:   --silent        Mimimal logging\n\n";
}

bool cPluginAutotimer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  if(argc > 1 && !strcmp(argv[1], "--silent"))
    g_autotimer_Silent = true;
  return true;
}

bool cPluginAutotimer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginAutotimer::Start(void)
{
  // Start any background activities the plugin shall perform.

  /* do scans soon after startup */
  NextFullScan  = time(NULL) + 120;
  NextQuickScan = time(NULL) + 60;

  return true;
}

void cPluginAutotimer::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginAutotimer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.

  // If this takes much time, do in slices (?)
  time_t now = time(NULL);
  if(NextFullScan <= now) {

    /* full EPG scan (daily) */
    int64_t now_ms = cTimeMs::Now();
    ISYSLOG("autotimer: performing full scan (%d seconds late)",
	    (int)(now - NextFullScan));
    ScanEpg(false);
    NextFullScan = now + FULLSCAN_INTERVAL;
    ISYSLOG("autotimer: full scan done (%d ms). Next full scan at %s",
	    (int)(cTimeMs::Now() - now_ms), TimeToStr(NextFullScan));
  }
}

void cPluginAutotimer::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!

  time_t now = time(NULL);
  if(NextQuickScan <= now) {

    /* scan few next events:
     *   - when previously scanned events have been ended
     *   - but at least once in 10 min
     */
    int64_t now_ms = cTimeMs::Now();
    ISYSLOG("autotimer: performing quick scan (%d seconds late)",
	    (int)(now - NextQuickScan));
    NextQuickScan = now + ScanEpg(true);
    if(NextQuickScan > (now + QUICKSCAN_INTERVAL))
      NextQuickScan = now + QUICKSCAN_INTERVAL;
    ISYSLOG("autotimer: quick scan done (%d ms). Next quick scan at %s",
	    (int)(cTimeMs::Now() - now_ms), TimeToStr(NextQuickScan));
  }
}

void cPluginAutotimer::Recording(const cDevice *Device, const char *Name, 
				 const char *FileName, bool On)
{
#if 0
  if(On) {
    ISYSLOG("autotimer: status -> recording %s", Name);
    if(Name && FileName) {
      for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti)) {
	if ((ti->HasFlags(tfActive)) && (ti->HasFlags(tfRecording)) && (ti->HasFlags(tfAutoTimer))) {
	  ISYSLOG("autotimer:   possible timer: %s", ti->File());
	  if(!strcmp(Name, ti->File())) {
	    ISYSLOG("  MATCH: %s == %s", ti->File(), Name);
	    const cEvent *ev = ti->Event();
	    if(ev) {
	      FILE *fp = fopen("/video/autotimer.rec.log","a");
	      if(fp) {
		fprintf(fp, "%s\t%s\t%s\n", ev->Title(), ev->ShortText(), ev->Description());
		fclose(fp);
		// TODO: Store FileName for Stop event
		// - add to recording now list
	      }
	    }
	  }
	}
      }
    }
  }
  else {
    ISYSLOG("autotimer: status -> STOP recording %s %s", Name, FileName);
    if(Name && FileName) {
      //cTimer *t0 = NULL;
      for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti)) {
	if ((ti->HasFlags(tfActive)) && /*(ti->HasFlags(tfRecording)) &&*/ (ti->HasFlags(tfAutoTimer))) {
	  ISYSLOG("autotimer:    possible timer: %s", ti->File());
	  // Name is NULL but FileName is still valid
	  // - search (and remove from) currently recording list
	  // - add to recorded list
	  // - do not create new autotimers for re-transmissions of same event
	  // - ? handle restarts during recording ?
	}
      }
    }
  }
#endif
}

cString cPluginAutotimer::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginAutotimer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.

  int64_t now = time(NULL);
  NextFullScan = now;
  Housekeeping();

  return NULL;
}

cMenuSetupPage *cPluginAutotimer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.

  int64_t now = time(NULL);
  NextFullScan = now;
  Housekeeping();

  return NULL;
}

bool cPluginAutotimer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginAutotimer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginAutotimer::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginAutotimer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginAutotimer); // Don't touch this!

/*
 * timersync.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#include <getopt.h>

#include <vdr/plugin.h>
#include <vdr/channels.h>
#include <vdr/timers.h>
#include <vdr/menu.h>
#include <vdr/skins.h>

#include "../svdrpservice/svdrpservice.h"


int IndexOf(cTimers& Pool, cTimer *Timer)
{
  cString str = Timer->ToText();
  for(cTimer *t = Pool.First(); t; t = Pool.Next(t))
    if(!strcmp(str, t->ToText()))
      return t->Index();
  return -1;
}

bool Contains(cTimers& Pool, cTimer *Timer)
{
  cString str = Timer->ToText();
  for(cTimer *timer = Pool.First(); timer; timer = Pool.Next(timer))
    if(!strcmp(str, timer->ToText()))
      return true;
  return false;
}

cTimer *str2timer(const char *s)
{
  cTimer *t = new cTimer;
  if(!t->Parse(s))
    isyslog("str2timer: error parsing timer \'%s\'", s);
  return t;
}


static const char *VERSION        = "0.1.0";
static const char *DESCRIPTION    = "Synchronize timers with server";
//static const char *MAINMENUENTRY  = "TimerSync status";

#define tfRemote  ((eTimerFlags)(0x0400))

#define SYNC_INTERVAL   (10*60)    // 10*60s = 10 min
#define RETRY_INTERVAL  (60)       // 60s = 1 min

#define DEFAULT_SERVER  "0.0.0.0"  // svdrpservice default
#define DEFAULT_PORT    0          // svdrpservice default


class cPluginTimersync : public cPlugin {
private:

  // Optional configuration
  cString ServerAddress;
  int     ServerPort;

  // State
  bool    SyncActive;
  int     TimersState;
  time_t  LastSync;
  time_t  SyncInterval;

  // Remote
  cPlugin *svdrpService;

  cTimers ServerTimers;  // what we got at last sync
  cTimer  **TimerPointers;

public:
  cPluginTimersync(void);
  virtual ~cPluginTimersync();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void MainThreadHook(void);
  //virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  //virtual cOsdObject *MainMenuAction(void) { SyncTimers(); }
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);

  bool SyncTimers(void);
  void HandleChannelModifications(void);
};

cPluginTimersync::cPluginTimersync(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!

  ServerAddress = DEFAULT_SERVER;
  ServerPort = DEFAULT_PORT;

  SyncActive = false;
  TimersState = 0;
  LastSync = 0;
  SyncInterval = SYNC_INTERVAL;

  svdrpService = NULL;
  TimerPointers = NULL;
}

cPluginTimersync::~cPluginTimersync()
{
  free(TimerPointers);
}

const char *cPluginTimersync::CommandLineHelp(void)
{
  return "  -s <IP[:PORT]>, --server=<IP[:PORT]>  VDR Server address and optional SVDRP port\n";
}

bool cPluginTimersync::ProcessArgs(int argc, char *argv[])
{
  static const struct option long_options[] = {
    { "server", required_argument, NULL, 's' },
    { NULL, 0, NULL, 0 }
  };

  int c;
  const char *pt;
  while((c = getopt_long(argc, argv, "s:", long_options, NULL)) != -1) {
    switch (c) {
      case 's':
	ServerAddress = optarg;
	if(NULL != (pt = strchr(ServerAddress, ':'))) {
	  ServerPort = atoi(pt+1);
	  *(char*)pt = 0;
	}
	break;
      default:
	return false;
    }
  }
  return true;
}

bool cPluginTimersync::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.

  // Disable recording and SVDRP timer modifications - for ever !
  Timers.IncBeingEdited(); 

  return true;
}

bool cPluginTimersync::Start(void)
{
  // Start any background activities the plugin shall perform.

  svdrpService = cPluginManager::GetPlugin("svdrpservice");
  if (!svdrpService) {
    isyslog("timersync: svdrpservice plugin not found, aborting !");
    Timers.DecBeingEdited(); 
    return false;
  }

  if(Timers.Count()) {
    isyslog("timersync: WARNING: timers already defined, aborting !");
    Timers.DecBeingEdited(); 
    return false;
  }

  Timers.Modified(TimersState);
  isyslog("timersync: WARNING: RECORDING IN THIS VDR INSTANCE DISABLED !");
  SyncActive = true;

  return true;
}

bool cPluginTimersync::SyncTimers(void)
{
  bool Result = false;

  //
  // get a connection handle. This will open the connection to the server
  // or for a shared connection simply add a referer to the existing one.
  //

  SvdrpConnection_v1_0 conn;
  conn.serverIp = ServerAddress;
  conn.serverPort = ServerPort;
  conn.shared = true;            // share connection with others
  conn.handle = -1;              // Handle < 0: get me a handle
  svdrpService->Service("SvdrpConnection-v1.0", &conn);
  if (conn.handle < 0) {
    isyslog("timersync: can't connect to %s:%hu (SVDRP)", *conn.serverIp, conn.serverPort);
    return false;
  }
  dsyslog("timersync: Connected to %s:%hu", *conn.serverIp, conn.serverPort);
  ServerAddress = conn.serverIp;

  //
  // Verify we're not running at other end
  //

  SvdrpCommand_v1_0 cmd;
  cmd.handle = conn.handle;
  cmd.command = "PLUG timersync PING\r\n";
  cmd.responseCode = 500;
  svdrpService->Service("SvdrpCommand-v1.0", &cmd);

  if (cmd.responseCode == 900) {
    isyslog("timersync: plugin active at server, aborting sync !");
    SyncActive = false;
    svdrpService->Service("SvdrpConnection-v1.0", &conn);
    return false;
  }
  if (cmd.responseCode == 500) {
    isyslog("timersync: Can't check if plugin is active at server, aborting sync !");
    SyncActive = false;
    svdrpService->Service("SvdrpConnection-v1.0", &conn);
    return false;
  }

  //
  // build list of timers to be removed and added (=add/del/modified)
  //

  cTimers added, deleted;

  if(Timers.Count() > 0 || ServerTimers.Count() > 0) {
    cTimer *local, *server;

    for(local = Timers.First(); local; local = Timers.Next(local)) {
      if(!Contains(ServerTimers, local)) {
	// added or modified
	isyslog("timersync: Local timer has been added: %s", *local->ToText());
	added.Add(str2timer(local->ToText()));
// - store actual cTimer pointers 
//   --> can tell what timers were really new and what only modified ...
//   TimerPointers[]
// - or check tfRemote flag
      }
    }

    for(server = ServerTimers.First(); server; server = ServerTimers.Next(server)) {
      if(!Contains(Timers, server)) {
	// removed or modified
	isyslog("timersync: Local timer has been removed: %s", *server->ToText());
	deleted.Add(str2timer(server->ToText()));
      }
    }
  }

  //
  // refresh timer list from server
  // 

  cmd.command = "LSTT ID\r\n";
  svdrpService->Service("SvdrpCommand-v1.0", &cmd);

  if (cmd.responseCode != 250) {
    isyslog("timersync: SVDRP LSTT failed");

  } else {

    // Step through the reply
    Timers.cList<cTimer>::Clear();
    ServerTimers.cList<cTimer>::Clear();

    free(TimerPointers);
    TimerPointers = (cTimer**)malloc(sizeof(cTimer*) * cmd.reply.Count() + 1);
    memset(TimerPointers, 0, sizeof(cTimer*) * cmd.reply.Count() + 1);
    int count = 0;

    for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line)) {

      cTimer *t = new cTimer;
      t->Parse( strchr(line->Text(), ' '));
      t->SetFlags(t->Flags() | tfRemote);
      Timers.Add(t);
      TimerPointers[count++] = t;

      // update cache
      t = new cTimer;
      t->Parse( strchr(line->Text(), ' '));
      t->SetFlags(t->Flags() | tfRemote);
      ServerTimers.Add(t);
    }
    TimerPointers[count] = NULL;

    Result = true;
  }

  //
  // Execute remove
  //

  cTimer *rm;
  bool Committed = false;
  while(NULL != (rm = deleted.Last())) {
    int ind = IndexOf(Timers, rm);
    if(ind < 0) {
      isyslog("timersync: CAN'T DELETE TIMER (modified or deleted at server): %s", *rm->ToText());
      Skins.Message(mtError, "Can't remove timer from server", 3);
    } else {
      isyslog("timersync: removing server timer %d (%s)", ind, *rm->ToText());

      cString svdrpcmd = cString::sprintf("DELT %d\r\n", ind+1);
      cmd.command = svdrpcmd;
      svdrpService->Service("SvdrpCommand-v1.0", &cmd);
      if (cmd.responseCode != 250) {
	isyslog("timersync: SVDRP %s failed !", *svdrpcmd);
	isyslog(" --> %s <-- ", cmd.reply.First()->Text());
	Skins.Message(mtError, "Can't remove timer from server", 3);
      } else
	Committed = true;

      Timers.Del(Timers.Get(ind));
      ServerTimers.Del(ServerTimers.Get(ind));
    }
    Timers.SetModified();
    deleted.Del(rm);
  }

  //
  // Execute add
  //

  cTimer *add;
  while(NULL != (add = added.First())) {
    isyslog("timersync: adding timer %s", *add->ToText());

    cString svdrpcmd = cString::sprintf("NEWT %s\r\n", *add->ToText());
    cmd.command = svdrpcmd;
    svdrpService->Service("SvdrpCommand-v1.0", &cmd);

    if (cmd.responseCode != 250) {
      isyslog("timersync: SVDRP %s failed !", *svdrpcmd);
      isyslog(" --> %s <-- ", cmd.reply.First()->Text());
      Skins.Message(mtError, "Can't add timer to server", 3);
    } else
      Committed = true;

    Timers.Add(str2timer(add->ToText()));
    ServerTimers.Add(str2timer(add->ToText()));
    Timers.SetModified();

    added.Del(add);
  }

#if 0
  if(Committed)
    Skins.Message(mtInfo, "Timer modifications saved to server", 1);
#endif

  //
  // Finally release the connection (handle >= 0). This will close the
  // SVDRP connection unless an other plugin still uses it.
  //

  svdrpService->Service("SvdrpConnection-v1.0", &conn);
  return Result;
}

void cPluginTimersync::HandleChannelModifications(void)
{
  //
  // This doesn't run in VDR main loop anymore
  // (Timers.BeingEdited() > 0), so do "maintenance" here ...
  //
# define CHANNELSAVEDELTA  600 // seconds before saving channels.conf after automatic modifications

  // Handle channel and timer modifications:
  if (!Channels.BeingEdited() && Timers.BeingEdited() == 1) {
    int modified = Channels.Modified();
    static time_t ChannelSaveTimeout = 0;
    /*static int TimerState = 0;*/
    // Channels and timers need to be stored in a consistent manner,
    // therefore if one of them is changed, we save both.
    if (modified == CHANNELSMOD_USER /*|| Timers.Modified(TimerState)*/)
      ChannelSaveTimeout = 1; // triggers an immediate save
    else if (modified && !ChannelSaveTimeout)
      ChannelSaveTimeout = time(NULL) + CHANNELSAVEDELTA;
    bool timeout = ChannelSaveTimeout == 1 || ChannelSaveTimeout && time(NULL) > ChannelSaveTimeout && !cRecordControls::Active();
    if ((modified || timeout) && Channels.Lock(false, 100)) {
      if (timeout) {
	Channels.Save();
	//Timers.Save();
	ChannelSaveTimeout = 0;
      }
      for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
	if (Channel->Modification(CHANNELMOD_RETUNE)) {
	  cRecordControls::ChannelDataModified(Channel);
	  if (Channel->Number() == cDevice::CurrentChannel()) {
	    if (!cDevice::PrimaryDevice()->Replaying() || cDevice::PrimaryDevice()->Transferring()) {
	      if (cDevice::ActualDevice()->ProvidesTransponder(Channel)) { // avoids retune on devices that don't really access the transponder
		isyslog("retuning due to modification of channel %d", Channel->Number());
		Channels.SwitchTo(Channel->Number());
	      }
	    }
	  }
	}
      }
      Channels.Unlock();
    }
  }
}

void cPluginTimersync::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!

  if(!SyncActive)
    return;

  if(Timers.BeingEdited() > 1) {
    // timers menu open (?)
    return;
  }

  HandleChannelModifications();
  
  if(Timers.Modified(TimersState)) {
    isyslog("timersync: timers have been modified, new state = %d. "
	    "committing changes to %s", TimersState, *ServerAddress);
    SyncTimers();
    LastSync = 0;
    return;
  }

  if(LastSync + SyncInterval < time(NULL)) {
    isyslog("timersync: syncing with %s", *ServerAddress);
    if( SyncTimers() )
      LastSync = time(NULL);
    else
      LastSync = time(NULL) - SyncInterval + RETRY_INTERVAL;
  }
}

cString cPluginTimersync::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  if(!strcasecmp(Command, "PING")) {
    return cString("PING");
  }
  return NULL;
}

VDRPLUGINCREATOR(cPluginTimersync); // Don't touch this!

/*
 * taste.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: taste.c,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#include <vdr/plugin.h>

#include "status.h"
#include "event.h"
#include "setup.h"
#include "config.h"
#include "i18n.h"

static const char *VERSION        = "0.0.2d";
static const char *DESCRIPTION    = "Lock unwanted shows by keywords";
static const char *MAINMENUENTRY  = "Schedule not acceptable";

class cPluginTaste : public cPlugin {
private:
  cStatusTaste *mStatus;

public:
  cPluginTaste(void);
  virtual ~cPluginTaste();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual const char *MainMenuEntry(void) { return SetupTaste.HideMenuEntry ? NULL : tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginTaste::cPluginTaste(void):
    cPlugin(),
    mStatus(NULL)
{
}

cPluginTaste::~cPluginTaste()
{
  delete mStatus;
}

bool cPluginTaste::Initialize(void)
{
  return EventsTaste.Load(AddDirectory(cPlugin::ConfigDirectory(), "taste.conf"), true, false);
}

bool cPluginTaste::Start(void)
{
  RegisterI18n(Phrases);
  mStatus = new cStatusTaste();
  return true;
}

cOsdObject *cPluginTaste::MainMenuAction(void)
{
  const cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (channel != NULL && !channel->GroupSep()) { 
    cSchedulesLock schedLock;
    const cSchedules *scheds = cSchedules::Schedules(schedLock);
    if (scheds == NULL)
      return NULL;

    const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
    if (sched == NULL)
      return NULL;

    const cEvent *present = sched->GetPresentEvent(true);
    EventsTaste.Add(new cEventTaste(present->Title()));
    EventsTaste.Save();
  }
  return NULL;
}

cMenuSetupPage *cPluginTaste::SetupMenu(void)
{
  return new cMenuSetupTaste();
}

bool cPluginTaste::SetupParse(const char *Name, const char *Value)
{
  return SetupTaste.Parse(Name, Value);
}

VDRPLUGINCREATOR(cPluginTaste); // Don't touch this!

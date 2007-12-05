#include "switchtimer.h"
#include "epgsearchtools.h"

cSwitchTimers SwitchTimers;

// -- cSwitchTimer -----------------------------------------------------------------
cSwitchTimer::cSwitchTimer(void)
{
    event = NULL;
    switchMinsBefore = 1;
    announceOnly = false;
}

cSwitchTimer::cSwitchTimer(const cEvent* Event, int SwitchMinsBefore, int AnnounceOnly)
{
    event = Event;
    switchMinsBefore = SwitchMinsBefore;
    announceOnly = AnnounceOnly;
}

bool cSwitchTimer::Parse(const char *s)
{
  char *line;
  char *pos;
  char *pos_next;
  int parameter = 1;
  int valuelen;
#define MAXVALUELEN (10 * MaxFileName)

  char value[MAXVALUELEN];

  cChannel* channel = NULL;
  unsigned int eventID = 0;
  time_t startTime=0;
  
  pos = line = strdup(s);
  pos_next = pos + strlen(pos);
  if (*pos_next == '\n') *pos_next = 0;
  while (*pos) {
    while (*pos == ' ') pos++;
    if (*pos) {
      if (*pos != ':') {
        pos_next = strchr(pos, ':');
        if (!pos_next)
          pos_next = pos + strlen(pos);
        valuelen = pos_next - pos + 1;
        if (valuelen > MAXVALUELEN) 
	    valuelen = MAXVALUELEN;
        strn0cpy(value, pos, valuelen);
        pos = pos_next;
        switch (parameter) {
	    case 1:  
		channel = Channels.GetByChannelID(tChannelID::FromString(value), true, true);
		break;
	    case 2:
		eventID = atoi(value);
		break;
	    case 3:
		startTime = atol(value);
		break;
	    case 4:
		switchMinsBefore = atoi(value);
		break;
	    case 5:
		announceOnly = atoi(value);
		break;
        } //switch
      }
      parameter++;
    }
    if (*pos) pos++;
  } //while
  
  time_t now = time(NULL);
  event = NULL;
  if (startTime > now && channel)
  {
      cSchedulesLock schedulesLock;
      const cSchedules* schedules = cSchedules::Schedules(schedulesLock);
      const cSchedule *Schedule = schedules->GetSchedule(channel->GetChannelID());
      if (Schedule) 
      {
	  event = Schedule->GetEvent(eventID, startTime);
	  if (!event)
	      event = Schedule->GetEventAround(startTime);
      }
  }
  free(line);
  return (parameter >= 3) ? true : false;
}

const char *cSwitchTimer::ToText(bool& ignore)
{
    ignore = false;
    if (!event)
    {
	ignore = true;
	return NULL;
    }
    cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true, true);
    if (!channel) return NULL;
    char* buffer = NULL;
    asprintf(&buffer, "%s:%u:%ld:%d:%d", CHANNELSTRING(channel), event->EventID(), event->StartTime(), switchMinsBefore, announceOnly?1:0);
    return buffer;
}

bool cSwitchTimer::Save(FILE *f)
{
    bool ignore = false;
    const char* buffer = ToText(ignore);
    if (!ignore)
	return fprintf(f, "%s\n", buffer) > 0;
    return true;
}


cSwitchTimer* cSwitchTimers::InSwitchList(const cEvent* event)
{
    if (!event) return false;
    cMutexLock SwitchTimersLock(this);
    cSwitchTimer* switchTimer = SwitchTimers.First();
    while (switchTimer) 
    {
	if (switchTimer->event == event)
	    return switchTimer;
	switchTimer = SwitchTimers.Next(switchTimer);
    }
    return NULL;
}

bool cSwitchTimers::Exists(const cSwitchTimer* SwitchTimer)
{
    cMutexLock SwitchTimersLock(this);
    cSwitchTimer* switchTimer = SwitchTimers.First();
    while (switchTimer) 
    {
	if (switchTimer == SwitchTimer)
	    return true;
	switchTimer = SwitchTimers.Next(switchTimer);
    }
    return false;
}

/*
 * recstatus.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/menu.h>
#include "i18n.h"

static const char *VERSION        = "0.0.5";
static const char *DESCRIPTION    = trNOOP("recording status monitor");
static const char *MAINMENUENTRY  = trNOOP("Recording status");

#undef DAYDATETIMESTRING
#if VDRVERSNUM >= 10318
#define DAYDATETIME(x) *DayDateTime(x)
#else
#define DAYDATETIME(x) DayDateTime(x)
#endif

// --- cRecObj --------------------------------------------------------
class cRecObj : public cListObject {
public:
    char* name;
    const cDevice* device;
    cTimer* timer;
public:
    cRecObj(const char* Name, const cDevice* Device, cTimer* Timer)
	{
	    name = strdup(Name);
	    device = Device;
	    timer = Timer;
	}
    ~cRecObj()
	{
	    if (name) free(name);
	}
 };

class cRecStatusMonitor : public cStatus 
{
protected:
#if VDRVERSNUM >= 10338
    virtual void Recording(const cDevice *Device, const char *Name, const char *FileName, bool On);
#else
    virtual void Recording(const cDevice *Device, const char *Name);
#endif
 public:
    cRecStatusMonitor();
};

cList<cRecObj> CurrentRecordings;

// --- cMenuRecItem ------------------------------------------------------
class cMenuRecItem : public cOsdItem {
  char *Name;
public: 
  cMenuRecItem(const char* name) 
      { 
	  Name = NULL;
	  if (name) {  Name = strdup(name); SetText(Name, false); }
      }
  cMenuRecItem(const cRecObj* r) 
      { 
	  Name = NULL;
	  if (r->name) 
	  {  
	      Name = strdup(r->name); 
	      char* itemText = NULL;
	      asprintf(&itemText, "%s\t%s", DAYDATETIME(r->timer->StartTime()), Name);
	      SetText(itemText, false); 
	  }
      }
  char* RecName() { return Name; }
};

class cMenuRecStatus : public cOsdMenu {
private:
public:
    cMenuRecStatus():cOsdMenu(tr("Recording status"), 15)
      {
	  for (int i = 0; i < cDevice::NumDevices(); i++) 
	  {
	      cDevice *d = cDevice::GetDevice(i);
	      char* devName = NULL;
	      char* devInfo = NULL;
	      if (d->HasDecoder() || d->IsPrimaryDevice())
	          asprintf(&devInfo, " (%s%s%s)", d->HasDecoder() ? tr("device with decoder") : "", (d->HasDecoder() && d->IsPrimaryDevice()) ? ", " : "", d->IsPrimaryDevice() ? tr("primary device") : "");

	      asprintf(&devName, "--- %s %d%s ---", tr("Device"), i+1, devInfo ? devInfo : "");
	      cMenuRecItem* DeviceHeader =  new cMenuRecItem(devName);
	      DeviceHeader->SetSelectable(false);
	      Add(DeviceHeader);
	      free(devName);
	      if (devInfo)
	          free(devInfo);
	      int Count = 0;
	      for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)) 
	      {
		  if (r && r->device == d)
		  {
		      Add(new cMenuRecItem(r));
		      Count++;
		  }
	      }
	      if (Count == 0)
	      {
		  cMenuRecItem* norec =  new cMenuRecItem(tr("no current recordings"));
		  norec->SetSelectable(false);
		  Add(norec);
	      }
	      if (i < cDevice::NumDevices())
	      {
	          DeviceHeader =  new cMenuRecItem("");
	          DeviceHeader->SetSelectable(false);
	          Add(DeviceHeader);
              }
	  }	 
      }
    eOSState Play(char* file)
	{
	    cRecording* recordingFound = NULL;
	    for(cRecording* recording = Recordings.First(); recording; recording = Recordings.Next(recording))
		if (strstr(recording->Title(), file))
		    recordingFound = recording;
	    if (!recordingFound)
		return osContinue;
	    cReplayControl::SetRecording(recordingFound->FileName(), recordingFound->Title());
	    return osReplay;
	}
    eOSState ProcessKey(eKeys Key)
	{
	    eOSState state = cOsdMenu::ProcessKey(Key);	    
	    if (state == osUnknown) 
	    {
		if (Key == kOk)
		{
		    cMenuRecItem *ri = (cMenuRecItem*)Get(Current());
		    if (ri->Selectable())
			return Play(ri->RecName());
		}
		else
		    state = osContinue;
	    }
	    return state;
	}
  };



class cPluginRecstatus : public cPlugin {
private:
    cRecStatusMonitor* recStatusMonitor;
  // Add any member variables or functions you may need here.
public:
  cPluginRecstatus(void);
  virtual ~cPluginRecstatus();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginRecstatus::cPluginRecstatus(void)
{
    recStatusMonitor = NULL;
}

cPluginRecstatus::~cPluginRecstatus()
{
    delete recStatusMonitor;
}

const char *cPluginRecstatus::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRecstatus::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRecstatus::Initialize(void)
{
#if APIVERSNUM < 10507
   RegisterI18n(Phrases);
#endif
  return true;
}

bool cPluginRecstatus::Start(void)
{
    recStatusMonitor = new cRecStatusMonitor;
    return true;
}

void cPluginRecstatus::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRecstatus::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginRecstatus::MainMenuAction(void)
{
    return new cMenuRecStatus();
}

cMenuSetupPage *cPluginRecstatus::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginRecstatus::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginRecstatus::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRecstatus::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginRecstatus::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

cRecStatusMonitor::cRecStatusMonitor()
{
}


#if VDRVERSNUM >= 10338 
void cRecStatusMonitor::Recording(const cDevice *Device, const char *Name, const char *FileName, bool On)
#else
void cRecStatusMonitor::Recording(const cDevice *Device, const char *Name)
#endif
{
    // insert new timers currently recording in TimersRecording
    if (Name)
    {
	for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti)) 
	    if (ti->Recording())
	    {
		// check if this is a new entry
		bool bFound = false;
		for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)) 
		    if (r->timer == ti)
			bFound = true;

		if (bFound) continue; // already handled
		CurrentRecordings.Add(new cRecObj(Name, Device, ti));
		return;
	    }
    }
    
    if (!Name)
    {
	// remove timers that finished recording from TimersRecording
	for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)) 
	{
	    if (!r->timer->Recording())
	    {
		CurrentRecordings.Del(r);
		break;
	    }
	}
    }
}

VDRPLUGINCREATOR(cPluginRecstatus); // Don't touch this!

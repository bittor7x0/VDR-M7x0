/*
 * radioinfo.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/receiver.h>
#include <vdr/remote.h>

#include "radioInfoOsd.h"
#include "radioInfoReceiver.h" 
#include "radioInfoFilter.h"
#include "sRadioInfo.h"
#include "radioinfo.h" 
#include "tools.h"
#include "config.h"
#include "radioInfoSetupMenu.h"


static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = "Provides extra information for radio channels";
static const char *MAINMENUENTRY  = NULL;


cPluginRadioinfo::cPluginRadioinfo(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  //radioInfoMonitor = NULL;
  radioInfoOsd      = NULL;
  radioInfoReceiver = NULL;
  radioInfoFilter   = NULL;
  
  currentChannel = NULL;
  currentDevice  = NULL;
  
  radioInfo = new sRadioInfo;
  radioInfo->updating = true;
  
  wantsOsd = false;
  
  currentRadioInfo = this;

  config.MAX_RETRIES  = 30;
  config.QUICK_DETECT = true;
  config.DELAY        = 2;
  config.SCAN_DELAY   = 5; 
}

cPluginRadioinfo::~cPluginRadioinfo()
{
  // Clean up after yourself!    
  if (radioInfoOsd)      delete radioInfoOsd;
  if (radioInfoReceiver) delete radioInfoReceiver;
  if (radioInfoFilter)   delete radioInfoFilter;
  if (radioInfo)         delete radioInfo;
}


const char* cPluginRadioinfo::Version(void)       { return VERSION; }
const char* cPluginRadioinfo::Description(void)   { return DESCRIPTION; }
const char* cPluginRadioinfo::MainMenuEntry(void) { return MAINMENUENTRY; }


const char *cPluginRadioinfo::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRadioinfo::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRadioinfo::Initialize(void)
{
    
  return true;
}

bool cPluginRadioinfo::Start(void)
{
  // Start any background activities the plugin shall perform. 
  // Initialize any background activities the plugin shall perform.
  
  radioInfoFilter = new cRadioInfoFilter;
  
  return true;
}

void cPluginRadioinfo::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRadioinfo::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginRadioinfo::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  if (wantsOsd)
  { 
    if (!radioInfoOsd)
      radioInfoOsd = new cRadioInfoOsd(&radioInfoOsd, radioInfo);
    else {
      DEBUG_MSG("Trying to create osd, one already exists");
    }
      
    wantsOsd = false;
    return radioInfoOsd;
  }
  else
    return NULL;
}

cMenuSetupPage *cPluginRadioinfo::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cRadioInfoSetupMenu;
}

bool cPluginRadioinfo::SetupParse(const char *Name, const char *Value)
{ 
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "MAX_RETRIES"))   config.MAX_RETRIES  = atoi(Value);
  else if (!strcasecmp(Name, "QUICK_DETECT"))  config.QUICK_DETECT = atoi(Value);
  else if (!strcasecmp(Name, "DELAY"))         config.DELAY        = atoi(Value);
  else if (!strcasecmp(Name, "SCAN_DELAY"))    config.SCAN_DELAY   = atoi(Value);
  
  else return false;
  
  return true;
}

bool cPluginRadioinfo::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRadioinfo::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginRadioinfo::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}


void cPluginRadioinfo::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{  
  if (ChannelNumber)
  {
    if (radioInfoReceiver)
    { 
      delete radioInfoReceiver;
      radioInfoReceiver = NULL;
    }
  
    const cChannel* chan = Channels.GetByNumber(ChannelNumber);
		 		
		// Is it a radio channel?
		if (chan && chan->Vpid() == 0 && chan->Apid(0) != 0) 
		{
      currentChannel = (cChannel*) chan;
      currentDevice  = (cDevice*)  Device;
  
		  resetInfo();
		   
		  // Find Info PID
      radioInfoFilter->Attach(currentDevice);
      
      // Ready for OSD
		  wantsOsd = true;
		  
		}
		else // Not a radio channel
		{
		  wantsOsd = false;
		  if (radioInfoOsd)
		    hideOsd();
		}
  }
  else // About to switch channel
  {
    radioInfoFilter->Detach();
    
    if (radioInfoReceiver) {
      radioInfoReceiver->Detach();    
    }
    
    currentChannel = NULL;
    currentDevice  = NULL;
  }

}


void cPluginRadioinfo::OsdClear(void) 
{ 
  if (wantsOsd) { 
    showOsd(); 
  }
}


void cPluginRadioinfo::showOsd(void)
{
  DEBUG_MSG("Opening OSD");
  //TODO: Some devices need the screen cleared; use something like
  //      cDevice::PrimaryDevice()->StillPicture(const uchar *Data, int Length); 
  cRemote::CallPlugin("radioinfo"); 
}


void cPluginRadioinfo::hideOsd(void)
{
  DEBUG_MSG("Closing OSD");
  
  if (radioInfoOsd) {
    radioInfoOsd->ProcessKey(kMenu, false); // called to restore main menu
  }

  cRemote::CallPlugin("radioinfo"); // Hide menu
 
  // Note: do not delete radioInfoOsd here
}



void cPluginRadioinfo::foundInfoPid(int Pid)
{  
  if (Pid != -1)
  {
    // Start receiving data from info PID

    if (radioInfoReceiver) { DEBUG_MSG("Receiver already exits; this shoudn't happen!"); }
    
    #if VDRVERSNUM >= 10500    
      radioInfoReceiver = new cRadioInfoReceiver(Pid, currentChannel->GetChannelID(), radioInfo);
    #else
      radioInfoReceiver = new cRadioInfoReceiver(Pid, currentChannel->Ca(), radioInfo);
    #endif
    
    radioInfoReceiver->Attach(currentDevice);
  }
  
  else // No PID found
  {
    //TODO: If no PID is found perhaps the program should periodically retry to find one.
  
  }
}



void cPluginRadioinfo::resetInfo(void)
{
  if (radioInfo)
  {
    radioInfo->updating = true;
    strcpy(radioInfo->title, "No Info");
    radioInfo->artist[0] = '\0';
    radioInfo->extra1[0] = '\0';
    radioInfo->extra2[0] = '\0';
    radioInfo->extra3[0] = '\0';
    radioInfo->updating = false;  
  }
}


sRadioInfoConfig config;


VDRPLUGINCREATOR(cPluginRadioinfo); // Don't touch this!

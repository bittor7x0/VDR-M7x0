/*
 * radioinfo.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#include <vdr/remote.h>

#include "radioInfoFilter.h"
#include "radioInfoData.h"
#include "radioinfo.h" 
#include "tools.h"
#include "config.h"
#include "radioInfoSetupMenu.h"

#if VDRVERSNUM < 10510
#error This version of RadioInfo only works with VDR 1.5.10 or above.
#endif 


///////////////////////////////////////////////////////////////////////////////


static const char *VERSION        = "0.1.0";
static const char *DESCRIPTION    = "Provides extra information for radio channels";
static const char *MAINMENUENTRY  = NULL;

cPluginRadioinfo* cPluginRadioinfo::currentRadioInfo = NULL;

sRadioInfoConfig config;


///////////////////////////////////////////////////////////////////////////////


cPluginRadioinfo::cPluginRadioinfo(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!

  radioInfoOsd      = NULL;
  radioInfoReceiver = NULL;
  
  currentChannel = NULL;
  currentDevice  = NULL;
  
  currentRadioInfo = this;

  config.maxRetries  = 30;
  config.quickDetect = true;
  config.osdDelay    = 2;
  config.scanDelay   = 5; 
}


//-----------------------------------------------------------------------------

cPluginRadioinfo::~cPluginRadioinfo()
{
  // Clean up after yourself!    
  delete radioInfoOsd;
  delete radioInfoReceiver;
}

//-----------------------------------------------------------------------------

const char* cPluginRadioinfo::Version(void)       { return VERSION; }
const char* cPluginRadioinfo::Description(void)   { return DESCRIPTION; }
const char* cPluginRadioinfo::MainMenuEntry(void) { return MAINMENUENTRY; }


//-----------------------------------------------------------------------------

const char *cPluginRadioinfo::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}


//-----------------------------------------------------------------------------

bool cPluginRadioinfo::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}


//-----------------------------------------------------------------------------

bool cPluginRadioinfo::Initialize(void)
{
  return true;
}


//-----------------------------------------------------------------------------

bool cPluginRadioinfo::Start(void)
{
  // Start any background activities the plugin shall perform. 
  // Initialize any background activities the plugin shall perform.
  
  return true;
}


//-----------------------------------------------------------------------------

void cPluginRadioinfo::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}


//-----------------------------------------------------------------------------

void cPluginRadioinfo::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}


//-----------------------------------------------------------------------------

cOsdObject *cPluginRadioinfo::MainMenuAction(void)
{
  return NULL;
}


//-----------------------------------------------------------------------------

cMenuSetupPage *cPluginRadioinfo::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cRadioInfoSetupMenu;
}


//-----------------------------------------------------------------------------
 
bool cPluginRadioinfo::SetupParse(const char *Name, const char *Value)
{ 
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "maxRetries"))   config.maxRetries  = atoi(Value);
  else if (!strcasecmp(Name, "quickDetect"))  config.quickDetect = atoi(Value);
  else if (!strcasecmp(Name, "osdDelay"))     config.osdDelay    = atoi(Value);
  else if (!strcasecmp(Name, "scanDelay"))    config.scanDelay   = atoi(Value);
  
  else return false;
  
  return true;
}


//-----------------------------------------------------------------------------

bool cPluginRadioinfo::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}


//-----------------------------------------------------------------------------

const char **cPluginRadioinfo::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}


//-----------------------------------------------------------------------------

cString cPluginRadioinfo::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}


//-----------------------------------------------------------------------------

void cPluginRadioinfo::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{  
  if (currentDevice && Device != currentDevice) return;

  if (ChannelNumber)
  { 
    const cChannel* chan = Channels.GetByNumber(ChannelNumber);
		 		
		// Is it a radio channel?
		if (chan && chan->Vpid() == 0 && chan->Apid(0) != 0) 
		{
      currentChannel = (cChannel*) chan;
      currentDevice  = (cDevice*)  Device;
  
		  radioInfoData.Reset();
		   
		  // Find Info PID
      cRadioInfoFilter::Instance()->Attach(currentDevice);
      
      if (radioInfoOsd) {
        DEBUG_MSG("OSD already exits; this shoudn't happen!");
        return;	
      }
      
      radioInfoOsd = new cRadioInfoOsd(&radioInfoData);	    
		}
		else // Not a radio channel
		{      

		}
  }
  else // About to switch channel
  {
    cRadioInfoFilter::Instance()->Detach();
    
    if (radioInfoReceiver) {
      radioInfoReceiver->Detach();
      delete radioInfoReceiver;
      radioInfoReceiver = NULL;    
    }
    
    if (radioInfoOsd) {
      delete radioInfoOsd;
      radioInfoOsd = NULL;
    }   
      
    currentChannel = NULL;
    currentDevice  = NULL;
  }
  
}


//-----------------------------------------------------------------------------

void cPluginRadioinfo::FoundInfoPid(int Pid)
{  
  if (Pid != -1)
  {
    // Start receiving data from info PID

    if (radioInfoReceiver) { DEBUG_MSG("Receiver already exits; this shoudn't happen!"); }
    
    radioInfoReceiver = new cRadioInfoReceiver(Pid, currentChannel->GetChannelID(), &radioInfoData);

    radioInfoReceiver->Attach(currentDevice);
  }
  else // No PID found
  {
    //TODO: If no PID is found perhaps the program should periodically retry to find one.
  }
}


///////////////////////////////////////////////////////////////////////////////


VDRPLUGINCREATOR(cPluginRadioinfo); // Don't touch this!


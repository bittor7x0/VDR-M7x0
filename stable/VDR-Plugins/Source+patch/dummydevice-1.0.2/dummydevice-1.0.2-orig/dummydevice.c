/*
 * dummydevice.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/config.h>
#include <vdr/device.h>

class cDummyDevice : cDevice {
public:
    cDummyDevice() : cDevice() {}
    virtual ~cDummyDevice() {}

    virtual bool HasDecoder(void) const { return true; }

    virtual bool SetPlayMode(ePlayMode PlayMode) {return true;}
    virtual int  PlayVideo(const uchar *Data, int Length) {return Length;}

#if VDRVERSNUM < 10318
    virtual void PlayAudio(const uchar *Data, int Length) {return Length;}
#elif  VDRVERSNUM < 10342
    virtual int  PlayAudio(const uchar *Data, int Length) {return Length;}
#else
    virtual int  PlayAudio(const uchar *Data, int Length, uchar Id) {return Length;}
#endif

    virtual bool Poll(cPoller &Poller, int TimeoutMs = 0) {return true;}
    virtual bool Flush(int TimeoutMs = 0) {return true;}
    bool Start(void) {return true;}
};

#include <vdr/plugin.h>

static const char *VERSION        = "1.0.2";
static const char *DESCRIPTION    = "Output device that does nothing";

class cPluginDummydevice : public cPlugin {
private:
public:
  cPluginDummydevice(void) {}
  virtual ~cPluginDummydevice() {}
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void) { return NULL; }
  virtual bool ProcessArgs(int argc, char *argv[]) { return true; }
  virtual bool Initialize(void);
  virtual bool Start(void) { return true; }
  virtual void Housekeeping(void) {}
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void) { return NULL; }
  virtual cMenuSetupPage *SetupMenu(void) { return NULL; }
  virtual bool SetupParse(const char *Name, const char *Value) { return false; };
  };

bool cPluginDummydevice::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  new cDummyDevice();

  return true;
}

VDRPLUGINCREATOR(cPluginDummydevice); // Don't touch this!

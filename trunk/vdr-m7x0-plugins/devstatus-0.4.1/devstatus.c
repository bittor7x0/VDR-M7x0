/*
 * devstatus.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <sys/ioctl.h>
#include <linux/dvb/frontend.h>

#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/menu.h>
#include <vdr/i18n.h>


static const char *VERSION        = "0.4.1";
static const char *DESCRIPTION    = trNOOP("Status of dvb devices");
static const char *MAINMENUENTRY  = trNOOP("Device status");


#undef DAYDATETIMESTRING
#if VDRVERSNUM >= 10318
#define DAYDATETIME(x) *DayDateTime(x)
#else
#define DAYDATETIME(x) DayDateTime(x)
#endif


static int showRecordings = 1;
static int showStrength  = 1;
static int showChannels  = 1;
static int showChProvider  = 1;
static int update = 0;

//----------------Tools

const char* cardtypeAsString(int typ) {
   switch (typ) {
         case FE_QPSK:   return tr("satellite card");
         case FE_QAM:    return tr("cable card");    break;
         case FE_OFDM:   return tr("terrestrial card");    break;
         default:        return tr("unknown cardtype");
   };
}


int getFrequencyMHz(int value) {
   int ret = value;
   while (ret > 20000) ret /= 1000;
   return ret;
}

cChannel* getTunedChannel(cDevice *d)  {
   int channelNo;
   cChannel *channel = NULL;

   for (channelNo = 1; channelNo <= Channels.MaxNumber(); channelNo++) {
      if( (channel = Channels.GetByNumber(channelNo)) ) {
         if (d->IsTunedToTransponder(channel)) {
           return channel;
         }
      }
   }
   return NULL;
}

int getTunedFrequency (cDevice *device) {
    cChannel *tunedChannel = getTunedChannel(device);
    return tunedChannel ? tunedChannel->Frequency() : 0;
}


cChannel* nextTransponderChannel( cDevice *device, int direction) {
// search for the next transponder (direction=1) or the previous one (-1) 
   int channelNo;
   cChannel *channel = NULL;
   cChannel *resChannel = NULL;
   int oldQRG = getTunedFrequency(device);
            
   for (channelNo = 1; channelNo <= Channels.MaxNumber(); channelNo++) {
      if( (channel = Channels.GetByNumber(channelNo)) ) {
          if( device->ProvidesSource( channel->Source() ) ) { // same source (DVB-T, -S, ...)
             if( !ISTRANSPONDER(channel->Frequency(),oldQRG) ) {  //not the same transponder
               if( channel->Frequency()*direction > oldQRG*direction ) {  
                 if(  resChannel == NULL 
                   || (abs(channel->Frequency() - oldQRG) < abs(resChannel->Frequency() - oldQRG))
                 ) {  
                    resChannel = channel;
                 }                  
               }                  
             }
          }
      }
   }
   return resChannel;
}           


// --- cRecObj --------------------------------------------------------
class cRecObj : public cListObject {
public:
    char* name;
    const cDevice* device;
    cTimer* timer;
public:
    cRecObj(const char* Name, const cDevice* Device, cTimer* Timer) {
            name = strdup(Name);
            device = Device;
            timer = Timer;
    }
    ~cRecObj() {
            if (name) free(name);
    }
 };

// --- cDevStatusMonitor ---------------------------------------------------
class cDevStatusMonitor : public cStatus {
protected:
#if VDRVERSNUM >= 10338
    virtual void Recording(const cDevice *Device, const char *Name, const char *FileName, bool On);
#else
    virtual void Recording(const cDevice *Device, const char *Name);
#endif
    virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
};

cList<cRecObj> CurrentRecordings;

// --- cMenuRecItem ------------------------------------------------------
class cMenuRecItem : public cOsdItem {
  char *Name;
public: 
  int ChannelNr;
  int DeviceNr;
  cMenuRecItem(const char* name) { 
      Name = NULL;
      ChannelNr = 0;
      DeviceNr = -1;
      if (name) {  Name = strdup(name); SetText(Name, false); }
  }
  cMenuRecItem(const cRecObj* r) { 
      Name = NULL;
      if (r->name) {  
          Name = strdup(r->name); 
          char* itemText = NULL;
          asprintf(&itemText, "%s\t%s", DAYDATETIME(r->timer->StartTime()), Name);
          SetText(itemText, false); 
      }
  }
  char* RecName()  { return Name; }
  int GetChannelNr()  { return ChannelNr; }
  bool IsChannel() { return ChannelNr != 0; }
  bool HasDevice() { return DeviceNr >= 0; }
  cChannel* GetChannel()  { return Channels.GetByNumber(ChannelNr); }
  cDevice* GetDevice() { return cDevice::GetDevice(DeviceNr); };
};


class cMenuDevStatus : public cOsdMenu {
private:
public:
     void deviceinfoAsString(cDevice *d) {
        struct dvb_frontend_info m_FrontendInfo;
        int m_Frontend;
        // fe_status_t status;
        uint16_t signal = 0;
        uint16_t snr = 0;
        // uint32_t ber = 0;
        char* output = NULL;

        #define FRONTEND_DEVICE "/dev/dvb/adapter%d/frontend%d"
        cString dev = cString::sprintf(FRONTEND_DEVICE, d->CardIndex(), 0);
        m_Frontend = open(dev, O_RDONLY | O_NONBLOCK);
        if (m_Frontend < 0) {
           return;
        }
        int rcfe  = ioctl(m_Frontend, FE_GET_INFO, &m_FrontendInfo);
        // CHECK(ioctl(m_Frontend, FE_READ_STATUS, &status));
        int rcsig = ioctl(m_Frontend, FE_READ_SIGNAL_STRENGTH, &signal);
        int rcsnr = ioctl(m_Frontend, FE_READ_SNR, &snr);
        // CHECK(ioctl(m_Frontend, FE_READ_BER, &ber));
        close(m_Frontend);

        // 2. line type (name) - devpath
        if( rcfe == 0 ) {
           asprintf(&output, "%s (%s) - %s", 
                    cardtypeAsString(m_FrontendInfo.type), 
                    m_FrontendInfo.name,
                    *dev
           );
        } else {
           asprintf(&output, "no information available (rc=%d) - %s", rcfe, *dev);
        }
        cMenuRecItem* norec =  new cMenuRecItem(output);
        norec->SetSelectable(false);
        Add(norec);
        free(output);

        // 3. line strength
        if( showStrength ) {
            if( (rcsig == 0) && (rcsnr == 0) && ((signal > 0) || (snr>0)) ) {
                asprintf(&output, tr("frequency: %d MHz, signal: %d%%, s/n: %d%%"), 
                                     getFrequencyMHz (getTunedFrequency(d)) ,
                                     signal / 655, snr / 655);
            } else {
                asprintf(&output, tr("no signal information available (rc=%d)"), rcsig );
            }
                          
            norec =  new cMenuRecItem(output);
            norec->SetSelectable(false);
            Add(norec);
            free(output);
        }
     }

    cMenuDevStatus():cOsdMenu(tr("Device status"), 15) {
          Write();
    }

    void Write (void) { //Repaint screen
          int last = Current();
          Clear(); // clear OSD
          for (int i = 0; i < cDevice::NumDevices(); i++) {
              cDevice *d = cDevice::GetDevice(i);
              char* devName = NULL;
              char* devInfo = NULL;

              //  line  --- device <n> ....
              if (d->HasDecoder() || d->IsPrimaryDevice())
                  asprintf(&devInfo, " (%s%s%s)", 
                    d->HasDecoder()      ? tr("device with decoder") : "", 
                    (d->HasDecoder() && d->IsPrimaryDevice()) ? ", " : "", 
                    d->IsPrimaryDevice() ? tr("primary device")      : ""
                  );
             
              asprintf(&devName, "--- %s %d %s %s ---",
                        tr("Device"), 
                        i+1, 
                        (i == cDevice::ActualDevice()->CardIndex()) ? tr("-- Live"):"",
                        devInfo ? devInfo : ""
              ) ;
              cMenuRecItem* DeviceHeader =  new cMenuRecItem(devName);
              DeviceHeader->DeviceNr = i;
              DeviceHeader->SetSelectable(true);
              Add(DeviceHeader);
              free(devName);
              if (devInfo)
                  free(devInfo);

              // name, type 
              // signal
              deviceinfoAsString(d);
                                                                        
              // recordings
              if( showRecordings ) {
                  int Count = 0;
                  for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)) {
                      if (r && r->device == d) {
                          Add(new cMenuRecItem(r));
                          Count++;
                      }
                  }
                  if (Count == 0) {
                      cMenuRecItem* norec =  new cMenuRecItem(tr("currently no recordings"));
                      norec->SetSelectable(false);
                      Add(norec);
                  }
              }

              // channels        
              if (showChannels) {
                 cMenuRecItem* norec = NULL;
                 char* output = NULL;
                 int channelNo;
                 cChannel *channel = NULL;
       
                 for (channelNo = 1; channelNo <= Channels.MaxNumber(); channelNo++) {
                    if( (channel = Channels.GetByNumber(channelNo)) ) {
                       if (d->IsTunedToTransponder(channel)) {
                            bool currentLive = channelNo == d->CurrentChannel() 
                                               && (i == cDevice::ActualDevice()->CardIndex());
                                              /* TRANSLATORS: printf string for channel line */
                            asprintf(&output, tr("%5d  %s %s  %s  %s%s%s"), 
                                        channelNo, 
                                        (channel->Vpid() > 0) 
                                              /* TRANSLATORS: abbr. for tv-channels */
                                            ? (channel->Apid(0) > 0)  ? tr("t")  
                                              /* TRANSLATORS: abbr. for video only-channels */
                                                                      : tr("v") 
                                              /* TRANSLATORS: abbr. for radio-channels */
                                            : (channel->Apid(0) > 0)  ? tr("r") 
                                              /* TRANSLATORS: abbr. for no signal-channels */
                                                                      : tr("-"),

                                             /* TRANSLATORS: abbr. for live channel */
                                        currentLive ?               tr("+")  
                                             /* TRANSLATORS: abbr. for crypted channels */ 
                                            : (channel->Ca() > 4) ? tr("x")  
                                             /* TRANSLATORS: abbr. for FTA channels */
                                            :                       tr(" "),

                                        channel->Name(),

                                        showChProvider ? "(":"",
                                        showChProvider ? channel->Provider():"",
                                        showChProvider ? ")":""
                            );

                            norec = new cMenuRecItem(output);
                            norec->ChannelNr = channelNo;
                            norec->DeviceNr = i;
                            norec->SetSelectable(true);
                            Add(norec);
                            free(output);
                       }
                    }
                 }
              }

              // Leerzeile schreiben
              if (i < cDevice::NumDevices()) {
                  DeviceHeader =  new cMenuRecItem("");
                  DeviceHeader->SetSelectable(false);
                  Add(DeviceHeader);
              }
          }      

          cMenuRecItem* norec = NULL;

          norec = new cMenuRecItem("");
          norec->SetSelectable(true); // so we can scroll to the end of the list
          Add(norec);

          SetHelp( showRecordings ? tr("no recordings"):tr("recordings"), 
                   showStrength   ? tr("no strength")  :tr("strength"),
                   showChannels   ? tr("no channels")  :tr("channels"), 
                   tr("Refresh display")
                 );

          SetCurrent(Get(last));

          Display();       
    } //Write()

    eOSState Play(char* file) {
            // Play the selected recording 
            cRecording* recordingFound = NULL;
            for(cRecording* recording = Recordings.First(); recording; recording = Recordings.Next(recording))
                if (strstr(recording->Title(), file))
                    recordingFound = recording;
            if (!recordingFound)
                return osContinue;
            cReplayControl::SetRecording(recordingFound->FileName(), recordingFound->Title());
            return osReplay;
    }

    eOSState ProcessKey(eKeys Key) {
            cMenuRecItem *ri;
            eOSState state = osUnknown;
            switch(Key) {
              case kRed:
                    showRecordings = !showRecordings;
                    Write();
                    break;
              case kGreen:
                    showStrength = !showStrength;
                    Write();
                    break;
              case kYellow:
                    showChannels = !showChannels;
                    Write();
                    break;
              case kBlue:
                    Write();
                    break;
              case kChanUp:  case k7: 
              case kChanDn:  case k9:
                    // switch Transponder
                    ri = (cMenuRecItem*)Get(Current());
                    if( ri->Selectable() & ri->HasDevice() ) {

                        cChannel *newChannel = nextTransponderChannel(
                            ri->GetDevice(), ((Key==k9)||(Key==kChanUp)? 1:-1)
                        );
                        if( newChannel != NULL ) 
                           // Switch; no live view, so crypted
                           // transponder will be tuneable
                           ri->GetDevice()->SwitchChannel( newChannel, false );

                        Write(); //repaint; maybe 'Live' has changed
                        return osContinue;
                    }
                    break;

              case kOk:
                    ri = (cMenuRecItem*)Get(Current());
                    if (ri->Selectable()) {
                        if (ri->IsChannel()) {
                            Channels.SwitchTo(ri->GetChannelNr());
                            Write(); //repaint; maybe 'Live' has changed
                            return osContinue;
                        } else
                            return Play(ri->RecName());
                    }
                    break;

              case kNone:
                    if( update ) {
                       update=0;
                       Write();
                       break;
                    }
                    break;

              default:
                    state = cOsdMenu::ProcessKey(Key);
                    break;
            }
            return state;
        }
}; //cMenuDevStatus 



class cPluginDevstatus : public cPlugin {
private:
    cDevStatusMonitor* devStatusMonitor;
public:
  cPluginDevstatus(void);
  virtual ~cPluginDevstatus();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
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

// --- cMenuSetupDevstatus -------------------------------------------------------

class cMenuSetupDevstatus : public cMenuSetupPage {
private:
  int newShowRecordings;
  int newShowStrength;
  int newShowChannels;
  int newShowChProvider;
protected:
  virtual void Store(void);
public:
  cMenuSetupDevstatus(void);
};

cMenuSetupDevstatus::cMenuSetupDevstatus(void) {
  newShowRecordings = showRecordings;
  newShowStrength   = showStrength;
  newShowChannels   = showChannels;
  newShowChProvider = showChProvider;
  Add(new cMenuEditBoolItem(tr("Show recordings"       ), &newShowRecordings));
  Add(new cMenuEditBoolItem(tr("Show signals"          ), &newShowStrength));
  Add(new cMenuEditBoolItem(tr("Show channels"         ), &newShowChannels));
  Add(new cMenuEditBoolItem(tr("Show channel provider" ), &newShowChProvider));
}

void cMenuSetupDevstatus::Store(void) {
  SetupStore("ShowRecordings", showRecordings = newShowRecordings);
  SetupStore("ShowStrength",   showStrength = newShowStrength);
  SetupStore("ShowChannels",   showChannels = newShowChannels);
  SetupStore("ShowChProvider", showChProvider = newShowChProvider);
}

bool cPluginDevstatus::SetupParse(const char *Name, const char *Value) {
  // Parse your own setup parameters and store their values
  if      (!strcasecmp(Name, "ShowRecordings"))  showRecordings = atoi(Value) % 2;
  else if (!strcasecmp(Name, "ShowStrength"  ))  showStrength = atoi(Value) % 2;
  else if (!strcasecmp(Name, "ShowChannels"  ))  showChannels = atoi(Value) % 2;
  else if (!strcasecmp(Name, "ShowChProvider"))  showChProvider = atoi(Value) % 2;
  else return false;
  return true;
}

cMenuSetupPage *cPluginDevstatus::SetupMenu(void) {
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupDevstatus;
}

//---

cPluginDevstatus::cPluginDevstatus(void) {
    devStatusMonitor = NULL;
}

cPluginDevstatus::~cPluginDevstatus() {
    delete devStatusMonitor;
}

bool cPluginDevstatus::ProcessArgs(int argc, char *argv[]) {
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginDevstatus::Initialize(void) {
#if APIVERSNUM < 10507
   RegisterI18n(Phrases);
#endif
  return true;
}

bool cPluginDevstatus::Start(void) {
    devStatusMonitor = new cDevStatusMonitor;
    return true;
}

void cPluginDevstatus::Stop(void) {
  // Stop any background activities the plugin shall perform.
}

void cPluginDevstatus::Housekeeping(void) {
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginDevstatus::MainMenuAction(void) {
    return new cMenuDevStatus();
}



bool cPluginDevstatus::Service(const char *Id, void *Data) {
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginDevstatus::SVDRPHelpPages(void) {
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
    "DEVSTAT\n"
    "    Print all devices",
    "RECNUMBER\n"
    "    Print number of concurrent recordings for all devices.",
    NULL
    };
  return HelpPages;
}

cString cPluginDevstatus::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
  // Process SVDRP commands this plugin implements
  if(strcasecmp(Command, "DEVSTAT") == 0) {
    char* output = NULL;
    asprintf(&output, "%s:\n", tr("List of DVB devices"));
    for (int i = 0; i < cDevice::NumDevices(); i++) {
      cDevice *d = cDevice::GetDevice(i);
      char* devName = NULL;
      char* devInfo = NULL;
      if (d->HasDecoder() || d->IsPrimaryDevice())
        asprintf(&devInfo, " (%s%s%s)", d->HasDecoder() ? tr("device with decoder") : "", (d->HasDecoder() && d->IsPrimaryDevice()) ? ", " : "", d->IsPrimaryDevice() ? tr("primary device") : "");
      asprintf(&devName, "--- %s %d%s ---", tr("Device"), i+1, devInfo ? devInfo : "");
      asprintf(&output, "%s  %s:\n", output , devName); // add device output and there info's
      free(devName);
      if (devInfo)
        free(devInfo);
    int Count = 0;
    for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)){ // add recordings to the output
      if (r && r->device == d){
        char* Name = NULL;
        if (r->name){
          Name = strdup(r->name);
          char* itemText = NULL;
          asprintf(&itemText, "%s %s", DAYDATETIME(r->timer->StartTime()), Name);
          asprintf(&output, "%s    %s\n", output , itemText);
          free(itemText);
        }
        Count++;
      }
    }
    if (Count == 0)
      asprintf(&output, "%s    %s\n", output, tr("currently no recordings"));
    if (i < cDevice::NumDevices())
      asprintf(&output, "%s\n", output);
    }
    // we use the default reply code here
    // return cString::sprintf("%s", output);
    return output;
  }

  if(strcasecmp(Command, "RECNUMBER") == 0) {
    char* output = NULL;
    asprintf(&output, "%s:\n", tr("Number of concurrent recordings"));
    for (int i = 0; i < cDevice::NumDevices(); i++) {
      cDevice *d = cDevice::GetDevice(i);
      char* devName = NULL;
      asprintf(&devName, "%s %d", tr("Device"), i+1);
      asprintf(&output, "%s %s:", output , devName); // add device output and there info's
      free(devName);
    int Count = 0;
    for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)){ // add recordings to the output
      if (r && r->device == d) {
        Count++;
      }
    }
    asprintf(&output, "%s %i\n", output, Count);
    if (i < cDevice::NumDevices())
      asprintf(&output, "%s\n", output);
    }
    // we use the default reply code here
    // return cString::sprintf("%s", output);
    return output;
  }

  return NULL;
}


/*--------*/



#if VDRVERSNUM >= 10338 
void cDevStatusMonitor::Recording(const cDevice *Device, const char *Name, const char *FileName, bool On) {
#else
void cDevStatusMonitor::Recording(const cDevice *Device, const char *Name) {
#endif
    if (Name) {
        // insert new timers currently recording in TimersRecording
        for (cTimer *ti = Timers.First(); ti; ti = Timers.Next(ti))
            if (ti->Recording()) {
                // check if this is a new entry
                bool bFound = false;
                for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r))
                    if (r->timer == ti)
                        bFound = true;

                if (bFound) continue; // already handled
                CurrentRecordings.Add(new cRecObj(Name, Device, ti));
                update=1;
                return;
            }
    }

    if (!Name) {
        // remove timers that finished recording from TimersRecording
        for (cRecObj *r = CurrentRecordings.First(); r; r = CurrentRecordings.Next(r)) {
            if (!r->timer->Recording()) {
                CurrentRecordings.Del(r);
                update = 1;
                break;
            }
        }
    }
}

void cDevStatusMonitor::ChannelSwitch(const cDevice *Device, int ChannelNumber) {
      if (!ChannelNumber) return;
      update=1;
}


VDRPLUGINCREATOR(cPluginDevstatus); // Don't touch this!

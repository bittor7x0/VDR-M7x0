/*
 * eitscan.c: EIT scanner
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "eitscan.h"
#include <stdlib.h>
#include "channels.h"
#include "dvbdevice.h"
#include "skins.h"
#include "transfer.h"

// --- cScanData -------------------------------------------------------------

class cScanData : public cListObject {
private:
  cChannel channel;
public:
  cScanData(const cChannel *Channel);
  virtual int Compare(const cListObject &ListObject) const;
  int Source(void) const { return channel.Source(); }
  int Transponder(void) const { return channel.Transponder(); }
  const cChannel *GetChannel(void) const { return &channel; }
  };

cScanData::cScanData(const cChannel *Channel)
{
  channel = *Channel;
}

int cScanData::Compare(const cListObject &ListObject) const
{
  const cScanData *sd = (const cScanData *)&ListObject;
  int r = Source() - sd->Source();
  if (r == 0)
     r = Transponder() - sd->Transponder();
  return r;
}

// --- cScanList -------------------------------------------------------------

class cScanList : public cList<cScanData> {
public:
  void AddTransponders(cList<cChannel> *Channels);
  void AddTransponder(const cChannel *Channel);
  };

void cScanList::AddTransponders(cList<cChannel> *Channels)
{
  for (cChannel *ch = Channels->First(); ch; ch = Channels->Next(ch))
      AddTransponder(ch);
  Sort();
}

void cScanList::AddTransponder(const cChannel *Channel)
{
  if (Channel->Source() && Channel->Transponder()) {
     for (cScanData *sd = First(); sd; sd = Next(sd)) {
         if (sd->Source() == Channel->Source() && ISTRANSPONDER(sd->Transponder(), Channel->Transponder()))
            return;
         }
     Add(new cScanData(Channel));
     }
}

// --- cTransponderList ------------------------------------------------------

class cTransponderList : public cList<cChannel> {
public:
  void AddTransponder(cChannel *Channel);
  };

void cTransponderList::AddTransponder(cChannel *Channel)
{
  for (cChannel *ch = First(); ch; ch = Next(ch)) {
      if (ch->Source() == Channel->Source() && ch->Transponder() == Channel->Transponder()) {
         delete Channel;
         return;
         }
      }
  Add(Channel);
}

// --- cEITScanner -----------------------------------------------------------

cEITScanner EITScanner;

cEITScanner::cEITScanner(void)
{
  lastScan = lastActivity = time(NULL);
  currentDevice = NULL;
  currentChannel = 0;
  scanList = NULL;
  transponderList = NULL;
}

cEITScanner::~cEITScanner()
{
  delete scanList;
  delete transponderList;
}

void cEITScanner::AddTransponder(cChannel *Channel)
{
  if (!transponderList)
     transponderList = new cTransponderList;
  transponderList->AddTransponder(Channel);
}

void cEITScanner::ForceScan(void)
{
  lastActivity = 0;
}

void cEITScanner::Activity(void)
{
  if (currentChannel) {
     Channels.SwitchTo(currentChannel);
     currentChannel = 0;
     }
  lastActivity = time(NULL);
}

void cEITScanner::Process(void)
{
  if ((Setup.EPGScanTimeout || !lastActivity) && Channels.MaxNumber() > 1) { // !lastActivity means a scan was forced
     time_t now = time(NULL);
     if (now - lastScan > ScanTimeout && now - lastActivity > ActivityTimeout) {
        if (Channels.Lock(false, 10)) {
           if (!scanList) {
              scanList = new cScanList;
              scanList->AddTransponders(&Channels);
              if (transponderList) {
                 scanList->AddTransponders(transponderList);
                 delete transponderList;
                 transponderList = NULL;
                 }
              }
           bool AnyDeviceSwitched = false;
           for (int i = 0; i < cDevice::NumDevices(); i++) {
               cDevice *Device = cDevice::GetDevice(i);
               if (Device) {
                  for (cScanData *ScanData = scanList->First(); ScanData; ScanData = scanList->Next(ScanData)) {
                      const cChannel *Channel = ScanData->GetChannel();
                      if (Channel) {
                         if (!Channel->Ca() || Channel->Ca() == Device->DeviceNumber() + 1 || Channel->Ca() >= CA_ENCRYPTED_MIN) {
                            if (Device->ProvidesTransponder(Channel)) {
                               if (!Device->Receiving()) {
                                  bool MaySwitchTransponder = Device->MaySwitchTransponder();
                                  if (MaySwitchTransponder || Device->ProvidesTransponderExclusively(Channel) && now - lastActivity > Setup.EPGScanTimeout * 3600) {
                                     if (!MaySwitchTransponder) {
                                        if (Device == cDevice::ActualDevice() && !currentChannel) {
                                           cDevice::PrimaryDevice()->StopReplay(); // stop transfer mode
                                           currentChannel = Device->CurrentChannel();
                                           Skins.Message(mtInfo, tr("Starting EPG scan"));
                                           }
                                        }
                                     currentDevice = Device;//XXX see also dvbdevice.c!!!
                                     //dsyslog("EIT scan: device %d  source  %-8s tp %5d", Device->DeviceNumber() + 1, *cSource::ToString(Channel->Source()), Channel->Transponder());
                                     Device->SwitchChannel(Channel, false);
                                     currentDevice = NULL;
                                     scanList->Del(ScanData);
                                     AnyDeviceSwitched = true;
                                     break;
                                     }
                                  }
                               }
                            }
                         }
                      }
                  }
               }
           if (!scanList->Count() || !AnyDeviceSwitched) {
              delete scanList;
              scanList = NULL;
              if (lastActivity == 0) // this was a triggered scan
                 Activity();
              }
           Channels.Unlock();
           }
        lastScan = time(NULL);
        }
     }
}

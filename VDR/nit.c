/*
 * nit.c: NIT section filter
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "nit.h"
//M7X0 BEGIN AK
#include "m7x0_dvb/frontend.h"
//M7X0 END AK
#include "channels.h"
#include "eitscan.h"
#include "libsi/section.h"
#include "libsi/descriptor.h"
#include "tools.h"

cNitFilter::cNitFilter(void)
{
  numNits = 0;
  networkId = 0;
  Set(0x10, 0x40);  // NIT
}

void cNitFilter::SetStatus(bool On)
{
  cFilter::SetStatus(On);
  numNits = 0;
  networkId = 0;
  sectionSyncer.Reset();
}

void cNitFilter::Process(u_short Pid, u_char Tid, const u_char *Data, int Length)
{
  SI::NIT nit(Data, false);
  if (!nit.CheckCRCAndParse())
     return;
  // Some broadcasters send more than one NIT, with no apparent way of telling which
  // one is the right one to use. This is an attempt to find the NIT that contains
  // the transponder it was transmitted on and use only that one:
  int ThisNIT = -1;
  if (!networkId) {
     for (int i = 0; i < numNits; i++) {
         if (nits[i].networkId == nit.getNetworkId()) {
            if (nit.getSectionNumber() == 0) {
               // all NITs have passed by
               for (int j = 0; j < numNits; j++) {
                   if (nits[j].hasTransponder) {
                      networkId = nits[j].networkId;
                      //printf("taking NIT with network ID %d\n", networkId);
                      //XXX what if more than one NIT contains this transponder???
                      break;
                      }
                   }
               if (!networkId) {
                  //printf("none of the NITs contains transponder %d\n", Transponder());
                  return;
                  }
               }
            else {
               ThisNIT = i;
               break;
               }
            }
         }
     if (!networkId && ThisNIT < 0 && numNits < MAXNITS) {
        if (nit.getSectionNumber() == 0) {
           *nits[numNits].name = 0;
           SI::Descriptor *d;
           for (SI::Loop::Iterator it; (d = nit.commonDescriptors.getNext(it)); ) {
               switch (d->getDescriptorTag()) {
                 case SI::NetworkNameDescriptorTag: {
                      SI::NetworkNameDescriptor *nnd = (SI::NetworkNameDescriptor *)d;
                      nnd->name.getText(nits[numNits].name, MAXNETWORKNAME);
                      }
                      break;
                 default: ;
                 }
               delete d;
               }
           nits[numNits].networkId = nit.getNetworkId();
           nits[numNits].hasTransponder = false;
           //printf("NIT[%d] %5d '%s'\n", numNits, nits[numNits].networkId, nits[numNits].name);
           ThisNIT = numNits;
           numNits++;
           }
        }
     }
  else if (networkId != nit.getNetworkId())
     return; // ignore all other NITs
  else if (!sectionSyncer.Sync(nit.getVersionNumber(), nit.getSectionNumber(), nit.getLastSectionNumber()))
     return;
  if (!Channels.Lock(true, 10))
     return;
  SI::NIT::TransportStream ts;
  for (SI::Loop::Iterator it; nit.transportStreamLoop.getNext(ts, it); ) {
      SI::Descriptor *d;

      SI::Loop::Iterator it2;
      SI::FrequencyListDescriptor *fld = (SI::FrequencyListDescriptor *)ts.transportStreamDescriptors.getNext(it2, SI::FrequencyListDescriptorTag);
      int NumFrequencies = fld ? fld->frequencies.getCount() + 1 : 1;
      int Frequencies[NumFrequencies];
      if (fld) {
         int ct = fld->getCodingType();
         if (ct > 0) {
            int n = 1;
            for (SI::Loop::Iterator it3; fld->frequencies.hasNext(it3); ) {
                int f = fld->frequencies.getNext(it3);
                switch (ct) {
                  case 1: f = BCD2INT(f) / 100; break;
                  case 2: f = BCD2INT(f) / 10; break;
                  case 3: f = f * 10;  break;
                  }
                Frequencies[n++] = f;
                }
            }
         else
            NumFrequencies = 1;
         }
      delete fld;

      for (SI::Loop::Iterator it2; (d = ts.transportStreamDescriptors.getNext(it2)); ) {
          switch (d->getDescriptorTag()) {
            case SI::SatelliteDeliverySystemDescriptorTag: {
                 SI::SatelliteDeliverySystemDescriptor *sd = (SI::SatelliteDeliverySystemDescriptor *)d;
                 int Source = cSource::FromData(cSource::stSat, BCD2INT(sd->getOrbitalPosition()), sd->getWestEastFlag());
                 int Frequency = Frequencies[0] = BCD2INT(sd->getFrequency()) / 100;
                 static char Polarizations[] = { 'h', 'v', 'l', 'r' };
                 char Polarization = Polarizations[sd->getPolarization()];
                 static int CodeRates[] = { FEC_NONE, FEC_1_2, FEC_2_3, FEC_3_4, FEC_5_6, FEC_7_8, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_NONE };
                 int CodeRate = CodeRates[sd->getFecInner()];
                 int SymbolRate = BCD2INT(sd->getSymbolRate()) / 10;
                 if (ThisNIT >= 0) {
                    for (int n = 0; n < NumFrequencies; n++) {
                        if (ISTRANSPONDER(cChannel::Transponder(Frequencies[n], Polarization), Transponder())) {
                           nits[ThisNIT].hasTransponder = true;
                           //printf("has transponder %d\n", Transponder());
                           break;
                           }
                        }
                    break;
                    }
                 bool found = false;
                 for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
                     if (!Channel->GroupSep() && Channel->Source() == Source && Channel->Nid() == ts.getOriginalNetworkId() && Channel->Tid() == ts.getTransportStreamId()) {
                        if (Setup.UpdateChannels >= 5) {
                           if (!ISTRANSPONDER(cChannel::Transponder(Frequency, Polarization), Channel->Transponder())) {
                              for (int n = 0; n < NumFrequencies; n++) {
                                  if (ISTRANSPONDER(cChannel::Transponder(Frequencies[n], Polarization), Channel->Transponder())) {
                                     Frequency = Frequencies[n];
                                     break;
                                     }
                                  }
                              }
                           if (ISTRANSPONDER(cChannel::Transponder(Frequency, Polarization), Transponder())) // only modify channels if we're actually receiving this transponder
                              Channel->SetSatTransponderData(Source, Frequency, Polarization, SymbolRate, CodeRate);
                           }
                        found = true;
                        }
                     }
                 if (!found && Setup.UpdateChannels >= 5) {
                    for (int n = 0; n < NumFrequencies; n++) {
                        cChannel *Channel = new cChannel;
                        Channel->SetId(ts.getOriginalNetworkId(), ts.getTransportStreamId(), 0, 0);
                        if (Channel->SetSatTransponderData(Source, Frequencies[n], Polarization, SymbolRate, CodeRate))
                           EITScanner.AddTransponder(Channel);
                        else
                           delete Channel;
                        }
                    }
                 }
                 break;
            case SI::CableDeliverySystemDescriptorTag: {
                 SI::CableDeliverySystemDescriptor *sd = (SI::CableDeliverySystemDescriptor *)d;
                 int Source = cSource::FromData(cSource::stCable);
                 int Frequency = Frequencies[0] = BCD2INT(sd->getFrequency()) / 10;
                 //XXX FEC_outer???
                 static int CodeRates[] = { FEC_NONE, FEC_1_2, FEC_2_3, FEC_3_4, FEC_5_6, FEC_7_8, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_AUTO, FEC_NONE };
                 int CodeRate = CodeRates[sd->getFecInner()];
                 static int Modulations[] = { QPSK, QAM_16, QAM_32, QAM_64, QAM_128, QAM_256, QAM_AUTO };
                 int Modulation = Modulations[min(sd->getModulation(), 6)];
                 int SymbolRate = BCD2INT(sd->getSymbolRate()) / 10;
                 if (ThisNIT >= 0) {
                    for (int n = 0; n < NumFrequencies; n++) {
                        if (ISTRANSPONDER(Frequencies[n] / 1000, Transponder())) {
                           nits[ThisNIT].hasTransponder = true;
                           //printf("has transponder %d\n", Transponder());
                           break;
                           }
                        }
                    break;
                    }
                 bool found = false;
                 for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
                     if (!Channel->GroupSep() && Channel->Source() == Source && Channel->Nid() == ts.getOriginalNetworkId() && Channel->Tid() == ts.getTransportStreamId()) {
                        if (Setup.UpdateChannels >= 5) {
                           if (!ISTRANSPONDER(Frequency / 1000, Channel->Transponder())) {
                              for (int n = 0; n < NumFrequencies; n++) {
                                  if (ISTRANSPONDER(Frequencies[n] / 1000, Channel->Transponder())) {
                                     Frequency = Frequencies[n];
                                     break;
                                     }
                                  }
                              }
                           if (ISTRANSPONDER(Frequency / 1000, Transponder())) // only modify channels if we're actually receiving this transponder
                              Channel->SetCableTransponderData(Source, Frequency, Modulation, SymbolRate, CodeRate);
                           }
                        found = true;
                        }
                     }
                 if (!found && Setup.UpdateChannels >= 5) {
                    for (int n = 0; n < NumFrequencies; n++) {
                        cChannel *Channel = new cChannel;
                        Channel->SetId(ts.getOriginalNetworkId(), ts.getTransportStreamId(), 0, 0);
                        if (Channel->SetCableTransponderData(Source, Frequencies[n], Modulation, SymbolRate, CodeRate))
                           EITScanner.AddTransponder(Channel);
                        else
                           delete Channel;
                        }
                    }
                 }
                 break;
            case SI::TerrestrialDeliverySystemDescriptorTag: {
                 SI::TerrestrialDeliverySystemDescriptor *sd = (SI::TerrestrialDeliverySystemDescriptor *)d;
                 int Source = cSource::FromData(cSource::stTerr);
                 int Frequency = Frequencies[0] = sd->getFrequency() * 10;
                 static int Bandwidths[] = { BANDWIDTH_8_MHZ, BANDWIDTH_7_MHZ, BANDWIDTH_6_MHZ, BANDWIDTH_AUTO, BANDWIDTH_AUTO, BANDWIDTH_AUTO, BANDWIDTH_AUTO, BANDWIDTH_AUTO };
                 int Bandwidth = Bandwidths[sd->getBandwidth()];
                 static int Constellations[] = { QPSK, QAM_16, QAM_64, QAM_AUTO };
                 int Constellation = Constellations[sd->getConstellation()];
                 static int Hierarchies[] = { HIERARCHY_NONE, HIERARCHY_1, HIERARCHY_2, HIERARCHY_4, HIERARCHY_AUTO, HIERARCHY_AUTO, HIERARCHY_AUTO, HIERARCHY_AUTO };
                 int Hierarchy = Hierarchies[sd->getHierarchy()];
                 static int CodeRates[] = { FEC_1_2, FEC_2_3, FEC_3_4, FEC_5_6, FEC_7_8, FEC_AUTO, FEC_AUTO, FEC_AUTO };
                 int CodeRateHP = CodeRates[sd->getCodeRateHP()];
                 int CodeRateLP = CodeRates[sd->getCodeRateLP()];
                 static int GuardIntervals[] = { GUARD_INTERVAL_1_32, GUARD_INTERVAL_1_16, GUARD_INTERVAL_1_8, GUARD_INTERVAL_1_4 };
                 int GuardInterval = GuardIntervals[sd->getGuardInterval()];
                 static int TransmissionModes[] = { TRANSMISSION_MODE_2K, TRANSMISSION_MODE_8K, TRANSMISSION_MODE_AUTO, TRANSMISSION_MODE_AUTO };
                 int TransmissionMode = TransmissionModes[sd->getTransmissionMode()];
                 if (ThisNIT >= 0) {
                    for (int n = 0; n < NumFrequencies; n++) {
                        if (ISTRANSPONDER(Frequencies[n] / 1000000, Transponder())) {
                           nits[ThisNIT].hasTransponder = true;
                           //printf("has transponder %d\n", Transponder());
                           break;
                           }
                        }
                    break;
                    }
                 bool found = false;
                 for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
                     if (!Channel->GroupSep() && Channel->Source() == Source && Channel->Nid() == ts.getOriginalNetworkId() && Channel->Tid() == ts.getTransportStreamId()) {
                        if (Setup.UpdateChannels >= 5) {
                           if (!ISTRANSPONDER(Frequency / 1000000, Channel->Transponder())) {
                              for (int n = 0; n < NumFrequencies; n++) {
                                  if (ISTRANSPONDER(Frequencies[n] / 1000000, Channel->Transponder())) {
                                     Frequency = Frequencies[n];
                                     break;
                                     }
                                  }
                              }
                           if (ISTRANSPONDER(Frequency / 1000000, Transponder())) // only modify channels if we're actually receiving this transponder
                              Channel->SetTerrTransponderData(Source, Frequency, Bandwidth, Constellation, Hierarchy, CodeRateHP, CodeRateLP, GuardInterval, TransmissionMode);
                           }
                        found = true;
                        }
                     }
                 if (!found && Setup.UpdateChannels >= 5) {
                    for (int n = 0; n < NumFrequencies; n++) {
                        cChannel *Channel = new cChannel;
                        Channel->SetId(ts.getOriginalNetworkId(), ts.getTransportStreamId(), 0, 0);
                        if (Channel->SetTerrTransponderData(Source, Frequencies[n], Bandwidth, Constellation, Hierarchy, CodeRateHP, CodeRateLP, GuardInterval, TransmissionMode))
                           EITScanner.AddTransponder(Channel);
                        else
                           delete Channel;
                        }
                    }
                 }
                 break;
            default: ;
            }
          delete d;
          }
      }
  Channels.Unlock();
}

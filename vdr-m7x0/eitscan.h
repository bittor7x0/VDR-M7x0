/*
 * eitscan.h: EIT scanner
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __EITSCAN_H
#define __EITSCAN_H

#include <time.h>
#include "channels.h"
#include "config.h"
#include "device.h"

class cScanList;
class cTransponderList;

class cEITScanner {
private:
//M7X0 BEGIN AK
// Increase timeouts.
  enum { ActivityTimeout = 60,
         ScanTimeout = 30
       };
//M7X0 END AK
  time_t lastScan, lastActivity;
  cDevice *currentDevice;
  int currentChannel;
  cScanList *scanList;
  cTransponderList *transponderList;
public:
  cEITScanner(void);
  ~cEITScanner();
  bool Active(void) { return currentChannel || lastActivity == 0; }
  bool UsesDevice(const cDevice *Device) { return currentDevice == Device; }
  void AddTransponder(cChannel *Channel);
  void ForceScan(void);
  void Activity(void);
  void Process(void);
  };

extern cEITScanner EITScanner;

#endif //__EITSCAN_H

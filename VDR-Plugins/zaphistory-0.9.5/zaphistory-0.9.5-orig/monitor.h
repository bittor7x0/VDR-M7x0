/*
 * monitor.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Provides a cStatus derived monitor to be informed about VDR activities like
 * channel switch, replay start/stop
 */

#ifndef __MONITOR_H
#define __MONITOR_H

#include <vdr/status.h>

class cZapHistoryMonitor : public cStatus {
    public:
	void ChannelSwitch( const cDevice *Device, int ChannelNumber);
	
	void Replaying( const cControl *Control, const char *Name, const char *FileName, bool On);
};

#endif

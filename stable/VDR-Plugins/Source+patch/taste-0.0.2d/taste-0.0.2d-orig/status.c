/*
 * $Id: status.c,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#include <vdr/channels.h>

#include "status.h"
#include "control.h"
#include "event.h"

cStatusTaste::cStatusTaste(void):
    cStatus(),
    mLastChannel(0)
{
}

void cStatusTaste::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{
  printf("cStatusTaste::ChannelSwitch(%p, %d)\n", Device, ChannelNumber);

	if (Device == cDevice::ActualDevice()) {
		if (mLastChannel == 0) {
			if (ChannelNumber != 0 && ChannelNumber == cDevice::CurrentChannel())
				mLastChannel = ChannelNumber;
			return;
		}

		if (ChannelNumber == 0) {
			mLastChannel = cDevice::CurrentChannel();
			return;
		}

		const cChannel *channel = Channels.GetByNumber(ChannelNumber);
		if (channel != NULL && !channel->GroupSep()) {
			cSchedulesLock schedLock;
			const cSchedules *scheds = cSchedules::Schedules(schedLock);
			if (scheds == NULL)
				return;

			const cSchedule *sched = scheds->GetSchedule(channel->GetChannelID());
			if (sched == NULL)
				return;

			const cEvent *present = sched->GetPresentEvent(true);
			const cEvent *follow  = sched->GetFollowingEvent(true);
			if (present == NULL)
				return;
			
			if (!cControlTaste::IsRequested() && !EventsTaste.Acceptable(present->Title())) {
				isyslog("channel %d is not acceptable at present", ChannelNumber);
				cControl::Launch(new cControlTaste(mLastChannel, channel, present, follow));
			}
		}
	}
}
  

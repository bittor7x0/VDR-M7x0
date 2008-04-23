/*
 * $Id: control.c,v 1.2 2005/01/02 18:43:34 lordjaxom Exp $
 */

#include "control.h"
#include "config.h"
#include "common.h"

#include <vdr/remote.h>

#define TASTETIMEOUT (SetupTaste.MessageTimeout * 1000) // ms

bool cControlTaste::mRequested = false;

cControlTaste::cControlTaste(int LastChannel, const cChannel *Channel, const cEvent *Present, const cEvent *Following):
    cControl(new cPlayer),
    mLastChannel(LastChannel),
		mChannel(Channel),
		mPresent(Present),
		mFollowing(Following),
		mStart(0),
		mSwitch(true),
		mOsd(NULL)
{
	needsFastResponse = true;
	cRemote::Put(kOk, true); // Hide OSD
}

cControlTaste::~cControlTaste()
{
	if (mOsd != NULL)
		delete mOsd;

	if (mSwitch) {
		// possibly first or last available channel, fall back to old channel
		if (!cDevice::SwitchChannel(mChannel->Number() - mLastChannel))
			Channels.SwitchTo(mLastChannel);
	}
}

void cControlTaste::Show(void)
{
	if (mOsd == NULL)
		mOsd = Skins.Current()->DisplayChannel(true);

	mOsd->SetChannel(mChannel, 0);
	mOsd->SetEvents(mPresent, mFollowing);
	mOsd->SetMessage(mtError, tr("Channel not acceptable!"));
	mOsd->Flush();

	mStart = time_ms();
}

eOSState cControlTaste::ProcessKey(eKeys Key)
{
  printf("cControlTaste::ProcessKey(%d) this = %p\n", Key, this);

  switch (Key) {
	case kNone:
		if (mStart == 0)
			Show();
		else if (time_ms() - mStart > TASTETIMEOUT) {
			mSwitch = true;
			return osEnd;
		}
	  return osContinue;

	case kOk:
		printf("kok\n");
		mRequested = true;
		printf("mswitch = false, return osEnd\n");
		mSwitch = false;
		cDevice::PrimaryDevice()->SwitchChannel(mChannel, true);
		return osEnd;

  default:
    break;
  }
  return osContinue;
}


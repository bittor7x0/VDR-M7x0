#include <vdr/plugin.h>
#include "channelswitcher.h"
#include "channelmenu.h"

cSwitcher::cSwitcher(void)
{
	ListNbr=0;
}

cSwitcher::~cSwitcher(void)
{
	if (Setup.UpdateChannels==0)
	{
	Setup.UpdateChannels=SaveSetup;
	Setup.Save();
	}
}


cOsdObject *cSwitcher::MainMenuAction(void)
{
	return new cSwitchMenu(*this);
}

bool cSwitcher::Initialize(void)
{
	SaveSetup=Setup.UpdateChannels;
	return true;
}


VDRPLUGINCREATOR(cSwitcher); // Don't touch this!

#include "chanorg.h"
#include "chan-menu.h"

cPluginChanOrg ChanOrg;

void cPluginChanOrg::Debug(void)
{
	cChannel *chan=ChanOrg.OrgChans.First();
		fprintf(stderr,"Chaine Index %i %s\n",chan->Index(),chan->Name());
	while ((chan=ChanOrg.OrgChans.Next(chan)))
	{
		fprintf(stderr,"Chaine Index %i %s\n",chan->Index(),chan->Name());
	}
}

cPluginChanOrg::cPluginChanOrg(void)
{
   ReDraw=false;
}

cPluginChanOrg::~cPluginChanOrg(void)
{
}

bool cPluginChanOrg::Initialize(void)
{
	RegisterI18n(Phrases);
	return true;
}

const char *cPluginChanOrg::CommandLineHelp(void)
{
	return NULL;
}

bool cPluginChanOrg::ProcessArgs(int argc, char *argv[])
{
	return true;
}

bool cPluginChanOrg::Start(void)
{
	return true;
}

cOsdObject *cPluginChanOrg::MainMenuAction(void)
{
		return new cChanOrgMenu;
}

VDRPLUGINCREATOR(cPluginChanOrg);

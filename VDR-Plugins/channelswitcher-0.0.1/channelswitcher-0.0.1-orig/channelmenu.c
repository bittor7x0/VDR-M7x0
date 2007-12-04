#include "channelmenu.h"
#include "channelswitcher.h"
#include <dirent.h>


cSwitchMenu::cSwitchMenu(cSwitcher &SwitcherPlug) : cOsdMenu(tr("Channels.conf SwitcherPlug"),20)
{
	switcher=&SwitcherPlug;
	cOsdItem *item= new cOsdItem("Defaut",osUser3);
	Add(item);
	switcher->List[SwitcherPlug.ListNbr]="";
	switcher->ListNbr+=1;
	char *dir;
	asprintf(&dir,"%s/../",cPlugin::ConfigDirectory());
	DIR *d = opendir(dir);
	if (d)
	{
		struct dirent *e;
		while ((e = readdir(d)) != NULL)
		{
			if (!strncmp("channels.conf.",e->d_name,14))
			{
				char *name;
				asprintf(&name,"%s",e->d_name);
				strtok(name,".");
				name=strtok(NULL,".");
				name=strtok(NULL,".");
				item = new cOsdItem(name,osUser3);
				Add(item);
				asprintf(&name,".%s",name);
				switcher->List[SwitcherPlug.ListNbr]=name;	
				switcher->ListNbr+=1;
			}
		}
	closedir(d);
	}
}

cSwitchMenu::~cSwitchMenu(void)
{
}

eOSState cSwitchMenu::ProcessKey(eKeys Key)
{
	eOSState state=cOsdMenu::ProcessKey(Key);
	char *ToLoad;
	switch(state)
	{
		case osUser3:
			cTimer *time;
			time = Timers.First();
			if ( time )
			{
				Skins.Message(mtError,"Programmation en cours...");
				sleep(Setup.OSDMessageTime);
				Skins.Message(mtStatus,"");
				return osBack;
			}
			Setup.UpdateChannels=0;
			asprintf(&ToLoad,"%s/../channels.conf%s",cPlugin::ConfigDirectory(),switcher->List[Current()]);
			Channels.Load(ToLoad);
			asprintf(&ToLoad,"channels.conf%s actif",switcher->List[Current()]);
			Skins.Message(mtStatus,ToLoad);
			sleep(Setup.OSDMessageTime);
			Skins.Message(mtStatus,"");
			return osBack;
			break;
		default :
			break;
	}
	return state;
}

void cSwitchMenu::Draw(void)
{
}

void cSwitchMenu::Switch(void)
{
}

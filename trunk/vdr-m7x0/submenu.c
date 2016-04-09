#include "submenu.h"
#include <string.h>
#include <stdio.h>
#include "plugin.h"

cSubMenuItemInfo::cSubMenuItemInfo()
	{
	    menuLevel = 0;
	    kindOfItem = 0;
	    name[0] = 0;
	}

cSubMenuItemInfo::~cSubMenuItemInfo()
{
}

void cSubMenuItemInfo::setname(const char *n)
{
	int i,l;
	l = strlen(n);

	for(i=0;i<SIZEMAXNAMESUBMENUITEMINFO;i++)
	name[i] = 0;

	if( l >= SIZEMAXNAMESUBMENUITEMINFO )
	l = SIZEMAXNAMESUBMENUITEMINFO;

	for(i=0;i<l;i++)
	{
		name[i] = n[i];
	}
    name[i]=0;
    name[l]=0;
}

void cSubMenu::LoadInfoSubMenu(void)
{
	nbSubMenuItem = 0;
	cString fichier = cString::sprintf("%s/MainMenu.conf",cPlugin::ConfigDirectory());
	FILE *f = fopen(fichier,"rt");
	if ( f != NULL)
	{
		char lig[80];
		while (fgets(lig,80,f) )
		{
			ParseOneLigOfFile( lig );
		}
		fclose(f);
	}
	else
	{
		/* -- create example file -- */
		cPlugin *p;

		FILE *fo = fopen(fichier,"wt");
		if (fo!=NULL)
		{
			fprintf(fo,"0::Schedule\n");
			fprintf(fo,"0::Recordings\n");
			fprintf(fo,"0::Timers\n");
			if (cPluginManager::GetPlugin("epgsearchonly"))
				fprintf(fo,"0::epgsearchonly\n");
			if (cPluginManager::GetPlugin("conflictcheckonly"))
				fprintf(fo,"0::conflictcheckonly\n");

			// Plugins Item:
			int j=0;
			int current=0;
			do
			{
				p = cPluginManager::GetPlugin(j);
				if ( p != NULL && strcmp(p->Name(), "aide")
                               && strcmp(p->Name(), "channellists")
                               && strcmp(p->Name(), "channelscan")
                               && strcmp(p->Name(), "conflictcheckonly")
                               && strcmp(p->Name(), "epgsearch")
                               && strcmp(p->Name(), "epgsearchonly")
                               && strcmp(p->Name(), "setup")
                               && strcmp(p->Name(), "skinenigmang")
                               && strcmp(p->Name(), "skinreel")
                               && strcmp(p->Name(), "skinsoppalusikka") )
				{
					if (current == 0)
						fprintf(fo,"0:%d:Plugins\n", ++current);
					fprintf(fo, "%d::%s\n", current, p->Name());
				}
				j++;
			}
			while (p !=NULL);

			fprintf(fo,"0:%d:Configuraci%cn\n", ++current, 243);
			int setup = current++;
			fprintf(fo,"%d::Setup\n", setup);
			if (cPluginManager::GetPlugin("setup"))
				fprintf(fo, "%d::setup\n", setup);
			fprintf(fo, "%d:%d:%s\n", setup, current, tr("Channels"));
			if (cPluginManager::GetPlugin("channelscan"))
				fprintf(fo,"%d::channelscan\n", current);
			if (cPluginManager::GetPlugin("channellists"))
				fprintf(fo,"%d::channellists\n", current);
			fprintf(fo,"%d::Channels\n", current);
			fprintf(fo,"0::Commands\n");
			if (cPluginManager::GetPlugin("aide"))
				fprintf(fo,"0::aide\n");

			fclose(fo);
			LoadInfoSubMenu();
			Arbo.Populate(this);
		}
		/* -- Create example file -- */
	}
}

void cSubMenu::ParseOneLigOfFile(const char *lig)
{
	int level=-1;
	int kind=-1;
	int i,j,l;
	char txt[SIZEMAXNAMESUBMENUITEMINFO];
	char name[SIZEMAXNAMESUBMENUITEMINFO];
	int status = 0;
	int namefound = 0;

	l = strlen(lig);

	name[0] = 0;

	i=0; j=0;

	while(true)
	{
		switch(status)
		{
			case 0: //looking for level
				txt[j++] = lig[i];
				if ( lig[i] == ':' )
				{
					txt[j] = 0;
					status++;
					level = atoi(txt);
					j = 0;
				}
				break;
			case 1:
				txt[j++] = lig[i];
				if (lig[i] == ':')
				{
					txt[j] = 0;
					status++;
					kind = atoi(txt);
					j = 0;
				}
				break;
			case 2:
				if (lig[i]>32 || lig[i]<0)
					name[j++] = lig[i];
				if(lig[i]==0)
				{
					name[j]=0;
					namefound=1;
					goto exit_loop;
				}
				break;
			default:
				break;
		}
		if (lig[i] == '/')
			break;
		if (lig[i] == 0)
			break;
		if (i == l)
			break;

		i++;
	}

exit_loop:
	if (level!=-1 && kind!=-1 && namefound==1)
	{
		name[j]=0;
		subMenuItem[nbSubMenuItem].menuLevel = level;
		subMenuItem[nbSubMenuItem].kindOfItem = kind;
		subMenuItem[nbSubMenuItem].setname(name);
		nbSubMenuItem++;
	}
}

void cSubMenu::SaveInfoSubMenu()
{
	cString fichier = cString::sprintf("%s/MainMenu.conf",cPlugin::ConfigDirectory());
	FILE *f = fopen(fichier,"w");
	if (f)
	{
	  for (int i=0;i<nbSubMenuItem;i++)
	  {
		  fprintf(f,"%i:%i:%s\n",subMenuItem[i].menuLevel,subMenuItem[i].kindOfItem,subMenuItem[i].name);
	  }
	  fclose(f);
	}
}

cSubMenu::~cSubMenu()
{
	delete [] subMenuItem;
}

cSubMenu::cSubMenu()
{
	subMenuItem = new cSubMenuItemInfo[NBMAXSUBMENUANDITEM];

	nbLevelMenu = 0;
	currentMenuLevel = 0;
	LoadInfoSubMenu();
	Arbo.Populate(this);
	offset=0;
}

int cSubMenu::GetByLevel (int Level, int curr)
{
	int j=0;
	int Index[NBMAXSUBMENUANDITEM];
	for (int i=0;i<nbSubMenuItem;i++)
	{
		if (subMenuItem[i].menuLevel==Level)
		{
			Index[j]=i;
			j++;
		}
	}
	return subMenuItem[Index[curr]].kindOfItem;
}
int cSubMenu::GetByLevel (int Level)
{
	/*
	int Index[NBMAXSUBMENUANDITEM];
	int j=0;
	for (int i=0;i<nbSubMenuItem;i++)
	{
		if (subMenuItem[i].menuLevel==Level)
		{
			Index[j]=i;
			j++;
		}
	Index[j]=-1;
	}
	return Index;
	*/
	return 0;
}

void cSubMenu::CreateItem(int type,int position,int where,const char *Name)
{
	int OldMenuLevel;
	switch(type)
	{
		case 0:
			switch(where)
			{
				case 0:
					OldMenuLevel=subMenuItem[position].menuLevel;
					InsertItem(position);
					subMenuItem[position].setname(Name);
					subMenuItem[position].menuLevel=OldMenuLevel;
					subMenuItem[position].kindOfItem=Arbo.MaxLevel+1;
					break;
				case 1:
					OldMenuLevel=subMenuItem[position].menuLevel;
					InsertItem(position);
					subMenuItem[position+1].setname(Name);
					subMenuItem[position+1].menuLevel=OldMenuLevel;
					subMenuItem[position+1].kindOfItem=Arbo.MaxLevel+1;
					break;
				case 2:
					OldMenuLevel=subMenuItem[position].kindOfItem;
					InsertItem(position);
					subMenuItem[position+1].setname(Name);
					subMenuItem[position+1].menuLevel=OldMenuLevel;
					subMenuItem[position+1].kindOfItem=Arbo.MaxLevel+1;
					break;
				default:
					break;
			}
			Arbo.Populate(this);
			break;
		case 1 :
			switch(where)
			{
				case 0:
					OldMenuLevel=subMenuItem[position].menuLevel;
					InsertItem(position);
					subMenuItem[position].setname(Name);
					subMenuItem[position].menuLevel=OldMenuLevel;
					subMenuItem[position].kindOfItem=0;
					break;
				case 1:
					OldMenuLevel=subMenuItem[position].menuLevel;
					InsertItem(position);
					subMenuItem[position+1].setname(Name);
					subMenuItem[position+1].menuLevel=OldMenuLevel;
					subMenuItem[position+1].kindOfItem=0;
					break;
				case 2:
					OldMenuLevel=subMenuItem[position].kindOfItem;
					InsertItem(position);
					subMenuItem[position+1].setname(Name);
					subMenuItem[position+1].menuLevel=OldMenuLevel;
					subMenuItem[position+1].kindOfItem=0;
					break;
			}
			Arbo.Populate(this);
			break;
		default :
			break;
	}
}
void cSubMenu::Dump(void)
{
	for (int i=0;i<nbSubMenuItem;i++)
	{
		fprintf(stderr,"%i Nom : %s Menu : %i Kind : %i\n",i,subMenuItem[i].name,subMenuItem[i].menuLevel,subMenuItem[i].kindOfItem);
	}
}

void cSubMenu::DeleteItem(int position,int kindItem)
{
	int Kind;
	if (kindItem!=-1)
		Kind=kindItem;
	else
		Kind=subMenuItem[position].kindOfItem;
	for(int i=position;i<nbSubMenuItem;i++)
	{
		subMenuItem[i]=subMenuItem[i+1];
	}
	nbSubMenuItem--;
	Arbo.Populate(this);
	if (Kind!=0)
	{
		for(int i=position;i<nbSubMenuItem;i++)
		{
			if (subMenuItem[i].menuLevel==Kind)
			{
				DeleteItem(i,subMenuItem[i].kindOfItem);
				i--;
			}
		}
	}
	Arbo.Populate(this);
}

void cSubMenu::InsertItem(int position)
{
	for (int i=nbSubMenuItem;i>position;i--)
	{
		subMenuItem[i]=subMenuItem[i-1];
	}
	nbSubMenuItem++;
	Arbo.Populate(this);
}


cArbo::cArbo(void)
{
	MaxLevel=0;
	for (int i=0;i<NBMAXSUBMENUANDITEM;i++)
	{
		MaxItem[i]=0;
	}
}

void cArbo::Clear()
{
	MaxLevel=0;
	for (int i=0;i<NBMAXSUBMENUANDITEM;i++)
	{
		MaxItem[i]=0;
		for (int j=0;j<NBMAXSUBMENUANDITEM;j++)
		{
			Index[i][j]=0;
		}
	}
}

cArbo::~cArbo(void)
{
}

void cArbo::Populate(cSubMenu *submenu)
{
  Clear();
  for (int i=0;i<submenu->nbSubMenuItem;i++)
  {
  Index[ submenu->subMenuItem[i].menuLevel ][ submenu->Arbo.MaxItem[submenu->subMenuItem[i].menuLevel]] = i;
  submenu->Arbo.MaxItem[submenu->subMenuItem[i].menuLevel]++;
  if (submenu->subMenuItem[i].menuLevel > submenu->Arbo.MaxLevel)
	  {
		  submenu->Arbo.MaxLevel = submenu->subMenuItem[i].menuLevel;
	  }
  if (submenu->subMenuItem[i].kindOfItem > submenu->Arbo.MaxLevel)
	  {
		  submenu->Arbo.MaxLevel = submenu->subMenuItem[i].kindOfItem;
	  }
  }
}

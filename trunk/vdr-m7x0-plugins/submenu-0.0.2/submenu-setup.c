#include "i18n.h"
#include "submenu-setup.h"
#include <vdr/submenu.h>
#include <vdr/interface.h>

static char *System[6] = { "Channels","Recordings","Timers","Setup","Commands","Schedule"};


// cSubMenuSelectType : Menu Osd pour la selection du Type d'Item a ajouter...
//
// ---------------------------------------------------------------------------

cSubMenuSelectType::cSubMenuSelectType(int pos,int ou,cSubMenu &SM) : cOsdMenu(tr("Select Item type to insert"),10), SMSetup(SM)
{
	close=false;
	position=pos;
	where=ou;
	Add(new cOsdItem(tr("Sub Menu"),osUser7));
	Add(new cOsdItem(tr("System Item"),osUser8));
	Add(new cOsdItem(tr("Plugin Item"),osUser9));
}

cSubMenuSelectType::~cSubMenuSelectType()
{
}

eOSState cSubMenuSelectType::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
	if (close && !HasSubMenu()) return osBack;
	switch(state)
	{
		case osUser7 :
			close=true;
			return AddSubMenu(new cSubMenuSubMenuItem(position,where,SMSetup));
			break;
		case osUser8 :
			close=true;
			return AddSubMenu(new cSubMenuSystemItem(position,where,SMSetup));
			break;
		case osUser9:
			close=true;
			return AddSubMenu(new cSubMenuPluginItem(position,where,SMSetup));
			break;
		default:
			break;
	}
	return state;
}

//cSubMenuPluginItem : Item de type Plugin : Liste des plugins lancés sur le systeme
//
//----------------------------------------------------------------------------------

cSubMenuPluginItem::cSubMenuPluginItem(int pos,int ou,cSubMenu &SM) : cOsdMenu(tr("Insert Plugin Item"),10), SMSetup(SM)
{
	position=pos;
	where=ou;

	DispOrder=NULL;
	ToDisp=NULL;

	cPlugin *p;
	int i=0;
	while((p = cPluginManager::GetPlugin(i)))
	{
		i++;
	}
	int PlugNbr=i;
	ToDisp=(int *)realloc(ToDisp,(PlugNbr+1)*sizeof(int));
	for (int i=0;i<PlugNbr;i++)
	{
		ToDisp[i]=i;
	}
	for (int i=0;i<SMSetup.nbSubMenuItem;i++)
	{
		for(int j=0;j<PlugNbr;j++)
		{
			p=cPluginManager::GetPlugin(j);
			if(!strcmp(SMSetup.subMenuItem[i].name,p->Name()))
			{
				ToDisp[j]=-1;
			}
		}
	}
	int order=0;
	for (int i=0;i<PlugNbr;i++)
	{
		if (ToDisp[i]!=-1)
		{
			Add(new cOsdItem(tr(cPluginManager::GetPlugin(i)->Name()),osUser9));
			DispOrder=(int *)realloc(DispOrder,(order+1)*sizeof(int));	
			DispOrder[order]=i;
			order++;
		}
	}
}

cSubMenuPluginItem::~cSubMenuPluginItem()
{
	delete ToDisp;
	delete DispOrder;
}

eOSState cSubMenuPluginItem::ProcessKey(eKeys Key)
{
	eOSState state=cOsdMenu::ProcessKey(Key);
	switch(Key)
	{
		case kOk :
			if (Current()!=-1)
			{
				SMSetup.CreateItem(1,position,where,(char *)cPluginManager::GetPlugin(DispOrder[Current()])->Name());
				return osBack;
			}
			break;
		case kBack : 
			return osBack;
			break;
		default :
			break;
	}
	return state;
}

// cSubMenuSystemItem : Item de type System : Channels,Recordings,Setup etc etc
//
// ----------------------------------------------------------------------------

cSubMenuSystemItem::cSubMenuSystemItem(int pos,int ou,cSubMenu &SM) : cOsdMenu(tr("Insert System Item"),10), SMSetup(SM)
{
	position=pos;
	where=ou;
	DispOrder = NULL;
	int ToDisplay[] = {0,1,2,3,4,5};
	int order=0;
	for(int i=0;i<SMSetup.nbSubMenuItem;i++)
	{
		for(int j=0;j<6;j++)
		{
			if (!strcmp(SMSetup.subMenuItem[i].name,System[j]))
			{
				ToDisplay[j]=-1;
			}
		}
	}
	for (int i=0;i<6;i++)
	{
		if (ToDisplay[i]!=-1)
		{
			Add(new cOsdItem(tr(System[i]),osUser9));
			DispOrder=(int *)realloc(DispOrder,sizeof(DispOrder)+sizeof(int));
			DispOrder[order]=i;
			order++;
		}
	}
}

eOSState cSubMenuSystemItem::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
	switch(Key)
	{
		case kOk :
			   if (Current()!=-1)
			   {
				   SMSetup.CreateItem(1,position,where,System[DispOrder[Current()]]);
				   return osBack;
			   }
			   break;
		case kBack:
			   return osBack;
			   break;
		default:
			   break;

	}
	return state;
}

cSubMenuSystemItem::~cSubMenuSystemItem()
{
	delete DispOrder;
}



// cSubMenuSubMenuItem  : Item de type Sous Menu
//
// ---------------------------------------------


cSubMenuSubMenuItem::cSubMenuSubMenuItem(int position,int ou,cSubMenu &SM) : cOsdMenu(tr("Insert SubMenu Item"),10), SMSetup(SM)
{
	where=ou;
	strn0cpy(Name," ",64);
	pos=position;
        static const char allowed[] = { " abcdefghijklmnopqrstuvwxyzÐÑÒÓÔÕñÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîï0123456789-.#~" };
	Add(new cMenuEditStrItem("Nom",Name,sizeof(Name),allowed));
	Display();
}

cSubMenuSubMenuItem::~cSubMenuSubMenuItem()
{
	delete Name;
}


eOSState cSubMenuSubMenuItem::ProcessKey (eKeys key)
{
	eOSState state = cOsdMenu::ProcessKey(key);
	switch(key)
	{
		case kOk : 
			   SMSetup.CreateItem(0,pos,where,Name);
			   return osBack;
			   break;
		case kBack :
			   return osBack;
			   break; 
	 	default :
			   break;
	}
	return state;
	
}


// cSubMenuSetup : page Setup du plugin SubMenu
//
// --------------------------------------------

cSubMenuSetup::cSubMenuSetup(void)
{
	curLevel = 0;
	fromsub=false;
	MarkIndex=-1;
	Marked=false;
	CreateIndex=-1;
	Created=false;
	DrawMenu();
	DrawButton();
}

void cSubMenuSetup::DrawSubMenu(int level)
{
	curLevel++;
	char *line, *tmp;
	for(int i=0;i<SMSetup.Arbo.MaxItem[level];i++)
	{
		asprintf(&line,"%s","");
		for (int z=0;z<curLevel*5;z++)
		{
			tmp = line;
			asprintf(&line,"%s ",tmp);
			free(tmp);
		}
		if (SMSetup.subMenuItem[SMSetup.Arbo.Index[level][i]].kindOfItem!=0)
		{
			tmp = line;
			asprintf(&line,"%s+",tmp);
			free(tmp);
		}
		tmp = line;
		asprintf(&line,"%s%s",tmp,tr(SMSetup.subMenuItem[SMSetup.Arbo.Index[level][i]].name));
		free(tmp);
		Add (new cOsdItem(line),true);
		SMSetup.Arbo.CurrentIndex[Current()]=SMSetup.Arbo.Index[level][i];
		if (SMSetup.subMenuItem[SMSetup.Arbo.Index[level][i]].kindOfItem!=0)
		{
		   DrawSubMenu(SMSetup.subMenuItem[SMSetup.Arbo.Index[level][i]].kindOfItem);
		   curLevel--;
		}
		free(line);
	}
}

void cSubMenuSetup::DrawMenu(void)
{
	int cur=Current();
	Clear();
	char *line, *tmp;
	for (int j=0;j<SMSetup.Arbo.MaxItem[0];j++)
	{
		asprintf(&line,"%s"," ");
		if (SMSetup.subMenuItem[SMSetup.Arbo.Index[0][j]].kindOfItem!=0)
		{
			free(line);
			asprintf(&line,"+");
		}
		tmp = line; 
		asprintf(&line,"%s%s",tmp,tr(SMSetup.subMenuItem[SMSetup.Arbo.Index[0][j]].name));
		free(tmp);
		Add (new cOsdItem(line),true);
		SMSetup.Arbo.CurrentIndex[Current()]=SMSetup.Arbo.Index[0][j];
		if (SMSetup.subMenuItem[SMSetup.Arbo.Index[0][j]].kindOfItem!=0)
		{
			DrawSubMenu(SMSetup.subMenuItem[SMSetup.Arbo.Index[0][j]].kindOfItem);
			curLevel--;
		}
		free(line);
	}
	SetCurrent(Get(cur));
	Display();
}


cSubMenuSetup::~cSubMenuSetup(void)
{
}


void cSubMenuSetup::Store(void)
{
}

eOSState cSubMenuSetup::ProcessKey(eKeys key)
{
	eOSState state=cOsdMenu::ProcessKey(key);
	if (HasSubMenu())
	{
		fromsub=true;
		return osContinue;
	}
	if (fromsub && !HasSubMenu())
	{
		fromsub=false;
		DrawMenu();
		return osContinue;
	}
	switch(key)
	{
		case kRed:
			if (!Created && !Marked)
			{
				Created=true;
				CreateIndex=Current();
			}
			else if (Created)
			{
				DrawMenu();
				Created=false;
				CreateIndex=-1;
				return AddSubMenu(new cSubMenuSelectType(SMSetup.Arbo.CurrentIndex[Current()],0,SMSetup));
			}
			else if (Marked)
			{
				fprintf(stderr,"Move Avant\n");
				Marked=false;
				MarkIndex=-1;
			}
			DrawButton();
			break;
		case kGreen:
			if (!Created && !Marked)
			{
			}
			else if(Created)
			{
				Created=false;
				CreateIndex=-1;
				return AddSubMenu(new cSubMenuSelectType(SMSetup.Arbo.CurrentIndex[Current()],1,SMSetup));
			}
			else if(Marked)
			{
				fprintf(stderr,"Move Apres\n");
				Marked=false;
				MarkIndex=-1;
			}
			DrawButton();
			break;
		case kYellow:
			if (!Marked && !Created)
			{
				SMSetup.DeleteItem(SMSetup.Arbo.CurrentIndex[Current()]);
				DrawMenu();
			}
			else if (SMSetup.subMenuItem[SMSetup.Arbo.CurrentIndex[Current()]].kindOfItem != 0)
			{
				if (Marked)
				{
					fprintf(stderr,"Deplacer Dans\n");
				}
				else if (Created)
				{
					Created=false;
					CreateIndex=-1;
					return AddSubMenu(new cSubMenuSelectType(SMSetup.Arbo.CurrentIndex[Current()],2,SMSetup ) );
				}
			}
			DrawButton();
			break;
		case kBlue : 
			if (Marked || Created)
			{
				Marked=false;
				MarkIndex=-1;
				Created=false;
				CreateIndex=-1;
			}
			else
			{
				Marked=true;
				MarkIndex=Current();
			}
			DrawButton();
			break;
		case kUp:
		case kDown:
		case kLeft:
		case kRight:
			if (Created) 
			{
				Created=false;
				CreateIndex=-1;
			}
			DrawButton();
			break;
		case kOk:
			if (Interface->Confirm(tr("Apply Changes?")))
			{
				SMSetup.SaveInfoSubMenu();
			}
			return osEnd;
			break;
		default: 
			DrawButton();
			break;
	}
	return state;
}

void cSubMenuSetup::DrawButton(void)
{
	if (!Marked && !Created)
		SetHelp(tr("Create"),tr("Edit"),tr("Delete"),tr("Move"));
	else
		SetHelp(tr("Before"),tr("After"),SMSetup.subMenuItem[SMSetup.Arbo.CurrentIndex[Current()]].kindOfItem == 0 ? NULL : tr("Into"),tr("Cancel"));
}

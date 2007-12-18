#include "chanorg.h"
#include "chan-menu.h"
#include <vdr/interface.h>

#ifndef VDRVERSNUM
#include <vdr/config.h>
#endif

cGroupMove::cGroupMove(cChannel *Group) : cOsdMenu(tr("Move before group"),10)
{
	ToMove=Group;
	Setup();
}

void cGroupMove::Setup()
{
	cChannel *tmp=ChanOrg.OrgChans.First();
        Clear();
	cChannel *last=new cChannel();
#if VDRVERSNUM<10315
	last->SetName(tr(":End"));
	ChanOrg.OrgChans.NewChannel(last,last->Name(),0,0,0,0);
#else
	last->SetName(tr(":End"),"","");
	ChanOrg.OrgChans.NewChannel(last,last->Name(),last->ShortName(),last->Provider(),0,0,0,0);
#endif
	ChanOrg.OrgChans.Last()->Parse(last->Name());
	ChanOrg.OrgChans.ReNumber();
	int j=0;
	if (ChanOrg.OrgChans.First()->GroupSep() && tmp!=ToMove)
	{
		Add(new cOsdItem(ChanOrg.OrgChans.First()->Name(),osUser1));
		Group[j]=ChanOrg.OrgChans.First();
		j++;
	}
	int i=0;
	while ((i=ChanOrg.OrgChans.GetNextGroup(i))>0)
	{
		tmp=ChanOrg.OrgChans.Get(i);
		Group[j]=tmp;
		if (tmp != ToMove && tmp->Index()!=ChanOrg.OrgChans.GetNextGroup(ToMove->Index()))
		{
		Add(new cOsdItem(tmp->Name(),osUser1));
		j++;
		}
	}
	SetCurrent(Get(ChanOrg.LastAffect));
	Display();
}

cGroupMove::~cGroupMove()
{
}

eOSState cGroupMove::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
	switch (state)
	{
		case osUser1 : 
			MoveGroup(Group[Current()]);
			ChanOrg.OrgChans.Del(ChanOrg.OrgChans.Last());
			ChanOrg.OrgChans.ReNumber();
			ChanOrg.ReDraw=true;
			return osBack;
			break;
		default:break;
	}
	return state;
}

void cGroupMove::MoveGroup(cChannel *chan)
{
	cChannel *movelist[5000];
	cChannel *move=ToMove;
	int i=0;
	int nbr=0;
	while (move && (!move->GroupSep() || move==ToMove) )
	{
		movelist[i]=move;
		move=ChanOrg.OrgChans.Next(move);
		i++;
	}
	nbr=i;
	for (int j=0;j<nbr;j++)
	{
		if (movelist[j]->Index() > chan->Index())
		ChanOrg.OrgChans.Move(movelist[j],chan);
		else
		ChanOrg.OrgChans.Move(movelist[j]->Index(),chan->Index()-1);
	}
}


cAffect::cAffect(cChannel *ch) : cOsdMenu ("" ,10 )
{
	char *title;
	asprintf(&title,"%s %s",tr("Select Group for"),ch->Name());
	this->SetTitle(title);
	chan=ch;
	Setup();
}

cAffect::~cAffect()
{
}

eOSState cAffect::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
	switch(Key)
	{
		case kOk : 
			   ChanOrg.LastAffect = Current();
			   if (chan->Number()>Group[Current()]->Number())
			   {
				ChanOrg.OrgChans.Move(chan,ChanOrg.OrgChans.Next(Group[Current()]));
			   }
			   else
			   {
			   ChanOrg.OrgChans.Move(chan,Group[Current()]);
			   }
			   ChanOrg.OrgChans.ReNumber();
			   ChanOrg.ReDraw=true;
			   return osBack;
			   break;
		default : break;
	}
	return state;
}

void cAffect::Setup()
{
	Clear();
	int j=0;
	if (ChanOrg.OrgChans.First()->GroupSep())
	{
		Add(new cOsdItem(ChanOrg.OrgChans.First()->Name(),osUser1));
		Group[j]=ChanOrg.OrgChans.First();	
		j++;
	}
	int i=0;
	cChannel *tmp;
	while ((i=ChanOrg.OrgChans.GetNextGroup(i))>0)
	{
		tmp=GetByIndex(i);
		Group[j]=tmp;
		Add(new cOsdItem(tmp->Name(),osUser1));
		j++;
	}
	SetCurrent(Get(ChanOrg.LastAffect));
	Display();
}

cGroup::cGroup(int Position,int Numero,cChannel *Chan) : cOsdMenu (tr("Group"),10)
{
	strn0cpy(Name," ",64);
	Num=Numero;
	Pos=Position;
	CurChan=Chan;
	New=true;
	Setup();
}

cGroup::cGroup(const char* Nom,int Numero,int Position,cChannel *Chan) : cOsdMenu (tr("Group"),10)
{
	strn0cpy(Name,Nom,64);
	New=false;
	CurChan=Chan;
	Num=Numero;
	Pos=Position;
	Setup();
}


void cGroup::Setup(void)
{
        static const char allowed[] = { " abcdefghijklmnopqrstuvwxyzÐÑÒÓÔÕñÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîï0123456789-.#~" };
	Clear();
	Add(new cMenuEditStrItem(tr("Name"),Name,sizeof(Name),allowed));
	Add(new cMenuEditIntItem(tr("Number"),&Num));
	Display();
}

cGroup::~cGroup(void)
{
	CurChan=NULL;
}

eOSState cGroup::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
	if (state==osUnknown)
	{
		switch(Key)
		{
			case kOk : 
				   if (New)
				   {
					   Insert();
				   }
				   else
				   {
				           Edit();
				   }
				   ChanOrg.ReDraw=true;
				   return osBack;
				   break;
			default :break;
		}
	}
	return state;
}

//Classe cChanOrg

cChanOrgMenu::cChanOrgMenu(void) : cOsdMenu(tr("Channels Organizer"),20)
{
	ClearMarks();
	SaveTimers();
	MakeChannelsList();
	Draw();
}

cChanOrgMenu::~cChanOrgMenu(void)
{
	delete &ChanOrg;
}

void cChanOrgMenu::TriAlphaCroiss(cChannel *start,cChannel *stop)
{
	if (start->Index()+1>=stop->Index()) return;
	bool DelLast=false;
	if (stop==ChanOrg.OrgChans.Last())
	{
		cChannel *last=new cChannel();
#if VDRVERSNUM<10315
		last->SetName(":Last");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),0,0,0,0);
#else
		last->SetName(":Last","","");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),last->ShortName(),last->Provider(),0,0,0,0);
#endif
		ChanOrg.OrgChans.Last()->Parse(last->Name());
		ChanOrg.OrgChans.ReNumber();
		stop=ChanOrg.OrgChans.Last();
		DelLast=true;
	}
	bool cont=false;
	cChannel *chan = ChanOrg.OrgChans.Next(start);
	cChannel *tmp = ChanOrg.OrgChans.Next(chan);
	while (chan && ((start==ChanOrg.OrgChans.Last())? (tmp!=NULL) :(tmp!=stop)))
	{
	 if ( ( strcasecmp(chan->Name(),tmp->Name()) ) > 0) 
	 {
		ChanOrg.OrgChans.Move(chan,tmp);
		cont=true;
	 }
	else chan=tmp;
	tmp=ChanOrg.OrgChans.Next(chan);
	}
	ChanOrg.OrgChans.ReNumber();
	chan=NULL;
	tmp=NULL;
	if (DelLast)
	{
		ChanOrg.OrgChans.Del(ChanOrg.OrgChans.Last());
		stop=ChanOrg.OrgChans.Last();
	}
	if (cont) TriAlphaCroiss(start,stop);
}

void cChanOrgMenu::TriAlphaDec(cChannel *start,cChannel *stop)
{
	if (start->Index()+1>=stop->Index()) return;
	bool DelLast=false;
	if (stop==ChanOrg.OrgChans.Last())
	{
		cChannel *last=new cChannel();
#if VDRVERSNUM<10315
		last->SetName(":Last");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),0,0,0,0);
#else
		last->SetName(":Last","","");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),last->ShortName(),last->Provider(),0,0,0,0);
#endif
		ChanOrg.OrgChans.Last()->Parse(last->Name());
		ChanOrg.OrgChans.ReNumber();
		stop=ChanOrg.OrgChans.Last();
		DelLast=true;
	}
	bool cont=false;
	cChannel *chan = ChanOrg.OrgChans.Next(start);
	cChannel *tmp = ChanOrg.OrgChans.Next(chan);
	while (chan && ((start==ChanOrg.OrgChans.Last())? (tmp!=NULL) :(tmp!=stop)))
	{
	 if ( ( strcasecmp(chan->Name(),tmp->Name()) ) < 0) 
	 {
		ChanOrg.OrgChans.Move(chan,tmp);
		cont=true;
	 }
	else chan=tmp;
	tmp=ChanOrg.OrgChans.Next(chan);
	}
	ChanOrg.OrgChans.ReNumber();
	chan=NULL;
	tmp=NULL;
	if (DelLast)
	{
		ChanOrg.OrgChans.Del(ChanOrg.OrgChans.Last());
		stop=ChanOrg.OrgChans.Last();
	}
	if (cont) TriAlphaDec(start,stop);
}

void cChanOrgMenu::TriTranspCroiss(cChannel *start,cChannel *stop)
{
	if (start->Index()+1>=stop->Index()) return;
	bool DelLast=false;
	if (stop==ChanOrg.OrgChans.Last())
	{
		cChannel *last=new cChannel();
#if VDRVERSNUM<10315
		last->SetName(":Last");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),0,0,0,0);
#else
		last->SetName(":Last","","");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),last->ShortName(),last->Provider(),0,0,0,0);
#endif
		ChanOrg.OrgChans.Last()->Parse(last->Name());
		ChanOrg.OrgChans.ReNumber();
		stop=ChanOrg.OrgChans.Last();
		DelLast=true;
	}
	bool cont=false;
	cChannel *chan = ChanOrg.OrgChans.Next(start);
	cChannel *tmp = ChanOrg.OrgChans.Next(chan);
	while (chan && ((start==ChanOrg.OrgChans.Last())? (tmp!=NULL) :(tmp!=stop)))
	{
	 if (  chan->Frequency() > tmp->Frequency()) 
	 {
		ChanOrg.OrgChans.Move(chan,tmp);
		cont=true;
	 }
	else chan=tmp;
	tmp=ChanOrg.OrgChans.Next(chan);
	}
	ChanOrg.OrgChans.ReNumber();
	chan=NULL;
	tmp=NULL;
	if (DelLast)
	{
		ChanOrg.OrgChans.Del(ChanOrg.OrgChans.Last());
		stop=ChanOrg.OrgChans.Last();
	}
	if (cont) TriTranspCroiss(start,stop);
}

void cChanOrgMenu::TriTranspDec(cChannel *start,cChannel *stop)
{
	if (start->Index()+1>=stop->Index()) return;
	bool DelLast=false;
	if (stop==ChanOrg.OrgChans.Last())
	{
		cChannel *last=new cChannel();
#if VDRVERSNUM<10315
		last->SetName(":Last");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),0,0,0,0);
#else
		last->SetName(":Last","","");
		ChanOrg.OrgChans.NewChannel(last,last->Name(),last->ShortName(),last->Provider(),0,0,0,0);
#endif
		ChanOrg.OrgChans.Last()->Parse(last->Name());
		ChanOrg.OrgChans.ReNumber();
		stop=ChanOrg.OrgChans.Last();
		DelLast=true;
	}
	bool cont=false;
	cChannel *chan = ChanOrg.OrgChans.Next(start);
	cChannel *tmp = ChanOrg.OrgChans.Next(chan);
	while (chan && ((start==ChanOrg.OrgChans.Last())? (tmp!=NULL) :(tmp!=stop)))
	{
	 if ( chan->Frequency()<tmp->Frequency()) 
	 {
		ChanOrg.OrgChans.Move(chan,tmp);
		cont=true;
	 }
	else chan=tmp;
	tmp=ChanOrg.OrgChans.Next(chan);
	}
	ChanOrg.OrgChans.ReNumber();
	chan=NULL;
	tmp=NULL;
	if (DelLast)
	{
		ChanOrg.OrgChans.Del(ChanOrg.OrgChans.Last());
		stop=ChanOrg.OrgChans.Last();
	}
	if (cont) TriTranspDec(start,stop);
}

void cChanOrgMenu::MakeChannelsList(void)
{
  ChanOrg.OrgChans.cList<cChannel>::Clear();
  for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
	  if (!channel->GroupSep() || *channel->Name())
	  {
		 
#if VDRVERSNUM<10315
		 ChanOrg.OrgChans.NewChannel(channel,channel->Name(),channel->Nid(),channel->Tid(),channel->Sid(),channel->Rid());
#else
		 ChanOrg.OrgChans.NewChannel(channel,channel->Name(),channel->ShortName(),channel->Provider(),channel->Nid(),channel->Tid(),channel->Sid(),channel->Rid());
#endif
#if VDRVERSNUM>10317
		 ChanOrg.OrgChans.Last()->Parse(*channel->ToText());
#else
		 ChanOrg.OrgChans.Last()->Parse(channel->ToText());
#endif
	  }
      }
}

void cChanOrgMenu::Draw(void)
{
ChanOrg.ReDraw=false;
cChannel *chan=ChanOrg.OrgChans.First();
char *buff;
int i = Current();
Clear();
while ( (chan) )
{
	if ( chan->GroupSep() )
	{
		asprintf(&buff,"    %s",chan->Name());
		Add(new cOsdItem(buff,osUser1));
	}
	else
	{
		asprintf(&buff,"%i : %s",chan->Number(),chan->Name());
		Add(new cOsdItem(buff,osUser1));
	}
	chan=ChanOrg.OrgChans.Next(chan);
}
SetCurrent(Get(i));
if (i>=0)
{
	chan=GetByIndex(i);
}
else
{
	chan=GetByIndex(0);
}
if (chan) chan->GroupSep()? SetHelp(tr("Del Group"),tr("Edit Group"),tr("Sort Group"),tr("Move")) : SetHelp (tr("Add Group"),tr("Affect"),tr("Zap"),tr("Move"));
ChanOrg.OrgChans.ReNumber();
Display();
}

cChannel *cAffect::GetByIndex(int Index)
{
cChannel *chan=ChanOrg.OrgChans.First();
	for(int i=0;i!=Index;i++)
	{
		chan=ChanOrg.OrgChans.Next(chan);
	}
	return chan;
}

cChannel *cChanOrgMenu::GetByIndex(int Index)
{
cChannel *chan=ChanOrg.OrgChans.First();
	for(int i=0;i!=Index;i++)
	{
		chan=ChanOrg.OrgChans.Next(chan);
	}
	return chan;
}

void cGroup::Edit(void)
{
	char *name;
	if (Num==0) asprintf(&name,":%s",Name);
	else asprintf(&name,":@%i %s",Num,Name);
#if VDRVERSNUM<10315
	CurChan->SetName(name);
#else
	CurChan->SetName(name,"","");
#endif
	CurChan->Parse(name);
	ChanOrg.OrgChans.ReNumber();
}

void cGroup::Insert(void)
{
	int num=CurChan->Number();
	cChannel *channel=ChanOrg.OrgChans.Last();
#if VDRVERSNUM<10315
	ChanOrg.OrgChans.NewChannel(channel,channel->Name(),channel->Nid(),channel->Tid(),channel->Sid(),channel->Rid());
#else
	ChanOrg.OrgChans.NewChannel(channel,channel->Name(),channel->ShortName(),channel->Provider(),channel->Nid(),channel->Tid(),channel->Sid(),channel->Rid());
#endif
	for (int i=ChanOrg.OrgChans.Count();i>Pos;i--)
	{
		ChanOrg.OrgChans.Move(i-1,i); 
	}
	char *name;
	CurChan=ChanOrg.OrgChans.Prev(CurChan);
	if (Num==0) asprintf(&name,":%s",Name);
	else asprintf(&name,":@%i %s",Num,Name);
#if VDRVERSNUM<10315
	CurChan->SetName(name);
#else
	CurChan->SetName(name,"","");
#endif
	CurChan->SetNumber(num);
	CurChan->Parse(name);
	ChanOrg.OrgChans.ReNumber();
}

void cChanOrgMenu::SaveTimers()
{
	int i=0;
	for (cTimer *t = Timers.First(); t; t= Timers.Next(t))
	{
		ChanOrg.Tmr.Add(new cTimer(*t));
#if VDRVERSNUM>10317
		ChanOrg.Tmr.Last()->Parse((const char *)t->ToText(false));
		asprintf(&ChanOrg.TmrChan[i],"%s",*t->ToText(true));
#else
		ChanOrg.Tmr.Last()->Parse(t->ToText(false));
		asprintf(&ChanOrg.TmrChan[i],"%s",t->ToText(true));
#endif
		i++;
	}
}

void cChanOrgMenu::RestoreTimers()
{
	int i=0;
	Timers.cList<cTimer>::Clear();
	for (cTimer *t = ChanOrg.Tmr.First(); t; t = ChanOrg.Tmr.Next(t))
	{
		Timers.Add(new cTimer(*t));
		Timers.Last()->Parse(ChanOrg.TmrChan[i]);
		i++;
	}
}


eOSState cChanOrgMenu::ProcessKey(eKeys Key)
{
	eOSState state = cOsdMenu::ProcessKey(Key);
	cChannel *chan=ChanOrg.OrgChans.First();
	char totext[255];
	if (!HasSubMenu())
	{
	switch(state)
	{
		case osUser1 : 
			        if (Interface->Confirm(tr("Apply Changes?")))
				{
					Channels.cList<cChannel>::Clear();
					while (chan)
					{
#if VDRVERSNUM<10315
						Channels.NewChannel(chan,chan->Name(),chan->Nid(),chan->Tid(),chan->Sid(),chan->Rid());
#else
						Channels.NewChannel(chan,chan->Name(),chan->ShortName(),chan->Provider(),chan->Nid(),chan->Tid(),chan->Sid(),chan->Rid());
#endif
#if VDRVERSNUM>10317
						strcpy(totext,(const char *)chan->ToText());
#else
						strcpy(totext,chan->ToText());
#endif
						Channels.Last()->Parse(totext);
						chan=ChanOrg.OrgChans.Next(chan);
					}
					Channels.ReNumber();
#if VDRVERSNUM>10317
					Channels.SetModified(true);
					RestoreTimers();
#endif
				}
				return osEnd;
				break;

		case osContinue :
			switch(Key)
			{
			case kUp:
			case kUp | k_Repeat:
			case kDown:
			case kDown | k_Repeat:
			case kLeft:
			case kLeft | k_Repeat:
			case kRight:
			case kRight | k_Repeat:
				  if (!MarkMove)
				  (GetByIndex(Current())->GroupSep())? SetHelp(tr("Del Group"),tr("Edit Group"),tr("Sort Group"),tr("Move")) : SetHelp (tr("Add Group"),tr("Affect"),tr("Zap"),tr("Move"));
				  MarkTri=false;
				  break;
			default : break;
			}
		case osUnknown:
			if (ChanOrg.ReDraw) Draw();
			switch(Key)
			{
			case kBlue:
				if (MarkMove)
				{
					if ( (Marked != Current()) && Marked!=Current()+1    )

					{
						ChanOrg.OrgChans.Move(Marked,Current()<=Marked ? Current()+1 : Current() );
						ChanOrg.OrgChans.ReNumber();
					}
				MarkMove=false;
				Marked=0;
				Draw();
				}
				else if (NoMark())
				{
					if (GetByIndex(Current())->GroupSep())
					{
					      return AddSubMenu(new cGroupMove(GetByIndex(Current())));
					}
					else
					{
					MarkMove=true;
					SetHelp(tr("Cancel"),NULL,tr("Before"),tr("After"));
					Marked=Current();
					}
				}
				else if (MarkTri)
				{
					//Tri Transp Dec
					Skins.Message(mtStatus,tr("Sorting"));
					TriTranspDec(GetByIndex(Current()),ChanOrg.OrgChans.GetNextGroup(Current())>0? GetByIndex(ChanOrg.OrgChans.GetNextGroup(Current())):ChanOrg.OrgChans.Last());
					Skins.Message(mtStatus,tr("Sort done"));
					sleep(Setup.OSDMessageTime);
					Skins.Message(mtStatus,"");
					MarkTri=false;
					Draw();
				}
				break;
			case kRed :
				   if (MarkMove) ClearMarks();
				   else
					   if (! (GetByIndex(Current())) -> GroupSep() )
					   {
						   return AddSubMenu(new cGroup(Current(),GetByIndex(Current())->Number(),GetByIndex(Current())));
					   }
					   else if (!MarkMove && !MarkTri)
						   {
					   	   ChanOrg.OrgChans.Del( GetByIndex(Current()) );
						   ChanOrg.OrgChans.ReNumber();
						   Draw();
						   }
				    	   else
					   {
						   //Tri Alpha Croiss
						Skins.Message(mtStatus,tr("Sorting"));
						TriAlphaCroiss(GetByIndex(Current()),ChanOrg.OrgChans.GetNextGroup(Current())>0? GetByIndex(ChanOrg.OrgChans.GetNextGroup(Current())):ChanOrg.OrgChans.Last());
						Skins.Message(mtStatus,tr("Sort done"));
						sleep(Setup.OSDMessageTime);
						Skins.Message(mtStatus,"");
						MarkTri=false;
						Draw();
						   
					   }
				   break;
			case kYellow :
				   if (MarkMove)
				   {
					if ( Marked!=Current() && Marked+1!=Current())
					{
						ChanOrg.OrgChans.Move(Marked,Current()<=Marked ? Current() : Current()-1 );
						ChanOrg.OrgChans.ReNumber();
					}
					MarkMove=false;
					Marked=0;
					Draw();
				   }
				   else if (( GetByIndex(Current()) ) -> GroupSep())
				   {
					if (MarkTri)
					{
					Skins.Message(mtStatus,tr("Sorting"));
					TriTranspCroiss(GetByIndex(Current()),ChanOrg.OrgChans.GetNextGroup(Current())>0? GetByIndex(ChanOrg.OrgChans.GetNextGroup(Current())):ChanOrg.OrgChans.Last());
					Skins.Message(mtStatus,tr("Sort done"));
					sleep(Setup.OSDMessageTime);
					Skins.Message(mtStatus,"");
					MarkTri=false;
					Draw();
					}
					else
					{
						MarkTri=true;
						SetHelp(tr("Alpha Up"),tr("Alpha Down"),tr("Transp Up"),tr("Transp Down"));
					}

				   }
				   else 
				   {
					   cDevice::PrimaryDevice()->SwitchChannel(Channels.GetByChannelID(ChanOrg.OrgChans.Get(Current())->GetChannelID()),true);
				   }
				   break;
			case kGreen : 
				   if ( (GetByIndex(Current())) -> GroupSep() &&!MarkTri )
				   {
				      return AddSubMenu(new cGroup( GetByIndex(Current())->Name(), GetByIndex(Current())->Number(), Current(), GetByIndex(Current()) ) );
				   }
				   else if(!MarkTri)
				   {
				      return AddSubMenu(new cAffect(GetByIndex(Current())));
				   }
				   else
				   {
					   //Tri Alpha Dec
					   Skins.Message(mtStatus,tr("Sorting"));
					   TriAlphaDec(GetByIndex(Current()),ChanOrg.OrgChans.GetNextGroup(Current())>0? GetByIndex(ChanOrg.OrgChans.GetNextGroup(Current())):ChanOrg.OrgChans.Last());
					   Skins.Message(mtStatus,tr("Sort done"));
					   sleep(Setup.OSDMessageTime);
					   Skins.Message(mtStatus,"");
					   MarkTri=false;
					   Draw();
				   }

				      break;
			case kNone : break;
			default: 
				     break;
			}
		default : break;
	}
	}
	return state;
}


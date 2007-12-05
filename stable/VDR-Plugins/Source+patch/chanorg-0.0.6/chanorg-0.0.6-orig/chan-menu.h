#ifndef __CHANMENU_H
#define __CHANMENU_H
#include <vdr/plugin.h>
#include <vdr/menu.h>
#include "i18n.h"
class cGroupMove : public cOsdMenu
{
	private:
		cChannel *ToMove;
		cChannel *Group[255];
	public:
	cGroupMove(cChannel *Group);
	~cGroupMove(void);
	virtual eOSState ProcessKey(eKeys Key);
	void Setup(void);
	void MoveGroup(cChannel *chan);
};

class cAffect : public cOsdMenu
{
	private : 
		cChannel *chan;
		cChannel *Group[256];
	public :
		cAffect(cChannel *ch);
		~cAffect();
		virtual eOSState ProcessKey(eKeys Key);
		void Setup();
		cChannel *GetByIndex(int);
};

class cGroup : public cOsdMenu
{
	private:
		char Name[64]; //64 = MaxChannelName in channels.h
		int Pos;
		int Num;
		cChannel *CurChan;
		bool New;
	public:
	cGroup(int Position,int Numero,cChannel *Chan);
	cGroup(const char *Nom,int Numero,int Position,cChannel *Chan);
	~cGroup();
	virtual eOSState ProcessKey(eKeys Key);
	char *GetNom(void) { return Name;}
	int GetNum(void) { return Num;}
	void Setup(void);
	void Insert(void);
	void Edit(void);
};

class cChanOrgMenu : public cOsdMenu
{
	private:
		bool MarkMove;
		bool MarkTri;
		int Marked;
	public :
		cChanOrgMenu(void);
		~cChanOrgMenu(void);
		//cChannels OrgChans;
		void MakeChannelsList(void);
		void Draw(void);
		virtual eOSState ProcessKey (eKeys Key);
		bool NoMark(){return (!MarkMove && !MarkTri);}
		void ClearMarks(void){MarkMove=false;MarkTri=false;Draw();}
		cChannel *GetByIndex(int Index);
		void TriAlphaCroiss(cChannel *start,cChannel *stop);
		void TriAlphaDec(cChannel *start,cChannel *stop);
		void TriTranspCroiss(cChannel *start,cChannel *stop);
		void TriTranspDec(cChannel *start,cChannel *stop);
		void SaveTimers(void);
		void RestoreTimers(void);
};


#endif

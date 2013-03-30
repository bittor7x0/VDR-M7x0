#ifndef __SUBMENUSETUP_H
#define __SUBMENUSETUP_H

#include <vdr/plugin.h>
#include <vdr/submenu.h>

class cSubMenuSelectType : public cOsdMenu
{
	private:
		cSubMenu &SMSetup;
		int position;
		int where;
		bool close;
	public:
		cSubMenuSelectType(int pos,int ou,cSubMenu &SMSetup);
		~cSubMenuSelectType(void);
		eOSState ProcessKey(eKeys Key);
};

class cSubMenuPluginItem : public cOsdMenu
{
	private:
		cSubMenu &SMSetup;
		int position;
		int where;
		int *ToDisp;
		int *DispOrder;
	public:
		cSubMenuPluginItem(int pos,int ou,cSubMenu &SMSetup);
		~cSubMenuPluginItem(void);
		eOSState ProcessKey(eKeys Key);
};


class cSubMenuSystemItem : public cOsdMenu
{
	private:
		cSubMenu &SMSetup;
		int position;
		int where;
		int * DispOrder;
	public:
		cSubMenuSystemItem(int pos,int ou,cSubMenu &SMSetup);
		~cSubMenuSystemItem(void);
		eOSState ProcessKey(eKeys Key);
};

class cSubMenuSubMenuItem : public cOsdMenu
{
	private:
		cSubMenu &SMSetup;
		int pos;
		int where;
		char Name[64];
	public:
		cSubMenuSubMenuItem(int position,int ou,cSubMenu &SMSetup);
		~cSubMenuSubMenuItem();
		eOSState ProcessKey(eKeys Key);
};

class cSubMenuSetup : public cMenuSetupPage
{
	private:
		cSubMenu SMSetup;
		void DrawMenu(void);
		void DrawSubMenu(int level);
		bool fromsub;
		int curLevel;
		int MarkIndex;
		bool Marked;
		int CreateIndex;
		bool Created;
	protected:
		virtual void Store(void);
	public:
		cSubMenuSetup(void);
		~cSubMenuSetup(void);
		virtual eOSState ProcessKey(eKeys key);
		void DrawButton(void);
};

#endif //_SUBMENUSETUP_H

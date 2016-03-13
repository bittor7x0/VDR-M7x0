#ifndef __SUBMENU_H
#define __SUBMENU_H

#define SIZEMAXNAMESUBMENUITEMINFO 30
#define NBMAXSUBMENUANDITEM 255

class cSubMenu;

class cSubMenuItemInfo
{
	private:
	public:
		cSubMenuItemInfo();
		~cSubMenuItemInfo();

		int menuLevel;
		int kindOfItem;
		char name[SIZEMAXNAMESUBMENUITEMINFO];  //todo revoir ca(la taille en dur)

		void setname(const char *n);
};

class cArbo
{
	private:
	public:
	      cArbo(void);
	      ~cArbo(void);
	      int Index[NBMAXSUBMENUANDITEM][NBMAXSUBMENUANDITEM];
	      int CurrentIndex[NBMAXSUBMENUANDITEM];
	      int MaxLevel;
	      int MaxItem[NBMAXSUBMENUANDITEM];
	      void Populate(cSubMenu *submenu);
              void Clear(void);
};

class cSubMenu
{
	private:
	public:
		int currentMenuLevel;
		cSubMenuItemInfo *subMenuItem;
		cArbo Arbo;
		int nbSubMenuItem;
		int oldNumberMenu[ NBMAXSUBMENUANDITEM ];
		int nbLevelMenu;

		int offset; // For MenuCmdPosition issue

		cSubMenu();
		virtual ~cSubMenu();
		void LoadInfoSubMenu(void);
		void ParseOneLigOfFile (const char *lig);
		void SaveInfoSubMenu(void);
		int GetByLevel(int Level, int curr);
		int GetByLevel(int Level);
		void CreateItem(int type,int position,int where,const char *Name="");
		void DeleteItem(int position,int kindItem=-1);
		void InsertItem(int position);
		void Dump(void);
};
#endif //__SUBMENU_H

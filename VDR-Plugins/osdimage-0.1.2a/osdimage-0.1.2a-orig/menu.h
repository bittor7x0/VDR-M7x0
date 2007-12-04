// menu.h, brougs78

//#define X_DISPLAY_MISSING

#ifndef _OSDIMAGE_MENU_H_
#define _OSDIMAGE_MENU_H_

#include "osdimage.h"
#include "definitions.h"
//#include "i18n.h"
#include <vdr/osd.h>
#include <vdr/menuitems.h>
#include <vdr/menu.h>
#include <vdr/skins.h>
#include <Magick++.h>
#include <stdio.h>
//#include <vector>
//#include <string.h>


enum eScanMode 
{
	browsing,
	currentPics,
	allPics,
};


class cOSDImageMenu : public cOsdMenu //, public cOSDImageDummy
{
public:
   cOSDImageMenu(sFiles &pics);
	~cOSDImageMenu();

	//void cOSDImageMenu::Show();
	eOSState cOSDImageMenu::ProcessKey(eKeys Key);
	
	bool cOSDImageMenu::ScanDir(eScanMode scan);
	
	void cOSDImageMenu::View(char *selectItem = NULL);
	
	//void SetShowMenu(bool status) { showMenu = status; }
	//bool GetShowMenu(void) { return showMenu; }
	
private:
	sFiles *myPics;
	bool validRootDir;
	string supportedFormats;
};

#endif

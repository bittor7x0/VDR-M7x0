#ifndef __streamplayer_menu_h__
#define __streamplayer_menu_h__

#include <vdr/osd.h>
#include <vdr/menuitems.h>
#include "streamplayer.h"
#include "compatibility.h"

class cOsdBookmarkItem;

class cStreamplayerMenu : public cOsdMenu {
protected:
	char Url[256];

	void SetUrl(char *url);
	void StartPlay();
public:
	cStreamplayerMenu();
	virtual eOSState ProcessKey(eKeys Key);

	friend class cOsdBookmarkItem;
};

class cStreamplayerSetupMenu : public cMenuSetupPage {
private:
	int BufferSize;
	int BufferPreFill;
protected:
	virtual void Store();
public:
	cStreamplayerSetupMenu();
};

#endif

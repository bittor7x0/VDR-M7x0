#include <vdr/tools.h>
#include <vdr/config.h>
#include <vdr/player.h>
#include "menu.h"
#include "player.h"

const char *UrlChars="_abcdefghijklmnopqrstuvwxyz0123456789.-:@/";
const char *TitleChars=" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-/";

//
// cOsdBookmarkItem
//

class cOsdBookmarkItem : public cOsdItem {
protected:
	cBookmark *bookmark;
public:
	cOsdBookmarkItem(cBookmark *Bookmark) { bookmark=Bookmark; Set(); }
	virtual void Set() { SetText(bookmark->GetTitle()); }
	cBookmark* GetBookmark() { return bookmark; }
};

//
// cBookmarkEditMenu
//

class cBookmarkEditMenu : public cOsdMenu {
	cBookmark *Bookmark;
	char Url[256];
	char Title[80];
public:
	cBookmarkEditMenu(cBookmark *bm);
	virtual eOSState ProcessKey(eKeys Key);

};

//
// cBookmarkEditMenu
//

cBookmarkEditMenu::cBookmarkEditMenu(cBookmark *bm)
	:cOsdMenu(tr("Edit Bookmark"),6){
	Bookmark=bm;

	if (Bookmark) {
		strn0cpy(Url,Bookmark->GetUrl(),sizeof Url);
		strn0cpy(Title,Bookmark->GetTitle(),sizeof Title);
	} else {
		Url[0]=0;
		Title[0]=0;
	}

	Add(new cMenuEditStrItem (tr("Title"), (char*)&Title,sizeof Title,TitleChars));
	Add(new cMenuEditStrItem (tr("URL"), (char*)&Url,sizeof Url,UrlChars));
}

eOSState cBookmarkEditMenu::ProcessKey(eKeys Key) {
	eOSState state = cOsdMenu::ProcessKey(Key);

	if (state != osUnknown) return state;

	switch (Key) {
	case kOk:
		if (Bookmark) {
			Bookmark->Set(Title,Url);
			cPluginStreamplayer::Plugin->Bookmarks.Save();
			return osBack;
		} else {
			Bookmark=new cBookmark(Title,Url);
			cPluginStreamplayer::Plugin->Bookmarks.Add(Bookmark);
			cPluginStreamplayer::Plugin->Bookmarks.Save();
			return osUser1;
		}
	default:
		break;
	}
	return osUnknown;
}


//
// cStreamplayerMenu
//

cStreamplayerMenu::cStreamplayerMenu()
	:cOsdMenu(tr("Stream Player"),6) {
	cBookmark *Bookmark;
	int i=0;

	SetUrl(cPluginStreamplayer::Plugin->PlayURL);

	SetHelp(tr("Edit"), tr("New"), 0, tr("Select"));

	Add(new cMenuEditStrItem (tr("URL"), (char*)&Url,sizeof Url,UrlChars));
	cOsdItem *sep=new cOsdItem(tr("Bookmarks:"));
	sep->SetColor(clrCyan, clrBackground);
	Add(sep);

	while ((Bookmark = cPluginStreamplayer::Plugin->Bookmarks.Get(i)) != NULL) {
		Add(new cOsdBookmarkItem(Bookmark));
		i++;
	}
}

eOSState cStreamplayerMenu::ProcessKey(eKeys Key) {
	eOSState state = cOsdMenu::ProcessKey(Key);

	if (state==osUser1) {
		Add(new cOsdBookmarkItem(cPluginStreamplayer::Plugin->Bookmarks.Last()));
		return CloseSubMenu();
	}

	if (state!=osUnknown) return state;


	switch (Key) {
	case kOk:
		if (Current()>=2) {
			cOsdBookmarkItem *obi=(cOsdBookmarkItem*)Get(Current());
			SetUrl(obi->GetBookmark()->GetUrl());
		}
		if (Current()==0 || Current()>=2) {
			StartPlay();
			return osEnd;
		}
		break;
	case kRed:
		if (Current()>=2) {
  			if (HasSubMenu()) return osContinue;
			cOsdBookmarkItem *obi=(cOsdBookmarkItem*)Get(Current());
			return AddSubMenu(new cBookmarkEditMenu(obi->GetBookmark()));
		}
		break;
	case kGreen:
  		if (HasSubMenu()) return osContinue;
		return AddSubMenu(new cBookmarkEditMenu(NULL));

		break;
	case kBlue:
		if (Current()>=2) {
			cOsdBookmarkItem *obi=(cOsdBookmarkItem*)Get(Current());
			SetUrl(obi->GetBookmark()->GetUrl());
		}
		break;
	default:
		break;
	}
	return state;
}

void cStreamplayerMenu::SetUrl(char *url) {
	strncpy(Url,url,sizeof Url);
	cOsdItem *item=Get(0);
	if (item) {
		DisplayCurrent(false);
		SetCurrent(item);
		item->Set();
		DisplayCurrent(true);
	}
}

void cStreamplayerMenu::StartPlay() {
	cPluginStreamplayer::Plugin->StartPlayer(Url);
}


cStreamplayerSetupMenu::cStreamplayerSetupMenu() {
	BufferSize=cPluginStreamplayer::Plugin->BufferSize;
	BufferPreFill=cPluginStreamplayer::Plugin->BufferPreFill;
	
	Add(new cMenuEditIntItem(tr("Receive buffer size (k)"), &BufferSize));
	Add(new cMenuEditIntItem(tr("fill up buffer at start (k)"), &BufferPreFill));
}

void cStreamplayerSetupMenu::Store() {
	SetupStore("BufferSize", cPluginStreamplayer::Plugin->BufferSize=BufferSize);
	SetupStore("BufferPreFill", cPluginStreamplayer::Plugin->BufferPreFill=BufferPreFill);
}

/*
 * status.h: osd status published via SVDRP
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _SVDRPEXT_STATUS__H
#define _SVDRPEXT_STATUS__H

#include <vdr/tools.h>
#include <vdr/status.h>

#define MENU_OPEN  901
#define MENU_CLOSED 902

class cSvdrpExtItem: public cListObject {
	private:
		char *text;
	public:
		const char* Text() { return text; }
		void Update(char* Text) { free(text); text = Text; };
		cSvdrpExtItem(char* Text): text(Text) {};
		~cSvdrpExtItem() { free(text); }
};

class cSvdrpExtStatus: public cStatus {
	friend class cPluginSvdrpExt;
	public:
		enum { MaxTabs = 6 };
	private:
		char *title;
		char *message;
		char *red;
		char *green;
		char *yellow;
		char *blue;
		char *text;
		cList<cSvdrpExtItem>	items;
		int	selected;
		unsigned short	tabs[MaxTabs];
	protected:
		static void append(char *&tail, char type, const char *src, int max);
	public:
		cSvdrpExtStatus();
		virtual ~cSvdrpExtStatus();

		virtual void OsdClear();
		virtual void OsdTitle(const char *Title);
		virtual void OsdStatusMessage(const char *Message);
		virtual void OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue);
		virtual void OsdTextItem(const char *Text, bool Scroll);
		virtual void OsdItem(const char *Text, int Index);
		virtual void OsdCurrentItem(const char *Text);
};

#endif //_SVDRPEXT_STATUS__H

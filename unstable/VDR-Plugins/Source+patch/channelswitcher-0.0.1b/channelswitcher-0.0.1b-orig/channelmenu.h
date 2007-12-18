#ifndef __SWITCHERMENU_H
#define __SWITCHERMENU_H
#include <vdr/menuitems.h>
#include <vdr/plugin.h>
#include "channelswitcher.h"

class cSwitchMenu : public cOsdMenu 
{
	private:
		cSwitcher *switcher;
	public:
		cSwitchMenu(cSwitcher &SwitcherPlug);
		~cSwitchMenu(void);
		virtual eOSState ProcessKey(eKeys Key);
		void Draw(void);
		void Switch(void);
};

#endif

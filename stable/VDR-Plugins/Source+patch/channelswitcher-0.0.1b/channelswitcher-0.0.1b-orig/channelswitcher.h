#ifndef __CHANSWITCH_H
#define __CHANSWITCH_H
#include <vdr/plugin.h>

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Channel.conf Switcher";
static const char *MAINMENUENTRY  = "Channel Switcher";

class cSwitcher: public cPlugin
{
	public:
		char *List[50];
		int ListNbr;
		cSwitcher(void);
		~cSwitcher(void);
		virtual const char *Version(void) {return VERSION; }
		virtual const char *Description(void) {return DESCRIPTION; }
		virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
		virtual cOsdObject *MainMenuAction(void);
		virtual bool Initialize(void);
		int SaveSetup;
};
#endif

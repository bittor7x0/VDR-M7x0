/*
 * osddemo.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: cPluginOSDImage.c 1.3 2004/05/16 09:28:51 kls Exp $
 */

#ifndef _OSDIMAGE_H_
#define _OSDIMAGE_H_

#include "definitions.h"
#include "menu.h"
#include "viewer.h"
#include "setup.h"
#include "i18n.h"
#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/osd.h>
#include <vdr/tools.h>
//#include <vdr/interface.h>


static const char *VERSION        = "0.1.2";
static const char *DESCRIPTION    = "OSD Image Viewer";
static const char *MAINMENUENTRY  = "OSD Image";


// --- cPluginOSDImage --------------------------------------------------------

class cPluginOSDImage : public cPlugin
{
public:
	cPluginOSDImage(void);
	virtual ~cPluginOSDImage();
	virtual const char *Version(void) { return VERSION; }
	virtual const char *Description(void) { return tr(DESCRIPTION); }
	virtual const char *CommandLineHelp(void);
	virtual bool ProcessArgs(int argc, char *argv[]);
	virtual bool Start(void);
	virtual void Housekeeping(void);
	virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
	virtual cOsdObject *MainMenuAction(void);
	virtual cMenuSetupPage *SetupMenu(void);
	virtual bool SetupParse(const char *Name, const char *Value);
	
private:
	//char picture[256];
	sFiles myPics;
	//int curPic;
};

#endif

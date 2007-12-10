/*
 * osddemo.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: osddemo.c 1.3 2004/05/16 09:28:51 kls Exp $
 */

#include "osdimage.h"


bool showMenu;


cPluginOSDImage::cPluginOSDImage(void)
{
	// Initialize any member variables here.
	// DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
	// VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginOSDImage::~cPluginOSDImage()
{
	// Clean up after yourself!
}

const char *cPluginOSDImage::CommandLineHelp(void)
{
	// Return a string that describes all known command line options.
	return "  -d DIR,   --directory=DIR   picture-directory (REQUIRED)\n";
}

bool cPluginOSDImage::ProcessArgs(int argc, char *argv[])
{
	// Implement command line argument processing here if applicable.
	
	string buf;
	static struct option long_options[] =
	{
		{ "directory", required_argument, NULL, 'd' },
		{ NULL }
	};

	
	int c;
	while ((c = getopt_long(argc, argv, "d:", long_options, NULL)) != -1)
	{
		switch (c)
		{
			case 'd':
				if (optarg && (optarg == strchr(optarg, '/') ) )
				{
					buf.assign(optarg);
					
					while (buf.rfind('/') == buf.length() - 1)
					{
						buf.resize(buf.length() - 1);
					}
					
#ifdef DEBUG
					printf("picture-directory: '%s'\n", buf.c_str() );
#endif			
	
					myPics.PushDir(buf);
					
					return true;
				}
				else
				{
					printf("OSDImage-plugin: You have to specify a proper picture-directory (see help)\n");
					
					return false;
				}
			default:
				printf("OSDimage-plugin: You have to specify a picture-directory (see help)\n");
				
				return false;
		}
	}
	
	printf("OSDimage-plugin: You have to specify a picture-directory (see help)\n");	
	
	return false;
}

bool cPluginOSDImage::Start(void)
{
	// Start any background activities the plugin shall perform.
	RegisterI18n(Phrases);
	showMenu = true;
	return true;
}

void cPluginOSDImage::Housekeeping(void)
{
	// Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginOSDImage::MainMenuAction(void)
{
	// Perform the action when selected from the main VDR menu.
	
	if (showMenu)
	{
		myPics.Reset();
		return new cOSDImageMenu(myPics);
	}
	else
	{
		return new cOSDImageViewer(myPics);
	}
}

cMenuSetupPage *cPluginOSDImage::SetupMenu(void)
{
	// Return a setup menu in case the plugin supports one.
	
	return new cOSDImageSetupPage;
}

bool cPluginOSDImage::SetupParse(const char *Name, const char *Value)
{
	// Parse your own setup parameters and store their values.
	
	return OSDImageSetup.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cPluginOSDImage); // Don't touch this!


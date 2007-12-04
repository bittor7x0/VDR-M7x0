// menu.c, brougs78

#include "menu.h"
#include <string>
#include <vector>
#include <list>


extern bool showMenu;
	

cOSDImageMenu::cOSDImageMenu(sFiles &pics) : cOsdMenu(tr("OSD Image Viewer"))
{
	list<CoderInfo> coderList;

	
	myPics = &pics;

	// determine the supported file-formats of ImageMagick
	// get list (all readable, don't care about writeable, don't care about multi-frame support) 
	coderInfoList(&coderList, CoderInfo::TrueMatch, CoderInfo::AnyMatch, CoderInfo::AnyMatch);
	
	list<CoderInfo>::iterator entry = coderList.begin();

	supportedFormats = "-iname '*." + entry->name() + "'";
	entry++;
	
	while( entry != coderList.end() )
	{
		supportedFormats += " -o -iname '*." + entry->name() + "'";
		entry ++;
	}

#ifdef DEBUG
	printf("Support for %d file-extension available via ImageMagick\n", coderList.size() );
#endif
	
	// Test directory	
	if (ScanDir(browsing) )
	{
		SetHelp(tr("Show all"), NULL, NULL, tr("Exit") );
		validRootDir = true;
		View();
	}
	else
	{
		validRootDir = false;
		SetStatus(tr("No pictures found or directory not valid!") );
	}
}


cOSDImageMenu::~cOSDImageMenu()
{
   // Destructor
}


eOSState cOSDImageMenu::ProcessKey(eKeys key)
{
	eOSState state = cOsdMenu::ProcessKey(key);
	char buf[256];
	
	
	if (!validRootDir)
	{
		if (key != kNone)
		{
			return osEnd;
		}
	}
	
	switch (state)
	{
		case osUnknown:
			switch (key)
			{
				case kRed:
					// generate a Pic-list for the current dir (including pics of subdirs)

					if (myPics->IsRoot() ) // don't forget the [..]-entry
					{
						if (myPics->Size() < 1)
						{
							SetStatus(tr("No pictures found") );
							break;
						}
					}
					else
					{
						if (myPics->Size() < 2)
						{
							SetStatus(tr("No pictures found") );
							break;
						}
					}
					
					ScanDir(allPics);
					CloseSubMenu();
					showMenu = false;
					return (osPlugin);
					break;
				case kBlue:
					return (osEnd);
					break;
				case kOk:
					if ( (unsigned int)Current() < myPics->nDirs)
					{
						//if ( (Current() == 0) && not(myPics->IsRoot() ) )
						if (myPics->files[Current()] == DIR_UP)
						{
							// [..] -> one dir up and set focus on the dir
							// Todo:
							
							strcpy(buf, myPics->CurDir() );
							myPics->PopDir();
							
							ScanDir(browsing);
							View(buf);
						}
						else
						{
							// decent into dir
							
							myPics->PushDir(myPics->files[Current()] );
							
							ScanDir(browsing);
							View();
						}
						
						break;
					}
					else
					{
						//generate Pic-List for the current dir (showing the current pic)

						myPics->curPic = Current() - myPics->nDirs;

						ScanDir(currentPics);
						CloseSubMenu();
						showMenu = false;
						return (osPlugin);
						break;
					}
				case kBack:
					break;
				case kNone:
					break;
				default:
					break;
			}
			break;
		case osBack:
			break;
		case osEnd:
			return (osEnd);
			break;
		default:
			break;
	}
	return (state);
}


bool cOSDImageMenu::ScanDir(eScanMode scan)
{
	char *cmd = NULL;
	char *line = NULL;
	FILE *p;
	char buf[256];
	bool ret;
#ifdef DEBUG
	int time = time_ms(), dt;
#endif
				
	
	ret = true;
	
	myPics->files.clear();

	if (scan == browsing)
	{
		// Scan the directories
#ifdef DEBUG
		printf("Generating Browse-List for '%s'\n", myPics->CurPath() );
#endif

		// Add [..] for returning to parent dir

		if ( !(myPics->IsRoot() ) )
		{
			myPics->files.push_back(DIR_UP);
		}


		// Scan the dirs

		asprintf(&cmd, FIND_DIRS, myPics->CurPath() );
		p = popen(cmd, "r");

		if (p)
		{
			while ( (line = readline(p) ) !=  NULL)
			{
				if (strcmp(line, myPics->CurPath() ) > 0)
				{
					strcpy(buf, line + strlen(myPics->CurPath() ) + 1);
					myPics->files.push_back(buf);
				}
			}

			pclose(p);
			free(line);
			free(cmd);
		}
		else
		{
			printf("Error while opening pipe!\n");
			ret = false;
		}

		myPics->nDirs = myPics->Size();
	}


	// Scan the files

	if ( (scan == browsing) || (scan == currentPics) )
	{
		// Scan only the current dir
#ifdef DEBUG
		if (scan == currentPics) printf("Generating Pic-List for '%s' (without subdirs)\n", myPics->CurPath() );
#endif
		asprintf(&cmd, FIND_FILES, myPics->CurPath(), supportedFormats.c_str() );
	}
	else // scan = allPics
	{
		// Scan the dir including the subdirs
#ifdef DEBUG
		printf("Generating Pic-List for '%s' (including subdirs)\n", myPics->CurPath() );
#endif
		asprintf(&cmd, FIND_PICS, myPics->CurPath(), supportedFormats.c_str() );
	}

	p = popen(cmd, "r");

	if (p)
	{
		while ( (line = readline(p) ) !=  NULL)
		{
			strcpy(buf, line + strlen(myPics->CurPath() ) + 1);
			myPics->files.push_back(buf);
		}

		pclose(p);
		free(line);
		free(cmd);
	}
	else
	{
		printf("Error while opening pipe!\n");
		ret = false;
	}

#ifdef DEBUG
	dt = time_ms() - time;
	printf("Generating file-list & sorting it: %dms\n", dt);
#endif

	if (myPics->Size() == 0)
	{
		printf("No pictures found or bad directory\n");
		ret = false;
	}
	
	return ret;
}


void cOSDImageMenu::View(char *selectItem)
{
	cOsdItem *item;
	
	
	Clear();

	for (unsigned int i = 0; i < myPics->Size(); i++)
	{
		if (i < myPics->nDirs)
		{
			Add(item = new cOsdItem( ("[" + myPics->files[i] + "]").c_str() ) );
		}
		else
		{
			Add(item = new cOsdItem(myPics->files[i].c_str() ) );
		}
		
		if ( selectItem && (myPics->files[i] == selectItem) )
		{
			SetCurrent(item);
		}
	}

	Display();
}


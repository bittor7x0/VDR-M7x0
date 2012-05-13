/*
 * filebrowserdemo.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>

/*
 * Includes needed to use filebrowser
 * menu-filebrowser: The menu itself
 * statebag: some parameters for the menu
 * commands: definition of commands
 */
#include "filebrowserlib/menu-filebrowser.h"
#include "filebrowserlib/statebag.h"
#include "filebrowserlib/commands.h"
#include "filebrowserdemo-customcommand.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Enter description for 'filebrowserdemo' plugin";
static const char *MAINMENUENTRY  = "Filebrowserdemo";

class cPluginFilebrowserdemo : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginFilebrowserdemo(void);
  virtual ~cPluginFilebrowserdemo();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  };

cPluginFilebrowserdemo::cPluginFilebrowserdemo(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginFilebrowserdemo::~cPluginFilebrowserdemo()
{
  // Clean up after yourself!
}

cOsdObject *cPluginFilebrowserdemo::MainMenuAction(void)
{
  cFilebrowserStatebag* Statebag=new cFilebrowserStatebag();
  //Set some flags
  Statebag->ShowHiddenFiles=true;
  //Set Base Directory - that is important
  strcpy(Statebag->BaseDir, "/");
  /*
   * Statebag holds some parameters for the menu
   * and also Commands - so you have to fill at least
   * this list
   * Take a look at statebag.h - Filter and ShowFullPath might be interesting
   */
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cCustomCommand()));
  /* FilebrowserCommandContainer is simply a class that 
   * handles cList-related functions without cluttering
   * the classes themselves
   * */

  /* Now you could add some other custom commands, and, for example, the Sources-Command
   * (needs command-sources.[ch]) to be able to switch sources like mplayer & co
   * or Mark/Unmark-Commands if you like your user to pass several files to your plugin
   * This commands fill Statebag->SelectedFiles
   */
  
  return new cOsdMenuFilebrowser("/", Statebag);
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

VDRPLUGINCREATOR(cPluginFilebrowserdemo); // Don't touch this!

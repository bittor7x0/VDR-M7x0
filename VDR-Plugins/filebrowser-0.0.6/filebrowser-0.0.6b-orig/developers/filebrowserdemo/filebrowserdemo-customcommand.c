#include "filebrowserdemo-customcommand.h"
#include <vdr/osdbase.h>

cCustomCommand::cCustomCommand() : cFilebrowserCommand(NULL)
/* Statebag isn't needed - but will want to pass it to 
 * the ctor if you need access to marked files etc. */
{
  /* Set a name */
  Name=strdup("Do sth.");
  /* Set some Use* and other Flags (see commands.h) */
  Confirm=true;
}

bool cCustomCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
/*
 * Now you can do just anything you like - DestinationFile will be NULL if you didn't
 * set appropriate Use* Flags in your constructor*/
/* Another approach would be to set State in your ctor, save the filename(s) here somewhere
 * and react to the state given in your cOsdMenu that is he parent of the filebrowser menu
 * in ProcessKey
 */
  Menu->Ins(new cOsdItem("hello world"),true);
  Menu->Display();
  return true;
}

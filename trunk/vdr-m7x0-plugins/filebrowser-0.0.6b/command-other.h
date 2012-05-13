//
// C++ Interface: command-other.h
//
// Description:
//
//
// Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILEBROWSER_COMMAND_OTHER_H
#define FILEBROWSER_COMMAND_OTHER_H

#include <vdr/osdbase.h>
#include "commands.h"
#include "statebag.h"

/**
@author Holger Brunn
*/

class cFilebrowserCommandOtherCommands : public cFilebrowserCommand
{
  protected:
    cFilebrowserCommands* Commands;
  public:
    cFilebrowserCommandOtherCommands(cFilebrowserStatebag* Statebag);
    ~cFilebrowserCommandOtherCommands();
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

class cOsdMenuOtherCommands : public cOsdMenu
{
  protected:
    cFilebrowserCommands* Commands;
    cFilebrowserCommands* RunCommands;
  public:
    cOsdMenuOtherCommands(cFilebrowserCommands* Commands, char* CurrentFile, cFilebrowserCommands* RunCommands);
    ~cOsdMenuOtherCommands();
    eOSState ProcessKey(eKeys Key);
};

#endif

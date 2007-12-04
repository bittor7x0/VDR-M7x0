//
// C++ Interface: commands-plugins
//
// Description: 
//
//
// Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILEBROWSER_COMMANDS_PLUGINS_H
#define FILEBROWSER_COMMANDS_PLUGINS_H

#include <vdr/tools.h>
#include "commands.h"

/**
	@author Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>
*/
class cFilebrowserCommandAnyPlugin : public cFilebrowserCommand
{
  protected:
    cString Extensions;
    cString ServiceID;
    bool ServiceAvailable;
  public:
    cFilebrowserCommandAnyPlugin(const char* Name);
    bool Matches(const char* Filename);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
};

struct MPlayerServiceData {
  int result;
  union {
    const char *filename;
  } data;
};

class cFilebrowserCommandMPlayerPlugin : public cFilebrowserCommandAnyPlugin
{
  public:
    cFilebrowserCommandMPlayerPlugin();
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

struct MP3ServiceData {
  int result;
  union {
    const char *filename;
  } data;
};

class cFilebrowserCommandMP3Plugin : public cFilebrowserCommandAnyPlugin
{
  public:
    cFilebrowserCommandMP3Plugin();
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename);
};

class cFilebrowserCommandImagePlugin : public cFilebrowserCommandAnyPlugin
{
  public:
    cFilebrowserCommandImagePlugin();
};

#endif

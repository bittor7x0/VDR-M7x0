//
// C++ Interface: sources
//
// Description:
//
//
// Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILEBROWSER_COMMAND_SOURCES_H
#define FILEBROWSER_COMMAND_SOURCES_H

#include <vdr/osdbase.h>
#include <vdr/tools.h>
#include "statebag.h"
#include "commands.h"

class cFilebrowserCommandSourcesList : public cFilebrowserCommand
{
  public:
    class cSource : public cListObject
    {
      public:
        cSource(char* Line);
        cString Name;
        cString Path;
        cString Filter;
        void SwitchTo(cFilebrowserStatebag* Statebag);
    };
    class cSourcesList : public cList<cSource> {};
  protected:
    cSourcesList* Sources;
  public:
    cFilebrowserCommandSourcesList(cFilebrowserStatebag* Statebag);
    ~cFilebrowserCommandSourcesList();
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* SelectedFile);
    bool Matches(const char* Filename);
};

class cOsdMenuSourcesList : public cOsdMenu
{
  protected:
    cFilebrowserStatebag* Statebag;
    cFilebrowserCommandSourcesList::cSourcesList* Sources;
  
  public:
    cOsdMenuSourcesList(cFilebrowserStatebag* Statebag, cFilebrowserCommandSourcesList::cSourcesList* Sources);

    ~cOsdMenuSourcesList();

};

class cOsdItemSource : public cOsdItem
{
  protected:
    cFilebrowserCommandSourcesList::cSource* Source;
    cFilebrowserStatebag* Statebag;

  public:
    cOsdItemSource(cFilebrowserStatebag* Statebag, cFilebrowserCommandSourcesList::cSource* Source);
    eOSState ProcessKey(eKeys Key);
};

#endif

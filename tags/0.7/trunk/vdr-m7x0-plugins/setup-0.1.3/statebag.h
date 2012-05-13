/***************************************************************************
                          statebag.h  -  description
                             -------------------
    begin                : Di Jan 25 2005
    copyright            : (C) 2005 by Holger Brunn
    email                : holger.brunn@stud.uni-karlsruhe.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILEBROWSER_STATEBAG_H
#define FILEBROWSER_STATEBAG_H

#include <vdr/tools.h>
#include <vdr/plugin.h>

//Forward declarations for used classes
class cStringList;
#ifdef FILEBROWSER_PLUGIN_BUILD
class cThreadList;
#endif
class cFilebrowserCommands;

class cFilebrowserStatebag
{
  protected:
    cStringList* SelectedFiles;
#ifdef FILEBROWSER_PLUGIN_BUILD
    cThreadList* Threads;
#endif
    cFilebrowserCommands* Commands;

  public:
    cFilebrowserStatebag();
    ~cFilebrowserStatebag();

    cStringList* GetSelectedFiles() { return SelectedFiles; };
#ifdef FILEBROWSER_PLUGIN_BUILD
    cThreadList* GetThreads() { return Threads; };
#endif
    cFilebrowserCommands* GetCommands() { return Commands; };

    char BaseDir[_POSIX_PATH_MAX];
    bool ShowHiddenFiles;
    bool ShowFullPath;
    
    cString Filter;
    cString CurrentDirectory;
    cString ConfigDirectory;
    cStringList* CurrentFiles;

#ifdef FILEBROWSER_PLUGIN_BUILD
    /*
    Is an update requested by some thread?
    */
    bool UpdateRequested;

    cPlugin* FilebrowserPlugin;
#endif
};

#endif

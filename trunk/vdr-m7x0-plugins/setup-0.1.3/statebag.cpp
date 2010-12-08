/***************************************************************************
                          statebag.c  -  description
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

#include "statebag.h"
#include "tools.h"
#include "commands.h"
#ifdef FILEBROWSER_PLUGIN_BUILD
#include "threads.h"
#endif

cFilebrowserStatebag::cFilebrowserStatebag()
{
#ifdef FILEBROWSER_PLUGIN_BUILD
  Threads=new cThreadList();
#endif
  SelectedFiles=new cStringList();
  Commands=new cFilebrowserCommands();
  BaseDir[0]='\0';
  CurrentFiles=new cStringList();
}

cFilebrowserStatebag::~cFilebrowserStatebag()
{
  //this also frees objects
#ifdef FILEBROWSER_PLUGIN_BUILD
  if(Threads) delete Threads;
#endif
  if(SelectedFiles) delete SelectedFiles;
  if(Commands) delete Commands;
  if(CurrentFiles) delete CurrentFiles;
}
 

/***************************************************************************
                          menu-threads.h  -  description
                             -------------------
    begin                : Do Jan 27 2005
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

#ifndef FILEBROWSER_MENU_THREADS_H
#define FILEBROWSER_MENU_THREADS_H
 
#include <vdr/osdbase.h>
#include <vdr/status.h>
#include "statebag.h"
#include "threads.h"

class cOsdMenuThreadList : public cOsdMenu, public cStatus
{
  protected:
    cFilebrowserStatebag* Statebag;
  public:
    cOsdMenuThreadList(cFilebrowserStatebag* Statebag);
    eOSState ProcessKey(eKeys Key);
    void UpdateHelp();
    void UpdateMenuItem(cOsdItem *Item, cCommandThread *Thread);
    void OsdCurrentItem(const char *Text);
};

#endif

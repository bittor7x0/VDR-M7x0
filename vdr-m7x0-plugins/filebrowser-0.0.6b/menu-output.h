/***************************************************************************
                          menu-output.h  -  description
                             -------------------
    begin                : So Jan 16 2005
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
 
#ifndef FILEBROWSER_OUTPUT_H
#define FILEBROWSER_OUTPUT_H

#include <vdr/menu.h>

//Forwarddeclaration for cCommandThreadTextOutput
class cCommandThreadTextOutput;

class cOsdMenuTextOutput : public cMenuText
{
  friend class cConfigCommandThread;

  protected:
    cCommandThreadTextOutput* Thread;
    bool Update;

    void UpdateHelp();

  public:
    cOsdMenuTextOutput(cCommandThreadTextOutput* Thread);
    ~cOsdMenuTextOutput();
    virtual void Display(void);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif

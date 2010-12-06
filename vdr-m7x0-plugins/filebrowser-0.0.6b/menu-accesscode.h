//
// C++ Interface: menu-accesscode
//
// Description: 
//
//
// Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILEBROWSER_ACCESSCODE_H

#define FILEBROWSER_ACCESSCODE_H

#include <vdr/osdbase.h>

/**
	@author Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>
*/
class cOsdMenuAccessCode : public cOsdMenu
{
  protected:
    char* Code;
    char* CodeBuffer;
    bool CheckCode();
  public:
    cOsdMenuAccessCode(char* Code);
    eOSState ProcessKey(eKeys Key);
    ~cOsdMenuAccessCode();
};

#endif

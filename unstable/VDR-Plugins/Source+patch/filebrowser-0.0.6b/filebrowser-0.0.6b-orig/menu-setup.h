//
// C++ Interface: menu-setup
//
// Description:
//
//
// Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILEBROWSER_SETUP_H
#define FILEBROWSER_SETUP_H

#include <vdr/menuitems.h>
#include "statebag.h"

#define SETUP_PARAM_BASEDIR "BaseDir"
#define SETUP_PARAM_FILTER "Filter"
#define SETUP_PARAM_SHOW_HIDDEN_FILES "ShowHiddenFiles"
#define SETUP_PARAM_SHOW_FULL_PATH "ShowFullPath"

class cFilebrowserSetupPage : public cMenuSetupPage
{
  protected:
    int NewShowHiddenFiles;
    int NewShowFullPath;
    cFilebrowserStatebag* Statebag;
  
  public:
    cFilebrowserSetupPage(cFilebrowserStatebag* Statebag);
    ~cFilebrowserSetupPage();

    virtual void Store(void);
};

#endif

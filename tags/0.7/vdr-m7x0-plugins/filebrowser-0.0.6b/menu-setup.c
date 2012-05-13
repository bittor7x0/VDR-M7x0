/*
*  C Implementation: menu-setup
*
* Description: 
*
*
* Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2005
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include <strings.h>
#include "menu-setup.h"
#define MAX(a,b) ((a) > (b) ? (a) : (b))

cFilebrowserSetupPage::cFilebrowserSetupPage(cFilebrowserStatebag* Statebag) : cMenuSetupPage()
{
  this->Statebag=Statebag;
  NewShowHiddenFiles=Statebag->ShowHiddenFiles;
  NewShowFullPath=Statebag->ShowFullPath;
  Add(new cMenuEditBoolItem(tr("Show hidden files"), &NewShowHiddenFiles));
  Add(new cMenuEditBoolItem(tr("Show full path in title"), &NewShowFullPath));
}

cFilebrowserSetupPage::~cFilebrowserSetupPage()
{
}

void cFilebrowserSetupPage::Store()
{
  SetupStore(SETUP_PARAM_SHOW_HIDDEN_FILES, Statebag->ShowHiddenFiles = NewShowHiddenFiles);
  SetupStore(SETUP_PARAM_SHOW_FULL_PATH, Statebag->ShowFullPath = NewShowFullPath);
}


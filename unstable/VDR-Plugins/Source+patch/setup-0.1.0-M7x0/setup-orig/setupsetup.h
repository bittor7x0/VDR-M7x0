/****************************************************************************
 * DESCRIPTION:
 *             Setup Dialog
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2005 by Ralf Dotzert
 ****************************************************************************/


#ifndef SETUPSETUP_H
#define SETUPSETUP_H

#include <vdr/plugin.h>
#include "common.h"

/**
@author Ralf Dotzert
*/
//*****************************************************************************
// Setup Configuration
//*****************************************************************************

class cSetupSetup{
public:
  char *_allowedChar;
  char *_strMenuSuffix;
  char *_strEntryPrefix;
  char _menuSuffix[20];
  char _entryPrefix[2];
  
  cSetupSetup();
  bool SetupParse( const char * Name, const char * Value );
};

extern cSetupSetup setupSetup;

//*****************************************************************************
// Setup Page
//*****************************************************************************
class cSetupSetupPage : public cMenuSetupPage
{
  public:
    cSetupSetupPage();
    void Store( void );
    eOSState ProcessKey( eKeys Key );
};

#endif

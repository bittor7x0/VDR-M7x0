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


#include <vdr/menu.h>
//#include <vdr/submenu.h>
#include <string.h>

#include "setupsetup.h"


//*****************************************************************************
// Setup Configuration
//*****************************************************************************

cSetupSetup::cSetupSetup()
{
  // Set Default Values
  _allowedChar     = " .,:;+-*~?!\"$%&/\()=?`')_abcdefghijklmnopqrstuvwxyz0123456789";
  _strMenuSuffix   = "Menu Suffix";
  _strEntryPrefix  = "Entry Prefix";
   strcpy(_menuSuffix, " ...");
   strcpy(_entryPrefix, "-");
}

bool cSetupSetup::SetupParse( const char * Name, const char * Value )
{
  if      (strcmp(Name, _strMenuSuffix )    == 0) snprintf(_menuSuffix, sizeof(_menuSuffix), Value);
  else if (strcmp(Name, _strEntryPrefix )   == 0) snprintf(_entryPrefix, sizeof(_entryPrefix), Value);
  else return false;

  return true;
}

//*****************************************************************************
// Setup Page
//*****************************************************************************
cSetupSetupPage::cSetupSetupPage( )
{
  Add(new cMenuEditStrItem(tr(setupSetup._strMenuSuffix),    setupSetup._menuSuffix, sizeof(setupSetup._menuSuffix), setupSetup._allowedChar));
  Add(new cMenuEditStrItem(tr(setupSetup._strEntryPrefix),   setupSetup._entryPrefix, sizeof(setupSetup._entryPrefix), setupSetup._allowedChar));
}


void cSetupSetupPage::Store( void )
{
  SetupStore(setupSetup._strMenuSuffix,      setupSetup._menuSuffix);
  SetupStore(setupSetup._strEntryPrefix,     setupSetup._entryPrefix);
}

eOSState cSetupSetupPage::ProcessKey( eKeys Key )
{
//  cSubMenu vdrSubMenu;
  char *menuXML = NULL;
  eOSState state = cOsdMenu::ProcessKey(Key);

  switch(Key)
  {
    case kOk :
              // Load Menu Configuration
/**
			  asprintf(&menuXML, "%s/setup/vdr-menu.xml", cPlugin::ConfigDirectory());
              if(vdrSubMenu.LoadXml(menuXML))
              {
                vdrSubMenu.SetMenuSuffix(setupSetup._menuSuffix);
                vdrSubMenu.SaveXml();
              }
              free(menuXML);
*/
              Store();
		  break;

    default :
      break;
  }
  return state;

}



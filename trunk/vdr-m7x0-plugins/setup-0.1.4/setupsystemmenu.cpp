/***********************************************************
 * Header file holding Class Definitions from <VDR>/menu.c *
 *                                                         *
 * This file is generated automaticly!                     *
 * Do not make changes!                                    *
 *                                                         *
 ***********************************************************/

#include "setupsystemmenu.h"
#include "setupmenu.h"
#include "common.h"

class cMenuSetupBase : public cMenuSetupPage {
protected:
  cSetup data;
  virtual void Store(void);
public:
  cMenuSetupBase(void);
  };

//############################################################################
// cSetupSystemMenu
//############################################################################
cSetupSystemMenu::cSetupSystemMenu()
{
}
cSetupSystemMenu::~cSetupSystemMenu()
{
}
// Get SystemMenu
cOsdMenu *cSetupSystemMenu::GetSystemMenu(const char *sysCommand, Config  *config)
{
 cOsdMenu *menu=NULL;
 if(sysCommand != NULL)
 {
    if(strcmp(sysCommand, "ActPlugins") == 0)
        menu =new cSetupPluginMenu(config);
    else
      error("<menu System=%s> not known", sysCommand);
  }
  return (menu);
}

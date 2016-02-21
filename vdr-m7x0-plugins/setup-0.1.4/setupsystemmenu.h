/*********************************************************
 * DESCRIPTION: 
 *             Header File
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de        
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 *********************************************************/

#ifndef SETUPSYSTEMMENU_H
#define SETUPSYSTEMMENU_H

#include "config.h"
#include "common.h"

class cSetupSystemMenu
{
public:
  cSetupSystemMenu();
  ~cSetupSystemMenu();
  static cOsdMenu *GetSystemMenu(const char *sysCommand, Config  *config);
};

#endif

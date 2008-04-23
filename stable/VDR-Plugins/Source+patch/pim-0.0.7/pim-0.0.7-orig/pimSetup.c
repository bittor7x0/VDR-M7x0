/*
 * $Id: pimSetup.c,v 1.1.1.1 2005/02/17 05:45:53 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: pimSetup.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:53  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.3  2004/12/13 07:58:12  ksltuf
 * new setup option 'firstIsMonday'
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pimSetup.h"

cPimSetup globalSetup;

cPimSetup::cPimSetup(void)
{
  osdOffsetX = 0;
  osdOffsetY = 0;
  reminderMode = 0;
  activityMode = 0;
  firstIsMonday = true;
  showCW = false;
  previewLastNextW = false;
  inversEventColor = false;
  reminderTime = 0;
  remindDelay = 0;
  ConfigDir = NULL;
}

cPimSetup::~cPimSetup(void)
{
  if (ConfigDir) {
    free(ConfigDir);
    ConfigDir = NULL;
  }
}


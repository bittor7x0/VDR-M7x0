/*
 * $Id: config.c,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

cSetupTaste SetupTaste;

cSetupTaste::cSetupTaste(void):
	HideMenuEntry(0),
	MessageTimeout(2)
{
}

bool cSetupTaste::Parse(const char *Name, const char *Value)
{
  if (strcmp(Name, "HideMenuEntry")  == 0) HideMenuEntry  = atoi(Value);
  else if (strcmp(Name, "MessageTimeout") == 0) MessageTimeout = atoi(Value);
  else return false;
  return true;
}

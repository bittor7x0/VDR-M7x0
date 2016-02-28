/***************************************************************************
                          tools.c  -  description
                             -------------------
    begin                : Sa Jan 15 2005
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

#include <malloc.h>
#include <ctype.h>
#include "tools.h"

/*
  Implementation cStringList
*/

bool cStringList::Contains(const char* String)
{
  for(cStringContainer *i = First(); i; i=Next(i))
  {
    if(strcmp(i->GetObject(), String)==0)
    {
      return true;
    }
  }
  return false;
}

void cStringList::Remove(const char* String)
{
  for(cStringContainer *i = First(); i!=NULL; i=Next(i))
  {
      if(strcmp(i->GetObject(), String)==0)
      {
        Del(i);
        break;
      }
  }
}

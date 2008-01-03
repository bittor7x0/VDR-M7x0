/*
 * $Id: calEventMenuItem.c,v 1.1.1.1 2005/02/17 05:45:51 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calEventMenuItem.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:51  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include "calEventMenuItem.h"


cCalEventMenuItem::cCalEventMenuItem(const char *text,int id) :
  _text(text), _id(id)
{
  char *buffer = NULL;
  asprintf (&buffer, "%s", _text);

  SetText(buffer, false);
}


cCalEventMenuItem::~cCalEventMenuItem()
{

}



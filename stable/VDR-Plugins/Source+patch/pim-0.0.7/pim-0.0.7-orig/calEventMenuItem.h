/*
 * $Id: calEventMenuItem.h,v 1.1.1.1 2005/02/17 05:45:53 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calEventMenuItem.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:53  sundin
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


#ifndef __CALEVENTMENUITEM_H_
#define __CALEVENTMENUITEM_H_

#include <vdr/osdbase.h>
#include <vdr/osd.h>

class cCalEventMenuItem : public cOsdItem
{

 public:

  cCalEventMenuItem(const char*, int);
  ~cCalEventMenuItem();

  int getId() { return _id; }

 private:

  const char* _text;
  int   _id;
};

#endif


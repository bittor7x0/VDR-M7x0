/*
 * $Id: calEvents.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calEvents.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.3  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#ifndef __CALEVENTS_H_
#define __CALEVENTS_H_

#include "calEvent.h"

#include <vdr/config.h>

class cCalEvents : public cConfig<cCalEvent>
{


};


#endif

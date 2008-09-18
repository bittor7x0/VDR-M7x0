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

#ifndef DEBUG_H
#define DEBUG_H
#include "vdr/plugin.h"

#ifdef  DEBUG_STDOUT
#define DEBUG1(p1)                      printf(p1)
#define DEBUG2(p1, p2)                  printf(p1, p2)
#define DEBUG3(p1, p2, p3)              printf(p1, p2, p3)
#define DEBUG4(p1, p2, p3, p4)          printf(p1, p2, p3, p4)
#define DEBUG5(p1, p2, p3, p4, p5)      printf(p1, p2, p3, p4, p5)
#define DEBUG6(p1, p2, p3, p4, p5, p6)  printf(p1, p2, p3, p4, p5, p6)
#define DEBUG7(p1, p2, p3, p4, p5, p6, p7)  printf(p1, p2, p3, p4, p5, p6, p7)
#else
#define DEBUG1(p1)                      esyslog(p1)
#define DEBUG2(p1, p2)                  esyslog(p1, p2)
#define DEBUG3(p1, p2, p3)              esyslog(p1, p2, p3)
#define DEBUG4(p1, p2, p3, p4)          esyslog(p1, p2, p3, p4)
#define DEBUG5(p1, p2, p3, p4, p5)      esyslog(p1, p2, p3, p4, p5)
#define DEBUG6(p1, p2, p3, p4, p5, p6)  esyslog(p1, p2, p3, p4, p5, p6)
#define DEBUG7(p1, p2, p3, p4, p5, p6, p7)  esyslog(p1, p2, p3, p4, p5, p6, p7)

#endif
#define DBG_PREFIX                      "ERROR: open7x0-Firmware Setup"

#endif /* DEBUG_H */

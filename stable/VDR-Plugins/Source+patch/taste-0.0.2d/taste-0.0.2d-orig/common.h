/*
 * $Id$
 */

#ifndef VDR_TASTE_COMMON_H
#define VDR_TASTE_COMMON_H

#include <vdr/tools.h>

#if VDRVERSNUM >= 10318
#	define time_ms() cTimeMs::Now()
#endif

#endif // VDR_TASTE_COMMON_H

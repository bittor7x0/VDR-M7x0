/*
 * osdadjust.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */


#ifndef _OSDADJUST_I18N__H
#define _OSDADJUST_I18N__H

#include <vdr/i18n.h>

#if VDRVERSNUM < 10507

extern const tI18nPhrase Phrases[];

#ifndef trNOOP
#  define trNOOP(s) (s)
#endif

#ifndef trVDR
#  define trVDR(s) tr(s)
#endif

#endif // VDRVERSNUM < 10507

#endif //_OSDADJUST_I18N_H

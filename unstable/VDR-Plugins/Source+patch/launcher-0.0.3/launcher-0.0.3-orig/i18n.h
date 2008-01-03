/*
 * autotimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.h 1.0 2004/04/05 13:36:00 hflor Exp $
 */

#ifndef _I18N__H
#define _I18N__H

#include <vdr/i18n.h>

#if APIVERSNUM < 10507
#define trNOOP(s) (s)
extern const tI18nPhrase Phrases[];
#endif

#if APIVERSNUM < 10509
#define trVDR(s) tr(s)
#endif

#endif //_I18N__H

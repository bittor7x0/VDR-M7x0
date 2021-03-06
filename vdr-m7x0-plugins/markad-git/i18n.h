/*
 * i18n.h: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.h 1.2 2002/05/11 14:48:16 kls Exp $
 */

#ifndef _I18N__H
#define _I18N__H

#include <vdr/i18n.h>

#if APIVERSNUM < 10507
extern const tI18nPhrase Phrases[];
#define trNOOP(s) (s)
#endif

#if APIVERSNUM < 10509
#define trVDR(s) tr(s)
#endif

#endif //_I18N__H

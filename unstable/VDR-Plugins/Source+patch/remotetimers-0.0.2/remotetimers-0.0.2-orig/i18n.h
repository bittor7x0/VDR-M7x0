/*
 * i18n.h: translations
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _REMOTETIMERS_I18N__H
#define _REMOTETIMERS_I18N__H

#include <vdr/i18n.h>

#if VDRVERSNUM < 10507
#define trNOOP(s) (s)
#define trREMOTETIMERS(s) tr(s)
extern const tI18nPhrase Phrases[];
#else
#undef tr
#define tr(s) I18nTranslate(s)
#define trREMOTETIMERS(s) I18nTranslate(s, "vdr-" PLUGIN_NAME_I18N)
#endif

#endif //_REMOTETIMERS_I18N__H

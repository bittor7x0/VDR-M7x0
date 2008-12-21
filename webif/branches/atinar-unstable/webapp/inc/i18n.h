/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* (c) Christian Kelinski <k@kille.cx>
* Please checkout the README file!
* 
* Originally written for the open7x0.org VDR-FW project:
* www.open7x0.org
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __I18N_H__
#define __I18N_H__

#define I18NNUM 4

#include <time.h>

extern int langID;
extern char acceptedLang[3];

typedef char *tI18nPhrase[I18NNUM];
extern const tI18nPhrase i18n[];
extern const char wochentage[7][15];
extern const char weekdays[I18NNUM][7][15];
extern const char shortWeekdays[7][4];
extern const int startOfWeek[I18NNUM];

const char *tr(const char *s);
char * dateStr(time_t t);

#endif

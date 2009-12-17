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
#include "misc.h"

typedef const char * i18nVal_t[I18NNUM];

typedef struct i18n_s {
	const char * key;
	i18nVal_t val;
} i18n_t;

extern int langId;

extern const i18nVal_t locale;
extern const i18nVal_t alpha2;
extern const i18nVal_t alpha3;
extern const char * const weekdays[I18NNUM][7];
extern const char * const shortWeekdays[I18NNUM][7];
extern const char * const monthNames[I18NNUM][12];
extern const i18nVal_t dateFormat; //to use with strftime
extern const int startOfWeek[I18NNUM];

const char *tr(const char *s);
const char *formatDate(struct tm *timeptr,boolean_t addHour);
#endif

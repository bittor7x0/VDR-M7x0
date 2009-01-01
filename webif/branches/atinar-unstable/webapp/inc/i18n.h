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

extern const char * i18n[][I18NNUM];
extern const char * weekdays[I18NNUM][7];
extern const char * shortWeekdays[I18NNUM][7];
extern const char * dateFormat[I18NNUM]; //to use with strftime
extern const int startOfWeek[I18NNUM];

const char *tr(const char *s);
const char *formatDate(struct tm *timeptr);
#endif

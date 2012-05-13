/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_I18N_H
#define WI_I18N_H

typedef enum langId_e {
    I18N_BROWSER_SELECT = -1,
    I18N_MIN,
    I18N_EN = I18N_MIN,
    I18N_DE,
    I18N_ES,
    I18N_FR,
    I18N_MAX = I18N_FR,
    I18N_NUM
} langId_t;

#include <time.h>
#include "misc.h"

typedef const char * i18nVal_t[I18N_NUM];

typedef struct i18n_s {
    const char * key;
    i18nVal_t val;
} i18n_t;

extern langId_t langId;

extern const i18nVal_t locale;
extern const i18nVal_t alpha2;
extern const i18nVal_t alpha3;
extern const char * const weekdays[I18N_NUM][7];
extern const char * const shortWeekdays[I18N_NUM][7];
extern const char * const monthNames[I18N_NUM][12];
extern const i18nVal_t dateFormat; //to use with strftime
extern const int startOfWeek[I18N_NUM];

void i18nsort();
const char *tr(const char *s);
const char *formatDate(struct tm *timeptr, bool addHour);
#endif

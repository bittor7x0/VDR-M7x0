/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_SETTINGS_H
#define	WI_SETTINGS_H

#include <stdbool.h>

#include "conf.h"

#ifdef	__cplusplus
extern "C" {
#endif

    void readSettings(vars_t *rqargs, settingList_t *settings);

    bool saveSettingsAndSetCookies(wcontext_t *wctx, vars_t *vars, int hostId, settingList_t *settings);

    void printSettingsForm(wcontext_t *wctx, settingList_t *settings, bool onlyQueryHost);

    void printSettingsPage(wcontext_t *wctx);

    void printViewFilePage(wcontext_t *wctx);

#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */


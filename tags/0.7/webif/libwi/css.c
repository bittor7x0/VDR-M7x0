/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include "css.h"
#include "misc.h"

const char * const cssTheme[] = {"theme0", "theme1"};

void printApplyThemeJavaScript(wcontext_t *wctx, const char * fieldId, bool cookie) {
    openJavascriptTag(wctx);
    WCTX_IPRINTFT("$('#%s').on('change.webif',function(){\n", 0, 1, fieldId);
    WCTX_IPRINTF("webif.setCssTheme($(this).val(),%s);\n", (cookie) ? "true" : "false");
    WCTX_IPRINTT("});\n", -1, 0);
    closeJavascriptTag(wctx);

}

void printCssThemeSelector(wcontext_t *wctx, const char * id) {
    settingConfig_t *cfg = getSettingConfig(CF_WEBIFCONF, "css_theme");
    if (cfg) {
        char *v;
        crit_goto_if(asprintf(&v, "%d", wctx->conf.cssTheme) == -1, outOfMemory);
        cfg->printInput(wctx, cfg, id, NULL, "cssThemeSelector", -1, v);
        free(v);
    }
    return;
outOfMemory:
    exit(EXIT_FAILURE);
}
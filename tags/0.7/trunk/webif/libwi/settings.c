/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "i18n.h"
#include "conf.h"
#include "misc.h"
#include "settings.h"

/**
 * Helper function used by readSettings to parse args.
 * @param arg: request arg.
 * @param p: pointer to setting list to parse.
 * @return : 0 to continue parsing.
 */
int prepareSettings(var_t* arg, void *p) {
    settingList_t * settings = (settingList_t *) p;
    setting_t *setting;
    const char *argname = var_get_name(arg);
    char *unds = strchr(argname, '_');
    if (unds == NULL)
        return 0;
    bool isName = false;
    bool isValue = false;
    bool isOldValue = false;
    bool saveCookie = false;
    bool deleteCookie = false;
    errno = 0;
    int idx = strtol(unds + 1, NULL, 10);
    warn_return_sif(errno != 0, 0);
    warn_return_if(idx < 0, 0);
    int undsp = unds - argname;
    if (strncmp("paramName", argname, undsp) == 0) {
        isName = true;
    } else if (strncmp("paramValue", argname, undsp) == 0) {
        isValue = true;
    } else if (strncmp("oldParamValue", argname, undsp) == 0) {
        isOldValue = true;
    } else if (strncmp("saveCookie", argname, undsp) == 0) {
        saveCookie = true;
    } else if (strncmp("deleteCookie", argname, undsp) == 0) {
        deleteCookie = true;
    } else {
        return 0;
    }
    if (idx >= settings->length) {
        int len = idx + 1;
        settings->entry = realloc(settings->entry, len * sizeof (setting_t));
        crit_goto_if(settings->entry == NULL, outOfMemory);
        int j;
        for (j = settings->length; j < len; j++) {
            initSetting(settings->entry + j);
        }
        settings->length = len;
    }
    setting = settings->entry + idx;
    const char *argvalue = var_get_value(arg);
    if (saveCookie) {
        setting->saveCookie = (argvalue != NULL) && strncmp("true", argvalue, 4) == 0;
    } else if (deleteCookie) {
        setting->deleteCookie = (argvalue != NULL) && strncmp("true", argvalue, 4) == 0;
    } else {
        char *value = (argvalue) ? strdup(argvalue) : NULL;
        if (isName) {
            setting->name = value;
        } else if (isValue) {
            setting->value = value;
        } else if (isOldValue) {
            setting->oldValue = value;
        } else {
            warn("%s descartado", argname);
            free(value);
        }
    }
    return 0;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

/**
 * Read settings passed in request args.
 * @param vars: Request args.
 * @param settings: Settings to prepare.
 */
void readSettings(vars_t *vars, settingList_t *settings) {
    vars_foreach(vars, prepareSettings, settings);
}

/**
 * Read settings passed in request args and set cookies where defined.
 * @param wctx: request context.
 * @param vars: request args.
 * @param hostId: (optional) id of host to whom settings belong.
 * @param settings
 *	List of settings to prepare. 
 *	Should be inited but empty.
 *	Only fileId must be set.
 * @return true if all settings are validated.
 */
bool saveSettingsAndSetCookies(wcontext_t *wctx, vars_t *vars, int hostId, settingList_t *settings) {
    int i;
    setting_t *setting;
    bool allSettingsValid = true;
    readSettings(vars, settings);
    if (settings->length == 0) {
        WCTX_MESSAGE(MT_ALERT, fileMapping[settings->fileId].fileName, fileMapping[settings->fileId].fileName);
        return false;
    }
    for (i = 0, setting = settings->entry; i < settings->length; i++, setting++) {
        setting->isValid = true;
        if (!setting->config) {
            setting->config = getSettingConfig(settings->fileId, setting->name);
        }
        if (setting->config && setting->config->validate != NULL) {
            setting->config->validate(setting->config, setting);
        }
        if (!setting->isValid) {
            allSettingsValid = false;
        } else if (setting->saveCookie || setting->deleteCookie) {
            time_t expire = 0;
            const char * path = NULL;
            const char * domain = NULL;
            int secure = false;
            response_set_cookie(wctx->response, setting->name,
                    (setting->deleteCookie) ? NULL : setting->value, expire, path, domain, secure);
        }
    }
    if (allSettingsValid) {
        bool ok = writeConf(hostId, settings);
        if (ok) {
            WCTX_MESSAGE(MT_INFO, tr("setup"), tr("cfgFileSaveOk%s"), fileMapping[settings->fileId].fileName);
            if ((settings->fileId == CF_WEBIFCONF)) {
                webifConf.alreadySet = false; //TODO comprobar que es asi
            }
        } else {
            WCTX_MESSAGE(MT_ALERT, tr("setup"), tr("cfgFileSaveErr%s"), fileMapping[settings->fileId].fileName);
        }
        return ok;
    } else {
        return false;
    }
}

static int compareSettings(const void *pa, const void *pb) {
    const setting_t *sa = pa;
    const setting_t *sb = pb;
    const settingConfig_t *ca = sa->config;
    const settingConfig_t *cb = sb->config;
    if (ca != NULL || cb != NULL) {
        if (cb == NULL) return -1;
        if (ca == NULL) return 1;
        if (ca->displayOrder > 0 || cb->displayOrder > 0) {
            if (cb->displayOrder == 0) return -1;
            if (ca->displayOrder == 0) return 1;
            if (ca->displayOrder != cb->displayOrder) return ca->displayOrder - cb->displayOrder;
        }
    }
    return strcmp(sa->name, sb->name);
}

/**
 * Print an HTML form to modify settings.
 * 
 * @param wctx: Request context.
 * @param settings: List of settings to show.
 * @param onlyQueryHost: Only query which host to setup.
 */
void printSettingsForm(wcontext_t *wctx, settingList_t *settings, bool onlyQueryHost) {
    if (settings->fileId == CF_WEBIFCONF && webifConf.noChannelLogos) {
        WCTX_MESSAGE(MT_ALERT, tr("noLogosErrTitle"), tr("noLogosErrMessage"));
    }
    WCTX_IPRINTT("<form action=\"" URLWI "\" method=\"post\"", 0, 1);
    if (settings->fileId == CF_WEBIFCONF) {
        qsort(settings->entry, settings->length, sizeof (setting_t), &compareSettings);
        WCTX_PRINT("enctype=\"multipart/form-data\"");
    }
    WCTX_PRINT(">\n");
    WCTX_IPRINTT("<fieldset class=\"hidden\">", 0, 1);
    WCTX_IPRINTF("<input type=\"hidden\" name=\"oldHostId\" value=\"%d\" />\n", settings->hostId);
    WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\" />\n", PN_SETTINGS);
    WCTX_IPRINTF("<input type=\"hidden\" name=\"cfgFileId\" value=\"%d\" />\n", settings->fileId);
    WCTX_IPRINTT("</fieldset>", -1, 0);
    WCTX_IPRINTFT("<table id=\"cfgFile\" class=\"form1\">\n", 0, 1); //TODO fieldsets
    {
        WCTX_IPRINTT("<tfoot class=\"form1Foot\">\n", 0, 1);
        {
            WCTX_IPRINTT("<tr class=\"buttons\">\n", 0, 1);
            WCTX_IPRINTT("<td colspan=\"2\">\n", 0, 1);
            if (onlyQueryHost) {
                WCTX_IPRINTF(
                        "<button id=\"confirm\" class=\"confirm ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" >"
                        "<span class=\"ui-button-icon-primary ui-icon ui-icon-check\"></span>"
                        "<span class=\"ui-button-text\">%s</span>"
                        "</button>\n"
                        , PA_READ_CONFIG, tr("readConfig"));
            } else {
                if (webifConf.hosts.numVDRs > 1 && settings->fileId == CF_VDRCONF) {
                    WCTX_IPRINTT("<label>Host:\n", 0, 1);
                    printVDRSelect(wctx, "hostId", settings->hostId);
                    WCTX_IPRINTT("<label>\n", -1, 0);
                }
                WCTX_IPRINTF(
                        "<button id=\"confirm\" class=\"confirm ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" >"
                        "<span class=\"ui-button-icon-primary ui-icon ui-icon-check\"></span>"
                        "<span class=\"ui-button-text\">%s</span>"
                        "</button>\n"
                        , PA_SAVE_CONFIG, tr("saveConfig"));
            }
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("</tr>\n", -1, 0);
        }
        WCTX_IPRINTT("</tfoot>\n", -1, 0);
        WCTX_IPRINTT("<tbody class=\"form1Body\">\n", 0, 1);
        {
            const char *ParamValue = "paramValue";
            const char *OldParamValue = "oldParamValue";
            if (onlyQueryHost) {
                WCTX_IPRINTT("<tr class=\"param\">\n", 0, 1);
                WCTX_IPRINT("<th>Host</th>\n");
                WCTX_IPRINTT("<td>\n", 0, 1);
                printVDRSelect(wctx, "hostId", settings->hostId);
                WCTX_IPRINTT("</td>\n", -1, 0);
                WCTX_IPRINTT("</tr>\n", -1, 0);
            } else {
                int i;
                setting_t *setting;
                const char *Revert = tr("revert");
                const char *paramI18n;
                for (i = 0; i < settings->length; i++) {
                    const char * valueToShow;
                    setting = settings->entry + i;
                    settingConfig_t *cfg = setting->config = getSettingConfig(settings->fileId, setting->name);
                    //search by i18n key like "fileKey.paramName"
                    wctxBufferPrintf(wctx, "%s.%s", fileMapping[settings->fileId].i18nKey, setting->name);
                    paramI18n = tr(wctx->buffer);
                    if (paramI18n == wctx->buffer) paramI18n = setting->name;
                    WCTX_IPRINTT("<tr class=\"param\">\n", 0, 1);
                    WCTX_IPRINTF("<th><label title=\"%s\">%s</label><input type=\"hidden\" name=\"paramName_%d\" value=\"%s\" /></th>\n",
                            setting->name, paramI18n, i, setting->name);
                    WCTX_IPRINTT("<td>\n", 0, 1);
                    if (setting->saveCookie) {
                        valueToShow = setting->oldValue;
                    } else if (setting->isValid) {
                        valueToShow = setting->value;
                    } else {
                        message_t *msg = newMessage(MT_ALERT, NULL, tr("cfgErrInvalidParamValue%s"), setting->value);
                        printMessage(wctx, msg);
                        freeMessage(msg);
                        free(msg);
                        if (setting->oldValue) {
                            valueToShow = setting->oldValue;
                        } else if (cfg && cfg->defaultValue) {
                            valueToShow = cfg->defaultValue;
                        } else {
                            valueToShow = setting->value;
                        }
                    };
                    if (setting->comment) {
                        WCTX_IPRINTF("<div class=\"info\">%s</div>\n", wctxHtmlEncode(wctx, setting->comment));
                    }
                    if (!cfg || !cfg->printInput || !cfg->printInput(wctx, cfg, NULL, ParamValue, ParamValue, i, valueToShow)) {
                        settingPrintInputText(wctx, cfg, NULL, ParamValue, ParamValue, i, valueToShow);
                    }
                    printInput(wctx, "hidden", NULL, OldParamValue, OldParamValue, i, setting->value, 0);
                    WCTX_IPRINTF(
                            "<a class=\"revert ui-button ui-button-icon-only ui-state-default\" title=\"%s\">"
                            "<span class=\"ui-button-icon-primary ui-icon ui-icon-arrowreturnthick-1-w\"></span>"
                            "<span class=\"ui-button-text\">%s</span>"
                            "</a>"
                            , Revert, Revert);
                    if (cfg && cfg->cookie && cfg->setField) {
                        const char *CookieSave = tr("cookie.save");
                        WCTX_IPRINTFT("<label title=\"%s\">Cookie\n", 0, 1, CookieSave);
                        WCTX_IPRINTF("<input type=\"checkbox\" value=\"true\" title=\"%s\" name=\"saveCookie_%d\"/>\n", CookieSave, i);
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        const char *cookie = request_get_cookie(wctx->request, setting->name);
                        if (setting->saveCookie || setting->deleteCookie) {
                            //se va a establecer en la respuesta actual y el userAgent aun no la incluirá hasta la siguiente petición
                            //pero vamos a mostrar el valor que se asignara como si ya se hubiera hecho.
                            cookie = (setting->deleteCookie) ? NULL : setting->value;
                        }
                        if (cookie) {
                            const char *DeleteCookie = tr("cookie.delete");
                            WCTX_IPRINTFT("<label title=\"%s\">%s [%s]\n", 0, 1, DeleteCookie, tr("delete"), cookie);
                            WCTX_IPRINTF("<input type=\"checkbox\" value=\"true\" title=\"%s\" name=\"deleteCookie_%d\"/>\n", DeleteCookie, i);
                            WCTX_IPRINTT("</label>\n", -1, 0);
                        }
                    }
                    WCTX_IPRINTT("</td>\n", -1, 0);
                    WCTX_IPRINTT("</tr>\n", -1, 0);
                }
                if (settings->fileId == CF_WEBIFCONF) {
                    WCTX_IPRINTT("<tr>\n", 0, 1);
                    WCTX_IPRINTF("<th>%s</th>\n", tr("cfgWi.logos"));
                    WCTX_IPRINTT("<td>\n", 0, 1);
                    WCTX_IPRINT("<input id=\"logos_file\" type=\"file\" name=\"logos\"/>");
                    WCTX_IPRINTT("</td>\n", -1, 0);
                    WCTX_IPRINTT("</tr>\n", -1, 0);
                }
            }
        }
        WCTX_IPRINTT("</tbody>\n", -1, 0);
    }
    WCTX_IPRINTT("</table>\n", -1, 0);
    WCTX_IPRINTT("</form>\n", -1, 0);
}

static void printSettingsHtmlHeadExtra(wcontext_t *wctx, const char *title) {
    if (!wctx->isAjaxRequest) {
        printCssLinkTag(wctx, "settings.css", "screen");
        printJavascriptTag(wctx, "wi.settings.js");
        openJavascriptTag(wctx);
        WCTX_IPRINTF("$(function(){webif.settingsPageInit();});\n");
        closeJavascriptTag(wctx);
    }
}

void printSettingsPage(wcontext_t *wctx) {
    if (webifConf.configViewDisabled || webifConf.configChangeDisabled) {
        response_set_status(wctx->response, HTTP_STATUS_UNAUTHORIZED);
        return;
    }
    wctxBufferReserve(wctx, 1024);
    vars_t *rqargs = request_get_args(wctx->request);
    settingList_t settings;
    initSettingList(&settings);
    settings.fileId = (vars_countn(rqargs, "cfgFileId") > 0) ? vars_get_value_i(rqargs, "cfgFileId") : CF_WEBIFCONF;
    int hostId = vars_get_value_i(rqargs, "hostId");
    int oldHostId = vars_get_value_i(rqargs, "oldHostId");
    wctx->action = (vars_countn(rqargs, "a") > 0) ? vars_get_value_i(rqargs, "a") : PA_READ_CONFIG;
    bool queryHost = false;

    if (validateCnfFileId(settings.fileId, wctx)) {
        bool showForm = true;
        if (wctx->action == PA_SAVE_CONFIG) {
            settings.hostId = oldHostId;
            if (settings.fileId == CF_WEBIFCONF) {
                const char *logosFieldName = "logos";
                vars_t *uploads = request_get_uploads(wctx->request);
                if (vars_countn(uploads, logosFieldName) > 0) {
                    extractLogosFromRequest(wctx, logosFieldName);
                }
            }
            if (!saveSettingsAndSetCookies(wctx, rqargs, hostId, &settings)) {
                showForm = false;
            }
        } else { // read current conf
            settings.hostId = hostId;
            queryHost = (webifConf.hosts.numVDRs > 1 && settings.fileId == CF_VDRCONF && vars_countn(rqargs, "hostId") == 0);
            if (!queryHost) {
                if (!readConf(&settings)) {
                    if (fileMapping[settings.fileId].canCreateNew) {
                        WCTX_MESSAGE(MT_INFO, NULL, tr("cfgFileIsNew_%d_%s"), settings.fileId, fileMapping[settings.fileId].fileName);
                    } else {
                        showForm = false;
                        WCTX_MESSAGE(MT_ALERT, NULL, tr("cfgFileDoesNotExist_%s"), fileMapping[settings.fileId].fileName);
                    }
                }
            }
        }
        initHtmlPage(wctx, tr(fileMapping[settings.fileId].i18nKey), printSettingsHtmlHeadExtra, NULL); //must go after setting cookies
        if (showForm) {
            printSettingsForm(wctx, &settings, queryHost);
        }
        finishHtmlPage(wctx);
    } else {
        response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
    }
    freeSettings(&settings);
}

void printViewFilePage(wcontext_t *wctx) {
    if (webifConf.configViewDisabled) {
        response_set_status(wctx->response, HTTP_STATUS_UNAUTHORIZED);
        return;
    }
    vars_t *rqargs = request_get_args(wctx->request);
    int fileNum = (vars_countn(rqargs, "fileNum") > 0) ? vars_get_value_i(rqargs, "fileNum") : 0;
    if (wctx->action == PA_DOWNLOAD_ALL) {
        int i;
        int j = 3;
        char *tar_args[fileMappingLength + 4];
        tar_args[0] = "tar";
        tar_args[1] = "-czf";
        tar_args[2] = "/var/tmp/files.tar.gz";
        for (i = 0; i < fileMappingLength; i++) {
            if (fileExists(fileMapping[i].fileName)) {
                tar_args[j++] = (char*) fileMapping[i].fileName;
            }
        }
        tar_args[j] = NULL;
        if (j > 3) {
            pid_t pid = 0;
            pid = fork();
            if (pid == 0) {
                execvp("/bin/tar", tar_args);
            }
            if (pid > 0) {
                waitpid(pid, NULL, 0);
                response_set_content_type(wctx->response, "application/gzip");
                response_set_field(wctx->response, "Content-Disposition", "attachment; filename=\"files.tar.gz\"");
                FILE *f = fopen("/var/tmp/files.tar.gz", "rb");
                if (f) {
                    char buffer;
                    while (!feof(f)) {
                        buffer = fgetc(f);
                        io_putc(wctx->out, buffer);
                    }
                    fclose(f);
                }
            }
        } else {
            response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
        }
    } else if (fileNum >= 0 && fileNum < fileMappingLength) {
        if (wctx->action == PA_DOWNLOAD) {
            if (fileExists(fileMapping[fileNum].fileName)) {
                response_set_content_type(wctx->response, "application/octetstream");
                {
                    char *cd;
                    asprintf(&cd, "attachment; filename=\"%s.txt\"", fileMapping[fileNum].fileName);
                    response_set_field(wctx->response, "Content-Disposition", cd);
                    free(cd);
                }
                FILE *f = fopen(fileMapping[fileNum].fileName, "r");
                if (f) {
                    char buffer[1024];
                    while (fgets(buffer, 1023, f) != NULL) {
                        WCTX_PRINT(buffer);
                    }
                    fclose(f);
                }
            } else {
                response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
            }
        } else {
            initHtmlPage(wctx, tr("fileView"), NULL, NULL);
            if (fileExists(fileMapping[fileNum].fileName)) {
                const char *Download = tr("download");
                WCTX_IPRINTT("<div class=\"level3Div\">\n", 0, 1);
                WCTX_IPRINTT("<div class=\"level3Head\">\n", 0, 1);
                WCTX_IPRINTT("<ul class=\"controls\"><li class=\"control\">\n", 0, 1);
                WCTX_IPRINTF(
                        "<a class=\"ui-button ui-button-icon-only ui-state-default\" href=\"" URLWI_PA(AMP) AMP "fileNum=%d\" title=\"%s\">"
                        "<span class=\"ui-button-icon-primary ui-icon ui-icon-disk\"></span>"
                        "<span class=\"ui-button-text\">%s</span>"
                        "</a>\n"
                        , PN_FILES, PA_DOWNLOAD, fileNum, Download, Download);
                WCTX_IPRINTT("</li></ul>\n", -1, 0);
                WCTX_IPRINTF("%s\n", fileMapping[fileNum].fileName);
                WCTX_IPRINTT("</div>" HTMLCOMENT("level3Head") "\n", -1, 0);
                WCTX_IPRINTT("<div class=\"level3\">\n", 0, 1);
                WCTX_IPRINTT("<pre>\n", 0, 1);

                FILE *f = fopen(fileMapping[fileNum].fileName, "r");
                if (f) {
                    wctxBufferReserve(wctx, 4096);
                    char buffer[1024];
                    while (fgets(buffer, 1023, f) != NULL) {
                        char *s = wctxHtmlEncode(wctx, buffer);
                        io_write(wctx->out, s, strlen(s));
                    }
                    fclose(f);
                }
                WCTX_IPRINTT("</pre>\n", -1, 0);
                WCTX_IPRINTT("</div>\n", -1, 0); //level3
                WCTX_IPRINTT("</div>\n", -1, 0); //level3-div
            } else {
                WCTX_MESSAGE(MT_ALERT, NULL, tr("cfgFileDoesNotExist_%s"), fileMapping[fileNum].fileName);
            }
            finishHtmlPage(wctx);
        }
    }
}


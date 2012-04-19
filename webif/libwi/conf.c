/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */
#include <klone/utils.h>
#include <regex.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"
#include "toolbox/buf.h"

const char *charset[] = {
    "UTF-8",
    "ISO-8859-15",
};

bool settingPrintInputText(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value) {
    printInput(wctx, "text", id, name, cssClass, idx, value, (cfg) ? cfg->size : 0);
    return true;
}

bool settingPrintInputPassword(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value) {
    printInput(wctx, "password", id, name, cssClass, idx, value, (cfg) ? cfg->size : 0);
    return true;
}

bool settingPrintCheckbox(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value) {
    if (cfg->options) {
        //value==first option => false
        //value==second option =>true
        bool booleanValue = false;
        bool booleanValueSet = false;
        const char *option = cfg->options;
        int l = strcspn(option, "|");
        if (strncmp(option, value, l) == 0) {
            booleanValueSet = true;
        }
        if (option[l] != '|') {
            warn("Only one option for a checkboxCfg");
            option = NULL;
        } else {
            option += l + 1;
            l = strcspn(option, "|");
            if (strncmp(option, value, l) == 0) {
                booleanValue = true;
                booleanValueSet = true;
            }
        }
        if (!booleanValueSet) {
            warn("No option match for checkbox value");
        }
        openInputTag(wctx, "input", "checkbox", id, name, idx, cssClass, NULL, true);
        WCTX_PRINTF(" value=\"%.*s\"%s/>\n", (option) ? l : 4, (option) ? option : "true", checked[booleanValue]);
        return true;
    }
    return false;
}

bool settingPrintSelectOrRadio(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value
        , char ** ptr_id, bool asRadio, const char *labelpfx
        ) {
    if (cfg && cfg->options && strlen(cfg->options) > 0) {
        const char *type = NULL;
        if (asRadio) {
            WCTX_IPRINTT("<fieldset>\n", 0, 1);
        } else {
            openInputTag(wctx, "select", type, id, name, idx, cssClass, ptr_id, true);
            WCTX_PRINT(" size=\"1\">\n");
            WCTX_INCTAB();
        }
        const char * option = cfg->options;
        int o = cfg->indexOffset;
        for (;;) {
            int l = strcspn(option, "|");
            char *optionValue = strndup(option, l);
            char *optionLabel;
            if (labelpfx != NULL) {
                const char *buff = wctxBufferPrintf(wctx, "%s%s", labelpfx, optionValue);
                optionLabel = strdup(wctxHtmlEncode(wctx, tr(buff)));
            } else {
                optionLabel = strdup(wctxHtmlEncode(wctx, tr(optionValue)));
            }
            if (asRadio) {
                WCTX_IPRINTF("<label>%s ", optionLabel);
                openInputTag(wctx, "input", "radio", id, name, idx, cssClass, (ptr_id && *ptr_id) ? NULL : ptr_id, false);
            } else {
                WCTX_IPRINT("<option");
            }
            bool isSelected = false;
            if (cfg->isIndex) {
                isSelected = sameIntEx(value, o);
                WCTX_PRINTF(" value=\"%d\"", o);
            } else {
                isSelected = sameString(value, optionValue);
                WCTX_PRINTF(" value=\"%s\"", optionValue);
            }
            if (asRadio) {
                WCTX_PRINTF("%s/></label>\n", checked[isSelected]);
            } else {
                WCTX_PRINTF("%s>%s</option>\n", selected[isSelected], optionLabel);
            }
            free(optionValue);
            free(optionLabel);
            option += l;
            if (option[0] != '|') break;
            option++;
            o++;
        }
        if (asRadio) {
            WCTX_IPRINTT("</fieldset>\n", -1, 0);
        } else {
            WCTX_IPRINTT("</select>\n", -1, 0);
        }
        return true;
    }
    return false;
}

bool settingPrintSelect(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value
        ) {
    return settingPrintSelectOrRadio(wctx, cfg, id, name, cssClass, idx, value, NULL, false, NULL);
}

bool settingPrintRadio(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value
        ) {
    return settingPrintSelectOrRadio(wctx, cfg, id, name, cssClass, idx, value, NULL, true, NULL);
}

static bool settingPrintSelectCssTheme(wcontext_t *wctx, const settingConfig_t * const cfg
        , const char *id, const char *name, const char *cssClass, int idx, const char *value
        ) {
    char *real_id = NULL;
    bool result = settingPrintSelectOrRadio(wctx, cfg, id, name, cssClass, idx, value, &real_id, false, tr("css_theme"));
    if (result) {
        printApplyThemeJavaScript(wctx, real_id, id != NULL);
    }
    free(real_id);
    return result;
}

void validateCheckbox(const settingConfig_t * const cfg, setting_t * const setting) {
    const char *option = (cfg->options) ? cfg->options : "false|true";
    if (!strchr(option, '|')) {
        crit("validateCheckbox: wrong config, only one option");
        exit(EXIT_FAILURE);
    }
    setting->isValid = false;
    int l = strcspn(option, "|");
    if (setting->value == NULL || strncmp(option, setting->value, l) == 0) {
        //value==NULL => first option (unchecked checkboxes aren't submitted)
        setting->value = strndup(option, l);
        setting->isValid = true;
    } else {
        option += l + 1;
        l = strcspn(option, "|");
        if (option[l] == '|') warn("More than two options for a checkboxCfg");
        setting->isValid = boolean(setting->value != NULL && strncmp(option, setting->value, l) == 0);
    }
}

settingValidateInputFunction_t validateHostsField;

bool setAjaxDisabled(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        wc->ajaxDisabled = sameString(setting->value, "true");
        return true;
    }
    return false;
}

bool setBodyFontSize(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        if (wc->bodyFontSize && (wc == &webifConf || wc->bodyFontSize != webifConf.bodyFontSize)) {
            free(wc->bodyFontSize);
        }
        wc->bodyFontSize = strdup(setting->value);
        return true;
    }
    return false;
}

bool setDefaultLanguage(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t *wc = (webifConf_t*) configurable;
        errno = 0;
        int langId = strtol(setting->value, NULL, 10);
        if (errno == 0 && langId >= -1 && langId < I18N_NUM) {
            wc->langId = langId;
            return true;
        }
    }
    return false;
}

bool setFilesystemEncoding(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t *wc = (webifConf_t*) configurable;
        errno = 0;
        enum charset_e encoding = strtol(setting->value, NULL, 10);
        if (errno == 0 && encoding >= CHARSET_MIN && encoding <= CHARSET_MAX) {
            wc->filesystemEncoding = encoding;
            return true;
        }
    }
    return false;
}

bool setEpgGridHours(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t *wc = (webifConf_t*) configurable;
        errno = 0;
        int epgGridHours = strtol(setting->value, NULL, 0);
        if (errno == 0 && epgGridHours > 0 && epgGridHours <= 12) {
            wc->epgGridHours = epgGridHours;
            return true;
        }
        errno = 0;
    }
    return false;
}

bool setCssTheme(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        errno = 0;
        cssTheme_t theme = strtol(setting->value, NULL, 10);
        if (errno == 0 && theme >= CSS_THEME_MIN && theme <= CSS_THEME_MAX) {
            wc->cssTheme = theme;
            return true;
        }
        errno = 0;
    }
    return false;
}
void parseHostsField(const char *hostsField, hostList_t *hosts);

bool setHosts(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t *wc = (webifConf_t*) configurable;
        hostList_t *hosts = &wc->hosts;
        parseHostsField(setting->value, hosts);
        warn_if(hosts->length == 0);
        return hosts->length > 0;
    }
    return false;
}

bool setMaxDepth(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        errno = 0;
        int maxDepth = strtol(setting->value, NULL, 10);
        if (errno == 0 && maxDepth >= 0) {
            wc->maxDepth = maxDepth;
            return true;
        }
    }
    return false;
}

bool setNoBoxShadow(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        wc->noBoxShadow = sameString(setting->value, "true");
        return true;
    }
    return false;
}

bool setNoCssColumns(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        wc->noCssColumns = sameString(setting->value, "true");
        return true;
    }
    return false;
}

bool setNoTextShadow(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        wc->noTextShadow = sameString(setting->value, "true");
        return true;
    }
    return false;
}

bool setPlaylistType(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        errno = 0;
        playlistType_t plt = strtol(setting->value, NULL, 10);
        if (errno == 0 && plt >= PL_M3U && plt <= PL_XSPF) {
            wc->playlistType = plt;
            return true;
        }
    }
    return false;
}

bool setPopupsDisabled(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        wc->popupsDisabled = sameString(setting->value, "true");
        return true;
    }
    return false;
}

bool setUseHtml5VideoTag(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t * wc = (webifConf_t *) configurable;
        wc->useHtml5VideoTag = sameString(setting->value, "true");
        return true;
    }
    return false;
}

bool setVideoHeight(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t *wc = (webifConf_t*) configurable;
        errno = 0;
        int height = strtol(setting->value, NULL, 0);
        if (errno == 0 && height > 0 && height < 5000) {
            wc->videoHeight = height;
            return true;
        }
    }
    return false;
}

bool setVideoWidth(void *configurable, setting_t * const setting) {
    if (configurable && setting && setting->value && setting->value[0]) {
        webifConf_t *wc = (webifConf_t*) configurable;
        errno = 0;
        int width = strtol(setting->value, NULL, 0);
        if (errno == 0 && width > 0 && width < 5000) {
            wc->videoWidth = width;
            return true;
        }
    }
    return false;
}

enum settingDisplayOrder_e {
    webif_display_order_start,
    // MAIN
    default_language,
    hosts,
    use_external_www_folder,
    filesystem_encoding,
    // SECURITY
    user,
    password,
    deletion_disabled,
    config_view_disabled,
    config_change_disabled,
    // TIMERS
    default_margin_start,
    default_margin_stop,
    // FEATURES
    use_html5_video_tag,
    video_width,
    video_height,
    playlist_type,
    event_search_url,
    ajax_disabled,
    popups_disabled,
    // APPEARANCE
    css_theme,
    body_font_size,
    no_css_columns,
    no_box_shadow,
    no_text_shadow,
    max_depth,
    epg_grid_hours,
    display_host_id,
    webif_display_order_end,
};
settingConfig_t webifParamConfig[] = {
    {
        .name = "ajax_disabled",
        .displayOrder = ajax_disabled,
        .defaultValue = "false",
        .options = "false|true",
        .cookie = true,
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox,
        .setField = &setAjaxDisabled
    },
    {
        .name = "body_font_size",
        .displayOrder = body_font_size,
        .defaultValue = NULL,
        .size = 4,
        .cookie = true,
        .setField = &setBodyFontSize
    },
    {
        .name = "config_change_disabled",
        .displayOrder = config_change_disabled,
        .defaultValue = "false",
        .options = "false|true",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "config_view_disabled",
        .displayOrder = config_view_disabled,
        .defaultValue = "false",
        .options = "false|true",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "css_theme",
        .displayOrder = css_theme,
        .defaultValue = "1",
        .options = "0|1",
        .isIndex = false,
        .cookie = true,
        .printInput = &settingPrintSelectCssTheme,
        .setField = &setCssTheme
    },
    {
        .name = "default_language",
        .displayOrder = default_language,
        .defaultValue = "-1",
        .options = "langBrowserDefined|langEnglish|langGerman|langSpanish|langFrench",
        .isIndex = true,
        .indexOffset = -1,
        .printInput = &settingPrintSelect,
        .setField = &setDefaultLanguage
    },
    {
        .name = "default_margin_start",
        .displayOrder = default_margin_start,
        .defaultValue = "15",
        .size = 3
    },
    {
        .name = "default_margin_stop",
        .displayOrder = default_margin_stop,
        .defaultValue = "20",
        .size = 3
    },
    {
        .name = "deletion_disabled",
        .displayOrder = deletion_disabled,
        .defaultValue = "false",
        .options = "false|true",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "display_host_id",
        .displayOrder = display_host_id,
        .defaultValue = "true",
        .options = "false|true",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "epg_grid_hours",
        .displayOrder = epg_grid_hours,
        .defaultValue = "4",
        .size = 5,
        .cookie = true,
        .setField = &setEpgGridHours
    },
    {
        .name = "event_search_url",
        .displayOrder = event_search_url,
        .defaultValue = "http://www.google.com/search?as_q={title}+{subtitle}",
        .size = 50
    },
    {
        .name = "filesystem_encoding",
        .displayOrder = filesystem_encoding,
        .defaultValue = "1",
        .options = "UTF8|ISO88591",
        .isIndex = true,
        .printInput = &settingPrintSelect,
        .setField = &setFilesystemEncoding
    },

    {
        .name = "hosts",
        .displayOrder = hosts,
        .defaultValue = ",127.0.0.1,2001,/var/vdr/video0;",
        .size = 60,
        .validate = &validateHostsField,
        .setField = &setHosts
    },
    {
        .name = "max_depth",
        .displayOrder = max_depth,
        .defaultValue = "2",
        .size = 2,
        .cookie = true,
        .setField = &setMaxDepth
    },
    {
        .name = "no_box_shadow",
        .displayOrder = no_box_shadow,
        .defaultValue = "false",
        .options = "false|true",
        .cookie = true,
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox,
        .setField = &setNoBoxShadow,
    },
    {
        .name = "no_css_columns",
        .displayOrder = no_css_columns,
        .defaultValue = "false",
        .options = "false|true",
        .cookie = true,
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox,
        .setField = &setNoCssColumns,
    },
    {
        .name = "no_text_shadow",
        .displayOrder = no_text_shadow,
        .defaultValue = "false",
        .options = "false|true",
        .cookie = true,
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox,
        .setField = &setNoTextShadow,
    },
    {
        .name = "password",
        .displayOrder = password,
        .size = 10,
        .printInput = &settingPrintInputPassword
    },
    {
        .name = "playlist_type",
        .displayOrder = playlist_type,
        .defaultValue = "0",
        .options = "M3U|XSPF",
        .isIndex = true,
        .cookie = true,
        .printInput = &settingPrintSelect,
        .setField = &setPlaylistType
    },
    {
        .name = "popups_disabled",
        .displayOrder = popups_disabled,
        .defaultValue = "false",
        .options = "false|true",
        .cookie = true,
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox,
        .setField = &setPopupsDisabled
    },
    {
        .name = "use_external_www_folder",
        .displayOrder = use_external_www_folder,
        .defaultValue = "false",
        .options = "false|true",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "use_html5_video_tag",
        .displayOrder = use_html5_video_tag,
        .defaultValue = "true",
        .options = "false|true",
        .cookie = true,
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox,
        .setField = &setUseHtml5VideoTag
    },
    {
        .name = "user",
        .displayOrder = user,
        .size = 10
    },
    {
        .name = "video_height",
        .displayOrder = video_height,
        .defaultValue = "480",
        .size = 4,
        .cookie = true,
        .setField = &setVideoHeight
    },
    {
        .name = "video_width",
        .displayOrder = video_width,
        .defaultValue = "640",
        .size = 4,
        .cookie = true,
        .setField = &setVideoWidth
    },
};
settingConfig_t rcParamConfig[] = {
    {
        .name = "ip",
        .defaultValue = "192.168.100.102",
        .size = 15
    },
    {
        .name = "net",
        .defaultValue = "inet",
        .options = "inet|DHCP",
        .printInput = &settingPrintSelect
    },
    {
        .name = "netdate",
        .defaultValue = "NO",
        .options = "NO|YES",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "netmask",
        .defaultValue = "255.255.255.0",
        .size = 15
    },
};
settingConfig_t vdrParamConfig[] = {
    {
        .name = "AbortWhenPluginFails",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "CutterAutoDelete",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "DelTimeshiftRec",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "EmergencyExit",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "HardLinkCutter",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "HotStandby",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "MarkInstantRecord",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "MenuButtonCloses",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "MenuScrollPage",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "MenuScrollWrap",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "PauseLifetime",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "SetSystemTime",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShowInfoOnChSwitch",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShowProgressBar",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShowRecDate",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShowRecLength",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShowRecTime",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShowReplayMode",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "ShutdownMessage",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "SplitEditedFiles",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "UseDolbyDigital",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "UseDolbyInRecordings",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "UseSubtitle",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "UseVps",
        .defaultValue = "0",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
    {
        .name = "WarEagleIcons",
        .defaultValue = "1",
        .options = "0|1",
        .validate = &validateCheckbox,
        .printInput = &settingPrintCheckbox
    },
};
settingConfigList_t settingConfigList[] = {//Indexed by cfgFileId_t
    {
        .exclusive = true,
        .length = sizeof (webifParamConfig) / sizeof (settingConfig_t),
        .entry = webifParamConfig
    },
    {
        .exclusive = false,
        .length = sizeof (rcParamConfig) / sizeof (settingConfig_t),
        .entry = rcParamConfig
    },
    {
        .exclusive = false,
        .length = sizeof (vdrParamConfig) / sizeof (settingConfig_t),
        .entry = vdrParamConfig
    }
};
const cfgFile_t fileMapping[] = {//Partially indexed by cfgFileId_t
    {
        .i18nKey = "cfgWi",
        .fileName = "/etc/webif/webif.conf",
        .canCreateNew = true
    },
    {
        .i18nKey = "cfgRc",
        .fileName = "/etc/rc.conf"
    },
    {
        .i18nKey = "vdrSetup",
        .fileName = "/etc/vdr/setup.conf"
    },
    {
        .i18nKey = "fstab",
        .fileName = "/etc/fstab"
    },
    {
        .i18nKey = "rc.local",
        .fileName = "/etc/rc.local.conf"
    },
    {
        .i18nKey = "bootMenu",
        .fileName = "/etc/bootmenu/bootmenu.conf"
    },
    {
        .i18nKey = "svdrpHosts",
        .fileName = "/etc/vdr/svdrphosts.conf"
    },
    {
        .i18nKey = "channels",
        .fileName = "/etc/vdr/channels.conf"
    },
    {
        .i18nKey = "streamdevHosts",
        .fileName = "/etc/vdr/plugins/streamdevhosts.conf"
    }
};
const int fileMappingLength = sizeof (fileMapping) / sizeof (cfgFile_t);

void initSetting(setting_t * const setting) {
    setting->name = NULL;
    setting->value = NULL;
    setting->oldValue = NULL;
    setting->comment = NULL;
    setting->config = NULL;
    setting->written = false;
    setting->isValid = true;
    setting->saveCookie = false;
    setting->deleteCookie = false;
}

void freeSetting(setting_t * const setting) {
    free(setting->name);
    free(setting->value);
    free(setting->oldValue);
    free(setting->comment);
    initSetting(setting);
}

void initSettingList(settingList_t * const settings) {
    settings->fileId = CF_WEBIFCONF;
    settings->hostId = 0;
    settings->length = 0;
    settings->entry = NULL;
}

void freeSettings(settingList_t * const settings) {
    int i;
    for (i = 0; i < settings->length; i++) freeSetting(settings->entry + i);
    free(settings->entry);
    initSettingList(settings);
}

int settingConfigCompare(const void *a, const void *b) {
    const settingConfig_t *sca = a;
    const settingConfig_t *scb = b;
    return strcmp(sca->name, scb->name);
}

settingConfig_t *findSettingConfig(settingConfigList_t *configs, const char *name) {
    settingConfig_t test = {.name = name};
    return (settingConfig_t *) bsearch(&test, configs->entry, configs->length, sizeof (settingConfig_t), settingConfigCompare);
}

settingConfig_t *getSettingConfig(cfgFileId_t fileId, const char *name) {
    return findSettingConfig(settingConfigList + fileId, name);
}

void parseSetting(char *s, settingList_t * const settings, settingConfigList_t * const configs) {
    while (s[0] == ' ')s++;
    if (s[0] && s[0] != '#') {
        char *e, *n;
        e = s + strcspn(s, "\n\r");
        e[0] = 0;
        n = strchr(s, '=');
        if (n && n > s) {
            for (e = n; e > s && e[-1] == ' '; e--);
            if (e > s) {
                e[0] = 0;
                settingConfig_t *config = findSettingConfig(configs, s);
                if (!configs->exclusive || config) {
                    settings->entry = realloc(settings->entry, (++settings->length) * sizeof (setting_t));
                    crit_goto_if(settings->entry == NULL, outOfMemory);
                    setting_t *setting = settings->entry + settings->length - 1;
                    initSetting(setting);
                    setting->name = strdup(s);
                    for (s = n + 1; s[0] == ' '; s++);
                    if (s[0]) {
                        for (n = strchrnul(s + 1, '#'); n[0] == '#' && n[-1] != ' '; n = strchrnul(n + 1, '#'));
                        for (e = n; e > s && e[-1] == ' '; e--);
                        if (e > s) {
                            setting->value = strndup(s, e - s);
                            setting->oldValue = strdup(setting->value);
                        }
                        if (n[0] == '#') {
                            for (s = n + 1; s[0] == ' '; s++);
                            if (s[0]) {
                                for (e = strchrnul(s, 0); e > s && e[-1] == ' '; e--);
                                setting->comment = (e > s) ? strndup(s, e - s) : NULL;
                            }
                        }
                    }
                    if (config) {
                        config->alreadySet = true;
                        setting->config = config;
                    }
                }
            }
        }
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

/**
 * Check that fileId is within limits.
 * @param fileId : id to check.
 * @param wctx : if not null prints message in case of error.
 * @return true if valid.
 */
bool validateCnfFileId(int fileId, wcontext_t *wctx) {
    if (fileId < CF_MIN || fileId > CF_MAX) {
        warn("Wrong fileId %d", fileId);
        WCTX_MESSAGE(MT_ALERT, NULL, tr("cfgFileWrongFileId%d"), fileId);
        return false;
    }
    return true;
}

bool readConf(settingList_t * const settings) {
    if (!validateCnfFileId(settings->fileId, NULL)) return false;
    int i;
    char * s;
    settingConfig_t * config;
    settingConfigList_t * const configs = &settingConfigList[settings->fileId];
    setting_t *setting;
    for (i = 0, config = configs->entry; i < configs->length; i++, config++) {
        config->alreadySet = false;
    }
    char *data = NULL;
    int offset = 0;
    if (settings->fileId == CF_VDRCONF) {
        hostConf_t * host = getHost(settings->hostId);
        if (host == NULL || !host->isVdr) {
            warn("Host %d no existe o no ejecuta VDR", settings->hostId);
            return false;
        }
        data = execSvdrp(host, "PARG");
        offset = 4;
    } else {
        u_buf_t *buffer;
        warn_return_if(u_buf_create(&buffer), false);
        if (u_buf_load(buffer, fileMapping[settings->fileId].fileName) == 0) {
            data = u_buf_ptr(buffer);
            u_buf_detach(buffer);
            u_buf_free(buffer);
        } else {
            u_buf_free(buffer);
            return false;
        }
    }
    if (data) {
        for (s = strtok(data, "\r\n"); s != 0; s = strtok(0, "\r\n")) {
            if (offset == 0 || atoi(s) == SVDRP_CMD_OK) {
                parseSetting(s + offset, settings, configs);
            }
        }
        free(data);
    }
    for (i = 0, config = configs->entry; i < configs->length; i++, config++) {
        if (config->alreadySet) continue;
        settings->entry = realloc(settings->entry, (++settings->length) * sizeof (setting_t));
        crit_goto_if(settings->entry == NULL, outOfMemory);
        setting = settings->entry + settings->length - 1;
        initSetting(setting);
        setting->name = strdup(config->name);
        setting->value = (config->defaultValue) ? strdup(config->defaultValue) : NULL;
        setting->oldValue = NULL;
        setting->config = config;
        config->alreadySet = true;
    }
    return boolean(settings->length > 0);
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

FILE * tmpCfgFile(char tmpName[16]) {
    strcpy(tmpName, "/etc/cfgXXXXXX"); //TODO tiene que estar en etc para que rename no genere error
    errno = 0;
    int tfd = mkstemp(tmpName);
    if (errno) {
        warn(strerror(errno));
        return NULL;
    }
    FILE *t = fdopen(tfd, "w");
    if (errno) {
        warn(strerror(errno));
        unlink(tmpName);
        close(tfd);
        return NULL;
    }
    return t;
}

bool writeConfFile(settingList_t * const params) {
    bool result = false;
    bool isNew = false;
    bool exclusive = settingConfigList[params->fileId].exclusive;
    const char *fileName = fileMapping[params->fileId].fileName;
    errno = 0;
    FILE *f = fopen(fileName, "r");
    if (errno) {
        if (errno == ENOENT) {
            isNew = true;
            errno = 0;
        } else {
            warn(strerror(errno));
            return false;
        }
    }
    char tmpName[16];
    FILE *t = tmpCfgFile(tmpName);
    if (t == NULL) {
        if (!isNew) fclose(f);
        return false;
    }
    int i;
    char *name;
    char *value;
    char *comment;
    setting_t *setting;
    for (i = 0; i < params->length; i++) params->entry[i].written = false;
    if (!isNew) {
        char buffer[256];
        char *s;
        int l, ln, lv, lc;
        while (!feof(f) && (s = fgets(buffer, sizeof (buffer), f)) != NULL) { //TODO considerar u_buf_load
            name = NULL;
            value = NULL;
            comment = NULL;
            s += strspn(s, " ");
            if (!s[0] || s[0] == '#') { //write comments and empty lines unmodified
                fputs(buffer, t);
                if (errno) {
                    warn(strerror(errno));
                    errno = 0;
                }
            } else {
                name = s;
                l = strcspn(s, "=\n\r");
                if (s[l] == '=') {
                    ln = l;
                    while (ln > 0 && s[ln - 1] == ' ') ln--;
                    if (ln > 0) {
                        s += l + 1;
                        s += strspn(s, " ");
                        value = s;
                        lv = l = strcspn(s, "#\n\r");
                        if (s[l] == '#') comment = s + l + 1;
                        while (lv > 0 && s[lv - 1] == ' ') lv--;
                        name[ln] = 0;
                        value[lv] = 0;
                        setting = NULL;
                        for (i = 0; i < params->length; i++) {
                            if (strcmp(params->entry[i].name, name) == 0) {
                                setting = params->entry + i;
                                break;
                            }
                        }
                        if (exclusive && !setting) {
                            dbg("param [%s] eliminado de conf [%s]", name, fileName);
                        } else {
                            if (setting) {
                                if (setting->saveCookie) {
                                    dbg("param %s no se guarda en servidor, se envia como cookie y se mantiene el valor  %s", setting->name, value);
                                } else {
                                    value = setting->value;
                                }
                            }
                            if (comment) {
                                while (comment[0] == ' ') comment++;
                                lc = strcspn(comment, "\n\r");
                                while (lc > 0 && comment[lc] == ' ') lc--;
                                fprintf(t, "%s=%s #%s\n", name, (value) ? value : "", comment);
                            } else {
                                fprintf(t, "%s=%s\n", name, (value) ? value : "");
                            }
                        }
                        if (errno) {
                            warn(strerror(errno));
                            errno = 0;
                        }
                        if (setting) setting->written = true;
                    }
                }
            }
        }
        fclose(f);
    }
    for (i = 0, setting = params->entry; i < params->length; i++, setting++) {
        if (!setting->written) {
            fprintf(t, "%s=%s\n", setting->name, (setting->value) ? setting->value : "");
            setting->written = true;
        }
    }
    fclose(t);
    char *backupName;
    asprintf(&backupName, "%s.back", fileName);
    if (backupName) {
        remove(backupName);
        if (errno) {
            if (errno == ENOENT) {
                errno = 0;
            } else {
                warn(strerror(errno));
            }
        }
        if (!errno) {
            if (!isNew) {
                rename(fileName, backupName);
                if (errno) {
                    warn(strerror(errno));
                    errno = 0;
                }
            }
            rename(tmpName, fileName);
            if (errno) {
                warn(strerror(errno));
                errno = 0;
                result = false;
            } else {
                result = true;
            }
        }
        if (errno) {
            warn(strerror(errno));
            errno = 0;
        }
        free(backupName);
    }
    return result;
}

bool sendConfVDR(int hostId, settingList_t * const params) {
    bool onlyDiff = boolean(hostId == params->hostId);
    hostConf_t *host = getHost(hostId);
    if (host && host->isVdr) {
        bool result = true;
        int i;
        setting_t *setting;
        for (i = 0, setting = params->entry; i < params->length; i++, setting++) {
            if (!setting->saveCookie && (!onlyDiff || strcmp(setting->value, setting->oldValue) != 0)) {
                char *data = execSvdrp(host, "PARS %s=%s", setting->name, (setting->value) ? setting->value : "");
                setting->written = boolean(data && atoi(data) == SVDRP_CMD_OK);
                if (!setting->written) {
                    result = false;
                }
            }
        }
        return result;
    } else {
        warn("Host %d no existe o no ejecuta VDR", hostId);
    }
    return false;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool writeConf(int hostId, settingList_t * const params) {
    if (!validateCnfFileId(params->fileId, NULL)) return false;
    return (params->fileId == CF_VDRCONF) ? sendConfVDR(hostId, params) : writeConfFile(params);
}

time_t get_mtime(const char *fileName) {
    struct stat status;
    if (stat(fileName, &status) == 0) {
        return status.st_mtime;
    } else {
        warn(strerror(errno));
        return 0;
    }
}

bool readWebifConf() {
    errno = 0;
    time_t current_mtime = get_mtime(fileMapping[CF_WEBIFCONF].fileName);
    if (webifConf.mtime < current_mtime || errno) {
        freeWebifConf();
        errno = 0;
    }
    if (webifConf.alreadySet) {
        return true;
    }
    settingList_t settings;
    setting_t *setting;
    initSettingList(&settings);
    settings.hostId = 0;
    settings.fileId = CF_WEBIFCONF;
    if (readConf(&settings)) {
        int i;
        for (i = 0, setting = settings.entry; i < settings.length; i++, setting++) {
            const settingConfig_t *cfg = setting->config;
            if (cfg && cfg->setField) { //TODO mover a readConf
                //dbg("setField(%s,%s)", setting->name, setting->value);
                if (!cfg->setField(&webifConf, setting)) {
                    setting->isValid = false;
                    warn("Valor leído de %s = %s es erróneo", setting->name, setting->value);
                }
            } else if (strcmp(setting->name, "deletion_disabled") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.deletionDisabled = sameString(setting->value, "true");
                }
            } else if (strcmp(setting->name, "config_change_disabled") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.configChangeDisabled = sameString(setting->value, "true");
                }
            } else if (strcmp(setting->name, "config_view_disabled") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.configViewDisabled = sameString(setting->value, "true");
                }
            } else if (strcmp(setting->name, "use_external_www_folder") == 0) {//TODO cookie
                if (setting->value && setting->value[0]) {
                    webifConf.useExternalWwwFolder = sameString(setting->value, "true");
                }
            } else if (strcmp(setting->name, "display_host_id") == 0) {//TODO cookie
                if (setting->value && setting->value[0]) {
                    webifConf.displayHostId = sameString(setting->value, "true");
                }
            } else if (strcmp(setting->name, "default_margin_start") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.defaultMarginStart = atoi(setting->value);
                }
            } else if (strcmp(setting->name, "default_margin_stop") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.defaultMarginStop = atoi(setting->value);
                }
            } else if (strcmp(setting->name, "user") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.user = strdup(setting->value);
                }
            } else if (strcmp(setting->name, "password") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.password = strdup(setting->value);
                }
            } else if (strcmp(setting->name, "event_search_url") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.eventSearchUrl = strdup(setting->value);
                }
            } else if (strcmp(setting->name, "body_font_size") == 0) {
                if (setting->value && setting->value[0]) {
                    webifConf.bodyFontSize = strdup(setting->value);
                }
            }
        }
        freeSettings(&settings);
        struct stat st;
        int logos_stat = stat("/etc/webif/www/images/logos", &st);
        if (logos_stat == 0 && S_ISDIR(st.st_mode)) {
            webifConf.noChannelLogos = false;
        } else {
            warn("No existe directorio de logos");
            webifConf.noChannelLogos = true;
        }
        webifConf.alreadySet = true;
        webifConf.mtime = current_mtime;
        return true;
    }
    return false;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void initHostConf(hostConf_t * const host) {
    host->id = 0;
    host->name = NULL;
    host->ip = NULL;
    host->port = 0;
    host->video0 = NULL;
    host->isVdr = false;
    host->webifVersion = 0;
    host->socket = 0;
}

void freeHostConf(hostConf_t * const host) {
    free(host->name);
    free(host->ip);
    free(host->video0);
    initHostConf(host);
}

void initHostList(hostList_t * const hosts) {
    hosts->length = 0;
    hosts->numVDRs = 0;
    hosts->entry = NULL;
}

void freeHostList(hostList_t * const hosts) {
    int i;
    hostConf_t *host;
    for (i = 0, host = hosts->entry; i < hosts->length; i++, host++) {
        freeHostConf(host);
    }
    free(hosts->entry);
    initHostList(hosts);
}
webifConf_t webifConf = {
    .alreadySet = false,
    .version = "2.0",
    .filesystemEncoding = ISO885915,
};

void initWebifConf() {
    webifConf.alreadySet = false;
    webifConf.langId = 0;
    initHostList(&webifConf.hosts);
    webifConf.user = NULL;
    webifConf.password = NULL;
    webifConf.eventSearchUrl = NULL;
    webifConf.bodyFontSize = NULL;
}

void freeWebifConf() {
    if (webifConf.user != NULL) {
        free(webifConf.user);
        webifConf.user = NULL;
    }
    if (webifConf.password != NULL) {
        free(webifConf.password);
        webifConf.password = NULL;
    }
    freeHostList(&webifConf.hosts);
    if (webifConf.eventSearchUrl != NULL) {
        free(webifConf.eventSearchUrl);
    }
    initWebifConf();
}

/**************************
 * HOSTS MANAGEMENT
 **************************/
hostConf_t *getHost(int hostId) {
    return (hostId >= 0 && hostId < webifConf.hosts.length) ? webifConf.hosts.entry + hostId : NULL;
}

hostConf_t *getFirstVdrHost() {
    hostConf_t *host;
    int i;
    for (i = 0, host = webifConf.hosts.entry; i < webifConf.hosts.length; i++, host++)
        if (host->isVdr) return host;
    return NULL;
}

bool isLocalhost(hostConf_t *host) {
    return boolean(host == NULL || strncmp(host->ip, "127.", 4) == 0);
}

const char *getHostHttpAddr(hostConf_t *host, wcontext_t *wctx) {
    return (isLocalhost(host))
            ? request_get_field(wctx->request, "Host")->value
            : (host->name[0])
            ? host->name
            : host->ip;
}
#define HOST_PATTERN "[:space:]*([a-zA-Z0-9]*) *, *([0-9.]*) *, *([0-9]*) *, *([a-zA-Z0-9./_]*) *(, *([0-9]*) *)?"

bool validateHostsStr(const char *hosts) {
    if (hosts == NULL) return false;
    const char * pattern = "^(" HOST_PATTERN " *;? *)+$";
    regex_t regex;
    if (regcomp(&regex, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        crit("Error compiling the pattern!");
        exit(EXIT_FAILURE);
    }
    bool ok = (regexec(&regex, hosts, 0, NULL, 0) == 0);
    regfree(&regex);
    return ok;
}

void validateHostsField(const settingConfig_t * const cfg, setting_t * const setting) {
    setting->isValid = validateHostsStr(setting->value);
}

/**
 * Get hosts from JSON representation.
 *sintaxis:
 *  {[name:"<host name>",][ip:"<SVDRP ip>",][port:"<SVDRP port>",][video0:"<video0 path>"]}[,{<host 2>}...]
 *where: 
 * {},:"      are literals
 * []         optional content
 *<value>    replace by real value
 */
bool parseHostsFieldJson(const char *hostsField, hostList_t *hosts) {
    warn("No implementado");
    return false;
}

/**
 * Get hosts from textual representation.
 * @param hostsField: host textual description with the sintaxis
 *	[<host name>],[<SVDRP ip>],[<SVDRP port>],[<video0 path>];...
 */
bool parseHostsFieldPlain(const char *hostsField, hostList_t *hosts) {
    hostConf_t *host;

    enum {
        MAXREGMATCH = 7,
    };
    const char * pattern = HOST_PATTERN;
    regex_t regex;
    bool result = false;
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
        crit("Error compiling the pattern!");
    } else {
        regmatch_t regmatch[MAXREGMATCH];
        int err, m, so, l;
        const char *t = hostsField;
        while (t && t[0]) {
            if ((err = regexec(&regex, t, MAXREGMATCH, regmatch, 0)) == 0) {
                if (regmatch[0].rm_eo > 0 && t[regmatch[0].rm_eo] != ';') {
                    warn("Cada host debe acabar en ;");
                    break;
                }
                bool isValid = true;
                hosts->entry = realloc(hosts->entry, (++hosts->length) * sizeof (hostConf_t));
                crit_goto_if(hosts->entry == NULL, outOfMemory);
                host = hosts->entry + hosts->length - 1;
                initHostConf(host);
                for (m = 1; m < MAXREGMATCH && isValid; m++) {
                    so = regmatch[m].rm_so;
                    l = regmatch[m].rm_eo - so;
                    if (so >= 0 && l >= 0) {
                        errno = 0;
                        switch (m) {
                            case 1:
                                crit_goto_if((host->name = strndup(t + so, l)) == NULL, outOfMemory);
                                break;
                            case 2:
                                crit_goto_if((host->ip = strndup(t + so, l)) == NULL, outOfMemory);
                                break;
                            case 3:
                                host->port = strtol(t + so, NULL, 10);
                                break;
                            case 4:
                                if (l > 1 && t[so + l - 1] == '/') l--;
                                crit_goto_if((host->video0 = strndup(t + so, l)) == NULL, outOfMemory);
                                break;
                            case 6:
                                host->webifVersion = strtol(t + so, NULL, 10);
                                break;
                        };
                        if (errno) {
                            warn(strerror(errno));
                            isValid = false;
                            errno = 0;
                        }
                    }
                }

                warn_ifb(isValid
                        && (!host->name || !host->name[0])
                        && (!host->ip || !host->ip[0])
                        && (!host->video0 || !host->video0[0])) {
                    isValid = false;
                }
                if (isValid) {
                    host->id = hosts->length - 1;
                    host->isVdr = boolean(host->ip && host->ip[0] && host->port > 0);
                    if (host->isVdr) hosts->numVDRs++;
                } else {
                    freeHostConf(host);
                    hosts->length--;
                }
                if (regmatch[0].rm_eo > 0) {
                    t += regmatch[0].rm_eo;
                    while (*t == ';') {
                        t++;
                    }
                } else {
                    t = NULL;
                }
            } else if (err == REG_NOMATCH) {
                break;
            } else if (err == REG_ESPACE) {
                goto outOfMemory;
            }
        }
        result = true;
    }
    regfree(&regex);
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void parseHostsField(const char *hostsField, hostList_t *hosts) {
    freeHostList(hosts);
    if (hostsField != NULL) {
        while (*hostsField == ' ') hostsField++;
        if (hostsField[0] == '{') {
            parseHostsFieldJson(hostsField, hosts);
        } else if (hostsField[0] != '\0') {
            parseHostsFieldPlain(hostsField, hosts);
        }
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

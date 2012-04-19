/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <fcntl.h>
#include <iconv.h>
#include <klone/emb.h>
#include <klone/ioprv.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <locale.h>
#include <toolbox/str.h>
#include <u/libu.h>

#include "commands.h"
#include "misc.h"
#include "i18n.h"
#include "svdrp_comm.h"

const char *checked[2] = {"", " checked=\"checked\""};
const char *selected[2] = {"", " selected=\"selected\""};
const char *videoTypeStr[7] = {"unknown", "sd43", "sd169", "sd", "hd43", "hd169", "hd"}; //See videoType_t
const char *cssSortClass[] = {"sort sortdesc", "sort sortnone", "sort sortasc"};
const char *classCurrent[2] = {"", " class=\"act\""};
const char *tabs = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

systemType_t systemType = ST_UNKNOWN;

bool parseRequestStr(request_t *request, char ** pathStr, char ** queryStr) {
    *pathStr = NULL;
    *queryStr = NULL;
    if (request == NULL) {
        return false;
    }
    const char *r = request_get_client_request(request);
    int l;
    r += strcspn(r, " ");
    r += strspn(r, " ");
    l = strcspn(r, " ?");
    (*pathStr) = strndup(r, l);
    if (r[l] == '?') {
        r += l + 1;
        r += strcspn(r, " ");
        (*queryStr) = strndup(r, l);
    }
    return true;
}

char *strreplace(char *s, const char *s1, const char *s2) {
    //taken from tools.h
    char *p = strstr(s, s1);
    if (p) {
        int of = p - s;
        int l = strlen(s);
        int l1 = strlen(s1);
        int l2 = strlen(s2);
        if (l2 > l1) {
            s = realloc(s, l + l2 - l1 + 1);
        }
        char *sof = s + of;
        if (l2 != l1) {
            memmove(sof + l2, sof + l1, l - of - l1 + 1);
        }
        strncpy(sof, s2, l2);
    }
    return s;
}

bool isDST(time_t * aTime) {
    struct tm t = *localtime(aTime);
    return boolean(t.tm_isdst > 0);
}

bool sameDay(time_t oneDate, time_t anotherDate) {
    struct tm sone = *localtime(&oneDate);
    struct tm sanother = *localtime(&anotherDate);
    return boolean((sone.tm_year == sanother.tm_year) && (sone.tm_yday == sanother.tm_yday));
}

bool sameString(const char *s1, const char *s2) {
    return boolean((s1 == NULL && s2 == NULL) || (strcmp(s1, s2) == 0));
}

bool sameInt(const int i1, const int i2) {
    return boolean(i1 == i2);
}

bool sameIntEx(const char *s, const int i) {
    return boolean(strtol(s, NULL, 10) == i);
}

bool fileExists(const char *fileName) {
    struct stat status;

    if (stat(fileName, &status) < 0) {
        return false;
    }
    return boolean(S_ISREG(status.st_mode));
}

bool dirExists(const char *dirName) {
    struct stat status;

    if ((stat(dirName, &status)) < 0) {
        return false;
    }
    return boolean(S_ISDIR(status.st_mode));
}

char *stringIconv(const enum charset_e toCode, const enum charset_e fromCode, const char *fromStr, const int fromLen) {
    if (fromLen > 0) {
        char *toStr;
        u_string_t *tmp;
        u_string_create(NULL, 0, &tmp);
        if (u_string_aiconv(tmp, charset[toCode], charset[fromCode], fromStr, fromLen) == 0) {
            toStr = u_string_detach_cstr(tmp);
        } else {
            u_string_free(tmp);
            toStr = strndup(fromStr, fromLen);
        }
        crit_goto_if(!toStr, outOfMemory);
        return toStr;
    }
    return NULL;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool initWCtx(wcontext_t *wctx, request_t *request, response_t *response, session_t *session, io_t *out) {
    if (true) {
        //TODO necesario siempre?
        readWebifConf();
    }
    wctx->session = session;
    wctx->request = request;
    wctx->response = response;
    wctx->pageContext = NULL;
    wctx->out = out;
    if (!isAuthorized(wctx)) {
        return false;
    }
    wctx->conf = webifConf;
    if (true) { //read cookies based configuration (browser side)
        //TODO necesario siempre?
        int i;
        settingConfig_t *config;
        for (i = 0, config = settingConfigList[CF_WEBIFCONF].entry; i < settingConfigList[CF_WEBIFCONF].length; i++, config++) {
            if (config->cookie && config->setField) { //TODO Iterar sobre las cookies mejor
                const char *cookie = request_get_cookie(request, config->name);
                if (cookie) {
                    setting_t setting;
                    initSetting(&setting);
                    setting.config = config;
                    setting.name = (char *) config->name;
                    setting.value = (char *) cookie;
                    config->setField(&wctx->conf, &setting);
                }
            }
        }
    }
    if (true) { //parse request args && headers
        vars_t *rqargs = request_get_args(request);
        header_t *header = request_get_header(request);
        if (true) { //isAjaxRequest
            if (vars_countn(rqargs, "ajax") > 0) {
                wctx->isAjaxRequest = true;
            } else {
                field_t *field = header_get_field(header, "X-Requested-With");
                wctx->isAjaxRequest = boolean(field != NULL && strncmp("XMLHttpRequest", field->value, 14) == 0);
            }
        }
        wctx->ntabs = 0;
        wctx->rsHostId = (vars_countn(rqargs, "hostId") > 0) ? vars_get_value_i(rqargs, "hostId") : -1;
        wctx->page = (vars_countn(rqargs, ARG_PAGE) > 0) ? vars_get_value_i(rqargs, ARG_PAGE) : PN_NONE;
        wctx->action = (vars_countn(rqargs, ARG_ACTION) > 0) ? vars_get_value_i(rqargs, ARG_ACTION) : PA_DEFAULT;
        wctx->channelNum = vars_get_value_i(rqargs, "channelNum");
        if (vars_countn(rqargs, "sort") > 0)
            wctx->sortField = vars_get_value_i(rqargs, "sort");
        else switch (wctx->page) {
                case PN_RESOURCES:
                    wctx->sortField = SF_TITLE;
                    break;
                case PN_CHANNELS:
                    wctx->sortField = SF_CH_NUMBER;
                    break;
                case PN_SEARCHES:
                    wctx->sortField = SF_SEARCH;
                    break;
                default:
                    wctx->sortField = SF_START;
            }
        wctx->sortDirection = (vars_countn(rqargs, "direction") > 0) ? vars_get_value_i(rqargs, "direction") : SD_ASC;
        wctx->bufferLength = 0;
        wctx->buffer = NULL;
        wctx->decoratePage = true;
        wctx->rsType = (vars_countn(rqargs, "type") > 0) ? vars_get_value_i(rqargs, "type") : RT_UNKNOWN;
        if (true) { //path_info
            wctx->rsPath = NULL;
            const char *pi = request_get_path_info(request);
            if (pi && pi[0]) {
                while (pi[0] == '/')
                    pi++;
                if ((strlen(pi) >= 6) && (strncmp("webdav", pi, 6) == 0)) {
                    pi += 6;
                    while (pi[0] == '/') pi++;
                }
                int l = strlen(pi);
                while (l > 0 && pi[l - 1] == '/') l--;
                if (l > 0) {
                    wctx->rsPath = stringIconv(ISO885915, UTF8, pi, l); //TODO comprobar request Content-Encoding
                }
                if (request_get_method(request) == HM_GET && wctx->page == PN_NONE) {
                    wctx->page = PN_RESOURCES;
                    wctx->action = PA_SERVE_RSR;
                }
            } else if (vars_countn(rqargs, "path") > 0) {
                bool isACopy;
                wctx->rsPath = getRequestFieldValue(wctx, rqargs, "path", &isACopy);
                if (!isACopy) {
                    if (wctx->rsPath && wctx->rsPath[0]) {
                        wctx->rsPath = strdup(wctx->rsPath);
                        crit_goto_if(!wctx->rsPath, outOfMemory);
                    } else {
                        wctx->rsPath = NULL;
                    }
                }
            }
        }
        if (true) { //isReload
            field_t *field = header_get_field(header, "Cache-Control");
            wctx->isReload = boolean(field != NULL && strstr(field->value, "max-age=0") != NULL);
        }
        if (true) { //locale
            langId = wctx->conf.langId;
            if ((langId < I18N_MIN) || (langId > I18N_MAX)) {
                field_t *field = header_get_field(header, "Accept-Language");
                if (field != NULL) {
                    int i = 0;
                    for (i = I18N_MIN; i <= I18N_MAX; i++) {
                        if (strncmp(alpha2[i], field->value, 2) == 0) {
                            langId = i;
                            break;
                        }
                    }
                }
            }
            if ((langId < I18N_MIN) || (langId > I18N_MAX)) {
                langId = I18N_MIN;
            }
            setlocale(LC_ALL, locale[langId]);
            setlocale(LC_NUMERIC, "POSIX"); //necesario para usar '.' como separador decimal (requerido en css)
        }
    }
    initMessageList(&wctx->messages);
    if (wctx->page == PN_NONE) {
        wctx->page = PN_INDEX;
    }
    return true;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void freeWCtx(wcontext_t *wctx) {
    closeSvdrpAll();
    if (wctx->buffer != NULL) {
        free(wctx->buffer);
        wctx->bufferLength = 0;
        wctx->buffer = NULL;
        freeMessageList(&wctx->messages);
    }
    if (wctx->conf.bodyFontSize != NULL && wctx->conf.bodyFontSize != webifConf.bodyFontSize) {
        free(wctx->conf.bodyFontSize);
    }
    free(wctx->rsPath);
}

int wctxBufferReserve(wcontext_t *wctx, int length) {
    if ((length + 1) > wctx->bufferLength) {
        crit_goto_if((wctx->buffer = realloc(wctx->buffer, length + 1)) == NULL, outOfMemory);
        wctx->bufferLength = length + 1;
    }
    return wctx->bufferLength;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *wctxBufferPrintf(wcontext_t *wctx, const char *str, ...) {
    char *tmp = NULL;
    va_list ap, apcpy;
    size_t avail, sz, len;
    bool bufferIsInput = (str == wctx->buffer);
    //FIXME check also than wctx->buffer is not a va_arg
    if (bufferIsInput) {
        tmp = wctx->buffer;
        wctx->buffer = NULL;
        wctx->bufferLength = 0;
    }
    va_start(ap, str);
    len = strlen(str) + 10;
    do {
        va_copy(apcpy, ap);
        avail = wctxBufferReserve(wctx, len);
        sz = vsnprintf(wctx->buffer, avail, str, apcpy);
        len = sz;
        va_end(apcpy);
    } while (sz >= avail);
    va_end(ap);
    if (tmp) free(tmp);
    return wctx->buffer;
OutOfMemory:
    exit(EXIT_FAILURE);
}

static char *wctxHtmlEncodeDecodeN(wcontext_t *wctx, const char *str, int n, int flag) {
    char *tmp = NULL;
    if (str == wctx->buffer) {
        crit_goto_if((tmp = strdup(str)) == NULL, OutOfMemory);
        str = tmp;
    }
    int r = (flag == HTMLCPY_DECODE) ? n : 2 * n;
    wctxBufferReserve(wctx, r);
    u_htmlncpy(wctx->buffer, str, n, flag);
    if (tmp) free(tmp);
    return wctx->buffer;
OutOfMemory:
    exit(EXIT_FAILURE);
}

char *wctxHtmlEncode(wcontext_t *wctx, const char *str) {
    return wctxHtmlEncodeDecodeN(wctx, str, (str) ? strlen(str) : 0, HTMLCPY_ENCODE);
}

char *wctxHtmlEncodeN(wcontext_t *wctx, const char *str, int n) {
    return wctxHtmlEncodeDecodeN(wctx, str, n, HTMLCPY_ENCODE);
}

char *wctxHtmlDecode(wcontext_t *wctx, const char *str) {
    return wctxHtmlEncodeDecodeN(wctx, str, (str) ? strlen(str) : 0, HTMLCPY_DECODE);
}

char *wctxUrlEncode(wcontext_t *wctx, const char *str, const char *keep) {
    return wctxUrlEncodeN(wctx, str, (str) ? strlen(str) : 0, keep);
}

char *wctxUrlEncodeN(wcontext_t *wctx, const char *str, int n, const char *keep) {
    char *tmp = NULL;
    if (str == wctx->buffer) {
        crit_goto_if((tmp = strdup(str)) == NULL, OutOfMemory);
        str = tmp;
    }
    wctxBufferReserve(wctx, 2 * n);
    u_urlncpy_encode(wctx->buffer, str, n, keep);
    if (tmp) free(tmp);
    return wctx->buffer;
OutOfMemory:
    exit(EXIT_FAILURE);
}

char *wctxUrlDecode(wcontext_t *wctx, const char *str) {
    char *tmp = NULL;
    if (str == wctx->buffer) {
        crit_goto_if((tmp = strdup(str)) == NULL, OutOfMemory);
        str = tmp;
    }
    int len = strlen(str);
    wctxBufferReserve(wctx, len);
    warn_if(u_urlncpy(wctx->buffer, str, len, URLCPY_DECODE) < len);
    if (tmp) free(tmp);
    return wctx->buffer;
OutOfMemory:
    exit(EXIT_FAILURE);
}

/**
 * Convert to ISO-8859-15 request parameters in  UTF-8 (used in 
 * 
 * @param wctx
 * @param args
 * @param argName
 * @param isACopy
 * @return 
 */
char *getRequestFieldValue(wcontext_t *wctx, vars_t *args, const char *argName, bool *isACopy) {
    const char *argValue = vars_get_value(args, argName);
    if (isACopy) *isACopy = false;
    if (argValue && argValue[0] && wctx->isAjaxRequest) {
        const char *ct = request_get_field_value(wctx->request, "Content-Type");
        if (!ct || !strcasestr(ct, "charset") || strcasestr(ct, charset[UTF8])) {
            char *value = stringIconv(ISO885915, UTF8, argValue, strlen(argValue));
            if (strcmp(value, argValue) == 0) {
                free(value);
            } else {
                if (isACopy) *isACopy = true;
                return value;
            }
        }
    }
    return (char *) argValue;
outOfMemory:
    exit(EXIT_FAILURE);
}

/*
 * Decode special chars as encoded by VDR: '#xx'.
 * It's done in place, as result is always <=len.
 */
char * vdrDecode(char *name) {
    char *s;
    char *d;
    s = d = name;
    while (*s) {
        if (s[0] == '#' && isxdigit(s[1]) && isxdigit(s[2])) {
            *d = htoi(s[1]) << 4 | htoi(s[2]);
            s += 3;
        } else {
            *d = (*s == '_') ? ' ' : (*s == '~') ? '/' : (*s == '|') ? ':' : *s;
            s++;
        }
        d++;
    }
    d[0] = 0;
    return name;
}

char *vdrEncode(char *name) {
    char *s;
    s = name;
    while (*s) {
        *s = /*(*s == ' ') ? '_' :*/ (*s == '/' || *s == '\\') ? '~' : (*s == ':') ? '|' : *s;
        s++;
    }
    return name;
}

void initHtmlDoc(wcontext_t *wctx) {
    response_set_content_type(wctx->response, "text/html; charset=ISO-8859-1");
    response_set_field(wctx->response, "Content-Style-Type", "text/css");
    response_set_field(wctx->response, "Content-Script-Type", "text/javascript");
    response_set_field(wctx->response, "X-UA-Compatible", "IE=Edge");
    wctx->ntabs = 0;
    if (!wctx->isAjaxRequest) {
        bool html5 = true;
        if (html5) {
            WCTX_IPRINT("<!DOCTYPE html>\n");
            WCTX_IPRINTFT("<html lang=\"%s\">\n", 0, 1, alpha2[langId], alpha2[langId]);
        } else {
            WCTX_IPRINT("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
            WCTX_IPRINTFT("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\" lang=\"%s\">\n", 0, 1, alpha2[langId], alpha2[langId]);
        }
    }
}

void initHtmlPage(wcontext_t *wctx, const char *title,
        printHtmlExtraFunction_t *printHtmlHeadExtra, printHtmlExtraFunction_t *printTopInnerHtml) {
    initHtmlDoc(wctx);
    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTT("<head>\n", 0, 1);
        WCTX_IPRINTF("<title>%s</title>\n", title);
        WCTX_IPRINTF("<base href=\"%s\"/>\n", webifConf.useExternalWwwFolder ? "/www2/" : "/");
        if (wctx->decoratePage) {
            printCssLinkTag(wctx, "jquery-ui.css", "screen");
            if (wctx->conf.bodyFontSize && wctx->conf.bodyFontSize[0]) {
                openCssStyleTag(wctx, "screen");
                WCTX_IPRINTF("body { font-size:%s; }", wctx->conf.bodyFontSize);
                closeCssStyleTag(wctx);
            }
            printCssLinkTag(wctx, "screen.css", "screen");
            printCssLinkTag(wctx, "print.css", "print");
            printJavascriptTag(wctx, "jquery.js");
            printJavascriptTag(wctx, "jquery-ui.js");
            printJavascriptTag(wctx, "jquery.cookie.js");
            printJavascriptTag(wctx, "superfish.js");
            printJavascriptTag(wctx, "supersubs.js");
            printJavascriptTag(wctx, "wi.common.js");
            openJavascriptTag(wctx);
            WCTX_IPRINTF("window.Modernizr=function(a,b,c){function x(a){j.cssText=a}function y(a,b){return x(prefixes.join(a+';')+(b||''))}function z(a,b){return typeof a===b}function A(a,b){return!!~(''+a).indexOf(b)}function B(a,b){for(var d in a)if(j[a[d]]!==c)return b=='pfx'?a[d]:!0;return!1}function C(a,b,d){for(var e in a){var f=b[a[e]];if(f!==c)return d===!1?a[e]:z(f,'function')?f.bind(d||b):f}return!1}function D(a,b,c){var d=a.charAt(0).toUpperCase()+a.substr(1),e=(a+' '+n.join(d+' ')+d).split(' ');return z(b,'string')||z(b,'undefined')?B(e,b):(e=(a+' '+o.join(d+' ')+d).split(' '),C(e,b,c))}var d='2.5.3',e={},f=!0,g=b.documentElement,h='modernizr',i=b.createElement(h),j=i.style,k,l={}.toString,m='Webkit Moz O ms',n=m.split(' '),o=m.toLowerCase().split(' '),p={},q={},r={},s=[],t=s.slice,u,v={}.hasOwnProperty,w;!z(v,'undefined')&&!z(v.call,'undefined')?w=function(a,b){return v.call(a,b)}:w=function(a,b){return b in a&&z(a.constructor.prototype[b],'undefined')},Function.prototype.bind||(Function.prototype.bind=function(b){var c=this;if(typeof c!='function')throw new TypeError;var d=t.call(arguments,1),e=function(){if(this instanceof e){var a=function(){};a.prototype=c.prototype;var f=new a,g=c.apply(f,d.concat(t.call(arguments)));return Object(g)===g?g:f}return c.apply(b,d.concat(t.call(arguments)))};return e}),p.borderradius=function(){return D('borderRadius')},p.boxshadow=function(){return D('boxShadow')},p.textshadow=function(){return b.createElement('div').style.textShadow===''},p.csscolumns=function(){return D('columnCount')};for(var E in p)w(p,E)&&(u=E.toLowerCase(),e[u]=p[E](),s.push((e[u]?'':'no-')+u));return x(''),i=k=null,e._version=d,e._domPrefixes=o,e._cssomPrefixes=n,e.testProp=function(a){return B([a])},e.testAllProps=D,g.className=g.className.replace(/(^|\\s)no-js(\\s|$)/,'$1$2')+(f?' js '+s.join(' '):''),e}(this,this.document);\n");
            if (wctx->conf.noCssColumns) {
                WCTX_IPRINTF("$('html').removeClass('csscolumns');\n");
            }
            if (wctx->conf.noTextShadow) {
                WCTX_IPRINTF("$('html').removeClass('textshadow');\n");
            }
            if (wctx->conf.noBoxShadow) {
                WCTX_IPRINTF("$('html').removeClass('boxshadow');\n");
            }
            WCTX_IPRINTT("$.extend(webif.conf,{\n", 0, 1);
            WCTX_IPRINTF("'ajaxDisabled':%s,\n", wctx->conf.ajaxDisabled ? "true" : "false");
            WCTX_IPRINTF("'popupsDisabled':%s,\n", wctx->conf.popupsDisabled ? "true" : "false");
            WCTX_IPRINTF("'currentCssTheme':'%d'\n", wctx->conf.cssTheme);
            WCTX_IPRINTT("});\n", -1, 0);

            WCTX_IPRINTT("$.extend(webif.ajaxCommonData,{\n", 0, 1);
            WCTX_IPRINTF("page:'%d',\n", wctx->page);
            WCTX_IPRINTF("baseAction:'%d',\n", wctx->action);
            WCTX_IPRINTF("sort:'%d',\n", wctx->sortField);
            WCTX_IPRINTF("direction:'%d',\n", wctx->sortDirection);
            WCTX_IPRINTT("});\n", -1, 0);

            WCTX_IPRINTF("$.extend(webif.messages,{close:'%s',cancel:'%s',webSearch:'%s'});\n", tr("close"), tr("cancel"), tr("web.search"));
            if (webifConf.eventSearchUrl && webifConf.eventSearchUrl[0]) {
                WCTX_IPRINTF("$.extend(webif.urls,{eventSearch:'%s'});\n", webifConf.eventSearchUrl);
            }
            closeJavascriptTag(wctx);
        }
    }
    if (printHtmlHeadExtra) {
        printHtmlHeadExtra(wctx, title);
    }
    if (!wctx->isAjaxRequest) {
        if (wctx->decoratePage) {
            openJavascriptTag(wctx);
            WCTX_IPRINT("webif.afterConf();\n");
            closeJavascriptTag(wctx);
        }
        WCTX_IPRINTT("</head>\n", -1, 0);
        WCTX_IPRINTFT("<body id=\"body\" class=\"%s%s js-notReady\">\n", 0, 1
                , cssTheme[wctx->conf.cssTheme]
                , (webifConf.noChannelLogos) ? " noChannelLogos" : ""
                );
        if (wctx->decoratePage) {
            WCTX_IPRINTT("<div id=\"pageDiv\" class=\"level1Div\">\n", 0, 1);
            WCTX_IPRINTT("<div id=\"pageHead\" class=\"level1Head\">\n", 0, 1);
            WCTX_IPRINTT("<table class=\"layout\">\n", 0, 1);
            WCTX_IPRINT("<col class=\"menu\"/>\n");
            WCTX_IPRINT("<col class=\"logo\"/>\n");
            WCTX_IPRINTT("<tbody>\n", 0, 1);
            WCTX_IPRINTT("<tr>\n", 0, 1);
            WCTX_IPRINTT("<td>\n", 0, 1);
            WCTX_IPRINTF("<h1 id=\"pageTitle\">%s</h1>\n", title);
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("<td rowspan=\"2\">\n", 0, 1);
            WCTX_IPRINT("<div id=\"logo\"></div>\n");
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("</tr>\n", -1, 0);
            WCTX_IPRINTT("<tr>\n", 0, 1);
            WCTX_IPRINTT("<td>\n", 0, 1);
            printMenu(wctx);
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("</tr>\n", -1, 0);
            WCTX_IPRINTT("</tbody>\n", -1, 0);
            WCTX_IPRINTT("</table>\n", -1, 0);
            WCTX_IPRINTT("</div>" HTMLCOMENT("pageHead") "\n", -1, 0);
            WCTX_IPRINTT("<div id=\"page\" class=\"level1\">\n", 0, 1);

            WCTX_IPRINTT("<div class=\"level2Div\">\n", 0, 1);
            WCTX_IPRINTT("<h2 class=\"level2Head\">\n", 0, 1);
            if (printTopInnerHtml) {
                printTopInnerHtml(wctx, title);
            } else {
                WCTX_IPRINTF("%s\n", title);
            }
            WCTX_IPRINTT("</h2>\n", -1, 0);
            WCTX_IPRINTT("<div class=\"level2 ui-helper-clearfix\">\n", 0, 1);
        }
    }
}

void finishHtmlPage(wcontext_t *wctx) {
    printMessages(wctx);
    if (!wctx->isAjaxRequest) {
        if (wctx->decoratePage) {
            WCTX_IPRINTT("</div>" HTMLCOMENT("level2") "\n", -1, 0);
            WCTX_IPRINTT("</div>" HTMLCOMENT("level2Div") "\n", -1, 0);
            WCTX_IPRINTT("</div>" HTMLCOMENT("page") "\n", -1, 0);
            WCTX_IPRINTT("<div id=\"pageFoot\" class=\"level1Foot\">\n", 0, 1);
            printCssThemeSelector(wctx, "cssThemeSelector");
            WCTX_IPRINTF("<p>(C) 2008 <a href=\"mailto:atinar1@hotmail.com\">atinar</a></p>\n");
            WCTX_IPRINTF("<p>(C) 2006 open7x0-team</p>\n");
            WCTX_IPRINT("<p>");
            WCTX_PRINTF(tr("version_%s"), webifConf.version);
            WCTX_PRINT("</p>\n");
            WCTX_IPRINTT("</div>" HTMLCOMENT("pageFoot") "\n", -1, 0);
            WCTX_IPRINTT("</div>" HTMLCOMENT("pageDiv") "\n", -1, 0);
            WCTX_IPRINTF("<div id=\"loading\" title=\"%s\"></div>\n", tr("loading"));
        }
        WCTX_IPRINTT("</body>\n", -1, 0);
        WCTX_IPRINTT("</html>\n", -1, 0);
    }
}

void printMenu(wcontext_t *wctx) {
    if (wctx->isAjaxRequest) return;
    int i;
    const char *List = tr("list");
    WCTX_IPRINTFT("<ul id=\"menu\" class=\"sf-menu\"><li%s><a href=\"" URLWI_P "\">%s</a>\n", 0, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_EPG)]
            , PN_EPG, tr("epg"));
    WCTX_IPRINTFT("<ul><li%s><a href=\"" URLWI_P "\">%s</a>\n", 0, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_EPG) && (wctx->channelNum < 1)]
            , PN_EPG, tr("epg.grid"));
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P AMP "channelNum=%d\">%s</a>\n", -1, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_EPG) && (wctx->channelNum > 0)]
            , PN_EPG, (wctx->channelNum > 0) ? wctx->channelNum : 1, tr("epg.list"));
    WCTX_IPRINTT("</li></ul>\n", -1, 0);

    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P "\">%s</a>\n", -1, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_CHANNELS)]
            , PN_CHANNELS, tr("channels"));
    WCTX_IPRINTFT("<ul><li%s><a href=\"" URLWI_P "\">%s</a>\n", 0, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_CHANNELS)]
            , PN_CHANNELS, List);
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P AMP "channelNum=%d\">%s</a>\n", -1, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_LIVE_STREAM)]
            , PN_LIVE_STREAM, (wctx->channelNum > 0) ? wctx->channelNum : 1, tr("liveStream"));
    WCTX_IPRINTFT("</li><li><a href=\"" URLWI_P AMP "type=%d\">%s</a>\n", -1, 1
            , PN_PLAYLIST_CHN, webifConf.playlistType, tr("channel.playlist"));
    WCTX_IPRINTT("</li></ul>\n", -1, 0);

    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P "\">%s</a>\n", -1, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_TIMERS)]
            , PN_TIMERS, tr("timers"));
    WCTX_IPRINTFT("<ul><li%s><a href=\"" URLWI_P "\">%s</a>\n", 0, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_TIMERS) && WCTX_CURRENT_ACTION(PA_DEFAULT))]
            , PN_TIMERS, tr("timers"));
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_PA(AMP) "\">%s</a>\n", -1, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_TIMERS) && WCTX_CURRENT_ACTION(PA_EDIT_FORM))]
            , PN_TIMERS, PA_EDIT_FORM, tr("timer.create"));
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P "\">%s</a>\n", -1, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_SEARCHES) && WCTX_CURRENT_ACTION(PA_DEFAULT))]
            , PN_SEARCHES, tr("searches"));
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_PA(AMP) "\">%s</a>\n", -1, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_SEARCHES) && WCTX_CURRENT_ACTION(PA_EDIT_FORM))]
            , PN_SEARCHES, PA_EDIT_FORM, tr("search.create"));
    WCTX_IPRINTT("</li></ul>\n", -1, 0);

    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P "\">%s</a>\n", -1, 1
            , classCurrent[WCTX_CURRENT_PAGE(PN_RESOURCES)]
            , PN_RESOURCES, tr("recordings"));
    WCTX_IPRINTFT("<ul><li%s><a href=\"" URLWI_P "\">%s</a>\n", 0, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_RESOURCES) && WCTX_CURRENT_ACTION(PA_DEFAULT))]
            , PN_RESOURCES, tr("browse"));
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_PA(AMP) AMP "sort=%d" AMP "direction=%d\">%s (%s)</a>\n", -1, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_RESOURCES) && WCTX_CURRENT_ACTION(PA_LIST) && WCTX_SORT_BY(SF_TITLE))]
            , PN_RESOURCES, PA_LIST, SF_TITLE, SD_ASC, List, tr("title"));
    WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_PA(AMP) AMP "sort=%d" AMP "direction=%d\">%s (%s)</a>\n", -1, 1
            , classCurrent[boolean(WCTX_CURRENT_PAGE(PN_RESOURCES) && WCTX_CURRENT_ACTION(PA_LIST) && WCTX_SORT_BY(SF_START))]
            , PN_RESOURCES, PA_LIST, SF_START, SD_DESC, List, tr("date"));
    WCTX_IPRINTT("</li></ul>\n", -1, 0);

    if (!webifConf.configViewDisabled) {
        if (!webifConf.configChangeDisabled) {
            WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P "\">%s</a>\n", -1, 1
                    , classCurrent[WCTX_CURRENT_PAGE(PN_SETTINGS)]
                    , PN_SETTINGS, tr("setup"));
            WCTX_IPRINT("<ul>");
            for (i = CF_MIN; i <= CF_MAX; i++) {
                WCTX_PRINTFT("<li><a href=\"" URLWI_P AMP "cfgFileId=%d\">%s</a>\n", 1
                        , PN_SETTINGS, i, tr(fileMapping[i].i18nKey));
                WCTX_IPRINTT("</li>", -1, 0);
            }
            WCTX_PRINT("</ul>\n");
        }
        WCTX_IPRINTFT("</li><li%s><a href=\"" URLWI_P "\">%s</a>\n", -1, 1
                , classCurrent[WCTX_CURRENT_PAGE(PN_FILES)]
                , PN_FILES, tr("fileView"));
        WCTX_IPRINT("<ul>");
        int j = 0;
        for (i = 0; i < fileMappingLength; i++) {
            if (fileExists(fileMapping[i].fileName)) {
                WCTX_PRINTFT("<li><a href=\"" URLWI_P AMP "fileNum=%d\">%s</a>\n", 1
                        , PN_FILES, i, tr(fileMapping[i].i18nKey));
                WCTX_IPRINTT("</li>", -1, 0);
                j++;
            }
        }
        if (j > 0) {
            WCTX_PRINTFT("<li><a href=\"" URLWI_PA(AMP) AMP "fileNum=%d\">%s</a>\n", 1
                    , PN_FILES, PA_DOWNLOAD_ALL, i, tr("download_all"));
            WCTX_IPRINTT("</li>", -1, 0);
        }
        WCTX_PRINT("</ul>\n");
    }
    WCTX_IPRINTT("</li>", -1, 0);
    printCommandsMenuLI(wctx);

    WCTX_PRINTFT("<li><a href=\"#\">%s</a>\n", 1, tr("links"));
    WCTX_IPRINTT("<ul><li><a class=\"newWindow\" href=\"http://www.open7x0.org/\">open7x0.org</a>\n", 0, 1);
    WCTX_IPRINTT("</li><li><a class=\"newWindow\" href=\"http://vdr-m7x0.foroactivo.com.es/\">vdr-m7x0.foroactivo.com.es</a>\n", -1, 1);
    WCTX_IPRINTT("</li></ul>\n", -1, 0);
    WCTX_IPRINTT("</li></ul>\n", -1, 0);
}

void openInputTag(wcontext_t *wctx, const char *tag, const char *type,
        const char *id, const char *name, int idx, const char *cssClass,
        char ** ptr_id, bool ownLine) {
    if (!name) {
        name = "paramValue";
    }
    if (idx >= 0) {
        name = wctxBufferPrintf(wctx, "%s_%d", name, idx);
    }
    if (ownLine) {
        WCTX_IPRINTF("<%s ", tag);
    } else {
        WCTX_PRINTF("<%s ", tag);
    }
    WCTX_PRINTF("id=\"%s\" name=\"%s\" ", (id) ? id : name, name);
    if (type != NULL) {
        WCTX_PRINTF("type=\"%s\" ", type);
    }
    if (cssClass != NULL) {
        WCTX_PRINTF("class=\"%s\" ", cssClass);
    }
    if (ptr_id) {
        *ptr_id = strdup((id) ? id : name);
    }
}

void printInput(wcontext_t *wctx, const char *type, const char *id, const char *name, const char *cssClass, int idx, const char *value, int l) {
    openInputTag(wctx, "input", type, id, name, idx, cssClass, NULL, true);
    WCTX_PRINTF("value=\"%s\" size=\"%d\" />\n",
            (value) ? wctxHtmlEncode(wctx, value) : "", (l > 0) ? l : (value) ? strlen(value) + 3 : 10);
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void printSetDatePickerDefaults(wcontext_t *wctx) {
    int i;
    WCTX_IPRINTT("$.datepicker.setDefaults({\n", 0, 1);
    WCTX_IPRINTT("dayNames: [\n", 0, 1);
    for (i = 0; i < 7; i++) {
        WCTX_IPRINTF("\"%s\"%s\n", weekdays[langId][i], (i < 6) ? "," : "");
    }
    WCTX_IPRINTT("],\n", -1, 0); //dayNames
    WCTX_IPRINTT("dayNamesMin: [\n", 0, 1);
    for (i = 0; i < 7; i++) {
        WCTX_IPRINTF("\"%.2s\"%s\n", shortWeekdays[langId][i], (i < 6) ? "," : "");
    }
    WCTX_IPRINTT("],\n", -1, 0); //dayNamesMin
    WCTX_IPRINTF("firstDay: %d,\n", startOfWeek[langId]);
    WCTX_IPRINTT("monthNames: [\n", 0, 1);
    for (i = 0; i < 12; i++) {
        WCTX_IPRINTF("\"%s\"%s\n", monthNames[langId][i], (i < 11) ? "," : "");
    }
    WCTX_IPRINTT("],\n", -1, 0); //monthNames
    WCTX_IPRINTT("monthNamesShort: [\n", 0, 1);
    for (i = 0; i < 12; i++) {

        WCTX_IPRINTF("\"%.3s\"%s\n", monthNames[langId][i], (i < 11) ? "," : "");
    }
    WCTX_IPRINTT("],\n", -1, 0); //monthNamesShort
    WCTX_IPRINTF("buttonText: \"%s\",\n", tr("chooseDay"));
    WCTX_IPRINTT("});\n", -1, 0); //setDefaults
}

void printVDRSelect(wcontext_t *wctx, const char *name, const int hostId) {
    int i;
    hostConf_t *host;
    WCTX_IPRINTFT("<select name=\"%s\" size=\"1\">\n", 0, 1, name);
    for (i = 0, host = webifConf.hosts.entry; i < webifConf.hosts.length; i++, host++) {
        if (host->isVdr) {

            WCTX_IPRINTF("<option value=\"%d\" %s>%d - %s</option>\n"
                    , host->id, selected[boolean(host->id == hostId)], host->id, (host->name[0]) ? host->name : host->ip);
        }
    }
    WCTX_IPRINTT("</select>\n", -1, 0);
}

void printErrorPage(wcontext_t *wctx, int errorn) {
    initHtmlPage(wctx, tr("error"), NULL, NULL);
    WCTX_IPRINT("<p class=\"alert\">");
    if (errorn > 0) {
        switch (errorn) {
            case 401:
                WCTX_PRINT(tr("error.401"));
                break;
            case 404:
                WCTX_PRINTF(tr("error.404_%s"), request_get_client_request(wctx->request));
                break;
            default:
                WCTX_PRINTF(tr("error.%d"), errorn);
        }
    } else {
        WCTX_PRINT(tr("error"));
    }
    WCTX_PRINT("</p>\n");
    finishHtmlPage(wctx);
}

void openJavascriptTag(wcontext_t *wctx) {
    WCTX_IPRINT("<script type=\"text/javascript\">\n");
    WCTX_IPRINTT("//<![CDATA[\n", 0, 1);
}

void closeJavascriptTag(wcontext_t *wctx) {
    WCTX_IPRINTT("//]]>\n", -1, 0);
    WCTX_IPRINT("</script>\n");
}

void openCssStyleTag(wcontext_t *wctx, const char *media) {
    WCTX_IPRINTFT("<style type=\"text/css\" media=\"%s\">\n", 0, 1,
            (media) ? media : "screen");
#ifdef PRINT_XHTML
    WCTX_IPRINTT("<![CDATA[\n", 0, 1);
#endif
}

void closeCssStyleTag(wcontext_t *wctx) {
#ifdef PRINT_XHTML
    WCTX_IPRINTT("]]>", -1, 0);
#endif
    WCTX_IPRINTT("</style>\n", -1, 0);
}

void printJavascriptTag(wcontext_t *wctx, const char *relsrc) {
    WCTX_IPRINTF("<script type=\"text/javascript\" src=\"js/%s\"></script>\n", relsrc);
}

void printCssLinkTag(wcontext_t *wctx, const char *relsrc, const char *media) {
    WCTX_IPRINTF("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/%s\" media=\"%s\" />\n", relsrc, media);
}

void printMessage(wcontext_t * wctx, message_t *message) {
    WCTX_IPRINTFT("<div class=\"messageDiv %s\">\n", 0, 1, messageCssClass[message->type]);
    if (message->title && message->title[0]) {
        WCTX_IPRINTF("<p class=\"messageHead\">%s%s</p>\n", message->title, (message->msg && message->msg[0]) ? ":" : "");
    }
    if (message->msg && message->msg[0]) {
        WCTX_IPRINTF("<p class=\"message\">%s</p>\n"
                , (message->requieresHtmlEncoding) ? wctxHtmlEncode(wctx, message->msg) : message->msg);
    }
    WCTX_IPRINTT("</div>\n", -1, 0);
}

void printMessages(wcontext_t *wctx) {
    if (wctx->messages.length) {
        WCTX_IPRINTT("<div class=\"messagesDiv\">\n", 0, 1);
        WCTX_IPRINTF("<p class=\"messagesHead\">%s</p>\n", tr("messages"));
        WCTX_IPRINTT("<div class=\"messages\">\n", 0, 1);
        int i;
        message_t *message;
        for (i = 0, message = wctx->messages.entry; i < wctx->messages.length; i++, message++) {
            printMessage(wctx, message);
        }
        WCTX_IPRINTT("</div>" HTMLCOMENT("messages") "\n", -1, 0);
        WCTX_IPRINTT("</div>" HTMLCOMENT("messagesDiv") "\n", -1, 0);
    }
}

void printList1TH(wcontext_t *wctx, sortField_t sortField, const char *label) {
    WCTX_IPRINTF("<th class=\"%s\"><a href=\"" URLWI_PA(AMP) AMP "sort=%d" AMP "direction=%d\">"
            STATUS_ICON("sort") "%s"
            "</a></th>\n"
            , cssSortClass[(WCTX_SORT_BY(sortField) ? wctx->sortDirection : SD_NONE) + 1]
            , wctx->page, wctx->action
            , sortField, WCTX_SORT_BY(sortField) ? -wctx->sortDirection : SD_ASC, label);
}

/**
 * Extraer archivo embebido. Devuelve true si al terminar existe dst.
 */
bool extractEmbededFile(const char *src, const char *dst, bool overwrite) {
    bool result = false;
    io_t *io = NULL;
    if (createParentFolders(dst, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
        struct stat st;
        int dst_stat = stat(dst, &st);
        if (dst_stat == 0 && S_ISREG(st.st_mode) && !overwrite) {
            info("%s ya existe. Se conserva el anterior.", dst);
            result = true;
        } else if (((dst_stat == -1) && (errno == ENOENT)) || ((dst_stat == 0) && S_ISREG(st.st_mode) && overwrite)) {
            //Extraer archivo embebido
            embres_t *er;
            if ((emb_lookup(src, &er) != 0) || (er->type != ET_FILE)) {
                warn("%s no es un archivo embebido", src);
            } else {
                embfile_t *ef = (embfile_t *) er;
                dbg_err_if(u_file_open(dst, O_CREAT | O_TRUNC | O_WRONLY, &io));
                dbg_err_if(!io_write(io, (const char*) ef->data, ef->size));
                //dbg_err_if(io_codecs_remove(io)); 
                io_free(io);
                dst_stat = stat(dst, &st);
                if (dst_stat == 0 && S_ISREG(st.st_mode)) {
                    info("%s se ha extraido correctamente", dst);
                    result = true;
                } else {
                    warn("Error de extraccion. stat produce un error inesperado %s", strerror(errno));
                }
            }
        } else {
            warn("stat produce un error inesperado %s", strerror(errno));
        }
    }
    return result;
err:
    if (io) {

        io_free(io);
    }
    return false;
}

bool createParentFolders(const char *filename, mode_t mode) {
    bool result = true;
    if (filename && filename[0] == '/') {
        char *last_slash = strrchr(filename, '/');
        if (last_slash != filename) {
            struct stat st;
            int path_stat;
            char *path = strndup(filename, last_slash - filename);
            char *next_slash = path;
            for (;;) {
                next_slash = strchr(next_slash + 1, '/');
                if (next_slash) *next_slash = 0;
                path_stat = stat(path, &st);
                if (path_stat == 0) {
                    if (!S_ISDIR(st.st_mode)) {
                        result = false;
                        warn("%s no es directorio", path);
                        break;
                    }
                } else {
                    if ((errno != ENOENT) || mkdir(path, mode) != 0) {
                        result = false;
                        warn("error creando ruta %s: %s", path, strerror(errno));
                        break;
                    }
                }
                if (!next_slash) break;
                *next_slash = '/';
            };
            free(path);
        }
    } else {

        warn("%s no es una ruta absoluta", filename);
        result = false;
    }
    return result;
}

bool extractLogosFromFile(wcontext_t *wctx, const char *logos_tgz) {
    bool result = false;
    //TODO interfiere con hooks.
    pid_t pid = vfork();
    if (pid < 0) {
        //fork failure
        warn("fallo la bifurcacion");
        result = false;
    } else if (pid == 0) {
        //child process
        const char *images = "/etc/webif/www/images/";
        if (createParentFolders(images, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
            const char *tar = "/bin/tar";
            execl((char *) tar, (char *) tar, "-C", (char*) images, "-xzf", (char*) logos_tgz, (char*) NULL);
            //exec only returns if call fails
            perror("exec");
            _exit(EXIT_FAILURE);
        } else {
            _exit(-2);
        }
    } else {
        //parent process
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exitStatus;
            if ((exitStatus = WEXITSTATUS(status)) == 0) {
                if (wctx != NULL) {
                    WCTX_MESSAGE(MT_INFO, tr("setup"), "Logos extraídos"); //TODO i18n
                }
                info("Logos extraídos");
                webifConf.noChannelLogos = false;
                result = true;
            } else {
                if (wctx != NULL) {
                    WCTX_MESSAGE(MT_ALERT, tr("setup"), "Error extrayendo logos"); //TODO i18n
                }
                warn("Error descomprimiendo logos");
            }
        } else {
            if (wctx != NULL) {

                WCTX_MESSAGE(MT_ALERT, tr("setup"), "Extracción de logos interrumpida"); //TODO i18n
            }
            warn("child process signaled");
            result = false;
        }
    }
    return result;
}

bool extractLogosFromRequest(wcontext_t *wctx, const char *fieldName) {
    bool result = false;
    char localFilename[U_FILENAME_MAX];
    char clientFilename[U_FILENAME_MAX];
    char mimeType[MIME_TYPE_BUFSZ];
    size_t idx = 0;
    size_t size;
    if (request_get_uploaded_file(wctx->request, fieldName, idx, localFilename, clientFilename, mimeType, &size) == 0) {
        result = extractLogosFromFile(wctx, localFilename);
    }
    return result;
}

char *extendPath(const char *path, const char *name) {
    char *path2 = NULL;
    int lp = (path) ? strlen(path) : 0;
    while (lp > 0 && path[lp] == '/') {
        lp--;
    }
    int ln = (name) ? strlen(name) : 0;
    while (ln > 0 && name[0] == '/') {
        ln--;
        name++;
    }
    if (lp || ln) {
        if (ln == 0) {
            path2 = strndup(path, lp);
        } else if (lp == 0) {
            path2 = strndup(name, ln);
        } else {
            if (asprintf(&path2, "%.*s/%.*s", lp, path, ln, name) < 0) path2 = NULL;
        }
        crit_goto_if(!path2, outOfMemory);
    }
    return path2;
outOfMemory:
    exit(EXIT_FAILURE);
}

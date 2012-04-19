/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>
#include <klone/utils.h>

#include "epg.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"
#include "timers.h"

char *getTimerStrAt(hostConf_t * const host, int id);

void initTimer(vdrTimer_t * const timer) {
    memset(timer, 0, sizeof (vdrTimer_t));
    timer->flags = TF_ACTIVE;
    timer->lifetime = 99;
    timer->priority = 50;
    strcpy(timer->wdays, "-------");
    timer->inConflict = false;
    timer->percent = 100;
}

bool initTimerFromEvent(wcontext_t *wctx, vdrTimer_t * const timer, hostConf_t *host, const int channelNum, const int eventId) {
    initTimer(timer);
    bool result = false;
    if (!host || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, tr("timer.delete.err"), "Host nulo o no es VDR"); //TODO i18n
    } else {
        channelEvents_t channelEvents;
        initChannelEvents(&channelEvents);
        channelEvents.channelNum = channelNum;
        channelEvents.eventId = eventId;
        channelEvents.dumpMode = dmById;
        getChannelEvents(wctx, host, &channelEvents);
        if (channelEvents.length > 0) {
            epgEvent_t *event = channelEvents.entry;
            setFlag(TF_ACTIVE, timer->flags);
            timer->hostId = host->id;
            timer->channelNum = channelNum; //TODO verificar
            timer->type = TT_ONE_TIME;
            timer->title = strdup(event->title);
            setFlag(TF_TITLE, timer->my);
            timer->start = timer->stop = event->start;
            timer->stop += event->duration;
            result = true;
        }
        freeChannelEvents(&channelEvents);
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool makeTime(time_t *time, const char * cdate, int hour, int min) {
    bool result = false;
    struct tm sdate;
    if (strptime(cdate, "%Y-%m-%d", &sdate)) {
        sdate.tm_hour = hour;
        sdate.tm_min = min;
        sdate.tm_sec = 0;
        sdate.tm_wday = 0;
        sdate.tm_yday = 0;
        sdate.tm_isdst = -1;
        *time = mktime(&sdate);
        result = true;
    }
    return result;
}

bool initTimerFromArgs(vdrTimer_t * const timer, vars_t *args, wcontext_t *wctx) {
    bool result = true;
    bool isACopy;
    initTimer(timer);
    if (vars_countn(args, "active") == 0) {
        clearFlag(TF_ACTIVE, timer->flags);
    }
    timer->channelNum = (vars_countn(args, "channelNum") > 0) ? vars_get_value_i(args, "channelNum") : 0;
    timer->type = (vars_countn(args, "type") > 0) ? vars_get_value_i(args, "type") : TT_ONE_TIME;
    int nwdays = 0;
    if (timer->type == TT_REGULAR) {
        size_t i;
        for (i = 0; i < vars_countn(args, "wday"); i++) {
            int wday = vars_geti_value_i(args, "wday", i);
            if (wday > 6 || wday < 0) {
                result = false;
                WCTX_MESSAGE(MT_ALERT, tr("timer.create.ok"), tr("timerErrWrongWday"));
            } else {
                timer->wdays[(wday + 6) % 7] = weekdays[langId][wday][0];
                nwdays++;
            }
        }
    }
    if (
            ((timer->type != TT_ONE_TIME) && (timer->type != TT_REGULAR)) ||
            ((timer->type == TT_ONE_TIME) && (!vars_countn(args, "date"))) ||
            ((timer->type == TT_REGULAR) && (nwdays == 0)) ||
            (!vars_countn(args, "startHour")) ||
            (!vars_countn(args, "startMin")) ||
            (!vars_countn(args, "endHour")) ||
            (!vars_countn(args, "endMin"))
            ) {
        result = false;
        WCTX_MESSAGE(MT_ALERT, tr("timer.create.ok"), tr("timerErrIncompleteArgs"));
    } else {
        char *cdate = NULL;
        timer->lifetime = (vars_countn(args, "lifetime") > 0) ? vars_get_value_i(args, "lifetime") : 99;
        timer->priority = (vars_countn(args, "priority") > 0) ? vars_get_value_i(args, "priority") : 50;
        timer->title = getRequestFieldValue(wctx, args, "title", &isACopy);
        if (isACopy) setFlag(TF_TITLE, timer->my);
        timer->aux = getRequestFieldValue(wctx, args, "aux", &isACopy);
        if (isACopy) setFlag(TF_AUX, timer->my);
        bool addMargin = boolean(vars_countn(args, "addMargin") > 0);
        int startH = vars_get_value_i(args, "startHour");
        int startM = vars_get_value_i(args, "startMin");
        int endH = vars_get_value_i(args, "endHour");
        int endM = vars_get_value_i(args, "endMin");
        if (timer->type == TT_REGULAR) {
            time_t rawtime = time(NULL);
            struct tm *today;
            today = localtime(&rawtime);
            cdate = malloc(12);
            strftime(cdate, 11, "%Y-%m-%d", today);
            if (vars_countn(args, "firstDay")) {
                const char *arg = vars_get_value(args, "firstDay");
                while (*arg == ' ') arg++;
                if (*arg) {
                    timer->firstDay = strdup(arg);
                    dbg("Especificada fecha de inicio de timer: \"%s\"", timer->firstDay);
                }
            }
        } else {
            cdate = strdup(vars_get_value(args, "date"));
        }
        if ((endH < startH) || ((endH == startH) && (endM < startM))) {
            endH += 24; //mktime will normalize this
        }
        if (makeTime(&timer->start, cdate, startH, startM) && makeTime(&timer->stop, cdate, endH, endM)) {
            if (addMargin) {
                timer->start -= webifConf.defaultMarginStart * 60;
                timer->stop += webifConf.defaultMarginStop * 60;
            }
        }
        free(cdate);
    }
    return result;
}

void freeTimer(vdrTimer_t * const timer) {
    if (isFlagSet(TF_TITLE, timer->my)) free(timer->title);
    if (isFlagSet(TF_AUX, timer->my)) free(timer->aux);
    if (isFlagSet(TF_TIMER_STR, timer->my)) free(timer->timerStr);
    free(timer->firstDay);
    initTimer(timer);
}

void initTimerList(timerList_t * const list) {
    list->length = 0;
    list->entry = NULL;
}

void freeTimerList(timerList_t * const list) {
    int i;
    for (i = 0; i < list->length; i++) {
        freeTimer(&(list->entry[i]));
    }
    free(list->entry);
    initTimerList(list);
}

int compareTimers(const vdrTimer_t *ta, const vdrTimer_t *tb, sortField_t sortField, sortDirection_t sortDirection) {
    if (sortDirection == SF_NONE) return 0;
    int result = 0;
    int step;
    for (step = 0; step < 2; step++) {
        switch (sortField) {
            case SF_TM_NUMBER:
                result = sortDirection * (ta->id - tb->id);
                if (result) return result;
                else sortField = SF_HOST;
                break;
            case SF_TITLE:
                result = sortDirection * strcasecmp(ta->title, tb->title);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_TYPE:
                result = sortDirection * (ta->type - tb->type);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_CH_NUMBER:
                result = sortDirection * (ta->channelNum - tb->channelNum);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_CH_NAME:
                result = (ta->channel && tb->channel)
                        ? sortDirection * strcmp(ta->channel->channelName, tb->channel->channelName) : 0;
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_PRIORITY:
                result = sortDirection * (ta->priority - tb->priority);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_LIFETIME:
                result = sortDirection * (ta->lifetime - tb->lifetime);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_MUX:
                result = (ta->channel && tb->channel)
                        ? sortDirection * strcmp(ta->channel->multiplexName, tb->channel->multiplexName) : 0;
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_HOST:
                result = sortDirection * (ta->hostId - tb->hostId);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_START:
            default:
                return sortDirection * (ta->start - tb->start);
        }
    }
    return result;
}

void sortTimerList(timerList_t * const timers, const sortField_t sortField, const sortDirection_t sortDirection) {

    int compare(const void *a, const void *b) {
        return compareTimers((const vdrTimer_t *) a, (const vdrTimer_t *) b, sortField, sortDirection);
    }
    if (timers->length > 0 && sortField != SF_NONE) {
        qsort(timers->entry, timers->length, sizeof (vdrTimer_t), compare);
    }
}

void getTimerListHost(hostConf_t * const host, timerList_t * const timers, channelList_t const *const channels) {
    char *data = execSvdrp(host, "LSTT");
    if (data) {
        char *p;
        for (p = strtok(data, "\r\n"); p != 0; p = strtok(0, "\r\n")) {
            if (atoi(p) == SVDRP_CMD_OK) {
                timers->entry = realloc(timers->entry, (++timers->length) * sizeof (vdrTimer_t));
                crit_goto_if(timers->entry == NULL, outOfMemory);
                vdrTimer_t *tmp = timers->entry + timers->length - 1;
                initTimer(tmp);
                tmp->id = strtol(p + 4, &p, 10);
                tmp->hostId = host->id;
                p += strspn(p, " ");
                tmp->timerStr = strdup(p);
                setFlag(TF_TIMER_STR, tmp->my);
                if (parseTimer(p, tmp)) {
                    //TODO solo funciona si los canales son los mismos en todos los VDRs
                    if (tmp->channelNum > 0 && tmp->channelNum <= channels->length) {
                        tmp->channel = channels->channel + tmp->channelNum - 1;
                    }
                } else {
                    freeTimer(tmp);
                    timers->length--;
                }
            }
        }
        free(data);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void getTimerList(timerList_t * const timers, channelList_t const *const channels, const sortField_t sortField, const sortDirection_t sortDirection) {
    initTimerList(timers);
    int h;
    hostConf_t *host;
    for (h = 0, host = webifConf.hosts.entry; h < webifConf.hosts.length; h++, host++) {
        if (host->isVdr) {
            getTimerListHost(host, timers, channels);
        }
    }
    sortTimerList(timers, sortField, sortDirection);
}

bool addTimer(wcontext_t *wctx, hostConf_t * const host, const char *newTimerStr) {
    bool result = false;
    if (!host || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, tr("timer.create.err"), "Host nulo o no es VDR"); //TODO i18n
    } else {
        char *data = execSvdrp(host, "NEWT %s", newTimerStr);
        if (data) {
            char *p = data;
            int code = strtol(p, &p, 10);
            result = boolean(code == SVDRP_CMD_OK);
            if (wctx && *p && *(++p)) {
                message_t *msg = (result)
                        ? WCTX_MESSAGE(MT_INFO, tr("timer.create.ok"), "%s", p)
                        : WCTX_MESSAGE(MT_ALERT, tr("timer.create.err"), "%s", p);
                if (msg) msg->requieresHtmlEncoding = true;
            }
            free(data);
        }
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool deleteTimer(wcontext_t *wctx, hostConf_t *host, int timerId, const char *oldTimerStr) {
    bool result = false;
    if (!host || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, tr("timer.delete.err"), "Host nulo o no es VDR"); //TODO i18n
    } else if (oldTimerStr == NULL) {
        WCTX_MESSAGE(MT_ALERT, tr("timer.delete.err"), "Faltan argumentos"); //TODO i18n
    } else {
        char *timerStr = getTimerStrAt(host, timerId);
        if (timerStr != NULL) {
            if (strcmp(oldTimerStr, timerStr) == 0) {
                char *data = execSvdrp(host, "DELT %d", timerId);
                if (data != NULL) {
                    char *p = data;
                    int code = strtol(p, &p, 10);
                    result = boolean(code == SVDRP_CMD_OK);
                    if (wctx && *p && *(++p)) {
                        if (result)
                            WCTX_MESSAGE(MT_INFO, tr("timer.delete.ok"), "%s", p);
                        else
                            WCTX_MESSAGE(MT_ALERT, tr("timer.delete.err"), "%s", p);
                    }
                    free(data);
                }
            } else {
                WCTX_MESSAGE(MT_ALERT, tr("timer.delete.err"), "Probablemente la programación ha sido modificada"); //TODO i18n
            }
            free(timerStr);
        } else {
            WCTX_MESSAGE(MT_ALERT, tr("timer.delete.err"), "Programación no existe"); //TODO i18n
        }
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool editTimer(wcontext_t *wctx, hostConf_t * const host, int timerId, const char *oldTimerStr, const char *newTimerStr) {
    if (!host || !oldTimerStr || !newTimerStr) {
        WCTX_MESSAGE(MT_ALERT, tr("timer.update.err"), "Faltan argumentos"); //TODO i18n
        return false;
    }
    if (strcmp(oldTimerStr, newTimerStr) == 0) {
        WCTX_MESSAGE(MT_ALERT, tr("timer.update.err"), "Nada que hacer. No hay cambios"); //TODO i18n
        return true;
    }
    bool result = false;
    char *timerStr = getTimerStrAt(host, timerId);
    if (timerStr != NULL) {
        if (strcmp(oldTimerStr, timerStr) == 0) {
            char *data = execSvdrp(host, "MODT %d %s", timerId, newTimerStr);
            if (data != NULL) {
                char *p = data;
                int code = strtol(p, &p, 10);
                result = boolean(code == SVDRP_CMD_OK);
                if (wctx && *p && *(++p)) {
                    message_t *msg = (result)
                            ? WCTX_MESSAGE(MT_INFO, tr("timer.update.ok"), "%s", p)
                            : WCTX_MESSAGE(MT_ALERT, tr("timer.update.err"), "%s", p);
                    if (msg) msg->requieresHtmlEncoding = true;
                }
                free(data);
            } else {
                WCTX_MESSAGE(MT_ALERT, tr("timer.update.err"), tr("warnSvdrpConnection"));
            }
        } else {
            WCTX_MESSAGE(MT_ALERT, tr("timer.update.err"), "Programación no existe");
        }
        free(timerStr);
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *getTimerStrAt(hostConf_t * const host, int id) {
    char *timerStr = NULL;
    if ((id > 0) && (id < 10000)) {
        char *data = execSvdrp(host, "LSTT %d", id);
        if (data) {
            char *p;
            for (p = strtok(data, "\r\n"); p != 0; p = strtok(0, "\r\n")) {
                if (atoi(p) == SVDRP_CMD_OK) {
                    p += 4;
                    p += strcspn(p, " "); //ID
                    p += strspn(p, " ");
                    timerStr = strdup(p);
                    break;
                }
            }
            free(data);
        }
    }
    return timerStr;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool parseTimer(const char *line, vdrTimer_t * const timer) {
    char *c;
    char *h;
    int l;
    char cdate[50];
    struct tm sdate;

    const char *r = line;
    char *n; //pointer to next field, only to check integrity
    int field;
    for (field = 0; field < 9 && r[0]; field++) {
        l = strcspn(r, ":");
        errno = 0;
        switch (field) {
            case 0:
                timer->flags = (uint) strtol(r, &n, 10);
                if (n[0] != ':') goto wrongFormat;
                break;
            case 1:
                timer->channelNum = strtol(r, NULL, 10);
                break;
            case 2: //store date in cdate
                if (l == 7 || l == 18) {
                    strncpy(timer->wdays, r, 7);
                    timer->wdays[7] = '\0';
                    timer->type = TT_REGULAR;
                    time_t tdate;
                    if (l == 18) {
                        strncpy(cdate, r + 8, 10);
                        cdate[10] = '\0';
                        dbg("Especificada fecha de inicio de timer: \"%s\"", cdate);
                        if (makeTime(&tdate, cdate, 0, 0)) {
                            timer->firstDay = strdup(cdate);
                        }
                    } else {
                        strcpy(cdate, "1970-01-01");
                        tdate = time(NULL);
                        timer->firstDay = NULL;
                    }
                    int wday;
                    for (wday = 0; wday < 7; wday++) {
                        sdate = *localtime(&tdate);
                        if (timer->wdays[(sdate.tm_wday + 6) % 7] != '-') {
                            strftime(cdate, sizeof (cdate) - 1, "%Y-%m-%d", &sdate);
                            break;
                        }
                        tdate += 86400;
                    }
                } else {
                    strncpy(cdate, r, l);
                    cdate[l] = '\0';
                    timer->type = TT_ONE_TIME;
                }
                break;
            case 3: //store start hour in cdate
                h = cdate + strlen(cdate);
                strcpy(h, " 00:00");
                h++;
                /* no break */
            case 4: //store stop hour in cdate
                strncpy(h, r, 2);
                strncpy(h + 3, r + 2, 2);
                if (!strptime(cdate, "%Y-%m-%d %H:%M", &sdate)) {
                    warn("Error converting date \"%s\"", cdate);
                    return false;
                }
                sdate.tm_sec = 0;
                sdate.tm_isdst = -1;
                if (field == 3) {
                    timer->start = mktime(&sdate);
                } else {
                    timer->stop = mktime(&sdate);
                    if (timer->stop < timer->start) {
                        timer->stop += 24 * 60 * 60;
                    }
                }
                break;
            case 5:
                timer->priority = strtol(r, NULL, 10);
                break;
            case 6:
                timer->lifetime = strtol(r, NULL, 10);
                break;
            case 7:
                crit_goto_if((timer->title = strndup(r, l)) == NULL, outOfMemory);
                setFlag(TF_TITLE, timer->my);
                break;
            case 8:
                r += strspn(r, " ");
                if (r[0]) {
                    crit_goto_if((timer->aux = strdup(r)) == NULL, outOfMemory);
                    setFlag(TF_AUX, timer->my);
                }
                break;
        }
        if (errno) {
            warn(strerror(errno));
            goto wrongFormat;
        }
        r += l;
        if (r[0] != ':') break;
        r++;
    }
    return true;
wrongFormat:
    warn("Wrong timer string [%s]", line);
    return false;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *makeRegularTimerStr(uint flags, int channelNum, const char *wdays, const char *firstDay, time_t start, time_t stop
        , int priority, int lifetime, char *title, const char *aux) {
    struct tm t1 = *localtime(&start);
    struct tm t2 = *localtime(&stop);
    char *timerStr;
    crit_goto_if(asprintf(&timerStr, "%d:%d:%.7s%s%s:%02d%02d:%02d%02d:%d:%d:%s:%s"
            , flags
            , channelNum
            , wdays
            , (firstDay) ? "@" : ""
            , (firstDay) ? firstDay : ""
            , t1.tm_hour, t1.tm_min
            , t2.tm_hour, t2.tm_min
            , priority
            , lifetime
            , vdrEncode(title)
            , (aux) ? aux : "") < 1, outOfMemory);
    return timerStr;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *makeOneTimeTimerStr(uint flags, int channelNum, time_t start, time_t stop
        , int priority, int lifetime, char *title, const char *aux) {
    struct tm t1 = *localtime(&start);
    struct tm t2 = *localtime(&stop);
    char *timerStr;
    crit_goto_if(asprintf(&timerStr, "%d:%d:%d-%02d-%02d:%02d%02d:%02d%02d:%d:%d:%s:%s"
            , flags
            , channelNum
            , 1900 + t1.tm_year, t1.tm_mon + 1, t1.tm_mday
            , t1.tm_hour, t1.tm_min
            , t2.tm_hour, t2.tm_min
            , priority
            , lifetime
            , vdrEncode(title)
            , (aux) ? aux : "") < 1, outOfMemory);
    return timerStr;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *makeTimerStr(vdrTimer_t * const timer) {
    return (timer->type == TT_REGULAR)
            ? makeRegularTimerStr(timer->flags, timer->channelNum, timer->wdays, timer->firstDay, timer->start, timer->stop, timer->priority, timer->lifetime, timer->title, timer->aux)
            : makeOneTimeTimerStr(timer->flags, timer->channelNum, timer->start, timer->stop, timer->priority, timer->lifetime, timer->title, timer->aux);
}
//------------------------------

void printTimerForm(wcontext_t *wctx, vdrTimer_t * const timer, channelList_t const *const channels) {
    if (true) { //datepicker
        openJavascriptTag(wctx);
        WCTX_IPRINTT("$(function(){\n", 0, 1);
        WCTX_IPRINTF("$(\"#firstDay\").datepicker({ dateFormat: 'yy-mm-dd' });\n");
        WCTX_IPRINTT("})\n;", -1, 0);
        closeJavascriptTag(wctx);
    }

    WCTX_IPRINTT("<form class=\"form1 timerEdit js-dialogContent\" action=\"" URLWI "\" method=\"post\">\n", 0, 1);
    WCTX_IPRINTT("<fieldset class=\"hidden\">\n", 0, 1);
    {
        WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\"/>\n", PN_TIMERS);
        if (timer->id > 0) {
            WCTX_IPRINTF("<input type=\"hidden\" name=\"timerId\" value=\"%d\"/>\n", timer->id);
            WCTX_IPRINTF("<input type=\"hidden\" name=\"oldTimerStr\" value=\"%s\"/>\n"
                    , wctxHtmlEncode(wctx, timer->timerStr));
        }
    }
    WCTX_IPRINTT("</fieldset>\n", -1, 0);
    WCTX_IPRINTFT("<table class=\"form1\">\n", 0, 1);
    WCTX_IPRINTF("<caption class=\"js-dialogTitle\">%s</caption>\n"
            , tr((timer->id > 0) ? "timer.edit" : "timer.add"));
    WCTX_IPRINTT("<tfoot class=\"form1Foot\">\n", 0, 1);
    WCTX_IPRINTT("<tr class=\"buttons\">\n", 0, 1);
    WCTX_IPRINTT("<td class=\"js-dialogButtons\" colspan=\"2\">\n", 0, 1);
    WCTX_IPRINTF(
            "<button id=\"confirm\" class=\"confirm ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" >"
            "<span class=\"ui-button-icon-primary ui-icon ui-icon-check\"></span>"
            "<span class=\"ui-button-text\">%s</span>"
            "</button>\n"
            , PA_ADD, tr("accept"));
    if (timer->id > 0) {
        WCTX_IPRINTF(
                "<button id=\"delete\" class=\"delete ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" >"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-trash\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</button>\n"
                , PA_DELETE, tr("timer.delete"));
    }
    WCTX_IPRINTT("</td>\n", -1, 0);
    WCTX_IPRINTT("</tr>\n", -1, 0);
    WCTX_IPRINTT("</tfoot>\n", -1, 0);

    WCTX_IPRINTT("<tbody class=\"form1Body\">\n", 0, 1);
    WCTX_IPRINTF(
            "<tr>"
            "<th>%s</th>"
            "<td><input type=\"text\" name=\"title\" size=\"70\" value=\"%s\"/></td>"
            "</tr>\n"
            , tr("title"), (timer->title) ? vdrDecode(timer->title) : "");

    WCTX_IPRINTF(
            "<tr>"
            "<th>%s</th>"
            "<td><input type=\"checkbox\" name=\"active\" value=\"1\"%s/></td>"
            "</tr>\n"
            , tr("active"), checked[isFlagSet(TF_ACTIVE, timer->flags)]
            );

    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTF("<th>%s</th>\n", tr("channel"));
    WCTX_IPRINTT("<td>\n", 0, 1);
    printChannelListSelect(wctx, NULL, "channelNum", channels, timer->channelNum, NULL);
    WCTX_IPRINTT("</td>\n", -1, 0);
    WCTX_IPRINTT("</tr>\n", -1, 0);

    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTF("<th>%s</th>\n", "Host");
    WCTX_IPRINTT("<td>\n", 0, 1);
    if (webifConf.hosts.numVDRs > 1) {
        printVDRSelect(wctx, "hostId", timer->hostId);
    } else {
        WCTX_IPRINTF("<input type=\"text\" name=\"hostId\" class=\"readOnly\" readonly=\"readonly\" value=\"%d\"/>\n", timer->hostId);
    }
    WCTX_IPRINTF("<input type=\"hidden\" name=\"oldHostId\" value=\"%d\"/>\n", timer->hostId);
    WCTX_IPRINTT("</td>\n", -1, 0);
    WCTX_IPRINTT("</tr>\n", -1, 0);

    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTF("<th><input type=\"radio\" name=\"type\" value=\"%d\"%s/>%s</th>\n"
            , TT_ONE_TIME, checked[boolean(timer->type == TT_ONE_TIME)], tr("oneTimeRecording"));
    WCTX_IPRINTT("<td>\n", 0, 1);
    WCTX_IPRINTT("<select name=\"date\" size=\"1\">\n", 0, 1);
    time_t tdate = time(NULL);
    if (timer->start <= 0) {
        timer->start = tdate;
    } else if (tdate > timer->start) {
        tdate = timer->start;
    }
    char cdate[11];
    struct tm sdate;
    bool isThatDay = false;
    bool isAfterThatDay = false;
    int i;
    for (i = 0; i < 14; i++) { //TODO usar datepicker
        sdate = *localtime(&tdate);
        strftime(cdate, 11, "%Y-%m-%d", &sdate);
        if (!isAfterThatDay) {
            isThatDay = sameDay(tdate, timer->start);
            isAfterThatDay = boolean(isThatDay || tdate > timer->start);
        }
        WCTX_IPRINTF("<option value=\"%s\" %s>%s, %s</option>\n"
                , cdate, selected[isThatDay], weekdays[langId][sdate.tm_wday], formatDate(&sdate, false));
        if (isThatDay && (timer->type == TT_ONE_TIME)) {
            //marcar por si se cambia a regular
            timer->wdays[(sdate.tm_wday + 6) % 7] = weekdays[langId][sdate.tm_wday][0];
        }
        isThatDay = false;
        tdate += 24 * 60 * 60;
    }
    WCTX_IPRINTT("</select>\n", -1, 0);
    WCTX_IPRINTT("</td>\n", -1, 0);
    WCTX_IPRINTT("</tr>\n", -1, 0);

    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTF("<th><input type=\"radio\" name=\"type\" value=\"%d\"%s/>%s</th>\n"
            , TT_REGULAR, checked[boolean(timer->type == TT_REGULAR)], tr("regularRecording"));
    WCTX_IPRINTT("<td>\n", 0, 1);
    WCTX_IPRINTT("<fieldset id=\"regularTimerCfg\">\n", 0, 1);
    WCTX_IPRINTT("<fieldset id=\"wdayCfg\">\n", 0, 1);
    for (i = startOfWeek[langId]; i < startOfWeek[langId] + 7; i++) {
        WCTX_IPRINTF("<input type=\"checkbox\" name=\"wday\" value=\"%d\"%s/>%s\n"
                , (i % 7), checked[boolean(timer->wdays[(i + 6) % 7] != '-')], weekdays[langId][i % 7]);
    }
    WCTX_IPRINTT("</fieldset>\n", -1, 0);
    WCTX_IPRINTFT("<label id=\"firstDayLabel\">%s\n", 0, 1, tr("timer.firstDay"));
    WCTX_IPRINTF("<input type=\"text\" id=\"firstDay\" name=\"firstDay\" value=\"%s\" size=\"10\"/>\n", (timer->firstDay == NULL) ? "" : timer->firstDay);
    WCTX_IPRINTT("</label>\n", -1, 0);
    WCTX_IPRINTT("</fieldset>\n", -1, 0);
    WCTX_IPRINTT("</td>\n", -1, 0);
    WCTX_IPRINTT("</tr>\n", -1, 0);

    struct tm start_date = *localtime(&timer->start);
    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTF("<th>%s</th>\n", tr("start"));
    WCTX_IPRINTF(
            "<td>"
            "<input type=\"text\" name=\"startHour\" maxlength=\"2\" size=\"3\" value=\"%02d\"/>&nbsp;:&nbsp;"
            "<input type=\"text\" name=\"startMin\"  maxlength=\"2\" size=\"3\" value=\"%02d\"/>"
            "</td>\n", start_date.tm_hour, start_date.tm_min);
    WCTX_IPRINTT("</tr>\n", -1, 0);

    struct tm end_date = *localtime(&timer->stop);
    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTF("<th>%s</th>\n", tr("end"));
    WCTX_IPRINTF(
            "<td>"
            "<input type=\"text\" name=\"endHour\" maxlength=\"2\" size=\"3\" value=\"%02d\"/>&nbsp;:&nbsp;"
            "<input type=\"text\" name=\"endMin\"  maxlength=\"2\" size=\"3\" value=\"%02d\"/>"
            "</td>\n", end_date.tm_hour, end_date.tm_min);
    WCTX_IPRINTT("</tr>\n", -1, 0);

    WCTX_IPRINTF(
            "<tr>"
            "<th>%s</th>"
            "<td><input type=\"checkbox\" name=\"addMargin\" value=\"%d\" %s/></td>"
            "</tr>\n"
            , tr("margin.add"), boolean(timer->id < 1), checked[boolean(timer->id < 1)]);

    WCTX_IPRINTF(
            "<tr>"
            "<th>%s</th>"
            "<td><input type=\"text\" name=\"priority\" maxlength=\"2\" size=\"3\" value=\"%d\"/></td>"
            "</tr>\n"
            , tr("priority"), timer->priority);

    WCTX_IPRINTF(
            "<tr>"
            "<th>%s</th>"
            "<td><input type=\"text\" name=\"lifetime\" value=\"%d\" maxlength=\"2\" size=\"3\"/></td>"
            "</tr>\n"
            , tr("lifetime"), timer->lifetime);

    if (timer->aux) {
        int l = strlen(timer->aux);
        WCTX_IPRINTF(
                "<tr>"
                "<th>%s</th>"
                "<td><textarea name=\"aux\" spellcheck=\"false\" cols=\"%d\" rows=\"%d\">%s</textarea></td>"
                "</tr>\n"
                , tr("aux"), (l > 70) ? 70 : l, l / 70 + 2, wctxHtmlEncodeN(wctx, timer->aux, l));
    }
    WCTX_IPRINTT("</tbody>\n", -1, 0);

    WCTX_IPRINTT("</table>\n", -1, 0);
    WCTX_IPRINTT("</form>\n", -1, 0);
}

void printTimerBars(wcontext_t *wctx, timerList_t * const timers, const int channelNum
        , const time_t startTime, const time_t duration, const char *TimerEdit, bool wrapPBWithA) {
    const time_t endTime = startTime + duration;
    bool isStart, isEnd, anyMatch;
    anyMatch = false;
    if ((timers->length > 0) && (startTime > 0) && (duration > 0)) {
        int i;
        int mstart = 0;
        int mend = 0;
        vdrTimer_t *timer;
        struct tm sdate;
        int wday;
        for (i = 0, timer = timers->entry; i < timers->length; i++, timer++) {
            if ((timer->type == TT_REGULAR) && (timer->stop < startTime)) {
                sdate = *localtime(&timer->start);
                wday = (sdate.tm_wday + 6) % 7;
                while ((timer->stop < startTime) || (timer->wdays[wday] == '-')) {
                    //TODO daylight savings?
                    wday = (wday + 1) % 7;
                    timer->start += 24 * 60 * 60;
                    timer->stop += 24 * 60 * 60;
                }
            }
        }
        sortTimerList(timers, SF_START, SD_ASC);
        for (i = 0, timer = timers->entry; i < timers->length; i++, timer++) {
            if (((channelNum == 0) || (timer->channelNum == channelNum)) && (timer->start < endTime) && (timer->stop > startTime)) {
                mstart = timer->start - startTime; //margen no cubierto al inicio
                mend = endTime - timer->stop; //margen no cubierto al final
                isStart = boolean(mstart >= 0);
                isEnd = boolean(mend >= 0);
                anyMatch = true;
                double left = (mstart > 0) ? 100.0 * mstart / duration : 0.0;
                double right = (mend > 0) ? 100.0 * mend / duration : 0.0;
                double width = (100.0 - left - right);
                if (width < 0.0) width = 0;
                WCTX_IPRINTFT("<div id=\"timer%d_%d\" class=\"timer%s\">\n", 0, 1
                        , timer->id, ++timer->count, (timer->inConflict) ? " conflict" : "");
                WCTX_IPRINTFT("<a class=\"timerEdit progressbar\""
                        " href=\"" URLWI_PA(AMP) AMP "timerStr=%s" AMP "timerId=%d" AMP "hostId=%d\""
                        " title=\"%s: %s\">\n", 0, 1
                        , PN_TIMERS, PA_EDIT_FORM, wctxUrlEncode(wctx, timer->timerStr, NULL), timer->id, timer->hostId, TimerEdit, vdrDecode(timer->title));
                WCTX_IPRINTF("<span class=\"pb%s%s%s\" style=\"%s:%.2f%%;width:%.2f%%;\">&nbsp;</span>\n"
                        , (isStart) ? " pbstart" : "", (isEnd) ? " pbend" : ""
                        , (isFlagSet(TF_ACTIVE, timer->flags)) ? " pbenabled" : " pbdisabled"
                        , (isStart) ? "right" : "left", (isStart) ? right : left, width);
                WCTX_IPRINTT("</a>\n", -1, 0); //progressbar
                WCTX_IPRINTT("</div>\n", -1, 0); //timer
            }
        }
    }
    //if (!anyMatch) wctx_printf0("&nbsp;\n");
}

void initConflict(conflict_t * const conflict) {
    conflict->when = (time_t) 0;
    conflict->timer = NULL;
    conflict->nconcurrent = 0;
    conflict->pconcurrent = NULL;
}

void freeConflict(conflict_t * const conflict) {
    free(conflict->pconcurrent);
    initConflict(conflict);
}

void initConflictList(conflictList_t * const conflicts) {
    conflicts->length = 0;
    conflicts->entry = NULL;
}

void freeConflictList(conflictList_t * const conflicts) {
    int i;
    for (i = 0; i < conflicts->length; i++) {
        freeConflict(conflicts->entry + i);
    }
    free(conflicts->entry);
    initConflictList(conflicts);
}

bool parseConflict(const char *line, conflict_t * const conflict, timerList_t const * timers) {
    int l;
    int timerId;
    int i;
    vdrTimer_t *ptimer;
    const char *r;
    char *n; //pointer to next field, only to check integrity

    r = line;
    conflict->when = (time_t) strtol(r, &n, 10);
    if (n[0] != ':') goto wrongFormat;
    r = n + 1;

    timerId = strtol(r, &n, 10);
    if (n[0] != '|') goto wrongFormat;
    for (i = 0, ptimer = timers->entry; i < timers->length; i++, ptimer++) {
        if (ptimer->hostId == conflict->hostId && ptimer->id == timerId) {
            ptimer->inConflict = true;
            conflict->timer = ptimer;
            break;
        }
    }
    if (conflict->timer == NULL) {
        warn("Wrong timer %d in conflict", timerId);
        return false;
    }
    r = n + 1;

    conflict->timer->percent = strtol(r, &n, 10);
    if (n[0] != '|') goto wrongFormat;

    //TODO leer concurrentes

    return true;
wrongFormat:
    warn("Wrong conflict string s[%s] r[%s] n[%s]", line, r, n);
    if (errno) {
        warn(strerror(errno));
    }
    return false;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void getConflictListHost(hostConf_t * const host, conflictList_t * const conflicts, timerList_t const * timers) {
    char *data = execSvdrp(host, "PLUG epgsearch LSCC");
    if (data) {
        char *p;
        for (p = strtok(data, "\r\n"); p != 0; p = strtok(0, "\r\n")) {
            if (strtol(p, &p, 10) == SVDRP_PLUG_DEFAULT) {
                p++;
                conflicts->entry = realloc(conflicts->entry, (++conflicts->length) * sizeof (conflict_t));
                crit_goto_if(conflicts->entry == NULL, outOfMemory);
                conflict_t *tmp = conflicts->entry + conflicts->length - 1;
                initConflict(tmp);
                tmp->hostId = host->id;
                if (!parseConflict(p, tmp, timers)) {
                    freeConflict(tmp);
                    conflicts->length--;
                }
            }
        }
        free(data);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void getConflictList(conflictList_t * const conflicts, timerList_t const * timers) {
    int i;
    hostConf_t *host;
    vdrTimer_t *ptimer;
    initConflictList(conflicts);
    for (i = 0, ptimer = timers->entry; i < timers->length; i++, ptimer++) {
        ptimer->inConflict = false;
        ptimer->percent = 100;
    }
    for (i = 0, host = webifConf.hosts.entry; i < webifConf.hosts.length; i++, host++) {
        if (host->isVdr) {
            getConflictListHost(host, conflicts, timers);
        }
    }
}

static void printHtmlHeadExtra(wcontext_t * wctx, const char *title) {
    if (!wctx->isAjaxRequest) {
        printJavascriptTag(wctx, "jquery.form.js");
        printJavascriptTag(wctx, "wi.formHandler.js");
        printJavascriptTag(wctx, "wi.timers.js");
    }
    openJavascriptTag(wctx);
    if (!wctx->isAjaxRequest) {
        printSetDatePickerDefaults(wctx);
        WCTX_IPRINTF("$(function(){webif.timersPageInit();});\n");
    }
    WCTX_IPRINTT("$.extend(webif.messages,{\n", 0, 1);
    WCTX_IPRINTF("timerDeleteConfirm:'%s',\n", tr("timer.delete.confirm"));
    WCTX_IPRINTT("});\n", -1, 0);
    closeJavascriptTag(wctx);
}

void serveTimersPage(wcontext_t *wctx) {
    wctxBufferReserve(wctx, 4096);
    int i = 0;
    vars_t *rqargs = request_get_args(wctx->request);
    int timerId = vars_get_value_i(rqargs, "timerId");
    bool myTimerStr;
    bool myOldTimerStr;
    char *timerStr = getRequestFieldValue(wctx, rqargs, "timerStr", &myTimerStr);
    char *oldTimerStr = getRequestFieldValue(wctx, rqargs, "oldTimerStr", &myOldTimerStr);
    const int hostId = vars_get_value_i(rqargs, "hostId");
    const int oldHostId = vars_get_value_i(rqargs, "oldHostId");
    hostConf_t *host = (vars_countn(rqargs, "hostId") > 0) ? getHost(hostId) : NULL;
    if (!host || !host->isVdr) {
        host = getFirstVdrHost();
    }

    channelList_t vchannels;
    channelList_t *channels = &vchannels;
    initChannelList(channels);

    initHtmlPage(wctx, tr("timers"), printHtmlHeadExtra, NULL);

    if (wctx->isReload && wctx->action == PA_ADD) {
        wctx->action = PA_DEFAULT;
    }
    bool timersHaveChanged = false;
    if (wctx->action == PA_DELETE) {
        if ((timerId > 0) && (oldTimerStr)) {
            timersHaveChanged = deleteTimer(wctx, host, timerId, oldTimerStr);
            if (!timersHaveChanged) {
                wctx->action = PA_EDIT_FORM;
                if (myTimerStr) {
                    free(timerStr);
                }
                timerStr = oldTimerStr;
                myTimerStr = myOldTimerStr;
                myOldTimerStr = false;
            }
        }
        if (!wctx->isAjaxRequest) {
            wctx->action = PA_DEFAULT;
        }
    }
    if (wctx->action == PA_ADD) {
        if (timerStr == NULL) {
            vdrTimer_t timer;
            if (initTimerFromArgs(&timer, rqargs, wctx)) {
                timerStr = makeTimerStr(&timer);
                myTimerStr = true;
            }
            freeTimer(&timer);
        }
        if ((timerId > 0) && (timerStr) && (oldTimerStr) && (hostId == oldHostId)) {
            timersHaveChanged = editTimer(wctx, host, timerId, oldTimerStr, timerStr);
        } else if (timerStr) {
            if (addTimer(wctx, host, timerStr)) {
                timersHaveChanged = true;
                if ((timerId > 0) && (oldTimerStr) && (hostId != oldHostId)) {
                    hostConf_t *oldHost = getHost(oldHostId);
                    deleteTimer(wctx, oldHost, timerId, oldTimerStr);
                }
            }
        }
        if (!wctx->isAjaxRequest) {
            wctx->action = PA_DEFAULT;
        }
    }
    if (timersHaveChanged && wctx->isAjaxRequest) {
        if (vars_countn(rqargs, "epgGridStart")) {
            if (channels->length == 0 && host) {
                getChannelList(host, channels, SF_NONE, SD_NONE);
            }
            if (channels->length > 0) {
                time_t start, end;
                timerList_t timers;
                initTimerList(&timers);
                getTimerList(&timers, channels, wctx->sortField, wctx->sortDirection);
                getEpgGridLimits(wctx, vars_get_value(rqargs, "epgGridStart"), &start, &end);
                printTimersDiv(wctx, AJAX_REPLACE_PREFIX("timers"), &timers, start, end);
                freeTimerList(&timers);
            }
        } else if (vars_countn(rqargs, "epgChannelNum")) {
            const int channelNum = vars_get_value_i(rqargs, "epgChannelNum");
            if (channelNum > 0) {
                if (channels->length == 0 && host) {
                    getChannelList(host, channels, SF_NONE, SD_NONE);
                }
                if (channels->length > 0) {
                    timerList_t timers;
                    initTimerList(&timers);
                    getTimerList(&timers, channels, wctx->sortField, wctx->sortDirection);
                    printChannelEpg(wctx, AJAX_REPLACE_PREFIX("epg"), host, channelNum, &timers);
                    freeTimerList(&timers);
                }
            }
        } else {
            wctx->action = PA_DEFAULT;
        }
    }
    if (wctx->action == PA_EDIT_FORM || wctx->action == PA_TIMER_CREATE_FROM_EVENT) {
        if (channels->length == 0) {
            getChannelList(host, channels, SF_NONE, SD_NONE);
        }
        vdrTimer_t vtimer;
        vdrTimer_t *timer = &vtimer;
        initTimer(timer);
        if (wctx->action == PA_EDIT_FORM) {
            if (timerStr) {
                parseTimer(timerStr, timer);
                timer->timerStr = timerStr;
            } else {
                timer->start = time(NULL);
                timer->type = TT_ONE_TIME;
            }
            timer->id = timerId;
            timer->hostId = hostId;
        } else {
            initTimerFromEvent(wctx, timer, host, vars_get_value_i(rqargs, "channelNum"), vars_get_value_i(rqargs, "eventId"));
        }
        printTimerForm(wctx, timer, channels);
        freeTimer(timer);
    }
    if (wctx->action == PA_DEFAULT) { //list timers
        if (channels->length == 0 && host) {
            getChannelList(host, channels, SF_NONE, SD_NONE);
        }
        if (channels->length > 0) {
            timerList_t timers;
            conflictList_t conflicts;
            getTimerList(&timers, channels, wctx->sortField, wctx->sortDirection);
            getConflictList(&conflicts, &timers);
            bool printHost = boolean(webifConf.hosts.numVDRs > 1);
            WCTX_IPRINTFT("<table id=\"%s\" class=\"list\">\n", 0, 1, AJAX_REPLACE_PREFIX("timers"));
            WCTX_IPRINTF("<caption>%s</caption>\n", tr("timerList"));
            WCTX_IPRINT("<col class=\"number\"/>\n");
            if (printHost) WCTX_IPRINT("<col class=\"number\"/>\n");
            WCTX_IPRINT("<col class=\"tmDate date\"/>\n");
            WCTX_IPRINT("<col class=\"chName\"/>\n");
            WCTX_IPRINT("<col class=\"tmStart hour\"/>\n");
            WCTX_IPRINT("<col class=\"tmEnd hour\"/>\n");
            WCTX_IPRINT("<col class=\"tmName\"/>\n");
            WCTX_IPRINT("<col class=\"mxName\"/>\n");

            WCTX_IPRINTT("<thead class=\"listHead\">\n", 0, 1);
            WCTX_IPRINTT("<tr>\n", 0, 1);
            printList1TH(wctx, SF_TM_NUMBER, "#");
            if (printHost) {
                printList1TH(wctx, SF_HOST, "Host");
            }
            printList1TH(wctx, SF_START, tr("date"));
            printList1TH(wctx, SF_CH_NUMBER, tr("channel"));
            printList1TH(wctx, SF_START, tr("start"));
            printList1TH(wctx, SF_START, tr("end"));
            printList1TH(wctx, SF_TITLE, tr("title"));
            printList1TH(wctx, SF_MUX, tr("channelMux"));
            WCTX_IPRINTT("</tr>\n", -1, 0);
            WCTX_IPRINTT("</thead>\n", -1, 0);

            WCTX_IPRINTT("<tbody class=\"listBody\">\n", 0, 1);
            const char *EditTimer = tr("timer.edit");
            const char *Schedule = tr("epg");
            channel_t noChannel;
            const channel_t *channel;
            noChannel.channelNum = 1;
            noChannel.channelName = "";
            noChannel.multiplexName = "";
            const vdrTimer_t *timer;
            for (i = 0, timer = timers.entry; i < timers.length; i++, timer++) {
                struct tm t1 = *localtime(&timer->start);
                struct tm t2 = *localtime(&timer->stop);
                channel = (timer->channel) ? timer->channel : &noChannel;
                WCTX_IPRINTFT("<tr class=\"%s%s\">\n", 0, 1, isFlagSet(TF_ACTIVE, timer->flags) ? "active" : "inactive", (timer->inConflict) ? " conflict" : "");
                WCTX_IPRINTF("<td class=\"number\">%d</td>\n", timer->id);
                if (printHost) {
                    WCTX_IPRINTF("<td class=\"number\">%d</td>\n", timer->hostId);
                }
                if (timer->type == TT_REGULAR) {
                    WCTX_IPRINTF("<td class=\"date\">%s (%.7s)</td>\n", formatDate(&t1, 0), timer->wdays);
                } else {
                    WCTX_IPRINTF("<td class=\"date\">%s</td>\n", formatDate(&t1, 0));
                }
                WCTX_IPRINTF(
                        "<td>"
                        "<a class=\"channel\" href=\"" URLWI_P AMP "channelNum=%d#timer%d_1\" title=\"%s\">%s</a>"
                        "</td>\n"
                        , PN_EPG, channel->channelNum, timer->id, Schedule, channel->channelName);
                WCTX_IPRINTF("<td class=\"hour\">%02d:%02d</td>\n", t1.tm_hour, t1.tm_min);
                WCTX_IPRINTF("<td class=\"hour\">%02d:%02d</td>\n", t2.tm_hour, t2.tm_min);
                WCTX_IPRINTF(
                        "<td>"
                        "<a class=\"timerEdit title infoToggler\" href=\"" URLWI_PA(AMP) AMP "timerId=%d" AMP "timerStr=%s" AMP "hostId=%d\" title=\"%s\">%s</a>"
                        "</td>\n"
                        , PN_TIMERS, PA_EDIT_FORM, timer->id, wctxUrlEncode(wctx, timer->timerStr, NULL), timer->hostId, EditTimer, vdrDecode(timer->title));
                WCTX_IPRINTF("<td>%s</td>\n", channel->multiplexName);
                WCTX_IPRINTT("</tr>\n", -1, 0);
            }
            WCTX_IPRINTT("</tbody>\n", -1, 0);
            WCTX_IPRINTT("</table>\n", -1, 0);
            freeConflictList(&conflicts);
            freeTimerList(&timers);
        } else {
            WCTX_MESSAGE(MT_ALERT, NULL, tr("warnNoEPG"));
        }
    }
    finishHtmlPage(wctx);
    if (myTimerStr) {
        free(timerStr);
        myTimerStr = false;
    }
    if (myOldTimerStr) {
        free(oldTimerStr);
        myOldTimerStr = false;
    }
    freeChannelList(channels);

}

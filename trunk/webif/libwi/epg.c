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

#include "channels.h"
#include "epg.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"

const char *gridDateFmtI = "%Y-%m-%d %H:%M";
const char *gridDateFmtO = "%Y%m%d%H%M";

void initEpgEvent(epgEvent_t * const event) {
    event->my = 0; //strdup nothing
    event->id = 0;
    event->title = NULL;
    event->desc = NULL;
    event->shortdesc = NULL;
    event->start = 0;
    event->duration = 0;
    event->video = VT_UNKNOWN;
    event->audio.length = 0;
    event->audio.entry = NULL;
}

void initEpgEventFromArgs(wcontext_t *wctx, epgEvent_t * const event, vars_t *args) {
    bool isACopy;
    initEpgEvent(event);
    if (vars_countn(args, "title") > 0) {
        event->title = getRequestFieldValue(wctx, args, "title", &isACopy);
        if (isACopy) setFlag(EFI_TITLE, event->my);
    }
    if (vars_countn(args, "shortdesc") > 0) {
        event->shortdesc = getRequestFieldValue(wctx, args, "shortdesc", &isACopy);
        if (isACopy) setFlag(EFI_SHORTDESC, event->my);
    }
    if (vars_countn(args, "desc") > 0) {
        event->desc = getRequestFieldValue(wctx, args, "desc", &isACopy);
        if (isACopy) setFlag(EFI_DESC, event->my);
    }
    event->video = VT_UNKNOWN;
    event->audio.length = 0;
    event->audio.entry = NULL;
}

void freeEpgEvent(epgEvent_t * const event) {
    if (isFlagSet(EFI_TITLE, event->my))
        free(event->title);
    if (isFlagSet(EFI_DESC, event->my))
        free(event->desc);
    if (isFlagSet(EFI_SHORTDESC, event->my))
        free(event->shortdesc);
    int i;
    //for (i = 0; i < event->audio.length; i++) free(event->audio.entry[i]);
    free(event->audio.entry);
    initEpgEvent(event);
}

void initChannelEvents(channelEvents_t * const list) {
    memset(list, 0, sizeof (channelEvents_t));
    //list->entry=NULL;
    //list->length=0;
    //list->channel=NULL;
}

void freeChannelEvents(channelEvents_t * const list) {
    int i;
    epgEvent_t *entry;
    for (i = 0, entry = list->entry; i < list->length; i++, entry++) {
        freeEpgEvent(entry);
    }
    free(list->entry);
    initChannelEvents(list);
}

void initEvents(events_t * const list) {
    memset(list, 0, sizeof (events_t));
    //list->entry=NULL;
    //list->length=0;
    //list->start=list->end=0;
}

void freeEvents(events_t * const list) {
    int i;
    channelEvents_t *entry;
    for (i = 0, entry = list->entry; i < list->length; i++, entry++) {
        freeChannelEvents(entry);
    }
    free(list->entry);
    initEvents(list);
}

void parseEventLine(char c, char *s, epgEvent_t * const event) {
    int l = strcspn(s, "\n\r");
    audioList_t *al = &event->audio;
    switch (c) {
        case 'E':
        {
            int k;
            for (k = 0; k < 5; k++) {
                s += strspn(s, " ");
                l = strcspn(s, " ");
                switch (k) {
                    case 0: event->id = strtoul(s, NULL, 10);
                        break;
                    case 1: event->start = strtol(s, NULL, 10);
                        break;
                    case 2: event->duration = strtol(s, NULL, 10);
                        break;
                    case 3: event->tableId = strtol(s, NULL, 16);
                        break;
                    case 4: event->version = strtol(s, NULL, 16);
                        break;
                }
                s += l;
            }
            return;
        }
        case 'T':
            crit_goto_if((event->title = strndup(s, l)) == NULL, outOfMemory);
            setFlag(EFI_TITLE, event->my);
            return;
        case 'S':
            crit_goto_if((event->shortdesc = strndup(s, l)) == NULL, outOfMemory);
            setFlag(EFI_SHORTDESC, event->my);
            return;
        case 'D':
            crit_goto_if((event->desc = strndup(s, l)) == NULL, outOfMemory);
            setFlag(EFI_DESC, event->my);
            return;
        case 'X':
            if (l > 5) {
                char *av = strndup(s + 2, l - 2);
                int n;
                audio_t *audio = NULL;
                switch (s[0]) {
                        //TODO no parece que la EPG incluya nada útil aquí
                    case '1': //video
                        n = sscanf(av, "%02X", &event->video);
                        break;
                    case '2': //audio
                        al->entry = realloc(al->entry, (++al->length) * sizeof (audio_t));
                        audio = al->entry + al->length - 1;
                        n = sscanf(av, "%02X %7s", &audio->type, audio->lang);
                        break;
                }
                free(av);
            }
            return;
        default:
            return;
    }
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *parseChannelEvents(char *data, channelEvents_t * const channelEvents) {
    const char *nl = "\n\r";
    char *p, c;
    epgEvent_t *event = NULL;

    p = data;
    while (p && *p) {
        int code = strtol(p, &p, 10);
        if (code == SVDRP_RCRD && p[0] == '-') {
            p++;
            c = p[0];
            if (channelEvents != NULL && strchr("ETSDX", c)) { //lineas del evento
                if (p[0] == 'E') { //inicio del evento
                    channelEvents->entry = realloc(channelEvents->entry, (++channelEvents->length) * sizeof (epgEvent_t));
                    crit_goto_if(channelEvents->entry == NULL, outOfMemory);
                    event = channelEvents->entry + channelEvents->length - 1;
                    initEpgEvent(event);
                }
                if (event != NULL) {
                    p += 2;
                    parseEventLine(c, p, event);
                }
            } else if (c == 'e') { //fin de evento
                event = NULL;
            }
            p += strcspn(p, nl);
            if (*p) p += strspn(p, nl);
            if (c == 'c') { //fin de canal
                return p;
            }
        } else {
            break;
        }
    }
    return NULL;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

/*
 * Secuencia de lineas
 *			215-C : inicio de canal
 *			215-E : inicio de evento
 *			215-e : fin de evento
 *			215-c : fin de canal
 *			221   : cierre de conexion
 */
void parseEvents(char *data, events_t * const events, channelList_t const * const channels) {
    const char *nl = "\n\r";
    char *p, *l;
    int i, k;
    channelEvents_t *channelEvents;
    channel_t *channel;
    epgEvent_t *event;

    events->length = channels->length;
    events->entry = malloc(events->length * sizeof (channelEvents_t));
    crit_goto_if(events->entry == NULL, outOfMemory);
    for (i = 0, channelEvents = events->entry; i < events->length; i++, channelEvents++) {
        initChannelEvents(channelEvents);
    }
    l = data;
    while (l && *l) {
        int code = strtol(l, &p, 10);
        if (code == SVDRP_RCRD && p[0] == '-') {
            p++;
            if (p[0] == 'C') { //inicio de canal
                //seleccionar la channelEvents que corresponda al canal
                channelEvents = NULL;
                channel = NULL;
                event = NULL;
                p += 2;
                k = strcspn(p, " ");
                for (i = 0, channel = channels->channel; i < channels->length; i++, channel++) {
                    if (strncmp(channel->channelId, p, k) == 0) {
                        channelEvents = events->entry + i;
                        channelEvents->channelNum = channel->channelNum;
                        break;
                    }
                }
                l += strcspn(l, nl);
                if (*l) l += strspn(l, nl);
            } else if ((l = parseChannelEvents(l, channelEvents)) != NULL) {
                channelEvents = NULL;
            }
        } else {
            break;
        }
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void getChannelEvents(wcontext_t *wctx, hostConf_t *vdrHost, channelEvents_t * const channelEvents) {
    if (channelEvents->channelNum < 1) {
        warn("channelEvents->channelNum==%d", channelEvents->channelNum);
        return;
    }
    channelEvents->hostId = vdrHost->id;
    char *data = NULL;
    switch (channelEvents->dumpMode) {
        case dmAll:
            data = execSvdrp(vdrHost, "LSTE %d", channelEvents->channelNum);
            break;
        case dmBetween:
            data = execSvdrp(vdrHost, "LSTE %d between %d %d", channelEvents->channelNum, channelEvents->start, channelEvents->end);
            break;
        case dmNowNext:
            data = execSvdrp(vdrHost, "LSTE %d nownext", channelEvents->channelNum);
            break;
        case dmById:
            data = execSvdrp(vdrHost, "LSTE %d withid %d", channelEvents->channelNum, channelEvents->eventId);
            break;
        default:
            return;
    }
    if (data != NULL) {
        char *l = data;
        char *p;
        while (l && *l) {
            int code = strtol(l, &p, 10);
            if (code == SVDRP_RCRD && p[0] == '-') {
                p++;
                if (p[0] != 'C') {
                    l = parseChannelEvents(l, channelEvents);
                } else {
                    l += strcspn(l, "\n\r");
                    if (*l) l += strspn(l, "\n\r");
                }
            } else {
                break;
            }
        }
        free(data);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void getEvents(hostConf_t *vdrHost, events_t * const events, channelList_t const * const channels) {
    if (channels->length < 1) {
        return;
    }
    char *data = NULL;
    switch (events->dumpMode) {
        case dmBetween:
            if (events->channelNum > 0) {
                data = execSvdrp(vdrHost, "LSTE %d between %d %d", events->channelNum, events->start, events->end);
            } else {
                data = execSvdrp(vdrHost, "LSTE between %d %d", events->start, events->end);
            }
            break;
        case dmNowNext:
            if (events->channelNum > 0) {
                data = execSvdrp(vdrHost, "LSTE %d nownext", events->channelNum);
            } else {
                data = execSvdrp(vdrHost, "LSTE nownext");
            }
            break;
        case dmById:
            data = execSvdrp(vdrHost, "LSTE %d withid %d", events->channelNum, events->eventId);
            break;
        default:
            return;
    }
    events->hostId = vdrHost->id;

    if (data != NULL) {
        parseEvents(data, events, channels);
        free(data);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void getEpgGridLimits(wcontext_t* wctx, const char *arg_cstart, time_t *start, time_t *end) {
    time_t now = time(NULL);
    tzset();
    *start = now;
    if (arg_cstart != NULL) {
        if (arg_cstart[0] == 'l') {
            errno = 0;
            *start = (time_t) strtol(arg_cstart + 1, NULL, 10);
            if (errno || *start < 0) {
                warn("El argumento %s no especifica un tiempo correcto. Error %d", arg_cstart, errno);
                *start = now;
            }
        } else {
            struct tm sdate;
            char cstart[17] = "0000-00-00 00:00";
            int i;
            const char *o;
            char *d;
            const char *l = arg_cstart + strlen(arg_cstart);
            for (i = 0, o = arg_cstart, d = cstart; i < 5 && (*o) && o < l; i++) {
                int n = (i) ? 2 : 4;
                int l2 = strlen(o);
                if (l2 < n) d += (n - l2);
                else l2 = n;
                strncpy(d, o, l2);
                //dbg("cstart [%s]",cstart);
                d += n + 1;
                o += n;
            }
            //dbg("Fecha convertida de [%s] a [%s]",arg_cstart,cstart);
            if (strptime(cstart, gridDateFmtI, &sdate) == NULL) {
                warn("No se ha interpretado el argumento %s con el patron %s.", cstart, gridDateFmtI);
                *start = now;
            } else {
                sdate.tm_sec = 0;
                sdate.tm_min -= sdate.tm_min % 15;
                sdate.tm_isdst = -1;
                sdate.tm_zone = NULL;
                *start = mktime(&sdate);
            }
        }
    }
    if (*start < now - wctx->conf.epgGridHours * 60 * 60) {
        *start = now;
    }
    *end = *start + wctx->conf.epgGridHours * 60 * 60;
}

void printChangeDateAnchor(wcontext_t *wctx, char *ctime, bool isPrevious, int hours) {
    WCTX_IPRINTF("<span class=\"change %s ui-state-default\">"
            "<a href=\"" URLWI_P AMP "start=%s\" class=\"ui-icon ui-icon-circle-arrow-%s\" title=\"%s%dh\"> </a>"
            "</span>\n",
            (isPrevious) ? "previous" : "next",
            PN_EPG, ctime, (isPrevious) ? "w" : "e", (isPrevious) ? "-" : "+", hours);
}

void printEpgGrid(wcontext_t *wctx, events_t * const events, channelList_t * const channels, timerList_t * const timers, conflictList_t * const conflicts) {
    channelEvents_t *channelEvents;
    epgEvent_t *event;
    const channel_t *channel;
    int i1, i2;
    time_t eventStart, eventEnd, eventDuration;
    char cdatehour[13];
    char cstart[13];
    char cend[13];
    char cmark[6];
    const char *Epg = tr("epg");
    const char *EventFromSpan_d_d = tr("event.from_%d:%d");
    const char *EventToSpan_d_d = tr("event.to_%d:%d");
    const char *EventDurationSpan_d = tr("event.duration_%d");
    const char *TimerCreate = tr("timer.create");
    const char *SearchCreate = tr("search.create");
    const char *WebSearch = tr("event.web.search");

    time_t gridStart = events->start;
    time_t gridEnd = events->end;
    time_t gridDuration = gridEnd - gridStart;
    time_t aTime;
    time_t now = time(NULL);
    struct tm sstart = *localtime(&gridStart);
    strftime(cstart, 13, gridDateFmtO, &sstart);
    strftime(cend, 13, gridDateFmtO, localtime(&gridEnd));

    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTT("<div id=\"gridDiv\" class=\"gridDiv level3Div\">\n", 0, 1);
        WCTX_IPRINTT("<div id=\"gridHead\" class=\"gridHead level3Head\">\n", 0, 1);
    }
    WCTX_IPRINTT("<span id=\"gridDate\" class=\"gridDate\">\n", 0, 1);
    int hours;
    hours = 24;
    if (gridEnd > now + 24 * 60 * 60) { //one day before
        aTime = gridStart - hours * 60 * 60;
        strftime(cdatehour, 13, gridDateFmtO, localtime(&aTime));
        printChangeDateAnchor(wctx, cdatehour, true, hours);
    }
    WCTX_IPRINTF("<span class=\"day\">%s, %s</span>\n", weekdays[langId][sstart.tm_wday], formatDate(&sstart, false));
    if (1) {//one day after
        aTime = gridStart + 24 * 60 * 60;
        strftime(cdatehour, 13, gridDateFmtO, localtime(&aTime));
        printChangeDateAnchor(wctx, cdatehour, false, hours);
    }
    WCTX_IPRINTT("</span>\n", -1, 0); //gridDate
    if (wctx->isAjaxRequest) {
        WCTX_IPRINTF("<span id=\"datepickerValue\">%.8s</span>\n", cstart);
    } else {
        WCTX_IPRINTF("<input type=\"hidden\" id=\"datepicker\" value=\"%.8s\"/>\n", cstart);
        WCTX_IPRINTT("</div>" HTMLCOMENT("gridHead") "\n", -1, 0);
        WCTX_IPRINTT("<div id=\"grid\" class=\"grid level3\">\n", 0, 1);
        WCTX_IPRINTT("<div class=\"timeMarksDiv\">\n", 0, 1);
    }
    WCTX_IPRINT("<ul class=\"timeMarks\">");
    time_t mark;
    time_t step = 60 * 60;
    bool last = false;

    double left, width;
    for (i1 = 0, mark = gridStart; mark < gridEnd; i1++) {
        strftime(cmark, 6, "%H:%M", localtime(&mark));
        left = ((mark - gridStart)*100.0) / gridDuration;
        WCTX_PRINTF("<li class=\"timeMark\" style=\"left:%.2f%%\">\n", left);
        WCTX_INCTAB();
        if (i1 == 0) {
            WCTX_IPRINT("<span class=\"timeMarkControls\">\n");
            hours = gridDuration / (60 * 60);
            if (gridStart > now) {
                aTime = gridStart - gridDuration;
                strftime(cdatehour, 13, gridDateFmtO, localtime(&aTime));
                printChangeDateAnchor(wctx, cdatehour, true, hours);
            }
            WCTX_IPRINTF("<input type=\"text\" value=\"%s\" id=\"timepicker\" class=\"time\" size=\"5\"/>\n", cmark);
            printChangeDateAnchor(wctx, cend, false, hours);
            WCTX_IPRINT("</span>\n"); //timeMarkControls
        } else {
            WCTX_IPRINTF("<span class=\"time\">%s</span>\n", cmark);
        }
        WCTX_IPRINTT("</li>", -1, 0); //timeMark
        if (mark + step < gridEnd) {
            mark += step;
        } else if (last) {
            break;
        } else {
            mark += step * 75 / 100;
            last = true;
        }
    }
    WCTX_PRINT("</ul>" HTMLCOMENT("timeMarks") "\n");

    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTT("</div>" HTMLCOMENT("timeMarksDiv") "\n", -1, 0);
    }

    printTimersDiv(wctx, "timers", timers, events->start, events->end);

    WCTX_IPRINT("<ul class=\"channels\">");
    for (i1 = 0, channelEvents = events->entry; i1 < events->length; i1++, channelEvents++) {
        if (channelEvents->length > 0) {
            channel = channels->channel + i1;
            WCTX_PRINTT("<li class=\"channel\">\n", 1);
            WCTX_IPRINTT("<div class=\"logo\">\n", 0, 1);
            WCTX_IPRINTFT("<a href=\"" URLWI_P AMP "channelNum=%d\" title=\"%s\">\n", 0, 1,
                    PN_EPG, channel->channelNum, Epg);
            WCTX_IPRINTF("<img class=\"channelLogo\" alt=\"%s\" src=\"/www2/images/logos/%s.png\"/>\n"
                    , wctxChannelDisplayName(wctx, channel), wctxChannelFilename(wctx, channel->channelName, true));
            WCTX_IPRINTT("</a>\n", -1, 0);
            WCTX_IPRINTT("</div>\n", -1, 0);
            WCTX_IPRINTT("<div class=\"channelContent infoContext\">\n", 0, 1);
            WCTX_IPRINT("<ul class=\"channelEvents\">");
            time_t previousEventEnd = gridStart;
            for (i2 = 0, event = channelEvents->entry; i2 < channelEvents->length; i2++, event++) {
                bool started = event->start <= gridStart;
                eventStart = (started) ? gridStart : event->start;
                eventEnd = (event->start + event->duration);
                if (eventEnd > gridEnd) {
                    eventEnd = gridEnd;
                }
                if (i2 < channelEvents->length - 1) {
                    if (eventEnd > event[1].start) {
                        eventEnd = event[1].start;
                    }
                }
                eventDuration = eventEnd - eventStart;
                left = ((eventStart - gridStart)*100.0) / gridDuration;
                width = (eventDuration * 100.0) / gridDuration;
                WCTX_PRINTFT("<li class=\"itemDiv eventDiv%s infoSet\" style=\"left:%.2f%%;width:%.2f%%\">\n", 1
                        , (started) ? " started" : "", left, width);
                printEvent(wctx, event, channelEvents->channelNum
                        , "infoInjector"
                        , EventFromSpan_d_d, EventToSpan_d_d, EventDurationSpan_d
                        , TimerCreate, SearchCreate, WebSearch);
                WCTX_IPRINTT("</li>" HTMLCOMENT("eventDiv"), -1, 0);
                previousEventEnd = eventEnd;
            }
            WCTX_PRINT("</ul>" HTMLCOMENT("channelEvents") "\n");
            WCTX_IPRINT("<div class=\"itemInfo eventInfo infoTarget\" style=\"display:none\"></div>\n");
            WCTX_IPRINTT("</div>" HTMLCOMENT("channelContent") "\n", -1, 0);
            WCTX_IPRINTT("</li>" HTMLCOMENT("channel"), -1, 0);
        }
    }
    WCTX_PRINT("</ul>" HTMLCOMENT("channels") "\n");

    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTT("</div>" HTMLCOMENT("grid") "\n", -1, 0);
        WCTX_IPRINTT("</div>" HTMLCOMENT("gridDiv") "\n", -1, 0);
    }
}

void printChannelEvents(wcontext_t *wctx, const char *divId, channelEvents_t * const events, timerList_t * const timers) {
    if (events->length > 0) {
        epgEvent_t *event;
        int previous_yday = -1;
        const char *EventFromSpan = tr("event.from_%d:%d");
        const char *EventToSpan = tr("event.to_%d:%d");
        const char *EventDurationSpan = tr("event.duration_%d");
        const char *TimerCreate = tr("timer.create");
        const char *TimerEdit = tr("timer.edit");
        const char *SearchCreate = tr("search.create");
        const char *WebSearch = tr("event.web.search");
        int cont = 0;
        WCTX_IPRINTFT("<div id=\"%s\" class=\"level3\">\n", 0, 1, divId);
        for (cont = 0, event = events->entry; cont < events->length; cont++, event++) {
            if (event->start > 0) {
                struct tm sdate = *localtime(&event->start);
                if (previous_yday != sdate.tm_yday) {
                    previous_yday = sdate.tm_yday;
                    if (cont > 0) {
                        WCTX_IPRINTT("</div>" HTMLCOMENT("level4 coll day") "\n", -1, 0);
                        WCTX_IPRINTT("</div>" HTMLCOMENT("level4Div collDiv dayDiv") "\n", -1, 0);
                    }
                    WCTX_IPRINTT("<div class=\"level4Div collDiv dayDiv\">\n", 0, 1);
                    WCTX_IPRINTF("<h4 class=\"level4Head collHead dayHead\">%s %s</h4>\n"
                            , weekdays[langId][sdate.tm_wday], formatDate(&sdate, false));
                    WCTX_IPRINTT("<div class=\"level4 coll day\">\n", 0, 1);
                }
                WCTX_IPRINTT("<div class=\"itemDiv eventDiv infoSet\">\n", 0, 1);
                if (timers != NULL && timers->length > 0) {
                    WCTX_IPRINTT("<div class=\"timers\">\n", 0, 1);
                    printTimerBars(wctx, timers, events->channelNum, event->start, event->duration, TimerEdit, false);
                    WCTX_IPRINTT("</div>" HTMLCOMENT("timers") "\n", -1, 0);
                }
                printEvent(wctx, event, events->channelNum
                        , "infoToggler", EventFromSpan, EventToSpan, EventDurationSpan
                        , TimerCreate, SearchCreate, WebSearch);
                WCTX_IPRINTT("</div>" HTMLCOMENT("itemDiv") "\n", -1, 0);
            }
        }
        WCTX_IPRINTT("</div>" HTMLCOMENT("level4 coll day") "\n", -1, 0);
        WCTX_IPRINTT("</div>" HTMLCOMENT("level4Div collDiv dayDiv") "\n", -1, 0);
    } else {
        WCTX_MESSAGE(MT_ALERT, NULL, tr("warnNoEPG"));
    }
    WCTX_IPRINTT("</div>" HTMLCOMENT("level3") "\n", -1, 0);
}

void printChannelEpg(wcontext_t *wctx, const char *divId, hostConf_t *host, const int channelNum, timerList_t * const timers) {
    channelEvents_t events;
    initChannelEvents(&events);
    events.channelNum = channelNum;
    events.dumpMode = dmAll;
    getChannelEvents(wctx, host, &events);
    printChannelEvents(wctx, divId, &events, timers);
    freeChannelEvents(&events);
}

/**
 * Used in printLivePage to periodically update program info.
 * 
 * @param wctx
 * @param channelEvents
 */
void printChannelEpgTabs(wcontext_t *wctx, channelEvents_t *channelEvents) {
    // Get events for this channel
    if (channelEvents->length > 1) {
        //Refresh header field
        epgEvent_t *nne = channelEvents->entry;
        long int waitSeconds = 0;
        char *refresh;
        if ((channelEvents->length >= 2) && (nne[1].start > 0) && (nne[0].start > 0) && (nne[0].title) && (nne[1].title)) {
            time_t now = time(NULL);
            waitSeconds = nne[1].start - now;
        }
        if (waitSeconds < 5) {
            waitSeconds = 5;
        }
        asprintf(&refresh, "%ld;URL=/" URLWI_PA("&") "&" "channelNum=%d", waitSeconds,
                PN_EPG, PA_GET_EPG_NN, channelEvents->channelNum);
        response_set_field(wctx->response, "Refresh", refresh);
        free(refresh);
    }

    WCTX_IPRINTT("<div id=\"tabs\">\n", 0, 1);

    int i;
    char *title[channelEvents->length];
    epgEvent_t *event;
    for (i = 0, event = channelEvents->entry; i < channelEvents->length; i++, event++) {
        title[i] = NULL;
        if (event->title) {
            if (event->start > 0) {
                struct tm t = *localtime(&event->start);
                asprintf(&title[i], "%02d:%02d", t.tm_hour, t.tm_min);
            } else {
                title[i] = strdup((i == 0) ? tr("Now") : tr("next"));
            }
        }
    }
    WCTX_IPRINTT("<ul id=\"tabsSelector\">\n", 0, 1);
    int c = 0;
    for (i = 0; i < channelEvents->length; i++) {
        if (title[i]) {
            WCTX_PRINTFT("<li><a href=\"#tabs-%d\">%s</a>\n", 1, c++, title[i]);
            WCTX_IPRINTT("</li>", -1, 0);
        }
    }
    WCTX_PRINT("</ul>\n");
    c = 0;
    for (i = 0, event = channelEvents->entry; i < channelEvents->length; i++, event++) {
        if (title[i]) {
            WCTX_IPRINTFT("<div id=\"tabs-%d\" class=\"eventSummary\">\n", 0, 1, c++);
            WCTX_IPRINTF("<h3>%s</h3>\n", title[i]);
            WCTX_IPRINTF("<h4>%s</h4>\n", wctxHtmlEncode(wctx, (event->title) ? event->title : (event->shortdesc && event->desc) ? event->shortdesc : "--"));
            WCTX_IPRINTT("<div class=\"desc\">\n", 0, 1);
            printEventDesc(wctx, (event->desc) ? event->desc : event->shortdesc);
            WCTX_IPRINTT("</div>\n", -1, 0);
            WCTX_IPRINTT("</div>\n", -1, 0);
        }
    }
    for (i = 0; i < channelEvents->length; i++) free(title[i]);
    WCTX_IPRINTT("</div>\n", -1, 0); //tabs
}

void printTimersDiv(wcontext_t *wctx, const char *divId, timerList_t * const timers, const time_t start, const time_t end) {
    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTT("<div class=\"timersDiv\">\n", 0, 1);
    }
    WCTX_IPRINTFT("<div id=\"%s\" class=\"timers\">\n", 0, 1, divId);
    printTimerBars(wctx, timers, 0, start, end - start, tr("timer.edit"), true);
    WCTX_IPRINTT("</div>\n", -1, 0);
    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTT("</div>" HTMLCOMENT("timersDiv") "\n", -1, 0);
    }
}

void printEventDesc(wcontext_t *wctx, char * const desc) {
    if (!desc || strlen(desc) == 0) return;
    char *p = desc;
    char *p0 = p;
    bool inp = false;
    int l = 0;
    while (p[0]) {
        if (p[0] == '|') {
            if (l > 0) {
                WCTX_PRINTN(p0, l);
                l = 0;
            }
            if (inp) {
                WCTX_PRINT("</p>\n");
                inp = false;
            }
            p++;
            p0 = p;
        } else {
            if (!inp) {
                WCTX_IPRINT("<p>");
                inp = true;
            }
            if (p[0] == '&') {
                if (l > 0) {
                    WCTX_PRINTN(p0, l);
                    l = 0;
                }
                if (p[1] == 'a' && p[2] == 'm' && p[3] == 'p' && p[4] == ';') {
                    p += 5;
                } else {
                    p++;
                }
                p0 = p;
                WCTX_PRINT(AMP);
            } else {
                p++;
                l++;
            }
        }
    }
    if (l > 0) WCTX_PRINTN(p0, l);
    if (inp) WCTX_PRINT("</p>\n");
}

void printEvent(wcontext_t *wctx, epgEvent_t * const event, const int channelNum, const char* toggleInfoClass
        , const char *const EventFromSpan_d_d
        , const char *const EventToSpan_d_d
        , const char *const EventDurationSpan_d
        , const char *const TimerCreate
        , const char *const SearchCreate
        , const char *const WebSearch) {
    struct tm startt, endt;
    long int end_time = event->start + event->duration;
    startt = *localtime(&event->start);
    endt = *localtime(&end_time);
    WCTX_IPRINTT("<div class=\"itemHead eventHead\">\n", 0, 1);
    WCTX_IPRINT("<span class=\"fromTo\">");
    WCTX_PRINTF(EventFromSpan_d_d, startt.tm_hour, startt.tm_min);
    WCTX_PRINT("</span>\n");
    WCTX_IPRINTF("<a href=\"#\" class=\"title %s\">%s</a>\n"
            , toggleInfoClass
            , wctxHtmlEncode(wctx, event->title));
    WCTX_IPRINTF("<a href=\"#\" class=\"toggleInfo %s\"><span class=\"more\">+</span><span class=\"less\">-</span></a>\n",
            toggleInfoClass);
    WCTX_IPRINTFT("</div>" HTMLCOMENT("itemHead eventHead") "\n", -1, 0); //itemHead
    WCTX_IPRINTT("<div class=\"item event infoExtra\">\n", 0, 1);
    if (TimerCreate || SearchCreate) {
        WCTX_IPRINT("<ul class=\"controls\">");
        if (TimerCreate) {
            WCTX_PRINTFT(
                    "<li class=\"control\">"
                    "<a class=\"timerCreate ui-button ui-button-icon-only ui-state-default\" href=\""
                    URLWI_PA(AMP) AMP "channelNum=%d" AMP "eventId=%d"
                    "\" title=\"%s\">"
                    "<span class=\"ui-button-icon-primary ui-icon ui-icon-rec\"></span>"
                    "<span class=\"ui-button-text\">%s</span>"
                    "</a>\n", 1
                    , PN_TIMERS, PA_TIMER_CREATE_FROM_EVENT, channelNum, event->id, TimerCreate, TimerCreate);
            WCTX_IPRINTT("</li>", -1, 0);
        }
        if (SearchCreate) {
            WCTX_PRINTFT(
                    "<li class=\"control\">"
                    "<a class=\"searchCreate ui-button ui-button-icon-only ui-state-default\" href=\""
                    URLWI_PA(AMP) AMP "channelNum=%d" AMP "eventId=%d"
                    "\" title=\"%s\">"
                    "<span class=\"ui-button-icon-primary ui-icon ui-icon-search\"></span>"
                    "<span class=\"ui-button-text\">%s</span>"
                    "</a>\n", 1
                    , PN_SEARCHES, PA_SEARCH_CREATE_FROM_EVENT, channelNum, event->id, SearchCreate, SearchCreate);
            WCTX_IPRINTT("</li>", -1, 0);
        }
        if (WebSearch && webifConf.eventSearchUrl && webifConf.eventSearchUrl[0]
                && ((event->title && event->title[0]) || (event->shortdesc && event->shortdesc[0]))) {
            char *url = strdup(webifConf.eventSearchUrl);
            {
                const char *etitle = NULL;
                const char *tag = "{title}";
                while (strstr(url, tag)) {
                    if (!etitle) {
                        etitle = (event->title && event->title[0]) ? wctxUrlEncode(wctx, event->title, NULL) : "";
                    }
                    url = strreplace(url, tag, etitle);
                }
            }
            {
                const char *eshortdesc = NULL;
                const char *tag = "{subtitle}";
                while (strstr(url, tag)) {
                    if (!eshortdesc) {
                        eshortdesc = (event->shortdesc && event->shortdesc[0]) ? wctxUrlEncode(wctx, event->shortdesc, NULL) : "";
                    }
                    url = strreplace(url, tag, eshortdesc);
                }
            }
            WCTX_PRINTFT(
                    "<li class=\"control\">"
                    "<a class=\"webSearch newWindow ui-button ui-button-icon-only ui-state-default\" href=\"%s\" title=\"%s\">"
                    "<span class=\"ui-button-icon-primary ui-icon ui-icon-extlink\"></span>"
                    "<span class=\"ui-button-text\">%s</span>"
                    "</a>\n", 1
                    , url, WebSearch, WebSearch);
            WCTX_IPRINTT("</li>", -1, 0);
            if (url != webifConf.eventSearchUrl) {
                free(url);
            }
        }
        WCTX_PRINT("</ul>\n"); //controls
    }
    WCTX_IPRINTF("<p class=\"title\">%s</p>\n", wctxHtmlEncode(wctx, event->title));
    WCTX_IPRINT("<p class=\"fromTo\">");
    WCTX_PRINTF(EventFromSpan_d_d, startt.tm_hour, startt.tm_min);
    WCTX_PRINTF(EventToSpan_d_d, endt.tm_hour, endt.tm_min);
    WCTX_PRINTF(EventDurationSpan_d, event->duration / 60);
    WCTX_PRINT("</p>\n");
    if (event->shortdesc && event->shortdesc[0]) {
        WCTX_IPRINTF("<div class=\"shortdesc\">%s</div>\n", wctxHtmlEncode(wctx, event->shortdesc));
    }
    if (event->desc && event->desc[0]) {
        WCTX_IPRINTT("<div class=\"desc\">\n", 0, 1);
        printEventDesc(wctx, event->desc);
        WCTX_IPRINTT("</div>\n", -1, 0);
    }
    WCTX_IPRINTFT("</div>" HTMLCOMENT("item event") "\n", -1, 0); //item event
}

struct pageContext_s {
    time_t start;
    time_t end;
    channel_t *channel;
};

static void printHtmlHeadExtra(wcontext_t * wctx, const char *title) {
    struct pageContext_s *pc = wctx->pageContext;
    if (wctx->action == PA_DEFAULT || wctx->action == PA_GET_EPG_BY_ID) {
        bool showGrid = (wctx->channelNum < 1);
        if (showGrid) {
            printCssLinkTag(wctx, "epgGrid.css", "screen");
        } else {
            printCssLinkTag(wctx, "epg.css", "screen");
        }
        printJavascriptTag(wctx, "jquery.form.js");
        if (showGrid) {
            printJavascriptTag(wctx, "wi.epgGrid.js");
        } else {
            printJavascriptTag(wctx, "wi.epg.js");
        }
        printJavascriptTag(wctx, "wi.formHandler.js");
        printJavascriptTag(wctx, "wi.timers.js");
        printJavascriptTag(wctx, "wi.searches.js");

        openJavascriptTag(wctx);
        WCTX_IPRINTT("$(function(){\n", 0, 1);
        if (showGrid) {
            WCTX_IPRINT("webif.epgGridPageInit();\n");
        } else {
            WCTX_IPRINT("webif.epgPageInit();\n");
        }
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.actions,{\n", 0, 1);
        WCTX_IPRINTF("timerCreateFromEvent:%d,\n", PA_TIMER_CREATE_FROM_EVENT);
        WCTX_IPRINTF("searchCreateFromEvent:%d,\n", PA_SEARCH_CREATE_FROM_EVENT);
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.messages,{\n", 0, 1);
        WCTX_IPRINTF("timerCreate:\"%s\",\n", tr("timer.create"));
        WCTX_IPRINTF("timerDeleteConfirm:\"%s\",\n", tr("timer.delete.confirm"));
        WCTX_IPRINTF("searchCreate:\"%s\",\n", tr("search.create"));
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.images,{\n", 0, 1);
        WCTX_IPRINTF("calendar:\"images/calendar.gif\",\n");
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.ajaxCommonData,{\n", 0, 1);
        if (pc->start > 0) {
            WCTX_IPRINTF("epgGridStart : 'l%lu',\n", (unsigned long) pc->start);
        }
        if (pc->channel != NULL) {
            WCTX_IPRINTF("epgChannelNum : %d,\n", pc->channel->channelNum);
        }
        WCTX_IPRINTT("});\n", -1, 0);
        if (showGrid) {
            printSetDatePickerDefaults(wctx);
        }
        closeJavascriptTag(wctx);
    }
}

void printTopInnerHtml(wcontext_t *wctx, const char *title) {
    struct pageContext_s *pageContext = wctx->pageContext;
    if (wctx->action != PA_GET_EPG_NN) {
        if (pageContext->channel == NULL) {
            WCTX_IPRINTF("%s\n", title);
        } else {
            if (!webifConf.noChannelLogos) {
                WCTX_IPRINTF("<img id=\"channelLogo\" alt=\"\" class=\"channelLogo\" src=\"/www2/images/logos/%s.png\"/>\n",
                        wctxChannelFilename(wctx, pageContext->channel->channelName, true));
            }
            WCTX_IPRINTF(tr("epgFor_%s"), pageContext->channel->channelName);
        }
    }
}

void printEpgPage(wcontext_t *wctx) {
    const char *Title = tr("epg");
    vars_t *rqargs = request_get_args(wctx->request);

    hostConf_t *host = getFirstVdrHost();
    int channelNum = wctx->channelNum;
    struct pageContext_s pc;
    pc.channel = NULL;
    pc.start = 0;
    pc.end = 0;
    wctx->pageContext = &pc;
    if (wctx->action == PA_DEFAULT) {
        wctxBufferReserve(wctx, 2048);
        channelList_t channels;
        initChannelList(&channels);
        if (host == NULL) {
            WCTX_MESSAGE(MT_ALERT, NULL, tr("errorNoVdrHost"));
        } else {
            getChannelList(host, &channels, SF_CH_NUMBER, SD_ASC);
            if (channels.length == 0) {
                WCTX_MESSAGE(MT_ALERT, NULL, tr("warnNoEPG"));
            } else if (channelNum > 0 && channelNum <= channels.length) {
                pc.channel = channels.channel + channelNum - 1;
            }
        }
        if (pc.channel == NULL) {
            getEpgGridLimits(wctx, vars_get_value(rqargs, "start"), &pc.start, &pc.end);
        }
        initHtmlPage(wctx, Title, printHtmlHeadExtra, printTopInnerHtml); //call it after channel is set
        if (channels.length > 0) {
            timerList_t timers;
            conflictList_t conflicts;
            getTimerList(&timers, &channels, SF_START, SD_ASC);
            getConflictList(&conflicts, &timers);
            if (pc.channel != NULL) { //channel epg
                WCTX_IPRINTT("<div id=\"epgDiv\" class=\"level3Div\">\n", 0, 1);
                WCTX_IPRINTT("<div class=\"level3Head\">\n", 0, 1);
                WCTX_IPRINTT("<form id=\"channelChange\" action=\"" URLWI "\" method=\"get\">\n", 0, 1);
                WCTX_IPRINTT("<fieldset>\n", 0, 1);
                WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\"/>\n", PN_EPG);
                WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("channel"));
                printChannelListSelect(wctx, "channelNum", "channelNum", &channels, channelNum, "this.form.submit();");
                WCTX_IPRINTT("</label>\n", -1, 0);
                WCTX_IPRINTT("</fieldset>\n", -1, 0);
                WCTX_IPRINTT("</form>\n", -1, 0);
                WCTX_IPRINTT("</div>" HTMLCOMENT("level3Head") "\n", -1, 0);
                printChannelEpg(wctx, "epg", host, channelNum, &timers);
                WCTX_IPRINTT("</div>" HTMLCOMENT("level3Div") "\n", -1, 0);
            } else { // grid
                events_t events;
                initEvents(&events);
                events.start = pc.start;
                events.end = pc.end;
                events.dumpMode = dmBetween;
                getEvents(host, &events, &channels);
                if (events.length == 0) {
                    WCTX_MESSAGE(MT_ALERT, NULL, tr("warnNoEPG"));
                } else {
                    printEpgGrid(wctx, &events, &channels, &timers, &conflicts);
                }
                freeEvents(&events);
            }
            freeConflictList(&conflicts);
            freeTimerList(&timers);
        }
        freeChannelList(&channels);
        finishHtmlPage(wctx);
    } else if (wctx->action == PA_GET_EPG_NN) {
        wctxBufferReserve(wctx, 512);
        wctx->decoratePage = false;
        initHtmlPage(wctx, Title, NULL, NULL);
        if (channelNum > 0) {
            channelEvents_t channelEvents;
            initChannelEvents(&channelEvents);
            channelEvents.channelNum = channelNum;
            channelEvents.dumpMode = dmNowNext;
            getChannelEvents(wctx, host, &channelEvents);
            if (channelEvents.length > 0) {
                printChannelEpgTabs(wctx, &channelEvents);
            }
            freeChannelEvents(&channelEvents);
        }
        finishHtmlPage(wctx);
    } else if (wctx->action == PA_GET_EPG_BY_ID) { //TODO eliminar si no se usa
        wctx->decoratePage = false;
        initHtmlPage(wctx, Title, NULL, NULL);
        int eventId = vars_get_value_i(rqargs, "eventId");
        if (channelNum > 0 && eventId > 0) {
            channelEvents_t channelEvents;
            initChannelEvents(&channelEvents);
            channelEvents.channelNum = channelNum;
            channelEvents.eventId = eventId;
            channelEvents.dumpMode = dmById;
            getChannelEvents(wctx, host, &channelEvents);
            printChannelEvents(wctx, "epgById", &channelEvents, NULL);
            freeChannelEvents(&channelEvents);
        }
        finishHtmlPage(wctx);
    } else {
        response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
    }
}


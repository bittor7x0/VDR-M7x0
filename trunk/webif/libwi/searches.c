/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>
#include <klone/utils.h>

#include "searches.h"
#include "epg.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"

char *getSearchStrAt(hostConf_t * const host, int id);

void initChannelGroup(channelGroup_t * const group) {
    group->hostId = 0;
    group->name = NULL;
    group->length = 0;
    group->entry = NULL;
}

void freeChannelGroup(channelGroup_t * const group) {
    free(group->name);
    initChannelGroup(group);
}

void initChannelGroupList(channelGroupList_t * const groups) {
    groups->length = 0;
    groups->entry = NULL;
}

void freeChannelGroupList(channelGroupList_t * const groups) {
    int i;
    channelGroup_t *group;
    for (i = 0, group = groups->entry; i < groups->length; i++, group++) {
        freeChannelGroup(group);
    }
    free(groups->entry);
    initChannelGroupList(groups);
}

void getChannelGroupList(hostConf_t *host, channelGroupList_t * const groups, channelList_t const *const channels) {
    initChannelGroupList(groups);
    char *data = execSvdrp(host, "PLUG epgsearch LSTC");
    if (data) {
        char *g, *sg;
        channelGroup_t *group;
        for (g = strtok_r(data, "\r\n", &sg); g != 0; g = strtok_r(NULL, "\r\n", &sg)) {
            if (atoi(g) == SVDRP_PLUG_DEFAULT) {
                g += 4;
                groups->entry = realloc(groups->entry, (++groups->length) * sizeof (channelGroup_t));
                crit_goto_if(groups->entry == NULL, outOfMemory);
                group = groups->entry + groups->length - 1;
                initChannelGroup(group);
                group->hostId = host->id;

                channel_t *channel;
                int i;
                char *c, *sc;
                bool found;
                for (c = strtok_r(g, "|", &sc); c != 0; c = strtok_r(NULL, "|", &sc)) {
                    if (group->name == NULL) {
                        crit_goto_if((group->name = strdup(c)) == NULL, outOfMemory);
                    } else {
                        for (i = 0, channel = channels->channel, found = false; i < channels->length && !found; i++, channel++) {
                            if (strcmp(channel->channelId, c) == 0) {
                                group->entry = realloc(group->entry, (++group->length) * sizeof (channel_t*));
                                crit_goto_if(group->entry == NULL, outOfMemory);
                                group->entry[group->length - 1] = channel;
                                found = true;
                            }
                        }
                        if (!found) {
                            warn("Channel %s not found in host %d", c, host->id);
                        }
                    }
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

void printChannelGroupListSelect(wcontext_t *wctx, const char * name, const channelGroupList_t * const groups, const char *groupName) {
    int i;
    channelGroup_t *group;
    WCTX_IPRINTFT("<select name=\"%s\" size=\"1\">\n", 0, 1, name);
    for (i = 0, group = groups->entry; i < groups->length; i++, group++) {
        WCTX_IPRINTF("<option value=\"%s\" %s>%s</option>\n"
                , group->name, selected[boolean(groupName && strcmp(group->name, groupName) == 0)], group->name);
    }
    WCTX_IPRINTT("</select>\n", -1, 0);
}

void initSearch(search_t * const search) {
    memset(search, 0, sizeof (search_t));
    search->id = -1;
    search->searchMode = SEARCH_MODE_SUBSTRING;
    search->priority = 50;
    search->lifetime = 99;
    setFlag(EFI_TITLE, search->compareFlags);
    setFlag(EFI_SHORTDESC, search->compareFlags);
    setFlag(EFI_DESC, search->compareFlags);
    search->marginStart = webifConf.defaultMarginStart;
    search->marginStop = webifConf.defaultMarginStop;
    setFlag(SFL_USE_AS_SEARCH_TIMER, search->flags);
}

bool initSearchFromEvent(wcontext_t *wctx, search_t * const search, hostConf_t *host, const int channelNum, const int eventId) {
    initSearch(search);
    bool result = false;
    if (!host || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, tr("search.create.err"), tr("host.err.nullOrNotVdr"));
    } else {
        channelEvents_t channelEvents;
        initChannelEvents(&channelEvents);
        channelEvents.channelNum = channelNum;
        channelEvents.eventId = eventId;
        channelEvents.dumpMode = dmById;
        getChannelEvents(wctx, host, &channelEvents);
        if (channelEvents.length > 0) {
            epgEvent_t *event = channelEvents.entry;
            //setFlag(TF_ACTIVE,search->flags);
            if (event->shortdesc != NULL && event->shortdesc[0]) {
                crit_goto_if(asprintf(&search->search, "%s %s", event->title, event->shortdesc) < 1, outOfMemory);
                setFlag(ECF_COMPARE_SUBTITLE, search->compareFlags);
            } else {
                crit_goto_if(asprintf(&search->search, "%s", event->title) < 1, outOfMemory);
            }
            setFlag(ECF_COMPARE_TITLE, search->compareFlags);
            search->hostId = host->id;
            search->useChannel = 1;
            search->channelMin = search->channelMax = channelNum - 1;
            time_t ttime = event->start;
            struct tm stime = *localtime(&ttime);
            search->wday = stime.tm_wday;
            search->startTime = stime.tm_hour * 100 + stime.tm_min;
            ttime += event->duration;
            stime = *localtime(&ttime);
            search->stopTime = stime.tm_hour * 100 + stime.tm_min;
            result = true;
        }
        freeChannelEvents(&channelEvents);
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *decodeCatValue(char * catvalue) {
    while (strstr(catvalue, "!^colon^!")) {
        catvalue = strreplace(catvalue, "!^colon^!", ":");
    }
    while (strstr(catvalue, "!^pipe^!")) {
        catvalue = strreplace(catvalue, "!^pipe^!", "|");
    }
    return catvalue;
}

char *encodeCatValue(char * catvalue) {
    while (strstr(catvalue, ":")) {
        catvalue = strreplace(catvalue, ":", "!^colon^!");
    }
    while (strstr(catvalue, "|")) {
        catvalue = strreplace(catvalue, "|", "!^pipe^!");
    }
    return catvalue;
}

bool initSearchFromArgs(search_t * const search, vars_t *args, channelList_t *channels, searchCatList_t * const cats, wcontext_t *wctx) {
    char *arg;
    bool isACopy;
    initSearch(search);
    search->id = (vars_countn(args, "searchId") > 0) ? vars_get_value_i(args, "searchId") : -1;
    arg = getRequestFieldValue(wctx, args, "search", &isACopy);
    info("search:%s", arg);
    if (arg && (!search->search || strcmp(arg, search->search) != 0)) {
        if (isFlagSet(SFI_SEARCH, search->my)) free(search->search);
        if (strchr(arg, ':')) {
            search->search = (isACopy) ? arg : strdup(arg);
            char *c = search->search;
            for (c = strchr(c, ':'); c; c++) *c = '|';
            setFlag(SFI_SEARCH, search->my);
        } else {
            search->search = arg;
            setOrClearFlag(isACopy, SFI_SEARCH, search->my);
        }
    } else if (isACopy) {
        free(arg);
    }
    search->searchMode = vars_get_value_i(args, "searchMode");
    search->fuzzyTolerance = vars_get_value_i(args, "fuzzyTolerance");
    setOrClearFlag(vars_get_value_i(args, "useCase"), SFL_USE_CASE, search->flags);
    setOrClearFlag(vars_get_value_i(args, "compareTitle"), EFI_TITLE, search->compareFlags);
    setOrClearFlag(vars_get_value_i(args, "compareSubtitle"), EFI_SHORTDESC, search->compareFlags);
    setOrClearFlag(vars_get_value_i(args, "compareDescription"), EFI_DESC, search->compareFlags);

    setOrClearFlag(vars_get_value_i(args, "useExtEpgInfo"), SFL_USE_EXT_EPG_INFO, search->flags);
    if (isFlagSet(SFL_USE_EXT_EPG_INFO, search->flags)) {
        if (isFlagSet(SFI_EXT_EPG_VALUES, search->my)) {
            free(search->extEpgValues);
            search->extEpgValues = NULL;
        }
        int i;
        searchCat_t *cat;
        for (cat = cats->entry, i = 0; i < cats->length; cat++, i++) {
            char *catName;
            crit_goto_if(asprintf(&catName, "compare_cat_%d", cat->id) < 0, outOfMemory);
            char *catValue = getRequestFieldValue(wctx, args, catName, &isACopy);
            info("%s:[%s]", catName, catValue);
            free(catName);
            if (catValue && catValue[0]) {
                catValue = encodeCatValue((isACopy) ? catValue : strdup(catValue));
                isACopy = true;
                if (search->extEpgValues == NULL) {
                    crit_goto_if(asprintf(&search->extEpgValues, "%d#%s", cat->id, catValue) < 0, outOfMemory);
                } else {
                    char *tmp = search->extEpgValues;
                    crit_goto_if(asprintf(&search->extEpgValues, "%s|%d#%s", tmp, cat->id, catValue) < 0, outOfMemory);
                }
                setFlag(SFI_EXT_EPG_VALUES, search->my);
            }
            if (catValue && isACopy) {
                free(catValue);
            }
        }
        dbg("extEpgValues.2:[%s]", search->extEpgValues);
    }

    if (vars_get_value_i(args, "useTime")) {
        setFlag(SFL_USE_TIME, search->flags);
        int k;
        for (k = 0; k < 2; k++) {
            arg = (char *) vars_get_value(args, (k == 0) ? "startTime" : "stopTime");
            if (arg) {
                int *time = (k == 0) ? &search->startTime : &search->stopTime;
                char *c;
                int hour = strtol(arg, &c, 10);
                if (errno || hour < 0 || hour > 23 || c[0] != ':') {
                    //TODO imprimir error
                } else {
                    int min = strtol(c + 1, &c, 10);
                    if (errno || min < 0 || min > 59 || c[0]) {
                        //TODO imprimir error
                    } else {
                        (*time) = hour * 100 + min;
                    }
                }
            }
        }
    } else {
        clearFlag(SFL_USE_TIME, search->flags);
        search->startTime = 0;
        search->stopTime = 2359;
    }
    setOrClearFlag(vars_get_value_i(args, "useDuration"), SFL_USE_DURATION, search->flags);
    search->minDuration = vars_get_value_i(args, "minDuration");
    search->maxDuration = vars_get_value_i(args, "maxDuration");

    search->wday = 0;
    if (vars_get_value_i(args, "useWday")) {
        setFlag(SFL_USE_WDAY, search->flags);
        ;
        int ndays = vars_countn(args, "wday");
        if (ndays > 1) {
            int i, wday;
            uint wdayFlag;
            for (i = 0; i < ndays; i++) {
                wday = vars_geti_value_i(args, "wday", i);
                if (wday > 6 || wday < 0) {
                    WCTX_MESSAGE(MT_ALERT, tr("search.create.err"), tr("search.errWrongWday"));
                } else {
                    wdayFlag = 1 << wday;
                    setFlag(wdayFlag, search->wday);
                }
            }
            search->wday = -search->wday;
        } else {
            search->wday = vars_get_value_i(args, "wday");
        }
    } else {
        clearFlag(SFL_USE_WDAY, search->flags);
    }

    search->useChannel = vars_get_value_i(args, "useChannel");
    search->channelMin = vars_get_value_i(args, "channelMin") - 1;
    search->channelMax = vars_get_value_i(args, "channelMax") - 1;
    if (isFlagSet(SFI_CHANNEL_GROUP, search->my)) free(search->channelGroup);
    if (search->useChannel == 2) {
        search->channelGroup = getRequestFieldValue(wctx, args, "channelGroup", &isACopy);
        setOrClearFlag(isACopy, SFI_CHANNEL_GROUP, search->my);
    } else {
        search->channelGroup = NULL;
        clearFlag(SFI_CHANNEL_GROUP, search->my);
    }

    //useAsSearchTimer-start
    setOrClearFlag(vars_get_value_i(args, "useAsSearchTimer"), SFL_USE_AS_SEARCH_TIMER, search->flags);
    //action-start
    search->action = vars_get_value_i(args, "searchAction");
    //record-start
    if (isFlagSet(SFI_DIRECTORY, search->my)) free(search->directory);
    search->directory = getRequestFieldValue(wctx, args, "directory", &isACopy);
    info("directorio:%s", search->directory);
    setOrClearFlag(isACopy, SFI_DIRECTORY, search->my);
    setOrClearFlag(vars_get_value_i(args, "useEpisode"), SFL_USE_EPISODE, search->flags);
    search->priority = vars_get_value_i(args, "priority");
    search->lifetime = vars_get_value_i(args, "lifetime");
    search->marginStart = vars_get_value_i(args, "marginStart");
    search->marginStop = vars_get_value_i(args, "marginStop");
    //repeats-start
    setOrClearFlag(vars_get_value_i(args, "repeatsAvoid"), SFL_AVOID_REPEATS, search->flags);
    search->allowedRepeats = vars_get_value_i(args, "repeatsAllowed");
    search->repeatsCompareFlags = 0;
    setOrClearFlag(vars_get_value_i(args, "repeatsCompareTitle"), EFI_TITLE, search->repeatsCompareFlags);
    setOrClearFlag(vars_get_value_i(args, "repeatsCompareSubtitle"), EFI_SHORTDESC, search->repeatsCompareFlags);
    setOrClearFlag(vars_get_value_i(args, "repeatsCompareDescription"), EFI_DESC, search->repeatsCompareFlags);
    //repeats-end
    //record-end
    //switch-start
    search->switchMinsBefore = vars_get_value_i(args, "switchMinsBefore");
    setOrClearFlag(vars_get_value_i(args, "switchUnmuteSound"), SFL_UNMUTE_SOUND_ON_SWITCH, search->flags);
    //swit-end
    //action-end
    //useAsSearchTimer-end
    return true;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void freeSearch(search_t * const search) {
    if (isFlagSet(SFI_SEARCH_STR, search->my)) free(search->searchStr);
    if (isFlagSet(SFI_SEARCH, search->my)) free(search->search);
    if (isFlagSet(SFI_CHANNEL_GROUP, search->my)) free(search->channelGroup);
    if (isFlagSet(SFI_DIRECTORY, search->my)) free(search->directory);
    if (isFlagSet(SFI_EXT_EPG_VALUES, search->my)) free(search->extEpgValues);
    if (isFlagSet(SFI_BLACKLIST_IDS, search->my)) free(search->blacklistIds);
    initSearch(search);
}

void initSearchList(searchList_t * const list) {
    list->length = 0;
    list->entry = NULL;
}

void freeSearchList(searchList_t * const list) {
    int i;
    for (i = 0; i < list->length; i++) {
        freeSearch(&(list->entry[i]));
    }
    free(list->entry);
    initSearchList(list);
}

void getSearchListHost(hostConf_t * const host, searchList_t * const searches, channelList_t const *const channels) {
    char *data = execSvdrp(host, "PLUG epgsearch LSTS");
    if (data) {
        char *p;
        search_t *tmp;
        for (p = strtok(data, "\r\n"); p != 0; p = strtok(0, "\r\n")) {
            if (atoi(p) == SVDRP_PLUG_DEFAULT) {
                p += 4;
                searches->entry = realloc(searches->entry, (++searches->length) * sizeof (search_t));
                crit_goto_if(searches->entry == NULL, outOfMemory);
                tmp = searches->entry + searches->length - 1;
                initSearch(tmp);
                tmp->hostId = host->id;
                tmp->searchStr = strdup(p);
                setFlag(SFI_SEARCH_STR, tmp->my);
                if (parseSearch(p, tmp, channels)) {
                    //TODO asignar canales
                } else {
                    freeSearch(tmp); //TODO decidir cual de las dos veces
                    searches->length--;
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

int compareSearches(const search_t *sa, const search_t *sb, sortField_t sortField, sortDirection_t sortDirection) {
    if (sortDirection == SF_NONE) return 0;
    int result = 0;
    int step;
    for (step = 0; step < 2; step++) {
        switch (sortField) {
            case SF_PRIORITY:
                result = sortDirection * (sa->priority - sb->priority);
                if (result) return result;
                else sortField = SF_SEARCH;
                break;
            case SF_LIFETIME:
                result = sortDirection * (sa->lifetime - sb->lifetime);
                if (result) return result;
                else sortField = SF_SEARCH;
                break;
            case SF_HOST:
                result = sortDirection * (sa->hostId - sb->hostId);
                if (result) return result;
                else sortField = SF_SEARCH;
                break;
            case SF_DIRECTORY:
                result = sortDirection * strcmp(sa->directory, sb->directory);
                dbg("sa.dir [%s] sb.dir [%s] result %d", sa->directory, sb->directory, result);
                if (result) return result;
                else sortField = SF_SEARCH;
                break;
            case SF_SEARCH:
                result = sortDirection * strcmp(sa->search, sb->search);
                if (result) return result;
                else sortField = SF_START;
                break;
            case SF_START:
                result = sortDirection * (sa->startTime - sb->startTime);
                if (result) return result;
                else sortField = SF_SEARCH;
                break;
        }
    }
    return result;
}

void sortSearchList(searchList_t * const searches, const sortField_t sortField, const sortDirection_t sortDirection) {

    int compare(const void *a, const void *b) {
        return compareSearches((const search_t *) a, (const search_t *) b, sortField, sortDirection);
    }
    if (searches->length > 0 && sortField != SF_NONE) {
        qsort(searches->entry, searches->length, sizeof (search_t), compare);
    }
}

void getSearchList(searchList_t * const searches, channelList_t const *const channels, const sortField_t sortField, const sortDirection_t sortDirection) {
    initSearchList(searches);
    int h;
    hostConf_t *host;
    for (h = 0, host = webifConf.hosts.entry; h < webifConf.hosts.length; h++, host++) {
        if (host->isVdr) {
            getSearchListHost(host, searches, channels);
        }
    }
    sortSearchList(searches, sortField, sortDirection);
}

bool addSearch(wcontext_t *wctx, hostConf_t * const host, const char *newSearchStr) {
    bool result = false;
    if (!host || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, tr("search.create.err"), tr("host.err.nullOrNotVdr"));
    } else {
        char *data = execSvdrp(host, "PLUG epgsearch NEWS %s", newSearchStr);
        if (data) {
            char *p = data;
            int code = strtol(p, &p, 10);
            result = boolean(code == SVDRP_PLUG_DEFAULT);
            if (wctx && *p && *(++p)) {
                message_t *msg = (result)
                        ? WCTX_MESSAGE(MT_INFO, tr("search.create.ok"), "%s", p)
                        : WCTX_MESSAGE(MT_ALERT, tr("search.create.err"), "%s", p);
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

bool deleteSearch(wcontext_t *wctx, hostConf_t *host, int id, const char *oldSearchStr, bool andTimers) {
    bool result = false;
    if (!host || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, tr("search.delete.err"), tr("host.err.nullOrNotVdr"));
    } else if (oldSearchStr == NULL) {
        WCTX_MESSAGE(MT_ALERT, tr("search.delete.err"), "Faltan argumentos"); //TODO i18n
    } else {
        char *searchStr = getSearchStrAt(host, id);
        if (searchStr != NULL) {
            if (strcmp(oldSearchStr, searchStr) == 0) {
                char *data = execSvdrp(host, "PLUG epgsearch DELS %d%s", id, (andTimers) ? " DELT" : "");
                if (data != NULL) {
                    char *p = data;
                    int code = strtol(p, &p, 10);
                    result = boolean(code == SVDRP_PLUG_DEFAULT);
                    if (wctx && *p && *(++p)) {
                        message_t *msg = (result)
                                ? WCTX_MESSAGE(MT_INFO, tr("search.delete.ok"), "%s", p)
                                : WCTX_MESSAGE(MT_ALERT, tr("search.delete.err"), "%s", p);
                        if (msg) msg->requieresHtmlEncoding = true;
                    }
                    free(data);
                }
            } else {
                WCTX_MESSAGE(MT_ALERT, tr("search.delete.err"), "Probablemente la búsqueda ha sido modificada"); //TODO i18n
            }
            free(searchStr);
        } else {
            WCTX_MESSAGE(MT_ALERT, tr("search.delete.err"), "La búsqueda no existe"); //TODO i18n
        }
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool editSearch(wcontext_t *wctx, hostConf_t * const host, int id, const char *oldSearchStr, const char *newSearchStr) {
    if (!host || !oldSearchStr || !newSearchStr) {
        WCTX_MESSAGE(MT_ALERT, tr("search.update.err"), "Faltan argumentos"); //TODO i18n
        return false;
    }
    if (strcmp(oldSearchStr, newSearchStr) == 0) {
        WCTX_MESSAGE(MT_ALERT, tr("search.update.err"), "Nada que hacer. No hay cambios"); //TODO i18n
        return true;
    }
    bool result = false;
    char *searchStr = getSearchStrAt(host, id);
    if (searchStr != NULL) {
        if (strcmp(oldSearchStr, searchStr) == 0) {
            char *data = execSvdrp(host, "PLUG epgsearch EDIS %s", newSearchStr);
            if (data != NULL) {
                char *p = data;
                int code = strtol(p, &p, 10);
                result = boolean(code == SVDRP_PLUG_DEFAULT);
                if (wctx && *p && *(++p)) {
                    message_t *msg = (result)
                            ? WCTX_MESSAGE(MT_INFO, tr("search.update.ok"), "%s", p)
                            : WCTX_MESSAGE(MT_ALERT, tr("search.update.err"), "%s", p);
                    if (msg) msg->requieresHtmlEncoding = true;
                }
                free(data);
            } else {
                WCTX_MESSAGE(MT_ALERT, tr("search.update.err"), tr("warnSvdrpConnection"));
            }
        } else {
            warn("Las busquedas no coinciden:");
            warn("Act:[%s]", searchStr);
            warn("Old:[%s]", oldSearchStr);
            WCTX_MESSAGE(MT_ALERT, tr("search.update.err"), "Búsqueda no existe");
        }
        free(searchStr);
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool updateSearches(wcontext_t *wctx, hostConf_t *host) {
    if (!host) {
        return false;
    }
    bool result = false;
    char *command = NULL;
    char *data = execSvdrp(host, "PLUG epgsearch UPDS");
    if (data != NULL) {
        char *p = data;
        int code = strtol(p, &p, 10);
        result = boolean(code == SVDRP_PLUG_DEFAULT);
        if (wctx && *p && *(++p)) {
            message_t *msg = (result)
                    ? WCTX_MESSAGE(MT_INFO, tr("searches.update.ok"), "%s", p)
                    : WCTX_MESSAGE(MT_ALERT, tr("searches.update.err"), "%s", p);
            if (msg) msg->requieresHtmlEncoding = true;
        }
        free(data);
    }
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *getSearchStrAt(hostConf_t * const host, int id) {
    char *searchStr = NULL;
    if (id >= 0) {
        char *data = execSvdrp(host, "PLUG epgsearch LSTS %d", id);
        if (data) {
            char *p;
            for (p = strtok(data, "\r\n"); p != 0; p = strtok(0, "\r\n")) {
                if (atoi(p) == SVDRP_PLUG_DEFAULT) {
                    p += 4;
                    searchStr = strdup(p);
                    break;
                }
            }
            free(data);
        }
    }
    return searchStr;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool parseSearch(const char *line, search_t * const search, channelList_t const *const channels) {
    char *c;
    int l;
    struct tm sdate;

    const char *r = line;
    char *n; //pointer to next field, only to check integrity
    int field;
    for (field = 1; field <= 51 && r[0]; field++) {
        l = strcspn(r, ":");
        errno = 0;
        n = NULL;
        switch (field) {
            case 1:
                search->id = strtol(r, &n, 10);
                break;
            case 2:
                crit_goto_if((search->search = strndup(r, l)) == NULL, outOfMemory);
                setFlag(SFI_SEARCH, search->my);
                break;
            case 3:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_TIME, search->flags);
                break;
            case 4:
                search->startTime = (isFlagSet(SFL_USE_TIME, search->flags) && l) ? strtol(r, &n, 10) : 0;
                break;
            case 5:
                search->stopTime = (isFlagSet(SFL_USE_TIME, search->flags) && l) ? strtol(r, &n, 10) : 2359;
                break;
            case 6:
                search->useChannel = strtol(r, &n, 10);
                break;
            case 7:
                if (search->useChannel == 0) {
                    search->channelMin = 0;
                    search->channelMax = 0;
                } else if (search->useChannel == 1) {
                    int minNum = 0, maxNum = 0;
                    int fields = sscanf(r, "%d-%d", &search->channelMin, &search->channelMax);
                    if (fields == 0) { // stored with ID
                        char *channelMinbuffer = NULL;
                        char *channelMaxbuffer = NULL;
                        int channelsFound = sscanf(r, "%m[^|:]|%m[^|:]", &channelMinbuffer, &channelMaxbuffer);
                        int c;
                        channel_t *channel;
                        for (c = 0, channel = channels->channel; c < channels->length; c++, channel++)
                            if (strcmp(channel->channelId, channelMinbuffer) == 0) {
                                search->channelMin = c;
                                break;
                            }
                        if (channelsFound == 1) {
                            search->channelMax = search->channelMin;
                        } else
                            for (c = 0, channel = channels->channel; c < channels->length; c++, channel++)
                                if (strcmp(channel->channelId, channelMaxbuffer) == 0) {
                                    search->channelMax = c;
                                    break;
                                }
                        free(channelMinbuffer);
                        free(channelMaxbuffer);
                    }
                } else if (search->useChannel == 2) {
                    crit_goto_if((search->channelGroup = strndup(r, l)) == NULL, outOfMemory);
                    setFlag(SFI_CHANNEL_GROUP, search->my);
                }
                break;
            case 8:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_CASE, search->flags);
                break;
            case 9:
                search->searchMode = strtol(r, &n, 10);
                break;
            case 10:
                setOrClearFlag(strtol(r, &n, 10), EFI_TITLE, search->compareFlags);
                break;
            case 11:
                setOrClearFlag(strtol(r, &n, 10), EFI_SHORTDESC, search->compareFlags);
                break;
            case 12:
                setOrClearFlag(strtol(r, &n, 10), EFI_DESC, search->compareFlags);
                break;
            case 13:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_DURATION, search->flags);
                break;
            case 14:
                search->minDuration = (isFlagSet(SFL_USE_DURATION, search->flags) && l) ? strtol(r, &n, 10) : 0;
                break;
            case 15:
                search->maxDuration = (isFlagSet(SFL_USE_DURATION, search->flags) && l) ? strtol(r, &n, 10) : 0;
                break;
            case 16:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_AS_SEARCH_TIMER, search->flags);
                break;
            case 17:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_WDAY, search->flags);
                break;
            case 18:
                search->wday = strtol(r, &n, 10);
                break;
            case 19:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_EPISODE, search->flags);
                break;
            case 20:
                crit_goto_if((search->directory = strndup(r, l)) == NULL, outOfMemory);
                setFlag(SFI_DIRECTORY, search->my);
                break;
            case 21:
                search->priority = strtol(r, &n, 10);
                break;
            case 22:
                search->lifetime = strtol(r, &n, 10);
                break;
            case 23:
                search->marginStart = strtol(r, &n, 10);
                break;
            case 24:
                search->marginStop = strtol(r, &n, 10);
                break;
            case 25:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_VPS, search->flags);
                break;
            case 26:
                search->action = strtol(r, &n, 10);
                break;
            case 27:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_EXT_EPG_INFO, search->flags);
                break;
            case 28:
                crit_goto_if((search->extEpgValues = strndup(r, l)) == NULL, outOfMemory);
                setFlag(SFI_EXT_EPG_VALUES, search->my);
                break;
            case 29:
                setOrClearFlag(strtol(r, &n, 10), SFL_AVOID_REPEATS, search->flags);
                break;
            case 30:
                search->allowedRepeats = strtol(r, &n, 10);
                break;
            case 31:
                setOrClearFlag(strtol(r, &n, 10), EFI_TITLE, search->repeatsCompareFlags);
                break;
            case 32:
                setOrClearFlag(strtol(r, &n, 10), EFI_SHORTDESC, search->repeatsCompareFlags);
                break;
            case 33:
                setOrClearFlag(strtol(r, &n, 10), EFI_DESC, search->repeatsCompareFlags);
                break;
            case 34:
                search->catvaluesAvoidRepeat = strtol(r, &n, 10);
                break;
            case 35:
                search->repeatsWithinDays = strtol(r, &n, 10);
                break;
            case 36:
                search->delAfterDays = strtol(r, &n, 10);
                break;
            case 37:
                search->recordingsKeep = strtol(r, &n, 10);
                break;
            case 38:
                search->switchMinsBefore = strtol(r, &n, 10);
                break;
            case 39:
                search->pauseOnNrRecordings = strtol(r, &n, 10);
                break;
            case 40:
                search->blacklistMode = strtol(r, &n, 10);
                break;
            case 41:
                if (search->blacklistMode == BLACKLIST_SELECTION && 0 /*&& !ParseBlacklistIDs(value)*/) {
                    //TODO
                    warn("ERROR parsing blacklist IDs");
                    goto wrongFormat;
                }
                crit_goto_if((search->blacklistIds = strndup(r, l)) == NULL, outOfMemory);
                setFlag(SFI_BLACKLIST_IDS, search->my);
                break;
            case 42:
                search->fuzzyTolerance = strtol(r, &n, 10);
                break;
            case 43:
                setOrClearFlag(strtol(r, &n, 10), SFL_USE_IN_FAVORITES, search->flags);
                break;
            case 44:
                search->menuTemplate = strtol(r, &n, 10);
                break;
            case 45:
                search->delMode = strtol(r, &n, 10);
                break;
            case 46:
                search->delAfterCountRecs = strtol(r, &n, 10);
                break;
            case 47:
                search->delAfterDaysOfFirstRec = strtol(r, &n, 10);
                break;
            case 48:
                search->useAsSearchTimerFrom = strtol(r, &n, 10);
                break;
            case 49:
                search->useAsSearchTimerTil = strtol(r, &n, 10);
                break;
            case 50:
                setOrClearFlag(strtol(r, &n, 10), SFL_IGNORE_MISSING_EPG_CATS, search->flags);
                break;
            case 51:
                setOrClearFlag(strtol(r, &n, 10), SFL_UNMUTE_SOUND_ON_SWITCH, search->flags);
                break;
        }
        if (errno) {
            warn(strerror(errno));
            goto wrongFormat;
        }
        if (n && n[0] != ':' && n[0] != 0) {
            goto wrongFormat;
        }
        r += l;
        if (r[0] != ':') break;
        r++;
    }
    return true;
wrongFormat:
    warn("Wrong search string [%s]", line);
    return false;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

char *makeSearchStr(search_t * const search, const channelList_t *channels) {
    char *searchStr;
    crit_goto_if(asprintf(&searchStr, "%d:%s:%d", (search->id >= 0) ? search->id : 0, search->search, isFlagSet(SFL_USE_TIME, search->flags)) < 0, outOfMemory);
    if (isFlagSet(SFL_USE_TIME, search->flags)) {
        crit_goto_if(asprintf(&searchStr, "%s:%04d:%04d", searchStr, search->startTime, search->stopTime) < 0, outOfMemory);
    } else {
        crit_goto_if(asprintf(&searchStr, "%s::", searchStr) < 0, outOfMemory);
    }
    crit_goto_if(asprintf(&searchStr, "%s:%d", searchStr, search->useChannel) < 0, outOfMemory);
    if (search->useChannel == 1) {
        crit_goto_if(asprintf(&searchStr, "%s:%s", searchStr, channels->channel[search->channelMin].channelId) < 0, outOfMemory);
        if (search->channelMin < search->channelMax) {
            crit_goto_if(asprintf(&searchStr, "%s|%s", searchStr, channels->channel[search->channelMax].channelId) < 0, outOfMemory);
        }
    } else if (search->useChannel == 2) {
        crit_goto_if(asprintf(&searchStr, "%s:%s", searchStr, (search->channelGroup) ? search->channelGroup : "0") < 0, outOfMemory);
    } else {
        crit_goto_if(asprintf(&searchStr, "%s:0", searchStr) < 0, outOfMemory);
    }
    crit_goto_if(asprintf(&searchStr, "%s:%d:%d:%d:%d:%d:%d",
            searchStr,
            isFlagSet(SFL_USE_CASE, search->flags),
            search->searchMode,
            isFlagSet(EFI_TITLE, search->compareFlags),
            isFlagSet(EFI_SHORTDESC, search->compareFlags),
            isFlagSet(EFI_DESC, search->compareFlags),
            isFlagSet(SFL_USE_DURATION, search->flags)
            ) < 0, outOfMemory);
    if (isFlagSet(SFL_USE_DURATION, search->flags)) {
        crit_goto_if(asprintf(&searchStr, "%s:%04d:%04d", searchStr, search->minDuration, search->maxDuration) < 0, outOfMemory);
    } else {
        crit_goto_if(asprintf(&searchStr, "%s::", searchStr) < 0, outOfMemory);
    };
    crit_goto_if(asprintf(&searchStr,
            "%s:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%d:%d:%s",
            searchStr,
            isFlagSet(SFL_USE_AS_SEARCH_TIMER, search->flags),
            isFlagSet(SFL_USE_WDAY, search->flags),
            search->wday,
            isFlagSet(SFL_USE_EPISODE, search->flags),
            search->directory,
            search->priority,
            search->lifetime,
            search->marginStart,
            search->marginStop,
            isFlagSet(SFL_USE_VPS, search->flags),
            search->action,
            isFlagSet(SFL_USE_EXT_EPG_INFO, search->flags),
            (search->extEpgValues) ? search->extEpgValues : ""
            ) < 0, outOfMemory);
    crit_goto_if(asprintf(&searchStr,
            "%s:%d:%d:%d:%d:%d:%ld:%d:%d:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%d:%ld:%ld:%d:%d",
            searchStr,
            isFlagSet(SFL_AVOID_REPEATS, search->flags),
            search->allowedRepeats,
            isFlagSet(EFI_TITLE, search->repeatsCompareFlags),
            isFlagSet(EFI_SHORTDESC, search->repeatsCompareFlags),
            isFlagSet(EFI_DESC, search->repeatsCompareFlags),
            search->catvaluesAvoidRepeat,
            search->repeatsWithinDays,
            search->delAfterDays,
            search->recordingsKeep,
            search->switchMinsBefore,
            search->pauseOnNrRecordings,
            search->blacklistMode,
            (search->blacklistIds) ? search->blacklistIds : "",
            search->fuzzyTolerance,
            isFlagSet(SFL_USE_IN_FAVORITES, search->flags),
            search->menuTemplate,
            search->delMode,
            search->delAfterCountRecs,
            search->delAfterDaysOfFirstRec,
            search->useAsSearchTimerFrom,
            search->useAsSearchTimerTil,
            isFlagSet(SFL_IGNORE_MISSING_EPG_CATS, search->flags),
            isFlagSet(SFL_UNMUTE_SOUND_ON_SWITCH, search->flags)
            ) < 0, outOfMemory);

    return searchStr;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void printSearchForm(wcontext_t *wctx, search_t * const search, channelList_t const *const channels, searchCatList_t const *const cats, const char *cssLevel) {
    settingConfig_t searchModeCfg = {
        .name = "search.mode",
        .defaultValue = "0",
        .options = "search.phrase|search.allWords|search.atLeastOne|search.exactMatch|search.regex|search.fuzzy"
        /*"|search.lt|search.let|search.gt|search.get|search.eq|search.neq"*/,
        .isIndex = true
    };
    settingConfig_t checkboxCfg = {
        .name = "",
        .defaultValue = "0",
        .options = "0|1",
        .isIndex = false
    };
    settingConfig_t useChannelCfg = {
        .name = "",
        .defaultValue = "0",
        .options = "no|interval|channel.group|onlyFTA",
        .isIndex = true
    };
    settingConfig_t searchActionCfg = {
        .name = "",
        .defaultValue = "0",
        .options = "search.record|search.announce|search.switch",
        .isIndex = true
    };

    char *paramValue;
    const char *cssClass = "paramValue";
    hostConf_t *host = getHost(search->hostId);

    WCTX_IPRINTFT("<div class=\"%sDiv\">\n", 0, 1, cssLevel);
    WCTX_IPRINTF("<div class=\"%sHead js-dialogTitle\">%s</div>\n", cssLevel, tr("search.edit"));
    {
        WCTX_IPRINTFT("<div class=\"%s\">\n", 0, 1, cssLevel);
        {
            WCTX_IPRINTT("<form class=\"form1 searchEdit js-dialogContent\" action=\"" URLWI "\" method=\"post\">\n", 0, 1);
            {
                WCTX_IPRINTT("<fieldset id=\"searchSet\">\n", 0, 1);
                {
                    WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\"/>\n", PN_SEARCHES);
                    if (search->id >= 0) {
                        WCTX_IPRINTF("<input type=\"hidden\" name=\"searchId\" value=\"%d\"/>\n", search->id);
                        WCTX_IPRINTF("<input type=\"hidden\" name=\"oldSearchStr\" value=\"%s\"/>\n"
                                , wctxHtmlEncode(wctx, search->searchStr));
                    }
                    if (webifConf.hosts.numVDRs > 1) {
                        WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, "Host");
                        printVDRSelect(wctx, "hostId", search->hostId);
                        WCTX_IPRINTT("</label>\n", -1, 0);
                    } else {
                        WCTX_IPRINTF("<input type=\"hidden\" name=\"hostId\" value=\"%d\"/>\n", search->hostId);
                    }
                    WCTX_IPRINTF("<input type=\"hidden\" name=\"oldHostId\" value=\"%d\"/>\n", search->hostId);
                    WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"search\" value=\"%s\" size=\"40\"/></label>\n"
                            , tr("search.search"), (search->search) ? search->search : "");
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.mode"));
                    {
                        asprintf(&paramValue, "%d", search->searchMode);
                        settingPrintSelect(wctx, &searchModeCfg, "searchMode", "searchMode", cssClass, -1, paramValue);
                        free(paramValue);
                    }
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    WCTX_IPRINTFT("<label id=\"fuzzyToleranceLabel\">%s&nbsp;\n", 0, 1, tr("search.fuzzy.tolerance"));
                    WCTX_IPRINTF("<input type=\"text\" name=\"fuzzyTolerance\" value=\"%d\" size=\"2\"/>", search->fuzzyTolerance);
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.useCase"));
                    settingPrintCheckbox(wctx, &checkboxCfg, NULL, "useCase", cssClass, -1, isFlagSet(SFL_USE_CASE, search->flags) ? "1" : "0");
                    WCTX_IPRINTT("</label>\n", -1, 0);

                    WCTX_IPRINTT("<fieldset id=\"compareSet\">\n", 0, 1);
                    { //compare
                        WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.compareTitle"));
                        settingPrintCheckbox(wctx, &checkboxCfg, NULL, "compareTitle", cssClass, -1, isFlagSet(EFI_TITLE, search->compareFlags) ? "1" : "0");
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.compareSubtitle"));
                        settingPrintCheckbox(wctx, &checkboxCfg, NULL, "compareSubtitle", cssClass, -1, isFlagSet(EFI_SHORTDESC, search->compareFlags) ? "1" : "0");
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.compareDescription"));
                        settingPrintCheckbox(wctx, &checkboxCfg, NULL, "compareDescription", cssClass, -1, isFlagSet(EFI_DESC, search->compareFlags) ? "1" : "0");
                        WCTX_IPRINTT("</label>\n", -1, 0);
                    }
                    WCTX_IPRINTT("</fieldset>\n", -1, 0);
                }
                WCTX_IPRINTT("</fieldset>" HTMLCOMENT("searchSet") "\n", -1, 0);

                if (cats->length > 0) {
                    WCTX_IPRINTFT("<fieldset id=\"catsSet\"%s>\n", 0, 1, (cats->length > 0) ? "" : " style=\"display:none\"");
                    {
                        WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.useExtEpgInfo"));
                        settingPrintCheckbox(wctx, &checkboxCfg, "catsFilter", "useExtEpgInfo", cssClass, -1, isFlagSet(SFL_USE_EXT_EPG_INFO, search->flags) ? "1" : "0");
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        printSearchCatListFilter(wctx, search, cats, "catsFilterCfg");
                    }
                    WCTX_IPRINTT("</fieldset>" HTMLCOMENT("catsSet") "\n", -1, 0);
                }

                WCTX_IPRINTT("<fieldset>\n", 0, 1);
                { //start
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.startFilter"));
                    settingPrintCheckbox(wctx, &checkboxCfg, "startFilter", "useTime", cssClass, -1, isFlagSet(SFL_USE_TIME, search->flags) ? "1" : "0");
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    WCTX_IPRINTT("<fieldset id=\"startFilterCfg\">\n", 0, 1);
                    {
                        int hour, min;
                        hour = search->startTime / 100;
                        min = search->startTime - hour * 100;
                        WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"startTime\" value=\"%02d:%02d\" size=\"5\"/></label>\n", tr("search.startAfter"), hour, min);
                        hour = search->stopTime / 100;
                        min = search->stopTime - hour * 100;
                        WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"stopTime\" value=\"%02d:%02d\" size=\"5\"/></label>\n", tr("search.startBefore"), hour, min);
                    }
                    WCTX_IPRINTT("</fieldset>\n", -1, 0);
                }
                WCTX_IPRINTT("</fieldset>\n", -1, 0);

                WCTX_IPRINTT("<fieldset>\n", 0, 1);
                { //duration
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.durationFilter"));
                    settingPrintCheckbox(wctx, &checkboxCfg, "durationFilter", "useDuration", cssClass, -1, isFlagSet(SFL_USE_DURATION, search->flags) ? "1" : "0");
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    {
                        WCTX_IPRINTT("<fieldset id=\"durationFilterCfg\">\n", 0, 1);
                        WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"minDuration\" value=\"%d\" size=\"3\"/></label>\n", tr("search.durationMin"), search->minDuration);
                        WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"maxDuration\" value=\"%d\" size=\"3\"/></label>\n", tr("search.durationMax"), search->maxDuration);
                        WCTX_IPRINTT("</fieldset>\n", -1, 0);
                    }
                }
                WCTX_IPRINTT("</fieldset>\n", -1, 0);

                WCTX_IPRINTT("<fieldset>\n", 0, 1);
                { //week day
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.wdayFilter"));
                    settingPrintCheckbox(wctx, &checkboxCfg, "wdayFilter", "useWday", cssClass, -1, isFlagSet(SFL_USE_WDAY, search->flags) ? "1" : "0");
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    {
                        WCTX_IPRINTT("<fieldset id=\"wdayFilterCfg\">\n", 0, 1);
                        int wdayFlag;
                        int c, wday;
                        for (c = 0, wday = startOfWeek[langId]; c < 7; c++, wday = (wday + 1) % 7) {
                            wdayFlag = 1 << wday;
                            WCTX_IPRINTF(
                                    "<label>%s&nbsp;"
                                    "<input type=\"checkbox\" name=\"wday\" value=\"%d\"%s/>"
                                    "</label>\n"
                                    , weekdays[langId][wday], wday
                                    , checked[(search->wday < 0) ? isFlagSet(wdayFlag, -search->wday) : boolean(wday == search->wday)]);
                        }
                    }
                    WCTX_IPRINTT("</fieldset>\n", -1, 0);
                }
                WCTX_IPRINTT("</fieldset>\n", -1, 0);

                WCTX_IPRINTT("<fieldset>\n", 0, 1); //channel
                {
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.channelFilter"));
                    asprintf(&paramValue, "%d", search->useChannel);
                    settingPrintSelect(wctx, &useChannelCfg, "channelFilter", "useChannel", cssClass, -1, paramValue);
                    free(paramValue);
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    WCTX_IPRINTT("<fieldset id=\"channelFilterCfg\">\n", 0, 1);
                    {
                        WCTX_IPRINTFT("<label id=\"channelMinLabel\">%s&nbsp;\n", 0, 1, tr("search.channelMin"));
                        printChannelListSelect(wctx, NULL, "channelMin", channels, search->channelMin + 1, NULL);
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        WCTX_IPRINTFT("<label id=\"channelMaxLabel\">%s&nbsp;\n", 0, 1, tr("search.channelMax"));
                        printChannelListSelect(wctx, NULL, "channelMax", channels, search->channelMax + 1, NULL);
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        { //channelGroup
                            channelGroupList_t groups;
                            getChannelGroupList(host, &groups, channels);
                            WCTX_IPRINTFT("<label id=\"channelGroupLabel\">%s&nbsp;\n", 0, 1, tr("search.channel.group"));
                            if (groups.length > 0) {
                                printChannelGroupListSelect(wctx, "channelGroup", &groups, search->channelGroup);
                                //TODO que hacer si no esta en la lista
                            } else {
                                WCTX_IPRINTF("<input type=\"text\" name=\"channelGroup\" size=\"5\" value=\"%s\"/>\n", (search->channelGroup) ? search->channelGroup : "");
                            }
                            WCTX_IPRINTT("</label>\n", -1, 0);
                            freeChannelGroupList(&groups);
                        }
                    }
                    WCTX_IPRINTT("</fieldset>\n", -1, 0);
                }
                WCTX_IPRINTT("</fieldset>\n", -1, 0);

                WCTX_IPRINTT("<fieldset>\n", 0, 1);
                { //useAsSearchTimer
                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.useAsSearchTimer"));
                    settingPrintCheckbox(wctx, &checkboxCfg, "useAsSearchTimer", "useAsSearchTimer", cssClass, -1, isFlagSet(SFL_USE_AS_SEARCH_TIMER, search->flags) ? "1" : "0");
                    WCTX_IPRINTT("</label>\n", -1, 0);
                    WCTX_IPRINTT("<fieldset id=\"useAsSearchTimerCfg\">\n", 0, 1);
                    {
                        WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.action"));
                        asprintf(&paramValue, "%d", search->action);
                        settingPrintSelect(wctx, &searchActionCfg, "searchAction", "searchAction", cssClass, -1, paramValue);
                        free(paramValue);
                        WCTX_IPRINTT("</label>\n", -1, 0);
                        WCTX_IPRINTT("<fieldset id=\"recordCfg\">\n", 0, 1);
                        { //action: record
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"directory\" value=\"%s\" size=\"50\" /></label>\n"
                                    , tr("search.directory"), (search->directory) ? search->directory : "%Title%~%Subtitle%");
                            WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.useEpisode"));
                            settingPrintCheckbox(wctx, &checkboxCfg, "useEpisode", "useEpisode", cssClass, -1, isFlagSet(SFL_USE_EPISODE, search->flags) ? "1" : "0");
                            WCTX_IPRINTT("</label>\n", -1, 0);
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"priority\" value=\"%d\" size=\"3\" /></label>\n", tr("search.priority"), search->priority);
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"lifetime\" value=\"%d\" size=\"3\" /></label>\n", tr("search.lifetime"), search->lifetime);
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"marginStart\" value=\"%d\" size=\"3\" /></label>\n", tr("search.margin.before"), search->marginStart);
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"marginStop\" value=\"%d\" size=\"3\" /></label>\n", tr("search.margin.after"), search->marginStop);
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"delAfterDays\" value=\"%d\" size=\"2\" /></label>\n", tr("search.delAfterDays"), search->delAfterDays);
                            //wctx_printfn("<fieldset id=\"repeats\">\n",0,1);
                            { //repeats
                                WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.repeats.avoid"));
                                settingPrintCheckbox(wctx, &checkboxCfg, "repeatsAvoid", "repeatsAvoid", cssClass, -1, isFlagSet(SFL_AVOID_REPEATS, search->flags) ? "1" : "0");
                                WCTX_IPRINTT("</label>\n", -1, 0);
                                WCTX_IPRINTT("<fieldset id=\"repeatsAvoidCfg\">\n", 0, 1);
                                {
                                    WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"repeatsAllowed\" value=\"%d\" size=\"2\"/></label>\n"
                                            , tr("search.repeats.maxAllowed"), search->allowedRepeats);
                                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.repeats.compareTitle"));
                                    settingPrintCheckbox(wctx, &checkboxCfg, NULL, "repeatsCompareTitle", cssClass, -1, isFlagSet(EFI_TITLE, search->repeatsCompareFlags) ? "1" : "0");
                                    WCTX_IPRINTT("</label>\n", -1, 0);
                                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.repeats.compareSubtitle"));
                                    settingPrintCheckbox(wctx, &checkboxCfg, NULL, "repeatsCompareSubtitle", cssClass, -1, isFlagSet(EFI_SHORTDESC, search->repeatsCompareFlags) ? "1" : "0");
                                    WCTX_IPRINTT("</label>\n", -1, 0);
                                    WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.repeats.compareDescription"));
                                    settingPrintCheckbox(wctx, &checkboxCfg, NULL, "repeatsCompareDescription", cssClass, -1, isFlagSet(EFI_DESC, search->repeatsCompareFlags) ? "1" : "0");
                                    WCTX_IPRINTT("</label>\n", -1, 0);
                                }
                                WCTX_IPRINTT("</fieldset>\n", -1, 0);
                            }
                            //wctx_printfn("</fieldset>\n",-1,0);
                        }
                        WCTX_IPRINTT("</fieldset>\n", -1, 0);

                        /*
                        wctx_printfn("<fieldset id=\"announceCfg\">\n",0,1);
                        wctx_printfn("</fieldset>\n",-1,0);
                         */

                        WCTX_IPRINTT("<fieldset id=\"switchCfg\">\n", 0, 1);
                        { //action: switch
                            WCTX_IPRINTF("<label>%s&nbsp;<input type=\"text\" name=\"switchMinsBefore\" value=\"%d\" size=\"2\"/></label>\n"
                                    , tr("search.switch.minsBefore"), search->switchMinsBefore);
                            WCTX_IPRINTFT("<label>%s&nbsp;\n", 0, 1, tr("search.switch.unmuteSound"));
                            settingPrintCheckbox(wctx, &checkboxCfg, "switchUnmuteSound", "repeatsAvoid", cssClass, -1, isFlagSet(SFL_UNMUTE_SOUND_ON_SWITCH, search->flags) ? "1" : "0");
                            WCTX_IPRINTT("</label>\n", -1, 0);
                        }
                        WCTX_IPRINTT("</fieldset>\n", -1, 0);
                    }
                    WCTX_IPRINTT("</fieldset>\n", -1, 0);
                }
                WCTX_IPRINTT("</fieldset>\n", -1, 0);

                WCTX_IPRINTT("<fieldset class=\"controls js-dialogButtons\">\n", 0, 1);
                { //controls
                    WCTX_IPRINTF(
                            "<button id=\"confirm\" class=\"confirm ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" >"
                            "<span class=\"ui-button-icon-primary ui-icon ui-icon-check\"></span>"
                            "<span class=\"ui-button-text\">%s</span>"
                            "</button>\n"
                            , PA_ADD, tr("accept"));
                    if (search->id>-1) { //not new
                        WCTX_IPRINTF(
                                "<button id=\"searchDelete\" class=\"delete searchDelete ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" >"
                                "<span class=\"ui-button-icon-primary ui-icon ui-icon-trash\"></span>"
                                "<span class=\"ui-button-text\">%s</span>"
                                "</button>\n"
                                , PA_DELETE, tr("search.delete"));
                        WCTX_IPRINTF(
                                "<button id=\"searchDeletePlus\" class=\"delete searchDelete ui-state-default ui-button ui-button-text-icon-primary\" name=\"" ARG_ACTION "\" type=\"submit\" value=\"%d\" title=\"%s\" >"
                                "<span class=\"ui-button-icon-primary ui-icon ui-icon-trash\"></span>"
                                "<span class=\"ui-button-text\">%s</span>"
                                "</button>\n"
                                , PA_DELETE_PLUS, tr("search.delete+.title"), tr("search.delete+"));
                    }
                }
                WCTX_IPRINTT("</fieldset>\n", -1, 0);
            }
            WCTX_IPRINTT("</form>\n", -1, 0);
        }
        WCTX_IPRINTT("</div>\n", -1, 0); //[cssLevel]
    }
    WCTX_IPRINTT("</div>\n", -1, 0); //[cssLevel]Div
}

//	SearchCats--------------------------

void initSearchCat(searchCat_t * const cat) {
    cat->id = 0;
    cat->name = NULL;
    cat->menuname = NULL;
    cat->searchmode = SEARCH_MODE_SUBSTRING; // default: all substrings must exist
    cat->values = NULL;
    cat->nvalues = 0;
    //cat->currentvalue=NULL;
}

void freeSearchCat(searchCat_t * const cat) {
    int i;
    free(cat->name);
    free(cat->menuname);
    for (i = 0; i < cat->nvalues; i++) {
        free(cat->values[i]);
    }
    free(cat->values);
    //free(cat->currentvalue);
}

void initSearchCatList(searchCatList_t * const list) {
    list->length = 0;
    list->entry = NULL;
}

void freeSearchCatList(searchCatList_t * const list) {
    int i;
    for (i = 0; i < list->length; i++) {
        freeSearchCat(&(list->entry[i]));
    }
    free(list->entry);
    initSearchCatList(list);
}

void getSearchCatList(searchCatList_t * const cats, hostConf_t *host) {
    initSearchCatList(cats);
    char *data = execSvdrp(host, "PLUG epgsearch LSTE");
    if (data) {
        char *line, *sline;
        searchCat_t *cat;
        for (line = strtok_r(data, "\r\n", &sline); line != 0; line = strtok_r(NULL, "\r\n", &sline)) {
            if (atoi(line) == SVDRP_PLUG_DEFAULT) {
                line += 4;
                cats->entry = realloc(cats->entry, (++cats->length) * sizeof (searchCat_t));
                crit_goto_if(cats->entry == NULL, outOfMemory);
                cat = cats->entry + cats->length - 1;
                initSearchCat(cat);
                cat->hostId = host->id;

                int i = 1;
                char *field, *nfield;
                field = line;
                while (*field) {
                    while (field[0] == ' ') field++;
                    if (*field == '|') {
                        field++;
                    } else {
                        nfield = strchrnul(field, '|');
                        if (nfield[0] == '|') {
                            *nfield = 0;
                            nfield++;
                        }
                        switch (i) {
                            case 1:
                                cat->id = atoi(field);
                                break;
                            case 2:
                                crit_goto_if((cat->name = strdup(field)) == NULL, outOfMemory);
                                break;
                            case 3:
                                crit_goto_if((cat->menuname = strdup(field)) == NULL, outOfMemory);
                                break;
                            case 4:
                            {
                                char *value, *svalue;
                                for (value = strtok_r(field, ",", &svalue); value != 0; value = strtok_r(NULL, ",", &svalue)) {
                                    while (*value == ' ') value++;
                                    cat->values = realloc(cat->values, (++cat->nvalues) * sizeof (char*));
                                    crit_goto_if(cat->values == NULL, outOfMemory);
                                    crit_goto_if((cat->values[cat->nvalues - 1] = strdup(value)) == NULL, outOfMemory);
                                }
                                break;
                            }
                            case 5:
                                cat->searchmode = atoi(field);
                                break;
                        }
                        field = nfield;
                    }
                    i++;
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

char *currentCatValue(const search_t *search, const searchCat_t *cat) {
    if (search->extEpgValues) {
        char *tmp = strdup(search->extEpgValues);
        char *t;
        char *val = NULL;
        for (t = strtok(tmp, "|"); t; t = strtok(NULL, "|")) {
            char *n;
            int id = strtol(t, &n, 10);
            if (id == cat->id) {
                if (*n == '#') {
                    val = decodeCatValue(strdup(n + 1));
                    break;
                } else {
                    warn("extEpgValues mal formado: %s", search->extEpgValues);
                }
            }
        }
        free(tmp);
        if (val) return val;
    }
    return strdup("");
}

bool printSearchCatListFilter(wcontext_t *wctx, const search_t *search, const searchCatList_t * const searchCatList, const char *fieldsetId) {
    if (searchCatList->length > 0) {
        WCTX_IPRINTFT("<fieldset id=\"%s\">\n", 0, 1, (fieldsetId) ? fieldsetId : "compareCatsSet");
        int ncat;
        searchCat_t *cat;
        for (cat = searchCatList->entry, ncat = 0; ncat < searchCatList->length; cat++, ncat++) {
            WCTX_IPRINTFT("<label><a class=\"searchCat\" title=\"%%%s%%\"href=\"#\">%s</a>&nbsp;\n", 0, 1, cat->name, cat->menuname);
            char *cv = currentCatValue(search, cat);
            if (cat->nvalues == 0) {
                WCTX_IPRINTF("<input type=\"text\" name=\"compare_cat_%d\" value=\"%s\"/>\n", cat->id, cv);
            } else {
                WCTX_IPRINTFT("<select name=\"compare_cat_%d\" size=\"1\">\n", 0, 1, cat->id);
                int nvalue;
                bool noneSelected = true;
                for (nvalue = 0; nvalue < cat->nvalues; nvalue++) {
                    const char *value = cat->values[nvalue];
                    const char *valuee = wctxHtmlEncode(wctx, value);
                    bool thisSelected = sameString(cv, value);
                    if (thisSelected) noneSelected = false;
                    WCTX_IPRINTF("<option value=\"%s\"%s>%s</option>\n", valuee, selected[thisSelected], valuee);
                }
                WCTX_IPRINTF("<option value=\"\"%s></option>\n", selected[noneSelected]);
                WCTX_IPRINTT("</select>\n", -1, 0);
            }
            free(cv);
            WCTX_IPRINTT("</label>\n", -1, 0);
        }
        WCTX_IPRINTT("</fieldset>\n", -1, 0);
        return true;
    }
    return false;
}

static void printSearchesHtmlHeadExtra(wcontext_t *wctx, const char *title) {
    if (!wctx->isAjaxRequest) {
        printCssLinkTag(wctx, "searches.css", "screen");
        printJavascriptTag(wctx, "jquery.form.js");
        printJavascriptTag(wctx, "wi.formHandler.js");
        printJavascriptTag(wctx, "wi.searches.js");
    }
    openJavascriptTag(wctx);
    if (!wctx->isAjaxRequest) {
        WCTX_IPRINTF("$(function(){webif.searchesPageInit();});\n");
    }
    WCTX_IPRINTT("$.extend(webif.messages,{\n", 0, 1);
    WCTX_IPRINTF("searchDeleteConfirm:'%s',\n", tr("search.delete.confirm"));
    WCTX_IPRINTF("nothingToSearchFor:'%s',\n", tr("search.err.nothingToSearchFor"));
    WCTX_IPRINTF("nothingToCompareWith:'%s'\n", tr("search.err.nothingToCompareWith"));
    WCTX_IPRINTT("});\n", -1, 0);
    closeJavascriptTag(wctx);
}

void printSearchesPage(wcontext_t *wctx) {
    wctxBufferReserve(wctx, 1024);
    vars_t *rqargs = request_get_args(wctx->request);
    int searchId = (vars_countn(rqargs, "searchId") > 0) ? vars_get_value_i(rqargs, "searchId") : -1;
    bool mySearchStr;
    bool myOldSearchStr;
    char *searchStr = getRequestFieldValue(wctx, rqargs, "searchStr", &mySearchStr);
    char *oldSearchStr = getRequestFieldValue(wctx, rqargs, "oldSearchStr", &myOldSearchStr);
    const int hostId = vars_get_value_i(rqargs, "hostId");
    const int oldHostId = vars_get_value_i(rqargs, "oldHostId");
    hostConf_t *host = (vars_countn(rqargs, "hostId") > 0) ? getHost(hostId) : NULL;
    if (!host || !host->isVdr) {
        host = getFirstVdrHost();
    }
    channelList_t vchannels;
    channelList_t *channels = &vchannels;
    initChannelList(channels);

    searchCatList_t vcats;
    searchCatList_t *cats = &vcats;
    initSearchCatList(cats);

    initHtmlPage(wctx, tr("searches"), printSearchesHtmlHeadExtra, NULL);

    if (wctx->action == PA_DELETE || wctx->action == PA_DELETE_PLUS) {
        if (!wctx->isReload && (searchId >= 0) && (oldSearchStr)) {
            if (!deleteSearch(wctx, host, searchId, oldSearchStr, boolean(wctx->action == PA_DELETE_PLUS))) {
                wctx->action = PA_EDIT_FORM;
                if (mySearchStr) {
                    free(searchStr);
                }
                searchStr = oldSearchStr;
                mySearchStr = myOldSearchStr;
                myOldSearchStr = false;
            }
        }
        wctx->action = PA_DEFAULT;
    }
    if (wctx->action == PA_ADD) {
        if (!wctx->isReload) {
            if (channels->length == 0 && host) {
                getChannelList(host, channels, SF_NONE, SD_NONE);
            }
            if (cats->length == 0) {
                getSearchCatList(cats, host);
            }
            if (searchStr == NULL) {
                search_t vsearch;
                if (initSearchFromArgs(&vsearch, rqargs, channels, cats, wctx)) {
                    searchStr = makeSearchStr(&vsearch, channels);
                    mySearchStr = true;
                }
                freeSearch(&vsearch);
            }
            bool searchesChanged = false;
            if ((searchId >= 0) && (oldSearchStr) && (hostId == oldHostId)) {
                searchesChanged = editSearch(wctx, host, searchId, oldSearchStr, searchStr);
            } else if (searchStr) {
                searchesChanged = addSearch(wctx, host, searchStr);
                if (searchesChanged) {
                    if ((searchId >= 0) && (oldSearchStr) && hostId != oldHostId) {
                        hostConf_t *oldHost = getHost(oldHostId);
                        deleteSearch(wctx, oldHost, searchId, oldSearchStr, true);
                    }
                }
            }
            if (searchesChanged) {
                updateSearches(wctx, host);
            }
        }
        wctx->action = PA_DEFAULT;
    }
    if (wctx->action == PA_EDIT_FORM || wctx->action == PA_SEARCH_CREATE_FROM_EVENT) {
        if (channels->length == 0) {
            getChannelList(host, channels, SF_NONE, SD_NONE);
        }
        if (cats->length == 0) {
            getSearchCatList(cats, host);
        }
        search_t vsearch;
        search_t *search = &vsearch;
        initSearch(search);
        if (wctx->action == PA_EDIT_FORM) {
            if (searchStr) {
                parseSearch(searchStr, search, channels);
                search->searchStr = searchStr;
            }
            search->id = searchId;
            search->hostId = hostId;
        } else { //create new from event
            initSearchFromEvent(wctx, search, host, vars_get_value_i(rqargs, "channelNum"), vars_get_value_i(rqargs, "eventId"));
        }
        printSearchForm(wctx, search, channels, cats, "level3");
        freeSearch(search);
    }
    if (wctx->action == PA_DEFAULT) { //list searches
        if (channels->length == 0 && host) {
            getChannelList(host, channels, SF_NONE, SD_NONE);
        }
        if (channels->length > 0) {
            searchList_t searches;
            getSearchList(&searches, channels, wctx->sortField, wctx->sortDirection);
            bool printHost = boolean(webifConf.hosts.numVDRs > 1);
            WCTX_IPRINTFT("<table id=\"%s\" class=\"list\">\n", 0, 1, AJAX_REPLACE_PREFIX("searches"));
            WCTX_IPRINTF("<caption>%s</caption>\n", tr("searches"));
            WCTX_IPRINT("<col class=\"searchId number\"/>\n");
            if (printHost) WCTX_IPRINT("<col class=\"hostId number\"/>\n");
            WCTX_IPRINT("<col class=\"search\"/>\n");
            WCTX_IPRINT("<col class=\"directory\"/>\n");
            WCTX_IPRINT("<col class=\"hour\"/>\n");
            WCTX_IPRINT("<col class=\"hour\"/>\n");
            WCTX_IPRINTT("<thead class=\"listHead\">\n", 0, 1);
            WCTX_IPRINTT("<tr>\n", 0, 1);
            printList1TH(wctx, SF_SE_NUMBER, "#");
            if (printHost) {
                printList1TH(wctx, SF_HOST, "Host");
            }
            printList1TH(wctx, SF_SEARCH, tr("search"));
            printList1TH(wctx, SF_DIRECTORY, tr("directory"));
            printList1TH(wctx, SF_START, tr("start"));
            printList1TH(wctx, SF_START, tr("end"));
            WCTX_IPRINTT("</tr>\n", -1, 0);
            WCTX_IPRINTT("</thead>" HTMLCOMENT("listHead") "\n", -1, 0);

            WCTX_IPRINTT("<tbody class=\"listBody\">\n", 0, 1);
            const char *EditSearch = tr("search.edit");
            const search_t *search;
            int i, hour, min;
            for (i = 0, search = searches.entry; i < searches.length; i++, search++) {
                WCTX_IPRINTFT("<tr class=\"%s\">\n", 0, 1, isFlagSet(SFL_USE_AS_SEARCH_TIMER, search->flags) ? "active" : "inactive");
                WCTX_IPRINTF("<td class=\"number\">%d</td>\n", search->id);
                if (printHost)
                    WCTX_IPRINTF("<td class=\"number\">%d</td>\n", search->hostId);
                WCTX_IPRINTF(
                        "<td>"
                        "<a class=\"searchEdit title infoToggler\" href=\"" URLWI_PA(AMP) AMP "searchId=%d" AMP "searchStr=%s" AMP "hostId=%d\" title=\"%s\">"
                        "%s"
                        "</a>"
                        "</td>\n"
                        , PN_SEARCHES, PA_EDIT_FORM, search->id
                        , wctxUrlEncode(wctx, search->searchStr, NULL), search->hostId, EditSearch, search->search);
                WCTX_IPRINTF("<td>%s</td>\n", wctxHtmlEncode(wctx, search->directory));
                if (isFlagSet(SFL_USE_TIME, search->flags)) {
                    hour = search->startTime / 100;
                    min = search->startTime - hour * 100;
                    WCTX_IPRINTF("<td class=\"hour\">%02d:%02d</td>\n", hour, min);
                    hour = search->stopTime / 100;
                    min = search->stopTime - hour * 100;
                    WCTX_IPRINTF("<td class=\"hour\">%02d:%02d</td>\n", hour, min);
                } else {
                    WCTX_IPRINT("<td class=\"hour\">00:00</td>\n");
                    WCTX_IPRINT("<td class=\"hour\">23:59</td>\n");
                }

                WCTX_IPRINTT("</tr>\n", -1, 0);
            }
            WCTX_IPRINTT("</tbody>" HTMLCOMENT("listBody") "\n", -1, 0);
            WCTX_IPRINTT("</table>" HTMLCOMENT("list") "\n", -1, 0);
            freeSearchList(&searches);
        } else {
            //TODO
        }
    }
    finishHtmlPage(wctx);
    if (mySearchStr) {
        free(searchStr);
        mySearchStr = false;
    }
    if (myOldSearchStr) {
        free(oldSearchStr);
        myOldSearchStr = false;
    }
    freeChannelList(channels);
    freeSearchCatList(cats);
}

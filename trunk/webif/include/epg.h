/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_EPG_H
#define WI_EPG_H

#include <time.h>
#include <klone/io.h>
#include "channels.h"
#include "conf.h"
#include "timers.h"

#define MAXLANGCODE2 8 // up to two 3 letter language codes, separated by '+' and zero terminated

typedef enum eventDumpMode_e {
    dmAll,
    dmNowNext,
    dmAtTime,
    dmBetween,
    dmById
} eventDumpMode_t;

enum eventFields_e { //usado como flags para determinar que variables se han de liberar
    EFI_TITLE = 1,
    EFI_SHORTDESC = 2,
    EFI_DESC = 4
};

//TODO audioType_t

typedef enum videoType_e {
    VT_UNKNOWN,
    VT_SD43,
    VT_SD169,
    VT_SD,
    VT_HD43,
    VT_HD169,
    VT_HD
} videoType_t;

typedef struct audio_s {
    unsigned char type;
    char lang[MAXLANGCODE2];
} audio_t;

typedef struct audioList_s {
    int length;
    audio_t *entry;
} audioList_t;

typedef struct epgEvent_s {
    int my;
    int id;
    char *title;
    char *shortdesc;
    char *desc;
    time_t start;
    int duration;
    int tableId;
    int version;
    videoType_t video;
    audioList_t audio;
} epgEvent_t;

//Lista de eventos de un canal

typedef struct channelEvents_s {
    int length;
    epgEvent_t *entry;
    int hostId;
    time_t start;
    time_t end;
    int eventId;
    int channelNum;
    eventDumpMode_t dumpMode;
} channelEvents_t;

//Lista de eventos de varios canales
//entre dos instantes de tiempo.

typedef struct events_s {
    int length;
    channelEvents_t *entry;
    int hostId;
    time_t start;
    time_t end;
    int eventId;
    int channelNum;
    eventDumpMode_t dumpMode;
} events_t;

void initEpgEvent(epgEvent_t * const event);
void initEpgEventFromArgs(wcontext_t *wctx, epgEvent_t * const event, vars_t *args);
void freeEpgEvent(epgEvent_t * const event);
void initChannelEvents(channelEvents_t * const list);
void freeChannelEvents(channelEvents_t * const list);
void initEvents(events_t * const list);
void freeEvents(events_t * const list);
void parse215E(char * line, epgEvent_t * const event);
void parseEventLine(char c, char *s, epgEvent_t * const event);
void parseEvents(char *data, events_t * const events, channelList_t const * const channels);
void getEvents(hostConf_t *vdrHost, events_t * const events, channelList_t const * const channels);
void getChannelEvents(wcontext_t *wctx, hostConf_t *vdrHost, channelEvents_t * const channelEvents);
void printChannelEvents(wcontext_t *wctx, const char *divId, channelEvents_t * const channelEvents, timerList_t * const timers);
void printEventDesc(wcontext_t *wctx, char * const desc);
void printEvent(wcontext_t *wctx, epgEvent_t * const event, const int channelNum
        , const char* toggleInfoClass
        , const char *const EventFromSpan
        , const char *const EventToSpan
        , const char *const EventDurationSpan
        , const char *const TimerCreate
        , const char *const SearchCreate
        , const char *const WebSearch);
void getEpgGridLimits(wcontext_t* wctx, const char *argStart, time_t *start, time_t *end);
void printEpgGrid(wcontext_t *wctx, events_t * const events, channelList_t * const channels, timerList_t * const timers, conflictList_t * const conflicts);
void printChannelEpg(wcontext_t *wctx, const char *divId, hostConf_t *host, const int channelNum, timerList_t * const timers);
void printTimersDiv(wcontext_t *wctx, const char *divId, timerList_t * const timers, const time_t start, const time_t end);
void printEpgPage(wcontext_t *wctx);

#endif

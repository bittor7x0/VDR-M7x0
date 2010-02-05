/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* (c) Christian Kelinski <k@kille.cx>
* Please checkout the README file!
* 
* Originally written for the open7x0.org VDR-FW project:
* www.open7x0.org
* 
* Rewritten for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __EPG_H__
#define __EPG_H__

#include <time.h>
#include <klone/io.h>
#include "channels.h"
#include "conf.h"
#include "timers.h"

typedef enum eventDumpMode_e{ 
	dmAll, 
	dmNowNext, 
	dmAtTime, 
	dmBetween, 
	dmByIds
} eventDumpMode_t;

enum eventFields_e { //usado como flags para determinar que variables se han de liberar
	EFI_TITLE=1,
	EFI_SHORTDESC=2,
	EFI_DESC=4
};

typedef struct epgEvent_s {
	int    my;
	int    id;
	char   *title;
	char   *shortdesc;
	char   *desc;
	time_t start;
	int    duration;
	int    tableId;
	int    version;
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

extern const char *gridDateFmt;
void initEpgEvent(epgEvent_t * const event);
void initEpgEventFromArgs(wcontext_t *wctx, epgEvent_t * const event, vars_t *args);
void freeEpgEvent(epgEvent_t * const event);
void initChannelEvents(channelEvents_t * const list);
void freeChannelEvents(channelEvents_t * const list);
void initEvents(events_t * const list);
void freeEvents(events_t * const list);
void parse215E(char * line, epgEvent_t * const event);
void parseEventLine(char c, char *s, epgEvent_t *const event);
void parseEvents(char *data, events_t * const events, channelList_t const * const channels);
void getEvents(hostConf_t *vdrHost, events_t * const events, channelList_t const * const channels);
void getChannelEvents(hostConf_t *vdrHost, channelEvents_t * const channelEvents);
void printEventDesc(wcontext_t *wctx, char * const desc, bool encode);
void printEvent(wcontext_t *wctx, epgEvent_t * const event, const int channelNum, bool compact, const char *const TimerCreate, const char *const SearchCreate);
void getEpgGridLimits(const char *argStart,time_t *start, time_t *end);
void printEpgGrid(wcontext_t *wctx, events_t * const events, channelList_t * const channels, timerList_t * const timers);
void printChannelEpg(wcontext_t *wctx, const char *id, hostConf_t *host, const int channelNum, timerList_t *const timers);
void printTimersDiv(wcontext_t *wctx, const char *id, timerList_t * const timers, const time_t start, const time_t end);

#endif

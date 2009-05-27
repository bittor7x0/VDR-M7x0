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

enum eventFields_s{
	EF_TITLE=1,
	EF_SHORTDESC=2,
	EF_DESC=4
};

typedef struct eventEntry_s {
	int    my;
	int    id;
	char   *title;
	char   *shortdesc;
	char   *desc;
	time_t start;
	int    duration;
	int    tableId;
	int    version;
} eventEntry_t;

typedef struct nowNextEntry_s {
	eventEntry_t event[2];
} nowNextEntry_t;

typedef struct nowNextList_s {
	int length;
	nowNextEntry_t *entry;
} nowNextList_t;

void initEE(eventEntry_t * const event);
void freeEE(eventEntry_t * const event);
void initNNE(nowNextEntry_t * const nowNext);
void freeNNE(nowNextEntry_t * const nowNext);
void initNNL(nowNextList_t * const list);
void freeNNL(nowNextList_t * const list);
void parse215E(char * line, eventEntry_t * const event);
boolean_t parseLineEvent(char c, char *s, eventEntry_t *const event);
void getNowNextList(hostConf_t *vdrHost, nowNextList_t * const list, channelList_t const * const channels);
void freeNNL(nowNextList_t * const list);
void printEventDesc(io_t *out, int ntabs, char * const desc, const char * const lineDelim, char * const aux);
void printEvent(io_t *out, int ntabs, eventEntry_t * const event);
void printEventInfobox(io_t *out, int ntabs, eventEntry_t * const event);
void initEventFromArgs(eventEntry_t * const event, vars_t *args);

#endif

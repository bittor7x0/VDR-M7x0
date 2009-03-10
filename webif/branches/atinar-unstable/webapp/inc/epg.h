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
* Modified for http://vdr-m7x0.foroactivo.com.es by:
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

typedef struct eventEntry_s {
	char   *title;
	char   *shortdesc;
	char   *desc;
	time_t time;
	int    duration;
} eventEntry;

typedef struct nowNextEntry_s {
	eventEntry event[2];
} nowNextEntry;

typedef struct nowNextList_s {
	int length;
	nowNextEntry *entry;
} nowNextList;

void initEE(eventEntry * const event);
void freeEE(eventEntry * const event);
void initNNE(nowNextEntry * const nowNext);
void freeNNE(nowNextEntry * const nowNext);
void initNNL(nowNextList * const list);
void freeNNL(nowNextList * const list);
void getNowNextList(nowNextList * const list, channelList const * const channels);
void freeNNL(nowNextList * const list);
void printInfo(io_t *out, const int ntabs, eventEntry * const ee);
void printInfobox(io_t *out, const int ntabs, eventEntry * const ee);

#endif

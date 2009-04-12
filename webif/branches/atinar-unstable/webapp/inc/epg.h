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

typedef struct eventEntry_s {
	char   *title;
	char   *shortdesc;
	char   *desc;
	time_t time;
	int    duration;
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
void getNowNextList(nowNextList_t * const list, channelList_t const * const channels);
void freeNNL(nowNextList_t * const list);
void printEventDesc(io_t *out, const char * const tabs, char * const desc);
void printInfo(io_t *out, const char * const tabs, eventEntry_t * const ee);
void printInfobox(io_t *out, const char * const tabs, eventEntry_t * const ee);

#endif

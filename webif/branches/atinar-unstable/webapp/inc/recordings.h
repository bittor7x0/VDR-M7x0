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

#ifndef __RECORDINGS_H__
#define __RECORDINGS_H__

#include "misc.h"

typedef struct recEntry_s {
	int id;
	time_t start;
	boolean_t seen;
	boolean_t direct;
	boolean_t cut;
	char * name;
	char * path;
} recEntry_t;

typedef struct recList_s {
	int length;
	recEntry_t *entry;
} recList_t;

typedef struct audioList_s {
	int length;
	char **entry;
} audioList_t;

typedef struct recInfo_s { //TODO =eventEntry
	char * title;
	char * subtitle;
	char * desc;
	char * channelId;
	time_t start;
	time_t stop;
	int duration;
	videoType_t video;
	audioList_t audio;
	char * path; //TODO deprecate
} recInfo_t;

void initRE(recEntry_t * const entry);
void freeRE(recEntry_t * const entry);
void initRL(recList_t  * const list);
void freeRL(recList_t  * const list);
void initRI(recInfo_t * const info);
void freeRI(recInfo_t * const info);
void getRecList(recList_t * const list, sortField_t sortBy, sortDirection_t sortDirection);
boolean_t editRec(const recEntry_t *rec, const recInfo_t * oldInfo, char ** message);
boolean_t deleRec(const recEntry_t *rec, const recInfo_t * oldInfo, char ** message);
boolean_t getRecInfo(const recEntry_t *rec, recInfo_t * const info);
#endif

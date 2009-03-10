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
	int ID;
	time_t start;
	int seen;
	int direct;
	int cut;
	char * title;
	char * path;
} recEntry_t;

typedef struct recList_s {
	int length;
	recEntry_t *entry;
} recList_t;


typedef struct recInfo_s {
	char * title;
	char * subtitle;
	char * desc;
	char * channelId;
	time_t start;
	time_t stop;
	int    duration;
	int	 ar; // 0=?, 1=4:3, 2=16:9, 3=?:?, 4=HD4:3, 5=HD16:9, 6=HD?:?
	char * audio;
} recInfo_t;

void initRE(recEntry_t * const entry);
void freeRE(recEntry_t * const entry);
void initRL(recList_t  * const list);
void freeRL(recList_t  * const list);
void getRecList(recList_t * const list, sortField_t sortBy, sortDirection_t sortDirection);
boolean_t editRec(int recId, const recInfo_t * oldInfo, const char * newName, char ** message);
boolean_t deleRec(const int recId, const recInfo_t * oldInfo, char ** message);

void initRI(recInfo_t * const info);
void freeRI(recInfo_t * const info);
boolean_t getRecInfo(recInfo_t * const info, const int recId);
boolean_t parseRecInfo(recInfo_t * const info, char * const data, boolean_t fromFile);
boolean_t readRecInfo(recInfo_t * const info, const char * filename);
#endif

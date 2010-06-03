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

#ifndef __TIMERS_H__
#define __TIMERS_H__

#include "channels.h"
#include "conf.h"

enum timerFlags_e { 
	TF_NONE = 0x0000,
	TF_ACTIVE = 0x0001,
	TF_INSTANT = 0x0002,
	TF_VPS = 0x0004,
	TF_RECORDING = 0x0008,
	TF_ALL = 0xFFFF
};

typedef enum timerType_e {
	TT_UNDEFINED = 0,
	TT_ONE_TIME = 1,
	TT_REGULAR = 2
} timerType_t;

enum timerFields_e {
	TF_TITLE=1,
	TF_AUX=2,
	TF_CHANNEL=4,
	TF_TIMER_STR=8,
};

typedef struct vdrTimer_s {
	int my;
	int id;
	int hostId;
 	time_t start;
	time_t stop;
	//TODO time_t eventStart;
	//TODO time_t eventStop;
	timerType_t type;
	int priority;
	int lifetime;
	uint flags;
	char wdays[8];
	char *title;
	char *aux;
	int channelNum;
	channel_t const *channel;
	char *timerStr;
	int count; //to create different html id's in program.kl1
	bool inConflict;
	int percent; //event coverage
} vdrTimer_t;

typedef struct timerList_s {
	int length;
	vdrTimer_t *entry;
} timerList_t;

//Implementation note:
//timers are holded in timerList, don't deallocate here
typedef struct conflict_s {
	int hostId;
	time_t when;
	vdrTimer_t *timer; //failed timer
	int nconcurrent;
	vdrTimer_t ** pconcurrent;
} conflict_t;

typedef struct conflictList_s {
	int length;
	conflict_t *entry;
} conflictList_t;


void initTimer(vdrTimer_t *const entry);
bool initTimerFromEvent(wcontext_t *wctx, vdrTimer_t *const timer, hostConf_t *host, const int channelNum, const int eventId);
bool initTimerFromArgs(vdrTimer_t *const timer, vars_t *args, wcontext_t *wctx);
void freeTimer(vdrTimer_t *const entry);
void initTimerList(timerList_t *const list);
void freeTimerList(timerList_t *const list);
void getTimerList(timerList_t *const timers, channelList_t const *const channels, const sortField_t sortBy, const sortDirection_t sortDirection);
bool addTimer(wcontext_t *wctx, hostConf_t *host, const char *newTimerStr);
bool editTimer(wcontext_t *wctx, hostConf_t *host, int id, const char *oldTimerStr, const char *newTimerStr);
bool deleteTimer(wcontext_t *wctx, hostConf_t *host, int id, const char *oldTimerStr);
bool parseTimer(const char *line, vdrTimer_t *const timer);
char *makeRegularTimerStr(uint flags,int channelNum,const char *wdays,time_t start,time_t stop,int priority,int lifetime,char *title,const char *aux);
char *makeOneTimeTimerStr(uint flags,int channelNum,time_t start,time_t stop,int priority,int lifetime,char *title,const char *aux);
char *makeTimerStr(vdrTimer_t *const timer);
void printTimerForm(wcontext_t *wctx, vdrTimer_t *const timer, channelList_t const *const channels);
void printTimerBars(wcontext_t *wctx, timerList_t *const timers, const int channelNum
	, const time_t startTime, const time_t duration,const char *TimerEdit,bool wrapPBWithA);
void initConflict(conflict_t * const conflict);
void freeConflict(conflict_t * const conflict);
void initConflictList(conflictList_t * const conflicts);
void freeConflictList(conflictList_t * const conflicts);
void getConflictList(conflictList_t * const conflicts, timerList_t const * timers);


#endif

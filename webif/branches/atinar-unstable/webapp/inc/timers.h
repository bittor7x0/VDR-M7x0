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
#include "misc.h"

enum timerFlags_e { 
	TF_NONE = 0x0000,
	TF_ACTIVE = 0x0001,
	TF_INSTANT = 0x0002,
	TF_VPS = 0x0004,
	TF_RECORDING = 0x0008,
	TF_ALL = 0xFFFF
};

typedef enum timerType_e {
	TT_UNDEFINED  = 0,
	TT_ONE_TIME = 1,
	TT_REGULAR  = 2
} timerType_t;

#define TIMER_ACTIVE(timer) boolean(timer.flags & TF_ACTIVE==TF_ACTIVE)

typedef struct timerEntry_s {
	int ID;
 	time_t start;
	time_t stop;
	time_t eventStart;
	time_t eventStop;
	timerType_t type;
	int priority;
	int lifetime;
	uint flags;
	char reg_timer[8];
	char * title;
	char * aux;
	int channelNum;
	channelEntry_t const *channel;
	char * timerStr;
	int count; //to create different html id's in program.kl1
} timerEntry_t;

typedef struct timerList_s {
	int length;
	timerEntry_t *entry;
} timerList_t;

void initTE(timerEntry_t * const entry);
void freeTE(timerEntry_t * const entry);
void initTL(timerList_t  * const list);
void freeTL(timerList_t  * const list);
void getTimerList(timerList_t * const timers, channelList_t const * const channels, sortField_t sortBy, sortDirection_t sortDirection);
void sortTimerList(timerList_t * const timers, sortField_t sortBy, sortDirection_t sortDirection);
boolean_t addTimer(const char * newTimerStr, char ** message);
boolean_t editTimer(int timerID, const char * oldTimerStr, const char * newTimerStr, char ** message);
boolean_t deleTimer(int timerID, const char * delTimerStr, char ** message);

#endif

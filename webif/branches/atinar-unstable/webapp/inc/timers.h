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

//Channelliste kurz
typedef struct timerEntry {
  time_t start;
  time_t stop;
  int type;           // Einmalig (ttOnce) oder Regelm‰ﬂig (ttPriodic)
  int priority;
  int lifetime;
  int active;
  int ID;
  char reg_timer[8];
  char title[100];
  int channelNum;
  char channelName[50];
  char mux[50];
  char * newt;
} timerEntry;

void initTE(timerEntry * o);
void freeTE(timerEntry * o, int max);
timerEntry * getTimerList(int * max, int sortBy, int sortDirection);
int checkForTimer(timerEntry * timerList, int maxTimer, int channelNum, time_t startTime, time_t endTime, int duration);
int addTimer(const char * newt);
int getTimerStrAt(int timerID, char timerStr[256]);
int editTimer(int timerID, char * oldTimer, char * newTimer);
int deleTimer(int timerID, char * timer);

#endif

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
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __RECORDINGS_H__
#define __RECORDINGS_H__

typedef struct recEntry {
  time_t start;
  int seen;
  int direct;
  int cut;
  char title[50];
  char path[200];
} recEntry;

typedef struct recEntry2 {
	char * title;
	char * subtitle;
	char * desc;
	time_t start;
	time_t stop;
	int    duration;
  char   channelName[50];
  char   channel_id[50];
  int    channelNum;
  int		 ar; // 0=?, 1=4:3, 2=16:9, 3=?:?, 4=HD4:3, 5=HD16:9, 6=HD?:?
  char * audio;
} recEntry2;

recEntry * getRecList(int * max, int sortBy, int sortDirection);
//int getChannel(char channelName[30]);

// returns 0 if its a dir with subdirs, 1 if its a repeating timer or 2 if its a dir with multiple recs, 3 if its a mix of 0 and 2
//int readRecDir(const char * path, int round, time_t * start, char ** title, int * numF, int * numD, int * size, char ** info);
int readRecDir(const char * path, int round, int * numF, int * numD, int * size, recEntry2 * info);
void freeRE2(recEntry2 * o, int max);
#endif
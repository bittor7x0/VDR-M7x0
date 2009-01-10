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
#include "channels.h"

//TODO sustituir en nowNextEntry
typedef struct eventEntry_s {
  char   channelId[50];
  int    channelNum;
  char   *title;
  char   *shortdesc;
  char   *desc;
  time_t time;
} eventEntry;

typedef struct nowNextEntry_s {
  char   channelName[50];
  char   channelId[50];
  int    channelNum;
  char   now_title[100];
  char   now_short[100];
  char   now_desc[502];
  time_t now_time;
  int    now_duration;
  char   next_title[100];
  char   next_short[100];
  char   next_desc[502];
  time_t next_time;
  int    next_duration;
} nowNextEntry;

void initNNE(nowNextEntry * now_next);
nowNextEntry * getNowNext(channelList *channels,int *max,int channelNum);
//int getChannel(char channelName[30]);

#endif

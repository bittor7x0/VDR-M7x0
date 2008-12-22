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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>

#include "svdrp_comm.h"
#include "svdrp_parse.h"
#include "misc.h"
#include "timers.h"
#include "channels.h"

// Sortiert die Liste, die beiden nächsten Variablen geben an,
// wonach und in welche Richtung sortiert wird.
int compareTE_sortBy=0;
int compareTE_sortDirection=1;
int compareTE(const void * a, const void * b) {
  switch (compareTE_sortBy) {
	case 0: return 0; break;
	case 1: return (( ((const timerEntry*)a)->start - ((const timerEntry*)b)->start )*compareTE_sortDirection); break;	
	case 2: return (( ((const timerEntry*)a)->stop - ((const timerEntry*)b)->stop )*compareTE_sortDirection); break;	
	case 3: return (( ((const timerEntry*)a)->type - ((const timerEntry*)b)->type )*compareTE_sortDirection); break;	
	case 4: return (strcasecmp(((const timerEntry*)a)->title,((const timerEntry*)b)->title)*compareTE_sortDirection); break;
	case 5: return (( ((const timerEntry*)a)->channelNum - ((const timerEntry*)b)->channelNum )*compareTE_sortDirection); break;	
	case 6: return (strcmp(((const timerEntry*)a)->channelName,((const timerEntry*)b)->channelName)*compareTE_sortDirection); break;
	case 7: return (( ((const timerEntry*)a)->priority - ((const timerEntry*)b)->priority )*compareTE_sortDirection); break;	
	case 8: return (( ((const timerEntry*)a)->lifetime - ((const timerEntry*)b)->lifetime )*compareTE_sortDirection); break;	
	case 9: return (( ((const timerEntry*)a)->active - ((const timerEntry*)b)->active )*compareTE_sortDirection); break;	
	case 10:return (strcmp(((const timerEntry*)a)->mux,((const timerEntry*)b)->mux)*compareTE_sortDirection); break;
	default: return 0; break;
  }
}

// Assign default timer values
void initTE(timerEntry * o) {
	if (o==NULL) return;
	o->active=1;
	o->lifetime=99;
	o->priority=50;
	o->newt=NULL;
}

// Free timer list
void freeTE(timerEntry * o,int max) {
	if (o==NULL) return;
	int i=0;
	for (i=0;i<max;i++) {
		free(o[i].newt);
	}
	free(o);
}

// Besorgt eine Liste von Timer vom VDR und sortiert sie
timerEntry * getTimerList(int * max, int sortBy, int sortDirection) {
  char * data;
  char * p;
  timerEntry *timer;
  channelList * channels;
  int i=0;
  int max_cl=-1;

  channels=get_channel_list(&max_cl);
  
  timer=NULL;
  write_svdrp("LSTT\r");
  data=read_svdrp();

  for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
    int code=atoi(p);
    if ( code==250 ) {
      i++;
      timerEntry *tmp=(timerEntry *)realloc(timer,i*sizeof*timer);
      if (!tmp) {
        warn("Reallocation failed. Old size is %d, new size should be %d",(i-1)*sizeof*timer,i*sizeof*timer);
        exit(1);
      }
      timer=tmp;
      timer[i-1].ID=strtol(p+4,&p,10);
      p+=strspn(p," ");
      timer[i-1].newt=strdup(p);
      parse_timer(p,&(timer[i-1]));
      if (timer[i-1].channelNum>0 && timer[i-1].channelNum<max_cl) {
        strcpy(timer[i-1].channelName,channels[timer[i-1].channelNum-1].channelName);
        strcpy(timer[i-1].mux,channels[timer[i-1].channelNum-1].multiplexName);
      } else {
        strcpy(timer[i-1].channelName,"");
        strcpy(timer[i-1].mux,"");
      }
    }
  }

  if (i>0 && sortBy!=0) {
    //Quick sort timers
	compareTE_sortBy=sortBy;
	compareTE_sortDirection=sortDirection;
    qsort(timer,i,sizeof*timer,compareTE);
  } 
  
  free(channels);
  free(data);
  *max=i;
  return timer;
}

// Checks, if there is an timer for the time-slot on channelNum
// descriped by startTime and endTime/duration onyl one of endTime an ddurtion is needed. 
// If both are passed, the function uses endTime.
// it returns:
// 0  if no timer matches
// 1  if theres a partial overlapping
// 2  if it's full covered by a timer
// -1 if theres a partial overlapping on a different channel (not implemented yet)
// -2 if it's full covered by a timer on a different channel (not implemented yet)
// -3 an error occured
int checkForTimer(timerEntry * timerList, int maxTimer, int channelNum, time_t startTime, time_t endTime, int duration) {
  if ( (!timerList) || (channelNum<1) || (maxTimer<1) || (startTime==0) || ( (endTime==0) && (duration==0) ) ) {
    return 0;	  
  } else {
    if (endTime==0) { endTime=startTime+duration; }
    int i=0;
    for (i=0;i<maxTimer;i++) {
      if (timerList[i].channelNum==channelNum) {
        if ( (timerList[i].start<startTime) && (timerList[i].stop>endTime) ) { return 2; }
        if ( (timerList[i].start<startTime) && (timerList[i].stop>startTime) ) { return 1; }
        if ( (timerList[i].start<endTime) && (timerList[i].stop>endTime) ) { return 1; }
      }
    }
    return 0;
  }
}

int addTimer(const char * newt) {
  char * data=NULL;
  char * p=NULL;
  char ret_code[10]="";
  int ok=0;
  
  p=malloc(strlen(newt)+2);
  sprintf(p,"%s\r",newt);
	  
  write_svdrp(p);
  data=read_svdrp();
  free(p); p=NULL;
  for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
    parse_ret_code(p,ret_code);
    if (!strcmp(ret_code,"250")) {
      ok=1;
    }
  }
  free(p); free(data);
  return ok;
}

int getTimerStrAt(int timerID, char timerStr[256]) {
  char * data;
  char * p;
  char retCodeStr[10];
  int retCode;
  int i=0;

  if ( (timerID>0) && (timerID<10000) ) {
    p=malloc(12);
    sprintf(p,"LSTT %d\r",timerID);
    write_svdrp(p);
    data=read_svdrp();
    free(p); p=NULL;
    for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
      retCode=parse_ret_code(p,retCodeStr);
      if (retCode>500) {
        return (retCode-500+1);
      } else if (retCode==250) {
        i=strcspn(p+4," ");
        strcpy(timerStr,p+i+5);
//        timerStr=strdup(p+i+4);
        return 0;
      }
    }
  }
}

int deleTimer(int timerID, char * timer) {
  char * data=NULL;
  char * p=NULL;
  char timerStr[256]="";
  char ret_code[10]="";
  int ok=3;
  
  if (getTimerStrAt(timerID,timerStr)==0) {
    if (!strcmp(timer,timerStr)) {
      p=malloc(12);
      sprintf(p,"DELT %d\r",timerID);
      write_svdrp(p);
      data=read_svdrp();
      free(p); p=NULL;
      for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
        parse_ret_code(p,ret_code);
        if (!strcmp(ret_code,"250")) {
        	ok=0;
        }        
      }
      free(p); free(data);
      return ok;      
    } else { 
    	dbg("Error trying to delete a timer: there is no timer as specified!");
    	dbg("Timer to delete: %s",timer);
    	dbg("Timer in list  : %s",timerStr);
    	return 2;
    }
  } else { return 1; }
}

int editTimer(int timerID, char * oldTimer, char * newTimer) {
  char * data=NULL;
  char * p=NULL;
  char timerStr[256]="";
  char ret_code[10]="";
  int ok=3;

  if (getTimerStrAt(timerID,timerStr)==0) {
    if (!strcmp(oldTimer,timerStr)) {
      p=malloc(strlen(newTimer)+12);
      sprintf(p,"MODT %d %s\r",timerID,newTimer);
      write_svdrp(p);
      data=read_svdrp();
      free(p); p=NULL;
      for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
        parse_ret_code(p,ret_code);
    	if (!strcmp(ret_code,"250")) {
    	  ok=0;
    	}        
      }
      free(p); free(data);
      return ok;
    } else { return 2; }
  } else { return 1; }
}

// Holt den Programmplatz des aktuellen Senders, liefert -1 zurück falls es einen Fehler gab.
/*
int getChannel(char channelName[30]) {
char * data;
int chanNum=0;
char chanName[30]="";

  write_svdrp("CHAN\r");
  data=read_svdrp();
  parse_chan(data,&chanNum,chanName);
  if (channelName!=NULL) {
    strcpy(channelName,chanName);
  }  
  return chanNum;
}
*/

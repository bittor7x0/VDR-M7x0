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

// Compare two timerEntries using the field and order,
// specified by this two variables.
int compareTE_sortBy=0;
int compareTE_sortDirection=1;
int compareTE(const void * a, const void * b) {
	const timerEntry * ta=(const timerEntry *)a;
	const timerEntry * tb=(const timerEntry *)b;
	switch (compareTE_sortBy) {
		case 1: return compareTE_sortDirection * ( ta->start - tb->start );	
		case 2: return compareTE_sortDirection * ( ta->stop - tb->stop );	
		case 3: return compareTE_sortDirection * ( ta->type - tb->type );	
		case 4: return compareTE_sortDirection * strcasecmp(ta->title,tb->title);
		case 5: return compareTE_sortDirection * ( ta->channelNum - tb->channelNum );	
		case 6: return compareTE_sortDirection * strcmp(ta->channelName,tb->channelName);
		case 7: return compareTE_sortDirection * ( ta->priority - tb->priority );	
		case 8: return compareTE_sortDirection * ( ta->lifetime - tb->lifetime );	
		case 9: return compareTE_sortDirection * ( ta->active - tb->active );	
		case 10:return compareTE_sortDirection * strcmp(ta->mux,tb->mux);
		default: return 0;
	}
}

// Assign default timer values
void initTE(timerEntry * o) {
	if (o==NULL) return;
	o->active=1;
	o->lifetime=99;
	o->priority=50;
	o->title=NULL;
	o->aux=NULL;
	o->newt=NULL;
	o->channelName[0]='\0';
	o->mux[0]='\0';
}

// Free timer entry
void freeTE(timerEntry o) {
	free(o.title);
	free(o.aux);
	free(o.newt);
}

// Free timer list
void freeTimerList(timerEntry * o,int max) {
	if (o==NULL) return;
	int i=0;
	for (i=0;i<max;i++) {
		freeTE(o[i]);
	}
	free(o);
}

// Retrieve a timer list from VDR and sort it
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

	for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		if (atoi(p)==250) {
			i++;
			timerEntry *tmp=(timerEntry *)realloc(timer,i*sizeof*timer);
			if (!tmp) {
				warn("Reallocation failed. Old size is %d, new size should be %d",(i-1)*sizeof*timer,i*sizeof*timer);
				exit(1);
			}
			timer=tmp;
			tmp=&(timer[i-1]);
			initTE(tmp);
			tmp->ID=strtol(p+4,&p,10);
			p+=strspn(p," ");
			tmp->newt=strdup(p);
			parse_timer(p,tmp);
			if (tmp->channelNum>0 && tmp->channelNum<max_cl) {
				strcpy(tmp->channelName,channels[tmp->channelNum-1].channelName);
				strcpy(tmp->mux,channels[tmp->channelNum-1].multiplexName);
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
		if (endTime==0) { 
			endTime=startTime+duration; 
		}
		int i;
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
	int ok=0;
  
	if (asprintf(&p,"%s\r",newt)<0) {
		return 0;
	}  
	write_svdrp(p);
	free(p);
	data=read_svdrp();
	if (data!=NULL){
		for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
			if (atoi(p)==250) {
				ok=1;
				break;
			}
		}
		free(data);
	}
	return ok;
}

char * getTimerStrAt(int timerID) {
	char * data;
	char * p;
	char * timerStr=NULL;

	if ( (timerID>0) && (timerID<10000) ) {
		if (asprintf(&p,"LSTT %d\r",timerID)<0){
			warn("Not enough memory for command in getTimerStrAt");
		} else {
			write_svdrp(p);
			free(p);
			data=read_svdrp();
			if (data!=NULL) {
				for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
					if (atoi(p)==250) {
						p+=4;
						p+=strcspn(p," "); //ID
						p+=strspn(p," ");
						timerStr=strdup(p);
						break;
					}
				}
				free(data);  //TODO_KILLE
			}
		}
	}
	return timerStr;
}

int deleTimer(int timerID, char * timer) {
	char * data=NULL;
	char * p=NULL;
	char * timerStr;
	int ok=3;
  
	timerStr=getTimerStrAt(timerID);
	if (timerStr!=NULL) {
		if (strcmp(timer,timerStr)==0) {
			if (asprintf(&p,"DELT %d\r",timerID)<0){
				warn("Not enough memory for command in deleTimer");
			} else {
				write_svdrp(p);
				free(p);
				data=read_svdrp();
				if (data!=NULL){
					for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
						if (atoi(p)==250) {
							ok=0;
							break;
						}
					}
					free(data);
				}
			}
		} else { 
			warn("Error trying to delete a timer: there is no timer as specified!");
			warn("Timer to delete(%d): [%s]",strlen(timer),   timer);
			warn("Timer in list  (%d): [%s]",strlen(timerStr),timerStr);
			ok=2;
		}
		free(timerStr);
	} else {
		ok=1;
	}
	return ok;
}

int editTimer(int timerID, char * oldTimer, char * newTimer) {
	char * data=NULL;
	char * p=NULL;
	char * timerStr;
	int ok=3;

	timerStr=getTimerStrAt(timerID);
	if (timerStr!=NULL) {
		if (strcmp(oldTimer,timerStr)==0) {
			if (asprintf(&p,"MODT %d %s\r",timerID,newTimer)<0){
				warn("Not enough memory for command in editTimer");
			} else {
				write_svdrp(p);
				free(p);
				data=read_svdrp();
				if (data!=NULL){
					for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
						if (atoi(p)==250) {
							ok=0;
							break;
						}        
					}
					free(data);
				}
			}
		} else {
			warn("Error trying to edit a timer: there is no timer as specified!");
			warn("Timer to edit(%d): [%s]",strlen(oldTimer),oldTimer);
			warn("Timer in list(%d): [%s]",strlen(timerStr),timerStr);
			ok=2;
		}
		free(timerStr);
	} else {
		ok=1;
	}
	return ok;
}

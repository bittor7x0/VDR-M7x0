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

/*
* Compare two timerEntries using the field and order,
* specified by this two variables.
*/
sortField_t compareTE_sortBy=SF_NONE;
sortDirection_t compareTE_sortDirection=SD_ASC;
int compareTE(const void * a, const void * b) {
	const timerEntry_t * ta=(const timerEntry_t *)a;
	const timerEntry_t * tb=(const timerEntry_t *)b;
	if (compareTE_sortDirection==SF_NONE) return 0;
	switch (compareTE_sortBy) {
		case SF_TM_NUMBER:   return compareTE_sortDirection * ( ta->ID-tb->ID );
		case SF_START:    return compareTE_sortDirection * ( ta->start - tb->start );	
		case SF_TYPE:     return compareTE_sortDirection * ( ta->type - tb->type );	
		case SF_TITLE:    return compareTE_sortDirection * strcasecmp(ta->title,tb->title);
		case SF_CH_NUMBER:   return compareTE_sortDirection * ( ta->channelNum - tb->channelNum );	
		case SF_NAME:     
			return compareTE_sortDirection * (ta->channel && tb->channel ) ? strcmp(ta->channel->channelName,tb->channel->channelName) : 0;
		case SF_PRIORITY: return compareTE_sortDirection * ( ta->priority - tb->priority );	
		case SF_LIFETIME: return compareTE_sortDirection * ( ta->lifetime - tb->lifetime );	
		case SF_ACTIVE:   return compareTE_sortDirection * ( ta->active - tb->active );	
		case SF_MUX:
			return compareTE_sortDirection * (ta->channel && tb->channel) ? strcmp(ta->channel->multiplexName,tb->channel->multiplexName) : 0;
		default:          return 0;
	}
}

// Assign default timer values
void initTE(timerEntry_t * const entry) {
	if (entry==NULL) return;
	entry->active=1;
	entry->lifetime=99;
	entry->priority=50;
	entry->title=NULL;
	entry->aux=NULL;
	entry->timerStr=NULL;
	entry->channel=NULL;
	strcpy(entry->reg_timer,"-------"); 
	entry->count=0;
}

// Free timer entry
void freeTE(timerEntry_t * const entry) {
	free(entry->title);
	free(entry->aux);
	free(entry->timerStr);
	initTE(entry);
}

void initTL(timerList_t * const list){
	list->length=0;
	list->entry=NULL;
}

void freeTL(timerList_t * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeTE(&(list->entry[i]));
	}
	free(list->entry);
	initTL(list);
}


// Retrieve a timer list from VDR and sort it
void getTimerList(timerList_t * const timers, channelList_t const * const channels, sortField_t sortBy, sortDirection_t sortDirection){
	char * data;
	char * p;
	int i=0;

	initTL(timers);
	if (write_svdrp("LSTT\r")<=0){
		return;
	}
	data=read_svdrp();

	for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		if (atoi(p)==250) {
			i++;
			timerEntry_t *tmp=(timerEntry_t *)realloc(timers->entry,i*sizeof(timerEntry_t));
			if (!tmp) {
				warn("getTimerList:Reallocation failed.");
				exit(1);
			}
			timers->length=i;
			timers->entry=tmp;
			tmp=&(timers->entry[i-1]);
			initTE(tmp);
			tmp->ID=strtol(p+4,&p,10);
			p+=strspn(p," ");
			tmp->timerStr=strdup(p);
			parseTimer(p,tmp);
			if (tmp->channelNum>0 && tmp->channelNum<=channels->length) {
				tmp->channel = &channels->entry[tmp->channelNum-1];
			}
		}
	}
	free(data);
	sortTimerList(timers,sortBy,sortDirection);
}

void sortTimerList(timerList_t * const timers, sortField_t sortBy, sortDirection_t sortDirection){
	if (timers->length>0 && sortBy!=SF_NONE) {
		//Quick sort timers
		compareTE_sortBy=sortBy;
		compareTE_sortDirection=sortDirection;
		qsort(timers->entry,timers->length,sizeof(timerEntry_t),compareTE);
	} 
}


boolean_t addTimer(const char * newTimerStr, char ** message) {
	boolean_t result= BT_FALSE;
	char * command=NULL;
  
	if (newTimerStr==NULL) {
		return BT_FALSE;
	}
	if (asprintf(&command,"NEWT %s\r",newTimerStr)<0) {
		warn("Not enough memory for command in addTimer");
		exit(1);
	} else {
		int error=(write_svdrp(command)<=0);
		free(command);
		if (!error) {
			char * data=read_svdrp();
			if (data!=NULL){
				char * p=data;
				int code=strtol(p,&p,10);
				result=boolean(code==250);
				if (message && *p && *(++p)){
					(*message)=strdup(p);
				}
				free(data);
			}
		}
	}
	return result;
}

char * getTimerStrAt(int timerID) {
	char * command=NULL;
	char * timerStr=NULL;

	if ( (timerID>0) && (timerID<10000) ) {
		if (asprintf(&command,"LSTT %d\r",timerID)<0){
			warn("Not enough memory for command in getTimerStrAt");
			exit(1);
		} else {
			int error=(write_svdrp(command)<=0);
			free(command);
			if (!error) {
				char * data=read_svdrp();
				if (data!=NULL) {
					char * p;
					for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
						if (atoi(p)==250) {
							p+=4;
							p+=strcspn(p," "); //ID
							p+=strspn(p," ");
							timerStr=strdup(p);
							break;
						}
					}
					free(data);
				}
			}
		}
	}
	return timerStr;
}

boolean_t deleTimer(int timerID, const char * delTimerStr, char ** message) {
	boolean_t result=BT_FALSE;
	char * command=NULL;
	char * timerStr;
  
	timerStr=getTimerStrAt(timerID);
	if (timerStr!=NULL) {
		if (strcmp(delTimerStr,timerStr)==0) {
			if (asprintf(&command,"DELT %d\r",timerID)<0){
				warn("Not enough memory for command in deleTimer");
				exit(1);
			} else {
				int error=(write_svdrp(command)<=0);
				free(command);
				if (!error){
					char * data=read_svdrp();
					if (data!=NULL){
						char * p=data;
						int code=strtol(p,&p,10);
						result=boolean(code==250);
						if (message && *p && *(++p)){
							(*message)=strdup(p);
						}
						free(data);
					}
				}
			}
		} else { 
			warn("Error trying to delete a timer: there is no timer as specified!");
			warn("Timer to delete(%d): [%s]",strlen(delTimerStr), delTimerStr);
			warn("Timer in list  (%d): [%s]",strlen(timerStr),timerStr);
		}
		free(timerStr);
	}
	return result;
}

boolean_t editTimer(int timerID, const char * oldTimerStr, const char * newTimerStr, char ** message) {
	boolean_t result= BT_FALSE;
	char * timerStr;

	if ((oldTimerStr==NULL) || (newTimerStr==NULL)){
		return BT_FALSE;
	}
	if (strcmp(oldTimerStr,newTimerStr)==0){
		return BT_TRUE;
	}
	timerStr=getTimerStrAt(timerID);
	if (timerStr!=NULL) {
		if (strcmp(oldTimerStr,timerStr)==0) {
			char * command=NULL;
			if (asprintf(&command,"MODT %d %s\r",timerID,newTimerStr)<0){
				warn("Not enough memory for command in editTimer");
				exit(1);
			} else {
				int error=(write_svdrp(command)<=0);
				free(command);
				if (!error){
					char * data=read_svdrp();
					if (data!=NULL){
						char * p=data;
						int code=strtol(p,&p,10);
						result=boolean(code==250);
						if (message && *p && *(++p)){
							(*message)=strdup(p);
						}
						free(data);
					}
				}
			}
		} else {
			warn("Error trying to edit a timer: there is no timer as specified!");
			warn("Timer to edit(%d): [%s]",strlen(oldTimerStr),oldTimerStr);
			warn("Timer in list(%d): [%s]",strlen(timerStr),timerStr);
		}
		free(timerStr);
	}
	return result;
}


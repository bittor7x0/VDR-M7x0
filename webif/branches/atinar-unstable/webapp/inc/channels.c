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
#include "channels.h"

sortField_t compareCE_sortBy=SF_NONE;
sortDirection_t compareCE_sortDirection=SD_ASC;
int compareCE(const void * a, const void * b) {
	const channelEntry_t *ca = (const channelEntry_t*)a;
	const channelEntry_t *cb = (const channelEntry_t*)b;
	int result;
	switch (compareCE_sortBy) {
		case SF_MUX:
			result=strcasecmp(ca->multiplexName,cb->multiplexName)*compareCE_sortDirection;
			if (result) return result;
		case SF_NAME: 
			result=strcasecmp(ca->channelName,cb->channelName)*compareCE_sortDirection;
			if (result) return result;
		case SF_CH_NUMBER: 
			return (ca->channelNum-cb->channelNum)*compareCE_sortDirection;
		default: 
			return 0;
	}
}

void initCE(channelEntry_t * const entry){
	entry->channelId=NULL;
	entry->channelName=NULL;
	entry->multiplexName=NULL;
	entry->parameter=NULL;
	entry->source=NULL;
}

void freeCE(channelEntry_t * const entry){
	free(entry->channelId);
	free(entry->channelName);
	free(entry->multiplexName);
	free(entry->parameter);
	free(entry->source);
	initCE(entry);
}

void initCL(channelList_t * const list){
	list->length=0;
	list->entry=NULL;
}

void freeCL(channelList_t * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeCE(&(list->entry[i]));
	}
	free(list->entry);
	list->length=0;
	list->entry=NULL;
}

// Obtain the channel list from VDR
void getChannelList(channelList_t * const list, sortField_t sortBy, sortDirection_t sortDirection) {
	initCL(list);
	
	char *data, *p;
	if (write_svdrp("LSTC\r")<=0){
		return;
	}
	data=read_svdrp();
	
	for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		if (atoi(p)==250){
			channelEntry_t * tmp=(channelEntry_t *)realloc(list->entry,(list->length+1)*sizeof(channelEntry_t));
			if (!tmp) {
				warn("getChannelList:Reallocation failed.");
				exit(1);
			}
			list->entry=tmp;
			list->length++;
			parseChannel(p+4,&list->entry[list->length-1]);
		}
	}
	free(data);
	if (list->length>0 && sortBy!=SF_NONE) {
		//Quick sort recordings
		compareCE_sortBy=sortBy;
		compareCE_sortDirection=sortDirection;
		qsort(list->entry,list->length,sizeof(channelEntry_t),compareCE);
	} 
}

boolean_t getChannel(int channelNum, channelEntry_t * const channel) {
	char cmd[10];
	char *data;
	char *r;
	int l;
	int code;

	initCE(channel);
	if (channelNum<1) {
		sprintf(cmd,"CHAN\r");
	} else {
		sprintf(cmd,"CHAN %d\r",channelNum);
	}
	if (write_svdrp(cmd)<=0){
		return 0;
	}
	data=read_svdrp();
	if (data==NULL) {
		return 0;
	}
	code=strtol(data,&r,10);
	if (code!=250) {
		return BT_FALSE;
	}
	r++;
	channel->channelNum=strtol(r,&r,10);
	r++;
	l=strcspn(r,"\r\n");
	channel->channelName=strndup(r,l);
	return BT_TRUE;
}

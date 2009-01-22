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

void initCE(channelEntry * const entry){
	entry->channelId=NULL;
	entry->channelName=NULL;
	entry->multiplexName=NULL;
	entry->parameter=NULL;
	entry->source=NULL;
}

void freeCE(channelEntry * const entry){
	free(entry->channelId);
	free(entry->channelName);
	free(entry->multiplexName);
	free(entry->parameter);
	free(entry->source);
	initCE(entry);
}

void initCL(channelList * const list){
	list->length=0;
	list->entry=NULL;
}

void freeCL(channelList * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeCE(&(list->entry[i]));
	}
	free(list->entry);
	list->length=0;
	list->entry=NULL;
}

// Obtain the channel list from VDR
void getChannelList(channelList * const list) {
	list->length=0;
	list->entry=NULL;
	
	char *data, *p;
	if (write_svdrp("LSTC\r")<=0){
		return;
	}
	data=read_svdrp();
	
	for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		if (atoi(p)==250){
			channelEntry * tmp=(channelEntry *)realloc(list->entry,(list->length+1)*sizeof(channelEntry));
			if (!tmp) {
				warn("Reallocation failed.");
				exit(1);
			}
			list->entry=tmp;
			list->length++;
			parseChannel(p+4,&list->entry[list->length-1]);
		}
	}
	free(data);
}

int getChannel(int channelNum, channelEntry * const channel) {
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
		return 0;
	}
	r++;
	channel->channelNum=strtol(r,&r,10);
	r++;
	l=strcspn(r,"\r\n");
	channel->channelName=strndup(r,l);
	return ~0;
}

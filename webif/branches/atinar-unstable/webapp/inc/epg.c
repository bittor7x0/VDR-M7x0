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
#include "epg.h"
#include "channels.h"

void initNNE(nowNextEntry * now_next){
	strcpy(now_next[0].now_title,"");
	strcpy(now_next[0].now_desc,"");
	strcpy(now_next[0].now_short,"");
	now_next[0].now_time=0;
	now_next[0].now_duration=0;
	strcpy(now_next[0].next_title,"");
	strcpy(now_next[0].next_desc,"");
	strcpy(now_next[0].next_short,"");
	now_next[0].next_time=0;
	now_next[0].next_duration=0;
}

void freeNowNextEntry(nowNextEntry * o,int max) {
	int i=0;
//	for (i=0;i<max;i++) {
//	}
	free(o);
}

nowNextEntry * getNowNext(channelList *channels,int *max, int channelNum) {
	//TODO quitar nombre del canal de nowNext y convertir en array de dos eventos.
	char *p;
	char cmd[20];
	int pass=0;
	;
	nowNextEntry *now_next;
	int i=0;
	int k=0;
	int error=0;
	
	*max=0;
	if (channels->length<1){
		return NULL;
	}
	
	if (channelNum>0) {
		now_next=(nowNextEntry *)malloc(sizeof*now_next);
		if (now_next==NULL){
			return NULL;
		}
		strcpy(now_next[0].channelId,channels->entry[channelNum-1].channelId);
		strcpy(now_next[0].channelName,channels->entry[channelNum-1].channelName);
		now_next[0].channelNum=channelNum;
		initNNE(now_next);
	} else {
		now_next=(nowNextEntry *)malloc(channels->length*sizeof*now_next);
		if (now_next==NULL){
			return NULL;
		}
		for(i=0;i<channels->length;i++) {
			strcpy(now_next[i].channelId,channels->entry[i].channelId);
			strcpy(now_next[i].channelName,channels->entry[i].channelName);
			now_next[i].channelNum=i+1;
			initNNE(&now_next[i]);
		}
	}
	
	for(pass=0;pass<2;pass++) {
		if (channelNum>0) {
			sprintf(cmd,"LSTE %d %s\r",channelNum, (pass==0) ? "now" : "next");
		} else {
			sprintf(cmd,"LSTE %s\r", (pass==0) ? "now" : "next");
		}	
		if (write_svdrp(cmd)<=0) {
			break;
		}
	
		char * data;
		data=read_svdrp();
		if (data==NULL){
			break;
		}
		i=0;
		for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
			if (i>=channels->length){
				break;
			}
			int code = strtol(p,&p,10);
			if (code==215 && p[0]=='-') {
				p++;
				if (*p=='C') {
					if (channelNum==0) {
						p+=2;
						k=strcspn(p," ");
						for(i=0;i<channels->length;i++) {
							if (strncmp(now_next[i].channelId,p,k)==0) {  //TODO now_next -> channels
								 break;
							}
						}
					}
				} else if (*p=='D') {
					p+=2;
					switch (pass) {
						case 0 :strncpy(now_next[i].now_desc,p,500); break;
						case 1 :strncpy(now_next[i].next_desc,p,500); break;
					}
				} else if (*p=='T') {
					p+=2;
					switch (pass) {
						case 0 :strcpy(now_next[i].now_title,p); break;
						case 1 :strcpy(now_next[i].next_title,p); break;
					}
				} else if (*p=='S') {
					p+=2;
					switch (pass) {
						case 0 :strcpy(now_next[i].now_short,p); break;
						case 1 :strcpy(now_next[i].next_short,p); break;
					}
				} else if (*p=='E') {
					p+=2;
					switch (pass) {
						case 0: parse_215E(p,NULL,&(now_next[i].now_time),&(now_next[i].now_duration),NULL,NULL); break;
						case 1: parse_215E(p,NULL,&(now_next[i].next_time),&(now_next[i].next_duration),NULL,NULL); break;
					}
				}
			}
		}
		free(data);
	}
exit:
	*max=channels->length;
	return now_next;
}

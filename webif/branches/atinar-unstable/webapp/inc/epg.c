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

void initEE(eventEntry * const event){
	event->title=NULL;
	event->desc=NULL;
	event->shortdesc=NULL;
	event->time=0;
	event->duration=0;
}

void freeEE(eventEntry * const event){
	free(event->title);
	free(event->desc);
	free(event->shortdesc);
	initEE(event);
}
	

void initNNE(nowNextEntry * const nowNext){
	int i;
	for (i=0;i<2;i++) initEE(&nowNext->event[i]);
}

void freeNNE(nowNextEntry * const nowNext) {
	int i;
	for (i=0;i<2;i++) freeEE(&nowNext->event[i]);
}

void initNNL(nowNextList * const list){
	list->entry=NULL;
	list->length=0;
}

void freeNNL(nowNextList * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeNNE(&list->entry[i]);
	}
	free(list->entry);
	initNNL(list);
}

void getNowNextList(nowNextList * const list, channelList const * const channels) {
	char *p;
	char cmd[20];
	int pass=0;
	int i=0;
	int k=0;
	int error=0;
	
	initNNL(list);
	if (channels->length<1){
		return;
	}
	
	list->length=channels->length;
	list->entry=(nowNextEntry *)malloc(list->length*sizeof(nowNextEntry));
	if (list->entry==NULL){
		list->length=0;
		return;
	}
	for(i=0;i<list->length;i++) {
		initNNE(&list->entry[i]);
	}
	
	for(pass=0;pass<2;pass++) {
		if (channels->length==1) {
			sprintf(cmd,"LSTE %d %s\r",channels->entry[0].channelNum, (pass==0) ? "now" : "next");
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
					if (channels->length>1) {
						p+=2;
						k=strcspn(p," ");
						for(i=0;i<channels->length;i++) {
							if (strncmp(channels->entry[i].channelId,p,k)==0) {
								break;
							}
						}
					}
					initEE(&list->entry[i].event[pass]);
				} else if (*p=='D') {
					p+=2;
					list->entry[i].event[pass].desc=strdup(p);
				} else if (*p=='T') {
					p+=2;
					list->entry[i].event[pass].title=strdup(p);
				} else if (*p=='S') {
					p+=2;
					list->entry[i].event[pass].shortdesc=strdup(p);
				} else if (*p=='E') {
					p+=2;
					parse_215E(p,NULL,&(list->entry[i].event[pass].time),&(list->entry[i].event[pass].duration),NULL,NULL); 
				} else if (*p=='e' && channels->length==1) {
					break;
				}
			}
		}
		free(data);
	}
}

char * io_printf_infobox(io_t *out, eventEntry * const ee){
	char *eT;
	struct tm *timeptr;
	long int end_time = ee->time+ee->duration;
	io_printf(out,"<div class=\"box_right\"><div class=\"infobox\">&nbsp;<img src=\"/img/info16.png\" alt=\"%s\" /><div class=\"shadow\"><div class=\"info\">\n",tr("more infos"));
	eT=encode_printf(out,"  <b>%s</b><br />",ee->title);
	free(encode_printf(out,"  %s<br />\n",ee->shortdesc));
	timeptr=localtime(&ee->time);
	io_printf(out,"  %s <nobr><b>%02d:%02d</b></nobr>",tr("from"),timeptr->tm_hour,timeptr->tm_min);
	timeptr=localtime(&end_time);
	io_printf(out," %s <nobr><b>%02d:%02d</b></nobr> (%s: %s %d %s)\n  <p>",tr("to"),timeptr->tm_hour,timeptr->tm_min,tr("runtime"),tr("approx."),ee->duration/60,tr("minutes"));
	free(encode_printf(out,"%s",ee->desc));
	io_printf(out,"</p></div></div></div></div>");
	return eT;
}


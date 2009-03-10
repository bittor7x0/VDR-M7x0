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

void printInfo(io_t *out, const int ntabs, eventEntry * const ee){
	struct tm startt,endt;
	long int end_time = ee->time+ee->duration;
	char tabs[ntabs+1];
	int t;
	for(t=0;t<ntabs;t++) tabs[t]='\t';
	tabs[ntabs]=0;
	io_printf(out,
"%s<div class=\"title\">%s</div>\n"
		,tabs,ee->title
	);
	if (ee->shortdesc && strlen(ee->shortdesc)>0){
		io_printf(out,
"%s<div class=\"subtitle\">%s</div>\n"
			,tabs,ee->shortdesc
		);
	}
	startt=*localtime(&ee->time);
	endt=*localtime(&end_time);
		io_printf(out,
"%s<div class=\"duration\">"
			,tabs
		);
	io_printf(out
		,tr("fromToFmt")
		,startt.tm_hour,startt.tm_min,endt.tm_hour,endt.tm_min,ee->duration/60
	);
	io_printf(out,
			"</div>\n"
	);
	if (ee->desc && strlen(ee->desc)>0){
		io_printf(out,
"%s<div class=\"desc\">\n"
			,tabs
		);
		char *p=ee->desc;
		int l;
		char c;
		for(;;){
			l=strcspn(p,"|");
			if (l==0) break;
			c=p[l];
			p[l]=0;
			io_printf(out,
"%s	<p>%s</p>\n"
			,tabs,p
			);
			if(c==0) break;
			p[l]=c;
			p+=l;
			p+=strspn(p,"|");
		}
		io_printf(out,
"%s</div>\n"
			,tabs
		);
	}
}

void printInfobox(io_t *out, const int ntabs, eventEntry * const ee){
	char tabs[ntabs+1];
	int t;
	for(t=0;t<ntabs;t++) tabs[t]='\t';
	tabs[ntabs]=0;
	io_printf(out,
"%s<div class=\"infobox infoboxCssHover\"><span class=\"ui-icon ui-icon-info\">%s</span>\n"
"%s	<div class=\"infoWrapper\">\n"
"%s		<div class=\"info\">\n"
		,tabs
		,tr("moreInfo")
		,tabs
		,tabs
	);
	printInfo(out,ntabs+3,ee);
	io_printf(out,
"%s		</div>\n"
"%s	</div>\n"
"%s</div>\n"
		,tabs
		,tabs
		,tabs
	);
}


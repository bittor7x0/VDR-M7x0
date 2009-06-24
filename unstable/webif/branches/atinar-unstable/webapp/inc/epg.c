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
* Rewritten for http://vdr-m7x0.foroactivo.com.es by:
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
#include <klone/utils.h>

#include "svdrp_comm.h"
#include "i18n.h"
#include "misc.h"
#include "epg.h"
#include "channels.h"

void initEE(eventEntry_t * const event){
	event->my=0; //strdup nothing
	event->id=0;
	event->title=NULL;
	event->desc=NULL;
	event->shortdesc=NULL;
	event->start=0;
	event->duration=0;
}

void initEventFromArgs(eventEntry_t * const event, vars_t *args){
	event->title=(vars_countn(args, "title")>0) 
		? condstrdup(isFlagSet(EFI_TITLE,event->my),vars_get_value(args,"title")) 
		: NULL;
	event->shortdesc=(vars_countn(args, "shortdesc") > 0) 
		? condstrdup(isFlagSet(EFI_SHORTDESC,event->my),vars_get_value(args,"shortdesc")) 
		: NULL;
	event->desc=(vars_countn(args, "desc")>0) 
		? condstrdup(isFlagSet(EFI_DESC,event->my),vars_get_value(args,"desc")) 
		: NULL;
}

void freeEE(eventEntry_t * const event){
	if (isFlagSet(EFI_TITLE,event->my))
		free(event->title);
	if (isFlagSet(EFI_DESC,event->my))
		free(event->desc);
	if (isFlagSet(EFI_SHORTDESC,event->my))
		free(event->shortdesc);
	initEE(event);
}

void initNNE(nowNextEntry_t * const nowNext){
	int i;
	for (i=0;i<2;i++) initEE(&nowNext->event[i]);
}

void freeNNE(nowNextEntry_t * const nowNext) {
	int i;
	for (i=0;i<2;i++) freeEE(&nowNext->event[i]);
}

void initNNL(nowNextList_t * const list){
	list->entry=NULL;
	list->length=0;
}

void freeNNL(nowNextList_t * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeNNE(&list->entry[i]);
	}
	free(list->entry);
	initNNL(list);
}

void parseEventRecord(char * line, eventEntry_t * const event) {
	char *r=line;
	int k;
	int l;
	for(k=0;k<5;k++) {
		r+=strspn(r," ");
		l=strcspn(r," ");
		switch (k) {
			case 0: event->id      =strtoul(r,NULL,10); break;
			case 1: event->start   =strtol(r,NULL,10); break;
			case 2: event->duration=strtol(r,NULL,10); break;
			case 3: event->tableId =strtol(r,NULL,16); break;
			case 4: event->version =strtol(r,NULL,16); break;
		}
		r+=l;
	}
}

boolean_t parseLineEvent(char c, char *s, eventEntry_t *const event){
	switch (c) {
		case 'E':
			parseEventRecord(s,event);
			return BT_TRUE;
		case 'T':
			crit_goto_if((event->title=strdup(s))==NULL,outOfMemory);
			setFlag(EFI_TITLE,event->my);
			return BT_TRUE;
		case 'D':
			crit_goto_if((event->desc=strdup(s))==NULL,outOfMemory);
			setFlag(EFI_DESC,event->my);
			return BT_TRUE;
		case 'S': 
			crit_goto_if((event->shortdesc=strdup(s))==NULL,outOfMemory);
			setFlag(EFI_SHORTDESC,event->my);
			return BT_TRUE;
		default:
			return BT_FALSE;
	}
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getNowNextList(hostConf_t *vdrHost, nowNextList_t * const list, channelList_t const * const channels) {
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
	list->entry=(nowNextEntry_t *)malloc(list->length*sizeof(nowNextEntry_t));
	if (list->entry==NULL){
		list->length=0;
		return;
	}
	for(i=0;i<list->length;i++) {
		initNNE(&list->entry[i]);
	}
	
	for(pass=0;pass<2;pass++) {
		if (channels->length==1) {
			sprintf(cmd,"LSTE %d %s",channels->channel[0].channelNum, (pass==0) ? "now" : "next");
		} else {
			sprintf(cmd,"LSTE %s", (pass==0) ? "now" : "next");
		}	
		char * data=execSvdrp(vdrHost,cmd);
		if (data==NULL){
			break;
		}
		i=0;
		for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
			if (i>=channels->length){
				break;
			}
			int code = strtol(p,&p,10);
			if (code==SVDRP_RCRD && p[0]=='-') {
				p++;
				if (p[0]=='C') {
					if (channels->length>1) {
						p+=2;
						k=strcspn(p," ");
						for(i=0;i<channels->length;i++) {
							if (strncmp(channels->channel[i].channelId,p,k)==0) {
								break;
							}
						}
					}
					initEE(&list->entry[i].event[pass]);
				} else if (p[0]=='e' && channels->length==1) {
					break;
				} else {
					parseLineEvent(p[0],p+2,list->entry[i].event+pass);
				}
			}
		}
		free(data);
	}
}

void printEventDesc(io_t *out, int ntabs, char * const desc, const char * const lineDelim, char * const aux){
	if (!desc || strlen(desc)==0) return;
	char *p=desc;
	int l;
	while(p[0]){
		l=strcspn(p,lineDelim);
		if (l==0) break;
		if (aux){
			u_htmlncpy(aux,p,l,HTMLCPY_ENCODE);
			io_printf(out,"%.*s%s\n",0,tabs,aux);
		} else {
			io_printf(out,"%.*s<p>%.*s</p>\n",ntabs,tabs,l,p);
		}
		p+=l;
		p+=strspn(p,lineDelim);
	}
}

void printEvent(io_t *out, int ntabs, eventEntry_t * const event){
	struct tm startt,endt;
	long int end_time = event->start+event->duration;
	io_printf(out,"%.*s<div class=\"title\">%s</div>\n",ntabs,tabs,event->title);
	if (event->shortdesc && strlen(event->shortdesc)>0){
		io_printf(out,"%.*s<div class=\"shortdesc\">%s</div>\n",ntabs,tabs,event->shortdesc);
	}
	startt=*localtime(&event->start);
	endt=*localtime(&end_time);
	io_printf(out,"%.*s<div class=\"duration\">",ntabs,tabs);
	io_printf(out,tr("fromToFmt")
		,startt.tm_hour,startt.tm_min,endt.tm_hour,endt.tm_min,event->duration/60
	);
	io_printf(out,"</div>\n");
	if (event->desc && event->desc[0]){
		io_printf(out,"%.*s<div class=\"desc\">\n",ntabs++,tabs);
		printEventDesc(out,ntabs,event->desc,"|",NULL);
		io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	}
}

void printEventInfobox(io_t *out, int ntabs, eventEntry_t * const event){
	io_printf(out,"%.*s<div class=\"infobox infoboxCssHover\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<span class=\"ui-icon ui-icon-info\">%s</span>\n",ntabs,tabs,tr("moreInfo"));
	io_printf(out,"%.*s<div class=\"infoWrapper\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<div class=\"info\">\n",ntabs++,tabs);
	printEvent(out,ntabs,event);	
	io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	io_printf(out,"%.*s</div>\n",--ntabs,tabs);
}



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
#include <dirent.h>
#include <u/libu.h>

#include "svdrp_comm.h"
#include "svdrp_parse.h"
#include "misc.h"
#include "recordings.h"
#include "channels.h"
#include "epg.h"

// Sort the list, using the field and direction 
// specified by the next two variables.
sortField_t compareRE_sortBy=SF_NONE;
sortDirection_t compareRE_sortDirection=SD_ASC;
int compareRE(const void * a, const void * b) {
	const recEntry_t *ra = (const recEntry_t*)a;
	const recEntry_t *rb = (const recEntry_t*)b;
	switch (compareRE_sortBy) {
		case SF_START: 
			return (ra->start-rb->start)*compareRE_sortDirection;
		case SF_TITLE: 
			return strcasecmp(ra->name,rb->name)*compareRE_sortDirection;
		case SF_RC_NUMBER:
			return ra->id-rb->id;
		default: 
			return 0;
	}
}

void initRE(recEntry_t * const entry){
	entry->name=NULL;
	entry->path=NULL;
}

void freeRE(recEntry_t * const entry){
	free(entry->name);
	free(entry->path);
	initRE(entry);
}

void initRL(recList_t * const list){
	list->length=0;
	list->entry=NULL;
}

void freeRL(recList_t * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeRE(&(list->entry[i]));
	}
	free(list->entry);
	list->length=0;
	list->entry=NULL;
}

void parseRec(char * line, boolean_t incpath, recEntry_t * const rec){
	char * r=line;
	int l;
	struct tm timeptr;
	
	r+=strspn(r," ");
	rec->id=strtol(r,&r,10);
	r+=strspn(r," ");
	rec->path=NULL;
	if ( incpath ) {    //Requires vdr patched to include path
		//TODO cambiar SVDRP para enviar solo rutas relativas
		if (r[15]!='/') { //16==strlen("/var/vdr/video0/")
			warn( "VDR should be patched to return path");
		} else {
			r+=16;
			l=strcspn(r," ");
			rec->path=strndup(r,l);
			r+=l;
			r+=strspn(r," ");
		}
	}
	r=strptime(r,"%d.%m.%y %H:%M",&timeptr);
	if (r==NULL){ 
		printf("Error converting recording date!\n");
		rec->name=NULL;
		free(rec->path);
		rec->path=NULL;
		return;
	}
	timeptr.tm_sec=0;
	timeptr.tm_isdst=-1;
	rec->start=mktime(&timeptr);
	rec->seen=boolean(r[0]==' ');
	r++;
	r+=strspn(r," ");
	l=strcspn(r,"/\n\r");
	rec->name=strndup(r,l);
	rec->direct=boolean(strchr(rec->name,'@')==NULL);
	rec->cut=boolean(strchr(rec->name,'%')==NULL);
	r+=l;
	return;
}

void getRecList(recList_t * const list, sortField_t sortBy, sortDirection_t sortDirection){
	initRL(list);
	char * data;
	char * p;
	int i=0;
	int code;
	
	if (write_svdrp("LSTR path\r")<=0) {
		return;
	}
	data=read_svdrp();
	for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		code=atoi(p);
		if (code==550) {
			break;
		} else if (code==250) {
			i++;
			recEntry_t *tmp=(recEntry_t *)realloc(list->entry,i*sizeof(recEntry_t));
			if (!tmp) {
				printf("Reallocation failed\n");
				exit(1);
			}
			list->length=i;
			list->entry=tmp;
			parseRec(p+4,BT_TRUE,&list->entry[i-1]);
		}
	}  
	free(data);
	if (list->length>0 && sortBy!=0) {
		//Quick sort recordings
		compareRE_sortBy=sortBy;
		compareRE_sortDirection=sortDirection;
		qsort(list->entry,list->length,sizeof(recEntry_t),compareRE);
	} 
}

boolean_t isSameRec(const recEntry_t *rec, const recInfo_t * oldInfo){
	if (rec->path!=NULL && oldInfo->path!=NULL && strcmp(rec->path,oldInfo->path)==0){
		return BT_TRUE;
	}
	recInfo_t info;
	boolean_t result=BT_FALSE;
	if (getRecInfo(rec,&info)) {
		if (   sameString(info.title,oldInfo->title) 
			&& sameString(info.subtitle,oldInfo->subtitle) 
			&& sameString(info.desc,oldInfo->desc)) {
			result=BT_TRUE;
		} else { 
			warn("Different recording as expected:");
			warn("Old title(%d): [%s]",strlen(oldInfo->title), oldInfo->title);
			warn("Current  (%d): [%s]",strlen(info.title),info.title);
			warn("Old subtitle(%d): [%s]",strlen(oldInfo->subtitle), oldInfo->subtitle);
			warn("Current (%d): [%s]",strlen(info.subtitle),info.subtitle);
			warn("Old desc (%d): [%s]",strlen(oldInfo->desc), oldInfo->desc);
			warn("Current (%d): [%s]",strlen(info.desc),info.desc);
		}
	}
	freeRI(&info);
	return result;
}

boolean_t editRec(const recEntry_t *rec, const recInfo_t * oldInfo, char ** message) {
	boolean_t result=BT_FALSE;
	char *command;
	if (isSameRec(rec,oldInfo)) {
		if (asprintf(&command,"RENR %d %s\r",rec->id, rec->name)<0){
			warn("editRec: Out of memory");
			exit(1);
		} else {
			info("editRec command [%s]",command);
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
	}
	return result;
}

boolean_t deleRec(const recEntry_t *rec, const recInfo_t * oldInfo, char ** message) {
	boolean_t result=BT_FALSE;
	recInfo_t info;
	char *command;
	if (isSameRec(rec,oldInfo)) {
		if (asprintf(&command,"DELR %d\r",rec->id)<0){
			warn("deleRec: Out of memory");
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
	}
	return result;
}

void initRI(recInfo_t * const info){
	info->title=NULL;
	info->subtitle=NULL;
	info->desc=NULL;
	info->channelId=NULL;
	info->video=VT_UNKNOWN;
	info->audio.length=0;
	info->audio.entry=NULL;
	info->path=NULL;
}

void freeRI(recInfo_t * const info){
	free(info->title);
	free(info->subtitle);
	free(info->desc);
	free(info->channelId);
	int i;
	for (i=0;i<info->audio.length;i++) free(info->audio.entry[i]);
	free(info->audio.entry);
	free(info->path);
	initRI(info);
}

boolean_t parseRecInfo(recInfo_t * const info, char * const data, boolean_t fromFile){
	initRI(info);
	char *s;
	boolean_t result=BT_TRUE;
	audioList_t *al;
	al=&info->audio;
	for (s=strtok(data,"\r\n");s!=NULL;s=strtok(0,"\r\n")) {
		if (!fromFile){
			int code=strtol(s,&s,10);
			if (code==215){
				s++;
			} else {
				continue;
			}
		}
		switch (s[0]) {
			case 'C':
				info->channelId=strdup(s+2);
				break;
			case 'E':
				parse_215E(s+2,NULL,&(info->start),&(info->duration),NULL,NULL);
				info->stop=info->start+info->duration;
				break;
			case 'T':
				info->title=strdup(s+2);
				break;
			case 'D':
				info->desc=strdup(s+2);
				break;
			case 'S': 
				info->subtitle=strdup(s+2);
				break;
			case 'P':
				info->path=strdup(s+2+16);
				break;
			case 'X': //components
				switch(s[2]) {
					case '1': //video
						if (strchr("15" , s[5])) info->video=VT_SD43;
						if (strchr("236", s[5])) info->video=VT_SD169;
						if (strchr("48",  s[5])) info->video=VT_SD;
						if (strchr("9D",  s[5])) info->video=VT_HD43;
						if (strchr("ABEF",s[5])) info->video=VT_HD169;
						if (strchr("C0",  s[5])) info->video=VT_HD;
						break;
					case '2': //audio
						al->entry=realloc(al->entry,(al->length+1)*sizeof(char*));
						al->entry[al->length]=strdup(s+7);
						al->length++;
						break;
				}
				break;
		}
	}
	return result;
}

boolean_t readRecInfo(const char * recPath, recInfo_t * const info){
	FILE *handle;
	char *fileName;
	asprintf(&fileName,"/var/vdr/video0/%s/info.vdr",recPath);
	handle=fopen(fileName,"r");
	free(fileName);
	if (handle) {
		enum {BUFSZ=513};
		char s[BUFSZ];
		char *data=NULL;
		int l=0;
		while (!feof(handle)) {
			fgets(s,BUFSZ,handle);
			int n=strlen(s);
			data=realloc(data,l+n+1);
			if (!data) {
				warn("readRecInfo:(Re)allocation failed");
				exit(1);
			}
			data[l]=0;
			strcat(data,s);
			l+=n;
		}
		fclose(handle);
		if (data!=NULL){
			boolean_t result=parseRecInfo(info,data,BT_TRUE);
			free(data);
			return result;
		}
	}
	return BT_FALSE;
}

boolean_t getRecInfo(const recEntry_t *rec, recInfo_t * const info){
	char * command=NULL;
	if (rec==NULL) return BT_FALSE;
	if (rec->path!=NULL) {
		//TODO solo si es local
		return readRecInfo(rec->path,info);
	} else if (rec->id>0) {
		if (asprintf(&command,"LSTR %d\r",rec->id)<0){
			warn("getRecInfo:Out of memory");
			exit(1);
		} else {
			int error=(write_svdrp(command)<=0);
			free(command);
			if (error) {
				return BT_FALSE;
			}
			char * data=read_svdrp();
			if (data!=NULL) {
				boolean_t result=parseRecInfo(info,data,BT_FALSE);
				free(data);
				return result;
			}
		}
	}
	return BT_FALSE;
}

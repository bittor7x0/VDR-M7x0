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
#include <klone/utils.h>

#include "channels.h"
#include "conf.h"
#include "epg.h"
#include "i18n.h"
#include "misc.h"
#include "recordings.h"
#include "svdrp_comm.h"

void initRec(rec_t * const rec){
	rec->my=0;
	rec->hostId=0;
	rec->id=0;
	rec->name=NULL;
	rec->path=NULL;
	rec->flags=0;
	initEE(&rec->event);
	rec->video=VT_UNKNOWN;
	rec->audio.length=0;
	rec->audio.entry=NULL;
}

void initRecFromArgs(rec_t * const rec, vars_t *args){
	initRec(rec);
	rec->hostId=(vars_countn(args, "hostId")>0)?vars_get_value_i(args,"hostId"):0;
	rec->id=(vars_countn(args,"id")>0) ? vars_get_value_i(args,"id") : 0;
	rec->name=(vars_countn(args, "name")>0)
		? condstrdup(isFlagSet(RF_NAME,rec->my),vars_get_value(args,"name"))
		: NULL;
	rec->path=(vars_countn(args, "path")>0)
		? condstrdup(isFlagSet(RF_PATH,rec->my),vars_get_value(args,"path"))
		: NULL;
	initEventFromArgs(&rec->event,args);
	rec->video=VT_UNKNOWN;
	rec->audio.length=0;
	rec->audio.entry=NULL;
}

void freeRec(rec_t * const entry){
	if (isFlagSet(RF_NAME,entry->my)) free(entry->name);
	if (isFlagSet(RF_PATH,entry->my)) free(entry->path);
	freeEE(&entry->event);
	int i;
	for (i=0;i<entry->audio.length;i++) free(entry->audio.entry[i]);
	free(entry->audio.entry);
	initRec(entry);
}

void initRecList(recList_t * const list){
	list->length=0;
	list->entry=NULL;
}

void freeRecList(recList_t * const list){
	int i;
	for (i=0;i<list->length;i++){
		freeRec(&(list->entry[i]));
	}
	free(list->entry);
	list->length=0;
	list->entry=NULL;
}

void parseRec(char * line, boolean_t withPath, rec_t * const rec){
	char * r=line;
	int l;
	struct tm timeptr;
	
	rec->my = rec->event.my = ~0;  //all pointers are owned by rec (must be freed)
	r+=strspn(r," ");
	rec->id=strtol(r,&r,10);
	r+=strspn(r," ");
	rec->path=NULL;
	if (withPath) {
		if (strchr(r,'/')==NULL) { 
			warn("VDR should be patched to return path");
		} else {
			if (r[0]=='/') {
				warn("Recordig path is not relative [%s]", r);
			}
			l=strcspn(r," ");
			int l2=l;
			while (r[l2-1]=='/') l2--;
			rec->path=(l2>0) ? strndup(r,l2) : NULL;
			r+=l;
			r+=strspn(r," ");
		}
	}
	r=strptime(r,"%d.%m.%y %H:%M",&timeptr);
	if (r==NULL){ 
		warn("Error converting recording date!");
		freeRec(rec);
		return;
	}
	timeptr.tm_sec=0;
	timeptr.tm_isdst=-1;
	rec->event.start=mktime(&timeptr);
	if (r[0]==' ') rec->flags|=RE_NEW;
	r++;
	r+=strspn(r," ");
	l=strcspn(r,"/\n\r");
	rec->name=strndup(r,l);
	if (strchr(rec->name,'@')) rec->flags|=RE_DIRECT;
	if (strchr(rec->name,'%')) rec->flags|=RE_EDITED;
	r+=l;
	return;
}

void getRecListHost(hostConf_t *host, recList_t * const recs){
	char * data;
	char * p;
	data=execSvdrp(host,"LSTR path");
	for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		int code=atoi(p);
		if (code==SVDRP_CMD_OK) {
			crit_goto_if((recs->entry=(rec_t *)realloc(recs->entry,(++recs->length)*sizeof(rec_t)))==NULL,outOfMemory);
			rec_t *rec=recs->entry+recs->length-1;
			initRec(rec);
			rec->hostId=host->id;
			parseRec(p+4,BT_TRUE,rec);
		}
	}  
	free(data);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

int compareRecs(const rec_t *ra, const rec_t *rb, sortField_t sortBy, sortDirection_t sortDirection) {
	int result;
	switch (sortBy) {
		case SF_TITLE:
			if ((result=strcasecmp(ra->name,rb->name))!=0) return sortDirection*result;
		case SF_START: 
			if ((result=ra->event.start-rb->event.start)!=0) return sortDirection*result;
		case SF_RC_NUMBER:
			return ra->id-rb->id;
		default: 
			return 0;
	}
}

void sortRecList(recList_t * const recs, sortField_t sortBy, sortDirection_t sortDirection){
	int compare(const void * a, const void * b) {
		return compareRecs((const rec_t*)a,(const rec_t*)b,sortBy,sortDirection);
	}
	if (recs->length>0 && sortBy!=0) {
		qsort(recs->entry,recs->length,sizeof(rec_t),compare);
	} 
}

void getRecList(recList_t * const recs, sortField_t sortBy, sortDirection_t sortDirection){
	initRecList(recs);
	int h;
	hostConf_t *host;
	for (h=0,host=webifConf.hosts;h<webifConf.hostsLength;h++,host++){
		if (host->isVdr){
			getRecListHost(host,recs);
		}
	}
	sortRecList(recs,sortBy,sortDirection);
}


boolean_t editRec(const rec_t *rec, char ** message) {
	if (!rec->path) {
		*message=strdup(tr("recErrorNoPath"));
		crit_goto_if(*message==NULL,outOfMemory);
		return BT_FALSE;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host){
		*message=strdup(tr("recErrorWrongHost"));
		crit_goto_if(*message==NULL,outOfMemory);
		return BT_FALSE;
	}
	boolean_t result=BT_FALSE;
	char *command;
	crit_goto_if(asprintf(&command,"RENR %s %s",rec->path,rec->name)<0,outOfMemory);
	info("editRec command [%s]",command);
	char *data=execSvdrp(host,command);
	free(command);
	if (data!=NULL){
		char * p=data;
		int code=strtol(p,&p,10);
		result=boolean(code==SVDRP_CMD_OK);
		if (message && *p && *(++p)){
			*message=strdup(p);
		}
		free(data);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t deleRec(const rec_t *rec, char ** message) {
	if (!rec->path) {
		*message=strdup(tr("recErrorNoPath"));
		crit_goto_if(*message==NULL,outOfMemory);
		return BT_FALSE;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host){
		*message=strdup(tr("recErrorWrongHost"));
		crit_goto_if(*message==NULL,outOfMemory);
		return BT_FALSE;
	}
	boolean_t result=BT_FALSE;
	char *command;
	crit_goto_if(asprintf(&command,"DELR %s",rec->path)<0,outOfMemory);
	char * data=execSvdrp(host,command);
	free(command);
	if (data!=NULL){
		char * p=data;
		int code=strtol(p,&p,10);
		result=boolean(code==SVDRP_CMD_OK);
		if (message && *p && *(++p)){
			*message=strdup(p);
		}
		free(data);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t parseRecInfo(rec_t * const rec, char * const data, boolean_t fromFile){
	char *s;
	boolean_t result=BT_TRUE;
	audioList_t *al;
	al=&rec->audio;
	for (s=strtok(data,"\r\n");s!=NULL;s=strtok(0,"\r\n")) {
		if (!fromFile){
			int code=strtol(s,&s,10);
			if (code==SVDRP_RCRD){
				if (s[0]==' ') 
					return; //End of recording info
				s++;
			} else {
				continue;
			}
		}
		switch (s[0]) {
			case 'P':
				if (rec->path==NULL){
					crit_goto_if((rec->path=strdup(s+2))==NULL,outOfMemory);
					rec->my|=RF_PATH;
				}
				break;
			case 'X': //components
				switch(s[2]) {
					case '1': //video
						if (strchr("15" , s[5])) rec->video=VT_SD43;
						if (strchr("236", s[5])) rec->video=VT_SD169;
						if (strchr("48",  s[5])) rec->video=VT_SD;
						if (strchr("9D",  s[5])) rec->video=VT_HD43;
						if (strchr("ABEF",s[5])) rec->video=VT_HD169;
						if (strchr("C0",  s[5])) rec->video=VT_HD;
						break;
					case '2': //audio
						al->entry=realloc(al->entry,(al->length+1)*sizeof(char*));
						al->entry[al->length]=strdup(s+7);
						al->length++;
						break;
				}
				break;
			default:
				parseLineEvent(s[0],s+2,&rec->event);
		}
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t readRecInfo(rec_t * const rec){
	if (rec->hostId<0 || rec->hostId>=webifConf.hostsLength){
		return BT_FALSE;
	}
	hostConf_t *host=getHost(rec->hostId);
	FILE *handle;
	char *fileName;
	if (rec->path[0]=='/') {
		crit_goto_if(asprintf(&fileName,"%s/info.vdr",rec->path)<0,outOfMemory);
	} else {
		crit_goto_if(asprintf(&fileName,"%s/%s/info.vdr",host->video0,rec->path)<0,outOfMemory);
	}
	handle=fopen(fileName,"r");
	free(fileName);
	if (handle) {
		enum {BUFSZ=512};
		char s[BUFSZ];
		char *data=NULL;
		int l=0;
		while (!feof(handle)) {
			fgets(s,BUFSZ,handle);
			int n=strlen(s);
			data=realloc(data,l+n+1);
			crit_goto_if(!data,outOfMemory);
			data[l]=0;
			strcat(data,s);
			l+=n;
		}
		fclose(handle);
		if (data!=NULL){
			boolean_t result=parseRecInfo(rec,data,BT_TRUE);
			free(data);
			return result;
		}
	}
	return BT_FALSE;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t getRecInfo(rec_t *rec){
	if (rec==NULL) return BT_FALSE;
	if (rec->path==NULL) {
		warn("No path");
		return BT_FALSE;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (host){
		if (host->video0[0]) { //recordins are locally accessible
			clearFlag(RE_REMOTE_INFO_PENDING,rec->flags);
			return readRecInfo(rec);
		} else if (isFlagSet(RE_REMOTE_INFO_PENDING,rec->flags)){
			//Info is estimated from path, instead of reading it from remote host
			//TODO
			return BT_TRUE;
		} else {
			char * command=NULL;
			crit_goto_if(asprintf(&command,"LSTR %s",rec->path)<0,outOfMemory);
			char * data=execSvdrp(host,command);
			free(command);
			if (data!=NULL) {
				boolean_t result=parseRecInfo(rec,data,BT_FALSE);
				free(data);
				return result;
			}
		}
	} else {
		warn("No valid host defined for rec %s", rec->path);
	}
	return BT_FALSE;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t printRecPlaylist(request_t *request, response_t *response, io_t *out, rec_t *rec, char *const aux){
	if (isValidRecPath(rec->path)){
		const char *hostAddr=getHostHttpAddr(getHost(rec->hostId),request);
		response_set_content_type(response,M3U_MIME_TYPE);
		u_urlncpy(aux,rec->path,strlen(rec->path),URLCPY_ENCODE);
		io_printf(out,"http://%s/streamrec.kl1?path=%s",hostAddr,aux);
		return BT_TRUE;
	} else {
		response_set_status(response,HTTP_STATUS_NO_CONTENT);
		warn("Invalid rec path [%s]",rec->path);
		return BT_FALSE;
	}
}

void printRecControls(io_t *out,int ntabs,const rec_t *rec,char *const aux,const char *Play,const char *Edit,const char *Delete){
	if (rec->path==NULL) return;
	u_htmlncpy(aux,rec->path,strlen(rec->path),HTMLCPY_ENCODE);
	io_printf(out,"%.*s<input type=\"hidden\" name=\"path\" value=\"%s\" />\n",ntabs,tabs,aux);
	if (rec->id>0) {
		io_printf(out,"%.*s<input type=\"hidden\" name=\"id\" value=\"%d\" />\n",ntabs,tabs,rec->id);
	}
	io_printf(out,"%.*s<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n",ntabs,tabs,rec->hostId);
	if (Play) {
		io_printf(out,"%.*s"
			"<button type=\"submit\" class=\"play control buttons-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<span class=\"ui-icon ui-icon-play\">%s</span>"
			"</button>\n",ntabs,tabs,PA_PLAY,Play,Play);
	}
	if (Edit) {
		io_printf(out,"%.*s"
			"<button type=\"submit\" class=\"edit control buttons-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<span class=\"ui-icon ui-icon-edit\">%s</span>"
			"</button>\n",ntabs,tabs,PA_EDIT,Edit,Edit);
	}
	if (Delete && !webifConf.deletionDisabled) {
		io_printf(out,"%.*s"
			"<button type=\"submit\" class=\"delete control buttons-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<span class=\"ui-icon ui-icon-trash\">%s</span>"
			"</button>\n",ntabs,tabs,PA_DELETE,Delete,Delete);
	}
}

void printRecEditForm(io_t *out, int ntabs, rec_t *rec, char *const aux){
	//TODO delegar en printRecControls
	int l;
	io_printf(out,"%.*s<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n",ntabs,tabs,rec->hostId);
	if (rec->id>0){
		io_printf(out,"%.*s<input type=\"hidden\" name=\"id\" value=\"%d\"/>\n",ntabs,tabs,rec->id);
	}
	if (rec->path!=NULL) {
		const char *Play=tr("play");
		u_htmlncpy(aux,rec->path,strlen(rec->path),HTMLCPY_ENCODE);
		io_printf(out,"%.*s<input type=\"hidden\" name=\"path\" value=\"%s\"/>\n",ntabs,tabs,aux);
	}
	io_printf(out,"%.*s<table id=\"recEdit\" class=\"list\" summary=\"\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<caption>%s</caption>\n",ntabs,tabs,tr("rec.edit"));
	if (rec->name!=NULL || rec->path!=NULL){
		if (rec->name==NULL){
			l=strrchr(rec->path,'/')-rec->path;
			vdrDecode(aux,rec->path,l);
			crit_goto_if((rec->name=strdup(aux))==NULL,outOfMemory);
			rec->my|=RF_NAME;
		};
		l=strlen(rec->name);
		u_htmlncpy(aux,rec->name,l,HTMLCPY_ENCODE);
		io_printf(out,"%.*s<tr>\n",ntabs++,tabs);
		io_printf(out,"%.*s<th>%s</th>\n",ntabs,tabs,tr("name"));
		io_printf(out,"%.*s<td>\n",ntabs++,tabs);
		io_printf(out,"%.*s<input type=\"text\" name=\"name\" value=\"%s\" size=\"%d\"/>\n",ntabs,tabs,aux,5+(l>55)?55:l);
		io_printf(out,"%.*s<input type=\"hidden\" name=\"oldName\" value=\"%s\"/>\n",ntabs,tabs,aux);
		io_printf(out,"%.*s</td>\n",--ntabs,tabs);
		io_printf(out,"%.*s</tr>\n",--ntabs,tabs);
	}
	if (rec->event.title!=NULL){
		l=strlen(rec->event.title);
		u_htmlncpy(aux,rec->event.title,l,HTMLCPY_ENCODE);
		io_printf(out,"%.*s<tr>\n",ntabs++,tabs);
		io_printf(out,"%.*s<th>%s</th>\n",ntabs,tabs,tr("title"));
		io_printf(out,"%.*s<td>\n",ntabs++,tabs);
		//io_printf(out,"%.*s<textarea name=\"title\" cols=\"%d\" rows=\"%d\">",ntabs,tabs,(l>70)?70:l,(l/70)+2);
		io_printf(out,"%s",aux);
		//io_printf(out,"</textarea>\n");
		io_printf(out,"%.*s</td>\n",--ntabs,tabs);
		io_printf(out,"%.*s</tr>\n",--ntabs,tabs);
	}
	if (rec->event.shortdesc!=NULL){
		l=strlen(rec->event.shortdesc);
		u_htmlncpy(aux,rec->event.shortdesc,strlen(rec->event.shortdesc),HTMLCPY_ENCODE);
		io_printf(out,"%.*s<tr>\n",ntabs++,tabs);
		io_printf(out,"%.*s<th>%s</th>\n",ntabs,tabs,tr("shortdesc"));
		io_printf(out,"%.*s<td>\n",ntabs++,tabs);
		//io_printf(out,"%.*s<textarea name=\"shortdesc\" cols=\"%d\" rows=\"%d\">",ntabs,tabs,(l>70)?70:l,(l/70)+2);
		io_printf(out,"%s",aux);
		//io_printf(out,"</textarea>\n");
		io_printf(out,"%.*s</td>\n",--ntabs,tabs);
		io_printf(out,"%.*s</tr>\n",--ntabs,tabs);
	}
	if (rec->event.desc!=NULL){
		l=strlen(rec->event.desc);
		io_printf(out,"%.*s<tr>\n",ntabs++,tabs);
		io_printf(out,"%.*s<th>%s</th>\n",ntabs,tabs,tr("desc"));
		io_printf(out,"%.*s<td>\n",ntabs++,tabs);
		//io_printf(out,"%.*s<textarea name=\"desc\" cols=\"%d\" rows=\"%d\">\n",ntabs++,tabs,(l>70)?70:l,(l/70)+2);
		printEventDesc(out,ntabs,rec->event.desc,"|",aux);
		//io_printf(out,"%.*s</textarea>\n",--ntabs,tabs);
		io_printf(out,"%.*s</td>\n",--ntabs,tabs);
		io_printf(out,"%.*s</tr>\n",--ntabs,tabs);
	}

	io_printf(out,"%.*s<tr class=\"buttons\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<td colspan=\"2\">\n",ntabs++,tabs);
	if (rec->name!=NULL) {
		io_printf(out,"%.*s"
			"<button id=\"confirm\" type=\"submit\" name=\"a\" value=\"%d\" class=\"confirm ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-check\">&nbsp;</span>%s</div>"
			"</button>\n",ntabs,tabs,PA_CONFIRM,tr("accept"));
	}
	if (rec->path!=NULL) {
		io_printf(out,"%.*s"
			"<button id=\"play\" type=\"submit\" name=\"a\" value=\"%d\" class=\"play ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-play\">&nbsp;</span>%s</div>"
			"</button>\n",ntabs,tabs,PA_PLAY,tr("play"));
	}
	if (!webifConf.deletionDisabled && (rec->id>0 || rec->path) ) {
		io_printf(out,"%.*s"
			"<button id=\"delete\" type=\"submit\" name=\"a\" value=\"%d\" class=\"delete ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-trash\">&nbsp;</span>%s</div>"
			"</button>\n",ntabs,tabs,PA_DELETE,tr("rec.delete"));
	}
	io_printf(out,"%.*s</td>\n",--ntabs,tabs);
	io_printf(out,"%.*s</tr>\n",--ntabs,tabs);

	io_printf(out,"%.*s</table>\n",--ntabs,tabs);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}


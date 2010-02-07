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

#define isValidRecPath(path) (path && strlen(path)>4 && strcmp(path+strlen(path)-4,".rec")==0)

void initRec(rec_t * const rec){
	rec->my=0;
	rec->hostId=0;
	rec->id=0;
	rec->name=NULL;
	rec->path=NULL;
	rec->flags=0;
	initEpgEvent(&rec->event);
	rec->video=VT_UNKNOWN;
	rec->audio.length=0;
	rec->audio.entry=NULL;
}

bool initRecFromArgs(wcontext_t *wctx, rec_t * const rec, vars_t *args){
	bool isACopy;
	initRec(rec);
	rec->hostId=(vars_countn(args, "hostId")>0)?vars_get_value_i(args,"hostId"):0;
	rec->id=(vars_countn(args,"id")>0) ? vars_get_value_i(args,"id") : 0;
	if (vars_countn(args, "name")>0) {
		rec->name=wctxGetRequestParam(wctx,args,"name",&isACopy);
		if (isACopy) setFlag(RF_NAME,rec->my);
	}
	if (vars_countn(args, "path")>0){
		rec->path=wctxGetRequestParam(wctx,args,"path",&isACopy);
		if (isACopy) setFlag(RF_PATH,rec->my);
		if (!isValidRecPath(rec->path)) {
			printMessage(wctx,"alert",tr("rec.delete.err"),tr("recErrNoValidPath"), false);
			return false;
		}
	}
	dbg("rec->path [%s]",rec->path);
	initEpgEventFromArgs(wctx,&rec->event,args);
	rec->video=VT_UNKNOWN;
	rec->audio.length=0;
	rec->audio.entry=NULL;
	return true;
}

void freeRec(rec_t * const entry){
	if (isFlagSet(RF_NAME,entry->my)) free(entry->name);
	if (isFlagSet(RF_PATH,entry->my)) free(entry->path);
	freeEpgEvent(&entry->event);
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

void parseRec(char * line, bool withPath, rec_t * const rec){
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
			parseRec(p+4,true,rec);
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


bool editRec(wcontext_t *wctx, const rec_t *rec, const char *oldName) {
	bool result=false;
	if (!rec->path) {
		if (wctx) printMessage(wctx,"alert",tr("rec.update.err"),tr("recErrorNoPath"),false);
		return false;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host){
		if (wctx) printMessage(wctx,"alert",tr("rec.update.err"),tr("recErrorWrongHost"),false);
		return false;
	}
	if (oldName==NULL){
		if (wctx) printMessage(wctx,"alert",tr("rec.update.err"),"Es necesario el nombre antiguo",false); //TODO i18n
		return false;
	}
	if (sameString(oldName,rec->name)){
		if (wctx) printMessage(wctx,"alert",tr("rec.update.err"),"No se ha cambiado el nombre",false); //TODO i18n
		return false;
	}

	char *command;
	crit_goto_if(asprintf(&command,"RENR %s %s",rec->path,rec->name)<0,outOfMemory);
	info("editRec command [%s]",command);
	char *data=execSvdrp(host,command);
	free(command);
	if (data!=NULL){
		char * p=data;
		int code=strtol(p,&p,10);
		result=boolean(code==SVDRP_CMD_OK);
		if (wctx && *p && *(++p)){
			printMessage(wctx,(result)?"message":"alert",tr((result)?"rec.update.ok":"rec.update.err"),p,true);
		}
		free(data);
	} else {
		if (wctx) printMessage(wctx,"alert",tr("rec.update.err"),tr("warnSvdrpConnection"),false);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool deleteRec(wcontext_t *wctx, const rec_t *rec) {
	if (!rec->path) {
		if (wctx) printMessage(wctx,"alert",tr("rec.delete.err"),tr("recErrorNoPath"),false);
		return false;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host){
		if (wctx) printMessage(wctx,"alert",tr("rec.delete.err"),tr("recErrorWrongHost"),false);
		return false;
	}
	bool result=false;
	char *command;
	crit_goto_if(asprintf(&command,"DELR %s",rec->path)<0,outOfMemory);
	char * data=execSvdrp(host,command);
	free(command);
	if (data!=NULL){
		char * p=data;
		int code=strtol(p,&p,10);
		result=boolean(code==SVDRP_CMD_OK);
		if (wctx && *p && *(++p)){
			printMessage(wctx,(result)?"message":"alert",tr((result)?"rec.delete.ok":"rec.delete.err"),p,true);
		}
		free(data);
	} else {
		if (wctx) printMessage(wctx,"alert",tr("rec.delete.err"),tr("warnSvdrpConnection"),false);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool parseRecInfo(rec_t * const rec, char * const data, bool fromFile){
	char *s;
	bool result=true;
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
			case 'X': //components  TODO asignar a event y mover a parseEventLine
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
				parseEventLine(s[0],s+2,&rec->event);
		}
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool readRecInfo(rec_t * const rec){
	if (rec->hostId<0 || rec->hostId>=webifConf.hostsLength){
		return false;
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
			bool result=parseRecInfo(rec,data,true);
			free(data);
			return result;
		}
	}
	return false;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool getRecInfo(rec_t *rec){
	if (rec==NULL) return false;
	if (rec->path==NULL) {
		warn("No path");
		return false;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (host){
		if (host->video0[0]) { //recordins are locally accessible
			clearFlag(RE_REMOTE_INFO_PENDING,rec->flags);
			return readRecInfo(rec);
		} else if (isFlagSet(RE_REMOTE_INFO_PENDING,rec->flags)){
			//Info is estimated from path, instead of reading it from remote host
			//TODO
			return true;
		} else {
			char * command=NULL;
			crit_goto_if(asprintf(&command,"LSTR %s",rec->path)<0,outOfMemory);
			char * data=execSvdrp(host,command);
			free(command);
			if (data!=NULL) {
				bool result=parseRecInfo(rec,data,false);
				free(data);
				return result;
			}
		}
	} else {
		warn("No valid host defined for rec %s", rec->path);
	}
	return false;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

//TODO usar printEvent
void printRecInfo(wcontext_t *wctx, const rec_t * const rec){
	int minutes = rec->event.duration/60;
	wctx_printfn(wctx,"<div class=\"recInfo\">\n",0,1);
	wctx_printfn(wctx,"<div class=\"recDate\">\n",0,1);
	wctx_printf0(wctx,
		"<span class=\"recStart field\">"
			"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
		"</span>\n",tr("date"),formatDate(localtime(&rec->event.start),1));
	wctx_printf0(wctx,
		"<span class=\"recDuration field\">"
			"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%d&#39;</span>"
		"</span>\n",tr("runtime"),minutes);
	wctx_printfn(wctx,"</div>\n",-1,0);
	if (webifConf.displayHostId && webifConf.hostsLength>1){
		hostConf_t *host=getHost(rec->hostId);
		if (host){
			wctx_printfn(wctx,"<div class=\"recHost\">\n",0,1);
			wctx_printf0(wctx,
				"<span class=\"field\">"
					"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
				"</span>\n","Host",(host->name[0])?host->name:(host->ip[0])?host->ip:"?");
			wctx_printfn(wctx,"</div>\n",-1,0);
		}
	}
	if (rec->event.desc && rec->event.desc[0]) {
		wctx_printfn(wctx,"<div class=\"desc\">\n",0,1);
		printEventDesc(wctx,rec->event.desc,false);
		wctx_printfn(wctx,"</div>\n",-1,0);
	}
	wctx_printfn(wctx,"</div>\n",-1,0);
}



void printRecPlayLink(wcontext_t *wctx, const rec_t *rec, bool direct){
	const char *hostAddr=getHostHttpAddr(getHost(rec->hostId),wctx);
	wctx_printf(wctx,"http://%s/%s.kl1?path=%s",hostAddr,(direct)?"streamrec":"playlistrec",CTX_URL_ENCODE(rec->path,-1,"-_.~"));
}

bool printRecPlaylist(wcontext_t *wctx, rec_t *rec){
	if (isValidRecPath(rec->path)){
		response_set_content_type(wctx->response,M3U_MIME_TYPE);
		sprintf(wctx->buffer,"inline; filename=%s.m3u",(rec->name!=NULL)?rec->name:tr("recording.filename"));
		response_set_field(wctx->response,"Content-Disposition",wctx->buffer);
		printRecPlayLink(wctx,rec,true);
		return true;
	} else {
		response_set_status(wctx->response,HTTP_STATUS_NO_CONTENT);
		warn("Invalid rec path [%s]",rec->path);
		return false;
	}
}

void printRecControls(wcontext_t *wctx,const rec_t *rec,const char *Play,const char *Edit,const char *Delete){
	if (rec->path==NULL) return;
	wctx_printf0(wctx,"<input type=\"hidden\" name=\"path\" value=\"%s\" />\n",rec->path);
	if (rec->id>0) {
		wctx_printf0(wctx,"<input type=\"hidden\" name=\"id\" value=\"%d\" />\n",rec->id);
	}
	wctx_printf0(wctx,"<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n",rec->hostId);
	wctx_printfn(wctx,"<ul class=\"controls\"><!--\n",0,1);
	if (Play) {
		wctx_printfn(wctx,"--><li class=\"control\">\n",0,1);
#ifdef PRINT_REC_CONTROLS_AS_FORM		
		wctx_printf0(wctx,
			"<button type=\"submit\" class=\"play control button-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<div><span class=\"ui-icon ui-icon-play\">%s</span></div>"
			"</button>\n",PA_PLAY,Play,Play)
#else
		wctx_printf0(wctx,"<a href=\"");
		printRecPlayLink(wctx,rec,false);
		wctx_printf(wctx,"\" class=\"play control button-i ui-state-default\" title=\"%s\">"
				"<span class=\"ui-icon ui-icon-play\">%s</span>"
			"</a>\n",Play,Play);
#endif
		wctx_printfn(wctx,"</li><!--\n",-1,0);
	}
	if (Edit) {
		wctx_printfn(wctx,"--><li class=\"control\">\n",0,1);
		wctx_printf0(wctx,
			"<button type=\"submit\" class=\"edit control button-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<div><span class=\"ui-icon ui-icon-edit\">%s</span></div>"
			"</button>\n",PA_EDIT,Edit,Edit);
		wctx_printfn(wctx,"</li><!--\n",-1,0);
	}
	if (Delete && !webifConf.deletionDisabled) {
		wctx_printfn(wctx,"--><li class=\"control\">\n",0,1);
		wctx_printf0(wctx,
			"<button type=\"submit\" class=\"delete control button-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<div><span class=\"ui-icon ui-icon-trash\">%s</span></div>"
			"</button>\n",PA_DELETE,Delete,Delete);
		wctx_printfn(wctx,"</li><!--\n",-1,0);
	}
	wctx_printfn(wctx,"--></ul>\n",-1,0);
}

void printRecEditForm(wcontext_t *wctx, rec_t *rec){
	int l;
	wctx_printf0(wctx,"<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n",rec->hostId);
	if (rec->id>0){
		wctx_printf0(wctx,"<input type=\"hidden\" name=\"id\" value=\"%d\"/>\n",rec->id);
	}
	if (rec->path!=NULL) {
		dbg("rec->path=[%s]",rec->path);
		wctx_printf0(wctx,"<input type=\"hidden\" name=\"path\" value=\"%s\"/>\n",rec->path);
	} else {
		warn("rec->path==NULL");
	}
	wctx_printfn(wctx,"<table id=\"recEdit\" class=\"list formContent\" summary=\"\">\n",0,1);
	wctx_printf0(wctx,"<caption class=\"formTitle\">%s</caption>\n",tr("rec.edit"));
	if (rec->name!=NULL || rec->path!=NULL){
		if (rec->name==NULL){
			l=strrchr(rec->path,'/')-rec->path;
			CTX_CHK_BUFFER(l);
			vdrDecode(wctx->buffer,rec->path,l);
			crit_goto_if((rec->name=strdup(wctx->buffer))==NULL,outOfMemory);
			setFlag(RF_NAME,rec->my);
		};
		wctx_printfn(wctx,"<tr>\n",0,1);
		wctx_printf0(wctx,"<th>%s</th>\n",tr("name"));
		wctx_printfn(wctx,"<td>\n",0,1);
		l=strlen(rec->name);
		CTX_HTML_ENCODE(rec->name,l);
		wctx_printf0(wctx,"<input type=\"text\" name=\"name\" value=\"%s\" size=\"%d\"/>\n",wctx->buffer,5+(l>55)?55:l);
		wctx_printf0(wctx,"<input type=\"hidden\" name=\"oldName\" value=\"%s\"/>\n",wctx->buffer);
		wctx_printfn(wctx,"</td>\n",-1,0);
		wctx_printfn(wctx,"</tr>\n",-1,0);
	}
	if (rec->event.title!=NULL){
		wctx_printfn(wctx,"<tr>\n",0,1);
		wctx_printf0(wctx,"<th>%s</th>\n",tr("title"));
		wctx_printfn(wctx,"<td>\n",0,1);
		//wctx_printf0(wctx,"<textarea name=\"title\" cols=\"%d\" rows=\"%d\">",(l>70)?70:l,(l/70)+2);
		wctx_printf(wctx,"%s",CTX_HTML_ENCODE(rec->event.title,-1));
		//wctx_printf(wctx,"</textarea>\n");
		wctx_printfn(wctx,"</td>\n",-1,0);
		wctx_printfn(wctx,"</tr>\n",-1,0);
	}
	if (rec->event.shortdesc!=NULL){
		wctx_printfn(wctx,"<tr>\n",0,1);
		wctx_printf0(wctx,"<th>%s</th>\n",tr("shortdesc"));
		wctx_printfn(wctx,"<td>\n",0,1);
		//wctx_printf0(wctx,"<textarea name=\"shortdesc\" cols=\"%d\" rows=\"%d\">",(l>70)?70:l,(l/70)+2);
		wctx_printf(wctx,"%s",CTX_HTML_ENCODE(rec->event.shortdesc,-1));
		//wctx_printf(wctx,"</textarea>\n");
		wctx_printfn(wctx,"</td>\n",-1,0);
		wctx_printfn(wctx,"</tr>\n",-1,0);
	}
	if (rec->event.desc!=NULL){
		wctx_printfn(wctx,"<tr>\n",0,1);
		wctx_printf0(wctx,"<th>%s</th>\n",tr("desc"));
		wctx_printfn(wctx,"<td>\n",0,1);
		//l=strlen(rec->event.desc);
		//wctx_printfn(wctx,"<textarea name=\"desc\" cols=\"%d\" rows=\"%d\">\n",0,1,(l>70)?70:l,(l/70)+2);
		printEventDesc(wctx,rec->event.desc,false);
		//wctx_printfn(wctx,"</textarea>\n",-1,0);
		wctx_printfn(wctx,"</td>\n",-1,0);
		wctx_printfn(wctx,"</tr>\n",-1,0);
	}

	wctx_printfn(wctx,"<tr class=\"buttons\">\n",0,1);
	wctx_printfn(wctx,"<td class=\"buttons ajaxButtonsHolder\" colspan=\"2\">\n",0,1);
	if (rec->name!=NULL) {
		wctx_printf0(wctx,
			"<button type=\"submit\" name=\"a\" value=\"%d\" class=\"confirm ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-check\">&nbsp;</span>%s</div>"
			"</button>\n",PA_CONFIRM,tr("accept"));
	}
	if (rec->path!=NULL) {
		const char *Play=tr("play");
		wctx_printf0(wctx,"<a href=\"");
		printRecPlayLink(wctx,rec,false);
		wctx_printf(wctx,"\" class=\"play ui-state-default button-i-t\">"
				"<span class=\"ui-icon ui-icon-play\">&nbsp;</span>%s"
			"</a>\n",Play);
	}
	if (!webifConf.deletionDisabled && (rec->id>0 || rec->path) ) {
		wctx_printf0(wctx,
			"<button type=\"submit\" name=\"a\" value=\"%d\" class=\"delete ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-trash\">&nbsp;</span>%s</div>"
			"</button>\n",PA_DELETE,tr("rec.delete"));
	}
	wctx_printfn(wctx,"</td>\n",-1,0);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	wctx_printfn(wctx,"</table>\n",-1,0);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}


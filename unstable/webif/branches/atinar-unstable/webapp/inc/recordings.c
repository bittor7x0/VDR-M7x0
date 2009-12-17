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

boolean_t initRecFromArgs(context_t *ctx, rec_t * const rec, vars_t *args){
	initRec(rec);
	rec->hostId=(vars_countn(args, "hostId")>0)?vars_get_value_i(args,"hostId"):0;
	rec->id=(vars_countn(args,"id")>0) ? vars_get_value_i(args,"id") : 0;
	if (vars_countn(args, "name")>0) {
		if (ctxGetRequestParamAsCpy(ctx,&rec->name,args,"name")) setFlag(RF_NAME,rec->my);
	}
	if (vars_countn(args, "path")>0){
		if (ctxGetRequestParamAsCpy(ctx,&rec->path,args,"path")) setFlag(RF_PATH,rec->my);
		if (!isValidRecPath(rec->path)) {
			printMessage(ctx,"alert",tr("rec.delete.err"),tr("recErrNoValidPath"), BT_FALSE);
			return BT_FALSE;
		}
	}
	dbg("rec->path [%s]",rec->path);
	initEpgEventFromArgs(ctx,&rec->event,args);
	rec->video=VT_UNKNOWN;
	rec->audio.length=0;
	rec->audio.entry=NULL;
	return BT_TRUE;
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


boolean_t editRec(context_t *ctx, const rec_t *rec, const char *oldName) {
	boolean_t result=BT_FALSE;
	if (!rec->path) {
		if (ctx) printMessage(ctx,"alert",tr("rec.update.err"),tr("recErrorNoPath"),BT_TRUE);
		return BT_FALSE;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host){
		if (ctx) printMessage(ctx,"alert",tr("rec.update.err"),tr("recErrorWrongHost"),BT_TRUE);
		return BT_FALSE;
	}
	if (oldName==NULL){
		if (ctx) printMessage(ctx,"alert",tr("rec.update.err"),"Es necesario el nombre antiguo",BT_TRUE); //TODO i18n
		return BT_FALSE;
	}
	if (sameString(oldName,rec->name)){
		if (ctx) printMessage(ctx,"alert",tr("rec.update.err"),"No se ha cambiado el nombre",BT_TRUE); //TODO i18n
		return BT_FALSE;
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
		if (ctx && *p && *(++p)){
			printMessage(ctx,(result)?"message":"alert",tr((result)?"rec.update.ok":"rec.update.err"),p,BT_TRUE);
		}
		free(data);
	} else {
		if (ctx) printMessage(ctx,"alert",tr("rec.update.err"),tr("warnSvdrpConnection"),BT_TRUE);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t deleteRec(context_t *ctx, const rec_t *rec) {
	if (!rec->path) {
		if (ctx) printMessage(ctx,"alert",tr("rec.delete.err"),tr("recErrorNoPath"),BT_TRUE);
		return BT_FALSE;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host){
		if (ctx) printMessage(ctx,"alert",tr("rec.delete.err"),tr("recErrorWrongHost"),BT_TRUE);
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
		if (ctx && *p && *(++p)){
			printMessage(ctx,(result)?"message":"alert",tr((result)?"rec.delete.ok":"rec.delete.err"),p,BT_TRUE);
		}
		free(data);
	} else {
		if (ctx) printMessage(ctx,"alert",tr("rec.delete.err"),tr("warnSvdrpConnection"),BT_TRUE);
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

//TODO usar printEvent
void printRecInfo(context_t *ctx, const rec_t * const rec){
	int minutes = rec->event.duration/60;
	ctx_printfn(ctx,"<div class=\"recInfo\">\n",0,1);
	ctx_printfn(ctx,"<div class=\"recDate\">\n",0,1);
	ctx_printf0(ctx,
		"<span class=\"recStart field\">"
			"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
		"</span>\n",tr("date"),formatDate(localtime(&rec->event.start),1));
	ctx_printf0(ctx,
		"<span class=\"recDuration field\">"
			"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%d&#39;</span>"
		"</span>\n",tr("runtime"),minutes);
	ctx_printfn(ctx,"</div>\n",-1,0);
	if (webifConf.displayHostId && webifConf.hostsLength>1){
		hostConf_t *host=getHost(rec->hostId);
		if (host){
			ctx_printfn(ctx,"<div class=\"recHost\">\n",0,1);
			ctx_printf0(ctx,
				"<span class=\"field\">"
					"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
				"</span>\n","Host",(host->name[0])?host->name:(host->ip[0])?host->ip:"?");
			ctx_printfn(ctx,"</div>\n",-1,0);
		}
	}
	if (rec->event.desc && rec->event.desc[0]) {
		ctx_printfn(ctx,"<div class=\"desc\">\n",0,1);
		printEventDesc(ctx,rec->event.desc,BT_FALSE);
		ctx_printfn(ctx,"</div>\n",-1,0);
	}
	ctx_printfn(ctx,"</div>\n",-1,0);
}



void printRecPlayLink(context_t *ctx, const rec_t *rec, boolean_t direct){
	const char *hostAddr=getHostHttpAddr(getHost(rec->hostId),ctx);
	ctx_printf(ctx,"http://%s/%s.kl1?path=%s",hostAddr,(direct)?"streamrec":"playlistrec",CTX_URL_ENCODE(rec->path,-1,"-_.~"));
}

boolean_t printRecPlaylist(context_t *ctx, rec_t *rec){
	if (isValidRecPath(rec->path)){
		response_set_content_type(ctx->response,M3U_MIME_TYPE);
		sprintf(ctx->buffer,"inline; filename=%s.m3u",(rec->name!=NULL)?rec->name:tr("recording.filename"));
		response_set_field(ctx->response,"Content-Disposition",ctx->buffer);
		printRecPlayLink(ctx,rec,BT_TRUE);
		return BT_TRUE;
	} else {
		response_set_status(ctx->response,HTTP_STATUS_NO_CONTENT);
		warn("Invalid rec path [%s]",rec->path);
		return BT_FALSE;
	}
}

void printRecControls(context_t *ctx,const rec_t *rec,const char *Play,const char *Edit,const char *Delete){
	if (rec->path==NULL) return;
	ctx_printf0(ctx,"<input type=\"hidden\" name=\"path\" value=\"%s\" />\n",rec->path);
	if (rec->id>0) {
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"id\" value=\"%d\" />\n",rec->id);
	}
	ctx_printf0(ctx,"<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n",rec->hostId);
	ctx_printfn(ctx,"<ul class=\"controls\"><!--\n",0,1);
	if (Play) {
		ctx_printfn(ctx,"--><li class=\"control\">\n",0,1);
#ifdef PRINT_REC_CONTROLS_AS_FORM		
		ctx_printf0(ctx,
			"<button type=\"submit\" class=\"play control button-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<div><span class=\"ui-icon ui-icon-play\">%s</span></div>"
			"</button>\n",PA_PLAY,Play,Play)
#else
		ctx_printf0(ctx,"<a href=\"");
		printRecPlayLink(ctx,rec,BT_FALSE);
		ctx_printf(ctx,"\" class=\"play control button-i ui-state-default\" title=\"%s\">"
				"<span class=\"ui-icon ui-icon-play\">%s</span>"
			"</a>\n",Play,Play);
#endif
		ctx_printfn(ctx,"</li><!--\n",-1,0);
	}
	if (Edit) {
		ctx_printfn(ctx,"--><li class=\"control\">\n",0,1);
		ctx_printf0(ctx,
			"<button type=\"submit\" class=\"edit control button-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<div><span class=\"ui-icon ui-icon-edit\">%s</span></div>"
			"</button>\n",PA_EDIT,Edit,Edit);
		ctx_printfn(ctx,"</li><!--\n",-1,0);
	}
	if (Delete && !webifConf.deletionDisabled) {
		ctx_printfn(ctx,"--><li class=\"control\">\n",0,1);
		ctx_printf0(ctx,
			"<button type=\"submit\" class=\"delete control button-i ui-state-default\" name=\"a\" value=\"%d\" title=\"%s\">"
				"<div><span class=\"ui-icon ui-icon-trash\">%s</span></div>"
			"</button>\n",PA_DELETE,Delete,Delete);
		ctx_printfn(ctx,"</li><!--\n",-1,0);
	}
	ctx_printfn(ctx,"--></ul>\n",-1,0);
}

void printRecEditForm(context_t *ctx, rec_t *rec){
	int l;
	ctx_printf0(ctx,"<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n",rec->hostId);
	if (rec->id>0){
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"id\" value=\"%d\"/>\n",rec->id);
	}
	if (rec->path!=NULL) {
		dbg("rec->path=[%s]",rec->path);
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"path\" value=\"%s\"/>\n",rec->path);
	} else {
		warn("rec->path==NULL");
	}
	ctx_printfn(ctx,"<table id=\"recEdit\" class=\"list formContent\" summary=\"\">\n",0,1);
	ctx_printf0(ctx,"<caption class=\"formTitle\">%s</caption>\n",tr("rec.edit"));
	if (rec->name!=NULL || rec->path!=NULL){
		if (rec->name==NULL){
			l=strrchr(rec->path,'/')-rec->path;
			CTX_CHK_BUFFER(l);
			vdrDecode(ctx->buffer,rec->path,l);
			crit_goto_if((rec->name=strdup(ctx->buffer))==NULL,outOfMemory);
			setFlag(RF_NAME,rec->my);
		};
		ctx_printfn(ctx,"<tr>\n",0,1);
		ctx_printf0(ctx,"<th>%s</th>\n",tr("name"));
		ctx_printfn(ctx,"<td>\n",0,1);
		l=strlen(rec->name);
		CTX_HTML_ENCODE(rec->name,l);
		ctx_printf0(ctx,"<input type=\"text\" name=\"name\" value=\"%s\" size=\"%d\"/>\n",ctx->buffer,5+(l>55)?55:l);
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"oldName\" value=\"%s\"/>\n",ctx->buffer);
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"</tr>\n",-1,0);
	}
	if (rec->event.title!=NULL){
		ctx_printfn(ctx,"<tr>\n",0,1);
		ctx_printf0(ctx,"<th>%s</th>\n",tr("title"));
		ctx_printfn(ctx,"<td>\n",0,1);
		//ctx_printf0(ctx,"<textarea name=\"title\" cols=\"%d\" rows=\"%d\">",(l>70)?70:l,(l/70)+2);
		ctx_printf(ctx,"%s",CTX_HTML_ENCODE(rec->event.title,-1));
		//ctx_printf(ctx,"</textarea>\n");
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"</tr>\n",-1,0);
	}
	if (rec->event.shortdesc!=NULL){
		ctx_printfn(ctx,"<tr>\n",0,1);
		ctx_printf0(ctx,"<th>%s</th>\n",tr("shortdesc"));
		ctx_printfn(ctx,"<td>\n",0,1);
		//ctx_printf0(ctx,"<textarea name=\"shortdesc\" cols=\"%d\" rows=\"%d\">",(l>70)?70:l,(l/70)+2);
		ctx_printf(ctx,"%s",CTX_HTML_ENCODE(rec->event.shortdesc,-1));
		//ctx_printf(ctx,"</textarea>\n");
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"</tr>\n",-1,0);
	}
	if (rec->event.desc!=NULL){
		ctx_printfn(ctx,"<tr>\n",0,1);
		ctx_printf0(ctx,"<th>%s</th>\n",tr("desc"));
		ctx_printfn(ctx,"<td>\n",0,1);
		//l=strlen(rec->event.desc);
		//ctx_printfn(ctx,"<textarea name=\"desc\" cols=\"%d\" rows=\"%d\">\n",0,1,(l>70)?70:l,(l/70)+2);
		printEventDesc(ctx,rec->event.desc,BT_FALSE);
		//ctx_printfn(ctx,"</textarea>\n",-1,0);
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"</tr>\n",-1,0);
	}

	ctx_printfn(ctx,"<tr class=\"buttons\">\n",0,1);
	ctx_printfn(ctx,"<td class=\"buttons ajaxButtonsHolder\" colspan=\"2\">\n",0,1);
	if (rec->name!=NULL) {
		ctx_printf0(ctx,
			"<button type=\"submit\" name=\"a\" value=\"%d\" class=\"confirm ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-check\">&nbsp;</span>%s</div>"
			"</button>\n",PA_CONFIRM,tr("accept"));
	}
	if (rec->path!=NULL) {
		const char *Play=tr("play");
		ctx_printf0(ctx,"<a href=\"");
		printRecPlayLink(ctx,rec,BT_FALSE);
		ctx_printf(ctx,"\" class=\"play ui-state-default button-i-t\">"
				"<span class=\"ui-icon ui-icon-play\">&nbsp;</span>%s"
			"</a>\n",Play);
	}
	if (!webifConf.deletionDisabled && (rec->id>0 || rec->path) ) {
		ctx_printf0(ctx,
			"<button type=\"submit\" name=\"a\" value=\"%d\" class=\"delete ui-state-default button-i-t\" >"
				"<div><span class=\"ui-icon ui-icon-trash\">&nbsp;</span>%s</div>"
			"</button>\n",PA_DELETE,tr("rec.delete"));
	}
	ctx_printfn(ctx,"</td>\n",-1,0);
	ctx_printfn(ctx,"</tr>\n",-1,0);

	ctx_printfn(ctx,"</table>\n",-1,0);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}


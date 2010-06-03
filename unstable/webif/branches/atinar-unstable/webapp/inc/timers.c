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
#include <klone/utils.h>

#include "epg.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"
#include "timers.h"

char *getTimerStrAt(hostConf_t *const host, int id);

void initTimer(vdrTimer_t *const timer) {
	memset(timer,0,sizeof(vdrTimer_t));
	timer->flags=TF_ACTIVE;
	timer->lifetime=99;
	timer->priority=50;
	strcpy(timer->wdays,"-------"); 
	timer->inConflict=false;
	timer->percent=100;
}

bool initTimerFromEvent(wcontext_t *wctx, vdrTimer_t *const timer, hostConf_t *host, const int channelNum, const int eventId){
	initTimer(timer);
	bool result= false;
	if (!host || !host->isVdr) {
		if (wctx) printMessage(wctx,"alert",tr("timer.delete.err"),"Host nulo o no es VDR",false); //TODO i18n
	} else {
		channelEvents_t channelEvents;
		initChannelEvents(&channelEvents);
		channelEvents.channelNum=channelNum;
		channelEvents.eventId=eventId;
		channelEvents.dumpMode=dmByIds;
		getChannelEvents(host,&channelEvents);
		if (channelEvents.length>0){
			epgEvent_t *event=channelEvents.entry;
			setFlag(TF_ACTIVE,timer->flags);
			timer->hostId=host->id;
			timer->channelNum=channelNum; //TODO verificar
			timer->type=TT_ONE_TIME;
			timer->title=strdup(event->title); setFlag(TF_TITLE,timer->my);
			timer->start=timer->stop=event->start;
			timer->stop+=event->duration;
			result=true;
		}
		freeChannelEvents(&channelEvents);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool makeTime(time_t *time, const char * cdate, int hour, int min ){
	bool result=false;
	struct tm sdate;
	if (strptime(cdate,"%Y-%m-%d",&sdate)) {
		sdate.tm_hour=hour;
		sdate.tm_min=min;
		sdate.tm_sec=0;
		sdate.tm_wday=0; 
		sdate.tm_yday=0;  
		sdate.tm_isdst=-1;
		*time=mktime(&sdate);
		result=true;
	}
	return result;
}


bool initTimerFromArgs(vdrTimer_t *const timer, vars_t *args, wcontext_t *wctx){
	bool result=true;
	bool isACopy;
	initTimer(timer);
	timer->flags=0;
	if (vars_countn(args,"active")>0){
		setFlag(TF_ACTIVE,timer->flags);
	}
	timer->channelNum=(vars_countn(args,"channelNum")>0) ? vars_get_value_i(args,"channelNum") : 0;
	timer->type=(vars_countn(args,"type")>0) ? vars_get_value_i(args,"type") : TT_ONE_TIME;
	char *cdate=NULL;
	if (timer->type==TT_ONE_TIME && vars_countn(args,"date")>0) {
		cdate=strdup(vars_get_value(args,"date"));
	} else {
		size_t i;
		for(i=0;i<vars_countn(args,"wday");i++){
			int wday=vars_geti_value_i(args, "wday",i);
			if (wday>6||wday<0){
				result=false;
				printMessage(wctx,"alert",tr("timerErrWrongWday"),NULL,false);
			} else {
				timer->wdays[(wday+6)%7]=weekdays[langId][wday][0]; 
			}
		}
	}
	timer->lifetime=(vars_countn(args,"lifetime")>0) ? vars_get_value_i(args,"lifetime") : 99;
	timer->priority=(vars_countn(args,"priority")>0) ? vars_get_value_i(args,"priority") : 50;
	timer->title=wctxGetRequestParam(wctx,args,"title",&isACopy);
	if (isACopy) setFlag(TF_TITLE,timer->my);
	timer->aux=wctxGetRequestParam(wctx,args,"aux",&isACopy);
	if (isACopy) setFlag(TF_AUX,timer->my);
	bool addMargin= boolean(vars_countn(args,"addMargin")>0);
	if	( (timer->type==TT_UNDEFINED) ||
		( (timer->type==TT_ONE_TIME) && (!cdate) ) ||
		( (timer->type==TT_REGULAR) && (strlen(timer->wdays)<7) ) ||
		( (!vars_countn(args,"startHour")) || (!vars_countn(args,"startMin")) || (!vars_countn(args,"endHour")) || (!vars_countn(args,"endMin")) ) ) 
	{
		result=false;
		printMessage(wctx,"alert",tr("timerErrIncompleteArgs"),NULL,false);
	} else {
		int startH=vars_get_value_i(args,"startHour"); 
		int startM=vars_get_value_i(args,"startMin");
		int endH=vars_get_value_i(args,"endHour");
		int endM=vars_get_value_i(args,"endMin");
		if (timer->type==TT_REGULAR) {
			//El dia no importa, solo las horas
			time_t rawtime=time(NULL);
			struct tm *today;
			today=localtime(&rawtime); 
			cdate=malloc(12);    
			strftime(cdate,11,"%Y-%m-%d",today);
		}
		if ( (endH<startH) || (endH==startH)&&(endM<startM)) {
			endH+=24; //mktime will normalize this
		}
		if (makeTime(&timer->start,cdate,startH,startM) && makeTime(&timer->stop,cdate,endH,endM)){
			if (addMargin){
				timer->start-=webifConf.defaultMarginStart*60;
				timer->stop+=webifConf.defaultMarginStop*60;
			}
		}
		free(cdate);
	}
	return result;
}

void freeTimer(vdrTimer_t *const timer) {
	if (isFlagSet(TF_TITLE,timer->my)) free(timer->title);
	if (isFlagSet(TF_AUX,timer->my)) free(timer->aux);
	if (isFlagSet(TF_TIMER_STR,timer->my)) free(timer->timerStr);
	initTimer(timer);
}

void initTimerList(timerList_t *const list){
	list->length=0;
	list->entry=NULL;
}

void freeTimerList(timerList_t *const list){
	int i;
	for (i=0;i<list->length;i++){
		freeTimer(&(list->entry[i]));
	}
	free(list->entry);
	initTimerList(list);
}

int compareTimers(const vdrTimer_t *ta, const vdrTimer_t *tb, sortField_t sortBy, sortDirection_t sortDirection){
	if (sortDirection==SF_NONE) return 0;
	int result=0;
	int step;
	for (step=0;step<2;step++){
		switch (sortBy) {
			case SF_TM_NUMBER:   
				result=sortDirection*(ta->id-tb->id);
				if (result) return result; else sortBy=SF_HOST; break;
			case SF_TITLE:
				result=sortDirection*strcasecmp(ta->title,tb->title);
				if (result) return result; else sortBy=SF_START; break;
			case SF_TYPE:     
				result=sortDirection*(ta->type-tb->type);	
				if (result) return result; else sortBy=SF_START; break;
			case SF_CH_NUMBER:
				result=sortDirection*(ta->channelNum-tb->channelNum);	
				if (result) return result; else sortBy=SF_START; break;
			case SF_NAME:
				result=(ta->channel && tb->channel) 
					? sortDirection*strcmp(ta->channel->channelName,tb->channel->channelName) : 0;
				if (result) return result; else sortBy=SF_START; break;
			case SF_PRIORITY: 
				result=sortDirection*(ta->priority-tb->priority);	
				if (result) return result; else sortBy=SF_START; break;
			case SF_LIFETIME: 
				result=sortDirection*(ta->lifetime-tb->lifetime);	
				if (result) return result; else sortBy=SF_START; break;
			case SF_MUX:
				result=(ta->channel&& tb->channel) 
					? sortDirection*strcmp(ta->channel->multiplexName,tb->channel->multiplexName) : 0;
				if (result) return result; else sortBy=SF_START; break;
			case SF_HOST:
				result=sortDirection*(ta->hostId-tb->hostId);
				if (result) return result; else sortBy=SF_START; break;
			case SF_START:    
			default:
				return sortDirection*(ta->start-tb->start);	
		}
	}
	return result;
}

void sortTimerList(timerList_t *const timers, const sortField_t sortBy, const sortDirection_t sortDirection){
	int compare(const void *a, const void *b) {
		return compareTimers((const vdrTimer_t *)a,(const vdrTimer_t *)b, sortBy, sortDirection);
	}
	if (timers->length>0 && sortBy!=SF_NONE) {
		qsort(timers->entry,timers->length,sizeof(vdrTimer_t),compare);
	} 
}

void getTimerListHost(hostConf_t *const host, timerList_t *const timers, channelList_t const *const channels){
	char *data=execSvdrp(host,"LSTT");
	if (data){
		char *p;
		for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
			if (atoi(p)==SVDRP_CMD_OK) {
				crit_goto_if((timers->entry=(vdrTimer_t *)realloc(timers->entry,(++timers->length)*sizeof(vdrTimer_t)))==NULL,outOfMemory);
				vdrTimer_t *tmp=timers->entry+timers->length-1;
				initTimer(tmp);
				tmp->id=strtol(p+4,&p,10);
				tmp->hostId=host->id;
				p+=strspn(p," ");
				tmp->timerStr=strdup(p); setFlag(TF_TIMER_STR,tmp->my);
				if (parseTimer(p,tmp)){
					//TODO solo funciona si los canales son los mismos en todos los VDRs
					if (tmp->channelNum>0 && tmp->channelNum<=channels->length) {
						tmp->channel = channels->channel+tmp->channelNum-1;
					}
				} else {
					freeTimer(tmp);
					timers->length--;
				} 
			}
		}
		free(data);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getTimerList(timerList_t *const timers, channelList_t const *const channels, const sortField_t sortBy, const sortDirection_t sortDirection){
	initTimerList(timers);
	int h;
	hostConf_t *host;
	for (h=0,host=webifConf.hosts;h<webifConf.hostsLength;h++,host++){
		if (host->isVdr){
			getTimerListHost(host,timers,channels);
		}
	}
	sortTimerList(timers,sortBy,sortDirection);
}

bool addTimer(wcontext_t *wctx, hostConf_t *const host, const char *newTimerStr) {
	bool result= false;
	if (!host || !host->isVdr) {
		if (wctx) printMessage(wctx,"alert",tr("timer.create.err"),"Host nulo o no es VDR",false); //TODO i18n
	} else {
		char *command;
		crit_goto_if(asprintf(&command,"NEWT %s",newTimerStr)<0,outOfMemory);
		char *data=execSvdrp(host,command);
		free(command);
		if (data){
			char *p=data;
			int code=strtol(p,&p,10);
			result=boolean(code==SVDRP_CMD_OK);
			if (wctx && *p && *(++p)){
				printMessage(wctx,(result)?"message":"alert",tr((result)?"timer.create.ok":"timer.create.err"),p,true);
			}
			free(data);
		}
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool deleteTimer(wcontext_t *wctx, hostConf_t *host, int timerId, const char *oldTimerStr) {
	bool result=false;
	if (!host || !host->isVdr) {
		if (wctx) printMessage(wctx,"alert",tr("timer.delete.err"),"Host nulo o no es VDR",false); //TODO i18n
	} else if (oldTimerStr==NULL) {
		if (wctx) printMessage(wctx,"alert",tr("timer.delete.err"),"Faltan argumentos",false); //TODO i18n
	} else {
		char *timerStr=getTimerStrAt(host, timerId);
		if (timerStr!=NULL) {
			if (strcmp(oldTimerStr,timerStr)==0) {
				char *command=NULL;
				crit_goto_if(asprintf(&command,"DELT %d",timerId)<0,outOfMemory);
				char *data=execSvdrp(host,command);
				free(command);
				if (data!=NULL){
					char *p=data;
					int code=strtol(p,&p,10);
					result=boolean(code==SVDRP_CMD_OK);
					if (wctx && *p && *(++p)){
						printMessage(wctx,(result)?"message":"alert",tr((result)?"timer.delete.ok":"timer.delete.err"),p,true);
					}
					free(data);
				}
			} else {
				if (wctx) printMessage(wctx,"alert",tr("timer.delete.err"),"Probablemente la programación ha sido modificada",false); //TODO i18n
			}
			free(timerStr);
		} else { 
			if (wctx) printMessage(wctx,"alert",tr("timer.delete.err"),"Programación no existe",false); //TODO i18n
		}
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool editTimer(wcontext_t *wctx, hostConf_t *const host, int timerId, const char *oldTimerStr, const char *newTimerStr) {
	if (!host || !oldTimerStr || !newTimerStr){
		if (wctx) printMessage(wctx,"alert",tr("timer.update.err"),"Faltan argumentos",false); //TODO i18n
		return false;
	}
	if (strcmp(oldTimerStr,newTimerStr)==0){
		if (wctx) printMessage(wctx,"alert",tr("timer.update.err"),"Nada que hacer. No hay cambios",false); //TODO i18n
		return true;
	}
	bool result= false;
	char *timerStr=getTimerStrAt(host,timerId);
	if (timerStr!=NULL) {
		if (strcmp(oldTimerStr,timerStr)==0) {
			char *command=NULL;
			crit_goto_if(asprintf(&command,"MODT %d %s",timerId,newTimerStr)<0,outOfMemory);
			char *data=execSvdrp(host,command);
			free(command);
			if (data!=NULL){
				char *p=data;
				int code=strtol(p,&p,10);
				result=boolean(code==SVDRP_CMD_OK);
				if (wctx && *p && *(++p)){
					printMessage(wctx,(result)?"message":"alert",tr((result)?"timer.update.ok":"timer.update.err"),p,true);
				}
				free(data);
			} else {
				if (wctx) printMessage(wctx,"alert",tr("timer.update.err"),tr("warnSvdrpConnection"),false);
			}
		} else {
			printMessage(wctx,"alert",tr("timer.update.err"),"Programación no existe",false);
		}
		free(timerStr);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *getTimerStrAt(hostConf_t *const host, int id) {
	char *timerStr=NULL;
	if ( (id>0) && (id<10000) ) {
		char *command=NULL;
		crit_goto_if(asprintf(&command,"LSTT %d",id)<0,outOfMemory);
		char *data=execSvdrp(host,command);
		free(command);
		if (data) {
			char *p;
			for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
				if (atoi(p)==SVDRP_CMD_OK) {
					p+=4;
					p+=strcspn(p," "); //ID
					p+=strspn(p," ");
					timerStr=strdup(p);
					break;
				}
			}
			free(data);
		}
	}
	return timerStr;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool parseTimer(const char *line, vdrTimer_t *const timer ){
	char *c;
	char *h;
	int l;
	char cdate[50];
	struct tm sdate;

	const char *r=line;
	char *n; //pointer to next field, only to check integrity
	int field;
	for(field=0;field<9 && r[0];field++) {
		l=strcspn(r,":");
		errno=0;
		switch(field){
			case 0:
				timer->flags=(uint)strtol(r,&n,10); 
				if (n[0]!=':') goto wrongFormat;
				break;
			case 1: 
				timer->channelNum=strtol(r,NULL,10); break;
			case 2: //store date in cdate
				if (l==7) {
					strncpy(timer->wdays,r,l);
					timer->wdays[l]='\0';
					timer->type=TT_REGULAR;
					strcpy(cdate,"1970-01-02");
					time_t tdate=time(NULL);
					int wday;
					for (wday=0;wday<7;wday++) {
						sdate=*localtime(&tdate);
						if (timer->wdays[(sdate.tm_wday+6)%7]!='-') {
							strftime(cdate,sizeof(cdate)-1,"%Y-%m-%d",&sdate);
							break;
						}
						tdate+=86400;
					}
				} else {
					strncpy(cdate,r,l);
					cdate[l]='\0';
					timer->type=TT_ONE_TIME;
				}
				break;
			case 3: //store start hour in cdate
				h=cdate+strlen(cdate);
				strcpy(h," 00:00");
				h++;
				//don't break here!
			case 4: //store stop hour in cdate
				strncpy(h,r,2);
				strncpy(h+3,r+2,2);
				if (!strptime(cdate,"%Y-%m-%d %H:%M",&sdate)) { 
					warn("Error converting date \"%s\"",cdate);
					return false;
				}
				sdate.tm_sec=0; 
				sdate.tm_isdst=-1;
				if (field==3) {
					timer->start=mktime(&sdate);
				} else {
					timer->stop=mktime(&sdate);
					if ( timer->stop < timer->start) {
						timer->stop+=24*60*60;
					}
				}
				break;
			case 5: 
				timer->priority=strtol(r,NULL,10); 
				break;
			case 6: 
				timer->lifetime=strtol(r,NULL,10); 
				break;
			case 7: 
				crit_goto_if((timer->title=strndup(r,l))==NULL,outOfMemory); 
				setFlag(TF_TITLE,timer->my); 
				break;
			case 8:
				r+=strspn(r," ");
				if (r[0]) {
					crit_goto_if((timer->aux=strdup(r))==NULL,outOfMemory);
					setFlag(TF_AUX,timer->my);
				}
				break;
		}
		if (errno) {
			warn(strerror(errno));
			goto wrongFormat;
		}
		r+=l;
		if (r[0]!=':') break;
		r++;
	}
	return true;
wrongFormat:
	warn("Wrong timer string [%s]",line);
	return false;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *makeRegularTimerStr(uint flags,int channelNum,const char *wdays,time_t start,time_t stop,int priority,int lifetime,char *title,const char *aux) {
	struct tm t1=*localtime(&start);
	struct tm t2=*localtime(&stop);
	char *timerStr;
	char *c;
	for(c=title;*c;c++) if (*c==':') *c='|';
	crit_goto_if(asprintf(&timerStr,"%d:%d:%.7s:%02d%02d:%02d%02d:%d:%d:%s:%s"
		,flags
		,channelNum
		,wdays
		,t1.tm_hour,t1.tm_min
		,t2.tm_hour,t2.tm_min
		,priority
		,lifetime
		,title
		,(aux)?aux:"")<1,outOfMemory);
	for(c=title;*c;c++) if (*c=='|') *c=':';
	return timerStr;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *makeOneTimeTimerStr(uint flags,int channelNum,time_t start,time_t stop,int priority,int lifetime,char *title,const char *aux) {
	struct tm t1=*localtime(&start);
	struct tm t2=*localtime(&stop);
	char *timerStr;
	char *c;
	for(c=title;*c;c++) if (*c==':') *c='|';
	crit_goto_if(asprintf(&timerStr,"%d:%d:%d-%02d-%02d:%02d%02d:%02d%02d:%d:%d:%s:%s"
		,flags
		,channelNum
		,1900+t1.tm_year,t1.tm_mon+1,t1.tm_mday
		,t1.tm_hour,t1.tm_min
		,t2.tm_hour,t2.tm_min
		,priority
		,lifetime
		,title
		,(aux)?aux:"")<1,outOfMemory);
	for(c=title;*c;c++) if (*c=='|') *c=':';
	return timerStr;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *makeTimerStr(vdrTimer_t *const timer){
	return (timer->type==TT_REGULAR) 
	? makeRegularTimerStr(timer->flags,timer->channelNum,timer->wdays,timer->start,timer->stop,timer->priority,timer->lifetime,timer->title,timer->aux)
	: makeOneTimeTimerStr(timer->flags,timer->channelNum,timer->start,timer->stop,timer->priority,timer->lifetime,timer->title,timer->aux);
}
//------------------------------
void printTimerForm(wcontext_t *wctx, vdrTimer_t *const timer, channelList_t const *const channels){
	wctx_printfn(wctx,"<form id=\"timerEdit\" action=\"/timers.kl1\" method=\"post\">\n",0,1);

	if (timer->id>0) {
		wctx_printf0(wctx,"<input type=\"hidden\" name=\"timerId\" value=\"%d\"/>\n",timer->id);
		wctx_printf0(wctx,"<input type=\"hidden\" name=\"oldTimerStr\" value=\"%s\"/>\n",CTX_HTML_ENCODE(timer->timerStr,-1));
	}
	const char *TimerEdit=tr((timer->id>0)?"timer.edit":"timer.add");
	wctx_printfn(wctx,"<table id=\"timerEdit\" class=\"list formContent\" summary=\"%s\">\n",0,1,TimerEdit);
	wctx_printf0(wctx,"<caption class=\"formTitle\">%s</caption>\n",TimerEdit);

	wctx_printfn(wctx,"<tbody>\n",0,1);
	wctx_printf0(wctx,
		"<tr>"
			"<th>%s</th>"
			"<td><input type=\"checkbox\" name=\"active\" value=\"1\"%s/></td>"
		"</tr>\n"
		,tr("active"),checked[isFlagSet(TF_ACTIVE,timer->flags)]
	);

	wctx_printfn(wctx,"<tr>\n",0,1);
	wctx_printf0(wctx,"<th>%s</th>\n",tr("channel"));
	wctx_printfn(wctx,"<td>\n",0,1);
	printChannelListSelect(wctx,NULL,"channelNum",channels,timer->channelNum,NULL);
	wctx_printfn(wctx,"</td>\n",-1,0);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	wctx_printfn(wctx,"<tr>\n",0,1);
	wctx_printf0(wctx,"<th>%s</th>\n","Host");
	wctx_printfn(wctx,"<td>\n",0,1);
	if (webifConf.numVDRs>1){
		printVDRSelect(wctx,"hostId",timer->hostId);
	} else {
		wctx_printf0(wctx,"<input type=\"text\" name=\"hostId\" class=\"readOnly\" readonly=\"readonly\" value=\"%d\"/>\n",timer->hostId);
	}
	wctx_printf0(wctx,"<input type=\"hidden\" name=\"oldHostId\" value=\"%d\"/>\n",timer->hostId);
	wctx_printfn(wctx,"</td>\n",-1,0);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	wctx_printfn(wctx,"<tr>\n",0,1);
	wctx_printf0(wctx,"<th><input type=\"radio\" name=\"type\" value=\"%d\"%s/>%s</th>\n"
		,TT_ONE_TIME,checked[boolean(timer->type==TT_ONE_TIME)],tr("oneTimeRecording"));
	wctx_printfn(wctx,"<td>\n",0,1);
	wctx_printfn(wctx,"<select name=\"date\" size=\"1\">\n",0,1);
	time_t tdate=time(NULL);
	if (timer->start<=0){
		timer->start=tdate;
	}
	else if (tdate>timer->start) {
		tdate=timer->start;
	}
	char cdate[11];
	struct tm sdate;
	bool isThatDay=false;
	bool isAfterThatDay=false;
	int i;
	for(i=0;i<14;i++) { //TODO usar datepicker
		sdate=*localtime(&tdate);
		strftime(cdate,11,"%Y-%m-%d",&sdate);
		if (!isAfterThatDay){
			isThatDay=sameDay(tdate,timer->start);
			isAfterThatDay=boolean(isThatDay||tdate>timer->start);
		}
		wctx_printf0(wctx,"<option value=\"%s\" %s>%s, %s</option>\n"
			,cdate,selected[isThatDay],weekdays[langId][sdate.tm_wday],formatDate(&sdate,false));
		if (isThatDay && (timer->type==TT_ONE_TIME)) { 
			//marcar por si se cambia a regular
			timer->wdays[(sdate.tm_wday+6)%7]=weekdays[langId][sdate.tm_wday][0]; 
		}
		isThatDay=false;
		tdate+=24*60*60;
	}
	wctx_printfn(wctx,"</select>\n",-1,0);
	wctx_printfn(wctx,"</td>\n",-1,0);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	wctx_printfn(wctx,"<tr>\n",0,1);
	wctx_printf0(wctx,"<th><input type=\"radio\" name=\"type\" value=\"%d\"%s/>%s</th>\n"
		,TT_REGULAR,checked[boolean(timer->type==TT_REGULAR)],tr("regularRecording"));
	wctx_printfn(wctx,"<td>\n",0,1);
	for(i=startOfWeek[langId];i<startOfWeek[langId]+7;i++) {
		wctx_printf0(wctx,"<input type=\"checkbox\" name=\"wday\" value=\"%d\"%s/>%s\n"
			,(i%7),checked[boolean(timer->wdays[(i+6)%7]!='-')],weekdays[langId][i%7]);
	}
	wctx_printfn(wctx,"</td>\n",-1,0);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	struct tm start_date=*localtime(&timer->start);
	wctx_printfn(wctx,"<tr>\n",0,1);
	wctx_printf0(wctx,"<th>%s</th>\n",tr("start"));
	wctx_printf0(wctx,
		"<td>"
			"<input type=\"text\" name=\"startHour\" maxlength=\"2\" size=\"3\" value=\"%02d\"/>&nbsp;:&nbsp;"
			"<input type=\"text\" name=\"startMin\"  maxlength=\"2\" size=\"3\" value=\"%02d\"/>"
		"</td>\n",start_date.tm_hour,start_date.tm_min);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	struct tm end_date=*localtime(&timer->stop);
	wctx_printfn(wctx,"<tr>\n",0,1);
	wctx_printf0(wctx,"<th>%s</th>\n",tr("end"));
	wctx_printf0(wctx,
		"<td>"
			"<input type=\"text\" name=\"endHour\" maxlength=\"2\" size=\"3\" value=\"%02d\"/>&nbsp;:&nbsp;"
			"<input type=\"text\" name=\"endMin\"  maxlength=\"2\" size=\"3\" value=\"%02d\"/>"
		"</td>\n",end_date.tm_hour,end_date.tm_min);
	wctx_printfn(wctx,"</tr>\n",-1,0);

	wctx_printf0(wctx,
		"<tr>"
			"<th>%s</th>"
			"<td><input type=\"checkbox\" name=\"addMargin\" value=\"%d\" %s/></td>"
		"</tr>\n"
		,tr("margin.add"),boolean(timer->id<1),checked[boolean(timer->id<1)]);

	wctx_printf0(wctx,
		"<tr>"
			"<th>%s</th>"
			"<td><input type=\"text\" name=\"priority\" maxlength=\"2\" size=\"3\" value=\"%d\"/></td>"
		"</tr>\n"
		,tr("priority"),timer->priority);

	wctx_printf0(wctx,
		"<tr>"
			"<th>%s</th>"
			"<td><input type=\"text\" name=\"lifetime\" value=\"%d\" maxlength=\"2\" size=\"3\"/></td>"
		"</tr>\n"
		,tr("lifetime"),timer->lifetime);

	wctx_printf0(wctx,
		"<tr>"
			"<th>%s</th>"
			"<td><input type=\"text\" name=\"title\" size=\"70\" value=\"%s\"/></td>"
		"</tr>\n"
		,tr("title"),(timer->title)?timer->title:"");

	if (timer->aux) {
		int l=strlen(timer->aux);
		wctx_printf0(wctx,
			"<tr>"
				"<th>%s</th>"
				"<td><textarea name=\"aux\" spellcheck=\"false\" cols=\"%d\" rows=\"%d\">%s</textarea></td>"
			"</tr>\n"
			,tr("aux"),(l>70)?70:l,l/70+2,CTX_HTML_ENCODE(timer->aux,l));
	}

	wctx_printfn(wctx,"<tr class=\"buttons\">\n",0,1);
	wctx_printfn(wctx,"<td class=\"ajaxButtonsHolder\" colspan=\"2\">\n",0,1);
	wctx_printf0(wctx,
		"<button id=\"confirm\" class=\"confirm ui-state-default button-i-t\" name=\"a\" type=\"submit\" value=\"%d\" >"
			"<div><span class=\"ui-icon ui-icon-check\">&nbsp;</span>%s</div>"
		"</button>\n"
		,PA_ADD,tr("accept"));
	if (timer->id>0){
		wctx_printf0(wctx,
			"<button id=\"delete\" class=\"delete ui-state-default button-i-t\" name=\"a\" type=\"submit\" value=\"%d\" >"
				"<div><span class=\"ui-icon ui-icon-trash\">&nbsp;</span>%s</div>"
			"</button>\n"
			,PA_DELETE,tr("timer.delete"));
	}
	wctx_printfn(wctx,"</td>\n",-1,0);
	wctx_printfn(wctx,"</tr>\n",-1,0);
	wctx_printfn(wctx,"</tbody>\n",-1,0);
			
	wctx_printfn(wctx,"</table>\n",-1,0);
	wctx_printfn(wctx,"</form>\n",-1,0);
}

void printTimerBars(wcontext_t *wctx, timerList_t *const timers, const int channelNum
	, const time_t startTime, const time_t duration,const char *TimerEdit,bool wrapPBWithA) {
	const time_t endTime=startTime+duration;
	bool isStart, isEnd, anyMatch;
	anyMatch=false;
	if ( (timers->length>0) && (startTime>0) && (duration>0) ) {
		int i;
		int mstart=0;
		int mend=0;
		vdrTimer_t *timer;
		struct tm sdate;
		int wday;
		for (i=0,timer=timers->entry;i<timers->length;i++,timer++) {
			if((timer->type == TT_REGULAR) && (timer->stop < startTime)){
				sdate=*localtime(&timer->start);
				wday=(sdate.tm_wday+6)%7;
				while((timer->stop < startTime) || (timer->wdays[wday]=='-')){
					//TODO daylight savings?
					wday=(wday+1)%7;
					timer->start+=24*60*60;
					timer->stop+=24*60*60;
				}
			}
		}
		sortTimerList(timers,SF_START,SD_ASC);
		for (i=0,timer=timers->entry;i<timers->length;i++,timer++) {
			if (((channelNum==0)||(timer->channelNum==channelNum)) && (timer->start <= endTime) && (timer->stop >= startTime)) {
				mstart=timer->start-startTime; //margen no cubierto al inicio
				mend  =endTime-timer->stop;    //margen no cubierto al final
				isStart=boolean(mstart>=0);
				isEnd=boolean(mend>=0);
				anyMatch=true;
				double left=(mstart>0)?100.0*mstart/duration:0.0;
				double right=(mend>0)?100.0*mend/duration:0.0;
				double width=(100.0-left-right);
				if (width<0.0) width=0;
				wctx_printfn(wctx,"<div id=\"timer%d_%d\" class=\"timer%s\">\n",0,1,++timer->count,timer->id,(timer->inConflict)?" conflict":"");
				wctx_printf0(wctx,"<a class=\"timerEdit progressbar\"");
				wctx_printf(wctx," href=\"timers.kl1?a=%d&amp;timerStr=%s&amp;timerId=%d&hostId=%d\"",PA_EDIT,CTX_URL_ENCODE(timer->timerStr,-1,NULL),timer->id,timer->hostId);
				wctx_printf(wctx," title=\"%s: %s\"",TimerEdit,timer->title);
				wctx_printf(wctx,">\n");
				inctab(wctx);
				wctx_printf0(wctx,"<span class=\"pb%s%s%s\""
					,(isStart)?" pbstart":"", (isEnd)?" pbend":"", (isFlagSet(TF_ACTIVE,timer->flags))?" pbenabled":" pbdisabled");
				wctx_printf(wctx," style=\"%s:%.2f%%;width:%.2f%%;\"",(isStart)?"right":"left",(isStart)?right:left,width);
				wctx_printf(wctx,">\n");
				inctab(wctx);
				wctx_printf0(wctx,"&nbsp;\n");
				wctx_printfn(wctx,"</span>\n",-1,0); //pb
				wctx_printfn(wctx,"</a>\n",-1,0); //progressbar
				wctx_printfn(wctx,"</div>\n",-1,0); //timer
			}
		}
	}
	//if (!anyMatch) wctx_printf0(wctx,"&nbsp;\n");
}

void initConflict(conflict_t * const conflict){
	conflict->when=(time_t)0;
	conflict->timer=NULL;
	conflict->nconcurrent=0;
	conflict->pconcurrent=NULL;
}

void freeConflict(conflict_t * const conflict){
	free(conflict->pconcurrent);
	initConflict(conflict);
}

void initConflictList(conflictList_t * const conflicts){
	conflicts->length=0;
	conflicts->entry=NULL;
}

void freeConflictList(conflictList_t * const conflicts){
	int i;
	for (i=0;i<conflicts->length;i++){
		freeConflict(conflicts->entry+i);
	}
	free(conflicts->entry);
	initConflictList(conflicts);
}

bool parseConflict(const char *line, conflict_t *const conflict, timerList_t const * timers ){
	int l;
	int timerId;
	int i;
	vdrTimer_t *ptimer;
	const char *r;
	char *n; //pointer to next field, only to check integrity

	r=line;
	conflict->when=(time_t)strtol(r,&n,10);
	if (n[0]!=':') goto wrongFormat;
	r=n+1;
	
	timerId=strtol(r,&n,10); 
	if (n[0]!='|') goto wrongFormat;
	for (i=0,ptimer=timers->entry;i<timers->length;i++,ptimer++){
		if (ptimer->hostId==conflict->hostId && ptimer->id==timerId){
			ptimer->inConflict=true;
			conflict->timer=ptimer;
			break;
		}
	}
	if (conflict->timer==NULL){
		warn("Wrong timer %d in conflict",timerId);
		return false;
	}
	r=n+1;
	
	conflict->timer->percent=strtol(r,&n,10); 
	if (n[0]!='|') goto wrongFormat;

	//TODO leer concurrentes

	return true;
wrongFormat:
	warn("Wrong conflict string s[%s] r[%s] n[%s]",line,r,n);
	if (errno) {
		warn(strerror(errno));
	}
	return false;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getConflictListHost(hostConf_t *const host, conflictList_t * const conflicts, timerList_t const * timers){
	char *data=execSvdrp(host,"PLUG epgsearch LSCC REL");
	if (data){
		char *p;
		for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
			if (strtol(p,&p,10)==SVDRP_PLUG_DEFAULT) {
				p++;
				crit_goto_if((conflicts->entry=(conflict_t *)realloc(conflicts->entry,(++conflicts->length)*sizeof(conflict_t)))==NULL,outOfMemory);
				conflict_t *tmp=conflicts->entry+conflicts->length-1;
				initConflict(tmp);
				tmp->hostId=host->id;
				if (!parseConflict(p,tmp,timers)){
					freeConflict(tmp);
					conflicts->length--;
				} 
			}
		}
		free(data);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getConflictList(conflictList_t * const conflicts, timerList_t const * timers){
	int i;
	hostConf_t *host;
	vdrTimer_t *ptimer;
	initConflictList(conflicts);
	for (i=0,ptimer=timers->entry;i<timers->length;i++,ptimer++){
		ptimer->inConflict=false;
		ptimer->percent=100;
	}
	for (i=0,host=webifConf.hosts;i<webifConf.hostsLength;i++,host++){
		if (host->isVdr){
			getConflictListHost(host,conflicts,timers);
		}
	}
}


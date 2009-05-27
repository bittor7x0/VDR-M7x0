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

#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"
#include "timers.h"

void initTimer(vdrTimer_t *const timer) {
	memset(timer,0,sizeof(vdrTimer_t));
	timer->flags=TF_ACTIVE;
	timer->lifetime=99;
	timer->priority=50;
	strcpy(timer->wdays,"-------"); 
}

boolean_t initTimerFromArgs(vdrTimer_t *const timer, vars_t *args, io_t *out, int ntabs){
	initTimer(timer);
	boolean_t result=BT_TRUE;
	timer->flags=0;
	if (vars_countn(args,"active")>0){
		timer->flags|=TF_ACTIVE;
	}
	timer->channelNum=(vars_countn(args,"channelNum")>0) ? vars_get_value_i(args,"channelNum") : 0;
	timer->type=(vars_countn(args,"type")>0) ? vars_get_value_i(args,"type") : TT_ONE_TIME;
	char *date=NULL;
	if (timer->type==TT_ONE_TIME && vars_countn(args,"date")>0) {
		date=strdup(vars_get_value(args,"date"));
	} else {
		size_t i;
		for(i=0;i<vars_countn(args,"wday");i++){
			int wday=vars_geti_value_i(args, "wday",i);
			if (wday>6||wday<0){
				result=BT_FALSE;
				if (out) printMessage(out,ntabs,"alert",tr("timerErrWrongWday"),NULL,NULL);
			} else {
				timer->wdays[(wday+6)%7]=weekdays[langId][wday][0]; 
			}
		}
	}
	const char *startH=vars_get_value(args,"startHour"); 
	const char *startM=vars_get_value(args,"startMin");
	const char *endH=vars_get_value(args,"endHour");
	const char *endM=vars_get_value(args,"endMin");
	timer->lifetime=(vars_countn(args,"lifetime")>0) ? vars_get_value_i(args,"lifetime") : 99;
	timer->priority=(vars_countn(args,"priority")>0) ? vars_get_value_i(args,"priority") : 50;
	timer->title=(char *)vars_get_value(args,"title");
	timer->aux=(char *)vars_get_value(args,"aux");
	boolean_t addMargin= boolean(vars_countn(args,"addMargin")>0);
	if	( (timer->type==TT_UNDEFINED) ||
		( (timer->type==TT_ONE_TIME) && (!date) ) ||
		( (timer->type==TT_REGULAR) && (strlen(timer->wdays)<7) ) ||
		( (!startH) || (!startM) || (!endH) || (!endM) ) ) 
	{
		result=BT_FALSE;
		if (out) printMessage(out,ntabs,"alert",tr("timerErrIncompleteArgs"),NULL,NULL);
	} else {
		if (timer->type==TT_REGULAR) {
			//El dia no importa, solo las horas
			time_t rawtime=time(NULL);
			struct tm *today;
			today=localtime(&rawtime); 
			date=malloc(12);    
			strftime(date,11,"%Y-%m-%d",today);
		}
		if ( (endH<startH) || (endH==startH)&&(endM<startM)) {
			endH+=24; //mktime will normalize this
		}
		if (makeTime(&timer->start,date,startH,startM) && makeTime(&timer->stop,date,endH,endM)){
			if (addMargin){
				timer->start-=vdrConf.marginStart;
				timer->stop+=vdrConf.marginStop;
			}
		}
		free(date);
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
				return sortDirection*(ta->start-tb->start);	
		}
	}
	return result;
}

void sortTimerList(timerList_t *const timers, const sortField_t sortBy, const sortDirection_t sortDirection){
	int compare(const void *a, const void *b) {
		compareTimers((const vdrTimer_t *)a,(const vdrTimer_t *)b, sortBy, sortDirection);
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
			if (atoi(p)==250) {
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
					freeTimer(tmp); //TODO decidir cual de las dos veces
					timers->length--;
				} 
			}
		}
		free(data);
	}
#ifdef TEST_REPEATED_HOSTS
	closeSvdrp(host);
#endif
	return;
outOfMemory:
	crit("getTimerListHost:out of memory");
	exit(1);
}

void getTimerList(timerList_t *const timers, channelList_t const *const channels, const sortField_t sortBy, const sortDirection_t sortDirection){
	initTimerList(timers);
	int h;
	hostConf_t *host;
#ifdef TEST_REPEATED_HOSTS
	closeSvdrpAll();
#endif
	for (h=0,host=webifConf.hosts;h<webifConf.hostsLength;h++,host++){
		if (host->isVdr){
			getTimerListHost(host,timers,channels);
		}
	}
	sortTimerList(timers,sortBy,sortDirection);
}

boolean_t addTimer(hostConf_t *const host, const char *newTimerStr, char **message) {
	boolean_t result= BT_FALSE;
	if (host && newTimerStr) {
		char *command;
		crit_goto_if(asprintf(&command,"NEWT %s",newTimerStr)<0,outOfMemory);
		char *data=execSvdrp(host,command);
		free(command);
		if (data){
			char *p=data;
			int code=strtol(p,&p,10);
			result=boolean(code==250);
			if (message && *p && *(++p)){
				(*message)=strdup(p);
			}
			free(data);
		}
	}
	return result;
outOfMemory:
	crit("addTimer:out of memory");
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
				if (atoi(p)==250) {
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
	crit("getTimerStrAt:out of memory");
	exit(1);
}

boolean_t deleTimer(hostConf_t *host, int id, const char *delTimerStr, char **message) {
	boolean_t result=BT_FALSE;
	char *timerStr=getTimerStrAt(host, id);
	if (timerStr!=NULL) {
		if (strcmp(delTimerStr,timerStr)==0) {
			char *command=NULL;
			crit_goto_if(asprintf(&command,"DELT %d",id)<0,outOfMemory);
			char *data=execSvdrp(host,command);
			free(command);
			if (data!=NULL){
				char *p=data;
				int code=strtol(p,&p,10);
				result=boolean(code==250);
				if (message && *p && *(++p)){
					(*message)=strdup(p);
				}
				free(data);
			}
		} else { 
			warn("Error trying to delete a timer: there is no timer as specified!");
			warn("Timer to delete(%d): [%s]",strlen(delTimerStr), delTimerStr);
			warn("Timer in list  (%d): [%s]",strlen(timerStr),timerStr);
		}
		free(timerStr);
	}
	return result;
outOfMemory:
	crit("deleTimer:out of memory");
	exit(1);
}

boolean_t editTimer(hostConf_t *const host, int id, const char *oldTimerStr, const char *newTimerStr, char **message) {
	if (!host || !oldTimerStr || !newTimerStr){
		return BT_FALSE;
	}
	if (strcmp(oldTimerStr,newTimerStr)==0){
		return BT_TRUE;
	}
	boolean_t result= BT_FALSE;
	char *timerStr=getTimerStrAt(host,id);
	if (timerStr!=NULL) {
		if (strcmp(oldTimerStr,timerStr)==0) {
			char *command=NULL;
			crit_goto_if(asprintf(&command,"MODT %d %s",id,newTimerStr)<0,outOfMemory);
			char *data=execSvdrp(host,command);
			free(command);
			if (data!=NULL){
				char *p=data;
				int code=strtol(p,&p,10);
				result=boolean(code==250);
				if (message && *p && *(++p)){
					(*message)=strdup(p);
				}
				free(data);
			}
		} else {
			warn("Error trying to edit a timer: there is no timer as specified!");
			warn("Timer to edit(%d): [%s]",strlen(oldTimerStr),oldTimerStr);
			warn("Timer in list(%d): [%s]",strlen(timerStr),timerStr);
		}
		free(timerStr);
	}
	return result;
outOfMemory:
	crit("editTimer:out of memory");
	exit(1);
}

boolean_t parseTimer(const char *line, vdrTimer_t *const timer ){
	//timer->my= ~0;
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
					warn("Error converting timer date \"%s\"!\n",cdate);
					return BT_FALSE;
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
	return BT_TRUE;
wrongFormat:
	warn("Wrong timer string [%s]",line);
	return BT_FALSE;
outOfMemory:
	crit("parseTimer:out of memory");
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
	crit("makeRegularTimerStr:out of memory");
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
	crit("makeOneTimeTimerStr:out of memory");
	exit(1);
}

char *makeTimerStr(vdrTimer_t *const timer){
	return (timer->type==TT_REGULAR) 
	? makeRegularTimerStr(timer->flags,timer->channelNum,timer->wdays,timer->start,timer->stop,timer->priority,timer->lifetime,timer->title,timer->aux)
	: makeOneTimeTimerStr(timer->flags,timer->channelNum,timer->start,timer->stop,timer->priority,timer->lifetime,timer->title,timer->aux);
}
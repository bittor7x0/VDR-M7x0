/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* (c) Atinar <atinar1@hotmail.com>
* Please checkout the README file!
* 
* Originally written for http://vdr-m7x0.foroactivo.com.es
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

#include "searches.h"
#include "epg.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"

char *getSearchStrAt(hostConf_t *const host, int id);

void initChannelGroup(channelGroup_t *const group){
	group->hostId=0;
	group->name=NULL;
	group->length=0;
	group->entry=NULL;
}

void freeChannelGroup(channelGroup_t *const group){
	free(group->name);
	initChannelGroup(group);
}

void initChannelGroupList(channelGroupList_t *const groups){
	groups->length=0;
	groups->entry=NULL;
}

void freeChannelGroupList(channelGroupList_t *const groups){
	int i;
	channelGroup_t *group;
	for (i=0,group=groups->entry;i<groups->length;i++,group++){
		freeChannelGroup(group);
	}
	free(groups->entry);
	initChannelGroupList(groups);
}

void getChannelGroupList(hostConf_t *host, channelGroupList_t *const groups, channelList_t const *const channels){
	initChannelGroupList(groups);
	char *data=execSvdrp(host,"PLUG epgsearch LSTC");
	if (data){
		char *g,*sg;
		channelGroup_t *group;
		for (g=strtok_r(data,"\r\n",&sg);g!=0;g=strtok_r(NULL,"\r\n",&sg)) {
			if (atoi(g)==SVDRP_PLUG_DEFAULT) {
				g+=4;
				crit_goto_if((groups->entry=(channelGroup_t *)realloc(groups->entry,(++groups->length)*sizeof(channelGroup_t)))==NULL,outOfMemory);
				group=groups->entry+groups->length-1;
				initChannelGroup(group);
				group->hostId=host->id;

				channel_t *channel;
				int i;
				char *c,*sc;
				boolean_t found;
				for (c=strtok_r(g,"|",&sc);c!=0;c=strtok_r(NULL,"|",&sc)) {
					if (group->name==NULL) {
						crit_goto_if((group->name=strdup(c))==NULL,outOfMemory);
					} else {
						for (i=0,channel=channels->channel,found=BT_FALSE;i<channels->length && !found;i++,channel++){
							if (strcmp(channel->channelId,c)==0){
								crit_goto_if((group->entry=(channel_t* *)realloc(group->entry,(++group->length)*sizeof(channel_t*)))==NULL,outOfMemory);
								group->entry[group->length-1]=channel;
								found=BT_TRUE;
							}
						}
						if (!found) {
							warn("Channel %s not found in host %d", c, host->id);
						}
					}
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

void printChannelGroupListSelect(context_t *ctx,const char * name,const channelGroupList_t *const groups,const char *groupName){
	int i;
	channelGroup_t *group;
	ctx_printfn(ctx,"<select name=\"%s\" size=\"1\">\n",0,1,name);
	for (i=0,group=groups->entry;i<groups->length;i++,group++) {
		ctx_printf0(ctx,"<option value=\"%s\" %s>%s</option>\n"
			,group->name,selected[boolean(groupName && strcmp(group->name,groupName)==0)],group->name);
	}
	ctx_printfn(ctx,"</select>\n",-1,0);
}

void initSearch(search_t *const search) {
	memset(search,0,sizeof(search_t));
	search->id=-1;
	search->searchMode=SEARCH_MODE_SUBSTRING;
	search->priority=50;
	search->lifetime=99;
	setFlag(EFI_TITLE,search->compareFlags);
	setFlag(EFI_SHORTDESC,search->compareFlags);
	setFlag(EFI_DESC,search->compareFlags);
	search->marginStart=webifConf.defaultMarginStart;
	search->marginStop=webifConf.defaultMarginStop;
	setFlag(SFL_USE_AS_SEARCH_TIMER,search->flags);
}

boolean_t initSearchFromEvent(context_t *ctx, search_t *const search, hostConf_t *host, const int channelNum, const int eventId) {
	initSearch(search);
	boolean_t result= BT_FALSE;
	if (!host || !host->isVdr) {
		if (ctx) printMessage(ctx,"alert",tr("search.create.err"),"Host nulo o no es VDR",BT_FALSE); //TODO i18n
	} else {
		channelEvents_t channelEvents;
		initChannelEvents(&channelEvents);
		channelEvents.channelNum=channelNum;
		channelEvents.eventId=eventId;
		channelEvents.dumpMode=dmByIds;
		getChannelEvents(host,&channelEvents);
		if (channelEvents.length>0){
			epgEvent_t *event=channelEvents.entry;
			//setFlag(TF_ACTIVE,search->flags);
			if (event->shortdesc!=NULL && event->shortdesc[0]){
				crit_goto_if(asprintf(&search->search,"%s %s",event->title,event->shortdesc)<1,outOfMemory);
				setFlag(ECF_COMPARE_SUBTITLE,search->compareFlags);
			} else {
				crit_goto_if(asprintf(&search->search,"%s",event->title)<1,outOfMemory);
			}
			setFlag(ECF_COMPARE_TITLE,search->compareFlags);
			search->hostId=host->id;
			search->useChannel=1;
			search->channelMin=search->channelMax=channelNum-1; 
			time_t ttime=event->start;
			struct tm stime=*localtime(&ttime);
			search->wday=stime.tm_wday;
			search->startTime=stime.tm_hour*100+stime.tm_min;
			ttime+=event->duration;
			stime=*localtime(&ttime);
			search->stopTime=stime.tm_hour*100+stime.tm_min;
			result=BT_TRUE;
		}
		freeChannelEvents(&channelEvents);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t initSearchFromArgs(search_t *const search, vars_t *args, channelList_t *channels, context_t *ctx){
	initSearch(search);
	const char *oldSearchStr=vars_get_value(args,"oldSearchStr");
	if (oldSearchStr) parseSearch(oldSearchStr,search,channels);
	const char *arg;
	arg=vars_get_value(args,"search");
	if (arg && (!search->search || strcmp(arg,search->search)!=0)) {
		if (isFlagSet(SFI_SEARCH,search->my)) free(search->search);
		if (strchr(arg,':')){
			search->search=strdup(arg);
			char *c=search->search;
			for(c=strchr(c,':');c;c++) *c='|';
			setFlag(SFI_SEARCH,search->my);
		} else {
			search->search=(char *)arg;
			clearFlag(SFI_SEARCH,search->my);
		}
	}
	search->searchMode=vars_get_value_i(args,"searchMode");
	setOrClearFlag(vars_get_value_i(args,"useCase"),SFL_USE_CASE,search->flags);
	setOrClearFlag(vars_get_value_i(args,"compareTitle"),EFI_TITLE,search->compareFlags);
	setOrClearFlag(vars_get_value_i(args,"compareSubtitle"),EFI_SHORTDESC,search->compareFlags);
	setOrClearFlag(vars_get_value_i(args,"compareDescription"),EFI_DESC,search->compareFlags);
	if (vars_get_value_i(args,"useTime")) {
		setFlag(SFL_USE_TIME,search->flags);
		int k;
		for (k=0;k<2;k++){
			arg=vars_get_value(args,(k==0)?"startTime":"stopTime");
			if (arg) {
				int *time=(k==0)?&search->startTime:&search->stopTime;
				char *c;
				int hour=strtol(arg,&c,10);
				if (errno || hour<0 || hour>23 || c[0]!=':'){
					//TODO imprimir error
				} else {
					int min=strtol(c+1,&c,10);
					if (errno || min<0 || min>59 || c[0]){
						//TODO imprimir error
					} else {
						(*time)=hour*100+min;
					}
				} 
			}
		}
	} else {
		clearFlag(SFL_USE_TIME,search->flags);
		search->startTime=0;
		search->stopTime=2359;
	}
	setOrClearFlag(vars_get_value_i(args,"useDuration"),SFL_USE_DURATION,search->flags);
	search->minDuration=vars_get_value_i(args,"minDuration");
	search->maxDuration=vars_get_value_i(args,"maxDuration");

	search->wday=0;
	if (vars_get_value_i(args,"useWday")) {
		setFlag(SFL_USE_WDAY,search->flags); ;
		int ndays=vars_countn(args,"wday");
		if (ndays>1){
			int i,wday;
			uint wdayFlag;
			for(i=0;i<ndays;i++){
				wday=vars_geti_value_i(args,"wday",i);
				if (wday>6||wday<0){
					printMessage(ctx,"alert",tr("search.errWrongWday"),NULL,BT_FALSE);
				} else {
					wdayFlag=1<<wday;
					setFlag(wdayFlag,search->wday); 
				}
			}
			search->wday=-search->wday;
		} else {
			search->wday=vars_get_value_i(args,"wday");
		}
	} else {
		clearFlag(SFL_USE_WDAY,search->flags);
	}

	search->useChannel=vars_get_value_i(args,"useChannel");
	search->channelMin=vars_get_value_i(args,"channelMin")-1;
	search->channelMax=vars_get_value_i(args,"channelMax")-1;
	if (isFlagSet(SFI_CHANNEL_GROUP,search->my)) free(search->channelGroup);
	clearFlag(SFI_CHANNEL_GROUP,search->my);
	search->channelGroup=(search->useChannel==2)?(char *)vars_get_value(args,"channelGroup"):NULL;

	//useAsSearchTimer-start
	setOrClearFlag(vars_get_value_i(args,"useAsSearchTimer"),SFL_USE_AS_SEARCH_TIMER,search->flags);
	//action-start
	search->action=vars_get_value_i(args,"searchAction");
	//record-start
	if (isFlagSet(SFI_DIRECTORY,search->my)) free(search->directory);
	clearFlag(SFI_DIRECTORY,search->my);
	search->directory=(char *)vars_get_value(args,"directory");
	setOrClearFlag(vars_get_value_i(args,"useEpisode"),SFL_USE_EPISODE,search->flags);
	search->priority=vars_get_value_i(args,"priority");
	search->lifetime=vars_get_value_i(args,"lifetime");
	search->marginStart=vars_get_value_i(args,"marginStart");
	search->marginStop=vars_get_value_i(args,"marginStop");
	//repeats-start
	setOrClearFlag(vars_get_value_i(args,"repeatsAvoid"),SFL_AVOID_REPEATS,search->flags);
	search->allowedRepeats=vars_get_value_i(args,"repeatsAllowed");
	search->repeatsCompareFlags=0;
	setOrClearFlag(vars_get_value_i(args,"repeatsCompareTitle"),EFI_TITLE,search->repeatsCompareFlags);
	setOrClearFlag(vars_get_value_i(args,"repeatsCompareSubtitle"),EFI_SHORTDESC,search->repeatsCompareFlags);
	setOrClearFlag(vars_get_value_i(args,"repeatsCompareDescription"),EFI_DESC,search->repeatsCompareFlags);
	//repeats-end
	//record-end
	//switch-start
	search->switchMinsBefore=vars_get_value_i(args,"switchMinsBefore");
	setOrClearFlag(vars_get_value_i(args,"switchUnmuteSound"),SFL_UNMUTE_SOUND_ON_SWITCH,search->flags);
	//swit-end
	//action-end
	//useAsSearchTimer-end
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void freeSearch(search_t *const search) {
	if (isFlagSet(SFI_SEARCH_STR,search->my)) free(search->searchStr);
	if (isFlagSet(SFI_SEARCH,search->my)) free(search->search);
	if (isFlagSet(SFI_CHANNEL_GROUP,search->my)) free(search->channelGroup);
	if (isFlagSet(SFI_DIRECTORY,search->my)) free(search->directory);
	if (isFlagSet(SFI_EXT_EPG_VALUES,search->my)) free(search->extEpgValues);
	if (isFlagSet(SFI_BLACKLIST_IDS,search->my)) free(search->blacklistIds);
	initSearch(search);
}

void initSearchList(searchList_t *const list){
	list->length=0;
	list->entry=NULL;
}

void freeSearchList(searchList_t *const list){
	int i;
	for (i=0;i<list->length;i++){
		freeSearch(&(list->entry[i]));
	}
	free(list->entry);
	initSearchList(list);
}

void getSearchListHost(hostConf_t *const host, searchList_t *const searches, channelList_t const *const channels){
	char *data=execSvdrp(host,"PLUG epgsearch LSTS");
	if (data){
		char *p;
		search_t *tmp;
		for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
			if (atoi(p)==SVDRP_PLUG_DEFAULT) {
				p+=4;
				crit_goto_if((searches->entry=(search_t *)realloc(searches->entry,(++searches->length)*sizeof(search_t)))==NULL,outOfMemory);
				tmp=searches->entry+searches->length-1;
				initSearch(tmp);
				tmp->hostId=host->id;
				tmp->searchStr=strdup(p); 
				setFlag(SFI_SEARCH_STR,tmp->my);
				if (parseSearch(p,tmp,channels)){
					//TODO asignar canales
				} else {
					freeSearch(tmp); //TODO decidir cual de las dos veces
					searches->length--;
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

int compareSearchs(const search_t *sa, const search_t *sb, sortField_t sortBy, sortDirection_t sortDirection){
	if (sortDirection==SF_NONE) return 0;
	int result=0;
	int step;
	for (step=0;step<2;step++){
		switch (sortBy) {
			case SF_PRIORITY:
				result=sortDirection*(sa->priority-sb->priority);	
				if (result) return result; else sortBy=SF_NAME; break;
			case SF_LIFETIME: 
				result=sortDirection*(sa->lifetime-sb->lifetime);	
				if (result) return result; else sortBy=SF_NAME; break;
			case SF_HOST:
				result=sortDirection*(sa->hostId-sb->hostId);
				if (result) return result; else sortBy=SF_NAME; break;
			case SF_DIRECTORY:
				result=sortDirection*strcmp(sa->directory,sb->directory);
				dbg("sa.dir [%s] sb.dir [%s] result %d",sa->directory,sb->directory,result);
				if (result) return result; else sortBy=SF_NAME; break;
			case SF_NAME:
				result=sortDirection*strcmp(sa->search,sb->search);
				if (result) return result; else sortBy=SF_START; break;
			case SF_START:    
				result=sortDirection*(sa->startTime-sb->startTime);	
				if (result) return result; else sortBy=SF_NAME; break;
		}
	}
	return result;
}

void sortSearchList(searchList_t *const searches, const sortField_t sortBy, const sortDirection_t sortDirection){
	int compare(const void *a, const void *b) {
		return compareSearchs((const search_t *)a,(const search_t *)b, sortBy, sortDirection);
	}
	if (searches->length>0 && sortBy!=SF_NONE) {
		qsort(searches->entry,searches->length,sizeof(search_t),compare);
	} 
}

void getSearchList(searchList_t *const searches, channelList_t const *const channels, const sortField_t sortBy, const sortDirection_t sortDirection){
	initSearchList(searches);
	int h;
	hostConf_t *host;
	for (h=0,host=webifConf.hosts;h<webifConf.hostsLength;h++,host++){
		if (host->isVdr){
			getSearchListHost(host,searches,channels);
		}
	}
	sortSearchList(searches,sortBy,sortDirection);
}

boolean_t addSearch(context_t *ctx, hostConf_t *const host, const char *newSearchStr) {
	boolean_t result= BT_FALSE;
	if (!host || !host->isVdr) {
		if (ctx) printMessage(ctx,"alert",tr("search.create.err"),"Host nulo o no es VDR",BT_FALSE); //TODO i18n
	} else {
		char *command;
		crit_goto_if(asprintf(&command,"PLUG epgsearch NEWS %s",newSearchStr)<0,outOfMemory);
		char *data=execSvdrp(host,command);
		free(command);
		if (data){
			char *p=data;
			int code=strtol(p,&p,10);
			result=boolean(code==SVDRP_PLUG_DEFAULT);
			if (ctx && *p && *(++p)){
				printMessage(ctx,(result)?"message":"alert",tr((result)?"search.create.ok":"search.create.err"),p,BT_TRUE);
			}
			free(data);
		}
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t deleteSearch(context_t *ctx, hostConf_t *host, int id, const char *oldSearchStr) {
	boolean_t result=BT_FALSE;
	if (!host || !host->isVdr) {
		if (ctx) printMessage(ctx,"alert",tr("search.delete.err"),"Host nulo o no es VDR",BT_FALSE); //TODO i18n
	} else if (oldSearchStr==NULL) {
		if (ctx) printMessage(ctx,"alert",tr("search.delete.err"),"Faltan argumentos",BT_FALSE); //TODO i18n
	} else {
		char *searchStr=getSearchStrAt(host, id);
		if (searchStr!=NULL) {
			if (strcmp(oldSearchStr,searchStr)==0) {
				char *command=NULL;
				crit_goto_if(asprintf(&command,"PLUG epgsearch DELS %d",id)<0,outOfMemory);
				char *data=execSvdrp(host,command);
				free(command);
				if (data!=NULL){
					char *p=data;
					int code=strtol(p,&p,10);
					result=boolean(code==SVDRP_PLUG_DEFAULT);
					if (ctx && *p && *(++p)){
						printMessage(ctx,(result)?"message":"alert",tr((result)?"search.delete.ok":"search.delete.err"),p,BT_TRUE);
					}
					free(data);
				}
			} else { 
				if (ctx) printMessage(ctx,"alert",tr("search.delete.err"),"Probablemente la búsqueda ha sido modificada",BT_FALSE); //TODO i18n
			}
			free(searchStr);
		} else { 
			if (ctx) printMessage(ctx,"alert",tr("search.delete.err"),"La búsqueda no existe",BT_FALSE); //TODO i18n
		}
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t editSearch(context_t *ctx, hostConf_t *const host, int id, const char *oldSearchStr, const char *newSearchStr) {
	if (!host || !oldSearchStr || !newSearchStr){
		if (ctx) printMessage(ctx,"alert",tr("search.update.err"),"Faltan argumentos",BT_TRUE); //TODO i18n
		return BT_FALSE;
	}
	if (strcmp(oldSearchStr,newSearchStr)==0){
		if (ctx) printMessage(ctx,"alert",tr("search.update.err"),"Nada que hacer. No hay cambios",BT_TRUE); //TODO i18n
		return BT_TRUE;
	}
	boolean_t result= BT_FALSE;
	char *searchStr=getSearchStrAt(host,id);
	if (searchStr!=NULL) {
		if (strcmp(oldSearchStr,searchStr)==0) {
			char *command=NULL;
			crit_goto_if(asprintf(&command,"PLUG epgsearch EDIS %s",newSearchStr)<0,outOfMemory);
			char *data=execSvdrp(host,command);
			free(command);
			if (data!=NULL){
				char *p=data;
				int code=strtol(p,&p,10);
				result=boolean(code==SVDRP_PLUG_DEFAULT);
				if (ctx && *p && *(++p)){
					printMessage(ctx,(result)?"message":"alert",tr((result)?"search.update.ok":"search.update.err"),p,BT_TRUE);
				}
				free(data);
			} else {
				if (ctx) printMessage(ctx,"alert",tr("search.update.err"),tr("warnSvdrpConnection"),BT_TRUE);
			}
		} else {
			printMessage(ctx,"alert",tr("search.update.err"),"Búsqueda no existe",BT_FALSE);
		}
		free(searchStr);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t updateSearches(context_t *ctx, hostConf_t *host){
	if (!host){
		return BT_FALSE;
	}
	boolean_t result= BT_FALSE;
	char *command=NULL;
	char *data=execSvdrp(host,"PLUG epgsearch UPDS");
	if (data!=NULL){
		char *p=data;
		int code=strtol(p,&p,10);
		result=boolean(code==SVDRP_PLUG_DEFAULT);
		if (ctx && *p && *(++p)){
			printMessage(ctx,(result)?"message":"alert",tr((result)?"searches.update.ok":"searches.update.err"),p,BT_TRUE);
		}
		free(data);
	}
	return result;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *getSearchStrAt(hostConf_t *const host, int id) {
	char *searchStr=NULL;
	if (id>=0) {
		char *command=NULL;
		crit_goto_if(asprintf(&command,"PLUG epgsearch LSTS %d",id)<0,outOfMemory);
		char *data=execSvdrp(host,command);
		free(command);
		if (data) {
			char *p;
			for (p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
				if (atoi(p)==SVDRP_PLUG_DEFAULT) {
					p+=4;
					searchStr=strdup(p);
					break;
				}
			}
			free(data);
		}
	}
	return searchStr;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t parseSearch(const char *line, search_t *const search, channelList_t const *const channels ){
	char *c;
	int l;
	struct tm sdate;

	const char *r=line;
	char *n; //pointer to next field, only to check integrity
	int field;
	for(field=1;field<=51 && r[0];field++) {
		l=strcspn(r,":");
		errno=0;
		n=NULL;
		switch(field){
			case 1:  
				search->id=strtol(r,&n,10);
				break;
			case 2:  
				crit_goto_if((search->search=strndup(r,l))==NULL,outOfMemory);
				setFlag(SFI_SEARCH,search->my);
				break;
			case 3:  
				setOrClearFlag(strtol(r,&n,10),SFL_USE_TIME,search->flags);
				break;
			case 4:
				search->startTime = (isFlagSet(SFL_USE_TIME,search->flags) && l) ? strtol(r,&n,10) : 0;
				break;
			case 5:  
				search->stopTime = (isFlagSet(SFL_USE_TIME,search->flags) && l) ? strtol(r,&n,10) : 2359;
				break;
			case 6:  
				search->useChannel=strtol(r,&n,10);
				break;
			case 7:  
				if (search->useChannel == 0) {
					search->channelMin = 0;
					search->channelMax = 0;
				}
				else if (search->useChannel == 1) {
					int minNum=0, maxNum=0;
					int fields = sscanf(r, "%d-%d", &search->channelMin, &search->channelMax);
					if (fields == 0) { // stored with ID
						char *channelMinbuffer = NULL;
						char *channelMaxbuffer = NULL;
						int channelsFound = sscanf(r, "%a[^|:]|%a[^|:]", &channelMinbuffer, &channelMaxbuffer);
						int c;
						channel_t *channel;
						for(c=0,channel=channels->channel;c<channels->length;c++,channel++) 
							if (strcmp(channel->channelId,channelMinbuffer)==0) { 
								search->channelMin=c; 
								break;
							}
						if (channelsFound == 1) {
							search->channelMax = search->channelMin;
						} else 
							for(c=0,channel=channels->channel;c<channels->length;c++,channel++) 
								if (strcmp(channel->channelId,channelMaxbuffer)==0) { 
									search->channelMax=c; 
									break;
								}
						free(channelMinbuffer);
						free(channelMaxbuffer);
					}
				}
				else if (search->useChannel == 2){
					crit_goto_if((search->channelGroup = strndup(r,l))==NULL,outOfMemory);
					setFlag(SFI_CHANNEL_GROUP,search->my);
				}
				break;
			case 8:  
				setOrClearFlag(strtol(r,&n,10),SFL_USE_CASE,search->flags);
				break;
			case 9:  
				search->searchMode = strtol(r,&n,10);
				break;
			case 10: 
				setOrClearFlag(strtol(r,&n,10),EFI_TITLE,search->compareFlags);
				break;
			case 11: 
				setOrClearFlag(strtol(r,&n,10),EFI_SHORTDESC,search->compareFlags);
				break;
			case 12:
				setOrClearFlag(strtol(r,&n,10),EFI_DESC,search->compareFlags);
				break;
			case 13:
				setOrClearFlag(strtol(r,&n,10),SFL_USE_DURATION,search->flags);
				break;
			case 14: 
				search->minDuration = (isFlagSet(SFL_USE_DURATION,search->flags) && l) ? strtol(r,&n,10) : 0;
				break;
			case 15: 
				search->maxDuration = (isFlagSet(SFL_USE_DURATION,search->flags) && l) ? strtol(r,&n,10) : 0;
				break;
			case 16: 
				setOrClearFlag(strtol(r,&n,10),SFL_USE_AS_SEARCH_TIMER,search->flags);
				break;
			case 17:
				setOrClearFlag(strtol(r,&n,10),SFL_USE_WDAY,search->flags);
				break;
			case 18: 
				search->wday = strtol(r,&n,10);
				break;
			case 19:
				setOrClearFlag(strtol(r,&n,10),SFL_USE_EPISODE,search->flags);
				break;
			case 20:  
				crit_goto_if((search->directory = strndup(r,l))==NULL,outOfMemory);
				setFlag(SFI_DIRECTORY,search->my);
				break;
			case 21: 
				search->priority = strtol(r,&n,10);
				break;
			case 22: 
				search->lifetime = strtol(r,&n,10);
				break;
			case 23: 
				search->marginStart = strtol(r,&n,10);
				break;
			case 24: 
				search->marginStop = strtol(r,&n,10);
				break;
			case 25:
				setOrClearFlag(strtol(r,&n,10),SFL_USE_VPS,search->flags);
				break;
			case 26: 
				search->action = strtol(r,&n,10);
				break;
			case 27:
				setOrClearFlag(strtol(r,&n,10),SFL_USE_EXT_EPG_INFO,search->flags);
				break;
			case 28: 
				crit_goto_if((search->extEpgValues = strndup(r,l))==NULL,outOfMemory);
				setFlag(SFI_EXT_EPG_VALUES,search->my);
				break;
			case 29:
				setOrClearFlag(strtol(r,&n,10),SFL_AVOID_REPEATS,search->flags);
				break;
			case 30: 
				search->allowedRepeats = strtol(r,&n,10);
				break;
			case 31:
				setOrClearFlag(strtol(r,&n,10),EFI_TITLE,search->repeatsCompareFlags);
				break;
			case 32:
				setOrClearFlag(strtol(r,&n,10),EFI_SHORTDESC,search->repeatsCompareFlags);
				break;
			case 33:
				setOrClearFlag(strtol(r,&n,10),EFI_DESC,search->repeatsCompareFlags);
				break;
			case 34: 
				search->catvaluesAvoidRepeat = strtol(r,&n,10);
				break;
			case 35: 
				search->repeatsWithinDays = strtol(r,&n,10);
				break;
			case 36: 
				search->delAfterDays = strtol(r,&n,10);
				break;
			case 37:  
				search->recordingsKeep = strtol(r,&n,10);
				break;
			case 38: 
				search->switchMinsBefore = strtol(r,&n,10);
				break;
			case 39: 
				search->pauseOnNrRecordings = strtol(r,&n,10);
				break;
			case 40: 
				search->blacklistMode = strtol(r,&n,10);
				break;
			case 41: 
				if (search->blacklistMode == BLACKLIST_SELECTION && 0 /*&& !ParseBlacklistIDs(value)*/) {
					//TODO
					warn("ERROR parsing blacklist IDs");
					goto wrongFormat;
				}
				crit_goto_if((search->blacklistIds = strndup(r,l))==NULL,outOfMemory);
				setFlag(SFI_BLACKLIST_IDS,search->my);
				break;		
			case 42: 
				search->fuzzyTolerance = strtol(r,&n,10);
				break;
			case 43:
				setOrClearFlag(strtol(r,&n,10),SFL_USE_IN_FAVORITES,search->flags);
				break;
			case 44: 
				search->menuTemplate = strtol(r,&n,10);
				break;
			case 45: 
				search->delMode = strtol(r,&n,10);
				break;
			case 46: 
				search->delAfterCountRecs = strtol(r,&n,10);
				break;
			case 47: 
				search->delAfterDaysOfFirstRec = strtol(r,&n,10);
				break;
			case 48:
				search->useAsSearchTimerFrom = strtol(r,&n,10);
				break;
			case 49:
				search->useAsSearchTimerTil = strtol(r,&n,10);
				break;
			case 50:
				setOrClearFlag(strtol(r,&n,10),SFL_IGNORE_MISSING_EPG_CATS,search->flags);
				break;
			case 51:
				setOrClearFlag(strtol(r,&n,10),SFL_UNMUTE_SOUND_ON_SWITCH,search->flags);
				break;
		}
		if (errno) {
			warn(strerror(errno));
			goto wrongFormat;
		}
		if (n && n[0]!=':' && n[0]!=0) {
			goto wrongFormat;
		}
		r+=l;
		if (r[0]!=':') break;
		r++;
	}
	return BT_TRUE;
wrongFormat:
	warn("Wrong search string [%s]",line);
	return BT_FALSE;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *makeSearchStr(search_t *const search,const channelList_t *channels){
	char *searchStr;
	crit_goto_if(asprintf(&searchStr,"%d:%s:%d",(search->id>=0)?search->id:0,search->search,isFlagSet(SFL_USE_TIME,search->flags))<0,outOfMemory);
	if (isFlagSet(SFL_USE_TIME,search->flags)){
		crit_goto_if(asprintf(&searchStr,"%s:%04d:%04d",searchStr,search->startTime,search->stopTime)<0,outOfMemory);
	} else {
		crit_goto_if(asprintf(&searchStr,"%s::",searchStr)<0,outOfMemory);
	}
	crit_goto_if(asprintf(&searchStr,"%s:%d",searchStr,search->useChannel)<0,outOfMemory);
	if (search->useChannel==1){
		crit_goto_if(asprintf(&searchStr,"%s:%s",searchStr,channels->channel[search->channelMin].channelId)<0,outOfMemory);
		if (search->channelMin < search->channelMax) {
			crit_goto_if(asprintf(&searchStr,"%s|%s",searchStr,channels->channel[search->channelMax].channelId)<0,outOfMemory);
		}
	} else if (search->useChannel==2) {
		crit_goto_if(asprintf(&searchStr,"%s:%s",searchStr,(search->channelGroup)?search->channelGroup:"0")<0,outOfMemory);
	} else {
		crit_goto_if(asprintf(&searchStr,"%s:0",searchStr)<0,outOfMemory);
	}
	crit_goto_if(asprintf(&searchStr,"%s:%d:%d:%d:%d:%d:%d",
		searchStr,
		isFlagSet(SFL_USE_CASE,search->flags),
		search->searchMode,
		isFlagSet(EFI_TITLE,search->compareFlags),
		isFlagSet(EFI_SHORTDESC,search->compareFlags),
		isFlagSet(EFI_DESC,search->compareFlags),
		isFlagSet(SFL_USE_DURATION,search->flags)
	)<0,outOfMemory);
	if (isFlagSet(SFL_USE_DURATION,search->flags)){
		crit_goto_if(asprintf(&searchStr,"%s:%04d:%04d",searchStr,search->minDuration,search->maxDuration)<0,outOfMemory); 
	} else {
		crit_goto_if(asprintf(&searchStr,"%s::",searchStr)<0,outOfMemory);
	};
	crit_goto_if(asprintf(&searchStr,
		"%s:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%ld:%d:%d:%d:%d:%d:%d:%s:%d:%d:%d:%d:%d:%d:%ld:%ld:%d:%d",
		searchStr,
		isFlagSet(SFL_USE_AS_SEARCH_TIMER,search->flags),
		isFlagSet(SFL_USE_WDAY,search->flags),
		search->wday,
		isFlagSet(SFL_USE_EPISODE,search->flags),
		search->directory,
		search->priority,
		search->lifetime,
		search->marginStart,
		search->marginStop,
		isFlagSet(SFL_USE_VPS,search->flags),
		search->action,
		isFlagSet(SFL_USE_EXT_EPG_INFO,search->flags),
		isFlagSet(SFL_USE_EXT_EPG_INFO,search->flags)?search->extEpgValues:"",
		isFlagSet(SFL_AVOID_REPEATS,search->flags),
		search->allowedRepeats,
		isFlagSet(EFI_TITLE,search->repeatsCompareFlags),
		isFlagSet(EFI_SHORTDESC,search->repeatsCompareFlags),
		isFlagSet(EFI_DESC,search->repeatsCompareFlags),
		search->catvaluesAvoidRepeat,
		search->repeatsWithinDays,
		search->delAfterDays,
		search->recordingsKeep,
		search->switchMinsBefore,
		search->pauseOnNrRecordings,
		search->blacklistMode,
		(search->blacklistIds)?search->blacklistIds:"",
		search->fuzzyTolerance,
		isFlagSet(SFL_USE_IN_FAVORITES,search->flags),
		search->menuTemplate,
		search->delMode,
		search->delAfterCountRecs,
		search->delAfterDaysOfFirstRec,
		search->useAsSearchTimerFrom,
		search->useAsSearchTimerTil,
		isFlagSet(SFL_IGNORE_MISSING_EPG_CATS,search->flags), 
		isFlagSet(SFL_UNMUTE_SOUND_ON_SWITCH,search->flags)
	)<0,outOfMemory);

   return searchStr;
outOfMemory:
	crit("Out of memory");
	exit(1);
}
//------------------------------
void printSearchForm(context_t *ctx, search_t *const search, channelList_t const *const channels, const char *cssLevel){
	cfgParamConfig_t searchModeCfg={"search.mode","0",
		"search.phrase|search.allWords|search.atLeastOne|search.exactMatch|search.regex|search.fuzzy",BT_TRUE,0,NULL,NULL,BT_FALSE};
	cfgParamConfig_t checkboxCfg={"","0","0|1",BT_FALSE,0,NULL,NULL,BT_FALSE};
	cfgParamConfig_t useChannelCfg={"","0","no|interval|channel.group|onlyFTA",BT_TRUE,0,NULL,NULL,BT_FALSE};
	cfgParamConfig_t searchActionCfg={"","0","search.record|search.announce|search.switch",BT_TRUE,0,NULL,NULL,BT_FALSE};

	char *paramValue;
	struct tm sdate;
	const char *SearchEdit=tr((search->searchStr)?"search.edit":"search.add");
	hostConf_t *host=getHost(search->hostId);

	ctx_printfn(ctx,"<form id=\"searchEdit\" action=\"/searches.kl1\" method=\"post\">\n",0,1);
	ctx_printfn(ctx,"<div class=\"%s-div  formContent\">\n",0,1,cssLevel);
	ctx_printf0(ctx,"<h3 class=\"%s-top formTitle\"\">%s</h3>\n",cssLevel,tr("search.edit"));
	ctx_printfn(ctx,"<div class=\"%s\">\n",0,1,cssLevel);

	ctx_printfn(ctx,"<fieldset>\n",0,1);
	if (search->id>=0) {
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"searchId\" value=\"%d\"/>\n",search->id);
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"oldSearchStr\" value=\"%s\"/>\n",CTX_HTML_ENCODE(search->searchStr,-1));
	}
	if (webifConf.numVDRs>1){
		ctx_printfn(ctx,"<label>%s\n",0,1,"Host");
		printVDRSelect(ctx,"hostId",search->hostId);
		ctx_printfn(ctx,"</label>\n",-1,0);
	} else {
		ctx_printf0(ctx,"<input type=\"hidden\" name=\"hostId\" value=\"%d\"/>\n",search->hostId);
	}
	ctx_printf0(ctx,"<input type=\"hidden\" name=\"oldHostId\" value=\"%d\"/>\n",search->hostId);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"search\" value=\"%s\"/></label>\n",tr("search.search"),(search->search)?search->search:"");
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.mode"));
	asprintf(&paramValue,"%d",search->searchMode);
	printSelect(ctx,&searchModeCfg,NULL,"searchMode",0,paramValue);
	free(paramValue);
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.useCase"));
	printCheckbox(ctx,&checkboxCfg,NULL,"useCase",0,isFlagSet(SFL_USE_CASE,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"compareSet\">\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.compareTitle"));
	printCheckbox(ctx,&checkboxCfg,NULL,"compareTitle",0,isFlagSet(EFI_TITLE,search->compareFlags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.compareSubtitle"));
	printCheckbox(ctx,&checkboxCfg,NULL,"compareSubtitle",0,isFlagSet(EFI_SHORTDESC,search->compareFlags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.compareDescription"));
	printCheckbox(ctx,&checkboxCfg,NULL,"compareDescription",0,isFlagSet(EFI_DESC,search->compareFlags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</fieldset><!--\n",-1,0);

	ctx_printfn(ctx,"--><fieldset>\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.startFilter"));
	printCheckbox(ctx,&checkboxCfg,"startFilter","useTime",0,isFlagSet(SFL_USE_TIME,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"startFilterCfg\">\n",0,1);
	int hour,min;
	hour=search->startTime/100;
	min=search->startTime-hour*100;
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"startTime\" value=\"%02d:%02d\" size=\"5\"/></label>\n",tr("search.startAfter"),hour,min);
	hour=search->stopTime/100;
	min=search->stopTime-hour*100;
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"stopTime\" value=\"%02d:%02d\" size=\"5\"/></label>\n",tr("search.startBefore"),hour,min);
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</fieldset><!--\n",-1,0);

	ctx_printfn(ctx,"--><fieldset>\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.durationFilter"));
	printCheckbox(ctx,&checkboxCfg,"durationFilter","useDuration",0,isFlagSet(SFL_USE_DURATION,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"durationFilterCfg\">\n",0,1);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"minDuration\" value=\"%d\" size=\"3\"/></label>\n",tr("search.durationMin"),search->minDuration);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"maxDuration\" value=\"%d\" size=\"3\"/></label>\n",tr("search.durationMax"),search->maxDuration);
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</fieldset><!--\n",-1,0);

	ctx_printfn(ctx,"--><fieldset>\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.wdayFilter"));
	printCheckbox(ctx,&checkboxCfg,"wdayFilter","useWday",0,isFlagSet(SFL_USE_WDAY,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"wdayFilterCfg\">\n",0,1);
	int wdayFlag;
	int c,wday;
	for(c=0,wday=startOfWeek[langId];c<7;c++,wday=(wday+1)%7) {
		wdayFlag=1<<wday;
		ctx_printf0(ctx,
			"<label>%s"
				"<input type=\"checkbox\" name=\"wday\" value=\"%d\"%s/>"
			"</label>"
			,weekdays[langId][wday],wday,checked[(search->wday<0)?isFlagSet(wdayFlag,-search->wday):boolean(wday==search->wday)]);
	}
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</fieldset><!--\n",-1,0);

	//channel-start
	ctx_printfn(ctx,"--><fieldset>\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.channelFilter"));
	asprintf(&paramValue,"%d",search->useChannel);
	printSelect(ctx,&useChannelCfg,"channelFilter","useChannel",0,paramValue);
	free(paramValue);
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"channelFilterCfg\">\n",0,1);
	ctx_printfn(ctx,"<label id=\"channelMinLabel\">%s\n",0,1,tr("search.channelMin"));
	printChannelListSelect(ctx,NULL,"channelMin",channels,search->channelMin+1,NULL);
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<label id=\"channelMaxLabel\">%s\n",0,1,tr("search.channelMax"));
	printChannelListSelect(ctx,NULL,"channelMax",channels,search->channelMax+1,NULL);
	ctx_printfn(ctx,"</label>\n",-1,0);
	if (1) { //channelGroup
		channelGroupList_t groups;
		getChannelGroupList(host,&groups,channels);
		ctx_printfn(ctx,"<label id=\"channelGroupLabel\">%s\n",0,1,tr("search.channel.group"));
		if (groups.length>0) {
			printChannelGroupListSelect(ctx,"channelGroup",&groups,search->channelGroup);
			//TODO que hacer si no esta en la lista
		} else {
			ctx_printf0(ctx,"<input type=\"text\" name=\"channelGroup\" size=\"5\" value=\"%s\"/>",(search->channelGroup)?search->channelGroup:"");
		} 
		ctx_printfn(ctx,"</label>\n",-1,0);
		freeChannelGroupList(&groups);
	}
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</fieldset><!--\n",-1,0);
	//channel-end

	//useAsSearchTimer-start
	ctx_printfn(ctx,"--><fieldset>\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.useAsSearchTimer"));
	printCheckbox(ctx,&checkboxCfg,"useAsSearchTimer","useAsSearchTimer",0,isFlagSet(SFL_USE_AS_SEARCH_TIMER,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"useAsSearchTimerCfg\">\n",0,1);
	//action-start
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.action"));
	asprintf(&paramValue,"%d",search->action);
	printSelect(ctx,&searchActionCfg,"searchAction","searchAction",0,paramValue);
	free(paramValue);
	ctx_printfn(ctx,"</label>\n",-1,0);

	//record-start
	ctx_printfn(ctx,"<fieldset id=\"recordCfg\">\n",0,1);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"directory\" value=\"%s\" size=\"50\" /></label>\n"
		,tr("search.directory"),(search->directory)?search->directory:"%Title%~%Subtitle%");
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.useEpisode"));
	printCheckbox(ctx,&checkboxCfg,"useEpisode","useEpisode",0,isFlagSet(SFL_USE_EPISODE,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"priority\" value=\"%d\" size=\"3\" /></label>\n",tr("search.priority"),search->priority);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"lifetime\" value=\"%d\" size=\"3\" /></label>\n",tr("search.lifetime"),search->lifetime);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"marginStart\" value=\"%d\" size=\"3\" /></label>\n",tr("search.margin.before"),search->marginStart);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"marginStop\" value=\"%d\" size=\"3\" /></label>\n",tr("search.margin.after"),search->marginStop);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"delAfterDays\" value=\"%d\" size=\"2\" /></label>\n",tr("search.delAfterDays"),search->delAfterDays);

	//repeats-start
	//ctx_printfn(ctx,"<fieldset id=\"repeats\">\n",0,1);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.repeats.avoid"));
	printCheckbox(ctx,&checkboxCfg,"repeatsAvoid","repeatsAvoid",0,isFlagSet(SFL_AVOID_REPEATS,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<fieldset id=\"repeatsAvoidCfg\">\n",0,1);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"repeatsAllowed\" value=\"%d\" size=\"2\"/></label>\n"
		,tr("search.repeats.maxAllowed"),search->allowedRepeats);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.repeats.compareTitle"));
	printCheckbox(ctx,&checkboxCfg,NULL,"repeatsCompareTitle",0,isFlagSet(EFI_TITLE,search->repeatsCompareFlags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.repeats.compareSubtitle"));
	printCheckbox(ctx,&checkboxCfg,NULL,"repeatsCompareSubtitle",0,isFlagSet(EFI_SHORTDESC,search->repeatsCompareFlags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.repeats.compareDescription"));
	printCheckbox(ctx,&checkboxCfg,NULL,"repeatsCompareDescription",0,isFlagSet(EFI_DESC,search->repeatsCompareFlags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	//ctx_printfn(ctx,"</fieldset>\n",-1,0);
	//repeats-end

	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	//record-end

	//announce-start
	/*
	ctx_printfn(ctx,"<fieldset id=\"announceCfg\">\n",0,1);
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	*/
	//announce-stop

	//switch-start
	ctx_printfn(ctx,"<fieldset id=\"switchCfg\">\n",0,1);
	ctx_printf0(ctx,"<label>%s<input type=\"text\" name=\"switchMinsBefore\" value=\"%d\" size=\"2\"/></label>\n"
		,tr("search.switch.minsBefore"),search->switchMinsBefore);
	ctx_printfn(ctx,"<label>%s\n",0,1,tr("search.switch.unmuteSound"));
	printCheckbox(ctx,&checkboxCfg,"switchUnmuteSound","repeatsAvoid",0,isFlagSet(SFL_UNMUTE_SOUND_ON_SWITCH,search->flags)?"1":"0");
	ctx_printfn(ctx,"</label>\n",-1,0);
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	//switch-end

	//action-end
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</fieldset><!--\n",-1,0);
	//useAsSearchTimer-end

	ctx_printfn(ctx,"--><fieldset class=\"controls ajaxButtonsHolder\">\n",0,1);
	ctx_printf0(ctx,
		"<button id=\"confirm\" class=\"confirm ui-state-default button-i-t\" name=\"a\" type=\"submit\" value=\"%d\" >"
			"<div><span class=\"ui-icon ui-icon-check\">&nbsp;</span>%s</div>"
		"</button>\n"
		,PA_ADD,tr("accept"));
	ctx_printf0(ctx,
		"<button id=\"searchDelete\" class=\"delete searchDelete ui-state-default button-i-t\" name=\"a\" type=\"submit\" value=\"%d\" >"
			"<div><span class=\"ui-icon ui-icon-trash\">&nbsp;</span>%s</div>"
		"</button>\n"
		,PA_DELETE,tr("search.delete"));
	ctx_printfn(ctx,"</fieldset>\n",-1,0);
	ctx_printfn(ctx,"</div>\n",-1,0); //[cssLevel]
	ctx_printfn(ctx,"</div>\n",-1,0); //[cssLevel]-div
	ctx_printfn(ctx,"</form>\n",-1,0);
}

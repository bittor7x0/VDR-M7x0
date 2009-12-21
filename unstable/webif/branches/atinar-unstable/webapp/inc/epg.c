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

const char *gridDateFmtI = "%Y-%m-%d %H:%M";
const char *gridDateFmtO = "%Y%m%d%H%M";

void initEpgEvent(epgEvent_t * const event){
	event->my=0; //strdup nothing
	event->id=0;
	event->title=NULL;
	event->desc=NULL;
	event->shortdesc=NULL;
	event->start=0;
	event->duration=0;
}

void initEpgEventFromArgs(context_t *ctx, epgEvent_t * const event, vars_t *args){
	initEpgEvent(event);
	if (vars_countn(args, "title")>0) {
		if (ctxGetRequestParamAsCpy(ctx,&event->title,args,"title")) setFlag(EFI_TITLE,event->my);
	}
	if (vars_countn(args, "shortdesc")>0) {
		if (ctxGetRequestParamAsCpy(ctx,&event->shortdesc,args,"shortdesc")) setFlag(EFI_SHORTDESC,event->my);
	}
	if (vars_countn(args, "desc")>0) {
		if (ctxGetRequestParamAsCpy(ctx,&event->desc,args,"desc")) setFlag(EFI_DESC,event->my);
	}
}

void freeEpgEvent(epgEvent_t * const event){
	if (isFlagSet(EFI_TITLE,event->my))
		free(event->title);
	if (isFlagSet(EFI_DESC,event->my))
		free(event->desc);
	if (isFlagSet(EFI_SHORTDESC,event->my))
		free(event->shortdesc);
	initEpgEvent(event);
}

void initChannelEvents(channelEvents_t * const list){
	memset(list,0,sizeof(channelEvents_t));
	//list->entry=NULL;
	//list->length=0;
	//list->channel=NULL;
}
void freeChannelEvents(channelEvents_t * const list){
	int i;
	epgEvent_t *entry;
	for (i=0,entry=list->entry;i<list->length;i++,entry++){
		freeEpgEvent(entry);
	}
	free(list->entry);
	initChannelEvents(list);
}
void initEvents(events_t * const list){
	memset(list,0,sizeof(events_t));
	//list->entry=NULL;
	//list->length=0;
	//list->start=list->end=0;
}
void freeEvents(events_t * const list){
	int i;
	channelEvents_t *entry;
	for (i=0,entry=list->entry;i<list->length;i++,entry++){
		freeChannelEvents(entry);
	}
	free(list->entry);
	initEvents(list);
}

void parseEventLine(char c, char *s, epgEvent_t *const event){
	int l=strcspn(s,"\n\r");
	switch (c) {
		case 'E': {
				int k;
				for(k=0;k<5;k++) {
					s+=strspn(s," ");
					l=strcspn(s," ");
					switch (k) {
						case 0: event->id     =strtoul(s,NULL,10); break;
						case 1: event->start   =strtol(s,NULL,10); break;
						case 2: event->duration=strtol(s,NULL,10); break;
						case 3: event->tableId =strtol(s,NULL,16); break;
						case 4: event->version =strtol(s,NULL,16); break;
					}
					s+=l;
				}
				return;
			}
		case 'T':
			crit_goto_if((event->title=strndup(s,l))==NULL,outOfMemory);
			setFlag(EFI_TITLE,event->my);
			return;
		case 'S': 
			crit_goto_if((event->shortdesc=strndup(s,l))==NULL,outOfMemory);
			setFlag(EFI_SHORTDESC,event->my);
			return;
		case 'D':
			crit_goto_if((event->desc=strndup(s,l))==NULL,outOfMemory);
			setFlag(EFI_DESC,event->my);
			return;
		case 'X':
			//TODO
			return;
		default:
			return;
	}
outOfMemory:
	crit("Out of memory");
	exit(1);
}

char *parseChannelEvents(char *data, channelEvents_t * const channelEvents){
	const char *nl="\n\r";
	char *p,c;
	epgEvent_t *event=NULL;

	p=data;
	while (p && *p){
		int code = strtol(p,&p,10);
		if (code==SVDRP_RCRD && p[0]=='-') {
			p++;
			c=p[0];
			if (channelEvents!=NULL && strchr("ETSDX",c)){ //lineas del evento
				if (p[0]=='E'){ //inicio del evento
					crit_goto_if((channelEvents->entry=(epgEvent_t *)realloc(channelEvents->entry,(++channelEvents->length)*sizeof(epgEvent_t)))==NULL,outOfMemory);
					event=channelEvents->entry+channelEvents->length-1;
					initEpgEvent(event);
				}
				if (event!=NULL){
					p+=2;
					parseEventLine(c,p,event);
				}
			} else if (c=='e'){ //fin de evento
				event=NULL;
			} 
			p+=strcspn(p,nl);
			if (*p) p+=strspn(p,nl);
			if (c=='c'){ //fin de canal
				return p;
			}
		} else {
			break;
		}
	}
	return NULL;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

/*
* Secuencia de lineas
*			215-C : inicio de canal
*			215-E : inicio de evento
*			215-e : fin de evento
*			215-c : fin de canal
*			221   : cierre de conexion
*/
void parseEvents(char *data, events_t * const events, channelList_t const * const channels){
	const char *nl="\n\r";
	char *p,*l;
	int i,k;
	channelEvents_t *channelEvents;
	channel_t *channel;
	epgEvent_t *event;

	events->length=channels->length;
	crit_goto_if((events->entry=(channelEvents_t *)malloc(events->length*sizeof(channelEvents_t)))==NULL,outOfMemory);
	for(i=0,channelEvents=events->entry;i<events->length;i++,channelEvents++){
		initChannelEvents(channelEvents);
	}
	l=data;
	while (l && *l){
		int code = strtol(l,&p,10);
		if (code==SVDRP_RCRD && p[0]=='-') {
			p++;
			if (p[0]=='C') { //inicio de canal
				//seleccionar la channelEvents que corresponda al canal
				channelEvents=NULL;
				channel=NULL;
				event=NULL;
				p+=2;
				k=strcspn(p," ");
				for(i=0,channel=channels->channel;i<channels->length;i++,channel++) {
					if (strncmp(channel->channelId,p,k)==0) {
						channelEvents=events->entry+i;
						channelEvents->channelNum=channel->channelNum;
						break;
					}
				}
				l+=strcspn(l,nl);
				if (*l) l+=strspn(l,nl);
			} else if ((l=parseChannelEvents(l,channelEvents))!=NULL) {
				channelEvents=NULL;
			}
		} else {
			break;
		}
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getChannelEvents(hostConf_t *vdrHost, channelEvents_t * const channelEvents){
	if (channelEvents->channelNum<1){
		warn("channelEvents->channelNum==%d", channelEvents->channelNum);
		return;
	}
	char *cmd;
	switch(channelEvents->dumpMode){
		case dmBetween:
			crit_goto_if(asprintf(&cmd,"LSTE %d between %d %d",channelEvents->channelNum, channelEvents->start, channelEvents->end)<1,outOfMemory);
			break;
		case dmNowNext:
			crit_goto_if(asprintf(&cmd,"LSTE %d nownext",channelEvents->channelNum)<1,outOfMemory);
			break;
		case dmByIds:
			crit_goto_if(asprintf(&cmd,"LSTE %d withid %d",channelEvents->channelNum,channelEvents->eventId)<0,outOfMemory);
			break;
		default:
			return;
	}
	channelEvents->hostId=vdrHost->id;
	char *data=execSvdrp(vdrHost,cmd);
	free(cmd);
	if (data!=NULL){
		char *l=data;
		char *p;
		while (l && *l) {
			int code = strtol(l,&p,10);
			if (code==SVDRP_RCRD && p[0]=='-') {
				p++;
				if (p[0]!='C'){
					l=parseChannelEvents(l,channelEvents);
				} else {
					l+=strcspn(l,"\n\r");
					if (*l) l+=strspn(l,"\n\r");
				}
			} else {
				break;
			}
		}
		free(data);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getEvents(hostConf_t *vdrHost, events_t * const events, channelList_t const * const channels) {
	if (channels->length<1){
		return;
	}
	char *cmd;
	switch(events->dumpMode){
		case dmBetween:
			if (events->channelNum>0){
				crit_goto_if(asprintf(&cmd,"LSTE %d between %d %d",events->channelNum, events->start, events->end)<1,outOfMemory);
			} else {
				crit_goto_if(asprintf(&cmd,"LSTE between %d %d",events->start, events->end)<1,outOfMemory);
			}
			break;
		case dmNowNext:
			if (events->channelNum>0){
				crit_goto_if(asprintf(&cmd,"LSTE %d nownext",events->channelNum)<1,outOfMemory);
			} else {
				crit_goto_if(asprintf(&cmd,"LSTE nownext")<1,outOfMemory);
			}
			break;
		case dmByIds:
			crit_goto_if(asprintf(&cmd,"LSTE %d withid %d",events->channelNum,events->eventId)<0,outOfMemory);
			break;
		default:
			return;
	}
	events->hostId=vdrHost->id;
	char *data=execSvdrp(vdrHost,cmd);
	if (data!=NULL){
		parseEvents(data,events,channels);
		free(data);
	}
	free(cmd);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getEpgGridLimits(const char *arg_cstart,time_t *start, time_t *end){
	time_t now=time(NULL);
	tzset();
	struct tm sdate;
	char cstart[17]="0000-00-00 00:00";
	if (arg_cstart!=NULL){
		int i;
		const char *o;
		char *d;
		const char *l=arg_cstart+strlen(arg_cstart);
		for(i=0,o=arg_cstart,d=cstart;i<5 && (*o) && o<l;i++){
			int n=(i)?2:4;
			int l2=strlen(o);
			if (l2<n) d+=(n-l2); else l2=n;
			strncpy(d,o,l2);
			dbg("cstart [%s]",cstart);
			d+=n+1;
			o+=n;
		}
		dbg("Fecha convertida de [%s] a [%s]",arg_cstart,cstart);
		if (strptime(cstart,gridDateFmtI,&sdate)==NULL){
			warn("No se ha interpretado el argumento %s con el patron %s.", cstart, gridDateFmtI);
			arg_cstart=NULL;
		}
	}
	if (arg_cstart==NULL){
		sdate=*localtime(&now);
	}
	sdate.tm_sec=0;
	sdate.tm_min-=sdate.tm_min%15;
	sdate.tm_isdst=-1;
	sdate.tm_zone=NULL;
	*start=mktime(&sdate);
	*end=*start+3*60*60;
}

void printChangeDateButton(context_t *ctx,char *ctime,boolean_t isPrevious,int hours,boolean_t isLi){
	const char *class=(isPrevious)?"previous":"next";
	if (isLi) ctx_printf(ctx,"<li class=\"%s\">\n",class); inctab(ctx);
	ctx_printfn(ctx,"<a class=\"change ui-state-default ui-corner-all\" href=\"/epgGrid.kl1?start=%s\" title=\"%s%dh\">\n",0,1,ctime,(isPrevious)?"-":"+",hours);
	ctx_printf0(ctx,"<span class=\"ui-icon ui-icon-circle-triangle-%s\">%s</span>\n",(isPrevious)?"w":"e",tr(class));
	ctx_printfn(ctx,"</a>\n",-1,0);
	if (isLi) ctx_printfn(ctx,"</li>",-1,0); //previus
}

const char * ctxChannelFilename(context_t *ctx, const char *channelName){
	int l;
	const char *s;
	char *d;
	const char encode[]="+&";
	l=0;
	for (s=channelName;*s;s++){
		l+=(*s>127 || strchr(encode,*s))?3:1;
	}
	CTX_CHK_BUFFER(l+1);
	for (s=channelName,d=ctx->buffer;*s;s++){
		if (*s==' '){
			*d='_';
			d++;
		} else if (*s>127 || strchr(encode,*s)) {
			sprintf(d, "@%02X", (unsigned char)s[0]);
			d+=3;
		} else {
			*d=*s;
			d++;
		}
	}
	*d=0;
	return ctx->buffer;
}

void printEpgGrid(context_t *ctx, events_t * const events, channelList_t * const channels, timerList_t * const timers){
	channelEvents_t *channelEvents;
	epgEvent_t *event;
	const channel_t *channel;
	int i1,i2;
	time_t eventStart, eventEnd, eventDuration;
	char cdatehour[13];
	char cstart[13];
	char cend[13];
	char cmark[6];
	const char *Epg=tr("epg");
	const char *TimerCreate=tr("timer.create");
	const char *TimerEdit=tr("timer.edit");
	const char *SearchCreate=tr("search.create");

	time_t gridStart=events->start;
	time_t gridEnd=events->end;
	time_t gridDuration=gridEnd-gridStart;
	time_t aTime;
	time_t now=time(NULL);
	struct tm sstart=*localtime(&gridStart);
	strftime(cstart,13,gridDateFmtO,&sstart);
	strftime(cend,13,gridDateFmtO,localtime(&gridEnd));

	int totalWidth=webifConf.tvScheduleGridWidth;
	int logoWidth=CHAN_LOGO_W;
	int channelWidth=totalWidth-logoWidth;

	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"<div id=\"grid-div\" class=\"level3-div\" style=\"width:%dpx\">\n",0,1,totalWidth);
		ctx_printfn(ctx,"<h3 id=\"grid-top\" class=\"level3-top\">\n",0,1);
	}
	CTX_CHK_BUFFER(30);
	strftime(ctx->buffer,30,"%A, %x",&sstart);
	ctx_printfn(ctx,"<span id=\"gridDate\">\n",0,1);
	int hours;
	hours=24;
	if (gridStart>now){ //one day before
		aTime=gridStart-hours*60*60;
		strftime(cdatehour,13,gridDateFmtO,localtime(&aTime));
		printChangeDateButton(ctx,cdatehour,BT_TRUE,hours,BT_FALSE);
	}
	ctx_printf0(ctx,"%s\n",ctx->buffer);
	if (1) {//one day after
		aTime=gridStart+24*60*60;
		strftime(cdatehour,13,gridDateFmtO,localtime(&aTime));
		printChangeDateButton(ctx,cdatehour,BT_FALSE,hours,BT_FALSE);
	}
	ctx_printfn(ctx,"</span>\n",-1,0);
	if (ctx->isAjaxRequest){
		ctx_printf0(ctx,"<span id=\"datepickerValue\">%.8s</span>\n",cstart);
	} else {
		ctx_printf0(ctx,"<input type=\"text\" id=\"datepicker\" value=\"%.8s\" size=\"8\"/>\n",cstart);
		ctx_printfn(ctx,"</h3>\n",-1,0);
		ctx_printfn(ctx,"<div id=\"grid\" class=\"level3\">\n",0,1);
		ctx_printf0(ctx,"<ul id=\"timeTable\">");
		ctx_printf(ctx,"<li class=\"container\">\n"); inctab(ctx);
	}

	ctx_printf0(ctx,"<ul id=\"timeTableControls\" class=\"controls\">");
	hours=gridDuration/(60*60);
	if (gridStart>now){
		aTime=gridStart-gridDuration;
		strftime(cdatehour,13,gridDateFmtO,localtime(&aTime));
		printChangeDateButton(ctx,cdatehour,BT_TRUE,hours,BT_TRUE);
	}
	printChangeDateButton(ctx,cend,BT_FALSE,hours,BT_TRUE);
	ctx_printf(ctx,"</ul>\n"); //controls

	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"</li>",-1,0); //container
		ctx_printf(ctx,"<li class=\"container\">\n"); inctab(ctx);
	}

	ctx_printf0(ctx,"<ul id=\"timeTableMarks\" class=\"marks\" style=\"left:%dpx;width:%dpx;\">",logoWidth,channelWidth);
	time_t mark;
	time_t step=60*60;
	boolean_t last=BT_FALSE;

	int left,width;
	for (i1=0,mark=gridStart;mark<gridEnd;i1++){
		strftime(cmark,6,"%H:%M",localtime(&mark));
		left=((mark-gridStart)*channelWidth)/gridDuration;
		ctx_printf(ctx,"<li id=\"timemark%d\" class=\"mark\" style=\"left:%dpx\">\n",i1,left); inctab(ctx);
		if (i1==0){
			ctx_printf0(ctx,"<input type=\"text\" value=\"%s\" id=\"timepicker\" size=\"5\"/>\n",cmark);
		} else {
			ctx_printf0(ctx,"<span>%s</span>\n",cmark);
		}
		ctx_printfn(ctx,"</li>",-1,0); //mark
		if (mark+step<gridEnd){
			mark+=step;
		} else if (last){
			break;
		} else {
			mark+=step*75/100;
			last=BT_TRUE;
		}
	}
	ctx_printf(ctx,"</ul>\n"); //marks

	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"</li>",-1,0); //container
		ctx_printf(ctx,"</ul>\n"); //timeTable
	}

	printTimersDiv(ctx,"timers",timers,events->start,events->end);

	ctx_printf0(ctx,"<ul id=\"channels\">");
	for(i1=0,channelEvents=events->entry;i1<events->length;i1++,channelEvents++){
		if (channelEvents->length>0){
			channel=channels->channel+i1;
			ctx_printf(ctx,"<li id=\"c-%d\" class=\"channel\"><!--\n",channel->channelNum); inctab(ctx);
			ctx_printfn(ctx,"--><div class=\"logo\">\n",0,1);
			ctx_printfn(ctx,"<a class=\"%s\" href=\"epg.kl1?channelNum=%d\" title=\"%s\">\n",0,1,(webifConf.noLogos)?"nologo":"logo",channel->channelNum,Epg);
			if (webifConf.noLogos){
				ctx_printf0(ctx,"<span class=\"nologo\">%s</span>",ctxChannelDisplayName(ctx,channel));
			} else {
				ctx_printf0(ctx,"<img id=\"logo_%s\" alt=\"%s\" src=\"/www2/images/logos/%s.png\"/>\n"
					,channel->channelId,ctxChannelDisplayName(ctx,channel),CTX_URL_ENCODE(channel->channelName,-1,"-_./"));
			}
			ctx_printfn(ctx,"</a>",-1,0);
			ctx_printfn(ctx,"</div><!--\n",-1,0); //logo
			ctx_printfn(ctx,"--><div class=\"channelContent\" style=\"width:%dpx\">\n",0,1,channelWidth);
			ctx_printf0(ctx,"<ul class=\"channelEvents\">");
			time_t previousEventEnd=gridStart;
			for(i2=0,event=channelEvents->entry;i2<channelEvents->length;i2++,event++){
				eventStart=(event->start>gridStart)?event->start:gridStart;
				eventEnd=(event->start+event->duration); if (eventEnd>gridEnd) eventEnd=gridEnd;
				eventDuration=eventEnd-eventStart;
				left=((eventStart-gridStart)*channelWidth)/gridDuration;
				width=(eventDuration*channelWidth)/gridDuration;
				ctx_printf(ctx,"<li id=\"e-%d\" class=\"event\" style=\"left:%dpx;width:%dpx\">\n",event->id,left,width); inctab(ctx);
				printEvent(ctx,event,channelEvents->channelNum,(width<55),TimerCreate,SearchCreate);
				ctx_printfn(ctx,"</li>",-1,0); //event
				previousEventEnd=eventEnd;
			}
			ctx_printf(ctx,"</ul>\n",-1,0); //channelEvents
			ctx_printfn(ctx,"</div><!--\n",-1,0); //channelContent
			ctx_printfn(ctx,"--></li>",-1,0); //channel
		}
	}
	ctx_printf(ctx,"</ul>\n"); //channels

	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"</div>\n",-1,0); //grid
		ctx_printfn(ctx,"</div>\n",-1,0); //grid-div
	}
}

void printChannelEpg(context_t *ctx, const char *id, hostConf_t *host, const int channelNum, timerList_t *const timers ){
	char *cmd;
	char * data;
	char * p;
	epgEvent_t event;
	time_t next_time=0;
	int previous_yday=-1;
	int pcbegin;
	int pcend;
	const char *TimerCreate=tr("timer.create");
	const char *TimerEdit=tr("timer.edit");
	const char *SearchCreate=tr("search.create");

	initEpgEvent(&event);
	CTX_CHK_BUFFER(20);
	sprintf(ctx->buffer,"LSTE %d",channelNum);
	data=execSvdrp(host,ctx->buffer);

	int retCode;
	int cont=0;
	ctx_printfn(ctx,"<div id=\"%s\" class=\"level3\">\n",0,1,id);
	for(p=strtok(data,"\r\n");p!=0;p=strtok(0,"\r\n")) {
		retCode=strtol(p,&p,10);
		if (retCode==SVDRP_RCRD){
			p++;
			if (p[0]=='e') {
				next_time=event.start+event.duration;
				if (event.start>0) {
					cont++;
					struct tm sdate=*localtime(&event.start);
					if (previous_yday!=sdate.tm_yday) {
						previous_yday=sdate.tm_yday;
						if (cont>1){
							ctx_printfn(ctx,"</div>\n",-1,0); //level4
							ctx_printfn(ctx,"</div>\n",-1,0); //level4-div
						}
						ctx_printfn(ctx,"<div class=\"level4-div\">\n",0,1);
						ctx_printfn(ctx,"<div class=\"level4-top\">%s %s</div>\n",0,1
							,weekdays[langId][sdate.tm_wday],formatDate(&sdate,0));
						ctx_printfn(ctx,"<div class=\"level4\">\n",0,1);
					}
					ctx_printfn(ctx,"<div class=\"event\">\n",0,1);
					ctx_printfn(ctx,"<div class=\"timers\">\n",0,1);
					printTimerBars(ctx,timers,channelNum,event.start,event.duration,TimerEdit,BT_FALSE);
					ctx_printfn(ctx,"</div>\n",-1,0); //timers
					printEvent(ctx,&event,channelNum,BT_FALSE,TimerCreate,SearchCreate);	
					ctx_printfn(ctx,"</div>\n",-1,0); //event
				} 
				freeEpgEvent(&event);
			} else {
				parseEventLine(p[0],p+2,&event);
			}
		}
	}
	freeEpgEvent(&event);
	free(data);
	if (cont){
		ctx_printfn(ctx,"</div>\n",-1,0); //level4
		ctx_printfn(ctx,"</div>\n",-1,0); //level4-div
	} else {
		printMessage(ctx,"alert",tr("warnNoEPG"),NULL,BT_FALSE);
	}
	ctx_printfn(ctx,"</div>\n",-1,0); //level3
}

void printTimersDiv(context_t *ctx, const char *id, timerList_t * const timers, const time_t start, const time_t end){
	if (!ctx->isAjaxRequest) ctx_printfn(ctx,"<div id=\"timers-div\" style=\"padding-left:%dpx;\">\n",0,1,CHAN_LOGO_W);
	ctx_printfn(ctx,"<div id=\"%s\">\n",0,1,id);
	printTimerBars(ctx,timers,0,start,end-start,tr("timer.edit"),BT_TRUE);
	ctx_printfn(ctx,"</div>\n",-1,0); //div.id
	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"</div>\n",-1,0); //timers-div
	}
}

void printEventDesc(context_t *ctx, char * const desc, boolean_t encode){
	if (!desc || strlen(desc)==0) return;
	const char * const lineDelim="|";
	char *p=desc;
	int l;
	while(p[0]){
		l=strcspn(p,lineDelim);
		if (l==0) break;
		if (encode) {
			ctx_printf0(ctx,"%s\n",CTX_HTML_ENCODE(p,l));
		} else {
			ctx_printf0(ctx,"<p>%.*s</p>\n",l,p);
		}
		p+=l;
		p+=strspn(p,lineDelim);
	}
}

void printEvent(context_t *ctx, epgEvent_t * const event, const int channelNum, boolean_t compact, const char *const TimerCreate, const char *const SearchCreate){
	struct tm startt,endt;
	long int end_time = event->start+event->duration;
	startt=*localtime(&event->start);
	endt=*localtime(&end_time);
	ctx_printfn(ctx,"<div class=\"eventContent %s\">\n",0,1,(compact)?" compact":"");
	if (compact){
		ctx_printf0(ctx,"<span class=\"more titleLink\">+</span>\n");
	}
	if (TimerCreate || SearchCreate){
		ctx_printfn(ctx,"<ul class=\"controls\"><!--\n",0,1);
		if (TimerCreate) {
			ctx_printf0(ctx,
				"--><li class=\"control\">"
					"<a class=\"timerCreate button-i ui-state-default\" href=\"/timers.kl1?a=%d&amp;channelNum=%d&amp;eventId=%d\" title=\"%s\">"
						"<span class=\"ui-icon ui-icon-rec\">%s</span>"
					"</a>"
				"</li><!--\n"
				,PA_TIMER_CREATE_FROM_EVENT,channelNum,event->id,TimerCreate,TimerCreate);
		}
		if (SearchCreate) {
			ctx_printf0(ctx,
				"--><li class=\"control\">"
					"<a class=\"searchCreate button-i ui-state-default\" href=\"/searches.kl1?a=%d&amp;channelNum=%d&amp;eventId=%d\" title=\"%s\">"
						"<span class=\"ui-icon ui-icon-search\">%s</span>"
					"</a>"
				"</li><!--\n"
				,PA_SEARCH_CREATE_FROM_EVENT,channelNum,event->id,SearchCreate,SearchCreate);
		}
		ctx_printfn(ctx,"--></ul><!--\n",-1,0); //controls
	}
	ctx_printf0(ctx,"-->");
	ctx_printf(ctx,tr("fromToFmt"),startt.tm_hour,startt.tm_min,endt.tm_hour,endt.tm_min,event->duration/60);
	ctx_printf0(ctx,"<span class=\"title titleLink\">%s</span>\n",event->title);
	if (event->shortdesc && strlen(event->shortdesc)>0){
		ctx_printf0(ctx,"<div class=\"shortdesc\">%s</div>\n",event->shortdesc);
	}
	if (event->desc && event->desc[0]){
		ctx_printfn(ctx,"<div class=\"desc\">\n",0,1);
		printEventDesc(ctx,event->desc,BT_FALSE);
		ctx_printfn(ctx,"</div>\n",-1,0);
	}
	ctx_printfn(ctx,"</div>\n",-1,0);
}


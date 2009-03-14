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

#define _XOPEN_SOURCE
#define _GNU_SOURCE

//svdrp_parse stellt Funktionen bereit, um die Zeilen einer Antwort zu parsen
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>

#include "svdrp_parse.h"

#ifdef FOR_BUSYBOX
	#include "scan_config.c"
#endif

char vdr_setup[256];

int marginStart=5*60;
int marginStop=15*60;

void parse_215E(char * line, unsigned int * event_id, long int * start_time, int * duration, int * table_id, int * version) {
	char * r=line;
	int k;
	int l;
	
	for(k=0;k<5;k++) {
		r+=strspn(r," ");
		l=strcspn(r," ");
		switch (k) {
			case 0: if (event_id  ) *event_id =strtoul(r,NULL,10); break;
			case 1: if (start_time) *start_time=strtol(r,NULL,10); break;
			case 2: if (duration  ) *duration  =strtol(r,NULL,10); break;
			case 3: if (table_id  ) *table_id  =strtol(r,NULL,16); break;
			case 4: if (version   ) *version   =strtol(r,NULL,16); break;
		}
		r+=l;
	}
}

boolean_t makeTimerStr(char **timerStr, int active, int channelNum, time_t start, time_t stop, int priority, int lifetime, const char * title) {
	time_t ts=start;//-marginStart;
	time_t te=stop;//+marginStop;
	struct tm t1=*localtime(&ts);
	struct tm t2=*localtime(&te);
	return (asprintf(timerStr,"%d:%d:%d-%02d-%02d:%02d%02d:%02d%02d:%d:%d:%s:"
		,active
		,channelNum
		,1900+t1.tm_year,t1.tm_mon+1,t1.tm_mday
		,t1.tm_hour,t1.tm_min
		,t2.tm_hour,t2.tm_min
		,priority
		,lifetime
		,title	)<1) ?  BT_FALSE : BT_TRUE;
}

void parseRec(char * line, recEntry_t * const recording){
	char * r=line;
	int l;
	struct tm timeptr;
	
	r+=strspn(r," ");
	recording->ID=(int)strtol(r,&r,10);
	r+=strspn(r," ");
	r=strptime(r,"%d.%m.%y %H:%M",&timeptr);
	if (r==NULL){ 
		printf("Error converting recording date!\n");
		recording->title=NULL;
		recording->path=NULL;
		return;
	}
	timeptr.tm_sec=0;
	timeptr.tm_isdst=-1;
	recording->start=mktime(&timeptr);
	recording->seen=(r[0]==' ');
	r++;
	r+=strspn(r," ");
	l=strcspn(r,"/\n\r");
	recording->title=strndup(r,l);
	recording->direct=(strchr(recording->title,'@')==NULL) ? 0 : 1;
	recording->cut=(strchr(recording->title,'%')==NULL) ? 0 : 1;
	r+=l;
	if (r[0]=='/') {    //Requires vdr patched to include path
		l=strcspn(r,"\n\r");
		recording->path=strndup(r,l);
	}
	else {
		recording->path=NULL;
	}
	return;
}

void parseTimer(const char * line, timerEntry_t * const timer ){
	char * r;
	char * c;
	char * h;
	int l;
	int k;
	char s[50];
	struct tm timeptr;

	//TODO check EINVAL, ERANGE
	timer->active=strtol(line,&r,10); 
	r++;
	timer->channelNum=strtol(r,&r,10); 
	r++;
	l=strcspn(r,":");
	if (l==7) {
		strncpy(timer->reg_timer,r,l);
		timer->reg_timer[l]='\0';
		timer->type=TT_REGULAR;
		strcpy(s,"1970-01-02");
		time_t ttDate=time(NULL);
		for (k=0;k<7;k++) {
			timeptr=*localtime(&ttDate);
			if (timer->reg_timer[(timeptr.tm_wday+6)%7]!='-') {
				strftime(s,sizeof(s)-1,"%Y-%m-%d",&timeptr);
				break;
			}
			ttDate+=86400;
		}
	} else {
		strncpy(s,r,l);
		s[l]='\0';
		timer->type=TT_ONE_TIME;
	}
	r+=l+1;
	l=4;
	strcat(s," ");
	h = s+strlen(s); // Hour position
	for (k=0;k<2;k++){
		strncpy(h,r,2);
		h[2]=':';
		strncpy(h+3,r+2,2);
		h[5]='\0';
		r+=l+1;
		if (!strptime(s,"%Y-%m-%d %H:%M",&timeptr)) { 
			warn("Error converting timer date \"%s\"!\n",s); 
		}
		timeptr.tm_sec=0; 
		timeptr.tm_isdst=-1;
		if (k==0) {
			timer->start=mktime(&timeptr);
		} else {
			timer->stop=mktime(&timeptr);
		}
	}
	if ( timer->stop < timer->start) {
		timer->stop+=24*60*60;
	}
	timer->priority=strtol(r,&r,10); r++;
	timer->lifetime=strtol(r,&r,10); r++;
	l=strcspn(r,":");

	timer->title=strndup(r,l);
	//TODO parse aux
}

void parseChannel(char * line, channelEntry_t * channel) {
	char *r, *r2;
	int k,l,l2;

	r=line;
	channel->channelNum=strtol(r,&r,10);
	r+=strspn(r," ");
	char *tmp[13];
	for(k=0;k<13;k++) {
		l=strcspn(r,":");
		switch(k){
			case  0:
				r2=r;
				l2=strcspn(r2,",;:");
				channel->channelName=strndup(r2,l2);
				r2+=l2+1;
				if (r2[-1]!=':') {
					l2=strcspn(r2,":");
				} else {
					l2=0;
				}
				channel->multiplexName=strndup(r2,l2);
				break;
			case  1: channel->frequency=strtol(r,NULL,10); break;
			case  2: channel->parameter=strndup(r,l); break;
			case  3: channel->source=strndup(r,l);    break;
			case  5: channel->vpid=strtol(r,NULL,10); break;
			case  8: channel->caid=strtol(r,NULL,10); break;
			case  9: channel->sid =strtol(r,NULL,10); break;
			case 10: channel->nid =strtol(r,NULL,10); break;
			case 11: channel->tid =strtol(r,NULL,10); break;
			case 12: channel->rid =strtol(r,NULL,10); break;
		}
		r+=l+1;
	}
	if( channel->rid ){
		if (asprintf(&channel->channelId,"%s-%d-%d-%d-%d",channel->source,channel->nid,channel->tid,channel->sid,channel->rid)<0) {
			channel->channelId=NULL;
			//TODO warn
		}
	} else {
		if (asprintf(&channel->channelId,"%s-%d-%d-%d",channel->source,channel->nid,channel->tid,channel->sid)<0) {
			channel->channelId=NULL;
			//TODO warn
		}
	}
}


void get_config_info() {

#ifdef FOR_BUSYBOX

	//***** WICHTIG: Tabelle ist im Feld 'name' alphabetisch zu sortieren,
	//***** da eine binäre Suche verwendet wird.

	t_scan_config config_param[] =
	{
	// cnt, name,			default
		{ 0, "RELEVANT_VDR_CONFIG",	0 },
		{ 0, "TIMEZONE",		"CET-1CEST,M3.5.0,M10.5.0/03:00:00" },

		{0,0,0} // END OF TABLE
	};

	scan_config(config_param);
	setenv("TZ",config_param[CONFIG_TIMEZONE].value,1);

	if (config_param[CONFIG_RELEVANT_VDR_CONFIG].value)
	{
		snprintf(vdr_setup,sizeof(vdr_setup),"%s/setup.conf",
		config_param[CONFIG_RELEVANT_VDR_CONFIG].value);

		FILE *f = fopen(vdr_setup,"r");
		if (f) {
			while (!feof(f)) {
				char buffer[256], param[sizeof(buffer)];
				fgets(buffer,sizeof(buffer),f);
				buffer[strlen(buffer)-1] = '\0';
				const int i=strcspn(buffer,"=");
				strncpy(param,buffer,i);
				if (!strcmp(param,"MarginStart ")) {
					margin_start=strtol(buffer+i+2,NULL,10)*60;
				} else if (!strcmp(param,"MarginStop ")) {
					marginStop=strtol(buffer+i+2,NULL,10)*60;
				}
			}
			fclose(f);
		}
	}

#endif // FOR_BUSYBOX
}


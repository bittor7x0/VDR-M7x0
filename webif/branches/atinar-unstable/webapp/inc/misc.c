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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <locale.h>
#include <u/libu.h>
#include <klone/ioprv.h>

#include "misc.h"
#include "i18n.h"
#include "conf.h"

int isM740AV=BT_FALSE;
int isM750S=BT_FALSE;
int isM750C=BT_FALSE;
int boxSysType=BT_FALSE;
const int sysNone=0;
const int sys740av=1;
const int sys750s=2;
const int sys750c=3;

const int httpPort=80;

const char *checked[2]={""," checked=\"checked\""};
const char *selected[2]={""," selected=\"selected\""};
const char *arStr[7]={"1.gif","43.png","169.png","1.gif","43.png","169.png","1.gif"}; //TODO mover a css
const char *cssSortClass[]={"sortdesc","sortnone","sortasc"};
const char *classCurrent[2]={""," class=\"act\""};

sortField_t sortBy;
sortDirection_t sortDirection;
pageNumber_t currentPage;
pageAction_t currentAction;

//TODO mover a conf.c
char server_ip[16];
uint16_t server_port;

char myip[16]="";

boolean_t parseRequestStr(request_t *request, char ** pathStr, char ** queryStr) {
	*pathStr=NULL; 
	*queryStr=NULL;
	if (request==NULL){
		return BT_FALSE;
	}
	const char *r=request_get_client_request(request);
	int l;
	r+=strcspn(r," ");
	r+=strspn(r," ");
	l=strcspn(r," ?");
	(*pathStr)=strndup(r,l);
	if (r[l]=='?'){
		r+=l+1;
		r+=strcspn(r," ");
		(*queryStr)=strndup(r,l);
	}
	return BT_TRUE;
}

char * strcatEx(char ** dest, const char * s) {
	int size=0;
	
	if (*dest==NULL) { *dest=malloc(strlen(s)+2); strcpy(*dest,""); }
	size=strlen(*dest)+strlen(s);
	
	char * tmp =(char *)realloc(*dest,size+2);
	if (!tmp) {
		exit(1);
	}
	*dest=tmp;

	strcat(*dest,s);
	return *dest;
}

boolean_t isDST(time_t * aTime) {
	struct tm t=*localtime(aTime);
	return boolean(t.tm_isdst>0);
}

boolean_t sameDay(time_t oneDate,time_t anotherDate) {
	struct tm sone=*localtime(&oneDate);
	struct tm sanother=*localtime(&anotherDate);
	return boolean((sone.tm_year==sanother.tm_year) && (sone.tm_yday==sanother.tm_yday));
}

boolean_t sameString(const char * s1, const char * s2) {
	return boolean( (s1==NULL && s2==NULL) || (strcmp(s1,s2)==0) );
}

boolean_t sameInt(const int i1, const int i2) {
	return boolean(i1==i2);
}

boolean_t sameIntEx(const char * s, const int i) {
	return boolean(strtol(s,NULL,10)==i);
}

boolean_t fileExists(const char * fileName) {
	struct stat status;

	if (stat(fileName, &status)< 0) { return BT_FALSE; }
	return boolean(S_ISREG(status.st_mode));
}

boolean_t dirExists(const char * dirName) {
	struct stat status;
	
	if (( stat (dirName, &status)) < 0) { return BT_FALSE; }
	return boolean(S_ISDIR(status.st_mode));
}

boolean_t vdrRunning() {
	if (fileExists("/var/run/runvdr.pid")) {
		FILE *f = fopen("/var/run/runvdr.pid","r");
		if (f) {
			char pid[10]="";
			char proc[30]="";
			fgets(pid,sizeof(pid),f);
			fclose(f);
			if (pid[strlen(pid)-1]=='\n') { pid[strlen(pid)-1]='\0'; }
			sprintf(proc,"/proc/%s/cmdline",pid);
			return fileExists(proc);
		}
	} else {
		return BT_FALSE;
	}
}

void config(session_t *session, request_t *request) {
	header_t *header = request_get_header(request);
	if (header_get_field(header,"Accept-Language")!=NULL) {
		field_t *field=header_get_field(header,"Accept-Language");
		strncpy(acceptedLang,field->value,2);
		acceptedLang[2]='\0';
	}
	readWEBIFCONF();
	if ((webifConf.langID>-1) && (webifConf.langID<I18NNUM)) {
		langID=webifConf.langID;  
	} else {
		int i=0;
		if ((langID<0) || (langID>=I18NNUM)) {
			for (i=0;i<I18NNUM;i++) {
				if (strcmp(acceptedLang,alpha2[i])==0) {
					langID=i;
				}
			} 
		}
		if ((langID<0) || (langID>=I18NNUM)) {
			langID=0;
		}
	}
	setlocale(LC_ALL,locale[langID]);
// get systemtype from /etc/systemtype
	boxSysType=sysNone;
	isM740AV=BT_TRUE; 
	isM750S=BT_FALSE; 
	isM750C=BT_FALSE;
	FILE *f = fopen("/etc/systemtype","r");
	if (f) {
		char str[10];
		fgets(str,sizeof(str),f);
		fclose(f);
		if (!strcmp(str,"m740")) {
			boxSysType=sys740av;
			isM740AV=BT_TRUE;
		} else if (!strcmp(str,"m750s")) {
			boxSysType=sys750s;
			isM750S=BT_TRUE;
		}
	}
	
	set_server_address(session,server_ip,&server_port);
	whatsmyip(myip);
}

const char * sortClass(sortField_t sf){
	return cssSortClass[((sf==sortBy)?sortDirection:SD_NONE)+1];
}

char * htmlEncode(const char * const s){
	char * eS=NULL;
	if ((s) && strlen(s)>0) {
		int l=strlen(s);
		eS=malloc(2*l);
		if (eS && u_htmlncpy(eS,s,l,HTMLCPY_ENCODE)!=-1) {
			return eS;
		}
	}
	return NULL;
}

short htoi(unsigned char c) {
	return (c>='0' && c<='9') ? c-'0' : (c>='a' && c<='f') ? c-'a'+10 : (c>='A' && c<='F') ? c-'A'+10 : 0;
}

void vdrDecode(char *dst, char *src){
	char *s, *d;
	s=src;
	d=dst;
	while (s[0]!=0) {
		if (s[0]=='#' && isxdigit(s[1]) && isxdigit(s[2])) {
			*d= htoi(s[1]) << 4 | htoi(s[2]);
			s+=2;
		} else {
			*d=(*s=='_') ? ' ' : *s;
		}
		s++;
		d++;
	}
	d[0]=0;
}

boolean_t makeTime(time_t *time, const char * date, const char * hour, const char * min ){
	boolean_t result=BT_FALSE;
	struct tm timePtr;
	char * s;
	if (asprintf(&s,"%s %s:%s:00",date,hour,min)>0) {
		if (strptime(s,"%Y-%m-%d %H:%M:%S",&timePtr)) {
			timePtr.tm_wday=0; 
			timePtr.tm_yday=0;  
			timePtr.tm_isdst=-1;
			*time=mktime(&timePtr);
			result=BT_TRUE;
		}
		free(s);
	}
	return result;
}

#define USE_XHTML

void printDoctypeOpenHtml(io_t *out){
#define USE_XHTML
#ifdef USE_XHTML
	io_printf(out,
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\" lang=\"%s\">\n"
		,alpha2[langID],alpha2[langID]
	);
#else
	io_printf(out,
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
"<html lang=\"%s\">\n"
		,alpha2[langID]
	);
#endif
}

void printXhtmlHead(response_t *response,io_t *out, const char *title, const char *headExtra, ...){
	response_set_content_type(response,"text/html; charset=ISO-8859-1");
	response_set_field(response,"Content-Style-Type","text/css");
	response_set_field(response,"Content-Script-Type", "text/javascript");
	printDoctypeOpenHtml(out);
#ifdef USE_XHTML
	const char *lct=" /";
#else
	const char *lct="";
#endif

	io_printf(out,
"<head>\n"
"	<title>%s - open7x0 VDR-FW</title>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/screen.css\" media=\"screen\"%s>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/print.css\" media=\"print\"%s>\n"
"	<!--[if IE 5.0]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/screen-ie50.css\" media=\"screen\"%s>\n"
"	<![endif]-->\n"
"	<!--[if IE 5.5]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/screen-ie55.css\" media=\"screen\"%s>\n"
"	<![endif]-->\n"
"	<!--[if IE 6]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/screen-ie6.css\" media=\"screen\"%s>\n"
"	<![endif]-->\n"
"	<!--[if IE 7]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/screen-ie7.css\" media=\"screen\"%s>\n"
"	<![endif]-->\n"
"	<script type=\"text/javascript\" src=\"/js/jquery-1.2.6.min.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"/js/jquery.hoverIntent.min.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"/js/superfish.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"/js/supersubs.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"/js/common.js\"></script>\n"
"	<script type=\"text/javascript\">\n"
"		$(function(){\n"
"			initPage(%d);\n"
"		});\n"
"	</script>\n"
		,title
		,lct
		,lct
		,lct
		,lct
		,lct
		,lct
		,currentPage
	);
	if (headExtra) {
		va_list ap;
		va_start(ap, headExtra); /* init variable list arguments */
		io_vprintf(out, headExtra, ap);
		va_end(ap);
	}
	io_printf(out,
"</head>\n"
"<body>\n"
"<div id=\"page-div\">\n"
"	<div id=\"page-top\">\n"
"		<h1>%s</h1>\n"
"	</div>\n"
"	<div id=\"page\">\n"
		,title
	);
}

void printMenu(io_t *out){
	int i;
	int clock_id;
#ifdef WEBIF_CRONO
	clock_id=CLOCK_REALTIME;
	struct timespec cronostart,cronoend;
	clock_gettime(clock_id,&cronostart);
#endif
	const char *Summary=tr("summary");
	io_printf(out,
"		<ul id=\"menu\" class=\"sf-menu\">\n"
	);
	io_printf(out,
"			<li%s><a href=\"index.kl1\">%s</a></li>\n"
		,classCurrent[boolean(currentPage==PN_INDEX)]
		,tr("start")
	);
	io_printf(out,
"			<li%s><a href=\"program.kl1\">%s</a></li>\n"
		,classCurrent[boolean(currentPage==PN_PROGRAMS)]
		,tr("schedule")
	);
	io_printf(out,
"			<li%s>\n"
"				<a href=\"channels.kl1\">%s</a>\n"
"				<ul>\n"
"					<li%s><a href=\"channels.kl1\">%s</a></li>\n"
"					<li%s><a href=\"watchit.kl1\">%s</a></li>\n"
"					<li><a href=\"playlistch.kl1\">%s</a></li>\n"
"				</ul>\n"
"			</li>\n"
		,classCurrent[boolean(currentPage==PN_CHANNELS)]
		,tr("channels")
			,classCurrent[boolean(currentPage==PN_CHANNELS)]
			,Summary
			,classCurrent[boolean(currentPage==PN_WATCHIT)]
			,tr("channelWatch")
			,tr("playlistDownload")
	);
	io_printf(out,
"			<li%s>\n"
"				<a href=\"timers.kl1\">%s</a>\n"
"				<ul>\n"
"					<li%s><a href=\"timers.kl1\">%s</a></li>\n"
"					<li%s><a href=\"timers.kl1?a=%d\">%s</a></li>\n"
"				</ul>\n"
"			</li>\n"
		,classCurrent[boolean(currentPage==PN_TIMERS)]
		,tr("timers")
			,classCurrent[boolean((currentPage==PN_TIMERS) && (currentAction!=PA_EDIT))]
			,Summary
			,classCurrent[boolean((currentPage==PN_TIMERS) && (currentAction==PA_EDIT))]
			,PA_EDIT,tr("timerCreate")
	);
	io_printf(out,
"			<li%s>\n"
"				<a href=\"browse.kl1\">%s</a>\n"
"				<ul>\n"
"					<li%s><a href=\"browse.kl1\">%s</a></li>\n"
"					<li%s><a href=\"recordings.kl1?sort=%d&amp;direction=%d\">%s (%s)</a></li>\n"
"					<li%s><a href=\"recordings.kl1?sort=%d&amp;direction=%d\">%s (%s)</a></li>\n"
"				</ul>\n"
"			</li>\n"
		,classCurrent[boolean(currentPage==PN_RECORDINGS || currentPage==PN_BROWSE)]
		,tr("recordings")
			,classCurrent[boolean(currentPage==PN_BROWSE)]
			,tr("browse")
			,classCurrent[boolean(currentPage==PN_RECORDINGS && sortBy==SF_TITLE)]
			,SF_TITLE,SD_ASC,Summary,tr("title")
			,classCurrent[boolean(currentPage==PN_RECORDINGS && sortBy==SF_START)]
			,SF_START,SD_DESC,Summary,tr("date")
	);
	/* TODO activar cuando realmente haga algo
	io_printf(out,
"			<li%s>\n"
"				<a href=\"settings.kl1\">%s</a>\n"
"				<ul>\n"
		,classCurrent[boolean(currentPage==PN_SETTINGS)]
		,tr("setup")
	);
	for (i=0;i<numOfConfFiles;i++) {
			io_printf(out,
"					<li><a href=\"settings.kl1?show=%s\">%s</a></li>\n"
				,confFileProperties[i].name,confFileProperties[i].longName
			);
			//TODO ,classCurrent[boolean((showWhat!=NULL) && (!strcmp(showWhat,confFileProperties[i].name)))]

	}
	io_printf(out,
"				</ul>\n"
"			</li>\n"
	);
	*/
	io_printf(out,
"			<li%s>\n"
"				<a href=\"view.kl1\">%s</a>\n"
"				<ul>\n"
		,classCurrent[boolean(currentPage==PN_FILEVIEW)]
		,tr("fileView")
	);
	for(i=0;i<maxFileMapping;i++) {
		if (fileExists(fileMapping[i].file)) {
			io_printf(out,
"					<li><a href=\"view.kl1?action=%d&amp;fileNum=%d\">&bull;&nbsp;%s</a></li>\n"
					,PA_SHOW,i,tr(fileMapping[i].i18nKey));
		}
	}
	/*
	io_printf(out,
"					<li>"
						"<a href=\"view.kl1?action=%d\">&bull;&nbsp;%s</a>"
					"</li>\n"
		,PA_DOWNLOAD_ALL,tr("downloadAll")
	);
	*/
	io_printf(out,
"				</ul>\n"
"			</li>\n"
	);
	//a.target is deprecated in html 4.01:
	io_printf(out,
"			<li>\n"
"				<a href=\"#\">%s</a>\n"
"				<ul>\n"
"					<li><a class=\"newWindow\" href=\"http://www.open7x0.org/\">open7x0.org</a></li>\n"
"					<li><a class=\"newWindow\" href=\"http://vdr-m7x0.foroactivo.com.es/\">vdr-m7x0.foroactivo.com.es</a></li>\n"
"				</ul>\n"
"			</li>\n"
"		</ul>\n"
		,tr("links")
	);
#ifdef WEBIF_CRONO
	clock_gettime(clock_id,&cronoend);
	info("printMenu:time ellapsed %d secs, %f microsecs", cronoend.tv_sec-cronostart.tv_sec, (double)(cronoend.tv_nsec-cronostart.tv_nsec)/1000000.0);
#endif
}

void printFooter(io_t *out){
	io_printf(out,
"	</div>\n"
"	<div id=\"page-bottom\">\n"
"		<p>(C) 2006 open7x0-team</p>\n"
"		<p>(C) 2008 <a href=\"mailto:atinar1@hotmail.com\">atinar</a></p>\n"
"	</div>\n"
"</div>\n"
"</body>\n"
"</html>\n"
	);
}


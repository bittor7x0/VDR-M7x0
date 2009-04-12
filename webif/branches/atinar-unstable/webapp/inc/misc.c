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
#include "svdrp_comm.h"

const char *checked[2]={""," checked=\"checked\""};
const char *selected[2]={""," selected=\"selected\""};
const char *videoTypeStr[7]={"unknown","sd43","sd169","sd","hd43","hd169","hd"}; //See videoType_t
const char *cssSortClass[]={"sortdesc","sortnone","sortasc"};
const char *classCurrent[2]={""," class=\"act\""};
const char *classActive[2]={" class=\"inactive\""," class=\"active\""};

systemType_t systemType=ST_UNKNOWN;
sortField_t sortBy;
sortDirection_t sortDirection;
pageNumber_t currentPage;
pageAction_t currentAction;

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

void config(session_t *session, request_t *request) {
	header_t *header = request_get_header(request);
	if (header_get_field(header,"Accept-Language")!=NULL) {
		field_t *field=header_get_field(header,"Accept-Language");
		strncpy(acceptedLang,field->value,2);
		acceptedLang[2]='\0';
	}
	readWebifConf();
	langId=webifConf.langId;  
	if ((langId<0) || (langId>=I18NNUM)) {
		int i=0;
		for (i=0;i<I18NNUM;i++) {
			if (strcmp(acceptedLang,alpha2[i])==0) {
				langId=i;
				break;
			}
		} 
	}
	if ((langId<0) || (langId>=I18NNUM)) {
		langId=0;
	}
	setlocale(LC_ALL,locale[langId]);
	//----System type----
	systemType=ST_UNKNOWN;
	FILE *f = fopen("/etc/systemtype","r");
	if (f) {
		char str[10];
		fgets(str,sizeof(str),f);
		fclose(f);
		if (strncmp(str,"m740",4)==0) {
			systemType=ST_M740AV;
		} else if (strncmp(str,"m750s",5)==0) {
			systemType=ST_M750S;
		} else if (strncmp(str,"m750c",5)==0) {
			systemType=ST_M750C;
		}
	}
	setSvdrpServerAddress(session);
}

const char * sortClass(sortField_t sf){
	return cssSortClass[((sf==sortBy)?sortDirection:SD_NONE)+1];
}

short htoi(unsigned char c) {
	return (c>='0' && c<='9') ? c-'0' : (c>='a' && c<='f') ? c-'a'+10 : (c>='A' && c<='F') ? c-'A'+10 : 0;
}

/*
 * Decode special chars as encoded by VDR: '#xx'
 */
void vdrDecode(char *dst, char *src){
	char *s, *d;
	s=src;
	d=dst;
	while (s[0]) {
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

void initHtmlDoc(response_t *response,io_t *out){
	response_set_content_type(response,"text/html; charset=ISO-8859-1");
	response_set_field(response,"Content-Style-Type","text/css");
	response_set_field(response,"Content-Script-Type","text/javascript");
	io_printf(out,
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\" lang=\"%s\">\n"
		,alpha2[langId],alpha2[langId]
	);
}

void initHtmlPage(response_t *response,io_t *out, const char *title, const char *headExtra, ...){
	initHtmlDoc(response,out);
	const char * www=(webifConf.useExternalWwwFolder)?"/www2":"";
	io_printf(out,
"<head>\n"
"	<title>%s - open7x0 VDR-FW</title>\n"
		,title
	);
	io_printf(out,
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen.css\" media=\"screen\" />\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/print.css\" media=\"print\" />\n"
"	<!--[if IE 5.0]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie50.css\" media=\"screen\" />\n"
"	<![endif]-->\n"
"	<!--[if IE 5.5]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie55.css\" media=\"screen\" />\n"
"	<![endif]-->\n"
"	<!--[if IE 6]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie6.css\" media=\"screen\" />\n"
"	<![endif]-->\n"
"	<!--[if IE 7]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie7.css\" media=\"screen\" />\n"
"	<![endif]-->\n"
"	<!--[if IE 8]>\n"
"	<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie8.css\" media=\"screen\" />\n"
"	<![endif]-->\n"
	,www
	,www
	,www
	,www
	,www
	,www
	,www
	);
	io_printf(out,
"	<script type=\"text/javascript\" src=\"%s/js/jquery-1.3.2.min.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"%s/js/jquery-ui-1.7.min.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"%s/js/jquery.hoverIntent.min.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"%s/js/superfish.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"%s/js/supersubs.js\"></script>\n"
"	<script type=\"text/javascript\" src=\"%s/js/common.js\"></script>\n"
	,www
	,www
	,www
	,www
	,www
	,www
	);
	io_printf(out,
"	<script type=\"text/javascript\">\n"
"		$(function(){\n"
"			initPage(%d);\n"
"		});\n"
"	</script>\n"
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
	if (!webifConf.configChangeDisabled){
		io_printf(out,
"			<li%s>\n"
"				<a href=\"settings.kl1\">%s</a>\n"
"				<ul>\n"
			,classCurrent[boolean(currentPage==PN_SETTINGS)]
			,tr("setup")
		);
		for (i=0;i<cfgFileLength;i++) {
				io_printf(out,
"					<li><a href=\"settings.kl1?cfgFileId=%d\">%s</a></li>\n"
						,i,tr(cfgFile[i].i18nKey)
				);
		}
		io_printf(out,
"				</ul>\n"
"			</li>\n"
		);
	}
	if (!webifConf.configViewDisabled){
		io_printf(out,
"			<li%s>\n"
"				<a href=\"view.kl1\">%s</a>\n"
"				<ul>\n"
			,classCurrent[boolean(currentPage==PN_FILEVIEW)]
			,tr("fileView")
		);
		for(i=0;i<fileMappingLength;i++) {
			if (fileExists(fileMapping[i].fileName)) {
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
	}
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
}

void finishHtmlPage(io_t *out){
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


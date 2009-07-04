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

webifState_t webifState = {
	PN_INDEX,
	PA_NOACTION,
	SF_START,
	SD_ASC,
};

const char *checked[2]={""," checked=\"checked\""};
const char *selected[2]={""," selected=\"selected\""};
const char *videoTypeStr[7]={"unknown","sd43","sd169","sd","hd43","hd169","hd"}; //See videoType_t
const char *cssSortClass[]={"sortdesc","sortnone","sortasc"};
const char *classCurrent[2]={""," class=\"act\""};
const char *classActive[2]={" class=\"inactive\""," class=\"active\""};
const char *tabs="\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

systemType_t systemType=ST_UNKNOWN;

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
}

short htoi(unsigned char c) {
	return (c>='0' && c<='9') ? c-'0' : (c>='a' && c<='f') ? c-'a'+10 : (c>='A' && c<='F') ? c-'A'+10 : 0;
}

/*
 * Decode special chars as encoded by VDR: '#xx'
 */
void vdrDecode(char *dst, char *src, int l){
	char *s, *d;
	s=src;
	d=dst;
	while (*s && l>0) {
		if (s[0]=='#' && isxdigit(s[1]) && isxdigit(s[2])) {
			*d= htoi(s[1]) << 4 | htoi(s[2]);
			s+=3;
			l-=3;
		} else {
			*d=(*s=='_') ? ' ' : (*s=='/') ? '~' : *s;
			s++;
			l--;
		}
		d++;
	}
	d[0]=0;
}


void vdrEncode(char *dst, char *src, int l){
	char *s, *d;
	s=src;
	d=dst;
	while (*s && l>0) {
		switch (*s) {
			case '!': case '@': case '$': case '%': case '&': case '(': case ')':
			case '+': case ',': case '-': case ';': case '=': 
			case '0' ... '9'  : case 'a' ... 'z'  : case 'A' ... 'Z': 
			case 'ä': case 'Ä': case 'ö': case 'Ö': case 'ü': case 'Ü': case 'ß':
				*d=*s;
				d++;
				break;
			case ' ': 
				*d = '_'; 
				d++;  
				break;
			case '~': 
				*d = '/'; 
				d++; 
				break;
			default:
				sprintf(d, "#%02X", (unsigned char)s[0]);
				d+=3;
		}
		s++;
		l--;
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

void returnHttpNoContent(response_t *response){
	response_set_status(response,HTTP_STATUS_NO_CONTENT);
	response_print_header(response);
}	

int initHtmlDoc(response_t *response,io_t *out){
	response_set_content_type(response,"text/html; charset=ISO-8859-1");
	response_set_field(response,"Content-Style-Type","text/css");
	response_set_field(response,"Content-Script-Type","text/javascript");
	io_printf(out,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
	io_printf(out,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\" lang=\"%s\">\n",alpha2[langId],alpha2[langId]);
	return 1;
}

int initHtmlPage(response_t *response,io_t *out, const char *title, const char *headExtra, ...){
	int ntabs=initHtmlDoc(response,out);
	const char * www=(webifConf.useExternalWwwFolder)?"/www2":"";
	io_printf(out,TABS "<head>\n" NTABS(,ntabs++));
	io_printf(out,TABS "<title>%s</title>\n" NTABS(,ntabs),title);
	io_printf(out,TABS "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=Edge\" />\n" NTABS(,ntabs));
	io_printf(out,TABS "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen.css\" media=\"screen\" />\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/print.css\" media=\"print\" />\n" NTABS(,ntabs),www);
	const char *ie[]={"6","7","8"};
	int v;
	for (v=0;v<(sizeof(ie)/sizeof(char*));v++){
		io_printf(out,TABS "<!--[if IE %s]>\n" NTABS(,ntabs++),ie[v]);
		io_printf(out,TABS "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie%s.css\" media=\"screen\" />\n" NTABS(,ntabs),www,ie[v]);
		io_printf(out,TABS "<![endif]-->\n" NTABS(,--ntabs));
	}

	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/jquery-1.3.2.min.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/jquery-ui-1.7.min.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/jquery.hoverIntent.min.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/superfish.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/supersubs.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/common.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<!--[if lte IE7]>\n" NTABS(,ntabs++));
	/*
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/IE8.js\"></script>\n" NTABS(,ntabs),www);
	*/
	io_printf(out,TABS "<script type=\"text/javascript\" src=\"%s/js/jquery.iefixbuttons-0.3.js\"></script>\n" NTABS(,ntabs),www);
	io_printf(out,TABS "<script type=\"text/javascript\">\n" NTABS(,ntabs));
	io_printf(out,TABS "	$(function(){$('form').ieFixButtons()});\n" NTABS(,ntabs));
	io_printf(out,TABS "</script>\n" NTABS(,ntabs));
	io_printf(out,TABS "<![endif]-->\n" NTABS(,--ntabs));

	io_printf(out,TABS "<script type=\"text/javascript\">\n" NTABS(,ntabs++));
	io_printf(out,TABS "webif.state={'currentPage':%d,'currentAction':%d};\n",
		ntabs,tabs,webifState.currentPage,webifState.currentAction);
	io_printf(out,
		TABS "$(function(){\n"
		TABS "webif.defaultPageInit()\n"
		TABS "});\n" 
		NTABS(,ntabs) 
		NTABS(,ntabs+1) 
		NTABS(,ntabs));
	io_printf(out,TABS "</script>\n" NTABS(,--ntabs));
		
	if (headExtra) {
		va_list ap;
		va_start(ap, headExtra); /* init variable list arguments */
		io_vprintf(out, headExtra, ap);
		va_end(ap);
	}
	io_printf(out,TABS "</head>\n" NTABS(,--ntabs));
	io_printf(out,TABS "<body id=\"body-p%d-a%d\">\n" NTABS(,ntabs++),webifState.currentPage,webifState.currentAction);
	io_printf(out,TABS "<div id=\"page-div\">\n" NTABS(,ntabs++));
	io_printf(out,TABS "<div id=\"page-top\">\n" NTABS(,ntabs++));
	io_printf(out,TABS "<h1>%s</h1>\n" NTABS(,ntabs),title);
	io_printf(out,TABS "</div>\n" NTABS(,--ntabs)); //page-top
	io_printf(out,TABS "<div id=\"page\">\n" NTABS(,ntabs++));
	return ntabs;
}

#define CURRENT_PAGE(pageNumber) boolean(webifState.currentPage==pageNumber)
#define CURRENT_ACTION(action) boolean(webifState.currentAction==action)
#define SORT_BY(sortField) boolean(webifState.sortBy==sortField)

void printMenu(io_t *out,int ntabs){
	int i;
	int clock_id;
	const char *Summary=tr("summary");
	io_printf(out,TABS "<ul id=\"menu\" class=\"sf-menu\"><li%s><a href=\"index.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_INDEX)],tr("start"));

	io_printf(out,TABS "</li><li%s><a href=\"program.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_PROGRAMS)],tr("schedule"));

	io_printf(out,TABS "</li><li%s><a href=\"channels.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_CHANNELS)],tr("channels"));
	io_printf(out,TABS "<ul><li%s><a href=\"channels.kl1\">%s</a>\n"
		NTABS(,++ntabs),classCurrent[CURRENT_PAGE(PN_CHANNELS)],Summary);
	io_printf(out,TABS "</li><li%s><a href=\"watchit.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_WATCHIT)],tr("channelWatch"));
	io_printf(out,TABS "</li><li><a href=\"playlistch.kl1?type=%d\">%s</a>\n"
		NTABS(,ntabs),webifConf.playlistType,tr("channel.playlist.download"));
	io_printf(out,TABS "</li></ul>\n" NTABS(,ntabs--));

	io_printf(out,TABS "</li><li%s><a href=\"timers.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_TIMERS)],tr("timers"));
	io_printf(out,TABS "<ul><li%s><a href=\"timers.kl1\">%s</a>\n"
		NTABS(,++ntabs),classCurrent[boolean(CURRENT_PAGE(PN_TIMERS) && !CURRENT_ACTION(PA_EDIT))],tr("timers"));
	io_printf(out,TABS "</li><li%s><a href=\"timers.kl1?a=%d\">%s</a>\n"
		NTABS(,ntabs),classCurrent[boolean(CURRENT_PAGE(PN_TIMERS) && CURRENT_ACTION(PA_EDIT))],PA_EDIT,tr("timer.create"));
	io_printf(out,TABS "</li><li%s><a href=\"searches.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[boolean(CURRENT_PAGE(PN_SEARCHES) && !CURRENT_ACTION(PA_EDIT))],tr("searches"));
	io_printf(out,TABS "</li><li%s><a href=\"searches.kl1?a=%d\">%s</a>\n"
		NTABS(,ntabs),classCurrent[boolean(CURRENT_PAGE(PN_SEARCHES) && CURRENT_ACTION(PA_EDIT))],PA_EDIT,tr("search.create"));
	io_printf(out,TABS "</li></ul>\n" NTABS(,ntabs--));
		
	io_printf(out,TABS "</li><li%s><a href=\"browse.kl1\">%s</a>\n"
		NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_RECORDINGS) || CURRENT_PAGE(PN_BROWSE)],tr("recordings"));
	io_printf(out,TABS "<ul><li%s><a href=\"browse.kl1\">%s</a>\n"
		NTABS(,++ntabs),classCurrent[CURRENT_PAGE(PN_BROWSE)],tr("browse"));
	io_printf(out,TABS "</li><li%s><a href=\"recordings.kl1?sort=%d&amp;direction=%d\">%s (%s)</a>\n"
		NTABS(,ntabs),classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && SORT_BY(SF_TITLE))],SF_TITLE,SD_ASC,Summary,tr("title"));
	io_printf(out,TABS "</li><li%s><a href=\"recordings.kl1?sort=%d&amp;direction=%d\">%s (%s)</a>\n"
		NTABS(,ntabs),classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && SORT_BY(SF_START))],SF_START,SD_DESC,Summary,tr("date"));
	io_printf(out,TABS "</li></ul>\n" NTABS(,ntabs--));
		
	if (!webifConf.configChangeDisabled){
		io_printf(out,TABS "</li><li%s><a href=\"settings.kl1\">%s</a>\n"
			NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_SETTINGS)],tr("setup"));
		io_printf(out,TABS "<ul>" NTABS(,++ntabs));
		for (i=0;i<cfgFileLength;i++) {
			io_printf(out,"<li><a href=\"settings.kl1?cfgFileId=%d\">%s</a>\n"
				TABS "</li>"
				,i,tr(cfgFile[i].i18nKey)
				NTABS(,ntabs));
		}
		io_printf(out,"</ul>\n");
		DECNTABS(ntabs);
	}
	if (!webifConf.configViewDisabled){
		io_printf(out,TABS "</li><li%s><a href=\"view.kl1\">%s</a>\n"
			NTABS(,ntabs),classCurrent[CURRENT_PAGE(PN_FILEVIEW)],tr("fileView"));
		io_printf(out,TABS "<ul>" NTABS(,++ntabs));
		for(i=0;i<fileMappingLength;i++) {
			if (fileExists(fileMapping[i].fileName)) {
				io_printf(out,"<li><a href=\"view.kl1?action=%d&amp;fileNum=%d\">&bull;&nbsp;%s</a>\n"
					TABS "</li>"
					,PA_SHOW,i,tr(fileMapping[i].i18nKey)
					NTABS(,ntabs));
			}
		}
		io_printf(out,"</ul>\n");
		DECNTABS(ntabs);
	}
	
	io_printf(out,
		TABS "</li><li><a href=\"#\">%s</a>\n"
		TABS "<ul><li><a class=\"newWindow\" href=\"http://www.open7x0.org/\">open7x0.org</a>\n"
		TABS "</li><li><a class=\"newWindow\" href=\"http://vdr-m7x0.foroactivo.com.es/\">vdr-m7x0.foroactivo.com.es</a>\n"
		TABS "</li></ul>\n"
		TABS "</li></ul>\n"
		NTABS(,ntabs),tr("links")
		NTABS(,ntabs+1)
		NTABS(,ntabs+1)
		NTABS(,ntabs+1)
		NTABS(,ntabs));
}

void finishHtmlPage(io_t *out,int ntabs){
	if (ntabs<4) {
		dbg("Indentacion erronea, revisar markup");
		ntabs=4;
	}
	io_printf(out,TABS "</div>\n" NTABS(,--ntabs)); //page
	io_printf(out,TABS "<div id=\"page-bottom\">\n" NTABS(,ntabs++));
	io_printf(out,TABS "<p>(C) 2006 open7x0-team</p>\n" NTABS(,ntabs));
	io_printf(out,TABS "<p>(C) 2008 <a href=\"mailto:atinar1@hotmail.com\">atinar</a></p>\n" NTABS(,ntabs));
	io_printf(out,TABS "</div>\n" NTABS(,--ntabs)); //page-bottom
	io_printf(out,TABS "</div>\n" NTABS(,--ntabs)); //page-div
	io_printf(out,TABS "</body>\n" NTABS(,--ntabs));
	io_printf(out,TABS "</html>\n" NTABS(,--ntabs));
}


void printMessage(io_t *out, int ntabs, const char *cssClass, const char *title, const char *message, char *const aux){
	io_printf(out,TABS "<div class=\"%s\">\n" NTABS(,ntabs++),cssClass);
	if (title) io_printf(out,TABS "<p>%s%s</p>\n" NTABS(,ntabs),title,(message && message[0])?":":"");
	if (message && message[0]) {
		if (aux){
			int l=strlen(message);
			u_htmlncpy(aux,message,l,HTMLCPY_ENCODE);
			message=aux;
		}
		io_printf(out,TABS "<p class=\"response\">%s</p>\n" NTABS(,ntabs),message);
	}
	io_printf(out,TABS "</div>\n" NTABS(,--ntabs));
}

void printList1TH(io_t *out, int ntabs, const char *page, sortField_t sortField, const char *label){
	io_printf(out,TABS "<th><a class=\"%s\" href=\"%s?sort=%d&amp;direction=%d\">%s</a></th>\n"
		NTABS(,ntabs),cssSortClass[(SORT_BY(sortField)?webifState.sortDirection:SD_NONE)+1],page
		,sortField,SORT_BY(sortField)?-webifState.sortDirection:SD_ASC,label);
}

char *condstrdup(int isFlagSet,const char *s){
	return (isFlagSet)?strdup(s):(char *)s;
}


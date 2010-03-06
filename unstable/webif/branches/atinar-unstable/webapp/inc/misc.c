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

#include <fcntl.h>
#include <iconv.h>
#include <klone/emb.h>
#include <klone/ioprv.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <locale.h>
#include <u/libu.h>

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
const char *tabs="\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

systemType_t systemType=ST_UNKNOWN;

bool parseRequestStr(request_t *request, char ** pathStr, char ** queryStr) {
	*pathStr=NULL; 
	*queryStr=NULL;
	if (request==NULL){
		return false;
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
	return true;
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

char *strreplace(char *s, const char *s1, const char *s2){
	//taken from tools.h
	char *p = strstr(s, s1);
	if (p) {
		int of = p - s;
		int l  = strlen(s);
		int l1 = strlen(s1);
		int l2 = strlen(s2);
		if (l2 > l1){
			s = (char *)realloc(s, l + l2 - l1 + 1);
		}
		char *sof = s + of;
		if (l2 != l1) {
			memmove(sof + l2, sof + l1, l - of - l1 + 1);
		}
		strncpy(sof, s2, l2);
	}
	return s;
}


bool isDST(time_t * aTime) {
	struct tm t=*localtime(aTime);
	return boolean(t.tm_isdst>0);
}

bool sameDay(time_t oneDate,time_t anotherDate) {
	struct tm sone=*localtime(&oneDate);
	struct tm sanother=*localtime(&anotherDate);
	return boolean((sone.tm_year==sanother.tm_year) && (sone.tm_yday==sanother.tm_yday));
}

bool sameString(const char * s1, const char * s2) {
	return boolean( (s1==NULL && s2==NULL) || (strcmp(s1,s2)==0) );
}

bool sameInt(const int i1, const int i2) {
	return boolean(i1==i2);
}

bool sameIntEx(const char * s, const int i) {
	return boolean(strtol(s,NULL,10)==i);
}

bool fileExists(const char * fileName) {
	struct stat status;

	if (stat(fileName, &status)< 0) { return false; }
	return boolean(S_ISREG(status.st_mode));
}

bool dirExists(const char * dirName) {
	struct stat status;
	
	if (( stat (dirName, &status)) < 0) { return false; }
	return boolean(S_ISDIR(status.st_mode));
}

bool initCtx(wcontext_t *wctx, pageNumber_t currentPage, session_t *session, request_t *request, response_t *response, io_t *out, int bufferLength ){
	readWebifConf();
	wctx->session=session;
	wctx->request=request;
	wctx->response=response;
	wctx->out=out;
	wctx->ntabs=0;
	wctx->currentPage=currentPage;
	wctx->currentAction=PA_NOACTION;
	wctx->currentChannelNum=0;
	wctx->sortBy=SF_START;
	wctx->sortDirection=SD_ASC;
	wctx->bufferLength=0;
	wctx->buffer=NULL;
	wctx->decoratePage=true;
	if (strlen(webifConf.user)>0 && strlen(webifConf.password)>0 && !is_connected(wctx)) {
		return false;
	}
	if (bufferLength>0){
		crit_goto_if((wctx->buffer=malloc(bufferLength))==NULL,outOfMemory);
		wctx->bufferLength=bufferLength;
	}

	header_t *header = request_get_header(request);
	field_t *field;
	field=header_get_field(header,"X-Requested-With");
	wctx->isAjaxRequest=boolean(field!=NULL && strncmp("XMLHttpRequest",field->value,14)==0);

	field=header_get_field(header,"Cache-Control");
	wctx->isReload=boolean(field!=NULL && strstr(field->value,"max-age=0")!=NULL);

	langId=webifConf.langId;  
	if ((langId<0) || (langId>=I18NNUM)) {
		field=header_get_field(header,"Accept-Language");
		if (field!=NULL){
			int i=0;
			for (i=0;i<I18NNUM;i++) {
				if (strncmp(alpha2[i],field->value,2)==0) {
					langId=i;
					break;
				}
			} 
		}
	}
	if ((langId<0) || (langId>=I18NNUM)) {
		langId=0;
	}
	setlocale(LC_ALL,locale[langId]);
	return true;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void freeCtx(wcontext_t *wctx){
	if (wctx->buffer!=NULL){
		free(wctx->buffer);
		wctx->bufferLength=0;
		wctx->buffer=NULL;
	}
}

void chkCtxBuffer(wcontext_t *wctx,int length, const char * routineName){
	if ((length+1)>wctx->bufferLength){
		if (routineName){
			dbg("buffer insuficiente en %s:  %d %d",routineName,wctx->bufferLength,length);
		}
		crit_goto_if((wctx->buffer=realloc(wctx->buffer,length+1))==NULL,outOfMemory);
		wctx->bufferLength=length+1;
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}
char * wctxHtmlEncode(wcontext_t *wctx, const char * const str, int l, const char * routineName){
	if (l<0) l=strlen(str);
	chkCtxBuffer(wctx,2*l,routineName);
	u_htmlncpy(wctx->buffer,str,l,HTMLCPY_ENCODE);
	return wctx->buffer;
}

char * wctxUrlEncode(wcontext_t *wctx, const char * const url, int l, const char * keep, const char *routineName){
	if (l<0) l=strlen(url);
	chkCtxBuffer(wctx,2*l,routineName);
	u_urlncpy_encode(wctx->buffer,url,l,keep);
	return wctx->buffer;
}

char * wctxUrlDecode(wcontext_t *wctx, const char * const url, const char *routineName){
	int l=strlen(url);
	chkCtxBuffer(wctx,l,routineName);
	u_urlncpy(wctx->buffer,url,l,URLCPY_DECODE);
	return wctx->buffer;
}

/*
 * Request hechas a traves de ajax normalmente se codifican en UTF-8, que kloned no procesa bien.
 * Devuelve true si se duplica el string, false si se retorna el arg original.
 */
char * wctxGetRequestParam(wcontext_t *wctx, vars_t *args, const char *argName, bool *isACopy){
	const char *argValue=vars_get_value(args,argName);
	if (isACopy) *isACopy=false;
	if (argValue && argValue[0] && wctx->isAjaxRequest) { 
		//TODO comprobar los headers de request para verificar que se utiliza UTF-8
		iconv_t iconv_cd = iconv_open("ISO-8859-15","UTF-8");
		if( iconv_cd == (iconv_t)-1) {
			warn("Error creando descriptor de conversion UTF-8->ISO-8859-15\n");
		} else {
			size_t inbytesleft = strlen(argValue);
			CTX_CHK_BUFFER(inbytesleft);
			size_t outbytesleft = wctx->bufferLength - 1;
			char *latin1_ptr = wctx->buffer;
			char *utf8_ptr = (char *)argValue;
			size_t chars = iconv(iconv_cd, &utf8_ptr, &inbytesleft, &latin1_ptr, &outbytesleft);
			if(chars == -1) {
				warn("Error de conversion UTF8->ISO-8859-15 [%s]",argValue);
			} else {
				*latin1_ptr=0;
				if (isACopy) *isACopy=true;
				return strdup(wctx->buffer);
			}
		}
	}
	return (char *)argValue;
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

void returnHttpNoContent(response_t *response){
	response_set_status(response,HTTP_STATUS_NO_CONTENT);
	response_print_header(response);
}	

void initHtmlDoc(wcontext_t *wctx){
	response_set_content_type(wctx->response,"text/html; charset=ISO-8859-15");
	response_set_field(wctx->response,"Content-Style-Type","text/css");
	response_set_field(wctx->response,"Content-Script-Type","text/javascript");
	wctx->ntabs=0;
	if (!wctx->isAjaxRequest){
		wctx_printf0(wctx,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
		wctx_printfn(wctx,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\" lang=\"%s\">\n",0,1,alpha2[langId],alpha2[langId]);
	}
}

void initHtmlPage(wcontext_t *wctx, const char *title, printHtmlHeadExtra_t printHtmlHeadExtra){
	initHtmlDoc(wctx);
	if (!wctx->isAjaxRequest){
		wctx_printfn(wctx,"<head>\n",0,1);
		wctx_printf0(wctx,"<title>%s</title>\n",title);
		wctx_printf0(wctx,"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=Edge\" />\n");
		if (wctx->decoratePage){
			wctx_printf0(wctx,"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen.css\" media=\"screen\" />\n",webifConf.www);
			wctx_printf0(wctx,"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/print.css\" media=\"print\" />\n",webifConf.www);
			wctx_printf0(wctx,"<script type=\"text/javascript\" src=\"%s/js/jquery-1.3.2.min.js\"></script>\n",webifConf.www);
			wctx_printf0(wctx,"<script type=\"text/javascript\" src=\"%s/js/jquery-ui-1.7.2.min.js\"></script>\n",webifConf.www);
			wctx_printf0(wctx,"<script type=\"text/javascript\" src=\"%s/js/jquery.hoverIntent.min.js\"></script>\n",webifConf.www);
			wctx_printf0(wctx,"<script type=\"text/javascript\" src=\"%s/js/superfish.js\"></script>\n",webifConf.www);
			wctx_printf0(wctx,"<script type=\"text/javascript\" src=\"%s/js/supersubs.js\"></script>\n",webifConf.www);
			wctx_printf0(wctx,"<script type=\"text/javascript\" src=\"%s/js/common.js\"></script>\n",webifConf.www);
			wctx_printfn(wctx,"<script type=\"text/javascript\">\n",0,1);
			wctx_printf0(wctx,"$.extend(webif.state,{'currentPage':%d,'currentAction':%d});\n",wctx->currentPage,wctx->currentAction);
			wctx_printf0(wctx,"$.extend(webif.messages,{close:'%s',cancel:'%s'});\n",tr("close"),tr("cancel"));
			wctx_printfn(wctx,"</script>\n",-1,0);
		}
	}
	if (printHtmlHeadExtra) {
		printHtmlHeadExtra(wctx);
	}
	if (!wctx->isAjaxRequest && wctx->decoratePage){
		wctx_printfn(wctx,"<!--[if IE 8]>\n",0,1);
		wctx_printf0(wctx,"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie8.css\" media=\"screen\" />\n",webifConf.www);
		wctx_printfn(wctx,"<![endif]-->\n",-1,0);
	}
		
	if (!wctx->isAjaxRequest){
		wctx_printfn(wctx,"</head>\n",-1,0);
		wctx_printfn(wctx,"<body id=\"body-p%d-a%d\">\n",0,1,wctx->currentPage,wctx->currentAction);
		if (wctx->decoratePage){
			wctx_printfn(wctx,"<div id=\"page-div\">\n",0,1);
			wctx_printfn(wctx,"<div id=\"page-top\">\n",0,1);
			wctx_printfn(wctx,"<table class=\"layout\" cellspacing=\"5\">\n",0,1);
			wctx_printf0(wctx,"<col class=\"menu\"/>\n");
			wctx_printf0(wctx,"<col class=\"logo\"/>\n");
			wctx_printfn(wctx,"<tbody>\n",0,1);
			wctx_printfn(wctx,"<tr>\n",0,1);
			wctx_printfn(wctx,"<td>\n",0,1);
			wctx_printf0(wctx,"<h1 id=\"page-title\">%s</h1>\n",title);
			wctx_printfn(wctx,"</td>\n",-1,0);
			wctx_printfn(wctx,"<td rowspan=\"2\">\n",0,1);
			wctx_printf0(wctx,"<div id=\"logo\"></div>\n");
			wctx_printfn(wctx,"</td>\n",-1,0);
			wctx_printfn(wctx,"</tr>\n",-1,0);
			wctx_printfn(wctx,"<tr>\n",0,1);
			wctx_printfn(wctx,"<td>\n",0,1);
			printMenu(wctx);
			wctx_printfn(wctx,"</td>\n",-1,0);
			wctx_printfn(wctx,"</tr>\n",-1,0);
			wctx_printfn(wctx,"</tbody>\n",-1,0);
			wctx_printfn(wctx,"</table>\n",-1,0);
			wctx_printfn(wctx,"</div>\n",-1,0); //page-top
			wctx_printfn(wctx,"<div id=\"page\">\n",0,1);
		}
/*		No nos pasemos, de momento:
		wctx_printfn(wctx,"<!--[if lte IE 8]>\n",0,1);
		printMessage(wctx,"alert",tr("browser.not_supported"),tr("browser.please_update"),false);
		wctx_printfn(wctx,"<![endif]-->\n",-1,0);
*/
	}
}

#define CURRENT_PAGE(pageNumber) boolean(wctx->currentPage==pageNumber)
#define CURRENT_ACTION(action) boolean(wctx->currentAction==action)
#define SORT_BY(sortField) boolean(wctx->sortBy==sortField)

void printMenu(wcontext_t *wctx){
	if (wctx->isAjaxRequest) return;
	int i;
	int clock_id;
	const char *Summary=tr("summary");
	wctx_printf0(wctx,"<ul id=\"menu\" class=\"sf-menu\"><li%s><a href=\"epgGrid.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_EPG_GRID)],tr("epg"));
	wctx_printfn(wctx,"<ul><li%s><a href=\"epgGrid.kl1\">%s</a>\n"
		,1,0,classCurrent[CURRENT_PAGE(PN_EPG_GRID)],tr("epg.grid"));
	wctx_printf0(wctx,"</li><li%s><a href=\"epg.kl1?channelNum=%d\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_PROGRAMS)],(wctx->currentChannelNum>0)?wctx->currentChannelNum:1,tr("epg.list"));
	wctx_printfn(wctx,"</li></ul>\n",0,-1);

	wctx_printf0(wctx,"</li><li%s><a href=\"channels.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_CHANNELS)],tr("channels"));
	wctx_printfn(wctx,"<ul><li%s><a href=\"channels.kl1\">%s</a>\n"
		,1,0,classCurrent[CURRENT_PAGE(PN_CHANNELS)],Summary);
	wctx_printf0(wctx,"</li><li%s><a href=\"live.kl1?channelNum=%d\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_LIVE_STREAM)],(wctx->currentChannelNum>0)?wctx->currentChannelNum:1,tr("liveStream"));
	wctx_printf0(wctx,"</li><li><a href=\"playlistch.kl1?type=%d\">%s</a>\n"
		,webifConf.playlistType,tr("channel.playlist"));
	wctx_printfn(wctx,"</li></ul>\n",0,-1);

	wctx_printf0(wctx,"</li><li%s><a href=\"timers.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_TIMERS)],tr("timers"));
	wctx_printfn(wctx,"<ul><li%s><a href=\"timers.kl1\">%s</a>\n"
		,1,0,classCurrent[boolean(CURRENT_PAGE(PN_TIMERS) && !CURRENT_ACTION(PA_EDIT))],tr("timers"));
	wctx_printf0(wctx,"</li><li%s><a href=\"timers.kl1?a=%d\">%s</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_TIMERS) && CURRENT_ACTION(PA_EDIT))],PA_EDIT,tr("timer.create"));
	wctx_printf0(wctx,"</li><li%s><a href=\"searches.kl1\">%s</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_SEARCHES) && !CURRENT_ACTION(PA_EDIT))],tr("searches"));
	wctx_printf0(wctx,"</li><li%s><a href=\"searches.kl1?a=%d\">%s</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_SEARCHES) && CURRENT_ACTION(PA_EDIT))],PA_EDIT,tr("search.create"));
	wctx_printfn(wctx,"</li></ul>\n",0,-1);
		
	wctx_printf0(wctx,"</li><li%s><a href=\"recordings.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_RECORDINGS)],tr("recordings"));
	wctx_printfn(wctx,"<ul><li%s><a href=\"recordings.kl1\">%s</a>\n"
		,1,0,classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && CURRENT_ACTION(PA_BROWSE))],tr("browse"));
	wctx_printf0(wctx,"</li><li%s><a href=\"recordings.kl1?a=%d&amp;sort=%d&amp;direction=%d\">%s (%s)</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && CURRENT_ACTION(PA_SUMMARY) && SORT_BY(SF_TITLE))],PA_SUMMARY,SF_TITLE,SD_ASC,Summary,tr("title"));
	wctx_printf0(wctx,"</li><li%s><a href=\"recordings.kl1?a=%d&amp;sort=%d&amp;direction=%d\">%s (%s)</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && CURRENT_ACTION(PA_SUMMARY) && SORT_BY(SF_START))],PA_SUMMARY,SF_START,SD_DESC,Summary,tr("date"));
	wctx_printfn(wctx,"</li></ul>\n",0,-1);
		
	if (!webifConf.configChangeDisabled){
		wctx_printf0(wctx,"</li><li%s><a href=\"settings.kl1\">%s</a>\n",classCurrent[CURRENT_PAGE(PN_SETTINGS)],tr("setup"));
		wctx_printfn(wctx,"<ul>",1,0);
		for (i=0;i<cfgFileLength;i++) {
			wctx_printf(wctx,"<li><a href=\"settings.kl1?cfgFileId=%d\">%s</a>\n",i,tr(cfgFile[i].i18nKey));
			wctx_printf0(wctx,"</li>");
		}
		wctx_printf(wctx,"</ul>\n");
		dectab(wctx);
	}
	if (!webifConf.configViewDisabled){
		wctx_printf0(wctx,"</li><li%s><a href=\"view.kl1\">%s</a>\n",classCurrent[CURRENT_PAGE(PN_FILEVIEW)],tr("fileView"));
		wctx_printfn(wctx,"<ul>",1,0);
		for(i=0;i<fileMappingLength;i++) {
			if (fileExists(fileMapping[i].fileName)) {
				wctx_printf(wctx,"<li><a href=\"view.kl1?action=%d&amp;fileNum=%d\">&bull;&nbsp;%s</a>\n",PA_SHOW,i,tr(fileMapping[i].i18nKey));
				wctx_printf0(wctx,"</li>");
			}
		}
		wctx_printf(wctx,"</ul>\n");
		dectab(wctx);
	}
	
	if (fileExists(commandsFile)) {
		wctx_printf0(wctx,"</li><li%s><a href=\"commands.kl1?a=%d\">%s</a>\n",classCurrent[CURRENT_PAGE(PN_COMMANDS)],PA_COMMANDS_SHOW,tr("commands"));
	}
	
	wctx_printf0(wctx,"</li><li><a href=\"#\">%s</a>\n",tr("links"));
	wctx_printfn(wctx,"<ul><li><a class=\"newWindow\" href=\"http://www.open7x0.org/\">open7x0.org</a>\n",1,0);
	wctx_printf0(wctx,"</li><li><a class=\"newWindow\" href=\"http://vdr-m7x0.foroactivo.com.es/\">vdr-m7x0.foroactivo.com.es</a>\n");
	wctx_printfn(wctx,"</li></ul>\n",0,-1);
	wctx_printf0(wctx,"</li></ul>\n");
}

void printInput(wcontext_t *wctx, const char *type, const char *id, const char *name, int idx, const char *value, int l){
	char tmpName[20];
	if (name==NULL) {
		snprintf(tmpName,20,"paramValue_%d",idx);
		name=tmpName;
	}
	wctx_printf0(wctx,"<input type=\"%s\" ",type);
	if (id!=NULL){
		wctx_printf(wctx,"id=\"%s\" ",id);
	}
	wctx_printf(wctx,"name=\"%s\" value=\"%s\" size=\"%d\" />\n",name,(value)?CTX_HTML_ENCODE(value,-1):"",(l>0)?l:(value)?strlen(value)+3:10);
}


void finishHtmlPage(wcontext_t *wctx){
	if (!wctx->isAjaxRequest){
		if (wctx->decoratePage){
#ifndef IGNORE_TABS
			if (wctx->ntabs<4) {
				dbg("Indentacion erronea, revisar markup");
				wctx->ntabs=4;
			}
#endif
		}
		if (wctx->decoratePage){
			wctx_printfn(wctx,"</div>\n",-1,0); //page
			wctx_printfn(wctx,"<div id=\"page-bottom\">\n",0,1);
			wctx_printf0(wctx,"<p>(C) 2006 open7x0-team</p>\n");
			wctx_printf0(wctx,"<p>(C) 2008 <a href=\"mailto:atinar1@hotmail.com\">atinar</a></p>\n");
			wctx_printfn(wctx,"</div>\n",-1,0); //page-bottom
			wctx_printfn(wctx,"</div>\n",-1,0); //page-div
		}
		wctx_printfn(wctx,"</body>\n",-1,0);
		wctx_printfn(wctx,"</html>\n",-1,0);
	}
}

void initJavascript(wcontext_t *wctx){
	wctx_printfn(wctx,"<script type=\"text/javascript\">\n",0,1);
	//wctx_printfn(wctx,"<![CDATA[\n",0,1);
}

void finishJavascript(wcontext_t *wctx){
	//wctx_printfn(wctx,"]]>",-1,0);
	wctx_printfn(wctx,"</script>\n",-1,0);
}

void printMessage(wcontext_t *wctx, const char *cssClass, const char *title, const char *message, bool encode){
	wctx_printfn(wctx,"<div class=\"response-div %s\">\n",0,1,cssClass);
	if (title && title[0]){
		wctx_printf0(wctx,"<p class=\"response-top\">%s%s</p>\n",title,(message && message[0])?":":"");
	}
	if (message && message[0]) {
		wctx_printf0(wctx,"<p class=\"response\">%s</p>\n",(encode) ? CTX_HTML_ENCODE(message,-1) : message);
	}
	wctx_printfn(wctx,"</div>\n",-1,0);
}

void printList1TH(wcontext_t *wctx, const char *page, sortField_t sortField, const char *label){
	wctx_printf0(wctx,"<th><a class=\"%s\" href=\"%s?a=%d&amp;sort=%d&amp;direction=%d\">%s</a></th>\n"
		,cssSortClass[(SORT_BY(sortField)?wctx->sortDirection:SD_NONE)+1],page,wctx->currentAction
		,sortField,SORT_BY(sortField)?-wctx->sortDirection:SD_ASC,label);
}

/**
* Extraer archivo embebido. Devuelve true si al terminar existe dst.
*/
bool extractEmbededFile(const char *src, const char *dst, bool overwrite){
	bool result=false;
	io_t *io=NULL;
	if (createParentFolders(dst,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)){
		struct stat st;
		int dst_stat=stat(dst, &st);
		if (dst_stat == 0 && S_ISREG(st.st_mode) && !overwrite){
			info("%s ya existe. Se conserva el anterior.",dst);
			result=true;
		} else if (((dst_stat==-1) && (errno==ENOENT)) || ((dst_stat==0) && S_ISREG(st.st_mode) && overwrite)){
			//Extraer archivo embebido
			embres_t *er;
			if ((emb_lookup(src, &er)!=0) || (er->type!=ET_FILE)) {
				warn("%s no es un archivo embebido",src);
			} else {
				embfile_t *ef=(embfile_t *)er;
				dbg_err_if(u_file_open(dst, O_CREAT | O_TRUNC | O_WRONLY, &io));
				dbg_err_if(!io_write(io, (const char*)ef->data, ef->size));
				//dbg_err_if(io_codecs_remove(io)); 
				io_free(io);
				dst_stat=stat(dst, &st);
				if (dst_stat == 0 && S_ISREG(st.st_mode)){
					info("%s se ha extraido correctamente",dst);
					result=true;
				} else {
					warn("Error de extraccion. stat produce un error inesperado %s",strerror(errno));
				}
			}
		} else {
			warn("stat produce un error inesperado %s",strerror(errno));
		}
	}
	return result;
err:
	if (io){
		io_free(io);
	}
	return false;
}

bool createParentFolders(const char *filename, mode_t mode){
	bool result=true;
	if (filename && filename[0]=='/'){
		char *last_slash=strrchr(filename,'/');
		if (last_slash!=filename){
			struct stat st;
			int path_stat;
			char *path=strndup(filename,last_slash-filename);
			char *next_slash=path;
			for (;;) {
				next_slash=strchr(next_slash+1,'/');
				if (next_slash) *next_slash=0;
				path_stat=stat(path,&st);
				if (path_stat==0){
					if (!S_ISDIR(st.st_mode)){
						result=false;
						warn("%s no es directorio",path);
						break;
					}
				} else {
					if( (errno!=ENOENT) || mkdir(path,mode)!=0){
						result=false;
						warn("error creando ruta %s: %s",path,strerror(errno));
						break;
					}
				}
				if (!next_slash) break;
				*next_slash='/';
			};
			free(path);
		}
	} else {
		warn("%s no es una ruta absoluta",filename);
		result=false;
	}
	return result;
}

bool extractLogosFromFile(wcontext_t *wctx, const char *logos_tgz){
	bool result=false;
	//TODO interfiere con hooks.
	pid_t pid=vfork();
	if (pid<0) { 
		//fork failure
		warn("fallo la bifurcacion");
		result=false;
	} else if (pid==0) {
		//child process
		const char *images="/etc/webif/www/images/";
		if (createParentFolders(images,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)){
			const char *tar="/bin/tar";
			execl((char *)tar, (char *)tar, "-C", (char*)images, "-xzf", (char*)logos_tgz, (char*)NULL);
			//exec only returns if call fails
			perror("exec");
			_exit(-1);
		} else {
			_exit(-2);
		}
	} else { 
		//parent process
		int status;
		pid_t child_pid=waitpid(pid,&status,0);
		if (WIFEXITED(status)){
			int exitStatus;
			if ((exitStatus=WEXITSTATUS(status))==0){
				if (wctx!=NULL){
					printMessage(wctx,"message",tr("setup"),"Logos extraídos",false); //TODO i18n
				}
				info("Logos extraídos");
				webifConf.noLogos=false;
				result=true;
			} else {
				if (wctx!=NULL){
					printMessage(wctx,"alert",tr("setup"),"Error extrayendo logos",false); //TODO i18n
				}
				warn("Error descomprimiendo logos");
			}
		} else {
			if (wctx!=NULL){
				printMessage(wctx,"alert",tr("setup"),"Extracción de logos interrumpida",false); //TODO i18n
			}
			warn("child process signaled");
			result=false;
		}
	}
	return result;
}

bool extractLogosFromRequest(wcontext_t *wctx, const char *fieldName){
	bool result=false;
	char localFilename[U_FILENAME_MAX]; 
	char clientFilename[U_FILENAME_MAX];
	char mimeType[MIME_TYPE_BUFSZ];
	size_t idx=0;
	size_t size;
	if (request_get_uploaded_file(wctx->request,fieldName,idx,localFilename,clientFilename,mimeType,&size)==0){
		result=extractLogosFromFile(wctx,localFilename);
	}
	return result;
}

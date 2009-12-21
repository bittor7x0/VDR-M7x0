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

void initCtx(context_t *ctx, session_t *session, request_t *request, response_t *response, io_t *out, int bufferLength ){
	readWebifConf();
	ctx->session=session;
	ctx->request=request;
	ctx->response=response;
	ctx->out=out;
	ctx->ntabs=0;
	ctx->currentPage=PN_INDEX;
	ctx->currentAction=PA_NOACTION;
	ctx->currentChannelNum=0;
	ctx->sortBy=SF_START;
	ctx->sortDirection=SD_ASC;
	ctx->bufferLength=bufferLength;
	ctx->buffer=NULL;
	if (bufferLength>0){
		crit_goto_if((ctx->buffer=malloc(bufferLength))==NULL,outOfMemory);
	}

	header_t *header = request_get_header(request);
	field_t *field;
	field=header_get_field(header,"X-Requested-With");
	ctx->isAjaxRequest=boolean(field!=NULL && strncmp("XMLHttpRequest",field->value,14)==0);

	field=header_get_field(header,"Cache-Control");
	ctx->isReload=boolean(field!=NULL && strstr(field->value,"max-age=0")!=NULL);

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
	/*----System type----
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
	*/
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void freeCtx(context_t *ctx){
	if (ctx->buffer!=NULL){
		free(ctx->buffer);
		ctx->bufferLength=0;
		ctx->buffer=NULL;
	}
}

void chkCtxBuffer(context_t *ctx,int length, const char * routineName){
	if ((length+1)>ctx->bufferLength){
		if (routineName){
			warn("buffer insuficiente en %s:  %d %d",routineName,ctx->bufferLength,length);
		}
		crit_goto_if((ctx->buffer=realloc(ctx->buffer,length+1))==NULL,outOfMemory);
		ctx->bufferLength=length+1;
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}
char * ctxHtmlEncode(context_t *ctx, const char * const str, int l, const char * routineName){
	if (l<0) l=strlen(str);
	chkCtxBuffer(ctx,2*l,routineName);
	u_htmlncpy(ctx->buffer,str,l,HTMLCPY_ENCODE);
	return ctx->buffer;
}

char * ctxUrlEncode(context_t *ctx, const char * const url, int l, const char * keep, const char *routineName){
	if (l<0) l=strlen(url);
	chkCtxBuffer(ctx,2*l,routineName);
	u_urlncpy_encode(ctx->buffer,url,l,keep);
	return ctx->buffer;
}

char * ctxUrlDecode(context_t *ctx, const char * const url, const char *routineName){
	int l=strlen(url);
	chkCtxBuffer(ctx,l,routineName);
	u_urlncpy(ctx->buffer,url,l,URLCPY_DECODE);
	return ctx->buffer;
}

/*
 * Request hechas a traves de ajax normalmente se codifican en UTF-8, que kloned no procesa bien.
 * Devuelve true si se duplica el string, false si se retorna el arg original.
 */
boolean_t ctxGetRequestParamAsCpy(context_t *ctx, char **target, vars_t *args, const char *argName){
	const char *argValue=vars_get_value(args,argName);
	if (argValue && argValue[0] && ctx->isAjaxRequest) { 
		//TODO comprobar los headers de request para verificar que se utiliza UTF-8
		iconv_t iconv_cd = iconv_open("ISO-8859-1","UTF-8");
		if( iconv_cd == (iconv_t)-1) {
			warn("Error creando descriptor de conversion UTF-8->ISO-8859-1\n");
		} else {
			size_t inbytesleft = strlen(argValue);
			CTX_CHK_BUFFER(inbytesleft);
			size_t outbytesleft = ctx->bufferLength - 1;
			char *latin1_ptr = ctx->buffer;
			char *utf8_ptr = (char *)argValue;
			size_t chars = iconv(iconv_cd, &utf8_ptr, &inbytesleft, &latin1_ptr, &outbytesleft);
			if(chars == -1) {
				warn("Error de conversion UTF8->ISO-8859-1");
			} else {
				*latin1_ptr=0;
				*target=strdup(ctx->buffer);
				return BT_TRUE;
			}
		}
	}
	*target=(char *)argValue;
	return BT_FALSE;
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

void initHtmlDoc(context_t *ctx){
	response_set_content_type(ctx->response,"text/html; charset=ISO-8859-1");
	response_set_field(ctx->response,"Content-Style-Type","text/css");
	response_set_field(ctx->response,"Content-Script-Type","text/javascript");
	ctx->ntabs=0;
	if (!ctx->isAjaxRequest){
		ctx_printf0(ctx,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
		ctx_printfn(ctx,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"%s\" lang=\"%s\">\n",0,1,alpha2[langId],alpha2[langId]);
	}
}

void initHtmlPage(context_t *ctx, const char *title, printHtmlHeadExtra_t printHtmlHeadExtra){
	initHtmlDoc(ctx);
	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"<head>\n",0,1);
		ctx_printf0(ctx,"<title>%s</title>\n",title);
		ctx_printf0(ctx,"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=Edge\" />\n");
		ctx_printf0(ctx,"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen.css\" media=\"screen\" />\n",webifConf.www);
		ctx_printf0(ctx,"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/print.css\" media=\"print\" />\n",webifConf.www);
		ctx_printf0(ctx,"<script type=\"text/javascript\" src=\"%s/js/jquery-1.3.2.min.js\"></script>\n",webifConf.www);
		ctx_printf0(ctx,"<script type=\"text/javascript\" src=\"%s/js/jquery-ui-1.7.2.min.js\"></script>\n",webifConf.www);
		ctx_printf0(ctx,"<script type=\"text/javascript\" src=\"%s/js/jquery.hoverIntent.min.js\"></script>\n",webifConf.www);
		ctx_printf0(ctx,"<script type=\"text/javascript\" src=\"%s/js/superfish.js\"></script>\n",webifConf.www);
		ctx_printf0(ctx,"<script type=\"text/javascript\" src=\"%s/js/supersubs.js\"></script>\n",webifConf.www);
		ctx_printf0(ctx,"<script type=\"text/javascript\" src=\"%s/js/common.js\"></script>\n",webifConf.www);
		ctx_printfn(ctx,"<script type=\"text/javascript\">\n",0,1);
		ctx_printf0(ctx,"$.extend(webif.state,{'currentPage':%d,'currentAction':%d});\n",ctx->currentPage,ctx->currentAction);
		ctx_printf0(ctx,"$.extend(webif.messages,{close:'%s',cancel:'%s'});\n",tr("close"),tr("cancel"));
		ctx_printfn(ctx,"</script>\n",-1,0);
	}
	if (printHtmlHeadExtra) {
		printHtmlHeadExtra(ctx);
	}
	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"<!--[if IE 8]>\n",0,1);
		ctx_printf0(ctx,"<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/css/screen-ie8.css\" media=\"screen\" />\n",webifConf.www);
		ctx_printfn(ctx,"<![endif]-->\n",-1,0);
	}
		
	if (!ctx->isAjaxRequest){
		ctx_printfn(ctx,"</head>\n",-1,0);
		ctx_printfn(ctx,"<body id=\"body-p%d-a%d\">\n",0,1,ctx->currentPage,ctx->currentAction);
		ctx_printfn(ctx,"<div id=\"page-div\">\n",0,1);
		ctx_printfn(ctx,"<div id=\"page-top\">\n",0,1);
		ctx_printfn(ctx,"<table class=\"layout\" cellspacing=\"5\">\n",0,1);
		ctx_printf0(ctx,"<col class=\"menu\"/>\n");
		ctx_printf0(ctx,"<col class=\"logo\"/>\n");
		ctx_printfn(ctx,"<tbody>\n",0,1);
		ctx_printfn(ctx,"<tr>\n",0,1);
		ctx_printfn(ctx,"<td>\n",0,1);
		ctx_printf0(ctx,"<h1 id=\"page-title\">%s</h1>\n",title);
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"<td rowspan=\"2\">\n",0,1);
		ctx_printf0(ctx,"<div id=\"logo\"></div>\n");
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"</tr>\n",-1,0);
		ctx_printfn(ctx,"<tr>\n",0,1);
		ctx_printfn(ctx,"<td>\n",0,1);
		printMenu(ctx);
		ctx_printfn(ctx,"</td>\n",-1,0);
		ctx_printfn(ctx,"</tr>\n",-1,0);
		ctx_printfn(ctx,"</tbody>\n",-1,0);
		ctx_printfn(ctx,"</table>\n",-1,0);
		ctx_printfn(ctx,"</div>\n",-1,0); //page-top
		ctx_printfn(ctx,"<div id=\"page\">\n",0,1);
/*		No nos pasemos, de momento:
		ctx_printfn(ctx,"<!--[if lte IE 8]>\n",0,1);
		printMessage(ctx,"alert",tr("browser.not_supported"),tr("browser.please_update"),BT_FALSE);
		ctx_printfn(ctx,"<![endif]-->\n",-1,0);
*/
	}
}

#define CURRENT_PAGE(pageNumber) boolean(ctx->currentPage==pageNumber)
#define CURRENT_ACTION(action) boolean(ctx->currentAction==action)
#define SORT_BY(sortField) boolean(ctx->sortBy==sortField)

void printMenu(context_t *ctx){
	if (ctx->isAjaxRequest) return;
	int i;
	int clock_id;
	const char *Summary=tr("summary");
	/*
	ctx_printf0(ctx,"<ul id=\"menu\" class=\"sf-menu\"><li%s><a href=\"index.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_INDEX)],tr("start"));
	*/
	ctx_printf0(ctx,"<ul id=\"menu\" class=\"sf-menu\"><li%s><a href=\"epgGrid.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_EPG_GRID)],tr("epg"));
	ctx_printfn(ctx,"<ul><li%s><a href=\"epgGrid.kl1\">%s</a>\n"
		,1,0,classCurrent[CURRENT_PAGE(PN_EPG_GRID)],tr("epg.grid"));
	ctx_printf0(ctx,"</li><li%s><a href=\"epg.kl1?channelNum=%d\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_PROGRAMS)],(ctx->currentChannelNum>0)?ctx->currentChannelNum:1,tr("epg.list"));
	ctx_printfn(ctx,"</li></ul>\n",0,-1);

	ctx_printf0(ctx,"</li><li%s><a href=\"channels.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_CHANNELS)],tr("channels"));
	ctx_printfn(ctx,"<ul><li%s><a href=\"channels.kl1\">%s</a>\n"
		,1,0,classCurrent[CURRENT_PAGE(PN_CHANNELS)],Summary);
	ctx_printf0(ctx,"</li><li%s><a href=\"live.kl1?channelNum=%d\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_LIVE_STREAM)],(ctx->currentChannelNum>0)?ctx->currentChannelNum:1,tr("liveStream"));
	ctx_printf0(ctx,"</li><li><a href=\"playlistch.kl1?type=%d\">%s</a>\n"
		,webifConf.playlistType,tr("channel.playlist"));
	ctx_printfn(ctx,"</li></ul>\n",0,-1);

	ctx_printf0(ctx,"</li><li%s><a href=\"timers.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_TIMERS)],tr("timers"));
	ctx_printfn(ctx,"<ul><li%s><a href=\"timers.kl1\">%s</a>\n"
		,1,0,classCurrent[boolean(CURRENT_PAGE(PN_TIMERS) && !CURRENT_ACTION(PA_EDIT))],tr("timers"));
	ctx_printf0(ctx,"</li><li%s><a href=\"timers.kl1?a=%d\">%s</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_TIMERS) && CURRENT_ACTION(PA_EDIT))],PA_EDIT,tr("timer.create"));
	ctx_printf0(ctx,"</li><li%s><a href=\"searches.kl1\">%s</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_SEARCHES) && !CURRENT_ACTION(PA_EDIT))],tr("searches"));
	ctx_printf0(ctx,"</li><li%s><a href=\"searches.kl1?a=%d\">%s</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_SEARCHES) && CURRENT_ACTION(PA_EDIT))],PA_EDIT,tr("search.create"));
	ctx_printfn(ctx,"</li></ul>\n",0,-1);
		
	ctx_printf0(ctx,"</li><li%s><a href=\"recordings.kl1\">%s</a>\n"
		,classCurrent[CURRENT_PAGE(PN_RECORDINGS)],tr("recordings"));
	ctx_printfn(ctx,"<ul><li%s><a href=\"recordings.kl1\">%s</a>\n"
		,1,0,classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && CURRENT_ACTION(PA_BROWSE))],tr("browse"));
	ctx_printf0(ctx,"</li><li%s><a href=\"recordings.kl1?a=%d&amp;sort=%d&amp;direction=%d\">%s (%s)</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && CURRENT_ACTION(PA_SUMMARY) && SORT_BY(SF_TITLE))],PA_SUMMARY,SF_TITLE,SD_ASC,Summary,tr("title"));
	ctx_printf0(ctx,"</li><li%s><a href=\"recordings.kl1?a=%d&amp;sort=%d&amp;direction=%d\">%s (%s)</a>\n"
		,classCurrent[boolean(CURRENT_PAGE(PN_RECORDINGS) && CURRENT_ACTION(PA_SUMMARY) && SORT_BY(SF_START))],PA_SUMMARY,SF_START,SD_DESC,Summary,tr("date"));
	ctx_printfn(ctx,"</li></ul>\n",0,-1);
		
	if (!webifConf.configChangeDisabled){
		ctx_printf0(ctx,"</li><li%s><a href=\"settings.kl1\">%s</a>\n",classCurrent[CURRENT_PAGE(PN_SETTINGS)],tr("setup"));
		ctx_printfn(ctx,"<ul>",1,0);
		for (i=0;i<cfgFileLength;i++) {
			ctx_printf(ctx,"<li><a href=\"settings.kl1?cfgFileId=%d\">%s</a>\n",i,tr(cfgFile[i].i18nKey));
			ctx_printf0(ctx,"</li>");
		}
		ctx_printf(ctx,"</ul>\n");
		dectab(ctx);
	}
	if (!webifConf.configViewDisabled){
		ctx_printf0(ctx,"</li><li%s><a href=\"view.kl1\">%s</a>\n",classCurrent[CURRENT_PAGE(PN_FILEVIEW)],tr("fileView"));
		ctx_printfn(ctx,"<ul>",1,0);
		for(i=0;i<fileMappingLength;i++) {
			if (fileExists(fileMapping[i].fileName)) {
				ctx_printf(ctx,"<li><a href=\"view.kl1?action=%d&amp;fileNum=%d\">&bull;&nbsp;%s</a>\n",PA_SHOW,i,tr(fileMapping[i].i18nKey));
				ctx_printf0(ctx,"</li>");
			}
		}
		ctx_printf(ctx,"</ul>\n");
		dectab(ctx);
	}
	
	ctx_printf0(ctx,"</li><li><a href=\"#\">%s</a>\n",tr("links"));
	ctx_printfn(ctx,"<ul><li><a class=\"newWindow\" href=\"http://www.open7x0.org/\">open7x0.org</a>\n",1,0);
	ctx_printf0(ctx,"</li><li><a class=\"newWindow\" href=\"http://vdr-m7x0.foroactivo.com.es/\">vdr-m7x0.foroactivo.com.es</a>\n");
	ctx_printfn(ctx,"</li></ul>\n",0,-1);
	ctx_printf0(ctx,"</li></ul>\n");
}

void finishHtmlPage(context_t *ctx){
	if (!ctx->isAjaxRequest){
#ifndef IGNORE_TABS
		if (ctx->ntabs<4) {
			dbg("Indentacion erronea, revisar markup");
			ctx->ntabs=4;
		}
#endif
		//ctx_printfn(ctx,"</div>\n",-1,0); //main
		ctx_printfn(ctx,"</div>\n",-1,0); //page
		ctx_printfn(ctx,"<div id=\"page-bottom\">\n",0,1);
		ctx_printf0(ctx,"<p>(C) 2006 open7x0-team</p>\n");
		ctx_printf0(ctx,"<p>(C) 2008 <a href=\"mailto:atinar1@hotmail.com\">atinar</a></p>\n");
		ctx_printfn(ctx,"</div>\n",-1,0); //page-bottom
		ctx_printfn(ctx,"</div>\n",-1,0); //page-div
		ctx_printfn(ctx,"</body>\n",-1,0);
		ctx_printfn(ctx,"</html>\n",-1,0);
	}
}

void initJavascript(context_t *ctx){
	ctx_printfn(ctx,"<script type=\"text/javascript\">\n",0,1);
	//ctx_printfn(ctx,"<![CDATA[\n",0,1);
}

void finishJavascript(context_t *ctx){
	//ctx_printfn(ctx,"]]>",-1,0);
	ctx_printfn(ctx,"</script>\n",-1,0);
}

void printMessage(context_t *ctx, const char *cssClass, const char *title, const char *message, boolean_t encode){
	ctx_printfn(ctx,"<div class=\"response-div %s\">\n",0,1,cssClass);
	if (title && title[0]){
		ctx_printf0(ctx,"<p class=\"response-top\">%s%s</p>\n",title,(message && message[0])?":":"");
	}
	if (message && message[0]) {
		ctx_printf0(ctx,"<p class=\"response\">%s</p>\n",(encode) ? CTX_HTML_ENCODE(message,-1) : message);
	}
	ctx_printfn(ctx,"</div>\n",-1,0);
}

void printList1TH(context_t *ctx, const char *page, sortField_t sortField, const char *label){
	ctx_printf0(ctx,"<th><a class=\"%s\" href=\"%s?a=%d&amp;sort=%d&amp;direction=%d\">%s</a></th>\n"
		,cssSortClass[(SORT_BY(sortField)?ctx->sortDirection:SD_NONE)+1],page,ctx->currentAction
		,sortField,SORT_BY(sortField)?-ctx->sortDirection:SD_ASC,label);
}

/**
* Extraer archivo embebido. Devuelve true si al terminar existe dst.
*/
boolean_t extractEmbededFile(const char *src, const char *dst, boolean_t overwrite){
	boolean_t result=BT_FALSE;
	io_t *io=NULL;
	if (createParentFolders(dst,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)){
		struct stat st;
		int dst_stat=stat(dst, &st);
		if (dst_stat == 0 && S_ISREG(st.st_mode) && !overwrite){
			info("%s ya existe. Se conserva el anterior.",dst);
			result=BT_TRUE;
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
					result=BT_TRUE;
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
	return BT_FALSE;
}

boolean_t createParentFolders(const char *filename, mode_t mode){
	boolean_t result=BT_TRUE;
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
						result=BT_FALSE;
						warn("%s no es directorio",path);
						break;
					}
				} else {
					if( (errno!=ENOENT) || mkdir(path,mode)!=0){
						result=BT_FALSE;
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
		result=BT_FALSE;
	}
	return result;
}

//Executed in a child process. Should no return.
void extractLogosFromFileAndExit(const char *logos_tgz){
	const char *images="/etc/webif/www/images/";
	if (createParentFolders(images,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)){
		char *args[] = {"/bin/tar", "-C", (char*)images, "-xzf", (char*)logos_tgz, (char*)NULL};
		execv(args[0],args);
		//execv only returns if call fails
		perror("execvp");
		_exit(-1);
	} else {
		_exit(-2);
	}
}
	
boolean_t extractLogos(context_t *ctx, void (*extractLogosAndExit)(void)){
	boolean_t result=BT_FALSE;
	pid_t pid=vfork();
	if (pid<0) { 
		//fork failure
		warn("fallo la bifurcacion");
		result=BT_FALSE;
	} else if (pid==0) { 
		//child process
		extractLogosAndExit();
	} else { 
		//parent process
		int status;
		pid_t child_pid=waitpid(pid,&status,0);
		if (WIFEXITED(status)){
			int exitStatus;
			if ((exitStatus=WEXITSTATUS(status))==0){
				if (ctx!=NULL){
					printMessage(ctx,"message",tr("setup"),"Logos extraídos",BT_FALSE); //TODO i18n
				}
				info("Logos extraídos");
				webifConf.noLogos=BT_FALSE;
				result=BT_TRUE;
			} else {
				if (ctx!=NULL){
					printMessage(ctx,"alert",tr("setup"),"Error extrayendo logos",BT_FALSE); //TODO i18n
				}
				warn("Error descomprimiendo logos");
			}
		} else {
			if (ctx!=NULL){
				printMessage(ctx,"alert",tr("setup"),"Extracción de logos interrumpida",BT_FALSE); //TODO i18n
			}
			warn("child process signaled");
			result=BT_FALSE;
		}
	}
	return result;
}

boolean_t extractLogosFromFile(context_t *ctx, const char *logos_tgz){
	void extractLogosAndExit(void){
		//child process
		extractLogosFromFileAndExit(logos_tgz);
	}
	return extractLogos(ctx,extractLogosAndExit);
}

boolean_t extractLogosFromRequest(context_t *ctx, const char *fieldName){
	void extractLogosAndExit(void){
		//child process
		char localFilename[U_PATH_MAX]; 
		char clientFilename[U_PATH_MAX];
		char mimeType[MIME_TYPE_BUFSZ];
		size_t idx=0;
		size_t size;
		if (request_get_uploaded_file(ctx->request,fieldName,idx,localFilename,clientFilename,mimeType,&size)==0){
			if (localFilename[0]){
				extractLogosFromFileAndExit(localFilename);
			} else {
				warn("variable retornada de request_get_uploaded_file se ha anulado!!!");
				_exit(-3);
			}
		}
	}
	return extractLogos(ctx,extractLogosAndExit);
}


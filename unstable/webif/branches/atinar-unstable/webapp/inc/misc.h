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

#ifndef __MISC_H__
#define __MISC_H__

#include <time.h>
#include <klone/io.h>
#include <klone/session.h>
#include <klone/response.h>
#include <klone/request.h>
#define M3U_MIME_TYPE "audio/mpegurl"

typedef enum boolean_e {
	BT_FALSE=0,
	BT_TRUE=1 //importante positivo porque se usa como indice
} boolean_t;

typedef enum sortField_e {
	SF_NONE,
	SF_START,
	SF_TYPE,
	SF_TITLE,
	SF_RC_NUMBER,
	SF_CH_NUMBER,
	SF_TM_NUMBER,
	SF_SE_NUMBER,
	SF_NAME,
	SF_PRIORITY,
	SF_LIFETIME,
	SF_MUX,
	SF_HOST,
	SF_DIRECTORY
} sortField_t;

typedef enum sortDirection_e {
	SD_DESC=-1,
	SD_NONE,
	SD_ASC=1
} sortDirection_t;

typedef enum pageNumber_e {
	PN_INDEX,
	PN_PROGRAMS,
	PN_EPG_GRID,
	PN_CHANNELS,
	PN_TIMERS,
	PN_SEARCHES,
	PN_RECORDINGS,
	PN_SETTINGS,
	PN_FILEVIEW,
	PN_LIVE_STREAM,
	PN_LINKS
} pageNumber_t;

typedef enum pageAction_e {
	PA_NOACTION,
	PA_CONFIRM,
	PA_CANCEL,
	PA_BROWSE,
	PA_SUMMARY,
	PA_EDIT,
	PA_NEW,
	PA_ADD,
	PA_DELETE,
	PA_SHOW,
	PA_PLAY,
	PA_PRINT,
	PA_DOWNLOAD,
	PA_DOWNLOAD_ALL,
	PA_SAVE_CONFIG,
	PA_TIMER_CREATE_FROM_EVENT,
	PA_SEARCH_CREATE_FROM_EVENT,
	PA_GET_REC_INFO,
	PA_GET_EPG_GRID,
	PA_GET_TIMERS,
	PA_PARTITION_USAGE,
} pageAction_t;

typedef enum playlistType_e {
	PL_M3U,
	PL_XSPF
} playlistType_t;

typedef enum videoType_e {
	VT_UNKNOWN,
	VT_SD43,
	VT_SD169,
	VT_SD,
	VT_HD43,
	VT_HD169,
	VT_HD
} videoType_t;

typedef enum systemType_e {
	ST_UNKNOWN,
	ST_M740AV,
	ST_M750S,
	ST_M750C
} systemType_t;

typedef struct context_s {
	session_t *session;
	request_t *request;
	response_t *response;
	io_t *out;
	int ntabs;
	pageNumber_t currentPage;
	pageAction_t currentAction;
	int currentChannelNum;
	sortField_t sortBy;
	sortDirection_t sortDirection;
	int bufferLength;
	char *buffer;
	boolean_t isAjaxRequest;
	boolean_t isReload;
} context_t;

extern const char *checked[2];
extern const char *selected[2];
extern const char *videoTypeStr[7];
extern const char *classCurrent[2];
extern const char *classActive[2];
extern const char *tabs;
#define boolean(i) ((i)?BT_TRUE:BT_FALSE)
#define isFlagSet(flag,flags)   boolean(((flag) & (flags)) == (flag) )
#define hasFlag(flag,flags)     boolean(((flag) & (flags)) != 0 )  //one of the flags in 'flag' is set in 'flags'
#define setFlag(flag,flags)     ((flags) |=  (flag))
#define clearFlag(flag,flags)   ((flags) &= ~(flag))
#define setOrClearFlag(isSet,flag,flags)  if (isSet) {setFlag(flag,flags);} else {clearFlag(flag,flags);}
#define AJAX_REPLACE_PREFIX(id) ((ctx->isAjaxRequest)? "replace_" id : id)

#undef IGNORE_TABS
#ifdef IGNORE_TABS
	#define ctx_printf(ctx,str,...) io_printf(ctx->out,str,##__VA_ARGS__)
	#define ctx_printf0(ctx,str,...) io_printf(ctx->out,str,##__VA_ARGS__)
	#define ctx_printfn(ctx,str,pre,post,...) io_printf(ctx->out,str,##__VA_ARGS__)
	#define inctab(ctx)
	#define dectab(ctx)
#else
	#define ctx_printf(ctx,str,...) io_printf(ctx->out,str,##__VA_ARGS__)
	#define ctx_printf0(ctx,str,...) \
			{ \
				io_printf(ctx->out,"%.*s",ctx->ntabs,tabs); \
				io_printf(ctx->out,str,##__VA_ARGS__); \
			}
	#define ctx_printfn(ctx,str,pre,post,...) \
			{ \
				ctx->ntabs += pre; \
				if (ctx->ntabs>0) io_printf(ctx->out,"%.*s",ctx->ntabs,tabs); \
				io_printf(ctx->out,str,##__VA_ARGS__); \
				ctx->ntabs += post; \
			}
	#define inctab(ctx) ctx->ntabs++
	#define dectab(ctx) ctx->ntabs--
#endif

#define CTX_CHK_BUFFER(l) chkCtxBuffer(ctx,l,__FUNCTION__)
#define CTX_HTML_ENCODE(str,l) ctxHtmlEncode(ctx,str,l,__FUNCTION__)
#define CTX_URL_ENCODE(url,l,keep) ctxUrlEncode(ctx,url,l,keep,__FUNCTION__)
#define CTX_URL_DECODE(url) ctxUrlDecode(ctx,url,__FUNCTION__)

typedef void (*printHtmlHeadExtra_t)(context_t *ctx);

void initCtx(context_t *ctx, session_t *session, request_t *request, response_t *response, io_t *out, int bufferLength );
void freeCtx(context_t *ctx);
void chkCtxBuffer(context_t *ctx,int length, const char *routine);
char * ctxHtmlEncode(context_t *ctx, const char * const str, int l, const char *routine);
char * ctxUrlEncode(context_t *ctx, const char * const url, int l, const char * keep, const char *routine);
char * ctxUrlDecode(context_t *ctx, const char * const url, const char *routine);
boolean_t ctxGetRequestParamAsCpy(context_t *ctx, char **target, vars_t *args, const char *argName);
boolean_t isDST(time_t * aTime);
boolean_t sameDay(time_t oneDate,time_t anotherDate);
boolean_t sameString(const char * s1, const char * s2);
boolean_t sameInt(const int i1, const int i2);
boolean_t sameIntEx(const char * s, const int i);
boolean_t parseRequestStr(request_t *request, char ** pathStr, char ** queryStr);
char * strcatEx(char ** dest, const char * s);
boolean_t fileExists(const char * fileName);
void vdrDecode(char *dst, char *src, int l);
void vdrEncode(char *dst, char *src, int l);
void returnHttpNoContent(response_t *response);
void initHtmlDoc(context_t *ctx);
void initHtmlPage(context_t *ctx, const char *title, printHtmlHeadExtra_t printHtmlHeadExtra);
void initJavascript(context_t *ctx);
void finishJavascript(context_t *ctx);
void printMenu(context_t *ctx);
void printMessage(context_t *ctx, const char *cssClass, const char *title, const char *message, boolean_t encode);
void printList1TH(context_t *ctx, const char *page, sortField_t aSortField, const char *label);
void finishHtmlPage(context_t *ctx);

#endif

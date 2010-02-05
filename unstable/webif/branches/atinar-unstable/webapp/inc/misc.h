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

#include <stdbool.h>
#include <time.h>
#include <klone/io.h>
#include <klone/session.h>
#include <klone/response.h>
#include <klone/request.h>
#define M3U_MIME_TYPE "audio/mpegurl"

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
	PN_LINKS,
	PN_PLAYLIST_REC,
	PN_PLAYLIST_CHN,
	PN_NOW_NEXT,
	PN_STREAM_REC
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

typedef struct wcontext_s {
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
	bool isAjaxRequest;
	bool isReload;
} wcontext_t;

extern const char *checked[2];
extern const char *selected[2];
extern const char *videoTypeStr[7];
extern const char *classCurrent[2];
extern const char *classActive[2];
extern const char *tabs;
#define boolean(i) ((i)?true:false)
#define isFlagSet(flag,flags)   boolean(((flag) & (flags)) == (flag) )
#define hasFlag(flag,flags)     boolean(((flag) & (flags)) != 0 )  //one of the flags in 'flag' is set in 'flags'
#define setFlag(flag,flags)     ((flags) |=  (flag))
#define clearFlag(flag,flags)   ((flags) &= ~(flag))
#define setOrClearFlag(isSet,flag,flags)  if (isSet) {setFlag(flag,flags);} else {clearFlag(flag,flags);}
#define AJAX_REPLACE_PREFIX(id) ((wctx->isAjaxRequest)? "replace_" id : id)

#undef IGNORE_TABS
#ifdef IGNORE_TABS
	#define wctx_printf(wctx,str,...) io_printf(wctx->out,str,##__VA_ARGS__)
	#define wctx_printf0(wctx,str,...) io_printf(wctx->out,str,##__VA_ARGS__)
	#define wctx_printfn(wctx,str,pre,post,...) io_printf(wctx->out,str,##__VA_ARGS__)
	#define inctab(wctx)
	#define dectab(wctx)
#else
	#define wctx_printf(wctx,str,...) io_printf(wctx->out,str,##__VA_ARGS__)
	#define wctx_printf0(wctx,str,...) \
			{ \
				io_printf(wctx->out,"%.*s",wctx->ntabs,tabs); \
				io_printf(wctx->out,str,##__VA_ARGS__); \
			}
	#define wctx_printfn(wctx,str,pre,post,...) \
			{ \
				wctx->ntabs += pre; \
				if (wctx->ntabs>0) io_printf(wctx->out,"%.*s",wctx->ntabs,tabs); \
				io_printf(wctx->out,str,##__VA_ARGS__); \
				wctx->ntabs += post; \
			}
	#define inctab(wctx) wctx->ntabs++
	#define dectab(wctx) wctx->ntabs--
#endif

#define CTX_CHK_BUFFER(l) chkCtxBuffer(wctx,l,__FUNCTION__)
#define CTX_HTML_ENCODE(str,l) wctxHtmlEncode(wctx,str,l,__FUNCTION__)
#define CTX_URL_ENCODE(url,l,keep) wctxUrlEncode(wctx,url,l,keep,__FUNCTION__)
#define CTX_URL_DECODE(url) wctxUrlDecode(wctx,url,__FUNCTION__)

typedef void (*printHtmlHeadExtra_t)(wcontext_t *wctx);

bool initCtx(wcontext_t *wctx, pageNumber_t currentPage, session_t *session, request_t *request, response_t *response, io_t *out, int bufferLength );
void freeCtx(wcontext_t *wctx);
void chkCtxBuffer(wcontext_t *wctx,int length, const char *routine);
char * wctxHtmlEncode(wcontext_t *wctx, const char * const str, int l, const char *routine);
char * wctxUrlEncode(wcontext_t *wctx, const char * const url, int l, const char * keep, const char *routine);
char * wctxUrlDecode(wcontext_t *wctx, const char * const url, const char *routine);
char * wctxGetRequestParam(wcontext_t *wctx, vars_t *args, const char *argName, bool *isACopy);
bool isDST(time_t * aTime);
bool sameDay(time_t oneDate,time_t anotherDate);
bool sameString(const char * s1, const char * s2);
bool sameInt(const int i1, const int i2);
bool sameIntEx(const char * s, const int i);
bool parseRequestStr(request_t *request, char ** pathStr, char ** queryStr);
char * strcatEx(char ** dest, const char * s);
bool fileExists(const char * fileName);
void vdrDecode(char *dst, char *src, int l);
void vdrEncode(char *dst, char *src, int l);
void returnHttpNoContent(response_t *response);
void initHtmlDoc(wcontext_t *wctx);
void initHtmlPage(wcontext_t *wctx, const char *title, printHtmlHeadExtra_t printHtmlHeadExtra);
void initJavascript(wcontext_t *wctx);
void finishJavascript(wcontext_t *wctx);
void printMenu(wcontext_t *wctx);
void printMessage(wcontext_t *wctx, const char *cssClass, const char *title, const char *message, bool encode);
void printList1TH(wcontext_t *wctx, const char *page, sortField_t aSortField, const char *label);
void finishHtmlPage(wcontext_t *wctx);
bool extractEmbededFile(const char *src, const char *dst, bool overwrite);
bool createParentFolders(const char *path, mode_t mode);
bool extractLogosFromFile(wcontext_t *wctx, const char *logos_tgz/*,bool overwrite*/);
bool extractLogosFromRequest(wcontext_t *wctx, const char *fieldName);
#endif

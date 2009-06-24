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
	PN_CHANNELS,
	PN_TIMERS,
	PN_SEARCHES,
	PN_RECORDINGS,
	PN_SETTINGS,
	PN_FILEVIEW,
	PN_WATCHIT,
	PN_BROWSE,
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
	PA_SAVE_CONFIG
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

typedef struct webifState_e {
	pageNumber_t currentPage;
	pageAction_t currentAction;
	sortField_t sortBy;
	sortDirection_t sortDirection;
} webifState_t;

extern systemType_t systemType;
extern const char *checked[2];
extern const char *selected[2];
extern const char *videoTypeStr[7];
extern const char *classCurrent[2];
extern const char *classActive[2];
extern const char *tabs;
extern webifState_t webifState;
#define boolean(i) ((i)?BT_TRUE:BT_FALSE)
#define isFlagSet(flag,flags)   boolean(((flag) & (flags)) == (flag) )
#define hasFlag(flag,flags)     boolean(((flag) & (flags)) != 0 )  //one of the flags in 'flag' is set in 'flags'
#define setFlag(flag,flags)     ((flags) |=  (flag))
#define clearFlag(flag,flags)   ((flags) &= ~(flag))
#define setOrClearFlag(isSet,flag,flags)  if (isSet) {setFlag(flag,flags);} else {clearFlag(flag,flags);}

void config(session_t *session, request_t *request);
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
boolean_t makeTime(time_t *time, const char * date, const char * hour, const char * min );
void returnHttpNoContent(response_t *response);
int initHtmlDoc(response_t *response,io_t *out);
int initHtmlPage(response_t *response,io_t *out, const char *title, const char *headExtra, ...);
void printMenu(io_t *out, int ntabs);
void printMessage(io_t *out, int ntabs, const char *cssClass, const char *title, const char *message, char *const aux);
void printList1TH(io_t *out, int ntabs, const char *page, sortField_t aSortField, const char *label);
void finishHtmlPage(io_t *out, int ntabs);
char *condstrdup(int isFlagSet,const char *s);


#endif

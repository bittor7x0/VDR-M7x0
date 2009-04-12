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
	SF_NAME,
	SF_PRIORITY,
	SF_LIFETIME,
	SF_MUX
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

extern systemType_t systemType;
extern const char *checked[2];
extern const char *selected[2];
extern const char *videoTypeStr[7];
extern const char *classCurrent[2];
extern const char *classActive[2];
extern sortField_t sortBy;
extern sortDirection_t sortDirection;
extern pageNumber_t currentPage;
extern pageAction_t currentAction;

#define boolean(i) ((i)?BT_TRUE:BT_FALSE)
#define isFlagSet(flag,flags) boolean((flag & flags) == flag )
#define newSortDirection(sf) ((sf==sortBy)?-sortDirection:1)

void config(session_t *session, request_t *request);
boolean_t isDST(time_t * aTime);
boolean_t sameDay(time_t oneDate,time_t anotherDate);
boolean_t sameString(const char * s1, const char * s2);
boolean_t sameInt(const int i1, const int i2);
boolean_t sameIntEx(const char * s, const int i);
boolean_t parseRequestStr(request_t *request, char ** pathStr, char ** queryStr);
char * strcatEx(char ** dest, const char * s);
boolean_t fileExists(const char * fileName);
const char * sortClass(sortField_t sf);
void vdrDecode(char *dst, char *src);
boolean_t makeTime(time_t *time, const char * date, const char * hour, const char * min );
void initHtmlDoc(response_t *response,io_t *out);
void initHtmlPage(response_t *response,io_t *out, const char *title, const char *headExtra, ...);
void printMenu(io_t *out);
void finishHtmlPage(io_t *out);

#endif

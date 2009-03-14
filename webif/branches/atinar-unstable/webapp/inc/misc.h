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
	SF_ACTIVE,
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
	PA_PRINT,
	PA_DOWNLOAD,
	PA_DOWNLOAD_ALL
} pageAction_t;

typedef enum playlistType_e {
	PL_M3U,
	PL_XSPF
} playlistType;

extern int isM740AV;
extern int isM750S;
extern int isM750C;
extern int boxSysType;
extern const int sysNone;
extern const int sys740av;
extern const int sys750s;
extern const int sys750c;

extern const int httpPort;

extern const char *checked[2];
extern const char *selected[2];
extern const char *arStr[7];
extern const char *classCurrent[2];
extern sortField_t sortBy;
extern sortDirection_t sortDirection;
extern pageNumber_t currentPage;
extern pageAction_t currentAction;
extern char server_ip[16];
extern uint16_t server_port;
extern char myip[16];

#define boolean(i) ((i)?BT_TRUE:BT_FALSE)
#define newSortDirection(sf) ((sf==sortBy)?-sortDirection:1)

void config(session_t *session, request_t *request);
boolean_t vdrRunning();
boolean_t isDST(time_t * aTime);
boolean_t sameDay(time_t oneDate,time_t anotherDate);
boolean_t sameString(const char * s1, const char * s2);
boolean_t sameInt(const int i1, const int i2);
boolean_t sameIntEx(const char * s, const int i);
boolean_t parseRequestStr(request_t *request, char ** pathStr, char ** queryStr);
char * strcatEx(char ** dest, const char * s);
boolean_t fileExists(const char * fileName);
const char * sortClass(sortField_t sf);
char * htmlEncode(const char * const s);
void vdrDecode(char *dst, char *src);
boolean_t makeTime(time_t *time, const char * date, const char * hour, const char * min );
void printDoctypeOpenHtml(io_t *out);
void printXhtmlHead(response_t *response,io_t *out, const char *title, const char *headExtra, ...);
void printMenu(io_t *out);
void printFooter(io_t *out);

#endif

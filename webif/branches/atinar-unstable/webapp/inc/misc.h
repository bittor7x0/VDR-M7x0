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

extern int isM740AV;
extern int isM750S;
extern int isM750C;
extern int boxSysType;
extern const int sysNone;
extern const int sys740av;
extern const int sys750s;
extern const int sys750c;

extern const int httpPort;

extern const int ttOnce;
extern const int ttPeriodic;

extern const int sortNone;
extern const int sortStart;
extern const int sortStop;
extern const int sortType;
extern const int sortTitle;
extern const int sortChannelNum;
extern const int sortChannelName;
extern const int sortPriority;
extern const int sortLifetime;
extern const int sortActive;
extern const int sortMux;

extern const int sortAsc;
extern const int sortDesc;

extern const char checked[2][8];
extern const char selected[2][9];
extern const char arStr[7][8];

int isDST(time_t * aTime);
int sameDay(time_t oneDate,time_t anotherDate);
int sameString(const char * s1, const char * s2);
int sameInt(const int i1, const int i2);
int sameIntEx(const char * s, const int i);
int parseRequestStr(const char * requestStr, char ** pathStr, char ** queryStr);
char * strcatEx(char ** dest, const char * s);

void getConfig();
int fileExists(const char * fileName);
int legalPath(char * pathName);
int vdrRunning();

enum sortField {
	SF_NONE,
	SF_START,
	//SF_STOP,
	SF_TYPE=3,
	SF_TITLE,
	SF_NUMBER,
	SF_NAME,
	SF_PRIORITY,
	SF_LIFETIME,
	SF_ACTIVE,
	SF_MUX
};

enum sortDirection {
	SD_DESC=-1,
	SD_NONE,
	SD_ASC=1
};

enum pageNumber {
	PN_INDEX = 4,
	PN_PROGRAMS,
	PN_CHANNELS,
	PN_TIMERS,
	PN_RECORDINGS,
	PN_SETTINGS,
	PN_BROWSE=92,
	PN_LINKS= 96
};

extern enum sortField sortBy;
extern enum sortDirection sortDirection;
extern enum pageNumber currentPage;

extern const char * action;
extern const char * SUMMARY;
extern const char * EDIT;
extern const char * NEW;
extern const char * ADD;
extern const char * DELE;
extern const char * NOACTION;

extern char server_ip[16];
extern uint16_t server_port;
extern char myip[16];
extern char * newt;


const char * sortClass(enum sortField sf);
#define printLink(sf) io_printf(out,"%s?sort=%d&direction=%d",SCRIPT_NAME,sf,(sf==sortBy)?-sortDirection:1)
#endif

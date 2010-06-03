/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* Originally written for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __BROWSE_H__
#define __BROWSE_H__

#include "i18n.h"
#include "misc.h"

enum recDirFlags_e { //also used to sort recordings
	DF_ISREC   = 0x0001,
	DF_EDITED  = 0x0002,
	DF_DELETED = 0x0004,
	DF_HASRECS = 0x0008,
	DF_ISDIR   = 0x0010,
};

typedef struct recDir_s recDir_t;

typedef struct recDirList_s {
	int length;
	recDir_t *entry;
} recDirList_t;

struct recDir_s {
	int hostId;
	char *name;
	char *dname;
	int numRecs;
	int numDirs;
	int flags;
	time_t start;
	recDirList_t subdirs;
};

void initRecDir(recDir_t *rdir);
void freeRecDir(recDir_t *rdir);
void initRecDirList(recDirList_t *rdirs);
void freeRecDirList(recDirList_t *rdirs);
void getRecDirList(const char * const path, recDirList_t * const rdirs,bool onlyLocal);
void printRecDirListUL(wcontext_t *wctx,const char *const path,int pl, recDirList_t *rdirs,int level);
void printRecDir(wcontext_t *wctx,const char *const path,int pl,recDir_t *rdir,int level);
void printRecPathUl(wcontext_t *wctx,const char *path);

#endif

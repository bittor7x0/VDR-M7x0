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

#include <dirent.h>
#include <errno.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "recording.h"
#include "svdrp_comm.h"

void initFE(fragmentEntry_t *const entry){
	entry->path=NULL;
	entry->size=0;
	entry->start=0;
	entry->end=0;
}

void freeFE(fragmentEntry_t *const entry){
	free(entry->path);
	initFE(entry);
}

void initFL(fragmentList_t *const list){
	list->length=0;
	list->fragNum=0;
	list->fragPos=0;
	list->entry=NULL;
	list->totalSize=0;
}
	
void freeFL(fragmentList_t *const list){
	int i;
	for(i=0;i<list->length;i++){
		freeFE(&list->entry[i]);
	}
	free(list->entry);
	initFL(list);
}

void debugFragmentList(const char * const label, fragmentList_t * const fragments){
	int i;
	fragmentEntry_t *f;
	for(i=0,f=fragments->entry; i<fragments->length; i++,f++){
		info("%s i[%d] start[%llu] end[%llu]",label,i,f->start,f->end);
	}
}

bool seekFragment(fragmentList_t * const list, uint64_t pos){
	fragmentEntry_t * f;
	int i;
	//dbg("pos[%llu]",pos);
	//dbg("length[%d],totalSize[%llu]",list->length,list->totalSize);
	for(i=0,f=list->entry; i<list->length; i++,f++){
		//dbg("i[%d] start[%llu] end[%llu]",i,f->start,f->end);
		if (pos>=f->start && pos<=f->end) {
			list->fragPos=pos-f->start;
			list->fragNum=i;
			return true;
		}
	}
	return false;
}

int isVideoFile(const struct dirent * ent){
	char *c;
	char *c0=(char *)ent->d_name;
	for (c=c0;c<c0+3;c++) if (!isdigit(*c)) return 0;
	return (strcmp(c,".vdr")==0);
}

void addRecFragments(fragmentList_t * const fragments, const char * const fullpath){
	struct stat st;
	fragmentEntry_t *f;
	int i;
	if ( (stat(fullpath, &st)==0) && S_ISDIR(st.st_mode)) {
		struct dirent **namelist;
		int n=scandir(fullpath, &namelist, isVideoFile, alphasort);
		if (n>0) {
			fragments->entry=(fragmentEntry_t*)realloc(fragments->entry,(fragments->length+n)*sizeof(fragmentEntry_t));
			crit_goto_if(fragments->entry==NULL,outOfMemory);
			for (i=0, f=fragments->entry+fragments->length ; i<n ; i++, f++) {
				initFE(f);
				crit_goto_if(asprintf(&(f->path),"%s/%s",fullpath,namelist[i]->d_name)<0,outOfMemory);
				if (stat(f->path,&st)==0) {
					f->size=st.st_size;
					f->start=fragments->totalSize;
					f->end=f->start+f->size-1;
					//dbg("i[%d] start[%llu] end[%llu]",i,f->start,f->end);
					fragments->totalSize=f->end+1;
				} else {
					warn("file %s error %s",f->path,strerror(errno));
					freeFE(f);
				}
			}
			fragments->length+=n;
			for (i=0;i<n;i++) free(namelist[i]);
			free(namelist);
		}
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void addRecDirListFragments(fragmentList_t * const fragments, const char * const path, const recDirList_t *rdirs);

void addRecDirFragments(fragmentList_t * const fragments, const char * const path, const recDir_t *rdir){
	if (isFlagSet(DF_ISREC,rdir->flags)){
		char * fullpath;
		hostConf_t *host=getHost(rdir->hostId);
		if (!host || !host->video0 || host->video0[0]!='/') {
			warn("Recording host %d is not local or video0 %s not valid", rdir->hostId, (host)?host->video0:"null");
			return;
		}
		crit_goto_if(asprintf(&fullpath,"%s/%s",host->video0,path)<0,outOfMemory);
		addRecFragments(fragments,fullpath);
		free(fullpath);
	} else if (isFlagSet(DF_ISDIR,rdir->flags)) {
		addRecDirListFragments(fragments,path,&rdir->subdirs);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void addRecDirListFragments(fragmentList_t * const fragments, const char * const path, const recDirList_t *rdirs){
	int i;
	recDir_t *rdir;
	for(i=0,rdir=rdirs->entry;i<rdirs->length;i++,rdir++){
		char *path2;
		crit_goto_if(asprintf(&path2,"%s/%s",path,rdir->name)<0,outOfMemory);
		addRecDirFragments(fragments,path2,rdir);
		free(path2);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void getFragmentList(fragmentList_t * const fragments, const char * const path ){
	recDirList_t rdirs;
	initRecDirList(&rdirs);
	getRecDirList(path,&rdirs,true);
	if (rdirs.length){
		addRecDirFragments(fragments,path,rdirs.entry);
	}
	freeRecDirList(&rdirs);
	//debugFragmentList("getFragmentList",fragments);
}




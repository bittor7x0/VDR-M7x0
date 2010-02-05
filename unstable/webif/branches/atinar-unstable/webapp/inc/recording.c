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

bool seekFragment(fragmentList_t * const list, uint64_t pos){
	fragmentEntry_t * f;
	int i;
	list->fragNum=-1;
	for(i=0; i<list->length; i++){
		f=&list->entry[i];
		if (pos>=f->start && pos<=f->end) {
			list->fragPos=pos-f->start;
			list->fragNum=i;
			break;
		}
	}
	return boolean(list->fragNum>-1);
}

int isVideoFile(const struct dirent * ent){
	char *c;
	char *c0=(char *)ent->d_name;
	for (c=c0;c<c0+3;c++) if (!isdigit(*c)) return 0;
	return (strcmp(c,".vdr")==0);
}

bool getFragmentList(fragmentList_t * const list, const rec_t *rec){
	initFL(list);
	if (rec->path==NULL) {
		warn("Path should be specified");
		return false;
	}
	hostConf_t *host=getHost(rec->hostId);
	if (!host || !host->video0 || host->video0[0]!='/') {
		warn("Recording host %d is not local or video0 %s not valid", rec->hostId, (host)?host->video0:"null");
		return false;
	}
	struct stat st;
	fragmentEntry_t *f;
	int i;
	char * fullpath;
	if (rec->path[0]=='/'){
		warn("Absolute recording path [%s]",rec->path);
		fullpath=rec->path;
	}
	else {
		crit_goto_if(asprintf(&fullpath,"%s/%s",host->video0,rec->path)<0,outOfMemory);
	}
	if ( (stat(fullpath, &st)==0) && S_ISDIR(st.st_mode)) {
		struct dirent **namelist;
		list->length = scandir(fullpath, &namelist, isVideoFile, alphasort);
		if (list->length<0) list->length=0;
		if (list->length>0) {
			list->entry=(fragmentEntry_t*)malloc((list->length)*sizeof(fragmentEntry_t));
			crit_goto_if(list->entry==NULL,outOfMemory);
			for (i=0;i<list->length;i++) {
				f=&list->entry[i];
				initFE(f);
				crit_goto_if(asprintf(&(f->path),"%s/%s",fullpath,namelist[i]->d_name)<0,outOfMemory);
				if (stat(f->path,&st)==0) {
					f->size=st.st_size;
					f->start=list->totalSize;
					f->end=f->start+f->size-1;
					list->totalSize=f->end+1;
				} else {
					warn("file %s error %s",f->path,strerror(errno));
					freeFE(f);
				}
			}
			for (i=0;i<list->length;i++) free(namelist[i]);
			free(namelist);
		}
	}
	if (fullpath!=rec->path) free(fullpath);
	return boolean(list->length>0 && list->totalSize>0);
outOfMemory:
	crit("Out of memory");
	exit(1);
}




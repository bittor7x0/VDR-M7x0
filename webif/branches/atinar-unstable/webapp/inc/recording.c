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

boolean_t seekFragment(fragmentList_t *list, uint64_t pos){
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

void getFragmentsFromSVDRP(fragmentList_t * const list, int recId){
	char *cmd=NULL;
	if (asprintf(&cmd, "PLST %d\r", recId)==-1){
		error("getFragmentsFromSVDRP:Out of memory");
		exit(1);
	}
	int error=(write_svdrp(cmd)<=0);
	free(cmd);
	if (error){
		return;
	}
	char *data=read_svdrp();
	close_svdrp();
	if (data!=NULL){
		int l;
		const char *p=data;
		fragmentEntry_t * tmp,*f;
		while(*p) {
			p+=strspn(p,"\r\n ");
			errno=0;
			int code=(int)strtol(p,&p,10);
			if (errno) {
				warn("Bad format in data");
				break;
			}
			p+=strspn(p,"- ");
			l=strcspn(p,"\r\n");
			if (code==215) {
				tmp=(fragmentEntry_t*)realloc(list->entry,(list->length+1)*sizeof(fragmentEntry_t));
				if( tmp==NULL) {
					break;
				}
				list->entry=tmp;
				f=&list->entry[list->length];
				initFE(f);
				f->path=strndup(p,l);
				if (f->path==NULL){
					break;
				}
				list->length++;
			} else {
				warn("Bad format in data: %s", data);
				break;
			}
			p+=l;
		}
		free(data);
	}
}


int isVideoFile(const struct dirent * ent){
	char *c;
	char *c0=(char *)ent->d_name;
	for (c=c0;c<c0+3;c++) if (!isdigit(*c)) return 0;
	return (strcmp(c,".vdr")==0);
}

void getFragmentsFromDir(fragmentList_t * const list, const char *path){
	struct stat st;
	if ( (stat(path, &st)==0) && S_ISDIR(st.st_mode)) {
		struct dirent **namelist;
		list->length = scandir(path, &namelist, isVideoFile, alphasort);
		if (list->length<0) list->length=0;
		if (list->length>0) {
			list->entry=(fragmentEntry_t*)malloc((list->length)*sizeof(fragmentEntry_t));
			if (list->entry==NULL){
				error("getFragmentsFromDir:Out of memory");
				exit(1);
			}
			fragmentEntry_t *f;
			int i;
			for (i=0;i<list->length;i++) {
				f=&list->entry[i];
				initFE(f);
				if (asprintf(&(f->path),"%s/%s",path,namelist[i]->d_name)==-1)  {
					error("getFragmentsFromDir:Out of memory");
					exit(1);
				}				
			}
			for (i=0;i<list->length;i++) free(namelist[i]);
			free(namelist);
		}
	}
}

boolean_t getFragmentList(fragmentList_t * const list, const char *recPath, int recId){
	initFL(list);
	if (recPath!=NULL) {
		getFragmentsFromDir(list,recPath);
	} else if (recId>0){
		getFragmentsFromSVDRP(list,recId);
	} else {
		warn("RecPath or recId should be specified");
		return BT_FALSE;
	}
	struct stat st;
	fragmentEntry_t *f;
	int i;
	for (i=0;i<list->length;i++) {
		f=&list->entry[i];
		if (f->path!=NULL && stat(f->path,&st)==0) {
			f->size=st.st_size;
			f->start=list->totalSize;
			f->end=f->start+f->size-1;
			list->totalSize=f->end+1;
		} else {
			//TODO warn()
			freeFE(f);
		}
	}
	info("Number of fragments: %d. Total size %lld bytes",list->length,list->totalSize);
	return boolean(list->length>0 && list->totalSize>0);
}




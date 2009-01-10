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


void freeRF(recFragment * fragment, int fragnum){
	int i;
	for(i=0;i<fragnum;i++){
		free(fragment[i].path);
	}
	free(fragment);
}

int whichFragment(recFragment * fragment, int fragnum, uint64_t seek, uint64_t *position){
	uint64_t start=0; //first byte position of current fragment
	uint64_t end=0;  //last byte position of current fragment
	recFragment * f;
	int i;
	for(i=0; i<fragnum; i++){
		f=&fragment[i];
		end=start+f->size-1;
		if (seek>=start && seek<=end ) {
			(*position)=seek-start;
			info("fragment[%d]:[%lld,%lld] match seek=%lld from position=%lld",i,start,end,seek,*position);
			return i;
		}
		start=end+1;
	}
	return -1;
}

recFragment * parseRecData(const char *recdata, int *fragnum, uint64_t *total_size) {
	dbg("parseRecData(%s,fragnum,total_size",recdata);
	char *p;
	int l;
	recFragment * fragment=NULL;
	recFragment * tmp;
	struct stat st;
	(*total_size)=0;
	(*fragnum)=0;
	if (recdata==NULL){
		return NULL;
	}
	p=(char *)recdata;
	while(p && *p) {
		p+=strspn(p,"\r\n ");
		errno=0;
		int code=(int)strtol(p,&p,10);
		if (errno) {
			warn("Bad format in recdata");
			break;
		}
		p+=strspn(p,"- ");
		l=strcspn(p,"\r\n");
		if ( code==215 ) {
			(*fragnum)++;
			tmp=(recFragment*)realloc(fragment,(*fragnum)*sizeof*fragment);
			if( tmp==NULL) {
				break;
			}
			fragment=tmp;
			tmp=&fragment[(*fragnum)-1];
			tmp->path=strndup(p,l);
			stat(tmp->path, &st);
			tmp->size=st.st_size;
			/*
			info("fragment[%d] ",(*fragnum)-1);
			info("start=%lld",*total_size);
			info("size=%lld",tmp->size);
			info((const char *)tmp->path);
			*/
			(*total_size)+=tmp->size;
		}
		p+=l;
	}
	info("Number of fragments: %d. Total size %lld bytes",*fragnum,*total_size);
	return fragment;
}

const char * getRecData(session_t *session, int id) {
	char *recdata;
#ifdef KEEP_PLST_IN_SESSION
	char *recid;
	/*
	 * vlc doesn't seem to keep the session_id, so this doesn't work.
	 */
	info("getRecData(session=%p,id=%d)",session,id);
	*recid=session_get(session,"recid");
	*recdata=session_get(session,"recdata");
	info("3 session recid=%p",recid);
	info("3 session recdata=%p",recdata);
	if (recid!=NULL){
		if (id!=(int)atol(recid)){
			session_del(session,"recid");
			session_del(session,"recdata");
			recid=NULL;
			recdata=NULL;
			info("Session variables deleted");
		}
	}
	else {
		recdata=NULL;
	}
#endif
	if (recdata==NULL){
		char *cmd=NULL;
		if (asprintf(&cmd, "PLST %d\r", id)==-1){
			error("No hay memoria");
			return NULL;
		}
		int error=(write_svdrp(cmd)<=0);
		free(cmd);
		if (error){
			return NULL;
		}
		cmd=NULL;
		recdata=read_svdrp();
		close_svdrp();
#ifdef KEEP_PLST_IN_SESSION
		if (recdata){
			if (asprintf(&recid,"%d",id)!=-1) {
				session_set(session,"recid",recid);
				session_set(session,"recdata",recdata);
			}
		}
#endif
	}
	return recdata;
}

int isVideoFile(const struct dirent * ent){
	char *c;
	char *c0=(char *)ent->d_name;
	for (c=c0;c<c0+3;c++) if (!isdigit(*c)) return 0;
	return (strcmp(c,".vdr")==0);
}

recFragment * getFragmentsDir(const char *path, int *fragnum, uint64_t *total_size){
	recFragment *fragment=NULL, *tmp;
	*fragnum=0;
	*total_size=0;
	if (path==NULL){
		return NULL;
	}
	struct stat st;
	
	if ( (stat(path, &st)==0) && S_ISDIR(st.st_mode)) {
		struct dirent **namelist;
		*fragnum = scandir(path, &namelist, isVideoFile, alphasort);
		if (*fragnum<0) *fragnum=0;
		if (*fragnum>0) {
			int i;
			fragment=(recFragment*)malloc((*fragnum)*sizeof*fragment);
			if (fragment==NULL){
				for (i=0;i<*fragnum;i++) free(namelist[i]);
				free(namelist);
				*fragnum=0;
				return;
			}
			for (i=0;i<*fragnum;i++) {
				tmp=&fragment[i];
				tmp->path=NULL;
				tmp->size=0;
				if ((asprintf(&(tmp->path),"%s/%s",path,namelist[i]->d_name)!=-1) && (stat(tmp->path,&st)==0)) {
					tmp->size=st.st_size;
					(*total_size)+=tmp->size;
				}				
				free(namelist[i]);
			}
			free(namelist);
		}
	}
	return fragment;
}



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

#include <klone/utils.h>
#include <time.h>
#include <dirent.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>

#include "browse.h"
#include "conf.h"
#include "epg.h"
#include "recordings.h"
#include "svdrp_comm.h"

const int flagsCompareMask=DF_ISDIR|DF_HASRECS|DF_ISREC;
const char *classIsRec[]={""," recRecRec"};
const char *classIsDel[]={""," recRecDel"};
const char *classIsEdt[]={""," recRecEdt"};

void initRecDir(recDir_t *rdir){
	rdir->hostId=-1;
	rdir->name=NULL;
	rdir->dname=NULL;
	rdir->numRecs=0;
	rdir->numDirs=0;
	rdir->flags=0;
	initRecDirList(&rdir->subdirs);
}

void freeRecDir(recDir_t *rdir){
	free(rdir->name);
	free(rdir->dname);
	freeRecDirList(&rdir->subdirs);
	initRecDir(rdir); 
}

void initRecDirList(recDirList_t *rdirs){
	rdirs->length=0;
	rdirs->entry=NULL;
}

void freeRecDirList(recDirList_t *rdirs){
	int i=0;
	recDir_t *rdir;
	for(i=0,rdir=rdirs->entry;i<rdirs->length;i++,rdir++){
		freeRecDir(rdir);
	}
	free(rdirs->entry);
}

int compareRecDir(const void * a, const void * b) {
	const recDir_t * ta=(const recDir_t *)a;
	const recDir_t * tb=(const recDir_t *)b;
	int taflags=ta->flags&flagsCompareMask;
	int tbflags=tb->flags&flagsCompareMask;
	return (taflags!=tbflags)
	? taflags-tbflags
	: strcmp((ta->dname)?ta->dname:ta->name,(tb->dname)?tb->dname:tb->name);
}

char * vdrDecodeName(char *name, boolean_t inPlace, boolean_t *isEdt){
	char *dname=(inPlace) ? name : strdup(name);
	char *s;
	s=name;
	if (s[0]=='%'){
		if (isEdt) *isEdt=BT_TRUE;
		s++;
	} else {
		if (isEdt) *isEdt=BT_FALSE;
	}
	vdrDecode(dname,s,strlen(s));
	return dname;
}

void prepareRecDir(recDir_t * const rdir){
	if (rdir->subdirs.length>0){
		qsort(rdir->subdirs.entry,rdir->subdirs.length,sizeof(recDir_t),compareRecDir);
		int i;
		recDir_t *rdir2;
		boolean_t allDeleted=BT_TRUE;
		for (i=0,rdir2=rdir->subdirs.entry;i<rdir->subdirs.length;i++,rdir2++){
			prepareRecDir(rdir2);
			if (!isFlagSet(DF_DELETED,rdir2->flags)) {
				allDeleted=BT_FALSE;
			}
			if (isFlagSet(DF_ISREC,rdir2->flags)) {
				rdir->numRecs++;
				rdir->flags|=DF_HASRECS;
			}
			if (isFlagSet(DF_ISDIR,rdir2->flags) && !isFlagSet(DF_HASRECS,rdir2->flags)) {
				rdir->numDirs++;
			}
			rdir->numRecs+=rdir2->numRecs;
			rdir->numDirs+=rdir2->numDirs;
		}
		if (allDeleted) {
			rdir->flags|=DF_DELETED;
		}
	}
}

recDir_t *addSubdir(const char * name, const hostConf_t * const host, recDirList_t * const rdirs) {
	recDir_t *rdir2;
	if ( (name[0]=='.' && (name[1]==0 || (name[1]=='.' && name[2]==0)))  ){
		return NULL;
	}
	int i;
	boolean_t found=BT_FALSE;
	for (i=0,rdir2=rdirs->entry;i<rdirs->length;i++,rdir2++){
		if (strcmp(name,rdir2->name)==0){
			found=BT_TRUE;
			break;
		}
	}
	const char *ext=strrchr(name,'.');
	boolean_t isRec=boolean(ext && strcmp(ext,".rec")==0);
	boolean_t isDel=boolean(ext && !isRec && strcmp(ext,".del")==0);
	boolean_t isEdt=BT_FALSE;
	if (!found || isRec || isDel) {
		rdirs->length++;
		rdirs->entry=(recDir_t *)realloc(rdirs->entry,rdirs->length*sizeof(recDir_t));
		crit_goto_if(rdirs->entry==NULL,outOfMemory);
		rdir2=rdirs->entry+(rdirs->length-1);
		initRecDir(rdir2);
		rdir2->name=strdup(name);
		crit_goto_if(rdir2->name==NULL,outOfMemory);
	}
	if ( isRec || isDel ) {
		rdir2->flags|=DF_ISREC;
		rdir2->hostId=host->id;
		if (isDel) rdir2->flags|=DF_DELETED;
	} else {
		if (!found){
			rdir2->flags |= DF_ISDIR;
			rdir2->dname=vdrDecodeName((char *)name,BT_FALSE,&isEdt);
			if (isEdt) rdir2->flags|=DF_EDITED;
		}
	}
	return rdir2;
outOfMemory:
	crit("addSubdir:out of memory");
	exit(1);
}

void addSubdirs(const char * const fullpath, const hostConf_t * const host, recDir_t * const rdir) {
	DIR *dir = opendir(fullpath);
	if (dir == 0) {
		return;
	}
	struct dirent* dirE;
	recDir_t *rdir2;
	recDirList_t *rdirs=&rdir->subdirs;
	while(0 != (dirE = readdir(dir))) {
		if (dirE->d_type==DT_DIR) {
			rdir2=addSubdir(dirE->d_name,host,rdirs);
			if (rdir2 && isFlagSet(DF_ISDIR,rdir2->flags)){
				char *fullpath2;
				crit_goto_if(asprintf(&fullpath2,"%s/%s",fullpath,rdir2->name)<0,outOfMemory);
				addSubdirs(fullpath2,host,rdir2);
				free(fullpath2);
			}
		}
	}
	return;
outOfMemory:
	crit("addSubdirs:out of memory");
	exit(1);
}

void addRemoteRecs(const char * const path, hostConf_t *host, recDir_t *rdir) {
	int l=(path)?strlen(path):0;
	char *data=execSvdrp(host,"LSTR path");
	if (data){
		recDir_t *rdir2;
		char *r, *sr;
		for(r=strtok_r(data,"\r\n",&sr);r!=0;r=strtok_r(0,"\r\n",&sr)) {
			int code=strtol(r,&r,10);
			if (code==250) {
				r++;
				rdir2=rdir;
				char *f,*sf;
				int i;
				for(f=strtok_r(r," ",&sf),i=0;f && rdir2;f=strtok_r(0," ",&sf),i++) {
					if(i==1) {
						if (f[0]=='/'){
							//TODO quitar video0
						}
						if (!path || (strncmp(path,f,l)==0)){
							f+=l;
							if (f[0]=='/') f++;
							char *d,*sd;
							for(d=strtok_r(f,"/",&sd);d;d=strtok_r(0,"/",&sd)) {
								rdir2=addSubdir(d,host,&rdir2->subdirs);
								if (rdir2==NULL) break;
							}
						}
						break;
					}
				}
			}
		}
		free(data);
	}
	return;
outOfMemory:
	crit("addRemoteSubdirs:out of memory");
	exit(1);
}

void getRecDir(const char * const path, recDir_t * const rdir) {
	int h;
	hostConf_t *host;
	boolean_t parsed[webifConf.hostsLength];
	for (h=0;h<webifConf.hostsLength;h++) 
		parsed[h]=BT_FALSE;
	for (h=0,host=webifConf.hosts;h<webifConf.hostsLength;h++,host++){
		if (!parsed[h] && host->video0[0]){
			char *fullpath;
			if (path && *path) {
				crit_goto_if(asprintf(&fullpath,"%s/%s",host->video0,path)<0,outOfMemory);
			} else  {
				crit_goto_if((fullpath=strdup(host->video0))==NULL,outOfMemory);
			}
			addSubdirs(fullpath,host,rdir);
			free(fullpath);
			parsed[h]=BT_TRUE;
			int h2;
			hostConf_t *host2 = host+1;
			for (h2=h+1;h2<webifConf.hostsLength;h2++,host2++) 
				if (!parsed[h2] && host2->video0[0] && strcmp(host->video0,host2->video0)==0)
					parsed[h2]=BT_TRUE;
		} else {
			addRemoteRecs(path,host,rdir);
		}
	}
	prepareRecDir(rdir);
	return;
outOfMemory:
	crit("getRecDir:out of memory");
	exit(1);
}

const char * getPrintName(recDir_t *rdir,char *const aux){
	if (isFlagSet(DF_ISREC,rdir->flags)){
		struct tm dates;
		strptime(rdir->name,"%Y-%m-%d.%H.%M",&dates);
		strcpy(aux,formatDate(&dates,BT_TRUE));
		return aux;
	} else {
		return (rdir->dname!=NULL)?rdir->dname:rdir->name;
	}
}

//TODO usar printEvent
void printRecInfo(io_t *out, int ntabs, const rec_t * const rec){
	int minutes = rec->event.duration/60;
	io_printf(out,"%.*s<div class=\"recInfo\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<div class=\"recDate\">",ntabs++,tabs);
	io_printf(out,"%.*s"
		"<span class=\"recStart field\">"
			"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
		"</span>\n",ntabs,tabs,tr("date"),formatDate(localtime(&rec->event.start),1));
	io_printf(out,"%.*s"
		"<span class=\"recDuration field\">"
			"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%d&#39;</span>"
		"</span>\n",ntabs,tabs,tr("runtime"),minutes);
	io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	if (webifConf.displayHostId && webifConf.hostsLength>1){
		hostConf_t *host=getHost(rec->hostId);
		if (host){
			io_printf(out,"%.*s<div class=\"recHost\">",ntabs++,tabs);
			io_printf(out,"%.*s"
				"<span class=\"field\">"
					"<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
				"</span>\n",ntabs,tabs,"Host",(host->name[0])?host->name:(host->ip[0])?host->ip:"?");
			io_printf(out,"%.*s</div>\n",--ntabs,tabs);
		}
	}
	if (rec->event.desc && rec->event.desc[0]) {
		io_printf(out,"%.*s<div class=\"desc\">\n",ntabs++,tabs);
		printEventDesc(out,ntabs,rec->event.desc,"|",NULL);
		io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	}
	io_printf(out,"%.*s</div>\n",--ntabs,tabs);
}


void printRecording(io_t *out,int ntabs,int pl,recDir_t *rdir, rec_t *const rec, char *const aux,int level){
	if (level==0){
		io_printf(out,"%.*s<li class=\"recFolderWrapper\">\n",ntabs++,tabs);
		io_printf(out,"%.*s<div class=\"recFolder%s%s\">\n",ntabs++,tabs
			,classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
		io_printf(out,"%.*s<div class=\"recFolderHeader\">\n",ntabs++,tabs);
	} else {
		io_printf(out,"%.*s<li class=\"recRec%s%s\">\n",ntabs++,tabs
			,classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
	}
	io_printf(out,"%.*s<form action=\"/browse.kl1\" method=\"get\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<input type=\"hidden\" name=\"pl\" value=\"%d\" />\n",ntabs,tabs,pl);
	if (!isFlagSet(DF_DELETED,rdir->flags)){
		printRecControls(out,ntabs,rec,aux,tr("play"),tr("recEdit"),tr("recDelete"));
	}
	io_printf(out,"%.*s</form>",--ntabs,tabs);
	//if (webifConf.displayHostId) {
	//	io_printf(out,"%.*s%s <span class=\"host\">[h%d]</span>\n",ntabs,tabs,getPrintName(rdir,aux),rec->hostId);
	//} else {
	io_printf(out,"%.*s%s\n",ntabs,tabs,getPrintName(rdir,aux));
	//}
	if (level==0){
		io_printf(out,"%.*s</div>\n",--ntabs,tabs);
		setFlag(RE_REMOTE_INFO_PENDING,rec->flags); //don't read info if host is remote
		if (getRecInfo(rec)) {
			if (isFlagSet(RE_REMOTE_INFO_PENDING,rec->flags)){
				io_printf(out,"%.*s<div class=\"recFolderDetails recFolderDetailsPending\"></div>\n",ntabs++,tabs);
			} else {
				io_printf(out,"%.*s<div class=\"recFolderDetails\">\n",ntabs++,tabs);
				printRecInfo(out,ntabs,rec);
				io_printf(out,"%.*s</div>\n",--ntabs,tabs);
			}
		}
		io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	}
	io_printf(out,"%.*s</li>\n",--ntabs,tabs);
	return;
outOfMemory:
	crit("printRecording:out of memory");
	exit(1);
}

void printFolder(io_t *out,int ntabs,const char *const path,int pl,recDir_t *rdir,char *const aux,int level){
	if (rdir->numRecs==0) return; //TODO parametro de configuracion?
	const char * const Browse=tr("browse");
	if (level==0) {
		io_printf(out,"%.*s<li class=\"recFolderWrapper\">\n",ntabs++,tabs);
		io_printf(out,"%.*s<div class=\"recFolder%s%s\">\n",ntabs++,tabs
			,classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
	} else {
		io_printf(out,"%.*s<li class=\"recFolder%s%s\">\n",ntabs++,tabs
			,classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
	}
	u_urlncpy(aux,path,strlen(path),URLCPY_ENCODE);
	io_printf(out,"%.*s<div class=\"recFolderHeader\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<ul class=\"controls ui-widget ui-helper-clearfix\">\n",ntabs,tabs);
	io_printf(out,"%.*s"
		"<li class=\"control ui-state-default ui-corner-all\">"
			"<a href=\"browse.kl1?path=%s\" class=\"ui-icon ui-icon-folder-rec\" title=\"%s\">%s</a>"
		"</li>\n",ntabs,tabs,aux,Browse,Browse);
	io_printf(out,"%.*s</ul>\n",--ntabs,tabs);
	io_printf(out,"%.*s%s\n",ntabs,tabs,getPrintName(rdir,aux));
	io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	if (webifConf.maxDepth<=0 || level<webifConf.maxDepth)
		printRecDirListUL(out,ntabs,path,pl,rdir,aux,level+1);
	if (level==0){
		io_printf(out,"%.*s<ul class=\"recFolderFooter\">\n",ntabs++,tabs);
		if (rdir->numDirs>0) {
			io_printf(out,"%.*s"
				"<li class=\"field\">"
					"<span class=\"label\">%s:</span> "
					"<span class=\"value\">%d</span>"
				"</li>\n",ntabs,tabs,tr("subfolders"),rdir->numDirs);
		}
		if (rdir->numRecs>0) {
			io_printf(out,"%.*s"
				"<li class=\"field\">"
					"<span class=\"label\">%s:</span> "
					"<span class=\"value\">%d</span>"
				"</li>\n",ntabs,tabs,tr("recordings"),rdir->numRecs);
		}
		io_printf(out,"%.*s</ul>\n",--ntabs,tabs);
		io_printf(out,"%.*s</div>\n",--ntabs,tabs);
	}
	io_printf(out,"%.*s</li>\n",--ntabs,tabs);
	return;
outOfMemory:
	crit("printFolder:out of memory");
	exit(1);
}

char *getPath2(const char *const path,const char *const name){
	char *path2;
	if (!path || strlen(path)==0){
		path2=strdup(name);
	}
	else {
		if (asprintf(&path2,"%s/%s",path,name)<0) path2=NULL;
	}
	return path2;
}

void printRecDir(io_t *out,int ntabs,const char *const path,int pl,recDir_t *rdir,char *const aux,int level) {
	const char * const Play=tr("play");
	const char * const Browse=tr("browse");
	if ( rdir->subdirs.length==0 && isFlagSet(DF_ISREC,rdir->flags)) {
		rec_t rec;
		initRec(&rec);
		rec.my=0;
		rec.path=(char *)path;
		rec.hostId=rdir->hostId;
		printRecording(out,ntabs,pl,rdir,&rec,aux,level);
		freeRec(&rec);
	} else if ( rdir->subdirs.length==1 && isFlagSet(DF_ISREC,rdir->subdirs.entry[0].flags)) {
		rec_t rec;
		initRec(&rec);
		rec.my=RF_PATH;
		rec.hostId=rdir->subdirs.entry[0].hostId;
		crit_goto_if((rec.path=getPath2(path,rdir->subdirs.entry[0].name))==NULL,outOfMemory);
		printRecording(out,ntabs,pl,rdir,&rec,aux,level);
		freeRec(&rec);
	} else {
		printFolder(out,ntabs,path,pl,rdir,aux,level);
	}
	return;
outOfMemory:
	crit("printRecDir:out of memory");
	exit(1);
}

void printRecDirList(io_t *out,int ntabs,const char *const path,int pl, recDirList_t *rdirs,char *const aux,int level,int xflags) {
	int i;
	recDir_t *rdir;
	for(i=0,rdir=rdirs->entry;i<rdirs->length;i++,rdir++){
		if (xflags) rdir->flags|=xflags;
		char *path2=getPath2(path,rdir->name);
		crit_goto_if(path2==NULL,outOfMemory);
		int j=(rdir->name[0]=='%')?1:0;
		if (rdir->name[j]=='_' && rdir->name[j+1]==0 ){
			if (rdir->subdirs.length>0){
				printRecDirList(out,ntabs,path2,pl,&rdir->subdirs,aux,level,(j==0)?0:DF_EDITED);
			}
		} else {
			printRecDir(out,ntabs+1,path2,pl,rdir,aux,level);
		}
		free(path2);
	}
	return;
outOfMemory:
	crit("printRecDirList:out of memory");
	exit(1);
}

void printRecDirListUL(io_t *out,int ntabs,const char *path,int pl, recDir_t *rdir,char *const aux,int level){
	recDirList_t *rdirs = &rdir->subdirs;
	if (rdirs->length>0){
		io_printf(out,"%.*s<ul class=\"recFolders\">\n",ntabs++,tabs);
		printRecDirList(out,ntabs,path,pl,rdirs,aux,level,0);
		io_printf(out,"%.*s</ul>\n",--ntabs,tabs);
	}
}

void printRecPathUl(io_t *out,int ntabs,const char *path,char *const aux){
	io_printf(out,"%.*s<ul class=\"recPath\">\n",ntabs++,tabs);
	io_printf(out,"%.*s<li class=\"recDir\"><a%s href=\"browse.kl1\">%s</a></li>\n"
		,ntabs,tabs,classCurrent[boolean(!path || !path[0])],tr("recordings"));
	if (path && path[0]) {
		int ln;
		int lp=0;
		const char *n=path;
		while((ln=strcspn(n,"/"))>0){
			lp+=ln;
			if (strncmp(n,"_",ln)!=0) {
				u_urlncpy(aux,path,lp,URLCPY_ENCODE);
				char *dn=strndup(n,ln);
				crit_goto_if(dn==NULL,outOfMemory);
				io_printf(out,"%.*s<li class=\"recDir\"><a%s href=\"browse.kl1?path=%s\">%s</a></li>\n"
					,ntabs,tabs,classCurrent[path[lp]==0],aux,vdrDecodeName(dn,BT_TRUE,NULL));
				free(dn);
			}
			if (path[lp]!='/') break;
			lp++;
			n=path+lp;
		}
	}
	io_printf(out,"%.*s</ul>\n",--ntabs,tabs);
	return;
outOfMemory:
	crit("printRecPathUl:out of memory");
	exit(1);
}

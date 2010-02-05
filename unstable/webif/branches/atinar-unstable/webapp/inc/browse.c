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

char * vdrDecodeName(char *name, bool inPlace, bool *isEdt){
	char *dname=(inPlace) ? name : strdup(name);
	char *s;
	s=name;
	if (s[0]=='%'){
		if (isEdt) *isEdt=true;
		s++;
	} else {
		if (isEdt) *isEdt=false;
	}
	vdrDecode(dname,s,strlen(s));
	return dname;
}

void prepareRecDir(recDir_t * const rdir){
	if (rdir->subdirs.length>0){
		qsort(rdir->subdirs.entry,rdir->subdirs.length,sizeof(recDir_t),compareRecDir);
		int i;
		recDir_t *rdir2;
		bool allDeleted=true;
		for (i=0,rdir2=rdir->subdirs.entry;i<rdir->subdirs.length;i++,rdir2++){
			prepareRecDir(rdir2);
			if (!isFlagSet(DF_DELETED,rdir2->flags)) {
				allDeleted=false;
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
	bool found=false;
	for (i=0,rdir2=rdirs->entry;i<rdirs->length;i++,rdir2++){
		if (strcmp(name,rdir2->name)==0){
			found=true;
			break;
		}
	}
	const char *ext=strrchr(name,'.');
	bool isRec=boolean(ext && strcmp(ext,".rec")==0);
	bool isDel=boolean(ext && !isRec && strcmp(ext,".del")==0);
	bool isEdt=false;
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
			rdir2->dname=vdrDecodeName((char *)name,false,&isEdt);
			if (isEdt) rdir2->flags|=DF_EDITED;
		}
	}
	return rdir2;
outOfMemory:
	crit("Out of memory");
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
	crit("Out of memory");
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
			if (code==SVDRP_CMD_OK) {
				r++;
				rdir2=rdir;
				char *f,*sf;
				int i;
				for(f=strtok_r(r," ",&sf),i=0;f && rdir2;f=strtok_r(0," ",&sf),i++) {
					if(i==1) {
						if (f[0]=='/'){
							warn("Absolute path %s",f);
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
	crit("Out of memory");
	exit(1);
}

void getRecDir(const char * const path, recDir_t * const rdir) {
	int h;
	hostConf_t *host;
	bool parsed[webifConf.hostsLength];
	for (h=0;h<webifConf.hostsLength;h++) 
		parsed[h]=false;
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
			parsed[h]=true;
			int h2;
			hostConf_t *host2 = host+1;
			for (h2=h+1;h2<webifConf.hostsLength;h2++,host2++) 
				if (!parsed[h2] && host2->video0[0] && strcmp(host->video0,host2->video0)==0)
					parsed[h2]=true;
		} else {
			addRemoteRecs(path,host,rdir);
		}
	}
	prepareRecDir(rdir);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

const char * getPrintName(wcontext_t *wctx,recDir_t *rdir){
	if (isFlagSet(DF_ISREC,rdir->flags)){
		struct tm dates;
		strptime(rdir->name,"%Y-%m-%d.%H.%M",&dates);
		CTX_CHK_BUFFER(18);
		strcpy(wctx->buffer,formatDate(&dates,true));
		return wctx->buffer;
	} else {
		return (rdir->dname!=NULL)?rdir->dname:rdir->name;
	}
}

void printRecording(wcontext_t *wctx,int pl,recDir_t *rdir, rec_t *const rec,int level){
	const char * const Play=tr("play");
	if (level==0){
		wctx_printf(wctx,"<li class=\"level4-div recFolder%s%s\">\n",classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
		wctx_printfn(wctx,"<div class=\"level4-top recFolderHeader\">\n",1,1);
	} else {
		wctx_printf(wctx,"<li class=\"recRec%s%s\">\n",classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
		inctab(wctx);
	}
	wctx_printf0(wctx,"<a href=\"");
	printRecPlayLink(wctx,rec,false);
	wctx_printf(wctx,"\" class=\"playRec\" title=\"%s\">",Play);
	if (webifConf.displayHostId) {
		wctx_printf(wctx,"%s <span class=\"host\">[h%d]</span>",getPrintName(wctx,rdir),rec->hostId);
	} else {
		wctx_printf(wctx,"%s",getPrintName(wctx,rdir));
	}
	wctx_printf(wctx,"</a>\n");
	if (level==0){
		wctx_printfn(wctx,"</div>\n",-1,0); //recFolderHeader
		wctx_printfn(wctx,"<div class=\"level4 recDetails\">\n",0,1);
		if (!isFlagSet(DF_DELETED,rdir->flags)){
			wctx_printfn(wctx,"<form action=\"/recordings.kl1\" method=\"get\" class=\"recControls controls\" >\n",0,1);
			wctx_printf0(wctx,"<input type=\"hidden\" name=\"na\" value=\"%d\" />\n",PA_BROWSE);
			wctx_printf0(wctx,"<input type=\"hidden\" name=\"pl\" value=\"%d\" />\n",pl);
			printRecControls(wctx,rec,tr("play"),tr("rec.edit"),tr("rec.delete"));
			wctx_printfn(wctx,"</form>\n",-1,0);
		}
		setFlag(RE_REMOTE_INFO_PENDING,rec->flags);
		if (getRecInfo(rec)) {
			if (isFlagSet(RE_REMOTE_INFO_PENDING,rec->flags)){
				wctx_printf0(wctx,"<div class=\"recInfo ajaxLoading\"></div>\n");
			} else {
				printRecInfo(wctx,rec);
			}
		}
		wctx_printfn(wctx,"</div>\n",-1,0); //recDetails
	}
	wctx_printfn(wctx,"</li>",-1,0);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void printFolder(wcontext_t *wctx,const char *const path,int pl,recDir_t *rdir,int level){
	if (rdir->numRecs==0) return; //TODO parametro de configuracion?
	const char * const Browse=tr("browse");
	if (level==0) {
		wctx_printf(wctx,"<li class=\"level4-div recFolder%s%s\">\n",classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
	} else {
		wctx_printf(wctx,"<li class=\"recFolder%s%s\">\n",classIsDel[isFlagSet(DF_DELETED,rdir->flags)],classIsEdt[isFlagSet(DF_EDITED,rdir->flags)]);
	}
	inctab(wctx);
	wctx_printfn(wctx,"<div class=\"level4-top recFolderHeader\">\n",0,1);
	wctx_printf0(wctx,"<a href=\"recordings.kl1?a=%d&amp;path=%s\" class=\"openFolder\" title=\"%s\">"
		,PA_BROWSE,CTX_URL_ENCODE(path,-1,NULL),Browse);
	wctx_printf(wctx,"%s</a>\n",getPrintName(wctx,rdir));
	wctx_printfn(wctx,"</div>\n",-1,0);
	if (webifConf.maxDepth<=0 || level<webifConf.maxDepth) {
		printRecDirListUL(wctx,path,pl,rdir,level+1);
	}
	if (level==0 && webifConf.printRecFolderSummary){
		wctx_printf0(wctx,"<ul class=\"level4-bottom recFolderFooter\">");
		if (rdir->numDirs>0) {
			wctx_printf(wctx,
				"<li class=\"field\">"
					"<span class=\"label\">%s:</span> "
					"<span class=\"value\">%d</span>"
				"</li>",tr("subfolders"),rdir->numDirs);
		}
		if (rdir->numRecs>0) {
			wctx_printf(wctx,
				"<li class=\"field\">"
					"<span class=\"label\">%s:</span> "
					"<span class=\"value\">%d</span>"
				"</li>",tr("recordings"),rdir->numRecs);
		}
		wctx_printf(wctx,"</ul>\n");
	}
	wctx_printfn(wctx,"</li>",-1,0);
	return;
outOfMemory:
	crit("Out of memory");
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

void printRecDir(wcontext_t *wctx,const char *const path,int pl,recDir_t *rdir,int level) {
	const char * const Play=tr("play");
	const char * const Browse=tr("browse");
	if ( rdir->subdirs.length==0 && isFlagSet(DF_ISREC,rdir->flags)) {
		rec_t rec;
		initRec(&rec);
		rec.my=0;
		rec.path=(char *)path;
		rec.hostId=rdir->hostId;
		printRecording(wctx,pl,rdir,&rec,level);
		freeRec(&rec);
	} else if ( rdir->subdirs.length==1 && isFlagSet(DF_ISREC,rdir->subdirs.entry[0].flags)) {
		rec_t rec;
		initRec(&rec);
		rec.my=RF_PATH;
		rec.hostId=rdir->subdirs.entry[0].hostId;
		crit_goto_if((rec.path=getPath2(path,rdir->subdirs.entry[0].name))==NULL,outOfMemory);
		printRecording(wctx,pl,rdir,&rec,level);
		freeRec(&rec);
	} else {
		printFolder(wctx,path,pl,rdir,level);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void printRecDirList(wcontext_t *wctx,const char *const path,int pl, recDirList_t *rdirs,int level,int xflags) {
	int i;
	recDir_t *rdir;
	for(i=0,rdir=rdirs->entry;i<rdirs->length;i++,rdir++){
		if (xflags) rdir->flags|=xflags;
		char *path2=getPath2(path,rdir->name);
		crit_goto_if(path2==NULL,outOfMemory);
		int j=(rdir->name[0]=='%')?1:0;
		if (rdir->name[j]=='_' && rdir->name[j+1]==0 ){
			if (rdir->subdirs.length>0){
				printRecDirList(wctx,path2,pl,&rdir->subdirs,level,(j==0)?0:DF_EDITED);
			}
		} else {
			printRecDir(wctx,path2,pl,rdir,level);
		}
		free(path2);
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void printRecDirListUL(wcontext_t *wctx,const char *path,int pl, recDir_t *rdir,int level){
	recDirList_t *rdirs = &rdir->subdirs;
	if (rdirs->length>0 || wctx->isAjaxRequest){
		if (level==0){
			wctx_printf0(wctx,"<ul id=\"%s\" class=\"level3 recFolders\">",AJAX_REPLACE_PREFIX("recFolders"));
		} else {
			wctx_printf0(wctx,"<ul class=\"level4 recFolders\">");
		}
		if (rdirs->length>0) {
			printRecDirList(wctx,path,pl,rdirs,level,0);
		}
		wctx_printf(wctx,"</ul>\n");
	}
}

void printRecPathUl(wcontext_t *wctx,const char *path){
	wctx_printf0(wctx,"<ul id=\"%s\" class=\"recPath\">",AJAX_REPLACE_PREFIX("recPath"));
	wctx_printf(wctx,"<li class=\"recDir\">\n");
	wctx_printfn(wctx,"<a%s href=\"recordings.kl1?a=%d\">%s</a>\n",1,0,classCurrent[boolean(!path || !path[0])],PA_BROWSE,tr("recordings"));
	wctx_printfn(wctx,"</li>",-1,0);
	if (path && path[0]) {
		int ln;
		int lp=0;
		const char *n=path;
		while((ln=strcspn(n,"/"))>0){
			lp+=ln;
			if (strncmp(n,"_",ln)!=0) {
				char *dn=strndup(n,ln);
				crit_goto_if(dn==NULL,outOfMemory);
				wctx_printf(wctx,"<li class=\"recDir\">\n");
				wctx_printfn(wctx,"<a%s href=\"recordings.kl1?a=%d&amp;path=%s\">%s</a>\n",1,0
					,classCurrent[boolean(path[lp]==0)],PA_BROWSE,CTX_URL_ENCODE(path,lp,NULL),vdrDecodeName(dn,true,NULL));
				wctx_printfn(wctx,"</li>",-1,0);
				free(dn);
			}
			if (path[lp]!='/') break;
			lp++;
			n=path+lp;
		}
	}
	wctx_printf(wctx,"</ul>\n");
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

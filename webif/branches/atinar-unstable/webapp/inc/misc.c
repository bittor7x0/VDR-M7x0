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
* Modified for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <locale.h>
#include <u/libu.h>

#include "misc.h"
#include "i18n.h"
#include "conf.h"

int isM740AV=0;
int isM750S=0;
int isM750C=0;
int boxSysType=0;
const int sysNone=0;
const int sys740av=1;
const int sys750s=2;
const int sys750c=3;

const int httpPort=80;

const int ttOnce=0;
const int ttPeriodic=1;

//TODO deprecate
const int sortNone=0;
const int sortStart=1;
const int sortStop=2;
const int sortType=3;
const int sortTitle=4;
const int sortChannelNum=5;
const int sortChannelName=6;
const int sortPriority=7;
const int sortLifetime=8;
const int sortActive=9;
const int sortMux=10;

const int sortAsc=1;
const int sortDesc=-1;

const char checked[2][8]={"","checked"};
const char selected[2][9]={"","selected"};
const char arStr[7][8]={"1.gif","43.png","169.png","1.gif","43.png","169.png","1.gif"};
const char * cssSortClass[]={"sortdesc","sortnone","sortasc"};

enum sortField sortBy;
enum sortDirection sortDirection;
enum pageNumber currentPage;

const char * action = NULL;
const char * SUMMARY = "summary";
const char * EDIT = "edit";
const char * NEW = "new";
const char * ADD = "add";
const char * DELE = "dele";
const char * NOACTION = "";

char server_ip[16];
uint16_t server_port;

char myip[16]="";
char * newt=NULL;

int parseRequestStr(const char * requestStr, char ** pathStr, char ** queryStr) {
	int i=0,j=0,k=0;
	
	if (!requestStr) { return 0; }
	
	*pathStr=NULL; *queryStr=NULL;
	
	i=strcspn(requestStr," ");
	j=strcspn(requestStr+i+1," ");
	k=strcspn(requestStr+i+1,"?");
	
	*pathStr=malloc(j+1);
	if (k<j) { 
		strncpy(*pathStr,requestStr+i+1,k);
		(*pathStr)[k]=0;
	} else {
		strncpy(*pathStr,requestStr+i+1,j);
		(*pathStr)[j]=0;
	}

	if (k<j) {
		*queryStr=malloc(j-k+1);
		strncpy(*queryStr,requestStr+i+k+2,j-k-1);
		(*queryStr)[j-k-1]=0;
	}
	
	return 1;
}

char * strcatEx(char ** dest, const char * s) {
	int size=0;
	
	if (*dest==NULL) { *dest=malloc(strlen(s)+2); strcpy(*dest,""); }
	size=strlen(*dest)+strlen(s);
	
  char * tmp =(char *)realloc(*dest,size+2);
  if (!tmp) {
    exit(1);
  }
  *dest=tmp;

  strcat(*dest,s);
  return *dest;
}

int isDST(time_t * aTime) {
  struct tm t=*localtime(aTime);
  if (t.tm_isdst>0) { return 1; } else { return 0; }
}

int sameDay(time_t oneDate,time_t anotherDate) {
  if ( (oneDate/86400)==(anotherDate/86400) ) { return 1; } else { return 0; }
}

int sameString(const char * s1, const char * s2) {
  if (strcmp(s1,s2)) { return 0; } else { return 1; }	
}

int sameInt(const int i1, const int i2) {
	if (i1==i2) { return 1; } else { return 0; }
}

int sameIntEx(const char * s, const int i) {
	if (strtol(s,NULL,10)==i) { return 1; } else { return 0; }
}

int fileExists(const char * fileName) {
  struct stat status;

  if (( stat (fileName, &status)) < 0) { return 0; }
  if (! S_ISREG(status.st_mode)) { return 0; } 
  return 1;
}

int dirExists(const char * dirName) {
	struct stat status;
	
	if (( stat (dirName, &status)) < 0) { return 0; }
	if (! S_ISDIR(status.st_mode)) {return 0; }
	return 1;
}

int legalPath(char * pathName) {
	char * dir;
	for(dir=strtok(pathName,"/");dir!=0;dir=strtok(0,"/")) {
		if (strcmp(dir,"..")==1) { return 0; }
		if (strcmp(dir,"~")==1) {return 0; }
	}
	return 1;
}

int vdrRunning() {
	if (fileExists("/var/run/runvdr.pid")) {
		FILE *f = fopen("/var/run/runvdr.pid","r");
		if (f) {
			char pid[10]="";
			char proc[30]="";
			fgets(pid,sizeof(pid),f);
			fclose(f);
			if (pid[strlen(pid)-1]=='\n') { pid[strlen(pid)-1]='\0'; }
			sprintf(proc,"/proc/%s/cmdline",pid);
			if (fileExists(proc)) return 1; else return 0;
		}
	} else {
		return 0;
	}
}

void getConfig() {
int i=0;
  readWEBIFCONF();

  if ((webifConf.langID>-1) && (webifConf.langID<I18NNUM)) {
	langID=webifConf.langID;  
  } else {
    if ((langID<0) || (langID>=I18NNUM)) {
      for (i=0;i<I18NNUM;i++) {
        if (strcmp(acceptedLang,i18n[1][i])==0) {
          langID=i;
        }
      } 
    }
    if ((langID<0) || (langID>=I18NNUM)) {
      langID=0;
    }
  }
  setlocale(LC_ALL,"de_DE");
// get systemtype from /etc/systemtype
  boxSysType=sysNone;
  char str[10];
  FILE *f = fopen("/etc/systemtype","r");
  if (f) {
	fgets(str,sizeof(str),f);
	fclose(f);
  }
  if (!strcmp(str,"m740")) {
    boxSysType=sys740av;
    isM740AV=1; isM750S=0; isM750C=0;
  } else if (!strcmp(str,"m750s")) {
    boxSysType=sys750s;
    isM740AV=0; isM750S=1; isM750C=0;
  }
}

const char * sortClass(enum sortField sf){
	return cssSortClass[((sf==sortBy)?sortDirection:SD_NONE)+1];
}


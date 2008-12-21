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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <u/libu.h>

#include "svdrp_comm.h"
#include "svdrp_parse.h"
#include "misc.h"
#include "recordings.h"
#include "channels.h"
#include "epg.h"

// Sortiert die Liste, die beiden nächsten Variablen geben an,
// wonach und in welche Richtung sortiert wird.
int compareRE_sortBy=0;
int compareRE_sortDirection=1;
int compareRE(const void * a, const void * b) {
  switch (compareRE_sortBy) {
	case 0: return 0; break;
	case 1: return (( ((const recEntry*)a)->start - ((const recEntry*)b)->start )*compareRE_sortDirection); break;	
	case 4: return (strcasecmp(((const recEntry*)a)->title,((const recEntry*)b)->title)*compareRE_sortDirection); break;
	default: return 0; break;
  }
}

recEntry * getRecList(int * max, int sortBy, int sortDirection) {
char * data;
char * p;

char ret_code[10];

recEntry *rec;
int i=0;
char rc[20]="";

  
  rec=(recEntry *)malloc(sizeof*rec);
  write_svdrp("LSTR\r");
  data=read_svdrp();
  
  for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
    parse_ret_code(p,ret_code);
    sprintf(rc,"250-%d",i+1);
    if (!strcmp(ret_code, "550")) {
      i=0;
    } else if (!strcmp(ret_code, rc) || !strcmp(ret_code, "250")) {
      i++;
      recEntry *tmp=(recEntry *)realloc(rec,i*sizeof*rec);
      if (!tmp) {
        printf("Reallocation failed\n");
        exit(1);
      }
      rec=tmp;
      parse_rec(p,strlen(rc)+1,&(rec[i-1].seen),&(rec[i-1].direct),&(rec[i-1].cut),&(rec[i-1].start),rec[i-1].title,NULL);
    }
  }  
  
  if (i>0 && sortBy!=0) {
    //Aufnahmen schnell noch sortieren
	compareRE_sortBy=sortBy;
	compareRE_sortDirection=sortDirection;
    qsort(rec,i,sizeof*rec,compareRE);
  } 

  free(data);
  *max=i;
  return rec;
}

recEntry2 * mallocRE2(int max) {
	recEntry2 * o=(recEntry2 *)malloc((sizeof*o)*max);
	memset(o,0,(sizeof*o)*max);
	return o;
}

void freeRE2(recEntry2 * o,int max) {
	int i=0;
	for (i=0;i<max;i++) {
		free(o[i].title);
		free(o[i].subtitle);
		free(o[i].desc);
		free(o[i].audio);
	}
	free(o);
}

void copyRE2(recEntry2 * src, recEntry2 * dest,int max) {
	int i=0;
	for (i=0;i<max;i++) {
		if (src[i].title!=NULL) dest[i].title=strdup(src[i].title);
		if (src[i].subtitle!=NULL) dest[i].subtitle=strdup(src[i].subtitle);
		if (src[i].desc!=NULL) dest[i].desc=strdup(src[i].desc);
		dest[i].start=src[i].start;
		dest[i].stop=src[i].stop;
		dest[i].duration=src[i].duration;
	  strcpy(dest[i].channelName,src[i].channelName);
	  strcpy(dest[i].channel_id,src[i].channel_id);
	  dest[i].channelNum=src[i].channelNum;
	  dest[i].ar=src[i].ar; 
		if (src[i].audio!=NULL) dest[i].audio=strdup(src[i].audio);
	}
}

recEntry2 * readInfoVDR(const char * filename) {
	FILE *handle;
	recEntry2 * info=mallocRE2(1);
	char s[513];
	
	handle=fopen(filename,"r");
	if (handle) {
		while (!feof(handle)) {
			fgets(s,512,handle);
			switch (s[0]) {
				case 'C'	:	strncpy(info[0].channel_id,s+2,50);
	      	break;
				case 'E'	:	parse_215E(s,2,NULL,&(info[0].start),&(info[0].duration),NULL,NULL);
						info[0].stop=info[0].start+info[0].duration;
	      	break;
				case 'T'	:	info[0].title=strdup(s+2);
	      	break;
				case 'D'	:	info[0].desc=strdup(s+2);
	      	break;
				case 'S'	: info[0].subtitle=strdup(s+2);
					break;
				case 'X'	:
						switch(s[2]) {
						case '1' :if (s[5]=='1' || s[5]=='5') info[0].ar=1;
									 if (s[5]=='2' || s[5]=='3' || s[5]=='6' || s[5]=='7') info[0].ar=2;
									 if (s[5]=='4' || s[5]=='8') info[0].ar=3;
									 if (s[5]=='9' || s[5]=='D') info[0].ar=4;
									 if (s[5]=='A' || s[5]=='B' || s[5]=='E' || s[5]=='F') info[0].ar=5;
									 if (s[5]=='C' || s[5]=='0') info[0].ar=6;
							  break;
						case '2'	:if (info[0].audio==NULL) {
										info[0].audio=strdup(s+7);
									} else {
										info[0].audio=realloc(info[0].audio,strlen(info[0].audio)+strlen(s)-4);
										strcat(info[0].audio,", "); strcat(info[0].audio,s+7);
									}
								break;
						}
					break;
			}
		}
	}
	return info;
	fclose(handle);
}

// returns 0 if its a dir with subdirs, 1 if its a repeating timer or 2 if its a dir with multiple recs, 3 if its a mix of 0 and 2
int readRecDir(const char * path, int round, int * numF, int * numD, int * size, recEntry2 * info) {
	char *s, *s2;
	char newestRec[28]="";

	s=malloc(strlen(path)+30); strcpy(s,path);
	s2=malloc(strlen(path)+30); strcpy(s2,"");
	
	*numF=0; *numD=0;
	
	if (path[strlen(path)-1]=='/') strcat(s,"_/"); else strcat(s,"/_/");
	
	if (dirExists(s)) {
	  if (round==1) readRecDir(s,round,numF,numD,size,info);
	  free(s); free(s2);
	  return 1;
	}  else {
		strcpy(s,path);
		if (path[strlen(path)-1]!='/') strcat(s,"/");
		DIR *dir = opendir(s); 
		if (dir != 0) { 
			struct dirent* dirE; 
		  while(0 != (dirE = readdir(dir))) { 
		  	const char* p = dirE->d_name; 
		    if ( (p[0]=='.' && (p[1]==0 || (p[1]=='.' && p[2]==0))) ) 
		      continue; 
		    if (!strcmp(p,"info.vdr")) {
		    	recEntry2 * infoVDR;
		    	strcpy(s2,s); strcat(s2,"info.vdr");
		    	infoVDR=readInfoVDR(s2);
		    	if (infoVDR && info[0].title==NULL && info[0].desc==NULL) {
		    		copyRE2(infoVDR,info,1);
		    		freeRE2(infoVDR,1);
		    		free(s); free(s2);
		    		return 2;
		    	} else {
		    		free(s); free(s2);
		    		return -1;
		    	}
		    } else if (dirE->d_type==DT_DIR && strcmp(p+(strlen(p)-4),".rec")==0) {
		    	*numF+=1;
		    	strcpy(newestRec,p);
		    } else if (dirE->d_type==DT_DIR) {
		    	*numD+=1;
		    } 
		  } 
	  	if (*numD>0) {
	  		free(s); free(s2);
	  		return 3;
	  	}
		  if (*numF>0) {
		  	strcpy(s2,s); strcat(s2,newestRec);
		  	int * na; int * nb;
		  	if (round==1) readRecDir(s2,round+1,&na,&nb,size,info);
		  	free(s); free(s2);
		  	if (*numF==1) return 2; else return 3;
		  } 		  	
		} else { free(s); free(s2); return -2; }
	}
	free(s); free(s2); return 0;
}

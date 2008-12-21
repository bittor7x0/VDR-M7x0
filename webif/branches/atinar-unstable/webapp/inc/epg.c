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
#include <u/libu.h>

#include "svdrp_comm.h"
#include "svdrp_parse.h"
#include "misc.h"
#include "epg.h"
#include "channels.h"

void freeNowNextEntry(nowNextEntry * o,int max) {
	int i=0;
	free(o);
//	for (i=0;i<max;i++) {
//	}
}

nowNextEntry * getNowNext(int * max, int channelNum) {
char * data;
char * p;

char ret_code[10]="";
int pass=0;

channelList * channels;
int max_cl=-1;

nowNextEntry *now_next;
int i=0;
int k=0;
  channels=get_channel_list(&max_cl);
  if (channelNum>0) {
  	now_next=(nowNextEntry *)malloc(sizeof*now_next);
  	strcpy(now_next[0].channel_id,channels[channelNum-1].channel_id);
  	strcpy(now_next[0].channelName,channels[channelNum-1].channelName);
  	now_next[0].channelNum=channelNum;
  	strcpy(now_next[0].now_title,"");
  	strcpy(now_next[0].now_desc,"");
  	strcpy(now_next[0].now_short,"");
  	now_next[0].now_time=0;
  	now_next[0].now_duration=0;
  	strcpy(now_next[0].next_title,"");
  	strcpy(now_next[0].next_desc,"");
  	strcpy(now_next[0].next_short,"");
  	now_next[0].next_time=0;
  	now_next[0].next_duration=0;
  	char cmd[20]="";
  	sprintf(cmd,"LSTE %d now\r",channelNum);
  	write_svdrp(cmd);
  } else {
  	now_next=(nowNextEntry *)malloc(max_cl*sizeof*now_next);
  	for(i=0;i<max_cl;i++) {
  		strcpy(now_next[i].channel_id,channels[i].channel_id);
  		strcpy(now_next[i].channelName,channels[i].channelName);
  		now_next[i].channelNum=i+1;
  		strcpy(now_next[i].now_title,"");
  		strcpy(now_next[i].now_desc,"");
  		strcpy(now_next[i].now_short,"");
  		now_next[i].now_time=0;
  		now_next[i].now_duration=0;
  		strcpy(now_next[i].next_title,"");
  		strcpy(now_next[i].next_desc,"");
  		strcpy(now_next[i].next_short,"");
  		now_next[i].next_time=0;
  		now_next[i].next_duration=0;
  	}
    write_svdrp("LSTE now\r");
  }

  for(pass=0;pass<2;pass++) {
    data=read_svdrp();
    for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
      parse_ret_code(p,ret_code);
      if (!strcmp(ret_code,"550")) {
        *max=max_cl;
        free(data);
        return now_next;
      } else if (!strcmp(ret_code, "215-C")) {
  	    k=strcspn(p+6," ");
	      char tmp[50];
        strcpy(tmp,p+6+k+1);
        i=0;
        if (channelNum==0) {
        	while (strcmp(now_next[i].channelName,tmp) && i<max_cl) { i++; }
        }
      } else if (!strcmp(ret_code,"215-D")) {
	      switch (pass) {
          case 0 :strncpy(now_next[i].now_desc,p+6,500); break;
          case 1 :strncpy(now_next[i].next_desc,p+6,500); break;
        }
      } else if (!strcmp(ret_code,"215-T")) {
	      switch (pass) {
          case 0 :strcpy(now_next[i].now_title,p+6); break;
          case 1 :strcpy(now_next[i].next_title,p+6); break;
        }
      } else if (!strcmp(ret_code,"215-S")) {
	      switch (pass) {
          case 0 :strcpy(now_next[i].now_short,p+6); break;
          case 1 :strcpy(now_next[i].next_short,p+6); break;
        }
      } else if (!strcmp(ret_code,"215-E")) {
        switch (pass) {
          case 0: parse_215E(p,6,NULL,&(now_next[i].now_time),&(now_next[i].now_duration),NULL,NULL); break;
          case 1: parse_215E(p,6,NULL,&(now_next[i].next_time),&(now_next[i].next_duration),NULL,NULL); break;
        }
      } else if ( !strcmp(ret_code,"215") && pass==0 ) {
      	if (channelNum>0) {
        	char cmd[20]="";
        	sprintf(cmd,"LSTE %d now\r",channelNum);
        	write_svdrp(cmd);      		
      	} else {
      		write_svdrp("LSTE next\r");
      	}
      }
    }
  }	
  free(data);
  *max=max_cl;
  return now_next;
}

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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>

#include "svdrp_comm.h"
#include "svdrp_parse.h"
#include "channels.h"

// Besorgt sich die Channelliste vom VDR
channelList * get_channel_list(int * max) {
  char * data;
  char * p;

  int i=1;
  int j=0;
  int k=0;
  char ret_code[10]="";
  char rc[10]="";
  channelList * channels;

  channels=(channelList *)malloc(sizeof(channelList));

  write_svdrp("LSTC\r");
  data=read_svdrp();

  for(p=(char *)strtok(data,"\r\n");p!=0;p=(char *)strtok(0,"\r\n")) {
    parse_ret_code(p,ret_code);

    sprintf(rc,"250-%d",i);
    if (!strcmp(ret_code, "550")) {
      *max=-1;
      free(data);
      free(channels);
      return NULL;
    } else if (!strcmp(ret_code,rc) || !strcmp(ret_code, "250")) {
    
      channelList * tmpChannels=(channelList *)realloc(channels,i*sizeof(channelList));
      if (!tmpChannels) {
        warn("Reallocation failed. Old size is %d, new size should be %d",(i-1)*sizeof*channels,i*sizeof*channels);
        exit(1);
      }
      channels=tmpChannels;
      
      j=strlen(rc)+1;
      k=0;
      char tmp[13][50]={"","","","","","","","","","","","",""};
      for(k=0;k<13;k++) {
        strncpy(tmp[k],p+j+k,strcspn(p+j+k,":"));
        j+=strcspn(p+j+k,":");
      }
      k=strcspn(tmp[0],",;:");
      if (k<strlen(tmp[0])) {
        strncpy(channels[i-1].channelName,tmp[0],k);
        channels[i-1].channelName[k]='\0';
        k=strcspn(tmp[0],";");
        strcpy(channels[i-1].multiplexName,tmp[0]+k+1);
      } else {
        strcpy(channels[i-1].channelName,tmp[0]);
        strcpy(channels[i-1].multiplexName,"");
      }
      channels[i-1].channel_num=i;
      channels[i-1].frequency=strtol(tmp[1],NULL,10);
      strcpy(channels[i-1].parameter,tmp[2]);
      strcpy(channels[i-1].source,tmp[3]);
      channels[i-1].vpid=strtol(tmp[5],NULL,10);
      channels[i-1].caid=strtol(tmp[8],NULL,10);
      channels[i-1].sid=strtol(tmp[9],NULL,10);
      channels[i-1].nid=strtol(tmp[10],NULL,10);
      channels[i-1].tid=strtol(tmp[11],NULL,10);
      channels[i-1].rid=strtol(tmp[12],NULL,10);
      sprintf(channels[i-1].channel_id,"%s-%s-%s-%s",tmp[3],tmp[10],tmp[11],tmp[9]);
      
//      k=strcspn(p+strlen(rc)+1,",;:");
//      strncpy(channels[i-1].channel_name,p+strlen(rc)+1,k);
//      channels[i-1].channel_name[k]='\0';
//      channels[i-1].channel_num=i;
      i++;
    }
  }

  free(data);
  *max=i-1;
  return channels;
}

// Holt den Programmplatz des aktuellen Senders, liefert -1 zurück falls es einen Fehler gab.
int getChannel(char channelName[30]) {
char * data;
int chanNum=0;
char chanName[30]="";

  write_svdrp("CHAN\r");
  data=read_svdrp();
  parse_chan(data,&chanNum,chanName);
  if (channelName!=NULL) {
    strcpy(channelName,chanName);
  }  
  return chanNum;
}

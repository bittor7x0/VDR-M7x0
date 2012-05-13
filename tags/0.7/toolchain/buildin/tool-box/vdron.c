/*
  (C) 2001 by Martin Hammerschmid <linux@home.pages.at>
  see http://home.pages.at/linux/dvb.html
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  (see COPYING).
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define STINGBUFSIZE 8192

int main(int argc, char *argv[]) {
  FILE *file;
  char instring[STINGBUFSIZE];
  char * strptr;
  int i,j,hh,mm,Y,M,D;
  char sday[10],hhmm[5];
  time_t now,start;
  struct tm now1,now2;
  unsigned int active;
  
  if (argc !=4) {
    printf("Usage: %s <min> <timers.conf> <command>\n"
    "%s checks a timers.conf-file as used in vdr "
    "(see http://www.cadsoft.de/people/kls/vdr/) and executes "
    "a command <min> minutes before a timer gets active. "
    "This program is intended to be called every minute "
    "by crond to power on a vdr box with ether-wake "
    "(see http://www.scyld.com/expert/wake-on-lan.html).\n"
    "Example:\n    %s 3 /video/timers.conf \"ether-wake -i eth0 00:01:80:05:21:62\"\n"
    ,argv[0],argv[0],argv[0]);	  
    return 0;	  
  }
  
  file=fopen(argv[2] , "r");
  if (file==NULL) {
    fprintf(stderr,"ERROR: file %s does not exist.\n",argv[2]);
    return -1;
  }
  now=time(NULL)+atoi(argv[1])*60;
  localtime_r(&now,&now1);
  now1.tm_sec=0;
  now=mktime(&now1);
  while ( fgets(instring,STINGBUFSIZE-1,file ) != NULL ) {
    sscanf(instring,"%d:", &active );   
    if ( (active & 1) ) {
      for (j=0;instring[j]!=':' ;j++);	    
      for (i=j+1;instring[i]!=':' ;i++);
      for (j=i+1;instring[j]!=':' ;j++);
      snprintf(sday,j-i,"%s", instring+i+1);
      snprintf(hhmm,5,"%s",instring+j+1);
      mm=atoi(hhmm+2); hhmm[2]='\0';hh=atoi(hhmm);
      localtime_r(&now,&now1); 
      if (isdigit(sday[0]))  
        now1.tm_mday=atoi(sday); 
      else if ( !( sday[(now1.tm_wday+6)%7] != '-' ) ) 
	continue;
      else if ( (strptr=strchr(sday,'@')) != NULL ) { 
        sscanf(strptr+1,"%d-%d-%d",&Y,&M,&D);
	localtime_r(&now,&now2);
        now2.tm_sec=0; now2.tm_wday=-1;
	now2.tm_year=Y-1900;
	now2.tm_mon=M-1;
	now2.tm_mday=D;
	now2.tm_hour=hh;
	now2.tm_min=mm;
	start=mktime(&now2);
	if (start >now) continue;
      }	      
      now1.tm_hour=hh; now1.tm_min=mm;
      start=mktime(&now1);
      if (now == start) {
        fclose(file);
        system(argv[3]); 
        return 0;	
      }
    }	  
  }
  fclose(file);
  return 0;
}

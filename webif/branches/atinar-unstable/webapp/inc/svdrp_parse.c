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
* atinar <atinar@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#define _XOPEN_SOURCE
#define _GNU_SOURCE

//svdrp_parse stellt Funktionen bereit, um die Zeilen einer Antwort zu parsen
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "svdrp_parse.h"

#ifdef FOR_BUSYBOX
  #include "scan_config.c"
#endif

char vdr_setup[256];

int margin_start=5*60;
int margin_stop=15*60;

void parse_chan(char * line, int * chan_id, char chan_name[30]) {
  char tmp[5]="";
  int i=0;

  i=strcspn(line+4," ");
  strncpy(tmp,line+4,i);
  *chan_id=strtol(tmp,NULL,10);
  strcpy(chan_name,line+4+i+1);
}

int parse_ret_code(char * line, char ret_code[10]) {
  long int i=0;
  char s[4];

  i=strcspn(line," ");
  strncpy(ret_code,line,i);
  ret_code[i]='\0';
  strncpy(s,ret_code,3);
  s[4]='\0';
  i=strtol(s,NULL, 10);
  if ( (i>99) && (i<1000)) {
    return (int)i;
  } else {
	return 500;
  }
}

void parse_215E(char * line, int offset, unsigned int * event_id, long int * start_time, int * duration, int * table_id, int * version) {
  int i=offset;
  int k=0;

  char tmp[5][50]={"","","","",""};
  for(k=0;k<4;k++) {
    strncpy(tmp[k],line+i+k,strcspn(line+i+k," "));
    i+=strcspn(line+i+k," ");
  }
  if (event_id!=NULL) { *event_id=strtoul(tmp[0],NULL,10); }
  if (start_time!=NULL) { *start_time=strtol(tmp[1],NULL,10); }
  if (duration!=NULL) { *duration=strtol(tmp[2],NULL,10); }
  if (table_id!=NULL) { *table_id=strtol(tmp[3],NULL,16); }
  if (version!=NULL) { *version=strtol(tmp[4],NULL,16); }
}

void make_timer(char newt[256], int active, int channel_num, time_t start_time, time_t end_time, int priority, int lifetime, char title[50]) {
  time_t ts=start_time-margin_start;
  time_t te=end_time+margin_stop;
  struct tm t1=*localtime(&ts);
  struct tm t2=*localtime(&te);
  sprintf(newt,"NEWT %d:%d:%d-%02d-%02d:%02d%02d:%02d%02d:%d:%d:%s:",active,channel_num,1900+t1.tm_year,t1.tm_mon+1,t1.tm_mday,t1.tm_hour,t1.tm_min,t2.tm_hour,t2.tm_min,priority,lifetime,title);
}

int makeTimerEx(char newt[256], int active, int channel_num, int type, char reg_timer[8], const char * argDate, const char * startH, const char * startM, const char * endH, const char * endM, int marginStart, int marginEnd, int priority, int lifetime, const char * title) {
  char s[25]; 
  char * date;
  
  if (type==2) {
	time_t rawtime=time(NULL);
	struct tm * today;
	today=localtime(&rawtime); 
    date=malloc(12);    
    strftime(date,11,"%Y-%m-%d",today);
  } else {
	date=strdup(argDate);
  }
  
  struct tm startTimePtr;
  sprintf(s,"%s %s:%s:00",date,startH,startM);
  if (!strptime(s,"%Y-%m-%d %H:%M:%S",&startTimePtr)) { return 1; }
  startTimePtr.tm_wday=0; startTimePtr.tm_yday=0;  startTimePtr.tm_isdst=-1;
  time_t start_time=mktime(&startTimePtr)-marginStart;

  struct tm endTimePtr;
  sprintf(s,"%s %s:%s:00",date,endH,endM);
  if (!strptime(s,"%Y-%m-%d %H:%M:%S",&endTimePtr)) { return 1; }
  endTimePtr.tm_wday=0; endTimePtr.tm_yday=0;  endTimePtr.tm_isdst=-1;
  time_t end_time=mktime(&endTimePtr)+marginEnd;
  if (end_time<start_time) { end_time+=86400; }
  
  struct tm t1=*localtime(&start_time);
  struct tm t2=*localtime(&end_time);
  if (type==2) {
    sprintf(newt,"NEWT %d:%d:%s:%02d%02d:%02d%02d:%d:%d:%s:",active,channel_num,reg_timer,t1.tm_hour,t1.tm_min,t2.tm_hour,t2.tm_min,priority,lifetime,title);  
  } else {
	sprintf(newt,"NEWT %d:%d:%d-%02d-%02d:%02d%02d:%02d%02d:%d:%d:%s:",active,channel_num,1900+t1.tm_year,t1.tm_mon+1,t1.tm_mday,t1.tm_hour,t1.tm_min,t2.tm_hour,t2.tm_min,priority,lifetime,title);
  }
  free(date);
  return 0;
}

void parse_rec(char * line, struct recEntry * recording){
  char * r=line;
  int l;
  struct tm timeptr;
  r+=strspn(r," ");
  recording->ID=(int)strtol(r,&r,10);
  r+=strspn(r," ");
  r=strptime(r,"%d.%m.%y %H:%M",&timeptr);
  if (r==NULL){ 
    printf("Error converting recording date!\n");
    recording->title=NULL;
    recording->path=NULL;
    return;
  }
  timeptr.tm_sec=0;
  timeptr.tm_isdst=-1;
  recording->start=mktime(&timeptr);
  recording->seen=(r[0]==' ');
  r++;
  r+=strspn(r," ");
  l=strcspn(r,"/\n\r");
  recording->title=strndup(r,l);
  recording->direct=(strchr(recording->title,'@')==NULL) ? 0 : 1;
  recording->cut=(strchr(recording->title,'%')==NULL) ? 0 : 1;
  r+=l;
  if (r[0]=='/') {    //Requires vdr patched to include path
    l=strcspn(r,"\n\r");
    recording->path=strndup(r,l);
  }
  else {
    recording->path=NULL;
  }
  return;
}

void parse_timer(char * line, int offset, int * active, int * chan_id, int * type, char reg_timer[8], time_t * start_time, time_t * end_time, int * priority, int * lifetime, char title[100]) {
  int i=offset;
  int k=0;
  char s[50];

  char tmp[8][100]={"","","","","","","",""};
  for(k=0;k<8;k++) {
    strncpy(tmp[k],line+i+k,strcspn(line+i+k,":"));
    i+=strcspn(line+i+k,":");
  }

  if (active!=NULL) { *active=strtol(tmp[0],NULL,10); }
  if (chan_id!=NULL) { *chan_id=strtol(tmp[1],NULL,10); }
  if (strlen(tmp[2])==7) {
    strcpy(reg_timer,tmp[2]);
    if (type!=NULL) { *type=1; }
    strcpy(tmp[2],"1970-01-02");
    time_t ttDate=time(NULL);
    for (k=0;k<7;k++) {
      struct tm next_rec=*localtime(&ttDate);
      if (reg_timer[(next_rec.tm_wday+6)%7]!='-') {
        sprintf(tmp[2],"%04d-%02d-%02d",1900+next_rec.tm_year,1+next_rec.tm_mon,next_rec.tm_mday);
        break;
      }
      ttDate+=86400;
    }
  } else {
	if (type!=NULL) { *type=0; }
    strcpy(reg_timer,"");
  }
  if (start_time!=NULL) {
	struct tm timeptr;
	sprintf(s,"%s %c%c:%c%c:00",tmp[2],tmp[3][0],tmp[3][1],tmp[3][2],tmp[3][3]);
	if (!strptime(s,"%Y-%m-%d %H:%M:%S",&timeptr)) { printf("Fehler bei der Datumskonvertierung!\n"); }
	timeptr.tm_wday=0; timeptr.tm_yday=0;
    timeptr.tm_isdst=-1;
    *start_time=mktime(&timeptr);
  }
  if (end_time!=NULL) {
	struct tm timeptr;
	sprintf(s,"%s %c%c:%c%c:00",tmp[2],tmp[4][0],tmp[4][1],tmp[4][2],tmp[4][3]);
	if (!strptime(s,"%Y-%m-%d %H:%M:%S",&timeptr)) { printf("Fehler bei der Datumskonvertierung!\n"); }
	timeptr.tm_wday=0; timeptr.tm_yday=0;
    timeptr.tm_isdst = -1;
    *end_time=mktime(&timeptr);
    if ( start_time!=NULL && *end_time<*start_time ) {
      *end_time+=24*60*60;
      timeptr=*localtime(end_time);
    }
  }
  if (priority!=NULL) { *priority=strtol(tmp[5],NULL,10); }
  if (lifetime!=NULL) { *lifetime=strtol(tmp[6],NULL,10); }
  if (title!=NULL) { strcpy(title,tmp[7]); }
}

void parse_channel(char * line, char channel_name[50], char channel_id[50]) {

}


//Noch ein wenig was, um QUERY_STRING zu verarbeiten

// Wandelt einzelne Hexzeichen (%xx) in ASCII-Zeichen
// und kodierte Leerzeichen (+) in echte Leerzeichen um
void hex2ascii(char *str)  {
   int x, y;

   for(x=0,y=0; str[y] != '\0'; ++x,++y) {
      str[x] = str[y];
      /* Ein Hexadezimales Zeichen? */
      if(str[x] == '%')  {
         str[x] = convert(&str[y+1]);
         y += 2;
      }
      /* Ein Leerzeichen? */
      else if( str[x] == '+')
         str[x]=' ';
   }
   /* Geparsten String sauber terminieren */
   str[x] = '\0';
}

// Funktion konvertiert einen String von zwei hexadezimalen
// Zeichen und gibt das einzelne dafür stehende Zeichen zurück
char convert(char *hex) {
   char ascii;

   /* erster Hexawert */
   ascii =
   (hex[0] >= 'A' ? ((hex[0] & 0xdf) - 'A')+10 : (hex[0] - '0'));

   ascii <<= 4; /* Bitverschiebung schneller als ascii*=16 */
   /* zweiter Hexawert */
   ascii +=
   (hex[1] >= 'A' ? ((hex[1] & 0xdf) - 'A')+10 : (hex[1] - '0'));
   return ascii;
}

void get_config_info() {

#ifdef FOR_BUSYBOX

  //***** WICHTIG: Tabelle ist im Feld 'name' alphabetisch zu sortieren,
  //***** da eine binäre Suche verwendet wird.

  t_scan_config config_param[] =
  {
  // cnt, name,			default
    { 0, "RELEVANT_VDR_CONFIG",	0 },
    { 0, "TIMEZONE",		"CET-1CEST,M3.5.0,M10.5.0/03:00:00" },

    {0,0,0} // END OF TABLE
  };

  scan_config(config_param);
  setenv("TZ",config_param[CONFIG_TIMEZONE].value,1);

  if (config_param[CONFIG_RELEVANT_VDR_CONFIG].value)
  {
    snprintf(vdr_setup,sizeof(vdr_setup),"%s/setup.conf",
		config_param[CONFIG_RELEVANT_VDR_CONFIG].value);

    FILE *f = fopen(vdr_setup,"r");
    if (f) {
      while (!feof(f)) {
	char buffer[256], param[sizeof(buffer)];
        fgets(buffer,sizeof(buffer),f);
        buffer[strlen(buffer)-1] = '\0';
        const int i=strcspn(buffer,"=");
        strncpy(param,buffer,i);
        if (!strcmp(param,"MarginStart ")) {
          margin_start=strtol(buffer+i+2,NULL,10)*60;
        } else if (!strcmp(param,"MarginStop ")) {
          margin_stop=strtol(buffer+i+2,NULL,10)*60;
        }
      }
      fclose(f);
    }
  }

#endif // FOR_BUSYBOX
}


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

#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <u/libu.h>
#include <sys/types.h>

#include "svdrp_comm.h"

#define SVDRP_BUFFER_SIZE 4096

int svdrp_socket=0;

const char eod_pattern[]="^[0-9]{3} ";
regex_t regbuf;

void signal_handler(int sig) {
	dbg("Got signal [%s]",sig);
	if (svdrp_socket!=0) {
		warn("Broken Pipe.");
		svdrp_socket=0;
	}
}

//Open the connection
int open_svdrp() {
  char * data;
  pid_t pid;
  pid=getpid();
  signal(SIGPIPE,signal_handler);

  if (svdrp_socket==0) {
  	dbg("Try to open SVDRP connection to 127.0.0.1 from pid [%d]",pid);
  	svdrp_addr.sin_family = AF_INET;
  	svdrp_addr.sin_port = htons(2001);
  	inet_aton("127.0.0.1", &svdrp_addr.sin_addr);

  	// create tcp socket
  	if ((svdrp_socket=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
  		warn("Failed to create TCP Socket.");
  		exit(errno);
  	}
  	// Connect to VDR
  	if (connect(svdrp_socket, (struct sockaddr*)&svdrp_addr, sizeof(svdrp_addr)) < 0) {
  		warn("Failed to connect to VDR.");
  		exit(errno);
  	}

  	//compile pattern for regexec later in a different function
  	if (regcomp(&regbuf,eod_pattern,REG_EXTENDED | REG_NOSUB | REG_NEWLINE)!=0) {
  		warn("Error compiling the pattern!");
  		exit(1);
  	}
    dbg("SVDRP connection to 127.0.0.1 established.");
  	data=read_svdrp();
  	free(data);
  } else {
  	dbg("SVDRP connection to 127.0.0.1 is already up.");
  }
  return 0;
}

int close_svdrp() {
  char * data;
  dbg("Try to close SVDRP connection.");
  write_svdrp("QUIT\r");
  data=read_svdrp();
  free(data);
  close(svdrp_socket);
  svdrp_socket=0;
  if (svdrp_socket==0) { 
  	dbg("SVDRP connection closed."); 
  	return 1; } 
  else { 
  	warn("Can't close SVDRP socket!"); 
  	return 0; 
  }
}

char * read_svdrp() {
  char * data;
  char buffer[SVDRP_BUFFER_SIZE]="";
  int l=0; //current length without trailing 0
  int n=0;
  int eod_matches=1;

  if (svdrp_socket==0) {
  	warn("No socket found!");
  	return NULL;
  }

  data=NULL;
  while ( (n=recv(svdrp_socket,buffer,SVDRP_BUFFER_SIZE-1,0))>0 ) {
    buffer[n]='\0';
    char * tmp=realloc(data,l+n+1);
    if (!tmp) {
      warn("(Re)allocation failed. Old size is %d, new size should be %d",(data==NULL)?0:l+1,l+n+1);
      exit(1);
    }
    if (data==NULL) {
      tmp[0]='\0';
    }
    data=tmp;
    strcat(data,buffer); 
    l+=n;
    if (eod_matches!=0) {
      int offset=(l>n+5) ? l-n-5 : 0;
      eod_matches=regexec(&regbuf,data+offset,0,NULL,0);
    }
    if ((eod_matches==0) && (data[l-1]=='\n')){
      break;
    }
  }
  return data;
}

int write_svdrp(char *data) {
	dbg("Sending data to VDR [%s]",data);
  if (open_svdrp()==0) {
    return write(svdrp_socket,data,strlen(data)+1);
  } else {
    warn("Can't establish connection to VDR via SVDRP. Exit.");
    return 0;
  }
}

void whatsmyip(char myip[16]) {
int fd;
int ret;
struct ifreq ifr;

  fd = socket(PF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
	perror("socket");
	return;
  }

  strcpy(ifr.ifr_name, "eth0");
  ifr.ifr_addr.sa_family = AF_INET;
  ret = ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);

  if (ret < 0) {
	perror("ioctl");
	return;
  }
  strcpy(myip,inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
}


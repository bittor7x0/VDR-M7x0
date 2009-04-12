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

#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <u/libu.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "conf.h"
#include "svdrp_comm.h"

enum {SVDRP_BUFFER_SIZE=4096};

int svdrp_socket=0;
char svdrpServerIp[16];
uint16_t svdrpServerPort;

const char eod_pattern[]="^[0-9]{3} ";
regex_t regbuf;

void signal_handler(int sig) {
	dbg("Got signal [%s]",sig);
	if (svdrp_socket!=0) {
		warn("Broken Pipe.");
		svdrp_socket=0;
	}
}

//Set server ip and port
void setSvdrpServerAddress(session_t * session){
	const char * sessSvdrpServerIp=session_get(session,"svdrpServerIp");
	const char * sessSvdrpServerPort=session_get(session,"svdrpServerPort");
	strncpy(svdrpServerIp,(sessSvdrpServerIp==NULL)?webifConf.svdrpIp:sessSvdrpServerIp,15);
	if (sessSvdrpServerPort==NULL) {
		svdrpServerPort=webifConf.svdrpPort;
	}
	else {
		errno=0;
		svdrpServerPort=(int)strtol(sessSvdrpServerPort,NULL,10);
		if (errno!=0) {
			warn("Incorrect port %s set in session",sessSvdrpServerPort);
			svdrpServerPort=webifConf.svdrpPort;
		}
	}
}


//Open the connection
int open_svdrp() {
	char * data;
	pid_t pid;
	pid=getpid();
	signal(SIGPIPE,signal_handler);

	if (svdrp_socket<1) {
		struct sockaddr_in svdrpServerAddr;
		dbg("Try to open SVDRP connection to %s:%d from pid [%d]",svdrpServerIp,svdrpServerPort,pid);
		svdrpServerAddr.sin_family = AF_INET;
		svdrpServerAddr.sin_port = htons(svdrpServerPort);
		inet_aton(svdrpServerIp, &svdrpServerAddr.sin_addr);

		// create tcp socket
		errno=0;
		if ((svdrp_socket=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("open_svdrp()");
			svdrp_socket=0;
			return errno;
		}

		// Connect to VDR
		errno=0;
		if (connect(svdrp_socket, (struct sockaddr*)&svdrpServerAddr, sizeof(svdrpServerAddr)) < 0) {
			perror("open_svdrp()");
			close(svdrp_socket);
			svdrp_socket=0;
			return errno;
		}

		//compile pattern for regexec later in a different function
		if (regcomp(&regbuf,eod_pattern,REG_EXTENDED | REG_NOSUB | REG_NEWLINE)!=0) {
			warn("Error compiling the pattern!");
			exit(1);
		}
		dbg("SVDRP connection to %s  established.",svdrpServerIp);
		data=read_svdrp();
		free(data);
	} else {
		dbg("SVDRP connection to %s is already up.",svdrpServerIp);
	}
	return 0;
}

void close_svdrp() {
	char * data;
	dbg("Try to close SVDRP connection.");
	if (write_svdrp("QUIT\r")<=0){
		warn("Can't connect with SVDRP server!");
		return;
	}
	data=read_svdrp();
	free(data);
	errno=0;
	if (shutdown(svdrp_socket,SHUT_RDWR)<0){
		perror("close_svdrp()");
		switch (errno) {
			case EBADF:
			case ENOTSOCK:
				warn("Not a valid socket");
				svdrp_socket=0;
				return;
			case ENOTCONN:
				warn("Socket wasn't connected");
				svdrp_socket=0;
				return;
			default:
				break;
		}
		errno=0;
	}
	if (close(svdrp_socket)<0) {
		perror("close_svdrp()");
		errno=0;
	}
	svdrp_socket=0;
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
			warn("read_svdrp:Reallocation failed.");
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

int isVdrLocal(){
	return (strncmp(svdrpServerIp,"127.",4)==0);
}


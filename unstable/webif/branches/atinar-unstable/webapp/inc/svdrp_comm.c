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
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <klone/utils.h>

#include "conf.h"
#include "svdrp_comm.h"

void closeSocket(hostConf_t *host){
	errno=0;
	if (shutdown(host->socket,SHUT_RDWR)<0){
		warn(strerror(errno));
		switch (errno) {
			case EBADF:
			case ENOTSOCK:
			case ENOTCONN:
				host->socket=0;
				return;
			default:
				break;
		}
		errno=0;
	}
	if (close(host->socket)<0) {
		warn(strerror(errno));
		errno=0;
	}
	host->socket=0;
}

void readSvdrp(hostConf_t *host, char **pdata, regex_t *const eod_regex) {
	enum {BUFFERSZ=4096};
	char buffer[BUFFERSZ]="";
	int l=0; //current length without trailing 0
	int n=0;
	int eod_matches=1;

	if (host->socket==0) {
		warn("No socket found!");
		return;
	}
	(*pdata)=NULL;
	while ( (n=recv(host->socket,buffer,BUFFERSZ-1,0))>0 ) {
		buffer[n]='\0';
		crit_goto_if(((*pdata)=realloc((*pdata),l+n+1))==NULL,outOfMemory);
		(*pdata)[l]=0;
		strcat((*pdata),buffer);
		l+=n;
		if (eod_matches!=0) {
			int offset=(l>n+5) ? l-n-5 : 0;
			eod_matches=regexec(eod_regex,(*pdata)+offset,0,NULL,0);
		}
		if ((eod_matches==0) && ((*pdata)[l-1]=='\n')){
			break;
		}
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

boolean_t openSvdrp(hostConf_t *host, char **pdata, regex_t *const eod_regex) {
	if (host->socket<1) {
		struct sockaddr_in hostAddress;
		dbg("Opening SVDRP connection to %s:%d",host->ip,host->port);
		hostAddress.sin_family = AF_INET;
		hostAddress.sin_port = htons(host->port);
		inet_aton(host->ip, &hostAddress.sin_addr);

		// create tcp socket
		errno=0;
		if ((host->socket=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			warn(strerror(errno));
			host->socket=0;
			return BT_FALSE;
		}

		// Connect to VDR
		errno=0;
		if (connect(host->socket, (struct sockaddr*)&hostAddress, sizeof(hostAddress)) < 0) {
			warn(strerror(errno));
			closeSocket(host);
			return BT_FALSE;
		}

		readSvdrp(host,pdata,eod_regex);
		if (strtol(*pdata,NULL,10)!=220){
			warn("Unexpected SVDRP response: %s",*pdata);
			//TODO cerrar mejor la conexion
			closeSocket(host);
		}
		free(*pdata);
		*pdata=NULL;
		return boolean(host->socket>0);
	} else {
		return BT_TRUE;
	}
}

void closeSvdrp(hostConf_t *host) {
	if (host && host->isVdr && host->socket>0){
		dbg("Closing SVDRP connection to %s:%d",host->ip,host->port);
		free(execSvdrp(host,"QUIT"));
		closeSocket(host);
	}
}

void closeSvdrpAll() {
	int h;
	hostConf_t *host;
	for (h=0,host=webifConf.hosts;h<webifConf.hostsLength;h++,host++){
		if (host->isVdr && host->socket>0) {
			closeSvdrp(host);
		}
	}
}

boolean_t writeSvdrp(hostConf_t *host, const char *cmd) {
	dbg("Sending SVDRP cmd [%s]",cmd);
	int l=strlen(cmd);
	int w=write(host->socket,cmd,l+1);
	if (w<l) warn("Written only %d bytes from %d",w,l);
	return boolean(w>=l);
}

static jmp_buf execSvdrpContext;

void signalHandler(int sig) {
	dbg("Got signal [%s]",sig);
	longjmp(execSvdrpContext,sig);
}
		
char *execSvdrp(hostConf_t * const host, const char *cmd){
	if (host && host->isVdr){
		regex_t eod_regex; //regex to detect End Of Data

		dbg("Executing SVDRP cmd [%s]",cmd);
		boolean_t isQuit=boolean(strncmp(cmd,"QUIT",4)==0);
		if (isQuit && host->socket<1){
			warn("SVDRP connection already closed before QUIT");
			return NULL;
		}
		if (regcomp(&eod_regex,"^[0-9]{3} ",REG_EXTENDED | REG_NOSUB | REG_NEWLINE)!=0) {
			crit(strerror(errno));
			exit(1);
		}
		sighandler_t oldSignalHandler=signal(SIGPIPE,signalHandler);
		volatile char *data=NULL;
		char *result=NULL;
		if (setjmp(execSvdrpContext)!=0){
			warn("Broken Pipe.");
			free((char *)data);
			data=NULL;
			host->socket=0;
		} else {
			if (openSvdrp(host,(char **)&data,&eod_regex)) {
				free((char *)data);
				data=NULL;
				if (writeSvdrp(host,cmd)) {
					readSvdrp(host,(char **)&data,&eod_regex);
					result=(char *)data;
					data=NULL;
				}
				if (webifConf.alwaysCloseSvdrp && !isQuit){
					if (writeSvdrp(host,"QUIT")) {
						readSvdrp(host,(char **)&data,&eod_regex);
						free((char *)data);
						closeSocket(host);
					}
					data=NULL;
				}
			} else {
				warn("Can't establish SVDRP connection");
				return NULL;
			}
		}
		signal(SIGPIPE,oldSignalHandler);
		regfree(&eod_regex);
		return result;
	} else {
		warn("Not a valid host [id=%d]", host->id);
		return NULL;
	}
}


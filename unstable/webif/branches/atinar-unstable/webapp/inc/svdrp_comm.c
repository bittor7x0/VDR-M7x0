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

char *readSvdrp(hostConf_t *host) {
	enum {BUFFERSZ=4096};
	char buffer[BUFFERSZ]="";
	int l=0; //current length without trailing 0
	int n=0;
	bool eod=false; //end of data code already received (but last line may be still incomplete)

	if (host->socket<1) {
		warn("No socket found!");
		return NULL;
	}
	char *data=NULL;
	while ( (n=recv(host->socket,buffer,BUFFERSZ-1,0))>0 ) {
		buffer[n]='\0';
		crit_goto_if((data=realloc(data,l+n+1))==NULL,outOfMemory);
		data[l]=0;
		strcat(data,buffer);
		l+=n;
		if (!eod) {
			int offset=(l>n+5) ? l-n-5 : 0;
			eod=boolean(regexec(&webifConf.eod_regex,data+offset,0,NULL,0)==0);
		}
		if (eod && (data[l-1]=='\n')){
			break;
		}
	}
	if (n<0 || !eod){
		if (n<0)
			warn(strerror(errno));
		else
			warn("SVDRP response truncated [%s]. Discarding everything",data);
		free(data);
		data=NULL;
		errno=0;
	}
	return data;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool openSvdrp(hostConf_t *host) {
	if (host->socket<1) {
		struct sockaddr_in hostAddress;
		//dbg("Opening SVDRP connection to %s:%d",host->ip,host->port);
		hostAddress.sin_family = AF_INET;
		hostAddress.sin_port = htons(host->port);
		inet_aton(host->ip, &hostAddress.sin_addr);

		// create tcp socket
		errno=0;
		if ((host->socket=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			warn(strerror(errno));
			host->socket=0;
			return false;
		}

		// Connect to VDR
		errno=0;
		if (connect(host->socket, (struct sockaddr*)&hostAddress, sizeof(hostAddress)) < 0) {
			warn(strerror(errno));
			closeSocket(host);
			return false;
		}
		char *data=readSvdrp(host);
		if (data==NULL || strtol(data,NULL,10)!=SVDRP_VDR_READY){
			warn("Unexpected SVDRP response: %s",data);
			closeSocket(host);
		}
		free(data);
		return boolean(host->socket>0);
	} else {
		return true;
	}
}

void closeSvdrp(hostConf_t *host) {
	if (host && host->isVdr && host->socket>0){
		//dbg("Closing SVDRP connection to %s:%d",host->ip,host->port);
		free(execSvdrp(host,"QUIT"));
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

bool writeSvdrp(hostConf_t *host, const char *cmd) {
	dbg("SVDRP cmd [%s]",cmd);
	if (host->socket<1){
		warn("socket is closed");
		return false;
	} else {
		int l=strlen(cmd);
		int w=write(host->socket,cmd,l+1);
		if (w<l) warn("Written only %d bytes from %d",w,l);
		return boolean(w>=l);
	}
}

char *execSvdrp(hostConf_t * const host, const char *cmd){
	if (host && host->isVdr){
		bool isQuit=boolean(strncmp(cmd,"QUIT",4)==0);
		if (isQuit && host->socket<1){
			warn("SVDRP connection already closed before QUIT");
			return NULL;
		}
		//sighandler_t oldSignalHandler=signal(SIGPIPE,SIG_IGN);
		//if (oldSignalHandler==SIG_IGN){
		//	dbg("ya era SIG_IGN"); //TODO
		//}
		char *data=NULL;
		if (isQuit || openSvdrp(host)) {
			if (writeSvdrp(host,cmd)) {
				data=readSvdrp(host);
			}
			if (isQuit){
				closeSocket(host);
			}
		} else {
			warn("Can't establish SVDRP connection");
		}
		//signal(SIGPIPE,oldSignalHandler);
		return data;
	} else {
		warn("Not a valid host [id=%d]", host->id);
		return NULL;
	}
}


#ifndef __SVDRPCLIENT_H
#define __SVDRPCLIENT_H

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "log.h"

#define SVDRPCONNECT 220
#define SVDRPDISCONNECT 221
#define CMDSUCCESS 250

class cSVDRPClient {
 private:
    int sock;
public:
    bool bConnected;

    cSVDRPClient(int Port)
	{
	    bConnected = false;
	    sock  = socket(PF_INET, SOCK_STREAM, 0);
	    if (sock < 0) 
	    {
		LogFile.eSysLog("error creating socket!");
		return;
	    }

	    struct sockaddr_in AdrSock;
	    AdrSock.sin_family = AF_INET;
	    AdrSock.sin_port   = htons(EPGSearchConfig.SVDRPPort);
	    inet_aton("127.0.0.1", &AdrSock.sin_addr);

	    if (connect(sock, (struct sockaddr*)&AdrSock, sizeof(AdrSock)) == -1)
	    {
		LogFile.eSysLog("error connecting to socket!");
		return;
	    }
	    bConnected = (Receive() == SVDRPCONNECT);
	    if (!bConnected)
		LogFile.eSysLog("EPGSearch: could not connect to VDR!");
	}

    long getAddrFromString(char* hostnameOrIp, struct sockaddr_in* addr)
	{
	    unsigned long ip;

	    struct hostent * he;

	    if(hostnameOrIp==NULL || addr==NULL)
		return -1;

	    ip=inet_addr(hostnameOrIp);

	    if(ip!=INADDR_NONE)
	    {
		addr->sin_addr.s_addr=ip;		
		return 0;
	    }
	    else
	    {
		he=gethostbyname(hostnameOrIp);
		if(he==NULL)
		    return -1;
		else
		    memcpy(&(addr->sin_addr),he->h_addr_list[0],4);
		return 0;
	    }
	}

    bool SendCmd(char* cmd) 
	{
	    if (!bConnected)
		return false;

	    char* szCmd = NULL;

	    asprintf(&szCmd, "%s\r\n", cmd);
	    Send(szCmd);
	    free(szCmd);
	    bool cmdret = (Receive() == CMDSUCCESS);

	    asprintf(&szCmd, "QUIT\r\n");
	    Send(szCmd);
	    free(szCmd);
	    long rc = 0;
	    if ((rc = Receive()) != SVDRPDISCONNECT)
		LogFile.eSysLog("could not disconnect (%ld)!", rc);
	    
	    close(sock);
	    return cmdret;
	}
    bool Send(char* szSend) 
	{
	    int length = strlen(szSend);
	    int sent = 0;
	    do
	    {
		sent += send(sock, szSend + sent, length - sent, 0);
		if( sent < 0 )
		{
		    LogFile.eSysLog("error sending command!");
		    return false;
		}
	    }
	    while ( sent < length );
	    return true;
	}
    long Receive() 
	{
	    char* csResp = strdup("");
	    char ch;
	    long rc = 0;

	    bool bCheckMultiLine = true;

	    while(bCheckMultiLine)
	    {
		while( strlen(csResp) < 2 || strcmp(csResp + strlen(csResp) - 2, "\r\n") != 0)
		{
		    if (recv(sock, &ch, 1, 0 ) < 0)
		    {
			LogFile.eSysLog("EPGSearch: error receiving response!");
			return -1;
		    }
		    char* Temp = NULL;
		    asprintf(&Temp, "%s%c", csResp, ch);
		    free(csResp);
		    csResp = Temp;
		}
		if( csResp[3] == ' ' )
		{
		    bCheckMultiLine = false;
		    rc = atol(csResp);
		}
		free(csResp);
		csResp = strdup("");
	    }		
	    free(csResp);
	    return rc;
	}

    static const char *SVDRPSendCmd;
};

#endif

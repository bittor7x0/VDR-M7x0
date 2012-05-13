#include "httpplayer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug.h"


int CopyLine(uchar *dst, int dstlen, uchar *src, int srclen) {
	int i=0;

	// Copy until end of buffer or end of line
	while (srclen>0 && *src!='\n') {
		if (i<dstlen) dst[i]=*src;
		i++;
		src++;
		srclen--;
	}

	if (srclen<=0) return 0;
	// no end of line found, fail.

	if (i<dstlen) {
		// Buffer space left. Terminate string.
		dst[i]=0;
		if (i>0 && dst[i-1]=='\r') dst[i-1]=0;
	} else {
		// Output buffer overflow. Truncate.
		dst[dstlen-1]=0;
	}

	return i+1;
}

/*
char* ParseEndLine(char *(&p), int &len) {
	int i=0;

	while (i<len && p[i]!='\n') i++;
	if (i>=len) return NULL;

	p[i]=0;
	if (i>0 && p[i-1]=='\r') p[i-1]=0;

	i++;
	char *res=p;
	p+=i;
	len-=i;
	return res;
}
*/

//
// cDataStreamSocketTCP
//

cDataStreamSocketTCP::cDataStreamSocketTCP(char *address, uint16_t port, char *request) {
	Address=strdup(address);
	Port=port;
	Request=strdup(request);

	pSrcBuffer=SrcBuffer;
	nSrcBuffer=0;

	Socket=0;
	State=stClosed;
	LastConn=0;
	activitycounter=0;
}

cDataStreamSocketTCP::~cDataStreamSocketTCP() {
	free(Address);
	free(Request);
}

inline int setsockopt_int(int s, int  level,  int  optname,  int optval) {
	return setsockopt(s,level,optname,&optval,sizeof optval);
}

bool cDataStreamSocketTCP::Open() {
	sockaddr_in sConn;
	int res;
	
	bzero(&sConn,sizeof sConn);

	LastConn=time(NULL);
	activitycounter=(activitycounter+1)&0xff;

	sConn.sin_family = AF_INET;
	hostent *h=gethostbyname(Address);
	if (!h) {
		ReportError("%s: gethostbyname %s.\n",tr("Error"),tr("failed"));
		return false;
	}
	memcpy(&sConn.sin_addr,h->h_addr,h->h_length);
	sConn.sin_port = htons ( Port );

	Socket=socket(AF_INET,SOCK_STREAM,0);
	if (Socket<0) {
		Socket=0;
		ReportError("%s: socket %s.\n",tr("Error"),tr("failed"));
		return false;
	}
	
	// Set socket reuse options
	if (setsockopt_int(Socket,SOL_SOCKET,SO_REUSEADDR,1) == -1) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_REUSEADDR,1,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}
	#ifdef SO_REUSEPORT
	if (setsockopt_int(Socket,SOL_SOCKET,SO_REUSEPORT,1) == -1) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_REUSEPORT,1,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}
	#endif

	// Set socket buffer size to 512k
	if (setsockopt_int(Socket,SOL_SOCKET,SO_RCVBUF,512*1024) == -1) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_RCVBUF,512*1024,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}

	// Set non-blocking	
	res=fcntl(Socket,F_SETFL,O_NONBLOCK);
	if (res<0) {
		ReportError("%s: fcntl(*,F_SETFL,O_NONBLOCK) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
		return false;
	}

	_dprintf("Connecting...\n");
	res=connect(Socket,(sockaddr*)&sConn,sizeof sConn);
	if (res<0 && errno!=EINPROGRESS) {
		ReportError("%s: connect() %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
		return false;
	}
	State=stConnecting;

	return true;
}

void cDataStreamSocketTCP::Close() {
	_dprintf("Closing...\n");
	close(Socket);
	Socket=0;
	State=stClosed;
}


uchar* cDataStreamSocketTCP::Get(int &Count) {
	if (nSrcBuffer>0) {
		// return remains
		Count=nSrcBuffer;
		return pSrcBuffer;
	}
	// Buffer is empty, try getting some data
	
	
	if (State==stClosed) {
		// Do not open too quickly
		if (time(NULL)-LastConn>CONNECT_WAIT) Open();
	}
	if (State==stConnecting) {
		pollfd pfd;
		pfd.fd=Socket;
		pfd.events=POLLOUT;

		if (0!=poll(&pfd,1,0)) {
			int err;
			socklen_t len=sizeof err;
			getsockopt(Socket,SOL_SOCKET,SO_ERROR,&err,&len);

			if (err!=0) {
				ReportError("%s: Connecting %s: %s.\n",tr("Error"),tr("failed"),strerror(err));
				Close();
			} else {
				_dprintf("Connected.\n");
				LastConn=time(NULL);
				activitycounter=(activitycounter+1)&0xff;
				State=stRequest;
			}
		} else if (time(NULL)-LastConn>TRANSMIT_TIMEOUT) {
			ReportError("%s: %s.",tr("Error"),tr("Connect timed out"));
			Close();
		}
	}
	if (State==stRequest) {
		_dprintf("Send Request:\n%s",Request);
		int res=send(Socket,Request,strlen(Request),0);

		if (res>0) State=stReceive;

		if (res<0 && errno!=EAGAIN && errno!=EWOULDBLOCK) {
			Close();
		}
	}
	if (State==stReceive) {
		int res=recv(Socket,SrcBuffer,sizeof SrcBuffer,0);
		if (res>0) {
			pSrcBuffer=SrcBuffer;
			nSrcBuffer=res;
			activitycounter=(activitycounter+1)&0xff;
			LastConn=time(NULL);
		}
		if (res==0) {
			ReportError("%s\n",tr("connection closed by peer"));
			Close();
		}
		if (res<0) {
			if (errno!=EAGAIN) {
				ReportError("%s: recv %s: %s.\n",tr("Error"),tr("failed"),strerror(errno));
				Close();
			} else if (time(NULL)-LastConn>TRANSMIT_TIMEOUT) {
				ReportError("%s: %s.\n",tr("Error"),tr("Connection timed out"));
				Close();
			}
		}
	}

	Count=nSrcBuffer;
	if (nSrcBuffer>0) return pSrcBuffer;

	return NULL;
}

void cDataStreamSocketTCP::Del(int Count) {
	nSrcBuffer-=Count;
	pSrcBuffer+=Count;
}

void cDataStreamSocketTCP::Reset() {
	nSrcBuffer=0;
}

int cDataStreamSocketTCP::GetPollHandle() {
	if (Socket>0 && nSrcBuffer==0) return Socket;
	return 0;
}



//
// cFilterHTTPHeader
//

cFilterHTTPHeader::cFilterHTTPHeader(cDataStreamSource *Src)
	:cDataStreamProcessor(Src) {
	State=esStart;
}

void cFilterHTTPHeader::ProcessData(uchar *Data, int Count, int &Forwardable, int &Deletable) {
	Forwardable=0;
	Deletable=0;

	if (State==esFinished) {
		Forwardable=Count;
		return;
	}
	if (State==esErrorHTTPNotFound) return;
	if (State==esErrorHTTPResponseFailed) return;

	if (State==esStart) {
		uchar line[80];
		int response;
		int res=CopyLine(line,sizeof line,Data,Count);
		if (res==0) return;
		Deletable+=res;
		Data+=res;
		Count-=res;

		if (1!=sscanf((char*)line,"HTTP/1.%*i %i ",&response)) {
			ReportError("%s: %s: \"%s\"\n",tr("Error"),tr("Expected HTTP header not found"),line);
			State=esErrorHTTPNotFound;
			return;
		}
		if (response!=200) {
			ReportError("%s: %s %i, %s.\n",tr("Error"),tr("HTTP response"),response,tr("failed"));
			State=esErrorHTTPResponseFailed;
			return;
		}
		State=esEatHeader;
	}
	while (State==esEatHeader) {
		uchar line[80];
		int res=CopyLine(line,sizeof line,Data,Count);
		if (res==0) return;
		Deletable+=res;
		Data+=res;
		Count-=res;

		if (line[0]==0) {
			_dprintf("Empty line found, end headers.\n");
			State=esFinished;
			// Pass through all remaining data
			Forwardable=Count;
		} else {
			_dprintf("Header Line: %s\n",line);
		}
	}
}

//
// cStreamPlayerHTTP
//


cStreamPlayerHTTP::cStreamPlayerHTTP(char *url) {
	Src=NULL;
	http=NULL;
	ParseURL(url);

	_dprintf("Playing http://%s:%i%s\n",address,port,path);
}

cStreamPlayerHTTP::~cStreamPlayerHTTP() {
	// Kill thread before deepest nested destructor finishes
	// Not good style, but not to avoid.
	Detach();
	Stop();
}


cDataStreamSource* cStreamPlayerHTTP::NewSource() {
	char buffer[256];
	snprintf(buffer,sizeof buffer,
		"GET %s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"User-Agent: streamplayer\r\n"
		"Range: bytes=0-\r\n"
		"Connection: Close\r\n"
		"\r\n",path,address);
	Src=new cDataStreamSocketTCP(address,port,buffer);
	return Src;
}

void cStreamPlayerHTTP::DeleteSource(cDataStreamSource *src) {
	delete src;
	Src=NULL;
}

cDataStreamSource* cStreamPlayerHTTP::NewPostBufferFilters(cDataStreamSource* src) {
	http=new cFilterHTTPHeader(src);
	return http;
}

void cStreamPlayerHTTP::DeletePostBufferFilters(cDataStreamSource* filter) {
	if (filter) delete filter;
	http=NULL;
}

void cStreamPlayerHTTP::ParseURL(char *url) {
	char *dest;
	char cport[8];
	int len;

	address[0]=0;
	port=80;
	path[0]=0;

	while (*url=='/') url++;

	dest=address;
	len=sizeof address;
	for(;;) {
		if (*url==0 || *url==':' || *url=='/') break;
		if (len<2) break;
		*dest=*url;
		url++;
		dest++;
		len--;
	}
	*dest=0;

	if (*url==':') {
		url++;
		dest=cport;
		len=sizeof cport;
		for (;;) {
			if (*url==0 || *url=='/') break;
			if (len<2) break;
			*dest=*url;
			url++;
			dest++;
			len--;
		}
		*dest=0;
		port=atoi(cport);
	}

	dest=path;
	len=sizeof path;
	if (*url!='/') {
		*dest='/';
		dest++;
		len--;
	}
	for(;;) {
		if (*url==0) break;
		if (len<2) break;
		*dest=*url;
		url++;
		dest++;
		len--;
	}
	*dest=0;
}

char* cStreamPlayerHTTP::GetStatus() {
	int bps=0;
	int buffered=0;
	cFilterHTTPHeader::eState httpstate;
	char *activity="........";

	if (Src==NULL) {
		return NULL;
	}

	activity+=7-(Src->GetActivityCounter()&7);

	switch (Src->GetState()) {
	case cDataStreamSocketTCP::stClosed:
		snprintf(StatusString,sizeof StatusString,"%s http://%s:%i%s, %s%s",tr("Failed to connect to"),address,port,path,tr("waiting"),activity);
		break;

	case cDataStreamSocketTCP::stConnecting:
	case cDataStreamSocketTCP::stRequest:
		snprintf(StatusString,sizeof StatusString,"%s http://%s:%i%s %s",tr("Connecting to"),address,port,path,activity);
		break;

	case cDataStreamSocketTCP::stResponse:
		snprintf(StatusString,sizeof StatusString,"%s http://%s:%i%s %s",tr("Waiting for response from"),address,port,path,activity);
		break;

	case cDataStreamSocketTCP::stReceive:
		if (Throughput) bps=Throughput->GetBytesPerSec();
		if (StreamBuffer) buffered=StreamBuffer->Available();

		httpstate=cFilterHTTPHeader::esStart;
		if (http) httpstate=http->GetState();

		switch (httpstate) {
		case cFilterHTTPHeader::esStart:
		case cFilterHTTPHeader::esEatHeader:
			snprintf(StatusString,sizeof StatusString,"%s http://%s:%i%s (%0.1f Kb/s, buf %ik)",tr("Reading Headers from"),address,port,path,bps/1024.0,buffered/1024);
			break;

		case cFilterHTTPHeader::esFinished:
			snprintf(StatusString,sizeof StatusString,"%s http://%s:%i%s (%0.1f Kb/s, buf %ik)",tr("Receiving from"),address,port,path,bps/1024.0,buffered/1024);
			break;

		case cFilterHTTPHeader::esErrorHTTPNotFound:
			snprintf(StatusString,sizeof StatusString,"%s: %s.",tr("Error"),tr("HTTP header not found"));
			break;

		case cFilterHTTPHeader::esErrorHTTPResponseFailed:
			snprintf(StatusString,sizeof StatusString,"%s: %s.",tr("Error"),tr("HTTP response error"));
			break;
		}

		break;

	default:
		StatusString[0]=0;
		break;
	}
	return StatusString;
}


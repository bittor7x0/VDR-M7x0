#include "udpplayer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "debug.h"

//
// cDataStreamSocketUDP
//

cDataStreamSocketUDP::cDataStreamSocketUDP(char *address, uint16_t port) { 
	Port=port; 
	Address=NULL;
	if (address) Address=strdup(address);
	sock=0; 
	pSrcBuffer = SrcBuffer;
	nSrcBuffer = 0;
}

cDataStreamSocketUDP::~cDataStreamSocketUDP() {
	Close();
	free(Address);
}

inline int setsockopt_int(int s, int  level,  int  optname,  int optval) {
	return setsockopt(s,level,optname,&optval,sizeof optval);
}

bool cDataStreamSocketUDP::Open() {
	sockaddr_in sServ;
	int res;

	sock=socket(AF_INET,SOCK_DGRAM,0);
	if (sock<0) {
		sock=0;
		return false;
	}

	// Set socket reuse options
	if (setsockopt_int(sock,SOL_SOCKET,SO_REUSEADDR,1) == -1) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_REUSEADDR,1,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}
	#ifdef SO_REUSEPORT
	if (setsockopt_int(sock,SOL_SOCKET,SO_REUSEPORT,1) == -1) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_REUSEPORT,1,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}
	#endif

	// Set socket buffer size to 512k
	if (setsockopt_int(sock,SOL_SOCKET,SO_RCVBUF,512*1024) == -1) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_RCVBUF,512*1024,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}

	// Bind address
	bzero(&sServ,sizeof sServ);
	sServ.sin_family = AF_INET;
	sServ.sin_addr.s_addr = htonl ( INADDR_ANY );
	sServ.sin_port = htons ( Port );

	res=bind(sock,(sockaddr*)&sServ,sizeof sServ);
	if (res<0) {
		ReportError("%s: bind() %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
		return false;
	}

	// Set non-blocking
	res=fcntl(sock,F_SETFL,O_NONBLOCK);
	if (res<0) {
		ReportError("%s: fcntl(*,F_SETFL,O_NONBLOCK) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
		return false;
	}
	
	// Enable broadcast support for socket
	res=setsockopt_int(sock, SOL_SOCKET, SO_BROADCAST, 1);
	if (res<0) {
		ReportError("%s: setsockopt(*,SOL_SOCKET,SO_BROADCAST,*,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
	}

	if (Address) {
		hostent *h=gethostbyname(Address);
		if (!h) {
			ReportError("%s: gethostbyname %s.\n",tr("Error"),tr("failed"));
			return false;
		}
		in_addr addr;
		memcpy(&addr,h->h_addr,h->h_length);
		
		
		// Join multicast group
		if (IN_MULTICAST(ntohl(addr.s_addr))) {
			_dprintf("Joining multicast group\n");
			ip_mreq mreq;
			mreq.imr_interface.s_addr = htonl ( INADDR_ANY );
			mreq.imr_multiaddr.s_addr=addr.s_addr;
			res=setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof mreq);
			if (res<0) {
				ReportError("%s: setsockopt(*,IPPROTO_IP,IP_ADD_MEMBERSHIP,*,*) %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
			}
		}
		
		// connect socket to source 
		sockaddr_in sSource;
		bzero(&sSource,sizeof sSource);
		sSource.sin_family = AF_INET;
		sSource.sin_addr.s_addr=addr.s_addr;
		
		res=connect(sock,(sockaddr*)&sSource,sizeof sSource);
		if (res<0) {
			ReportError("%s: connect() %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
			return false;
		}
	}

	return true;
}

void cDataStreamSocketUDP::Close() {
	if (sock) {
		close(sock);
	}
	sock=0;
}

uchar* cDataStreamSocketUDP::Get(int &Count) {
	if (nSrcBuffer>0) {
		// return remains
		Count=nSrcBuffer;
		return pSrcBuffer;
	}

	if (!sock) {
		if (!Open()) return NULL;
	}

	int r=recvfrom(sock,SrcBuffer,sizeof SrcBuffer,0,NULL,NULL);
	if (r<0) {
		if (errno==EAGAIN) return NULL;
			ReportError("%s: recvfrom() %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
		return NULL;
	}
	pSrcBuffer=SrcBuffer;
	nSrcBuffer=r;

	Count=nSrcBuffer;
	return pSrcBuffer;
}


void cDataStreamSocketUDP::Del(int Count) {
	nSrcBuffer-=Count;
	pSrcBuffer+=Count;
}

void cDataStreamSocketUDP::Reset() {
	nSrcBuffer=0;
}

int cDataStreamSocketUDP::GetPollHandle() {
	if (sock>0 && nSrcBuffer==0) return sock;
	return 0;
}

//
// cStreamPlayerUDP
//

cStreamPlayerUDP::cStreamPlayerUDP(char *url) {
	address=NULL;
	port=1234;
	
	ParseURL(url);  
	
	if (address) {
		_dprintf("Listening on UDP address %s port %i\n",address,port);
	} else {
		_dprintf("Listening on UDP port %i\n",port);
	}
}

cStreamPlayerUDP::~cStreamPlayerUDP() {
	// Kill thread before deepest nested destructor finishes
	// Not good style, but not to avoid.
	Detach();
	Stop();
}

void cStreamPlayerUDP::ParseURL(char *url) {
	// scan URL string format
	if (url[0]>='0' && url[0]<='9') {
		// old style 'udp:'port
		port=atoi(url);
	} else if (url[0]=='/' && url[1]=='/') {
		// new style 'udp://'['@'[address][':'port]]
		url+=2;
		
		while (*url!=0 && *url!='@') url++;
		// skip all before '@' - this part has currently no meaning.

		if (*url==0) return;
		// all default - no '@'
		
		// now url points to '@'
		
		url++;
		// now url points to start of address or ':'
		
		int len=0;
		
		while (url[len]!=0 && url[len]!=':') len++;
		if (len>0) {
			// copy address part
			address=(char*)malloc(len+1);
			memcpy(address,url,len);
			address[len]=0;
			url+=len;
		}
			
		if (*url==0) return;
		
		// now url points to ':'
		url++;
		
		port=atoi(url);
	}
}

cDataStreamSource* cStreamPlayerUDP::NewSource() {
	return new cDataStreamSocketUDP(address,port);
}

void cStreamPlayerUDP::DeleteSource(cDataStreamSource *src) {
	delete src;
}


char* cStreamPlayerUDP::GetStatus() {
	int bps=0;
	int buffered=0;
	if (Throughput) bps=Throughput->GetBytesPerSec();
	if (StreamBuffer) buffered=StreamBuffer->Available();

	snprintf(StatusString,sizeof StatusString,"%s %i (%0.1f Kb/s, buf %ik)",tr("Listening on UDP port"),port,bps/1024.0,buffered/1024);
	return StatusString;
}


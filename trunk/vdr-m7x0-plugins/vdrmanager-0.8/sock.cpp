/*
 * extendes sockets
 */
#include <unistd.h>
#include <vdr/plugin.h>
#include "sock.h"
#include "helpers.h"

static int clientno = 0;

/*
 * cVdrmonSocket
 */
cVdrmanagerSocket::cVdrmanagerSocket() {
	sock = -1;
}

cVdrmanagerSocket::~cVdrmanagerSocket() {
	Close();
}

void cVdrmanagerSocket::Close() {
	if (sock >= 0) {
		close(sock);
		sock = -1;
	}
}

int cVdrmanagerSocket::GetSocket() {
	return sock;
}

bool cVdrmanagerSocket::MakeDontBlock() {
	// make it non-blocking:
	int oldflags = fcntl(sock, F_GETFL, 0);
	if (oldflags < 0) {
		LOG_ERROR;
		return false;
	}
	oldflags |= O_NONBLOCK;
	if (fcntl(sock, F_SETFL, oldflags) < 0) {
		LOG_ERROR;
		return false;
	}

	return true;
}

const char * cVdrmanagerSocket::GetPassword() {
	return password;
}

/*
 * cVdrmonServerSocket
 */
cVdrmanagerServerSocket::cVdrmanagerServerSocket() :
		cVdrmanagerSocket() {
}

cVdrmanagerServerSocket::~cVdrmanagerServerSocket() {
}

bool cVdrmanagerServerSocket::Create(int port, const char * password, bool forceCheckSvrp) {
	// save password
	this->password = password;
	this->forceCheckSvdrp = forceCheckSvrp;
	// create socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		LOG_ERROR;
		return false;
	}

	// allow it to always reuse the same port:
	int ReUseAddr = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ReUseAddr, sizeof(ReUseAddr));

	// bind to address
	struct sockaddr_in name;
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
		LOG_ERROR;
		Close();
		return false;
	}

	// make it non-blocking:
	if (!MakeDontBlock()) {
		Close();
		return false;
	}

	// listen to the socket:
	if (listen(sock, 100) < 0) {
		LOG_ERROR;
		Close();
		return false;
	}

	return true;
}

cVdrmanagerClientSocket * cVdrmanagerServerSocket::Accept() {
	cVdrmanagerClientSocket * newsocket = NULL;

	// accept the connection
	struct sockaddr_in clientname;
	uint size = sizeof(clientname);
	int newsock = accept(sock, (struct sockaddr *) &clientname, &size);
	if (newsock > 0) {
		// create client socket
		newsocket = new cVdrmanagerClientSocket(password);
		if (!newsocket->Attach(newsock)) {
			delete newsocket;
			return NULL;
		}

		if (!IsPasswordSet() || forceCheckSvdrp == true) {
			bool accepted = SVDRPhosts.Acceptable(clientname.sin_addr.s_addr);
			if (!accepted) {
				newsocket->PutLine(string("NACC Access denied.\n"));
				newsocket->Flush();
				delete newsocket;
				newsocket = NULL;
			}
			dsyslog(
					"[vdrmanager] connect from %s, port %hd - %s", inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port), accepted ? "accepted" : "DENIED");
		}
	} else if (errno != EINTR && errno != EAGAIN
		)
		LOG_ERROR;

	return newsocket;
}

/*
 * cVdrmonClientSocket
 */
cVdrmanagerClientSocket::cVdrmanagerClientSocket(const char * password) {
	readbuf = writebuf = "";
	disconnected = false;
	client = ++clientno;
	this->password = password;
	login = false;
}

cVdrmanagerClientSocket::~cVdrmanagerClientSocket() {
}

bool cVdrmanagerClientSocket::IsLineComplete() {
	// check a for complete line
	string::size_type pos = readbuf.find("\r", 0);
	if (pos == string::npos)
		pos = readbuf.find("\n");
	return pos != string::npos;
}

bool cVdrmanagerSocket::IsPasswordSet(){
	return strcmp(password, "");
}

bool cVdrmanagerClientSocket::GetLine(string& line) {
	// check the line
	string::size_type pos = readbuf.find("\r", 0);
	if (pos == string::npos)
		pos = readbuf.find("\n", 0);
	if (pos == string::npos)
		return false;

	// extract the line ...
	line = readbuf.substr(0, pos);

	// handle \r\n
	if (readbuf[pos] == '\r' && readbuf.length() > pos
			&& readbuf[pos + 1] == '\n')
		pos++;

	// ... and move the remainder
	readbuf = readbuf.substr(pos + 1);

	return true;
}

bool cVdrmanagerClientSocket::Read() {
	if (Disconnected())
		return false;

	int rc;
	bool len = 0;
	char buf[2001];
	while ((rc = read(sock, buf, sizeof(buf) - 1)) > 0) {
		buf[rc] = 0;
		readbuf += buf;
		len += rc;
	}

	if (rc < 0 && errno != EAGAIN)
	{
		LOG_ERROR;
		return false;
	} else if (rc == 0) {
		disconnected = true;
	}

	return len > 0;
}

bool cVdrmanagerClientSocket::Disconnected() {
	return disconnected;
}

void cVdrmanagerClientSocket::Disconnect() {
	disconnected = true;
}

bool cVdrmanagerClientSocket::PutLine(string line) {
	//TODO http://projects.vdr-developer.org/issues/790
	//string line2 = cHelpers::compress_string(line);
	//unsigned long  l =  line.size();
	//unsigned long  l2 = line2.size();
	//if(l2 == 0){
		//l2 = 1;
	//}
	//dsyslog("[vdrmanager] PutLine, line size is %lu, with zlib it would be %lu (factor %lu)", l, l2, l/l2);
	// add line to write buffer
	writebuf += line;

	// data present?
	if (writebuf.length() > 0) {
		// write so many bytes as possible
		int rc = write(sock, writebuf.c_str(), writebuf.length());
		if (rc < 0 && errno != EAGAIN)
		{
			LOG_ERROR;
			return false;
		}

		// move the remainder
		if (rc > 0)
			writebuf = writebuf.substr(rc, writebuf.length() - rc);
	}

	return true;
}

bool cVdrmanagerClientSocket::Flush() {
	string empty = "";
	return PutLine(empty);
}

bool cVdrmanagerClientSocket::Attach(int fd) {
	sock = fd;
	return MakeDontBlock();
}

int cVdrmanagerClientSocket::GetClientId() {
	return client;
}

bool cVdrmanagerClientSocket::WritePending() {
	return writebuf.length() > 0;
}

bool cVdrmanagerClientSocket::IsLoggedIn() {
	return login || !password || !*password;
}

void cVdrmanagerClientSocket::SetLoggedIn() {
	login = true;
}

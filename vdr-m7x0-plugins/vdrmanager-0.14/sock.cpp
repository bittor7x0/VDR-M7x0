/*
 * extendes sockets
 */
#include <unistd.h>
#include <vdr/plugin.h>
#include "sock.h"
#include "helpers.h"
#include "compressor.h"

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

bool cVdrmanagerSocket::IsPasswordSet(){
  return strcmp(password, "");
}


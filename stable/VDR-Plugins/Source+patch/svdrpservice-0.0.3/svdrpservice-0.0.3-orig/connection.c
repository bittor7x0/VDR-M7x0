#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "connection.h"

#if DEBUG == 1
#define DEBUG_PRINTF(args...) fprintf(stderr, args)
#elif DEBUG == 2
#define DEBUG_PRINTF(args...) dsyslog(args)
#else
#define DEBUG_PRINTF(args...)
#endif

#define SVDRPSERVICE_READ_TIMEOUTMS 1500

int cSvdrpConnection::Connect(const char *ServerIp, unsigned short ServerPort) {
	if (!ServerIp) {
		esyslog("svdrpservice: No server IP specified");
		return -1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ServerPort);
	if (!::inet_aton(ServerIp, &server_addr.sin_addr)) {
		esyslog("svdrpservice: Invalid server IP '%s'", ServerIp);
		return -1;
	}

	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		esyslog("svdrpservice: Error creating socket for connection to %s: %m", ServerIp);
		return -1;
	}
	// set nonblocking
	int flags = ::fcntl(sock, F_GETFL, 0);
	if (flags < 0 || ::fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
		esyslog("svdrpservice: Unable to use nonblocking I/O for %s: %m", ServerIp);
		return -1;
	}
	if (::connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		if (errno != EINPROGRESS) {
			esyslog("svdrpservice: connect to %s:%hu failed: %m", ServerIp, ServerPort);
			return -1;
		}

		int result;
		fd_set fds;
		struct timeval tv;
		cTimeMs starttime;
		int timeout = SVDRPSERVICE_READ_TIMEOUTMS;
		do {
			FD_ZERO(&fds);
			FD_SET(sock, &fds);
			tv.tv_usec = (timeout % 1000) * 1000;
			tv.tv_sec = timeout / 1000;
			result = ::select(sock + 1, NULL, &fds, NULL, &tv);
		} while (result == -1 && errno == EINTR &&
				(timeout = SVDRPSERVICE_READ_TIMEOUTMS - starttime.Elapsed()) > 100);

		if (result == 0) {	// timeout
			result = -1;
			errno = ETIMEDOUT;
		}
		else if (result == 1) {	// check socket for errors
			int error;
			socklen_t size = sizeof(error);
			result = ::getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &size);
			if (result == 0 && error != 0) {
				result = -1;
				errno = error;
			}
		}

		if (result != 0) {
			esyslog("svdrpservice: Error connecting to %s:%hu: %m", ServerIp, ServerPort);
			::close(sock);
			return -1;
		}
	}
	return sock;
}

cSvdrpConnection::cSvdrpConnection(const char *ServerIp, unsigned short ServerPort, bool Shared):
		serverPort(ServerPort), refCount(0), shared(Shared) {
	serverIp = ServerIp ? ::strdup(ServerIp) : NULL;
	bufSize = BUFSIZ;
	buffer = MALLOC(char, bufSize);
}

cSvdrpConnection::~cSvdrpConnection(void) {
	Close();
	free(serverIp);
	free(buffer);
}

bool cSvdrpConnection::HasDestination(const char *ServerIp, unsigned short ServerPort) const {
	if (ServerIp == NULL || serverIp == NULL)
		return false;
	return serverPort == ServerPort && !strcmp(serverIp, ServerIp);
}

bool cSvdrpConnection::Open() {
	if (file.IsOpen()) {
		//TODO: make sure the connection is still alive (e.g. send STAT command)
	}
	if (!file.IsOpen()) {
		int fd = Connect(serverIp, serverPort);
		if (fd < 0)
			return false;

		if (!file.Open(fd)) {
			::close(fd);
			return false;
		}
		
		// check for greeting
		if (Receive() != 220) {
			esyslog("svdrpservice: did not receive greeting from %s. Closing...", serverIp);
			Abort();
			return false;
		}

		isyslog("SvdrpService: connected to %s:%hu", serverIp, serverPort);
	}
	return true;
}

void cSvdrpConnection::Close(void) {
	if (Send("QUIT\r\n", false))
		Receive();
	file.Close();
}

void cSvdrpConnection::Abort(void) {
	file.Close();
}

bool cSvdrpConnection::Send(const char *Cmd, bool Reconnect) {
	if (!Cmd)
		return false;

	if (Reconnect && !file.IsOpen())
		Open();
	if (!file.IsOpen()) {
		esyslog("svdrpservice: unable to send command to %s. Socket is closed", serverIp);
		return false;
	}

	DEBUG_PRINTF("SEND %s", Cmd);
	unsigned int len = ::strlen(Cmd);
	if (safe_write(file, Cmd, len) < 0) {
		esyslog("svdrpservice: error while writing to %s: %m", serverIp);
		Abort();
		return false;
	}
	return true;
}

unsigned short cSvdrpConnection::Receive(cList<cLine>* List) {
	while (ReadLine()) {
		char *tail;
		long int code = ::strtol(buffer, &tail, 10);
		if (tail - buffer == 3 &&
				code >= 100 && code <= 999 &&
				(*tail == ' ' || *tail =='-')) {
			if (List)
				List->Add(new cLine(buffer + 4));
			if (*tail == ' ')
				return (unsigned short) code;
		}
		else {
			esyslog("svdrpservice: invalid reply from %s: '%s'", serverIp, buffer);
			Close();
			break;
		}
	}
	if (List)
		List->Clear();
	return 0;
}

bool cSvdrpConnection::ReadLine() {
	if (!file.IsOpen())
		return false;

	unsigned int tail = 0;
	while (cFile::FileReady(file, SVDRPSERVICE_READ_TIMEOUTMS)) {
		unsigned char c;
		int r = safe_read(file, &c, 1);
		if (r > 0) {
			if (c == '\n' || c == 0x00) {
				// strip trailing whitespace:
				while (tail > 0 && strchr(" \t\r\n", buffer[tail - 1]))
					buffer[--tail] = 0;

				// line complete, make sure the string is terminated
				buffer[tail] = 0;
				DEBUG_PRINTF("READ %s\n", buffer);
				return true;
			}
			else if ((c <= 0x1F || c == 0x7F) && c != 0x09) {
				// ignore control characters
				}
			else {
				if (tail >= bufSize - 1) {
					bufSize += BUFSIZ;
					buffer = (char*) realloc(buffer, bufSize);
					if (!buffer) {
						esyslog("svdrpservice: unable to increase buffer size to %d byte", bufSize);
						Close();
						return false;
					}
				}
				buffer[tail++] = c;
				buffer[tail] = 0;
			}
		}
		else {
			esyslog("svdrpservice: lost connection to %s", serverIp);
			buffer[0] = 0;
			Abort();
			return false;
		}
	}
	esyslog("svdrpservice: timeout waiting for reply from %s", serverIp);
	buffer[0] = 0;
	Abort();
	return false;
}

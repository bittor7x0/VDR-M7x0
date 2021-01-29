/*
 * select
 */

#include <sys/select.h>
#include <vdr/plugin.h>
#include <vdr/timers.h>
#include "sock.h"
#include "select.h"
#include "handler.h"
#include "helpers.h"

struct node {
	cVdrmanagerClientSocket * socket;
	node * next;
};

cSelect::cSelect() {
	serversocket = NULL;
	sslServersocket = NULL;
	clientsockets = NULL;
	clientsocketcount = 0;
	handler = new cHandler;
	pollfds = NULL;
	stopped = false;
	nexttimer = 0;
	shutdown = 0;
}

cSelect::~cSelect() {
	if (serversocket)
		delete serversocket;
	if (sslServersocket)
	  delete sslServersocket;

	while (clientsockets) {
		node * next = clientsockets->next;
		delete clientsockets->socket;
		delete clientsockets;
		clientsockets = next;
	}

	if (handler)
		delete handler;

	if (pollfds)
		delete pollfds;
}

void cSelect::SetServerSockets(cVdrmanagerServerSocket * sock, cVdrmanagerServerSocket * sslSock) {
	serversocket = sock;
	sslServersocket = sslSock;
}

void cSelect::AddClientSocket(cVdrmanagerClientSocket * sock) {
	// remember socket
	node * newnode = new node;
	newnode->next = clientsockets;
	newnode->socket = sock;
	clientsockets = newnode;
	clientsocketcount++;
}

void cSelect::RemoveClientSocket(cVdrmanagerClientSocket * sock) {
	node * curnode = clientsockets;
	node * lastnode = NULL;
	while (curnode) {
		if (curnode->socket == sock) {
			// unlink node
			if (lastnode)
				lastnode->next = curnode->next;
			else
				clientsockets = curnode->next;

			// free socket and node
			delete curnode->socket;
			delete curnode;

			clientsocketcount--;
			break;
		}
		lastnode = curnode;
		curnode = curnode->next;
	}

	if (clientsockets) {
		curnode = clientsockets;
		while (curnode) {
			curnode = curnode->next;
		}
	}
}

cVdrmanagerClientSocket * cSelect::GetClientSocket(int sock) {
	node * curnode = clientsockets;
	while (curnode) {
		if (curnode->socket->GetSocket() == sock)
			return curnode->socket;
		curnode = curnode->next;
	}

	return NULL;
}

bool cSelect::Action() {
	for (; !stopped;) {
		if (!Poll())
			return false;
	}

	return true;
}

int cSelect::CreatePollfds() {

  int i = 0;

  int count = clientsocketcount + (sslServersocket ? 2 : 1);

  // we poll for the server socket and for each client socket
	pollfds = new struct pollfd[count];

	pollfds[i].fd = serversocket->GetSocket();
	pollfds[i].revents = 0;
  pollfds[i++].events = POLLIN;

  if (sslServersocket) {
    pollfds[i].fd = sslServersocket->GetSocket();
    pollfds[i].revents = 0;
    pollfds[i++].events = POLLIN;
  }

	node * curnode = clientsockets;
	while (curnode) {
	  cVdrmanagerClientSocket * sock = curnode->socket;
		pollfds[i].fd = sock->GetSocket();
#if VDRMANAGER_USE_SSL
		if (sock->IsSSL()) {
		  if (sock->GetSslWantsSelect() == SSL_ERROR_WANT_READ) {
		    pollfds[i].events = POLLIN | POLLHUP;
		  } else if (sock->GetSslWantsSelect() == SSL_ERROR_WANT_WRITE) {
		    pollfds[i].events = POLLOUT;
		  } else {
		    pollfds[i].events = POLLIN | POLLHUP;
		    if (sock->WritePending())
		      pollfds[i].events |= POLLOUT;
		  }
		} else {
#endif
		  pollfds[i].events = POLLIN | POLLHUP;
		  if (curnode->socket->WritePending())
		    pollfds[i].events |= POLLOUT;
#if VDRMANAGER_USE_SSL
		}
#endif
		pollfds[i++].revents = 0;
		curnode = curnode->next;
	}

	return count;
}

bool cSelect::Poll() {

	// poll for events
	int count = CreatePollfds();

	int rc = 0;
	while ((rc = poll(pollfds, count, -1)) < 0) {
		if (errno != EINTR)
			break;
	}
	if (rc < 0) {
		LOG_ERROR;
		delete pollfds;
		return false;
	}

	// timeout?
	if (rc == 0)
		return true;

	// client requests or outstanding writes
	for (int i = (sslServersocket ? 2 : 1); i < count; i++) {
		cVdrmanagerClientSocket * sock = GetClientSocket(pollfds[i].fd);
		if (sock) {
#if VDRMANAGER_USE_SSL
		  if ((pollfds[i].revents & (POLLIN|POLLOUT)) && sock->GetSslReadWrite() != SSL_NO_RETRY) {
		    if (sock->GetSslReadWrite() == SSL_RETRY_READ) {
		      handler->HandleClientRequest(sock);
		    } else {
		      sock->Flush();
		    }
		  } else
#endif
		  if (pollfds[i].revents & POLLOUT) {
        // possibly outstanding writes
        sock->Flush();
      } else if (pollfds[i].revents & (POLLIN | POLLHUP)) {
				// client request
				handler->HandleClientRequest(sock);
			}

      // disconnect?
      if (sock->Disconnected()) {
        RemoveClientSocket(sock);
			}
		}
	}

	// new client?
	for(int i = 0; i < (sslServersocket ? 2 : 1); i++) {
    if (pollfds[i].revents & POLLIN) {
      // get client socket
      cVdrmanagerClientSocket * sock;
      if (i == 0)
        sock = serversocket->Accept();
      else
        sock = sslServersocket->Accept();

      if (sock) {
        // Add client socket
        AddClientSocket(sock);
        // Send current data
        handler->HandleNewClient(sock);
      }
    }
	}

	delete pollfds;

	return true;
}

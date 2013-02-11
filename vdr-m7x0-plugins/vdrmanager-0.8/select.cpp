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

void cSelect::SetServerSocket(cVdrmanagerServerSocket * sock) {
	serversocket = sock;
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

void cSelect::CreatePollfds() {
	// construct pollfd array
	// we need one pollfd for the eventpipe,
	// the serversocket and each clientsocket
	pollfds = new struct pollfd[clientsocketcount + 1];
	pollfds[0].fd = serversocket->GetSocket();
	pollfds[0].events = POLLIN;

	node * curnode = clientsockets;
	int i = 1;
	while (curnode) {
		pollfds[i].fd = curnode->socket->GetSocket();
		pollfds[i].events = POLLIN;
		if (curnode->socket->WritePending())
			pollfds[i].events |= POLLOUT;
		pollfds[i++].revents = 0;
		curnode = curnode->next;
	}
}

bool cSelect::Poll() {
	// poll for events
	CreatePollfds();
	int rc = 0;
	while ((rc = poll(pollfds, clientsocketcount + 1, -1)) < 0) {
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
	for (int i = 1; i < clientsocketcount + 1; i++) {
		cVdrmanagerClientSocket * sock = GetClientSocket(pollfds[i].fd);
		if (sock) {
			if (pollfds[i].revents & (POLLIN | POLLHUP)) {
				// client request
				handler->HandleClientRequest(sock);

				// disconnect?
				if (sock->Disconnected()) {
					RemoveClientSocket(sock);
				}
			} else if (pollfds[i].revents & POLLOUT) {
				// possibly outstanding writes
				sock->Flush();
			}
		}
	}

	// new client?
	if (pollfds[0].revents & POLLIN) {
		// get client socket
		cVdrmanagerClientSocket * sock = serversocket->Accept();
		if (sock) {
			// Add client socket
			AddClientSocket(sock);
			// Send current data
			handler->HandleNewClient(sock);
		}
	}

	delete pollfds;

	return true;
}

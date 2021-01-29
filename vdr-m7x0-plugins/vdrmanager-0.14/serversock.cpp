/*
 * extendes sockets
 */
#include <unistd.h>
#include <vdr/plugin.h>

#if VDRMANAGER_USE_SSL
#include <openssl/err.h>
#endif

#include "serversock.h"
#include "helpers.h"
#include "compressor.h"

static int clientno = 0;

/*
 * cVdrmonServerSocket
 */
cVdrmanagerServerSocket::cVdrmanagerServerSocket() : cVdrmanagerSocket() {
  port = -1;
  sslCtx = NULL;
}

cVdrmanagerServerSocket::~cVdrmanagerServerSocket() {
#if VDRMANAGER_USE_SSL
  if (sslCtx)
    SSL_CTX_free(sslCtx);
#endif
}

bool cVdrmanagerServerSocket::Create(int port, const char * password, bool forceCheckSvrp, int compressionMode,
                                     const char * certFile, const char * keyFile) {

  this->port = port;
  this->password = password;
	this->forceCheckSvdrp = forceCheckSvrp;
	this->compressionMode = compressionMode;

	// create socket
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		LOG_ERROR;
		return false;
	}

  isyslog("[vdrmanager] created %sSSL server socket on port %d", certFile ? "" : "non ", port);

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

#if VDRMANAGER_USE_SSL

  if (certFile) {
    isyslog("[vdrmanager] initialize SSL");

    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();
    OpenSSL_add_ssl_algorithms();

    SSL_load_error_strings();
    SSL_library_init();

    SSL_METHOD * method = (SSL_METHOD *)SSLv23_server_method();
    sslCtx = SSL_CTX_new(method);
    if (sslCtx == NULL) {
      long errorCode = ERR_get_error();
      char * error = ERR_error_string(errorCode, NULL);
      esyslog("[vdrmanager] Error initializing SSL context: %s", error);
      Close();
      return false;
    }
    SSL_CTX_set_options(sslCtx, SSL_OP_NO_SSLv3);

    /* set the local certificate from CertFile */
   SSL_CTX_use_certificate_file(sslCtx, certFile, SSL_FILETYPE_PEM);
    /* set the private key from KeyFile */
   SSL_CTX_use_PrivateKey_file(sslCtx, keyFile, SSL_FILETYPE_PEM);
    /* verify private key */
   if (!SSL_CTX_check_private_key(sslCtx)) {
     long errorCode = ERR_get_error();
     char * error = ERR_error_string(errorCode, NULL);
     esyslog("[vdrmanager] Error checking SSL keys: %s", error);
     Close();
     return false;
   }

   SSL_CTX_set_mode(sslCtx, SSL_MODE_ENABLE_PARTIAL_WRITE);
  }
#endif

	return true;
}

cVdrmanagerClientSocket * cVdrmanagerServerSocket::Accept() {
        cVdrmanagerClientSocket * newsocket = NULL;

        isyslog("[vdrmanager] new %sclient on port %d", sslCtx ? "SSL " : "", port);

        // accept the connection
        struct sockaddr_in clientname;
        uint size = sizeof(clientname);
        int newsock = accept(sock, (struct sockaddr *) &clientname, &size);
        if (newsock > 0) {
                // create client socket
                newsocket = new cVdrmanagerClientSocket(password, compressionMode);
                if (!newsocket->Attach(newsock, sslCtx)) {
                        delete newsocket;
                        return NULL;
                }

                if (!IsPasswordSet() || forceCheckSvdrp == true) {
                        bool accepted = SVDRPhosts.Acceptable(clientname.sin_addr.s_addr);
                        if (!accepted) {
                                newsocket->Write(string("NACC Access denied.\n"));
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

int cVdrmanagerServerSocket::GetPort() {
  return port;
}

/*
 * extendes sockets
 */
#include <unistd.h>
#include <vdr/plugin.h>

#if VDRMANAGER_USE_SSL
#include <openssl/err.h>
#endif

#include "clientsock.h"
#include "helpers.h"
#include "compressor.h"

static int clientno = 0;

/*
 * cVdrmonClientSocket
 */
cVdrmanagerClientSocket::cVdrmanagerClientSocket(const char * password, int compressionMode) {
	readbuf = "";
	writebuf = "";
	sendbuf = NULL;
	sendsize = 0;
	sendoffset = 0;
	disconnected = false;
	initDisconnect = false;
	client = ++clientno;
	this->password = password;
	this->compressionMode = compressionMode;
	login = false;
	compression = false;
	initCompression = false;
#if VDRMANAGER_USE_SSL
	ssl = NULL;
	sslReadWrite = SSL_NO_RETRY;
	sslWantsSelect = SSL_ERROR_NONE;
#endif
}

cVdrmanagerClientSocket::~cVdrmanagerClientSocket() {
#if VDRMANAGER_USE_SSL
  if (ssl) {
    SSL_free(ssl);
  }
#endif
}

bool cVdrmanagerClientSocket::IsLineComplete() {
	// check a for complete line
	string::size_type pos = readbuf.find("\r", 0);
	if (pos == string::npos)
		pos = readbuf.find("\n");
	return pos != string::npos;
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
	for(;;) {
#if VDRMANAGER_USE_SSL
	  if (ssl)
	    rc = ReadSSL();
	  else
#endif
	    rc = ReadNoSSL();

	  // something read?
	  if (rc <= 0)
	    break;

	  // command string completed?
    if (readbuf.find("\n") != string::npos)
      break;
	}

	// command string completed
	if (rc > 0) {
	  return true;
	}

	// we must retry
	if (rc == 0) {
	  return true;
	}

	// socket closed?
	if (rc == -1) {
    disconnected = true;
    return true;
  }

	// real error
	disconnected = true;
	return false;
}

int cVdrmanagerClientSocket::ReadNoSSL() {

  char buf[2001];
  int rc = read(sock, buf, sizeof(buf) - 1);

  if (rc > 0) {
    // data read
    buf[rc] = 0;
    readbuf += buf;
    return rc;
  }

  // socket closed
  if (rc == 0) {
    return -1;
  }

  if (errno == EAGAIN) {
    return 0;
  }

  return -2;
}

#if VDRMANAGER_USE_SSL

int cVdrmanagerClientSocket::ReadSSL() {

  sslReadWrite = SSL_NO_RETRY;
  sslWantsSelect = SSL_ERROR_NONE;

  bool len = 0;
  char buf[2001];

  ERR_clear_error();
  int rc = SSL_read(ssl, buf, sizeof(buf) - 1);

  if (rc > 0) {
    buf[rc] = 0;
    readbuf += buf;
    return rc;
  }

  int error = SSL_get_error(ssl, rc);
  if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE) {
    // we must retry
    sslReadWrite = SSL_RETRY_READ;
    sslWantsSelect = error;
    return 0;
  }

  if (error == SSL_ERROR_ZERO_RETURN) {
    // socket closed
    return -1;
  }

  // real error
  long errorCode = ERR_get_error();
  char * errorText = ERR_error_string(errorCode, NULL);
  esyslog("[vdrmanager] error reading from SSL (%ld) %s", errorCode, errorText);
  return -2;
}
#endif

bool cVdrmanagerClientSocket::Disconnected() {
	return disconnected;
}

void cVdrmanagerClientSocket::Disconnect() {
	initDisconnect = true;
}

void cVdrmanagerClientSocket::Write(string line) {
  writebuf += line;
}

bool cVdrmanagerClientSocket::Flush() {

  if (Disconnected()) {
    return false;
  }

	// initialize sendbuf if needed
  if (sendbuf == NULL) {
    if (!compression) {
      sendbuf = (char *)malloc(writebuf.length()+1);
      strcpy(sendbuf, writebuf.c_str());
      sendsize = writebuf.length();
#if VDRMANAGER_USE_GZIP || VDRMANAGER_USE_ZLIB
    } else {
      Compress();
#endif
    }
    sendoffset = 0;
    writebuf.clear();
  }

  // write so many bytes as possible
  int rc;
  for(;sendsize > 0;) {

#if VDRMANAGER_USE_SSL
    if (ssl)
      rc = FlushSSL();
    else
#endif
      rc = FlushNoSSL();

    if (rc <= 0) {
      break;
    }
    sendsize -= rc;
    sendoffset += rc;
  }

  if (rc == 0) {
    // nothing written
    return true;
  }

  // error
  if (rc < 0) {
    if (sendbuf != NULL) {
      free(sendbuf);
      sendbuf = NULL;
    }
    disconnected = true;
    return false;
  }

  // all data written?
  if (sendsize > 0) {
    return true;
  }

  if (sendbuf != NULL) {
    free(sendbuf);
    sendbuf = NULL;
  }

  if (initCompression) {
	  isyslog("Compression is activated now");
	  initCompression = false;
	  compression = true;
	}

	if (initDisconnect) {
	  initDisconnect = false;
	  disconnected = true;
	}

	return true;
}

int cVdrmanagerClientSocket::FlushNoSSL() {

  // write so many bytes as possible
  int rc = write(sock, sendbuf + sendoffset, sendsize);
  if (rc >= 0) {
      return rc;
  }

  if (errno == EAGAIN) {
      return 0;
  }

  LOG_ERROR;
  return -1;
}

#if VDRMANAGER_USE_SSL

int cVdrmanagerClientSocket::FlushSSL() {

  sslReadWrite = SSL_NO_RETRY;
  sslWantsSelect = SSL_ERROR_NONE;

  ERR_clear_error();
  int rc = SSL_write(ssl, sendbuf + sendoffset, sendsize);
  if (rc > 0) {
    return rc;
  }

  int error = SSL_get_error(ssl, rc);
  if (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE) {
    // we must retry after the wanted operation is possible
    sslReadWrite = SSL_RETRY_WRITE;
    sslWantsSelect = error;
    return 0;
  }

  if (error == SSL_ERROR_ZERO_RETURN) {
    // the socket was closed
    return -1;
  }

  // real error
  long errorCode = ERR_get_error();
  char * errorText = ERR_error_string(errorCode, NULL);
  esyslog("[vdrmanager] error writing to SSL (%ld) %s", errorCode, errorText);
  return -1;
}

#endif

bool cVdrmanagerClientSocket::Attach(int fd, SSL_CTX * sslCtx) {
	sock = fd;
	if (!MakeDontBlock()) {
	  return false;
	}

#if VDRMANAGER_USE_SSL
	if (sslCtx) {
	  ssl = SSL_new(sslCtx);
    SSL_set_accept_state(ssl);
    BIO *bio = BIO_new_socket(sock, BIO_NOCLOSE);
	  SSL_set_bio(ssl, bio, bio);
	  BIO_set_nbio(bio, 1);
	}
#endif

	return true;
}

int cVdrmanagerClientSocket::GetClientId() {
	return client;
}

bool cVdrmanagerClientSocket::WritePending() {
	return sendsize > 0;
}

bool cVdrmanagerClientSocket::IsLoggedIn() {
	return login || !password || !*password;
}

void cVdrmanagerClientSocket::SetLoggedIn() {
	login = true;
}

void cVdrmanagerClientSocket::ActivateCompression() {

  string mode = "NONE";

  switch (compressionMode) {
#if VDRMANAGER_USE_GZIP
  case COMPRESSION_GZIP:
    mode = "GZIP";
    initCompression = true;
    break;
#endif
#if VDRMANAGER_USE_ZLIB
  case COMPRESSION_ZLIB:
    mode = "ZLIB";
    initCompression = true;
    break;
#endif
  default:
    mode = "NONE";
    break;
  }

  Write("!OK " + mode + "\r\n");
}

#if VDRMANAGER_USE_GZIP || VDRMANAGER_USE_ZLIB

void cVdrmanagerClientSocket::Compress() {
  cCompressor compressor = cCompressor();

  switch (compressionMode) {
#if VDRMANAGER_USE_GZIP
  case COMPRESSION_GZIP:
    compressor.CompressGzip(writebuf);
    break;
#endif
#if VDRMANAGER_USE_ZLIB
  case COMPRESSION_ZLIB:
    compressor.CompressZlib(writebuf);
    break;
#endif
  }

  sendbuf = compressor.GetData();
  sendsize = compressor.getDataSize();

  double ratio = 1.0 * writebuf.length() / sendsize;
  dsyslog("[vdrmanager] Compression stats: raw %ld, compressed %ld, ratio %f:1", writebuf.length(), sendsize, ratio);
}

#endif

#if VDRMANAGER_USE_SSL

int cVdrmanagerClientSocket::GetSslReadWrite() {
  return sslReadWrite;
}

int cVdrmanagerClientSocket::GetSslWantsSelect() {
  return sslWantsSelect;
}

bool cVdrmanagerClientSocket::IsSSL() {
  return ssl != NULL;
}

#endif

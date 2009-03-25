/* This code is used to receive a web page.
 * It is based on the following code that can be found at
 *
 * http://www.japu.org/tmp/http.c
 */

/* HTTP/1.0 client program
 * Copyright (C) 2000, HIRATA Yasuyuki <yasuyu-h@is.aist-nara.ac.jp>
 *                     (Computer Languages Laboratory, NAIST)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR (HIRATA YASUYUKI) ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <regex.h>
#include <unistd.h>
#include <err.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vdr/plugin.h>
#include "http.h"

int httpGet(char *stUrl, char *outfile) {
	int c, last, iRet;
	FILE *outfp = NULL;
	FILE *rfp = NULL;
	struct uri_t uri;

	dsyslog("DEBUG : weatherng-http: outfile: %s\n", outfile);
	iRet=1;

	if (!parse_uri(stUrl, &uri)) {
		esyslog("ERROR : weatherng-http: URI format error.");
		iRet=0;
	}

	if (iRet==1 && !outfile) {
		outfile = rindex(uri.path, '/');
		if (!outfile || !outfile[1]) {
			outfile = DEFAULT_INDEX_FILE;
		} else {
			outfile++;
		}
	}

	if (iRet==1 && (outfp = fopen(outfile, "w")) == NULL) {
		esyslog("ERROR : weatherng-http: Could not open outfile %s.", outfile);
		iRet=0;
	}

	dsyslog("DEBUG : weatherng-http: scheme: [%s]\nuserinfo: [%s]\nhost: [%s]"
		"port: [%s]\npath: [%s]\nquery: [%s]", uri.scheme, uri.userinfo,
			uri.host, uri.port, uri.path, uri.query);

	if (iRet==1 && strcmp(uri.scheme, "http") != 0) {
		esyslog("ERROR : weatherng-http: Scheme %s not supported.", uri.scheme);
		iRet=0;
	}

	if (iRet==1 && (rfp = http_open(&uri)) == NULL) {
		esyslog("ERROR : weatherng-http: Could not open http.");
		iRet=0;
	}

	if (iRet==1) {
		char *buf1=NULL, *buf2=NULL;

		c = last = 0;
		for (;;) {
			c = fgetc(rfp);
			if (c == '\r') {
				continue;
			} else if (c == EOF || (c == '\n' && last == '\n')) {
				break;
			}
			if (buf1==NULL) {//Set the first char in the buf1
				asprintf(&buf1, "%c", c);
			} else {//append the char to buf1
				asprintf(&buf2, "%s%c", buf1, c);
				free(buf1);
				asprintf(&buf1, "%s", buf2);
				free(buf2);
			}

			last = c;
		}

		dsyslog("DEBUG : weatherng-http: %s", buf1);
		free(buf1);

		dsyslog("DEBUG : weatherng-http: Get data");
		while ((c = fgetc(rfp)) != EOF) {
			fputc(c, outfp);
		}
		fflush(outfp);
		fclose(outfp);
		fclose(rfp);
	}

	return iRet;
}

int parse_uri(const char *uri, struct uri_t *urip) {
	regex_t reg;
	regmatch_t match[URI_RE_NMATCH];

	if (!uri || regcomp(&reg, URI_REGEX, REG_EXTENDED | REG_ICASE) != 0
			|| regexec(&reg, uri, URI_RE_NMATCH, match, 0) != 0)
		return 0;

	memset(urip, 0, sizeof(struct uri_t));

	if (-1 < match[URI_RE_SCHEME_NUM].rm_so)
		strnncpy(urip->scheme, uri + match[URI_RE_SCHEME_NUM].rm_so,
				URI_SCHEME_MAXLEN, match[URI_RE_SCHEME_NUM].rm_eo
						- match[URI_RE_SCHEME_NUM].rm_so);
	else
		urip->scheme[0] = 0;

	if (-1 < match[URI_RE_USERINFO_NUM].rm_so)
		strnncpy(urip->userinfo, uri + match[URI_RE_USERINFO_NUM].rm_so,
				URI_USERINFO_MAXLEN, match[URI_RE_USERINFO_NUM].rm_eo
						- match[URI_RE_USERINFO_NUM].rm_so);
	else
		urip->userinfo[0] = 0;

	if (-1 < match[URI_RE_HOST_NUM].rm_so)
		strnncpy(urip->host, uri + match[URI_RE_HOST_NUM].rm_so,
				URI_HOST_MAXLEN, match[URI_RE_HOST_NUM].rm_eo
						- match[URI_RE_HOST_NUM].rm_so);
	else
		urip->host[0] = 0;

	if (-1 < match[URI_RE_PORT_NUM].rm_so)
		strnncpy(urip->port, uri + match[URI_RE_PORT_NUM].rm_so,
				URI_PORT_MAXLEN, match[URI_RE_PORT_NUM].rm_eo
						- match[URI_RE_PORT_NUM].rm_so);
	else
		urip->port[0] = 0;

	if (-1 < match[URI_RE_PATH_NUM].rm_so)
		strnncpy(urip->path, uri + match[URI_RE_PATH_NUM].rm_so,
				URI_PATH_MAXLEN, match[URI_RE_PATH_NUM].rm_eo
						- match[URI_RE_PATH_NUM].rm_so);
	else
		urip->path[0] = 0;

	if (-1 < match[URI_RE_QUERY_NUM].rm_so)
		strnncpy(urip->query, uri + match[URI_RE_QUERY_NUM].rm_so,
				URI_QUERY_MAXLEN, match[URI_RE_QUERY_NUM].rm_eo
						- match[URI_RE_QUERY_NUM].rm_so);
	else
		urip->query[0] = 0;

	return 1;
}

FILE *http_open(const struct uri_t *urip) {
	FILE *sfp, *rfp;
	int sock;
	struct sockaddr_in saddr;
	struct hostent *hp;
	struct servent *sp;

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return NULL;

	memset(&saddr, 0, sizeof saddr);
	//saddr.sin_len = sizeof saddr;
	saddr.sin_family = AF_INET;

	if (inet_aton(urip->host, &saddr.sin_addr) == 0) {
		if ((hp = gethostbyname2(urip->host, AF_INET)) == NULL)
			return NULL;
		saddr.sin_addr = *(struct in_addr *)hp->h_addr_list[0];
	}

	sp = getservbyname("http", "tcp");
	if (*(urip->port))
		saddr.sin_port = atoi(urip->port);
	else
		saddr.sin_port = sp->s_port;

	if (connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
		return NULL;

	if ((sfp = fdopen(sock, "w")) == NULL || (rfp = fdopen(sock, "r")) == NULL)
		return NULL;

	fprintf(sfp, "GET /%s%s%s %s" CRLF,
			urip->path,
			(*(urip->query)? "?": ""),
			urip->query,
			HTTP_VERSION);
	fprintf(sfp
			, "Host: %s" CRLF, urip->host);
			fprintf(sfp
			, "User-Agent: %s/%s" CRLF,
			HTTP_USER_AGENT_NAME,
			HTTP_USER_AGENT_VERSION);
			fprintf(sfp, CRLF);

			fflush(sfp);

			return rfp;
		}

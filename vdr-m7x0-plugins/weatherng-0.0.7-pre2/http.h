
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

#define strnncpy(dst, src, maxlen, len) \
    strncpy((dst), (src), (((maxlen) < (len))? (maxlen): (len)))
#define usage() \
    errx(EXIT_FAILURE, "usage: %s [-O outfile] URI", __progname);

#define CRLF "\r\n"
#define DEFAULT_INDEX_FILE "index.html"

#define HTTP_USER_AGENT_NAME "CreamyMami"
#define HTTP_USER_AGENT_VERSION "0.0"
#define HTTP_FROM "mail@address.example"
#define HTTP_VERSION "HTTP/1.0"

#define URI_REGEX /* subset regex of URI defined in RFC 2396 */ \
    /* scheme */ \
    "^([a-z][a-z0-9+.-]*):" \
    /* userinfo (opt) */ \
    "//(([a-z0-9_.!~*\'();:&=+$,-]*)@)?" \
    /* hostname */ \
    "(((([a-z0-9]|[a-z0-9][a-z0-9-]*[a-z0-9])\\.)*" \
    "([a-z]|[a-z][a-z0-9-]*[a-z]))\\.?|" \
    /* IPv4address */ \
    "([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+))" \
    /* port (opt) */ \
    "(:([0-9]+))?" \
    /* path (opt) */ \
    "/(([a-z0-9_.!~*\'():@&=+$,-]|%[0-9a-f][0-9a-f])*" \
    "(;([a-z0-9_.!~*\'():@&=+$,-]|%[0-9a-f][0-9a-f])*)*" \
    "(/([a-z0-9_.!~*\'():@&=+$,-]|%[0-9a-f][0-9a-f])*" \
    "(;([a-z0-9_.!~*\'():@&=+$,-]|%[0-9a-f][0-9a-f])*)*" ")*" ")" \
    /* query (opt) */ \
    "(\\?([a-z0-9_.!~*\'():@&=+$,;/?:@&=+$,-]|%[0-9a-f][0-9a-f])*)?$"
#define URI_RE_NMATCH 32
#define URI_RE_SCHEME_NUM 1
#define URI_RE_USERINFO_NUM 3
#define URI_RE_HOST_NUM 4
#define URI_RE_HOSTNAME_NUM 5
#define URI_RE_IPV4ADDR_NUM 9
#define URI_RE_PORT_NUM 11
#define URI_RE_PATH_NUM 12
#define URI_RE_QUERY_NUM 20

#define URI_SCHEME_MAXLEN 16
#define URI_USERINFO_MAXLEN 32
#define URI_HOST_MAXLEN 128
#define URI_PORT_MAXLEN 5
#define URI_PATH_MAXLEN 128
#define URI_QUERY_MAXLEN 128
#define URI_MAXLEN (URI_SCHEME_MAXLEN + URI_USERINFO_MAXLEN + \
                    URI_HOST_MAXLEN + URI_PATH_MAXLEN + URI_QUERY_MAXLEN + 7)

struct uri_t {
  char scheme[URI_SCHEME_MAXLEN + 1];
  char userinfo[URI_USERINFO_MAXLEN + 1];
  char host[URI_HOST_MAXLEN + 1];
  char port[URI_PORT_MAXLEN + 1];
  char path[URI_PATH_MAXLEN + 1];
  char query[URI_QUERY_MAXLEN + 1];
};

int parse_uri(const char *uri, struct uri_t *urip);
FILE *http_open(const struct uri_t *urip);
int httpGet(char *stUrl, char *outfile);


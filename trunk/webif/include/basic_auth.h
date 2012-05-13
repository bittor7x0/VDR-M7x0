/**
 * 
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: AUGER Mickael
 * @author : atinar
 * 
 * Authenfication with HTTP basic method
 */

#ifndef WI_BASIC_AUTH_H
#define WI_BASIC_AUTH_H

#include <klone/klone.h>
#include <klone/context.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <regex.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "misc.h"

//maximum length of string concatenation of login and password 
#define MAX_SIZE_OF_LOGIN_PASSWORD 100

/**
 * Test if the user (client) is connected.
 * This function must be call by all pages protected by password.
 * @param _request request (global var) of KLone server
 * @param _response response (global var) of KLone server
 * @param _out standard output (global var) of KLone server
 * @return false if the user is not connected
 */
bool isAuthorized(wcontext_t *wctx);

/**
 * Decode a base64 string like "login:password"
 * Warning : free the returned string after use
 * @param encodedstring string encoded in base64
 * @return decoded string like "login:password"
 */
char *base64decode(const char *encodedstring);

#endif

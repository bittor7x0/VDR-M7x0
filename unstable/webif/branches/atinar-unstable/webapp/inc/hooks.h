/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* Originally written for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __HOOKS_H__
#define __HOOKS_H__

#include <sys/stat.h>
#include "misc.h"

boolean_t extractEmbededFile(const char *src, const char *dst);
boolean_t createParentFolders(const char *path, mode_t mode);

#endif


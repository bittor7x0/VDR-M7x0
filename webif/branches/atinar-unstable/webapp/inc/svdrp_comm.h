/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* (c) Christian Kelinski <k@kille.cx>
* Please checkout the README file!
* 
* Originally written for the open7x0.org VDR-FW project:
* www.open7x0.org
* 
* Modified for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __SVDRP_COMM_H__
#define __SVDRP_COMM_H__

#include <netinet/in.h>
#include <klone/request.h>
#include "conf.h"

char *execSvdrp(hostConf_t *host,const char *cmd);
void closeSvdrp(hostConf_t *host);
void closeSvdrpAll();

#endif

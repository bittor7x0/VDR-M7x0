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
#include "klone/session.h"

extern int svdrp_socket;

//Set server ip and port and return values if provided pointers
void set_server_address(session_t * session, char aserver_ip[16], uint16_t * aserver_port);

//Open the SVDRP connection
int open_svdrp();

//Close the SVDRP connection
int close_svdrp();

//Read data from SVDRP connection
char * read_svdrp();

//Write data to the SVDRP connection
int write_svdrp(char *data);

//Return public server ip
void whatsmyip(char myip[16]);

//TODO Are vdr and webifd running in the same machine?
int isVdrLocal();

#endif

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
#include <klone/session.h>

extern int svdrp_socket;
extern char svdrpServerIp[16];
extern uint16_t svdrpServerPort;

//Set server ip and port and return values if provided pointers
void setSvdrpServerAddress(session_t * session);

//Open the SVDRP connection
int open_svdrp();

//Close the SVDRP connection
void close_svdrp();

//Read data from SVDRP connection
char * read_svdrp();

//Write data to the SVDRP connection
int write_svdrp(char *data);

//TODO Are vdr and webifd running in the same machine?
int isVdrLocal();

#endif

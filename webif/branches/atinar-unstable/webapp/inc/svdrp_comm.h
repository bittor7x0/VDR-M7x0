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

extern int svdrp_socket;

//Öffnet die SVDRP-Verdindung
int open_svdrp();

//Schliesst die SVDRP-Verbindung
int close_svdrp();

//Liest die Daten von der SVDRP-Verbundung
char * read_svdrp();

//Schreibt Daten in den SVDRP-Socket
int write_svdrp(char *data);

//Gibt die eigene IP von eth0 zurürck
void whatsmyip(char myip[16]);

#endif

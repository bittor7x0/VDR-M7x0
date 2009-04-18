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

#ifndef __SVDRP_PARSE_H__
#define __SVDRP_PARSE_H__

#include <netinet/in.h>
#include "recordings.h"
#include "timers.h"

//svdrp_parse stellt Funktionen bereit, um die Zeilen einer Antwort zu parsen
//TODO eliminar y mover funciones a los sitios pertinentes.
void parse_215E(char * line, unsigned int * event_id, long int * start_time, int * duration, int * table_id, int * version);


void parseTimer(const char * line, timerEntry_t * const timer);

void parseChannel(char * line, channelEntry_t * channel);

boolean_t makeTimerStr(char **timerStr, uint flags, int channelNum, time_t start_time, time_t end_time
	, int priority, int lifetime, const char *title);

#endif

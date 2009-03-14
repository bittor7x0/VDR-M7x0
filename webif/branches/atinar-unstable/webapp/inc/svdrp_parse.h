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

//TODO  mover a conf
extern int marginStart;
extern int marginStop;

//svdrp_parse stellt Funktionen bereit, um die Zeilen einer Antwort zu parsen

void parse_215E(char * line, unsigned int * event_id, long int * start_time, int * duration, int * table_id, int * version);

void parseRec(char * line, recEntry_t * const recording);

void parseTimer(const char * line, timerEntry_t * const timer);

void parseChannel(char * line, channelEntry_t * channel);

boolean_t makeTimerStr(char **timerStr, int active, int channelNum, time_t start_time, time_t end_time
	, int priority, int lifetime, const char *title);

//TODO mover a conf
//Liest die VDR-FW Konfigurationsdatei (falls status auf einer M740AV läuft
void get_config_info();

///////////////////////////////////////////////////////////////////////////////

#ifdef FOR_BUSYBOX

 #include "scan_config.h"

  //***** WICHTIG: Die Konstanten sind Indizes der folgenden Tabelle!

 enum {
	CONFIG_RELEVANT_VDR_CONFIG,
	CONFIG_TIMEZONE,
 };

 extern t_scan_config config_param[];

#endif

#endif

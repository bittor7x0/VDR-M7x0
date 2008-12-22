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

#ifndef __RECORDING_H__
#define __RECORDING_H__

#include "klone/session.h"

typedef struct recFragment {
  char * path;
  uint64_t size;
} recFragment;

void freeRF(recFragment * fragment, int fragnum);

int whichFragment(recFragment * fragment, int fragnum, uint64_t seek, uint64_t *position);
	
recFragment * parseRecData(const char *recdata, int *fragnum, uint64_t *total_size);

const char * getRecData(session_t *session, int id);

#endif

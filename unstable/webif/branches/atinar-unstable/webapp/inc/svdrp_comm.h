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

enum svdrpCode_e {
	SVDRP_HELP=214, //Help message
	SVDRP_RCRD=215, // EPG or recording data record
	SVDRP_IMG_DATA=216, //Image grab data (base 64)
	SVDRP_VDR_READY=220, //VDR service ready
	SVDRP_CLOSING=221, //VDR service closing transmission channel
	SVDRP_CMD_OK=250, //Requested VDR action okay, completed
	SVDRP_EPG_STRT=354, //Start sending EPG data
	SVDRP_CMD_ABRT=451, // Requested action aborted: local error in processing
	SVDRP_CMD_SYNX=500, // Syntax error, command unrecognized
	SVDRP_ARG_SYNX=501, // Syntax error in parameters or arguments
	SVDRP_CMD_UNKW=502, // Command not implemented
	SVDRP_ARG_UNKW=504, // Command parameter not implemented
	SVDRP_CMD_IGNR=550, // Requested action not taken
	SVDRP_TRANS_ERR=554, // Transaction failed
	SVDRP_PLUG_DEFAULT=900, // Default plugin reply code
};

char *execSvdrp(hostConf_t *host,const char *cmd);
void closeSvdrp(hostConf_t *host);
void closeSvdrpAll();

#endif

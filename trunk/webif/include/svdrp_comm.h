/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_SVDRP_COMM_H
#define WI_SVDRP_COMM_H

#include "conf.h"

enum svdrpCode_e {
	SVDRP_HELP = 214, //Help message
	SVDRP_RCRD = 215, // EPG or recording data record
	SVDRP_IMG_DATA = 216, //Image grab data (base 64)
	SVDRP_VDR_READY = 220, //VDR service ready
	SVDRP_CLOSING = 221, //VDR service closing transmission channel
	SVDRP_CMD_OK = 250, //Requested VDR action okay, completed
	SVDRP_EPG_STRT = 354, //Start sending EPG data
	SVDRP_CMD_ABRT = 451, // Requested action aborted: local error in processing
	SVDRP_CMD_SYNX = 500, // Syntax error, command unrecognized
	SVDRP_ARG_SYNX = 501, // Syntax error in parameters or arguments
	SVDRP_CMD_UNKW = 502, // Command not implemented
	SVDRP_ARG_UNKW = 504, // Command parameter not implemented
	SVDRP_CMD_IGNR = 550, // Requested action not taken
	SVDRP_TRANS_ERR = 554, // Transaction failed
	SVDRP_PLUG_DEFAULT = 900, // Default plugin reply code
};

int initSvdrpParser();
void freeSvdrpParser();
char *execSvdrp(hostConf_t *host, const char *fmt, ...) __attribute__ ((format (printf, 2, 3))) ;
void closeSvdrp(hostConf_t *host);
void closeSvdrpAll();

#endif

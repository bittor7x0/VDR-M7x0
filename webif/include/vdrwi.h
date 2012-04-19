/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef VDRWI_H
#define	VDRWI_H

#include "misc.h"

#ifdef	__cplusplus
extern "C" {
#endif

	void serveWiPage(request_t *request, response_t *response, session_t *session, io_t* in, io_t *out);


#ifdef	__cplusplus
}
#endif

#endif	/* VDRWI_H */


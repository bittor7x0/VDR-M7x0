/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <errno.h>
#include <klone/hook.h>
#include <klone/io.h>
#include <klone/utils.h>
#include "hooks.h"
#include "misc.h"
#include "svdrp_comm.h"

int onServerInit(void) {
	dbg("onServerInit");
	initWebifConf();
#ifdef TESTI18NORDER
    i18nsort();
#endif
	crit_goto_if(initSvdrpParser() != 0, err);
	return 0;
err:
	return ~0;
}

int onServerTerm(void) {
	dbg("onServerTerm");
	freeWebifConf();
	freeSvdrpParser();
	return 0;
err:
	return ~0;
}

int onChildInit(void) {
	dbg("onChildInit");
	initWebifConf();
#ifdef TESTI18NORDER
    i18nsort();
#endif
	crit_goto_if(initSvdrpParser() != 0, err);
	return 0;
err:
	return ~0;
}

int onChildTerm(void) {
	dbg("onChildTerm");
	freeWebifConf();
	freeSvdrpParser();
	return 0;
err:
	return ~0;
}

void hooks_setup(void) {
	hook_server_init(onServerInit);
	hook_server_term(onServerTerm);
	hook_child_init(onChildInit);
	hook_child_term(onChildTerm);
}

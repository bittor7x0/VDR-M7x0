/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_COMMANDS_H
#define	WI_COMMANDS_H

#include "misc.h"

#ifdef	__cplusplus
extern "C" {
#endif
	
extern char* commandsFile;

void printCommandsMenuLI(wcontext_t *wctx);
void printCommands(wcontext_t *wctx);
void executeCommand(wcontext_t *wctx, const char *argCommandTitle, const int argCommandNum);
void printCommandsPage(wcontext_t *wctx);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMANDS_H */


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

#include <errno.h>
#include <klone/hook.h>
#include <klone/io.h>
#include <klone/utils.h>
#include <stdio.h>
#include <string.h>
#include "hooks.h"
#include "misc.h"

/**
 * Acciones a realizar al iniciar el servidor.
 */
int onServerInit(void){
	initWebifConf();
	return 0;
err:
	return ~0;
}

/**
 * Acciones a realizar al terminar el servidor.
 */
int onServerTerm(void){
	freeWebifConf();
	return 0;
err:
	return ~0;
}

/**
 * Acciones al crear un proceso hijo.
 */
int onChildInit(void){
	initWebifConf();
	return 0;
err:
	return ~0;
}

/**
 * Acciones al terminar un proceso hijo.
 */
int onChildTerm(void){
	freeWebifConf();
	return 0;
err:
	return ~0;
}

/**
* Registrar hooks
*/
void hooks_setup(void){
	//hook_server_init( onServerInit );
	//hook_server_term( onServerTerm );
	hook_child_init( onChildInit );
	hook_child_term( onChildTerm );
}

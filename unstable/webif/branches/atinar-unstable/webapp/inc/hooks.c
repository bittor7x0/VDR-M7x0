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
#include <string.h>
#include "hooks.h"
#include "misc.h"

/**
 * Acciones a realizar al iniciar el servidor.
 */
int onServerInit(void){
	info("Iniciando servidor");
	const char *logos_tgz_src="/www/images/logos.tgz";
	const char *logos_tgz_dst="/etc/webif/www/images/logos.tgz";
	const char *logos_dir="/etc/webif/www/images/logos";
	if (!dirExists(logos_dir)){
		if (fileExists(logos_tgz_dst) || extractEmbededFile(logos_tgz_src,logos_tgz_dst,false)){
			info("Descomprimiendo archivo de logos",logos_tgz_dst);
			extractLogosFromFile(NULL,logos_tgz_dst);
		}
	}
	return 0;
err:
	return ~0;
}


/**
 * Acciones al crear un proceso hijo.
 */
int onChildInit(void){
	//TODO evitar que se ejecute en los forks para ejecutar orden externa.
	info("Iniciando proceso hijo");
	resetWebifConf();
	readWebifConf();
}

/**
 * Acciones al terminar un proceso hijo.
 */
int onChildTerm(void){
	info("Terminando proceso hijo");
	freeWebifConf();
}

/**
* Registrar hooks
*/
void hooks_setup(void){
	hook_server_init( onServerInit );
	hook_child_init( onChildInit );
	hook_child_term( onChildTerm );
}

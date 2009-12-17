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
#include <fcntl.h>
#include <klone/emb.h>
#include <klone/hook.h>
#include <klone/io.h>
#include <klone/utils.h>
#include <string.h>
#include <sys/types.h>
#include "hooks.h"

/**
 * Acciones a realizar al iniciar el servidor.
 */
int on_server_init(void){
	info("Iniciando servidor");
	const char *logos_tgz_src="/www/images/logos.tgz";
	const char *logos_tgz_dst="/etc/webif/www/images/logos.tgz";
	const char *logos_dir="/etc/webif/www/images/logos";
	if (extractEmbededFile(logos_tgz_src,logos_tgz_dst)){
		//descomprimir logos.tgz
		info("%s ya existe",logos_tgz_dst);
		char *tar_args[] = {"tar", "-C", "/etc/webif/www/images", "-xzf", (char*)logos_tgz_dst, "--no-same-owner", (char*)NULL};
		pid_t pid=0;
		int status;
		pid=fork();
		if (pid==0) { //proceso hijo
			execvp("/bin/tar", tar_args);
		}
		if (pid>0) { //proceso padre
			int status;
			pid_t child_pid=waitpid(pid,&status,0);
			if (WIFEXITED(status)){ 
				info("%s descomprimido correctamente. Estado %d",logos_tgz_dst,WEXITSTATUS(status));
			} else {
				warn("error descomprimiendo %s",logos_tgz_dst);
			}
		}
	}
	return 0;
err:
	return ~0;
}

/**
 * Acciones al crear un proceso hijo.
 */
int on_child_init(void){
	info("Iniciando proceso hijo");
	resetWebifConf();
	readWebifConf();
}

/**
 * Acciones al terminar un proceso hijo.
 */
int on_child_term(void){
	info("Terminando proceso hijo");
	freeWebifConf();
}

/**
* Registrar hooks
*/
void hooks_setup(void){
	hook_server_init( on_server_init );
	hook_child_init( on_child_init );
	hook_child_term( on_child_term );
}

/**
* Extraer archivo embebido
*/
boolean_t extractEmbededFile(const char *src, const char *dst){
	boolean_t result=BT_FALSE;
	io_t *io=NULL;
	if (createParentFolders(dst,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)){
		struct stat st;
		int dst_stat=stat(dst, &st);
		if (dst_stat == 0 && S_ISREG(st.st_mode)){
			info("%s ya existe. Se conserva el anterior.",dst);
			result=BT_TRUE;
		} else if (dst_stat == -1 && (errno == ENOENT)){
			//Extraer archivo embebido
			//TODO verificar que no exista y sea otra cosa
			embres_t *er;
			emb_lookup(src, &er);
			if (er->type!=ET_FILE) {
				warn("%s no es un archivo embebido",src);
			} else {
				embfile_t *ef=(embfile_t *)er;
				dbg_err_if(u_file_open(dst, O_CREAT | O_TRUNC | O_WRONLY, &io));
				dbg_err_if(!io_write(io, (const char*)ef->data, ef->size));
				//dbg_err_if(io_codecs_remove(io)); 
				io_free(io);
				dst_stat=stat(dst, &st);
				if (dst_stat == 0 && S_ISREG(st.st_mode)){
					info("%s se ha extraido correctamente",dst);
					result=BT_TRUE;
				} else {
					warn("Error de extraccion. stat produce un error inesperado %s",strerror(errno));
				}
			}
		} else {
			warn("stat produce un error inesperado %s",strerror(errno));
		}
	}
	return result;
err:
	if (io){
		io_free(io);
	}
	return BT_FALSE;
}

boolean_t createParentFolders(const char *filename, mode_t mode){
	boolean_t result=BT_TRUE;
	if (filename && filename[0]=='/'){
		char *last_slash=strrchr(filename,'/');
		if (last_slash!=filename){
			struct stat st;
			int path_stat;
			char *path=strndup(filename,last_slash-filename);
			char *next_slash=path;
			for (;;) {
				next_slash=strchr(next_slash+1,'/');
				if (next_slash) *next_slash=0;
				path_stat=stat(path,&st);
				if (path_stat==0){
					if (!S_ISDIR(st.st_mode)){
						result=BT_FALSE;
						warn("%s no es directorio",path);
						break;
					}
				} else {
					if( (errno!=ENOENT) || mkdir(path,mode)!=0){
						result=BT_FALSE;
						warn("error creando ruta %s: %s",path,strerror(errno));
						break;
					}
				}
				if (!next_slash) break;
				*next_slash='/';
			};
			free(path);
		}
	} else {
		warn("%s no es una ruta absoluta",filename);
		result=BT_FALSE;
	}
	return result;
}
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
* Rewritten for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#include <klone/utils.h>
#include <regex.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "conf.h"
#include "i18n.h"
#include "misc.h"
#include "svdrp_comm.h"

#define VIDEO0 "/var/vdr/video0"
char* commandsFile="/etc/webif/commands.conf";

void validateCheckbox(const cfgParamConfig_t * const cfg, cfgParam_t * const param){
	const char *option=(cfg->options)?cfg->options:"false|true";
	if (!strchr(option,'|')){
		crit("validateCheckbox: wrong config, only one option");
		exit(1);
	}
	param->isValid=false;
	int l=strcspn(option,"|");
	if (param->value==NULL || strncmp(option,param->value,l)==0){
		//value==NULL => first option (unchecked checkboxes aren't submitted)
		param->value=strndup(option,l);
		param->isValid=true;
	} else {
		option+=l+1;
		l=strcspn(option,"|");
		if (option[l]=='|') warn("More than two options for a checkboxCfg");
		param->isValid=boolean(param->value!=NULL && strncmp(option,param->value,l)==0);
	}
}

bool printInputText(wcontext_t *wctx,const cfgParamConfig_t * const cfg
, const char *id, const char *name, const char *cssClass, int idx,const char *value ){
	printInput(wctx,"text",id,name,cssClass,idx,value,(cfg)?cfg->size:0);
	return true;
}

bool printInputPassword(wcontext_t *wctx,const cfgParamConfig_t * const cfg
,const char *id, const char *name, const char *cssClass, int idx, const char *value){
	printInput(wctx,"password",id,name,cssClass,idx,value,(cfg)?cfg->size:0);
	return true;
}

bool printCheckbox(wcontext_t *wctx,const cfgParamConfig_t * const cfg
, const char *id, const char *name, const char *cssClass, int idx,const char *value){
	if (cfg->options) {
		//value==first option => false
		//value==second option =>true
		bool booleanValue=false;
		bool booleanValueSet=false;
		const char *option=cfg->options;
		int l=strcspn(option,"|");
		if (strncmp(option,value,l)==0){
			booleanValueSet=true;
		}
		if (option[l]!='|'){
			warn("Only one option for a checkboxCfg");
			option=NULL;
		} else {
			option+=l+1;
			l=strcspn(option,"|");
			if (strncmp(option,value,l)==0){
				booleanValue=true;
				booleanValueSet=true;
			}
		}
		if (!booleanValueSet) {
			warn("No option match for checkbox value");
		}
		wctx_printf0(wctx,"<input type=\"checkbox\"");
		if (id) wctx_printf(wctx," id=\"%s\" ",id);
		if (!name) name="paramValue";
		if (idx>=0){
			wctx_printf(wctx," name=\"%s_%d\" ",name,idx);
		} else {
			wctx_printf(wctx," name=\"%s\" ",name);
		}
		if (cssClass) wctx_printf(wctx," class=\"%s\" ",cssClass);
		wctx_printf(wctx," value=\"%.*s\"%s/>\n",(option)?l:4,(option)?option:"true",checked[booleanValue]);
		return true;
	}
	return false;
}

bool printSelect(wcontext_t *wctx,const cfgParamConfig_t * const cfg
	,const char *id, const char *name, const char *cssClass, int idx, const char *value
){
	if (cfg && cfg->options && strlen(cfg->options)>0) {
		wctx_printfn(wctx,"<select ",0,1);
		if (id) wctx_printf(wctx," id=\"%s\" ",id);
		if (!name) name="paramValue";
		if (idx>=0){
			wctx_printf(wctx," name=\"%s_%d\" ",name,idx);
		} else {
			wctx_printf(wctx," name=\"%s\" ",name);
		}
		if (cssClass) wctx_printf(wctx," class=\"%s\" ",cssClass);
		wctx_printf(wctx," size=\"1\">\n");
		const char * option=cfg->options;
		int o=cfg->indexOffset;
		for(;;) {
			int l=strcspn(option,"|");
			char *optionnt=strndup(option,l);
			const char *optiontre=CTX_HTML_ENCODE(tr(optionnt),-1);
			if (cfg->isIndex) {
				wctx_printf0(wctx,"<option value=\"%d\"%s>%s</option>\n",o,selected[sameIntEx(value,o)],optiontre);
			} else {
				wctx_printf0(wctx,"<option value=\"%s\"%s>%s</option>\n",optionnt,selected[sameString(value,optionnt)],optiontre);
			}
			free(optionnt);
			option+=l;
			if (option[0]!='|') break;
			option++;
			o++;
		}
		wctx_printfn(wctx,"</select>\n",-1,0);
		return true;
	}
	return false;
}

bool validateHostsStr(const char *hosts);

void validateHostsField(const cfgParamConfig_t * const cfg, cfgParam_t * const param){
	param->isValid=validateHostsStr(param->value);
}

webifConf_t webifConf;

cfgParamConfig_t webifParamConfig[]={
	{"deletion_disabled","false","false|true",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"default language","-1","langBrowserDefined|langEnglish|langGerman|langSpanish|langFrench",true,-1,NULL,(cfgParamPrintInput_t)printSelect,0,false},
	{"default_margin_start","15",NULL,false,0,NULL,NULL,3,false},
	{"default_margin_stop","20",NULL,false,0,NULL,NULL,3,false},
	{"display_host_id","true","false|true",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"config_change_disabled","false","false|true",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"config_view_disabled","false","false|true",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"hosts",",127.0.0.1,2001,/var/vdr/video0;",NULL,false,0,(cfgParamValidate_t)validateHostsField,NULL,50,false},
	{"max_depth","2",NULL,false,0,NULL,NULL,2,false},
	{"playlist_type","0","M3U|XSPF",true,0,NULL,printSelect,0,false},
	{"print_rec_folder_summary","false","false|true",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"use_external_www_folder","false","false|true",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"video_width","640",NULL,false,0,NULL,NULL,4,false},
	{"video_height","480",NULL,false,0,NULL,NULL,4,false},
	{"epg_grid_hours","4",NULL,false,0,NULL,NULL,5,false},
	{"channel_logo_width","54",NULL,false,0,NULL,NULL,3,false},
	{"user",NULL,NULL,false,0,NULL,NULL,10,false},
	{"password",NULL,NULL,false,0,NULL,(cfgParamPrintInput_t)printInputPassword,10,false},
	{"eventSearchUrl","http://www.google.com/search?as_q={title}+{subtitle}",NULL,false,0,NULL,NULL,50,false},
};

cfgParamConfig_t rcParamConfig[]={
	{"ip","192.168.100.102",NULL,false,0,NULL,NULL,15,false},
	{"net","inet","inet|DHCP",false,0,NULL,printSelect,0,false},
	{"netdate","NO","NO|YES",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"netmask","255.255.255.0",NULL,false,0,NULL,NULL,15,false},
};

cfgParamConfig_t vdrParamConfig[] = {
	{"AbortWhenPluginFails","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"CutterAutoDelete","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"DelTimeshiftRec","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"EmergencyExit","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"HardLinkCutter","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"HotStandby","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"MarkInstantRecord","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"MenuButtonCloses","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"MenuScrollPage","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"MenuScrollWrap","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"PauseLifetime","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"SetSystemTime","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShowInfoOnChSwitch","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShowProgressBar","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShowRecDate","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShowRecLength","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShowRecTime","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShowReplayMode","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"ShutdownMessage","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"SplitEditedFiles","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"UseDolbyDigital","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"UseDolbyInRecordings","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"UseSubtitle","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"UseVps","0","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
	{"WarEagleIcons","1","0|1",false,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,0,false},
};

#ifdef ENABLE_BOXAMP_CONF
cfgParamConfig_t boxampParamConfig[] = {
	{"boxamp_bin","boxamp",NULL,false,0,NULL,NULL,10,false},
	{"boxamp_mp3dir","/var/media/mp3",NULL,false,0,NULL,NULL,30,false},
	{"boxamp_opts","-f ${boxamp_path}",NULL,false,0,NULL,NULL,30,false},
	{"boxamp_path","/var/media/pc2/boxamp",NULL,false,0,NULL,NULL,30,false},
	{"boxamp_startup_time","300",NULL,false,0,NULL,NULL,4,false},
	{"runboxamp","/var/media/pc2/boxamp/runboxamp",NULL,false,0,NULL,NULL,50,false},
};
#endif

cfgParamConfigList_t cfgParamConfig[] = { //Indexed by cfgFileId_t
	{true,sizeof(webifParamConfig)/sizeof(cfgParamConfig_t),webifParamConfig},
	{false,sizeof(rcParamConfig)/sizeof(cfgParamConfig_t),rcParamConfig},
	{false,sizeof(vdrParamConfig)/sizeof(cfgParamConfig_t),vdrParamConfig},
#ifdef ENABLE_BOXAMP_CONF
	{false,sizeof(boxampParamConfig)/sizeof(cfgParamConfig_t),boxampParamConfig}
#endif
};

const cfgFile_t fileMapping[] = {
	{"cfgWi", "/etc/webif/webif.conf"},
	{"cfgRc", "/etc/rc.conf"},
	{"vdrSetup", "/etc/vdr/setup.conf"},
#ifdef ENABLE_BOXAMP_CONF
	{"cfgBa", "/etc/bootmenu/bm.local.boxamp.conf"},
#endif
	{"sysLog", "/var/log/messages"},
	{"fstab", "/etc/fstab"},
	{"rc.local", "/etc/rc.local.conf"},
	{"bootMenu" "/etc/bootmenu/bootmenu.conf"},
	{"svdrpHosts", "/etc/vdr/svdrphosts.conf"},
	{"channels", "/etc/vdr/channels.conf"},
	{"streamdevHosts", "/etc/vdr/plugins/streamdevhosts.conf"},
};
const int fileMappingLength=sizeof(fileMapping)/sizeof(cfgFile_t);

//TODO usar directamente fileMapping
const cfgFile_t cfgFile[] = {//Indexed by cfgFileId_t
	{"cfgWi", "/etc/webif/webif.conf"},
	{"cfgRc", "/etc/rc.conf"},
	{"vdrSetup", "/etc/vdr/setup.conf"},
#ifdef ENABLE_BOXAMP_CONF
	{"cfgBa", "/etc/bootmenu/bm.local.boxamp.conf"},
#endif
};
const int cfgFileLength=sizeof(cfgFile)/sizeof(cfgFile_t);

void initCfgParam(cfgParam_t * const param){
	param->name=NULL;
	param->value=NULL;
	param->oldValue=NULL;
	param->comment=NULL;
	param->config=NULL;
	param->written=false;
	param->isValid=true;
}

void freeCfgParam(cfgParam_t * const param){
	free(param->name);
	free(param->value);
	free(param->oldValue);
	free(param->comment);
	initCfgParam(param);
}

void initCfgParamList(cfgParamList_t * const params){
	params->fileId=CF_WEBIFCONF;
	params->hostId=0;
	params->length=0;
	params->entry=NULL;
}

void freeCfgParamList(cfgParamList_t * const params){
	int i;
	for (i=0;i<params->length;i++) freeCfgParam(params->entry+i);
	free(params->entry);
	initCfgParamList(params);
}

const cfgParamConfig_t *getCfgParamConfig(cfgFileId_t fileId, cfgParam_t * const param){
	if (param->config) {
		return param->config;
	}
	int i;
	const cfgParamConfig_t *config;
	for (i=0,config=cfgParamConfig[fileId].entry;i<cfgParamConfig[fileId].length;i++,config++){
		if (strcmp(config->name,param->name)==0){
			param->config=config;
			return config;
		}
	}
	return NULL;
}

void parseParam(char *s, cfgParamList_t * const params, cfgParamConfigList_t * const configs){
	cfgParam_t * param;
	cfgParamConfig_t * config;
	char *e,*n;
	int i;
	s+=strspn(s," ");
	e=s+strcspn(s,"\n\r");
	e[0]=0;
	if (s[0] && s[0]!='#') {
		n=strchr(s,'=');
		if (n && n>s){
			for(e=n-1; e>=s && e[0]==' ';e--) ;
			if (e>=s){ 
				int l=e-s+1;
				config=NULL;
				for (i=0;i<configs->length;i++){
					if (strncmp(s,configs->entry[i].name,l)==0){
						config=configs->entry+i;
						break;
					}
				}
				if (!configs->exclusive || config){
					crit_goto_if((params->entry=realloc(params->entry, (++params->length)*sizeof(cfgParam_t)))==NULL,outOfMemory);
					param=params->entry+params->length-1;
					initCfgParam(param);
					param->name=strndup(s,e-s+1);
					s=n+strspn(n,"= ");
					if (s[0]){
						n=strchrnul(s,'#');
						while (n[0]=='#' && n[-1]!=' '){
							n=strchrnul(n+1,'#');
						};
						for(e=n-1; e>=s && e[0]==' ';e--) ;
						if (e>=s){
							param->value=strndup(s,e-s+1);
							param->oldValue=strdup(param->value);
						}
						if (n[0]=='#'){
							s=n+strspn(n,"# ");
							for(e=s+strlen(s)-1; e>=s && e[0]==' ';e--) ;
							param->comment=(e>=s)?strndup(s,e-s+1):NULL;
						}
					}
					if (config){
						config->alreadySet=true;
						param->config=config;
					}
				}
			}
		}
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool readConf(cfgParamList_t * const params) {
	if (params->fileId<CF_MIN || params->fileId>CF_MAX) return false;
	int i;
	char * s;
	enum {BUFSZ=256};
	char buffer[BUFSZ]; //TODO longitud
	cfgParamConfig_t * config;
	cfgParamConfigList_t * const configs=&cfgParamConfig[params->fileId];
	cfgParam_t *param;
	for (i=0,config=configs->entry;i<configs->length;i++,config++){
		config->alreadySet=false;
	}
	if (params->fileId == CF_VDRCONF ){
		hostConf_t * host=getHost(params->hostId);
		if (host==NULL || ! host->isVdr){
			warn("Host %d no existe o no ejecuta VDR",params->hostId);
			return false;
		}
		char *data=execSvdrp(host,"PARG");
		if (data){
			for (s=strtok(data,"\r\n");s!=0;s=strtok(0,"\r\n")) {
				if (atoi(s)==SVDRP_CMD_OK) {
					parseParam(s+4,params,configs);
				}
			}
			free(data);
		}
	}
	else {
		FILE *f = fopen(cfgFile[params->fileId].fileName,"r");
		if (f) {
			while (!feof(f) && (s=fgets(buffer,BUFSZ,f))!=NULL) {
				parseParam(s,params, configs);
			}
			fclose(f);
		}
	}
	for (i=0,config=configs->entry;i<configs->length;i++,config++){
		if (config->alreadySet) continue;
		crit_goto_if((params->entry=realloc(params->entry, (++params->length)*sizeof(cfgParam_t)))==NULL,outOfMemory);
		param=params->entry+params->length-1;
		initCfgParam(param);
		param->name=strdup(config->name);
		param->value=(config->defaultValue)?strdup(config->defaultValue):NULL;
		param->oldValue=NULL;
		param->config=config;
		config->alreadySet=true;
	}
	return boolean(params->length>0);
outOfMemory:
	crit("Out of memory");
	exit(1);
}

FILE * tmpCfgFile(char tmpName[16]){
	strcpy(tmpName,"/etc/cfgXXXXXX"); //TODO tiene que estar en etc para que rename no genere error
	errno=0;
	int tfd=mkstemp(tmpName);
	if (errno){
		warn(strerror(errno));
		return NULL;
	}
	FILE *t = fdopen(tfd,"w");
	if (errno){
		warn(strerror(errno));
		unlink(tmpName);
		close(tfd);
		return NULL;
	}
	return t;
}

bool writeConfFile(cfgParamList_t * const params) {
	bool result=false;
	bool isNew=false;
	bool exclusive=cfgParamConfig[params->fileId].exclusive;
	const char *fileName=cfgFile[params->fileId].fileName;
	errno=0;
	FILE *f = fopen(fileName,"r");
	if (errno){
		if (errno==ENOENT){
			isNew=true;
			errno=0;
		} else {
			warn(strerror(errno));
			return false;
		}
	}
	char tmpName[16];
	FILE *t=tmpCfgFile(tmpName);
	if (t==NULL) {
		if (!isNew) fclose(f);
		return false;
	}
	int i;
	char *name;
	char *value;
	char *comment;
	cfgParam_t *param;
	for (i=0;i<params->length;i++) params->entry[i].written=false;
	if (!isNew) {
		enum {BUFSZ=256};
		char buffer[BUFSZ];
		char *s;
		int l,ln,lv,lc;
		while (!feof(f) && (s=fgets(buffer,BUFSZ,f))!=NULL) {
			name=NULL;
			value=NULL;
			comment=NULL;
			s+=strspn(s," ");
			if (!s[0] || s[0]=='#'){ //write comments and empty lines unmodified
				fputs(buffer,t);
				if (errno){
					warn(strerror(errno));
					errno=0;
				}
			} else {
				name=s;
				l=strcspn(s,"=\n\r");
				if (s[l]=='='){
					ln=l;
					while (ln>0 && s[ln-1]==' ') ln--;
					if (ln>0){
						s+=l+1;
						s+=strspn(s," ");
						value=s;
						lv=l=strcspn(s,"#\n\r");
						if (s[l]=='#') comment=s+l+1;
						while (lv>0 && s[lv-1]==' ') lv--;
						name[ln]=0;
						value[lv]=0;
						param=NULL;
						for (i=0;i<params->length;i++){
							if (strcmp(params->entry[i].name,name)==0){
								param=params->entry+i;
								break;
							}
						}
						if (exclusive && !param) {
							dbg("param [%s] eliminado de conf [%s]", name, fileName);
						} else {
							if (param){
								value=param->value;
							}
							if (comment) {
								while (comment[0]==' ') comment++;
								lc=strcspn(comment,"\n\r");
								while (lc>0 && comment[lc]==' ') lc--;
								fprintf(t,"%s=%s #%s\n",name,(value)?value:"",comment);
							} else {
								fprintf(t,"%s=%s\n",name,(value)?value:"");
							}
						}
						if (errno){
							warn(strerror(errno));
							errno=0;
						}
						if (param) param->written=true;
					}
				}
			}
		}
		fclose(f);
	}
	for(i=0,param=params->entry;i<params->length;i++,param++) {
		if (!param->written) {
			fprintf(t,"%s=%s\n",param->name,(param->value)?param->value:"");
			param->written=true;
		}
	}
	fclose(t);
	char *backupName;
	asprintf(&backupName,"%s.back",fileName);
	if (backupName) {
		remove(backupName);
		if (errno){
			if(errno==ENOENT){
				errno=0;
			} else {
				warn(strerror(errno));
			}
		}
		if (!errno ){
			if (!isNew ) {
				rename(fileName,backupName);
				if (errno){
					warn(strerror(errno));
					errno=0;
				}
			}
			rename(tmpName,fileName);
			if (errno){
				warn(strerror(errno));
				errno=0;
				result=false;
			} else {
				result=true;
			}
		}
		if (errno) {
			warn(strerror(errno));
			errno=0;
		}
		free(backupName);
	}
	return result;
}

bool sendConfVDR(int hostId, cfgParamList_t * const params) {
	bool onlyDiff=boolean(hostId==params->hostId);
	hostConf_t *host=getHost(hostId);
	if (host && host->isVdr){
		bool result=true;
		char *cmd;
		int i;
		cfgParam_t *param;
		for(i=0,param=params->entry;i<params->length;i++,param++) {
			if (!onlyDiff || strcmp(param->value,param->oldValue)!=0) {
				crit_goto_if(asprintf(&cmd,"PARS %s=%s",param->name,(param->value)?param->value:"")<0,outOfMemory);
				dbg("cmd[%s]",cmd);
				char *data=execSvdrp(host,cmd);
				param->written=boolean(data && atoi(data)==SVDRP_CMD_OK);
				if (!param->written){
					result=false;
				}
				free(cmd);
			}
		}
		return result;
	} else {
		warn("Host %d no existe o no ejecuta VDR",hostId);
	}
	return false;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

bool writeConf(int hostId, cfgParamList_t * const params) {
	return (params->fileId==CF_VDRCONF) ? sendConfVDR(hostId,params) : writeConfFile(params);
}

bool validateHostsStr(const char *hosts){
	if (hosts==NULL) return false;
	const char * pattern="^(([a-zA-Z0-9]*),([0-9.]*),([0-9]*),([a-zA-Z0-9./_]*);?)+$";
	regex_t regex;
	if (regcomp(&regex,pattern,REG_EXTENDED|REG_NOSUB)!=0) {
		crit("Error compiling the pattern!");
		exit(1);
	}
	bool ok=(regexec(&regex,hosts,0,NULL,0)==0);
	regfree(&regex);
	return ok;
}

//TODO
void parseHostsFieldJson(const char *hostsField){
	/*
	sintaxis:
	  {[name:"<host name>",][ip:"<SVDRP ip>",][port:"<SVDRP port>",][video0:"<video0 path>"]}[,{<host 2>}...]
	where: 
	  {},:"      are literals
	  []         optional content
	  <value>    replace by real value
	*/
	warn("No implementado");
	exit(1);
}

void parseHostsFieldOld(const char *hostsField){
	//sintaxis: [<host name>],[<SVDRP ip>],[<SVDRP port>],[<video0 path>];...
	hostConf_t *host;
	enum {MAXREGMATCH=HOSTNUMCONFFIELDS+1};
	const char * pattern="([a-zA-Z0-9]*),([0-9.]*),([0-9]*),([a-zA-Z0-9./_]*);?";
	regex_t regex;
	if (regcomp(&regex,pattern,REG_EXTENDED)!=0) {
		crit("Error compiling the pattern!");
		exit(1);
	}
	regmatch_t regmatch[MAXREGMATCH];
	int err,m,so,l;
	const char *t=hostsField;
	while (t[0]) {
		if ((err=regexec(&regex,t,MAXREGMATCH,regmatch,0))==0){
			bool isValid=true;
			crit_goto_if((webifConf.hosts=(hostConf_t*)realloc(webifConf.hosts,(++webifConf.hostsLength)*sizeof(hostConf_t)))==NULL,outOfMemory);
			host=webifConf.hosts+webifConf.hostsLength-1;
			initHostConf(host);
			for (m=1;m<MAXREGMATCH && isValid;m++){
				so=regmatch[m].rm_so;
				l=regmatch[m].rm_eo-so;
				if (so>=0 && l>=0) {
					errno=0;
					switch (m) {
						case 1: 
							crit_goto_if((host->name=strndup(t+so,l))==NULL,outOfMemory);
							break;
						case 2: 
							crit_goto_if((host->ip=strndup(t+so,l))==NULL,outOfMemory);
							break;
						case 3: 
							host->port=strtol(t+so,NULL,10);
							break;
						case 4: 
							if (l>1 && t[so+l-1]=='/') l--;
							crit_goto_if((host->video0=strndup(t+so,l))==NULL,outOfMemory);
							break;
						default :
							warn("Two much parameters for a host cfg.");
					};
					if (errno) {
						warn(strerror(errno));
						isValid=false;
						errno=0;
					}
				} else {
					isValid=false;
				}
			}
			if (isValid) {
				host->id=webifConf.hostsLength-1;
				host->isVdr=boolean(host->ip && host->ip[0] && host->port>0);
				if (host->isVdr) webifConf.numVDRs++;
			} else {
				freeHostConf(host);
				webifConf.hostsLength--;
			}
			if (regmatch[0].rm_eo>0) t+=regmatch[0].rm_eo; else break;
		} else if (err==REG_NOMATCH){
			break;
		} else if (err==REG_ESPACE){
			goto outOfMemory;
		}
	}
	regfree(&regex);
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void parseHostsField(const char *hostsField){
	hostConf_t *host;
	webifConf.hostsLength=0;
	webifConf.numVDRs=0;
	if (hostsField!=NULL){
		if (hostsField[0]=='{'){
			parseHostsFieldJson(hostsField);
		} else {
			parseHostsFieldOld(hostsField);
		}
	}
	if (webifConf.hostsLength==0){
		warn("No valid host set up");
		crit_goto_if((webifConf.hosts=(hostConf_t*)realloc(webifConf.hosts,(++webifConf.hostsLength)*sizeof(hostConf_t)))==NULL,outOfMemory);
		host=webifConf.hosts;
		initHostConf(host);
		crit_goto_if((host->name=strdup(""))==NULL,outOfMemory);
		crit_goto_if((host->ip=strdup("127.0.0.1"))==NULL,outOfMemory);
		host->port=2001;
		crit_goto_if((host->video0=strdup(VIDEO0))==NULL,outOfMemory);
		host->isVdr=true;
		webifConf.numVDRs=1;
	}
	return;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

time_t get_mtime(const char *fileName){
	struct stat status;
	if (stat(fileName,&status)==0){
		return status.st_mtime;
	} else {
		warn(strerror(errno));
		return 0;
	}
}

bool readWebifConf() {
	time_t current_mtime=get_mtime(cfgFile[CF_WEBIFCONF].fileName);
	if (webifConf.mtime<current_mtime || errno){
		webifConf.alreadySet=false;
		errno=0;
	}
	if (!webifConf.alreadyInit) {
		error("No se ha llamado a initWebifConf. Se llamará ahora, pero no se liberarán recursos");
		initWebifConf();
	}
	if (webifConf.alreadySet) {
		return true;
	}
	cfgParamList_t params;
	cfgParam_t *param;
	initCfgParamList(&params);
	params.hostId=0;
	params.fileId=CF_WEBIFCONF;
	if (readConf(&params)){
		int i;
		for (i=0,param=params.entry;i<params.length;i++,param++){
			if (strcmp(param->name,"default language")==0) {
				errno=0;
				webifConf.langId=strtol(param->value,NULL,10);
				if (errno!=0 || webifConf.langId<-1 || webifConf.langId>=I18NNUM) webifConf.langId=-1;
			} else if (strcmp(param->name,"hosts")==0) {
				parseHostsField(param->value);
			} else if (strcmp(param->name,"playlist_type")==0) {
				if (param->value && param->value[0]){
					webifConf.playlistType=strtol(param->value,NULL,10);
				}
			} else if (strcmp(param->name,"deletion_disabled")==0) {
				if (param->value && param->value[0]){
					webifConf.deletionDisabled=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"config_change_disabled")==0) {
				if (param->value && param->value[0]){
					webifConf.configChangeDisabled=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"config_view_disabled")==0) {
				if (param->value && param->value[0]){
					webifConf.configViewDisabled=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"use_external_www_folder")==0) {
				if (param->value && param->value[0]){
					webifConf.useExternalWwwFolder=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"display_host_id")==0) {
				if (param->value && param->value[0]){
					webifConf.displayHostId=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"print_rec_folder_summary")==0) {
				if (param->value && param->value[0]){
					webifConf.printRecFolderSummary=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"max_depth")==0) {
				if (param->value && param->value[0]){
					webifConf.maxDepth=strtol(param->value,NULL,10);
				}
			} else if (strcmp(param->name,"default_margin_start")==0) {
				if (param->value && param->value[0]){
					webifConf.defaultMarginStart=atoi(param->value);
				}
			} else if (strcmp(param->name,"default_margin_stop")==0) {
				if (param->value && param->value[0]){
					webifConf.defaultMarginStop=atoi(param->value);
				}
			} else if (strcmp(param->name,"video_width")==0) {
				if (param->value && param->value[0]){
					webifConf.videoWidth=atoi(param->value);
				}
			} else if (strcmp(param->name,"video_height")==0) {
				if (param->value && param->value[0]){
					webifConf.videoHeight=atoi(param->value);
				}
			} else if (strcmp(param->name,"epg_grid_hours")==0) {
				if (param->value && param->value[0]){
					webifConf.epgGridHours=atoi(param->value);
				}
			} else if (strcmp(param->name,"channel_logo_width")==0) {
				if (param->value && param->value[0]){
					webifConf.channelLogoWidth=atoi(param->value);
				}
			} else if (strcmp(param->name,"user")==0){
				if (param->value && param->value[0]){
					webifConf.user=strdup(param->value);
				}
			} else if (strcmp(param->name,"password")==0){
				if (param->value && param->value[0]){
					webifConf.password=strdup(param->value);
				}
			} else if (strcmp(param->name,"eventSearchUrl")==0){
				if (param->value && param->value[0]){
					webifConf.eventSearchUrl=strdup(param->value);
				}
			}
		}
		freeCfgParamList(&params);
		struct stat st;
		int logos_stat=stat("/etc/webif/www/images/logos",&st);
		if (logos_stat == 0 && S_ISDIR(st.st_mode)){
			webifConf.noLogos=false;
		} else {
			warn("No existe directorio de logos");
			webifConf.noLogos=true;
		}
		crit_goto_if((webifConf.www=strdup((webifConf.useExternalWwwFolder)?"/www2":""))==NULL,outOfMemory);
		webifConf.alreadySet=true;
		webifConf.mtime=current_mtime;
		return true;
	}
	return false;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void resetWebifConf(){
	webifConf.alreadySet=false;
	webifConf.langId=0;
	webifConf.hosts=NULL;
	webifConf.hostsLength=0;
	webifConf.numVDRs=0;
	webifConf.user=NULL;
	webifConf.password=NULL;
	webifConf.eventSearchUrl=NULL;
}

void initWebifConf(){
	if (webifConf.alreadyInit) {
		error("No se deberia ejecutar mas de una vez por proceso");
		return;
	}
	resetWebifConf();
	if (regcomp(&webifConf.eod_regex,"^[0-9]{3} ",REG_EXTENDED | REG_NOSUB | REG_NEWLINE)!=0) {
		crit(strerror(errno));
		exit(1);
	}
	webifConf.alreadyInit=true;
}

void freeWebifConf(){
	if (!webifConf.alreadyInit) {
		error("O no se ha iniciado, o ya se han liberado recursos");
		return;
	}
	free(webifConf.www);
	webifConf.www=NULL;
	if (webifConf.user!=NULL){
		free(webifConf.user);
	}
	if (webifConf.password!=NULL){
		free(webifConf.password);
	}
	if (webifConf.hostsLength>0){
		int i;
		hostConf_t *host;
		for (i=0,host=webifConf.hosts;i<webifConf.hostsLength;i++,host++){
			freeHostConf(host);
		}
		free(webifConf.hosts);
	}
	if (webifConf.eventSearchUrl!=NULL){
		free(webifConf.eventSearchUrl);
	}
	regfree(&webifConf.eod_regex);
	resetWebifConf();
	webifConf.alreadyInit=false;
}

void initHostConf(hostConf_t * const host){
	host->id=0;
	host->name=NULL;
	host->ip=NULL;
	host->video0=NULL;
	host->socket=0;
}

void freeHostConf(hostConf_t * const host){
	free(host->name);
	free(host->ip);
	free(host->video0);
	initHostConf(host);
}

hostConf_t *getHost(int hostId){
	return (hostId>=0 && hostId<webifConf.hostsLength) ? webifConf.hosts+hostId : NULL;
}

hostConf_t *getFirstVdrHost(){
	hostConf_t *host;
	int i;
	for (i=0,host=webifConf.hosts;i<webifConf.hostsLength;i++,host++)
		if (host->isVdr) return host;
	return NULL;
}

bool isHostLocal(hostConf_t *host){
	return boolean(host!=NULL && strncmp(host->ip,"127.",4)==0);
}

const char *getHostHttpAddr(hostConf_t *host,wcontext_t *wctx){
	return (host==NULL)
	? NULL
	: (isHostLocal(host)) 
		? request_get_field(wctx->request,"Host")->value 
		: (host->name[0]) 
			? host->name 
			: host->ip;
}

void printVDRSelect(wcontext_t *wctx,const char * name,const int hostId){
	int i;
	hostConf_t *host;
	wctx_printfn(wctx,"<select name=\"%s\" size=\"1\">\n",0,1,name);
	for (i=0,host=webifConf.hosts;i<webifConf.hostsLength;i++,host++) {
		if (host->isVdr){
			wctx_printf0(wctx,"<option value=\"%d\" %s>%d - %s</option>\n",host->id,selected[boolean(host->id==hostId)],host->id,(host->name[0])?host->name:host->ip);
		}
	}
	wctx_printfn(wctx,"</select>\n",-1,0);
}

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

#define VIDEO0 "/var/vdr/video0"

void validateCheckbox(const cfgParamConfig_t * const paramConfig, cfgParam_t * const param){
	const char *option=(paramConfig->options)?paramConfig->options:"false|true";
	if (!strchr(option,'|')){
		crit("validateCheckbox: wrong config, only one option");
		exit(1);
	}
	param->isValid=BT_FALSE;
	int l=strcspn(option,"|");
	if (param->value==NULL || strncmp(option,param->value,l)==0){
		//value==NULL => first option (unchecked checkboxes aren't submitted)
		param->value=strndup(option,l);
		param->isValid=BT_TRUE;
	} else {
		option+=l+1;
		l=strcspn(option,"|");
		if (option[l]=='|') warn("More than two options for a checkboxCfg");
		param->isValid=boolean(param->value!=NULL && strncmp(option,param->value,l)==0);
	}
}

boolean_t printCheckbox(context_t *ctx,const cfgParamConfig_t * const paramConfig
	,const char *id, const char *paramName,int paramIdx,const char *paramValue
){
	if (paramConfig->options) {
		//value==first option => false
		//value==second option =>true
		boolean_t booleanValue=BT_FALSE;
		boolean_t booleanValueSet=BT_FALSE;
		const char *option=paramConfig->options;
		int l=strcspn(option,"|");
		if (strncmp(option,paramValue,l)==0){
			booleanValueSet=BT_TRUE;
		}
		if (option[l]!='|'){
			warn("Only one option for a checkboxCfg");
			option=NULL;
		} else {
			option+=l+1;
			l=strcspn(option,"|");
			if (strncmp(option,paramValue,l)==0){
				booleanValue=BT_TRUE;
				booleanValueSet=BT_TRUE;
			}
		}
		if (!booleanValueSet) {
			warn("No option match for checkbox value");
		}
		ctx_printf0(ctx,"<input type=\"checkbox\"");
		if (id) ctx_printf(ctx,"id=\"%s\" ",id);
		if (paramName==NULL){
			ctx_printf(ctx,"name=\"paramValue_%d\" ",paramIdx);
		} else {
			ctx_printf(ctx,"name=\"%s\" ",paramName);
		}
		ctx_printf(ctx,"value=\"%.*s\"%s/>\n",(option)?l:4,(option)?option:"true",checked[booleanValue]);
		return BT_TRUE;
	}
	return BT_FALSE;
}

boolean_t printSelect(context_t *ctx,const cfgParamConfig_t * const paramConfig
	,const char *id, const char *paramName,int paramIdx,const char *paramValue
){
	if (paramConfig && paramConfig->options && strlen(paramConfig->options)>0) {
		ctx_printfn(ctx,"<select ",0,1);
		if (id) ctx_printf(ctx,"id=\"%s\" ",id);
		if (paramName==NULL){
			ctx_printf(ctx,"name=\"paramValue_%d\" ",paramIdx);
		} else {
			ctx_printf(ctx,"name=\"%s\" ",paramName);
		}
		ctx_printf(ctx,"size=\"1\">\n");
		const char * option=paramConfig->options;
		int o=paramConfig->indexOffset;
		for(;;) {
			int l=strcspn(option,"|");
			char *optionnt=strndup(option,l);
			const char *optiontre=CTX_HTML_ENCODE(tr(optionnt),-1);
			if (paramConfig->isIndex) {
				ctx_printf0(ctx,"<option value=\"%d\"%s>%s</option>\n",o,selected[sameIntEx(paramValue,o)],optiontre);
			} else {
				ctx_printf0(ctx,"<option value=\"%s\"%s>%s</option>\n",optionnt,selected[sameString(paramValue,optionnt)],optiontre);
			}
			free(optionnt);
			option+=l;
			if (option[0]!='|') break;
			option++;
			o++;
		}
		ctx_printfn(ctx,"</select>\n",-1,0);
		return BT_TRUE;
	}
	return BT_FALSE;
}

boolean_t validateHostsStr(const char *hosts);

void validateHostsField(const cfgParamConfig_t * const paramConfig, cfgParam_t * const param){
	param->isValid=validateHostsStr(param->value);
}

webifConf_t webifConf;

cfgParamConfig_t webifParamConfig[]={ //Keep sorted by name !!
	{"always_close_svdrp","true","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"deletion_disabled","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"default language","-1","langBrowserDefined|langEnglish|langGerman|langSpanish|langFrench",BT_TRUE,-1,NULL,(cfgParamPrintInput_t)printSelect,BT_FALSE},
	{"default_margin_start","15",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"default_margin_stop","20",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"display_host_id","true","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"config_change_disabled","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"config_view_disabled","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"hosts",",127.0.0.1,2001,/var/vdr/video0;",NULL,BT_FALSE,0,(cfgParamValidate_t)validateHostsField,NULL/*(cfgParamPrintInput_t)printHostsField*/,BT_FALSE},
	{"max_depth","2",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"playlist_type","0","M3U|XSPF",BT_TRUE,0,NULL,printSelect,BT_FALSE},
	{"print_rec_folder_summary","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"use_external_www_folder","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"video_width","640",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"video_height","480",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"epg_schedule_grid_width","900",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"channel_logo_width","58",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
};

cfgParamConfig_t rcParamConfig[]={ //Keep sorted by name !!
	{"ip","192.168.100.102",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"net","inet","inet|DHCP",BT_FALSE,0,NULL,printSelect,BT_FALSE},
	{"netdate","NO","NO|YES",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"netmask","255.255.255.0",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
};

cfgParamConfig_t vdrParamConfig[] = { //Keep sorted by name !!
	{"AbortWhenPluginFails","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"CutterAutoDelete","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"DelTimeshiftRec","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"EmergencyExit","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"HardLinkCutter","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"HotStandby","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"MarkInstantRecord","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"MenuButtonCloses","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"MenuScrollPage","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"MenuScrollWrap","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"PauseLifetime","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"SetSystemTime","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShowInfoOnChSwitch","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShowProgressBar","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShowRecDate","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShowRecLength","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShowRecTime","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShowReplayMode","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"ShutdownMessage","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"SplitEditedFiles","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"UseDolbyDigital","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"UseDolbyInRecordings","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"UseSubtitle","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"UseVps","0","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"WarEagleIcons","1","0|1",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
};

cfgParamConfig_t boxampParamConfig[] = { //Keep sorted by name !!
	{"boxamp_bin","boxamp",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"boxamp_mp3dir","/var/media/mp3",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"boxamp_opts","-f ${boxamp_path}",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"boxamp_path","/var/media/pc2/boxamp",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"boxamp_startup_time","300",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"runboxamp","/var/media/pc2/boxamp/runboxamp",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
};

cfgParamConfigList_t cfgParamConfig[] = { //Indexed by cfgFileId_t
	{BT_TRUE,sizeof(webifParamConfig)/sizeof(cfgParamConfig_t),webifParamConfig},
	{BT_FALSE,sizeof(rcParamConfig)/sizeof(cfgParamConfig_t),rcParamConfig},
	{BT_FALSE,sizeof(vdrParamConfig)/sizeof(cfgParamConfig_t),vdrParamConfig},
	{BT_FALSE,sizeof(boxampParamConfig)/sizeof(cfgParamConfig_t),boxampParamConfig}
};

const cfgFile_t fileMapping[] = {
	{"cfgWi", "/etc/webif.conf"},
	{"cfgRc", "/etc/rc.conf"},
	{"vdrSetup", "/etc/vdr/setup.conf"},
	{"cfgBa", "/etc/bootmenu/bm.local.boxamp.conf"},
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
	{"cfgWi", "/etc/webif.conf"},
	{"cfgRc", "/etc/rc.conf"},
	{"vdrSetup", "/etc/vdr/setup.conf"},
	{"cfgBa", "/etc/bootmenu/bm.local.boxamp.conf"},
};
const int cfgFileLength=sizeof(cfgFile)/sizeof(cfgFile_t);

void initCfgParam(cfgParam_t * const param){
	param->name=NULL;
	param->value=NULL;
	param->comment=NULL;
	param->configId=-1;
	param->written=BT_FALSE;
	param->isValid=BT_TRUE;
}

void freeCfgParam(cfgParam_t * const param){
	free(param->name);
	free(param->value);
	free(param->comment);
	initCfgParam(param);
}

void initCfgParamList(cfgParamList_t * const params){
	params->length=0;
	params->entry=NULL;
}

void freeCfgParamList(cfgParamList_t * const params){
	int i;
	for (i=0;i<params->length;i++) freeCfgParam(params->entry+i);
	free(params->entry);
	initCfgParamList(params);
}

const cfgParamConfig_t *getCfgParamConfig(cfgFileId_t cfgFileId, cfgParam_t * const param){
	if (param->configId>=0 && param->configId<cfgParamConfig[cfgFileId].length) {
		return cfgParamConfig[cfgFileId].entry+param->configId;
	}
	int i;
	const cfgParamConfig_t *config;
	//TODO busqueda binaria
	for (i=0,config=cfgParamConfig[cfgFileId].entry;i<cfgParamConfig[cfgFileId].length;i++,config++){
		if (strcmp(config->name,param->name)==0){
			param->configId=i;
			return config;
		}
	}
	return NULL;
}
boolean_t isValidParamName(const char *name, int nameLength,cfgParamConfigList_t *configs){
	if (configs->exclusive){
		cfgParamConfig_t *config;
		int i;
		for (i=0,config=configs->entry;i<configs->length;i++,config++){
			if (strncmp(name,config->name,nameLength)==0) return BT_TRUE;
		}
		return BT_FALSE;
	} else
		return BT_TRUE;
}

boolean_t readConf(cfgFileId_t cfgFileId, cfgParamList_t * const params, boolean_t *isNew) {
	if (cfgFileId<CF_WEBIFCONF || cfgFileId>CF_BOXAMPCONF) return BT_FALSE;
	int i;
	cfgParamConfig_t *config;
	cfgParamConfigList_t *configs=&cfgParamConfig[cfgFileId];
	FILE *f = fopen(cfgFile[cfgFileId].fileName,"r");
	initCfgParamList(params);
	cfgParam_t *param;
	*isNew=BT_TRUE;
	for (i=0,config=configs->entry;i<configs->length;i++,config++) config->alreadySet=BT_FALSE;
	if (f) {
		enum {BUFSZ=256};
		char buffer[BUFSZ];
		const char *s;
		int l,l2;
		while (!feof(f) && (s=fgets(buffer,BUFSZ,f))!=NULL) {
			s+=strspn(s," ");
			if ((s[0]) && (s[0]!='#')) {
				l=strcspn(s,"=\n\r");
				if (s[l]=='='){
					l2=l;
					while (l2>0 && s[l2-1]==' ') l2--;
					if (l2>0 && isValidParamName(s,l2,configs)){
						crit_goto_if((params->entry=realloc(params->entry, (++params->length)*sizeof(cfgParam_t)))==NULL,outOfMemory);
						param=params->entry+params->length-1;
						initCfgParam(param);
						param->name=strndup(s,l2);
						s+=l+1;
						s+=strspn(s," ");
						l2=l=strcspn(s,"#\n\r");
						while (l2>0 && s[l2-1]==' ') l2--;
						param->value=strndup(s,l2);
						if (s[l]=='#'){
							s+=l+1;
							s+=strspn(s," ");
							l2=l=strcspn(s,"\n\r");
							while (l2>0 && s[l2-1]==' ') l2--;
							param->comment=strndup(s,l2); //TODO falla si # esta en value
						} else {
							param->comment=NULL;
						}
						for (i=0,config=configs->entry;i<configs->length;i++,config++){
							if (strcmp(config->name,param->name)==0){
								param->configId=i;	
								config->alreadySet=BT_TRUE;
								break;
							}
						}
					}
				}
			}
		}
		fclose(f);
		*isNew=BT_FALSE;
	}
	for (i=0,config=configs->entry;i<configs->length;i++,config++){
		if (config->alreadySet) continue;
		crit_goto_if((params->entry=realloc(params->entry, (++params->length)*sizeof(cfgParam_t)))==NULL,outOfMemory);
		param=params->entry+params->length-1;
		initCfgParam(param);
		param->name=strdup(config->name);
		param->value=(config->defaultValue)?strdup(config->defaultValue):NULL;
		dbg("Asignado parametro por defecto [%s]=[%s]",param->name,param->value);
		param->configId=i;
		config->alreadySet=BT_TRUE;
	}
	return boolean(params->length>0);
outOfMemory:
	crit("readConf:Out of memory");
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

boolean_t writeConf(cfgFileId_t cfgFileId, cfgParamList_t * const params) {
	boolean_t result=BT_FALSE;
	boolean_t isNew=BT_FALSE;
	boolean_t exclusive=cfgParamConfig[cfgFileId].exclusive;
	const char *fileName=cfgFile[cfgFileId].fileName;
	errno=0;
	FILE *f = fopen(fileName,"r");
	if (errno){
		if (errno==ENOENT){
			isNew=BT_TRUE;
			errno=0;
		} else {
			warn(strerror(errno));
			return BT_FALSE;
		}
	}
	char tmpName[16];
	FILE *t=tmpCfgFile(tmpName);
	if (t==NULL) {
		if (!isNew) fclose(f);
		return BT_FALSE;
	}
	int i;
	char *paramName;
	char *paramValue;
	char *paramComment;
	cfgParam_t *param;
	for (i=0;i<params->length;i++) params->entry[i].written=BT_FALSE;
	if (!isNew) {
		enum {BUFSZ=256};
		char buffer[BUFSZ];
		char *s;
		int l,ln,lv,lc;
		while (!feof(f) && (s=fgets(buffer,BUFSZ,f))!=NULL) {
			paramName=NULL;
			paramValue=NULL;
			paramComment=NULL;
			s+=strspn(s," ");
			if (!s[0] || s[0]=='#'){ //write comments and empty lines unmodified
				fputs(buffer,t);
				if (errno){
					warn(strerror(errno));
					errno=0;
				}
			} else {
				paramName=s;
				l=strcspn(s,"=\n\r");
				if (s[l]=='='){
					ln=l;
					while (ln>0 && s[ln-1]==' ') ln--;
					if (ln>0){
						s+=l+1;
						s+=strspn(s," ");
						paramValue=s;
						lv=l=strcspn(s,"#\n\r");
						if (s[l]=='#') paramComment=s+l+1;
						while (lv>0 && s[lv-1]==' ') lv--;
						paramName[ln]=0;
						paramValue[lv]=0;
						param=NULL;
						for (i=0;i<params->length;i++){
							if (strcmp(params->entry[i].name,paramName)==0){
								param=params->entry+i;
								break;
							}
						}
						if (exclusive && !param) {
							dbg("param [%s] eliminado de conf [%s]", paramName, fileName);
						} else {
							if (param && param->value){
								paramValue=param->value;
							}
							if (paramComment) {
								while (paramComment[0]==' ') paramComment++;
								lc=strcspn(paramComment,"\n\r");
								while (lc>0 && paramComment[lc]==' ') lc--;
								fprintf(t,"%s=%s #%s\n",paramName,paramValue,paramComment);
							} else {
								fprintf(t,"%s=%s\n",paramName,paramValue);
							}
						}
						if (errno){
							warn(strerror(errno));
							errno=0;
						}
						if (param) param->written=BT_TRUE;
					}
				}
			}
		}
		fclose(f);
	}
	for(i=0,param=params->entry;i<params->length;i++,param++) if (!param->written) {
		fprintf(t,"%s=%s\n",param->name,param->value);
		param->written=BT_TRUE;
	}
	fclose(t);
	char *backupName;
	asprintf(&backupName,"%s.webif",fileName);
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
				result=BT_FALSE;
			} else {
				result=BT_TRUE;
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


boolean_t validateHostsStr(const char *hosts){
	if (hosts==NULL) return BT_FALSE;
	const char * pattern="^(([a-zA-Z0-9]*),([0-9.]*),([0-9]*),([a-zA-Z0-9./_]*);?)+$";
	regex_t regex;
	if (regcomp(&regex,pattern,REG_EXTENDED|REG_NOSUB)!=0) {
		crit("Error compiling the pattern!");
		exit(1);
	}
	boolean_t ok=(regexec(&regex,hosts,0,NULL,0)==0);
	regfree(&regex);
	return ok;
}

boolean_t parseHostsField(const char *hosts,boolean_t apply){
	webifConf.hostsLength=0;
	webifConf.numVDRs=0;
	if (hosts==NULL) return BT_FALSE;
	enum {MAXREGMATCH=HOSTNUMCONFFIELDS+1};
	const char * pattern="([a-zA-Z0-9]*),([0-9.]*),([0-9]*),([a-zA-Z0-9./_]*);?";
	regex_t regex;
	if (regcomp(&regex,pattern,REG_EXTENDED)!=0) {
		crit("Error compiling the pattern!");
		exit(1);
	}
	regmatch_t regmatch[MAXREGMATCH];
	boolean_t ok=BT_FALSE;
	int err,m,so,l;
	const char *t=hosts;
	hostConf_t *host=webifConf.hosts;
	while (t[0]) {
		if ((err=regexec(&regex,t,MAXREGMATCH,regmatch,0))==0){
			boolean_t isValid=BT_TRUE;
			for (m=1;m<MAXREGMATCH && isValid;m++){
				so=regmatch[m].rm_so;
				l=regmatch[m].rm_eo-so;
				if (so>=0 && l>=0) {
					errno=0;
					switch (m) {
						case 1: strncpy(host->name,t+so,l); host->name[l]=0; break;
						case 2: strncpy(host->ip,t+so,l); host->ip[l]=0; break;
						case 3: host->port=strtol(t+so,NULL,10);break;
						case 4: 
							if (l>1 && t[so+l-1]=='/') l--;
							strncpy(host->video0,t+so,l); host->video0[l]=0;
							break;
						default :
							warn("Two much parameters for a host cfg.");
					};
					if (errno) {
						warn(strerror(errno));
						isValid=BT_FALSE;
						errno=0;
					}
				} else {
					isValid=BT_FALSE;
				}
			}
			if (isValid) {
				host->id=webifConf.hostsLength;
				host->isVdr=boolean(strlen(host->ip)>0 && host->port>0);
				if (host->isVdr) webifConf.numVDRs++;
				ok=BT_TRUE; //ok if there is at least one valid host
				webifConf.hostsLength++;
				host++;
			}
			if (regmatch[0].rm_eo>0) t+=regmatch[0].rm_eo; else break;
		} else if (err==REG_NOMATCH){
			break;
		} else if (err==REG_ESPACE){
			goto outOfMemory;
		}
	}
	regfree(&regex);
	return ok;
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

boolean_t readWebifConf() {
	//TODO integrar en readConf mediante funciones en paramCfgs
	errno=0;
	time_t current_mtime=get_mtime(cfgFile[CF_WEBIFCONF].fileName);
	if (webifConf.mtime<current_mtime || errno){
		webifConf.alreadySet=BT_FALSE;
		errno=0;
	}
	if (webifConf.alreadySet) {
		//dbg("webifConf ya actualizado, no se lee.");
		return;
	}
	freeWebifConf();
	cfgParamList_t params;
	cfgParam_t *param;
	boolean_t isNew;
	if (readConf(CF_WEBIFCONF,&params,&isNew)){
		int i;
		for (i=0,param=params.entry;i<params.length;i++,param++){
			if (strcmp(param->name,"default language")==0) {
				errno=0;
				webifConf.langId=strtol(param->value,NULL,10);
				if (errno!=0 || webifConf.langId<-1 || webifConf.langId>=I18NNUM) webifConf.langId=-1;
			} else if (strcmp(param->name,"hosts")==0) {
				if (!parseHostsField(param->value,BT_TRUE)){
					warn("No valid host set up");
					webifConf.hostsLength=1;
					webifConf.hosts[0].name[0]=0;
					strcpy(webifConf.hosts[0].ip,"127.0.0.1");
					webifConf.hosts[0].port=2001;
					strcpy(webifConf.hosts[0].video0,VIDEO0);
					webifConf.hosts[0].isVdr=BT_TRUE;
				}
			} else if (strcmp(param->name,"playlist_type")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.playlistType=strtol(param->value,NULL,10);
				}
			} else if (strcmp(param->name,"deletion_disabled")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.deletionDisabled=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"config_change_disabled")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.configChangeDisabled=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"config_view_disabled")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.configViewDisabled=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"use_external_www_folder")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.useExternalWwwFolder=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"display_host_id")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.displayHostId=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"print_rec_folder_summary")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.printRecFolderSummary=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"max_depth")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					errno=0;
					webifConf.maxDepth=strtol(param->value,NULL,10);
					if (errno){
						warn("webifConf.maxDepth %s",strerror(errno));
					}
				}
			} else if (strcmp(param->name,"always_close_svdrp")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.alwaysCloseSvdrp=sameString(param->value,"true");
				}
			} else if (strcmp(param->name,"default_margin_start")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.defaultMarginStart=atoi(param->value);
				}
			} else if (strcmp(param->name,"default_margin_stop")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.defaultMarginStop=atoi(param->value);
				}
			} else if (strcmp(param->name,"video_width")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.videoWidth=atoi(param->value);
				}
			} else if (strcmp(param->name,"video_height")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.videoHeight=atoi(param->value);
				}
			} else if (strcmp(param->name,"epg_schedule_grid_width")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.tvScheduleGridWidth=atoi(param->value);
				}
			} else if (strcmp(param->name,"channel_logo_width")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.channelLogoWidth=atoi(param->value);
				}
			}
		}
		freeCfgParamList(&params);
		struct stat st;
		int logos_stat=stat("/etc/webif/www/images/logos",&st);
		if (logos_stat == 0 && S_ISDIR(st.st_mode)){
			info("Existe directorio de logos");
			webifConf.noLogos=BT_FALSE;
		} else {
			warn("No existe directorio de logos");
			webifConf.noLogos=BT_TRUE;
		}
		crit_goto_if((webifConf.www=strdup((webifConf.useExternalWwwFolder)?"/www2":""))==NULL,outOfMemory);
		webifConf.alreadySet=BT_TRUE;
		webifConf.mtime=current_mtime;
		return BT_TRUE;
	}
	return BT_FALSE;
outOfMemory:
	crit("Out of memory");
	exit(1);
}

void resetWebifConf(){
	memset(&webifConf,0,sizeof(webifConf_t));
}

void freeWebifConf(){
	free(webifConf.www);
	webifConf.www=NULL;
	//TODO eliminar hosts
	resetWebifConf();
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

boolean_t isHostLocal(hostConf_t *host){
	return boolean(host!=NULL && strncmp(host->ip,"127.",4)==0);
}

const char *getHostHttpAddr(hostConf_t *host,context_t *ctx){
	return (host==NULL)
	? NULL
	: (isHostLocal(host)) 
		? request_get_field(ctx->request,"Host")->value 
		: (host->name[0]) 
			? host->name 
			: host->ip;
}

void printVDRSelect(context_t *ctx,const char * name,const int hostId){
	int i;
	hostConf_t *host;
	ctx_printfn(ctx,"<select name=\"%s\" size=\"1\">\n",0,1,name);
	for (i=0,host=webifConf.hosts;i<webifConf.hostsLength;i++,host++) {
		if (host->isVdr){
			ctx_printf0(ctx,"<option value=\"%d\" %s>%d - %s</option>\n",host->id,selected[boolean(host->id==hostId)],host->id,(host->name[0])?host->name:host->ip);
		}
	}
	ctx_printfn(ctx,"</select>\n",-1,0);
}

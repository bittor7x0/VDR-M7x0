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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "conf.h"
#include "i18n.h"
#include "misc.h"

webifConf_t webifConf = {
	BT_FALSE, //allreadySet
	-1, //langId
	"127.0.0.1",
	2001,
	PL_M3U,
	BT_FALSE, //disable rec deletion
	BT_FALSE, //disable config
	BT_FALSE, //useExternalWwwFolder
};

vdrConf_t vdrConf = {
	BT_FALSE, //allreadySet
	5*60, //marginStart
	15*60 //marginStop
};

boolean_t validateCheckbox(const cfgParamConfig_t * const paramConfig, cfgParam_t * const param){
	if (paramConfig->options) {
		//value==NULL => first option
		const char *option=paramConfig->options;
		int l;
		l=strcspn(option,"|");
		if (param->value==NULL){ //unchecked checkboxes aren't submitted
			param->value=strndup(option,l);
			return BT_TRUE;
		}
		if (option[l]!='|') {
			warn("validateCheckbox:only one option");
			return BT_FALSE;
		}
		option+=l+1;
		l=strcspn(option,"|");
		if (option[l]=='|') warn("validateCheckbox:more than 2 options");
		return boolean(param->value!=NULL && strncmp(option,param->value,l)==0);
	}
	return BT_FALSE;
}

boolean_t printCheckbox(io_t *out,const char * const tabs,const cfgParamConfig_t * const paramConfig
	,int paramIdx,const char *paramValue,const char encoded[]
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
			warn("printCheckbox:only one option");
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
			warn("printCheckbox:none of the options match the value");
		}
		io_printf(out,"%s<input type=\"checkbox\" name=\"paramValue_%d\" value=\"%.*s\"%s/>"
			,tabs,paramIdx,(option)?l:4,(option)?option:"true",checked[booleanValue]
		);
		return BT_TRUE;
	}
	return BT_FALSE;
}

boolean_t printSelect(io_t *out,const char * const tabs,const cfgParamConfig_t * const paramConfig
	,int paramIdx,const char *paramValue, char * const encoded
){
	if (paramConfig && paramConfig->options && strlen(paramConfig->options)>0) {
		io_printf(out,"%s<select name=\"paramValue_%d\" size=\"1\">\n",tabs,paramIdx);
		const char * option=paramConfig->options;
		int o=paramConfig->indexOffset;
		for(;;) {
			int l=strcspn(option,"|");
			char *optionnt=strndup(option,l);
			const char *optiontr=tr(optionnt);
			u_htmlncpy(encoded,optiontr,strlen(optiontr),HTMLCPY_ENCODE);
			if (paramConfig->isIndex) {
				io_printf(out,"%s	<option value=\"%d\" %s>%s</option>\n"
				,tabs,o,selected[sameIntEx(paramValue,o)],encoded);
			} else {
				io_printf(out,"%s	<option value=\"%s\"%s>%s</option>\n"
				,tabs,optionnt,selected[sameString(paramValue,optionnt)],encoded);
			}
			free(optionnt);
			option+=l;
			if (option[0]!='|') break;
			option++;
			o++;
		}
		io_printf(out,"%s</select>\n",tabs);
		return BT_TRUE;
	}
	return BT_FALSE;
}

cfgParamConfig_t webifParamConfig[]={ //Keep sorted by name !!
	{"default language","0","langBrowserDefined|langEnglish|langGerman|langSpanish|langFrench",BT_TRUE,-1,NULL,(cfgParamPrintInput_t)printSelect,BT_FALSE},
	{"config_change_disabled","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"config_view_disabled","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"playlist_type","M3U","M3U|XSPF",BT_TRUE,0,NULL,printSelect,BT_FALSE},
	{"rec_deletion_disabled","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
	{"svdrp_ip","127.0.0.1",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"svdrp_port","2001",NULL,BT_FALSE,0,NULL,NULL,BT_FALSE},
	{"use_external_www_folder","false","false|true",BT_FALSE,0,(cfgParamValidate_t)validateCheckbox,(cfgParamPrintInput_t)printCheckbox,BT_FALSE},
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
	{sizeof(webifParamConfig)/sizeof(cfgParamConfig_t),webifParamConfig},
	{sizeof(rcParamConfig)/sizeof(cfgParamConfig_t),rcParamConfig},
	{sizeof(vdrParamConfig)/sizeof(cfgParamConfig_t),vdrParamConfig},
	{sizeof(boxampParamConfig)/sizeof(cfgParamConfig_t),boxampParamConfig}
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

boolean_t readConf(cfgFileId_t cfgFileId, cfgParamList_t * const params, boolean_t *isNew) {
	if (cfgFileId<CF_WEBIFCONF || cfgFileId>CF_BOXAMPCONF) return BT_FALSE;
	int i;
	cfgParamConfig_t *config;
	cfgParamConfigList_t *configs=&cfgParamConfig[cfgFileId];
	FILE *f = fopen(cfgFile[cfgFileId].fileName,"r");
	initCfgParamList(params);
	cfgParam_t *param;
	*isNew=BT_TRUE;
	for (i=0,config=configs->entry;i<configs->length;i++,config++) config->allreadySet=BT_FALSE;
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
					if (l2>0){
						params->length++;
						params->entry=realloc(params->entry, params->length*sizeof(cfgParam_t));
						if (params->entry==NULL){
							error("readConf:Out of memory");
							exit(1);
						}
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
								config->allreadySet=BT_TRUE;
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
		if (config->allreadySet) continue;
		params->length++;
		params->entry=realloc(params->entry, params->length*sizeof(cfgParam_t));
		if (params->entry==NULL){
			error("readConf:Out of memory");
			exit(1);
		}
		param=params->entry+params->length-1;
		initCfgParam(param);
		param->name=strdup(config->name);
		param->value=(config->defaultValue)?strdup(config->defaultValue):NULL;
		param->configId=i;
		config->allreadySet=BT_TRUE;
	}
	return boolean(params->length>0);
}

FILE * tmpCfgFile(char tmpName[16]){
	strcpy(tmpName,"/etc/cfgXXXXXX"); //TODO tiene que estar en etc para que rename no genere error
	errno=0;
	int tfd=mkstemp(tmpName);
	if (errno){
		perror("writeConf");
		return NULL;
	}
	FILE *t = fdopen(tfd,"w");
	if (errno){
		perror("writeConf");
		unlink(tmpName);
		close(tfd);
		return NULL;
	}
	return t;
}

boolean_t writeConf(cfgFileId_t cfgFileId, cfgParamList_t * const params) {
	boolean_t result=BT_FALSE;
	boolean_t isNew=BT_FALSE;
	const char *fileName=cfgFile[cfgFileId].fileName;
	errno=0;
	FILE *f = fopen(fileName,"r");
	if (errno){
		if (errno==ENOENT){
			isNew=BT_TRUE;
			errno=0;
		} else {
			perror("writeConf");
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
					perror("writeConf");
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
						if (errno){
							perror("writeConf");
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
				perror("writeConf");
			}
		}
		if (!errno ){
			if (!isNew ) {
				rename(fileName,backupName);
				if (errno){
					warn(strerror(errno));
					perror("writeConf");
					errno=0;
				}
			}
			rename(tmpName,fileName);
			if (errno){
				warn(strerror(errno));
				perror("writeConf");
				errno=0;
				result=BT_FALSE;
			} else {
				result=BT_TRUE;
			}
		}
		if (errno) {
			perror("writeConf");
			errno=0;
		}
		free(backupName);
	}
	return result;
}
boolean_t readWebifConf() {
	if (webifConf.allreadySet) {
		warn("WebifConf ya se ha configurado. Omitiendo readWebifConf");
		return;
	}
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
			} else if (strcmp(param->name,"svdrp_ip")==0) {
				if (strlen(param->value)>15) {
					//TODO warn
				} else {
					strcpy(webifConf.svdrpIp,param->value);
				}
			} else if (strcmp(param->name,"svdrp_port")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.svdrpPort=strtol(param->value,NULL,10);
				}
			} else if (strcmp(param->name,"playlist_type")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.playlistType=strtol(param->value,NULL,10);
				}
			} else if (strcmp(param->name,"rec_deletion_disabled")==0) {
				if (param->value==NULL || strlen(param->value)<1 ){
					//TODO warn
				} else {
					webifConf.recDeletionDisabled=sameString(param->value,"true");
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
			}
		}
		freeCfgParamList(&params);
		webifConf.allreadySet=BT_TRUE;
		return BT_TRUE;
	}
	return BT_FALSE;
}


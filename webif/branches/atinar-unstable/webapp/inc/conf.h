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

#ifndef __CONF_H__
#define __CONF_H__

#include "misc.h"

typedef enum cfgFileId_e {
	CF_WEBIFCONF,
	CF_RCCONF,
	CF_VDRCONF,
	CF_BOXAMPCONF
} cfgFileId_t;

typedef struct cfgParam_s {
	char *name;
	char *value;
	char *comment;
	int configId;
	boolean_t isValid;
	boolean_t written;
} cfgParam_t;

typedef struct cfgParamConfig_s cfgParamConfig_t;
typedef boolean_t (*cfgParamValidate_t)(const cfgParamConfig_t * const paramConfig, cfgParam_t * const param);
typedef boolean_t (*cfgParamPrintInput_t)(io_t *out,const char * const tabs
	,const cfgParamConfig_t * const paramConfig,int paramIdx,const char *paramValue,char * const encoded);

struct cfgParamConfig_s {
	const char *name;         // Name of the param
	const char *defaultValue; // defaultValue if needed
	const char *options;      // if multiple choice enter the options here, seperated by |
	boolean_t isIndex; //value is index in options
	int indexOffset;
	cfgParamValidate_t validate; //function to validate a param
	cfgParamPrintInput_t printInput; //function to print an input for the param in a form
	boolean_t allreadySet;
};

typedef struct cfgParamConfigList_s {
	int length;
	cfgParamConfig_t * const entry;
} cfgParamConfigList_t;

typedef struct cfgParamList_s {
	int length;
	cfgParam_t *entry;
} cfgParamList_t;

typedef struct cfgFile_s {
	const char *i18nKey;
	const char *fileName;
} cfgFile_t;

typedef struct webifConf_s {
	boolean_t allreadySet;
	int langId;
	char svdrpIp[16];
	uint16_t svdrpPort;
	playlistType_t playlistType;
	boolean_t recDeletionDisabled;
	boolean_t configChangeDisabled;
	boolean_t configViewDisabled;
	boolean_t useExternalWwwFolder;
} webifConf_t;

typedef struct vdrConf_s { //TODO leer conf
	boolean_t allreadySet;
	int marginStart;
	int marginStop;
} vdrConf_t;

extern webifConf_t webifConf;
extern vdrConf_t vdrConf;
extern const cfgFile_t const cfgFile[];
extern const int cfgFileLength;
extern const cfgFile_t fileMapping[];
extern const int fileMappingLength;

const cfgParamConfig_t *getCfgParamConfig(cfgFileId_t cfgFileId, cfgParam_t * const param);
boolean_t readConf(cfgFileId_t cfgFileId, cfgParamList_t * const params, boolean_t *isNew);
boolean_t writeConf(cfgFileId_t cfgFileId, cfgParamList_t * const params);
boolean_t readWebifConf();

#endif

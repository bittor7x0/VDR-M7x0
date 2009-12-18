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

#include <sys/types.h>
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
typedef void (*cfgParamValidate_t)(const cfgParamConfig_t * const paramConfig, cfgParam_t * const param);
typedef boolean_t (*cfgParamPrintInput_t)(context_t *ctx,const cfgParamConfig_t * const paramConfig
	,const char *inputId, const char *paramName,int paramIdx,const char *paramValue);

boolean_t printCheckbox(context_t *ctx,const cfgParamConfig_t * const paramConfig
	,const char *checkboxId, const char *paramName,int paramIdx,const char *paramValue);
boolean_t printSelect(context_t *ctx,const cfgParamConfig_t * const paramConfig
	,const char *selectId, const char *paramName,int paramIdx,const char *paramValue);

struct cfgParamConfig_s {
	const char *name;         // Name of the param
	const char *defaultValue; // defaultValue if needed
	const char *options;      // if multiple choice enter the options here, seperated by |
	boolean_t isIndex; //value is index in options
	int indexOffset;
	cfgParamValidate_t validate; //function to validate a param
	cfgParamPrintInput_t printInput; //function to print an input for the param in a form
	boolean_t alreadySet;
};

typedef struct cfgParamConfigList_s {
	boolean_t exclusive; //Ignore params not listed
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

enum {
	MAXHOSTS=3,
	HOSTNUMCONFFIELDS=4
};

//TODO longitudes
typedef struct hostConf_f {
	int id;
	char name[10];    //public name of the server
	char ip[16];      //ip as seen by the webif server
	uint16_t  port;   //port where SVDRP is listening
	char video0[128]; //path to video0
	boolean_t isVdr;  //VDR is running in host
	int  socket;
} hostConf_t;

typedef struct webifConf_s {
	boolean_t alreadySet;
	time_t mtime; //st_mtime of conf file in last read
	int langId;
	hostConf_t hosts[MAXHOSTS];  //TODO hacer dinamico
	int hostsLength;
	int numVDRs;
	playlistType_t playlistType;
	boolean_t deletionDisabled;
	boolean_t configChangeDisabled;
	boolean_t configViewDisabled;
	boolean_t useExternalWwwFolder;
	boolean_t displayHostId; //display host id in recordings
	boolean_t printRecFolderSummary;
	int maxDepth; //maximum directory depth while browsing
	boolean_t alwaysCloseSvdrp;  //always close SVDRP connection after each command
	int defaultMarginStart;
	int defaultMarginStop;
	int videoWidth;
	int videoHeight;
	int tvScheduleGridWidth;
	int channelLogoWidth;
	char *www;
	boolean_t noLogos;
} webifConf_t;

#define CHAN_LOGO_W ((webifConf.noLogos)?80:webifConf.channelLogoWidth)

extern webifConf_t webifConf;
extern const cfgFile_t const cfgFile[];
extern const int cfgFileLength;
extern const cfgFile_t fileMapping[];
extern const int fileMappingLength;

const cfgParamConfig_t *getCfgParamConfig(cfgFileId_t cfgFileId, cfgParam_t * const param);
boolean_t readConf(cfgFileId_t cfgFileId, cfgParamList_t * const params, boolean_t *isNew);
boolean_t writeConf(cfgFileId_t cfgFileId, cfgParamList_t * const params);
boolean_t readWebifConf();
void freeWebifConf();
hostConf_t *getHost(int hostId);
hostConf_t *getFirstVdrHost();
boolean_t isHostLocal(hostConf_t *host);
const char *getHostHttpAddr(hostConf_t *host,context_t *ctx);
void printVDRSelect(context_t *ctx,const char * name,const int hostId);

#endif

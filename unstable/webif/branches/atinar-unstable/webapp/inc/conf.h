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
	bool isValid;
	bool written;
} cfgParam_t;

typedef struct cfgParamConfig_s cfgParamConfig_t;
typedef void (*cfgParamValidate_t)(const cfgParamConfig_t * const cfg, cfgParam_t * const param);
typedef bool (*cfgParamPrintInput_t)(wcontext_t *wctx,const cfgParamConfig_t * const cfg, const char *id, const char *name,int idx,const char *value);

bool printInputText(wcontext_t *wctx,const cfgParamConfig_t * const cfg, const char *id, const char *name,int idx,const char *value);
bool printInputPassword(wcontext_t *wctx,const cfgParamConfig_t * const cfg, const char *id, const char *name,int idx,const char *value);
bool printCheckbox(wcontext_t *wctx,const cfgParamConfig_t * const cfg, const char *id, const char *name,int idx,const char *value);
bool printSelect(wcontext_t *wctx,const cfgParamConfig_t * const cfg, const char *id, const char *name,int idx,const char *value);

struct cfgParamConfig_s {
	const char *name;         // Name of the param
	const char *defaultValue; // defaultValue if needed
	const char *options;      // if multiple choice enter the options here, seperated by |
	bool isIndex; //value is index in options
	int indexOffset;
	cfgParamValidate_t validate; //function to validate a param
	cfgParamPrintInput_t printInput; //function to print an input for the param in a form
	int size;
	bool alreadySet;
};

typedef struct cfgParamConfigList_s {
	bool exclusive; //Ignore params not listed
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
	bool isVdr;  //VDR is running in host
	int  socket;
} hostConf_t;

typedef struct webifConf_s {
	bool alreadySet;
	time_t mtime; //st_mtime of conf file in last read
	int langId;
	hostConf_t hosts[MAXHOSTS];  //TODO hacer dinamico
	int hostsLength;
	int numVDRs;
	playlistType_t playlistType;
	bool deletionDisabled;
	bool configChangeDisabled;
	bool configViewDisabled;
	bool useExternalWwwFolder;
	bool displayHostId; //display host id in recordings
	bool printRecFolderSummary;
	int maxDepth; //maximum directory depth while browsing
	bool alwaysCloseSvdrp;  //always close SVDRP connection after each command
	int defaultMarginStart;
	int defaultMarginStop;
	int videoWidth;
	int videoHeight;
	int tvScheduleGridWidth;
	int channelLogoWidth;
	char *www;
	bool noLogos;
	char user[30];
	char password[30];
} webifConf_t;

#define CHAN_LOGO_W ((webifConf.noLogos)?80:webifConf.channelLogoWidth)

extern webifConf_t webifConf;
extern const cfgFile_t const cfgFile[];
extern const int cfgFileLength;
extern const cfgFile_t fileMapping[];
extern const int fileMappingLength;
extern char* commandsFile;


const cfgParamConfig_t *getCfgParamConfig(cfgFileId_t cfgFileId, cfgParam_t * const param);
bool readConf(cfgFileId_t cfgFileId, cfgParamList_t * const params, bool *isNew);
bool writeConf(cfgFileId_t cfgFileId, cfgParamList_t * const params);
bool readWebifConf();
void freeWebifConf();
hostConf_t *getHost(int hostId);
hostConf_t *getFirstVdrHost();
bool isHostLocal(hostConf_t *host);
const char *getHostHttpAddr(hostConf_t *host,wcontext_t *wctx);
void printVDRSelect(wcontext_t *wctx,const char * name,const int hostId);

#endif

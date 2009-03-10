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
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

//TODO Integrar esto con i18n!!!

#ifndef __CONF_H__
#define __CONF_H__

#define WEBIFCONF  "/etc/webif.conf"
#define BOXAMPCONF "/etc/boxamp.conf"
#define VDRCONF    "/etc/vdr/setup.conf"

#include "misc.h"

typedef enum confParamType_e {
	PT_STRING,
	PT_INTEGER,
	PT_IPADDRESS
} confParamType_t;

typedef struct confParam_s {
	const char *name;         // Name of the param
	const char *i18nKey;        // key to i18n
	const char *defaultValue; // defaultValue if needed
	const char *options;      // if multiple choice enter the options here, seperatet by |
	char *value;              // Value as read from the config file
	int minValue;       // Min and Max Value if value is a number,
	int maxValue;       // min and max length if value is a string, ignored otherwise.
	boolean_t notNull;        // 0 -> value may be NULL, 1 -> value should not be NULL, if no value avaible default value is uses, if no dfault value avaible, an error will be raised
	confParamType_t type;
	boolean_t readOK;         // allready read from conf file
	boolean_t writeOK;        // already written to conf file
} confParam_t;

typedef struct confFileProperties_s {
	const char *name;
	const char *longName;
	const char *fileName;
	const char *stringsEnclosedBy;
	int	 numOfParams;
	boolean_t  readOk;
	void *confData;
} confFileProperties_t;

typedef struct webifConf_s {
	int langID;
} webifConf_t;

typedef struct boxampConf_s {
	char path[256];
	char audioPath[256];
	char options[256];
	int autoTerm;
} boxampConf_t;

typedef struct fileMapping_s {
	const char *name;
	const char *file;
	const char *i18nKey;
} fileMapping_t;

extern webifConf_t webifConf;
extern boxampConf_t boxampConf;
extern const int numOfConfFiles;
extern confFileProperties_t confFileProperties[];
extern const int maxFileMapping;
extern fileMapping_t fileMapping[];


int readConf(const char *name);
int readWEBIFCONF();
int writeWEBIFCONF(webifConf_t *newConf );
int readBOXAMPCONF();

#endif

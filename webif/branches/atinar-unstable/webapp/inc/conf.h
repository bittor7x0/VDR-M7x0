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

static struct __confFile {
  const char *name;					// Name of the param
  const char *label;				// unique ID
  const char *defaultValue;	// defaultValue if needed
  const char *options;			// if multiple choice enter the options here, seperatet by |
  char *value;							// Value as read from the config file
  int minValue;							// Min and Max Value if value is a number,
  int maxValue;							// min and max length if value is a string, ignored otherwise
  int notNull;							// 0 -> value may be NULL, 1 -> value should not be NULL, if no value avaible default value is uses, if no dfault value avaible, an error will be raised
  int type;									// 0 -> String, 1 -> Integer, 2 -> IP-Adress
  int readOK;								// 0 -> not yet read from conf file, 1 -> read from conf file
  int writeOK;							// 0 -> not yet wrote to conf file, 1 -> already wrote to conf file
} __confFile;

static struct __confFileProperties {
	const char *name;
	const char *longName;
	const char *fileName;
	const char *stringsEnclosedBy;
	int	 numOfParams;
	int  readOk;
	void *confData;
}  _confFileProperties;

typedef struct _webifConf {
  int langID;
} _webifConf;

typedef struct _boxampConf {
  char path[256];
  char audioPath[256];
  char options[256];
  int autoTerm;
} _boxampConf;

extern _webifConf webifConf;
extern _boxampConf boxampConf;
extern const int numOfConfFiles;
extern struct __confFileProperties confFileProperties[];


int readConf(const char *name);
int readWEBIFCONF();
int writeWEBIFCONF(_webifConf * newConf);
int readBOXAMPCONF();

#endif

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "conf.h"

webifConf_t webifConf = {-1};
boxampConf_t boxampConf = {"","","",1};

confParam_t confFileNet[3] = {
	{"net",      "confNet",      "inet",            "inet|DHCP", NULL, 0, 4, BT_TRUE, PT_STRING,    BT_FALSE, BT_FALSE},
	{"ip",       "confIP",       "192.168.100.102", "",          NULL, 0, 0, BT_TRUE, PT_IPADDRESS, BT_FALSE, BT_FALSE},
	{"netmask",  "confNetmask",  "255.255.255.0",   "",          NULL, 0, 0, BT_TRUE, PT_IPADDRESS, BT_FALSE, BT_FALSE}
};

confParam_t confFileWebif[1] = {
	{"default language",  "defaultLang", "",	"English|Deutsch|Español|Français",	NULL, 0, 3, BT_FALSE, PT_INTEGER, BT_FALSE, BT_FALSE}
};

confParam_t confFileBoxamp[4] = {
	{"CONFIG_BOXAMP_PATH",        "baPath",    "", "", NULL, 0, 0, BT_TRUE, PT_STRING, BT_FALSE, BT_FALSE},
	{"CONFIG_BOXAMP_AUDIO_PATH",  "baAudio",   "", "", NULL, 0, 0, BT_TRUE, PT_STRING, BT_FALSE, BT_FALSE},
	{"CONFIG_BOXAMP_OPTIONS",     "baOptions", "", "", NULL, 0, 0, BT_TRUE, PT_STRING, BT_FALSE, BT_FALSE},
	{"CONFIG_BOXAMP_AUTO_TERM",   "baTerm",    "", "", NULL, 0, 0, BT_TRUE, PT_STRING, BT_FALSE, BT_FALSE}
};

const int numOfConfFiles=3;
confFileProperties_t confFileProperties[] = {
	{"webif",   "Webinterface", "/etc/webif.conf",  "", 1, BT_FALSE, confFileWebif},
	{"network", "rc.conf",      "/etc/rc.conf",     "", 3, BT_FALSE, confFileNet},
	{"boxamp",  "BoxAmp",       "/etc/boxamp.conf", "", 4, BT_FALSE, confFileBoxamp}
};

fileMapping_t fileMapping[] = {
	{"syslog", "/var/log/messages", "sysLog"},
	{"vdr", "/etc/vdr/setup.conf", "vdrSetup"},
	{"fstab", "/etc/fstab", "fstab"},
	{"rc", "/etc/rc.conf", "rc"},
	{"rclocal", "/etc/rc.local.conf", "rc.local"},
	{"bootmenu" "/etc/bootmenu/bootmenu.conf", "bootMenu"},
	{"svdrp", "/etc/vdr/svdrphosts.conf", "svdrpHosts"},
	{"channels", "/etc/vdr/channels.conf", "channels"},
	{"streamdev", "/etc/vdr/plugins/streamdevhosts.conf", "streamdevHosts"},
	{"boxamp", "/etc/bootmenu/bm.local.boxamp.conf", "baSetup"}
};
const int maxFileMapping=sizeof(fileMapping)/sizeof(fileMapping_t);


boolean_t readLineFromConf(FILE *f, char param[251], char value[251]) {
	char buffer[256];

	fgets(buffer,sizeof(buffer),f);
	strcpy(param,""); strcpy(value,"");
	if ((strlen(buffer)>1) && (buffer[0]!='#')) {
		if (buffer[strlen(buffer)-1]=='\n') { buffer[strlen(buffer)-1]='\0'; }
		int i=strcspn(buffer,"=");
		strncpy(param,buffer,i);
		param[i]='\0';
		while (param[strlen(param)-1]==' ') { param[strlen(param)-1]='\0'; }
		while (buffer[i+1]==' ') { i++; }
		strcpy(value,buffer+i+1);
		return BT_TRUE;
	} else {
		return BT_FALSE;
	}
}

int readConf(const char *name) {
	int i=0;
	int confFileID=0;
	for (confFileID=0;confFileID<numOfConfFiles;confFileID++) {
		if (!strcmp(name,confFileProperties[confFileID].name)) {
			confFileProperties[confFileID].readOk=0;
			FILE *f = fopen(confFileProperties[confFileID].fileName,"r");
			confParam_t * confData=confFileProperties[confFileID].confData;
			if (f) {
				while (!feof(f)) {
					char param[251]="", value[251]="";
					if (readLineFromConf(f,param,value)) {
						for	(i=0;i<confFileProperties[confFileID].numOfParams;i++) {
							if ((!strcmp(param,confData[i].name )) && (confData[i].readOK!=1)) {
								confData[i].value=strdup(value);
							}
						}
					}
				}
				fclose(f);
				confFileProperties[confFileID].readOk=1;
				return confFileID;
			}
			return confFileID;
		}
	}
	return -1;
}

int readWEBIFCONF() {
	FILE *f = fopen(WEBIFCONF,"r");
	if (f) {
		while (!feof(f)) {
			char param[251], value[251];
			if (readLineFromConf(f,param,value)) {
				if (!strcmp(param,"default language")) {
					webifConf.langID=strtol(value,NULL,10);
				} /* else if (!strcmp(param,"MarginStop ")) {
				margin_stop=strtol(buffer+i+2,NULL,10)*60;
				} */
			}  
		}
		fclose(f);
		return 0;
	} else {
		return 1;  
	}
}

int writeWEBIFCONF(webifConf_t *newConf) {
	FILE *f = fopen(WEBIFCONF,"w");
	if (f) {
		fprintf(f,"default language = %d\n",newConf->langID);
		fclose(f);
		return 0;
	} else {
		return 1;
	}
}

int readBOXAMPCONF() {
	FILE *f = fopen(BOXAMPCONF,"r");
	if (f) {
		while (!feof(f)) {
			char param[251], value[251];
			if (readLineFromConf(f,param,value)) {
				if (!strcmp(param,"CONFIG_BOXAMP_PATH ")) {
					strcpy(boxampConf.path,value);
				} else if (!strcmp(param,"CONFIG_BOXAMP_AUDIO_PATH ")) {
					strcpy(boxampConf.audioPath,value);
				} else if (!strcmp(param,"CONFIG_BOXAMP_OPTIONS ")) {
					strcpy(boxampConf.options,value);
				} else if (!strcmp(param,"CONFIG_BOXAMP_AUTO_TERM ")) {
					boxampConf.autoTerm=strtol(value,NULL,10);
				}
			}
		}
		fclose(f);
		return 0;
	} else {
		return 1;  
	}
}

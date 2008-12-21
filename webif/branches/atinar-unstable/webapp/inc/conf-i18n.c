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

#include <string.h>

#include "conf-i18n.h"

const char confLang[3][3]=
		{	"--",
			"en",
			"de"
		};

const int confNumOfPhrases=4;
const tConfI18nPhrase confShort[]={
		{	"ifLang",
			"default language",
			"bevorzugte Sprache"
		},
		{	"netNet",
			"manual (inet) or automatic (DHCP) network configuration",
			"manuelle (inet) oder automatische (DHCP) Netzwerkkonfiguration"
		},
		{	"netIP",
			"IP address (if manual configuration)",
			"IP-Adresse (bei manueller Konfiguration)"
		},
		{	"netNetmask",
			"Subnet mask (if manual configuration)",
			"Subnetzmaske (bei manueller Konfiguration)"
		}
};

const char *trConfShort(const char *lang, const char *s) {
int i=0;
int confLangID=0;
  for (i=1;i<=CONFI18NNUM;i++) {
  	if (strcmp(lang,confLang[i])==0) {
  		confLangID=i;
  	}
  }

  if ((confLangID<1) || (confLangID>CONFI18NNUM)) {
    confLangID=1;
  }

  for (i=0;i<confNumOfPhrases;i++) {
    if (strcmp(s,confShort[i][0])==0) {
      return confShort[i][confLangID];
    }
  }
  return s;
}

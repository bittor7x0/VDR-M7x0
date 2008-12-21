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

#ifndef __CONF_I18N_H__
#define __CONF_I18N_H__

#define CONFI18NNUM 2

extern const char confLang[3][3];

typedef char *tConfI18nPhrase[CONFI18NNUM+1];
extern const tConfI18nPhrase confShort[];

const char *trConfShort(const char *lang, const char *s);

#endif

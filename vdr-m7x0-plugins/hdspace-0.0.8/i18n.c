/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 */

#include "i18n.h"
const tI18nPhrase Phrases[] = {

  { "Send e-mail when HD Space gets low",// English
    "Überprüft den freien Aufnahmeplatz, sendt mail wenn unter limit", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Enviar correo si el espacio en disco baja del límite",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Check interval [min]",// English
    "Interval zwischen den Prüfungen [min]", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Intervalo de comprobación [min]",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Mail from name",// English
    "Mailabsender", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Correo remitente",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Mail sendTo names",// English
    "Mailempfänger", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Correo destinatarios",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Mail server name",// English
    "Mailserver", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Servidor de correo",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Display warning on TV",// English
    "Warnung auf dem TV anzeigen", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Mostrar aviso en TV",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Yes",// English
    "Ja", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Sí",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "No",// English
    "Nein", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "No",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Min free hd space [GB]",// English
    "Mindest freier Festplattenplatz [GB]", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Mín. espacio libre en disco [GB]",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Recording space is %i%%(%iGB). Limit:%iGB",// English
    "Freie Aufnahmeplatz %i%%(%iGB). Limit:%iGB", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Espacio de grabación:%i%%(%iGB). Límite:%iGB",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Recording space OK. Limit:%iGB Now:%iGB",// English
    "Aufnahmeplatz OK. Limit:%iGB Jetzt:%iGB", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Espacio de grabación OK. Límite:%iGB Ahora:%iGB",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { NULL }
  };

//Mail text when the recording space is below the limit 
//**************** IMPORTANT ************************
//Do not change the order or remove any '%i' or '%s'
//when translating into new languages
//**************** IMPORTANT ************************
const tI18nPhrase MailLow[] = {
	//Subject:
  { "-Warning- Recording space is %i%%@%s, %iGB free.\n",// English
    "-Warnung- Aufnameplatz ist %i%%@%s, %iGB frei.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "-Aviso- El espacio de grabación es %i%%@%s, %iGB libre.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 1
  { "The free space on all harddisk and/or network drives,\n",// English
    "Der freie Speicher auf den Festplatten und/oder Netzwerklaufwerken,\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "El espacio libre en todos los discos y/o unidades de red,\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 2
  { "on wich the recordings of the %s are stored,\n",// English
    "auf denen die Aufnahmen für die %s gespeichert werden,\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "en el que se almacenan las grabaciones del %s,\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 3
  { "are below the lower level of %iGB.\n",// English
    "ist unter den Schwellwert von %iGB gefallen.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "son inferiores al nivel más bajo de %iGB.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 4
  { "The current value is %iGB.\n",// English
    "Aktuell liegt der Wert bei %iGB.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "El valor actual es %iGB.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 5
  { "The recording of one hour needs, in depandence from the station\n",// English
    "Für die Aufnahme einer Stunde werden je nach Sender und Sendung\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "El espacio necesario para grabar una hora, depende del canal emisor\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 6
  { "and the brodcarst, between 0.8GB and 1.5GB. With a free space of %iGB\n",// English
    "zwischen 0,8GB bis 1,5GB benötigt. Bei dem freien Speicher von %iGB\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "y está entre 0.8GB y 1.5GB. Con un espacio libre de %iGB\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 7
  { "there is space left for a total recording length\n",// English
    "bedeutet dieses, dass noch Platz für Aufnahmen mit einer Gesamtlänge\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "hay espacio suficiente para hacer una grabación que dure\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 8
  { "between %i and %i hours.\n",// English
    "zwischen %i und %i Stunden vorhanden ist.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "entre %i y %i horas.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { NULL }
  };

//Mail text when the recording space is over the limit 
//for the first time.
//**************** IMPORTANT ************************
//Do not change the order or remove any '%i' or '%s'
//when translating into new languages
//**************** IMPORTANT ************************
const tI18nPhrase MailOK[] = {
	//Subject:
  { "-OK- Recordingspace is %i%%@%s, %iGB free.\n",// English
    "-OK- Aufnameplatz ist %i%%@%s, %iGB frei.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "-OK- El espacio de grabación es %i%%@%s, %iGB libre.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 1
  { "The free space on all harddisk and/or network drives,\n",// English
    "Der freie Speicher auf den Festplatten und/oder Netzwerklaufwerken,\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "El espacio libre en todos los discos y/o unidades de red,\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 2
  { "on wich the recordings of the %s are stored,\n",// English
    "auf denen die Aufnahmen für die %s gespeichert werden,\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "en el que se almacenan las grabaciones del %s,\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 3
  { "is above the level of %iGB again.\n",// English
    "ist über den Schwellwert von %iGB.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "es otra vez superior al nivel de %iGB.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
	  //Line 4
  { "The current free space is %iGB.\n",// English
    "Aktuell liegt der Wert bei %iGB.\n", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "El espacio libre actual es %iGB.\n",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { NULL }
  };


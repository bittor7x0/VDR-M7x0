/*
 * WAP remote control plugin for VDR
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */


#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Remote control by WAP",
    "WAP Fernbedienung",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "WAP t�l�commande",
    "", // TODO
    "VDR:n kauko-ohjaus (WAP)",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Save",
    "Speichern",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Enregistrer",
    "", // TODO
    "Tallenna",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "no title",
    "kein Titel",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "pas de titre",
    "", // TODO
    "ei nime�",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "no subtitle",
    "kein Kurztitel",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "pas de abr�g�",
    "", // TODO
    "ei alaotsikkoa",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "no description",
    "keine Beschreibung",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "pas de description",
    "", // TODO
    "ei kuvausta",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "No description available.",
    "Keine Beschreibung verf�gbar.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Pas de r�sum� defini.",
    "", // TODO
    "Yhteenvetoa ei saatavilla.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Error in recording ID \"%s\".",
    "Fehlerhafte Aufnahme-ID \"%s\".",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Incorrect enregistrement ID \"%s\".",
    "", // TODO
    "Virhe tallenteen \"%s\" ID:ss�.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Missing recording id.",
    "Aufnahme-ID fehlt.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Enregistrement ID manque.",
    "", // TODO
    "Tallenteen ID puuttuu.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Event \"%s\" not found.",
    "Sendung \"%s\" nicht gefunden.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Ev�nement \"%d\" non trouv�.",
    "", // TODO
    "Tapahtumaa \"%s\" ei l�ydy.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Error in event ID \"%s\".",
    "Fehlerhafte Sendungs-ID \"%s\".",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Incorrect ev�nement ID \"%d\".",
    "", // TODO
    "Virhe tapahtuman \"%s\" ID:ss�.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "No schedule available.",
    "Kein Programm verf�gbar.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Pas de programmes d�finis.",
    "", // TODO
    "Ohjelmistoa ei saatavilla.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Channel \"%d\" not defined.",
    "Kanal \"%d\" undefiniert.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Cha�ne \"%d\" non d�finie.",
    "", // TODO
    "Kanavaa \"%s\" ei m��ritelty.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Can't get EPG data.",
    "Kann EPG Daten nicht lesen.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Impossible de lire le guide des programmes.",
    "", // TODO
    "EPG-tieto ei saatavilla.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Channel \"%d\" not found.",
    "Kanal \"%d\" nicht gefunden.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Cha�ne \"%d\" non trouv�.",
    "", // TODO
    "Kanavaa \"%s\" ei l�ydy.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "No channels defined.",
    "Keine Kan�le definiert.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Pas de cha�nes d�finies.",
    "", // TODO
    "Kanavia ei m��ritelty.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Timer \"%d\" not found.",
    "Timer \"%d\" nicht gefunden.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Programmation \"%d\" non trouv�.",
    "", // TODO
    "Ajastinta \"%s\" ei l�ydy.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "No timers defined.",
    "Keine Timer definiert.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Pas de programmations d�finies.",
    "", // TODO
    "Ajastimia ei m��ritelty.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "No recordings available.",
    "Keine Aufnahmen verf�gbar.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Pas de enregistrements disponibles.",
    "", // TODO
    "Tallenteita ei ole saatavilla.",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Parameters",
    "Parameter",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Param�tres",
    "", // TODO
    "Parametrit",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Apid",
    "Apid",
    "Apid",
    "Apid",
    "Apid",
    "Apid",
    "PID Audio",
    "Apid",
    "��ni PID",
    "Apid",
    "Apid",
    "Apid",
    "Audio PID",
    "PID Audio",
    "Apid",
    "PID �udio",
    "Apid (�����)",
    "Apid", // hrv
    "Apid",
    "Apid",
  },
  { "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Verkko ID",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid",
    "Nid", // hrv
    "Nid", 
    "Nid", 
  },
  { "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "L�hete ID",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid",
    "Tid", // hrv
    "Tid",
    "Tid",
  },
  { "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Radio ID",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid",
    "Rid", // hrv
    "Rid",
    "Rid",
  },
  { NULL }
  };


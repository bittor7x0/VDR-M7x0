/*
 * A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "display the current recordings on each device",
    "Anzeige der laufenden Aufnahmen je Ger�t",// Deutsch
    "",// TODO Slovenski
    "",
    "",// TODO Nederlands
    "",// TODO Portugues
    "Afficher les enregistrements pour chaque carte",
    "",// TODO Norsk
    "",
    "",// TODO Polski
    "Ver grabaciones actuales de cada dispositivo",// Espa�ol
    "",// TODO �������� (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
    "",// TODO ������� (Russian)
    "",// TODO Hrvatski (Croatian)
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "This plugin has no info text!",
    "Dieses Plugin hat keinen Infotext!",
    "",// TODO
    "Questo plugin non ha ulteriori informazioni",// TODO
    "",// TODO
    "",// TODO
    "Le plugin n'a pas de text d'info",
    "",// TODO
    "Laajennoksella ei ole infoteksti�!",
    "",// TODO
    "�Este plugin no tiene informaci�n!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "Recording status",
    "Aufnahmestatus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Status d'enregistrement",
    "",// TODO
    "",
    "",// TODO
    "Estado de la grabaci�n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "Device",
    "Ger�t",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Carte",
    "",// TODO
    "",
    "",// TODO
    "Dispositivo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "no current recordings",
    "keine laufenden Aufnahmen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pas d'enregistrement actuellement",
    "",// TODO
    "",
    "",// TODO
    "actualmente no hay grabaciones",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "device with decoder",
    "Ger�t hat Decoder",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La carte a un d�codeur",
    "",// TODO
    "",
    "",// TODO
    "Decodificador",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "primary device",
    "Prim�rger�t",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Carte primaire",
    "",// TODO
    "",
    "",// TODO
    "Dispositivo principal",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { NULL }
  };

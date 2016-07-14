/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id:
 */

#include "i18n.h"

 const tI18nPhrase Phrases[] = {
  { "Search Transponders for DVB Channels", //EN
    "Durchsucht Transponder nach DVB Kan�len.",// DE
    "", // Slovenski
    "",// IT
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "",// FI
    "", // Polski
    "Buscar transponders de canales DVB", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Search Mode",
    "Suchmodus",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Modo b�squeda",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "scanning on transponder",
    "Suche auf Transponder",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "buscando transponder",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#ifdef M750S
  { "Scanning configured satellites",
    "Durchsuche eingerichtete Satelliten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando sat�lites configurados",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "DiSEqC",
    "DiSEqC",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DiSEqC",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#endif
  { "FEC",
    "FEC",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "FEC",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "None",
    "Keine",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ninguno",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Auto",
    "Auto",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Auto",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "SearchMode$Auto",
    "Automatisch",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Autom�tica",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Manual",
    "Manuell",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Manual",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channelscan",
    "Kanalsuche",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscar canales",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Detailed search",
    "Detaillierte Suche",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "B�sq. detallada",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Position",
    "Position",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Posici�n",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Tuner Error",
    "Tuner Fehler",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Error en sintonizador",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning on transponder",
    "Durchsuche Transponder",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando transponder",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "channels in current list",
    "Kan�le in aktueller Liste",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "canales en lista actual",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "TV CHANNELS",
    "TV KAN�LE",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "CANALES TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "No new channels found",
    "Keine neuen Kan�le gefunden",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ning�n nuevo canal encontrado",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning aborted by User",
    "Suche abgebrochen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "B�squeda cancelada por el usuario",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },

  { "TV CHANNELS                     RADIO",
    "TV KAN�LE                       RADIO",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "CANALES TV                      RADIO",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Symbolrate",
    "Symbolrate",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "S�mbolo tipo",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Frequency",
    "Frequenz",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Frequencia",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s\tPlease Wait",
    "Durchsuche %s\tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s\tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%iMHz) \tPlease Wait",
    "Durchsuche %s (%iMHz) \tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s (%iMHz) \tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%.3fMHz) \tPlease Wait",
    "Durchsuche %s (%.3fMHz) \tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s (%.3fMHz) \tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Change",
    "Ausw�hlen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambiar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Start",
    "Start",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Iniciar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Backup",
    "Sichern",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Guardar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Delete",
    "L�schen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Borrar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$New",
    "Neu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Nuevo",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Radio only",
    "nur Radio",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "S�lo radio",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "TV only",
    "nur TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "S�lo TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Radio + TV",
    "Radio + TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Radio + TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Service Type",
    "Service Arten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tipo de servicio",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "enabled",
    "aktiviert",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "activado",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "disabled",
    "deaktiviert",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "desactivado",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Added new channels",
    "Neue Kan�le hinzugef�gt",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Nuevos canales a�adidos",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Frequency (kHz)",
    "Frequenz (kHz)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Frecuencia (kHz)",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Frequency (MHz)",
    "Frequenz (MHz)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Frecuencia (MHz)",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Expert",                                                                                               
    "Experten",                                                                                             
    "",                                                                                                     
    "",                                                                                              
    "",                                                                                               
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "Experto",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channel sel.",                                                                                               
    "Kanalliste",                                                                                             
    "",                                                                                                     
    "",                                                                                              
    "",                                                                                               
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "Lista canales",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                              
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#ifdef M750S
  { "DVB-S - Satellite",
    "DVB-S - Satellit",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DVB-S - Sat�lite",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "DVB-C - Cable",
    "DVB-C - Kabel",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DVB-C - Cable",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#endif
  { "DVB-T - Terrestrial",
    "DVB-T - Terrestrisch",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DVB-T - Terrestre",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Tuner",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sintonizador",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Press OK to proceede",
    "Dr�cken Sie OK um fortzufahren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Pulse OK para continuar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Press OK to finish or Exit for new scan",
    "Dr�cken Sie OK zum Beenden oder Exit f�r eine neue Kanalsuche.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Pulse OK para terminar o Exit para escanear otra vez",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Standard",
    "Standard",
    "",
    "", // Italiano
    "",
    "",
    "Standart",
    "",
    "",
    "",
    "Est�ndar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Extended",
    "Erweitert",
    "",
    "",
    "",
    "",
    "Pr�cision",
    "",
    "",
    "",
    "Ampliado",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Retrieving transponder list from %s",
    "Erhalte Transponderliste von %s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Recibiendo lista de transponders desde %s",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Terrestrial",
    "Terr.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Terrestre",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#ifdef M750S
  { "Cable",
    "Kabel",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cable",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#endif
  { "Running services on transponder: %i / %i",
    "Aktive Dienste auf Transponder: %i / %i",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ejecutando servicios en transponder: %i / %i",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%iMHz)\t%s",
    "Durchsuche %s (%iMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s (%iMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%.3fMHz)\t%s",
    "Durchsuche %s (%.3fMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s (%.3fMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Changes Done",
    "�nderung �bernommen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambios realizados",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Changes failed",
    "�nderung fehlgeschlagen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambios err�neos",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channel lists",
    "Kanallisten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Listas de canales",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Please check your Timers!",
    "Bitte Timer �berpr�fen!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "�Compruebe sus programaciones!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { NULL }
  };


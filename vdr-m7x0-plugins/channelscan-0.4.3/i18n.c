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
    "Durchsucht Transponder nach DVB Kanälen.",// DE
    "", // Slovenski
    "",// IT
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "",// FI
    "", // Polski
    "Buscar transponders de canales DVB", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "Modo búsqueda",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "Buscando satélites configurados",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "Automática",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "Búsq. detallada",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "Posición",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "channels in current list",
    "Kanäle in aktueller Liste",
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
    "", // Èesky (Czech)
  },
  { "TV CHANNELS",
    "TV KANÄLE",
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
    "", // Èesky (Czech)
  },
  { "No new channels found",
    "Keine neuen Kanäle gefunden",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ningún nuevo canal encontrado",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "Búsqueda cancelada por el usuario",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },

  { "TV CHANNELS                     RADIO",
    "TV KANÄLE                       RADIO",
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
    "", // Èesky (Czech)
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
    "Símbolo tipo",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "Button$Change",
    "Auswählen",
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "Button$Delete",
    "Löschen",
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "Sólo radio",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "Sólo TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "Added new channels",
    "Neue Kanäle hinzugefügt",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Nuevos canales añadidos",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "DVB-S - Satélite",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "Press OK to proceede",
    "Drücken Sie OK um fortzufahren",
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
    "", // Èesky (Czech)
  },
  { "Press OK to finish or Exit for new scan",
    "Drücken Sie OK zum Beenden oder Exit für eine neue Kanalsuche.",
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
    "", // Èesky (Czech)
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
    "Estándar",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Extended",
    "Erweitert",
    "",
    "",
    "",
    "",
    "Précision",
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "Changes Done",
    "Änderung übernommen",
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
    "", // Èesky (Czech)
  },
  { "Changes failed",
    "Änderung fehlgeschlagen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambios erróneos",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
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
    "", // Èesky (Czech)
  },
  { "Please check your Timers!",
    "Bitte Timer überprüfen!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "¡Compruebe sus programaciones!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { NULL }
  };


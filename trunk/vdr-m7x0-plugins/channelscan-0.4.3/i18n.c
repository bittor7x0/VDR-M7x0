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
    "Durchsucht Transponder nach DVB Kan‰len.",// DE
    "", // Slovenski
    "Ricerca Transponders di Canali DVB",// IT
    "Doorzoek Transponders naar DVB kanalen", // Nederlands
    "", // PortuguÍs
    "", // FranÁais
    "", // Norsk
    "Kanavahaku DVB-transpondereille",// FI
    "", // Polski
    "Buscar transponders de canales DVB", // EspaÒol
    "", // ≈ÎÎÁÌÈÍ‹ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal‡
    "øﬁÿ·⁄ ⁄–›–€ﬁ“", // ¿„··⁄ÿŸ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Search Mode",
    "Suchmodus",
    "",
    "Modalita' di Ricerca",
    "Zoek mode",
    "",
    "",
    "",
    "Hakutapa",
    "",
    "Modo b˙squeda",
    "",
    "",
    "",
    "",
    "",
    "¿’÷ÿ‹ ﬂﬁÿ·⁄–",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "scanning on transponder",
    "Suche auf Transponder",
    "",
    "Ricerca trasponder",
    "Scan op transponder",
    "",
    "",
    "",
    "haetaan transponderilta",
    "",
    "buscando transponder",
    "",
    "",
    "",
    "",
    "",
    "·⁄–›ÿ‡ﬁ“–›ÿ’ ‚‡–›·ﬂﬁ›‘’‡–",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "Buscando satÈlites configurados",
    "",
    "",
    "",
    "",
    "",
    "¡⁄–›ÿ‡ﬁ“–›ÿ’ ›–·‚‡ﬁ’››ÎÂ ·ﬂ„‚›ÿ⁄ﬁ“",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "DiSEqC",
    "DiSEqC",
    "DiSEqC",
    "DiSEqC",
    "DiSEqC",
    "",
    "",
    "",
    "DiSEqC-kytkin",
    "",
    "DiSEqC",
    "",
    "",
    "",
    "",
    "",
    "DiSEqC",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
#endif
  { "FEC",
    "FEC",
    "",
    "FEC",
    "FEC",
    "",
    "",
    "",
    "FEC",
    "",
    "FEC",
    "",
    "",
    "",
    "",
    "",
    "FEC",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "None",
    "Keine",
    "",
    "Nessun",
    "Geen",
    "",
    "",
    "",
    "ei",
    "",
    "Ninguno",
    "",
    "",
    "",
    "",
    "",
    "Ω’‚",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Auto",
    "Auto",
    "",
    "Auto",
    "Auto",
    "",
    "",
    "",
    "auto",
    "",
    "Auto",
    "",
    "",
    "",
    "",
    "",
    "∞“‚ﬁ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "SearchMode$Auto",
    "Automatisch",
    "",
    "Ricerca automatica",
    "Automatisch",
    "",
    "",
    "",
    "automaattinen",
    "",
    "Autom·tica",
    "",
    "",
    "",
    "",
    "",
    "∞“‚ﬁ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Manual",
    "Manuell",
    "",
    "Manuale",
    "Handmatig",
    "",
    "",
    "",
    "manuaalinen",
    "",
    "Manual",
    "",
    "",
    "",
    "",
    "",
    "¿„Á›ﬁŸ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Channelscan",
    "Kanalsuche",
    "",
    "Scansione canali",
    "Kanaal scan",
    "",
    "",
    "",
    "Kanavahaku",
    "",
    "Buscar canales",
    "",
    "",
    "",
    "",
    "",
    "øﬁÿ·⁄ ⁄–›–€ﬁ“",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "B˙sq. detallada",
    "",
    "",
    "",
    "",
    "",
    "øﬁ€›ÎŸ ﬂﬁÿ·⁄",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Position",
    "Position",
    "",
    "Posizione",
    "Positie",
    "",
    "",
    "",
    "Sijainti",
    "",
    "PosiciÛn",
    "",
    "",
    "",
    "",
    "",
    "øﬁ◊ÿÊÿÔ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "æËÿ—⁄– ‚Ó›’‡–",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Scanning on transponder",
    "Durchsuche Transponder",
    "",
    "Ricerca Transponders",
    "Scannen op transponder",
    "",
    "",
    "",
    "Haetaan transponderilta",
    "",
    "Buscando transponder",
    "",
    "",
    "",
    "",
    "",
    "¡⁄–›ÿ‡ﬁ“–›ÿ’ ‚‡–›·ﬂﬁ›‘’‡–",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "channels in current list",
    "Kan‰le in aktueller Liste",
    "",
    "Canali presenti nella lista",
    "Kanalen in huidige lijst",
    "",
    "",
    "",
    "T‰m‰nhetkiset kanavat",
    "",
    "canales en lista actual",
    "",
    "",
    "",
    "",
    "",
    "⁄–›–€ﬁ“ “ ·ﬂÿ·⁄’",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "TV CHANNELS",
    "TV KANƒLE",
    "",
    "CANALI TV",
    "TV KANALEN",
    "",
    "",
    "",
    "TV-kanavat",
    "",
    "CANALES TV",
    "",
    "",
    "",
    "",
    "",
    "∫∞Ω∞ªÀ ¬≤",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "No new channels found",
    "Keine neuen Kan‰le gefunden",
    "",
    "Non sono stati trovati nuovi canali",
    "Geen nieuwe kanalen gevonden",
    "",
    "",
    "",
    "Uusia kanavia ei lˆydetty",
    "",
    "Ning˙n nuevo canal encontrado",
    "",
    "",
    "",
    "",
    "",
    "Ω’‚ ›ﬁ“ÎÂ ⁄–›–€ﬁ“",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Scanning aborted by User",
    "Suche abgebrochen",
    "",
    "Ricerca interrotta dall'Utente",
    "Scannen afgebroken door User",
    "",
    "",
    "",
    "Haku keskeytetty",
    "",
    "B˙squeda cancelada por el usuario",
    "",
    "",
    "",
    "",
    "",
    "øﬁÿ·⁄ ﬂ‡’‡“–› ﬂﬁ€Ï◊ﬁ“–‚’€’‹",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },

  { "TV CHANNELS                     RADIO",
    "TV KANƒLE                       RADIO",
    "",
    "CANALI TV                       RADIO",
    "TV KANALEN                      RADIO",
    "",
    "",
    "",
    "TV-kanavat                      Radio",
    "",
    "CANALES TV                      RADIO",
    "",
    "",
    "",
    "",
    "",
    "∫∞Ω∞ªÀ ¬≤                       ¿∞¥∏æ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Symbolrate",
    "Symbolrate",
    "",
    "Symbolrate",
    "Symbolrate",
    "",
    "",
    "",
    "Symbolinopeus",
    "",
    "SÌmbolo tipo",
    "",
    "",
    "",
    "",
    "",
    "¡ÿ‹“.·⁄ﬁ‡ﬁ·‚Ï",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Frequency",
    "Frequenz",
    "",
    "Frequenza",
    "Frequentie",
    "",
    "",
    "",
    "Taajuus",
    "",
    "Frequencia",
    "",
    "",
    "",
    "",
    "",
    "«–·‚ﬁ‚–",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Scanning %s\tPlease Wait",
    "Durchsuche %s\tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Haku k‰ynniss‰ %s.\tOdota hetkinen...",
    "",
    "Buscando %s\tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "ø‡ﬁÿ◊“ﬁ‘ÿ‚·Ô ﬂﬁÿ·⁄\t∂‘ÿ‚’",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Scanning %s (%iMHz) \tPlease Wait",
    "Durchsuche %s (%iMHz) \tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Haku k‰ynniss‰ %s.\tOdota hetkinen...",
    "",
    "Buscando %s (%iMHz) \tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "øﬁÿ·⁄ %s (%iMHz) \t∂‘ÿ‚’",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Scanning %s (%.3fMHz) \tPlease Wait",
    "Durchsuche %s (%.3fMHz) \tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Haku k‰ynniss‰ %s.\tOdota hetkinen...",
    "",
    "Buscando %s (%.3fMHz) \tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "øﬁÿ·⁄ %s (%.3fMHz) \t∂‘ÿ‚’",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Button$Change",
    "Ausw‰hlen",
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
    "≤Î—‡–‚Ï",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Button$Start",
    "Start",
    "",
    "Start",
    "Start",
    "",
    "",
    "",
    "Aloita",
    "",
    "Iniciar",
    "",
    "",
    "",
    "",
    "",
    "ø„·⁄",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "¡ﬁÂ‡–›ÿ‚Ï",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Button$Delete",
    "Lˆschen",
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
    "√‘–€ÿ‚Ï",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "Ωﬁ“ÎŸ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Radio only",
    "nur Radio",
    "",
    "Solo radio",
    "Alleen Radio",
    "",
    "",
    "",
    "vain radio",
    "",
    "SÛlo radio",
    "",
    "",
    "",
    "",
    "",
    "¬ﬁ€Ï⁄ﬁ ‡–‘ÿﬁ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "TV only",
    "nur TV",
    "",
    "Solo TV",
    "Alleen TV",
    "",
    "",
    "",
    "vain TV",
    "",
    "SÛlo TV",
    "",
    "",
    "",
    "",
    "",
    "¬ﬁ€Ï⁄ﬁ ¬≤",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Radio + TV",
    "Radio + TV",
    "",
    "Radio + TV",
    "Radio + TV",
    "",
    "",
    "",
    "radio + TV",
    "",
    "Radio + TV",
    "",
    "",
    "",
    "",
    "",
    "¿–‘ÿﬁ + ¬≤",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Radio + TV + NVOD",
    "Radio + TV + NVOD",
    "",
    "Radio + TV + NVOD",
    "Radio + TV + NVOD",
    "",
    "",
    "",
    "radio + TV + NVOD",
    "",
    "Radio + TV + NVOD",
    "",
    "",
    "",
    "",
    "",
    "¿–‘ÿﬁ + ¬≤ + NVOD",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Service Type",
    "Service Arten",
    "",
    "Tipo servizio",
    "Service type",
    "",
    "",
    "",
    "Haettavat palvelut",
    "",
    "Tipo de servicio",
    "",
    "",
    "",
    "",
    "",
    "≤ÿ‘Î ·’‡“ÿ·ﬁ“",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "enabled",
    "aktiviert",
    "",
    "abilitato",
    "ingeschakeld",
    "",
    "",
    "",
    "p‰‰ll‰",
    "",
    "activado",
    "",
    "",
    "",
    "",
    "",
    "“⁄€ÓÁ’›",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "disabled",
    "deaktiviert",
    "",
    "disabilitato",
    "uitgeschakeld",
    "",
    "",
    "",
    "pois",
    "",
    "desactivado",
    "",
    "",
    "",
    "",
    "",
    "“Î⁄€ÓÁ’›",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Added new channels",
    "Neue Kan‰le hinzugef¸gt",
    "",
    "Aggiunti nuovi canali",
    "Nieuwe kanalen toegevoegd",
    "",
    "",
    "",
    "Uudet kanavat lis‰tty",
    "",
    "Nuevos canales aÒadidos",
    "",
    "",
    "",
    "",
    "",
    "¥ﬁ—–“€’›ﬁ ›ﬁ“ÎÂ ⁄–›–€ﬁ“",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "«–·‚ﬁ‚– (⁄≥Ê)",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "«–·‚ﬁ‚– (º≥Ê)",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Expert",                                                                                               
    "Experten",                                                                                             
    "",                                                                                                     
    "Esperto",                                                                                              
    "Expert",                                                                                               
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
    "Õ⁄·ﬂ’‡‚",                                                                                                     
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "¡ﬂÿ·ﬁ⁄ ⁄–›–€ﬁ“",                                                                                              
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "DVB-S - SatÈlite",
    "",
    "",
    "",
    "",
    "",
    "DVB-S - ¡ﬂ„‚›ÿ⁄",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "DVB-C - ∫–—’€Ï",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "DVB-T - Õ‰ÿ‡",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Press OK to proceede",
    "Dr¸cken Sie OK um fortzufahren",
    "",
    "Premere OK per continuare",
    "Druk OK om te vervolgen",
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
    "Ω–÷‹ÿ‚’ æ∫ Á‚ﬁ—Î ﬂ‡ﬁ‘ﬁ€÷ÿ‚Ï",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Press OK to finish or Exit for new scan",
    "Dr¸cken Sie OK zum Beenden oder Exit f¸r eine neue Kanalsuche.",
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
    "Ω–÷‹ÿ‚’ æ∫ Á‚ﬁ—Î ◊–⁄ﬁ›Áÿ‚Ï ÿ€ÿ ‘€Ô ›ﬁ“ﬁ”ﬁ ﬂﬁÿ·⁄–",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Standard",
    "Standard",
    "",
    "Standard", // Italiano
    "Standaard",
    "",
    "Standart",
    "",
    "Vakio",
    "",
    "Est·ndar",
    "",
    "",
    "",
    "",
    "",
    "¡‚–›‘–‡‚›ÎŸ",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Extended",
    "Erweitert",
    "",
    "",
    "",
    "",
    "PrÈcision",
    "",
    "Laaja",
    "",
    "Ampliado",
    "",
    "",
    "",
    "",
    "",
    "¿–·Ëÿ‡’››ÎŸ",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "øﬁ€„Á–Ó ·ﬂÿ·ﬁ⁄ ‚‡–›·ﬂﬁ›‘’‡ﬁ“ ﬁ‚ %s",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "Õ‰ÿ‡",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "∫–—’€Ï",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "∞⁄‚ÿ“›Î’ ·’‡“ÿ·Î ›– ‚‡–›·ﬂﬁ›‘’‡’: %i / %i",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "øﬁÿ·⁄ %s (%iMHz)\t%s",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "øﬁÿ·⁄ %s (%.3fMHz)\t%s",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Changes Done",
    "ƒnderung ¸bernommen",
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
    "∏◊‹’›’›ÿÔ ·ﬁÂ‡–›’›Î",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Changes failed",
    "ƒnderung fehlgeschlagen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambios errÛneos",
    "",
    "",
    "",
    "",
    "",
    "∏◊‹’›’›ÿÔ ›’ ·ﬁÂ‡–›’›Î",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
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
    "¡ﬂÿ·⁄ÿ ⁄–›–€ﬁ“",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { "Please check your Timers!",
    "Bitte Timer ¸berpr¸fen!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "°Compruebe sus programaciones!",
    "",
    "",
    "",
    "",
    "",
    "ø‡ﬁ“’‡Ï‚’ ﬂﬁ÷–€„Ÿ·‚– ‚–Ÿ‹’‡Î!",
    "",
    "", // Eesti
    "", // Dansk
    "", // »esky (Czech)
  },
  { NULL }
  };


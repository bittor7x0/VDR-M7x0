/*
 * i18n.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 *
 * Translations submitted by:
 *  - English:    Andreas Mair <andreas _@_ vdr-developer.org>
 *  - Deutsch:    Andreas Mair <andreas _@_ vdr-developer.org>
 *  - Italiano:   Gringo <vdr-italian _@_ tiscali.it>
 *  - Nederlands: dorpsgek @vdr-portal.de
 *  - Français:   Patrice Staudt <pat @vdr-portal.de>
 *  - Finnish:    Rolf Ahrenberg <rofafor @vdr-portal.de>
 *  - Polski:     ???
 *  - Español:    bittor from open7x0.org <bittor7x0 _at_ protonmail.com>
 *  - Svenska:    ???
 *  - Russian:    SergArb @vdr-portal.de / neptunvasja @vdr-portal.de
 *  - Eesti:      ???
 *  - Dansk:      ???
 *  - Czech:      ???
 *
 */

#include "common.h"
#include "i18n.h"

const tI18nPhrase Phrases[] = {
  {"en_US",             // English
   "de_DE",             // Deutsch
   "sl_SI",             // Slovenski
   "it_IT",             // Italiano
   "nl_NL",             // Nederlands
   "pt_PT",             // Português
   "fr_FR",             // Français  Patrice Staudt 18.05.2007
   "no_NO",             // Norsk
   "fi_FI",             // suomi (Finnish)
   "pl_PL",             // Polski
   "es_ES",             // Español
   "el_GR",             // ÅëëçíéêÜ (Greek)
   "sv_SE",             // Svenska
   "ro_RO",             // Românã
   "hu_HU",             // Magyar
   "ca_AD",             // Català
   "ru_RU",             // ÀãááÚØÙ (Russian)
   "hr_HR",             // Hrvatski
   "et_EE",             // Eesti
   "da_DK",             // Dansk
   "cs_CZ",             // Èesky (Czech)
   },
  {"Monday",       // English  Patrice Staudt 21.06.2007 Alle Sprache umgesetzt von LC_TIME
   "Montag",       // Deutsch
   "",   // Slovenski
   "",       // Italiano
   "",      // Nederlands
   "",      // Português
   "Lundi",        // Français
   "",       // Norsk
   "",    // suomi (Finnish)
   "", // Polski
   "Lunes",        // Español
   "",      // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",         // Românã
   "",        // Magyar
   "",      // Català
   "",  // ÀãááÚØÙ (Russian)
   "",  // Hrvatski
   "",    // Eesti
   "",       // Dansk
   "",      // Èesky (Czech)
   },
  {"Tuesday",      // English
   "Dienstag",     // Deutsch
   "",        // Slovenski
   "",      // Italiano
   "",      // Nederlands
   "",        // Português
   "Mardi",        // Français
   "",      // Norsk
   "",      // suomi (Finnish)
   "",       // Polski
   "Martes",       // Español
   "",        // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",        // Românã
   "",         // Magyar
   "",      // Català
   "",      // ÀãááÚØÙ (Russian)
   "",       // Hrvatski
   "",    // Eesti
   "",      // Dansk
   "",      // Èesky (Czech)
   },
  {"Wednesday",    // English
   "Mittwoch",     // Deutsch
   "",        // Slovenski
   "",    // Italiano
   "",     // Nederlands
   "",       // Português
   "Mercredi",     // Français
   "",       // Norsk
   "",  // suomi (Finnish)
   "",        // Polski
   "Miércoles",    // Español
   "",      // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",     // Românã
   "",       // Magyar
   "",     // Català
   "",        // ÀãááÚØÙ (Russian)
   "",      // Hrvatski
   "",    // Eesti
   "",       // Dansk
   "",      // Èesky (Czech)
   },
  {"Thursday",     // English
   "Donnerstag",   // Deutsch
   "",      // Slovenski
   "",      // Italiano
   "",    // Nederlands
   "",       // Português
   "Jeudi",        // Français
   "",      // Norsk
   "",      // suomi (Finnish)
   "",     // Polski
   "Jueves",       // Español
   "",       // ÅëëçíéêÜ (Greek)
   "",      // Svenska
   "",          // Românã
   "",    // Magyar
   "",       // Català
   "",      // ÀãááÚØÙ (Russian)
   "",     // Hrvatski
   "",    // Eesti
   "",      // Dansk
   "",      // Èesky (Czech)
   },
  {"Friday",       // English
   "Freitag",      // Deutsch
   "",        // Slovenski
   "",      // Italiano
   "",      // Nederlands
   "",        // Português
   "Vendredi",     // Français
   "",       // Norsk
   "",    // suomi (Finnish)
   "",       // Polski
   "Viernes",      // Español
   "",    // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",       // Românã
   "",       // Magyar
   "",    // Català
   "",      // ÀãááÚØÙ (Russian)
   "",        // Hrvatski
   "",        // Eesti
   "",       // Dansk
   "",      // Èesky (Czech)
   },
  {"Saturday",     // English
   "Samstag",      // Deutsch
   "",       // Slovenski
   "",       // Italiano
   "",     // Nederlands
   "",       // Português
   "Samedi",       // Français
   "",       // Norsk
   "",     // suomi (Finnish)
   "",       // Polski
   "Sábado",       // Español
   "",      // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",      // Românã
   "",      // Magyar
   "",     // Català
   "",      // ÀãááÚØÙ (Russian)
   "",       // Hrvatski
   "",      // Eesti
   "",       // Dansk
   "",      // Èesky (Czech)
   },
  {"Sunday",       // English
   "Sonntag",      // Deutsch
   "",      // Slovenski
   "",     // Italiano
   "",       // Nederlands
   "",      // Português
   "Dimanche",     // Français
   "",       // Norsk
   "",    // suomi (Finnish)
   "",    // Polski
   "Domingo",      // Español
   "",      // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",     // Românã
   "",     // Magyar
   "",     // Català
   "",  // ÀãááÚØÙ (Russian)
   "",     // Hrvatski
   "",     // Eesti
   "",       // Dansk
   "",      // Èesky (Czech)
   },
  {"EnigmaNG skin",             // English
   "EnigmaNG Oberfläche",       // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",             // Nederlands
   "",                          // Português
   "Skin EnigmaNG",             // Français
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",             // Polski
   "Piel EnigmaNG",             // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",            // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",             // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",             // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"EnigmaNG",                  // English
   "EnigmaNG",                  // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                  // Nederlands
   "",                          // Português
   "EnigmaNG",                  // Français
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",                  // Polski
   "EnigmaNG",                  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                  // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                  // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"min",                       // English
   "min",                       // Deutsch
   "",                          // Slovenski
   "",                       // Italiano
   "",                       // Nederlands
   "",                          // Português
   "min",                       // Français
   "",                          // Norsk
   "",                       // suomi (Finnish)
   "",                       // Polski
   "min",                       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                       // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                       // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                       // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Stereo",                    // English
   "Stereo",                    // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Português
   "Stéréo",                    // Français
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                    // Polski
   "Estéreo",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                    // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                    // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Left",                      // English
   "Links",                     // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                     // Nederlands
   "",                          // Português
   "Gauche",                    // Français
   "",                          // Norsk
   "",                     // suomi (Finnish)
   "",                    // Polski
   "Izquierda",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                   // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                     // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                     // Eesti
   "",                          // Danske
   "",      // Èesky (Czech)
   },
  {"Right",                     // English
   "Rechts",                    // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Português
   "Droite",                    // Français
   "",                          // Norsk
   "",                     // suomi (Finnish)
   "",                   // Polski
   "Derecha",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                     // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                // Nederlands
   "",                          // Português
   "Muet",                      // Français
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                    // Polski
   "Silenciar",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                   // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Volume",                    // English
   "Lautstärke",                // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Português
   "Volume",                    // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                  // Polski
   "Volumen",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",               // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show channel logos",        // English
   "Kanal-Logos anzeigen",      // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",         // Nederlands
   "",                          // Português
   "Afficher logos des chaînes", // Français
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",       // Polski
   "Mostrar los logos de los canales",  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",       // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",      // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Identify channel by",       // English
   "  Kanal-Identifikation durch",        // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",  // Nederlands
   "",                          // Português
   "  Identifier chaîne par",     // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",        // Polski
   "  Identificar el canal por",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",       // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "", // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                      // Nederlands
   "",                          // Português
   "Nom",                       // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                     // Polski
   "nombre",                    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                      // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                      // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"data",                      // English
   "ID",                        // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                      // Nederlands
   "",                          // Português
   "Donnée",                    // Français
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                 // Polski
   "datos",                     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                      // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                    // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Channel logo cache size",   // English
   "Größe des Logo-Caches",     // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",          // Nederlands
   "",                          // Português
   "Taille du cache des logos",  // Français
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",     // Polski
   "Tamaño de caché para los logos",   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",        // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",        // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Button$Flush cache",        // English
   "Cache leeren",              // Deutsch
   "",                          // Slovenski
   "",             // Italiano
   "",               // Nederlands
   "",                          // Português
   "Supprimer cache",           // Français
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",            // Polski
   "Limpiar caché",             // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",              // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                  // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Flushing channel logo cache...",    // English
   "Logo-Cache wird geleert...",        // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "", // Nederlands
   "",                          // Português
   "Suppression du cache...",   // Français
   "",                          // Norsk
   "",       // suomi (Finnish)
   "",    // Polski
   "Limpiando la caché...",     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",       // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Languages",                 // English
   "Sprachen",                  // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                     // Nederlands
   "",                          // Português
   "Langues",                   // Français
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                    // Polski
   "Idiomas",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                    // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Auxiliary information",     // English
   "Zusatzinformation",         // Deutsch
   "",                          // Slovenski
   "",    // Italiano
   "",           // Nederlands
   "",                          // Português
   "Information auxiliare",     // Français
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",     // Polski
   "Información adicional",     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                  // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show auxiliary information",        // English
   "Zusatzinfo anzeigen",       // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",      // Nederlands
   "",                          // Português
   "Afficher informations auxiliaires",   // Français
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",     // Polski
   "Mostrar información adicional",     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",        // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show remaining/elapsed time",          // English
   "Zeige abgel./restl. Zeit",             // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",  // Nederlands
   "",                          // Português
   "Afficher le temps écoulé/restant", // Français
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Mostrar tiempo restante/transcurrido",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",     // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"remaining",                 // English
   "restliche",                 // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                   // Nederlands
   "",                          // Português
   "restant",                   // Français
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",                          // Polski
   "restante",                  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"elapsed",                   // English
   "abgelaufene",               // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",                // Nederlands
   "",                          // Português
   "écoulé",                    // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "transcurrido",              // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show info area in main menu",            // English
   "Infobereich im Hauptmenü",               // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",         // Nederlands
   "",                          // Português
   "Surface d'infos dans le menu principal",  // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Mostrar info en menú principal",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show marker in lists",            // English
   "Symbol vor Listeneinträgen",      // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "", // Nederlands
   "",                          // Português
   "Marque devant les listes",  // Français
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Mostrar marcador en listas",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show progressbar",          // English
   "Fortschrittbalken anzeigen",        // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "",      // Nederlands
   "",                          // Português
   "Afficher barre de progression",     // Français
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",     // Polski
   "Mostrar barra de progreso", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",             // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",    // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show symbols",              // English
   "Symbole anzeigen",          // Deutsch
   "",                          // Slovenski
   "",            // Italiano
   "",            // Nederlands
   "",                          // Português
   "Afficher symbole",          // Français
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",           // Polski
   "Mostrar símbolos",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",             // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",       // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show VPS",                  // English
   "VPS anzeigen",              // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",                 // Nederlands
   "",                          // Português
   "Afficher le VPS",           // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "Mostrar VPS",               // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"WARNING",                   // English
   "WARNUNG",                   // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",              // Nederlands
   "",                          // Português
   "ATTENTION",                 // Français
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",                          // Polski
   "AVISO",                     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Timer conflict",            // English
   "Timerkonflikt",             // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",             // Nederlands
   "",                          // Português
   "Conflit de programmation",  // Français
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Conflicto de programación", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Timer conflicts",           // English
   "Timerkonflikte",            // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",                          // Nederlands
   "",                          // Português
   "Conflits de programmation", // Français
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",                          // Polski
   "Conflictos de programación",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",        // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"TIMERS",                    // English
   "TIMER",                     // Deutsch
   "",                          // Slovenski
   "",                     // Italiano
   "",                     // Nederlands
   "",                          // Português
   "PROGRAMMATION",             // Français
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "PROGRAMACIONES",            // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"RERUNS OF THIS SHOW",       // English
   "WIEDERHOLUNGEN",            // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",               // Nederlands
   "",                          // Português
   "REPETITION",                // Français
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "REPETICIONES",              // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"*** Invalid Channel ***",   // English
   "*** Ungültiger Kanal ***",  // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",   // Nederlands
   "",                          // Português
   "*** Chaîne non valable ***",  // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "*** Canal no válido ***",   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Size",                      // English
   "Größe",                     // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",                   // Nederlands
   "",                          // Português
   "Taille",                    // Français
   "",                          // Norsk
   "",                      // suomi (Finnish)
   "",                          // Polski
   "Tamaño",                    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                // Nederlands
   "",                      // Português
   "",                          // Français
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                          // Polski
   "Silenciar",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Volume",                    // English
   "Lautstärke",                // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Português
   "Volume",                    // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "Volumen",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show symbols in lists",     // English
   "Symbole in Listen",         // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",       // Nederlands
   "",                          // Português
   "Symboles dans les listes",   // Français
   "",                          // Norsk
   "",// suomi (Finnish)
   "",                          // Polski
   "Mostrar símbolos en listas",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#ifndef VDRO7OVERSION
  {"Try 8bpp single area",      // English
   "Einen 8bpp Bereich versuchen", // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",     // Nederlands
   "",                          // Português
   "Essayer une surface à 8bpp",// Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Probar un área de 8bpp",    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"Show flags",                // English
   "Fahnen anzeigen",           // Deutsch
   "",                          // Slovenski
   "",              // Italiano
   "",             // Nederlands
   "",                          // Português
   "Afficher drapeau",          // Français
   "",                          // Norsk
   "",               // suomi (Finnish)
   "",                          // Polski
   "Mostrar banderas",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",            // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show symbols in menu",      // English
   "Symbole im Menü",           // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",          // Nederlands
   "",                          // Português
   "Symboles dans le menu",     // Français
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Mostrar símbolos en menú",  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#if defined SKINENIGMA_HAVE_EPGSEARCH || defined USE_PLUGIN_MAILBOX
  {"never",                     // English
   "nie",                       // Deutsch
   "",                          // Slovenski
   "",                       // Italiano
   "",                     // Nederlands
   "",                          // Português
   "jamais",                    // Français
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                          // Polski
   "nunca",                     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"always",                    // English
   "immer",                     // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Português
   "toujours",                  // Français
   "",                          // Norsk
   "",                      // suomi (Finnish)
   "",                          // Polski
   "siempre",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"percent",                   // English
   "Prozent",                   // Deutsch
   "",                          // Slovenski
   "",               // Italiano
   "",                   // Nederlands
   "",                          // Português
   "Pourcentage",               // Français
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "porcentaje",                // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#ifdef SKINENIGMA_HAVE_EPGSEARCH
  {"if exists",                 // English
   "wenn vorhanden",            // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",          // Nederlands
   "",                          // Português
   "si existant",               // Français
   "",                          // Norsk
   "",              // suomi (Finnish)
   "",                          // Polski
   "si existe",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",            // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Number of Reruns",          // English
   "Anzahl Wiederholungen",     // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",        // Nederlands
   "",                          // Português
   "Nombre de répétitions",     // Français
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Número de repeticiones",    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",     // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Use Subtitle for reruns",   // English
   "Verw. Untertitel für Wdh.", // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "", // Nederlands
   "",                          // Português
   "Utilise sous-titres pour les répétitions", // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Usar subtítulo para repeticiones",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show timer conflicts",      // English
   "Timerkonflikte anzeigen",   // Deutsch
   "",                          // Slovenski
   "",    // Italiano
   "",       // Nederlands
   "",                          // Português
   "Afficher les conflits de programmation", // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Mostrar conflictos de programación",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"Timer check",               // English
   "Überwachung",               // Deutsch
   "",                          // Slovenski
   "",            // Italiano
   "",                 // Nederlands
   "",                          // Português
   "Observation de la programmation", // Français
   "",                          // Norsk
   "",       // suomi (Finnish)
   "",                          // Polski
   "Comprobar programación",    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",         // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#ifdef SKINENIGMA_HAVE_EPGSEARCH
  {"EPGSearch",                 // English
   "EPGSearch",                 // Deutsch
   "",                          // Slovenski
   "",               // Italiano
   "",                 // Nederlands
   "",                          // Português
   "Recherche EPG",             // Français
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "EPGSearch",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"Logos & Symbols",           // English
   "Logos & Symbole",           // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",         // Nederlands
   "",                          // Português
   "Logos et symboles",         // Français
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",                          // Polski
   "Logos y símbolos",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",        // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"use size.vdr only",         // English
   "nur size.vdr verwenden",    // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "", // Nederlands
   "",                          // Português
   "utiliser seulement size.vdr",  // Français Patrice Staudt 29.03.2007
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",                          // Polski
   "sólo usar size.vdr",        // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show recording's size",     // English
   "Aufnahmengröße anzeigen",   // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",       // Nederlands
   "",                          // Português
   "Afficher la taille des enregistrements",   // Français
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Mostrar tamaño de grabación",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#ifndef SKINENIGMA_NO_MENULOGO
  {"pixel algo",                // English
   "Pixel-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",            // Nederlands
   "",                          // Português
   "Algorithme pixel",          // Français
   "",                          // Norsk
   "",               // suomi (Finnish)
   "",                          // Polski
   "Algoritmo píxel",           // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"ratio algo",                // English
   "Ratio-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",            // Nederlands
   "",                          // Português
   "Algorithme ratio",          // Français
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",                          // Polski
   "Algoritmo ratio",           // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"zoom image",                // English
   "Zoomen",                    // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",                    // Nederlands
   "",                          // Português
   "Agrandir image",            // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Aumentar imagen",           // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Show event/recording images",     // English
   "Bilder in EPG/Aufnahmendetails",  // Deutsch
   "",                                // Slovenski
   "",      // Italiano
   "",         // Nederlands
   "",                                // Português
   "Images dans les EPG et détails d'enregistrements",     // Français
   "",                                // Norsk
   "",  // suomi (Finnish)
   "",                                // Polski
   "Mostrar imágenes grabación/suceso",// Español
   "",                                // ÅëëçíéêÜ (Greek)
   "",                                // Svenska
   "",                                // Românã
   "",                                // Magyar
   "",                                // Català
   "",               // ÀãááÚØÙ (Russian)
   "",                                // Hrvatski
   "",                                // Eesti
   "",                                // Dansk
   "",      // Èesky (Czech)
   },
#endif
#ifdef HAVE_IMAGEMAGICK
  {"  Resize images",           // English
   "  Bildberechnung benutzt",  // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",  // Nederlands
   "",                          // Português
   "  Redimensionne les images",  // Français
   "",                          // Norsk
   "",    // suomi (Finnish)
   "",                          // Polski
   "  Redimensionar imágenes",  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
#ifndef SKINENIGMA_NO_MENULOGO
  {"  Image width",             // English
   "  Bildbreite",              // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",            // Nederlands
   "",                          // Português
   "  Largeur",                 // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "  Anchura de imagen",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Image height",            // English
   "  Bildhöhe",                // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",             // Nederlands
   "",                          // Português
   "  Hauteur",                 // Français
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",                          // Polski
   "  Altura de imagen",        // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
#ifdef HAVE_IMAGEMAGICK
  {"  Image format",            // English
   "  Bildformat",              // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",            // Nederlands
   "",                          // Português
   "  Format d'image",          // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "  Formato de imagen",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"Full title width",          // English
   "Volle Titelbreite",         // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",        // Nederlands
   "",                          // Português
   "Largeur totale pour le titre", // Français
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Ancho completo para título",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#ifdef USE_PLUGIN_MAILBOX
  {"Show mail icon",            // English
   "Zeige Mail-Icon",           // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",          // Nederlands
   "",                          // Português
   "Afficher l'icône courriel",  // Français
   "",                          // Norsk
   "",    // suomi (Finnish)
   "",                          // Polski
   "Mostrar icono correo",      // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"only if new mail present",  // English
   "nur bei neuer Mail",        // Deutsch
   "",                          // Slovenski
   "",          // Italiano
   "",    // Nederlands
   "",                          // Português
   "juste lors d'un courriel",  // Français
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",                          // Polski
   "sólo si hay nuevo correo",  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
#ifndef DISABLE_ANIMATED_TEXT
  {"Animated Text",             // English
   "Bewegter Text",             // Deutsch
   "",                          // Slovenski
   "",             // Italiano
   "",         // Nederlands
   "",                          // Português
   "Texte défilant",            // Français
   "",                          // Norsk
   "",        // suomi (Finnish)
   "",                          // Polski
   "Texto animado",             // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Enable",                    // English
   "Verwenden",                 // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                  // Nederlands
   "",                          // Português
   "Permettre",                 // Français
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",                          // Polski
   "Activar",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scoll behaviour",         // English
   "  Scroll-Verhalten",        // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",            // Nederlands
   "",                          // Português
   "  Comportement de défilement", // Français
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",                          // Polski
   "  Comportamiento del scroll",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"to the left",               // English
   "nach links",                // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "",                // Nederlands
   "",                          // Português
   "vers la gauche",            // Français
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                          // Polski
   "a la izquierda",            // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"left and right",            // English
   "links und rechts",          // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "",           // Nederlands
   "",                          // Português
   "gauche et droite",          // Français
   "",                          // Norsk
   "",              // suomi (Finnish)
   "",                          // Polski
   "izquierda y derecha",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scroll delay (ms)",       // English
   "  Scroll-Verzögerung (ms)", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",   // Nederlands
   "",                          // Português
   "  Délai de défilement (ms)",// Français
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "  Retardo del scroll (ms)", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scroll pause (ms)",       // English
   "  Scroll-Pause (ms)",       // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "",        // Nederlands
   "",                          // Português
   "  Pause de défilement (ms)",// Français
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "  Pausa del scroll (ms)",   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Blink pause (ms)",        // English
   "  Blink-Pause (ms)",        // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",         // Nederlands
   "",                          // Português
   "  Pause de clignotement (ms)",// Français
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "  Pausa del parpadeo (ms)", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scroll OSD title",        // English
   "  OSD Titel scrollen",      // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",      // Nederlands
   "",                          // Português
   "  Défilement dans le titre du OSD",  // Français
   "",                          // Norsk
   "",// suomi (Finnish)
   "",                          // Polski
   "  Título OSD del scroll",   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scroll info area",        // English
   "  Infobereich scrollen",    // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",     // Nederlands
   "",                          // Português
   "  Barre de défilement dans la surface info",   // Français
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "  Scroll para el área info",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scroll active list items",  // English
   "  Aktive Listenzeile scrollen", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",  // Nederlands
   "",                          // Português
   "  Barre de défilement dans la liste active",  // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Scroll para la lista activa",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Scroll other items",      // English
   "  Andere Bereiche scrollen", // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",   // Nederlands
   "",                          // Português
   "  Barre de défilement dans d'autres parties",  // Français
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",                          // Polski
   "  Scroll para otros datos", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
#ifndef SKINENIGMA_NO_MENULOGO
  {"  Show symbols in menu",    // English
   "  Symbole im Menü",         // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",   // Nederlands
   "",                          // Português
   "  dans le menu",  		// Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar símbolos en menú",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"  Show symbols in replay",  // English
   "  Symbole bei der Wiedergabe", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",   // Nederlands
   "",                          // Português
   "  dans lecture",		// Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar símbolos en reproducción",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Show symbols in messages",  // English
   "  Symbole bei den Meldungen", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",  // Nederlands
   "",                          // Português
   "  dans les messages", 	// Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar símbolos en mensajes",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"  Show symbols in audio",   // English
   "  Symbole bei Audio",       // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",      // Nederlands
   "",                          // Português
   "  dans audio",        	// Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar símbolos en audio",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Fonts",                     // English
   "Fonts",                     // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",                     // Nederlands
   "",                          // Português
   "Les polices",               // Français
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",                          // Polski
   "Fuentes",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"OSD title",                 // English
   "OSD Titel",                 // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",                 // Nederlands
   "",                          // Português
   "Titre OSD",                 // Français
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "Título OSD",                // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Date",                      // English
   "Datum",                     // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                     // Nederlands
   "",                          // Português
   "Date",                      // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Fecha",                     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Messages",                  // English
   "Meldungen",                 // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                 // Nederlands
   "",                          // Português
   "Message",                   // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Mensajes",                  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Help keys",                 // English
   "Farb-Tasten",               // Deutsch
   "",                          // Slovenski
   "",             // Italiano
   "",              // Nederlands
   "",                          // Português
   "Touche couleur",            // Français
   "",                          // Norsk
   "",             // suomi (Finnish)
   "",                          // Polski
   "Botones de ayuda",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Channelinfo: title",        // English
   "Kanalinfo: Titel",          // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",         // Nederlands
   "",                          // Português
   "Chaîne info: titre ",        // Français
   "",                          // Norsk
   "",// suomi (Finnish)
   "",                          // Polski
   "Info canal: título",        // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Channelinfo: subtitle",     // English
   "Kanalinfo: Untertitel",     // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "",    // Nederlands
   "",                          // Português
   "Chaîne info: sous-titres",    // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Info canal: subtítulo",     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Channelinfo: language",     // English
   "Kanalinfo: Sprache",        // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",          // Nederlands
   "",                          // Português
   "Chaîne info: langue",       // Français
   "",                          // Norsk
   "",        // suomi (Finnish)
   "",                          // Polski
   "Info canal: idioma",        // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"List items",                // English
   "Listen",                    // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                   // Nederlands
   "",                          // Português
   "Liste",                     // Français
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                          // Polski
   "Listado",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Info area: timers title",   // English
   "Infobereich: Timer Titel",  // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",    // Nederlands
   "",                          // Português
   "Barre d'information: Titre Progammation",   // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Área info: título programación",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Info area: timers text",    // English
   "Infobereich: Timer Text",   // Deutsch
   "",                          // Slovenski
   "",    // Italiano
   "",    // Nederlands
   "",                          // Português
   "Barre d'information: Text programmation",   // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Área info: texto programación",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Info area: warning title",  // English
   "Infobereich: Warnung Titel", // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "", // Nederlands
   "",                          // Português
   "Barre d'information: avertissement titre",   // Français
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "Área info: título aviso",   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Info area: warning text",   // English
   "Infobereich: Warnung Text", // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",  // Nederlands
   "",                          // Português
   "Barre d'information: texte d'avertissement",      // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Área info: texto aviso",    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Details: title",            // English
   "Details: Titel",            // Deutsch
   "",                          // Slovenski
   "",          // Italiano
   "",            // Nederlands
   "",                          // Português
   "Détails: Titre",            // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Detalles: título",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Details: subtitle",         // English
   "Details: Untertitel",       // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",       // Nederlands
   "",                          // Português
   "Détails: Sous-titre",       // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Detalles: subtítulo",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Details: date",             // English
   "Details: Datumszeile",      // Deutsch
   "",                          // Slovenski
   "",            // Italiano
   "",            // Nederlands
   "",                          // Português
   "Détails: date",             // Français
   "",                          // Norsk
   "",    // suomi (Finnish)
   "",                          // Polski
   "Detalles: fecha",           // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Details: text",             // English
   "Details: Text",             // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",            // Nederlands
   "",                          // Português
   "Détails: Texte",             // Français
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",                          // Polski
   "Detalles: texto",           // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Replay: times",             // English
   "Wiedergabe: Zeiten",        // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "",          // Nederlands
   "",                          // Português
   "Rejouer : périodes",        // Français
   "",                          // Norsk
   "",        // suomi (Finnish)
   "",                          // Polski
   "Reproducir: veces",         // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Default OSD Font",          // English
   "Std. OSD Font",             // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",         // Nederlands
   "",                          // Português
   "OSD standard",              // Français
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Fuente OSD por defecto",    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Default Fixed Size Font",   // English
   "Std. Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",     // Nederlands
   "",                          // Português
   "Largeur fixe standard",     // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Fuente tamaño fijo por defecto",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Default Small Font",        // English
   "Std. kleine Schrift",       // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",      // Nederlands
   "",                          // Português
   "Petite police standard",           // Français
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Fuente pequeña por defecto",// Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Channel:",                  // English
   "Kanal:",                    // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                   // Nederlands
   "",                          // Português
   "Chaîne:",                   // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Canal:",                    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Search pattern:",           // English
   "Suchmuster:",               // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",              // Nederlands
   "",                          // Português
   "Modèle de recherche",       // Français
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "Buscar patrón:",            // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"No timer check",            // English
   "Ohne Überwachung",          // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",             // Nederlands
   "",                          // Português
   "Sans surveillance",         // Français
   "",                          // Norsk
   "",              // suomi (Finnish)
   "",                          // Polski
   "No comprobar programación", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"General",                   // English
   "Allgemein",                 // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                  // Nederlands
   "",                          // Português
   "Généralité",                // Français
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "General",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                      // Nederlands
   "",                          // Português
   "Nom",                       // Français
   "",                          // Norsk
   "",                      // suomi (Finnish)
   "",                          // Polski
   "Nombre",                    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Width",                     // English
   "Weite",                     // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",                   // Nederlands
   "",                          // Português
   "Largeur",                   // Français
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                          // Polski
   "Anchura",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Button$Set",                // English
   "Ändern",                    // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                // Nederlands
   "",                          // Português
   "Changer",                   // Français
   "",                          // Norsk
   "",                     // suomi (Finnish)
   "",                          // Polski
   "Cambiar",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"TrueType Font",             // English
   "TrueType Schrift",          // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",             // Nederlands
   "",                          // Português
   "TrueType",                  // Français
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",                          // Polski
   "Fuente TrueType",           // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"No TTF support!",            // English
   "TTF nicht unterstützt!",    // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",   // Nederlands
   "",                          // Português
   "Les polices TrueType ne sont pas supportées!", // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "¡Sin soporte TTF!",         // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
  {"Fixed Font",                // English
   "Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",         // Nederlands
   "",                          // Português
   "Police avec une largeur fixe", // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Fuente Fija",               // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#ifdef USE_PLUGIN_AVARDS
  {"Dynamic OSD size",          // English
   "Dynamische OSD-Größe",      // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",      // Nederlands
   "",                          // Português
   "Taille OSD dynamique",      // Français
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Tamaño OSD dinámico",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // Èesky (Czech)
   },
#endif
  {"No TrueType fonts installed!",          // English
   "Keine TrueType Schriften installiert!", // Deutsch
   "",                                      // Slovenski
   "", // Italiano
   "",   // Nederlands
   "",                                      // Português
   "Il n'y a pas de police TrueType d'installé!",  // Français
   "",                                      // Norsk
   "",   // suomi (Finnish)
   "",                                      // Polski
   "¡Ninguna fuente TrueType instalada!",   // Español
   "",                                      // ÅëëçíéêÜ (Greek)
   "",                                      // Svenska
   "",                                      // Românã
   "",                                      // Magyar
   "",                                      // Català
   "",                                      // ÀãááÚØÙ (Russian)
   "",                                      // Hrvatski
   "",                                      // Eesti
   "",                                      // Dansk
   "",      // Èesky (Czech)
   },
  {"Content: ",                             // English
   "",                                      // Deutsch
   "",                                      // Slovenski
   "",                                      // Italiano
   "",                                      // Nederlands
   "",                                      // Português
   "",                                      // Français
   "",                                      // Norsk
   "",                                      // suomi (Finnish)
   "",                                      // Polski
   "Contenido: ",                           // Español
   "",                                      // ÅëëçíéêÜ (Greek)
   "",                                      // Svenska
   "",                                      // Românã
   "",                                      // Magyar
   "",                                      // Català
   "",                                      // ÀãááÚØÙ (Russian)
   "",                                      // Hrvatski
   "",                                      // Eesti
   "",                                      // Dansk
   "",      // Èesky (Czech)
   },
  {NULL}
};
// vim:et:sw=2:ts=2:

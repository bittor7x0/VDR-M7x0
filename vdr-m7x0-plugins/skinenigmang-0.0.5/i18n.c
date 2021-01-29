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
 *  - Fran�ais:   Patrice Staudt <pat @vdr-portal.de>
 *  - Finnish:    Rolf Ahrenberg <rofafor @vdr-portal.de>
 *  - Polski:     ???
 *  - Espa�ol:    bittor from open7x0.org <bittor7x0 _at_ protonmail.com>
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
   "pt_PT",             // Portugu�s
   "fr_FR",             // Fran�ais  Patrice Staudt 18.05.2007
   "no_NO",             // Norsk
   "fi_FI",             // suomi (Finnish)
   "pl_PL",             // Polski
   "es_ES",             // Espa�ol
   "el_GR",             // �������� (Greek)
   "sv_SE",             // Svenska
   "ro_RO",             // Rom�n�
   "hu_HU",             // Magyar
   "ca_AD",             // Catal�
   "ru_RU",             // ������� (Russian)
   "hr_HR",             // Hrvatski
   "et_EE",             // Eesti
   "da_DK",             // Dansk
   "cs_CZ",             // �esky (Czech)
   },
  {"Monday",       // English  Patrice Staudt 21.06.2007 Alle Sprache umgesetzt von LC_TIME
   "Montag",       // Deutsch
   "",   // Slovenski
   "",       // Italiano
   "",      // Nederlands
   "",      // Portugu�s
   "Lundi",        // Fran�ais
   "",       // Norsk
   "",    // suomi (Finnish)
   "", // Polski
   "Lunes",        // Espa�ol
   "",      // �������� (Greek)
   "",       // Svenska
   "",         // Rom�n�
   "",        // Magyar
   "",      // Catal�
   "",  // ������� (Russian)
   "",  // Hrvatski
   "",    // Eesti
   "",       // Dansk
   "",      // �esky (Czech)
   },
  {"Tuesday",      // English
   "Dienstag",     // Deutsch
   "",        // Slovenski
   "",      // Italiano
   "",      // Nederlands
   "",        // Portugu�s
   "Mardi",        // Fran�ais
   "",      // Norsk
   "",      // suomi (Finnish)
   "",       // Polski
   "Martes",       // Espa�ol
   "",        // �������� (Greek)
   "",       // Svenska
   "",        // Rom�n�
   "",         // Magyar
   "",      // Catal�
   "",      // ������� (Russian)
   "",       // Hrvatski
   "",    // Eesti
   "",      // Dansk
   "",      // �esky (Czech)
   },
  {"Wednesday",    // English
   "Mittwoch",     // Deutsch
   "",        // Slovenski
   "",    // Italiano
   "",     // Nederlands
   "",       // Portugu�s
   "Mercredi",     // Fran�ais
   "",       // Norsk
   "",  // suomi (Finnish)
   "",        // Polski
   "Mi�rcoles",    // Espa�ol
   "",      // �������� (Greek)
   "",       // Svenska
   "",     // Rom�n�
   "",       // Magyar
   "",     // Catal�
   "",        // ������� (Russian)
   "",      // Hrvatski
   "",    // Eesti
   "",       // Dansk
   "",      // �esky (Czech)
   },
  {"Thursday",     // English
   "Donnerstag",   // Deutsch
   "",      // Slovenski
   "",      // Italiano
   "",    // Nederlands
   "",       // Portugu�s
   "Jeudi",        // Fran�ais
   "",      // Norsk
   "",      // suomi (Finnish)
   "",     // Polski
   "Jueves",       // Espa�ol
   "",       // �������� (Greek)
   "",      // Svenska
   "",          // Rom�n�
   "",    // Magyar
   "",       // Catal�
   "",      // ������� (Russian)
   "",     // Hrvatski
   "",    // Eesti
   "",      // Dansk
   "",      // �esky (Czech)
   },
  {"Friday",       // English
   "Freitag",      // Deutsch
   "",        // Slovenski
   "",      // Italiano
   "",      // Nederlands
   "",        // Portugu�s
   "Vendredi",     // Fran�ais
   "",       // Norsk
   "",    // suomi (Finnish)
   "",       // Polski
   "Viernes",      // Espa�ol
   "",    // �������� (Greek)
   "",       // Svenska
   "",       // Rom�n�
   "",       // Magyar
   "",    // Catal�
   "",      // ������� (Russian)
   "",        // Hrvatski
   "",        // Eesti
   "",       // Dansk
   "",      // �esky (Czech)
   },
  {"Saturday",     // English
   "Samstag",      // Deutsch
   "",       // Slovenski
   "",       // Italiano
   "",     // Nederlands
   "",       // Portugu�s
   "Samedi",       // Fran�ais
   "",       // Norsk
   "",     // suomi (Finnish)
   "",       // Polski
   "S�bado",       // Espa�ol
   "",      // �������� (Greek)
   "",       // Svenska
   "",      // Rom�n�
   "",      // Magyar
   "",     // Catal�
   "",      // ������� (Russian)
   "",       // Hrvatski
   "",      // Eesti
   "",       // Dansk
   "",      // �esky (Czech)
   },
  {"Sunday",       // English
   "Sonntag",      // Deutsch
   "",      // Slovenski
   "",     // Italiano
   "",       // Nederlands
   "",      // Portugu�s
   "Dimanche",     // Fran�ais
   "",       // Norsk
   "",    // suomi (Finnish)
   "",    // Polski
   "Domingo",      // Espa�ol
   "",      // �������� (Greek)
   "",       // Svenska
   "",     // Rom�n�
   "",     // Magyar
   "",     // Catal�
   "",  // ������� (Russian)
   "",     // Hrvatski
   "",     // Eesti
   "",       // Dansk
   "",      // �esky (Czech)
   },
  {"EnigmaNG skin",             // English
   "EnigmaNG Oberfl�che",       // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",             // Nederlands
   "",                          // Portugu�s
   "Skin EnigmaNG",             // Fran�ais
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",             // Polski
   "Piel EnigmaNG",             // Espa�ol
   "",                          // �������� (Greek)
   "",            // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",             // ������� (Russian)
   "",                          // Hrvatski
   "",             // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"EnigmaNG",                  // English
   "EnigmaNG",                  // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                  // Nederlands
   "",                          // Portugu�s
   "EnigmaNG",                  // Fran�ais
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",                  // Polski
   "EnigmaNG",                  // Espa�ol
   "",                          // �������� (Greek)
   "",                  // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                  // ������� (Russian)
   "",                          // Hrvatski
   "",                  // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"min",                       // English
   "min",                       // Deutsch
   "",                          // Slovenski
   "",                       // Italiano
   "",                       // Nederlands
   "",                          // Portugu�s
   "min",                       // Fran�ais
   "",                          // Norsk
   "",                       // suomi (Finnish)
   "",                       // Polski
   "min",                       // Espa�ol
   "",                          // �������� (Greek)
   "",                       // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                       // ������� (Russian)
   "",                          // Hrvatski
   "",                       // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Stereo",                    // English
   "Stereo",                    // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Portugu�s
   "St�r�o",                    // Fran�ais
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                    // Polski
   "Est�reo",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                    // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                    // ������� (Russian)
   "",                          // Hrvatski
   "",                    // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Left",                      // English
   "Links",                     // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                     // Nederlands
   "",                          // Portugu�s
   "Gauche",                    // Fran�ais
   "",                          // Norsk
   "",                     // suomi (Finnish)
   "",                    // Polski
   "Izquierda",                 // Espa�ol
   "",                          // �������� (Greek)
   "",                   // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                     // ������� (Russian)
   "",                          // Hrvatski
   "",                     // Eesti
   "",                          // Danske
   "",      // �esky (Czech)
   },
  {"Right",                     // English
   "Rechts",                    // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Portugu�s
   "Droite",                    // Fran�ais
   "",                          // Norsk
   "",                     // suomi (Finnish)
   "",                   // Polski
   "Derecha",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                    // ������� (Russian)
   "",                          // Hrvatski
   "",                     // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                // Nederlands
   "",                          // Portugu�s
   "Muet",                      // Fran�ais
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                    // Polski
   "Silenciar",                 // Espa�ol
   "",                          // �������� (Greek)
   "",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                // ������� (Russian)
   "",                          // Hrvatski
   "",                   // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Volume",                    // English
   "Lautst�rke",                // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Portugu�s
   "Volume",                    // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                  // Polski
   "Volumen",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                 // ������� (Russian)
   "",                          // Hrvatski
   "",               // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show channel logos",        // English
   "Kanal-Logos anzeigen",      // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "Afficher logos des cha�nes", // Fran�ais
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",       // Polski
   "Mostrar los logos de los canales",  // Espa�ol
   "",                          // �������� (Greek)
   "",       // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",      // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Identify channel by",       // English
   "  Kanal-Identifikation durch",        // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",  // Nederlands
   "",                          // Portugu�s
   "  Identifier cha�ne par",     // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",        // Polski
   "  Identificar el canal por",       // Espa�ol
   "",                          // �������� (Greek)
   "",     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",       // ������� (Russian)
   "",                          // Hrvatski
   "", // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                      // Nederlands
   "",                          // Portugu�s
   "Nom",                       // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                     // Polski
   "nombre",                    // Espa�ol
   "",                          // �������� (Greek)
   "",                      // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                  // ������� (Russian)
   "",                          // Hrvatski
   "",                      // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"data",                      // English
   "ID",                        // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                      // Nederlands
   "",                          // Portugu�s
   "Donn�e",                    // Fran�ais
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                 // Polski
   "datos",                     // Espa�ol
   "",                          // �������� (Greek)
   "",                      // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                    // ������� (Russian)
   "",                          // Hrvatski
   "",                    // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Channel logo cache size",   // English
   "Gr��e des Logo-Caches",     // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",          // Nederlands
   "",                          // Portugu�s
   "Taille du cache des logos",  // Fran�ais
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",     // Polski
   "Tama�o de cach� para los logos",   // Espa�ol
   "",                          // �������� (Greek)
   "",        // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",  // ������� (Russian)
   "",                          // Hrvatski
   "",        // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Button$Flush cache",        // English
   "Cache leeren",              // Deutsch
   "",                          // Slovenski
   "",             // Italiano
   "",               // Nederlands
   "",                          // Portugu�s
   "Supprimer cache",           // Fran�ais
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",            // Polski
   "Limpiar cach�",             // Espa�ol
   "",                          // �������� (Greek)
   "",                // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",              // ������� (Russian)
   "",                          // Hrvatski
   "",                  // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Flushing channel logo cache...",    // English
   "Logo-Cache wird geleert...",        // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "", // Nederlands
   "",                          // Portugu�s
   "Suppression du cache...",   // Fran�ais
   "",                          // Norsk
   "",       // suomi (Finnish)
   "",    // Polski
   "Limpiando la cach�...",     // Espa�ol
   "",                          // �������� (Greek)
   "",          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",       // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Languages",                 // English
   "Sprachen",                  // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                     // Nederlands
   "",                          // Portugu�s
   "Langues",                   // Fran�ais
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                    // Polski
   "Idiomas",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                    // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Auxiliary information",     // English
   "Zusatzinformation",         // Deutsch
   "",                          // Slovenski
   "",    // Italiano
   "",           // Nederlands
   "",                          // Portugu�s
   "Information auxiliare",     // Fran�ais
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",     // Polski
   "Informaci�n adicional",     // Espa�ol
   "",                          // �������� (Greek)
   "",          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",          // ������� (Russian)
   "",                          // Hrvatski
   "",                  // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show auxiliary information",        // English
   "Zusatzinfo anzeigen",       // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",      // Nederlands
   "",                          // Portugu�s
   "Afficher informations auxiliaires",   // Fran�ais
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",     // Polski
   "Mostrar informaci�n adicional",     // Espa�ol
   "",                          // �������� (Greek)
   "",     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",      // ������� (Russian)
   "",                          // Hrvatski
   "",        // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show remaining/elapsed time",          // English
   "Zeige abgel./restl. Zeit",             // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",  // Nederlands
   "",                          // Portugu�s
   "Afficher le temps �coul�/restant", // Fran�ais
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Mostrar tiempo restante/transcurrido",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",     // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"remaining",                 // English
   "restliche",                 // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                   // Nederlands
   "",                          // Portugu�s
   "restant",                   // Fran�ais
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",                          // Polski
   "restante",                  // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                 // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"elapsed",                   // English
   "abgelaufene",               // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",                // Nederlands
   "",                          // Portugu�s
   "�coul�",                    // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "transcurrido",              // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                  // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show info area in main menu",            // English
   "Infobereich im Hauptmen�",               // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "Surface d'infos dans le menu principal",  // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Mostrar info en men� principal",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show marker in lists",            // English
   "Symbol vor Listeneintr�gen",      // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "", // Nederlands
   "",                          // Portugu�s
   "Marque devant les listes",  // Fran�ais
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Mostrar marcador en listas",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show progressbar",          // English
   "Fortschrittbalken anzeigen",        // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "",      // Nederlands
   "",                          // Portugu�s
   "Afficher barre de progression",     // Fran�ais
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",     // Polski
   "Mostrar barra de progreso", // Espa�ol
   "",                          // �������� (Greek)
   "",             // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",    // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show symbols",              // English
   "Symbole anzeigen",          // Deutsch
   "",                          // Slovenski
   "",            // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "Afficher symbole",          // Fran�ais
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",           // Polski
   "Mostrar s�mbolos",          // Espa�ol
   "",                          // �������� (Greek)
   "",             // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",       // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show VPS",                  // English
   "VPS anzeigen",              // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",                 // Nederlands
   "",                          // Portugu�s
   "Afficher le VPS",           // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "Mostrar VPS",               // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"WARNING",                   // English
   "WARNUNG",                   // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",              // Nederlands
   "",                          // Portugu�s
   "ATTENTION",                 // Fran�ais
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",                          // Polski
   "AVISO",                     // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Timer conflict",            // English
   "Timerkonflikt",             // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",             // Nederlands
   "",                          // Portugu�s
   "Conflit de programmation",  // Fran�ais
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Conflicto de programaci�n", // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Timer conflicts",           // English
   "Timerkonflikte",            // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",                          // Nederlands
   "",                          // Portugu�s
   "Conflits de programmation", // Fran�ais
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",                          // Polski
   "Conflictos de programaci�n",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",        // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"TIMERS",                    // English
   "TIMER",                     // Deutsch
   "",                          // Slovenski
   "",                     // Italiano
   "",                     // Nederlands
   "",                          // Portugu�s
   "PROGRAMMATION",             // Fran�ais
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "PROGRAMACIONES",            // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"RERUNS OF THIS SHOW",       // English
   "WIEDERHOLUNGEN",            // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",               // Nederlands
   "",                          // Portugu�s
   "REPETITION",                // Fran�ais
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "REPETICIONES",              // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"*** Invalid Channel ***",   // English
   "*** Ung�ltiger Kanal ***",  // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",   // Nederlands
   "",                          // Portugu�s
   "*** Cha�ne non valable ***",  // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "*** Canal no v�lido ***",   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Size",                      // English
   "Gr��e",                     // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",                   // Nederlands
   "",                          // Portugu�s
   "Taille",                    // Fran�ais
   "",                          // Norsk
   "",                      // suomi (Finnish)
   "",                          // Polski
   "Tama�o",                    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                // Nederlands
   "",                      // Portugu�s
   "",                          // Fran�ais
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                          // Polski
   "Silenciar",                 // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                 // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Volume",                    // English
   "Lautst�rke",                // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Portugu�s
   "Volume",                    // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "Volumen",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show symbols in lists",     // English
   "Symbole in Listen",         // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",       // Nederlands
   "",                          // Portugu�s
   "Symboles dans les listes",   // Fran�ais
   "",                          // Norsk
   "",// suomi (Finnish)
   "",                          // Polski
   "Mostrar s�mbolos en listas",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#ifndef VDRO7OVERSION
  {"Try 8bpp single area",      // English
   "Einen 8bpp Bereich versuchen", // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",     // Nederlands
   "",                          // Portugu�s
   "Essayer une surface � 8bpp",// Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Probar un �rea de 8bpp",    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"Show flags",                // English
   "Fahnen anzeigen",           // Deutsch
   "",                          // Slovenski
   "",              // Italiano
   "",             // Nederlands
   "",                          // Portugu�s
   "Afficher drapeau",          // Fran�ais
   "",                          // Norsk
   "",               // suomi (Finnish)
   "",                          // Polski
   "Mostrar banderas",          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",            // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show symbols in menu",      // English
   "Symbole im Men�",           // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",          // Nederlands
   "",                          // Portugu�s
   "Symboles dans le menu",     // Fran�ais
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Mostrar s�mbolos en men�",  // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#if defined SKINENIGMA_HAVE_EPGSEARCH || defined USE_PLUGIN_MAILBOX
  {"never",                     // English
   "nie",                       // Deutsch
   "",                          // Slovenski
   "",                       // Italiano
   "",                     // Nederlands
   "",                          // Portugu�s
   "jamais",                    // Fran�ais
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                          // Polski
   "nunca",                     // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"always",                    // English
   "immer",                     // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                    // Nederlands
   "",                          // Portugu�s
   "toujours",                  // Fran�ais
   "",                          // Norsk
   "",                      // suomi (Finnish)
   "",                          // Polski
   "siempre",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                    // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"percent",                   // English
   "Prozent",                   // Deutsch
   "",                          // Slovenski
   "",               // Italiano
   "",                   // Nederlands
   "",                          // Portugu�s
   "Pourcentage",               // Fran�ais
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "porcentaje",                // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#ifdef SKINENIGMA_HAVE_EPGSEARCH
  {"if exists",                 // English
   "wenn vorhanden",            // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",          // Nederlands
   "",                          // Portugu�s
   "si existant",               // Fran�ais
   "",                          // Norsk
   "",              // suomi (Finnish)
   "",                          // Polski
   "si existe",                 // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",            // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Number of Reruns",          // English
   "Anzahl Wiederholungen",     // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",        // Nederlands
   "",                          // Portugu�s
   "Nombre de r�p�titions",     // Fran�ais
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "N�mero de repeticiones",    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",     // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Use Subtitle for reruns",   // English
   "Verw. Untertitel f�r Wdh.", // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "", // Nederlands
   "",                          // Portugu�s
   "Utilise sous-titres pour les r�p�titions", // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Usar subt�tulo para repeticiones",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show timer conflicts",      // English
   "Timerkonflikte anzeigen",   // Deutsch
   "",                          // Slovenski
   "",    // Italiano
   "",       // Nederlands
   "",                          // Portugu�s
   "Afficher les conflits de programmation", // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Mostrar conflictos de programaci�n",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"Timer check",               // English
   "�berwachung",               // Deutsch
   "",                          // Slovenski
   "",            // Italiano
   "",                 // Nederlands
   "",                          // Portugu�s
   "Observation de la programmation", // Fran�ais
   "",                          // Norsk
   "",       // suomi (Finnish)
   "",                          // Polski
   "Comprobar programaci�n",    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",         // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#ifdef SKINENIGMA_HAVE_EPGSEARCH
  {"EPGSearch",                 // English
   "EPGSearch",                 // Deutsch
   "",                          // Slovenski
   "",               // Italiano
   "",                 // Nederlands
   "",                          // Portugu�s
   "Recherche EPG",             // Fran�ais
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "EPGSearch",                 // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"Logos & Symbols",           // English
   "Logos & Symbole",           // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "Logos et symboles",         // Fran�ais
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",                          // Polski
   "Logos y s�mbolos",          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",        // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"use size.vdr only",         // English
   "nur size.vdr verwenden",    // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "", // Nederlands
   "",                          // Portugu�s
   "utiliser seulement size.vdr",  // Fran�ais Patrice Staudt 29.03.2007
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",                          // Polski
   "s�lo usar size.vdr",        // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",  // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show recording's size",     // English
   "Aufnahmengr��e anzeigen",   // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",       // Nederlands
   "",                          // Portugu�s
   "Afficher la taille des enregistrements",   // Fran�ais
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "Mostrar tama�o de grabaci�n",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",    // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#ifndef SKINENIGMA_NO_MENULOGO
  {"pixel algo",                // English
   "Pixel-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "Algorithme pixel",          // Fran�ais
   "",                          // Norsk
   "",               // suomi (Finnish)
   "",                          // Polski
   "Algoritmo p�xel",           // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"ratio algo",                // English
   "Ratio-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "Algorithme ratio",          // Fran�ais
   "",                          // Norsk
   "",                  // suomi (Finnish)
   "",                          // Polski
   "Algoritmo ratio",           // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"zoom image",                // English
   "Zoomen",                    // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",                    // Nederlands
   "",                          // Portugu�s
   "Agrandir image",            // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Aumentar imagen",           // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                 // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Show event/recording images",     // English
   "Bilder in EPG/Aufnahmendetails",  // Deutsch
   "",                                // Slovenski
   "",      // Italiano
   "",         // Nederlands
   "",                                // Portugu�s
   "Images dans les EPG et d�tails d'enregistrements",     // Fran�ais
   "",                                // Norsk
   "",  // suomi (Finnish)
   "",                                // Polski
   "Mostrar im�genes grabaci�n/suceso",// Espa�ol
   "",                                // �������� (Greek)
   "",                                // Svenska
   "",                                // Rom�n�
   "",                                // Magyar
   "",                                // Catal�
   "",               // ������� (Russian)
   "",                                // Hrvatski
   "",                                // Eesti
   "",                                // Dansk
   "",      // �esky (Czech)
   },
#endif
#ifdef HAVE_IMAGEMAGICK
  {"  Resize images",           // English
   "  Bildberechnung benutzt",  // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",  // Nederlands
   "",                          // Portugu�s
   "  Redimensionne les images",  // Fran�ais
   "",                          // Norsk
   "",    // suomi (Finnish)
   "",                          // Polski
   "  Redimensionar im�genes",  // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",    // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
#ifndef SKINENIGMA_NO_MENULOGO
  {"  Image width",             // English
   "  Bildbreite",              // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "  Largeur",                 // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "  Anchura de imagen",       // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Image height",            // English
   "  Bildh�he",                // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",             // Nederlands
   "",                          // Portugu�s
   "  Hauteur",                 // Fran�ais
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",                          // Polski
   "  Altura de imagen",        // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
#ifdef HAVE_IMAGEMAGICK
  {"  Image format",            // English
   "  Bildformat",              // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "  Format d'image",          // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "  Formato de imagen",       // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"Full title width",          // English
   "Volle Titelbreite",         // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",        // Nederlands
   "",                          // Portugu�s
   "Largeur totale pour le titre", // Fran�ais
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Ancho completo para t�tulo",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#ifdef USE_PLUGIN_MAILBOX
  {"Show mail icon",            // English
   "Zeige Mail-Icon",           // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",          // Nederlands
   "",                          // Portugu�s
   "Afficher l'ic�ne courriel",  // Fran�ais
   "",                          // Norsk
   "",    // suomi (Finnish)
   "",                          // Polski
   "Mostrar icono correo",      // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"only if new mail present",  // English
   "nur bei neuer Mail",        // Deutsch
   "",                          // Slovenski
   "",          // Italiano
   "",    // Nederlands
   "",                          // Portugu�s
   "juste lors d'un courriel",  // Fran�ais
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",                          // Polski
   "s�lo si hay nuevo correo",  // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",  // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
#ifndef DISABLE_ANIMATED_TEXT
  {"Animated Text",             // English
   "Bewegter Text",             // Deutsch
   "",                          // Slovenski
   "",             // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "Texte d�filant",            // Fran�ais
   "",                          // Norsk
   "",        // suomi (Finnish)
   "",                          // Polski
   "Texto animado",             // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Enable",                    // English
   "Verwenden",                 // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                  // Nederlands
   "",                          // Portugu�s
   "Permettre",                 // Fran�ais
   "",                          // Norsk
   "",          // suomi (Finnish)
   "",                          // Polski
   "Activar",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scoll behaviour",         // English
   "  Scroll-Verhalten",        // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "  Comportement de d�filement", // Fran�ais
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",                          // Polski
   "  Comportamiento del scroll",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"to the left",               // English
   "nach links",                // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "",                // Nederlands
   "",                          // Portugu�s
   "vers la gauche",            // Fran�ais
   "",                          // Norsk
   "",                // suomi (Finnish)
   "",                          // Polski
   "a la izquierda",            // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"left and right",            // English
   "links und rechts",          // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "",           // Nederlands
   "",                          // Portugu�s
   "gauche et droite",          // Fran�ais
   "",                          // Norsk
   "",              // suomi (Finnish)
   "",                          // Polski
   "izquierda y derecha",       // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scroll delay (ms)",       // English
   "  Scroll-Verz�gerung (ms)", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",   // Nederlands
   "",                          // Portugu�s
   "  D�lai de d�filement (ms)",// Fran�ais
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "  Retardo del scroll (ms)", // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scroll pause (ms)",       // English
   "  Scroll-Pause (ms)",       // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "",        // Nederlands
   "",                          // Portugu�s
   "  Pause de d�filement (ms)",// Fran�ais
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "  Pausa del scroll (ms)",   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Blink pause (ms)",        // English
   "  Blink-Pause (ms)",        // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "  Pause de clignotement (ms)",// Fran�ais
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "  Pausa del parpadeo (ms)", // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scroll OSD title",        // English
   "  OSD Titel scrollen",      // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",      // Nederlands
   "",                          // Portugu�s
   "  D�filement dans le titre du OSD",  // Fran�ais
   "",                          // Norsk
   "",// suomi (Finnish)
   "",                          // Polski
   "  T�tulo OSD del scroll",   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scroll info area",        // English
   "  Infobereich scrollen",    // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",     // Nederlands
   "",                          // Portugu�s
   "  Barre de d�filement dans la surface info",   // Fran�ais
   "",                          // Norsk
   "",     // suomi (Finnish)
   "",                          // Polski
   "  Scroll para el �rea info",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scroll active list items",  // English
   "  Aktive Listenzeile scrollen", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",  // Nederlands
   "",                          // Portugu�s
   "  Barre de d�filement dans la liste active",  // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Scroll para la lista activa",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Scroll other items",      // English
   "  Andere Bereiche scrollen", // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",   // Nederlands
   "",                          // Portugu�s
   "  Barre de d�filement dans d'autres parties",  // Fran�ais
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",                          // Polski
   "  Scroll para otros datos", // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
#ifndef SKINENIGMA_NO_MENULOGO
  {"  Show symbols in menu",    // English
   "  Symbole im Men�",         // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",   // Nederlands
   "",                          // Portugu�s
   "  dans le menu",  		// Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar s�mbolos en men�",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"  Show symbols in replay",  // English
   "  Symbole bei der Wiedergabe", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",   // Nederlands
   "",                          // Portugu�s
   "  dans lecture",		// Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar s�mbolos en reproducci�n",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Show symbols in messages",  // English
   "  Symbole bei den Meldungen", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",  // Nederlands
   "",                          // Portugu�s
   "  dans les messages", 	// Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar s�mbolos en mensajes",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"  Show symbols in audio",   // English
   "  Symbole bei Audio",       // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",      // Nederlands
   "",                          // Portugu�s
   "  dans audio",        	// Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "  Mostrar s�mbolos en audio",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Fonts",                     // English
   "Fonts",                     // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",                     // Nederlands
   "",                          // Portugu�s
   "Les polices",               // Fran�ais
   "",                          // Norsk
   "",            // suomi (Finnish)
   "",                          // Polski
   "Fuentes",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"OSD title",                 // English
   "OSD Titel",                 // Deutsch
   "",                          // Slovenski
   "",                // Italiano
   "",                 // Nederlands
   "",                          // Portugu�s
   "Titre OSD",                 // Fran�ais
   "",                          // Norsk
   "",           // suomi (Finnish)
   "",                          // Polski
   "T�tulo OSD",                // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Date",                      // English
   "Datum",                     // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                     // Nederlands
   "",                          // Portugu�s
   "Date",                      // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Fecha",                     // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Messages",                  // English
   "Meldungen",                 // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                 // Nederlands
   "",                          // Portugu�s
   "Message",                   // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Mensajes",                  // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Help keys",                 // English
   "Farb-Tasten",               // Deutsch
   "",                          // Slovenski
   "",             // Italiano
   "",              // Nederlands
   "",                          // Portugu�s
   "Touche couleur",            // Fran�ais
   "",                          // Norsk
   "",             // suomi (Finnish)
   "",                          // Polski
   "Botones de ayuda",          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Channelinfo: title",        // English
   "Kanalinfo: Titel",          // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "Cha�ne info: titre ",        // Fran�ais
   "",                          // Norsk
   "",// suomi (Finnish)
   "",                          // Polski
   "Info canal: t�tulo",        // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Channelinfo: subtitle",     // English
   "Kanalinfo: Untertitel",     // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "",    // Nederlands
   "",                          // Portugu�s
   "Cha�ne info: sous-titres",    // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Info canal: subt�tulo",     // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Channelinfo: language",     // English
   "Kanalinfo: Sprache",        // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",          // Nederlands
   "",                          // Portugu�s
   "Cha�ne info: langue",       // Fran�ais
   "",                          // Norsk
   "",        // suomi (Finnish)
   "",                          // Polski
   "Info canal: idioma",        // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"List items",                // English
   "Listen",                    // Deutsch
   "",                          // Slovenski
   "",                    // Italiano
   "",                   // Nederlands
   "",                          // Portugu�s
   "Liste",                     // Fran�ais
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                          // Polski
   "Listado",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Info area: timers title",   // English
   "Infobereich: Timer Titel",  // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",    // Nederlands
   "",                          // Portugu�s
   "Barre d'information: Titre Progammation",   // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "�rea info: t�tulo programaci�n",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Info area: timers text",    // English
   "Infobereich: Timer Text",   // Deutsch
   "",                          // Slovenski
   "",    // Italiano
   "",    // Nederlands
   "",                          // Portugu�s
   "Barre d'information: Text programmation",   // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "�rea info: texto programaci�n",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Info area: warning title",  // English
   "Infobereich: Warnung Titel", // Deutsch
   "",                          // Slovenski
   "",  // Italiano
   "", // Nederlands
   "",                          // Portugu�s
   "Barre d'information: avertissement titre",   // Fran�ais
   "",                          // Norsk
   "",  // suomi (Finnish)
   "",                          // Polski
   "�rea info: t�tulo aviso",   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Info area: warning text",   // English
   "Infobereich: Warnung Text", // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",  // Nederlands
   "",                          // Portugu�s
   "Barre d'information: texte d'avertissement",      // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "�rea info: texto aviso",    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Details: title",            // English
   "Details: Titel",            // Deutsch
   "",                          // Slovenski
   "",          // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "D�tails: Titre",            // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Detalles: t�tulo",          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Details: subtitle",         // English
   "Details: Untertitel",       // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",       // Nederlands
   "",                          // Portugu�s
   "D�tails: Sous-titre",       // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Detalles: subt�tulo",       // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Details: date",             // English
   "Details: Datumszeile",      // Deutsch
   "",                          // Slovenski
   "",            // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "D�tails: date",             // Fran�ais
   "",                          // Norsk
   "",    // suomi (Finnish)
   "",                          // Polski
   "Detalles: fecha",           // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Details: text",             // English
   "Details: Text",             // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",            // Nederlands
   "",                          // Portugu�s
   "D�tails: Texte",             // Fran�ais
   "",                          // Norsk
   "",   // suomi (Finnish)
   "",                          // Polski
   "Detalles: texto",           // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Replay: times",             // English
   "Wiedergabe: Zeiten",        // Deutsch
   "",                          // Slovenski
   "",         // Italiano
   "",          // Nederlands
   "",                          // Portugu�s
   "Rejouer : p�riodes",        // Fran�ais
   "",                          // Norsk
   "",        // suomi (Finnish)
   "",                          // Polski
   "Reproducir: veces",         // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Default OSD Font",          // English
   "Std. OSD Font",             // Deutsch
   "",                          // Slovenski
   "", // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "OSD standard",              // Fran�ais
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Fuente OSD por defecto",    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Default Fixed Size Font",   // English
   "Std. Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "",// Italiano
   "",     // Nederlands
   "",                          // Portugu�s
   "Largeur fixe standard",     // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Fuente tama�o fijo por defecto",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Default Small Font",        // English
   "Std. kleine Schrift",       // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",      // Nederlands
   "",                          // Portugu�s
   "Petite police standard",           // Fran�ais
   "",                          // Norsk
   "",      // suomi (Finnish)
   "",                          // Polski
   "Fuente peque�a por defecto",// Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Channel:",                  // English
   "Kanal:",                    // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                   // Nederlands
   "",                          // Portugu�s
   "Cha�ne:",                   // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "Canal:",                    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Search pattern:",           // English
   "Suchmuster:",               // Deutsch
   "",                          // Slovenski
   "",       // Italiano
   "",              // Nederlands
   "",                          // Portugu�s
   "Mod�le de recherche",       // Fran�ais
   "",                          // Norsk
   "",                 // suomi (Finnish)
   "",                          // Polski
   "Buscar patr�n:",            // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"No timer check",            // English
   "Ohne �berwachung",          // Deutsch
   "",                          // Slovenski
   "",     // Italiano
   "",             // Nederlands
   "",                          // Portugu�s
   "Sans surveillance",         // Fran�ais
   "",                          // Norsk
   "",              // suomi (Finnish)
   "",                          // Polski
   "No comprobar programaci�n", // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"General",                   // English
   "Allgemein",                 // Deutsch
   "",                          // Slovenski
   "",                  // Italiano
   "",                  // Nederlands
   "",                          // Portugu�s
   "G�n�ralit�",                // Fran�ais
   "",                          // Norsk
   "",                   // suomi (Finnish)
   "",                          // Polski
   "General",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "",                      // Italiano
   "",                      // Nederlands
   "",                          // Portugu�s
   "Nom",                       // Fran�ais
   "",                          // Norsk
   "",                      // suomi (Finnish)
   "",                          // Polski
   "Nombre",                    // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Width",                     // English
   "Weite",                     // Deutsch
   "",                          // Slovenski
   "",                 // Italiano
   "",                   // Nederlands
   "",                          // Portugu�s
   "Largeur",                   // Fran�ais
   "",                          // Norsk
   "",                    // suomi (Finnish)
   "",                          // Polski
   "Anchura",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Button$Set",                // English
   "�ndern",                    // Deutsch
   "",                          // Slovenski
   "",                   // Italiano
   "",                // Nederlands
   "",                          // Portugu�s
   "Changer",                   // Fran�ais
   "",                          // Norsk
   "",                     // suomi (Finnish)
   "",                          // Polski
   "Cambiar",                   // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"TrueType Font",             // English
   "TrueType Schrift",          // Deutsch
   "",                          // Slovenski
   "",        // Italiano
   "",             // Nederlands
   "",                          // Portugu�s
   "TrueType",                  // Fran�ais
   "",                          // Norsk
   "",         // suomi (Finnish)
   "",                          // Polski
   "Fuente TrueType",           // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"No TTF support!",            // English
   "TTF nicht unterst�tzt!",    // Deutsch
   "",                          // Slovenski
   "",      // Italiano
   "",   // Nederlands
   "",                          // Portugu�s
   "Les polices TrueType ne sont pas support�es!", // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "�Sin soporte TTF!",         // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
  {"Fixed Font",                // English
   "Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "",           // Italiano
   "",         // Nederlands
   "",                          // Portugu�s
   "Police avec une largeur fixe", // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Fuente Fija",               // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#ifdef USE_PLUGIN_AVARDS
  {"Dynamic OSD size",          // English
   "Dynamische OSD-Gr��e",      // Deutsch
   "",                          // Slovenski
   "",   // Italiano
   "",      // Nederlands
   "",                          // Portugu�s
   "Taille OSD dynamique",      // Fran�ais
   "",                          // Norsk
   "", // suomi (Finnish)
   "",                          // Polski
   "Tama�o OSD din�mico",       // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",      // �esky (Czech)
   },
#endif
  {"No TrueType fonts installed!",          // English
   "Keine TrueType Schriften installiert!", // Deutsch
   "",                                      // Slovenski
   "", // Italiano
   "",   // Nederlands
   "",                                      // Portugu�s
   "Il n'y a pas de police TrueType d'install�!",  // Fran�ais
   "",                                      // Norsk
   "",   // suomi (Finnish)
   "",                                      // Polski
   "�Ninguna fuente TrueType instalada!",   // Espa�ol
   "",                                      // �������� (Greek)
   "",                                      // Svenska
   "",                                      // Rom�n�
   "",                                      // Magyar
   "",                                      // Catal�
   "",                                      // ������� (Russian)
   "",                                      // Hrvatski
   "",                                      // Eesti
   "",                                      // Dansk
   "",      // �esky (Czech)
   },
  {"Content: ",                             // English
   "",                                      // Deutsch
   "",                                      // Slovenski
   "",                                      // Italiano
   "",                                      // Nederlands
   "",                                      // Portugu�s
   "",                                      // Fran�ais
   "",                                      // Norsk
   "",                                      // suomi (Finnish)
   "",                                      // Polski
   "Contenido: ",                           // Espa�ol
   "",                                      // �������� (Greek)
   "",                                      // Svenska
   "",                                      // Rom�n�
   "",                                      // Magyar
   "",                                      // Catal�
   "",                                      // ������� (Russian)
   "",                                      // Hrvatski
   "",                                      // Eesti
   "",                                      // Dansk
   "",      // �esky (Czech)
   },
  {NULL}
};
// vim:et:sw=2:ts=2:

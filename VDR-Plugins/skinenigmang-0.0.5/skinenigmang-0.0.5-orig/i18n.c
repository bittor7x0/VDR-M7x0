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
 *  - Español:    ???
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
   "ponedeljek",   // Slovenski
   "Lunedì",       // Italiano
   "maandag",      // Nederlands
   "Segunda",      // Português
   "Lundi",        // Français
   "Manday",       // Norsk
   "Maanantai",    // suomi (Finnish)
   "poniedzia³ek", // Polski
   "Lunes",        // Español
   "Äåõôİñá",      // ÅëëçíéêÜ (Greek)
   "Måndag",       // Svenska
   "Luni",         // Românã
   "Hétfõ",        // Magyar
   "Dilluns",      // Català
   "¿ŞİÕÔÕÛìİØÚ",  // ÀãááÚØÙ (Russian)
   "Ponedjeljak",  // Hrvatski
   "Esmaspäev",    // Eesti
   "Mandag",       // Dansk
   "Pondìlí",      // Èesky (Czech)
   },
  {"Tuesday",      // English
   "Dienstag",     // Deutsch
   "Torek",        // Slovenski
   "Martedì",      // Italiano
   "Dinsdag",      // Nederlands
   "Terça",        // Português
   "Mardi",        // Français
   "Tirsday",      // Norsk
   "Tiistai",      // suomi (Finnish)
   "Wtorek",       // Polski
   "Martes",       // Español
   "Ôñßôç",        // ÅëëçíéêÜ (Greek)
   "Tisdag",       // Svenska
   "Marşi",        // Românã
   "Kedd",         // Magyar
   "Dimarts",      // Català
   "²âŞàİØÚ",      // ÀãááÚØÙ (Russian)
   "Utorak",       // Hrvatski
   "Teisipäev",    // Eesti
   "Tirsdag",      // Dansk
   "Úterı",        // Èesky (Czech)
   },
  {"Wednesday",    // English
   "Mittwoch",     // Deutsch
   "sreda",        // Slovenski
   "mercoledì",    // Italiano
   "woensdag",     // Nederlands
   "Quarta",       // Português
   "Mercredi",     // Français
   "Onsday",       // Norsk
   "Keskiviikko",  // suomi (Finnish)
   "¶roda",        // Polski
   "Miércoles",    // Español
   "ÔåôÜñôç",      // ÅëëçíéêÜ (Greek)
   "Onsdag",       // Svenska
   "Miercuri",     // Românã
   "Szerda",       // Magyar
   "Dimecres",     // Català
   "ÁàÕÔĞ",        // ÀãááÚØÙ (Russian)
   "Srijeda",      // Hrvatski
   "Kolmapäev",    // Eesti
   "Onsdag",       // Dansk
   "Støeda",       // Èesky (Czech)
   },
  {"Thursday",     // English
   "Donnerstag",   // Deutsch
   "èetrtek",      // Slovenski
   "Giovedì",      // Italiano
   "Donderdag",    // Nederlands
   "Quinta",       // Português
   "Jeudi",        // Français
   "Torsdag",      // Norsk
   "Torstai",      // suomi (Finnish)
   "Czwartek",     // Polski
   "Jueves",       // Español
   "Ğİìğôç",       // ÅëëçíéêÜ (Greek)
   "Torsdag",      // Svenska
   "Joi",          // Românã
   "Csütörtök",    // Magyar
   "Dijous",       // Català
   "ÇÕâÒÕàÓ",      // ÀãááÚØÙ (Russian)
   "Èetvrtak",     // Hrvatski
   "Neljapäev",    // Eesti
   "Torsdag",      // Dansk
   "Ètvrtek",      // Èesky (Czech)
   },
  {"Friday",       // English
   "Freitag",      // Deutsch
   "Petek",        // Slovenski
   "Venerdì",      // Italiano
   "Vrijdag",      // Nederlands
   "Sexta",        // Português
   "Vendredi",     // Français
   "Fredag",       // Norsk
   "Perjantai",    // suomi (Finnish)
   "Pi±tek",       // Polski
   "Viernes",      // Español
   "ĞáñáóêåõŞ",    // ÅëëçíéêÜ (Greek)
   "Fredag",       // Svenska
   "Vineri",       // Românã
   "Péntek",       // Magyar
   "Divendres",    // Català
   "¿ïâİØæĞ",      // ÀãááÚØÙ (Russian)
   "Petak",        // Hrvatski
   "Reede",        // Eesti
   "Fredag",       // Dansk
   "Pátek",        // Èesky (Czech)
   },
  {"Saturday",     // English
   "Samstag",      // Deutsch
   "Sobota",       // Slovenski
   "Sabato",       // Italiano
   "Zaterdag",     // Nederlands
   "Sábado",       // Português
   "Samedi",       // Français
   "Lørdag",       // Norsk
   "Lauantai",     // suomi (Finnish)
   "Sobota",       // Polski
   "Sábado",       // Español
   "ÓÜââáôï",      // ÅëëçíéêÜ (Greek)
   "Lördag",       // Svenska
   "Sâmbãtã",      // Românã
   "Szombat",      // Magyar
   "Dissabte",     // Català
   "ÁãÑÑŞâĞ",      // ÀãááÚØÙ (Russian)
   "Subota",       // Hrvatski
   "Laupäev",      // Eesti
   "Lørdag",       // Dansk
   "Sobota",       // Èesky (Czech)
   },
  {"Sunday",       // English
   "Sonntag",      // Deutsch
   "Nedelja",      // Slovenski
   "Domenica",     // Italiano
   "Zondag",       // Nederlands
   "Domingo",      // Português
   "Dimanche",     // Français
   "Søndag",       // Norsk
   "Sunnuntai",    // suomi (Finnish)
   "Niedziela",    // Polski
   "Domingo",      // Español
   "ÊõñéáêŞ",      // ÅëëçíéêÜ (Greek)
   "Söndag",       // Svenska
   "Duminicã",     // Românã
   "Vasárnap",     // Magyar
   "Diumenge",     // Català
   "²ŞáÚàÕáÕİìÕ",  // ÀãááÚØÙ (Russian)
   "Nedjelja",     // Hrvatski
   "Pühapäev",     // Eesti
   "Søndag",       // Dansk
   "Nedìle",       // Èesky (Czech)
   },
  {"EnigmaNG skin",             // English
   "EnigmaNG Oberfläche",       // Deutsch
   "",                          // Slovenski
   "Interfaccia EnigmaNG",      // Italiano
   "EnigmaNG skin",             // Nederlands
   "",                          // Português
   "Skin EnigmaNG",             // Français
   "",                          // Norsk
   "EnigmaNG-ulkoasu",          // suomi (Finnish)
   "Skin EnigmaNG",             // Polski
   "Piel EnigmaNG",             // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "EnigmaNG-temat",            // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "EnigmaNG áÚØİ",             // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "EnigmaNG kest",             // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"EnigmaNG",                  // English
   "EnigmaNG",                  // Deutsch
   "",                          // Slovenski
   "EnigmaNG",                  // Italiano
   "EnigmaNG",                  // Nederlands
   "",                          // Português
   "EnigmaNG",                  // Français
   "",                          // Norsk
   "EnigmaNG",                  // suomi (Finnish)
   "EnigmaNG",                  // Polski
   "EnigmaNG",                  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "EnigmaNG",                  // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "EnigmaNG",                  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "EnigmaNG",                  // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"min",                       // English
   "min",                       // Deutsch
   "",                          // Slovenski
   "min",                       // Italiano
   "min",                       // Nederlands
   "",                          // Português
   "min",                       // Français
   "",                          // Norsk
   "min",                       // suomi (Finnish)
   "min",                       // Polski
   "min",                       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "min",                       // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÜØİ",                       // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "min",                       // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Stereo",                    // English
   "Stereo",                    // Deutsch
   "",                          // Slovenski
   "Stereo",                    // Italiano
   "Stereo",                    // Nederlands
   "",                          // Português
   "Stéréo",                    // Français
   "",                          // Norsk
   "Stereo",                    // suomi (Finnish)
   "Stereo",                    // Polski
   "Estereo",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Stereo",                    // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÁâÕàÕŞ",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Stereo",                    // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Left",                      // English
   "Links",                     // Deutsch
   "",                          // Slovenski
   "Sinistra",                  // Italiano
   "Links",                     // Nederlands
   "",                          // Português
   "Gauche",                    // Français
   "",                          // Norsk
   "Vasen",                     // suomi (Finnish)
   "W lewo",                    // Polski
   "Izquierda",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Vänster",                   // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "»ÕÒëÙ",                     // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Vasak",                     // Eesti
   "",                          // Danske
   "",                          // Èesky (Czech)
   },
  {"Right",                     // English
   "Rechts",                    // Deutsch
   "",                          // Slovenski
   "Destra",                    // Italiano
   "Rechts",                    // Nederlands
   "",                          // Português
   "Droite",                    // Français
   "",                          // Norsk
   "Oikea",                     // suomi (Finnish)
   "W prawo",                   // Polski
   "Derecha",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Höger",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿àĞÒëÙ",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Parem",                     // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "Muto",                      // Italiano
   "Geluid uit",                // Nederlands
   "",                          // Português
   "Muet",                      // Français
   "",                          // Norsk
   "Mykistetty",                // suomi (Finnish)
   "Wycisz",                    // Polski
   "Silenciar",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Dämpa",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "²ëÚÛ. ×ÒãÚ",                // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Hääletu",                   // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Volume",                    // English
   "Lautstärke",                // Deutsch
   "",                          // Slovenski
   "Volume",                    // Italiano
   "Volume",                    // Nederlands
   "",                          // Português
   "Volume",                    // Français
   "",                          // Norsk
   "Äänenvoimakkuus",           // suomi (Finnish)
   "G³o¶no¶æ",                  // Polski
   "Volumen",                   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Volym",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "³àŞÜÚŞáâì",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Helitugevus",               // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show channel logos",        // English
   "Kanal-Logos anzeigen",      // Deutsch
   "",                          // Slovenski
   "Mostra loghi canali",       // Italiano
   "Toon kanaallogo's",         // Nederlands
   "",                          // Português
   "Afficher logos des chaînes", // Français
   "",                          // Norsk
   "Näytä kanavalogot",         // suomi (Finnish)
   "Pokazuj logo kana³u",       // Polski
   "Mostrar los logos de los canales",  // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Visa kanallogotyper",       // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì ÛŞÓŞ ÚĞİĞÛŞÒ",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Kanalilogo näitamine",      // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Identify channel by",       // English
   "  Kanal-Identifikation durch",        // Deutsch
   "",                          // Slovenski
   "  Identifica canale per",     // Italiano
   "  Kanaalidentificatie door",  // Nederlands
   "",                          // Português
   "  Identifier chaîne par",     // Français
   "",                          // Norsk
   "  Tunnista kanava",           // suomi (Finnish)
   "  Rozpoznaj kana³ po",        // Polski
   "  Identificar el canal por...",       // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "  Identifiera kanal med",     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "  ¾ßàÕÔÕÛïâì ÚĞİĞÛ ßŞ",       // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "  Kanali tuvastamise meetod", // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "nome",                      // Italiano
   "naam",                      // Nederlands
   "",                          // Português
   "Nom",                       // Français
   "",                          // Norsk
   "nimestä",                   // suomi (Finnish)
   "nazwa",                     // Polski
   "Nombre",                    // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "namn",                      // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "İĞ×ÒĞİØî",                  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "nimi",                      // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"data",                      // English
   "ID",                        // Deutsch
   "",                          // Slovenski
   "dati",                      // Italiano
   "data",                      // Nederlands
   "",                          // Português
   "Donnée",                    // Français
   "",                          // Norsk
   "tiedoista",                 // suomi (Finnish)
   "parametry",                 // Polski
   "Datos",                     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "data",                      // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÔĞİİëÜ",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "andmed",                    // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Channel logo cache size",   // English
   "Größe des Logo-Caches",     // Deutsch
   "",                          // Slovenski
   "Dimensione cache dei loghi",// Italiano
   "Kanaallogo cache",          // Nederlands
   "",                          // Português
   "Taille du cache des logos",  // Français
   "",                          // Norsk
   "Välimuistin koko kanavalogoille",   // suomi (Finnish)
   "Rozmiar cache dla logo kana³u",     // Polski
   "Tamaño de la cache de los logos",   // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Cachestorlek för logotyper",        // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÀĞ×ÜÕà ÚÕèĞ ÛŞÓoâØßŞÒ ÚĞİĞÛŞÒ",  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Kanalilogo vahemälu suurus",        // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Button$Flush cache",        // English
   "Cache leeren",              // Deutsch
   "",                          // Slovenski
   "Pulisci cache",             // Italiano
   "Cache legen",               // Nederlands
   "",                          // Português
   "Supprimer cache",           // Français
   "",                          // Norsk
   "Tyhjennä",                  // suomi (Finnish)
   "Opró¿nij cache",            // Polski
   "Limpiar la cache",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Töm cachen",                // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¾çØáâØâì ÚÕè",              // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Tühjenda",                  // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Flushing channel logo cache...",    // English
   "Logo-Cache wird geleert...",        // Deutsch
   "",                          // Slovenski
   "Pulendo la cache...",       // Italiano
   "Kanaallogo cache legen...", // Nederlands
   "",                          // Português
   "Suppression du cache...",   // Français
   "",                          // Norsk
   "Tyhjennetään välimuistia...",       // suomi (Finnish)
   "Opró¿niam cache logo kana³u...",    // Polski
   "Limpiando la cache...",     // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Tömmer cachen...",          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¾çØéĞî ÚÕè...",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Kanalilogo vahemälu tühjendamine...",       // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Languages",                 // English
   "Sprachen",                  // Deutsch
   "",                          // Slovenski
   "Lingue",                    // Italiano
   "Talen",                     // Nederlands
   "",                          // Português
   "Langues",                   // Français
   "",                          // Norsk
   "Kielet",                    // suomi (Finnish)
   "Jêzyki",                    // Polski
   "Lenguajes",                 // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Språk",                     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "Ï×ëÚØ",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Keeled",                    // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Auxiliary information",     // English
   "Zusatzinformation",         // Deutsch
   "",                          // Slovenski
   "Informazione ausiliare",    // Italiano
   "Hulp informatie",           // Nederlands
   "",                          // Português
   "Information auxiliare",     // Français
   "",                          // Norsk
   "Lisätiedot",                // suomi (Finnish)
   "Informacje pomocnicze",     // Polski
   "Información auxiliar",      // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Extrainformation",          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÀĞáè. ØİäŞàÜĞæØï",          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Lisainfo",                  // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show auxiliary information",        // English
   "Zusatzinfo anzeigen",       // Deutsch
   "",                          // Slovenski
   "Mostra informazione ausiliare",     // Italiano
   "Toon hulp informatie",      // Nederlands
   "",                          // Português
   "Afficher informations auxiliaires",   // Français
   "",                          // Norsk
   "Näytä lisätiedot",          // suomi (Finnish)
   "Pokazuj informacje pomocnicze",     // Polski
   "Mostrar información auxiliar",      // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Visa extrainformation",     // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì àĞáè. ØİäŞàÜĞæØî",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Lisainfo näitamine",        // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show remaining/elapsed time",          // English
   "Zeige abgel./restl. Zeit",             // Deutsch
   "",                          // Slovenski
   "Mostra tempo rimasto/trascorso",       // Italiano
   "Toon resterend/gedane tijd",  // Nederlands
   "",                          // Português
   "Afficher le temps écoulé/restant", // Français
   "",                          // Norsk
   "Näytä tapahtuman aika",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì ŞáâĞÒ./ßàŞèÕÔ. ÒàÕÜï",     // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"remaining",                 // English
   "restliche",                 // Deutsch
   "",                          // Slovenski
   "rimasto",                   // Italiano
   "resterend",                   // Nederlands
   "",                          // Português
   "restant",                   // Français
   "",                          // Norsk
   "jäljellä oleva",            // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ŞáâĞÛìİëÕ",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"elapsed",                   // English
   "abgelaufene",               // Deutsch
   "",                          // Slovenski
   "trascorso",                 // Italiano
   "gedane",                // Nederlands
   "",                          // Português
   "écoulé",                    // Français
   "",                          // Norsk
   "kulunut",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ØáâÕÚèØÕ",                  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show info area in main menu",            // English
   "Infobereich im Hauptmenü",               // Deutsch
   "",                          // Slovenski
   "Mostra area info nel menu principale",   // Italiano
   "Info in hoofdmenu",         // Nederlands
   "",                          // Português
   "Surface d'infos dans le menu principal",  // Français
   "",                          // Norsk
   "Näytä infoalue päävalikossa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì Øİä. ŞÑÛĞáìâ Ò ÓÛ. ÜÕİî",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show marker in lists",            // English
   "Symbol vor Listeneinträgen",      // Deutsch
   "",                          // Slovenski
   "Mostra indicatore negli elenchi", // Italiano
   "Markering in lijsten tonen", // Nederlands
   "",                          // Português
   "Marque devant les listes",  // Français
   "",                          // Norsk
   "Näytä valintasymboli",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì ÜÕâÚã Ò áßØáÚĞå",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show progressbar",          // English
   "Fortschrittbalken anzeigen",        // Deutsch
   "",                          // Slovenski
   "Mostra barra avanzamento",  // Italiano
   "Progressiebalk tonen",      // Nederlands
   "",                          // Português
   "Afficher barre de progression",     // Français
   "",                          // Norsk
   "Näytä aikajana",            // suomi (Finnish)
   "Pokazuj pasek postêpu",     // Polski
   "Mostrar barra de progreso", // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Visa framsteg",             // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì ØİÔØÚĞâŞà ßàŞÔÒØÖÕİØï",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Edenemisriba näitamine",    // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show symbols",              // English
   "Symbole anzeigen",          // Deutsch
   "",                          // Slovenski
   "Mostra simboli",            // Italiano
   "Symbolen tonen",            // Nederlands
   "",                          // Português
   "Afficher symbole",          // Français
   "",                          // Norsk
   "Näytä symbolit",            // suomi (Finnish)
   "Pokazuj symbole",           // Polski
   "Mostrar símbolos",          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "Visa symboler",             // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì áØÜÒŞÛë",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "Sümbolite näitamine",       // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show VPS",                  // English
   "VPS anzeigen",              // Deutsch
   "",                          // Slovenski
   "Mostra VPS",                // Italiano
   "VPS tonen",                 // Nederlands
   "",                          // Português
   "Afficher le VPS",           // Français
   "",                          // Norsk
   "Näytä VPS-tieto",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì VPS",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"WARNING",                   // English
   "WARNUNG",                   // Deutsch
   "",                          // Slovenski
   "ATTENZIONE",                // Italiano
   "WAARSCHUWING",              // Nederlands
   "",                          // Português
   "ATTENTION",                 // Français
   "",                          // Norsk
   "VAROITUS",                  // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿Àµ´Ã¿Àµ¶´µ½¸µ",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Timer conflict",            // English
   "Timerkonflikt",             // Deutsch
   "",                          // Slovenski
   "Conflitto timer",           // Italiano
   "Timerconflict",             // Nederlands
   "",                          // Português
   "Conflit de programmation",  // Français
   "",                          // Norsk
   "Päällekkäinen ajastin",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ºŞİäÛØÚâ âĞÙÜÕàĞ",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Timer conflicts",           // English
   "Timerkonflikte",            // Deutsch
   "",                          // Slovenski
   "Conflitti timer",           // Italiano
   "Timerconflicten",                          // Nederlands
   "",                          // Português
   "Conflits de programmation", // Français
   "",                          // Norsk
   "Päällekkäisiä ajastimia",   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ºŞİäÛØÚâë âĞÙÜÕàŞÒ",        // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"TIMERS",                    // English
   "TIMER",                     // Deutsch
   "",                          // Slovenski
   "TIMER",                     // Italiano
   "TIMER",                     // Nederlands
   "",                          // Português
   "PROGRAMMATION",             // Français
   "",                          // Norsk
   "AJASTIMET",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "Â°¹¼µÀË",                   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"RERUNS OF THIS SHOW",       // English
   "WIEDERHOLUNGEN",            // Deutsch
   "",                          // Slovenski
   "RIPETI QUESTO SHOW",        // Italiano
   "HERHALINGEN",               // Nederlands
   "",                          // Português
   "REPETITION",                // Français
   "",                          // Norsk
   "TOISTUVAT TAPAHTUMAT",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿¾²Â¾Àµ½¸Ï",                // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"*** Invalid Channel ***",   // English
   "*** Ungültiger Kanal ***",  // Deutsch
   "",                          // Slovenski
   "*** Canale non valido ***", // Italiano
   "*** Ongeldig Kanaal ***",   // Nederlands
   "",                          // Português
   "*** Chaîne non valable ***",  // Français
   "",                          // Norsk
   "*** Virheellinen kanava ***", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "*** ½ÕßàĞÒØÛìİëÙ ÚĞİĞÛ ***",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Size",                      // English
   "Größe",                     // Deutsch
   "",                          // Slovenski
   "Dimensione",                // Italiano
   "Grootte",                   // Nederlands
   "",                          // Português
   "Taille",                    // Français
   "",                          // Norsk
   "Koko",                      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÀĞ×ÜÕà",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "Muto",                      // Italiano
   "Geluid uit",                // Nederlands
   "Muet",                      // Português
   "",                          // Français
   "",                          // Norsk
   "Mykistetty",                // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "²ëÚÛ.×ÒãÚ",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Volume",                    // English
   "Lautstärke",                // Deutsch
   "",                          // Slovenski
   "Volume",                    // Italiano
   "Volume",                    // Nederlands
   "",                          // Português
   "Volume",                    // Français
   "",                          // Norsk
   "Äänenvoimakkuus",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "³àŞÜÚŞáâì",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show symbols in lists",     // English
   "Symbole in Listen",         // Deutsch
   "",                          // Slovenski
   "Mostra simboli negli elenchi",       // Italiano
   "Symbolen in lijsten",       // Nederlands
   "",                          // Português
   "Symboles dans les listes",   // Français
   "",                          // Norsk
   "Näytä tapahtumien symbolit",// suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì áØÜÒŞÛë Ò áßØáÚĞå",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Try 8bpp single area",      // English
   "Einen 8bpp Bereich versuchen", // Deutsch
   "",                          // Slovenski
   "Prova area singola a 8bpp", // Italiano
   "Op 8bpp bereik testen",     // Nederlands
   "",                          // Português
   "Essayer une surface à 8bpp",// Français
   "",                          // Norsk
   "Suosi yhtä 8bpp kuva-aluetta", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show flags",                // English
   "Fahnen anzeigen",           // Deutsch
   "",                          // Slovenski
   "Mostra segni",              // Italiano
   "Vlaggen tonen",             // Nederlands
   "",                          // Português
   "Afficher drapeau",          // Français
   "",                          // Norsk
   "Näytä liput",               // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×Ğâì äÛĞÓØ",            // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show symbols in menu",      // English
   "Symbole im Menü",           // Deutsch
   "",                          // Slovenski
   "Mostra simboli nel menu",   // Italiano
   "Symbolen in menu",          // Nederlands
   "",                          // Português
   "Symboles dans le menu",     // Français
   "",                          // Norsk
   "Näytä valikkosymbolit",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×Ğâì áØÜÒŞÛë Ò ÜÕİî",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"never",                     // English
   "nie",                       // Deutsch
   "",                          // Slovenski
   "mai",                       // Italiano
   "nooit",                     // Nederlands
   "",                          // Português
   "jamais",                    // Français
   "",                          // Norsk
   "ei koskaan",                // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "İØÚŞÓÔĞ",                   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"always",                    // English
   "immer",                     // Deutsch
   "",                          // Slovenski
   "sempre",                    // Italiano
   "Altijd",                    // Nederlands
   "",                          // Português
   "toujours",                  // Français
   "",                          // Norsk
   "aina",                      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÒáÕÓÔĞ",                    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"percent",                   // English
   "Prozent",                   // Deutsch
   "",                          // Slovenski
   "percentuale",               // Italiano
   "procent",                   // Nederlands
   "",                          // Português
   "Pourcentage",               // Français
   "",                          // Norsk
   "prosentti",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ßàŞæÕİâ",                   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"if exists",                 // English
   "wenn vorhanden",            // Deutsch
   "",                          // Slovenski
   "se esiste",                 // Italiano
   "wanneer aanwezig",          // Nederlands
   "",                          // Português
   "si existant",               // Français
   "",                          // Norsk
   "jos olemassa",              // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÕáÛØ Ò İĞÛØçØØ",            // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Number of Reruns",          // English
   "Anzahl Wiederholungen",     // Deutsch
   "",                          // Slovenski
   "Numero di ripetizioni",     // Italiano
   "aantal herhalingen",        // Nederlands
   "",                          // Português
   "Nombre de répétitions",     // Français
   "",                          // Norsk
   "Uusintojen lukumäärä",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ºŞÛØçÕáâÒŞ ßŞÒâŞàÕİØÙ",     // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Use Subtitle for reruns",   // English
   "Verw. Untertitel für Wdh.", // Deutsch
   "",                          // Slovenski
   "Usa sottotitolo per ripetizioni",         // Italiano
   "Gebr. ondertitlel voor herh.", // Nederlands
   "",                          // Português
   "Utilise sous-titres pour les répétitions", // Français
   "",                          // Norsk
   "Käytä lyhyttä kuvausta uusinnoille", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¸áßŞÛì×. ßŞÔ×ĞÓŞÛŞÒÚØ ÔÛï ßŞÒâŞàÕİØï",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show timer conflicts",      // English
   "Timerkonflikte anzeigen",   // Deutsch
   "",                          // Slovenski
   "Mostra conflitti timer",    // Italiano
   "Timerconflict tonen",       // Nederlands
   "",                          // Português
   "Afficher les conflits de programmation", // Français
   "",                          // Norsk
   "Näytä päällekkäiset ajastimet", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×Ğâì ÚŞİäÛØÚâë âĞÙÜÕàŞÒ",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Timer check",               // English
   "Überwachung",               // Deutsch
   "",                          // Slovenski
   "Verifica timer",            // Italiano
   "Monitoren",                 // Nederlands
   "",                          // Português
   "Observation de la programmation", // Français
   "",                          // Norsk
   "Ajastimien valvonta",       // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿àŞÒÕàÚĞ âĞÙÜÕàŞÒ",         // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"EPGSearch",                 // English
   "EPGSearch",                 // Deutsch
   "",                          // Slovenski
   "Ricerca EPG",               // Italiano
   "EPGsearch",                 // Nederlands
   "",                          // Português
   "Recherche EPG",             // Français
   "",                          // Norsk
   "EPGSearch",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Logos & Symbols",           // English
   "Logos & Symbole",           // Deutsch
   "",                          // Slovenski
   "Loghi e simboli",           // Italiano
   "Logo's & Symbolen",         // Nederlands
   "",                          // Português
   "Logos et symboles",         // Français
   "",                          // Norsk
   "Logot ja symbolit",         // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "»ŞÓŞâØßë & áØÜÒŞÛë",        // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"use size.vdr only",         // English
   "nur size.vdr verwenden",    // Deutsch
   "",                          // Slovenski
   "usa solo size.vdr",         // Italiano
   "alleen size.vdr gebruiken", // Nederlands
   "",                          // Português
   "utiliser seulement size.vdr",  // Français Patrice Staudt 29.03.2007
   "",                          // Norsk
   "vain jos size.vdr",         // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ØáßŞÛì×ŞÒĞâì âŞÛìÚŞ size.vdr",  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show recording's size",     // English
   "Aufnahmengröße anzeigen",   // Deutsch
   "",                          // Slovenski
   "Mostra dimensione delle registrazioni",      // Italiano
   "Opnamegrootte tonen",       // Nederlands
   "",                          // Português
   "Afficher la taille des enregistrements",   // Français
   "",                          // Norsk
   "Näytä tallenteen koko",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×ëÒĞâì àĞ×ÜÕàë ×ĞßØáÕÙ",    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"pixel algo",                // English
   "Pixel-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "algoritmo pixel",           // Italiano
   "pixelalgoritme",            // Nederlands
   "",                          // Português
   "Algorithme pixel",          // Français
   "",                          // Norsk
   "näytteistys",               // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ĞÛÓŞàØâÜ ßØÚáÕÛï",          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"ratio algo",                // English
   "Ratio-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "algoritmo ratio",           // Italiano
   "algoritmeratio",            // Nederlands
   "",                          // Português
   "Algorithme ratio",          // Français
   "",                          // Norsk
   "skaalaus",                  // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ĞÛÓŞàØâÜ áŞŞâİŞèÕİØï",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"zoom image",                // English
   "Zoomen",                    // Deutsch
   "",                          // Slovenski
   "ingrandisci immagine",      // Italiano
   "zoomen",                    // Nederlands
   "",                          // Português
   "Agrandir image",            // Français
   "",                          // Norsk
   "zoomaus",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "ÃÒÕÛØçØâì",                 // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Show event/recording images",     // English
   "Bilder in EPG/Aufnahmendetails",  // Deutsch
   "",                                // Slovenski
   "Mostra immagini evento/registrazione",      // Italiano
   "EPG/opnamedetails tonen",         // Nederlands
   "",                                // Português
   "Images dans les EPG et détails d'enregistrements",     // Français
   "",                                // Norsk
   "Näytä kuvat lisätietovalikossa",  // suomi (Finnish)
   "",                                // Polski
   "",                                // Español
   "",                                // ÅëëçíéêÜ (Greek)
   "",                                // Svenska
   "",                                // Românã
   "",                                // Magyar
   "",                                // Català
   "¸×ŞÑàĞÖÕİØï Ò EPG",               // ÀãááÚØÙ (Russian)
   "",                                // Hrvatski
   "",                                // Eesti
   "",                                // Dansk
   "",                                // Èesky (Czech)
   },
  {"  Resize images",           // English
   "  Bildberechnung benutzt",  // Deutsch
   "",                          // Slovenski
   "  Ridimensiona immagini",   // Italiano
   "  Aanpassen beeldgrootte",  // Nederlands
   "",                          // Português
   "  Redimensionne les images",  // Français
   "",                          // Norsk
   "  Muokkaa kuvien kokoa",    // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "  ¿ÕàÕàÕèĞâì ØáßŞÛì×ãï",    // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Image width",             // English
   "  Bildbreite",              // Deutsch
   "",                          // Slovenski
   "  Lunghezza immagine",      // Italiano
   "  Beeldbreedte",            // Nederlands
   "",                          // Português
   "  Largeur",                 // Français
   "",                          // Norsk
   "  Kuvien leveys",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "  ÈØàØİĞ Ø×ŞÑàĞÖÕİØï",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Image height",            // English
   "  Bildhöhe",                // Deutsch
   "",                          // Slovenski
   "  Altezza immagine",        // Italiano
   "  Beeldhoogte",             // Nederlands
   "",                          // Português
   "  Hauteur",                 // Français
   "",                          // Norsk
   "  Kuvien korkeus",          // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "  ²ëáŞâĞ Ø×ŞÑàĞÖÕİØï",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Image format",            // English
   "  Bildformat",              // Deutsch
   "",                          // Slovenski
   "  Formato immagine",        // Italiano
   "  Beeldformaat",            // Nederlands
   "",                          // Português
   "  Format d'image",          // Français
   "",                          // Norsk
   "  Kuvaformaatti",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "  ÄŞàÜĞâ Ø×ŞÑàĞÖÕİØï",      // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Full title width",          // English
   "Volle Titelbreite",         // Deutsch
   "",                          // Slovenski
   "Lunghezza titolo completo", // Italiano
   "Volle titelbreedte",        // Nederlands
   "",                          // Português
   "Largeur totale pour le titre", // Français
   "",                          // Norsk
   "Levitä otsikkopalkki",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÛİĞï èØàØİĞ ×ĞÓŞÛŞÒÚĞ",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
#ifdef USE_PLUGIN_MAILBOX
  {"Show mail icon",            // English
   "Zeige Mail-Icon",           // Deutsch
   "",                          // Slovenski
   "Mostra icona email",        // Italiano
   "Mail-ikoon tonen",          // Nederlands
   "",                          // Português
   "Afficher l'icône courriel",  // Français
   "",                          // Norsk
   "Näytä sähköpostikuvake",    // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "¿ŞÚĞ×Ğâì ×İĞçŞÚ íÛÕÚâ. ßŞçâë",   // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"only if new mail present",  // English
   "nur bei neuer Mail",        // Deutsch
   "",                          // Slovenski
   "solo se presenti nuove email",          // Italiano
   "alleen bij nieuwe mail",    // Nederlands
   "",                          // Português
   "juste lors d'un courriel",  // Français
   "",                          // Norsk
   "jos uutta postia",          // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "âŞÛìÚŞ ßàØ İŞÒŞÜ áŞŞÑéÕİØØ",  // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
#endif
#ifndef DISABLE_ANIMATED_TEXT
  {"Animated Text",             // English
   "Bewegter Text",             // Deutsch
   "",                          // Slovenski
   "Testo animato",             // Italiano
   "Geanimeerde tekst",         // Nederlands
   "",                          // Português
   "Texte défilant",            // Français
   "",                          // Norsk
   "Tekstien elävöinti",        // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Enable",                    // English
   "Verwenden",                 // Deutsch
   "",                          // Slovenski
   "Abilita",                   // Italiano
   "Toelaten",                  // Nederlands
   "",                          // Português
   "Permettre",                 // Français
   "",                          // Norsk
   "Käytä elävöintiä",          // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scoll behaviour",         // English
   "  Scroll-Verhalten",        // Deutsch
   "",                          // Slovenski
   "  Comportamento scorrimento",// Italiano
   "  Scroll gedrag",            // Nederlands
   "",                          // Português
   "  Comportement de défilement", // Français
   "",                          // Norsk
   "  Vieritystapa",            // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"to the left",               // English
   "nach links",                // Deutsch
   "",                          // Slovenski
   "verso la sinistra",         // Italiano
   "naar links",                // Nederlands
   "",                          // Português
   "vers la gauche",            // Français
   "",                          // Norsk
   "vasemmalle",                // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"left and right",            // English
   "links und rechts",          // Deutsch
   "",                          // Slovenski
   "sinistra e destra",         // Italiano
   "links en rechts",           // Nederlands
   "",                          // Português
   "gauche et droite",          // Français
   "",                          // Norsk
   "edestakaisin",              // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scroll delay (ms)",       // English
   "  Scroll-Verzögerung (ms)", // Deutsch
   "",                          // Slovenski
   "  Ritardo scorrimento (ms)",// Italiano
   "  Scrollvertraging (ms)",   // Nederlands
   "",                          // Português
   "  Délai de défilement (ms)",// Français
   "",                          // Norsk
   "  Vierityksen viive (ms)",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scroll pause (ms)",       // English
   "  Scroll-Pause (ms)",       // Deutsch
   "",                          // Slovenski
   "  Pausa scorrimento (ms)",  // Italiano
   "  Scrollpause (ms)",        // Nederlands
   "",                          // Português
   "  Pause de défilement (ms)",// Français
   "",                          // Norsk
   "  Vierityksen tauko (ms)",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Blink pause (ms)",        // English
   "  Blink-Pause (ms)",        // Deutsch
   "",                          // Slovenski
   "  Pausa lampeggiante (ms)", // Italiano
   "  Blinkpauze (ms)",         // Nederlands
   "",                          // Português
   "  Pause de clignotement (ms)",// Français
   "",                          // Norsk
   "  Vilkutuksen tauko (ms)",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scroll OSD title",        // English
   "  OSD Titel scrollen",      // Deutsch
   "",                          // Slovenski
   "  Scorri titolo OSD",       // Italiano
   "  OSD titel scrollen",      // Nederlands
   "",                          // Português
   "  Défilement dans le titre du OSD",  // Français
   "",                          // Norsk
   "  Vieritä valikon otsikkoa",// suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scroll info area",        // English
   "  Infobereich scrollen",    // Deutsch
   "",                          // Slovenski
   "  Scorri informazioni area",// Italiano
   "  Infobereik scrollen",     // Nederlands
   "",                          // Português
   "  Barre de défilement dans la surface info",   // Français
   "",                          // Norsk
   "  Vieritä infoaluetta",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scroll active list items",  // English
   "  Aktive Listenzeile scrollen", // Deutsch
   "",                          // Slovenski
   "  Scorri valori lista attivi",// Italiano
   "  Lijst aktieve punten scrollen",  // Nederlands
   "",                          // Português
   "  Barre de défilement dans la liste active",  // Français
   "",                          // Norsk
   "  Vieritä aktiivista valintaa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Scroll other items",      // English
   "  Andere Bereiche scrollen", // Deutsch
   "",                          // Slovenski
   "  Scorri altri valori",     // Italiano
   "  Ander bereik scrollen",   // Nederlands
   "",                          // Português
   "  Barre de défilement dans d'autres parties",  // Français
   "",                          // Norsk
   "  Vieritä muita alueita",   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
#endif
  {"  Show symbols in menu",    // English
   "  Symbole im Menü",         // Deutsch
   "",                          // Slovenski
   "  Mostra simboli nel menu", // Italiano
   "  Symbolen in menu",   // Nederlands
   "",                          // Português
   "  dans le menu",  		// Français
   "",                          // Norsk
   "  Näytä symbolit valikossa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Show symbols in replay",  // English
   "  Symbole bei der Wiedergabe", // Deutsch
   "",                          // Slovenski
   "  Mostra simboli in esecuzione",// Italiano
   "  Symbolen bij weergave",   // Nederlands
   "",                          // Português
   "  dans lecture",		// Français
   "",                          // Norsk
   "  Näytä symbolit toistettaessa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Show symbols in messages",  // English
   "  Symbole bei den Meldungen", // Deutsch
   "",                          // Slovenski
   "  Mostra simboli nei messaggi",// Italiano
   "  Symbolen bij berichten",  // Nederlands
   "",                          // Português
   "  dans les messages", 	// Français
   "",                          // Norsk
   "  Näytä symbolit viesteissä", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"  Show symbols in audio",   // English
   "  Symbole bei Audio",       // Deutsch
   "",                          // Slovenski
   "  Mostra simboli in audio", // Italiano
   "  Symbolen bij audio",      // Nederlands
   "",                          // Português
   "  dans audio",        	// Français
   "",                          // Norsk
   "  Näytä symbolit äänivalikossa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Fonts",                     // English
   "Fonts",                     // Deutsch
   "",                          // Slovenski
   "Caratteri",                 // Italiano
   "Fonts",                     // Nederlands
   "",                          // Português
   "Les polices",               // Français
   "",                          // Norsk
   "Kirjasintyypit",            // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"OSD title",                 // English
   "OSD Titel",                 // Deutsch
   "",                          // Slovenski
   "Titolo OSD",                // Italiano
   "OSD titel",                 // Nederlands
   "",                          // Português
   "Titre OSD",                 // Français
   "",                          // Norsk
   "Valikon otsikko",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Date",                      // English
   "Datum",                     // Deutsch
   "",                          // Slovenski
   "Data",                      // Italiano
   "Datum",                     // Nederlands
   "",                          // Português
   "Date",                      // Français
   "",                          // Norsk
   "Päiväys",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Messages",                  // English
   "Meldungen",                 // Deutsch
   "",                          // Slovenski
   "Messaggi",                  // Italiano
   "Meldingen",                 // Nederlands
   "",                          // Português
   "Message",                   // Français
   "",                          // Norsk
   "Viestit",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Help keys",                 // English
   "Farb-Tasten",               // Deutsch
   "",                          // Slovenski
   "Tasti d'aiuto",             // Italiano
   "Help toetsen",              // Nederlands
   "",                          // Português
   "Touche couleur",            // Français
   "",                          // Norsk
   "Värinäppäimet",             // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Channelinfo: title",        // English
   "Kanalinfo: Titel",          // Deutsch
   "",                          // Slovenski
   "Info canale: titolo",       // Italiano
   "Kanaalinfo: titel",         // Nederlands
   "",                          // Português
   "Chaîne info: titre ",        // Français
   "",                          // Norsk
   "Kanavatieto: ohjelman nimi",// suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Channelinfo: subtitle",     // English
   "Kanalinfo: Untertitel",     // Deutsch
   "",                          // Slovenski
   "Info canale: sottotitolo",  // Italiano
   "Kanaalinfo: ondertitel",    // Nederlands
   "",                          // Português
   "Chaîne info: sous-titres",    // Français
   "",                          // Norsk
   "Kanavatieto: ohjelman kuvaus", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Channelinfo: language",     // English
   "Kanalinfo: Sprache",        // Deutsch
   "",                          // Slovenski
   "Info canale: lingua",       // Italiano
   "Kanaalinfo: taal",          // Nederlands
   "",                          // Português
   "Chaîne info: langue",       // Français
   "",                          // Norsk
   "Kanavatieto: kieli",        // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"List items",                // English
   "Listen",                    // Deutsch
   "",                          // Slovenski
   "Elenco",                    // Italiano
   "Lijsten",                   // Nederlands
   "",                          // Português
   "Liste",                     // Français
   "",                          // Norsk
   "Listat",                    // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Info area: timers title",   // English
   "Infobereich: Timer Titel",  // Deutsch
   "",                          // Slovenski
   "Info area: titoli timer",   // Italiano
   "Infobereik: Timertitel",    // Nederlands
   "",                          // Português
   "Barre d'information: Titre Progammation",   // Français
   "",                          // Norsk
   "Infoalue: ajastimen otsikko", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Info area: timers text",    // English
   "Infobereich: Timer Text",   // Deutsch
   "",                          // Slovenski
   "Info area: testo timer",    // Italiano
   "Infobereik: Timertekst",    // Nederlands
   "",                          // Português
   "Barre d'information: Text programmation",   // Français
   "",                          // Norsk
   "Infoalue: ajastimen leipäteksti", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Info area: warning title",  // English
   "Infobereich: Warnung Titel", // Deutsch
   "",                          // Slovenski
   "Info area: titolo avviso",  // Italiano
   "Infobereik: waarschuwing titel", // Nederlands
   "",                          // Português
   "Barre d'information: avertissement titre",   // Français
   "",                          // Norsk
   "Infoalue: varoituksen otsikko",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Info area: warning text",   // English
   "Infobereich: Warnung Text", // Deutsch
   "",                          // Slovenski
   "Info area: testo avviso",   // Italiano
   "Infobereik: waarschuwing tekst",  // Nederlands
   "",                          // Português
   "Barre d'information: texte d'avertissement",      // Français
   "",                          // Norsk
   "Infoalue: varoituksen leipäteksti", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Details: title",            // English
   "Details: Titel",            // Deutsch
   "",                          // Slovenski
   "Dettagli: titolo",          // Italiano
   "Details: titel",            // Nederlands
   "",                          // Português
   "Détails: Titre",            // Français
   "",                          // Norsk
   "Lisätiedot: ohjelman nimi", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Details: subtitle",         // English
   "Details: Untertitel",       // Deutsch
   "",                          // Slovenski
   "Dettagli: sottotitolo",     // Italiano
   "Details: ondertitel",       // Nederlands
   "",                          // Português
   "Détails: Sous-titre",       // Français
   "",                          // Norsk
   "Lisätiedot: ohjelman kuvaus", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Details: date",             // English
   "Details: Datumszeile",      // Deutsch
   "",                          // Slovenski
   "Dettagli: data",            // Italiano
   "Details: datum",            // Nederlands
   "",                          // Português
   "Détails: date",             // Français
   "",                          // Norsk
   "Lisätiedot: päivämäärä",    // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Details: text",             // English
   "Details: Text",             // Deutsch
   "",                          // Slovenski
   "Dettagli: testo",           // Italiano
   "Details: tekst",            // Nederlands
   "",                          // Português
   "Détails: Texte",             // Français
   "",                          // Norsk
   "Lisätiedot: leipäteksti",   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Replay: times",             // English
   "Wiedergabe: Zeiten",        // Deutsch
   "",                          // Slovenski
   "Esecuzione: volte",         // Italiano
   "Weergave: tijden",          // Nederlands
   "",                          // Português
   "Rejouer : périodes",        // Français
   "",                          // Norsk
   "Toisto: kellonajat",        // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Default OSD Font",          // English
   "Std. OSD Font",             // Deutsch
   "",                          // Slovenski
   "Carattere OSD predefinito", // Italiano
   "Standaard OSDfont",         // Nederlands
   "",                          // Português
   "OSD standard",              // Français
   "",                          // Norsk
   "oletuskirjasintyyppi",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Default Fixed Size Font",   // English
   "Std. Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "Carattere dimensione fissa pred.",// Italiano
   "Standaardwaarde font",     // Nederlands
   "",                          // Português
   "Largeur fixe standard",     // Français
   "",                          // Norsk
   "tasavälinen kirjasintyyppi", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Default Small Font",        // English
   "Std. kleine Schrift",       // Deutsch
   "",                          // Slovenski
   "Carattere piccolo pred.",   // Italiano
   "Standaard klein font",      // Nederlands
   "",                          // Português
   "Petite police standard",           // Français
   "",                          // Norsk
   "pieni kirjasintyyppi",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Channel:",                  // English
   "Kanal:",                    // Deutsch
   "",                          // Slovenski
   "Canale:",                   // Italiano
   "Kanaal:",                   // Nederlands
   "",                          // Português
   "Chaîne:",                   // Français
   "",                          // Norsk
   "Kanava:",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Search pattern:",           // English
   "Suchmuster:",               // Deutsch
   "",                          // Slovenski
   "Modello di ricerca:",       // Italiano
   "Zoekpatroon:",              // Nederlands
   "",                          // Português
   "Modèle de recherche",       // Français
   "",                          // Norsk
   "Hakutapa:",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"No timer check",            // English
   "Ohne Überwachung",          // Deutsch
   "",                          // Slovenski
   "Nessun timer verifica",     // Italiano
   "Geen controle",             // Nederlands
   "",                          // Português
   "Sans surveillance",         // Français
   "",                          // Norsk
   "Ei valvontaa",              // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"General",                   // English
   "Allgemein",                 // Deutsch
   "",                          // Slovenski
   "Generale",                  // Italiano
   "Algemeen",                  // Nederlands
   "",                          // Português
   "Généralité",                // Français
   "",                          // Norsk
   "Yleiset",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "Nome",                      // Italiano
   "Naam",                      // Nederlands
   "",                          // Português
   "Nom",                       // Français
   "",                          // Norsk
   "Nimi",                      // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Width",                     // English
   "Weite",                     // Deutsch
   "",                          // Slovenski
   "Larghezza",                 // Italiano
   "Breedte",                   // Nederlands
   "",                          // Português
   "Largeur",                   // Français
   "",                          // Norsk
   "Leveys",                    // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Button$Set",                // English
   "Ändern",                    // Deutsch
   "",                          // Slovenski
   "Imposta",                   // Italiano
   "Veranderen",                // Nederlands
   "",                          // Português
   "Changer",                   // Français
   "",                          // Norsk
   "Aseta",                     // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"TrueType Font",             // English
   "TrueType Schrift",          // Deutsch
   "",                          // Slovenski
   "Carattere TrueType",        // Italiano
   "TrueType Font",             // Nederlands
   "",                          // Português
   "TrueType",                  // Français
   "",                          // Norsk
   "TrueType-kirjasin",         // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"No TTF support!",            // English
   "TTF nicht unterstützt!",    // Deutsch
   "",                          // Slovenski
   "Nessun supporto TTF!",      // Italiano
   "TTF niet ondersteund!",   // Nederlands
   "",                          // Português
   "Les polices TrueType ne sont pas supportées!", // Français
   "",                          // Norsk
   "Ei TrueType-kirjasintyyppi tukea!", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
  {"Fixed Font",                // English
   "Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "Carattere fisso",           // Italiano
   "Vaste waarde font",         // Nederlands
   "",                          // Português
   "Police avec une largeur fixe", // Français
   "",                          // Norsk
   "Tasavälinen kirjasintyyppi", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
#ifdef USE_PLUGIN_AVARDS
  {"Dynamic OSD size",          // English
   "Dynamische OSD-Größe",      // Deutsch
   "",                          // Slovenski
   "Dimensione OSD dinamica",   // Italiano
   "Dynamisch OSDgrootte",      // Nederlands
   "",                          // Português
   "Taille OSD dynamique",      // Français
   "",                          // Norsk
   "Vaihtuva kuvaruutunäytön koko", // suomi (Finnish)
   "",                          // Polski
   "",                          // Español
   "",                          // ÅëëçíéêÜ (Greek)
   "",                          // Svenska
   "",                          // Românã
   "",                          // Magyar
   "",                          // Català
   "",                          // ÀãááÚØÙ (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // Èesky (Czech)
   },
#endif
  {"No TrueType fonts installed!",          // English
   "Keine TrueType Schriften installiert!", // Deutsch
   "",                                      // Slovenski
   "Nessun carattere TrueType installato!", // Italiano
   "Geen TrueType fonten geinstalleerd!",   // Nederlands
   "",                                      // Português
   "Il n'y a pas de police TrueType d'installé!",  // Français
   "",                                      // Norsk
   "TrueType-kirjasintyyppejä ei löydy!",   // suomi (Finnish)
   "",                                      // Polski
   "",                                      // Español
   "",                                      // ÅëëçíéêÜ (Greek)
   "",                                      // Svenska
   "",                                      // Românã
   "",                                      // Magyar
   "",                                      // Català
   "",                                      // ÀãááÚØÙ (Russian)
   "",                                      // Hrvatski
   "",                                      // Eesti
   "",                                      // Dansk
   "",                                      // Èesky (Czech)
   },
  {NULL}
};
// vim:et:sw=2:ts=2:

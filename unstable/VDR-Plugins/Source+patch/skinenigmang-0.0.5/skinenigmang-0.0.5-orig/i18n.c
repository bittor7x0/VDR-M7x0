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
 *  - Espa�ol:    ???
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
   "ponedeljek",   // Slovenski
   "Luned�",       // Italiano
   "maandag",      // Nederlands
   "Segunda",      // Portugu�s
   "Lundi",        // Fran�ais
   "Manday",       // Norsk
   "Maanantai",    // suomi (Finnish)
   "poniedzia�ek", // Polski
   "Lunes",        // Espa�ol
   "�������",      // �������� (Greek)
   "M�ndag",       // Svenska
   "Luni",         // Rom�n�
   "H�tf�",        // Magyar
   "Dilluns",      // Catal�
   "�����������",  // ������� (Russian)
   "Ponedjeljak",  // Hrvatski
   "Esmasp�ev",    // Eesti
   "Mandag",       // Dansk
   "Pond�l�",      // �esky (Czech)
   },
  {"Tuesday",      // English
   "Dienstag",     // Deutsch
   "Torek",        // Slovenski
   "Marted�",      // Italiano
   "Dinsdag",      // Nederlands
   "Ter�a",        // Portugu�s
   "Mardi",        // Fran�ais
   "Tirsday",      // Norsk
   "Tiistai",      // suomi (Finnish)
   "Wtorek",       // Polski
   "Martes",       // Espa�ol
   "�����",        // �������� (Greek)
   "Tisdag",       // Svenska
   "Mar�i",        // Rom�n�
   "Kedd",         // Magyar
   "Dimarts",      // Catal�
   "�������",      // ������� (Russian)
   "Utorak",       // Hrvatski
   "Teisip�ev",    // Eesti
   "Tirsdag",      // Dansk
   "�ter�",        // �esky (Czech)
   },
  {"Wednesday",    // English
   "Mittwoch",     // Deutsch
   "sreda",        // Slovenski
   "mercoled�",    // Italiano
   "woensdag",     // Nederlands
   "Quarta",       // Portugu�s
   "Mercredi",     // Fran�ais
   "Onsday",       // Norsk
   "Keskiviikko",  // suomi (Finnish)
   "�roda",        // Polski
   "Mi�rcoles",    // Espa�ol
   "�������",      // �������� (Greek)
   "Onsdag",       // Svenska
   "Miercuri",     // Rom�n�
   "Szerda",       // Magyar
   "Dimecres",     // Catal�
   "�����",        // ������� (Russian)
   "Srijeda",      // Hrvatski
   "Kolmap�ev",    // Eesti
   "Onsdag",       // Dansk
   "St�eda",       // �esky (Czech)
   },
  {"Thursday",     // English
   "Donnerstag",   // Deutsch
   "�etrtek",      // Slovenski
   "Gioved�",      // Italiano
   "Donderdag",    // Nederlands
   "Quinta",       // Portugu�s
   "Jeudi",        // Fran�ais
   "Torsdag",      // Norsk
   "Torstai",      // suomi (Finnish)
   "Czwartek",     // Polski
   "Jueves",       // Espa�ol
   "������",       // �������� (Greek)
   "Torsdag",      // Svenska
   "Joi",          // Rom�n�
   "Cs�t�rt�k",    // Magyar
   "Dijous",       // Catal�
   "�������",      // ������� (Russian)
   "�etvrtak",     // Hrvatski
   "Neljap�ev",    // Eesti
   "Torsdag",      // Dansk
   "�tvrtek",      // �esky (Czech)
   },
  {"Friday",       // English
   "Freitag",      // Deutsch
   "Petek",        // Slovenski
   "Venerd�",      // Italiano
   "Vrijdag",      // Nederlands
   "Sexta",        // Portugu�s
   "Vendredi",     // Fran�ais
   "Fredag",       // Norsk
   "Perjantai",    // suomi (Finnish)
   "Pi�tek",       // Polski
   "Viernes",      // Espa�ol
   "���������",    // �������� (Greek)
   "Fredag",       // Svenska
   "Vineri",       // Rom�n�
   "P�ntek",       // Magyar
   "Divendres",    // Catal�
   "�������",      // ������� (Russian)
   "Petak",        // Hrvatski
   "Reede",        // Eesti
   "Fredag",       // Dansk
   "P�tek",        // �esky (Czech)
   },
  {"Saturday",     // English
   "Samstag",      // Deutsch
   "Sobota",       // Slovenski
   "Sabato",       // Italiano
   "Zaterdag",     // Nederlands
   "S�bado",       // Portugu�s
   "Samedi",       // Fran�ais
   "L�rdag",       // Norsk
   "Lauantai",     // suomi (Finnish)
   "Sobota",       // Polski
   "S�bado",       // Espa�ol
   "�������",      // �������� (Greek)
   "L�rdag",       // Svenska
   "S�mb�t�",      // Rom�n�
   "Szombat",      // Magyar
   "Dissabte",     // Catal�
   "�������",      // ������� (Russian)
   "Subota",       // Hrvatski
   "Laup�ev",      // Eesti
   "L�rdag",       // Dansk
   "Sobota",       // �esky (Czech)
   },
  {"Sunday",       // English
   "Sonntag",      // Deutsch
   "Nedelja",      // Slovenski
   "Domenica",     // Italiano
   "Zondag",       // Nederlands
   "Domingo",      // Portugu�s
   "Dimanche",     // Fran�ais
   "S�ndag",       // Norsk
   "Sunnuntai",    // suomi (Finnish)
   "Niedziela",    // Polski
   "Domingo",      // Espa�ol
   "�������",      // �������� (Greek)
   "S�ndag",       // Svenska
   "Duminic�",     // Rom�n�
   "Vas�rnap",     // Magyar
   "Diumenge",     // Catal�
   "�����������",  // ������� (Russian)
   "Nedjelja",     // Hrvatski
   "P�hap�ev",     // Eesti
   "S�ndag",       // Dansk
   "Ned�le",       // �esky (Czech)
   },
  {"EnigmaNG skin",             // English
   "EnigmaNG Oberfl�che",       // Deutsch
   "",                          // Slovenski
   "Interfaccia EnigmaNG",      // Italiano
   "EnigmaNG skin",             // Nederlands
   "",                          // Portugu�s
   "Skin EnigmaNG",             // Fran�ais
   "",                          // Norsk
   "EnigmaNG-ulkoasu",          // suomi (Finnish)
   "Skin EnigmaNG",             // Polski
   "Piel EnigmaNG",             // Espa�ol
   "",                          // �������� (Greek)
   "EnigmaNG-temat",            // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "EnigmaNG ����",             // ������� (Russian)
   "",                          // Hrvatski
   "EnigmaNG kest",             // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"EnigmaNG",                  // English
   "EnigmaNG",                  // Deutsch
   "",                          // Slovenski
   "EnigmaNG",                  // Italiano
   "EnigmaNG",                  // Nederlands
   "",                          // Portugu�s
   "EnigmaNG",                  // Fran�ais
   "",                          // Norsk
   "EnigmaNG",                  // suomi (Finnish)
   "EnigmaNG",                  // Polski
   "EnigmaNG",                  // Espa�ol
   "",                          // �������� (Greek)
   "EnigmaNG",                  // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "EnigmaNG",                  // ������� (Russian)
   "",                          // Hrvatski
   "EnigmaNG",                  // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"min",                       // English
   "min",                       // Deutsch
   "",                          // Slovenski
   "min",                       // Italiano
   "min",                       // Nederlands
   "",                          // Portugu�s
   "min",                       // Fran�ais
   "",                          // Norsk
   "min",                       // suomi (Finnish)
   "min",                       // Polski
   "min",                       // Espa�ol
   "",                          // �������� (Greek)
   "min",                       // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���",                       // ������� (Russian)
   "",                          // Hrvatski
   "min",                       // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Stereo",                    // English
   "Stereo",                    // Deutsch
   "",                          // Slovenski
   "Stereo",                    // Italiano
   "Stereo",                    // Nederlands
   "",                          // Portugu�s
   "St�r�o",                    // Fran�ais
   "",                          // Norsk
   "Stereo",                    // suomi (Finnish)
   "Stereo",                    // Polski
   "Estereo",                   // Espa�ol
   "",                          // �������� (Greek)
   "Stereo",                    // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������",                    // ������� (Russian)
   "",                          // Hrvatski
   "Stereo",                    // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Left",                      // English
   "Links",                     // Deutsch
   "",                          // Slovenski
   "Sinistra",                  // Italiano
   "Links",                     // Nederlands
   "",                          // Portugu�s
   "Gauche",                    // Fran�ais
   "",                          // Norsk
   "Vasen",                     // suomi (Finnish)
   "W lewo",                    // Polski
   "Izquierda",                 // Espa�ol
   "",                          // �������� (Greek)
   "V�nster",                   // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�����",                     // ������� (Russian)
   "",                          // Hrvatski
   "Vasak",                     // Eesti
   "",                          // Danske
   "",                          // �esky (Czech)
   },
  {"Right",                     // English
   "Rechts",                    // Deutsch
   "",                          // Slovenski
   "Destra",                    // Italiano
   "Rechts",                    // Nederlands
   "",                          // Portugu�s
   "Droite",                    // Fran�ais
   "",                          // Norsk
   "Oikea",                     // suomi (Finnish)
   "W prawo",                   // Polski
   "Derecha",                   // Espa�ol
   "",                          // �������� (Greek)
   "H�ger",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������",                    // ������� (Russian)
   "",                          // Hrvatski
   "Parem",                     // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "Muto",                      // Italiano
   "Geluid uit",                // Nederlands
   "",                          // Portugu�s
   "Muet",                      // Fran�ais
   "",                          // Norsk
   "Mykistetty",                // suomi (Finnish)
   "Wycisz",                    // Polski
   "Silenciar",                 // Espa�ol
   "",                          // �������� (Greek)
   "D�mpa",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "����. ����",                // ������� (Russian)
   "",                          // Hrvatski
   "H��letu",                   // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Volume",                    // English
   "Lautst�rke",                // Deutsch
   "",                          // Slovenski
   "Volume",                    // Italiano
   "Volume",                    // Nederlands
   "",                          // Portugu�s
   "Volume",                    // Fran�ais
   "",                          // Norsk
   "��nenvoimakkuus",           // suomi (Finnish)
   "G�o�no��",                  // Polski
   "Volumen",                   // Espa�ol
   "",                          // �������� (Greek)
   "Volym",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������",                 // ������� (Russian)
   "",                          // Hrvatski
   "Helitugevus",               // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show channel logos",        // English
   "Kanal-Logos anzeigen",      // Deutsch
   "",                          // Slovenski
   "Mostra loghi canali",       // Italiano
   "Toon kanaallogo's",         // Nederlands
   "",                          // Portugu�s
   "Afficher logos des cha�nes", // Fran�ais
   "",                          // Norsk
   "N�yt� kanavalogot",         // suomi (Finnish)
   "Pokazuj logo kana�u",       // Polski
   "Mostrar los logos de los canales",  // Espa�ol
   "",                          // �������� (Greek)
   "Visa kanallogotyper",       // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ���� �������",   // ������� (Russian)
   "",                          // Hrvatski
   "Kanalilogo n�itamine",      // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Identify channel by",       // English
   "  Kanal-Identifikation durch",        // Deutsch
   "",                          // Slovenski
   "  Identifica canale per",     // Italiano
   "  Kanaalidentificatie door",  // Nederlands
   "",                          // Portugu�s
   "  Identifier cha�ne par",     // Fran�ais
   "",                          // Norsk
   "  Tunnista kanava",           // suomi (Finnish)
   "  Rozpoznaj kana� po",        // Polski
   "  Identificar el canal por...",       // Espa�ol
   "",                          // �������� (Greek)
   "  Identifiera kanal med",     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "  ���������� ����� ��",       // ������� (Russian)
   "",                          // Hrvatski
   "  Kanali tuvastamise meetod", // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "nome",                      // Italiano
   "naam",                      // Nederlands
   "",                          // Portugu�s
   "Nom",                       // Fran�ais
   "",                          // Norsk
   "nimest�",                   // suomi (Finnish)
   "nazwa",                     // Polski
   "Nombre",                    // Espa�ol
   "",                          // �������� (Greek)
   "namn",                      // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "��������",                  // ������� (Russian)
   "",                          // Hrvatski
   "nimi",                      // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"data",                      // English
   "ID",                        // Deutsch
   "",                          // Slovenski
   "dati",                      // Italiano
   "data",                      // Nederlands
   "",                          // Portugu�s
   "Donn�e",                    // Fran�ais
   "",                          // Norsk
   "tiedoista",                 // suomi (Finnish)
   "parametry",                 // Polski
   "Datos",                     // Espa�ol
   "",                          // �������� (Greek)
   "data",                      // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������",                    // ������� (Russian)
   "",                          // Hrvatski
   "andmed",                    // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Channel logo cache size",   // English
   "Gr��e des Logo-Caches",     // Deutsch
   "",                          // Slovenski
   "Dimensione cache dei loghi",// Italiano
   "Kanaallogo cache",          // Nederlands
   "",                          // Portugu�s
   "Taille du cache des logos",  // Fran�ais
   "",                          // Norsk
   "V�limuistin koko kanavalogoille",   // suomi (Finnish)
   "Rozmiar cache dla logo kana�u",     // Polski
   "Tama�o de la cache de los logos",   // Espa�ol
   "",                          // �������� (Greek)
   "Cachestorlek f�r logotyper",        // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������ ���� ���o����� �������",  // ������� (Russian)
   "",                          // Hrvatski
   "Kanalilogo vahem�lu suurus",        // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Button$Flush cache",        // English
   "Cache leeren",              // Deutsch
   "",                          // Slovenski
   "Pulisci cache",             // Italiano
   "Cache legen",               // Nederlands
   "",                          // Portugu�s
   "Supprimer cache",           // Fran�ais
   "",                          // Norsk
   "Tyhjenn�",                  // suomi (Finnish)
   "Opr�nij cache",            // Polski
   "Limpiar la cache",          // Espa�ol
   "",                          // �������� (Greek)
   "T�m cachen",                // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� ���",              // ������� (Russian)
   "",                          // Hrvatski
   "T�hjenda",                  // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Flushing channel logo cache...",    // English
   "Logo-Cache wird geleert...",        // Deutsch
   "",                          // Slovenski
   "Pulendo la cache...",       // Italiano
   "Kanaallogo cache legen...", // Nederlands
   "",                          // Portugu�s
   "Suppression du cache...",   // Fran�ais
   "",                          // Norsk
   "Tyhjennet��n v�limuistia...",       // suomi (Finnish)
   "Opr�niam cache logo kana�u...",    // Polski
   "Limpiando la cache...",     // Espa�ol
   "",                          // �������� (Greek)
   "T�mmer cachen...",          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������ ���...",                          // ������� (Russian)
   "",                          // Hrvatski
   "Kanalilogo vahem�lu t�hjendamine...",       // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Languages",                 // English
   "Sprachen",                  // Deutsch
   "",                          // Slovenski
   "Lingue",                    // Italiano
   "Talen",                     // Nederlands
   "",                          // Portugu�s
   "Langues",                   // Fran�ais
   "",                          // Norsk
   "Kielet",                    // suomi (Finnish)
   "J�zyki",                    // Polski
   "Lenguajes",                 // Espa�ol
   "",                          // �������� (Greek)
   "Spr�k",                     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�����",                          // ������� (Russian)
   "",                          // Hrvatski
   "Keeled",                    // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Auxiliary information",     // English
   "Zusatzinformation",         // Deutsch
   "",                          // Slovenski
   "Informazione ausiliare",    // Italiano
   "Hulp informatie",           // Nederlands
   "",                          // Portugu�s
   "Information auxiliare",     // Fran�ais
   "",                          // Norsk
   "Lis�tiedot",                // suomi (Finnish)
   "Informacje pomocnicze",     // Polski
   "Informaci�n auxiliar",      // Espa�ol
   "",                          // �������� (Greek)
   "Extrainformation",          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "����. ����������",          // ������� (Russian)
   "",                          // Hrvatski
   "Lisainfo",                  // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show auxiliary information",        // English
   "Zusatzinfo anzeigen",       // Deutsch
   "",                          // Slovenski
   "Mostra informazione ausiliare",     // Italiano
   "Toon hulp informatie",      // Nederlands
   "",                          // Portugu�s
   "Afficher informations auxiliaires",   // Fran�ais
   "",                          // Norsk
   "N�yt� lis�tiedot",          // suomi (Finnish)
   "Pokazuj informacje pomocnicze",     // Polski
   "Mostrar informaci�n auxiliar",      // Espa�ol
   "",                          // �������� (Greek)
   "Visa extrainformation",     // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ����. ����������",      // ������� (Russian)
   "",                          // Hrvatski
   "Lisainfo n�itamine",        // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show remaining/elapsed time",          // English
   "Zeige abgel./restl. Zeit",             // Deutsch
   "",                          // Slovenski
   "Mostra tempo rimasto/trascorso",       // Italiano
   "Toon resterend/gedane tijd",  // Nederlands
   "",                          // Portugu�s
   "Afficher le temps �coul�/restant", // Fran�ais
   "",                          // Norsk
   "N�yt� tapahtuman aika",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� �����./������. �����",     // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"remaining",                 // English
   "restliche",                 // Deutsch
   "",                          // Slovenski
   "rimasto",                   // Italiano
   "resterend",                   // Nederlands
   "",                          // Portugu�s
   "restant",                   // Fran�ais
   "",                          // Norsk
   "j�ljell� oleva",            // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������",                 // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"elapsed",                   // English
   "abgelaufene",               // Deutsch
   "",                          // Slovenski
   "trascorso",                 // Italiano
   "gedane",                // Nederlands
   "",                          // Portugu�s
   "�coul�",                    // Fran�ais
   "",                          // Norsk
   "kulunut",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "��������",                  // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show info area in main menu",            // English
   "Infobereich im Hauptmen�",               // Deutsch
   "",                          // Slovenski
   "Mostra area info nel menu principale",   // Italiano
   "Info in hoofdmenu",         // Nederlands
   "",                          // Portugu�s
   "Surface d'infos dans le menu principal",  // Fran�ais
   "",                          // Norsk
   "N�yt� infoalue p��valikossa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ���. ������� � ��. ����",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show marker in lists",            // English
   "Symbol vor Listeneintr�gen",      // Deutsch
   "",                          // Slovenski
   "Mostra indicatore negli elenchi", // Italiano
   "Markering in lijsten tonen", // Nederlands
   "",                          // Portugu�s
   "Marque devant les listes",  // Fran�ais
   "",                          // Norsk
   "N�yt� valintasymboli",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ����� � �������",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show progressbar",          // English
   "Fortschrittbalken anzeigen",        // Deutsch
   "",                          // Slovenski
   "Mostra barra avanzamento",  // Italiano
   "Progressiebalk tonen",      // Nederlands
   "",                          // Portugu�s
   "Afficher barre de progression",     // Fran�ais
   "",                          // Norsk
   "N�yt� aikajana",            // suomi (Finnish)
   "Pokazuj pasek post�pu",     // Polski
   "Mostrar barra de progreso", // Espa�ol
   "",                          // �������� (Greek)
   "Visa framsteg",             // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ��������� �����������",   // ������� (Russian)
   "",                          // Hrvatski
   "Edenemisriba n�itamine",    // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show symbols",              // English
   "Symbole anzeigen",          // Deutsch
   "",                          // Slovenski
   "Mostra simboli",            // Italiano
   "Symbolen tonen",            // Nederlands
   "",                          // Portugu�s
   "Afficher symbole",          // Fran�ais
   "",                          // Norsk
   "N�yt� symbolit",            // suomi (Finnish)
   "Pokazuj symbole",           // Polski
   "Mostrar s�mbolos",          // Espa�ol
   "",                          // �������� (Greek)
   "Visa symboler",             // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� �������",                          // ������� (Russian)
   "",                          // Hrvatski
   "S�mbolite n�itamine",       // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show VPS",                  // English
   "VPS anzeigen",              // Deutsch
   "",                          // Slovenski
   "Mostra VPS",                // Italiano
   "VPS tonen",                 // Nederlands
   "",                          // Portugu�s
   "Afficher le VPS",           // Fran�ais
   "",                          // Norsk
   "N�yt� VPS-tieto",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� VPS",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"WARNING",                   // English
   "WARNUNG",                   // Deutsch
   "",                          // Slovenski
   "ATTENZIONE",                // Italiano
   "WAARSCHUWING",              // Nederlands
   "",                          // Portugu�s
   "ATTENTION",                 // Fran�ais
   "",                          // Norsk
   "VAROITUS",                  // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "����ÿ��������",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Timer conflict",            // English
   "Timerkonflikt",             // Deutsch
   "",                          // Slovenski
   "Conflitto timer",           // Italiano
   "Timerconflict",             // Nederlands
   "",                          // Portugu�s
   "Conflit de programmation",  // Fran�ais
   "",                          // Norsk
   "P��llekk�inen ajastin",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� �������",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Timer conflicts",           // English
   "Timerkonflikte",            // Deutsch
   "",                          // Slovenski
   "Conflitti timer",           // Italiano
   "Timerconflicten",                          // Nederlands
   "",                          // Portugu�s
   "Conflits de programmation", // Fran�ais
   "",                          // Norsk
   "P��llekk�isi� ajastimia",   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "��������� ��������",        // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"TIMERS",                    // English
   "TIMER",                     // Deutsch
   "",                          // Slovenski
   "TIMER",                     // Italiano
   "TIMER",                     // Nederlands
   "",                          // Portugu�s
   "PROGRAMMATION",             // Fran�ais
   "",                          // Norsk
   "AJASTIMET",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "°�����",                   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"RERUNS OF THIS SHOW",       // English
   "WIEDERHOLUNGEN",            // Deutsch
   "",                          // Slovenski
   "RIPETI QUESTO SHOW",        // Italiano
   "HERHALINGEN",               // Nederlands
   "",                          // Portugu�s
   "REPETITION",                // Fran�ais
   "",                          // Norsk
   "TOISTUVAT TAPAHTUMAT",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���¾�����",                // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"*** Invalid Channel ***",   // English
   "*** Ung�ltiger Kanal ***",  // Deutsch
   "",                          // Slovenski
   "*** Canale non valido ***", // Italiano
   "*** Ongeldig Kanaal ***",   // Nederlands
   "",                          // Portugu�s
   "*** Cha�ne non valable ***",  // Fran�ais
   "",                          // Norsk
   "*** Virheellinen kanava ***", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "*** ������������ ����� ***",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Size",                      // English
   "Gr��e",                     // Deutsch
   "",                          // Slovenski
   "Dimensione",                // Italiano
   "Grootte",                   // Nederlands
   "",                          // Portugu�s
   "Taille",                    // Fran�ais
   "",                          // Norsk
   "Koko",                      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Mute",                      // English
   "Stumm",                     // Deutsch
   "",                          // Slovenski
   "Muto",                      // Italiano
   "Geluid uit",                // Nederlands
   "Muet",                      // Portugu�s
   "",                          // Fran�ais
   "",                          // Norsk
   "Mykistetty",                // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "����.����",                 // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Volume",                    // English
   "Lautst�rke",                // Deutsch
   "",                          // Slovenski
   "Volume",                    // Italiano
   "Volume",                    // Nederlands
   "",                          // Portugu�s
   "Volume",                    // Fran�ais
   "",                          // Norsk
   "��nenvoimakkuus",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show symbols in lists",     // English
   "Symbole in Listen",         // Deutsch
   "",                          // Slovenski
   "Mostra simboli negli elenchi",       // Italiano
   "Symbolen in lijsten",       // Nederlands
   "",                          // Portugu�s
   "Symboles dans les listes",   // Fran�ais
   "",                          // Norsk
   "N�yt� tapahtumien symbolit",// suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ������� � �������",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Try 8bpp single area",      // English
   "Einen 8bpp Bereich versuchen", // Deutsch
   "",                          // Slovenski
   "Prova area singola a 8bpp", // Italiano
   "Op 8bpp bereik testen",     // Nederlands
   "",                          // Portugu�s
   "Essayer une surface � 8bpp",// Fran�ais
   "",                          // Norsk
   "Suosi yht� 8bpp kuva-aluetta", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show flags",                // English
   "Fahnen anzeigen",           // Deutsch
   "",                          // Slovenski
   "Mostra segni",              // Italiano
   "Vlaggen tonen",             // Nederlands
   "",                          // Portugu�s
   "Afficher drapeau",          // Fran�ais
   "",                          // Norsk
   "N�yt� liput",               // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� �����",            // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show symbols in menu",      // English
   "Symbole im Men�",           // Deutsch
   "",                          // Slovenski
   "Mostra simboli nel menu",   // Italiano
   "Symbolen in menu",          // Nederlands
   "",                          // Portugu�s
   "Symboles dans le menu",     // Fran�ais
   "",                          // Norsk
   "N�yt� valikkosymbolit",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� ������� � ����",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"never",                     // English
   "nie",                       // Deutsch
   "",                          // Slovenski
   "mai",                       // Italiano
   "nooit",                     // Nederlands
   "",                          // Portugu�s
   "jamais",                    // Fran�ais
   "",                          // Norsk
   "ei koskaan",                // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������",                   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"always",                    // English
   "immer",                     // Deutsch
   "",                          // Slovenski
   "sempre",                    // Italiano
   "Altijd",                    // Nederlands
   "",                          // Portugu�s
   "toujours",                  // Fran�ais
   "",                          // Norsk
   "aina",                      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������",                    // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"percent",                   // English
   "Prozent",                   // Deutsch
   "",                          // Slovenski
   "percentuale",               // Italiano
   "procent",                   // Nederlands
   "",                          // Portugu�s
   "Pourcentage",               // Fran�ais
   "",                          // Norsk
   "prosentti",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������",                   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"if exists",                 // English
   "wenn vorhanden",            // Deutsch
   "",                          // Slovenski
   "se esiste",                 // Italiano
   "wanneer aanwezig",          // Nederlands
   "",                          // Portugu�s
   "si existant",               // Fran�ais
   "",                          // Norsk
   "jos olemassa",              // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���� � �������",            // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Number of Reruns",          // English
   "Anzahl Wiederholungen",     // Deutsch
   "",                          // Slovenski
   "Numero di ripetizioni",     // Italiano
   "aantal herhalingen",        // Nederlands
   "",                          // Portugu�s
   "Nombre de r�p�titions",     // Fran�ais
   "",                          // Norsk
   "Uusintojen lukum��r�",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ����������",     // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Use Subtitle for reruns",   // English
   "Verw. Untertitel f�r Wdh.", // Deutsch
   "",                          // Slovenski
   "Usa sottotitolo per ripetizioni",         // Italiano
   "Gebr. ondertitlel voor herh.", // Nederlands
   "",                          // Portugu�s
   "Utilise sous-titres pour les r�p�titions", // Fran�ais
   "",                          // Norsk
   "K�yt� lyhytt� kuvausta uusinnoille", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������. ������������ ��� ����������",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show timer conflicts",      // English
   "Timerkonflikte anzeigen",   // Deutsch
   "",                          // Slovenski
   "Mostra conflitti timer",    // Italiano
   "Timerconflict tonen",       // Nederlands
   "",                          // Portugu�s
   "Afficher les conflits de programmation", // Fran�ais
   "",                          // Norsk
   "N�yt� p��llekk�iset ajastimet", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� ��������� ��������",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Timer check",               // English
   "�berwachung",               // Deutsch
   "",                          // Slovenski
   "Verifica timer",            // Italiano
   "Monitoren",                 // Nederlands
   "",                          // Portugu�s
   "Observation de la programmation", // Fran�ais
   "",                          // Norsk
   "Ajastimien valvonta",       // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� ��������",         // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"EPGSearch",                 // English
   "EPGSearch",                 // Deutsch
   "",                          // Slovenski
   "Ricerca EPG",               // Italiano
   "EPGsearch",                 // Nederlands
   "",                          // Portugu�s
   "Recherche EPG",             // Fran�ais
   "",                          // Norsk
   "EPGSearch",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Logos & Symbols",           // English
   "Logos & Symbole",           // Deutsch
   "",                          // Slovenski
   "Loghi e simboli",           // Italiano
   "Logo's & Symbolen",         // Nederlands
   "",                          // Portugu�s
   "Logos et symboles",         // Fran�ais
   "",                          // Norsk
   "Logot ja symbolit",         // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� & �������",        // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"use size.vdr only",         // English
   "nur size.vdr verwenden",    // Deutsch
   "",                          // Slovenski
   "usa solo size.vdr",         // Italiano
   "alleen size.vdr gebruiken", // Nederlands
   "",                          // Portugu�s
   "utiliser seulement size.vdr",  // Fran�ais Patrice Staudt 29.03.2007
   "",                          // Norsk
   "vain jos size.vdr",         // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������������ ������ size.vdr",  // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show recording's size",     // English
   "Aufnahmengr��e anzeigen",   // Deutsch
   "",                          // Slovenski
   "Mostra dimensione delle registrazioni",      // Italiano
   "Opnamegrootte tonen",       // Nederlands
   "",                          // Portugu�s
   "Afficher la taille des enregistrements",   // Fran�ais
   "",                          // Norsk
   "N�yt� tallenteen koko",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������� ������� �������",    // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"pixel algo",                // English
   "Pixel-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "algoritmo pixel",           // Italiano
   "pixelalgoritme",            // Nederlands
   "",                          // Portugu�s
   "Algorithme pixel",          // Fran�ais
   "",                          // Norsk
   "n�ytteistys",               // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� �������",          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"ratio algo",                // English
   "Ratio-Algorithmus",         // Deutsch
   "",                          // Slovenski
   "algoritmo ratio",           // Italiano
   "algoritmeratio",            // Nederlands
   "",                          // Portugu�s
   "Algorithme ratio",          // Fran�ais
   "",                          // Norsk
   "skaalaus",                  // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� �����������",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"zoom image",                // English
   "Zoomen",                    // Deutsch
   "",                          // Slovenski
   "ingrandisci immagine",      // Italiano
   "zoomen",                    // Nederlands
   "",                          // Portugu�s
   "Agrandir image",            // Fran�ais
   "",                          // Norsk
   "zoomaus",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "���������",                 // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Show event/recording images",     // English
   "Bilder in EPG/Aufnahmendetails",  // Deutsch
   "",                                // Slovenski
   "Mostra immagini evento/registrazione",      // Italiano
   "EPG/opnamedetails tonen",         // Nederlands
   "",                                // Portugu�s
   "Images dans les EPG et d�tails d'enregistrements",     // Fran�ais
   "",                                // Norsk
   "N�yt� kuvat lis�tietovalikossa",  // suomi (Finnish)
   "",                                // Polski
   "",                                // Espa�ol
   "",                                // �������� (Greek)
   "",                                // Svenska
   "",                                // Rom�n�
   "",                                // Magyar
   "",                                // Catal�
   "����������� � EPG",               // ������� (Russian)
   "",                                // Hrvatski
   "",                                // Eesti
   "",                                // Dansk
   "",                                // �esky (Czech)
   },
  {"  Resize images",           // English
   "  Bildberechnung benutzt",  // Deutsch
   "",                          // Slovenski
   "  Ridimensiona immagini",   // Italiano
   "  Aanpassen beeldgrootte",  // Nederlands
   "",                          // Portugu�s
   "  Redimensionne les images",  // Fran�ais
   "",                          // Norsk
   "  Muokkaa kuvien kokoa",    // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "  ���������� ���������",    // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Image width",             // English
   "  Bildbreite",              // Deutsch
   "",                          // Slovenski
   "  Lunghezza immagine",      // Italiano
   "  Beeldbreedte",            // Nederlands
   "",                          // Portugu�s
   "  Largeur",                 // Fran�ais
   "",                          // Norsk
   "  Kuvien leveys",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "  ������ �����������",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Image height",            // English
   "  Bildh�he",                // Deutsch
   "",                          // Slovenski
   "  Altezza immagine",        // Italiano
   "  Beeldhoogte",             // Nederlands
   "",                          // Portugu�s
   "  Hauteur",                 // Fran�ais
   "",                          // Norsk
   "  Kuvien korkeus",          // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "  ������ �����������",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Image format",            // English
   "  Bildformat",              // Deutsch
   "",                          // Slovenski
   "  Formato immagine",        // Italiano
   "  Beeldformaat",            // Nederlands
   "",                          // Portugu�s
   "  Format d'image",          // Fran�ais
   "",                          // Norsk
   "  Kuvaformaatti",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "  ������ �����������",      // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Full title width",          // English
   "Volle Titelbreite",         // Deutsch
   "",                          // Slovenski
   "Lunghezza titolo completo", // Italiano
   "Volle titelbreedte",        // Nederlands
   "",                          // Portugu�s
   "Largeur totale pour le titre", // Fran�ais
   "",                          // Norsk
   "Levit� otsikkopalkki",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������ ������ ���������",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
#ifdef USE_PLUGIN_MAILBOX
  {"Show mail icon",            // English
   "Zeige Mail-Icon",           // Deutsch
   "",                          // Slovenski
   "Mostra icona email",        // Italiano
   "Mail-ikoon tonen",          // Nederlands
   "",                          // Portugu�s
   "Afficher l'ic�ne courriel",  // Fran�ais
   "",                          // Norsk
   "N�yt� s�hk�postikuvake",    // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "�������� ������ �����. �����",   // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"only if new mail present",  // English
   "nur bei neuer Mail",        // Deutsch
   "",                          // Slovenski
   "solo se presenti nuove email",          // Italiano
   "alleen bij nieuwe mail",    // Nederlands
   "",                          // Portugu�s
   "juste lors d'un courriel",  // Fran�ais
   "",                          // Norsk
   "jos uutta postia",          // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "������ ��� ����� ���������",  // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
#endif
#ifndef DISABLE_ANIMATED_TEXT
  {"Animated Text",             // English
   "Bewegter Text",             // Deutsch
   "",                          // Slovenski
   "Testo animato",             // Italiano
   "Geanimeerde tekst",         // Nederlands
   "",                          // Portugu�s
   "Texte d�filant",            // Fran�ais
   "",                          // Norsk
   "Tekstien el�v�inti",        // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Enable",                    // English
   "Verwenden",                 // Deutsch
   "",                          // Slovenski
   "Abilita",                   // Italiano
   "Toelaten",                  // Nederlands
   "",                          // Portugu�s
   "Permettre",                 // Fran�ais
   "",                          // Norsk
   "K�yt� el�v�inti�",          // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scoll behaviour",         // English
   "  Scroll-Verhalten",        // Deutsch
   "",                          // Slovenski
   "  Comportamento scorrimento",// Italiano
   "  Scroll gedrag",            // Nederlands
   "",                          // Portugu�s
   "  Comportement de d�filement", // Fran�ais
   "",                          // Norsk
   "  Vieritystapa",            // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"to the left",               // English
   "nach links",                // Deutsch
   "",                          // Slovenski
   "verso la sinistra",         // Italiano
   "naar links",                // Nederlands
   "",                          // Portugu�s
   "vers la gauche",            // Fran�ais
   "",                          // Norsk
   "vasemmalle",                // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"left and right",            // English
   "links und rechts",          // Deutsch
   "",                          // Slovenski
   "sinistra e destra",         // Italiano
   "links en rechts",           // Nederlands
   "",                          // Portugu�s
   "gauche et droite",          // Fran�ais
   "",                          // Norsk
   "edestakaisin",              // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scroll delay (ms)",       // English
   "  Scroll-Verz�gerung (ms)", // Deutsch
   "",                          // Slovenski
   "  Ritardo scorrimento (ms)",// Italiano
   "  Scrollvertraging (ms)",   // Nederlands
   "",                          // Portugu�s
   "  D�lai de d�filement (ms)",// Fran�ais
   "",                          // Norsk
   "  Vierityksen viive (ms)",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scroll pause (ms)",       // English
   "  Scroll-Pause (ms)",       // Deutsch
   "",                          // Slovenski
   "  Pausa scorrimento (ms)",  // Italiano
   "  Scrollpause (ms)",        // Nederlands
   "",                          // Portugu�s
   "  Pause de d�filement (ms)",// Fran�ais
   "",                          // Norsk
   "  Vierityksen tauko (ms)",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Blink pause (ms)",        // English
   "  Blink-Pause (ms)",        // Deutsch
   "",                          // Slovenski
   "  Pausa lampeggiante (ms)", // Italiano
   "  Blinkpauze (ms)",         // Nederlands
   "",                          // Portugu�s
   "  Pause de clignotement (ms)",// Fran�ais
   "",                          // Norsk
   "  Vilkutuksen tauko (ms)",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scroll OSD title",        // English
   "  OSD Titel scrollen",      // Deutsch
   "",                          // Slovenski
   "  Scorri titolo OSD",       // Italiano
   "  OSD titel scrollen",      // Nederlands
   "",                          // Portugu�s
   "  D�filement dans le titre du OSD",  // Fran�ais
   "",                          // Norsk
   "  Vierit� valikon otsikkoa",// suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scroll info area",        // English
   "  Infobereich scrollen",    // Deutsch
   "",                          // Slovenski
   "  Scorri informazioni area",// Italiano
   "  Infobereik scrollen",     // Nederlands
   "",                          // Portugu�s
   "  Barre de d�filement dans la surface info",   // Fran�ais
   "",                          // Norsk
   "  Vierit� infoaluetta",     // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scroll active list items",  // English
   "  Aktive Listenzeile scrollen", // Deutsch
   "",                          // Slovenski
   "  Scorri valori lista attivi",// Italiano
   "  Lijst aktieve punten scrollen",  // Nederlands
   "",                          // Portugu�s
   "  Barre de d�filement dans la liste active",  // Fran�ais
   "",                          // Norsk
   "  Vierit� aktiivista valintaa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Scroll other items",      // English
   "  Andere Bereiche scrollen", // Deutsch
   "",                          // Slovenski
   "  Scorri altri valori",     // Italiano
   "  Ander bereik scrollen",   // Nederlands
   "",                          // Portugu�s
   "  Barre de d�filement dans d'autres parties",  // Fran�ais
   "",                          // Norsk
   "  Vierit� muita alueita",   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
#endif
  {"  Show symbols in menu",    // English
   "  Symbole im Men�",         // Deutsch
   "",                          // Slovenski
   "  Mostra simboli nel menu", // Italiano
   "  Symbolen in menu",   // Nederlands
   "",                          // Portugu�s
   "  dans le menu",  		// Fran�ais
   "",                          // Norsk
   "  N�yt� symbolit valikossa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Show symbols in replay",  // English
   "  Symbole bei der Wiedergabe", // Deutsch
   "",                          // Slovenski
   "  Mostra simboli in esecuzione",// Italiano
   "  Symbolen bij weergave",   // Nederlands
   "",                          // Portugu�s
   "  dans lecture",		// Fran�ais
   "",                          // Norsk
   "  N�yt� symbolit toistettaessa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Show symbols in messages",  // English
   "  Symbole bei den Meldungen", // Deutsch
   "",                          // Slovenski
   "  Mostra simboli nei messaggi",// Italiano
   "  Symbolen bij berichten",  // Nederlands
   "",                          // Portugu�s
   "  dans les messages", 	// Fran�ais
   "",                          // Norsk
   "  N�yt� symbolit viesteiss�", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"  Show symbols in audio",   // English
   "  Symbole bei Audio",       // Deutsch
   "",                          // Slovenski
   "  Mostra simboli in audio", // Italiano
   "  Symbolen bij audio",      // Nederlands
   "",                          // Portugu�s
   "  dans audio",        	// Fran�ais
   "",                          // Norsk
   "  N�yt� symbolit ��nivalikossa", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Fonts",                     // English
   "Fonts",                     // Deutsch
   "",                          // Slovenski
   "Caratteri",                 // Italiano
   "Fonts",                     // Nederlands
   "",                          // Portugu�s
   "Les polices",               // Fran�ais
   "",                          // Norsk
   "Kirjasintyypit",            // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"OSD title",                 // English
   "OSD Titel",                 // Deutsch
   "",                          // Slovenski
   "Titolo OSD",                // Italiano
   "OSD titel",                 // Nederlands
   "",                          // Portugu�s
   "Titre OSD",                 // Fran�ais
   "",                          // Norsk
   "Valikon otsikko",           // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Date",                      // English
   "Datum",                     // Deutsch
   "",                          // Slovenski
   "Data",                      // Italiano
   "Datum",                     // Nederlands
   "",                          // Portugu�s
   "Date",                      // Fran�ais
   "",                          // Norsk
   "P�iv�ys",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Messages",                  // English
   "Meldungen",                 // Deutsch
   "",                          // Slovenski
   "Messaggi",                  // Italiano
   "Meldingen",                 // Nederlands
   "",                          // Portugu�s
   "Message",                   // Fran�ais
   "",                          // Norsk
   "Viestit",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Help keys",                 // English
   "Farb-Tasten",               // Deutsch
   "",                          // Slovenski
   "Tasti d'aiuto",             // Italiano
   "Help toetsen",              // Nederlands
   "",                          // Portugu�s
   "Touche couleur",            // Fran�ais
   "",                          // Norsk
   "V�rin�pp�imet",             // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Channelinfo: title",        // English
   "Kanalinfo: Titel",          // Deutsch
   "",                          // Slovenski
   "Info canale: titolo",       // Italiano
   "Kanaalinfo: titel",         // Nederlands
   "",                          // Portugu�s
   "Cha�ne info: titre ",        // Fran�ais
   "",                          // Norsk
   "Kanavatieto: ohjelman nimi",// suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Channelinfo: subtitle",     // English
   "Kanalinfo: Untertitel",     // Deutsch
   "",                          // Slovenski
   "Info canale: sottotitolo",  // Italiano
   "Kanaalinfo: ondertitel",    // Nederlands
   "",                          // Portugu�s
   "Cha�ne info: sous-titres",    // Fran�ais
   "",                          // Norsk
   "Kanavatieto: ohjelman kuvaus", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Channelinfo: language",     // English
   "Kanalinfo: Sprache",        // Deutsch
   "",                          // Slovenski
   "Info canale: lingua",       // Italiano
   "Kanaalinfo: taal",          // Nederlands
   "",                          // Portugu�s
   "Cha�ne info: langue",       // Fran�ais
   "",                          // Norsk
   "Kanavatieto: kieli",        // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"List items",                // English
   "Listen",                    // Deutsch
   "",                          // Slovenski
   "Elenco",                    // Italiano
   "Lijsten",                   // Nederlands
   "",                          // Portugu�s
   "Liste",                     // Fran�ais
   "",                          // Norsk
   "Listat",                    // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Info area: timers title",   // English
   "Infobereich: Timer Titel",  // Deutsch
   "",                          // Slovenski
   "Info area: titoli timer",   // Italiano
   "Infobereik: Timertitel",    // Nederlands
   "",                          // Portugu�s
   "Barre d'information: Titre Progammation",   // Fran�ais
   "",                          // Norsk
   "Infoalue: ajastimen otsikko", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Info area: timers text",    // English
   "Infobereich: Timer Text",   // Deutsch
   "",                          // Slovenski
   "Info area: testo timer",    // Italiano
   "Infobereik: Timertekst",    // Nederlands
   "",                          // Portugu�s
   "Barre d'information: Text programmation",   // Fran�ais
   "",                          // Norsk
   "Infoalue: ajastimen leip�teksti", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Info area: warning title",  // English
   "Infobereich: Warnung Titel", // Deutsch
   "",                          // Slovenski
   "Info area: titolo avviso",  // Italiano
   "Infobereik: waarschuwing titel", // Nederlands
   "",                          // Portugu�s
   "Barre d'information: avertissement titre",   // Fran�ais
   "",                          // Norsk
   "Infoalue: varoituksen otsikko",  // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Info area: warning text",   // English
   "Infobereich: Warnung Text", // Deutsch
   "",                          // Slovenski
   "Info area: testo avviso",   // Italiano
   "Infobereik: waarschuwing tekst",  // Nederlands
   "",                          // Portugu�s
   "Barre d'information: texte d'avertissement",      // Fran�ais
   "",                          // Norsk
   "Infoalue: varoituksen leip�teksti", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Details: title",            // English
   "Details: Titel",            // Deutsch
   "",                          // Slovenski
   "Dettagli: titolo",          // Italiano
   "Details: titel",            // Nederlands
   "",                          // Portugu�s
   "D�tails: Titre",            // Fran�ais
   "",                          // Norsk
   "Lis�tiedot: ohjelman nimi", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Details: subtitle",         // English
   "Details: Untertitel",       // Deutsch
   "",                          // Slovenski
   "Dettagli: sottotitolo",     // Italiano
   "Details: ondertitel",       // Nederlands
   "",                          // Portugu�s
   "D�tails: Sous-titre",       // Fran�ais
   "",                          // Norsk
   "Lis�tiedot: ohjelman kuvaus", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Details: date",             // English
   "Details: Datumszeile",      // Deutsch
   "",                          // Slovenski
   "Dettagli: data",            // Italiano
   "Details: datum",            // Nederlands
   "",                          // Portugu�s
   "D�tails: date",             // Fran�ais
   "",                          // Norsk
   "Lis�tiedot: p�iv�m��r�",    // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Details: text",             // English
   "Details: Text",             // Deutsch
   "",                          // Slovenski
   "Dettagli: testo",           // Italiano
   "Details: tekst",            // Nederlands
   "",                          // Portugu�s
   "D�tails: Texte",             // Fran�ais
   "",                          // Norsk
   "Lis�tiedot: leip�teksti",   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Replay: times",             // English
   "Wiedergabe: Zeiten",        // Deutsch
   "",                          // Slovenski
   "Esecuzione: volte",         // Italiano
   "Weergave: tijden",          // Nederlands
   "",                          // Portugu�s
   "Rejouer : p�riodes",        // Fran�ais
   "",                          // Norsk
   "Toisto: kellonajat",        // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Default OSD Font",          // English
   "Std. OSD Font",             // Deutsch
   "",                          // Slovenski
   "Carattere OSD predefinito", // Italiano
   "Standaard OSDfont",         // Nederlands
   "",                          // Portugu�s
   "OSD standard",              // Fran�ais
   "",                          // Norsk
   "oletuskirjasintyyppi",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Default Fixed Size Font",   // English
   "Std. Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "Carattere dimensione fissa pred.",// Italiano
   "Standaardwaarde font",     // Nederlands
   "",                          // Portugu�s
   "Largeur fixe standard",     // Fran�ais
   "",                          // Norsk
   "tasav�linen kirjasintyyppi", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Default Small Font",        // English
   "Std. kleine Schrift",       // Deutsch
   "",                          // Slovenski
   "Carattere piccolo pred.",   // Italiano
   "Standaard klein font",      // Nederlands
   "",                          // Portugu�s
   "Petite police standard",           // Fran�ais
   "",                          // Norsk
   "pieni kirjasintyyppi",      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Channel:",                  // English
   "Kanal:",                    // Deutsch
   "",                          // Slovenski
   "Canale:",                   // Italiano
   "Kanaal:",                   // Nederlands
   "",                          // Portugu�s
   "Cha�ne:",                   // Fran�ais
   "",                          // Norsk
   "Kanava:",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Search pattern:",           // English
   "Suchmuster:",               // Deutsch
   "",                          // Slovenski
   "Modello di ricerca:",       // Italiano
   "Zoekpatroon:",              // Nederlands
   "",                          // Portugu�s
   "Mod�le de recherche",       // Fran�ais
   "",                          // Norsk
   "Hakutapa:",                 // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"No timer check",            // English
   "Ohne �berwachung",          // Deutsch
   "",                          // Slovenski
   "Nessun timer verifica",     // Italiano
   "Geen controle",             // Nederlands
   "",                          // Portugu�s
   "Sans surveillance",         // Fran�ais
   "",                          // Norsk
   "Ei valvontaa",              // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"General",                   // English
   "Allgemein",                 // Deutsch
   "",                          // Slovenski
   "Generale",                  // Italiano
   "Algemeen",                  // Nederlands
   "",                          // Portugu�s
   "G�n�ralit�",                // Fran�ais
   "",                          // Norsk
   "Yleiset",                   // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Name",                      // English
   "Name",                      // Deutsch
   "",                          // Slovenski
   "Nome",                      // Italiano
   "Naam",                      // Nederlands
   "",                          // Portugu�s
   "Nom",                       // Fran�ais
   "",                          // Norsk
   "Nimi",                      // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Width",                     // English
   "Weite",                     // Deutsch
   "",                          // Slovenski
   "Larghezza",                 // Italiano
   "Breedte",                   // Nederlands
   "",                          // Portugu�s
   "Largeur",                   // Fran�ais
   "",                          // Norsk
   "Leveys",                    // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Button$Set",                // English
   "�ndern",                    // Deutsch
   "",                          // Slovenski
   "Imposta",                   // Italiano
   "Veranderen",                // Nederlands
   "",                          // Portugu�s
   "Changer",                   // Fran�ais
   "",                          // Norsk
   "Aseta",                     // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"TrueType Font",             // English
   "TrueType Schrift",          // Deutsch
   "",                          // Slovenski
   "Carattere TrueType",        // Italiano
   "TrueType Font",             // Nederlands
   "",                          // Portugu�s
   "TrueType",                  // Fran�ais
   "",                          // Norsk
   "TrueType-kirjasin",         // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"No TTF support!",            // English
   "TTF nicht unterst�tzt!",    // Deutsch
   "",                          // Slovenski
   "Nessun supporto TTF!",      // Italiano
   "TTF niet ondersteund!",   // Nederlands
   "",                          // Portugu�s
   "Les polices TrueType ne sont pas support�es!", // Fran�ais
   "",                          // Norsk
   "Ei TrueType-kirjasintyyppi tukea!", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
  {"Fixed Font",                // English
   "Schrift mit fester Breite", // Deutsch
   "",                          // Slovenski
   "Carattere fisso",           // Italiano
   "Vaste waarde font",         // Nederlands
   "",                          // Portugu�s
   "Police avec une largeur fixe", // Fran�ais
   "",                          // Norsk
   "Tasav�linen kirjasintyyppi", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
#ifdef USE_PLUGIN_AVARDS
  {"Dynamic OSD size",          // English
   "Dynamische OSD-Gr��e",      // Deutsch
   "",                          // Slovenski
   "Dimensione OSD dinamica",   // Italiano
   "Dynamisch OSDgrootte",      // Nederlands
   "",                          // Portugu�s
   "Taille OSD dynamique",      // Fran�ais
   "",                          // Norsk
   "Vaihtuva kuvaruutun�yt�n koko", // suomi (Finnish)
   "",                          // Polski
   "",                          // Espa�ol
   "",                          // �������� (Greek)
   "",                          // Svenska
   "",                          // Rom�n�
   "",                          // Magyar
   "",                          // Catal�
   "",                          // ������� (Russian)
   "",                          // Hrvatski
   "",                          // Eesti
   "",                          // Dansk
   "",                          // �esky (Czech)
   },
#endif
  {"No TrueType fonts installed!",          // English
   "Keine TrueType Schriften installiert!", // Deutsch
   "",                                      // Slovenski
   "Nessun carattere TrueType installato!", // Italiano
   "Geen TrueType fonten geinstalleerd!",   // Nederlands
   "",                                      // Portugu�s
   "Il n'y a pas de police TrueType d'install�!",  // Fran�ais
   "",                                      // Norsk
   "TrueType-kirjasintyyppej� ei l�ydy!",   // suomi (Finnish)
   "",                                      // Polski
   "",                                      // Espa�ol
   "",                                      // �������� (Greek)
   "",                                      // Svenska
   "",                                      // Rom�n�
   "",                                      // Magyar
   "",                                      // Catal�
   "",                                      // ������� (Russian)
   "",                                      // Hrvatski
   "",                                      // Eesti
   "",                                      // Dansk
   "",                                      // �esky (Czech)
   },
  {NULL}
};
// vim:et:sw=2:ts=2:

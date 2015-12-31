/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include "common.h"
#include "i18n.h"

const char *i18n_name = 0;

#if APIVERSNUM < 10507

const tI18nPhrase Phrases[] = {
/*
 * French     translations provided by Maxime Guilbot <maxime.guilbot@orange.fr>
 *               additions provided by Pierre-Henri Beguin <ph.beguin@free.fr>
 *               additions provided by Jerome Rousset <zekje@hotmail.com>
 * Slovenian  translations provided by Matjaz Thaler <matjaz.thaler@guest.arnes.si>
 * Greek      translations provided by Dimitrios Dimitrakos <mail@dimitrios.de>
 * Swedish    translations provided by Jan Ekholm <chakie@infa.abo.fi>
 * Spanish &
 * Catalan    translations provided by Ramon Roca <ramon.roca@xcombo.com>
 * Finnish    translations provided by Rolf Ahrenberg <rahrenbe@cc.hut.fi>
 *               additions provided by Ville Skyttä <ville.skytta@iki.fi>
 * Russian    translations provided by Vyacheslav Dikonov <sdiconov@mail.ru>
*/
  { "Parent",
    "Zurück",
    "Nazaj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parent",
    "", // TODO
    "Takaisin",
    "", // TODO
    "Predecesor",
    "Piso",
    "Tillbaka",
    "", // TODO
    "", // TODO
    "Anterior",
#if APIVERSNUM >= 10302
    "²ÒÕàå",
#endif
  },
  { "Select",
    "Auswählen",
    "Izberi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Sélectionner",
    "", // TODO
    "Valitse",
    "", // TODO
    "Seleccionar",
    "Epilogi",
    "Välj",
    "", // TODO
    "", // TODO
    "Escollir",
#if APIVERSNUM >= 10302
    "²ëÑàÐâì",
#endif
  },
  { "Error scanning directory!",
    "Fehler beim Lesen des Verzeichnisses!",
    "Napaka pri pregledovanju direktorija!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours du répertoire!",
    "", // TODO
    "Hakemiston avaaminen epäonnistui!",
    "", // TODO
    "¡Error al leer una carpeta!",
    "Lathos stin sarosi tou fakelou!",
    "Kunde inte läsa katalogen!",
    "", // TODO
    "", // TODO
    "Error al llegir una carpeta!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ áÚÐÝØàÞÒÐÝØï ÚÐâÐÛÞÓÐ!",
#endif
  },
  { "Source",
    "Datenträger",
    "Izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source",
    "", // TODO
    "Lähde",
    "", // TODO
    "Origen",
    "Pigi",
    "Källa",
    "", // TODO
    "", // TODO
    "Orígen",
#if APIVERSNUM >= 10302
    "´ØáÚ",
#endif
  },
  { "Mount",
    "Einbinden",
    "Priklopi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Monter",
    "", // TODO
    "Kiinnitä",
    "", // TODO
    "Montar",
    "Sindesi",
    "Montera",
    "", // TODO
    "", // TODO
    "Mount",
#if APIVERSNUM >= 10302
    "¿ÞÔÚÛîçØâì",
#endif
  },
  { "Unmount",
    "Aushängen",
    "Izklopi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Démonter",
    "", // TODO
    "Irrota",
    "", // TODO
    "Desmontar",
    "Aposindesi",
    "Avmontera",
    "", // TODO
    "", // TODO
    "Unmount",
#if APIVERSNUM >= 10302
    "¾âÚÛîçØâì",
#endif
  },
  { "Eject",
    "Auswerfen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Avaa",
    "", // TODO
    "Expulsar",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "", // TODO
#endif
  },
  { "Selected source is not mounted!",
    "Ausgewählte Datenträger ist nicht eingebunden!",
    "Izbran izvor ni prikljucen!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source sélectionnée non montée!",
    "", // TODO
    "Valittu lähde ei ole kiinnitetty!",
    "", // TODO
    "¡El origen deseado no está montado!",
    "Epilegmeni Pigi den ine sindemeni!",
    "Den valda källan är inte monterad!",
    "", // TODO
    "", // TODO
    "l'Origen sel.leccionat no està muntat!",
#if APIVERSNUM >= 10302
    "²ëÑàÐÝÝëÙ ÔØáÚ ÝÕ ßÞÔÚÛîçÕÝ!",
#endif
  },
  { "Mount succeeded",
    "Einbinden erfolgreich",
    "Priklop izveden",
    "", // TODO
    "", // TODO
    "", // TODO
    "Montage réussi",
    "", // TODO
    "Kiinnittäminen onnistui",
    "", // TODO
    "Montaje correcto",
    "I sindesi petixe",
    "Monteringen lyckades",
    "", // TODO
    "", // TODO
    "mount correcte",
#if APIVERSNUM >= 10302
    "´ØáÚ ßÞÔÚÛîçÕÝ",
#endif
  },
  { "Mount failed!",
    "Einbinden fehlgeschlagen!",
    "Napaka pri priklopu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du montage!",
    "", // TODO
    "Kiinnittäminen epäonnistui!",
    "", // TODO
    "¡Montaje erróneo!",
    "I sindesi apetixe!",
    "Monteringen misslyckades!",
    "", // TODO
    "", // TODO
    "No he pogut muntar!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ ßÞÔÚÛîçÕÝØï ÔØáÚÐ!",
#endif
  },
  { "Unmount succeeded",
    "Aushängen erfolgreich",
    "Izklop izveden",
    "", // TODO
    "", // TODO
    "", // TODO
    "Démontage réussi",
    "", // TODO
    "Irrottaminen onnistui",
    "", // TODO
    "Desmontaje correcto",
    "I aposindesi itan epitixisi",
    "Avmonteringen lyckades",
    "", // TODO
    "", // TODO
    "unmount amb èxit",
#if APIVERSNUM >= 10302
    "´ØáÚ ÞâÚÛîçÕÝ",
#endif
  },
  { "Unmount failed!",
    "Aushängen fehlgeschlagen!",
    "Napaka pri izklopu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du démontage!",
    "", // TODO
    "Irrottaminen epäonnistui!",
    "", // TODO
    "¡Desmontaje erróneo!",
    "I aposindesi den itan epitixis!",
    "Avmonteringen misslyckades!",
    "", // TODO
    "", // TODO
    "No puc desmontar!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ ÞâÚÛîçÕÝØï ÔØáÚÐ!",
#endif
  },
  { "Eject failed!",
    "Auswerfen fehlgeschlagen!",
    "Napaka pri izmetu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec de l'éjection!",
    "", // TODO
    "Aseman avaaminen epäonnistui!",
    "", // TODO
    "¡Expulsión errónea!",
    "I apovoli apetixe!",
    "Mata ut!",
    "", // TODO
    "", // TODO
    "No puc expulsar!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ Ø×ÒÛÕçÕÝØï ÔØáÚÐ!",
#endif
  },
  { "Hide mainmenu entry",
    "Hauptmenüeintrag verstecken",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Cacher l entrée du menu",
    "", // TODO
    "Piilota valinta päävalikosta",
    "", // TODO
    "Ocultar entrada en menú principal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "ÁÚàëâì ÚÞÜÐÝÔã Ò ÜÕÝî",
#endif
  },

// start of MP3 specific phrases

  { "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3-soitin",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
#if APIVERSNUM >= 10302
    "¿àÞØÓàëÒÐâÕÛì MP3",
#endif
  },
  { "A versatile audio player",
    "Ein vielseitiger Audio-Player",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Lecteur multiformats",
    "", // TODO
    "Monipuolinen audiosoitin",
    "", // TODO
    "Reproductor de audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "¿àÞØÓàëÒÐâÕÛì ÐãÔØÞäÐÙÛÞÒ",
#endif
  },
  { "Browse",
    "Blättern",
    "Isci",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parcourir",
    "", // TODO
    "Selaa",
    "", // TODO
    "Explorar",
    "Selida",
    "Bläddra",
    "", // TODO
    "", // TODO
    "Explorar",
#if APIVERSNUM >= 10302
    "ºÐâÐÛÞÓØ",
#endif
  },
  { "MP3 source",
    "MP3 Datenträger",
    "MP3 izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source MP3",
    "", // TODO
    "MP3-lähde",
    "", // TODO
    "Origen MP3",
    "Pigi MP3",
    "MP3 källa",
    "", // TODO
    "", // TODO
    "Orígen MP3",
#if APIVERSNUM >= 10302
    "´ØáÚ MP3",
#endif
  },
  { "Rename",
    "Umbenennen",
    "Preimenuj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer",
    "", // TODO
    "Nimeä",
    "", // TODO
    "Renombrar",
    "Alagi Onomatos",
    "Döp om",
    "", // TODO
    "", // TODO
    "Reanomenar",
#if APIVERSNUM >= 10302
    "¿ÕàÕØÜÕÝÞÒÐâì",
#endif
  },
  { "Add",
    "Hinzufügen",
    "Dodaj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter",
    "", // TODO
    "Lisää",
    "", // TODO
    "Añadir",
    "Prosthesi",
    "Lägg till",
    "", // TODO
    "", // TODO
    "Afegir",
#if APIVERSNUM >= 10302
    "´ÞÑÐÒØâì",
#endif
  },
  { "Remove",
    "Entfernen",
    "Odstrani",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever",
    "", // TODO
    "Poista",
    "", // TODO
    "Borrar",
    "Aferesi",
    "Radera",
    "", // TODO
    "", // TODO
    "Esborrar",
#if APIVERSNUM >= 10302
    "ÃÔÐÛØâì",
#endif
  },
  { "Play all",
    "Alle abspielen",
    "Predvajaj vse",
    "", // TODO
    "", // TODO
    "", // TODO
    "Jouer tous",
    "", // TODO
    "Soita kaikki",
    "", // TODO
    "Reproducir todo",
    "Peksimo olon",
    "Spela alla",
    "", // TODO
    "", // TODO
    "Escoltar-ho tot",
#if APIVERSNUM >= 10302
    "²ÞáßàÞØ×ÒÕáâØ Òáñ",
#endif
  },
  { "Add all",
    "Alle hinzufügen",
    "Dodaj vse",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter tous",
    "", // TODO
    "Lisää kaikki",
    "", // TODO
    "Añadir todo",
    "Prosthesi olon",
    "Lägg till alla",
    "", // TODO
    "", // TODO
    "Afegir-ho tot",
#if APIVERSNUM >= 10302
    "´ÞÑÐÒØâì Òáñ",
#endif
  },
  { "Empty directory!",
    "Leeres Verzeichnis!",
    "Direktorij prazen!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Répertoire vide!",
    "", // TODO
    "Hakemisto on tyhjä!",
    "", // TODO
    "¡Carpeta vacía!",
    "Adios fakelos!",
    "Tom katalog!",
    "", // TODO
    "", // TODO
    "Carpeta buida!",
#if APIVERSNUM >= 10302
    "¿ãáâÞÙ ÚÐâÐÛÞÓ!",
#endif
  },
  { "Add recursivly?",
    "Recursiv hinzufügen?",
    "Rekurzivno dodaj?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter récursivement?",
    "", // TODO
    "Lisää rekursiivisesti?",
    "", // TODO
    "¿Añadir recursivamente?",
    "Prosthesi recursiv?",
    "Lägg till rekursivt?",
    "", // TODO
    "", // TODO
    "Afegir recursivament?",
#if APIVERSNUM >= 10302
    "´ÞÑÐÒØâì àÕÚãàáØÒÝÞ?",
#endif
  },
  { "Remove entry?",
    "Eintrag entfernen?",
    "Odstrani izbrano?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever titre?",
    "", // TODO
    "Poistetaanko merkintä?",
    "", // TODO
    "¿Borrar entrada?",
    "Aferesi simiou?",
    "Radera post?",
    "", // TODO
    "", // TODO
    "Esborrar la entrada?",
#if APIVERSNUM >= 10302
    "ÃÔÐÛØâì ×ÐßØáì?",
#endif
  },
  { "Playlist editor",
    "Abspielliste bearbeiten",
    "Urejevalnik liste",
    "", // TODO
    "", // TODO
    "", // TODO
    "Editeur de liste de lecture",
    "", // TODO
    "Soittolistan muokkaus",
    "", // TODO
    "Editor de listas de reproducción",
    "Metatropes stin Playlista",
    "Spellisteditor",
    "", // TODO
    "", // TODO
    "Editor de llistes de reproducció",
#if APIVERSNUM >= 10302
    "ÀÕÔÐÚâÞà áßØáÚÐ ßÕáÕÝ",
#endif
  },
  { "Directory browser",
    "Verzeichnisanzeige",
    "Navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "Hakemistoselain",
    "", // TODO
    "Explorar carpetas",
    "Endiski fakelou",
    "Katalogbläddrare",
    "", // TODO
    "", // TODO
    "Navegar per les carpetes",
#if APIVERSNUM >= 10302
    "¿àÞáÜÞâà ÚÐâÐÛÞÓÞÒ",
#endif
  },
  { "Delete playlist?",
    "Abspielliste löschen?",
    "Odstrani listo?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Effacer liste?",
    "", // TODO
    "Poistetaanko soittolista?",
    "", // TODO
    "¿Borrar la lista?",
    "Svisimo listas?",
    "Radera spellista?",
    "", // TODO
    "", // TODO
    "Esborrar la Llista?",
#if APIVERSNUM >= 10302
    "ÃÔÐÛØâì áßØáÞÚ",
#endif
  },
  { "Are you sure?",
    "Wirklich löschen?",
    "Ste prepricani?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Etes-vous sûr?",
    "", // TODO
    "Oletko varma?",
    "", // TODO
    "¿Está seguro?",
    "Ise sigouros?",
    "Är du säker?",
    "", // TODO
    "", // TODO
    "N'esteu segur?",
#if APIVERSNUM >= 10302
    "²ë ãÒÕàÕÝë?",
#endif
  },
  { "unnamed",
    "unbenannt",
    "neimenovan",
    "", // TODO
    "", // TODO
    "", // TODO
    "sansnom",
    "", // TODO
    "nimetön",
    "", // TODO
    "sin nombre",
    "xoris onoma",
    "namnlös",
    "", // TODO
    "", // TODO
    "sense nom",
#if APIVERSNUM >= 10302
    "ÑÕ×ëÜïÝÝëÙ",
#endif
  },
  { "unknown",
    "unbekannt",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "inconnu",
    "", // TODO
    "tuntematon",
    "", // TODO
    "desconocido",
    "", // TODO
    "okänd",
    "", // TODO
    "", // TODO
    "desconegut",
#if APIVERSNUM >= 10302
    "ÝÕØ×ÒÕáâÝëÙ",
#endif
  },
  { "Error scanning playlists!",
    "Fehler beim Einlesen der Abspiellisten!",
    "Napaka pri pregledovanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours de la liste!",
    "", // TODO
    "Soittolistojen lukeminen epäonnistui!",
    "", // TODO
    "¡Error al leer las listas!",
    "Lathos stin sarosi tis Playlistas!",
    "Fel uppstod då spellistorna lästes!",
    "", // TODO
    "", // TODO
    "Error al llegir les llistes!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ ßÞØáÚÐ áßØáÚÞÒ ßÕáÕÝ!",
#endif
  },
  { "Error deleting playlist!",
    "Fehler beim Löschen der Abspielliste!",
    "Napaka pri odstranjevanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur d'effacement de la liste!",
    "", // TODO
    "Soittolistan poistaminen epäonnistui!",
    "", // TODO
    "¡Error al borrar una lista!",
    "Lathos stin akirosi tis Playlistas!",
    "Fel uppstod då spellistan raderades!",
    "", // TODO
    "", // TODO
    "Error al esborrar una llista!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ ãÔÐÛÕÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Error creating playlist!",
    "Fehler beim Erstellen der Abspielliste!",
    "Napaka pri kreiranju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de création de la liste!",
    "", // TODO
    "Soittolistan luominen epäonnistui!",
    "", // TODO
    "¡Error al crear una lista!",
    "Lathos stin dimiourgia tis Playlistas!",
    "Fel uppstod då spellistan skapades!",
    "", // TODO
    "", // TODO
    "Error al crear una llista!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ áÞ×ÔÐÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Can't edit a WinAmp playlist!",
    "Editieren von WinAmp Abspiellisten nicht möglich!",
    "Ne morem vnesti liste od WinAmp-a!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ne peut pas éditer une liste WinAmp!",
    "", // TODO
    "WinAmp-soittolistaa ei voida muokata!",
    "", // TODO
    "¡No puedo editar una lista del WinAmp!",
    "Den mporo na kano metatropes se WinAmp Playlista!",
    "Kan inte redigera en WinAmp-spellista!",
    "", // TODO
    "", // TODO
    "No puc editar una llista del WinAmp!",
#if APIVERSNUM >= 10302
    "½ÕÒÞ×ÜÞÖÝÞ àÕÔÐÚâØàÞÒÐâì áßØáÞÚ WinAmp!",
#endif
  },
  { "Error renaming playlist!",
    "Fehler beim Umbenennen der Abspielliste!",
    "Napaka pri preimenovanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur pour renommer la liste!",
    "", // TODO
    "Soittolistan uudelleennimeäminen epäonnistui!",
    "", // TODO
    "¡Error al renombrar la lista!",
    "Latsos stin metonomasi tis Playlistas!",
    "Fel uppstod då spellistan döptes om!",
    "", // TODO
    "", // TODO
    "Error al reanomenar la llista!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ ßÕàÕØÜÕÝÞÒÐÝØï áßØáÚÐ!",
#endif
  },
  { "Loading playlist...",
    "Lade Abspielliste...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Chargement playlist...",
    "", // TODO
    "Avataan soittolistaa...",
    "", // TODO
    "Cargando la lista de reproducción...",
    "", // TODO
    "Läser in spellista...",
    "", // TODO
    "", // TODO
    "Carregant la llista de reproducció...",
#if APIVERSNUM >= 10302
    "ÇâÕÝØÕ áßØáÚÐ ßÕáÕÝ...",
#endif
  },
  { "Error loading playlist!",
    "Fehler beim Lesen der Abspielliste!",
    "Napaka pri nalaganju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de chargement de la playlist!",
    "", // TODO
    "Soittolistan avaaminen epäonnistui!",
    "", // TODO
    "¡Error al cargar la lista de reproducción!",
    "Lathos sto fortoma tis Playlistas!",
    "Fel uppstod då spellistan lästes in!",
    "", // TODO
    "", // TODO
    "Error al carregar la llista de reproducció!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ çâÕÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Scanning playlists...",
    "Durchsuche Abspiellisten...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Scanne playlists...",
    "", // TODO
    "Selataan soittolistoja...",
    "", // TODO
    "Leyendo las listas de reproducción...",
    "", // TODO
    "Söker spellistor...",
    "", // TODO
    "", // TODO
    "Llegint les llistes de reproducció...",
#if APIVERSNUM >= 10302
    "¿ÞØáÚ áßØáÚÞÒ ßÕáÕÝ...",
#endif
  },
  { "Scanning directory...",
    "Durchsuche Verzeichnis...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Scanne le repertoire...",
    "", // TODO
    "Selataan hakemistoa...",
    "", // TODO
    "Leyendo las carpetas...",
    "", // TODO
    "Söker igenom katalog...",
    "", // TODO
    "", // TODO
    "Revisant les carpetes..",
#if APIVERSNUM >= 10302
    "ÁÚÐÝØàÞÒÐÝØÕ ÚÐâÐÛÞÓÞÒ...",
#endif
  },
  { "Building playlist...",
    "Baue Abspielliste auf...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Construit la playlist...",
    "", // TODO
    "Muodostetaan soittolistaa...",
    "", // TODO
    "Construyendo la lista de reproducción...",
    "", // TODO
    "Skapar en spellista...",
    "", // TODO
    "", // TODO
    "Construïnt la llista de reproducció...",
#if APIVERSNUM >= 10302
    "¿ÞáâàÞÕÝØÕ áßØáÚÐ ßÕáÕÝ...",
#endif
  },
  { "Error building playlist!",
    "Fehler beim Aufbau der Abspielliste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur construction playlist!",
    "", // TODO
    "Soittolistan muodostaminen epäonnistui!",
    "", // TODO
    "¡Error al construir la lista de reproducción!",
    "", // TODO
    "Fel uppstod då spellistan skapades!",
    "", // TODO
    "", // TODO
    "Error al consstruir la llista de reproducció!",
#if APIVERSNUM >= 10302
    "¾èØÑÚÐ ßÞáâàÞÕÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Remote CDDB lookup...",
    "Remote CDDB Abfrage...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Consultation CDDB...",
    "", // TODO
    "Kysytään CDDB-tietoja...",
    "", // TODO
    "Consultando CDDB...",
    "", // TODO
    "Fjärrförfrågan till CDDB...",
    "", // TODO
    "", // TODO
    "Consultant CDDB...",
#if APIVERSNUM >= 10302
    "·ÐßàÞá Ú ãÔÐÛñÝÝÞÙ ÑÐ×Õ CDDB...",
#endif
  },
  { "Connecting to stream server ...",
    "Verbinde mit Stream Server...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Connexion au Serveur Stream...",
    "", // TODO
    "Yhdistetään palvelimeen...",
    "", // TODO
    "Conectando con el servidor...",
    "", // TODO
    "Kontaktar stream-server...",
    "", // TODO
    "", // TODO
    "Connectant al servidor...",
#if APIVERSNUM >= 10302
    "¿ÞÔÚÛîçÕÝØÕ Ú áÕàÒÕàã ßÞâÞÚÞÒ...",
#endif
  },
  { "Rename playlist",
    "Abspielliste umbenennen",
    "Preimenuj listo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer la liste",
    "", // TODO
    "Nimeä soittolista",
    "", // TODO
    "Renombrar la lista de reproducción",
    "Metonomasi tis Playlistas",
    "Döp om spellistan",
    "", // TODO
    "", // TODO
    "Reanomenar la llista de repr.",
#if APIVERSNUM >= 10302
    "¿ÕàÕØÜÕÝÞÒÐâì áßØáÞÚ ßÕáÕÝ",
#endif
  },
  { "Old name:",
    "Alter Name:",
    "Staro ime:",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ancien nom:",
    "", // TODO
    "Vanha nimi",
    "", // TODO
    "Nombre anterior:",
    "Palio onoma:",
    "Gammalt namn:",
    "", // TODO
    "", // TODO
    "Nom anterior:",
#if APIVERSNUM >= 10302
    "ÁâÐàÞÕ ØÜï:",
#endif
  },
  { "New name",
    "Neuer Name",
    "Novo ime",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nouveau nom",
    "", // TODO
    "Uusi nimi",
    "", // TODO
    "Nombre nuevo",
    "Neo onoma",
    "Nytt namn",
    "", // TODO
    "", // TODO
    "Nou nom",
#if APIVERSNUM >= 10302
    "½ÞÒÞÕ ØÜï:",
#endif
  },
  { "Filenames",
    "Dateinamen",
    "Imena datotek",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noms de fichiers",
    "", // TODO
    "tiedostonimet",
    "", // TODO
    "Nombre de los archivos",
    "Onomata arxeion",
    "Filnamn",
    "", // TODO
    "",
    "Nom dels arxius",
#if APIVERSNUM >= 10302
    "ØÜÕÝÐ äÐÙÛÞÒ",
#endif
  },
  { "ID3 names",
    "ID3 Namen",
    "ID3 imena",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom ID3",
    "", // TODO
    "ID3-nimet",
    "", // TODO
    "Nombres ID3",
    "ID3 onomata",
    "ID3 namn",
    "", // TODO
    "", // TODO
    "Nom ID3",
#if APIVERSNUM >= 10302
    "ID3-ØÜÕÝÐ",
#endif
  },
  { "ID3 information",
    "ID3 Information",
    "ID3 informacija",
    "", // TODO
    "", // TODO
    "", // TODO
    "Information ID3",
    "", // TODO
    "ID3-informaatio",
    "", // TODO
    "Información ID3",
    "Plirofories ID3",
    "ID3 information",
    "", // TODO
    "", // TODO
    "Informació ID3",
#if APIVERSNUM >= 10302
    "ID3-ÜÕâÚØ",
#endif
  },
  { "ID3 info",
    "ID3 Info",
    "ID3 info",
    "", // TODO
    "", // TODO
    "", // TODO
    "Info ID3",
    "", // TODO
    "ID3-info",
    "", // TODO
    "Info ID3",
    "ID3 pliroforia",
    "ID3 info",
    "", // TODO
    "", // TODO
    "ID3 info",
#if APIVERSNUM >= 10302
    "ÔÐÝÝëÕ ID3",
#endif
  },
  { "Filename",
    "Dateiname",
    "Ime datoteke",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du fichier",
    "", // TODO
    "Tiedostonimi",
    "", // TODO
    "Nombre del archivo",
    "Onoma arxeiou",
    "Filnamn",
    "", // TODO
    "", // TODO
    "Nom de l'arxiu",
#if APIVERSNUM >= 10302
    "¸Üï äÐÙÛÐ",
#endif
  },
  { "Length",
    "Länge",
    "Dolzina",
    "", // TODO
    "", // TODO
    "", // TODO
    "Longueur",
    "", // TODO
    "Pituus",
    "", // TODO
    "Duración",
    "Megethos",
    "Längd",
    "", // TODO
    "", // TODO
    "Durada",
#if APIVERSNUM >= 10302
    "´ÛØÝÐ",
#endif
  },
  { "Title",
    "Titel",
    "Naslov",
    "", // TODO
    "", // TODO
    "", // TODO
    "Titre",
    "", // TODO
    "Kappale",
    "", // TODO
    "Título",
    "Titlos",
    "Tittel",
    "", // TODO
    "", // TODO
    "Títol",
#if APIVERSNUM >= 10302
    "½Ð×ÒÐÝØÕ",
#endif
  },
  { "Artist",
    "Interpret",
    "Avtor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Artiste",
    "", // TODO
    "Esittäjä",
    "", // TODO
    "Artista",
    "Ermineftis",
    "Artist",
    "", // TODO
    "", // TODO
    "Intèrpret",
#if APIVERSNUM >= 10302
    "¸áßÞÛÝØâÕÛì",
#endif
  },
  { "Album",
    "Album",
    "Album",
    "", // TODO
    "", // TODO
    "", // TODO
    "Album",
    "", // TODO
    "Albumi",
    "", // TODO
    "Álbum",
    "Album",
    "Album",
    "", // TODO
    "", // TODO
    "Àlbum",
#if APIVERSNUM >= 10302
    "°ÛìÑÞÜ",
#endif
  },
  { "Year",
    "Jahr",
    "Leto",
    "", // TODO
    "", // TODO
    "", // TODO
    "Année",
    "", // TODO
    "Vuosi",
    "", // TODO
    "Año",
    "Etos",
    "År",
    "", // TODO
    "", // TODO
    "Any",
#if APIVERSNUM >= 10302
    "³ÞÔ",
#endif
  },
  { "Samplerate",
    "Sample Rate",
    "Vzorcenje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Fréquence",
    "", // TODO
    "Näytteenottotaajuus",
    "", // TODO
    "Frecuencia",
    "Sixnotita",
    "Samplingshastighet",
    "", // TODO
    "", // TODO
    "Freqüència",
#if APIVERSNUM >= 10302
    "ÇÐáâÞâÐ",
#endif
  },
  { "Bitrate",
    "Bit Rate",
    "Bitna hitrost",
    "", // TODO
    "", // TODO
    "", // TODO
    "Bitrate",
    "", // TODO
    "Bittinopeus",
    "", // TODO
    "Bitrate",
    "Bitrate",
    "Bithastighet",
    "", // TODO
    "", // TODO
    "Bitrate",
#if APIVERSNUM >= 10302
    "¿ÞâÞÚ",
#endif
  },
  { "Setup.MP3$Initial loop mode",
    "Default Loop Modus",
    "Osnovni nacin ponavljanje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de répétition initial",
    "", // TODO
    "Jatkuva soitto oletuksena",
    "", // TODO
    "Modo de repetición inicial",
    "", // TODO
    "Normalt upprepningsläge",
    "", // TODO
    "", // TODO
    "Mode de repetició inicial",
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ ßÞÒâÞàÐ ßÞ ãÜÞÛçÐÝØî",
#endif
  },
  { "Setup.MP3$Initial shuffle mode",
    "Default Shuffle Modus",
    "Osnovni nacin mesano",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de lecture aléatoire initial",
    "", // TODO
    "Satunnaissoitto oletuksena",
    "", // TODO
    "Lectura aleatoria inicial",
    "", // TODO
    "Normalt blandläge",
    "", // TODO
    "", // TODO
    "Lectura aleatòria inicial",
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ áÛãçÐÙÝ. ÒëÑÞàÐ ßÞ ãÜÞÛçÐÝØî",
#endif
  },
  { "Setup.MP3$Title/Artist order",
    "Title/Artist Reihenfolge",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Näytä kappale ja esittäjä",
    "", // TODO
    "Orden Título/Artista",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Normal",
    "Normal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "normaalisti",
    "", // TODO
    "Normal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Reversed",
    "Umgedreht",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "käännettynä",
    "", // TODO
    "Inversa",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Audio output mode",
    "Audio Ausgabe Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de sortie audio",
    "", // TODO
    "Äänen ulostulomoodi",
    "", // TODO
    "Modo de salida audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ ÒëÒÞÔÐ ×ÒãÚÐ",
#endif
  },
  { "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
#if APIVERSNUM >= 10302
    "OSS",
#endif
  },
  { "Setup.MP3$Background scan",
    "Hintergrund Scan",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Scan en tache de fond",
    "", // TODO
    "Taustaskannaus",
    "", // TODO
    "Explorar en segundo plano",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "ÄÞÝÞÒÞÕ áÚÐÝØàÞÒÐÝØÕ",
#endif
  },
  { "ID3 tags",
    "ID3 Tags",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Seulement ID3",
    "", // TODO
    "vain ID3",
    "", // TODO
    "Etiquetas ID3",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "ÂÞÛìÚÞ ID3",
#endif
  },
  { "Setup.MP3$Editor display mode",
    "Editor Anzeige Modus",
    "Nacin prikaza urejevalnika",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage de l'éditeur",
    "", // TODO
    "Muokkaustilan näyttömoodi",
    "", // TODO
    "Modo de visualización del editor",
    "", // TODO
    "Redigerarens visuella läge",
    "", // TODO
    "", // TODO
    "Mode de visualització del editor",
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ àÕÔÐÚâÞàÐ áßØáÚÞÒ",
#endif
  },
  { "Setup.MP3$Display mode",
    "Anzeige Modus",
    "Nacin prikaza",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage",
    "", // TODO
    "Näyttömoodi",
    "", // TODO
    "Modo de visualización",
    "", // TODO
    "Visuellt läge",
    "", // TODO
    "", // TODO
    "Mode de visualització",
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ ØÝâÕàäÕÙáÐ",
#endif
  },
  { "title, artist",
    "title, artist",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "título, artista",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "", // TODO
#endif
  },
  { "album, year, title, artist",
    "album, year, title, artist",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "álbum, año, título, artista",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "", // TODO
#endif
  },
  { "info, album, year, title, artist",
    "info, album, year, title, artist",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "info, álbum, año, título, artista",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "", // TODO
#endif
  },
  { "Setup.MP3$Direct Playlist",
    "Playlist automatisch anzeigen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Lista de reproducción directa",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "", // TODO
#endif
  },
  { "Setup.MP3$Background mode",
    "Hintergrund Modus",
    "Ozadje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Arrière plan",
    "", // TODO
    "Taustamoodi",
    "", // TODO
    "Modo en segundo plano",
    "", // TODO
    "Bakgrundsläge",
    "", // TODO
    "", // TODO
    "Execució de fons",
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ äÞÝÐ",
#endif
  },
  { "Setup.MP3$CDDB for CD-Audio",
    "CDDB für CD-Audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "CDDB pour CD-Audio",
    "", // TODO
    "CDDB-tietokanta",
    "", // TODO
    "CDDB para el Audio-CD",
    "", // TODO
    "CDDB för CD-audio",
    "", // TODO
    "", // TODO
    "CDDB per al Audio-CD",
#if APIVERSNUM >= 10302
    "CDDB ÔÛï ÐãÔØÞ-CD",
#endif
  },
  { "disabled",
    "aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "désactiver",
    "", // TODO
    "pois",
    "", // TODO
    "desactivado",
    "", // TODO
    "avstängd",
    "", // TODO
    "", // TODO
    "desactivat",
#if APIVERSNUM >= 10302
    "ÒëÚÛ",
#endif
  },
  { "local only",
    "nur lokal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local uniquement",
    "", // TODO
    "vain paikallinen",
    "", // TODO
    "solo local",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "solsament local",
#if APIVERSNUM >= 10302
    "âÞÛìÚÞ ÛÞÚÐÛìÝ.",
#endif
  },
  { "local&remote",
    "lokal und entfernt",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local et distant",
    "", // TODO
    "paikallinen & etä",
    "", // TODO
    "local y remoto",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local i remot",
#if APIVERSNUM >= 10302
    "ÛÞÚÐÛìÝ. Ø ãÔÐÛñÝ.",
#endif
  },
  { "Setup.MP3$Use HTTP proxy",
    "HTTP Proxy benutzen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Utiliser un Proxy HTTP",
    "", // TODO
    "Käytä HTTP-välityspalvelinta",
    "", // TODO
    "Utilizar proxy HTTP",
    "", // TODO
    "Använd en HTTP-proxy",
    "", // TODO
    "", // TODO
    "Util·litzar proxy HTTP",
#if APIVERSNUM >= 10302
    "¸áßÞÛì×ÞÒÐâì HTTP ßàÞÚáØ",
#endif
  },
  { "Setup.MP3$HTTP proxy host",
    "HTTP Proxy Name",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du Proxy HTTP",
    "", // TODO
    "HTTP-välityspalvelimen osoite",
    "", // TODO
    "Nombre del puerto HTTP",
    "", // TODO
    "HTTP-proxyns namn",
    "", // TODO
    "", // TODO
    "Nom del proxy HTTP",
#if APIVERSNUM >= 10302
    "ÁÕàÒÕà HTTP ßàÞÚáØ",
#endif
  },
  { "Setup.MP3$HTTP proxy port",
    "HTTP Proxy Port",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Port du proxy HTTP",
    "", // TODO
    "HTTP-välityspalvelimen portti",
    "", // TODO
    "Puerto del Proxy HTTP",
    "", // TODO
    "HTTP-proxyns port",
    "", // TODO
    "", // TODO
    "Port del proxy HTTP",
#if APIVERSNUM >= 10302
    "¿Þàâ HTTP ßàÞÚáØ",
#endif
  },
  { "Black",
    "Schwarz",
    "Crnina",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noir",
    "", // TODO
    "musta",
    "", // TODO
    "Negra",
    "Mavro",
    "Svart",
    "", // TODO
    "", // TODO
    "Negre",
#if APIVERSNUM >= 10302
    "çñàÝëÙ íÚàÐÝ",
#endif
  },
  { "Images",
    "Bilder",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Images",
    "", // TODO
    "kansikuva",
    "", // TODO
    "Imágenes",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Mainmenu mode",
    "Hauptmenu Modus",
    "Nacin glavnega menija",
    "", // TODO
    "", // TODO
    "", // TODO
    "Menu principal",
    "", // TODO
    "Päävalikon tila",
    "", // TODO
    "Menú Principal",
    "", // TODO
    "Huvudmenyläge",
    "", // TODO
    "", // TODO
    "Menú Principal",
#if APIVERSNUM >= 10302
    "ÀÕÖØÜ ÓÛÐÒÝÞÓÞ ÜÕÝî",
#endif
  },
  { "Playlists",
    "Abspiellisten",
    "Lista",
    "", // TODO
    "", // TODO
    "", // TODO
    "Playlists",
    "", // TODO
    "soittolistat",
    "", // TODO
    "Lista de reproducción",
    "Listes peksimatos",
    "Spellistor",
    "", // TODO
    "", // TODO
    "Llista a reproduïr",
#if APIVERSNUM >= 10302
    "áßØáÚØ ßÕáÕÝ",
#endif
  },
  { "Browser",
    "Verz.anzeige",
    "Navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "selain",
    "", // TODO
    "Navegador",
    "", // TODO
    "Bläddra",
    "", // TODO
    "", // TODO
    "Navegador",
#if APIVERSNUM >= 10302
    "ÚÐâÐÛÞÓØ",
#endif
  },
  { "Setup.MP3$Abort player at end of list",
    "Abspieler am Listenende beenden",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Stopper le lecteur à la fin de la liste",
    "", // TODO
    "Keskeytä soittolistan loputtua",
    "", // TODO
    "Parar reproductor al final de la lista",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "²ëåÞÔ Ò ÚÞÝæÕ áßØáÚÐ ßÕáÕÝ",
#endif
  },
  { "Setup.MP3$Replay display",
    "Wiedergabe Anzeige",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Affichage relecture",
    "", // TODO
    "Toistotilan näyttömoodi",
    "", // TODO
    "Pantalla de reproducción",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "¸ÝâÕàäÕÙá Ò àÕÖØÜÕ ÒÞáßàÞØ×ÒÕÔÕÝØï",
#endif
  },
  { "classic",
    "klassisch",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Classique",
    "", // TODO
    "klassinen",
    "", // TODO
    "clásica",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "ÚÛÐááØçÕáÚØÙ",
#endif
  },
  { "via skin",
    "über Skin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Par skin",
    "", // TODO
    "ulkoasun mukaan",
    "", // TODO
    "vía skin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "áÞÓÛÐáÝÞ âÕÜÕ",
#endif
  },
  { "Setup.MP3$Keep selection menu",
    "Auswahlmenu geöffnet lassen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Pidä valintaikkunaa auki",
    "", // TODO
    "Mantener menú de selección",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },

  { NULL }
  };

#endif

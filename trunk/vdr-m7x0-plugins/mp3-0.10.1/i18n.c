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
 *               additions provided by Ville Skytt� <ville.skytta@iki.fi>
 * Russian    translations provided by Vyacheslav Dikonov <sdiconov@mail.ru>
*/
  { "Parent",
    "Zur�ck",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parent",
    "", // TODO
    "",
    "", // TODO
    "Predecesor",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Select",
    "Ausw�hlen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "S�lectionner",
    "", // TODO
    "",
    "", // TODO
    "Seleccionar",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Error scanning directory!",
    "Fehler beim Lesen des Verzeichnisses!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours du r�pertoire!",
    "", // TODO
    "",
    "", // TODO
    "�Error al leer una carpeta!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Source",
    "Datentr�ger",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source",
    "", // TODO
    "",
    "", // TODO
    "Origen",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Mount",
    "Einbinden",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Monter",
    "", // TODO
    "",
    "", // TODO
    "Montar",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Unmount",
    "Aush�ngen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "D�monter",
    "", // TODO
    "",
    "", // TODO
    "Desmontar",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
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
    "Ausgew�hlte Datentr�ger ist nicht eingebunden!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source s�lectionn�e non mont�e!",
    "", // TODO
    "",
    "", // TODO
    "�El origen deseado no est� montado!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Mount succeeded",
    "Einbinden erfolgreich",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Montage r�ussi",
    "", // TODO
    "",
    "", // TODO
    "Montaje correcto",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Mount failed!",
    "Einbinden fehlgeschlagen!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du montage!",
    "", // TODO
    "",
    "", // TODO
    "�Montaje err�neo!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Unmount succeeded",
    "Aush�ngen erfolgreich",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "D�montage r�ussi",
    "", // TODO
    "",
    "", // TODO
    "Desmontaje correcto",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Unmount failed!",
    "Aush�ngen fehlgeschlagen!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du d�montage!",
    "", // TODO
    "",
    "", // TODO
    "�Desmontaje err�neo!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Eject failed!",
    "Auswerfen fehlgeschlagen!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec de l'�jection!",
    "", // TODO
    "",
    "", // TODO
    "�Expulsi�n err�nea!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Hide mainmenu entry",
    "Hauptmen�eintrag verstecken",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Cacher l entr�e du menu",
    "", // TODO
    "",
    "", // TODO
    "Ocultar entrada en men� principal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },

// start of MP3 specific phrases

  { "MP3",
    "MP3",
    "",
    "",
    "",
    "",
    "MP3",
    "",
    "",
    "",
    "MP3",
    "",
    "",
    "",
    "",
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Reproductor de audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Browse",
    "Bl�ttern",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parcourir",
    "", // TODO
    "",
    "", // TODO
    "Explorar",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "MP3 source",
    "MP3 Datentr�ger",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source MP3",
    "", // TODO
    "",
    "", // TODO
    "Origen MP3",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Rename",
    "Umbenennen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer",
    "", // TODO
    "",
    "", // TODO
    "Renombrar",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Add",
    "Hinzuf�gen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter",
    "", // TODO
    "",
    "", // TODO
    "A�adir",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Remove",
    "Entfernen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever",
    "", // TODO
    "",
    "", // TODO
    "Borrar",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Play all",
    "Alle abspielen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Jouer tous",
    "", // TODO
    "",
    "", // TODO
    "Reproducir todo",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Add all",
    "Alle hinzuf�gen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter tous",
    "", // TODO
    "",
    "", // TODO
    "A�adir todo",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Empty directory!",
    "Leeres Verzeichnis!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "R�pertoire vide!",
    "", // TODO
    "",
    "", // TODO
    "�Carpeta vac�a!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Add recursivly?",
    "Recursiv hinzuf�gen?",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter r�cursivement?",
    "", // TODO
    "",
    "", // TODO
    "�A�adir recursivamente?",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Remove entry?",
    "Eintrag entfernen?",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever titre?",
    "", // TODO
    "",
    "", // TODO
    "�Borrar entrada?",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Playlist editor",
    "Abspielliste bearbeiten",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Editeur de liste de lecture",
    "", // TODO
    "",
    "", // TODO
    "Editor de listas de reproducci�n",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Directory browser",
    "Verzeichnisanzeige",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "",
    "", // TODO
    "Explorar carpetas",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Delete playlist?",
    "Abspielliste l�schen?",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Effacer liste?",
    "", // TODO
    "",
    "", // TODO
    "�Borrar la lista?",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Are you sure?",
    "Wirklich l�schen?",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Etes-vous s�r?",
    "", // TODO
    "",
    "", // TODO
    "�Est� seguro?",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "unnamed",
    "unbenannt",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "sansnom",
    "", // TODO
    "",
    "", // TODO
    "sin nombre",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "desconocido",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Error scanning playlists!",
    "Fehler beim Einlesen der Abspiellisten!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours de la liste!",
    "", // TODO
    "",
    "", // TODO
    "�Error al leer las listas!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Error deleting playlist!",
    "Fehler beim L�schen der Abspielliste!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur d'effacement de la liste!",
    "", // TODO
    "",
    "", // TODO
    "�Error al borrar una lista!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Error creating playlist!",
    "Fehler beim Erstellen der Abspielliste!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de cr�ation de la liste!",
    "", // TODO
    "",
    "", // TODO
    "�Error al crear una lista!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Can't edit a WinAmp playlist!",
    "Editieren von WinAmp Abspiellisten nicht m�glich!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ne peut pas �diter une liste WinAmp!",
    "", // TODO
    "",
    "", // TODO
    "�No puedo editar una lista del WinAmp!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Error renaming playlist!",
    "Fehler beim Umbenennen der Abspielliste!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur pour renommer la liste!",
    "", // TODO
    "",
    "", // TODO
    "�Error al renombrar la lista!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Cargando la lista de reproducci�n...",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Error loading playlist!",
    "Fehler beim Lesen der Abspielliste!",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de chargement de la playlist!",
    "", // TODO
    "",
    "", // TODO
    "�Error al cargar la lista de reproducci�n!",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Leyendo las listas de reproducci�n...",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Leyendo las carpetas...",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Construyendo la lista de reproducci�n...",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "�Error al construir la lista de reproducci�n!",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Consultando CDDB...",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Conectando con el servidor...",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Rename playlist",
    "Abspielliste umbenennen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer la liste",
    "", // TODO
    "",
    "", // TODO
    "Renombrar la lista de reproducci�n",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Old name:",
    "Alter Name:",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ancien nom:",
    "", // TODO
    "",
    "", // TODO
    "Nombre anterior:",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "New name",
    "Neuer Name",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nouveau nom",
    "", // TODO
    "",
    "", // TODO
    "Nombre nuevo",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Filenames",
    "Dateinamen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noms de fichiers",
    "", // TODO
    "",
    "", // TODO
    "Nombre de los archivos",
    "",
    "",
    "", // TODO
    "",
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "ID3 names",
    "ID3 Namen",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom ID3",
    "", // TODO
    "",
    "", // TODO
    "Nombres ID3",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "ID3 information",
    "ID3 Information",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Information ID3",
    "", // TODO
    "",
    "", // TODO
    "Informaci�n ID3",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "ID3 info",
    "ID3 Info",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Info ID3",
    "", // TODO
    "",
    "", // TODO
    "Info ID3",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Filename",
    "Dateiname",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du fichier",
    "", // TODO
    "",
    "", // TODO
    "Nombre del archivo",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Length",
    "L�nge",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Longueur",
    "", // TODO
    "",
    "", // TODO
    "Duraci�n",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Title",
    "Titel",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Titre",
    "", // TODO
    "",
    "", // TODO
    "T�tulo",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Artist",
    "Interpret",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Artiste",
    "", // TODO
    "",
    "", // TODO
    "Artista",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Album",
    "Album",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Album",
    "", // TODO
    "",
    "", // TODO
    "�lbum",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Year",
    "Jahr",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ann�e",
    "", // TODO
    "",
    "", // TODO
    "A�o",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Samplerate",
    "Sample Rate",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Fr�quence",
    "", // TODO
    "",
    "", // TODO
    "Frecuencia",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Bitrate",
    "Bit Rate",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Bitrate",
    "", // TODO
    "",
    "", // TODO
    "Bitrate",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Initial loop mode",
    "Default Loop Modus",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de r�p�tition initial",
    "", // TODO
    "",
    "", // TODO
    "Modo de repetici�n inicial",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Initial shuffle mode",
    "Default Shuffle Modus",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de lecture al�atoire initial",
    "", // TODO
    "",
    "", // TODO
    "Lectura aleatoria inicial",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Orden T�tulo/Artista",
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
    "",
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
    "",
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
    "",
    "", // TODO
    "Modo de salida audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "OSS",
    "OSS",
    "",
    "",
    "",
    "",
    "OSS",
    "",
    "",
    "",
    "OSS",
    "",
    "",
    "",
    "",
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Explorar en segundo plano",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Etiquetas ID3",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Editor display mode",
    "Editor Anzeige Modus",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage de l'�diteur",
    "", // TODO
    "",
    "", // TODO
    "Modo de visualizaci�n del editor",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Display mode",
    "Anzeige Modus",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage",
    "", // TODO
    "",
    "", // TODO
    "Modo de visualizaci�n",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "t�tulo, artista",
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
    "�lbum, a�o, t�tulo, artista",
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
    "info, �lbum, a�o, t�tulo, artista",
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
    "Lista de reproducci�n directa",
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
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Arri�re plan",
    "", // TODO
    "",
    "", // TODO
    "Modo en segundo plano",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$CDDB for CD-Audio",
    "CDDB f�r CD-Audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "CDDB pour CD-Audio",
    "", // TODO
    "",
    "", // TODO
    "CDDB para el Audio-CD",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "disabled",
    "aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "d�sactiver",
    "", // TODO
    "",
    "", // TODO
    "desactivado",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "solo local",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "local y remoto",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Utilizar proxy HTTP",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Nombre del puerto HTTP",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Puerto del Proxy HTTP",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Black",
    "Schwarz",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noir",
    "", // TODO
    "",
    "", // TODO
    "Negra",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Im�genes",
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
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Menu principal",
    "", // TODO
    "",
    "", // TODO
    "Men� Principal",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Playlists",
    "Abspiellisten",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Playlists",
    "", // TODO
    "",
    "", // TODO
    "Lista de reproducci�n",
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Browser",
    "Verz.anzeige",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "",
    "", // TODO
    "Navegador",
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Abort player at end of list",
    "Abspieler am Listenende beenden",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Stopper le lecteur � la fin de la liste",
    "", // TODO
    "",
    "", // TODO
    "Parar reproductor al final de la lista",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "Pantalla de reproducci�n",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
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
    "",
    "", // TODO
    "cl�sica",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "via skin",
    "�ber Skin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Par skin",
    "", // TODO
    "",
    "", // TODO
    "v�a skin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if APIVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Keep selection menu",
    "Auswahlmenu ge�ffnet lassen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "",
    "", // TODO
    "Mantener men� de selecci�n",
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

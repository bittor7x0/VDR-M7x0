/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2008, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: i18n.cpp 164 2008-11-27 00:00:38Z tom $
 *
 *
 * Translations provided by:
 *
 * Italian     Diego Pierotto <vdr-italian@tiscali.it>
 * French      Patrice Staudt <patr.staudt@laposte.net>
 *             NIVAL Michaël <mnival@vdrbox.lautre.net>
 * Finnish     Ville Skyttä <ville.skytta@iki.fi>
 * Spanish     Bittor Corl <bittor7x0 _at_ gmail.com>
 *
 */

#include "i18n.h"
#include <vdr/config.h>


#if VDRVERSNUM < 10507

const tI18nPhrase SudokuPlugin::Phrases[] = {
  { "Sudoku", // English
    "Sudoku", // Deutsch / German
    "", // Slovenski / Slovenian
    "Sudoku", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Sudoku", // Français / French
    "", // Norsk / Norwegian
    "Sudoku", // suomi / Finnish
    "", // Polski / Polish
    "Sudoku", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Sudoku - generate and solve Number Place puzzles", // English
    "Sudoku - Erzeugen und Lösen von Zahlenpuzzles", // Deutsch / German
    "", // Slovenski / Slovenian
    "Sudoku - Genera e risolvi il rompicapo dei numeri", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Sudoku - créer et résoudre des puzzles de chiffres", // Français / French
    "", // Norsk / Norwegian
    "Sudoku-numeroristikot", // suomi / Finnish
    "", // Polski / Polish
    "Solucionar rompecabezas numérico", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Generate a new puzzle", // English
    "Neues Puzzle erzeugen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Genera nuova partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Créer un nouveau puzzle", // Français / French
    "", // Norsk / Norwegian
    "Uusi ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Generar nuevo rompecabezas", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Load a puzzle", // English
    "Puzzle laden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Carica partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Charger un puzzle", // Français / French
    "", // Norsk / Norwegian
    "Lataa ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Cargar un rompecabezas", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Save the puzzle", // English
    "Puzzle speichern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Salva partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Sauvegarder le puzzle", // Français / French
    "", // Norsk / Norwegian
    "Tallenna ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Guardar rompecabezas", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Undo last action", // English
    "Letzte Aktion zurücknehmen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Annulla ultima azione", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Annuler la dernière action", // Français / French
    "", // Norsk / Norwegian
    "Peru viimeisin", // suomi / Finnish
    "", // Polski / Polish
    "Deshacer última acción", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Redo last action", // English
    "Letzte Aktion wiederholen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ripeti ultima azione", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Répéter la dernière action", // Français / French
    "", // Norsk / Norwegian
    "Toista viimeisin", // suomi / Finnish
    "", // Polski / Polish
    "Rehacer última acción", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Mark/unmark", // English
    "Markierung setzen/löschen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Marca/Smarca", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Marquer/Démarquer", // Français / French
    "", // Norsk / Norwegian
    "Merkitse/poista merkintä", // suomi / Finnish
    "", // Polski / Polish
    "Marcar/desmarcar", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Next cell", // English
    "Nächste Zelle", // Deutsch / German
    "", // Slovenski / Slovenian
    "Cella successiva", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Cellule suivante", // Français / French
    "", // Norsk / Norwegian
    "Seuraava ruutu", // suomi / Finnish
    "", // Polski / Polish
    "Celda siguiente", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Next number", // English
    "Nächste Zahl", // Deutsch / German
    "", // Slovenski / Slovenian
    "Num. successivo", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Nombre suivant", // Français / French
    "", // Norsk / Norwegian
    "Seuraava numero", // suomi / Finnish
    "", // Polski / Polish
    "Número siguiente", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Reset the puzzle", // English
    "Puzzle zurücksetzen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ricomincia partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Remise à zéro du puzzle", // Français / French
    "", // Norsk / Norwegian
    "Tyhjennä ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Reiniciar el rompecabezas", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Open setup menu", // English
    "Einstellungsmenü öffnen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Apri menu opzioni", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Ouvrir le menu de configuration", // Français / French
    "", // Norsk / Norwegian
    "Asetukset", // suomi / Finnish
    "", // Polski / Polish
    "Abrir menú de opciones", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Exit", // English
    "Beenden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Esci", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Terminer", // Français / French
    "", // Norsk / Norwegian
    "Poistu", // suomi / Finnish
    "", // Polski / Polish
    "Salir", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Sudoku list", // English
    "Sudoku-Liste", // Deutsch / German
    "", // Slovenski / Slovenian
    "Elenco Sudoku", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Liste Sudoku", // Français / French
    "", // Norsk / Norwegian
    "Tallennetut ristikot", // suomi / Finnish
    "", // Polski / Polish
    "Lista Sudoku", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Delete the puzzle?", // English
    "Puzzle löschen?", // Deutsch / German
    "", // Slovenski / Slovenian
    "Eliminare la partita?", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Supprimer le puzzle ?", // Français / French
    "", // Norsk / Norwegian
    "Poistetaanko ristikko?", // suomi / Finnish
    "", // Polski / Polish
    "¿Borrar el rompecabezas?", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Button$Load", // English
    "Laden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Carica", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Charger", // Français / French
    "", // Norsk / Norwegian
    "Lataa", // suomi / Finnish
    "", // Polski / Polish
    "Cargar", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Button$Back", // English
    "Zurück", // Deutsch / German
    "", // Slovenski / Slovenian
    "Indietro", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Retour", // Français / French
    "", // Norsk / Norwegian
    "Takaisin", // suomi / Finnish
    "", // Polski / Polish
    "Volver", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Edit sudoku list", // English
    "Sudoku-Liste bearbeiten", // Deutsch / German
    "", // Slovenski / Slovenian
    "Modifica elenco Sudoku", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Editer la liste Sudoku", // Français / French
    "", // Norsk / Norwegian
    "Ristikon tiedot", // suomi / Finnish
    "", // Polski / Polish
    "Editar lista Sudoku", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Description", // English
    "Beschreibung", // Deutsch / German
    "", // Slovenski / Slovenian
    "Descrizione", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Description", // Français / French
    "", // Norsk / Norwegian
    "Kuvaus", // suomi / Finnish
    "", // Polski / Polish
    "Descripción", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Givens count", // English
    "Anzahl vorgegebener Zahlen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Numero cifre assegnate", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Nombre de chiffre donné", // Français / French
    "", // Norsk / Norwegian
    "Annettujen numeroiden lukumäärä", // suomi / Finnish
    "", // Polski / Polish
    "Celdas rellenadas", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Symmetric givens", // English
    "Vorgegebene Zahlen symmetrisch anordnen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Cifre simmetriche assegnate", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Chiffre donné symétrique", // Français / French
    "", // Norsk / Norwegian
    "Järjestä annetut symmetrisesti", // suomi / Finnish
    "", // Polski / Polish
    "Números simétricos rellenados", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Mark errors", // English
    "Fehler markieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Segna errori", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Marquer les erreurs", // Français / French
    "", // Norsk / Norwegian
    "Merkitse virheet", // suomi / Finnish
    "", // Polski / Polish
    "Marcar errores", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Mark ambiguous numbers", // English
    "Unklare Zahlen markieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Segna numeri ambigui", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Marquer les chiffres anbigus", // Français / French
    "", // Norsk / Norwegian
    "Merkitse epävarmat", // suomi / Finnish
    "", // Polski / Polish
    "Marcar números ambiguos", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Show possible numbers as pattern", // English
    "Mögliche Zahlen als Muster anzeigen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Mostra num. possibili in valori", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Afficher les chiffres possible comme modèle", // Français / French
    "", // Norsk / Norwegian
    "Näytä mahdolliset kuviona", // suomi / Finnish
    "", // Polski / Polish
    "Mostrar posibles núm. como patrón", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Show possible numbers as digits", // English
    "Mögliche Zahlen als Ziffern anzeigen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Mostra num. possibili in cifre", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Afficher les chiffres possible comme chiffre", // Français / French
    "", // Norsk / Norwegian
    "Näytä mahdolliset numeroina", // suomi / Finnish
    "", // Polski / Polish
    "Mostrar posibles núm. como dígitos", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Clear marks on reset", // English
    "Markierungen löschen beim Reset", // Deutsch / German
    "", // Slovenski / Slovenian
    "Pulisci segni al riavvio", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Effacer les marques lors de la mise à zéro", // Français / French
    "", // Norsk / Norwegian
    "Poista merkinnät aloitettaessa", // suomi / Finnish
    "", // Polski / Polish
    "Quitar marcas al reiniciar", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Key Red", // English
    "Taste Rot", // Deutsch / German
    "", // Slovenski / Slovenian
    "Tasto Rosso", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Bouton rouge", // Français / French
    "", // Norsk / Norwegian
    "Punainen näppäin", // suomi / Finnish
    "", // Polski / Polish
    "Botón Rojo", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Key Green", // English
    "Taste Grün", // Deutsch / German
    "", // Slovenski / Slovenian
    "Tasto Verde", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Bouton vert", // Français / French
    "", // Norsk / Norwegian
    "Vihreä näppäin", // suomi / Finnish
    "", // Polski / Polish
    "Botón Verde", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Key Yellow", // English
    "Taste Gelb", // Deutsch / German
    "", // Slovenski / Slovenian
    "Tasto Giallo", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Bouton jaune", // Français / French
    "", // Norsk / Norwegian
    "Keltainen näppäin", // suomi / Finnish
    "", // Polski / Polish
    "Botón Amarillo", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Large font", // English
    "Große Schriftart", // Deutsch / German
    "", // Slovenski / Slovenian
    "Caratteri grandi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Grande police", // Français / French
    "", // Norsk / Norwegian
    "Suuri kirjasin", // suomi / Finnish
    "", // Polski / Polish
    "Fuente grande", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Large font height (pixel)", // English
    "Große Schrifthöhe (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Altezza car. grandi (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Hauteur grande police (pixel)", // Français / French
    "", // Norsk / Norwegian
    "Suuren kirjasimen korkeus (pikseleitä)", // suomi / Finnish
    "", // Polski / Polish
    "Alto fuente grande (pixel)", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Large font width (pixel)", // English
    "Große Schriftbreite (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Larghezza car. grandi (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Largeur grande police (pixel)", // Français / French
    "", // Norsk / Norwegian
    "Suuren kirjasimen leveys (pikseleitä)", // suomi / Finnish
    "", // Polski / Polish
    "Ancho fuente grande (pixel)", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Small font", // English
    "Kleine Schriftart", // Deutsch / German
    "", // Slovenski / Slovenian
    "Caratteri piccoli", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Petite police", // Français / French
    "", // Norsk / Norwegian
    "Pieni kirjasin", // suomi / Finnish
    "", // Polski / Polish
    "Fuente pequeña", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Small font height (pixel)", // English
    "Kleine Schrifthöhe (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Altezza car. piccoli (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Hauteur petite police (pixel)", // Français / French
    "", // Norsk / Norwegian
    "Pienen kirjasimen korkeus (pikseleitä)", // suomi / Finnish
    "", // Polski / Polish
    "Alto fuente pequeña (pixel)", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Small font width (pixel)", // English
    "Kleine Schriftbreite (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Larghezza car. piccoli (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Largeur petite police (pixel)", // Français / French
    "", // Norsk / Norwegian
    "Pienen kirjasimen leveys (pikseleitä)", // suomi / Finnish
    "", // Polski / Polish
    "Ancho fuente pequeña (pixel)", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Transparency (%)", // English
    "Transparenz (%)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Trasparenza (%)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Transparence (%)", // Français / French
    "", // Norsk / Norwegian
    "Läpinäkyvyys (%)", // suomi / Finnish
    "", // Polski / Polish
    "Transparencia (%)", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { "Congratulations!\nPress OK to start a new puzzle", // English
    "Herzlichen Glückwunsch!\nOK startet ein neues Puzzle", // Deutsch / German
    "", // Slovenski / Slovenian
    "Complimenti!\nPremi OK per iniziare una nuova partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "Félicitation!\nAvec OK vous lancez un nouveau puzzle", // Français / French
    "", // Norsk / Norwegian
    "Onnittelut!\nValitse 'OK' aloittaaksesi uuden ristikon", // suomi / Finnish
    "", // Polski / Polish
    "¡Enhorabuena!\nPulse OK para iniciar un nuevo rompecabezas", // Español / Spanish
    "", // ÅëëçíéêÜ / Greek
    "", // Svenska / Swedish
    "", // Românã / Romanian
    "", // Magyar / Hungarian
    "", // Català / Catalanian
    "", // ÀãááÚØÙ / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // Èesky / Czech
  },
  { NULL }
};

#endif

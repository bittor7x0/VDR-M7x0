/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2008, Thomas G�nther <tom@toms-cafe.de>
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
 *             NIVAL Micha�l <mnival@vdrbox.lautre.net>
 * Finnish     Ville Skytt� <ville.skytta@iki.fi>
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
    "", // Portugu�s / Portuguese
    "Sudoku", // Fran�ais / French
    "", // Norsk / Norwegian
    "Sudoku", // suomi / Finnish
    "", // Polski / Polish
    "Sudoku", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Sudoku - generate and solve Number Place puzzles", // English
    "Sudoku - Erzeugen und L�sen von Zahlenpuzzles", // Deutsch / German
    "", // Slovenski / Slovenian
    "Sudoku - Genera e risolvi il rompicapo dei numeri", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Sudoku - cr�er et r�soudre des puzzles de chiffres", // Fran�ais / French
    "", // Norsk / Norwegian
    "Sudoku-numeroristikot", // suomi / Finnish
    "", // Polski / Polish
    "Solucionar rompecabezas num�rico", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Generate a new puzzle", // English
    "Neues Puzzle erzeugen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Genera nuova partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Cr�er un nouveau puzzle", // Fran�ais / French
    "", // Norsk / Norwegian
    "Uusi ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Generar nuevo rompecabezas", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Load a puzzle", // English
    "Puzzle laden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Carica partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Charger un puzzle", // Fran�ais / French
    "", // Norsk / Norwegian
    "Lataa ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Cargar un rompecabezas", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Save the puzzle", // English
    "Puzzle speichern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Salva partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Sauvegarder le puzzle", // Fran�ais / French
    "", // Norsk / Norwegian
    "Tallenna ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Guardar rompecabezas", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Undo last action", // English
    "Letzte Aktion zur�cknehmen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Annulla ultima azione", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Annuler la derni�re action", // Fran�ais / French
    "", // Norsk / Norwegian
    "Peru viimeisin", // suomi / Finnish
    "", // Polski / Polish
    "Deshacer �ltima acci�n", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Redo last action", // English
    "Letzte Aktion wiederholen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ripeti ultima azione", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "R�p�ter la derni�re action", // Fran�ais / French
    "", // Norsk / Norwegian
    "Toista viimeisin", // suomi / Finnish
    "", // Polski / Polish
    "Rehacer �ltima acci�n", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Mark/unmark", // English
    "Markierung setzen/l�schen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Marca/Smarca", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Marquer/D�marquer", // Fran�ais / French
    "", // Norsk / Norwegian
    "Merkitse/poista merkint�", // suomi / Finnish
    "", // Polski / Polish
    "Marcar/desmarcar", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Next cell", // English
    "N�chste Zelle", // Deutsch / German
    "", // Slovenski / Slovenian
    "Cella successiva", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Cellule suivante", // Fran�ais / French
    "", // Norsk / Norwegian
    "Seuraava ruutu", // suomi / Finnish
    "", // Polski / Polish
    "Celda siguiente", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Next number", // English
    "N�chste Zahl", // Deutsch / German
    "", // Slovenski / Slovenian
    "Num. successivo", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Nombre suivant", // Fran�ais / French
    "", // Norsk / Norwegian
    "Seuraava numero", // suomi / Finnish
    "", // Polski / Polish
    "N�mero siguiente", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Reset the puzzle", // English
    "Puzzle zur�cksetzen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ricomincia partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Remise � z�ro du puzzle", // Fran�ais / French
    "", // Norsk / Norwegian
    "Tyhjenn� ristikko", // suomi / Finnish
    "", // Polski / Polish
    "Reiniciar el rompecabezas", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Open setup menu", // English
    "Einstellungsmen� �ffnen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Apri menu opzioni", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Ouvrir le menu de configuration", // Fran�ais / French
    "", // Norsk / Norwegian
    "Asetukset", // suomi / Finnish
    "", // Polski / Polish
    "Abrir men� de opciones", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Exit", // English
    "Beenden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Esci", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Terminer", // Fran�ais / French
    "", // Norsk / Norwegian
    "Poistu", // suomi / Finnish
    "", // Polski / Polish
    "Salir", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Sudoku list", // English
    "Sudoku-Liste", // Deutsch / German
    "", // Slovenski / Slovenian
    "Elenco Sudoku", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Liste Sudoku", // Fran�ais / French
    "", // Norsk / Norwegian
    "Tallennetut ristikot", // suomi / Finnish
    "", // Polski / Polish
    "Lista Sudoku", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Delete the puzzle?", // English
    "Puzzle l�schen?", // Deutsch / German
    "", // Slovenski / Slovenian
    "Eliminare la partita?", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Supprimer le puzzle ?", // Fran�ais / French
    "", // Norsk / Norwegian
    "Poistetaanko ristikko?", // suomi / Finnish
    "", // Polski / Polish
    "�Borrar el rompecabezas?", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Button$Load", // English
    "Laden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Carica", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Charger", // Fran�ais / French
    "", // Norsk / Norwegian
    "Lataa", // suomi / Finnish
    "", // Polski / Polish
    "Cargar", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Button$Back", // English
    "Zur�ck", // Deutsch / German
    "", // Slovenski / Slovenian
    "Indietro", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Retour", // Fran�ais / French
    "", // Norsk / Norwegian
    "Takaisin", // suomi / Finnish
    "", // Polski / Polish
    "Volver", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Edit sudoku list", // English
    "Sudoku-Liste bearbeiten", // Deutsch / German
    "", // Slovenski / Slovenian
    "Modifica elenco Sudoku", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Editer la liste Sudoku", // Fran�ais / French
    "", // Norsk / Norwegian
    "Ristikon tiedot", // suomi / Finnish
    "", // Polski / Polish
    "Editar lista Sudoku", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Description", // English
    "Beschreibung", // Deutsch / German
    "", // Slovenski / Slovenian
    "Descrizione", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Description", // Fran�ais / French
    "", // Norsk / Norwegian
    "Kuvaus", // suomi / Finnish
    "", // Polski / Polish
    "Descripci�n", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Givens count", // English
    "Anzahl vorgegebener Zahlen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Numero cifre assegnate", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Nombre de chiffre donn�", // Fran�ais / French
    "", // Norsk / Norwegian
    "Annettujen numeroiden lukum��r�", // suomi / Finnish
    "", // Polski / Polish
    "Celdas rellenadas", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Symmetric givens", // English
    "Vorgegebene Zahlen symmetrisch anordnen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Cifre simmetriche assegnate", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Chiffre donn� sym�trique", // Fran�ais / French
    "", // Norsk / Norwegian
    "J�rjest� annetut symmetrisesti", // suomi / Finnish
    "", // Polski / Polish
    "N�meros sim�tricos rellenados", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Mark errors", // English
    "Fehler markieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Segna errori", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Marquer les erreurs", // Fran�ais / French
    "", // Norsk / Norwegian
    "Merkitse virheet", // suomi / Finnish
    "", // Polski / Polish
    "Marcar errores", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Mark ambiguous numbers", // English
    "Unklare Zahlen markieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Segna numeri ambigui", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Marquer les chiffres anbigus", // Fran�ais / French
    "", // Norsk / Norwegian
    "Merkitse ep�varmat", // suomi / Finnish
    "", // Polski / Polish
    "Marcar n�meros ambiguos", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Show possible numbers as pattern", // English
    "M�gliche Zahlen als Muster anzeigen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Mostra num. possibili in valori", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Afficher les chiffres possible comme mod�le", // Fran�ais / French
    "", // Norsk / Norwegian
    "N�yt� mahdolliset kuviona", // suomi / Finnish
    "", // Polski / Polish
    "Mostrar posibles n�m. como patr�n", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Show possible numbers as digits", // English
    "M�gliche Zahlen als Ziffern anzeigen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Mostra num. possibili in cifre", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Afficher les chiffres possible comme chiffre", // Fran�ais / French
    "", // Norsk / Norwegian
    "N�yt� mahdolliset numeroina", // suomi / Finnish
    "", // Polski / Polish
    "Mostrar posibles n�m. como d�gitos", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Clear marks on reset", // English
    "Markierungen l�schen beim Reset", // Deutsch / German
    "", // Slovenski / Slovenian
    "Pulisci segni al riavvio", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Effacer les marques lors de la mise � z�ro", // Fran�ais / French
    "", // Norsk / Norwegian
    "Poista merkinn�t aloitettaessa", // suomi / Finnish
    "", // Polski / Polish
    "Quitar marcas al reiniciar", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Key Red", // English
    "Taste Rot", // Deutsch / German
    "", // Slovenski / Slovenian
    "Tasto Rosso", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Bouton rouge", // Fran�ais / French
    "", // Norsk / Norwegian
    "Punainen n�pp�in", // suomi / Finnish
    "", // Polski / Polish
    "Bot�n Rojo", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Key Green", // English
    "Taste Gr�n", // Deutsch / German
    "", // Slovenski / Slovenian
    "Tasto Verde", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Bouton vert", // Fran�ais / French
    "", // Norsk / Norwegian
    "Vihre� n�pp�in", // suomi / Finnish
    "", // Polski / Polish
    "Bot�n Verde", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Key Yellow", // English
    "Taste Gelb", // Deutsch / German
    "", // Slovenski / Slovenian
    "Tasto Giallo", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Bouton jaune", // Fran�ais / French
    "", // Norsk / Norwegian
    "Keltainen n�pp�in", // suomi / Finnish
    "", // Polski / Polish
    "Bot�n Amarillo", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Large font", // English
    "Gro�e Schriftart", // Deutsch / German
    "", // Slovenski / Slovenian
    "Caratteri grandi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Grande police", // Fran�ais / French
    "", // Norsk / Norwegian
    "Suuri kirjasin", // suomi / Finnish
    "", // Polski / Polish
    "Fuente grande", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Large font height (pixel)", // English
    "Gro�e Schrifth�he (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Altezza car. grandi (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Hauteur grande police (pixel)", // Fran�ais / French
    "", // Norsk / Norwegian
    "Suuren kirjasimen korkeus (pikseleit�)", // suomi / Finnish
    "", // Polski / Polish
    "Alto fuente grande (pixel)", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Large font width (pixel)", // English
    "Gro�e Schriftbreite (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Larghezza car. grandi (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Largeur grande police (pixel)", // Fran�ais / French
    "", // Norsk / Norwegian
    "Suuren kirjasimen leveys (pikseleit�)", // suomi / Finnish
    "", // Polski / Polish
    "Ancho fuente grande (pixel)", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Small font", // English
    "Kleine Schriftart", // Deutsch / German
    "", // Slovenski / Slovenian
    "Caratteri piccoli", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Petite police", // Fran�ais / French
    "", // Norsk / Norwegian
    "Pieni kirjasin", // suomi / Finnish
    "", // Polski / Polish
    "Fuente peque�a", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Small font height (pixel)", // English
    "Kleine Schrifth�he (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Altezza car. piccoli (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Hauteur petite police (pixel)", // Fran�ais / French
    "", // Norsk / Norwegian
    "Pienen kirjasimen korkeus (pikseleit�)", // suomi / Finnish
    "", // Polski / Polish
    "Alto fuente peque�a (pixel)", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Small font width (pixel)", // English
    "Kleine Schriftbreite (pixel)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Larghezza car. piccoli (pixel)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Largeur petite police (pixel)", // Fran�ais / French
    "", // Norsk / Norwegian
    "Pienen kirjasimen leveys (pikseleit�)", // suomi / Finnish
    "", // Polski / Polish
    "Ancho fuente peque�a (pixel)", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Transparency (%)", // English
    "Transparenz (%)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Trasparenza (%)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "Transparence (%)", // Fran�ais / French
    "", // Norsk / Norwegian
    "L�pin�kyvyys (%)", // suomi / Finnish
    "", // Polski / Polish
    "Transparencia (%)", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { "Congratulations!\nPress OK to start a new puzzle", // English
    "Herzlichen Gl�ckwunsch!\nOK startet ein neues Puzzle", // Deutsch / German
    "", // Slovenski / Slovenian
    "Complimenti!\nPremi OK per iniziare una nuova partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "F�licitation!\nAvec OK vous lancez un nouveau puzzle", // Fran�ais / French
    "", // Norsk / Norwegian
    "Onnittelut!\nValitse 'OK' aloittaaksesi uuden ristikon", // suomi / Finnish
    "", // Polski / Polish
    "�Enhorabuena!\nPulse OK para iniciar un nuevo rompecabezas", // Espa�ol / Spanish
    "", // �������� / Greek
    "", // Svenska / Swedish
    "", // Rom�n� / Romanian
    "", // Magyar / Hungarian
    "", // Catal� / Catalanian
    "", // ������� / Russian
    "", // Hrvatski / Croatian
    "", // Eesti / Estonian
    "", // Dansk / Danish
    "", // �esky / Czech
  },
  { NULL }
};

#endif

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
 * $Id: i18n.cpp 131 2008-03-30 13:49:29Z tom $
 *
 *
 * Translations provided by:
 *
 * Finnish     Ville Skytt� <ville.skytta@iki.fi>
 * Spanish     bittor from open7x0.org
 * Italian     Diego Pierotto <vdr-italian@tiscali.it>
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
    "", // Fran�ais / French
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
    "", // Fran�ais / French
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
    "Genera una nuova partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Uusi ristikko", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "Carica una partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Lataa ristikko", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "Salva la partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Tallenna ristikko", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "Ricomincia la partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Tyhjenn� ristikko", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "Apri il menu opzioni", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Asetukset", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Poistu", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Tallennetut ristikot", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Lataa", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Takaisin", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Ristikon tiedot", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "Kuvaus", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
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
    "", // Fran�ais / French
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
    "", // Fran�ais / French
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
    "", // Fran�ais / French
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "N�yt� mahdolliset kuviona", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "N�yt� mahdolliset numeroina", // suomi / Finnish
    "", // Polski / Polish
    "", // Espa�ol / Spanish
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
    "", // Fran�ais / French
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
  { "Transparency (%)", // English
    "Transparenz (%)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Trasparenza (%)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
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
    "", // Fran�ais / French
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

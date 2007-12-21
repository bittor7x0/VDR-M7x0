/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.cpp 68 2007-02-23 20:43:08Z tom $
 *
 *
 * Translations provided by:
 *
 * Finnish     Ville Skyttä <ville.skytta@iki.fi>
 * Spanish     bittor from open7x0.org
 *
 */

#include "i18n.h"
#include <vdr/config.h>


const tI18nPhrase SudokuPlugin::Phrases[] = {
  { "Sudoku", // English
    "Sudoku", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
  { "Givens count", // English
    "Anzahl vorgegebener Zahlen", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
  { "Clear marks on reset", // English
    "Markierungen löschen beim Reset", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
  { "Transparency (%)", // English
    "Transparenz (%)", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
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
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "Onnittelut!\nValitse 'OK' aloittaaksesi uuden tehtävän", // suomi / Finnish
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

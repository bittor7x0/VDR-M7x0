/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas G�nther <tom@toms-cafe.de>
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
 * $Id: i18n.c 87 2007-06-22 22:37:36Z tom $
 *
 *
 * Translations provided by:
 *
 * Spanish     bittor from open7x0.org
 *
 */

#include "i18n.h"
#include <vdr/config.h>


const tI18nPhrase Phrases[] = {
  { "Spider Arachnid", // English
    "Spider Arachnid", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Spider Arachnid", // Espa�ol / Spanish
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
  { "Spider Arachnid - the best patience game", // English
    "Spider Arachnid - die beste Patience", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Spider Arachnid - mejor juego paciencia", // Espa�ol / Spanish
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
  { "Variation", // English
    "Variante", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Variaci�n", // Espa�ol / Spanish
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
  { "Mini (one deck)", // English
    "Mini (ein Blatt)", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Mini (una baraja)", // Espa�ol / Spanish
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
  { "Normal", // English
    "Normal", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Normal", // Espa�ol / Spanish
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
  { "Only complete suits are allowed to remove", // English
    "Nur vollst�ndige S�tze k�nnen entfernt werden", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "S�lo puede eliminar los palos completos", // Espa�ol / Spanish
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
  { "No cards left", // English
    "Keine Karten �brig", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "No dej� cartas", // Espa�ol / Spanish
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
  { "Deal not allowed with empty piles", // English
    "Neue Karten nur, wenn alle Reihen belegt", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Reparto no permitido con montones vac�os", // Espa�ol / Spanish
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
  { "Congratulations!\nPress OK to start a new game", // English
    "Herzlichen Gl�ckwunsch!\nOK startet ein neues Spiel", // Deutsch / German
    "", // Slovenski / Slovenian
    "", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "�Felicidades!\nPulse OK para iniciar un nuevo juego", // Espa�ol / Spanish
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

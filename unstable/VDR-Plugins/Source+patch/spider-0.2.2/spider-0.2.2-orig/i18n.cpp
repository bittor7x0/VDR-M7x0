/*
 * Spider-Arachnid: A plug-in for the Video Disk Recorder
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
 * $Id: i18n.cpp 167 2008-11-30 00:03:32Z tom $
 *
 *
 * Translations provided by:
 *
 * Italian     Diego Pierotto <vdr-italian@tiscali.it>
 * Spanish     Bittor Corl <bittor7x0 _at_ gmail.com>
 *
 */

#include "i18n.h"
#include <vdr/config.h>


#if VDRVERSNUM < 10507

const tI18nPhrase SpiderPlugin::Phrases[] = {
  { "Spider Arachnid", // English
    "Spider Arachnid", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ragno Aracnide", // Italiano / Italian
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
    "Ragno Aracnide - il migliore gioco di pazienza", // Italiano / Italian
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
    "Variante", // Italiano / Italian
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
    "Mini (un mazzo)", // Italiano / Italian
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
    "Normale", // Italiano / Italian
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
  { "Custom", // English
    "Benutzerdefiniert", // Deutsch / German
    "", // Slovenski / Slovenian
    "Personale", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Seleccionar", // Espa�ol / Spanish
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
  { " Deck count", // English
    " Blatt-Anzahl", // Deutsch / German
    "", // Slovenski / Slovenian
    " Numero mazzi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    " Contar baraja", // Espa�ol / Spanish
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
  { " Pile count", // English
    " Anzahl Reihen", // Deutsch / German
    "", // Slovenski / Slovenian
    " Numero mucchi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    " Contar mont�n", // Espa�ol / Spanish
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
  { " Deal count", // English
    " Neue Karten geben", // Deutsch / German
    "", // Slovenski / Slovenian
    " Numero mani", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    " Contar reparto", // Espa�ol / Spanish
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
  { "OSD position left", // English
    "OSD-Position links", // Deutsch / German
    "", // Slovenski / Slovenian
    "Posizione sinistra OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Posici�n OSD izquierda", // Espa�ol / Spanish
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
  { "OSD position top", // English
    "OSD-Position oben", // Deutsch / German
    "", // Slovenski / Slovenian
    "Posizione superiore OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Posici�n OSD superior", // Espa�ol / Spanish
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
  { "OSD width", // English
    "OSD-Breite", // Deutsch / German
    "", // Slovenski / Slovenian
    "Larghezza OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Anchura OSD", // Espa�ol / Spanish
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
  { "OSD height", // English
    "OSD-H�he", // Deutsch / German
    "", // Slovenski / Slovenian
    "Altezza OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Altura OSD", // Espa�ol / Spanish
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
  { "OSD error compensation", // English
    "OSD-Fehler kompensieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Compensazione errore OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Error compensaci�n OSD", // Espa�ol / Spanish
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
  { "Shrink height", // English
    "H�he verringern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ridim. altezza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Encoger altura", // Espa�ol / Spanish
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
  { "Shrink width", // English
    "Breite verringern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ridim. larghezza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Encoger anchura", // Espa�ol / Spanish
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
  { "Shrink width and height", // English
    "Breite und H�he verringern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ridim. larghezza e altezza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Encoger anchura y altura", // Espa�ol / Spanish
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
  { "Reduce colors", // English
    "Farben reduzieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Riduci colori", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Reducir colores", // Espa�ol / Spanish
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
  { "Hide top row", // English
    "Oberste Zeile verbergen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Nascondi riga superiore", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Ocultar fila superior", // Espa�ol / Spanish
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
  { "Reset", // English
    "Zur�cksetzen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Reimposta", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Portugu�s / Portuguese
    "", // Fran�ais / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Reiniciar", // Espa�ol / Spanish
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
    "Solo i colori completi possono essere rimossi", // Italiano / Italian
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
    "Nessuna carta rimasta", // Italiano / Italian
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
    "Operazione non permessa con i mucchi vuoti", // Italiano / Italian
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
    "Complimenti!\nPremi OK per iniziare una nuova partita", // Italiano / Italian
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

#endif

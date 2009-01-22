/*
 * Spider-Arachnid: A plug-in for the Video Disk Recorder
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
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Spider Arachnid", // Español / Spanish
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
  { "Spider Arachnid - the best patience game", // English
    "Spider Arachnid - die beste Patience", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ragno Aracnide - il migliore gioco di pazienza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Spider Arachnid - mejor juego paciencia", // Español / Spanish
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
  { "Variation", // English
    "Variante", // Deutsch / German
    "", // Slovenski / Slovenian
    "Variante", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Variación", // Español / Spanish
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
  { "Mini (one deck)", // English
    "Mini (ein Blatt)", // Deutsch / German
    "", // Slovenski / Slovenian
    "Mini (un mazzo)", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Mini (una baraja)", // Español / Spanish
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
  { "Normal", // English
    "Normal", // Deutsch / German
    "", // Slovenski / Slovenian
    "Normale", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Normal", // Español / Spanish
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
  { "Custom", // English
    "Benutzerdefiniert", // Deutsch / German
    "", // Slovenski / Slovenian
    "Personale", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Seleccionar", // Español / Spanish
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
  { " Deck count", // English
    " Blatt-Anzahl", // Deutsch / German
    "", // Slovenski / Slovenian
    " Numero mazzi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    " Contar baraja", // Español / Spanish
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
  { " Pile count", // English
    " Anzahl Reihen", // Deutsch / German
    "", // Slovenski / Slovenian
    " Numero mucchi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    " Contar montón", // Español / Spanish
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
  { " Deal count", // English
    " Neue Karten geben", // Deutsch / German
    "", // Slovenski / Slovenian
    " Numero mani", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    " Contar reparto", // Español / Spanish
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
  { "OSD position left", // English
    "OSD-Position links", // Deutsch / German
    "", // Slovenski / Slovenian
    "Posizione sinistra OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Posición OSD izquierda", // Español / Spanish
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
  { "OSD position top", // English
    "OSD-Position oben", // Deutsch / German
    "", // Slovenski / Slovenian
    "Posizione superiore OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Posición OSD superior", // Español / Spanish
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
  { "OSD width", // English
    "OSD-Breite", // Deutsch / German
    "", // Slovenski / Slovenian
    "Larghezza OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Anchura OSD", // Español / Spanish
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
  { "OSD height", // English
    "OSD-Höhe", // Deutsch / German
    "", // Slovenski / Slovenian
    "Altezza OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Altura OSD", // Español / Spanish
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
  { "OSD error compensation", // English
    "OSD-Fehler kompensieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Compensazione errore OSD", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Error compensación OSD", // Español / Spanish
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
  { "Shrink height", // English
    "Höhe verringern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ridim. altezza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Encoger altura", // Español / Spanish
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
  { "Shrink width", // English
    "Breite verringern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ridim. larghezza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Encoger anchura", // Español / Spanish
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
  { "Shrink width and height", // English
    "Breite und Höhe verringern", // Deutsch / German
    "", // Slovenski / Slovenian
    "Ridim. larghezza e altezza", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Encoger anchura y altura", // Español / Spanish
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
  { "Reduce colors", // English
    "Farben reduzieren", // Deutsch / German
    "", // Slovenski / Slovenian
    "Riduci colori", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Reducir colores", // Español / Spanish
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
  { "Hide top row", // English
    "Oberste Zeile verbergen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Nascondi riga superiore", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Ocultar fila superior", // Español / Spanish
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
  { "Reset", // English
    "Zurücksetzen", // Deutsch / German
    "", // Slovenski / Slovenian
    "Reimposta", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Reiniciar", // Español / Spanish
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
  { "Only complete suits are allowed to remove", // English
    "Nur vollständige Sätze können entfernt werden", // Deutsch / German
    "", // Slovenski / Slovenian
    "Solo i colori completi possono essere rimossi", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Sólo puede eliminar los palos completos", // Español / Spanish
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
  { "No cards left", // English
    "Keine Karten übrig", // Deutsch / German
    "", // Slovenski / Slovenian
    "Nessuna carta rimasta", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "No dejó cartas", // Español / Spanish
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
  { "Deal not allowed with empty piles", // English
    "Neue Karten nur, wenn alle Reihen belegt", // Deutsch / German
    "", // Slovenski / Slovenian
    "Operazione non permessa con i mucchi vuoti", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "Reparto no permitido con montones vacíos", // Español / Spanish
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
  { "Congratulations!\nPress OK to start a new game", // English
    "Herzlichen Glückwunsch!\nOK startet ein neues Spiel", // Deutsch / German
    "", // Slovenski / Slovenian
    "Complimenti!\nPremi OK per iniziare una nuova partita", // Italiano / Italian
    "", // Nederlands / Dutch
    "", // Português / Portuguese
    "", // Français / French
    "", // Norsk / Norwegian
    "", // suomi / Finnish
    "", // Polski / Polish
    "¡Felicidades!\nPulse OK para iniciar un nuevo juego", // Español / Spanish
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

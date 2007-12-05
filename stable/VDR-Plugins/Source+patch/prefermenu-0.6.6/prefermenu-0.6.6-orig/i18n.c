/*
 * PreferMenu plugin to VDR (C++)
 *
 * (C) 2004,2005,2006 Olivier Jacques <oliviervdr@free.fr>
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

#include "i18n.h"

const tI18nPhrase Phrases[] = {
/*
 * French  translations provided by Maxime Guilbot <maxime.guilbot@orange.fr>
 */
  { "Channel", // ENGLISH
    "Kanal", // GERMAN
    "", // TODO
    "Canale", // ITALIAN
    "Kanaal", // NL
    "", // TODO
    "Chaînes", // FR
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Czatoma", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Origin X", // EN
    "Position X", // GERMAN
    "", // TODO
    "Origine X", // ITALIAN
    "X positie", // NL
    "", // TODO
    "Origine X", // FR
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "X helyzet", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Origin Y", // EN
    "Position Y", // GERMAN
    "", // TODO
    "Origine Y", // ITALIAN 
    "Y positie", // NL
    "", // TODO
    "Origine Y", // FR
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Y helyzet", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Width", //EN
    "Breite", // GER
    "", // TODO
    "Larghezza", // ITALIAN
    "Breedte", // NL
    "",
    "Largeur", 
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Szélesség", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "" // TODO
  },
  { "AddChannel",
    "Kanal hinzufügen", // GER
    "", // TODO
    "Aggiungi Canale", // ITALIAN
    "iKanaal bijdoen", // NL
    "", // TODO
    "Nouvelle chaîne",  // FR
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Adj csatornát hozza", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "" // TODO
  },
  { "RemoveChannel",
    "Kanal löschen", // GER
    "", // TODO
    "Rimuovi Canale", // ITALIAN
    "Kanaal wegneemen", // NL
    "", // TODO
    "Supprime chaîne", //FR
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Fok csatornát", // HU
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "" // TODO
  },
/*
  { "Background opacity",
    "Rand transparenz", // TODO
    "", // TODO
    "Alpha Bordo 1", // ITALIAN
    "", // TODO
    "", // TODO
    "Opacité du fond",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Selected channel opacity",
    "", // TODO
    "", // TODO
    "Alpha Bordo 2", // ITALIAN
    "", // TODO
    "", // TODO
    "Opacité de la chaîne selectionnée",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Border opacity",
    "Alpha Rand", // GER
    "", // TODO
    "AlphaBordo", // ITALIAN
    "Alpha kant", // NL
    "", // TODO
    "Opacité du contour", // FR
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Alpha szegély", // HU
  },
*/
  { "Sort by",
    "", // TODO
    "", // TODO
    "Ordina Per", // ITALIAN
    "", // TODO
    "", // TODO
    "Trier par",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "None",
    "", // TODO
    "", // TODO
    "Nessuno", // ITALIAN
    "", // TODO
    "", // TODO
    "Aucun",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Name",
    "", // TODO
    "", // TODO
    "Nome", // ITALIAN
    "", // TODO
    "", // TODO
    "Nom",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "As entered",
    "", // TODO
    "", // TODO
    "Non Ordinati", // ITALIAN
    "", // TODO
    "", // TODO
    "Tel que saisi",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Close on switch",
    "", // TODO
    "", // TODO
    "Chiudi al cambio canale", // ITALIAN
    "", // TODO
    "", // TODO
    "Fermer le menu en zappant",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },

 { NULL }
};

/*
 * i18n.c: Internationalization
 *
 * Clock Plugin for VDR. See http://vdr.aistleitner.info for download.
 * Copyright (C) 2007  Mario Aistleitner <vdr@aistleitner.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
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
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 *
 * Italian    translations provided by Gringo <vdr-italian@tiscali.it>
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Clock",
    "Uhr",
    "",// TODO
    "Orologio",
    "",// TODO
    "",// TODO
    "Heure",
    "",// TODO
    "",// TODO
    "",// TODO
    "Reloj",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "«–·Î",// TODO
  },
  { "Offset",
    "Abstand vom linken Rand",
    "",// TODO
    "Distanza dal bordo sinistro",
    "",// TODO
    "",// TODO
    "Distance du bord gauche",
    "",// TODO
    "",// TODO
    "",// TODO
    "Distancia desde la izquierda",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¿–·‚ﬁÔ›ÿ’ ﬁ‚ €’“ﬁ”ﬁ ⁄‡–Ô",// TODO
  },
  { "Show Seconds",
    "Sekunden anzeigen",
    "",// TODO
    "Mostra secondi",
    "",// TODO
    "",// TODO
    "Afficher secondes",
    "",// TODO
    "",// TODO
    "",// TODO
    "Mostrar segundos",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "øﬁ⁄–◊–‚Ï ·’⁄„›‘Î",// TODO
  },
  { "Type of the clock",
    "Art der Uhr",
    "",// TODO
    "Tipo di orologio",
    "",// TODO
    "",// TODO
    "Format d'affichage",
    "",// TODO
    "",// TODO
    "",// TODO
    "Tipo de reloj",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "≤ÿ‘ Á–·ﬁ“",// TODO
  },
  { "Length of sec arrow [%]",
    "L‰nge des Sekundenzeigers [%]",
    "",// TODO
    "Lunghezza freccia secondi [%]",
    "",// TODO
    "",// TODO
    "Longueur aiguille Sec [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Longitud del segundero [%]",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¥€ÿ›– ·’⁄„›‘›ﬁŸ ·‚‡’€⁄ÿ [%]",// TODO
  },
  { "Length of min arrow [%]",
    "L‰nge des Minutenzeigers [%]",
    "",// TODO
    "Lunghezza freccia minuti [%]",
    "",// TODO
    "",// TODO
    "Longueur aiguille Min [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Longitud del minutero [%]",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¥€ÿ›– ‹ÿ›„‚›ﬁŸ ·‚‡’€⁄ÿ [%]",// TODO
  },
  { "Length of hour arrow [%]",
    "L‰nge des Stundenzeigers [%]",
    "",// TODO
    "Lunghezza freccia ore [%]",
    "",// TODO
    "",// TODO
    "Longueur aiguille Hr [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Longitud aguja de las horas [%]",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¥€ÿ›–  Á–·ﬁ“ﬁŸ ·‚‡’€⁄ÿ [%]",// TODO
  },
  { "Width of sec arrow [%]",
    "Breite des Sekundenzeigers [%]",
    "",// TODO
    "Larghezza freccia secondi [%]",
    "",// TODO
    "",// TODO
    "Largeur des aiguilles Sec [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Ancho del segundero [%]",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "»ÿ‡ÿ›– ·’⁄„›‘›ﬁŸ ·‚‡’€⁄ÿ [%]",// TODO
  },
  { "Width of min arrow [%]",
    "Breite des Minutenzeigers [%]",
    "",// TODO
    "Larghezza freccia minuti [%]",
    "",// TODO
    "",// TODO
    "Largeur des aiguilles Min [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Ancho del minutero [%]",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "»ÿ‡ÿ›– ‹ÿ›„‚›ﬁŸ ·‚‡’€⁄ÿ [%]",// TODO
  },
  { "Width of hour arrow [%]",
    "Breite des Stundenzeigers [%]",
    "",// TODO
    "Larghezza freccia ore [%]",
    "",// TODO
    "",// TODO
    "Largeur des aiguilles Hr [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Ancho de la aguja de las horas [%]",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "»ÿ‡ÿ›– Á–·ﬁ“ﬁŸ ·‚‡’€⁄ÿ [%]",// TODO
  },
  { "Analog clock size",
    "Grˆﬂe der Analogen Uhr",
    "",// TODO
    "Dimensione orologio analogico",
    "",// TODO
    "",// TODO
    "Taille de l'horloge analogique",
    "",// TODO
    "",// TODO
    "",// TODO
    "TamaÒo del reloj analÛgico",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¿–◊‹’‡ ·‚‡’€ﬁÁ›ÎÂ Á–·ﬁ“",// TODO
  },
  { "Transparent",
    "Transparent",
    "",// TODO
    "Trasparente",
    "",// TODO
    "",// TODO
    "Transparent",
    "",// TODO
    "",// TODO
    "",// TODO
    "Transparencia",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ﬁ◊‡–Á›Î’",// TODO
  },
  { "White",
    "Weiss",
    "",// TODO
    "Bianco",
    "",// TODO
    "",// TODO
    "Blanc",
    "",// TODO
    "",// TODO
    "",// TODO
    "Blanco",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "±’€Î’",// TODO
  },
  { "Background color",
    "Hintergrundfarbe",
    "",// TODO
    "Colore sfondo",
    "",// TODO
    "",// TODO
    "Couleur de fond",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color del fondo",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆“’‚ ◊–‘›’”ﬁ ‰ﬁ›–",// TODO
  },
  { "Black",
    "Schwarz",
    "",// TODO
    "Nero",
    "",// TODO
    "",// TODO
    "Noir",
    "",// TODO
    "",// TODO
    "",// TODO
    "Negro",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "«Ò‡›ÎŸ",// TODO
  },
  { "Hour arrow color",
    "Farbe des Stundenzeigers",
    "",// TODO
    "Colore freccia ore",
    "",// TODO
    "",// TODO
    "Couleur aiguille Hr",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color de la aguja de las horas",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆“’‚ Á–·ﬁ“ﬁŸ ·‚‡’€⁄ÿ",// TODO
  },
  { "Red",
    "Rot",
    "",// TODO
    "Rosso",
    "",// TODO
    "",// TODO
    "Rouge",
    "",// TODO
    "",// TODO
    "",// TODO
    "Rojo",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∫‡–·›ÎŸ",// TODO
  },
  { "Green",
    "Gr¸n",
    "",// TODO
    "Verde",
    "",// TODO
    "",// TODO
    "Vert",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Verde",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∑’€Ò›ÎŸ",// TODO  
  },
  { "Yellow",
    "Gelb",
    "",// TODO
    "Giallo",
    "",// TODO
    "",// TODO
    "Jaune",
    "",// TODO
    "",// TODO
    "",// TODO
    "Amarillo",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∂Ò€‚ÎŸ",// TODO
  },
  { "Blue",
    "Blau",
    "",// TODO
    "Blu",
    "",// TODO
    "",// TODO
    "Bleu",
    "",// TODO
    "",// TODO
    "",// TODO
    "Azul",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¡ÿ›ÿŸ",// TODO
  },
  { "Seconds arrow color",
    "Farbe des Sekundenzeigers",
    "",// TODO
    "Colore freccia secondi",
    "",// TODO
    "",// TODO
    "Couleur aiguille Sec",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color del segundero",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆“’‚ ·’⁄„›‘›ﬁŸ ·‚‡’€⁄ÿ",// TODO
  },
  { "Minutes arrow color",
    "Farbe des Minutenzeigers",
    "",// TODO
    "Colore freccia minuti",
    "",// TODO
    "",// TODO
    "Couleur aiguille Min",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color del minutero",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆“’‚ ‹ÿ›„‚›ﬁŸ ·‚‡’€⁄ÿ",// TODO
  },
  { "Foreground color",
    "Vordergrund Farbe",
    "",// TODO
    "Colore primo piano",
    "",// TODO
    "",// TODO
    "Couleur premier plan",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color de primer plano",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆“’‚ ﬂ’‡’‘›’”ﬁ ‰ﬁ›–",// TODO
  },
  { "Arrow color",
    "Zeigerfarbe",
    "",// TODO
    "Colore freccia",
    "",// TODO
    "",// TODO
    "Couleur aiguilles",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color de las agujas",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆“’‚ ·‚‡’€ﬁ⁄",// TODO
  },
  { "Digital",
    "Digital",
    "",// TODO
    "Digitale",
    "",// TODO
    "",// TODO
    "Digital",
    "",// TODO
    "",// TODO
    "",// TODO
    "Digital",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∆Î‰‡ﬁ“Î’",// TODO
  },
  { "Analog",
    "Analog",
    "",// TODO
    "Analogico",
    "",// TODO
    "",// TODO
    "Analogique",
    "",// TODO
    "",// TODO
    "",// TODO
    "AnalÛgico",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¡‚‡’€ﬁÁ›Î’",// TODO
  },
  { "Internet time",
    "Internet Zeit",
    "",// TODO
    "Ora Internet",
    "",// TODO
    "",// TODO
    "Temps internet",
    "",// TODO
    "",// TODO
    "",// TODO
    "Hora de Internet",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∏›‚’‡›’‚ “‡’‹Ô",// TODO
  },
  { "Tea clock",
    "Teekocher",
    "",// TODO
    "Pausa tË",
    "",// TODO
    "",// TODO
    "Pause cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "Pausa para el cafÈ",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "≤‡’‹Ô ‘€Ô Á–Ô",// TODO
  },
  { "It's tea time",
    "Es ist Tee Zeit!",
    "",// TODO
    "E' l'ora del tË!",
    "",// TODO
    "",// TODO
    "c'est l'heure de la pause cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "Es la hora de la pausa para el cafÈ",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "≤‡’‹Ô Á–Ô!",// TODO
  },
  { "Time till tea",
    "Zeit bis der Tee fertig ist",
    "",// TODO
    "Tempo restante per il tË",
    "",// TODO
    "",// TODO
    "Temps restant jusqu'au cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "Tiempo hasta el cafÈ",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "≤‡’‹Ô ◊–“–‡⁄ÿ Á–Ô",// TODO
  },
  { "Show Countdown",
    "Countdown anzeigen",
    "",// TODO
    "Mostra conto alla rovescia",
    "",// TODO
    "",// TODO
    "Afficher le compte a rebours",
    "",// TODO
    "",// TODO
    "",// TODO
    "Mostrar cuenta atras",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "æ—‡–‚›ÎŸ ﬁ‚ÁÒ‚ “‡’‹’›ÿ",// TODO
  },
  { "FullScreen",
    "FullScreen",
    "",// TODO
    "Schermo intero",
    "",// TODO
    "",// TODO
    "FullScreen",
    "",// TODO
    "",// TODO
    "",// TODO
    "Pantalla completa",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "≤’·Ï ‡–◊‹’‡",// TODO
  },
  { "x Position",
    "x Position",
    "",// TODO
    "Posizione x",
    "",// TODO
    "",// TODO
    "x Position",
    "",// TODO
    "",// TODO
    "",// TODO
    "PosiciÛn X",// EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "x øﬁ◊ÿÊÿÔ",// TODO
  },
  { "y Position",
    "y Position",
    "",// TODO
    "Posizione y",
    "",// TODO
    "",// TODO
    "y Position",
    "",// TODO
    "",// TODO
    "",// TODO
    "PosiciÛn Y", // EspaÒol
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "y øﬁ◊ÿÊÿÔ",// TODO
  },
  { NULL }
  };





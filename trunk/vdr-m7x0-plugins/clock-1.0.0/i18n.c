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
    "",
    "",// TODO
    "",// TODO
    "Heure",
    "",// TODO
    "",// TODO
    "",// TODO
    "Reloj",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Offset",
    "Abstand vom linken Rand",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Distance du bord gauche",
    "",// TODO
    "",// TODO
    "",// TODO
    "Distancia desde la izquierda",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Show Seconds",
    "Sekunden anzeigen",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Afficher secondes",
    "",// TODO
    "",// TODO
    "",// TODO
    "Mostrar segundos",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Type of the clock",
    "Art der Uhr",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Format d'affichage",
    "",// TODO
    "",// TODO
    "",// TODO
    "Tipo de reloj",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Length of sec arrow [%]",
    "Länge des Sekundenzeigers [%]",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Longueur aiguille Sec [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Longitud del segundero [%]",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Length of min arrow [%]",
    "Länge des Minutenzeigers [%]",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Longueur aiguille Min [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Longitud del minutero [%]",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Length of hour arrow [%]",
    "Länge des Stundenzeigers [%]",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Longueur aiguille Hr [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Longitud aguja de las horas [%]",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Width of sec arrow [%]",
    "Breite des Sekundenzeigers [%]",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Largeur des aiguilles Sec [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Ancho del segundero [%]",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Width of min arrow [%]",
    "Breite des Minutenzeigers [%]",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Largeur des aiguilles Min [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Ancho del minutero [%]",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Width of hour arrow [%]",
    "Breite des Stundenzeigers [%]",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Largeur des aiguilles Hr [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "Ancho de la aguja de las horas [%]",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Analog clock size",
    "Größe der Analogen Uhr",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Taille de l'horloge analogique",
    "",// TODO
    "",// TODO
    "",// TODO
    "Tamaño del reloj analógico",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Transparent",
    "Transparent",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Transparent",
    "",// TODO
    "",// TODO
    "",// TODO
    "Transparencia",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "White",
    "Weiss",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Blanc",
    "",// TODO
    "",// TODO
    "",// TODO
    "Blanco",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Background color",
    "Hintergrundfarbe",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Couleur de fond",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color del fondo",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Black",
    "Schwarz",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Noir",
    "",// TODO
    "",// TODO
    "",// TODO
    "Negro",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Hour arrow color",
    "Farbe des Stundenzeigers",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Couleur aiguille Hr",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color de la aguja de las horas",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Red",
    "Rot",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Rouge",
    "",// TODO
    "",// TODO
    "",// TODO
    "Rojo",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Green",
    "Grün",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Vert",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Verde",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO  
  },
  { "Yellow",
    "Gelb",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Jaune",
    "",// TODO
    "",// TODO
    "",// TODO
    "Amarillo",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Blue",
    "Blau",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Bleu",
    "",// TODO
    "",// TODO
    "",// TODO
    "Azul",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Seconds arrow color",
    "Farbe des Sekundenzeigers",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Couleur aiguille Sec",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color del segundero",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Minutes arrow color",
    "Farbe des Minutenzeigers",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Couleur aiguille Min",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color del minutero",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Foreground color",
    "Vordergrund Farbe",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Couleur premier plan",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color de primer plano",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Arrow color",
    "Zeigerfarbe",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Couleur aiguilles",
    "",// TODO
    "",// TODO
    "",// TODO
    "Color de las agujas",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Digital",
    "Digital",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Digital",
    "",// TODO
    "",// TODO
    "",// TODO
    "Digital",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Analog",
    "Analog",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Analogique",
    "",// TODO
    "",// TODO
    "",// TODO
    "Analógico",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Internet time",
    "Internet Zeit",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Temps internet",
    "",// TODO
    "",// TODO
    "",// TODO
    "Hora de Internet",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Tea clock",
    "Teekocher",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Pause cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "Pausa para el café",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "It's tea time",
    "Es ist Tee Zeit!",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "c'est l'heure de la pause cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "Es la hora de la pausa para el café",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Time till tea",
    "Zeit bis der Tee fertig ist",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Temps restant jusqu'au cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "Tiempo hasta el café",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Show Countdown",
    "Countdown anzeigen",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "Afficher le compte a rebours",
    "",// TODO
    "",// TODO
    "",// TODO
    "Mostrar cuenta atrás",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "FullScreen",
    "FullScreen",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "FullScreen",
    "",// TODO
    "",// TODO
    "",// TODO
    "Pantalla completa",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "x Position",
    "x Position",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "x Position",
    "",// TODO
    "",// TODO
    "",// TODO
    "Posición X",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "y Position",
    "y Position",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "y Position",
    "",// TODO
    "",// TODO
    "",// TODO
    "Posición Y", // Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { NULL }
  };





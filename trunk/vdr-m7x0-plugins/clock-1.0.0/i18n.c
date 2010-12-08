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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "��������� �� ������ ����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�������� �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "��� �����",// TODO
  },
  { "Length of sec arrow [%]",
    "L�nge des Sekundenzeigers [%]",
    "",// TODO
    "Lunghezza freccia secondi [%]",
    "",// TODO
    "",// TODO
    "Longueur aiguille Sec [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����� ��������� ������� [%]",// TODO
  },
  { "Length of min arrow [%]",
    "L�nge des Minutenzeigers [%]",
    "",// TODO
    "Lunghezza freccia minuti [%]",
    "",// TODO
    "",// TODO
    "Longueur aiguille Min [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����� �������� ������� [%]",// TODO
  },
  { "Length of hour arrow [%]",
    "L�nge des Stundenzeigers [%]",
    "",// TODO
    "Lunghezza freccia ore [%]",
    "",// TODO
    "",// TODO
    "Longueur aiguille Hr [%]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�����  ������� ������� [%]",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������ ��������� ������� [%]",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������ �������� ������� [%]",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������ ������� ������� [%]",// TODO
  },
  { "Analog clock size",
    "Gr��e der Analogen Uhr",
    "",// TODO
    "Dimensione orologio analogico",
    "",// TODO
    "",// TODO
    "Taille de l'horloge analogique",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������ ���������� �����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� ������� ����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� ������� �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�������",// TODO
  },
  { "Green",
    "Gr�n",
    "",// TODO
    "Verde",
    "",// TODO
    "",// TODO
    "Vert",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�������",// TODO  
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� ��������� �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� �������� �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� ��������� ����",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "��������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�������� �����",// TODO
  },
  { "Tea clock",
    "Teekocher",
    "",// TODO
    "Pausa t�",
    "",// TODO
    "",// TODO
    "Pause cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����� ��� ���",// TODO
  },
  { "It's tea time",
    "Es ist Tee Zeit!",
    "",// TODO
    "E' l'ora del t�!",
    "",// TODO
    "",// TODO
    "c'est l'heure de la pause cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����� ���!",// TODO
  },
  { "Time till tea",
    "Zeit bis der Tee fertig ist",
    "",// TODO
    "Tempo restante per il t�",
    "",// TODO
    "",// TODO
    "Temps restant jusqu'au cafe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "����� ������� ���",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�������� ����� �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "���� ������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "x �������",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "y �������",// TODO
  },
  { NULL }
  };





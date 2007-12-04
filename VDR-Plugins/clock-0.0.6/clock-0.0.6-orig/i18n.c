/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Clock",
    "Uhr",
    "",// TODO
    "",// TODO
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
    "Часы",// TODO
  },
  { "Offset",
    "Abstand vom linken Rand",
    "",// TODO
    "",// TODO
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
    "Растояние от левого края",// TODO
  },
  { "Show Seconds",
    "Sekunden anzeigen",
    "",// TODO
    "",// TODO
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
    "Показать секунды",// TODO
  },
  { "Type of the clock",
    "Art der Uhr",
    "",// TODO
    "",// TODO
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
    "Вид часов",// TODO
  },
  { "Length of sec arrow [%]",
    "Lфnge des Sekundenzeigers [%]",
    "",// TODO
    "",// TODO
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
    "Длина секундной стрелки [%]",// TODO
  },
  { "Length of min arrow [%]",
    "Lфnge des Minutenzeigers [%]",
    "",// TODO
    "",// TODO
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
    "Длина минутной стрелки [%]",// TODO
  },
  { "Length of hour arrow [%]",
    "Lфnge des Stundenzeigers [%]",
    "",// TODO
    "",// TODO
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
    "Длина  часовой стрелки [%]",// TODO
  },
  { "Width of sec arrow [%]",
    "Breite des Sekundenzeigers [%]",
    "",// TODO
    "",// TODO
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
    "Ширина секундной стрелки [%]",// TODO
  },
  { "Width of min arrow [%]",
    "Breite des Minutenzeigers [%]",
    "",// TODO
    "",// TODO
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
    "Ширина минутной стрелки [%]",// TODO
  },
  { "Width of hour arrow [%]",
    "Breite des Stundenzeigers [%]",
    "",// TODO
    "",// TODO
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
    "Ширина часовой стрелки [%]",// TODO
  },
  { "Analog clock size",
    "Grіпe der Analogen Uhr",
    "",// TODO
    "",// TODO
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
    "Размер стрелочных часов",// TODO
  },
  { "Transparent",
    "Transparent",
    "",// TODO
    "",// TODO
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
    "Прозрачные",// TODO
  },
  { "White",
    "Weiss",
    "",// TODO
    "",// TODO
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
    "Белые",// TODO
  },
  { "Background color",
    "Hintergrundfarbe",
    "",// TODO
    "",// TODO
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
    "Цвет заднего фона",// TODO
  },
  { "Black",
    "Schwarz",
    "",// TODO
    "",// TODO
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
    "Чёрный",// TODO
  },
  { "Hour arrow color",
    "Farbe des Stundenzeigers",
    "",// TODO
    "",// TODO
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
    "Цвет часовой стрелки",// TODO
  },
  { "Red",
    "Rot",
    "",// TODO
    "",// TODO
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
    "Красный",// TODO
  },
  { "Green",
    "Grќn",
    "",// TODO
    "",// TODO
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
    "Зелёный",// TODO  
  },
  { "Yellow",
    "Gelb",
    "",// TODO
    "",// TODO
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
    "Жёлтый",// TODO
  },
  { "Blue",
    "Blau",
    "",// TODO
    "",// TODO
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
    "Синий",// TODO
  },
  { "Seconds arrow color",
    "Farbe des Sekundenzeigers",
    "",// TODO
    "",// TODO
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
    "Цвет секундной стрелки",// TODO
  },
  { "Minutes arrow color",
    "Farbe des Minutenzeigers",
    "",// TODO
    "",// TODO
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
    "Цвет минутной стрелки",// TODO
  },
  { "Foreground color",
    "Vordergrund Farbe",
    "",// TODO
    "",// TODO
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
    "Цвет переднего фона",// TODO
  },
  { "Arrow color",
    "Zeigerfarbe",
    "",// TODO
    "",// TODO
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
    "Цвет стрелок",// TODO
  },
  { "Digital",
    "Digital",
    "",// TODO
    "",// TODO
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
    "Цыфровые",// TODO
  },
  { "Analog",
    "Analog",
    "",// TODO
    "",// TODO
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
    "Стрелочные",// TODO
  },
  { "Internet time",
    "Internet Zeit",
    "",// TODO
    "",// TODO
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
    "Интернет время",// TODO
  },
  { "Tea clock",
    "Teekocher",
    "",// TODO
    "",// TODO
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
    "Время для чая",// TODO
  },
  { "It's tea time",
    "Es ist Tee Zeit!",
    "",// TODO
    "",// TODO
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
    "Время чая!",// TODO
  },
  { "Time till tea",
    "Zeit bis der Tee fertig ist",
    "",// TODO
    "",// TODO
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
    "Время заварки чая",// TODO
  },
  { "Show Countdown",
    "Countdown anzeigen",
    "",// TODO
    "",// TODO
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
    "Обратный отчёт времени",// TODO
  },
  { "FullScreen",
    "FullScreen",
    "",// TODO
    "",// TODO
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
    "Весь размер",// TODO
  },
  { "x Position",
    "x Position",
    "",// TODO
    "",// TODO
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
    "x Позиция",// TODO
  },
  { "y Position",
    "y Position",
    "",// TODO
    "",// TODO
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
    "y Позиция",// TODO
  },
  { NULL }
  };





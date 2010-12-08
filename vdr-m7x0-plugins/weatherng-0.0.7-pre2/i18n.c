/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.4 2004/01/16 13:28:23 kls Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Weather-NG",
    "Wetterbericht",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pr�vision M�t�o-NG",
    "",// TODO
    "S��ennuste",
    "",// TODO
    "Previsi�n meteorol�gica",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Weather.com: Weather forecast",
    "Weather.com: 10 Tage Wettervorhersage",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Weather.com: Pr�vision m�t�o",
    "",// TODO
    "Weather.com:in s��ennusteet",
    "",// TODO
    "Weather.com: Previsi�n meteorol�gica",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Weather",
    "Wetter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pr�vision",
    "",// TODO
    "S��",
    "",// TODO
    "Previsi�n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "ERROR : NO DATA",
    "FEHLER : KEINE DATEN VORHANDEN",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Virhe: S��tietoja ei saatavilla!",
    "",// TODO
    "No hay datos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "PRESS OK to download/update data/radarmaps",
    "TASTE OK druecken um Daten zu aktualisieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Paina 'OK' p�ivitt��ksesi s��ennusteet ja tutkakartat",
    "",// TODO
    "Pulsa OK para descargar informaci�n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Have you edited weatherng.sh ?",
    "Wurde weatherng.sh editiert ?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Olethan muokannut weatherng.sh:n toimivaksi?",
    "",// TODO
    "�Has editado weather.sh?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Path to weatherng.sh is :",
    "Pfad zu weatherng.sh ist :",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Weatherng.sh:n sijainti:",
    "",// TODO
    "La ruta a weather.sh es: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Path to weatherdata is :",
    "Pfad zu den Wetterdaten ist :",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "S��tietojen sijainti:",
    "",// TODO
    "La ruta a weatherdata es: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Sunset: ",
    "Sonnenuntergang: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Coucher du soleil: ",
    "",// TODO
    "Aurinko laskee: ",
    "",// TODO
    "Puesta de sol: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Sunrise: ",
    "Sonnenaufgang: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lever du soleil: ",
    "",// TODO
    "Aurinko nousee:",
    "",// TODO
    "Amanecer: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Weather: ",
    "Wetter: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pr�vision: ",
    "",// TODO
    "S��tila: ",
    "",// TODO
    "Previsi�n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Low temperature: ",
    "Tiefsttemperatur: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp. basse: ",
    "",// TODO
    "Alin l�mp�tila: ",
    "",// TODO
    "M�nima",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "High temperature: ",
    "H�chsttemperatur: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp. haute: ",
    "",// TODO
    "Ylin l�mp�tila: ",
    "",// TODO
    "M�xima",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Weather for: ",
    "Wetter f�r: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pr�vision pour: ",
    "",// TODO
    "S��: ",
    "",// TODO
    "Previsi�n para: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Wind comes from: ",
    "Wind kommt von: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Provenance du vent: ",
    "",// TODO
    "Tuulen suunta: ",
    "",// TODO
    "Direcci�n del viento:",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Wind speed: ",
    "Windgeschwind.: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vitesse du vent: ",
    "",// TODO
    "Tuulen nopeus: ",
    "",// TODO
    "Viento:",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { " km/h",
    " km/h",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    " km/h",
    "",// TODO
    " km/h",
    "",// TODO
    " km/h",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Humitidy: ",
    "max. Luftfeuchte: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Taux d'humidit�: ",
    "",// TODO
    "Ilmankosteus: ",
    "",// TODO
    "Humedad: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Precipitation: ",
    "Niederschlag: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pr�cipitation: ",
    "",// TODO
    "Sateen tod.n�k.: ",
    "",// TODO
    "Precipitaci�n: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { " %",
    " %",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    " %",
    "",// TODO
    " %",
    "",// TODO
    " %",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "�C",
    "�C",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "�C",
    "",// TODO
    "�C",
    "",// TODO
    "�C",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Update data on start",
    "Update von Daten bei Start",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "P�ivit� s��tiedot k�ynnistett�ess�",
    "",// TODO
    "Actualizar datos al arranque de VDR",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Radarbitmap left",
    "Radarbild von links",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Gauche de la carte",
    "",// TODO
    "Tutkakuvan vaakakeskitys",
    "",// TODO
    "Posici�n imagen radar (izquierda)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Radarbitmap top",
    "Radarbild von oben",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Haut de la carte",
    "",// TODO
    "Tutkakuvan pystykeskitys",
    "",// TODO
    "Posici�n imagen radar (arriba)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Radarbitmap width",
    "Radarbild Breite",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Largeur de la carte",
    "",// TODO
    "Tutkakuvan leveys",
    "",// TODO
    "Ancho imagen de radar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Radarbitmap height",
    "Radarbild H�he",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Hauteur de la carte",
    "",// TODO
    "Tutkakuvan korkeus",
    "",// TODO
    "Alto imagen de radar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Use 256 colors for radarmaps",
    "Radarbilder mit 256 Farben",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "N�yt� tutkakuvat 256 v�rill�",
    "",// TODO
    "Usar 256 colores para radarmaps",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Use small fonts",
    "Kleine Schriftart nutzen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser petite police",
    "",// TODO
    "K�yt� pieni� kirjasimia",
    "",// TODO
    "Usar fuentes peque�as",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Classic",
    "Klassisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Classic",
    "",// TODO
    "Klassinen",
    "",// TODO
    "Cl�sico",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "enElchi",
    "enElchi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "enElchi",
    "",// TODO
    "enElchi",
    "",// TODO
    "enElchi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Moronimo",
    "Moronimo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Moronimo",
    "",// TODO
    "Moronimo",
    "",// TODO
    "Moronimo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "DeepBlue",
    "DeepBlue",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "DeepBlue",
    "",// TODO
    "DeepBlue",
    "",// TODO
    "DeepBlue",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Enigma",
    "Enigma",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Enigma",
    "",// TODO
    "Enigma",
    "",// TODO
    "Enigma",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "SilverGreen",
    "SilverGreen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "SilverGreen",
    "",// TODO
    "SilverGreen",
    "",// TODO
    "SilverGreen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "MoronimoMKII",
    "MoronimoMKII",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "MoronimoMKII",
    "",// TODO
    "MoronimoMKII",
    "",// TODO
    "MoronimoMKII",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "MoronimoMKIII",
    "MoronimoMKIII",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "MoronimoMKIII",
    "",// TODO
    "MoronimoMKIII",
    "",// TODO
    "MoronimoMKIII",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "MoronimoMKIV",
    "MoronimoMKIV",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "MoronimoMKIV",
    "",// TODO
    "MoronimoMKIV",
    "",// TODO
    "MoronimoMKIV",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Theme",
    "Farbschema",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Th�me",
    "",// TODO
    "Teema",
    "",// TODO
    "Tema",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Height",
    "H�he",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Hauteur",
    "",// TODO
    "Korkeus",
    "",// TODO
    "Altura",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Offset horizontal",
    "Offset horizontal",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Offset horizontal",
    "",// TODO
    "Vaakakeskitys",
    "",// TODO
    "Desplazamiento horizontal",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Offset vertical",
    "Offset vertikal",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Offset vertical",
    "",// TODO
    "Pystykeskitys",
    "",// TODO
    "Desplazamiento vertical",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Symbols inverted",
    "Invertierte Symbole",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Symboles invers�s",
    "",// TODO
    "N�yt� symbolit invertoituna",
    "",// TODO
    "Invertir s�mbolos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Corner rounded",
    "Abgerundete Ecken",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "K�yt� py�ristettyj� kulmia",
    "",// TODO
    "Bordes redondeados",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "DAY",
    "TAGS�BER",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "JOUR",
    "",// TODO
    "P�IV�",
    "",// TODO
    "DIA",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "NIGHT",
    "NACHTS",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "NUIT",
    "",// TODO
    "Y�",
    "",// TODO
    "NOCHE",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Monday",
    "Montag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lundi",
    "",// TODO
    "Maanantai",
    "",// TODO
    "Lunes",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Tuesday",
    "Dienstag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mardi",
    "",// TODO
    "Tiistai",
    "",// TODO
    "Martes",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Wednesday",
    "Mittwoch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mercredi",
    "",// TODO
    "Keskiviikko",
    "",// TODO
    "Mi�rcoles",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Thursday",
    "Donnerstag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Jeudi",
    "",// TODO
    "Torstai",
    "",// TODO
    "Jueves",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Friday",
    "Freitag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vendredi",
    "",// TODO
    "Perjantai",
    "",// TODO
    "Viernes",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Saturday",
    "Samstag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Samedi",
    "",// TODO
    "Lauantai",
    "",// TODO
    "S�bado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Sunday",
    "Sonntag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Dimanche",
    "",// TODO
    "Sunnuntai",
    "",// TODO
    "Domingo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Sunny",
    "sonnig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ensoleill�",
    "",// TODO
    "aurinkoista",
    "",// TODO
    "Soleado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Rain",
    "Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie",
    "",// TODO
    "sateista",
    "",// TODO
    "Lluvia",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Thunder",
    "Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Tonnerre",
    "",// TODO
    "ukkosta",
    "",// TODO
    "Truenos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Cloudy",
    "bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Nuageux",
    "",// TODO
    "pilvist�",
    "",// TODO
    "Nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Mostly Cloudy",
    "meist bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Tr�s nuageux",
    "",// TODO
    "enimm�kseen pilvist�",
    "",// TODO
    "Parcialmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "PM T-Storms",
    "nachmittags Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages en soir�e",
    "",// TODO
    "iltap�iv�ll� ukkoskuuroja",
    "",// TODO
    "Tormenta el�ctrica nocturna",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "AM T-Storms",
    "vormittags Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages matinaux",
    "",// TODO
    "aamup�iv�ll� ukkoskuuroja",
    "",// TODO
    "Tormenta el�ctrica matinal",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Light Rain",
    "leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie l�g�re",
    "",// TODO
    "kevytt� sadetta",
    "",// TODO
    "Lluvia ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Snow",
    "Schnee",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Neige",
    "",// TODO
    "lumisadetta",
    "",// TODO
    "Nieve",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Showers",
    "Regenschauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses",
    "",// TODO
    "sadekuuroja",
    "",// TODO
    "Chubascos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "AM Showers",
    "vormittags Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses matinales",
    "",// TODO
    "aamup�iv�ll� sadekuuroja",
    "",// TODO
    "Chubascos matinales",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "PM Showers",
    "nachmittags Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses en soir�e",
    "",// TODO
    "iltap�iv�ll� sadekuuroja",
    "",// TODO
    "Chubascos nocturnos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Scattered Showers",
    "vereinzelt Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses isol�es",
    "",// TODO
    "hajanaisia sadekuuroja",
    "",// TODO
    "Chubascos dispersos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Rain / Thunder",
    "Regen / Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie / Orage",
    "",// TODO
    "sadetta / ukkosta",
    "",// TODO
    "Lluvia / Truenos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Rain / Wind",
    "Regen / Windig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie / Vent",
    "",// TODO
    "sadetta / tuulista",
    "",// TODO
    "Lluvia / Viento",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Mostly Clear",
    "meist klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "G�n�ralement d�gag�",
    "",// TODO
    "enimm�kseen selke��",
    "",// TODO
    "Parcialmente despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Haze",
    "dunstig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Brume",
    "",// TODO
    "utua",
    "",// TODO
    "Bruma",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Fog",
    "Nebel",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Brouillard",
    "",// TODO
    "sumua",
    "",// TODO
    "Niebla",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Wind",
    "windig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vent",
    "",// TODO
    "tuulista",
    "",// TODO
    "Viento",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Fair",
    "trocken",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Sec",
    "",// TODO
    "poutaa",
    "",// TODO
    "Buen tiempo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Clear",
    "heiter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "D�gag�",
    "",// TODO
    "selke��",
    "",// TODO
    "Despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Windy",
    "windig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vents l�ger",
    "",// TODO
    "tuulista",
    "",// TODO
    "Ventoso",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Drizzle",
    "Nieselregen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Bruine",
    "",// TODO
    "tihkua",
    "",// TODO
    "Llovizna",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Freezing",
    "Frost",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Froid",
    "",// TODO
    "hallaa",
    "",// TODO
    "Heladas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "M Cloudy",
    "meist bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "G�n�ralement nuageux",
    "",// TODO
    "enimm�kseen pilvist�",
    "",// TODO
    "Principalmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Partly Cloudy",
    "teilweise bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Partiellement nuageux",
    "",// TODO
    "osittain pilvist�",
    "",// TODO
    "Parcialmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "M Clear",
    "meist klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "G�n�ralement d�gag�",
    "",// TODO
    "enimm�kseen selke��",
    "",// TODO
    "Principalmente despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "N/A",
    "keine Angaben",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Non Disponible",
    "",// TODO
    "ei tietoja",
    "",// TODO
    "N/D",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Mostly Sunny",
    "meist sonnig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "G�n�ralement ensoleill�",
    "",// TODO
    "enimm�kseen aurinkoista",
    "",// TODO
    "Principalmente soleado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Partly Cloudy",
    "teilweise bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Partiellement nuageux",
    "",// TODO
    "osittain pilvist�",
    "",// TODO
    "Parcialmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Showers Late",
    "Regen sp�t",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses en soir�e",
    "",// TODO
    "sadetta my�hemmin",
    "",// TODO
    "Lluvia por la tarde",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "AM Clouds / PM Sun",
    "vormittags bew�lkt / nachmittags Sonne",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Matin nuageux / Apr�s-midi Soleil",
    "",// TODO
    "aamup�iv�ll� pilvist� / iltap�iv�ll� aurinkoista",
    "",// TODO
    "Ma�ana nubes / Tarde sol",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },

  { "Scattered T-Storms",
    "vereinzelt Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages isol�s",
    "",// TODO
    "hajanaisia ukkosmyrskyj�",
    "",// TODO
    "Tormentas el�ctricas dispersas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Overcast",
    "bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Couvert",
    "",// TODO
    "pilvist�",
    "",// TODO
    "Encapotado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Fog",
    "Nebel",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Brouillard",
    "",// TODO
    "sumua",
    "",// TODO
    "Niebla",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Mist",
    "Nebel",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Brume",
    "",// TODO
    "usvaa",
    "",// TODO
    "Niebla ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { "Maze",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "hernerokkasumua",
    "",// TODO
    "Niebla densa (maze)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Storm",
    "Sturm",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp�te",
    "",// TODO
    "myrsky�",
    "",// TODO
    "Tormenta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Snow",
    "Schnee",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Neige",
    "",// TODO
    "lumisadetta",
    "",// TODO
    "Nieve",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Light Rain",
    "Leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie fine",
    "",// TODO
    "kevytt� sadetta",
    "",// TODO
    "Lluvia ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Light Snow",
    "Leichter Schnee",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Neige (leger)",
    "",// TODO
    "kevytt� lumisadetta",
    "",// TODO
    "Nieve ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Few Snow",
    "wenig Schnee",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Neige abondante",
    "",// TODO
    "heikkoa lumisadetta",
    "",// TODO
    "Algo de nieve",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Clear",
    "Klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "D�gag�",
    "",// TODO
    "selke��",
    "",// TODO
    "Despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Light Drizzle",
    "wenig Nieselregen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "L�g�re bruine",
    "",// TODO
    "kevytt� tihkua",
    "",// TODO
    "Llovizna",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Scattered",
    "vereinzelt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Isol�",
    "",// TODO
    "hajanaista",
    "",// TODO
    "Nubes dispersas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Smoke",
    "dichter Nebel",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Brouillard dense",
    "",// TODO
    "paksua sumua",
    "",// TODO
    "Humo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Drifting Snow",
    "starkes Schneetreiben",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Forte neige",
    "",// TODO
    "voimakasta lumisadetta",
    "",// TODO
    "Nieve levantada por el viento",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "steady",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Stable",
    "",// TODO
    "tasaista",
    "",// TODO
    "constante",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "rising",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "augmentation",
    "",// TODO
    "alkavaa",
    "",// TODO
    "el aumento de ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "falling",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Chute",
    "",// TODO
    "laantuvaa",
    "",// TODO
    "ca�da",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Unlimited",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Sans limites",
    "",// TODO
    "m��rittelem�t�nt�",
    "",// TODO
    "Ilimitado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Showers in the Vicinity",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses dans les environs",
    "",// TODO
    "todenn�k�isesti sadekuuroja",
    "",// TODO
    "Chubascos en los alrededores",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Waning Crescent",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Diminution de croissant",
    "",// TODO
    "v�istyv� t�ysikuu",
    "",// TODO
    "Nueva visible",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Waxing Crescent",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Augmentation de croissant",
    "",// TODO
    "alkava t�ysikuu",
    "",// TODO
    "Media luna creciente",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Waxing Gibbous",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "v�istyv� uusikuu",
    "",// TODO
    "Gibada creciente",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Waning Gibbous",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "v�istyv� t�ysikuu",
    "",// TODO
    "Gibada menguante",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "T-Storms",
    "st�rmisches Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages",
    "",// TODO
    "ukkosmyrskyj�",
    "",// TODO
    "Tormentas el�ctricas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "T-Storms Early",
    "zun�chst schweres Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages matinaux",
    "",// TODO
    "ukkosmyrskyj� aikaisin",
    "",// TODO
    "Tormentas el�ctricas tempranas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Light Rain Early",
    "zun�chst leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie fine t�t le matin",
    "",// TODO
    "kevytt� sadetta aikaisin",
    "",// TODO
    "Lluvia fina de ma�ana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Showers Early",
    "zun�chst Regenschauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses matinales",
    "",// TODO
    "sadekuuroja aikaisin",
    "",// TODO
    "Chubascos de ma�ana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Cloudly Early",
    "zun�chst bew�lkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pilvist� aikaisin",
    "",// TODO
    "Nublado de ma�ana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "PM Light Rain",
    "nachmittags leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "iltap�iv�ll� kevytt� sadetta",
    "",// TODO
    "Lluvia ligera por la tarde",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "AM Light Rain",
    "vormittags leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "aamup�iv�ll� kevytt� sadetta",
    "",// TODO
    "Lluvia ligera por la ma�ana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Light Rain Late",
    "sp�ter leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "kevytt� sadetta my�h��n",
    "",// TODO
    "Lluvia ligera por la tarde",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
    { "Few Showers",
    "vereinzelte Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ajoittaisia sadekuuroja",
    "",// TODO
    "Algunos chubascos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
      { "Weather station 1",
      "",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",
      "",// TODO
      "Estaci�n meteorol�gica 1",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "������!",
  },
      { "Weather station 2",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "Estaci�n meteorol�gica 2",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "������!",
  },
      { "Weather station 3",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "Estaci�n meteorol�gica 3",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "������!",
  },
  { NULL }
  };

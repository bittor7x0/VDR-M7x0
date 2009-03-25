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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
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
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "������!",
  },
  { NULL }
  };

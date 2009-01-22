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
    "PrÈvision MÈtÈo-NG",
    "",// TODO
    "S‰‰ennuste",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Weather.com: Weather forecast",
    "Weather.com: 10 Tage Wettervorhersage",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Weather.com: PrÈvision mÈtÈo",
    "",// TODO
    "Weather.com:in s‰‰ennusteet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Weather",
    "Wetter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "PrÈvision",
    "",// TODO
    "S‰‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "ERROR : NO DATA",
    "FEHLER : KEINE DATEN VORHANDEN",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Virhe: S‰‰tietoja ei saatavilla!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "PRESS OK to download/update data/radarmaps",
    "TASTE OK druecken um Daten zu aktualisieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Paina 'OK' p‰ivitt‰‰ksesi s‰‰ennusteet ja tutkakartat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Path to weatherdata is :",
    "Pfad zu den Wetterdaten ist :",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "S‰‰tietojen sijainti:",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Weather: ",
    "Wetter: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "PrÈvision: ",
    "",// TODO
    "S‰‰tila: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Low temperature: ",
    "Tiefsttemperatur: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp. basse: ",
    "",// TODO
    "Alin l‰mpˆtila: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "High temperature: ",
    "Hˆchsttemperatur: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp. haute: ",
    "",// TODO
    "Ylin l‰mpˆtila: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Weather for: ",
    "Wetter f¸r: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "PrÈvision pour: ",
    "",// TODO
    "S‰‰: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Humitidy: ",
    "max. Luftfeuchte: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Taux d'humiditÈ: ",
    "",// TODO
    "Ilmankosteus: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Precipitation: ",
    "Niederschlag: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "PrÈcipitation: ",
    "",// TODO
    "Sateen tod.n‰k.: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "∞C",
    "∞C",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "∞C",
    "",// TODO
    "∞C",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Update data on start",
    "Update von Daten bei Start",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "P‰ivit‰ s‰‰tiedot k‰ynnistett‰ess‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Radarbitmap height",
    "Radarbild Hˆhe",
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
    "ø‡ÿ“’‚!",
  },
  { "Use 256 colors for radarmaps",
    "Radarbilder mit 256 Farben",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "N‰yt‰ tutkakuvat 256 v‰rill‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Use small fonts",
    "Kleine Schriftart nutzen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser petite police",
    "",// TODO
    "K‰yt‰ pieni‰ kirjasimia",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Theme",
    "Farbschema",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ThËme",
    "",// TODO
    "Teema",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Height",
    "Hˆhe",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Symbols inverted",
    "Invertierte Symbole",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Symboles inversÈs",
    "",// TODO
    "N‰yt‰ symbolit invertoituna",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Corner rounded",
    "Abgerundete Ecken",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "K‰yt‰ pyˆristettyj‰ kulmia",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "DAY",
    "TAGS‹BER",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "JOUR",
    "",// TODO
    "PƒIVƒ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "NIGHT",
    "NACHTS",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "NUIT",
    "",// TODO
    "Y÷",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Sunny",
    "sonnig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "EnsoleillÈ",
    "",// TODO
    "aurinkoista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Cloudy",
    "bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Nuageux",
    "",// TODO
    "pilvist‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Mostly Cloudy",
    "meist bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "TrËs nuageux",
    "",// TODO
    "enimm‰kseen pilvist‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "PM T-Storms",
    "nachmittags Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages en soirÈe",
    "",// TODO
    "iltap‰iv‰ll‰ ukkoskuuroja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "AM T-Storms",
    "vormittags Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages matinaux",
    "",// TODO
    "aamup‰iv‰ll‰ ukkoskuuroja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Light Rain",
    "leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie lÈgËre",
    "",// TODO
    "kevytt‰ sadetta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "AM Showers",
    "vormittags Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses matinales",
    "",// TODO
    "aamup‰iv‰ll‰ sadekuuroja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",                                                                                        
  },
  { "PM Showers",                                                                                     
    "nachmittags Schauer",                                                                        
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses en soirÈe",
    "",// TODO
    "iltap‰iv‰ll‰ sadekuuroja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",                                                                                        
  },
  { "Scattered Showers",
    "vereinzelt Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses isolÈes",
    "",// TODO
    "hajanaisia sadekuuroja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Mostly Clear",
    "meist klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "GÈnÈralement dÈgagÈ",
    "",// TODO
    "enimm‰kseen selke‰‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Clear",
    "heiter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "DÈgagÈ",
    "",// TODO
    "selke‰‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Windy",
    "windig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vents lÈger",
    "",// TODO
    "tuulista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "M Cloudy",
    "meist bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "GÈnÈralement nuageux",
    "",// TODO
    "enimm‰kseen pilvist‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Partly Cloudy",
    "teilweise bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Partiellement nuageux",
    "",// TODO
    "osittain pilvist‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",                                                                                        
  },
  { "M Clear",
    "meist klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "GÈnÈralement dÈgagÈ",
    "",// TODO
    "enimm‰kseen selke‰‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { "Mostly Sunny",
    "meist sonnig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "GÈnÈralement ensoleillÈ",
    "",// TODO
    "enimm‰kseen aurinkoista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Partly Cloudy",
    "teilweise bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Partiellement nuageux",
    "",// TODO
    "osittain pilvist‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Showers Late",
    "Regen sp‰t",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses en soirÈe",
    "",// TODO
    "sadetta myˆhemmin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "AM Clouds / PM Sun",
    "vormittags bewˆlkt / nachmittags Sonne",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Matin nuageux / AprËs-midi Soleil",
    "",// TODO
    "aamup‰iv‰ll‰ pilvist‰ / iltap‰iv‰ll‰ aurinkoista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },

  { "Scattered T-Storms",
    "vereinzelt Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages isolÈs",
    "",// TODO
    "hajanaisia ukkosmyrskyj‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
  { "Overcast",
    "bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Couvert",
    "",// TODO
    "pilvist‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
    { "Storm",
    "Sturm",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "TempÍte",
    "",// TODO
    "myrsky‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
    { "Light Rain",
    "Leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie fine",
    "",// TODO
    "kevytt‰ sadetta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
    { "Light Snow",
    "Leichter Schnee",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Neige (leger)",
    "",// TODO
    "kevytt‰ lumisadetta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  }, 
    { "Clear",
    "Klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "DÈgagÈ",
    "",// TODO
    "selke‰‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },   
    { "Light Drizzle",
    "wenig Nieselregen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "LÈgËre bruine",
    "",// TODO
    "kevytt‰ tihkua",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Scattered",
    "vereinzelt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "IsolÈ",
    "",// TODO
    "hajanaista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },  
    { "Unlimited",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Sans limites",
    "",// TODO
    "m‰‰rittelem‰tˆnt‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Showers in the Vicinity",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses dans les environs",
    "",// TODO
    "todenn‰kˆisesti sadekuuroja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Waning Crescent",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Diminution de croissant",
    "",// TODO
    "v‰istyv‰ t‰ysikuu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Waxing Crescent",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Augmentation de croissant",
    "",// TODO
    "alkava t‰ysikuu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Waxing Gibbous",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "v‰istyv‰ uusikuu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Waning Gibbous",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "v‰istyv‰ t‰ysikuu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "T-Storms",
    "st¸rmisches Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages",
    "",// TODO
    "ukkosmyrskyj‰",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "T-Storms Early",
    "zun‰chst schweres Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages matinaux",
    "",// TODO
    "ukkosmyrskyj‰ aikaisin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Light Rain Early",
    "zun‰chst leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie fine tÙt le matin",
    "",// TODO
    "kevytt‰ sadetta aikaisin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },  
    { "Showers Early",
    "zun‰chst Regenschauer",
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
    "ø‡ÿ“’‚!",
  },
    { "Cloudly Early",
    "zun‰chst bewˆlkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pilvist‰ aikaisin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
    { "PM Light Rain",
    "nachmittags leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "iltap‰iv‰ll‰ kevytt‰ sadetta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
    { "AM Light Rain",
    "vormittags leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "aamup‰iv‰ll‰ kevytt‰ sadetta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
  },
    { "Light Rain Late",
    "sp‰ter leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "kevytt‰ sadetta myˆh‰‰n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "ø‡ÿ“’‚!",
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
    "ø‡ÿ“’‚!",
  },
  { NULL }
  };

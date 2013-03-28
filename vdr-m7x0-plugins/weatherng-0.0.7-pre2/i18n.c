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
    "Prévision Météo-NG",
    "",// TODO
    "Sääennuste",
    "",// TODO
    "Previsión meteorológica",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Weather.com: Weather forecast",
    "Weather.com: 10 Tage Wettervorhersage",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Weather.com: Prévision météo",
    "",// TODO
    "Weather.com:in sääennusteet",
    "",// TODO
    "Weather.com: Previsión meteorológica",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Weather",
    "Wetter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Prévision",
    "",// TODO
    "Sää",
    "",// TODO
    "Previsión",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "ERROR : NO DATA",
    "FEHLER : KEINE DATEN VORHANDEN",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Virhe: Säätietoja ei saatavilla!",
    "",// TODO
    "Error: No hay datos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "ERROR : NO VALID DATA",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Error: No hay datos válidos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Do you have valid values in setup ?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "¿Hay datos válidos en la configuración?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "PRESS OK to download/update data/radarmaps",
    "TASTE OK druecken um Daten zu aktualisieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Paina 'OK' päivittääksesi sääennusteet ja tutkakartat",
    "",// TODO
    "Pulsa OK para descargar información",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Path to weatherdata is :",
    "Pfad zu den Wetterdaten ist :",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Säätietojen sijainti:",
    "",// TODO
    "La ruta a weatherdata es: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "Weather: ",
    "Wetter: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Prévision: ",
    "",// TODO
    "Säätila: ",
    "",// TODO
    "Previsión",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Low temperature: ",
    "Tiefsttemperatur: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp. basse: ",
    "",// TODO
    "Alin lämpötila: ",
    "",// TODO
    "Mínima",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "High temperature: ",
    "Höchsttemperatur: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Temp. haute: ",
    "",// TODO
    "Ylin lämpötila: ",
    "",// TODO
    "Máxima",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Weather for: ",
    "Wetter für: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Prévision pour: ",
    "",// TODO
    "Sää: ",
    "",// TODO
    "Previsión para: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "Dirección del viento:",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "Humitidy: ",
    "max. Luftfeuchte: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Taux d'humidité: ",
    "",// TODO
    "Ilmankosteus: ",
    "",// TODO
    "Humedad: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Precipitation: ",
    "Niederschlag: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Précipitation: ",
    "",// TODO
    "Sateen tod.näk.: ",
    "",// TODO
    "Precipitación: ",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
  },
  { "°C",
    "°C",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "°C",
    "",// TODO
    "°C",
    "",// TODO
    "°C",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "Posición imagen radar (izquierda)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "Posición imagen radar (arriba)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
  },
  { "Radarbitmap height",
    "Radarbild Höhe",
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
    "",
  },
  { "Use 256 colors for radarmaps",
    "Radarbilder mit 256 Farben",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Näytä tutkakuvat 256 värillä",
    "",// TODO
    "Usar 256 colores para radarmaps",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Use small fonts",
    "Kleine Schriftart nutzen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser petite police",
    "",// TODO
    "Käytä pieniä kirjasimia",
    "",// TODO
    "Usar fuentes pequeñas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "Clásico",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
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
    "",
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
    "",
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
    "",
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
    "",
  },
  { "Theme",
    "Farbschema",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Thème",
    "",// TODO
    "Teema",
    "",// TODO
    "Tema",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "Symbols inverted",
    "Invertierte Symbole",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Symboles inversés",
    "",// TODO
    "Näytä symbolit invertoituna",
    "",// TODO
    "Invertir símbolos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Corner rounded",
    "Abgerundete Ecken",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Käytä pyöristettyjä kulmia",
    "",// TODO
    "Bordes redondeados",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "DAY",
    "TAGSÜBER",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "JOUR",
    "",// TODO
    "PÄIVÄ",
    "",// TODO
    "DIA",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "NIGHT",
    "NACHTS",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "NUIT",
    "",// TODO
    "YÖ",
    "",// TODO
    "NOCHE",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
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
    "Miércoles",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
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
    "Sábado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
  },
  { "Sunny",
    "sonnig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ensoleillé",
    "",// TODO
    "aurinkoista",
    "",// TODO
    "Soleado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "Cloudy",
    "bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Nuageux",
    "",// TODO
    "pilvistä",
    "",// TODO
    "Nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Mostly Cloudy",
    "meist bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Très nuageux",
    "",// TODO
    "enimmäkseen pilvistä",
    "",// TODO
    "Parcialmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "PM T-Storms",
    "nachmittags Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages en soirée",
    "",// TODO
    "iltapäivällä ukkoskuuroja",
    "",// TODO
    "Tormenta eléctrica nocturna",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "AM T-Storms",
    "vormittags Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages matinaux",
    "",// TODO
    "aamupäivällä ukkoskuuroja",
    "",// TODO
    "Tormenta eléctrica matinal",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Light Rain",
    "leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie légère",
    "",// TODO
    "kevyttä sadetta",
    "",// TODO
    "Lluvia ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "AM Showers",
    "vormittags Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses matinales",
    "",// TODO
    "aamupäivällä sadekuuroja",
    "",// TODO
    "Chubascos matinales",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "PM Showers",
    "nachmittags Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses en soirée",
    "",// TODO
    "iltapäivällä sadekuuroja",
    "",// TODO
    "Chubascos nocturnos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Scattered Showers",
    "vereinzelt Schauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses isolées",
    "",// TODO
    "hajanaisia sadekuuroja",
    "",// TODO
    "Chubascos dispersos",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "Mostly Clear",
    "meist klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Généralement dégagé",
    "",// TODO
    "enimmäkseen selkeää",
    "",// TODO
    "Parcialmente despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
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
    "",
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
    "",
  },
  { "Clear",
    "heiter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Dégagé",
    "",// TODO
    "selkeää",
    "",// TODO
    "Despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Windy",
    "windig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vents léger",
    "",// TODO
    "tuulista",
    "",// TODO
    "Ventoso",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
  },
  { "M Cloudy",
    "meist bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Généralement nuageux",
    "",// TODO
    "enimmäkseen pilvistä",
    "",// TODO
    "Principalmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Partly Cloudy",
    "teilweise bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Partiellement nuageux",
    "",// TODO
    "osittain pilvistä",
    "",// TODO
    "Parcialmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "M Clear",
    "meist klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Généralement dégagé",
    "",// TODO
    "enimmäkseen selkeää",
    "",// TODO
    "Principalmente despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
  },
  { "Mostly Sunny",
    "meist sonnig",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Généralement ensoleillé",
    "",// TODO
    "enimmäkseen aurinkoista",
    "",// TODO
    "Principalmente soleado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Partly Cloudy",
    "teilweise bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Partiellement nuageux",
    "",// TODO
    "osittain pilvistä",
    "",// TODO
    "Parcialmente nublado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Showers Late",
    "Regen spät",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses en soirée",
    "",// TODO
    "sadetta myöhemmin",
    "",// TODO
    "Lluvia por la tarde",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "AM Clouds / PM Sun",
    "vormittags bewölkt / nachmittags Sonne",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Matin nuageux / Après-midi Soleil",
    "",// TODO
    "aamupäivällä pilvistä / iltapäivällä aurinkoista",
    "",// TODO
    "Mañana nubes / Tarde sol",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },

  { "Scattered T-Storms",
    "vereinzelt Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages isolés",
    "",// TODO
    "hajanaisia ukkosmyrskyjä",
    "",// TODO
    "Tormentas eléctricas dispersas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
  { "Overcast",
    "bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Couvert",
    "",// TODO
    "pilvistä",
    "",// TODO
    "Encapotado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
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
    "",
  },
    { "Storm",
    "Sturm",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Tempête",
    "",// TODO
    "myrskyä",
    "",// TODO
    "Tormenta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
  },
    { "Light Rain",
    "Leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie fine",
    "",// TODO
    "kevyttä sadetta",
    "",// TODO
    "Lluvia ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Light Snow",
    "Leichter Schnee",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Neige (leger)",
    "",// TODO
    "kevyttä lumisadetta",
    "",// TODO
    "Nieve ligera",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
  },
    { "Clear",
    "Klar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Dégagé",
    "",// TODO
    "selkeää",
    "",// TODO
    "Despejado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Light Drizzle",
    "wenig Nieselregen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Légère bruine",
    "",// TODO
    "kevyttä tihkua",
    "",// TODO
    "Llovizna",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Scattered",
    "vereinzelt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Isolé",
    "",// TODO
    "hajanaista",
    "",// TODO
    "Nubes dispersas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
    "",
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
    "",
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
    "",
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
    "caída",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Unlimited",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Sans limites",
    "",// TODO
    "määrittelemätöntä",
    "",// TODO
    "Ilimitado",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Showers in the Vicinity",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses dans les environs",
    "",// TODO
    "todennäköisesti sadekuuroja",
    "",// TODO
    "Chubascos en los alrededores",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Waning Crescent",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Diminution de croissant",
    "",// TODO
    "väistyvä täysikuu",
    "",// TODO
    "Nueva visible",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Waxing Crescent",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Augmentation de croissant",
    "",// TODO
    "alkava täysikuu",
    "",// TODO
    "Media luna creciente",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Waxing Gibbous",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "väistyvä uusikuu",
    "",// TODO
    "Gibada creciente",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Waning Gibbous",
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "väistyvä täysikuu",
    "",// TODO
    "Gibada menguante",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "T-Storms",
    "stürmisches Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages",
    "",// TODO
    "ukkosmyrskyjä",
    "",// TODO
    "Tormentas eléctricas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "T-Storms Early",
    "zunächst schweres Gewitter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Orages matinaux",
    "",// TODO
    "ukkosmyrskyjä aikaisin",
    "",// TODO
    "Tormentas eléctricas tempranas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Light Rain Early",
    "zunächst leichter Regen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pluie fine tôt le matin",
    "",// TODO
    "kevyttä sadetta aikaisin",
    "",// TODO
    "Lluvia fina de mañana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Showers Early",
    "zunächst Regenschauer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Averses matinales",
    "",// TODO
    "sadekuuroja aikaisin",
    "",// TODO
    "Chubascos de mañana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Cloudly Early",
    "zunächst bewölkt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pilvistä aikaisin",
    "",// TODO
    "Nublado de mañana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "PM Light Rain",
    "nachmittags leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "iltapäivällä kevyttä sadetta",
    "",// TODO
    "Lluvia ligera por la tarde",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "AM Light Rain",
    "vormittags leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "aamupäivällä kevyttä sadetta",
    "",// TODO
    "Lluvia ligera por la mañana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
  },
    { "Light Rain Late",
    "später leicht regnerisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "kevyttä sadetta myöhään",
    "",// TODO
    "Lluvia ligera por la tarde",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
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
    "",
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
      "Estación meteorológica 1",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",
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
      "Estación meteorológica 2",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",
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
      "Estación meteorológica 3",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",
  },
      { "Please check IP/Gate configuration",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "Comprueba la configuración de red",
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",// TODO
      "",
  },
  { NULL }
  };

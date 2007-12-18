/*
 * i18n.c: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Soppalusikka skin", // English
    "Soppalusikka Oberfläche", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Skin Soppalusikka", // Français
    "", // Norsk
    "Soppalusikka-ulkoasu", // suomi (Finnish)
    "Skin Soppalusikka", // Polski
    "Piel Soppalusikka", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Soppalusikka-temat", // Svenska
    "", // Românã
    "Soppalussika-skin", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Soppalusikka kest", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Soppalusikka", // English
    "Soppalusikka", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Soppalusikka", // Français
    "", // Norsk
    "Soppalusikka", // suomi (Finnish)
    "Soppalusikka", // Polski
    "Soppalusikka", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Soppalusikka", // Svenska
    "", // Românã
    "Soppalusikka", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Soppalusikka", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "min", // English
    "min", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "min", // Français
    "", // Norsk
    "min", // suomi (Finnish)
    "min", // Polski
    "min", // Español
    "", // ÅëëçíéêÜ (Greek)
    "min", // Svenska
    "", // Românã
    "perc", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "min", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Stereo", // English
    "Stereo", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Stéréo", // Français
    "", // Norsk
    "Stereo", // suomi (Finnish)
    "Stereo", // Polski
    "Estereo", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Stereo", // Svenska
    "", // Românã
    "Sztereó", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Stereo", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Left", // English
    "Links", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Gauche", // Français
    "", // Norsk
    "Vasen", // suomi (Finnish)
    "W lewo", // Polski
    "Izquierda", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Vänster", // Svenska
    "", // Românã
    "Bal", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Vasak", // Eesti
    "", // Danske
    "", // Èesky (Czech)
  },
  { "Right", // English
    "Rechts", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Droite", // Français
    "", // Norsk
    "Oikea", // suomi (Finnish)
    "W prawo", // Polski
    "Derecha", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Höger", // Svenska
    "", // Românã
    "Jobb", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Parem", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Mute", // English
    "Stumm", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Muet", // Français
    "", // Norsk
    "Mykistetty", // suomi (Finnish)
    "Wycisz", // Polski
    "Silenciar", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Dämpa", // Svenska
    "", // Românã
    "Némít", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Hääletu", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Volume", // English
    "Lautstärke", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Volume", // Français
    "", // Norsk
    "Äänenvoimakkuus", // suomi (Finnish)
    "G³o¶no¶æ", // Polski
    "Volumen", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Volym", // Svenska
    "", // Românã
    "Hangerõ", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Helitugevus", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Show channel logos", // English
    "Kanal-Logos anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Afficher logo des chaînes", // Français
    "", // Norsk
    "Näytä kanavalogot", // suomi (Finnish)
    "Pokazuj logo kana³u", // Polski
    "Mostrar los logos de los canales", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Visa kanallogotyper", // Svenska
    "", // Românã
    "Csatornalogók megjelenítése", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Kanalilogo näitamine", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Identify channel by", // English
    "Kanal-Identifikation durch", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Identifier chaîne par", // Français
    "", // Norsk
    "Tunnista kanava", // suomi (Finnish)
    "Rozpoznaj kana³ po", // Polski
    "Identificar el canal por...", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Identifiera kanal med", // Svenska
    "", // Românã
    "Csatornák azonosítása", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Kanali tuvastamise meetod", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "name", // English
    "Name", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Nom", // Français
    "", // Norsk
    "nimestä", // suomi (Finnish)
    "nazwa", // Polski
    "Nombre", // Español
    "", // ÅëëçíéêÜ (Greek)
    "namn", // Svenska
    "", // Românã
    "név szerint", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "nimi", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "data", // English
    "ID", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Donnée", // Français
    "", // Norsk
    "tiedoista", // suomi (Finnish)
    "parametry", // Polski
    "Datos", // Español
    "", // ÅëëçíéêÜ (Greek)
    "data", // Svenska
    "", // Românã
    "paraméterek szerint", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "andmed", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Channel logo cache size", // English
    "Größe des Logo-Caches", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Taille du cache des logo", // Français
    "", // Norsk
    "Välimuistin koko kanavalogoille", // suomi (Finnish)
    "Rozmiar cache dla logo kana³u", // Polski
    "Tamaño de la cache de los logos", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Cachestorlek för logotyper", // Svenska
    "", // Românã
    "Csatorna logó cache méret", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Kanalilogo vahemälu suurus", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Flush cache", // English
    "Cache leeren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Supprimer cache", // Français
    "", // Norsk
    "Tyhjennä", // suomi (Finnish)
    "Opró¿nij cache", // Polski
    "Limpiar la cache", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Töm cachen", // Svenska
    "", // Românã
    "Cache ürítés", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Tühjenda", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Flushing channel logo cache...", // English
    "Logo-Cache wird geleert...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Suppression du cache...", // Français
    "", // Norsk
    "Tyhjennetään välimuistia...", // suomi (Finnish)
    "Opró¿niam cache logo kana³u...", // Polski
    "Limpiando la cache...", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Tömmer cachen...", // Svenska
    "", // Românã
    "Csatornalogó cache ürítése...", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Kanalilogo vahemälu tühjendamine...", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Languages", // English
    "Sprachen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Langues", // Français
    "", // Norsk
    "Kielet", // suomi (Finnish)
    "Jêzyki", // Polski
    "Lenguajes", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Språk", // Svenska
    "", // Românã
    "Nyelvek", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Keeled", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Auxiliary information", // English
    "Zusatzinfo", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Information auxiliare", // Français
    "", // Norsk
    "Lisätiedot", // suomi (Finnish)
    "Informacje pomocnicze", // Polski
    "Información auxiliar", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Extrainformation", // Svenska
    "", // Românã
    "További információk", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Lisainfo", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Show auxiliary information", // English
    "Zusatzinfo anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Afficher information auxiliaire", // Français
    "", // Norsk
    "Näytä lisätiedot", // suomi (Finnish)
    "Pokazuj informacje pomocnicze", // Polski
    "Mostrar información auxiliar", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Visa extrainformation", // Svenska
    "", // Românã
    "További információk megjelenítése", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Lisainfo näitamine", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Show progressbar", // English
    "Fortschrittbalken anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Afficher barre de progression", // Français
    "", // Norsk
    "Näytä aikajana", // suomi (Finnish)
    "Pokazuj pasek postêpu", // Polski
    "Mostrar barra de progreso", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Visa framsteg", // Svenska
    "", // Românã
    "Lejátszósáv megjelenítése", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Edenemisriba näitamine", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Show symbols", // English
    "Symbole anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Afficher symbole", // Français
    "", // Norsk
    "Näytä symbolit", // suomi (Finnish)
    "Pokazuj symbole", // Polski
    "Mostrar símbolos", // Español
    "", // ÅëëçíéêÜ (Greek)
    "Visa symboler", // Svenska
    "", // Românã
    "Ikonok megjelenítése", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "Sümbolite näitamine", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Show VPS in channel info", // English
    "VPS anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Afficher VPS dans l'info. de la chaîne", // Français
    "", // Norsk
    "Näytä VPS kanavatiedossa", // suomi (Finnish)
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Românã
    "VPS információk megjelenítése", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Use single area (8bpp)", // English
    "Ein Bildbereich benutzen (8bpp)", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Käytä yksittäistä kuva-aluetta (8bpp)", // suomi (Finnish)
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Românã
    "Egyetlen terület használata (8bpp)", // Magyar
    "", // Catalàe
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { NULL }
};

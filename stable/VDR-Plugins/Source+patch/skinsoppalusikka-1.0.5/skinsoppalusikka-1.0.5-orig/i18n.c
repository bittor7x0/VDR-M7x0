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
    "Soppalusikka Oberfl�che", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Skin Soppalusikka", // Fran�ais
    "", // Norsk
    "Soppalusikka-ulkoasu", // suomi (Finnish)
    "Skin Soppalusikka", // Polski
    "Piel Soppalusikka", // Espa�ol
    "", // �������� (Greek)
    "Soppalusikka-temat", // Svenska
    "", // Rom�n�
    "Soppalussika-skin", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Soppalusikka kest", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Soppalusikka", // English
    "Soppalusikka", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Soppalusikka", // Fran�ais
    "", // Norsk
    "Soppalusikka", // suomi (Finnish)
    "Soppalusikka", // Polski
    "Soppalusikka", // Espa�ol
    "", // �������� (Greek)
    "Soppalusikka", // Svenska
    "", // Rom�n�
    "Soppalusikka", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Soppalusikka", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "min", // English
    "min", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "min", // Fran�ais
    "", // Norsk
    "min", // suomi (Finnish)
    "min", // Polski
    "min", // Espa�ol
    "", // �������� (Greek)
    "min", // Svenska
    "", // Rom�n�
    "perc", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "min", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Stereo", // English
    "Stereo", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "St�r�o", // Fran�ais
    "", // Norsk
    "Stereo", // suomi (Finnish)
    "Stereo", // Polski
    "Estereo", // Espa�ol
    "", // �������� (Greek)
    "Stereo", // Svenska
    "", // Rom�n�
    "Sztere�", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Stereo", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Left", // English
    "Links", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Gauche", // Fran�ais
    "", // Norsk
    "Vasen", // suomi (Finnish)
    "W lewo", // Polski
    "Izquierda", // Espa�ol
    "", // �������� (Greek)
    "V�nster", // Svenska
    "", // Rom�n�
    "Bal", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Vasak", // Eesti
    "", // Danske
    "", // �esky (Czech)
  },
  { "Right", // English
    "Rechts", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Droite", // Fran�ais
    "", // Norsk
    "Oikea", // suomi (Finnish)
    "W prawo", // Polski
    "Derecha", // Espa�ol
    "", // �������� (Greek)
    "H�ger", // Svenska
    "", // Rom�n�
    "Jobb", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Parem", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Mute", // English
    "Stumm", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Muet", // Fran�ais
    "", // Norsk
    "Mykistetty", // suomi (Finnish)
    "Wycisz", // Polski
    "Silenciar", // Espa�ol
    "", // �������� (Greek)
    "D�mpa", // Svenska
    "", // Rom�n�
    "N�m�t", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "H��letu", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Volume", // English
    "Lautst�rke", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Volume", // Fran�ais
    "", // Norsk
    "��nenvoimakkuus", // suomi (Finnish)
    "G�o�no��", // Polski
    "Volumen", // Espa�ol
    "", // �������� (Greek)
    "Volym", // Svenska
    "", // Rom�n�
    "Hanger�", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Helitugevus", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Show channel logos", // English
    "Kanal-Logos anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Afficher logo des cha�nes", // Fran�ais
    "", // Norsk
    "N�yt� kanavalogot", // suomi (Finnish)
    "Pokazuj logo kana�u", // Polski
    "Mostrar los logos de los canales", // Espa�ol
    "", // �������� (Greek)
    "Visa kanallogotyper", // Svenska
    "", // Rom�n�
    "Csatornalog�k megjelen�t�se", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Kanalilogo n�itamine", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Identify channel by", // English
    "Kanal-Identifikation durch", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Identifier cha�ne par", // Fran�ais
    "", // Norsk
    "Tunnista kanava", // suomi (Finnish)
    "Rozpoznaj kana� po", // Polski
    "Identificar el canal por...", // Espa�ol
    "", // �������� (Greek)
    "Identifiera kanal med", // Svenska
    "", // Rom�n�
    "Csatorn�k azonos�t�sa", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Kanali tuvastamise meetod", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "name", // English
    "Name", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Nom", // Fran�ais
    "", // Norsk
    "nimest�", // suomi (Finnish)
    "nazwa", // Polski
    "Nombre", // Espa�ol
    "", // �������� (Greek)
    "namn", // Svenska
    "", // Rom�n�
    "n�v szerint", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "nimi", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "data", // English
    "ID", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Donn�e", // Fran�ais
    "", // Norsk
    "tiedoista", // suomi (Finnish)
    "parametry", // Polski
    "Datos", // Espa�ol
    "", // �������� (Greek)
    "data", // Svenska
    "", // Rom�n�
    "param�terek szerint", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "andmed", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channel logo cache size", // English
    "Gr��e des Logo-Caches", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Taille du cache des logo", // Fran�ais
    "", // Norsk
    "V�limuistin koko kanavalogoille", // suomi (Finnish)
    "Rozmiar cache dla logo kana�u", // Polski
    "Tama�o de la cache de los logos", // Espa�ol
    "", // �������� (Greek)
    "Cachestorlek f�r logotyper", // Svenska
    "", // Rom�n�
    "Csatorna log� cache m�ret", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Kanalilogo vahem�lu suurus", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Flush cache", // English
    "Cache leeren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Supprimer cache", // Fran�ais
    "", // Norsk
    "Tyhjenn�", // suomi (Finnish)
    "Opr�nij cache", // Polski
    "Limpiar la cache", // Espa�ol
    "", // �������� (Greek)
    "T�m cachen", // Svenska
    "", // Rom�n�
    "Cache �r�t�s", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "T�hjenda", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Flushing channel logo cache...", // English
    "Logo-Cache wird geleert...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Suppression du cache...", // Fran�ais
    "", // Norsk
    "Tyhjennet��n v�limuistia...", // suomi (Finnish)
    "Opr�niam cache logo kana�u...", // Polski
    "Limpiando la cache...", // Espa�ol
    "", // �������� (Greek)
    "T�mmer cachen...", // Svenska
    "", // Rom�n�
    "Csatornalog� cache �r�t�se...", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Kanalilogo vahem�lu t�hjendamine...", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Languages", // English
    "Sprachen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Langues", // Fran�ais
    "", // Norsk
    "Kielet", // suomi (Finnish)
    "J�zyki", // Polski
    "Lenguajes", // Espa�ol
    "", // �������� (Greek)
    "Spr�k", // Svenska
    "", // Rom�n�
    "Nyelvek", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Keeled", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Auxiliary information", // English
    "Zusatzinfo", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Information auxiliare", // Fran�ais
    "", // Norsk
    "Lis�tiedot", // suomi (Finnish)
    "Informacje pomocnicze", // Polski
    "Informaci�n auxiliar", // Espa�ol
    "", // �������� (Greek)
    "Extrainformation", // Svenska
    "", // Rom�n�
    "Tov�bbi inform�ci�k", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Lisainfo", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Show auxiliary information", // English
    "Zusatzinfo anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Afficher information auxiliaire", // Fran�ais
    "", // Norsk
    "N�yt� lis�tiedot", // suomi (Finnish)
    "Pokazuj informacje pomocnicze", // Polski
    "Mostrar informaci�n auxiliar", // Espa�ol
    "", // �������� (Greek)
    "Visa extrainformation", // Svenska
    "", // Rom�n�
    "Tov�bbi inform�ci�k megjelen�t�se", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Lisainfo n�itamine", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Show progressbar", // English
    "Fortschrittbalken anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Afficher barre de progression", // Fran�ais
    "", // Norsk
    "N�yt� aikajana", // suomi (Finnish)
    "Pokazuj pasek post�pu", // Polski
    "Mostrar barra de progreso", // Espa�ol
    "", // �������� (Greek)
    "Visa framsteg", // Svenska
    "", // Rom�n�
    "Lej�tsz�s�v megjelen�t�se", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "Edenemisriba n�itamine", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Show symbols", // English
    "Symbole anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Afficher symbole", // Fran�ais
    "", // Norsk
    "N�yt� symbolit", // suomi (Finnish)
    "Pokazuj symbole", // Polski
    "Mostrar s�mbolos", // Espa�ol
    "", // �������� (Greek)
    "Visa symboler", // Svenska
    "", // Rom�n�
    "Ikonok megjelen�t�se", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "S�mbolite n�itamine", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Show VPS in channel info", // English
    "VPS anzeigen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Afficher VPS dans l'info. de la cha�ne", // Fran�ais
    "", // Norsk
    "N�yt� VPS kanavatiedossa", // suomi (Finnish)
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Rom�n�
    "VPS inform�ci�k megjelen�t�se", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Use single area (8bpp)", // English
    "Ein Bildbereich benutzen (8bpp)", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "K�yt� yksitt�ist� kuva-aluetta (8bpp)", // suomi (Finnish)
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Rom�n�
    "Egyetlen ter�let haszn�lata (8bpp)", // Magyar
    "", // Catal�e
    "", // ������� (Russian)
    "", // Hrvatski
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { NULL }
};

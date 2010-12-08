/*
 * i18n.c: Internationalization
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: i18n.c 277 2008-09-09 11:57:00Z andreas $
 *
 * Translations provided by:
 *
 * Slovenian   Miha Setina <mihasetina@softhome.net>, Matjaz Thaler <matjaz.thaler@guest.arnes.si>
 * Italian     Alberto Carraro <bertocar@tin.it>, Antonio Ospite <ospite@studenti.unina.it>, Sean Carlos <seanc@libero.it>, Nino Gerbino <ngerb@interfree.it>
 * Dutch       Arnold Niessen <niessen@iae.nl> <arnold.niessen@philips.com>, Hans Dingemans <hans.dingemans@tacticalops.nl>, Maarten Wisse <Maarten.Wisse@urz.uni-hd.de>
 * Portuguese  Paulo Lopes <pmml@netvita.pt>
 * French      Jean-Claude Repetto <jc@repetto.org>, Olivier Jacques <jacquesolivier@hotmail.com>, Gregoire Favre <greg@magma.unil.ch>, Nicolas Huillard <nhuillard@e-dition.fr>, Pierre Briec <pbriec@free.fr>
 * Norwegian   J�rgen Tvedt <pjtvedt@online.no>, Truls Slevigen <truls@slevigen.no>
 * Finnish     Hannu Savolainen <hannu@opensound.com>, Jaakko Hyv�tti <jaakko@hyvatti.iki.fi>, Niko Tarnanen <niko.tarnanen@hut.fi>, Rolf Ahrenberg <rahrenbe@cc.hut.fi>
 * Polish      Michael Rakowski <mrak@gmx.de>, Jaroslaw Swierczynski <swiergot@gmail.com>
 * Spanish     Ruben Nunez Francisco <ruben.nunez@tang-it.com>, Jesus Bravo Alvarez <jba@pobox.com>
 * Greek       Dimitrios Dimitrakos <mail@dimitrios.de>
 * Swedish     Tomas Prybil <tomas@prybil.se>, Jan Ekholm <chakie@infa.abo.fi>
 * Romanian    Paul Lacatus <paul@campina.iiruc.ro>, Lucian Muresan <lucianm@users.sourceforge.net>
 * Hungarian   Istvan & Silvia Koenigsberger <istvnko@hotmail.com>, Guido Josten <guido.josten@t-online.de>
 * Catalanian  Marc Rovira Vall <tm05462@salleURL.edu>, Ramon Roca <ramon.roca@xcombo.com>, Jordi Vil� <jvila@tinet.org>
 * Russian     Vyacheslav Dikonov <sdiconov@mail.ru>, Oleg Roitburd <oleg@roitburd.de>
 * Croatian    Drazen Dupor <drazen.dupor@dupor.com>, Dino Ravnic <dino.ravnic@fer.hr>
 * Estonian    Arthur Konovalov <kasjas@hot.ee>
 * Danish      Mogens Elneff <mogens@elneff.dk>
 * Czech       Vladim�r B�rta <vladimir.barta@k2atmitec.cz>
 *
 */

/*
 * How to add a new language:
 *
 * 1. Announce your translation action on the VDR mailing
 *    list to avoid duplicate work.
 * 2. Increase the value of 'I18nNumLanguages' in 'i18n.h'.
 * 3. Insert a new line in every member of the 'Phrases[]' array,
 *    containing the translated text for the new language.
 *    For example, assuming you want to add the Italian language,
 *
 *       { "English",
 *         "Deutsch",
 *       },
 *
 *    would become
 *
 *       { "English",
 *         "Deutsch",
 *         "Italiano",
 *       },
 *
 *    and so on. Append your language after the last existing language
 *    and write the name of your language in your language (not in English,
 *    which means that it should be 'Italiano', not 'Italian').
 *    Note that only the characters defined in 'fontosd.c' will
 *    be available!
 * 4. If your language requires a character set other than the default iso8859-1
 *    you need to make the appropriate entry in the second phrase below, and
 *    also create the necessary font files in case they are not yet part of
 *    the VDR source (see how this is done for iso8859-7, for instance). Don't
 *    forget to adjust the files font.h and font.c accordingly.
 * 5. Compile VDR and test the new language by switching to it
 *    in the "Setup/OSD" menu.
 * 6. Send the modified files to <kls@cadsoft.de> to have
 *    it included in the next version of VDR.
 *
 * In case an English phrase is used in more than one context (and might need
 * different translations in other languages) it can be preceded with an
 * arbitrary string to describe its context, separated from the actual phrase
 * by a '$' character (see for instance "Button$Stop" vs. "Stop").
 * Of course this means that no English phrase may contain the '$' character!
 * If this should ever become necessary, the existing '$' would have to be
 * replaced with something different...
 */

#include "i18n.h"
#include <ctype.h>
#include "config.h"
#include "tools.h"

const tI18nPhrase Phrases[] = {
  // The name of the language (this MUST be the first phrase!):
  { "English",
    "Deutsch",
    "Slovenski",
    "Italiano",
    "Nederlands",
    "Portugu�s",
    "Fran�ais",
    "Norsk",
    "suomi", // Finnish (this is not a typo - it's really lowercase!)
    "Polski",
    "Espa�ol",
    "��������", // Greek
    "Svenska",
    "Rom�n�",
    "Magyar",
    "Catal�",
    "�������", // Russian
    "Hrvatski",
    "Eesti",
    "Dansk",
    "�esky", //Czech
  },
  // The character set needed for this language (this MUST be the second phrase!):
  { "iso8859-15",
    "iso8859-15",
    "iso8859-2",
    "iso8859-15",
    "iso8859-15",
    "iso8859-1",
    "iso8859-1",
    "iso8859-1",
    "iso8859-15",
    "iso8859-2",
    "iso8859-15",
    "iso8859-7",
    "iso8859-1",
    "iso8859-2",
    "iso8859-2",
    "iso8859-1",
    "iso8859-5",
    "iso8859-2",
    "iso8859-13",
    "iso8859-15",
    "iso8859-2",
  },
  // The 3-letter names of the language (this MUST be the third phrase!):
  { "eng,dos",
    "deu,ger",
    "slv,slo",
    "ita",
    "dut,nla,nld",
    "por",
    "fra,fre",
    "nor",
    "fin,smi",
    "pol",
    "esl,spa",
    "ell,gre",
    "sve,swe",
    "rom,rum",
    "hun",
    "cat,cln",
    "rus",
    "hrv",
    "est",
    "dan",
    "cze,ces",
  },
  // Menu titles:
  { "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
    "VDR",
  },
  { "Schedule",
    "Programm",
    "Program",
    "Programmi",
    "Gids",
    "Programa��o",
    "Programmes",
    "Programmer",
    "Ohjelmisto",
    "Program",
    "Gu�a de programaci�n",
    "���������",
    "Program",
    "Program (EPG)",
    "Program",
    "Guia de Programaci�",
    "�������",
    "Raspored",
    "Kava",
    "Programoversigt",
    "Program (EPG)",
  },
  { "Channels",
    "Kan�le",
    "Kanali",
    "Canali",
    "Kanalen",
    "Canais",
    "Cha�nes",
    "Kanaler",
    "Kanavat",
    "Kana�y",
    "Canales",
    "�������",
    "Kanaler",
    "Canale",
    "Csatorn�k",
    "Canals",
    "������",
    "Programi",
    "Kanalid",
    "Kanaler",
    "Kan�ly",
  },
  { "Timers",
    "Timer",
    "Termini",
    "Timer",
    "Timers",
    "Timers",
    "Programmation",
    "Timere",
    "Ajastimet",
    "Timery",
    "Programaciones",
    "��������������������",
    "Timers",
    "Timer-e",
    "Id�z�t�",
    "Temporitzadors",
    "�������",
    "Termini",
    "Taimer",
    "Timere",
    "�asova�e",
  },
  { "Recordings",
    "Aufzeichnungen",
    "Posnetki",
    "Registrazioni",
    "Opnames",
    "Grava��es",
    "Enregistrements",
    "Opptak",
    "Tallenteet",
    "Nagrania",
    "Grabaciones",
    "��������",
    "Inspelningar",
    "�nregistr�ri",
    "Felvett ad�sok",
    "Veure programes gravats",
    "������",
    "Snimke",
    "Salvestused",
    "Optagelser",
    "Nahr�vky",
  },
  { "Recording info",
    "Aufzeichnung",
    "Podatki o snemanju",
    "Informazioni registrazione",
    "Opname info",
    "",// TODO
    "Infos sur l'enregistrement",
    "",// TODO
    "Tallenteen tiedot",
    "Informacje o nagraniu",
    "Informaci�n de grabaci�n",
    "����������� E�������",
    "Inspelning",
    "Detaliile �nregistr�rii",
    "Felv�tel",
    "",// TODO
    "���� � ������",
    "",// TODO
    "Salvestuse info",
    "Optagelses info",
    "Detail nahr�vky",
  },
  { "Setup",
    "Einstellungen",
    "Nastavitve",
    "Opzioni",
    "Instellingen",
    "Configurar",
    "Configuration",
    "Konfigurasjon",
    "Asetukset",
    "Ustawienia",
    "Configuraci�n VDR",
    "���������",
    "Inst�llningar",
    "Configura�ie",
    "Be�ll�t�sok",
    "Configuraci�",
    "���������",
    "Konfiguracija",
    "S�tted",
    "Indstillinger",
    "Nastaven�",
  },
  { "Commands",
    "Befehle",
    "Ukazi",
    "Comandi",
    "Commando's",
    "Comandos",
    "Commandes",
    "Kommandoer",
    "Komennot",
    "Polecenia",
    "�rdenes",
    "�������",
    "Kommandon",
    "Comenzi",
    "Parancsok",
    "Ordres",
    "�������",
    "Naredbe",
    "K�sud",
    "Kommandoer",
    "P��kazy",
  },
  { "Recording commands",
    "Befehle f�r Aufzeichnungen",
    "Ukazi za snemanje",
    "Comandi di registrazione",
    "Opname commando's",
    "",// TODO
    "Commandes d'enregistrement",
    "",// TODO
    "Tallennekomennot",
    "Polecenia nagra�",
    "�rdenes de grabaci�n",
    "������� ��� ��������",
    "Inspelningskommandon",
    "Comenzi pentru �nregistr�ri",
    "Parancsok a felvett filmekhez",
    "Ordres de gravaci�",
    "������� ������",
    "Naredbe za snimanje",
    "Salvestuse k�sud",
    "Optage kommandoer",
    "P��kazy nahr�v�n�",
  },
  { "Edit channel",
    "Kanal editieren",
    "Uredi kanal",
    "Modifica canale",
    "Kanaal aanpassen",
    "Modificar canal",
    "Modifier une cha�ne",
    "Editer kanal",
    "Muokkaa kanavaa",
    "Edycja kana�u",
    "Modificar canal",
    "����������� ��������",
    "�ndra kanal",
    "Modificare canal",
    "Csatorn�k be�ll�t�sa",
    "Editar canal",
    "�������������� ������",
    "Ure�ivanje programa",
    "Kanali muutmine",
    "Rediger kanal",
    "Editace kan�lu",
  },
  { "Edit timer",
    "Timer editieren",
    "Uredi termin",
    "Modifica timer",
    "Timer veranderen",
    "Modificar timer",
    "Changer la programmation",
    "Editer timer",
    "Muokkaa ajastinta",
    "Edycja timera",
    "Modificar programaci�n",
    "����������� ��������������������",
    "�ndra timer",
    "Modificare timer",
    "Id�z�t� megv�ltoztat�sa",
    "Editar temporitzador",
    "��������� �������",
    "Ure�ivanje termina",
    "Taimeri muutmine",
    "Rediger timer",
    "Editace �asova�e",
  },
  { "Event",
    "Sendung",
    "Oddaja",
    "Eventi",
    "Uitzending",
    "Evento",
    "Ev�nement",
    "Hendelse",
    "Tapahtuma",
    "Audycja",
    "Evento",
    "�������",
    "S�ndning",
    "Emisiune",
    "Ad�s",
    "Esdeveniment",
    "��������",
    "Emisija",
    "S�ndmus",
    "Udsendelse",
    "Ud�lost",
  },
  { "Info",
    "Info",
    "Info",
    "Informazioni",
    "Info",
    "",//TODO
    "Info",
    "",//TODO
    "Tiedot",
    "Info",
    "Info",
    "�����������",
    "Info",
    "Info",
    "Inform�ci�",
    "",//TODO
    "����",
    "Info",
    "Info",
    "Info",
    "Info",
  },
  { "Schedule - %s",
    "Programm - %s",
    "Program - %s",
    "Programma - %s",
    "Programma - %s",
    "Programa - %s",
    "Programmes - %s",
    "Program Guide - %s",
    "Ohjelmisto - %s",
    "Program - %s",
    "Gu�a de programaci�n - %s",
    "��������� - %s",
    "Program - %s",
    "Programul canalului %s",
    "Program - %s",
    "Guia de Programaci� - %s",
    "��������� - %s",
    "Raspored - %s",
    "Ajakava - %s",
    "Program - %s",
    "Program - %s",
  },
  { "This event - %s",
    "Diese Sendung - %s",
    "Ta oddaja - %s",
    "Questo evento - %s",
    "",//TODO
    "",//TODO
    "Cet �v�nement",
    "",//TODO
    "T�m� tapahtuma - %s",
    "Ta audycja - %s",
    "Este evento - %s",
    "",//TODO
    "Den h�r s�ndningen - %s",
    "Aceast� emisiune - %s",
    "Ez az ad�s - %s",
    "",//TODO
    "��� �������� - %s",
    "",//TODO
    "See s�ndmus - %s",
    "Denne udsendelse - %s",
    "Tato ud�lost - %s",
  },
  { "This event - all channels",
    "Diese Sendung - alle Kan�le",
    "Ta oddaja - vsi kanali",
    "Questo evento - tutti i canali",
    "",//TODO
    "",//TODO
    "Cet �v�nement - toutes les cha�nes",
    "",//TODO
    "T�m� tapahtuma - kaikki kanavat",
    "Ta audycja - wszystkie kana�y",
    "Este evento - todos los canales",
    "",//TODO
    "Den h�r s�ndningen - alla kanaler",
    "Aceast� emisiune - toate canalele",
    "Ez az ad�s - �sszes ad�",
    "",//TODO
    "��� �������� - ��� ������",
    "",//TODO
    "See s�ndmus - k�ik kanalid",
    "Denne udsendelse - alle kanaler",
    "Tato ud�lost - v�echny kan�ly",
  },
  { "All events - all channels",
    "Alle Sendungen - alle Kan�le",
    "Vse oddaje - vsi kanali",
    "Tutti gli eventi - tutti i canali",
    "",//TODO
    "",//TODO
    "Tous les �v�nements - toutes cha�nes",
    "",//TODO
    "Kaikki tapahtumat - kaikki kanavat",
    "Wszystkie audycje - wszystkie kana�y",
    "Todos los eventos - todos los canales",
    "",//TODO
    "Alla tillf�llen - alla kanaler",
    "Toate emisiunile - toate canalele",
    "Az �sszes ad�s - az �sszes ad�",
    "",//TODO
    "��� �������� - ��� ������",
    "",//TODO
    "K�ik s�ndmused - k�ik kanalid",
    "Alle udsendelser - alle kanaler",
    "V�echny ud�lost - v�echny kan�ly",
  },
  { "What's on now?",
    "Was l�uft jetzt?",
    "Kaj je na sporedu?",
    "In programmazione adesso",
    "Wat is er nu?",
    "Programa actual?",
    "Programmes en cours",
    "Hvilket program sendes n�?",
    "Menossa nyt",
    "Program bie��cy",
    "�Qu� hay ahora?",
    "������ ���������",
    "Vad s�nds nu?",
    "Ce emisiuni sunt acum?",
    "Mi megy most?",
    "Qu� fan ara?",
    "������ � �����:",
    "�to je trenutno na programu?",
    "Hetkel eetris",
    "Hvad vises nu?",
    "Aktu�ln� program",
  },
  { "What's on next?",
    "Was l�uft als n�chstes?",
    "Kaj sledi?",
    "Prossimi programmi",
    "Wat komt er hierna?",
    "Pr�ximo Programa?",
    "Prochains programmes",
    "Hvilket program er neste?",
    "Tulossa seuraavaksi",
    "Nast�pne audycje",
    "�Qu� hay despu�s?",
    "������� ���������",
    "Vilket �r n�sta program?",
    "Ce emisiuni urmeaz�?",
    "Mi k�vetkezik?",
    "Qu� fan despr�s?",
    "����� � ���������:",
    "�to slijedi?",
    "J�rgmisena eetris",
    "Hvad vises som det n�ste?",
    "N�sleduj�c� program",
  },
  // Button texts (should not be more than 10 characters!):
  { "Button$Edit",
    "Editieren",
    "Uredi",
    "Modifica",
    "Verander",
    "Modificar",
    "Modifier",
    "Editer",
    "Muokkaa",
    "Edytuj",
    "Modificar",
    "����������",
    "�ndra",
    "Modific�",
    "Be�ll�tani",
    "Editar",
    "�������������",
    "Promijeni",
    "Muuda",
    "Rediger",
    "Editace",
  },
  { "Button$New",
    "Neu",
    "Novo",
    "Nuovo",
    "Nieuw",
    "Novo",
    "Nouveau",
    "Ny",
    "Uusi",
    "Nowy",
    "Nuevo",
    "N��",
    "Ny",
    "Nou",
    "�j",
    "Nou",
    "��������",
    "Novi",
    "Uus",
    "Ny",
    "Nov�",
  },
  { "Button$Delete",
    "L�schen",
    "Izbri�i",
    "Cancella",
    "Verwijder",
    "Apagar",
    "Supprimer",
    "Slett",
    "Poista",
    "Usu�",
    "Borrar",
    "��������",
    "Ta bort",
    "�terge",
    "T�r�lni",
    "Esborrar",
    "�������",
    "Obri�i",
    "Kustuta",
    "Slet",
    "Smazat",
  },
  { "Button$Mark",
    "Markieren",
    "Ozna�i",
    "Marca",
    "Verplaats",
    "Marcar",
    "Marquer",
    "Marker",
    "Siirr�",
    "Zaznacz",
    "Marcar",
    "�������",
    "M�rk",
    "Marcheaz�",
    "Megjel�lni",
    "Marcar",
    "�����������",
    "Ozna�i",
    "Siirda",
    "Mark�r",
    "Ozna�it",
  },
  { "Button$On/Off",
    "Ein/Aus",
    "Vklop/Izklop",
    "On/Off",
    "Aan/Uit",
    "On/Off",
    "Marche/Arr",
    "Av/P�",
    "P��ll�/Pois",
    "W�/Wy�",
    "Activo/Desact.",
    "A����/K������",
    "P�/Av",
    "Act./Inact.",
    "Be/Ki",
    "On/Off",
    "���/����",
    "De-/Aktiviraj",
    "On/Off",
    "Til/Fra",
    "Zap./Vyp.",
  },
  { "Button$Timer",
    "Timer",
    "Termini",
    "Timer",
    "",// TODO
    "",// TODO
    "Programmation",
    "",// TODO
    "Ajastin",
    "Timer",
    "Programar",
    "",// TODO
    "Timer",
    "Timer",
    "Id�z�t�",
    "",// TODO
    "������",
    "",// TODO
    "Taimer",
    "Timer",
    "�asova�",
  },
  { "Button$Record",
    "Aufnehmen",
    "Posnemi",
    "Registra",
    "Opnemen",
    "Gravar",
    "Enregistre",
    "Ta opp",
    "Tallenna",
    "Nagraj",
    "Grabar",
    "�������",
    "Inspelning",
    "�nregistr.",
    "Felvenni",
    "Gravar",
    "������",
    "Snimi",
    "Salvesta",
    "Optag",
    "Nahr�t",
  },
  { "Button$Info",
    "Info",
    "Info",
    "Informazioni",
    "Info",
    "",//TODO
    "Info",
    "",//TODO
    "Tiedot",
    "Info",
    "Info",
    "�����������",
    "Info",
    "Info",
    "Inform�ci�",
    "",//TODO
    "����",
    "Info",
    "Info",
    "Info",
    "Info",
  },
  { "Button$Play",
    "Wiedergabe",
    "Predvajaj",
    "Riproduci",
    "Afspelen",
    "Play",
    "Lire",
    "Spill av",
    "Toista",
    "Odtw�rz",
    "Reproducir",
    "�����������",
    "Spela upp",
    "Redare",
    "Lej�tszani",
    "Reproduir",
    "�������������",
    "Start",
    "Start",
    "Afspil",
    "P�ehr�t",
  },
  { "Button$Pause",
    "Pause",
    "Pavza",
    "Pausa",
    "Pauze",
    "",// TODO
    "Pause",
    "",// TODO
    "Tauko",
    "Pauza",
    "Pausa",
    "�����",
    "Pausa",
    "Pauz�",
    "Sz�net",
    "Pausa",
    "�����",
    "Pauza",
    "Paus",
    "Pause",
    "Zastavit",
  },
  { "Button$Rewind",
    "Anfang",
    "Na za�etek",
    "Riavvolgi",
    "Naar begin",
    "Rebobinar",
    "Retour",
    "Spol tilbake",
    "Alkuun",
    "Pocz�tek",
    "Rebobinar",
    "�������� ���� ����",
    "�terspolning",
    "�napoi",
    "Vissza az elej�re",
    "Enrera",
    "�����",
    "Na po�etak",
    "Algusesse",
    "Forfra",
    "Na za��tek",
  },
  { "Button$Stop",
    "Beenden",
    "Ustavi",
    "Stop",
    "Eindigen",
    "Parar",
    "Arr�t",
    "Stopp",
    "Lopeta",
    "Zatrzymaj",
    "Parar",
    "�����",
    "Stopp",
    "Stop",
    "Stop",
    "Aturar",
    "����",
    "Stop",
    "Stopp",
    "Stop",
    "Ukon�it",
  },
  { "Button$Resume",
    "Weiter",
    "Nadaljuj",
    "Riprendi",
    "Verder",
    "Continuar",
    "Reprendre",
    "Fortsett",
    "Jatka",
    "Wzn�w",
    "Continuar",
    "���������",
    "Forts�tt",
    "Continuare",
    "Tov�bb",
    "Continuar",
    "����������",
    "Nastavi",
    "J�tka",
    "Forts�t",
    "Pokra�ovat",
  },
  { "Button$Open",
    "�ffnen",
    "Odpri",
    "Apri",
    "Openen",
    "Abrir",
    "Ouvrir",
    "�pne",
    "Avaa",
    "Otw�rz",
    "Abrir",
    "�������",
    "�ppna",
    "Deschide",
    "Kinyitni",
    "Obrir",
    "�������",
    "Otvori",
    "Ava",
    "�bn",
    "Otev��t",
  },
  { "Button$Switch",
    "Umschalten",
    "Preklopi",
    "Cambia",
    "Selecteer",
    "Seleccionar",
    "Regarder",
    "Skift til",
    "Valitse",
    "Prze��cz",
    "Cambiar",
    "A�����",
    "Byt",
    "Comut�",
    "�tkapcsolni",
    "Canviar",
    "�����������",
    "Prebaci",
    "Vali",
    "Skift",
    "P�epnout",
  },
  { "Button$Now",
    "Jetzt",
    "Sedaj",
    "Adesso",
    "Nu",
    "Agora",
    "Maintenant",
    "N�",
    "Nyt",
    "Teraz",
    "Ahora",
    "����",
    "Nu",
    "Acum",
    "Most",
    "Ara",
    "������",
    "Trenutno",
    "N��d",
    "Nu",
    "Nyn�",
  },
  { "Button$Next",
    "N�chste",
    "Sledi",
    "Prossimo",
    "Hierna",
    "Proximo",
    "Apr�s",
    "Neste",
    "Seuraavaksi",
    "Nast�pnie",
    "Despu�s",
    "�������",
    "N�sta",
    "Urm�tor",
    "K�vetkez�",
    "Seg�ent",
    "�����",
    "Slijedi",
    "Tulekul",
    "N�ste",
    "Dal��",
  },
  { "Button$Schedule",
    "Programm",
    "Program",
    "Programma",
    "Programma",
    "Programa��o",
    "Programme",
    "Programmer",
    "Ohjelmisto",
    "Program",
    "Gu�a",
    "���������",
    "Program",
    "Program",
    "Program",
    "Programar",
    "���������",
    "Raspored",
    "Ajakava",
    "Program",
    "Program",
  },
  { "Button$ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "AB�/���",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "���/���",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
  },
  { "Button$Insert",
    "Einf�gen",
    "Vstavi",
    "Inserisci",
    "Invoegen",
    "",// TODO
    "Ins�rer",
    "",// TODO
    "Lis��",
    "Wstaw",
    "Insertar",
    "��������",
    "Infoga",
    "Insereaz�",
    "Beilleszteni",
    "Inserir",
    "�������",
    "Umetni",
    "Lisa (INS)",
    "Inds�t",
    "Vlo�it",
  },
  { "Button$Overwrite",
    "�berschreiben",
    "Prepi�i",
    "Sovrascrivi",
    "Overschrijven",
    "",// TODO
    "�craser",
    "",// TODO
    "Korvaa",
    "Nadpisz",
    "Sobreescribir",
    "�������������",
    "Skriv �ver",
    "Suprascrie",
    "�t�rni",
    "Sobrescriure",
    "������",
    "Prepi�i",
    "Asenda (OVR)",
    "Overskriv",
    "P�epsat",
  },
  { "Button$Menu",
    "Men�",
    "Meni",
    "Menu",
    "Menu",
    "Menu",
    "Menu",
    "Meny",
    "Valikko",
    "Menu",
    "Men�",
    "M����",
    "Meny",
    "Meniu",
    "Men�",
    "Men�",
    "����",
    "Izbor",
    "Men��",
    "Menu",
    "Menu",
  },
  { "Button$Reset",
    "R�cksetzen",
    "Reset",
    "Resetta",
    "Herstart",
    "",//TODO
    "R�initialisation",
    "",//TODO
    "Nollaa",
    "Reset",
    "Reiniciar",
    "���������",
    "�terst�ll",
    "Resetare",
    "Reset",
    "Reiniciar",
    "�����",
    "Reset",
    "Nullimine",
    "Nulstille",
    "Reset",
  },
  { "Button$Scan",
    "Scan",
    "I��i",
    "Scansione",
    "Scan",
    "",//TODO
    "Scan",
    "",//TODO
    "P�ivit�",
    "Skanuj",
    "Escanear",
    "������",
    "Skanna",
    "C�utare canale",
    "Ad�skeres�s",
    "",//TODO
    "�����������",
    "Pretra�i",
    "Uuenda",
    "Skan",
    "Sn�mat",
  },
  { "Button$Audio",
    "Audio",
    "Zvok",
    "Audio",
    "Audio",
    "",// TODO
    "Audio",
    "",// TODO
    "��ni",
    "D�wi�k",
    "Audio",
    "����",
    "Ljud",
    "Sunet",
    "Hang",
    "",// TODO
    "����",
    "",// TODO
    "Audio",
    "Audio",
    "Zvuk",
  },
  // Confirmations:
  { "Delete channel?",
    "Kanal l�schen?",
    "Izbri�i kanal?",
    "Cancellare il canale?",
    "Kanaal verwijderen?",
    "Apagar o canal?",
    "Supprimer la cha�ne?",
    "Slette kanal?",
    "Poistetaanko kanava?",
    "Usun�� kana�?",
    "�Eliminar canal?",
    "�������� ��������?",
    "Ta bort kanalen?",
    "�terg canalul?",
    "Csatorna t�rl�se?",
    "Esborrar el canal?",
    "������� �����?",
    "Obrisati program?",
    "Kustutada kanal?",
    "Slet kanal?",
    "Smazat kan�l?",
  },
  { "Delete timer?",
    "Timer l�schen?",
    "Izbri�i termin?",
    "Cancellare il timer?",
    "Timer verwijderen?",
    "Apagar o timer?",
    "Supprimer la programmation?",
    "Slette timer?",
    "Poistetaanko ajastin?",
    "Usun�� timer?",
    "�Eliminar programaci�n?",
    "�������� ��������������������;?",
    "Ta bort timern?",
    "�terg timer-ul?",
    "Id�z�t� t�rl�se?",
    "Esborrar el temporitzador?",
    "������� ������?",
    "Obrisati termin?",
    "Kustutada taimer?",
    "Slet timer?",
    "Smazat �asova�?",
  },
  { "Delete recording?",
    "Aufzeichnung l�schen?",
    "Izbri�i posnetek?",
    "Cancellare la registrazione?",
    "Opname verwijderen?",
    "Apagar a grava��o?",
    "Supprimer l'enregistrement?",
    "Slette opptak?",
    "Poistetaanko tallenne?",
    "Usun�� nagranie?",
    "�Eliminar grabacion?",
    "�������� ��������?",
    "Ta bort inspelningen?",
    "�terg �nregistrarea?",
    "Felv�tel t�rl�se?",
    "Esborrar gravaci�?",
    "������� ������?",
    "Obrisati snimku?",
    "Kustutada salvestus?",
    "Slet optagelse?",
    "Smazat nahr�vku?",
  },
  { "Timer still recording - really delete?",
    "Timer zeichnet auf - trotzdem l�schen?",
    "Snemanje po terminu - zares izbri�i?",
    "Timer in registrazione - cancellare?",
    "Timer neemt nog op - toch verwijderen?",
    "Timer activo - t�m a certeza que quer apagar?",
    "Enregistrement en cours - confirmez la suppression",
    "Timer gj�r opptak - vil du slette likevel?",
    "Ajastettu tallennus k�ynniss� - poistetaanko silti?",
    "Trwa nagrywanie - na pewno usun��?",
    "Todav�a est� grabando - �eliminar realmente?",
    "�������������������� �� ������� - �������� �������?",
    "Timerstyrd inspelning p�g�r - Avbryta �nd�?",
    "Timer-ul tocmai �nregistreaz� - �terg, totu�i?",
    "Felv�tel folyamatban van - m�gis t�r�lni?",
    "Temporitzador activat - Esborrar de totes maneres?",
    "���� ������ �� ������� - ������������� �������?",
    "Jo� snimam - zaista obrisati?",
    "Salvestus aktiivne - kustutada?",
    "Timerstyret optagelse i gang - slet alligevel?",
    "Prob�h� nahr�v�n� - opravdu smazat?",
  },
  { "Stop recording?",
    "Aufzeichnung beenden?",
    "Prekini snemanje?",
    "Fermare la registrazione?",
    "Opname stoppen?",
    "Parar Grava��o?",
    "Arr�ter l'enregistrement?",
    "Stoppe opptak?",
    "Lopetetaanko tallennus?",
    "Zatrzyma� nagrywanie?",
    "�Parar grabaci�n?",
    "������� ��������?",
    "Stanna inspelning?",
    "Opresc �nregistrarea?",
    "Felv�tel befejezni?",
    "Aturar la gravaci�?",
    "���������� ������?",
    "Zaustaviti snimanje?",
    "L�petada salvestamine?",
    "Stop optagelse?",
    "Ukon�it nahr�v�n�?",
  },
  { "Cancel editing?",
    "Schneiden abbrechen?",
    "�elite prekiniti urejanje?",
    "Annullare la modifica?",
    "Bewerken afbreken?",
    "Cancelar Modifica��es?",
    "Annuler les modifications?",
    "Avbryte redigering?",
    "Perutaanko muokkaus?",
    "Anulowa� monta�?",
    "�Cancelar edici�n?",
    "A������ ������������?",
    "Avbryta redigeringen?",
    "Opresc montajul �nregistr�rii?",
    "V�g�st befejezni?",
    "Cancel�lar l'edici�?",
    "�������� ��������������?",
    "Otkazati ure�ivanje?",
    "T�histada muudatus?",
    "Afbryd redigering?",
    "Zru�it editaci?",
  },
  { "Really restart?",
    "Wirklich neu starten?",
    "Zares ponoven zagon?",
    "Eseguire un riavvio?",
    "Werkelijk opnieuw starten?",
    "Tem a certeza que quer reiniciar?",
    "Red�marrer?",
    "Vil du virkelig starte p� nytt?",
    "K�ynnistet��nk� uudelleen?",
    "Na pewno zrestartowa�?",
    "�Reiniciar realmente?",
    "N� ����� ������� ������������?",
    "Vill du verkligen starta om?",
    "Sigur repornesc?",
    "T�nyleg �jraind�tani?",
    "Segur que voleu reiniciar?",
    "������������� �������������?",
    "Zaista ponovo pokrenuti?",
    "Restart?",
    "Vil du virkelig genstarte?",
    "Opravdu restartovat?",
  },
  { "restart anyway?",
    "trotzdem neu starten?",
    "zares ponoven zagon?",
    "riavviare comunque?",
    "toch opnieuw starten?",
    "quer mesmo reiniciar?",
    "red�marrer?",
    "starte p� nytt likevel?",
    "k�ynnistet��nk� uudelleen?",
    "zrestartowa� mimo to?",
    "�reiniciar igualmente?",
    "������ �� ����� ������������?",
    "vill du �nd� starta om?",
    "repornesc, totu�i?",
    "m�gis �jraind�tani?",
    "Reiniciar de totes maneres?",
    "������������� �������������?",
    "svejedno restart sistema?",
    "restart?",
    "genstart alligevel?",
    "p�esto restartovat?",
  },
  { "shut down anyway?",
    "trotzdem ausschalten?",
    "zares izklopi?",
    "spegnere comunque?",
    "toch uitschakelen?",
    "quer mesmo desligar?",
    "confirmez l'arr�t",
    "sl� av likevel?",
    "sammutetaanko?",
    "wy��czy� mimo to?",
    "�apagar igualmente?",
    "������ �� ����� �����������?",
    "vill du �nd� avbryta?",
    "�nchid, totu�i?",
    "m�gis kikapcsolni?",
    "Apagar de totes maneres?",
    "������������� ���������?",
    "svejedno isklju�iti?",
    "l�litada v�lja?",
    "sluk alligevel?",
    "p�esto vypnout?",
  },
  { "Recording - restart anyway?",
    "Aufnahme l�uft - trotzdem neu starten?",
    "Snemanje - zares ponoven zagon?",
    "In registrazione - riavviare comunque?",
    "Opname loopt - toch opnieuw starten?",
    "Em grava��o - quer mesmo reiniciar?",
    "Enregistrement en cours - red�marrer?",
    "Gj�r opptak - starte p� nytt likevel?",
    "Tallennus kesken - k�ynnistet��nk� uudelleen?",
    "Trwa nagrywanie - zrestartowa� mimo to?",
    "Grabaci�n en curso - �reiniciar igualmente?",
    "������� ������� - ������ �� ����� ������������?",
    "Inspelning p�g�r, vill du �nd� starta om?",
    "Tocmai se �nregistreaz� - repornesc, totu�i?",
    "Felv�tel folyamatban van - m�gis �jraind�tani?",
    "Gravant - Reiniciar de totes maneres?",
    "���� ������ - ������������� �������������?",
    "Snimanje traje - svejedno restart sistema?",
    "Salvestamine aktiivne - restart?",
    "Optagelse igang - genstart alligevel?",
    "Syst�m je zanepr�zdn�n - p�esto restartovat?",
  },
  { "Editing - restart anyway?",
    "Schnitt l�uft - trotzdem neu starten?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Editando - �reiniciar igualmente?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Recording - shut down anyway?",
    "Aufnahme l�uft - trotzdem ausschalten?",
    "Snemanje - zares izklopi?",
    "In registrazione - spegnere comunque?",
    "Opname loopt - toch uitschakelen?",
    "Em grava��o - quer mesmo desligar?",
    "Enregistrement en cours - confirmez l'arr�t",
    "Gj�r opptak - sl� av likevel?",
    "Tallennus kesken - sammutetaanko?",
    "Trwa nagrywanie - wy��czy� mimo to?",
    "Grabaci�n en curso - �apagar igualmente?",
    "������� ������� - ������ �� ����� �����������?",
    "Inspelning p�g�r, vill du �nd� avbryta?",
    "Tocmai se �nregistreaz� - �nchid, totu�i?",
    "Felv�tel folyamatban van - m�gis kikapcsolni?",
    "Gravant - Apagar de totes maneres?",
    "���� ������ - ������������� ���������?",
    "Snimanje traje - svejedno isklju�iti?",
    "Salvestamine aktiivne - l�litada v�lja?",
    "Optagelse igang - sluk alligevel?",
    "Syst�m je zanepr�zdn�n - p�esto vypnout?",
  },
  { "Recording in %ld minutes, shut down anyway?",
    "Aufnahme in %ld Minuten - trotzdem ausschalten?",
    "Snemanje �ez %ld minut, zares izklopi?",
    "Registrazione fra %ld minuti - spegnere comunque?",
    "Opname in %ld minuten - toch uitschakelen?",
    "Em grava��o dentro de %ld minutos - quer mesmo desligar?",
    "Enregistrement dans %ld minutes - confirmez l'arr�t",
    "Skal gj�re opptak om %ld minutter - sl� av likevel?",
    "Tallennus alkaa %ld min kuluttua - sammutetaanko?",
    "Nagrywanie za %ld minut - wy��czy� mimo to?",
    "Grabaci�n dentro de %ld minutos, �apagar realmente?",
    "���������� ������� �� %ld ����� - ������ �� �����������?",
    "Inspelning startar om %ld minuter, vill du avsluta?",
    "�nregistrez peste %ld minute - �nchid, totu�i?",
    "Felv�tel %ld perc mulva kezd�dik - m�gis kikapcsolni?",
    "Hi ha una gravaci� en %ld minuts - Apagar de totes maneres?",
    "����� %ld ����� �������� ������ - ������������� ���������?",
    "Snimanje za %ld minuta - svejedno isklju�iti?",
    "Salvestamine algab %ld minuti p�rast - l�litada v�lja?",
    "Optagelse om %ld minutter - sluk alligevel?",
    "Nahr�v�n� za�ne za %ld minut - p�esto vypnout?",
  },
  { "Press any key to cancel shutdown",
    "Taste dr�cken, um Ausschalten abzubrechen",
    "Pritisnite katerikoli gumb za preklic izklopa",
    "Premere un tasto per annullare lo spegnimento",
    "Druk een toets om shutdown af te breken",
    "Pressione qualquer tecla para cancelar",
    "Appuyez sur une touche pour annuler l'arr�t",
    "Trykk en tast for ikke � sl� av",
    "Peru sammutus painamalla mit� tahansa n�pp�int�",
    "Naci�nij dowolny klawisz aby nie wy��cza�",
    "Pulse un bot�n para interrumpir apagado",
    "����� ��� ������� ��� ������� �����������",
    "Tryck valfri knapp f�r att �terkalla avst�ngningen",
    "Apas� orice tast� pentru a anula �nchiderea",
    "Nyomj egy gombot a le�ll�s meg�ll�t�s�hoz",
    "Prem qualsevol tecla per cancel�lar l'aturada",
    "������� ����� ������ ����� �������� ����������.",
    "Pritisnite tipku ako ne �elite ugasiti sistem",
    "Vajuta suvalist klahvi v�ljal�litamise t�histamiseks",
    "Tryk vilk�rlig tast for at annullere sluk",
    "Jak�koliv kl�vesa zru�� vypnut�",
  },
  { "Press any key to cancel restart",
    "Taste dr�cken, um Neustart abzubrechen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Pulse un bot�n para interrumpir reinicio",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "VDR will shut down later - press Power to force",
    "VDR schaltet sp�ter aus - Power zum erzwingen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "VDR sammuu my�hemmin - pakota virtakytkimell�",
    "",//TODO
    "VDR se apagar� m�s tarde - pulse Apagar para forzar",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "VDR will shut down in %s minutes",
    "VDR wird in %s Minuten ausschalten",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "VDR sammuu %s minuutin kuluttua",
    "",//TODO
    "VDR se apagar� en %s minutos",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "VDR will go in HotStandby in %s minutes",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "VDR har� un apagado r�pido en %s minutos",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Editing - shut down anyway?",
    "Schnitt l�uft - trotzdem ausschalten?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Leikkaus kesken - sammutetaanko?",
    "",//TODO
    "Editando - �apagar igualmente?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Plugin %s wakes up in %ld min, continue?",
    "Plugin %s wacht in %ld Min auf, weiter?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Laajennos %s her�� %ld minuutin kuluttua - sammutetaanko?",
    "",//TODO
    "El plugin %s iniciar� en %ld min, �continuar?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  // Channel parameters:
  { "Name",
    "Name",
    "Ime",
    "Nome",
    "Naam",
    "Nome",
    "Nom",
    "Navn",
    "Nimi",
    "Nazwa",
    "Nombre",
    "�����",
    "Namn",
    "Nume",
    "N�v",
    "Nom",
    "��������",
    "Naziv",
    "Nimi",
    "Navn",
    "N�zev",
  },
  { "Frequency",
    "Frequenz",
    "Frekvenca",
    "Frequenza",
    "Frequentie",
    "Frequ�ncia",
    "Fr�quence",
    "Frekvens",
    "Taajuus",
    "Cz�stotliwo��",
    "Frecuencia",
    "���������",
    "Frekvens",
    "Frecven��",
    "Frekvencia",
    "Freq��ncia",
    "�������",
    "Frekvencija",
    "Sagedus",
    "Frekvens",
    "Frekvence",
  },
  { "Polarization",
    "Polarisation",
    "Polarizacija",
    "Polarizzazione",
    "Polarisatie",
    "Polariza��o",
    "Polarisation",
    "Polarisasjon",
    "Polarisaatio",
    "Polaryzacja",
    "Polarizaci�n",
    "������",
    "Polarisation",
    "Polarizare",
    "Polariz�ci�",
    "Polaritzaci�",
    "�����������",
    "Polarizacija",
    "Polarisatsioon",
    "Polarisation",
    "Polarizace",
  },
  { "Source",
    "Quelle",
    "Pozicija",
    "Sorgente",
    "Bron",
    "",//TODO
    "Source",
    "",//TODO
    "L�hde",
    "�r�d�o",
    "Fuente",
    "����",
    "K�lla",
    "Surs�",
    "Forr�s",
    "Origen",
    "��������",
    "Izvor",
    "Allikas",
    "Kilde",
    "Zdroj",
  },
  { "Srate",
    "Srate",
    "Srate",
    "SymbolRate",
    "Srate",
    "Srate",
    "Fr�q. Symbole",
    "Srate",
    "Symbolinopeus",
    "Srate",
    "T.s�mbolos",
    "Srate",
    "Srate",
    "Rat� simboluri",
    "Srate",
    "Srate",
    "����. ��������",
    "Srate", // hrv
    "Srate",
    "Srate",
    "Srate",
  },
  { "Vpid",
    "Vpid",
    "Vpid",
    "PID Video",
    "Vpid",
    "PID de V�deo",
    "PID Vid�o",
    "Vpid",
    "Kuva-PID",
    "Vpid",
    "Vpid",
    "B����� PID",
    "VPID",
    "PID Video",
    "Vpid",
    "PID V�deo",
    "Vpid (�����)",
    "Vpid",
    "Vpid",
    "Vpid",
    "Vpid",
  },
  { "Ppid",
    "Ppid",
    "Ppid",
    "PID Pcr",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "PCR-PID",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
    "Ppid",
  },
  { "Apid1",
    "Apid1",
    "Apid1",
    "PID Audio (1)",
    "Apid1",
    "PID �udio (1)",
    "PID Audio (1)",
    "Apid1",
    "��ni-PID (1)",
    "Apid1",
    "Apid1",
    "Apid1",
    "APID1",
    "PID Audio (1)",
    "Apid1",
    "PID �udio (1)",
    "Apid1 (����� 1)",
    "Apid1", // hrv
    "Apid1",
    "Apid1",
    "Apid1",
  },
  { "Apid2",
    "Apid2",
    "Apid2",
    "PID Audio (2)",
    "Apid2",
    "Apid2",
    "PID Audio (2)",
    "Apid2",
    "��ni-PID (2)",
    "Apid2",
    "Apid2",
    "Apid2",
    "APID2",
    "PID Audio (2)",
    "Apid2",
    "PID �udio (2)",
    "Apid2 (����� 2)",
    "Apid2", // hrv
    "Apid2",
    "Apid2",
    "Apid2",
  },
  { "Dpid1",
    "Dpid1",
    "Dpid1",
    "PID AC3 (1)",
    "Dpid1",
    "PID AC3 (1)",
    "PID AC3 (1)",
    "AC3pid1",
    "Dolby-PID (1)",
    "Dpid1",
    "Dpid1",
    "Dpid1",
    "DPID1",
    "PID AC3 (1)",
    "Dpid1",
    "PID AC3 (1)",
    "Dpid1 (AC3 1)",
    "Dpid1", // hrv
    "Dpid1",
    "Dpid1",
    "Dpid1",
  },
  { "Dpid2",
    "Dpid2",
    "Dpid2",
    "PID AC3 (2)",
    "Dpid2",
    "PID AC3 (2)",
    "PID AC3 (2)",
    "AC3pid2",
    "Dolby-PID (2)",
    "Dpid2",
    "Dpid2",
    "Dpid2",
    "DPID2",
    "PID AC3 (2)",
    "Dpid2",
    "PID AC3 (2)",
    "Dpid2 (AC3 2)",
    "Dpid2", // hrv
    "Dpid2",
    "Dpid2",
    "Dpid2",
  },
  { "Tpid",
    "Tpid",
    "Tpid",
    "PID Teletext",
    "Tpid",
    "PID Teletexto",
    "PID T�l�texte",
    "Tekst-TV pid",
    "Teksti-TV-PID",
    "Tpid",
    "Tpid",
    "Tpid",
    "TPID",
    "PID Teletext",
    "Tpid",
    "PID Teletext",
    "Tpid (���������)",
    "Tpid", // hrv
    "Tpid",
    "Tpid",
    "Tpid",
  },
  { "CA",
    "CA",
    "CA",
    "CA",
    "CA",
    "Encripta��o",
    "Cryptage",
    "Kortleser",
    "Salaus (CA)",
    "CA",
    "Acceso condicional (CA)",
    "CA",
    "Kortl�sare",
    "CA (Acces Condi�ional)",
    "CA",
    "Acc�s Condicional",
    "CA (�������)",
    "Kodiranje (CA)",
    "CA",
    "CA",
    "CA",
  },
  { "Free To Air",
    "frei empfangbar",
    "nekodiran",
    "in chiaro",
    "",//TODO
    "",//TODO
    "Aucun cryptage",
    "",//TODO
    "vapaa",
    "nieszyfrowany",
    "en abierto",
    "",//TODO
    "Okodad",
    "FTA (necriptat)",
    "K�dolatlan",
    "",//TODO
    "FTA (��������������)",
    "",//TODO
    "FTA",
    "Free To Air",
    "voln�",
  },
  { "encrypted",
    "verschl�sselt",
    "kodiran",
    "criptato",
    "",//TODO
    "",//TODO
    "Crypt�",
    "",//TODO
    "salattu",
    "szyfrowany",
    "cifrado",
    "",//TODO
    "krypterad",
    "criptat",
    "K�dolt",
    "",//TODO
    "������������",
    "",//TODO
    "kr�ptitud",
    "kodet",
    "k�dovan�",
  },
  { "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Palvelu-ID",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid",
    "Sid", // hrv
    "Sid",
    "Sid",
    "Sid",
  },
  { "Inversion",
    "Inversion",
    "Inverzija",
    "Inversion",
    "Inversion",
    "Inversion",
    "Inversion",
    "Inversion",
    "Inversio",
    "Inwersja",
    "Inversion",
    "����������",
    "Inversion",
    "Inversiune",
    "Inversion",
    "Inversi�",
    "��������",
    "Inverzija",
    "Inversioon",
    "Inversion",
    "Inversion",
  },
  { "Bandwidth",
    "Bandbreite",
    "Pasovna �irina",
    "Banda passante",
    "Bandwidth",
    "Bandwidth",
    "Bande passante",
    "Bandwidth",
    "Kaistanleveys",
    "Pasmo",
    "Ancho de banda",
    "����� ����������",
    "Bandbredd",
    "L�rgime de band�",
    "Bandwidth",
    "Amplada de banda",
    "��������",
    "Bandwidth", // hrv "Pojasna �irina",
    "Ribalaius",
    "B�ndbredde",
    "���ka p�sma",
  },
  { "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "Suojaustaso (HP)",
    "CoderateH",
    "CoderateH",
    "������ ������ H",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH",
    "CoderateH", // hrv
    "CoderateH",
    "CoderateH",
    "CoderateH",
  },
  { "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "Suojaustaso (LP)",
    "CoderateL",
    "CoderateL",
    "������ ������ L",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL",
    "CoderateL", // hrv
    "CoderateL",
    "CoderateL",
    "CoderateL",
  },
  { "Modulation",
    "Modulation",
    "Modulacija",
    "Modulation",
    "Modulation",
    "Modulation",
    "Modulation",
    "Modulation",
    "Modulaatio",
    "Modulacja",
    "Modulaci�n",
    "����������",
    "Modulation",
    "Modula�ie",
    "Modulation",
    "Modulaci�",
    "���������",
    "Modulacija",
    "Modulatsioon",
    "Modulation",
    "Modulace",
  },
  { "Transmission",
    "Transmission",
    "Prenos",
    "Transmission",
    "Transmission",
    "Transmission",
    "Transmission",
    "Transmission",
    "Transmissio",
    "Transmisja",
    "Transmisi�n",
    "��������",
    "Transmission",
    "Transmisie",
    "Transmission",
    "Transmissi�",
    "��������",
    "Prijenos",
    "Transmissioon",
    "Transmission",
    "P�enos",
  },
  { "Guard",
    "Guard",
    "Za��ita",
    "Guard",
    "Guard",
    "Guard",
    "Guard",
    "Guard",
    "Suojav�li",
    "Guard",
    "Int.Guarda",
    "���������",
    "Guard",
    "Guard",
    "Guard",
    "Protegir",
    "������",
    "Za�tita",
    "Guard",
    "Guard",
    "Guard",
  },
  { "Hierarchy",
    "Hierarchie",
    "Hierarhija",
    "Hierarchy",
    "Hierarchy",
    "Hierarchy",
    "Hi�rarchie",
    "Hierarchy",
    "Hierarkia",
    "Hierarchia",
    "Jerarqu�a",
    "���������",
    "Hierarchy",
    "Ierarhie",
    "Hierarchy",
    "Jerarquia",
    "��������",
    "Hijerarhija",
    "Hierarhia",
    "Hierarki",
    "Hierarchy",
  },
  // Timer parameters:
  { "Active",
    "Aktiv",
    "Aktivno",
    "Attivo",
    "Actief",
    "Activo",
    "Actif",
    "Aktiv",
    "Aktiivinen",
    "Aktywny",
    "Activo",
    "������",
    "Aktiv",
    "Activ",
    "Aktiv",
    "Actiu",
    "�����������",
    "Aktivan",
    "Aktiivne",
    "Aktiv",
    "Aktivn�",
  },
  { "Channel",
    "Kanal",
    "Program",
    "Canale",
    "Kanaal",
    "Canal",
    "Cha�ne",
    "Kanal",
    "Kanava",
    "Kana�",
    "Canal",
    "������",
    "Kanal",
    "Canal",
    "Csatorna",
    "Canal",
    "�����",
    "Program",
    "Kanal",
    "Kanal",
    "Kan�l",
  },
  { "Day",
    "Tag",
    "Dan",
    "Giorno",
    "Dag",
    "Dia",
    "Jour",
    "Dag",
    "P�iv�",
    "Dzie�",
    "D�a",
    "�����",
    "Dag",
    "Ziua",
    "Nap",
    "Dia",
    "����",
    "Dan",
    "P�ev",
    "Dag",
    "Den",
  },
  { "Start",
    "Anfang",
    "Za�etek",
    "Inizio",
    "Begin",
    "Inicio",
    "D�but",
    "Start",
    "Aloitus",
    "Pocz�tek",
    "Comienzo",
    "����",
    "B�rjar",
    "�nceput",
    "Kezdet",
    "Inici",
    "������",
    "Po�etak",
    "Start",
    "Start",
    "Za��tek",
  },
  { "Stop",
    "Ende",
    "Konec",
    "Fine",
    "Einde",
    "Fim",
    "Fin",
    "Slutt",
    "Lopetus",
    "Koniec",
    "Fin",
    "�����",
    "Slutar",
    "Sf�r�it",
    "V�ge",
    "Fi",
    "�����",
    "Kraj",
    "Stopp",
    "Stop",
    "Konec",
  },
  { "VPS",
    "VPS",
    "VPS",
    "VPS",
    "VPS",
    "",// TODO
    "VPS",
    "",// TODO
    "VPS",
    "VPS",
    "VPS",
    "VPS",
    "VPS",
    "VPS",
    "VPS",
    "",// TODO
    "VPS ��������",
    "VPS", // hrv
    "VPS",
    "VPS",
    "VPS",
  },
  { "Priority",
    "Priorit�t",
    "Prioriteta",
    "Priorita'",
    "Prioriteit",
    "Prioridade",
    "Priorit�",
    "Prioritet",
    "Prioriteetti",
    "Priorytet",
    "Prioridad",
    "�������������",
    "Prioritet",
    "Prioritate",
    "Priorit�s",
    "Prioritat",
    "���������",
    "Prioritet",
    "Prioriteet",
    "Prioritet",
    "Priorita",
  },
  { "Lifetime",
    "Lebensdauer",
    "Veljavnost",
    "Durata",
    "Bewaarduur",
    "Dura��o",
    "Dur�e de vie",
    "Levetid",
    "Elinik�",
    "Czas �ycia",
    "Duraci�n",
    "�������� ���������",
    "Livstid",
    "Timp de p�strare",
    "�lettartam",
    "Durada",
    "���� ��������",
    "Trajanje",
    "Eluiga",
    "Levetid",
    "�ivotnost",
  },
  { "File",
    "Datei",
    "Datoteka",
    "Nome",
    "Filenaam",
    "Ficheiro",
    "Fichier",
    "Filnavn",
    "Tiedosto",
    "Plik",
    "Archivo",
    "������",
    "Filnamn",
    "Fi�ier",
    "File",
    "Arxiu",
    "����",
    "Datoteka",
    "Fail",
    "Fil",
    "Soubor",
  },
  { "First day",
    "Erster Tag",
    "Prvi dan",
    "Primo giorno",
    "Eerste dag",
    "Primeiro dia",
    "Premier jour",
    "F�rste dag",
    "1. p�iv�",
    "Pierwszy dzie�",
    "Primer d�a",
    "����� ����",
    "F�rsta dag",
    "Prima zi",
    "Els� nap",
    "Primer dia",
    "������ ����",
    "Prvi dan",
    "Esimene p�ev",
    "F�rste dag",
    "Prvn� den",
  },
  // Error messages:
  { "Channel is being used by a timer!",
    "Kanal wird von einem Timer benutzt!",
    "Kanal je zavzet s terminom za snemanje!",
    "Canale occupato da un timer!",
    "Kanaal wordt gebruikt door een timer!",
    "Canal a ser utilizador por um timer!",
    "Cette cha�ne est en cours d'utilisation!",
    "Kanalen er i bruk av en timer!",
    "Kanava on ajastimen k�yt�ss�!",
    "Kana� jest u�ywany przez timer!",
    "�Hay una grabaci�n programada para este canal!",
    "�� ������ ��������������� ��� �������������������!",
    "Kanalen anv�nds av en timer!",
    "Canalul este utilizat de un timer!",
    "Az ad�t az id�z�t� haszn�lja!",
    "Canal en �s per un temporitzador!",
    "����� ����� ��������!",
    "Program je trenutno zauzet terminom za snimanje",
    "Kanal on taimeri kasutuses!",
    "Kanal anvendes af en timer!",
    "Kan�l je dr�en� �asova�em nahr�vek!",
  },
  { "Can't switch channel!",
    "Kanal kann nicht umgeschaltet werden!",
    "Ne morem preklopiti kanala!",
    "Impossibile cambiare canale!",
    "Kan geen kanaal wisselen!",
    "N�o pode mudar de canal!",
    "Impossible de changer de cha�ne!",
    "Ikke mulig � skifte kanal!",
    "Kanavan vaihtaminen ei mahdollista!",
    "Nie mo�na prze��czy� kana�u!",
    "�No se puede cambiar de canal!",
    "������ �������� �������!",
    "Om�jligt att byta kanal!",
    "Nu pot comuta canalul!",
    "Az ad�t nem lehet elkapcsolni",
    "No puc canviar de canal!",
    "���������� ����������� �����!",
    "Ne mogu prebaciti program!",
    "Kanali vahetus ei ole v�imalik!",
    "Kan ikke skifte kanal!",
    "Kan�l nelze p�epnout!",
  },
  { "Timer is recording!",
    "Timer zeichnet gerade auf!",
    "Snemanje po terminu!",
    "Registrazione di un timer in corso!",
    "Timer is aan het opnemen!",
    "Timer a gravar!",
    "Enregistrement en cours!",
    "Timer gj�r opptak!",
    "Ajastintallennus k�ynniss�!",
    "Timer nagrywa!",
    "�Grabaci�n en curso!",
    "������� ��������������� �������!",
    "Timerstyrd inspelning p�g�r!",
    "Timer-ul tocmai �nregistreaz�!",
    "Felv�tel folyamatban van!",
    "El temporitzador est� gravant!",
    "���� ������ �� �������!",
    "Snimanje po definiranom terminu!",
    "Taimeriga salvestamine aktiivne!",
    "Timer optager nu!",
    "�asova� pr�v� nahr�v�!",
  },
  { "Error while accessing recording!",
    "Fehler beim Ansprechen der Aufzeichnung!",
    "Napaka pri dostopu do posnetka!",
    "Errore nell'accedere alla registrazione!",
    "Fout bij lezen opname!",
    "Erro ao aceder � grava��o",
    "Impossible d'acc�der � l'enregistrement",
    "Feil under lesing av opptak!",
    "Tallenteen toistaminen ep�onnistui!",
    "Bl�d podczas dost�pu do nagrania!",
    "�Error al acceder a la grabaci�n!",
    "�������� ���� ���������� ��������!",
    "Inspelningen g�r inte att l�sa!",
    "Eroare la accesarea �nregistr�rii",
    "Hiba a felv�tel hozz�f�r�s�n�l",
    "Error a l'accedir a la gravaci�!",
    "������ ������� � ������!",
    "Gre�ka pri pristupanju snimke!",
    "Salvestuse taasesitus eba�nnestus!",
    "Fejl ved l�sning af optagelse!",
    "Chyba p�i p��stupu k nahr�vk�m!",
  },
  { "Error while deleting recording!",
    "Fehler beim L�schen der Aufzeichnung!",
    "Napaka pri brisanju posnetka!",
    "Errore nel cancellare la registrazione!",
    "Fout bij verwijderen opname!",
    "Erro enquanto apagava uma grava��o!",
    "Erreur de suppression de l'enregistrement!",
    "Feil under sletting av opptak!",
    "Tallenteen poistaminen ep�onnistui!",
    "Bl�d podczas usuwania nagrania!",
    "�Error al borrar la grabaci�n!",
    "����� ���� ��� �������� ��� �������!",
    "Inspelningen g�r inte att ta bort!",
    "Eroare la �tergerea �nregistr�rii!",
    "Hiba a felv�tel t�rl�s�n�l!",
    "Error a l'esborrar la gravaci�!",
    "������ �������� ������!",
    "Gre�ka pri brisanju snimke!",
    "Salvestuse kustutamine eba�nnestus!",
    "Fejl ved sletning af optagelse!",
    "Chyba p�i maz�n� nahr�vky!",
  },
  { "*** Invalid Channel ***",
    "*** Ung�ltiger Kanal ***",
    "*** Neznan kanal ***",
    "*** Canale NON valido ***",
    "*** Ongeldig kanaal ***",
    "*** Canal Inv�lido! ***",
    "*** Cha�ne invalide! ***",
    "*** Ugyldig Kanal! ***",
    "*** Virheellinen kanavavalinta! ***",
    "*** Niepoprawny kana� ***",
    "*** Canal no v�lido ***",
    "*** ����� ������ ***",
    "*** Felaktig kanal ***",
    "*** Canal invalid ***",
    "*** �rv�nytelen csatorna ***",
    "*** Canal incorrecte ***",
    "*** ������������ ����� ***",
    "*** Neispravan program ***",
    "*** Vigane kanal ***",
    "*** Ugyldig kanal! ***",
    "*** Neplatn� kan�l ***",
  },
  { "Upcoming VPS recording!",
    "VPS-Aufnahme beginnt in K�rze!",
    "Sledi VPS snemanje!",
    "Registrazione VPS imminente!",
    "",//TODO
    "",//TODO
    "Enregistrement VPS commence procha�nement",
    "",//TODO
    "VPS-tallennus on alkamassa!",
    "Wkr�tce nagranie VPS!",
    "�Grabaci�n VPS pendiente!",
    "",//TODO
    "strax VPS inspelning...",
    "Urmeaz� o �nregistrare VPS!",
    "VPS-felv�tel r�gt�n kezd�dik!",
    "",//TODO
    "VPS-������ ����� ��������",
    "",//TODO
    "VPS salvestamine tulekul!",
    "VPS optagelse starter snart!",
    "Brzo za�ne nahr�v�n� VPS!",
  },
  { "No free DVB device to record!",
    "Keine freie DVB-Karte zum Aufnehmen!",
    "Ni proste DVB naprave za snemanje!",
    "Nessuna periferica DVB disponibile per registrare!",
    "Geen vrije DVB kaart om op te nemen!",
    "Nenhuma placa DVB disponivel para gravar!",
    "Pas de carte DVB disponible pour l'enregistrement!",
    "Ingen ledige DVB enheter for opptak!",
    "Ei vapaata DVB-viritint� tallennukselle!",
    "Brak wolnej karty DVB do nagrywania!",
    "�Ning�n dispositivo DVB disponible para grabar!",
    "���������� DVB ������ ��� �������!",
    "Det finns ingen ledig DVB enhet f�r inspelning!",
    "Nu mai sunt dispozitive DVB disponibile pentru �nregistrare!",
    "Nincs szabad DVB k�rtya a felv�telhez!",
    "No hi ha cap dispositiu DVB lliure per gravar!",
    "��� ���������� DVB-���������� ��� ������!",
    "Nema slobodnog DVB ure�aja za snimanje!",
    "Puudub vaba DVB seade salvestamiseks!",
    "Ingen fri DVB enhed til optagelse!",
    "��dn� DVB karta nen� voln� pro nahr�v�n�",
  },
  { "Channel not available!",
    "Kanal nicht verf�gbar!",
    "Kanal ni razpolo�ljiv!",
    "Canale non disponibile!",
    "Kanaal niet beschikbaar",
    "",//TODO
    "Cha�ne non disponible!",
    "",//TODO
    "Kanava ei ole k�ytett�viss�!",
    "Kana� nie jest dost�pny!",
    "!Canal no disponible!",
    "�� ������ ��� ����� ���������!",
    "Kanalen �r inte tillg�nglig!",
    "Canal indisponibil",
    "Az ad� nem el�rhet�",
    "Canal no disponible!",
    "����� ����������!",
    "Program nije dostupan!",
    "Kanal ei ole k�ttesaadav!",
    "Kanal er ikke tilg�ngelig!",
    "Kan�l nen� dostupn�!",
  },
  { "Channel settings are not unique!",
    "Kanaleinstellungen sind nicht eindeutig!",
    "Nastavitve kanala niso edinstvene!",
    "Parametri canale non univoci!",
    "Kanaalinstellingen zijn niet uniek!",
    "",//TODO
    "Caract�ristiques des cha�nes ne sont pas uniques",
    "",//TODO
    "Kanava-asetukset eiv�t ole yksil�lliset!",
    "Ustawienia kana�u nie s� unikalne!",
    "!Propiedades de canal duplicadas!",
    "�� ��������� ��� �������� ����������������!",
    "Kanalinst�llningarna �r ej unika!",
    "Parametrii canalului nu sunt univoci!",
    "Az ad�be�ll�t�sok nem egy�rtelm�ek",
    "Propietats del canal duplicades!",
    "��������� ������ �� ���������!",
    "Parametri programa nisu jednozna�ni!",
    "Kanaliseaded ei ole unikaalsed!",
    "Kanalindstillinger er ikke entydige!",
    "Nastaven� kan�l� nejsou unik�tn�!",
  },
  { "Channel locked (recording)!",
    "Kanal blockiert (zeichnet auf)!",
    "Zaklenjen kanal (snemanje)!",
    "Canale bloccato (in registrazione)!",
    "Kanaal geblokkeerd (neemt op)!",
    "Canal bloqueado (a gravar)!",
    "Cha�ne verrouill�e (enregistrement en cours)!",
    "Kanalen er l�st (opptak)!",
    "Kanava lukittu (tallennus k�ynniss�)!",
    "Kana� zablokowany (trwa nagrywanie)!",
    "�Canal bloqueado (grabando)!",
    "�� ������ ����� ������������ (������ �������)!",
    "Kanalen �r l�st (inspelning p�g�r)!",
    "Canal blocat (�nregistrare)!",
    "Az ad� blokkolva (felv�tel)!",
    "Canal bloquejat (gravant)!",
    "����� ������������ (���� ������)!",
    "Program blokiran (snimanje)!",
    "Kanal lukus (salvestamine aktiivne)!",
    "Kanal blokeret (optagelse i gang)",
    "Kan�l je blokovan� (nahr�v� se)!",
  },
  { "Can't start Transfer Mode!",
    "Transfer-Mode kann nicht gestartet werden!",
    "Ne morem za�eti s prenosnim na�inom!",
    "Impossibile iniziare la modalita' di trasferimento!",
    "Kan Transfer-Mode niet starten",
    "Imposs�vel iniciar modo de transfer�ncia!",
    "Impossible d'utiliser le mode transfert!",
    "Kan ikke starte transfer modus!",
    "Siirtotilan aloitus ep�onnistui!",
    "Nie mo�na uruchomi� trybu transferu!",
    "�No se puede iniciar el modo de transferencia!",
    "�������� ��������� ���������� ���������!",
    "Kan inte starta Transfer Mode!",
    "Nu pot porni modul de transfer!",
    "Transfer-Mode nem ind�that�!",
    "No puc iniciar el mode de transfer�ncia!",
    "���������� �������� ����� ��������!", // ???
    "Ne mogu zapo�eti 'prijenosni' na�in!",
    "Siirdemooduse start eba�nnestus!",
    "Kan ikke starte Transfer Mode!",
    "Nelze za��t s p�enosem",
  },
  { "No editing marks defined!",
    "Keine Schnittmarken gesetzt!",
    "Nobena prekinitvena to�ka ni definirana!",
    "Nessun marcatore di modifica definito!",
    "Geen bewerkingsmarkeringen gedefinieerd!",
    "",//TODO
    "Pas de marques d'�dition d�finies!",
    "",//TODO
    "Muokkausmerkinn�t puuttuvat!",
    "Nie zdefiniowano znacznik�w monta�u!",
    "�No se definieron marcas de edici�n!",
    "��� ����� ������� ������ ������������",
    "Det finns inga redigeringsm�rken",
    "Nu s-au pus marcaje de montaj pentru aceast� �nregistrare",
    "A v�g�pont nincs kijel�lve",
    "No hi ha marques d'edici� definides",
    "�� ������ ����� ��� �������!",
    "Nijedna prekidna to�ka nije postavljena!",
    "Redigeerimise markerid puuduvad!",
    "Der er ikke sat nogen redigeringsm�rker!",
    "Nejsou definov�ny edita�n� zna�ky!",
  },
  { "Can't start editing process!",
    "Schnitt kann nicht gestartet werden!",
    "Ne morem za�eti urejanja!",
    "Impossibile iniziare processo di modifica!",
    "Kan niet beginnen met bewerken!",
    "N�o pode iniciar a modifica��o!",
    "Impossible de commencer le montage!",
    "Kan ikke starte redigeringsprosessen!",
    "Muokkauksen aloitus ep�onnistui!",
    "Nie mo�na uruchomi� procesu edycji!",
    "�No se puede iniciar el proceso de edici�n!",
    "�������� ��������� ��� ������������!",
    "Kan inte starta redigering!",
    "Nu pot porni montajul �nregistr�rii!",
    "A v�g�s nem ind�that�!",
    "No puc iniciar el proc�s d'edici�!",
    "���������� ������ ������ ������!",
    "Ne mogu zapo�eti ure�ivanje!",
    "Redigeerimise start eba�nnestus!",
    "Kan ikke starte redigeringsprocessen!",
    "Nelze za��t edita�n� proces!",
  },
  { "Editing process already active!",
    "Schnitt bereits aktiv!",
    "Urejanje je �e aktivno!",
    "Processo di modifica gia' attivo!",
    "Bewerken is al actief!",
    "Processo de modifica��o j� activo!",
    "Montage d�j� en cours!",
    "Redigeringsprosessen er allerede aktiv!",
    "Muokkaus on jo k�ynniss�!",
    "Proces edycji jest ju� aktywny!",
    "�El proceso de edici�n ya est� activo!",
    "����������� ��������� �� �������!",
    "Redigeringen �r redan aktiv!",
    "Montajul �nregistr�rii este deja activ!",
    "A v�g�s m�r aktiv�lt!",
    "Proc�s d'edici� iniciat anteriorment!",
    "������� ������������ ��� �������!",
    "Ure�ivanje je ve� aktivirano!",
    "Redigeerimine juba aktiivne!",
    "Redigeringsproces er allerede aktiv!",
    "Edita�n� proces je ji� aktivn�!",
  },
  { "Can't shutdown - option '-s' not given!",
    "Ausschalten unm�glich - Option '-s' fehlt!",
    "Zaustavitev ni izvedljiva - opcija '-s' ni podana!",
    "Impossibile spegnere - parametro '-s' non passato!",
    "Shutdown onmogelijk - Optie '-s' ontbreekt!",
    "Imposs�vel desligar - falta a op��o '-s'!",
    "Arr�t impossible - option '-s' absente!",
    "Kan ikke sl� av - startet uten parameteret '-s'!",
    "Sammutus ei onnistu - '-s' parametri puuttuu!",
    "Nie mo�na wy��czy� - nie podano opcji '-s'!",
    "�No se puede apagar - falta el par�metro '-s'!",
    "�������� �� ����� �����������. ��������� � ���������� '-s'!",
    "Kan inte avsluta, m�ste anv�nda parameter '-s'",
    "Nu pot �nchide - vezi op�iunea '-s'",
    "A le�ll�t�s nem lehets�ges - Opci� '-s' hi�nyzik!",
    "No puc apagar, falta la opci� -s !",
    "���������� ���������� - �� ����� �������� '-s'!",
    "Ga�enje nemogu�e - nedostaje opcija '-s'!",
    "V�ljal�litamine eba�nnestus - '-s' parameeter puudub!",
    "Kan ikke slukke - parameter '-s' ikke angivet!",
    "Vypnut� nen� mo�n� - chb� volba '-s'!",
  },
  { "Not enough disk space to start recording!",
    "Nicht gen�gend Plattenplatz f�r Aufnahme!",
    "Premalo prostora na disku za snemanje!",
    "Spazio su disco insufficiente per registrare!",
    "",//TODO
    "",//TODO
    "Espace disque insuffisant pour d�marrer l'enregistrement!",
    "",//TODO
    "Tallennustilan puute est�� tallennuksen!",
    "Brak miejsca na dysku do nagrywania!",
    "�No hay suficiente espacio para grabar!",
    "",//TODO
    "Otillr�cklingt diskutrymme f�r inspelning!",
    "Insuficient spa�iul pe disc pentru �nregistrare!",
    "Nincs elegend� hely a felv�telre",
    "",//TODO
    "������������ ����� �� ����� ��� ������ ������",
    "",//TODO
    "Ebapiisavalt kettaruumi salvestamise alustamiseks!",
    "For lidt harddisk plads til optagelse!",
    "Nedostatek voln�ho m�sta k zapo�et� nahr�v�n�!",
  },
  { "Low disk space!",
    "Platte beinahe voll!",
    "Premalo prostora na disku!",
    "Poco spazio su disco!",
    "Disk bijna vol!",
    "Espa�o em disco reduzido!",
    "Disque presque plein!",
    "Lite ledig diskplass!",
    "Tallennustila loppumassa!",
    "Ma�o miejsca na dysku!",
    "�Poco espacio en disco!",
    "� ������� �������� �� �������!",
    "L�gt diskutrymme!",
    "Spa�iul pe disc e foarte sc�zut!",
    "A merev lemez majdnem tele!",
    "Disc gaireb� ple!",
    "������������ ����� �� �����!",
    "Nedovoljno prostora na disku",
    "K�vaketas t�is!",
    "Kun lidt diskplads tilbage!",
    "Disk bude brzy zapln�n!",
  },
  { "Opening CAM menu...",
    "CAM-Men� wird ge�ffnet...",
    "Odpiram CAM meni...",
    "Apertura menu CAM...",
    "CAM-menu wordt geopend...",
    "",//TODO
    "Ouverture du menu CAM",
    "",//TODO
    "Avataan CA-moduulin valikkoa...",
    "Otwieram menu CAM...",
    "Abriendo el men� CAM...",
    "",//TODO
    "�ppnar CAM menyn...",
    "Deschid meniul CAM...",
    "A CAM-men� nyit�s alatt...",
    "",//TODO
    "�������� ���� ������ ��������� ������� (CAM)",
    "",//TODO
    "CAM-men�� avamine...",
    "�bner CAM menu...",
    "Otev�r� se menu CAM...",
  },
  { "Can't open CAM menu!",
    "CAM-Men� kann nicht ge�ffnet werden!",
    "Ne morem odpreti CAM menija!",
    "Impossibile aprire il menu CAM!",
    "Kan CAM-menu niet openen!",
    "",//TODO
    "Impossible d'ouvrir le menu CAM!",
    "",//TODO
    "CA-moduulin valikko ei saatavilla",
    "Nie mo�na otworzy� menu CAM!",
    "�No se puede abrir el men� CAM!",
    "������� � �������� ��� CAM �����!",
    "Det g�r inte att �ppna CAM menyn!",
    "Nu pot deschide meniul CAM",
    "A CAM-Men� nem nyithat�",
    "No puc obrir el men� de la CAM!",
    "���� CAM-������ ����������!",
    "Ne mogu otvoriti CAM izbor!",
    "Ei saa avada CAM men��d!",
    "Kan ikke �bne CAM menuen!",
    "Menu CAM nen� dostupn�",
  },
  { "Resetting CAM...",
    "CAM wird zur�ckgesetzt...",
    "Resetiram CAM...",
    "Reimpostazione modulo CAM...",
    "CAM wordt herstart...",
    "",//TODO
    "R�initialisation du CAM",
    "",//TODO
    "CA-moduuli palautetaan alkutilaan...",
    "Resetuj� CAM...",
    "Reiniciando CAM...",
    "",//TODO
    "�terst�ller CAM ...",
    "Se reseteaz� CAM...",
    "A CAM �jra indul...",
    "",//TODO
    "���������� CAM...",
    "",//TODO
    "CAM mooduli taask�ivitus...",
    "Nulstiller CAM...",
    "Restartuje se CAM...",
  },
  { "Can't reset CAM!",
    "Zur�cksetzen des CAM fehlgeschlagen!",
    "Ne morem resetirati CAM-a!",
    "Impossibile reimpostare il modulo CAM!",
    "Kan CAM niet herstarten!",
    "",//TODO
    "Impossible de r�initialiser la CAM!",
    "",//TODO
    "CA-moduulin palautus alkutilaan ep�onnistui!",
    "Nie mo�na zresetowa� CAM!",
    "�No se puede reiniciar CAM!",
    "������� � ��������� ��� CAM",
    "Kan inte �terst�lla CAM!",
    "Nu pot reseta CAM",
    "A CAM-Reset nem siker�lt",
    "No puc reiniciar la CAM!",
    "������ ����������� CAM-������!",
    "Ne mogu resetirati CAM",
    "CAM mooduli taask�ivitus eba�nnestus!",
    "Kan ikke nulstille CAM!",
    "CAM modul nelze restartovat!",
  },
  { "CAM has been reset",
    "CAM wurde zur�ckgesetzt",
    "CAM je resetiran",
    "Modulo CAM reimpostato",
    "CAM is herstart",
    "",//TODO
    "La CAM a �t� r�initialis�e",
    "",//TODO
    "CA-moduuli palautettu alkutilaan",
    "CAM zosta� zresetowany",
    "CAM reiniciado",
    "����� ��������� ��� CAM",
    "CA modulen har �terst�llts",
    "CAM-ul a fost resetat",
    "A CAM vissza lett �ll�tva",
    "CAM reiniciada",
    "CAM-������ �����������",
    "CAM je resetiran",
    "CAM mooduli taask�ivitus tehtud",
    "CAM er blevet nulstillet",
    "CAM byl restartov�n",
  },
  { "Please enter %d digits!",
    "Bitte geben Sie %d Ziffern ein!",
    "Prosim vnesite %d �tevilk!",
    "Inserire %d cifre!",
    "Vul %d cijfers in!",
    "",//TODO
    "Veuillez entrer %d chiffres!",
    "",//TODO
    "Sy�t� %d numeroa!",
    "Prosz� poda� %d cyfr!",
    "�Escriba %d d�gitos!",
    "",//TODO
    "Mata in %d siffror!",
    "V� rog introduce�i %d cifre!",
    "�ss�n be %d sz�mot!",
    "",//TODO
    "������� %d �����",
    "",//TODO
    "Palun sisesta %d numbrit!",
    "Indtast venligst %d cifre!",
    "Pros�m vlo�te %d znak�!",
  },
  { "No audio available!",
    "Kein Audio verf�gbar!",
    "Zvok ni dosegljiv!",
    "Nessun audio disponibile!",
    "Geen audio beschikbaar!",
    "",//TODO
    "",//TODO
    "Pas d'audio disponible!",
    "��nen kieli ei ole valittavissa!",
    "D�wi�k nie jest dost�pny!",
    "�No hay audio disponible!",
    "�� ���������� ����",
    "Ljud saknas!",
    "Lipse�te sunetul!",
    "Hang nem lehets�ges!",
    "",//TODO
    "����������� ����!",
    "Audio nedostupan",
    "Audio k�ttesaamatu!",
    "Ingen lyd tilg�ngelig!",
    "Zvuk nen� dostupn�!",
  },
  // Setup pages:
  { "OSD",
    "OSD",
    "OSD",
    "OSD",
    "OSD",
    "OSD",
    "Affichage des menus",
    "OSD",
    "Kuvaruutun�ytt�",
    "OSD",
    "Men�s en pantalla",
    "OSD",
    "OSD",
    "OSD",
    "OSD",
    "Informaci� en pantalla",
    "����",
    "OSD",
    "Ekraanimen��",
    "OSD",
    "OSD",
  },
  { "EPG",
    "EPG",
    "Programski vodnik",
    "Guida programmi EPG",
    "EPG",
    "EPG",
    "Guide des programmes",
    "Programoversikt",
    "Ohjelmaopas",
    "EPG",
    "Gu�a de programaci�n",
    "������������ ������ ������������",
    "EPG",
    "EPG",
    "EPG",
    "Guia de Programes",
    "�������",
    "Pregled emisija (EPG)",
    "EPG",
    "EPG",
    "EPG",
  },
  { "EPG Channel Modes",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Modos EPG por canal",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Channel Default Mode",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Modo por defecto para canales",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Channel Inverse Mode",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Modo inverso para canales",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Channel Limit",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "L�mite para canales",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Default Mode",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Por defecto",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Do not save",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "No guardar",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Foreign EPG",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "EPG externa",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Full EPG",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "EPG completa",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Inverse Mode",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Modo inverso",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$No EPG",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Sin EPG",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Now and Next only",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Ahora y Despu�s",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "DVB",
    "DVB",
    "DVB",
    "Scheda DVB",
    "DVB",
    "DVB",
    "Cartes DVB",
    "DVB-enheter",
    "DVB",
    "DVB",
    "DVB",
    "DVB",
    "DVB",
    "Dispozitiv DVB",
    "DVB",
    "Tarja DVB",
    "DVB",
    "DVB",
    "DVB",
    "DVB",
    "DVB",
  },
  { "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
    "Configuraci� de l'LNB",
    "���������",
    "LNB",
    "LNB",
    "LNB",
    "LNB",
  },
  { "CICAM",
    "CICAM",
    "CICAM",
    "Accesso condizionato CICAM",
    "CICAM",
    "CICAM",
    "Acc�s conditionnel",
    "CICAM",
    "CICAM",
    "CICAM",
    "CICAM",
    "CICAM",
    "CICAM",
    "CICAM",
    "CICAM",
    "CI Acc�s condicional",
    "�������� ������",
    "CICAM",
    "CICAM",
    "CICAM",
    "CICAM",
  },
  { "Recording",
    "Aufnahme",
    "Snemanje",
    "Registrazione",
    "Opname",
    "A gravar",
    "Enregistrement",
    "Opptak",
    "Tallennus",
    "Nagranie",
    "Opciones de grabaci�n",
    "�������",
    "Inspelning",
    "�nregistrare",
    "Felv�tel",
    "Opcions de Gravaci�",
    "������",
    "Snimanje",
    "Salvestamine",
    "Optagelse",
    "Nahr�v�n�",
  },
  { "Cutter",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Opciones de edici�n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Replay",
    "Wiedergabe",
    "Predvajanje",
    "Riproduzione",
    "Afspelen",
    "Replay",
    "Lecture",
    "Spill av",
    "Toisto",
    "Odtwarzanie",
    "Opciones de reproducci�n",
    "�����������",
    "Uppspelning",
    "Redare",
    "Lej�tsz�s",
    "Opcions de Reproducci�",
    "���������������",
    "Reprodukcija",
    "Taasesitus",
    "Afspilning",
    "P�ehr�v�n�",
  },
  { "Miscellaneous",
    "Sonstiges",
    "Ostalo",
    "Generici",
    "Overig",
    "Outros",
    "Divers",
    "Forskjellig",
    "Sekalaiset",
    "R�ne",
    "Varios",
    "�������",
    "Diverse",
    "Diverse",
    "Egy�b",
    "Miscel�l�nia",
    "������",
    "Ostalo",
    "Muud s�tted",
    "Diverse",
    "R�zn�",
  },
  { "Plugins",
    "Plugins",
    "Vstavki",
    "Plugins",
    "Plugins",
    "Plugins",
    "Plugins",
    "Plugins",
    "Laajennokset",
    "Wtyczki",
    "Plugins",
    "����������",
    "Moduler",
    "Plugin-uri",
    "Plugins",
    "Plugins",
    "������ ����������",
    "Dodaci (Plugins)",
    "Laiendusmoodulid",
    "Plugins",
    "Moduly",
  },
  { "Plugin",
    "Plugin",
    "Vstavek",
    "Plugin",
    "Plugin",
    "Plugin",
    "Plugin",
    "Plugin",
    "Laajennos",
    "Wtyczka",
    "Plugin",
    "��������",
    "Modul",
    "Plugin (modul adi�ional)",
    "Plugin",
    "Plugin",
    "������",
    "Plugin",
    "Laiendusmoodul",
    "Plugin",
    "Modul",
  },
  { "Restart",
    "Neustart",
    "Ponoven zagon",
    "Riavvio",
    "Herstart",
    "Reiniciar",
    "Red�marrer",
    "Start p� nytt",
    "K�ynnist� uudelleen",
    "Restart",
    "Reiniciar",
    "������������",
    "Omstart",
    "Repornire",
    "�jraind�t�s",
    "Reiniciar",
    "�������������",
    "Restart sistema",
    "Restart",
    "Genstart",
    "Restart",
  },
  // Setup parameters:
  { "Setup.OSD$Language",
    "Sprache",
    "Jezik",
    "Lingua",
    "Taal",
    "Linguagem",
    "Langue",
    "Spr�k",
    "Kieli",
    "J�zyk",
    "Idioma",
    "������",
    "Spr�k",
    "Limba OSD",
    "Nyelv",
    "Idioma",
    "����",
    "Jezik",
    "Keel",
    "Sprog",
    "Jazyk",
  },
  { "Setup.OSD$Skin",
    "Oberfl�che",
    "Preobleka",
    "Stile interfaccia",
    "Skin",
    "",// TODO
    "Skin",
    "",// TODO
    "Ulkoasu",
    "Sk�rka",
    "Skin",
    "���������",
    "Skin",
    "Skin",
    "Men� n�zet",
    "",// TODO
    "�����",
    "Povr�ina",
    "Kest",
    "Skin",
    "Vzhled",
  },
  { "Setup.OSD$Theme",
    "Thema",
    "Tema",
    "Tema colori",
    "Thema",
    "",// TODO
    "Th�me",
    "",// TODO
    "Teema",
    "Motyw",
    "Tema",
    "����",
    "Tema",
    "Tem�",
    "T�ma",
    "",// TODO
    "�������",
    "Tema",
    "Teema",
    "Tema",
    "T�ma",
  },
  { "Setup.OSD$WarEagle icons",
    "WarEagle-Symbole",
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "", // Suomi
    "", // Polski
    "Iconos WarEagle",
    "", // Greek
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
#if VDRVERSNUM > 10302                                                         
    "", // Russian
#if VDRVERSNUM > 10307                                                         
    "", // Hrvatski
#if VDRVERSNUM > 10313                                                         
    "", // Eesti
#if VDRVERSNUM > 10316                                                         
    "", // Dansk
#endif
#endif
#endif
#endif
  },  
  { "Setup.OSD$Left",
    "Links",
    "Levo",
    "Sinistra",
    "Links",
    "",// TODO
    "Gauche",
    "",// TODO
    "Vaakakeskitys",
    "Od lewej",
    "Izquierda",
    "��������",
    "V�nster",
    "St�nga",
    "Balra",
    "",// TODO
    "������ �����",
    "Lijevo",
    "Vasakule",
    "Venstre",
    "Vlevo",
  },
  { "Setup.OSD$Top",
    "Oben",
    "Zgoraj",
    "In alto",
    "Boven",
    "",// TODO
    "Haut",
    "",// TODO
    "Pystykeskitys",
    "Od g�ry",
    "Arriba",
    "�����",
    "�vre",
    "Sus",
    "Fent",
    "",// TODO
    "������ ������",
    "Gore",
    "�lesse",
    "Top",
    "Naho�e",
  },
  { "Setup.OSD$Width",
    "Breite",
    "�irina",
    "Larghezza OSD",
    "Breedte",
    "Largura",
    "Largeur",
    "Bredde",
    "Leveys",
    "Szeroko��",
    "Anchura",
    "������",
    "Bredd",
    "L��ime OSD",
    "Sz�less�g",
    "Amplada",
    "������",
    "�irina",
    "Laius",
    "Bredde",
    "���ka",
  },
  { "Setup.OSD$Height",
    "H�he",
    "Vi�ina",
    "Altezza OSD",
    "Hoogte",
    "Altura",
    "Hauteur",
    "H�yde",
    "Korkeus",
    "Wysoko��",
    "Altura",
    "����",
    "H�jd",
    "�n�l�ime OSD",
    "Magass�g",
    "Al�ada",
    "������",
    "Visina",
    "K�rgus",
    "H�jde",
    "V��ka",
  },
  { "Setup.OSD$Message time (s)",
    "Anzeigedauer f�r Nachrichten (s)",
    "�as prikaza sporo�ila (s)",
    "Tempo del messaggio (s)",
    "Weergave duur van berichten (s)",
    "Mostrar contador (s)",
    "Dur�e affichage message (s)",
    "Tid meldinger skal vises (s)",
    "Viestien esitysaika (s)",
    "Czas trwania wiadomo�ci (s)",
    "Duraci�n de los mensajes (sg)",
    "������ �������� ��������� (�)",
    "Tid f�r meddelanden (sek)",
    "Timp afi�are mesaje (sec)",
    "Inform�ci� felt�ntet�s�nek id�tartama",
    "Durada dels missatges (s)",
    "������������ ������ ��������� (���)",
    "Vrijeme prikaza poruka (s)",
    "Teate esitusaeg (s)",
    "Tid beskeder skal vises (s)",
    "�as zobrazen� zp�vy (s)",
  },
  { "Setup.OSD$Use small font",
    "Kleine Schrift benutzen",
    "Uporabi male �rke",
    "Utilizzare caratteri piccoli",
    "Klein lettertype gebruiken",
    "",// TODO
    "Utiliser les petits caract�res",
    "",// TODO
    "K�yt� pieni� kirjasimia",
    "U�ywaj ma�ej czcionki",
    "Usar letra peque�a",
    "������������� ������ ��������������",
    "Anv�nd liten font",
    "Utilizare fonturi mici",
    "Kisbet�t haszn�lni",
    "",// TODO
    "������������ ������ �����",
    "Koristi mala slova",
    "V�ikese fondi kasutamine",
    "Anvend lille skrift",
    "Pou��vat mal� p�smo",
  },
  { "never",
    "nie",
    "nikoli",
    "mai",
    "nooit",
    "",// TODO
    "jamais",
    "",// TODO
    "ei koskaan",
    "nigdy",
    "nunca",
    "����",
    "aldrig",
    "niciodat�",
    "soha",
    "",// TODO
    "�������",
    "nikada",
    "mitte kunagi",
    "aldrig",
    "nikdy",
  },
  { "skin dependent",
    "je nach Oberfl�che",
    "odvisno od preobleke",
    "in base allo stile",
    "skin afhankelijk",
    "",// TODO
    "d�pend du skin",
    "",// TODO
    "ulkoasun mukaan",
    "zal. od sk�rki",
    "seg�n skin",
    "�������� ��� ��� ���������",
    "skin beroende",
    "dep. de skin",
    "Men� n�zet�t�l f�gg�en",
    "",// TODO
    "�������� ����",
    "ovisno o povr�ini",
    "kestast s�ltuv",
    "skin afh�ngig",
    "podle vzhledu",
  },
  { "always",
    "immer",
    "vedno",
    "sempre",
    "altijd",
    "",// TODO
    "toujours",
    "",// TODO
    "aina",
    "zawsze",
    "siempre",
    "�����",
    "alltid",
    "�ntotdeauna",
    "mindig",
    "",// TODO
    "������",
    "uvijek",
    "alati",
    "altid",
    "v�dy",
  },
  { "Setup.OSD$Channel info position",
    "Kanalinfo-Position",
    "Pozicija informacije o kanalu",
    "Posizione informazioni canale",
    "Kanaal info positie",
    "Posi��o de info dos canais",
    "Position infos cha�nes",
    "Posisjon p� kanalinformasjon",
    "Kanavatiedon sijainti",
    "Pozycja informacji o kanale",
    "Posici�n para informaci�n de canal",
    "���� ����������� ��������",
    "Placering av kanalinformation",
    "Pozi�ia informa�iilor despre canal",
    "Ad�sinform�ci� pozici�ja",
    "Posici� de la informaci� del canal",
    "��������� ���� ���������� � ������",
    "Pozicija informacije o programu",
    "Kanaliinfo asukoht",
    "Placering af kanalinfo",
    "Pozice informace o kan�lu",
  },
  { "Setup.OSD$Channel info time (s)",
    "Anzeigedauer f�r Kanalinfo (s)",
    "�as prikaza podatka o kanalu (s)",
    "Durata informazioni canale (s)",
    "Duur tonen kanaal-informatie (s)",
    "",// TODO
    "Dur�e affichage infos cha�nes (s)",
    "",// TODO
    "Kanavatiedon esitysaika (s)",
    "Czas trwania informacji o kanale (s)",
    "Tiempo de informaci�n de canal (sg)",
    "������ ��������� ����������� �������� �� (�)",
    "Kanal information (s)",
    "Durata afi��rii info-canal (s)",
    "Ad�sinform�ci� felt�ntet�s�nek id�tartama (s)",
    "",// TODO
    "����� ���������� � ������ (���)",
    "Vrijeme prikaza informacije o programu (s)",
    "Kanaliinfo esitusaeg (s)",
    "Tid kanalinfo skal vises (s)",
    "�as zobr. informace o kan�lu (s)",
  },
  { "Setup.OSD$Info on channel switch",
    "Info beim Kanalwechsel",
    "Poka�i naziv kanala",
    "Mostrare informazioni al cambio canale",
    "Kanaal info tonen",
    "Mostrar info ao mudar de Canal",
    "Affichage progr. en cours",
    "Info ved kanalskifte",
    "N�yt� kanavatieto vaihdettaessa",
    "Informacja przy zmianie kana�u",
    "Mostrar info al cambiar de canal",
    "����������� ���� ������ ��������",
    "Information vid kanalbyte",
    "Informa�ii la comutarea canalului",
    "Inform�ci� az ad� v�lt�s�n�l",
    "Informaci� del canvi de canal",
    "���������� ���������� � ������",
    "Informacije kod promjene kanala",
    "Kanaliinfo n�itamine",
    "Info ved kanalskift",
    "Informace p�i zm�n� kan�lu",
  },
  { "Setup.OSD$Timeout requested channel info",
    "Angeforderte Kanalinfo schlie�en",
    "�as prikaza informacije o kanalu",
    "Scadenza informazioni canale richieste",
    "", // TODO
    "", // TODO
    "Demande d'infos de cha�ne termin�e",
    "", // TODO
    "Sulje erikseen avattu kanavatieto",
    "Czas oczekiwania na informacje",
    "Auto-cerrar al solicitar info de canal",
    "", // TODO
    "Timeout f�r kanalinformation",
    "Durata afi��rii informa�ii canal",
    "A lek�rdezett ad�sinform�ci� bez�r�sa",
    "", // TODO
    "���������� � ������ �������",
    "", // TODO
    "Kanaliinfo kuvamise ajapiirang",
    "Timeout ved anmodet kanal info",
    "�asov� limit informace o kan�lu",
  },
  { "Setup.OSD$Scroll pages",
    "Seitenweise scrollen",
    "Drsni meni",
    "Scorrere per pagine",
    "Scrollen per pagina",
    "Scroll da p�gina no menu",
    "D�filement par pages",
    "Rask rulling i menyer",
    "Valikoiden vieritys sivuttain",
    "Przesuwaj stronami",
    "Deslizar p�ginas enteras",
    "������ �������",
    "Bl�ddra sidor",
    "Deruleaz� pagini",
    "Oldalank�nt l�ptetmi",
    "Despla�ar p�gina sencera",
    "��������� ������� ����",
    "Listaj po stranicama",
    "Lehek�lje kerimine",
    "Scroll sidevis",
    "Rolovat strany",
  },
  { "Setup.OSD$Scroll wraps",
    "Rundum scrollen",
    "Iz konca sko�i na za�etek",
    "Scorrere riavvolgendo",
    "Rondom scrollen",
    "",// TODO
    "D�filement rotatif",
    "",// TODO
    "Valikoiden vieritys ymp�ri",
    "Skrajne pozycje s�siaduj�",
    "Rotar arriba/abajo al deslizar",
    "������ ����-����",
    "Rulla texten",
    "Derulare circular�",
    "Korl�tlan l�ptet�s",
    "",// TODO
    "����������� ���������",
    "S kraja sko�i na po�etak",
    "Ridade kerimine",
    "Scroll rundt",
    "Z konce na za��tek",
  },
  { "Setup.OSD$Menu button closes",
    "Menu-Taste schlie�t",
    "Meni gumb zapre meni",
    "Usare il bottone Menu per chiudere",
    "",// TODO
    "",// TODO
    "Menu bouton ferm�",
    "",// TODO
    "Sulje valikkon�pp�imell�",
    "Przycisk Menu zamyka",
    "Cerrar al pulsar bot�n de men�",
    "",// TODO
    "Menyknappen st�nger",
    "Tasta 'Meniu' �nchide",
    "A men� gomb z�r",
    "",// TODO
    "������ ���� �������",
    "",// TODO
    "Sulgemine Men�� klahviga",
    "Menu taste lukker",
    "Kl�vesa Menu zav�r�",
  },
  { "Setup.OSD$Recording directories",
    "Aufnahmeverzeichnisse",
    "Direktoriji za posnetke",
    "Directory di registrazione",
    "Opname directories",
    "Directorias de grava��o",
    "Dossiers d'enregistrements",
    "Kataloger til opptak",
    "N�yt� tallennehakemistot",
    "Katalogi nagra�",
    "Mostrar directorios de grabaciones",
    "������� ��������",
    "Kataloger f�r inspelningar",
    "Directoare �nregistr�ri",
    "Felv�telek list�ja",
    "Gravacions en subcarpetes",
    "�������� �������� �������",
    "Imenik za snimke",
    "Kausta nime salvestamine",
    "Optagelser i foldere",
    "Seznam nahr�vek",
  },
  { "Setup.EPG$EPG scan timeout (h)",
    "Zeit bis zur EPG-Aktualisierung (h)",
    "�as do EPG pregleda (h)",
    "Timeout scansione aggiorn. EPG (ore)",
    "EPG-scan Timeout (h)",
    "Expirou o EPG (h)",
    "Inactivit� avant rech. EPG (h)",
    "Ledig tid f�r EPG-s�k (h)",
    "Ohjelmaoppaan taustap�ivitys (h)",
    "Czas skanowania EPG (h)",
    "Tiempo de exploraci�n de EPG (h)",
    "������ ��������� �������� EPG �� ����",
    "EPG s�kning timeout",
    "Interval achizi�ie EPG (h)",
    "Fennmaradt id� az EPG-�j�t�sig (h)",
    "M�xim d'Hores a cercar per la Guia",
    "�������� ������������ �������� (�)",
    "Vrijeme do EPG pregleda (h)",
    "EPG skaneerimise viide (h)",
    "Tid f�r EPG skanning (t)",
    "�asov� limit pro sn�m�n� EPG (h)",
  },
  { "Setup.EPG$EPG bugfix level",
    "EPG-Fehlerbereinigung",
    "Nivo za popravilo EPG napak",
    "Livello correzione EPG",
    "EPG foutcorrectieniveau",
    "N�vel de correct��o EPG",
    "Niveau de correction EPG",
    "Niv� for EPG-feilretting",
    "Ohjelmaoppaan korjaustaso",
    "Stopie� poprawek b��d�w EPG",
    "Nivel de correcci�n de EPG",
    "������ ��������� ������ EPG",
    "Niv� f�r EPG bugfix",
    "Nivel corec�ie EPG",
    "EPG hibaelh�r�t�s",
    "Nivell de correcci� de la Guia",
    "������� ��������� ������",
    "Nivo popravka EPG gre�aka",
    "EPG veaparanduse tase",
    "Niveau for EPG fejlrettelse",
    "EPG �rove� chyb",
  },
  { "Setup.EPG$EPG linger time (min)",
    "Alte EPG-Daten anzeigen (min)",// TODO
    "Prika�i stare EPG podatke (min)",
    "Visualizzazione dati vecchi (min)",
    "Oude EPG data tonen (min)",
    "",// TODO
    "Montrer l'EPG p�rim� (min)",
    "",// TODO
    "Vanha tieto n�kyy (min)",
    "Czas przechowywania EPG (min)",
    "Mostrar datos antiguos de EPG (m)",
    "������� ������������ ����������� (�����)",
    "Visa gammal information (min)",
    "Date EPG expirate cel mult (min)",
    "R�gi EPG adatok megmutat�sa (perc)",
    "",// TODO
    "�������� ���������� ������ (���)",
    "Prika�i stare EPG podatke (min)",
    "Vana EPG viide (min)",
    "Vise gammel EPG info (min)",
    "Ukazovat star�� EPG data (min)",
  },
  { "Setup.EPG$Set system time",
    "Systemzeit stellen",
    "Nastavi sistemski �as",
    "Settare orario automatico",
    "Systeem klok instellen",
    "Ajustar rel�gio do sistema",
    "Ajuster l'heure du syst�me",
    "Juster system-klokken",
    "Tahdista kellonaika",
    "Ustawiaj czas systemowy",
    "Ajustar reloj de sistema",
    "����������� ���� ����������",
    "St�ll in systemtid",
    "Potrive�te ceasul sistem",
    "Az id� be�ll�t�sa",
    "Ajustar l'hora del sistema",
    "���������� ��������� �����",
    "Podesi vrijeme sistema",
    "Kella s�nkroniseerimine",
    "Indstil system tid",
    "Nastavit syst�mov� �as",
  },
  { "Setup.EPG$Use time from transponder",
    "Transponder f�r Systemzeit",
    "Transponder za nastavitev �asa",
    "Utilizzare orario da transponder",
    "Gebruik transponder tijd voor systeem",
    "Usar rel�gio do transponder",
    "Utiliser l'heure de la cha�ne",
    "Bruk klokke fra transponder",
    "Vastaanota kellonaika transponderilta",
    "U�yj czasu z transpondera",
    "Transponder del que tomar la hora",
    "������������ ����������� ����",
    "Anv�nd klockan fr�n transponder",
    "Preia ora din transponder",
    "Id�h�z tartoz� Transponder",
    "Usar el temps del m�ltiplex",
    "������������ ����� ������������",
    "Transponder za pode�avanje sata",
    "Kella s�nkroniseerimise transponder",
    "Anvend tid fra transponder",
    "Pou��t �as z kan�lu",
  },
  { "Setup.EPG$Preferred languages", // note the plural
    "Bevorzugte Sprachen",
    "Prednostni jeziki",
    "Lingue preferite",
    "Voorkeurstalen",
    "",// TODO
    "Langues pr�f�r�es",
    "",// TODO
    "Suosikkikielet",
    "Preferowanych j�zyk�w",
    "Idiomas preferidos",
    "������������� �������",
    "�nskade spr�k",
    "Limbi preferate",
    "Kedvenc nyelvek",
    "",// TODO
    "�������������� ����� (�������)",
    "Preferirani jezici",
    "Eelistatuid keeli",
    "Foretrukne sprog",
    "Preferovan� jazyky",
  },
  { "Setup.EPG$Preferred language", // note the singular
    "Bevorzugte Sprache",
    "Prednostni jezik",
    "Lingua preferita",
    "Voorkeurstaal",
    "",// TODO
    "Langue pr�f�r�e",
    "",// TODO
    "Suosikkikieli",
    "Preferowany j�zyk",
    "Idioma preferido",
    "������������ ������",
    "�nskat spr�k",
    "Limba preferat�",
    "Kedvenc nyelv",
    "",// TODO
    "������",
    "Preferirani jezik",
    "Eelistatud keel",
    "Foretrukket sprog",
    "Preferovan� jazyk",
  },
  { "Setup.DVB$Primary DVB interface",
    "Prim�res DVB-Interface",
    "Primarna naprava",
    "Scheda DVB primaria",
    "Eerste DVB kaart",
    "Interface DVB prim�rio",
    "Carte DVB primaire",
    "Hoved DVB-enhet",
    "Ensisijainen DVB-sovitin",
    "Pierwszy interfejs DVB",
    "Interfaz DVB primario",
    "����� DVB �����",
    "Prim�r DVB enhet",
    "Dispozitiv DVB primar",
    "Els� DVB interface",
    "Tarja DVB prim�ria",
    "�������� DVB-����������",
    "Primarni DVB ure�aj",
    "Esmane DVB seade",
    "Prim�r DVB enhed",
    "Prim�rn� DVB interface",
  },
  { "Setup.DVB$Video display format",
    "Video-Anzeigeformat",
    "Format video prikaza",
    "Formato di visualizz. video",
    "Video display formaat",
    "",//TODO
    "Format d'affichage",
    "",//TODO
    "N�ytt�muoto",
    "Format wy�wietlania obrazu",
    "Formato de visualizaci�n",
    "����� ����������� ������",
    "Format f�r video display",
    "Formatul red�rii video",
    "K�perny�form�tum",
    "",//TODO
    "�������������� �����������",
    "Format video prikaza",
    "Video kuvamise formaat",
    "Video display format",
    "Form�t zobrazen� videa",
  },
  { "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "��������������",
    "pan&scan",
    "pan&scan",
    "pan&scan",
    "pan&scan",
  },
  { "letterbox",
    "Letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
    "���������",
    "letterbox",
    "letterbox",
    "letterbox",
    "letterbox",
  },
  { "center cut out",
    "Center-cut-out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "center cut out",
    "�������� �����",
    "center cut out",
    "center cut out",
    "center cut out",
    "dost�edu o��znout",
  },
  { "Setup.DVB$Video format",
    "Videoformat",
    "Video format",
    "Formato video",
    "Videoformaat",
    "Formato v�deo",
    "Format vid�o",
    "TV-Format",
    "Kuvasuhde",
    "Format obrazu",
    "Formato de v�deo",
    "����� ������",
    "Video format",
    "Format video",
    "Video form�tum",
    "Format del v�deo",
    "������ �����",
    "Video format",
    "TV k�lgsuhe",
    "Video format",
    "Form�t videa",
  },
  { "Setup.DVB$Use Dolby Digital",
    "Dolby-Digital-Ton benutzen",
    "Uporabljaj Dolby Digital",
    "Dolby Digital",
    "Dolby Digital gebruiken",
    "",//TODO
    "Utiliser le Dolby Digital",
    "",//TODO
    "K�yt� Dolby Digital -��nt�",
    "U�ywaj Dolby Digital",
    "Usar Dolby Digital",
    "����� ���� Dolby Digital",
    "Anv�nd Dolby Digital",
    "Sunet Dolby Digital",
    "Dolby-Digital hangot haszn�lni",
    "",//TODO
    "�������� Dolby Digital",
    "Koristi Dolby Digital",
    "Dolby Digital kasutamine",
    "Anvend Dolby Digital",
    "Pou��vat zvuk v Dolby Digital",
  },
  { "Setup.Recording$Record Dolby Digital",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Grabar Dolby Digital",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Setup.DVB$Update channels",
    "Kan�le aktualisieren",
    "Posodobi kanale",
    "Aggiornare i canali",
    "Kanalen actualiseren",
    "",// TODO
    "Mettre � jour les cha�nes",
    "",// TODO
    "P�ivit� kanavat",
    "Aktualizuj kana�y",
    "Actualizar canales",
    "��������� ��������",
    "Uppdatera kanaler",
    "Actualizare canale",
    "Ad�k aktualiz�l�sa",
    "",// TODO
    "��������� ��������� �������",
    "Aktualiziraj programe",
    "Kanalite uuendamine",
    "Opdat�r kanaler",
    "Aktualizace kan�l�",
  },
  { "names only",
    "nur Namen",
    "samo imena",
    "solo nomi",
    "alleen namen",
    "",// TODO
    "noms uniquement",
    "",// TODO
    "vain nimet",
    "tylko nazwy",
    "s�lo nombres",
    "���� �������",
    "bara namn",
    "doar numele",
    "kiz�r�lag nevek",
    "",// TODO
    "������ ��������",
    "samo imena",
    "ainult nimed",
    "kun navne",
    "jen n�zvy",
  },
  { "PIDs only",
    "nur PIDs",
    "samo PIDe",
    "solo PID",
    "",// TODO
    "",// TODO
    "PIDs uniquement",
    "",// TODO
    "vain PID:it",
    "tylko PID-y",
    "S�lo PIDs",
    "",// TODO
    "bara PIDs",
    "Numai PID-uri",
    "kiz�r�lag PID-k",
    "",// TODO
    "������ PID�",
    "",// TODO
    "ainult PID-id",
    "kun PIDs",
    "jen PIDs",
  },
  { "names and PIDs",
    "Namen und PIDs",
    "imena in PIDe",
    "nomi e PID",
    "namen en PIDs",
    "",// TODO
    "noms et PIDs",
    "",// TODO
    "nimet ja PID:it",
    "nazwy i PID-y",
    "nombres y PIDs",
    "������� ��� PIDs",
    "namn och PID",
    "nume si PID-uri",
    "nevek �s PID-k",
    "",// TODO
    "�������� � PID�",
    "imena i identifikatore (PIDs)",
    "nimi ja PID-id",
    "navne og PIDs",
    "n�zvy a PIDs",
  },
  { "add new channels",
    "neue Kan�le hinzuf�gen",
    "dodaj nove programe",
    "nuovi canali",
    "nieuwe kanalen toevoegen",
    "",// TODO
    "ajouter cha�nes",
    "",// TODO
    "uudet kanavat",
    "dodawaj nowe kana�y",
    "a�adir canales",
    "�������� ���� ��������",
    "l�gg till nya kanaler",
    "ad�ugare canale noi",
    "�j ad�k hozz�ad�sa",
    "",// TODO
    "����� ������",
    "dodaj nove programe",
    "uute kanalite lisamine",
    "tilf. ny kanaler",
    "p�idat nov� kan�ly",
  },
  { "add new transponders",
    "neue Transponder hinzuf�gen",
    "dodaj nove oddajnike",
    "nuovi transponder",
    "nieuwe transponders toevoegen",
    "",// TODO
    "ajouter transpondeurs",
    "",// TODO
    "uudet transponderit",
    "dodawaj nowe transpondery",
    "a�adir transponders",
    "�������� ���� �����������",
    "l�gg till nya transponders",
    "ad�ugare transpondere noi",
    "�j transponder hozz�ad�sa",
    "",// TODO
    "���. ������������",
    "dodaj nove transpondere", // hrv TODO transponderi == odasilja�i?
    "uute transponderite lisamine",
    "tilf. ny transp.",
    "p�idat nov� transpond�ry",
  },
  { "All channels",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Todos los canales",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Show only tv channels",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Mostrar s�lo canales de TV",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Show only radio channels",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Mostrar s�lo canales de radio",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Setup.DVB$Audio languages",  // note the plural
    "Audio-Sprachen",
    "Jeziki za zvok",
    "Lingue per l'Audio",
    "Audio talen",
    "",//TODO
    "Langues audio",
    "",//TODO
    "��nen kielet",
    "J�zyk�w d�wi�ku",
    "Idiomas de audio",
    "������� ����",
    "Antal ljudspr�k",
    "Limbi sunet",
    "Audio-nyelvek",
    "",//TODO
    "�������������� ����� (����)",
    "Audio jezici",
    "Audio keeli",
    "Audio sprog (ant.)",
    "Jazyky zvuku",
  },
  { "Setup.DVB$Audio language",  // note the singular
    "Audio-Sprache",
    "Jezik za zvok",
    "Lingua per l'Audio",
    "Audio taal",
    "",//TODO
    "Langue audio",
    "",//TODO
    "��nen kieli",
    "J�zyk d�wi�ku",
    "Idioma de audio",
    "������ ����",
    "Ljudspr�k",
    "Limba sunetului",
    "Audio-nyelv",
    "",//TODO
    "������",
    "Audio jezik",
    "Audio keel",
    "Audio sprog",
    "Jazyk zvuku",
  },
  { "Setup.LNB$SLOF (MHz)",
    "SLOF (MHz)",
    "Frekvenca preklopa (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
    "Limite de bandas LNB (MHz)",
    "Limite de bandes LNB (MHz)",
    "LO-grensefrekvens (MHz)",
    "LNB-rajataajuus (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
    "Frecven�� comutare band�, SLOF (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
    "������� ������������ (SLOF) (���)",
    "Frekv. preklopa SLOF (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
    "SLOF (MHz)",
  },
  { "Setup.LNB$Low LNB frequency (MHz)",
    "Untere LNB-Frequenz (MHz)",
    "Spodnja LNB-frek. (MHz)",
    "Freq LO LNB (MHz)",
    "Laagste LNB frequentie (MHz)",
    "Frequ�ncia base LNB (MHz)",
    "Fr�quence basse LNB (MHz)",
    "LO-frekvens i lavb�ndet (MHz)",
    "LNB-alataajuus (MHz)",
    "Dolna cz�stotliwo�� LNB (MHz)",
    "Frecuencia inferior del LNB (MHz)",
    "���� LNB-��������� (MHz)",
    "Undre LNB frekvens (MHz)",
    "Frecvn�� LNB inferioar� (Mhz)",
    "Als� LNB-frekvencia (MHZ)",
    "Freq��ncia LNB baixa (MHz)",
    "������ ������� ���������� (���)",
    "Donja LNB frekv. (MHz)",
    "Alumine (LO) LNB sagedus (MHz)",
    "Nedre LNB frekvens (MHz)",
    "Doln� frekvence LNB (MHz)",
  },
  { "Setup.LNB$High LNB frequency (MHz)",
    "Obere LNB-Frequenz (MHz)",
    "Zgornja LNB-frek. (MHz)",
    "Freq HI LNB (MHz)",
    "Hoogste LNB frequentie (MHz)",
    "Frequ�ncia alta LNB (MHz)",
    "Fr�quence haute LNB (MHz)",
    "LO-frekvens i h�yb�ndet (MHz)",
    "LNB-yl�taajuus (MHz)",
    "G�rna cz�stotliwo�� LNB (MHz)",
    "Frecuencia superior del LNB (MHz)",
    "��� LNB-��������� (MHz)",
    "�vre LNB frekvens (MHz)",
    "Frecvn�� LNB superioar� (MHz)",
    "Fels� LNB-frekvencia (MHZ)",
    "Freq��ncia LNB alta (MHz)",
    "������� ������� ���������� (���)",
    "Gornja LNB frekv. (MHz)",
    "�lemine (HI) LNB sagedus (MHz)",
    "�vre LNB frekvens (MHz)",
    "Horn� frekvence LNB (MHz)",
  },
  { "Setup.LNB$Use DiSEqC",
    "DiSEqC benutzen",
    "Uporabi DiSEqC",
    "Utilizza DiSEqC",
    "DiSEqC gebruiken",
    "Utilizar DiSEqC",
    "Utiliser le DiSEqC",
    "Bruk DiSEqC",
    "K�yt� DiSEqC-kytkint�",
    "U�ywaj DiSEqC",
    "Utilizar DiSEqC",
    "������������ DiSEqC",
    "Anv�nd DiSEqC",
    "Utilizare DiSEqC",
    "DiSEqC haszn�lata",
    "Utilitzar DiSEqC",
    "������������ DiSEqC",
    "Koristi DiSEqC",
    "DiSEqC kasutamine",
    "Anvend DiSEqC",
    "Pou��vat DiSEqC",
  },
  { "Setup.CICAM$CICAM DVB",
    "CICAM-DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "Acc�s conditionnel",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "Acc�s condicional CICAM",
    "CAM-������ DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
    "CICAM DVB",
  },
  { "Setup.Recording$Margin at start (min)",
    "Vorlauf zum Timer-Beginn (min)",
    "Premik za�etka snemanja (min)",
    "Margine iniziale (min)",
    "Tijd marge begin (min)",
    "Margem inicial (min)",
    "Marge ant�rieure (min)",
    "Opptaksmargin start (min)",
    "Aloitusmarginaali (min)",
    "Margines na pocz�tku (min)",
    "Comenzar grabaci�n antes (min)",
    "��������� ������ ���� ���� (�����)",
    "Marginal f�r start (min)",
    "Marj� la pornire (min)",
    "Id�eltol�d�s a kezd�sn�l (perc)",
    "Marge d'inici de gravaci� (min)",
    "���������� ������ ������ (���)",
    "Rezerva na po�etku snimanja (min)",
    "Salvestamise algusvaru (min)",
    "Margin ved start (min)",
    "Nahr�vat p�ed za��tkem (min)",
  },
  { "Setup.Recording$Margin at stop (min)",
    "Nachlauf am Timer-Ende (min)",
    "Zamik konca snemanja (min)",
    "Margine finale (min)",
    "Tijd marge eind (min)",
    "Margem final (min)",
    "Marge post�rieure (min)",
    "Opptaksmargin slutt (min)",
    "Lopetusmarginaali (min)",
    "Margines na ko�cu (min)",
    "Acabar grabaci�n despu�s de (min)",
    "��������� ������ ��� ����� (�����)",
    "Marginal f�r stopp (min)",
    "Marj� la oprire (min)",
    "Id�eltol�d�s a befejez�sn�l (perc)",
    "Marge de fi de gravaci� (min)",
    "������������ ��������� ������ (���)",
    "Rezerva na kraju (min)",
    "Salvestamise l�puvaru (min)",
    "Margin ved stop (min)",
    "Nahr�vat po konci (min)",
  },
  { "Setup.Recording$Primary limit",
    "Prim�r-Limit",
    "Primarna meja",
    "Limite primario",
    "Eerste grens",
    "Limite Prim�rio",
    "Limite primaire",
    "Prioritetsgrense HovedDVB",
    "Ensisijaisen sovittimen prioriteettiraja",
    "Podstawowy limit",
    "L�mite primario",
    "�������� ����",
    "Prim�r gr�ns",
    "Limit� primar�",
    "Prim�r-hat�r",
    "L�mit primari",
    "���. ��������� ������� ���. ����������",
    "Standardno ograni�enje",
    "Esmane prioriteet",
    "Prim�r gr�nse",
    "Prim�rn� limit",
  },
  { "Setup.Recording$Default priority",
    "Default-Priorit�t",
    "Privzeta prioriteta",
    "Priorita' predefinita",
    "Standaard prioriteit",
    "Prioridade por defeito",
    "Priorit� par d�faut",
    "Normal prioritet (Timer)",
    "Tallenteen oletusprioriteetti",
    "Domy�lny priorytet",
    "Prioridad por defecto",
    "�������������� �������������",
    "Normal prioritet",
    "Prioritate implicit�",
    "Default priority",
    "Prioritat per defecte",
    "��������� ������� �� ���������",
    "Standardni prioritet",
    "Vaikimisi prioriteet",
    "Standard prioritet",
    "V�choz� priorita",
  },
  { "Setup.Recording$Default lifetime (d)",
    "Default-Lebensdauer (d)",
    "Privzeti �ivljenski �as (d)",
    "Durata predefinita (gg)",
    "Standaard levensduur (d)",
    "Validade por defeito (d)",
    "Dur�e de vie par d�faut (j)",
    "Normal levetid timer (d)",
    "Tallenteen oletuselinik� (d)",
    "Domy�lny czas �ycia (d)",
    "Duraci�n por defecto (d�as)",
    "�������������� �������� ��������� (������)",
    "Normal livstid (dagar)",
    "Timp de p�strare predefinit (zile)",
    "Default �lettartam",
    "Durada predefinida",
    "���� �������� ������ �� ��������� (�)",
    "Standardno trajanje (d)",
    "Salvestuse eluiga (p�evi)",
    "Standard levetid (d)",
    "V�choz� �ivotnost",
  },
  { "Setup.Recording$Pause priority",
    "Pause-Priorit�t",
    "Prioriteta pavze",
    "Priorita' di pausa",
    "Pauze prioriteit",
    "",// TODO
    "Priorit� des pauses",
    "",// TODO
    "Taukotallenteen prioriteetti",
    "Priorytet pauzy",
    "Prioridad en modo pausa",
    "������������� ������������",
    "Prioritet f�r direktinspelning",
    "Prioritate pauz�",
    "Sz�net priorit�s",
    "Prioritat de la pausa",
    "��������� ����������� ���������",
    "Prioritet pauze",
    "Pausi prioriteet",
    "Pause prioritet",
    "Priorita p�eru�en�",
  },
  { "Setup.Recording$Pause lifetime (d)",
    "Pause-Lebensdauer (d)",
    "�as pavze (d)",
    "Durata pausa (gg)",
    "Pauze levensduur (d)",
    "",// TODO
    "Dur�e de vie des pauses (j)",
    "",// TODO
    "Taukotallenteen elinik� (d)",
    "Czas �ycia pauzy (d)",
    "Duraci�n en modo pausa (d�as)",
    "�������� �����������",
    "Livstid f�r direktinspelning (dagar)",
    "P�strarea emisiunilor 'pauzate' (zile)",
    "Sz�net �lettartama",
    "Durada de la pausa (d)",
    "�������� ����������� ��������� (�)",
    "Trajanje pauze (d)",
    "Pausi eluiga (p�evi)",
    "Pause levetid (d)",
    "�ivotnost p�eru�en� (d)",
  },
  { "Setup.Recording$Use episode name",
    "Episodenname verwenden",
    "Uporabi ime epizode",
    "Usare nome episodio",
    "Gebruik episode naam",
    "Utilizar o nome do epis�dio",
    "Utiliser le nom de l'�pisode",
    "Bruk episodenavn",
    "K�yt� jakson nime�",
    "U�ywaj nazwy epizodu",
    "Utilizar nombre de episodio",
    "����� �������� ����������",
    "Anv�nd episodnamn",
    "Utilizeaz� numele episodului",
    "Epiz�dn�v felhaszn�l�sa",
    "Utilitzar el nom de l'episodi",
    "������������ ����� �� ��������",
    "Koristi ime epizode",
    "Episoodi nime kasutamine",
    "Anvend udsendelsesnavn",
    "Pou��vat n�zev epizody",
  },
  { "Setup.Recording$Use VPS",
    "VPS benutzen",
    "Uporabi VPS",
    "Usare VPS",
    "VPS gebruiken",
    "",// TODO
    "Utiliser le VPS",
    "",// TODO
    "K�yt� VPS-toimintoa",
    "U�ywaj VPS",
    "Usar servicio VPS",
    "����� VPS",
    "Anv�nd VPS",
    "Utilizeaz� VPS",
    "VPS-t haszn�lni",
    "",// TODO
    "������������ ������� VPS",
    "Koristi VPS",
    "VPS-i kasutamine",
    "Anvend VPS",
    "Pou��vat VPS",
  },
  { "Setup.Recording$VPS margin (s)",
    "Zeitvorlauf bei VPS (s)",
    "�asovna rezerva pri VPS (s)",
    "Margine VPS",
    "Marge VPS (s)",
    "",// TODO
    "Marge VPS",
    "",// TODO
    "VPS-toiminnon aloitusmarginaali (s)",
    "Margines VPS (s)",
    "Margen VPS (sg)",
    "��������� VPS (�)",
    "VPS marginal (s)",
    "Marj� de timp la utilizare VPS (s)",
    "Id�eltol�d�s VPS-n�l (s)",
    "",// TODO
    "�������� ����� VPS (���)",
    "Vremenska rezerva kod VPS (s)",
    "VPS-i algusvaru (s)",
    "VPS margin (s)",
    "�asov� rezerva pro VPS (s)",
  },
  { "Setup.Recording$Mark instant recording",
    "Direktaufzeichnung markieren",
    "Ozna�i direktno snemanje",
    "Marcare registrazione immediata",
    "Direkte opnamen markeren",
    "Marca de grava��o r�pida",
    "Marquage enregistr. imm�diats",
    "Markere direkteopptak",
    "Merkitse pikatallenne",
    "Zaznaczaj natychm. nagrywanie",
    "Marcar grabaciones inmediatas",
    "������� ��������� ��������",
    "M�rk direktinspelning",
    "Marcheaz� �nregistrare imediat�",
    "Direktfelv�tel megjel�l�se",
    "Marcar gravaci� instant�nia",
    "�������� ��������� ������� ������",
    "Ozna�i direktno snimanje",
    "Otsesalvestuse m�rgistamine",
    "Mark�r direkte optagelse",
    "Ozna�it okam�it� nahr�vky",
  },
  { "Setup.Recording$Name instant recording",
    "Direktaufzeichnung benennen",
    "Ime za direktno snemanje",
    "Nome registrazione immediata",
    "Naam direkt-opname",
    "Nome de grava��o r�pida",
    "Noms enregistr. imm�diats",
    "Navngi direkteopptak",
    "Nime� pikatallenne",
    "Nazywaj natychm. nagrywanie",
    "Nombrar grabaciones inmediatas",
    "�������� ��������� ��������",
    "Namnge direktinspelning",
    "Nume �nregistrare imediat�",
    "Direktfelv�tel megnevez�se",
    "Anomenar gravacions instant�nies",
    "����� ���������� ������ �������",
    "Imenuj direktno snimanje",
    "Otsesalvestuse nimi",
    "Navngiv direkte optagelse",
    "Pojmenovat okam�it� nahr�vky",
  },
  { "Setup.Recording$Instant rec. time (min)",
    "Dauer der Direktaufzeichnung (min)",
    "�as direktnega snemanja (min)",
    "Tempo registrazione immediata (min)",
    "Duur van de directe opname (min)",
    "",//TODO
    "Dur�e enregistr. imm�diat (min)",
    "",//TODO
    "Pikatallennuksen kesto (min)",
    "Czas natychm. nagrywania (min)",
    "Tiempo de grabaci�n inmediata (min)",
    "�������� ��������� �������� (�����)",
    "Direktinspelning l�ngd (min)",
    "Timpul de �nregistare imediat� (min)",
    "Felv�tel id�tartama (perc)",
    "Temps de gravaci� instant�nia (min)",
    "������������ ������ ������ (���)",
    "Trajanje direktnog snimanja (min)",
    "Otsesalvestuse kestus (min)",
    "L�ngde af direkte optagelse (min)",
    "D�lka okam�it�ho nahr�v�n� (min)",
  },
  { "Setup.Recording$Max. video file size (MB)",
    "Max. Videodateigr��e (MB)",
    "Najve�ja velikost datoteke (MB)",
    "Dimensione massima file video (MB)",
    "Maximale omvang video file (MB)",
    "Tamanho m�ximo dos ficheiros (MB)",
    "Taille maxi des fichiers (MB)",
    "Maksimal st�rrelse p� videofiler (MB)",
    "Suurin tiedostokoko (Mt)",
    "Maksymalny rozmiar pliku (MB)",
    "Tama�o m�ximo de archivo (MB)",
    "������� ������� ������� (MB)",
    "Maximal filstorlek f�r inspelning (MB)",
    "Dimensiune maxim� a fi�ierului video (MB)",
    "Max. video File-terjedelem (MB)",
    "Mida m�xima de l'arxiu (MB)",
    "����. ������ ���������� (��)",
    "Maksimalna veli�ina datoteke (MB)",
    "Maksimaalne failisuurus (MB)",
    "Max. video filst�rrelse (MB)",
    "Maxim�ln� velikost nahr�vky (MB)",
  },
  { "Setup.Recording$Split edited files",
    "Editierte Dateien aufteilen",
    "Razdeli urejene datoteke",
    "Dividi i file modificati",
    "Bewerkte files opdelen",
    "Quebrar ficheiros",
    "S�parer les s�quences �dit�es",
    "Splitt redigerte filer",
    "Jaottele muokatut tallenteet",
    "Dziel edytowane pliki",
    "Partir archivos editados",
    "����������� �������������� �������",
    "Dela upp redigerade filer",
    "Separare fi�iere montate",
    "Feldolgozott File-k feloszt�sa",
    "Separar arxius",
    "������ ����������������� �����",
    "Podijeli ure�ene datoteke",
    "Redigeeritud failide t�keldamine",
    "Opdel redigerede filer",
    "D�lit editovan� soubory",
  },
  { "Setup.Replay$Multi speed mode",
    "Mehrstufiger Vor-/R�cklauf",
    "Re�im z ve� hitrostmi",
    "Modalita' multispeed",
    "Multi-speed mode",
    "Modo de multi-speed",
    "Mode multi-vitesses",
    "Multispeed modus",
    "K�yt� toiston moninopeustilaa",
    "Tryb wielopr�dko�ciowy",
    "Modo multi-velocidad",
    "������� ��������� ���������",
    "Multispeed mode",
    "Mod multi-vitez�",
    "MultiSpeed funkci�",
    "Mode de multivelocitat",
    "��������������� �����",
    "MultiSpeed na�in",
    "Multi-kiiruse moodus",
    "Multi hastighedsmodus",
    "V�cerychlostn� m�d",
  },
  { "Setup.Replay$Show replay mode",
    "Wiedergabestatus anzeigen",
    "Prika�i re�im predvajanja",
    "Mostrare modalita' riproduzione",
    "Weergave mode aangeven",
    "Mostrar modo de replay",
    "Affichage mode de lecture",
    "Vis avspillingsmodus",
    "N�yt� toiston tila",
    "Pokazuj tryb odtwarzania",
    "Mostrar modo de reproducci�n",
    "������� ���������� ������������",
    "Visa uppspelningsl�ge",
    "Afi�eaz� redarea",
    "Lej�tsz�s felt�ntet�se",
    "Mostrar mode de reproducci�",
    "���������� ����� ���������������", // ???
    "Prika�i na�in prikazivanja",
    "Korduse moodus",
    "Vis afspilningsmodus",
    "Zobrazit re�im p�ehr�v�n�",
  },
  { "Setup.Replay$Resume ID",
    "Wiedergabe-ID",
    "ID za predvajanje",
    "ID di rispristino",
    "Hervattings ID",
    "Resume ID",// TODO
    "ID de reprise",
    "Resume ID",// TODO
    "Tallenteen paluutunniste",
    "ID wznowienia",
    "ID de continuaci�n",
    "ID ������������",
    "�teruppta ID",
    "Identificator continuare",
    "Lej�tsz�s ID",
    "ID de Continuar",
    "ID ���������������", // ???
    "ID nastavka",
    "J�tkamise ID",
    "Genoptagelses ID",
    "ID obnoven�",
  },
  { "Setup.Miscellaneous$Min. event timeout (min)",
    "Br�ckenzeit zwischen Timern (min)",
    "Najmanj�i �as dogodka (min)",
    "Scadenza minima evento (min)",
    "Minimale event time-out (min)",
    "Per�odo m�nimo de pausa (min)",
    "MinEventTimeout (min)",
    "Minste hendelsespause (min)",
    "Tapahtuman odotusaika (min)",
    "Minimalny czas audycji (min)",
    "Tiempo m�n. antes de un evento (m)",
    "��������� ������ ���������� (�����)",
    "Minsta h�ndelse-pause (min)",
    "Durat� minim� emisiuni (min)",
    "Id�zit�sek k�z�tti id�tartam (perc)",
    "Temps m�nim en pausa (min)",
    "���. ����� �������� ������� (���)",
    "Minimalno vrijeme pauze",
    "Min. s�ndmuse kestus (min)",
    "Mindste h�ndelsespause (min)",
    "Min. pauza mezi ud�lostmi (min)",
  },
  { "Setup.Miscellaneous$Min. user inactivity (min)",
    "VDR ausschalten bei Inaktivit�t (min)",
    "Najmanj�i �as neaktivnosti (min)",
    "Periodo minimo di inattivita' (min)",
    "Minimum gebruikers inactiviteit (min)",
    "Per�odo m�nimo de inactividade (min)",
    "Dur�e minimale d'inact. (min)",
    "Minimumstid med inaktivitet (min)",
    "K�ytt�j�toiminnon odotusaika (min)",
    "Minimalny czas nieaktywno�ci (min)",
    "Tiempo m�n. de inactividad (m)",
    "��������� ������ �������� (�����)",
    "Minsta anv�ndar-inaktivitet (min)",
    "Durata minim� de inactivitate (min)",
    "VDR le�ll haszn�lat n�lk�l (perc)",
    "Temps m�nim d'inactivitat (min)",
    "���. ����� �������� ����� (���)",
    "Minimalno vrijeme neaktivnosti (min)",
    "Min. kasutaja tegevusetus (min)",
    "Min. bruger inaktivitet (min)",
    "�asov� limit neaktivnosti (min)",
  },
  { "Setup.Miscellaneous$SVDRP timeout (s)",
    "SVDRP trennen bei Inaktivit�t (s)",
    "SVDRP �as neaktivnosti (s)",
    "Scadenza SVDRP (s)",
    "SVDRP Timeout (s)",
    "Timeout SVDRP (s)",
    "Temps maxi SVDRP (s)",
    "Ubrukt SVDRP-levetid (s)",
    "SVDRP-komennon odotusaika (s)",
    "Czas oczekiwania na SVDRP (s)",
    "Tiempo de espera de SVDRP (sg)",
    "SVDRP ������� (�)",
    "SVDRP Timeout (s)",
    "Timeout SVDRP (sec)",
    "SVDRP sz�tkapcsol haszn�lat n�lk�l(s)",
    "SVDRP Timeout (s)",
    "�������� ������ ����. SVDRP (���)",
    "SVDRP vrijeme neaktivnosti",
    "SVDRP ooteaeg (s)",
    "SVDRP timeout (s)",
    "�asov� limit SVDRP (s)",
  },
  { "Setup.Miscellaneous$Zap timeout (s)",
    "Mindestzeit f�r Kanalhistorie (s)",
    "�as ugla�evanja (s)",
    "Scadenza Zapping (s)",
    "Zap timeout (s)",
    "",// TODO
    "Prise en compte cha�ne (s)",
    "",// TODO
    "Kanavavalinnan odotusaika (s)",
    "Czas oczekiwania na zap (s)",
    "Considerar canal como visto (sg)",
    "������� ������� (�)",
    "Zap timeout(s)",
    "Interval zapping (s)",
    "Ad�sv�lt�s ideje (s)",
    "",// TODO
    "�������� ������������ ������ (���)",
    "",// hrv TODO
    "Kanalivahetuse ooteaeg (s)",
    "Zap timeout (s)",
    "�asov� limit Zap (s)",
  },
  { "Setup.Miscellaneous$Initial channel",
    "Kanal beim Einschalten",
    "Privzeti kanal",
    "Canale iniziale",
    "",// TODO
    "",// TODO
    "Cha�ne initiale",
    "",// TODO
    "Kanava k�ynnistett�ess�",
    "Pocz�tkowy kana�",
    "Canal inicial",
    "",// TODO
    "Kanal vid uppstart",
    "Canalul de pornire",
    "Ad�s a bekapcsol�sn�l",
    "",// TODO
    "����� ��� ���������",
    "",// TODO
    "Kanal k�ivitamisel",
    "Kanal ved opstart",
    "Kan�l po spu�t�n�",
  },
  { "Setup.Miscellaneous$Initial volume",
    "Lautst�rke beim Einschalten",
    "Privzeta glasnost",
    "Volume iniziale",
    "",// TODO
    "",// TODO
    "Volume initial",
    "",// TODO
    "��nenvoimakkuus k�ynnistett�ess�",
    "Pocz�tkowa g�o�no��",
    "Volumen inicial",
    "",// TODO
    "Ljudstyrka vid uppstart",
    "Volumul la pornire",
    "Hanger� a bekapcsol�sn�l",
    "",// TODO
    "��������� ��� ���������",
    "",// TODO
    "Helitugevus k�ivitamisel",
    "Lydstyrke ved opstart",
    "Hlasitost po spu�t�n�",
  },
  { "Setup.Miscellaneous$as before",
    "wie vorher",
    "kot prej",
    "come prima",
    "",// TODO
    "",// TODO
    "comme avant",
    "",// TODO
    "edellinen",
    "jak ostatnio",
    "como antes",
    "",// TODO
    "som f�rut",
    "ca mai �nainte",
    "ahogy az el�bb",
    "",// TODO
    "��� ������",
    "",// TODO
    "nagu enne",
    "som f�r",
    "jako naposledy",
  },
  { "Setup.Miscellaneous$Abort when Plugin fails to load",
    "Abbrechen wenn Plugin nicht l�dt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Detener si un plugin falla al cargar",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Setup.Miscellaneous$Filter channels",
    "",// TODO
    "",// TODO
    "Filtra canali",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Filtrar canales",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Filtra canals",
    "",// TODO
    "",// hrv TODO
  },
  { "Tv only",
    "",// TODO
    "",// TODO
    "Solo Tv",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "S�lo TV",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Nom�s Tv",
    "",// TODO
    "",// hrv TODO
  },
  { "Radio only",
    "",// TODO
    "",// TODO
    "Solo Radio",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "S�lo Radio",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Nom�s Radio",
    "",// TODO
    "",// hrv TODO
  },
  // The days of the week:
  { "MTWTFSS",
    "MDMDFSS",
    "PTSCPSN",
    "LMMGVSD",
    "MDWDVZZ",
    "STQQSSD",
    "LMMJVSD",
    "MTOTFLS",
    "MTKTPLS",
    "PW�CPSN",
    "LMXJVSD",
    "�������",
    "MTOTFLS",
    "LMMJVSD",
    "HKSCPSV",
    "LMCJVSG",
    "���ǿ��",
    "PUS�PSN", // hrv
    "ETKNRLP",
    "MTOTFLS",
    "PUS�PSN",
  },
  { "MonTueWedThuFriSatSun", // must all be 3 letters!
    "MonDieMitDonFreSamSon",
    "PonTorSreCetPetSobNed",
    "LunMarMerGioVenSabDom",
    "MaaDinWoeDonVryZatZon",
    "SegTerQuaQuiSexSabDom",
    "LunMarMerJeuVenSamDim",
    "ManTirOnsTorFreL�rS�n",
    "Ma Ti Ke To Pe La Su ",
    "PonWto�roCzwPi�SobNie",
    "LunMarMi�JueVieS�bDom",
    "������������������K��",
    "M�nTisOnsTorFreL�rS�n",
    "LunMarMieJoiVinS�mDum",
    "H�tKedSzeCs�P�nSzoVas",
    "Dl Dm Dc Dj Dv Ds Dg ",
    "��ݲ�������ҿ����Ѳ��",
    "PonUtoSri�etPetSubNed", // hrv
    " E  T  K  N  R  L  P ",
    "ManTirOnsTorFreL�rS�n",
    "Po �t St �t P� So Ne ",
  },
  // The allowed characters in strings:
  { " abcdefghijklmnopqrstuvwxyz0123456789-.#~,/_@",
    " a�bcdefghijklmno�pqrstu�vwxyz0123456789-.#~,/_@",
    " abc�defghijklmnopqrs�tuvwxyz�0123456789-.#~,/_@",
    " a��bcde��fghi��jklmno�pqrstu�vwxyz0123456789-.#~,/_@",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~,/_@����������",
    "",// TODO
    " a�bc�de���fghi�jklmno�pqrstu��vwxyz0123456789-.#~,/_@",
    "",// TODO
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~,/_@",
    " a�bc�de�fghijkl�mn�o�pqrs�tuvwxyz��0123456789-.#~,/_@",
    " a�bc�de�fghi�jklmn�o�pqrstu��vwxyz0123456789-.#~,/_@",
    " ��������������������������������0123456789-.#~,/_@abcdefghijklmnopqrstuvwxyz",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~,/_@",
    " a��bcdefghi�jklmnopqrs�t�uvwxyz0123456789-.#~,/_@",
    " a�bcde�fghi�jklmno���pqrstu���vwxyz0123456789-.,#~,/_@",
    " a�bc�de��fghi�jklmno�pqrstu�vwxyz0123456789-.,#~,/_@�",
    " abcdefghijklmnopqrstuvwxyz��������������������������������0123456789-.#~,/_@",
    " abc��d�efghijklmnopqrs�tuvwxyz�0123456789-.#~,/_@", // hrv
    " abcdefghijklmnopqrs�z�tuvw����xy�0123456789-.#~,/_@",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~,/_@",
    " a�bc�d�e��fghi�jklmn�o�pqr�s�t�u��vwxy�z�0123456789-.#~,/_@",
  },
  // The character maps for entering letters via the numeric keys:
  { " 0\t-.#~,/_@1\tabc2\tdef3\tghi4\tjkl5\tmno6\tpqrs7\ttuv8\twxyz9",
    " 0\t-.#~,/_@1\tabc�2\tdef3\tghi4\tjkl5\tmno�6\tpqrs7\ttuv�8\twxyz9",
    " 0\t-.#~,/_@1\tabc�2\tdef3\tghi4\tjkl5\tmno6\tpqrs�7\ttuv8\twxyz�9",
    " 0\t-.#~,/_@1\ta�bc2\tde�f3\tghi�4\tjkl5\tmno�6\tpqrs7\ttu�v8\twxyz9",
    "",//TODO
    "",//TODO
    " 0\t-.#~,/_@1\tabc���2\tdef���3\tghi��4\tjkl5\tmno�6\tpqrs7\ttuv�8\twxyz9",
    "",//TODO
    " 0\t-.#~,/_@1\tabc��2\tdef3\tghi4\tjkl5\tmno�6\tpqrs7\ttuv8\twxyz9",
    " 0\t-.#~,/_@1\tabc2��\tdef3�\tghi4\tjkl5�\tmno6��\tpqrs7�\ttuv8\twxyz9��",
    " 0\t-.#~,/_@1\tabc��2\tdef�3\tghi�4\tjkl5\tmno��6\tpqrs7\ttuv��8\twxyz9",
    "",//TODO
    " 0\t-.#~,/_@1\tabc��2\tdef3\tghi4\tjkl5\tmno�6\tpqrs7\ttuv8\twxyz",
    " 0\t-.#~,/_@1\ta��bc2\tdef3\tghi�4\tjkl5\tmno6\tpqrs�7\tt�uv8\twxyz9",
    " 0\t-.#~,/_@1\ta�bc2\tde�f3\tghi�4\tjkl5\tmno���6\tpqrs7\ttu���v8\twxyz9",
    "",//TODO
    "",//TODO
    "",//TODO
    " 0\t-.#~,/_@1\tabc��2\tdef3\tghi4\tjkl5\tmno��6\tpqrs�7\ttuv�8\twxyz�9",
    " 0\t-.#~,/_@1\tabc��2\tdef3\tghi4\tjkl5\tmno�6\tpqrs7\ttuv8\twxyz9",
    " 0\t-.#~,/_@1\tabc��2\tdef���3\tghi�4\tjkl5\tmno��6\tpqrs��7\ttuv���8\twxyz��9",
  },
  // Learning keys:
  { "Learning Remote Control Keys",
    "Fernbedienung anlernen",
    "U�enje kod upravljalca",
    "Apprendimento tasti unita' remota",
    "Leren toetsen afstandsbediening",
    "Aprender as teclas do telecomando",
    "Apprentissage des codes de t�l�commande",
    "L�re fjernkontrolltaster",
    "Kaukos��timen opettaminen",
    "Nauka klawiszy pilota",
    "Aprendiendo botones del mando",
    "�������� �������� �������������",
    "Inl�rning av fj�rrkontrollsknappar",
    "�nv��are taste telecomand�",
    "T�vir�ny�t� betan�t�sa",
    "Aprenent les tecles del comandament a dist�ncia",
    "�������� ������",
    "U�enje k�da daljinskog upravlja�a",
    "Kaugjuhtimispuldi �petamine",
    "L�rer fjernbetjeningstaster",
    "U�en� k�d� d�lkov�ho ovl�d�n�",
  },
  { "Phase 1: Detecting RC code type",
    "Phase 1: System der Fernbedienung ermitteln",
    "Faza 1: Sprejemanje IR kode",
    "Fase 1: tipo ricevitore RC",
    "Fase 1: detecteren type afstandsbediening",
    "Fase 1: detectar tipo de receptor",
    "Phase 1: D�tection du type de code",
    "Fase 1: Finne fjernkontroll-kodetype",
    "Vaihe 1: Koodauksen tunnistaminen",
    "Etap 1: Wykrywanie rodzaju kodu pilota",
    "Fase 1: Detectando tipo de mando",
    "���� 1: ��������� ������ RC",
    "Steg1: identifiering av kod",
    "Faza 1: Detec�ia tipului telecomenzii",
    "Els� l�p�s: t�vir�ny�t� k�dj�nak meghat�roz�sa",
    "Fase 1: Detectant el tipus de receptor",
    "��� 1: ����������� ���� ���� ������",
    "Faza 1: detektiranje k�da daljinskog upravlja�a.",
    "Samm 1: Kodeerimiss�steemi tuvastamine",
    "Fase 1: Detekterer fjernbetjenings-kodetype",
    "F�ze 1: Detekce typu k�du",
  },
  { "Press any key on the RC unit",
    "Eine Taste auf der Fernbedienung dr�cken",
    "Pritisnite tipko na upravljalcu",
    "Premere un tasto dell'unita' RC",
    "Druk op een willekeurige knop",
    "Pressione qualquer tecla do telecomando",
    "Appuyer sur une touche de la t�l�commande",
    "Trykk en av tastene p� fjernkontrollen",
    "Paina mit� tahansa kaukos��timen n�pp�int�",
    "Naci�nij dowolny klawisz na pilocie",
    "Pulse cualquier bot�n en el mando",
    "����� ��� ������� ��� ���������������",
    "Tryck valfri tangent p� fj�rrkontrollen",
    "Ap�sa�i o tast� pe telecomand�",
    "Nyomjon meg egy gombot a t�vir�ny�t�n",
    "Premeu qualsevol tecla del comandament",
    "������� ����� ������ �� ������",
    "Pritisnite tipku na daljinskom upravlja�u",
    "Vajuta puldil suvalist klahvi",
    "Tryk vilk�rlig tast p� fjernbetjeningen",
    "Stiskn�te libovolnou kl�vesu ovl�d�n�",
  },
  { "RC code detected!",
    "System der Fernbedienung erkannt!",
    "IR koda sprejeta!",
    "Codice RC rilevato!",
    "Afstandsbediening code herkend!",
    "C�digo do telecomando detectado!",
    "Code de la t�l�commande d�tect�!",
    "Fjernkontroll-kodetype funnet!",
    "N�pp�inpainallus vastaanotettu!",
    "Wykryto kod pilota!",
    "�C�digo RC detectado!",
    "������� ������� RC!",
    "Koden detekterad!",
    "S-a detectat tipul telecomenzii!",
    "T�vir�ny�t� k�dja felismerve!",
    "Codi detectat!",
    "��������� ��� ������!",
    "K�d daljinskog upravlja�a je prepoznat!",
    "Kodeerimiss�steem tuvastatud!",
    "Fjernbetjenings-kodetype fundet!",
    "K�d byl detekov�n!",
  },
  { "Do not press any key...",
    "Keine Taste dr�cken...",
    "Ne pritiskajte tipk...",
    "Non premere alcun tasto...",
    "Druk niet op een knop...",
    "N�o pressione nada...",
    "N'appuyer sur aucune touche ...",
    "Ikke trykk p� noen av tastene...",
    "�l� paina kaukos��timen n�pp�imi�...",
    "Nie naciskaj teraz �adnego klawisza...",
    "No pulse ning�n bot�n...",
    "��� ����� �������...",
    "Tryck inte p� n�gon knapp...",
    "Nu ap�sa�i nici o tast�...",
    "Ne nyomjon meg gombot...",
    "No premeu cap tecla...",
    "�� ��������� ������...",
    "Ne pritiskajte tipku...",
    "�ra vajuta puldil �htki klahvi...",
    "Tryk ikke p� nogen tast...",
    "Nema�kejte ��dn� kl�vesy...",
  },
  { "Phase 2: Learning specific key codes",
    "Phase 2: Tastenbelegung anlernen",
    "Faza 2: U�enje posebnih kod",
    "Fase 2: Codici specifici dei tasti",
    "Fase 2: Leren specifieke toets-codes",
    "Fase 2: A aprender c�digos especificos",
    "Phase 2: Apprentissage des codes des touches",
    "Fase 2: L�re spesifikke tastekoder",
    "Vaihe 2: N�pp�inkoodien opettelu",
    "Etap 2: Nauka konkretnych kod�w klawiszy",
    "Fase 2: Aprendiendo c�digos espec�ficos",
    "���� 2: �������� ����������� ��������",
    "Fas 2: Inl�rning av specifika knapp-koder",
    "Faza 2: �nv��area codurilor anumitor taste",
    "M�sodik l�p�s: az egyes gombok betan�t�sa",
    "Fase 2: Aprenentantge de les funcions",
    "��� 2: ������� ����� ��������� ������",
    "Faza 2: U�enje posebnih k�dova",
    "Samm 2: Klahvide �petamine",
    "Fase 2: L�r individuelle tast koder",
    "F�ze 2: U�en� konkr�tn�ho k�du kl�ves",
  },
  { "Press key for '%s'",
    "Taste f�r '%s' dr�cken",
    "Pritisnite tipko za '%s'",
    "Premere il tasto per '%s'",
    "Druk knop voor '%s'",
    "Pressione tecla para '%s'",
    "Appuyer sur la touche '%s'",
    "Trykk tasten for '%s'",
    "Paina n�pp�int� toiminnolle '%s'",
    "Naci�nij klawisz dla '%s'",
    "Pulse un bot�n para '%s'",
    "����� �� ������� ��� '%s'",
    "Tryck p� knappen f�r '%s'",
    "Ap�sa�i tasta pentru '%s'",
    "'%s' gomb megnyom�sa",
    "Premeu una tecla per '%s'",
    "������� ������ '%s'",
    "Pritisnite tipku za '%s'",
    "Vajuta klahvile '%s'",
    "Tryk tasten for '%s'",
    "Stiskn�te kl�vesu pro '%s'",
  },
  { "Press 'Up' to confirm",
    "'Auf' dr�cken zum Best�tigen",
    "Pritisnite tipko 'Gor' za potrditev",
    "Premere 'Su' per confermare",
    "Druk 'Omhoog' om te bevestigen",
    "Pressione 'Cima' para confirmar",
    "Appuyer sur 'Haut' pour confirmer",
    "Trykk 'Opp' for � bekrefte",
    "Hyv�ksy painamalla 'Yl�s'",
    "Naci�nij 'Do g�ry' aby potwierdzi�",
    "Pulse 'Arriba' para confirmar",
    "����� '����' ��� �������",
    "Tryck 'Upp' f�r att bekr�fta",
    "Ap�sa�i 'Sus' pentru confirmare",
    "'Fel' megnyom�sa az elfogad�shoz",
    "Premeu 'Amunt' per confirmar",
    "������� '�����' ����� �����������",
    "Pritisnite 'Gore' za potvrdu",
    "Kinnitamiseks vajuta '�les'",
    "Tryk 'Op' for at bekr�fte",
    "Stiskn�te 'Nahoru' pro potvrzen�",
  },
  { "Press 'Down' to continue",
    "'Ab' dr�cken zum Weitermachen",
    "Pritisnite tipko 'Dol' za nadaljevanje",
    "Premere 'Giu' per continuare",
    "Druk 'Omlaag' om verder te gaan",
    "Pressione 'Baixo' para continuar",
    "Appuyer sur 'Bas' pour continuer",
    "Trykk Ned' for � fortsette",
    "Jatka painamalla 'Alas'",
    "Naci�nij 'Na d�' aby kontynuowa�",
    "Pulse 'Abajo' para continuar",
    "����� '����' ��� ��������",
    "Tryck 'Ner' f�r att bekr�fta",
    "Ap�sa�i 'Jos' pentru continuare",
    "'Le' megnyom�sa a folytat�shoz",
    "Premeu 'Avall' per continuar",
    "������� '����' ����� ����������",
    "Pritisnite 'Dolje' za nastavak",
    "J�tkamiseks vajuta 'Alla'",
    "Tryk 'Ned' for at forts�tte",
    "Stiskn�te 'Dol�' pro pokra�ov�n�",
  },
  { "(press 'Up' to go back)",
    "('Auf' dr�cken, um zur�ckzugehen)",
    "(pritisnite 'Gor' za nazaj)",
    "(premere 'Su' per tornare indietro)",
    "(druk 'Omhoog' om terug te gaan)",
    "(Pressione 'Cima' para voltar)",
    "(Appuyer sur 'Haut' pour revenir en arri�re)",
    "(trykk 'Opp' for � g� tilbake)",
    "(palaa takaisin painamalla 'Yl�s')",
    "(naci�nij 'Do g�ry' aby si� cofn��)",
    "(Pulse 'Arriba' para retornar)",
    "(����� '����' ��� ���������",
    "(Tryck 'Upp' f�r att backa)",
    "(Ap�sa�i 'Sus' pentru revenire)",
    "(�Fel' megnyom�sa a visszat�r�shez)",
    "(Premeu 'Amunt' per retornar)",
    "(������� '�����' ����� ���������)",
    "(pritisnite 'Gore' za nazad)",
    "(tagasiminekuks vajuta '�les')",
    "(tryk 'Op' for at g� tilbage)",
    "(stiskn�te 'Nahoru' pro zp�t)",
  },
  { "(press 'Down' to end key definition)",
    "('Ab' dr�cken zum Beenden)",
    "(pritisnite 'Dol' za konec)",
    "('Giu' per finire la definizione tasti)",
    "(Druk 'Omlaag' om te beeindigen)",
    "(Pressione 'Baixo' para terminar a defini��o)",
    "(Appuyer sur 'Bas' pour terminer)",
    "(trykk 'Ned' for � avslutte innl�ring)",
    "(lopeta opettaminen painamalla 'Alas')",
    "(naci�nij 'Na d�' aby zako�czy� definiowanie)",
    "(Pulse 'Abajo' para terminar el aprendizaje)",
    "(����� '����' ��� ���������� �������� ��������)",
    "(Tryck 'Ner' f�r att avsluta inl�rningen)",
    "(Ap�sa�i 'Jos' pentru terminare)",
    "('Le' megnyom�sa a befejez�shez)",
    "(Premeu 'Avall' per finalitzar l'aprenentatge)",
    "(������� '����' ����� ��������� ��������� ������)",
    "(pritisnite 'Dolje' za kraj)",
    "(l�petamiseks vajuta 'Alla')",
    "(tryk 'Ned' for at afslutte indl�ringen)",
    "(stiskn�te 'Dol�' pro ukon�en�)",
  },
  { "(press 'Menu' to skip this key)",
    "('Men�' dr�cken zum �berspringen)",
    "pritisnite 'Meni' za preskok te tipke",
    "(premere 'Menu' per saltare questo tasto)",
    "Druk 'Menu' om deze toets over te slaan",
    "",// TODO
    "('Menu' pour passer cette touche)",
    "",// TODO
    "n�pp�imen puuttuessa paina 'Valikko'",
    "(naci�nij 'Menu' aby pomin�� ten klawisz)",
    "Pulse 'Men�' para omitir este bot�n",
    "����� '�����' ��� ���������� ����� ��� ��������",
    "(Tryck 'Meny' f�r att hoppa �ver knappen.)",
    "Ap�sa�i 'Meniu' pentru a s�ri peste aceast� tast�",
    "A Men� gombot nyomni ennek a gombnak a kihagy�s�hoz",
    "Premeu 'Men�' per obviar aquesta tecla",
    "(������� '����' ����� ���������� ������)",
    "pritisnite 'Izbor' da presko�ite ovu tipku",
    "(selle klahvi vahelej�tmiseks vajuta 'Men��')",
    "(tryk 'Menu' for at springe denne tast over)",
    "(stiskn�te 'Menu' pro p�esko�en� definice kl�vesy)",
  },
  { "Phase 3: Saving key codes",
    "Phase 3: Tastenbelegung abspeichern",
    "Faza 3: Shranjevanje kod tipk",
    "Fase 3: Salvataggio codici tasti",
    "Fase 3: Opslaan toets codes",
    "Fase 3: A salvar os c�digos das teclas",
    "Phase 3: Sauvegarde des codes des touches",
    "Fase 3: Lagre tastekoder",
    "Vaihe 3: N�pp�inkoodien tallentaminen",
    "Etap 3: Zapisywanie kod�w klawiszy",
    "Fase 3: Guardando los c�digos de botones",
    "���� 3: ���������� �������",
    "Fas 3: Spara knappkoder",
    "Faza 3: Salvarea codurilor de taste",
    "Harmadik l�p�s: k�d ment�se",
    "Fase 3: Guardant els codis de les tecles",
    "��� 3: ����������� ����� ������",
    "Faza 3: Spremanje k�dova tipki",
    "Samm 3: Klavhikoodide salvestamine",
    "Fase 3: Gemmer tast koder",
    "F�ze 3: Ulo�en� k�d�",
  },
  { "Press 'Up' to save, 'Down' to cancel",
    "'Auf' speichert, 'Ab' bricht ab",
    "'Gor' za potrditev, 'Dol' za prekinitev",
    "'Su' per salvare, 'Giu' per annullare",
    "'Omhoog' te bewaren, 'Omlaag' voor annuleren",
    "'Cima' para Salvar, 'Baixo' para Cancelar",
    "Appuyer sur 'Haut' pour sauvegarder, 'Bas' pour annuler",
    "Trykk 'Opp' for � lagre, 'Ned' for � avbryte",
    "Paina 'Yl�s' tallentaaksesi ja 'Alas' peruuttaaksesi",
    "Naci�nij 'Do g�ry' aby zapisa�, 'Na d�' - aby anulowa�",
    "Pulse 'Arriba' para guardar, 'Abajo' para anular",
    "����� '����' ��� ����������, '����' ��� �������",
    "Tryck 'Upp' f�r att spara eller 'Ner' f�r att avsluta.",
    "Ap�sa�i 'Sus' pentru salvare, 'Jos' pentru anulare",
    "'Fel' ment�s, 'Le� m�gse",
    "Premeu 'Amunt' per guardar, 'Avall' per anul�lar",
    "������� '�����' ����� ���������, '����' ����� ����������",
    "'Gore' za potvrdu, 'Dolje' za prekid",
    "Salvestamiseks vajuta '�les' ja katkestamiseks 'Alla'",
    "Tryk 'Op' for at gemme, 'Ned' for at annullere",
    "Stiskn�te 'Nahoru' pro ulo�en�, 'Dol�' pro zru�en�",
  },
  // Key names:
  { "Key$Up",
    "Auf",
    "Gor",
    "Su",
    "Omhoog",
    "Cima",
    "Haut",
    "Opp",
    "Yl�s",
    "Do g�ry",
    "Arriba",
    "����",
    "Upp",
    "Sus",
    "Fel",
    "Amunt",
    "�����",
    "Gore",
    "�les",
    "Op",
    "Nahoru",
  },
  { "Key$Down",
    "Ab",
    "Dol",
    "Giu",
    "Omlaag",
    "Baixo",
    "Bas",
    "Ned",
    "Alas",
    "Na d�",
    "Abajo",
    "����",
    "Ner",
    "Jos",
    "Le",
    "Avall",
    "����",
    "Dolje",
    "Alla",
    "Ned",
    "Dol�",
  },
  { "Key$Menu",
    "Men�",
    "Meni",
    "Menu",
    "Menu",
    "Menu",
    "Menu",
    "Meny",
    "Valikko",
    "Menu",
    "Men�",
    "�����",
    "Meny",
    "Meniu",
    "Men�",
    "Men�",
    "����",
    "Izbor",
    "Men��",
    "Menu",
    "Menu",
  },
  { "Key$Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "��",
    "Ok",
    "OK",
    "Ok",
    "D'acord",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
    "Ok",
  },
  { "Key$Back",
    "Zur�ck",
    "Nazaj",
    "Indietro",
    "Terug",
    "Voltar",
    "Retour",
    "Tilbake",
    "Takaisin",
    "Wstecz",
    "Retornar",
    "����",
    "Tillbaka",
    "�napoi",
    "Vissza",
    "Retornar",
    "�����",
    "Nazad",
    "Tagasi",
    "Tilbage",
    "Zp�t",
  },
  { "Key$Left",
    "Links",
    "Levo",
    "Sinistra",
    "Links",
    "Esquerda",
    "Gauche",
    "Venstre",
    "Vasemmalle",
    "W lewo",
    "Izquierda",
    "��������",
    "V�nster",
    "St�nga",
    "Balra",
    "Esquerra",
    "������",
    "Lijevo",
    "Vasakule",
    "Venstre",
    "Vlevo",
  },
  { "Key$Right",
    "Rechts",
    "Desno",
    "Destra",
    "Rechts",
    "Direita",
    "Droite",
    "H�yre",
    "Oikealle",
    "W prawo",
    "Derecha",
    "�����",
    "H�ger",
    "Dreapta",
    "Jobbra",
    "Dreta",
    "�������",
    "Desno",
    "Paremale",
    "H�jre",
    "Vpravo",
  },
  { "Key$Red",
    "Rot",
    "Rde�a",
    "Rosso",
    "Rood",
    "Vermelho",
    "Rouge",
    "R�d",
    "Punainen",
    "Czerwony",
    "Rojo",
    "�������",
    "R�d",
    "Ro�u",
    "Piros",
    "Vermell",
    "�������",
    "Crveno",
    "Punane",
    "R�d",
    "�erven�",
  },
  { "Key$Green",
    "Gr�n",
    "Zelena",
    "Verde",
    "Groen",
    "Verde",
    "Vert",
    "Gr�nn",
    "Vihre�",
    "Zielony",
    "Verde",
    "�������",
    "Gr�n",
    "Verde",
    "Z�ld",
    "Verd",
    "�������",
    "Zeleno",
    "Roheline",
    "Gr�n",
    "Zelen�",
  },
  { "Key$Yellow",
    "Gelb",
    "Rumena",
    "Giallo",
    "Geel",
    "Amarelo",
    "Jaune",
    "Gul",
    "Keltainen",
    "��ty",
    "Amarillo",
    "�������",
    "Gul",
    "Galben",
    "S�rga",
    "Groc",
    "������",
    "�uto",
    "Kollane",
    "Gul",
    "�lut�",
  },
  { "Key$Blue",
    "Blau",
    "Modra",
    "Blu",
    "Blauw",
    "Azul",
    "Bleu",
    "Bl�",
    "Sininen",
    "Niebieski",
    "Azul",
    "����",
    "Bl�",
    "Albastru",
    "K�k",
    "Blau",
    "�����",
    "Plavo",
    "Sinine",
    "Bl�",
    "Modr�",
  },
  { "Key$Info",
    "Info",
    "Info",
    "Informazioni",
    "Info",
    "",//TODO
    "Info",
    "",//TODO
    "Tiedot",
    "Info",
    "Info",
    "�����������",
    "Info",
    "Info",
    "Inform�ci�",
    "",//TODO
    "����",
    "Info",
    "Info",
    "Info",
    "Info",
  },
  { "Key$Play",
    "Wiedergabe",
    "Predvajaj",
    "Riproduzione",
    "Weergeven",
    "",// TODO
    "Lecture",
    "",// TODO
    "Toista",
    "Odtw�rz",
    "Reproducir",
    "�����������",
    "Spela upp",
    "Redare",
    "Lej�tsz�s",
    "Reproduir",
    "���������������",
    "Start",
    "Start",
    "Afspil",
    "P�ehr�t",
  },
  { "Key$Pause",
    "Pause",
    "Pavza",
    "Pausa",
    "Pauze",
    "",// TODO
    "Pause",
    "",// TODO
    "Tauko",
    "Pauza",
    "Pausa",
    "�����",
    "Pausa",
    "Pauz�",
    "Sz�net",
    "Pausa",
    "�����",
    "Pauza",
    "Paus",
    "Pause",
    "P�eru�it",
  },
  { "Key$Stop",
    "Stopp",
    "Ustavi",
    "Stop",
    "Stop",
    "Stop",
    "Stop",
    "",// TODO
    "Lopeta",
    "Stop",
    "Parar",
    "T����",
    "Stoppa",
    "Stop",
    "Stop",
    "Aturar",
    "����",
    "Stop",
    "Stopp",
    "Stop",
    "Zastavit",
  },
  { "Key$Record",
    "Aufnehmen",
    "Snemaj",
    "Registrazione",
    "Opnemen",
    "",// TODO
    "Enregistrement",
    "",// TODO
    "Tallenna",
    "Nagrywaj",
    "Grabar",
    "E������",
    "Spela in",
    "�nregistrare",
    "Felvenni",
    "Gravar",
    "������",
    "Snimaj",
    "Salvestamine",
    "Optag",
    "Nahr�t",
  },
  { "Key$FastFwd",
    "Vorlauf",
    "Hitro naprej",
    "Avanti Veloce",
    "Vooruitspoelen",
    "",// TODO
    "Avance rapide",
    "",// TODO
    "Pikakelaus >>",
    "Do przodu",
    "Avance r�pido",
    "�������� ������",
    "Snabbspolning fram�t",
    "Derulare �nainte",
    "El�re p�rgetni",
    "Endavant r�pidament",
    "��������� ������",
    "Naprijed",
    "Edasikerimine",
    "Spol fremad",
    "Dop�edu",
  },
  { "Key$FastRew",
    "R�cklauf",
    "Hitro nazaj",
    "Indietro Veloce",
    "Terugspoelen",
    "",// TODO
    "Retour rapide",
    "",// TODO
    "Pikakelaus <<",
    "Do ty�u",
    "Retroceso r�pido",
    "�������� ����",
    "Snabbspolning bak�t",
    "Derulare �napoi",
    "Vissza p�rgetni",
    "Enrera r�pidament",
    "��������� �����",
    "Nazad",
    "Tagasikerimine",
    "Spol tilbage",
    "Dozadu",
  },
  { "Key$Next",
    "Vorw�rts",
    "Naprej",
    "Prossimo",
    "",// TODO
    "",// TODO
    "Suivant",
    "",// TODO
    "Seuraava",
    "Dalej",
    "Siguiente",
    "",// TODO
    "N�sta",
    "Urm�tor",
    "El�re",
    "",// TODO
    "������",
    "",// TODO
    "J�rgmine",
    "Fremad",
    "D�le",
  },
  { "Key$Prev",
    "Zur�ck",
    "Nazaj",
    "Precedente",
    "",// TODO
    "",// TODO
    "Pr�c�dent",
    "",// TODO
    "Edellinen",
    "Wstecz",
    "",// TODO
    "",// TODO
    "Bak�t",
    "Anterior",
    "Vissza",
    "",// TODO
    "�����",
    "",// TODO
    "Eelmine",
    "Tilbage",
    "Zp�t",
  },
  { "Key$Power",
    "Ausschalten",
    "Izklop",
    "Power",
    "Uitschakelen",
    "Ligar",
    "Arr�t",
    "Power",
    "Virta",
    "Wy��cz",
    "Apagar",
    "K�������",
    "P�/Av",
    "�nchidere",
    "Kikapcsolni",
    "Aturar",
    "���������",
    "Isklju�i",
    "Toide",
    "Sluk",
    "Vyp�na�",
  },
  { "Key$Channel+",
    "Kanal+",
    "Program+",
    "Canale +",
    "Kanaal+",
    "",// TODO
    "Cha�ne+",
    "",// TODO
    "Kanava +",
    "Kana�+",
    "Canal+",
    "������+",
    "Kanal+",
    "Canal+",
    "Csatorna+",
    "Canal +",
    "����� +",
    "Program +",
    "Kanal+",
    "Kanal+",
    "Kan�l+",
  },
  { "Key$Channel-",
    "Kanal-",
    "Program-",
    "Canale -",
    "Kanaal-",
    "",// TODO
    "Cha�ne-",
    "",// TODO
    "Kanava -",
    "Kana�-",
    "Canal-",
    "������-",
    "Kanal-",
    "Canal-",
    "Csatorna-",
    "Canal -",
    "����� -",
    "Program -",
    "Kanal-",
    "Kanal-",
    "Kan�l-",
  },
  { "Key$PrevChannel",
    "Vorheriger Kanal",
    "Prej�nji kanal",
    "Canale Prec.",
    "",// TODO
    "",// TODO
    "Cha�ne pr�c�dente",
    "",// TODO
    "Edellinen kanava",
    "Poprzedni kana�",
    "Canal anterior",
    "",// TODO
    "F�rra kanalen",
    "Canal anterior",
    "Az el�z� ad�s",
    "",// TODO
    "���������� �����",
    "",// TODO
    "Eelmine kanal",
    "Foreg�ende kanal",
    "P�edch�zej�c� kan�l",
  },
  { "Key$Volume+",
    "Lautst�rke+",
    "Glasnost+",
    "Volume +",
    "Volume+",
    "Volume+",
    "Volume+",
    "Volum+",
    "��nenvoimakkuus +",
    "G�o�niej",
    "Volumen+",
    "������+",
    "Volym+",
    "Volum+",
    "Hanger�+",
    "Volum +",
    "��������� +",
    "Glasnije", // hrv "Glasnocca +"
    "Helitugevus+",
    "Lydstyrke+",
    "Hlasitost+",
  },
  { "Key$Volume-",
    "Lautst�rke-",
    "Glasnost-",
    "Volume -",
    "Volume-",
    "Volume-",
    "Volume-",
    "Volum-",
    "��nenvoimakkuus -",
    "Ciszej",
    "Volumen-",
    "������-",
    "Volym-",
    "Volum-",
    "Hanger�-",
    "Volum -",
    "��������� -",
    "Ti�e", // hrv Glasnocca -"
    "Helitugevus-",
    "Lydstyrke-",
    "Hlasitost-",
  },
  { "Key$Mute",
    "Stumm",
    "Izklop zvoka",
    "Muto",
    "Geluid onderbreken",
    "Sem som",
    "Coupure du son",
    "Lyd av",
    "Mykistys",
    "Wycisz",
    "Mudo",
    "�������",
    "Ljud Av",
    "F�r� sunet",
    "Csend",
    "En silenci",
    "��������� ����",
    "Bez zvuka",
    "H��letu",
    "Sluk lyd",
    "Ticho",
  },
  { "Key$Audio",
    "Audio",
    "Zvok",
    "Audio",
    "Audio",
    "",// TODO
    "Audio",
    "",// TODO
    "��ni",
    "D�wi�k",
    "Audio",
    "����",
    "Ljud",
    "Sunet",
    "Hang",
    "",// TODO
    "����",
    "",// TODO
    "Audio",
    "Audio",
    "Zvuk",
  },
  // Miscellaneous:
  { "yes",
    "ja",
    "da",
    "si",
    "ja",
    "sim",
    "oui",
    "ja",
    "kyll�",
    "tak",
    "s�",
    "���",
    "ja",
    "da",
    "igen",
    "s�",
    "��",
    "da",
    "jah",
    "ja",
    "ano",
  },
  { "no",
    "nein",
    "ne",
    "no",
    "nee",
    "n�o",
    "non",
    "nei",
    "ei",
    "nie",
    "no",
    "���",
    "nej",
    "nu",
    "nem",
    "no",
    "���",
    "ne",
    "ei",
    "nej",
    "ne",
  },
  { "off",
    "aus",
    "izklop",
    "off",
    "uit",
    "off",
    "off",
    "",// TODO
    "pois",
    "wy��cz",
    "off",
    "�������",
    "av",
    "inactiv",
    "ki",
    "off",
    "����",
    "ugasi",
    "v�lja",
    "fra",
    "vyp.",
  },
  { "none",
    "keine",
    "nobeden",
    "nessuno",
    "geen",
    "",// TODO
    "aucun",
    "",// TODO
    "tyhj�",
    "brak",
    "ninguno",
    "������",
    "ingen",
    "niciuna(ul)",
    "semmi",
    "cap",
    "������",
    "nijedan",
    "t�hi",
    "ingen",
    "��dn�",
  },
  { "auto",
    "auto",
    "avtomatsko",
    "automatico",
    "auto",
    "",// TODO
    "auto",
    "",// TODO
    "auto",
    "auto",
    "auto",
    "��������",
    "automatisk",
    "automat",
    "auto",
    "auto",
    "����",
    "automatski",
    "automaatne",
    "automatisk",
    "auto",
  },
  { "top",
    "oben",
    "zgoraj",
    "in alto",
    "boven",
    "topo",
    "haut",
    "�vre",
    "yl�reuna",
    "g�ra",
    "arriba",
    "����",
    "�vre",
    "sus",
    "Fent",
    "a dalt",
    "������",
    "gore",
    "�leval",
    "top",
    "haho�e",
  },
  { "bottom",
    "unten",
    "spodaj",
    "in basso",
    "onder",
    "fundo",
    "bas",
    "nedre",
    "alareuna",
    "d�",
    "abajo",
    "����",
    "undre",
    "jos",
    "lent",
    "a baix",
    "�����",
    "dolje",
    "all",
    "bund",
    "dole",
  },
  { "Disk",
    "Disk",
    "Disk",
    "Disco",
    "Disk",
    "Disk",
    "Disque",
    "Disk",
    "Levy",
    "Dysk",
    "Disco",
    "������",
    "Disk",
    "Disc",
    "Lemez",
    "Disc",
    "����",
    "Disk",
    "Ketas",
    "Disk",
    "Disk",
  },
  { "free",
    "frei",
    "prosto",
    "liberi",
    "vrij",
    "livre",
    "restant",
    "ledig",
    "vapaana",
    "wolnego",
    "libre",
    "���������",
    "ledigt",
    "liber",
    "szabad",
    "lliure",
    "��������",
    "slobodno",
    "vaba",
    "fri",
    "volno",
  },
  { "Jump: ", // note the trailing blank
    "Springen: ",
    "Sko�i: ",
    "Vai a: ",
    "Springen: ",
    "Saltar: ",
    "Acc�s direct: ",
    "Hopp: ",
    "Siirry: ",
    "Skok: ",
    "Saltar: ",
    "����������: ",
    "Hopp: ",
    "Salt la: ",
    "Ugr�s:",
    "Salta a:",
    "�������: ",
    "Sko�i: ",
    "H�pe: ",
    "Hop: ",
    "Skok: ",
  },
  { "Volume ", // note the trailing blank
    "Lautst�rke ",
    "Glasnost ",
    "Volume ",
    "Volume ",
    "Volume ",
    "Volume ",
    "Volum ",
    "��nenvoimakkuus ",
    "G�o�no�� ",
    "Volumen ",
    "������ ",
    "Volym ",
    "Volum ",
    "Hanger�",
    "Volum ",
    "��������� ",
    "Glasno�a ",
    "Helitugevus ",
    "Lydstyrke ",
    "Hlasitost ",
  },
  { " Stop replaying", // note the leading blank!
    " Wiedergabe beenden",
    " Prekini predvajanje",
    " Stop riproduzione",
    " Stop afspelen",
    " Parar reprodu��o",
    " Arr�ter la lecture",
    " Stopp avspilling",
    " Lopeta toisto",
    " Zatrzymaj odtwarzanie",
    " Parar reproducci�n",
    " ����� ������������",
    " Avsluta uppspelning",
    " Opre�te redarea",
    " Lej�tsz�st befejzni",
    " Aturar la reproducci�",
    " ���������� ���������������",
    " Prekini reprodukciju",
    " L�petada taasesitamine",
    " Stop afspilning",
    " Zastavit p�ehr�v�n�",
  },
  { " Stop recording ", // note the leading and trailing blanks!
    " Aufzeichnung beenden ",
    " Prekini snemanje ",
    " Stop registrazione ",
    " Stop opnemen ",
    " Parar grava��o ",
    " Arr�ter l'enregistrement ",
    " Stopp opptak fra ",
    " Lopeta tallennus ",
    " Zatrzymaj nagrywanie ",
    " Parar grabaci�n ",
    " ����� ������� ",
    " Avsluta inspelning ",
    " Opre�te �nregistrarea ",
    " Felv�telt befejezni",
    " Aturar la gravaci� ",
    " ���������� ������ ",
    " Prekini snimanje ",
    " L�petada salvestamine ",
    " Stop optagelse ",
    " Zastavit nahr�v�n� ",
  },
  { " Cancel editing", // note the leading blank!
    " Schneiden abbrechen",
    " Prekini urejanje",
    " Annullare modifiche",
    " Bewerken afbreken",
    " Anular modifica��o",
    " Annuler le montage",
    " Avbryt redigering",
    " Peru muokkaus",
    " Anuluj edycj�",
    " Anular edici�n",
    " ������� ������������",
    " Avbryt redigering",
    " Opre�te montajul �nregistr�rii",
    " V�g�st megszak�tani",
    " Cancel�lar l'edici� ",
    " �������� ������ ������",
    " Prekini ure�ivanje",
    " Katkestada monteerimine",
    " Afbryd redigering",
    " Zru�it editaci",
  },
  { "Switching primary DVB...",
    "Prim�res Interface wird umgeschaltet...",
    "Preklapljanje primarne DVB naprave...",
    "Cambio su card DVB primaria...",
    "Eerste DVB-kaart wordt omgeschakeld...",
    "A mudar interface DVB prim�rio...",
    "Changement de carte DVB primaire...",
    "Bytter f�rste DVB-enhet...",
    "Vaihdetaan ensisijaista DVB-sovitinta...",
    "Prz곱czam na pierwszy interfejs DVB...",
    "Cambiando el interfaz DVB primario...",
    "� ����� DVB ����� �������...",
    "Byter prim�r DVB enhet...",
    "Comut dispozitiv DVB primar...",
    "Prim�r Interface �tkapcsolva...",
    "Canviant a la interf�cie DVB prim�ria...",
    "����� ��������� DVB-����������...",
    "Preklapanje primarnog DVB ure�aja...",
    "Esmase DVB seadme �mberl�litus...",
    "Skifter prim�r DVB enhed...",
    "P�epnout prim�rn� DVB...",
  },
  { "Up/Dn for new location - OK to move",
    "Auf/Ab f�r neue Position - dann OK",
    "Gor/Dol za novo poz. - Ok za premik",
    "Su/Giu per nuova posizione - OK per muovere",
    "Gebruik Omhoog/Omlaag - daarna Ok",
    "Cima/Baixo para nova localiza��o - Ok para mover",
    "Haut/Bas -> nouvelle place - OK -> d�placer",
    "Opp/Ned for ny plass - OK for � flytte",
    "'Yl�s/Alas' uusi paikka - 'OK' hyv�ksy",
    "Do g�ry/Na d� na now� pozycj� - Ok zatwierdza",
    "Arriba/Abajo para mover - OK para confirmar",
    "����/���� ��� ��� ����. ���� ��",
    "Upp/Ner f�r nya platsen - d�refter OK f�r att flytta",
    "Sus/Jos pentru noua loca�ie - OK pentru a muta",
    "Fel/Le egy uj opci��rt - azt�n OK",
    "Amunt/Avall per una nova localitzaci� - OK per moure",
    "��������� \"�����\"/\"����\" ��� ������ �������, � ����� \"OK\"",
    "Gore/Dolje na novu poziciju - zatim OK",
    "'�les/Alla' uus asukoht - 'OK' kinnitus",
    "Op/Ned for ny placering - OK for at flytte",
    "Nahoru/Dolu pro novou pozici - Ok p�esune",
  },
  { "Editing process started",
    "Schnitt gestartet",
    "Urejanje se je za�elo",
    "Processo di modifica iniziato",
    "Bewerken is gestart",
    "Processo de modifica��o iniciado",
    "Op�ration de montage lanc�e",
    "Redigeringsprosess startet",
    "Muokkaus aloitettu",
    "Proces edycji rozpocz�ty",
    "Proceso de edici�n iniciado",
    "������ � �����������",
    "Redigeringen startar",
    "Montajul �nregistr�rii a �nceput",
    "V�g�s elind�tva",
    "Proc�s d'edici� iniciat",
    "����� ������ ������",
    "Rezanje je zapo�eto",
    "Redigeerimine algas",
    "Redigeringsproces startet",
    "Edita�n� proces za�al",
  },
  { "Editing process finished",
    "Schnitt beendet",
    "Proces urejanja je kon�an",
    "Processo di modifica terminato",
    "Bewerken is klaar",
    "Processo de modifica��o terminado",
    "Montage termin�",
    "Redigeringsprosess avsluttet",
    "Muokkaus valmis",
    "Proces edycji zako�czony",
    "Proceso de edici�n terminado",
    "� ����������� ��������",
    "Redigering avslutad",
    "Montajul �nregistr�rii s-a terminat",
    "V�g�s befejezve",
    "Proc�s d'edici� finalitzat",
    "������ �������",
    "Rezanje zavr�eno",
    "Redigeerimine l�petatud",
    "Redigeringsproces afsluttet",
    "Edita�n� proces skon�il",
  },
  { "Editing process failed!",
    "Schnitt gescheitert!",
    "Napaka pri procesu urejanja!",
    "Processo di modifica fallito!",
    "Bewerken is mislukt!",
    "Falha no processo de modifica��o",
    "Echec du montage!",
    "Feil under redigering!",
    "Muokkaus ep�onnistui!",
    "Proces edycji nie powi�d� si�!",
    "�Proceso de edici�n fallido!",
    "� ����������� �������!",
    "Redigeringsprocessen misslyckades",
    "Montajul �nregistr�rii a e�uat",
    "V�g�s sikertelen!",
    "Proc�s d'edici� fallit!",
    "������ �� ����� ������� ������!",
    "Rezanje neuspje�no!",
    "Redigeerimine eba�nnestus",
    "Redigeringsproces fejlede!",
    "Edita�n� proces selhal!",
  },
  { "Recording started",
    "Aufzeichnung gestartet",
    "Snemanje se je pri�elo",
    "Registrazione avviata",
    "",//TODO
    "",//TODO
    "Enregistrement a commenc�",
    "",//TODO
    "Tallennus aloitettu",
    "Rozpocz�to nagrywanie",
    "Iniciando grabaci�n",
    "",//TODO
    "Inspelningen har startat",
    "A �nceput �nregistrarea",
    "A felv�tel elinditva Nd",
    "",//TODO
    "������ ������",
    "",//TODO
    "Salvestamine algas",
    "Optagelse startet",
    "Za�alo nahr�v�n�",
  },
  { "Pausing live video...",
    "Live-Signal wird angehalten...",
    "Ustavljanje predvajanja v �ivo...",
    "Pausa del canale in visione...",
    "Pauzeren live uitzending...",
    "",// TODO
    "Pause de l'�mission en direct...",
    "",// TODO
    "Pys�ytet��n l�hetys...",
    "Pauzuj� program na �ywo...",
    "Pausa de la emisi�n en directo...",
    "������ �������� �������",
    "Pausar direktinspelningen",
    "Trec �n pauz� emisiunea transmis�...",
    "Az �l� ad�s meg�ll�tva...",
    "Pausa d'emissi� en directe...",
    "����� ����������� ���������...",
    "Zaustavljanje �ivog signala...",
    "Saate edastamise peatamine...",
    "Pausere live udsendelse...",
    "P�eru�uje se b��c� program ...",
  },
  { "Starting EPG scan",
    "Aktualisiere EPG-Daten",
    "Pri�enjam EPG-scan",
    "Inizio scansione EPG",
    "Bezig met starten EPG scan",
    "",// TODO
    "Mise � jour du guide des programmes",
    "",// TODO
    "Ohjelmaoppaan p�ivitys aloitettu",
    "Rozpoczynam skanowanie EPG",
    "Iniciando la exploraci�n de EPG",
    "���� ������ EPG",
    "P�b�rjar EPG skanning",
    "Pornesc achizi�ia EPG",
    "EPG adatok aktualiz�l�sa",
    "",// TODO
    "������� EPG-������������",
    "Po�injem EPG-scan",
    "EPG skaneerimine k�ivitatud",
    "Starter EPG skanning",
    "Za��n� prohled�v�n� EPG",
  },
  { "This plugin has no setup parameters!",
    "Dieses Plugin hat keine Parameter!",
    "Ta vstavek nima nastavitvenih parametrov!",
    "Questo plugin non ha parametri di configurazione!",
    "Deze plugin heeft geen instelparameters",
    "",// TODO
    "Ce plugin n'a pas de param�tres!",
    "",// TODO
    "T�ll� laajennoksella ei ole asetuksia!",
    "Ta wtyczka nie ma �adnych ustawie�!",
    "Este plugin no admite configuraci�n",
    "���� � �������� ��� ���� �����������!",
    "Den h�r modulen har inga parametrar",
    "Acest plugin nu se configureaz�!",
    "Ennek a plugin-nak nincs setup-parametere!",
    "Aquest plugin no admet configuraci�!",
    "������ �� ����� ���������� ���������!",
    "Ovaj dodatak (Plugin) nema parametara!",
    "Sellel laienudusmoodulil ei ole seadeid!",
    "Denne plugin har ingen parametre",
    "Modul nem� konfigura�n� parametry!",
  },
  { "Classic VDR",
    "Klassischer VDR",
    "Klasi�ni VDR",
    "VDR Classico",
    "VDR Klassiek",
    "",// TODO
    "VDR Classique",// TODO
    "",// TODO
    "Klassinen VDR",
    "Klasyczny VDR",
    "VDR cl�sico",
    "������� VDR",
    "Klassisk VDR",
    "VDR clasic",
    "Klasszikus VDR",
    "",// TODO
    "������������",
    "Klasi�ni VDR",
    "Klassikaline VDR",
    "Klassisk VDR",
    "Klasick� VDR",
  },
  { "ST:TNG Panels",
    "ST:TNG-Konsolen",
    "ST:TNG Konsola",
    "Consolle ST:TNG",
    "ST:TNG Consoles",
    "",// TODO
    "Consoles ST:TNG",
    "",// TODO
    "ST:TNG konsoli",
    "Panel ST:TNG",
    "Paneles ST:TNG",
    "������ ST:TNG",
    "ST:TNG konsol",
    "Cons. ST:TNG",
    "ST:TNG Konzol",
    "",// TODO
    "ST:TNG ������",
    "ST:TNG Konzole",
    "ST:TNG konsool",
    "ST:TNG konsol",
    "ST:TNG konzola",
  },
  { "No title",
    "Kein Titel",
    "Brez naziva",
    "Senza titolo",
    "Geen titel",
    "",// TODO
    "Sans titre",
    "",// TODO
    "Ei esityst�",
    "Bez tytu�u",
    "Sin t�tulo",
    "����� �����",
    "ingen titel",
    "F�r� titlu",
    "n�v n�lk�l",
    "",// TODO
    "��� ��������",
    "Bez titla",
    "Pealkiri puudub",
    "Ingen titel",
    "Bez n�zvu",
  },
  { "Setup.DVB$Hot Standby",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Apagado r�pido",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Setup.DVB$TV mode",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Modo TV",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Setup.DVB$VCR mode",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Modo VCR",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Rename recording",
    "Aufzeichnung umbenennen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Renommer l'enregistrement",
    "",//TODO
    "Nime� tallenne",
    "",//TODO
    "Renombrar grabaci�n",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "������������� ������",
    "",//TODO
    "�mbernimetamine",
    "",//TODO
    "",//TODO
  },
  { "Setup.OSD$Main menu command position",
    "Befehle Position im Hauptmen�",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Position des commandes dans le menu",
    "",//TODO
    "Komentojen sijainti p��valikossa",
    "",//TODO
    "Posici�n comandos men� principal",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "���������� ������ � ������� ����",
    "",//TODO
    "K�su asukoht peamen��s",
    "",//TODO
    "",//TODO
  },
  { "Setup.EPG$Show progress bar",
    "Zeitbalken anzeigen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Montrer la barre de progression",
    "",//TODO
    "N�yt� aikajana",
    "",//TODO
    "Mostrar barra de progreso",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Edenemisriba",
    "",//TODO
    "",//TODO
  },
  { "Setup.Recording$Show date",
    "Aufnahmedatum anzeigen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Montrer la date d'enregistrement",
    "",//TODO
    "N�yt� tallenteen p�iv�ys",
    "",//TODO
    "Mostrar fecha de la grabaci�n",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "���������� ����",
    "",//TODO
    "Salvestuse kuup�ev",
    "",//TODO
    "",//TODO
  },
  { "Setup.Recording$Show time",
    "AufnahmeZeit anzeigen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Montrer l'heure d'enregistrement",
    "",//TODO
    "N�yt� tallenteen ajankohta",
    "",//TODO
    "Mostrar hora de la grabaci�n",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "���������� ����� ������",
    "",//TODO
    "Salvestuse kellaaeg",
    "",//TODO
    "",//TODO
  },
  { "Setup.Recording$Show length",
    "L�nge der Aufnahme anzeigen",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Monter la longueur de l'enregistrement",
    "",//TODO
    "N�yt� tallenteen kesto",
    "",//TODO
    "Mostrar duraci�n de la grabaci�n",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "���������� ����������������� ������",
    "",//TODO
    "Salvestuse pikkus",
    "",//TODO
    "",//TODO
  },
  { "Path",
    "Pfad",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Dossiers",
    "",//TODO
    "Polku",
    "",//TODO
    "Ruta",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Teekond",
    "",//TODO
    "",//TODO
  },
  { "Date",
    "Datum",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "P�iv�ys",
    "",//TODO
    "Fecha",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Length",
    "L�nge",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Pituus",
    "",//TODO
    "Duraci�n",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Size",
    "Gr��e",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Koko",
    "",//TODO
    "Tama�o",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "est. bit-rate",
    "Bitrate ca.",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Bitrate est.",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "%s:\t%.2f MBit/s (Video+Audio)",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "%s:\t%.2f MBit/s (V�deo+Audio)",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Delete marks information?",
    "Marks l�schen?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Poista tallenteen merkinn�t?",
    "",//TODO
    "�Borrar marcas de informaci�n?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Delete resume information?",
    "Resume l�schen?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Poista tallenteen paluutiedot?",
    "",//TODO
    "�Borrar informaci�n de continuaci�n?",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
  },
  { "Rename$Up",
    "H�her",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Haut",
    "",//TODO
    "Ylemm�s",
    "",//TODO
    "Arriba",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "�les",
    "",//TODO
    "",//TODO
  },
  { "Rename$Down",
    "Tiefer",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Bas",
    "",//TODO
    "Alemmas",
    "",//TODO
    "Abajo",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Alla",
    "",//TODO
    "",//TODO
  },
  { "Rename$Next",
    "N�chster",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Suivant",
    "",//TODO
    "Seuraava",
    "",//TODO
    "Siguiente",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "J�rgmine",
    "",//TODO
    "",//TODO
  },
  { "Rename$Previous",
    "Vorheriger",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Pr�c�dent",
    "",//TODO
    "Edellinen",
    "",//TODO
    "Anterior",
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "",//TODO
    "Eelmine",
    "",//TODO
    "",//TODO
  },
  { "Setup.Replay$Jump Seconds",
    "Sprungweite in Sekunden",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Segundos a saltar",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Setup.Replay$Jump Seconds (repeated)",
    "Sprungweite in Sek. (wiederh.)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Segundos a saltar (repetici�n)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Setup.Replay$Jump Frames (repeated)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Frames a saltar (repetici�n)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Setup.Recording$Sort recordings by",
    "Aufnahmen sortieren nach",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ordenar grabaciones por",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Setup.Recording$Sort directories before recordings",
    "Verzeichnisse vor Aufnahmen einsortieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ordenar dirs antes que grabaciones",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "main dir alphabetically, subdirs flexible",
    "Alphabet f�r Haupt-, flexibel f�r Unterverzeichnisse",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "dir alfab�ticamente, subdirs flexible",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "main dir by date, subdirs flexible",
    "Datum f�r Haupt-, flexibel f�r Unterverzeichnisse",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "dir por fecha, subdirs flexible",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "all alphabetically",
    "alles alphabetisch",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "todo alfab�ticamente",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "all by date",
    "alles nach Datum",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "todo por fecha",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Sorting",
    "Sortierung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ordenando",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { NULL }
  };

// --- cI18nEntry ------------------------------------------------------------

class cI18nEntry : public cListObject {
private:
  const tI18nPhrase *phrases;
  const char *plugin;
public:
  cI18nEntry(const tI18nPhrase * const Phrases, const char *Plugin);
  const tI18nPhrase *Phrases(void) { return phrases; }
  const char *Plugin(void) { return plugin; }
  };

cI18nEntry::cI18nEntry(const tI18nPhrase * const Phrases, const char *Plugin)
{
  phrases = Phrases;
  plugin = Plugin;
}

// --- cI18nList -------------------------------------------------------------

class cI18nList : public cList<cI18nEntry> {
public:
  cI18nEntry *Get(const char *Plugin);
  const tI18nPhrase *GetPhrases(const char *Plugin);
  };

cI18nEntry *cI18nList::Get(const char *Plugin)
{
  if (Plugin) {
     for (cI18nEntry *p = First(); p; p = Next(p)) {
         if (strcmp(p->Plugin(), Plugin) == 0)
            return p;
         }
     }
  return NULL;
}

const tI18nPhrase *cI18nList::GetPhrases(const char *Plugin)
{
  cI18nEntry *p = Get(Plugin);
  return p ? p->Phrases() : NULL;
}

cI18nList I18nList;

// ---

void I18nRegister(const tI18nPhrase * const Phrases, const char *Plugin)
{
  cI18nEntry *p = I18nList.Get(Plugin);
  if (p)
     I18nList.Del(p);
  if (Phrases)
     I18nList.Add(new cI18nEntry(Phrases, Plugin));
}

const char *I18nTranslate(const char *s, const char *Plugin)
{
  if (Setup.OSDLanguage) {
     const tI18nPhrase *p = Plugin ? I18nList.GetPhrases(Plugin) : Phrases;
     if (!p)
        p = Phrases;
     for (int i = ((p == Phrases) ? 1 : 2); i--; ) {
         for (; **p; p++) {
             if (strcmp(s, **p) == 0) {
                const char *t = (*p)[Setup.OSDLanguage];
                if (t && *t)
                   return t;
                }
             }
         p = Phrases;
         }
     esyslog("%s%sno translation found for '%s' in language %d (%s)", Plugin ? Plugin : "", Plugin ? ": " : "", s, Setup.OSDLanguage, Phrases[0][Setup.OSDLanguage]);
     }
  const char *p = strchr(s, '$');
  return p ? p + 1 : s;
}

const char * const * I18nLanguages(void)
{
  return &Phrases[0][0];
}

const char * const * I18nCharSets(void)
{
  return &Phrases[1][0];
}

const char *I18nLanguageCode(int Index)
{
  return 0 <= Index && Index < I18nNumLanguages ? Phrases[2][Index] : NULL;
}

int I18nLanguageIndex(const char *Code)
{
  for (int i = 0; i < I18nNumLanguages; i++) {
      const char *s = Phrases[2][i];
      while (*s) {
            int l = 0;
            for ( ; l < 3 && Code[l]; l++) {
                if (s[l] != tolower(Code[l]))
                   break;
                }
            if (l == 3)
               return i;
            s++;
            }
      }
  //dsyslog("unknown language code: '%s'", Code);
  return -1;
}

const char *I18nNormalizeLanguageCode(const char *Code)
{
  for (int i = 0; i < 3; i++) {
      if (Code[i]) {
         // ETSI EN 300 468 defines language codes as consisting of three letters
         // according to ISO 639-2. This means that they are supposed to always consist
         // of exactly three letters in the range a-z - no digits, UTF-8 or other
         // funny characters. However, some broadcasters apparently don't have a
         // copy of the DVB standard (or they do, but are perhaps unable to read it),
         // so they put all sorts of non-standard stuff into the language codes,
         // like nonsense as "2ch" or "A 1" (yes, they even go as far as using
         // blanks!). Such things should go into the description of the EPG event's
         // ComponentDescriptor.
         // So, as a workaround for this broadcaster stupidity, let's ignore
         // language codes with unprintable characters...
         if (!isprint(Code[i])) {
            //dsyslog("invalid language code: '%s'", Code);
            return "???";
            }
         // ...and replace blanks with underlines (ok, this breaks the 'const'
         // of the Code parameter - but hey, it's them who started this):
         if (Code[i] == ' ')
            *((char *)&Code[i]) = '_';
         }
      else
         break;
      }
  int n = I18nLanguageIndex(Code);
  return n >= 0 ? I18nLanguageCode(n) : Code;
}

bool I18nIsPreferredLanguage(int *PreferredLanguages, const char *LanguageCode, int &OldPreference, int *Position)
{
  int pos = 1;
  bool found = false;
  while (LanguageCode) {
        int LanguageIndex = I18nLanguageIndex(LanguageCode);
        for (int i = 0; i < I18nNumLanguages; i++) {
            if (PreferredLanguages[i] < 0)
               break; // the language is not a preferred one
            if (PreferredLanguages[i] == LanguageIndex) {
               if (OldPreference < 0 || i < OldPreference) {
                  OldPreference = i;
                  if (Position)
                     *Position = pos;
                  found = true;
                  break;
                  }
               }
            }
        if ((LanguageCode = strchr(LanguageCode, '+')) != NULL) {
           LanguageCode++;
           pos++;
           }
        else if (pos == 1 && Position)
           *Position = 0;
        }
  if (OldPreference < 0) {
     OldPreference = I18nNumLanguages; // higher than the maximum possible value
     return true; // if we don't find a preferred one, we take the first one
     }
  return found;
}

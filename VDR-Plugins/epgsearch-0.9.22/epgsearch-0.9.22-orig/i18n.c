/*
 * autotimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.1 2004/04/24 10:08:10 hflor Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "search the EPG for repeats and more",
    "Suche im EPG nach Wiederholungen und anderem",// Deutsch
    "",// TODO Slovenski
    "Cerca nella guida EPG: per parole, repliche e pi�",// Italiano Sean Carlos 2005.01.16
    "zoek in het EPG naar herhalingen en meer",
    "",// TODO Portugues
    "Recherche de r�p�tition dans EPG",// Francais Patrice Staudt 4.1.2005
    "",// TODO Norsk
    "Monipuolinen ohjelmaopas hakutoiminnolla",
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO �������� (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
    "",// TODO ������� (Russian)
    "",// TODO Hrvatski (Croatian)
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "This plugin has no info text!",
    "Dieses Plugin hat keinen Infotext!",
    "",// TODO
    "Questo plugin non ha ulteriori informazioni!",// TODO
    "Deze plugin heeft geen info tekst",
    "",// TODO
    "Le plugin n'a pas de text d'info",
    "",// TODO
    "Laajennoksella ei ole infoteksti�!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Program guide",
    "Programmf�hrer",
    "",// TODO
    "Ricerca",// TODO
    "Zoek",
    "",// TODO
    "Guide du programme",
    "",// TODO
    "Ohjelmaopas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
 { "Button$by channel",
    "nach Programm",
    "",// TODO
    "per canale",// Italiano
    "per kanaal",
    "",// TODO
    "de programme",
    "",// TODO
    "Kanavat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$by time",
    "nach Beginn",
    "",// TODO
    "per orario",// Italiano 
    "bij tijd",
    "",// TODO
    "de d�but",
    "",// TODO
    "Kellonajat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "additional starting times",
    "zus�tzliche Startzeiten",
    "",// TODO
    "altri orari di inizio",// Italiano
    "toegevoegde start tijden",
    "",// TODO
    "temps de d�buts suppl�mentaires",
    "",// TODO
    "L�ydetyt aloitusajat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Repeats",
    "Wiederholung",
    "",// TODO
    "Repliche",// Italiano
    "Herhalingen",
    "",// TODO
    "R�p�titions",
    "",// TODO
    "Toistuvat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Search term",
    "Suche",
    "",// TODO
    "Ricerca",// Italiano
    "Zoek term",
    "",// TODO
    "Rechercher",
    "",// TODO
    "Hakutermi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search mode",
    "Suchmodus",
    "",// TODO
    "Modalit� di ricerca",// Italiano
    "Zoek mode",
    "",// TODO
    "Mode de recherche",
    "",// TODO
    "Hakutapa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "phrase",
    "Ausdruck",
    "",// TODO
    "frase",// Italiano
    "uitdruk",
    "",// TODO
    "Phrase",
    "",// TODO
    "fraasi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "all words",
    "alle Worte",
    "",// TODO
    "tutte le parole",// Italiano
    "alle woorden",
    "",// TODO
    "tout les mots",
    "",// TODO
    "kaikki sanat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "at least one word",
    "ein Wort",
    "",// TODO
    "almeno una parola",// Italiano
    "ten minste een woord",
    "",// TODO
    "un mot",
    "",// TODO
    "yksi sana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Match case",
    "Gro�/klein",
    "",// TODO
    "Maiuscolo/Minuscolo",// Italiano
    "Idem case",
    "",// TODO
    "Maj/Minuscule",
    "",// TODO
    "Huomioi kirjainkoko",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use title",
    "Verw. Titel",
    "",// TODO
    "Utilizzare titolo",// Italiano
    "gebruik titel",
    "",// TODO
    "Utiliser titre",
    "",// TODO
    "K�yt� ohjelmanime�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use subtitle",
    "Verw. Untertitel",
    "",// TODO
    "Utilizzare sottotitolo",// Italiano
    "Gebruik ondertitel",
    "",// TODO
    "Utiliser sous-titre",
    "",// TODO
    "K�yt� lyhytt� ohjelmakuvausta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "Use time",
    "Verw. Uhrzeit",
    "",// TODO
    "Utilizzare l'orario",// Italiano
    "Gebruik tijd",
    "",// TODO
    "Utiliser l'heure",
    "",// TODO
    "K�yt� aloitusaikaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use description",
    "Verw. Beschreibung",
    "",// TODO
    "Utilizzare descrizione",// Italiano
    "Gebruik beschrijving",
    "",// TODO
    "Utiliser la description",
    "",// TODO
    "K�yt� ohjelmakuvausta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use channel",
    "Verw. Kanal",
    "",// TODO
    "Utilizzare canale",// Italiano
    "Gebruik kanaal",
    "",// TODO
    "Utiliser le canal",
    "",// TODO
    "K�yt� kanavaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Channel",
    "  Kanal",
    "",// TODO
    "  Canale",// Italiano
    "  Kanaal",
    "",// TODO
    "  Canal",
    "",// TODO
    "  Kanava",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Start after",
    "  Start nach",
    "",// TODO
    "  Comincia dopo",// Italiano
    "  Start na",
    "",// TODO
    "  D�part apr�s",
    "",// TODO
    "  Aloitusaika aikaisintaan",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Start before",
    "  Start vor",
    "",// TODO
    "  Comincia prima",// Italiano
    "  Start voor",
    "",// TODO
    "  D�part avant",
    "",// TODO
    "  Aloitusaika viimeist��n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search results",
    "Suchergebnisse",
    "",// TODO
    "Risultati della ricerca",// Italiano
    "Zoek resultaten",
    "",// TODO
    "R�sultat de recherche",
    "",// TODO
    "Hakutulokset",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit search",
    "Suche editieren",
    "",// TODO
    "Modifica ricerca",// Italiano
    "Zoeken bewerken",
    "",// TODO
    "Edition recherche",
    "",// TODO
    "Muokkaa hakua",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Hide main menu entry",
    "Hauptmen�eintrag verstecken",
    "",// TODO
    "Nascondi voce nel men� principale",// Italiano
    "Verberg hoofd menu ingave",
    "",// TODO
    "Invisible dans le menu principal",
    "",// TODO
    "Piilota valinta p��valikosta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Replace original schedule",
    "Originale Programm�bersicht ersetzen",
    "",// TODO
    "Sostituisci scheda di programmazione originale",// Italiano
    "Originele inhoud terugzetten",
    "",// TODO
    "",
    "",// TODO
    "Korvaa alkuper�inen ohjelmaopas",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit$Delete search?",
    "Suche l�schen?",
    "",// TODO
    "Cancellare criteri?",// Italiano
    "Zoeken wissen",
    "",// TODO
    "Effacer la recherche",
    "",// TODO
    "Poistetaanko haku?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit$Search text too short - use anyway?",
    "Suchtext zu kurz - trotzdem verwenden?",
    "",// TODO
    "Il testo da cercare � troppo corto.  Continuare lo stesso?",// Italiano
    "Zoek tekst tekort - toch gebruiken?",
    "",// TODO
    "Texte de recherche est trop court - l'utiliser comme m�me?",
    "",// TODO
    "Liian suppea hakuehto - etsit��nk� silti?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Error$Missing search text!",
    "Fehlender Suchtext!",
    "",// TODO
    "Manca il testo da cercare!",// Italiano
    "Mis zoek tekst!",
    "",// TODO
    "Le texte de recherche manque!",
    "",// TODO
    "Hakuehto puuttuu!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "EPG Commands",
    "EPG Befehle",
    "",// TODO
    "Comandi EPG",// Italiano
    "EPG kommando's",// TODO
    "",// TODO
    "Commande EPG",
    "",// TODO
    "Komennot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Red key",
    "Taste Rot",
    "",// TODO
    "Tasto rosso",// Italiano
    "Rode toets",
    "",// TODO
    "Touche rouge",
    "",// TODO
    "Punainen n�pp�in",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Blue key",
    "Taste Blau",
    "",// TODO
    "Tasto blu",// Italiano
    "Blauwe toets",
    "",// TODO
    "Touche bleu",
    "",// TODO
    "Sininen n�pp�in",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "Standard",
    "Standard",
    "",// TODO
    "Standard",// Italiano
    "Standaard",
    "",// TODO
    "Standart",
    "",// TODO
    "Vakio",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Commands",
    "Befehle",
    "",// TODO
    "Comandi",// Italiano
    "Kommando's",
    "",// TODO
    "Commandes",
    "",// TODO
    "Komennot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Search",
    "Suche",
    "",// TODO
    "Cerca",// Italiano
    "Zoek",
    "",// TODO
    "Recherche",
    "",// TODO
    "Etsi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use duration",
    "Verw. Dauer",
    "",// TODO
    "Utilizzare durata",// Italiano
    "Gebruiks duur",
    "",// TODO
    "Dur�e d'utilisation",
    "",// TODO
    "K�yt� kestoaikaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Max. duration",
    "  Max. Dauer",
    "",// TODO
    "  Durata Massima",// Italiano
    "  Max. duur",
    "",// TODO
    "  Dur�e max.",
    "",// TODO
    "  Kestoaika enint��n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Min. duration",
    "  Min. Dauer",
    "",// TODO
    "  Durata Minima",// Italiano
    "  Min. duur",
    "",// TODO
    "  Dur�e min.",
    "",// TODO
    "  Kestoaika v�hint��n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Overview",
    "�bersicht",
    "",// TODO
    "Sommario",// Italiano
    "Overzicht",
    "",// TODO
    "Sommaire",
    "",// TODO
    "Yleiskatsaus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use user-defined time",
    "Verw. benutzerdef. Zeit",
    "",// TODO
    "Utilizzare ora utente",// Italiano
    "Gebruik gebruiker's tijd",
    "",// TODO
    "Utiliser le temps de l'utilisateur",
    "",// TODO
    "M��rittele ajankohta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Description",
    "Beschreibung",
    "",// TODO
    "Descrizione",// Italiano
    "Beschrijving",
    "",// TODO
    "Description",
    "",// TODO
    "Kuvaus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Time",
    "Zeit",
    "",// TODO
    "Orario",// Italiano
    "Tijd",
    "",// TODO
    "Temps",
    "",// TODO
    "Kellonaika",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show progress in 'Now'",
    "Zeige Fortschritt in 'Jetzt'",
    "",// TODO
    "Visualizza progresso in 'Adesso'",// Italiano
    "Toon voortgang in 'Nu'",
    "",// TODO
    "Afficher le progr�s dans 'Maintenant'",
    "",// TODO
    "N�yt� aikajana 'Nyt'-sivulla",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show progress in 'Next'",
    "Zeige Fortschritt in 'N�chste'",
    "",// TODO
    "Visualizza progresso in 'Prossimo'",
    "Toon voortgang in 'Volgende'",
    "",// TODO
    "Afficher le progr�s dans 'apr�s'",
    "",// TODO
    "N�yt� aikajana 'Seuraavaksi'-sivulla",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "  Display width",
    "  Anzeigenbreite",
    "",// TODO
    "  Larghezza visualizzazione",// Italiano
    "  Beeld breedte",
    "",// TODO
    "  Largeur d'affichage",
    "",// TODO
    "  Aikajanan leveys",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "Show channel numbers",
    "Zeige Kanalnummern",
    "",// TODO
    "Visualizzare i numeri dei canali",// Italiano
    "Toon kanaal nummers",
    "",// TODO
    "Afficher le num�ro de la cha�ne",
    "",// TODO
    "N�yt� kanavien numerointi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use search timers",
    "Verwende Suchtimer",
    "",// TODO
    "Abilitare creazione del timer di ricerca",// TODO
    "Gebruik zoek timer's",
    "",// TODO
    "Utiliser le programmeur de recherche",
    "",// TODO
    "K�yt� hakuajastimia",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Update interval [min]",
    "  Aktualisierungsintervall [min]",
    "",// TODO
    "  Intervallo d'aggiornamento",// Italiano
    "  Update interval [min]",
    "",// TODO
    "  Interval d'actualisation [min]",
    "",// TODO
    "  P�ivitysv�li [min]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use day of week",
    "Verw. Wochentag",
    "",// TODO
    "Utilizzare giorno della settimana",// Italiano
    "Gebruik dag van de week",
    "",// TODO
    "Utiliser les jours de la semaine",
    "",// TODO
    "K�yt� viikonp�iv��",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Day of week",
    "  Wochentag",
    "",// TODO
    "  Giorno della settimana",// Italiano
    "  Dag van de week",
    "",// TODO
    "  Jours de la semaine",
    "",// TODO
    "  Viikonp�iv�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use as search timer",
    "Als Suchtimer verwenden",
    "",// TODO
    "Utilizzare come timer di ricerca",// Italiano
    "Gebruik als zoek timer",
    "",// TODO
    "Utiliser la recherche",
    "",// TODO
    "K�yt� hakuajastimena",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "user-defined",
    "benutzerdefiniert",
    "",// TODO
    "definizione dell'utente",// TODO
    "door gebruiker gedefinieert",
    "",// TODO
    "definition d'utilisateur",
    "",// TODO
    "valitut",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit user-defined days of week",
    "Benutzerdefinierte Wochentage editieren",
    "",// TODO
    "Modificare giorni della settimana definiti dall'utente",// TODO
    "Bewerk door gebruiker gedefinieerde dagen van de week",
    "",// TODO
    "Edition des journ�es d�finit par l'utilisateur",
    "",// TODO
    "Muokkaa valittuja viikonp�ivi�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Create search",
    "Suche anlegen",
    "",// TODO
    "Crea ricerca",// TODO
    "Creeer zoeken",
    "",// TODO
    "Cr�er une recherche",
    "",// TODO
    "Luo haku",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  Series recording",
    "  Serienaufnahme",
    "",// TODO
    "  Registrazione serie",// Italiano
    "  Serie's opnemen",
    "",// TODO
    "  Enregistrement de serie",
    "",// TODO
    "  Sarjatallennus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Directory",
    "Verzeichnis",
    "",// TODO
    "Cartella",// Italiano
    "Directory",
    "",// TODO
    "Dossier",
    "",// TODO
    "Hakemisto",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "found recordings",
    "gefundene Aufnahmen",
    "",// TODO
    "registrazioni trovate",// Italiano 
    "gevonden opname's",
    "",// TODO
    "Enregistrements trouv�es",
    "",// TODO
    "l�ydetyt tallenteet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search in recordings",
    "Suche in Aufnahmen",
    "",// TODO
    "Cerca nelle registrazioni",// Italiano
    "Zoek in opname's",
    "",// TODO
    "Recherche dans enregistrement",
    "",// TODO
    "Etsi tallenteista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "graphical",
    "graphisch",
    "",// TODO
    "grafica",// Italiano
    "grafisch",
    "",// TODO
    "Graphique",
    "",// TODO
    "grafiikka",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  SVDRP port",
    "  SVDRP Port",
    "",// TODO
    "  Porta SVDRP",// Italiano
    "  SVDRP poort",
    "",// TODO
    "  SVDRP Port",
    "",// TODO
    "  SVDRP-portti",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Toggle Green/Yellow",
    "Gr�n/Gelb umschalten",
    "",// TODO
    "Alternare i tasti Verde/Giallo",// Italiano
    "Groen/Geel verwisselen",
    "",// TODO
    "Commuter vert/jaune",
    "",// TODO
    "K�yt� aikasiirtym�� v�rin�pp�imill�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Time interval for FRew/FFwd [min]",
    "Zeitintervall f�r FRew/FFwd [min]",
    "",// TODO
    "Intervallo tempo indietro veloce/avanti veloce [min]",// Italiano
    "Tijd interval voor FRew/FFwd [min]",
    "",// TODO
    "Interval pour FRew/FFwd [min]",
    "",// TODO
    "Oletusaikasiirtym� [min]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use short channel names",
    "Kurze Sendernamen",
    "",// TODO
    "Utilizzare i nomi corti dei canali",// Italiano
    "Gebruik korte kanaal namen",
    "",// TODO
    "Nom de Cha�ne court",
    "",// TODO
    "K�yt� lyhyit� kanavien nimi�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  from channel",
    "  von Kanal",
    "",// TODO
    "  da canale",// Italiano
    "  van kanaal",
    "",// TODO
    "  de la Cha�ne",
    "",// TODO
    "  Kanavasta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "  to channel",
    "  bis Kanal",
    "",// TODO
    "  a canale",// Italiano
    "  tot kanaal",
    "",// TODO
    "  � la Cha�ne",
    "",// TODO
    "  Kanavaan",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Start menu",
    "Starte mit",
    "",// TODO
    "Menu d'avvio",// TODO
    "Start menu",
    "",// TODO
    "Menue de d�part",
    "",// TODO
    "Oletusn�ytt�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Please check channel criteria!",
    "Bitte Senderkriterien pr�fen!",
    "",// TODO
    "Prego controllare criteri del canale!",
    "AUB kanaal criteria nakijken",
    "",// TODO
    "V�rifier le crit�re de la cha�ne!",
    "",// TODO
    "Tarkasta hakuehdot!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Title",
    "Titel",
    "",// TODO
    "Titolo",// Italiano
    "Titel",
    "",// TODO
    "Titre",
    "",// TODO
    "Nimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Episode",
    "Episode",
    "",// TODO
    "Episodio",// Italiano
    "Episode",
    "",// TODO
    "�pisode",
    "",// TODO
    "Jaksot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$only FTA",
    "ohne PayTV",
    "",// TODO
    "solo FTA",// Italiano
    "alleen FTA",
    "",// TODO
    "sans TV-Payante",
    "",// TODO
    "Vapaat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$all channels",
    "mit PayTV",
    "",// TODO
    "tutti i canali",// Italiano
    "alle kanalen",
    "",// TODO
    "avec TV-Payante",
    "",// TODO
    "Kaikki",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "%d timer conflict(s)! First at %s. Show them?",
    "%d Timer-Konflikt(e)! Erster am %s. Anzeigen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "%d de conflits de programmation! Le premier le %s. Afficher?",
    "",// TODO
    "P��llekk�isi� ajastimia (%d kpl %s)! N�ytet��nk�?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Announce only",
    "Nur ank�ndigen",
    "",// TODO
    "Solo annuncio (niente timer)",// Italiano
    "Alleen aankondigen (geen timer)",
    "",// TODO
    "Annoncer seulement d�but d'une programme",
    "",// TODO
    "muistutus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "match exactly",
    "exakt",
    "",// TODO
    "esatta corrispondenza",// Italiano
    "precies passend",
    "",// TODO
    "correspond exactement",
    "",// TODO
    "t�sm�llinen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "regular expression",
    "regul�rer Ausdruck",
    "",// TODO
    "espressione regolare",// Italiano
    "reguliere uitdrukking",
    "",// TODO
    "expression r�guliere",
    "",// TODO
    "s��nn�llinen lauseke",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  // The allowed characters in strings:
  { "$ abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a�bcdefghijklmno�pqrs�tu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a�bcde��fghi�jklmnopqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a�bc�de���fghi�jklmno�pqrstu��vwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a�bcde�fghi�jklmn�o�pqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " ��������������������������������0123456789-.#~abcdefghijklmnopqrstuvwxyz\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a��bcdefghi�jklmnopqrs�t�uvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a�bcde�fghi�jklmno��pqrstu��vwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " a�bc�de��fghi�jklmno�pqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz��������������������������������0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abc��d�efghijklmnopqrs�tuvwxyz�0123456789-.#~\\^$[]|()*+?{}/:%@&", // hrv
    " abcdefghijklmnopqrs�z�tuvw����xy�0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:%@&",
  },
  { "Ignore PayTV channels",
    "PayTV-Sender ignorieren",
    "",// TODO
    "Ignora i canali PayTV",// Italiano
    "Ignoriere PayTV kanalen",
    "",// TODO
    "Ignorer les cha�nes payantes",// Francais Patrice Staudt 1.12.2005
    "",// TODO
    "J�t� salatut kanavat huomioimatta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Select",
    "Auswahl",
    "",// TODO
    "Selezionare",// Italiano
    "Selecteer",
    "",// TODO
    "Selection",// Francais Pat
    "",// TODO
    "Valitse",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Select directory",
    "Verzeichnis w�hlen",
    "",// TODO
    "Selezionare una cartella",// Italiano
    "Selekteer directory",
    "",// TODO
    "Selectionner le dossier",// Francais Pat
    "",// TODO
    "Valitse hakemisto",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use extended EPG info",
    "Verw. erweiterte EPG Info",
    "",// TODO
    "Utilizzare informazioni EPG estesa",// Italiano
    "Gebruik uitgebreide EPG info",
    "",// TODO
    "Utiliser les infos EPG avanc�es",// Francais Pat
    "",// TODO
    "K�yt� laajennettua ohjelmaopasta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$With subtitle",
    "Mit Untertitel",
    "",// TODO
    "Con sottotitoli",// Italiano
    "Met ondertitel",
    "",// TODO
    "Avec les sous-titres",// Francais Pat
    "",// TODO
    "Kuvaus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Without subtitle",
    "Ohne Untertitel",
    "",// TODO
    "Senza sottotitoli",// Italiano
    "Zonder ondertitel",
    "",// TODO
    "Sans sous-titres",// Francais Pat
    "",// TODO
    "Ei kuvausta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Reset",
    "Zur�cksetzen",
    "",// TODO
    "Ripristino",// Italiano
    "resetten",
    "",// TODO
    "Annuler",// Francais Pat
    "",// TODO
    "Alusta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Level",
    "Ebene",
    "",// TODO
    "Livello",// Italiano
    "Level",
    "",// TODO
    "Niveau",// Francais Pat
    "",// TODO
    "Taso",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "interval",
    "Bereich",
    "",// TODO
    "intervallo",// Italiano
    "interval",
    "",// TODO
    "interval",// Francais Pat
    "",// TODO
    "kyll�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "channel group",
    "Kanalgruppe",
    "",// TODO
    "gruppo canali",// Italiano
    "kanaal groep",
    "",// TODO
    "Groupe de cha�nes",// Francais Pat
    "",// TODO
    "kanavaryhm�",
    "",// TODO
    "",// TODO  
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Channel group",
    "Kanalgruppe",
    "",// TODO
    "Gruppo canali",// Italiano
    "Kanaal groep",
    "",// TODO
    "Groupe de cha�nes",// Francais Pat
    "",// TODO
    "Kanavaryhm�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Channel groups",
    "Kanalgruppen",
    "",// TODO
    "Gruppi canali",// Italiano
    "Kanaal groepen",
    "",// TODO
    "Groupe de cha�nes",// Francais Pat
    "",// TODO
    "Kanavaryhm�t",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit$Delete group?",
    "Gruppe l�schen?",
    "",// TODO
    "Eliminare il gruppo?",// Italiano
    "Groep wissen?",
    "",// TODO
    "Effacer groupe $Delete?",// Francais Pat
    "",// TODO
    "Poistetaanko kanavaryhm�?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Channel group used by:",
    "Gruppe wird verwendet von:",
    "",// TODO
    "Gruppo canali utilizzato da",// Italiano
    "Kanaal groep gebruikt door:",
    "",// TODO
    "Groupe est utilis� par:",// Francais Pat
    "",// TODO
    "Kanavaryhm� k�yt�ss�:",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Group name",
    "Gruppenname",
    "",// TODO
    "Gruppo nome",// Italiano
    "Groep naam",
    "",// TODO
    "Nom de groupe",// Francais Pat
    "",// TODO
    "Kanavaryhm�n nimi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit channel group",
    "Kanalgruppe editieren",
    "",// TODO
    "Editare il gruppo canali",// Italiano
    "Bewerk kanaal groep",
    "",// TODO
    "Editer groupe de cha�nes",// Francais Pat
    "",// TODO
    "Muokkaa kanavaryhm��",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Group name is empty!",
    "Gruppenname ist leer!",
    "",// TODO
    "Il gruppo nomi e' vuoto!",// Italiano
    "Groep naam is leeg!",
    "",// TODO
    "Le nom est vide",// Francais Pat
    "",// TODO
    "Kanavaryhm�lt� puuttuu nimi!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Group name already exists!",
    "Gruppe existiert bereits!",
    "",// TODO
    "Il gruppo nome esiste gia'!",// Italiano
    "Groep naam existeerd al!",
    "",// TODO
    "Le groupe existe d�j�!",// Francais Pat
    "",// TODO
    "Samanniminen kanavaryhm� on jo olemassa!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "Button$Invert selection",
    "Ausw. umdrehen",
    "",// TODO
    "Invertire la selezione",// Italiano
    "Selectie omdraaien",
    "",// TODO
    "Inversion de la selection",// Francais Pat
    "",// TODO
    "P�invastoin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$All yes",
    "Alle ja",
    "",// TODO
    "Tutto si",// Italiano
    "Allen ja",
    "",// TODO
    "Tous oui",// Francais Pat
    "",// TODO
    "Kaikki",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$All no",
    "Alle nein",
    "",// TODO
    "Tutto no",// Italiano
    "Allen nee",
    "",// TODO
    "Tous non",// Francais Pat
    "",// TODO
    "Ei yht��n",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search timer",
    "Suchtimer",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Programmation de recherche",// Francais Pat
    "",// TODO
    "Hakuajastin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Avoid repeats",
    "Vermeide Wiederholung",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Eviter les r�p�tions",// Francais Pat
    "",// TODO
    "Est� uusinnat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Compare subtitle",
    "Vergleiche Untertitel",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer les sous-titres",// Francais Pat
    "",// TODO
    "Vertaa jakson nime�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Compare summary",
    "Vergleiche Beschreibung",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer les descriptions",// Francais Pat
    "",// TODO
    "Vertaa kuvausta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search entries",
    "Sucheintr�ge",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Entr�e de recherche",// Francais Pat
    "",// TODO
    "Hakutermit",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "active",
    "aktiv",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "actif",// Francais Pat
    "",// TODO
    "aktiivista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Show all",
    "Zeige alle",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Montrer tous",// Francais Pat
    "",// TODO
    "N�yt� kaikki",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "Edit$Delete entry?",
    "Eintrag l�schen?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer l'entr�e",// Francais Pat
    "",// TODO
    "Poista hakutermi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit$Delete all entries?",
    "Alle Eintr�ge l�schen?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer toutes les entr�es",// Francais Pat
    "",// TODO
    "Poista kaikki hakutermit",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Delete all",
    "Alle l�schen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer toutes",// Francais Pat
    "",// TODO
    "Poista kaikki",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Default recording dir",
    "Standard Aufn.-Verzeichnis",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Dossier d'enregistrement standart",// Francais Pat
    "",// TODO
    "Oletushakemisto tallenteille",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Allowed repeats",
    "Erlaubte Wiederholungen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "R�p�titions autoris�es",// Francais Pat
    "",// TODO
    "Sallittujen uusintojen lukum��r�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Compare title",
    "Vergleiche Titel",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer titres",// Francais Pat
    "",// TODO
    "Vertaa nime�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$by name",
    "nach Name",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "par nom",// Francais Pat
    "",// TODO
    "Nimi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$by date",
    "nach Datum",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "par la date",// Francais Pat
    "",// TODO
    "P�iv�m��r�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Mark as 'already recorded'?",
    "als 'bereits aufgezeichnet' markieren?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "marquer comme 'd�j� enregistr�'?",// Francais Pat
    "",// TODO
    "Merkitse tallennetuksi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use VDR's timer edit menu",
    "VDR's Timer-Edit-Menu verw.",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Utiliser le menu �dition VDR-programme",// Francais Pat
    "",// TODO
    "K�yt� alkuper�ist� ajastinvalikkoa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Run search timer update?",
    "Suchtimer-Update ausf�hren?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Mise � jours programmation de recherche?",// Francais Pat
    "",// TODO
    "P�ivitet��nk� hakuajastimet?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Compare",
    "Vergleiche",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer",// Francais Pat
    "",// TODO
    "Vertaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Compare categories",
    "Vergl. Kategorien",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer categories",// Francais Pat
    "",// TODO
    "Vertaa kategorioita",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Only repeats within ... days",
    "Nur Wiederh. innerhalb ... Tagen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Que r�p�tion, pendant ... jours",// Francais Pat
    "",// TODO
    "Vain uusinnat ... p�iv�n sis�ll�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },

  { "Programming timer failed!",
    "Timer-Programmierung fehlschlagen!",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "La programmation a �chou�",// Francais Pat
    "",// TODO
    "Ajastimen ohjelmointi ep�onnistui!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Timer preview",
    "Timer-Vorsch.",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Pr�vu de programmation",// Francais Pat
    "",// TODO
    "Esikatselu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Subtitle",
    "Untertitel",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Sous-titres",// Francais Pat
    "",// TODO
    "Jakson nimi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search actions",
    "Suchaktionen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Actions de recherche",// Francais Pat
    "",// TODO
    "Hakukomennot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Actions",
    "Aktionen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Actions",// Francais Pat
    "",// TODO
    "Komennot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Execute search",
    "Suche ausf�hren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effectuer la recherche",// Francais Pat
    "",// TODO
    "Suorita haku",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use as search timer on/off",
    "Als Suchtimer verwenden an/aus",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Utiliser comme recherche oui/non",// Francais Pat
    "",// TODO
    "Aseta/poista hakuajastintoiminto",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Trigger search timer update",
    "Suchtimer jetzt aktualisieren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Selection",// Francais Pat
    "",// TODO
    "P�ivit� hakuajastimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show recordings done",
    "Zeige erledigte Aufnahmen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les enregistrements effectu�s",// Francais Pat
    "",// TODO
    "N�yt� tehdyt tallennukset",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search timer update done!",
    "Suchtimer-Update durchgef�hrt!",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "La mise � jours de recherche est effectu�e!",// Francais Pat
    "",// TODO
    "Hakuajastimet p�ivitetty!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show channel separators",
    "Zeige Kanal-Separatoren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher le separateur de cha�nes",// Francais Pat
    "",// TODO
    "N�yt� kanavaerottimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show channels without EPG",
    "Zeige Kan�le ohne EPG",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les cha�nes sans EPG",// Francais Pat
    "",// TODO
    "N�yt� ohjelmaoppaattomat kanavat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { ">>> no info! <<<",
    ">>> keine Info! <<<",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    ">>> Pas d'information! <<<",// Francais Pat
    "",// TODO
    ">>> ei tietoja! <<<",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Delete recordings after ... days",
    "Aufn. nach ... Tagen l�schen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer l'enregistrement apr�s ... jours",// Francais Pat
    "",// TODO
    "Poista tallenteet ... p�iv�n j�lkeen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show episode text in schedules",
    "Zeige Untertitel in �bersicht",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les sous-titre dans le programme",
    "",// TODO
    "N�yt� jakson nimi ohjelmistossa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Templates",
    "Vorlagen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Mod�les",
    "",// TODO
    "Mallipohjat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Search templates",
    "Such-Vorlagen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Rechercher mod�les",
    "",// TODO
    "Mallipohjat hakuehdoille",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit template",
    "Vorlage editieren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Editer mod�les",
    "",// TODO
    "Muokkaa mallipohjaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Template name",
    "Vorlagenname",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Nom du mod�le",
    "",// TODO
    "Mallipohjan nimi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use as template",
    "Als Vorlage verwenden",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Utiliser comme mod�les",
    "",// TODO
    "K�yt� mallipohjana",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Copy this entry to templates?",
    "Diesen Eintrag in Vorlagen kopieren?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Copier cette entr�e dans mod�les?",
    "",// TODO
    "Kopioidaanko hakutermi mallipohjaksi?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Keep ... recordings",
    "Behalte ... Aufnahmen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Garder .... enregistrements",
    "",// TODO
    "S�ilyt� ... tallennetta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Overwrite existing entries?",
    "Vorhandene Werte �berschreiben?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Ecraser les informations?",
    "",// TODO
    "Kirjoitetaanko olemassaolevan p��lle?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Default",
    "Standard",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Standart",
    "",// TODO
    "Oletus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Setup",
    "Einstellungen",
    "Nastavitve",
    "Opzioni",
    "Instellingen",
    "Configurar",
    "Configuration",
    "Konfigurasjon",
    "Asetukset",
    "Nastawy",
    "Configuraci�n",
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
  { "Button$Edit",
    "Editieren",
    "Uredi",
    "Modifica",
    "Verander",
    "Modificar",
    "Modifier",
    "Editer",
    "Muokkaa",
    "Edycja",
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
    "Usunac",
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
  { "Button$Record",
    "Aufnehmen",
    "Posnemi",
    "Registra",
    "Opnemen",
    "Gravar",
    "Enregistre",
    "Ta opp",
    "Tallenna",
    "Nagrywac",
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
    "Programa",
    "",// TODO
    "Timer",
    "Timer",
    "",// TODO
    "",// TODO
    "������",
    "",// TODO
    "Taimer",
    "Timer",
    "�asova�",
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
  { "recording with device",
    "Aufnahme mit Ger�t",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Enregistrement avec appareil",
    "",// TODO
    "Tallennetaan laitteella",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Add episode to manual timers",
    "Untertitel in manuellen Timern",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Ajouter sous-titre dans la programmation manuel",
    "",// TODO
    "Lis�� jakson nimi norm. ajastimiin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "never",
    "nie",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "jamais",
    "",// TODO
    "ei koskaan",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "always",
    "immer",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "toujours",
    "",// TODO
    "aina",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "smart",
    "intelligent",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "intelligent",
    "",// TODO
    "�lykk��sti",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Action",
    "Aktion",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Action",
    "",// TODO
    "Toiminto",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show day separators",
    "Zeige Tages-Separatoren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les s�parateur de jours",// TODO
    "",// TODO
    "N�yt� p�iv�erottimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Add/Remove to/from switch list?",
    "In/Aus Umschaltliste?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajouter/Effacer de la liste de changement?",
    "",// TODO
    "Lis��/poista kanavanvaihtolistalle",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Add to switch list?",
    "In Umschaltliste aufnehmen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajouter � la liste de changement de chaine?",
    "",// TODO
    "Lis�t��nk� kanavanvaihtolistalle?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Delete from switch list?",
    "Aus Umschaltliste entfernen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer de la liste de changement?",
    "",// TODO
    "Poistetaanko kanavanvaihtolistalta?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Switch list",
    "Umschaltliste",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Liste de changement de chaine",// TODO
    "",// TODO
    "Kanavanvaihtolista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Already running!",
    "L�uft bereits!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Est d�j� en cours",
    "",// TODO
    "Nyt menossa!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Show switch list",
    "Zeige Umschaltliste",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher liste de changement",// TODO
    "",// TODO
    "N�yt� kanavanvaihtolista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Switch only",
    "Nur umschalten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Seulement changer de chaine",
    "",// TODO
    "kanavanvaihto",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Switch ... minutes before start",
    "Umschalten ... Minuten vor Start",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Changer ... minutes avant le d�but",// TODO
    "",// TODO
    "Vaihda ... minuuttia ennen alkua",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Edit entry",
    "Eintrag editieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Editer l'entr�e",// TODO
    "",// TODO
    "Muokkaa valintaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "action at",
    "Ausf�hrung um",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effectuer �",// TODO
    "",// TODO
    "Kellonaika kanavanvaihdolle",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Record",
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
  { "only FTA",
    "ohne PayTV",
    "",// TODO
    "solo FTA",// Italiano
    "alleen FTA",
    "",// TODO
    "sans TV-Payante",
    "",// TODO
    "vapaat kanavat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Standard",
    "Standard",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Standard",// TODO
    "",// TODO
    "Suppea",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Extended",
    "Erweitert",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pr�cision",// TODO
    "",// TODO
    "Laaja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Pause when ... recordings exist",
    "Pause, wenn ... Aufnahmen exist.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pause, lorsque ... l'enregistrement existe.",// TODO
    "",// TODO
    "Keskeyt� ... tallenteen j�lkeen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Blacklists",
    "Ausschlusslisten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Listes des exclus",
    "",// TODO
    "Mustat listat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show blacklists",
    "Zeige Ausschlusslisten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher la liste des exclus",// TODO
    "",// TODO
    "N�yt� mustat listat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use blacklists",
    "Verw. Ausschlusslisten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser la liste des exclus",// TODO
    "",// TODO
    "K�yt� mustia listoja",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Selection",
    "Auswahl",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Selection",// TODO
    "",// TODO
    "valittu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "all",
    "alle",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "tous",// TODO
    "",// TODO
    "kaikki",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit$Delete blacklist?",
    "Ausschlussliste l�schen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer la liste des exclus",// TODO
    "",// TODO
    "Poistetaanko musta lista?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit blacklist",
    "Ausschlussliste editieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Editer la liste des exclus",// TODO
    "",// TODO
    "Muokkaa mustaa listaa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Select blacklists",
    "Ausschlusslisten ausw�hlen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Choisir la liste des exclus",// TODO
    "",// TODO
    "Valitse mustat listat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Blacklist results",
    "Ausschluss-Ergebnisse",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "R�sultats de la liste des exclus",// TODO
    "",// TODO
    "Mustan listan tulokset",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Edit$Delete template?",
    "Vorlage l�schen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer le mod�le",// Francais
    "",// TODO
    "Poistetaanko mallipohja?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Summary",
    "Inhalt",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Contenu",// TODO
    "",// TODO
    "Yhteenveto",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Auxiliary Info",
    "Zusatzinfo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Informations suppl�mentaires",// TODO
    "",// TODO
    "Lis�tiedot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Button$Aux info",
    "Zusatzinfo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Infos suppl�mentaires",
    "",// TODO
    "Lis�tiedot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Create blacklist",
    "Ausschlussliste anlegen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Cr�er la liste d'exclusions",
    "",// TODO
    "Luo musta lista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Disable associated timers too?",
    "Zugeh�rige Timer auch deaktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "D�sactiver la programmation correspondante",
    "",// TODO
    "Poistetaanko k�yt�st� my�s assosioidut ajastimet?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Activate associated timers too?",
    "Zugeh�rige Timer auch aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Activer la progammation correspondante",// TODO
    "",// TODO
    "Otetaanko k�ytt��n my�s assosioidut ajastimet?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Main menu entry",
    "Hauptmen�-Eintrag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Visible dans le menu principal",// TODO
    "",// TODO
    "Valinta p��valikossa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Delete created timers?",
    "Erstellte Timer l�schen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer les programmations cr�es",// TODO
    "",// TODO
    "Poista haulla luodut ajastimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Delete all timers created from this search?",
    "Alle Timer l�schen, die von dieser Suche erzeugt wurden?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer les programmations issues de cette recherche?",
    "",// TODO
    "Poistetaanko kaikki t�ll� haulla luodut ajastimet?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Use Wareagle icons",
    "Verw. Wareagle-Symbole",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les symboles de Wareagle",// TODO
    "",// TODO
    "K�yt� Wareagle-symboleita",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Show radio channels",
    "Zeige Radiokan�le",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les cha�nes de radio",// TODO
    "",// TODO
    "N�yt� radiokanavat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "pipes",
    "Pipes",
    "",// TODO
    "barre verticali",// Italiano
    "pipes",
    "",// TODO
    "barre verticale",
    "",// TODO
    "teksti", // 'text'/'ascii' as propotional for graphical
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "fuzzy",
    "unscharf",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "impr�cis",
    "",// TODO
    "sumea",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "Tolerance",
    "Toleranz",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Tol�rance",// TODO
    "",// TODO
    "Toleranssi",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)

  },
  { "'One press' timer creation",
    "Timer mit 'Aufnehmen' sofort anlegen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Programmation imm�diate avec enregistrement",
    "",// TODO
    "Luo ajastimet yhdell� painalluksella",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Ignore below priority",
    "Ignoriere unter Priorit�t",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ignorer la priorit� inf�rieure",// Francais Patrice Staudt
    "",// TODO
    "Sivuuta alhaisemmat prioriteetit",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Only check within next ... days",
    "Pr�fe nur die n�chsten ... Tage",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ne v�rifier que les prochains ... jours",
    "",// TODO
    "Tarkista vain seuraavat ... p�iv��",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Timer conflict check",
    "Auf Timer-Konflikte pr�fen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "V�rifier les conflits de programmation",
    "",// TODO
    "Tarkista p��llekk�iset ajastimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Timer conflicts",
    "Timer-Konflikte",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Conflits de programmation",
    "",// TODO
    "P��llekk�iset ajastimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "conflicts",
    "Konflikte",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Conflits",
    "",// TODO
    "p��llekk�isyytt�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$Details",
    "Details",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "D�tails",
    "",// TODO
    "Lis�tiedot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$Repeats",
    "Wiederh.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "R�p�titions",
    "",// TODO
    "Toistuvat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$Filter",
    "Filter",
    "",// TODO
    "",// TODO
    "",// TODO
    "Filtre",
    "",// TODO
    "Suodata",
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
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "no conflicts!",
    "keine Konflikte!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pas de conflits",
    "",// TODO
    "ei p��llekk�isyyksi�!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "no important conflicts!",
    "keine wichtigen Konflikte!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pas de conflits importants!",
    "",// TODO
    "ei merkitsevi� p��llekk�isyyksi�!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "C", // Label for a conflicting timer
    "K",
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO
    "C",// TODO Eesti
    "C",// TODO Dansk
    "C",// TODO �esky (Czech)
  },
  { "General",
    "Allgemein",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "G�n�ral",
    "",// TODO
    "Yleiset",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "EPG menus",
    "EPG Men�s",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Menu EPG",
    "",// TODO
    "Valikot",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Timer programming",
    "Timer-Programmierung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Programmation",
    "",// TODO
    "Ajastimien luonti",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Search and search timers",
    "Suche und Suchtimer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Recherche et progammation de recherche",
    "",// TODO
    "Haut ja hakuajastimet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Timer conflict checking",
    "Timer-Konflikt-Pr�fung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "V�rification-Conflits-Programmation",
    "",// TODO
    "P��llekk�isten ajastimien tarkistus",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "--- Automatic checking ---",
    "--- Automatische Pr�fung ---",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "--- V�rifications automatiques ---",
    "",// TODO
    "--- Automaattinen tarkistus ---",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "After each search timer update",
    "Nach jedem Suchtimer-Update",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mise � jours apr�s chaque recherche",
    "",// TODO
    "P�ivitett�ess� hakuajastimia",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "User-defined EPG times",
    "Benutzerdef. EPG-Zeiten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Date utilisateur pour la progammation",
    "",// TODO
    "K�ytt�j�n m��rittelem�t ajankohdat",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help",
    "Hilfe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aide",
    "",// TODO
    "Opaste",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$Help",
    "Hilfe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aide",
    "",// TODO
    "Opaste",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Hides the main menu entry and may be useful if this plugin is used to replace the original 'Schedule' entry.",
    "Blendet den Hauptmen�eintrag aus. Das kann n�tzlich sein, falls dieses Plugin verwendet wird, um den originalen 'Programm'-Eintrag zu ersetzen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Rend le menu invisible. C'est utile lorsque le plugin remplace le menu programmme d'origine",
    "",// TODO
    "Piilottaa valinnan p��valikosta. Suositellaan asetettavaksi p��lle, jos laajennoksella korvataan alkuper�inen ohjelmaopas.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$The name of the main menu entry which defaults to 'Programm guide'.",
    "Der Name des Hauptmen�eintrags. Die Standard-Vorgabe ist 'Programmf�hrer'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Le nom dans le menu principale. La pr�d�finition est 'Guide du programme'. ",
    "",// TODO
    "P��valikossa sijaitsevan valinnan nimi, joka on oletuksena 'Ohjelmaopas'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$When VDR is patched to allow this plugin to replace the original 'Schedule' entry, you can de/activate this replacement here.",
    "Falls VDR gepatched wurde, um dem Plugin zu erlauben, den originalen 'Programm'-Eintrag zu ersetzen, so kann hier diese Ersetzung de/aktiviert werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsque le VDR est muni du patch pour autoriser le remplacement du menu original 'Programme' , vous pouvez activer ou d�activer celui-ci.",
    "",// TODO
    "Korvaa alkuper�isen ohjelmaoppaan p��valikossa t�ll� laajennoksella, jos VDR on paikattu asianmukaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Choose between 'Overview - Now' and 'Schedule' as start menu when this plugin is called.",
    "Hier kann zwischen '�bersicht - Jetzt' und 'Programm' als Start-Men� f�r das Plugin gew�hlt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez choisir entre 'Maintenant' et 'Programme' comme menu de d�part pour le plugin.",
    "",// TODO
    "Valitsee oletusn�yt�ksi joko t�m�nhetkisen yleiskatsauksen tai valitun kanavan ohjelmiston.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Choose which standard function ('Record' or 'Commands') you like to have on the red key.\n(Can be toggled with key '0')",
    "Hier kann ausgew�hlt werden, welche Standardfunktion ('Aufnehmen' oder 'Befehle') auf der roten Taste liegen soll.\n(Umschalten auch mit Taste '0' m�glich)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez choisir entre la fonction standart 'Enregistrer' et 'Commande' comme menu de la touche rouge.\n(Changement possible avec la touch '0')",
    "",// TODO
    "Valitsee oletustoiminnon ('Tallenna' tai 'Komennot') punaiselle n�pp�imelle.\nPunaisen n�pp�imen toimintoa voidaan vaihtaa my�s lennossa painamalla '0' n�pp�int�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Choose which standard function ('Switch'/'Info' or 'Search') you like to have on the blue key.\n(Can be toggled with key '0')",
    "Hier kann ausgew�hlt werden, welche Standardfunktion ('Umschalten'/'Info' oder 'Suche') auf der blauen Taste liegen soll.\n(Umschalten auch mit Taste '0' m�glich)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez choisir entre la fonction standart 'Changer de chaine' et 'Recherche' comme menu de la touche bleu.\n(Changement possible avec la touch '0')",
    "",// TODO
    "Valitsee oletustoiminnon ('Valitse' tai 'Etsi') siniselle n�pp�imelle.\nSinisen n�pp�imen toimintoa voidaan vaihtaa my�s lennossa painamalla '0' n�pp�int�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Shows a progressbar in 'Overview - Now' that informs about the remaining time of the current event.\nDepending on your selected skin you can choose between 'Pipes' or 'graphical' to get the best look.",
    "Zeigt einen Fortschrittsbalken in '�bersicht - Jetzt' an, der wiedergibt wieweit die Sendung bereits fortgeschritten ist.\nAbh�ngig vom gew�hlten Skin kann man zwischen 'Pipes' und 'grafisch' w�hlen, um die Darstellung zu verbessern.",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Affiche la barre de progression dans le menu 'Maintenant', qui indique l'avancement de l'�mission en cours.\nL'afficher est mieu appropri� suivant le skin choisi en 'Bar verticale' ou 'graphique'.",
    "",// TODO
    "N�ytt�� aikajanan t�m�nhetkisess� yleiskatsauksessa.\nVoit valita k�ytt�m�si ulkoasun mukaan joko teksti- tai grafiikkapohjaisen aikajanan.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Display channel numbers in 'Overview - Now'.\n\n(To completely define your own menu look please inspect the MANUAL)",
    "Anzeige von Kanalnummern in '�bersicht - Jetzt'.\n\n(Zur vollst�ndigen Anpassung einer eigenen Men�darstellung, bitte das MANUAL lesen)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Affiche du num�ro de cha�ne dans la vue 'Maintenant'.\n\n(Pour les r�glages de l'affichage menu propre � l'utilisateur, lire le MANUAL)",
    "",// TODO
    "N�ytt�� kanavien numeroinnin t�m�nhetkisess� yleiskatsaksessa.\n\nLis�tietoja valikon muokkauksesta l�ytyy MANUAL-tiedostosta.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Display VDR channel groups as separators between your channels in 'Overview - Now'.",
    "Anzeige von VDR-Kanalgruppen als Trennlinien zwischen den Programmen in '�bersicht - Jetzt'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les groupes de chaines avec une ligne de s�paration dans la vue 'Maintenant'.",
    "",// TODO
    "N�ytt�� erottimet yleiskatsauksessa kanavaville.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Display a separator line at day break in 'Schedule'.",
    "Anzeige von Trennlinien beim Datumswechsel in der Programm�bersicht.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher une ligne de s�paration au changement de date dans la vue 'programme'.",
    "",// TODO
    "N�ytt�� erottimet ohjelmistossa p�iv�n vaihtuessa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Show also radio channels.",
    "Auch Radio-Kan�le anzeigen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher aussi les cha�nes de radio.",
    "",// TODO
    "N�ytt�� my�s radiokanavat listauksessa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$When a timer is created with 'Record' you can choose between an immediate creation of the timer or the display of the timer edit menu.",
    "Beim Erzeugen eines Timers mit 'Aufnehmen' kann hier zwischen dem sofortigen Anlegen des Timers oder der Anzeige des Timer-Edit-Men�s gew�hlt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lors de la programmation avec 'Enregistrer' vous pouvez choisir entre cr�er directement ou consulter les d�tails de la programmation.",
    "",// TODO
    "Valitsee oletustoiminnon ajastimen luonnille. Voit luoda ajastimen joko automaattisesti yhdell� painalluksella tai vaihtoehtoisesti avata ajastinvalikon.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Choose 'yes' here if you want to display channels without EPG in 'Overview - Now'. 'Ok' on these entries switches the channel.",
    "Hier kann eingestellt werden, ob auch Programme ohne EPG in '�bersicht - Jetzt' erscheinen sollen. Ein 'Ok' auf diesen Eintr�gen schaltet zu diesem Kanal um.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez faire disparaitre ou non les chaines sans infos EPG dans la vue 'Maintenant'. Un 'Ok' sur le programme change sur cette cha�nes.",
    "",// TODO
    "N�ytt�� t�m�nhetkisess� yleiskatsauksessa my�skin kanavat, joilta ei l�ydy ohjelmatietoja. Voit vaihtaa t�ll�iselle kanavalle 'OK' n�pp�imell�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Choose here the time interval which should be used for jumping through the EPG by pressing FRew/FFwd.\n\n(If you don't have those keys, you can toggle to this functionality pressing '0' and get '<<' and '>>' on the keys green and yellow)",
    "Hier kann die Zeitspanne eingestellt werden, die beim Dr�cken von FRew/FFwd als Sprung durch den EPG benutzt werden soll.\n\n(Falls diese Tasten nicht vorhanden sind, kann mit '0' ebenfalls diese Funktion erreicht werden. Man erh�lt dann '<<' und '>>' auf den Tasten Gr�n/Gelb)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous r�glez un laps de temps, qui est utilis� avec les touches '<<' et '>>' � travers les EPG.\n\n(Si vous n'avez pas ces touches sur votre t�l�commande, vous pouvez aussi atteindre ces fonctions par la touche '0' gr�ce � laquelle on obtient les fonctions '<<' et '>>' sur les touches Vert/Jaune)",
    "",// TODO
    "Valitsee ohjelmaoppaan oletusaikasiirtym�n pikakelausn�pp�imille.\n\nJos sinulla ei ole kyseisi� n�pp�imi� k�yt�ss�si, voit asettaa kyseiset toiminnot vihre�lle ja keltaiselle n�pp�imelle painamalla '0' n�pp�int�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify if green and yellow shall also be switched when pressing '0'.",
    "Hiermit wird angegeben, ob die Tasten Gr�n/Gelb beim Dr�cken von '0' mitumgeschaltet werden sollen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez, si les touches Verte/Jaune sont commut�es avec la touches '0'.",
    "",// TODO
    "M��rittelee vaihdetaanko vihre�n ja keltaisen n�pp�imen toimintoja painamalla '0' n�pp�int�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Besides 'Now' and 'Next' you can specify up to 4 other times in the EPG which can be used by repeatedly pressing the green key, e.g. 'prime time', 'late night',...",
    "Neben 'Jetzt' und 'N�chste' k�nnen bis zu 4 weitere Zeiten im EPG angegeben werden, die durch wiederholtes Dr�cken der Taste Gr�n verwendet werden k�nnen, z.B. 'Abends', 'Sp�tabend',...",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "En plus de 'Maintenant' et 'Apr�s' vous pouvez d�finir encore 4 autres cr�naux comme 'Le soir', 'La nuit' ou 'encore plus tard' en appuyant plusieurs fois sur la touche verte...",
    "",// TODO
    "M��rittelee k�ytt�j�n muokattavissa olevat ajankohdat 'Nyt' ja 'Seuraavaksi' n�ytt�jen rinnalle ohjelmaoppaan yleiskatsaukseen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$This is the description for your user-defined time as it will appear as label on the green button.",
    "Dies ist die Beschreibung f�r die benutzer-definierte Zeit, wie sie als Beschriftung f�r die Taste Gr�n verwendet werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici la description des temps utilisateurs, accessible en appuyant plusieurs foix sur la touche verte",
    "",// TODO
    "Asettaa kuvauksen vihre��n n�pp�imeen k�ytt�j�n muokkaamalle ajankohdalle.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify the user-defined time here in 'HH:MM'.",
    "Hier wird die benutzer-definierte Zeit im Format 'HH:MM' angegeben.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous d�finisez le temps utilisateur dans le format 'HH:MM' par exemple '20:45'",
    "",// TODO
    "Asettaa kellonajan k�ytt�j�n muokkaamalle ajankohdalle.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$This plugin has it's own timer edit menu extending the original one with some extra functionality like\n- an addtional directory entry\n- user-defined days of week for repeating timers\n- adding an episode name\n- support for EPG variables (see MANUAL)",
    "Diese Plugin hat sein eigenes Timer-Edit-Men�, das das orginale um einige zus�tzliche Funktionen erweitert:\n\n- ein zus�tzlicher Verzeichniseintrag\n- benutzerdef. Wochentage f�r Wiederh.-Timer\n- Erg�nzung um Episodenname\n- Unterst�tzung von EPG-Variablen (s.MANUAL)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ce plugin a sont propre �diteur de programmation, qui enrichit l'originale avec beaucoup le fonctions interressantes:\n\n- une entr�e de dossier suppl�mentaire\n- des programmations hebdomadaires d�finies par l'utilisateur\n- l'utilisation de variable de l'EPG (voire MANUAL)",
    "",// TODO
    "T�ll� laajennoksella on oma laajennettu ajastinvalikko mm. seuraavilla lis�toiminnoilla:\n\n- hakemiston m��ritys\n- vapaasti m��ritelt�v�t viikonp�iv�t toistuville ajastimille\n- jakson nimen t�ydennys\n- erilaisten EPG-muuttujien tuki (kts. MANUAL-tiedostosta)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$When creating a timer you can specify here a default recording directory.",
    "Beim Anlegen eines Timers kann hier ein Standard-Aufnahmeverzeichnis vorgegeben werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lors de la programmation vous pouvez d�finir un dossier par d�faut.",
    "",// TODO
    "M��rittelee oletushakemiston tallenteille.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$If you create a timer for a series, you can automatically add the episode name.\n\n- never: no addition\n- always: always add episode name if present\n- smart: add only if event lasts less than 80 mins.",
    "Wenn man einen Timer f�r eine Serie erzeugt, kann mit dieser Option automatisch der Episodenname erg�nzt werden.\n\n- niemals: keine Erg�nzung\n- immer: immer erg�nzen, falls Episodenname vorhanden.\n- intelligent: nur erg�nzen, wenn Sendung weniger als 80 min dauert",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsque l'on cr�� une programmation d'une s�rie, on peut voir automatiquement le nom de l'�pisode.\n\n- Jamais: sans nom d'�pisode\n- Toujours:Compl�ter toujours avec le noms de l'�pisode, si existant.\n- intelligent: compl�ter, si l'�pisode dure moins de 80 min",
    "",// TODO
    "Asettaa toiminnon sarjatallennuksen jakson nimen lis�ykselle:\n\n- ei koskaan: ei lis�t�\n- aina: lis�t��n aina, jos olemassa\n- �lykk��sti: lis�t��n vain, jos tapahtuman kesto on alle 80 min",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$'Search timers' can be used to automatically create timers for events that match your search criterions.",
    "'Suchtimer' k�nnen verwendet werden um automatisch Timer f�r Sendungen zu erstellen, die von einer Suche gefunden werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "'Programmation de recherche' peut �tre utilis� pour programmer automatiquement des �missions, qui sont trouv�es par la recherche.",
    "",// TODO
    "Hakuajastimilla luodaan automaattisesti ajastimia hakuehtoihin sopiviin tapahtumiin.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify here the time intervall to be used when searching for events in the background.",
    "Hier kann das Zeitintervall angegeben werden, in dem im Hintergrund automatisch nach neuen Sendungen gesucht werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'interval avec lequel de nouvelles �missions a programmer sont recherch�es.",
    "",// TODO
    "M��rittelee p�ivitysv�lin hakuajastimien taustap�ivitykselle.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Programming of new timers or timer changes is done with SVDRP. The default value should be correct, so change it only if you know what you are doing.",
    "Die Programmierung neuer Timer oder Timer-�nderungen erfolgt mit SVDRP. Der Vorgabewert hier sollte immer korrekt sein, also nur �ndern, wenn man wei�, was man tut.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La programmation de nouveaux enregistrements ou changements de programmation est r�alis� avec SVDRP. La valeur par d�faut doit absolument �tre correcte, ne la changez que si vous savez ce que vous faites.",
    "",// TODO
    "Uusien ajastimien luonti ja olemassa olevien muokkaus on toteutettu SVDRP-protokollan kautta. Suositellaan k�ytett�v�n vain oletusarvoa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify here the default priority of timers created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Priorit�t f�r Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous d�finissez la priorit� standard de la programmation cr�e par le plugin. Cette valeur peut �tre r�gl�e de facon individuelle pour chaque recherche.",
    "",// TODO
    "M��rittelee oletusprioriteetin t�m�n laajennoksen kautta luotaville ajastimille. Prioriteettia voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify here the default lifetime of timers/recordings created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Lebensdauer f�r Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous d�finissez la dur�e de vie de l'enregistrement cr�e par le plugin. Cette valeur pe        ut �tre r�gl�e de facon individuelle pour chaque recherche.",
    "",// TODO
    "M��rittelee oletuselini�n t�m�n laajennoksen kautta luotaville ajastimille. Elinaikaa voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify here the default start recording margin of timers/recordings created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Vorlaufzeit f�r Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous d�finissez la marge ant�rieure � la programmation cr�e par le plugin. Cette valeur peut �tre r�gl�e de facon individuelle pour chaque recherche.",
    "",// TODO
    "M��rittelee oletusmarginaalin tallennuksen aloitukselle t�m�n laajennoksen kautta luotaville ajastimille. Marginaaleja voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify here the default stop recording margin of timers/recordings created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Nachlaufzeit f�r Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous d�finissez la marge post�rieure de la programmation cr�e par le plugin. Cette valeur peut �tre r�gl�e de facon individuelle pour chaque recherche.",
    "",// TODO
    "M��rittelee oletusmarginaalin tallennuksen lopetukselle t�m�n laajennoksen kautta luotaville ajastimille. Marginaaleja voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Set this to 'yes' if don't want to see events on PayTV channels when searching for repeats.",
    "Um bei der Suche nach Wiederholungen Sendungen von PayTV-Kan�len zu ignorieren, kann hier die Option auf 'Ja' gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pour �viter les recherches sur les cha�nes payantes, vous mettez l'option sur 'oui'",
    "",// TODO
    "Piilottaa salatut kanavat haettaessa uusintoja.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Here you can setup templates for your searches.",
    "Hier k�nnen Vorlagen f�r Suchen erstellt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous cr�er vos recherches comme mod�les",
    "",// TODO
    "M��rittele mallipohjia hakuehdoille.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Here you can setup blacklists which can be used within a search to exclude events you don't like.",
    "Hiermit k�nnen Ausschlusslisten erstellt werden, die innerhalb einer Suche verwendet werden k�nnen, um Sendungen auszuschlie�en, die man nicht haben will.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec ceci vous changez les listes d'exclusions, qui sont utilis�es par la recherche automatique, pour �viter des emissions que vous ne voulez pas voir.",
    "",// TODO
    "M��rittele mustia listoja hakuehdoille. Mustien listojen tapahtumia ei n�ytet� hakutuloksissa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Here you can setup channel groups which can be used within a search. These are different to VDR channel groups and represent a set of arbitrary channels, e.g. 'FreeTV'.",
    "Hier k�nnen Kanalgruppen erstellt werden, die innerhalb einer Suche verwendet werden k�nnen. Die Kanalgruppen sind nicht mit den VDR-Kanalgruppen zu vergleichen, sondern stellen einfach eine Gruppe von beliebigen Sendern dar, z.B. 'FreeTV'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous d�finissez des groupes de cha�nes qui sont utilis�s par la recherche. Ce ne sont pas les groupes de cha�nes d�finis dans VDR comme 'Cin�ma', mais une s�lection propre de l'utilisateur.",
    "",// TODO
    "M��rittele kanavaryhmi� hakuehdoille. T�m�n laajennoksen kanavaryhm�t eiv�t ole yhteydess� VDR:n omiin kanavaryhmiin.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$If a timer with priority below the given value will fail it will not be classified as important. Only important conflicts will produce an OSD message about the conflict after an automatic conflict check.",
    "Falls ein Timer fehlschlagen wird, dessen Priorit�t unter dem angegeben Wert liegt, wird er als 'nicht wichtig' eingestuft. Nur bei wichtigen Konflikten erfolgt eine Nachricht per OSD bei der automatischen Konfliktpr�fung.",
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsqu'une programmation dont la priorit�e est en dessous de la valeur d�finie, n'aboutit pas, alors elle est qualifi�e de 'non-importante'. Seulement les conflits importants sont affich�s � l'�cran lors de la v�rification automatique.",
    "",// TODO
    "",// TODO
    "Asetusarvoa korkeammalla prioriteetilla olevat ajastimet ovat merkitsevi�. Vain merkitsev�t p��llekk�isyydet aiheuttavat viestin kuvaruutun�yt�lle automaattisen tarkistuksen yhteydess�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$This value reduces the conflict check to the given range of days. All other conflicts are classified as 'not yet important'.",
    "Mit diesem Wert kann der Zeitbereich der Konfliktpr�fung eingestellt werden. Alle Konflikte ausserhalb des Bereichs werden als 'noch nicht wichtig' eingestuft.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec cette valeur vous d�finissez l'interval de temps de la v�rification de conflits. Tous ces conflits en dehors de cet interval sont qualif�s comme 'pas encore importants.",
    "",// TODO
    "Asettaa ajanjakson p�ivin� p��llekk�isyyksien tarkistukselle. Muita p��llekk�isyyksi� ei pidet� viel� merkitsevin�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Set this to 'yes' if the conflict check should be performed after each search timer update.",
    "Hier auf 'Ja' setzen, wenn die Konfliktpr�fung nach jedem Suchtimer-Update erfolgen soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec 'Oui' vous impliquez la v�rification de conflits � la mise � jours de la recherche.",
    "",// TODO
    "Suorittaa p��llekk�isyyksien tarkistuksen jokaisen hakuajastimen p�ivityksen yhteydess�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Specify here the time intervall to be used for an automatic conflict check in the background.\n('0' disables an automatic check)",
    "Hier kann das Zeitintervall angegeben werden, in dem eine automatische Konfliktpr�fung im Hintergrund erfolgen soll.\n(mit '0' wird die automatsiche Pr�fung deaktiviert.)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'interval de temps dans lequel la v�rification de conflits est effectu�e.\n(avec '0' vous d�activez la v�rification.)",
    "",// TODO
    "M��ritt�� aikajakson taustalla teht�v�lle automaattiselle p��llekk�isyyksien tarkistukselle.\nArvolla '0' saat asetettua automaattisen tarkistuksen pois p��lt�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "every ... minutes",
    "alle ... Minuten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "toutes les ... minutes",
    "",// TODO
    "joka ... minuutti",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "if conflicts within next ... minutes",
    "Wenn n�chster Konflikt in ... Minuten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Si conflit dans ... minutes suivantes",
    "",// TODO
    "jos p��llekk�isyys ... min. kuluessa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$If the next conflict will appear in the given number of minutes you can specify here a shorter check intervall to get more OSD notifications about it.",
    "Wenn der n�chste Konflikt innerhalb der angegebenen Anzahl von Minuten liegt, kann man hiermit ein k�rzeres Pr�fintervall angeben, damit man h�ufiger �ber den Konflikt per OSD benachrichtigt wird.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Si le conflit � lieu dans l'interval de temp indiqu�, alors il est possible de r�gler un interval de temps plus petit pour la v�rification de conflits avec affichage � l�cran.",
    "",// TODO
    "Jos seuraava p��llekk�isyys esiintyy asetetun ajan kuluessa, t�ll� asetuksella pystyt m��ritt�m��n lyhyemm�n tarkistusv�lin saadaksesi tarkempia kuvaruutuviestej�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Values for EPG category",
    "Werte f�r EPG-Kategorie",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Valeur pour la cat�gories EPG",
    "",// TODO
    "Valinnat ohjelmaoppaan kategorioille",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$Apply",
    "�bernehmen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Appliquer",
    "",// TODO
    "K�yt�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Ignore conflict duration less ... min.",
    "Ignoriere Konfliktdauer unter ... Min.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ignorer les conflits inf�rieurs � ... min.",
    "",// TODO
    "Sivuuta alle ... min. p��llekk�isyydet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$If a conflicts duration is less then the given number of minutes it will not be classified as important. Only important conflicts will produce an OSD message about the conflict after an automatic conflict check.",
    "Falls die Konfliktdauer unter der angegebenen Anzahl an Minuten liegt, wird er als 'nicht wichtig' eingestuft. Nur bei wichtigen Konflikten erfolgt eine Nachricht per OSD bei der automatischen Konfliktpr�fung.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsque le temps de conflit est sous le nombre de minutes indiqu�, ceci est consid�r� comme 'non-important'. Uniquement les conflits importants sont affich�s � l'�cran lors de la v�rification automatique de la programmation.",
    "",// TODO
    "Jos ajastimien p��llekk�isyys on alle asetetun minuuttim��r�n, sit� ei pidet� merkitsev�n�. Vain merkitsev�t p��llekk�isyydet aiheuttavat viestin kuvaruutun�yt�lle tarkistuksen yhteydess�.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Timer conflict! Show?",
    "Timer-Konflikt! Anzeigen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les conflits de programmation?",
    "",// TODO
    "P��llekk�isi� ajastimia! N�ytet��nk�?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },

  { "After each timer programming",
    "Nach jeder Timer-Programmierung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Apr�s chaque programmation",
    "",// TODO
    "Jokaisen ajastimen luonnin j�lkeen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Set this to 'yes' if the conflict check should be performed after each manual timer programming. In the case of a conflict you get immediately a message that informs you about it. The message is only displayed if this timer is involved in any conflict.",
    "Hier auf 'Ja' setzen, wenn die Konfliktpr�fung nach jeder manuellen Timer-Programmierung erfolgen soll. Im Falle eines Konflikts wird dann sofort eine Nachricht angezeigt. Diese erscheint nur, wenn dieser Timer in einen Konflikt verwickelt ist.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "P��llekk�isten ajastimien tarkistus suoritaan aina jokaisen ajastimen luonnin j�lkeen. Jos luodulle ajastimelle l�ydet��n p��llekk�isyyksi�, niin siit� tiedotetaan k�ytt�j�lle heti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "When a recording starts",
    "Beim Beginn einer Aufnahme",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Tallennuksen alkaessa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Set this to 'yes' if the conflict check should be performed when a recording starts. In the case of a conflict you get immediately a message that informs you about it. The message is only displayed if the conflict is within the next 2 hours.",
    "Hier auf 'Ja' setzen, wenn die Konfliktpr�fung beim Beginn jeder Aufnahme erfolgen soll. Im Falle eines Konflikts wird dann sofort eine Nachricht angezeigt. Diese erscheint nur, wenn der Konflikt innerhalb der n�chsten 2 Stunden auftritt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "P��llekk�isten ajastimien tarkistus suoritaan aina tallennuksen alkaessa. Jos luodulle ajastimelle l�ydet��n p��llekk�isyyksi� seuraavan kahden tunnin aikana, niin siit� tiedotetaan k�ytt�j�lle heti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Favorites",
    "Favoriten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Favoris",
    "",// TODO
    "Suosikit",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Button$Favorites",
    "Favoriten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Favoris",
    "",// TODO
    "Suosikit",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Use in favorites menu",
    "In Favoritenmen� verw.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser dans le menu favoris",
    "",// TODO
    "K�yt� suosikkina",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Show favorites menu",
    "Zeige Favoriten-Men�",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher le menu favoris",
    "",// TODO
    "N�yt� suosikkivalikko",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "before user-def. times",
    "vor ben.-def. Zeiten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "avant le temp utilisateur",
    "",// TODO
    "ennen ajankohtia",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "after user-def. times",
    "nach ben.-def. Zeiten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "apr�s le temp utilisateur",
    "",// TODO
    "ajankohtien j�lkeen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },

  { "Help$A favorites menu can display a list of your favorite broadcasts. Enable this if you want an additional menu besides 'Now' and 'Next'\nAny search can be used as a favorite. You only have to set the option 'Use in favorites menu' when editing a search.",
    "Ein Favoriten-Men� kann eine Liste der Lieblingssendungen anzeigen. Hier ausw�hlen, wenn neben 'Jetzt' und 'N�chste' ein solches Men� gew�nscht wird.\nJede Suche kann als Favorit verwendet werden. Dazu einfach die Option 'In Favoritenmen� verw.' beim Editieren einer Suche setzen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Un menu favori peut contenir la liste de vos �mission pr�f�r�e. Faire le choix ici, si vous voulez avoir un menu favori � cot� de 'Maintenant' et 'plus tard.\nChaque recherche peut �tre utilis� comme favori. Pour cela simplement utiliser l'option Favori lors de l'�dition de recherche.",
    "",// TODO
    "Suosikkivalikossa listataan suosikiksi merkityt ohjelmat. Jos t�m� valinta on aktiivinen, suosikkivalikko l�ytyy 'Nyt'- ja 'Seuraavaksi'-valikkojen rinnalta. \nMik� tahansa haku voidaan merkit� suosikiksi aktivoimalla 'K�yt� suosikkina'-valinta.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Ok key",
    "Taste Ok",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Touche Ok",
    "",// TODO
    "'OK' n�pp�in",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Help$Choose here the behaviour of key 'Ok'. You can use it to display the summary or to switch to the corresponding channel.\nNote: the functionality of key 'blue' (Switch/Info/Search) depends on this setting.",
    "Hier kann das Verhalten der 'Ok'-Taste bestimmt werden. Man kann damit die Inhaltsangabe anzeigen oder zum entsprechenden Sender wechseln.\nHinweis: Die Funktion der Taste 'Blau' (Umschalten/Info/Suche) h�ngt von dieser Einstellung ab.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez influenc� le comportement de la touch 'Ok'. On peut afficher le continue d'information ou changer pour la cha�nes propos�e.\nIndice: La fonction de la touch 'bleu' (changer/Infos/Recherche) d�pend de ce r�glage.",  // Traduction de Leslie et Patrice Staudt 01.02.2007
    "",// TODO
    "Valitse toiminto 'OK' n�pp�imelle: n�yt� ohjelman tiedot tai vaihda kyseiselle kanavalle.\nHuomioi: sinisen n�pp�imen toiminto (Vaihda/Tiedot/Hae) riippuu t�st� asetuksesta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Switch",
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
    "",//TODO
    "",//TODO
    "����",
    "Info",
    "Info",
    "Info",
    "Info",
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
    "",//TODO
    "",//TODO
    "����",
    "Info",
    "Info",
    "Info",
    "Info",
  },
  { "Search timers activated in setup.",
    "Suchtimer wurden im Setup aktiviert.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La recherche a �t� activ�e dans la configuration.", // Traduction de Leslie et Patrice Staudt 18.10.2006
    "",// TODO
    "Hakuajastimet aktivoitu asetuksista",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Result menu layout",
    "Layout des Ergebnismen�s",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Dispositon du menu de r�sultat",
    "",// TODO
    "Hakutulosten ulkoasu",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO �esky (Czech)
  },
  { "Email notification",
    "Email-Benachrichtigung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Notification par mail",
    "",// TODO
    "Ilmoitukset s�hk�postilla",
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
    "",// TODO
  },
  { "Search timer notification",
    "Suchtimer-Benachrichtigung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Notification de recherche",
    "",// TODO
    "Ilmoitukset hakuajastimista",
    "/oui",// TODO
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
  { "Help$Set this to 'yes' if you want to get an email notification about the search timers that where programmed automatically in the background.",
    "Hier mit 'Ja' ausw�hlen, ob eine Email-Benachrichtigung �ber automatisch im Hintergrund programmierte Suchtimer versandt werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui',si vous voulez que les nouvelles programmations automatiques soient notifi�es par mail.",
    "",// TODO
    "T�ll� asetuksella saat ilmoitukset automaattisesti lis�tyist� hakuajastimista s�hk�postiisi.",
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
    "",// TODO
  },
  { "Timer conflict notification",
    "Timer-Konflikt-Benachrichtigung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Notification de conflit de programmation",
    "",// TODO
    "Ilmoitukset p��llekk�isist� ajastimista",
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
    "",// TODO
  },
  { "Help$Set this to 'yes' if you want to get an email notification about the timer conflicts.",
    "Hier mit 'Ja' ausw�hlen, ob eine Email-Benachrichtigung �ber Timer-Konflikte vers�ndet werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui',si les conflits de programmation doivent �tre notifi�s.",
    "",// TODO
    "T�ll� asetuksella saat ilmoitukset p��llekk�isist� ajastimista s�hk�postiisi.",
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
    "",// TODO
  },
  { "--- Email account ---",
    "--- Email-Konto ---",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "--- Compte mail ---",
    "",// TODO
    "--- S�hk�posti ---",
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
    "",// TODO
  },
  { "Send to",
    "Senden an",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vastaanottaja",
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
    "",// TODO
  },
  { "Help$Specify the email adress where notifications should be sent to.",
    "Hier wird die Email-Adresse angegeben, an welche die Benachrichtigungen versandt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'adresse mail, a laquelle les notifications sont envoy�es.",
    "",// TODO
    "M��rittelee s�hk�postiosoitteen, jonne ilmoitukset l�hetet��n.",
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
    "",// TODO
  },
  { "Mail method",
    "Mail-Methode",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "L�hetystapa",
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
    "",// TODO
  },
  { "Help$Specify here the method to use when sending mails.\nYou can choose between\n - 'sendmail': requires a properly configured email system\n - 'SendEmail.pl': simple script for mail delivery",
    "Bitte hier die gew�nschte Methode zum Emailversand ausw�hlen. Verf�gbar sind:\n - 'sendmail': erfordert ein korrekt konfiguriertes Mailsystem\n - 'SendEmail.pl': ein einfaches Skript zum Mailversand",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "M��rittelee l�hetystavan s�hk�posteille.\nVoit valita kahden vaihtoehdon v�lilt�:\n - 'sendmail': vaatii oikein konfiguroidun s�hk�postisysteemin\n - 'SendEmail.pl': yksinkertainen skriptitiedosto s�hk�postin l�hetykseen",
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
    "",// TODO
  },
  { "Email adress",
    "Email-Adresse",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Adresse Mail",
    "",// TODO
    "S�hk�postiosoite",
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
    "",// TODO
  },
  { "Help$Specify the email adress where notifications should be sent from.",
    "Hier wird die Email-Adresse angegeben, mit der die Benachrichtigungen versandt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "M��rittelee s�hk�postiosoitteen, josta ilmoitukset l�hetet��n.",
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
    "",// TODO
  },
  { "SMTP server",
    "SMTP Server",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Serveur SMTP",
    "",// TODO
    "SMTP-palvelin",
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
    "",// TODO
  },
  { "Help$Specify the SMTP server that should deliver the notifications. If it's using a port different from the default(25) append the port with \":port\".",
    "Hier wird der SMTP Server hinterlegt, �ber den die Benachrichtigungen versandt werden. Falls dieser einen anderen Port als den Standard(25) verwendet, dann mit \":port\" anh�ngen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez le serveur de mail sortant SMTP, par lequel les notifications vont �tre envoy�es. Si le port standart(25) n'est pas utilis�, alors ajoutez \":port\" par exemple smtp.orange.fr:2525. ",
    "",// TODO
    "M��rittelee k�ytett�v�n SMTP-palvelimen. Portti voidaan m��ritell� erikseen lis��m�ll� palvelimen osoitteen loppuun \":portti\", jos se eroaa oletuksesta (25).",
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
    "",// TODO
  },
  { "Use SMTP authentication",
    "Verw. SMTP-Authentifizierung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser l'authentification SMTP",
    "",// TODO
    "K�yt� SMTP-autentikointia",
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
    "",// TODO
  },
  { "Help$Set this to 'yes' if your account needs authentication to send mails.",
    "Hier auf 'Ja' setzen, wenn das Emailkonto SMTP-Authentifizierung zum Emailversand ben�tigt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui',si le compte mail utilise l'authentification pour les mails sortants.",
    "",// TODO
    "Asettaa SMTP-autentikoinnin p��lle s�hk�postin l�hetyst� varten.",
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
    "",// TODO
  },
  { "Auth user",
    "Auth-Benutzer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Authentification: utilisateur",
    "",// TODO
    "SMTP-k�ytt�j�tunnus",
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
    "",// TODO
  },
  { "Help$Specify the auth user, if this account needs authentication for SMTP.",
    "Hier wird der Auth-Benutzername angegeben, falls dieses Emailkonto eine Authentifizierung f�r SMTP ben�tigt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'utilisateur pour l'authentification, lorsque le compte mail sortant SMTP en a besoin.",
    "",// TODO
    "M��rittelee k�ytt�j�tunnuksen, jos SMTP-palvelimelle pit�� autentikoida.",
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
    "",// TODO
  },
  { "Auth password",
    "Auth-Passwort",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Authentification: mot de passe",
    "",// TODO
    "SMTP-salasana",
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
    "",// TODO
  },
  { "Help$Specify the auth password, if this account needs authentication for SMTP.",
    "Hier wird das Auth-Passwort angegeben, falls dieses Emailkonto eine Authentifizierung f�r SMTP ben�tigt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez le mot de passe pour l'authentification, lorsque le compte mail sortant SMTP en a besoin.",
    "",// TODO
    "M��rittelee salasanan, jos SMTP-palvelimelle pit�� autentikoida.",
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
    "",// TODO
  },
  { "Button$Test",
    "Test",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Test",
    "",// TODO
    "Testaa",
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
    "",// TODO
  },
  { "Mail account check failed!",
    "Mailkonto-Pr�fung fehlgeschlagen!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La v�rification du compte mail a �chou�",
    "",// TODO
    "S�hk�postilaatikon tarkistus ep�onnistui!",
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
    "",// TODO
  },
  { "Notification about new timers",
    "Benachrichtigung �ber neue Timer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Notification pour de nouvelles programmations",
    "",// TODO
    "Ilmoita uusista ajastimista",
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
    "",// TODO
  },
  { "No new timers were added.",
    "Es wurden keine neuen Timer angelegt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Il n'y a pas eu de nouvelle programmation.",
    "",// TODO
    "Yht��n uutta ajastinta ei lis�tty.",
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
    "",// TODO
  },
  { "No timers were modified.",
    "Es wurden keine Timer ge�ndert.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La programmation n'a pas �t� chang�e.",
    "",// TODO
    "Yht��n ajastinta ei muokattu.",
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
    "",// TODO
  },
  { "No timers were deleted.",
    "Es wurden keine Timer gel�scht.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aucune programmation n'a �t� effac�e.",
    "",// TODO
    "Yht��n ajastinta ei poistettu.",
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
    "",// TODO
  },
  { "for the next ... hours",
    "f�r die n�chsten ... Stunden",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pour les prochaines ... heures",
    "",// TODO
    "seuraaville ... tunnille",
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
    "",// TODO
  },
  { "Help$This value controls the timespan used to display your favorites.",
    "Mit diesem Wert wird die Zeitspanne eingestellt, f�r die Favoriten angezeigt werden sollen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec cette valeur vous r�glez un interval, pour afficher les favoris.",
    "",// TODO
    "T�m� arvo m��rittelee k�ytetyn ajanjakson suosikkien n�ytt�miselle.",
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
    "",// TODO
  },
  { "in %02ldm", // in x minutes
    "in %02ldm",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "en %02ldm",
    "",// TODO
    "%02ldm",
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
    "",// TODO
  },
  { "in %02ldh", // in x hours
    "in %02ldh",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "en %02ldh",
    "",// TODO
    "%02ldh",
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
    "",// TODO
  },
  { "in %02ldd", // in x days
    "in %02ldd",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "en %02ldd",
    "",// TODO
    "%02ldd",
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
    "",// TODO
  },
  { "Timer check",
    "�berwachung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "surveiller",
    "",// TODO
    "Valvontatapa",
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
    "",// TODO
  },
  { "no check",
    "ohne �berwachung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "sans surveillance",
    "",// TODO
    "ei valvontaa",
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
    "",// TODO
  },
  { "by event ID",
    "anhand Sendungskennung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "� partir du num�ro d'�mission",
    "",// TODO
    "tapahtuman tunniste",
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
    "",// TODO
  },
  { "by channel and time",
    "anhand Sender/Uhrzeit",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "� partir cha�nes/heures",
    "",// TODO
    "kanava ja aika",
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
    "",// TODO
  },
  { "Default timer check method",
    "Standard-Timer-Pr�fmethode",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "M�thode standart de v�rification de programmation",
    "",// TODO
    "Oletustapa ajastimen valvonnalle",
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
    "",// TODO
  },
  { "Help$Manual timers can be checked for EPG changes. Here you can setup the default check method for each channel. Choose between\n\n- no checking\n- by event ID: checks by an event ID supplied by the channel provider.\n- by channel and time: check by the duration match.",
    "Manuelle Timer k�nnen hinsichtlich EPG-�nderungen �berwacht werden. Hier kann die Standard-Pr�fmethode f�r jeden Kanal hinterlegt werden. Zur Wahl stehen:\n\n- keine Pr�fung\n- anhand Sendungskennung: gepr�ft wird anhand einer Kennung, die durch den Sender vergeben wird.\n- anhand Sender/Uhrzeit: gepr�ft wird anhand der Sendung, die am besten zur Dauer der urspr�nglichen Sendung passt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Des programmations manuelles peuvent �tre surveill�s par rapport au changement d'EPG. Ici vous pouvez d�finir une m�thode de v�rification standart pour chaque cha�ne. Les choix:\n\n- pas de v�rification\n- � partir du num�ro d'�mission: la v�rification se fait � partir d'un num�ro d�livr� par l'�metteur.\n- � partir de la cha�ne/heure: v�rification � partir de l'�mission, qui est la mieux adapt�e avec la dur�e de l'�mission initiale.",
    "",// TODO
    "K�sinluotuja ajastimia voidaan valvoa ohjelmaoppaan muutoksia vasten. Voit asettaa oletustavan jokaista kanavaa kohden:\n\n- ei valvontaa\n- tapahtuman tunniste: ohjelmisto-oppaassa l�hetett�v�n tunnisteen mukaan\n- kanava ja aika: tapahtuman keston ja ajankohdan mukaan",
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
    "",// TODO
  },
  { "Show timers created",
    "Zeige erstellte Timer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher la programmation effectu�e",
    "",// TODO
    "N�yt� luodut ajastimet",
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
    "",// TODO
  },
  { "No announcements when replaying",
    "Keine Ank�ndigungen bei Wiedergabe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pas d'annonces lors de la lecture",
    "",// TODO
    "�l� muistuta toiston aikana",
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
    "",// TODO
  },
  { "Help$Set this to 'yes' if you don't like to get any announcements of broadcasts if you currently replay anything.",
    "Bitte auf 'Ja' setzen, wenn w�hrend einer Wiedergabe keine Ank�ndigungen �ber Sendungen erw�nscht sind.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui' si aucune annonce d'�mission n'est voulue pendant la lecture.",
    "",// TODO
    "Laittamalla t�m�n asetuksen p��lle et saa ohjelmista muistutuksia toistettaessa tallenteita.",
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
    "",// TODO
  },
  { "Quick search",
    "Schnellsuche",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Recherche rapide",
    "",// TODO
    "Pikahaku",
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
    "",// TODO
  },
  { "Button$Extended",
    "Erweitert",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Extention",
    "",// TODO
    "Laaja",
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
    "",// TODO
  },
  { "Button$Simple",
    "Einfach",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Simple",
    "",// TODO
    "Suppea",
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
    "",// TODO
  },
  { "Recreate timers after deletion",
    "Timer nach L�schen neuprogrammieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Recrer la programmation apr�s l'�ffacement",
    "",// TODO
    "Luo ajastimet uudelleen",
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
    "",// TODO
  },
  { "Help$Set this to 'yes' if you want timers to be recreated with the next search timer update after deleting them.",
    "Hier 'Ja' w�hlen, wenn gel�schte Timer mit dem n�chsten Suchtimer-Update neu programmiert werden sollen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Choisissez 'oui' pour refaire les programmations de recherche lorsque vous les avez effac�s.",
    "",// TODO
    "Laittamalla t�m�n asetuksen p��lle saat luotua ajastimet uudelleen seuraava hakuajastin p�ivityksen yhteydess�, jos olet poistanut ne.",
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
    "",// TODO
  },
  { "Limit channels from 1 to", 
    "Kan�le begrenzen von 1 bis",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Limiter aux cha�nes de 1 �",
    "",// TODO
    "N�ytett�vien kanavien rajoitus",
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
    "",// TODO
  },
  { "Help$If you have a large channel set you can speed up things when you limit the displayed channels with this setting. Use '0' to disable the limit.",
    "Bei einer sehr gro�en Kanalliste l��t sich der Men�-Aufbau mit dieser Einstellung durch eine Einschr�nkung der angezeigten Kan�le beschleunigen. Mit '0' wird das Limit aufgehoben.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec cette limite vous rendez l'affichage du menu plus rapide si vous avez une liste de cha�nes tr�s longue. Avec '0' vous enlevez la limitation.",
    "",// TODO
    "Voit nopeuttaa toimintoja rajoittamalla n�ytett�vien kanavien lukum��r�� t�ll� asetuksella. Poistat rajoituksen asettamalla arvoksi nollan.",
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
    "",// TODO
  },
  { "Avoid notification when replaying",
    "Vermeide Nachricht bei Wiedergabe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Eviter les messages pendant la lecture",// Francais Patrice Staudt 29.03.2007
    "",// TODO
    "�l� n�yt� ilmoituksia toiston aikana",
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
    "",// TODO
  },
  { "Help$Set this to 'yes' if the don't want to get OSD messages about conflicts if you currently replay something. Nevertheless messages will be displayed if the first upcoming conflict is within the next 2 hours.",
    "Bitte auf 'Ja' setzen, wenn w�hrend einer Wiedergabe keine OSD-Benachrichtigungen �ber Timer-Konflikte gew�nscht sind. Die Benachrichtigung erfolgt trotzdem, wenn der n�chste Konflikt innerhalb der n�chsten 2 Stunden auftritt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez sur 'oui', si pendant la lecture vous ne voulez pas �tre d�rang� par les affichages des conflits de programmation. L'affichage est fait si les conflits on lieu dans le 2 heures � venir.",
    "",// TODO
    "Laittamalla t�m�n asetuksen p��lle saat estetty� kuvaruutuviestit p��llekk�isist� ajastimista toiston aikana. Kuvaruutuviesti n�ytet��n kuitenkin aina, jos p��llekk�inen ajastin on alle kahden tunnin kuluttua.",
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
    "",// TODO
  },
  { "%d new broadcast(s) found! Show them?",
    "%d neue Sendung(en) gefunden! Anzeigen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les %d nouvelles �missions trouv�es?",
    "",// TODO
    "L�ydettiin %d uutta l�hetyst�! N�ytet��nk�?",
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
    "",// TODO
  },
  { "%d new broadcast",
    "%d neue Sendung(en)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "%d nouvelle(s) �mission(s)",
    "",// TODO
    "%d uutta l�hetyst�",
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
    "",// TODO
  },
  { "announce again",
    "Erneut ank�ndigen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Annoncer � nouveau",
    "",// TODO
    "Muistuta uudelleen",
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
    "",// TODO
  },
  { "with next update",
    "beim n�chsten Update",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "lors de la prochaine mise � jour",
    "",// TODO
    "Seuraavassa p�ivityksess�",
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
    "",// TODO
  },
  { "again from",
    "erneut ab",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "recommencer � partir de",
    "",// TODO
    "Alkaen taas",
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
    "",// TODO
  },
  { "announce details: %s",
    "Ank�ndigungsdetails: %s",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Muistutuksen tiedot: %s",
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
    "",// TODO
  },
/*  { "No results! Try again with tolerance %d?",
    "Keine Ergebnisse! Nochmals mit Toleranz %d suchen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pas de r�sultats! Recherche � nouveau avec %d de tol�rence?",
    "",// TODO
    "Ei hakutuloksia! Yritet��nk� uudelleen toleranssilla %d?",
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
    "",// TODO
    },*/
{ NULL }
  };

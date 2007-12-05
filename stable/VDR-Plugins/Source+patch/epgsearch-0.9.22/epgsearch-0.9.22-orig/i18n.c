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
    "Cerca nella guida EPG: per parole, repliche e più",// Italiano Sean Carlos 2005.01.16
    "zoek in het EPG naar herhalingen en meer",
    "",// TODO Portugues
    "Recherche de répétition dans EPG",// Francais Patrice Staudt 4.1.2005
    "",// TODO Norsk
    "Monipuolinen ohjelmaopas hakutoiminnolla",
    "",// TODO Polski
    "",// TODO Español
    "",// TODO ÅëëçíéêÜ (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
    "",// TODO ÀãááÚØÙ (Russian)
    "",// TODO Hrvatski (Croatian)
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "This plugin has no info text!",
    "Dieses Plugin hat keinen Infotext!",
    "",// TODO
    "Questo plugin non ha ulteriori informazioni!",// TODO
    "Deze plugin heeft geen info tekst",
    "",// TODO
    "Le plugin n'a pas de text d'info",
    "",// TODO
    "Laajennoksella ei ole infotekstiä!",
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
    "",// TODO Èesky (Czech)
  },
  { "Program guide",
    "Programmführer",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Button$by time",
    "nach Beginn",
    "",// TODO
    "per orario",// Italiano 
    "bij tijd",
    "",// TODO
    "de début",
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
    "",// TODO Èesky (Czech)
  },
  { "additional starting times",
    "zusätzliche Startzeiten",
    "",// TODO
    "altri orari di inizio",// Italiano
    "toegevoegde start tijden",
    "",// TODO
    "temps de débuts supplémentaires",
    "",// TODO
    "Löydetyt aloitusajat",
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
    "",// TODO Èesky (Czech)
  },
  { "Repeats",
    "Wiederholung",
    "",// TODO
    "Repliche",// Italiano
    "Herhalingen",
    "",// TODO
    "Répétitions",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)

  },
  { "Search mode",
    "Suchmodus",
    "",// TODO
    "Modalità di ricerca",// Italiano
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Match case",
    "Groß/klein",
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
    "",// TODO Èesky (Czech)

  },
  { "Use title",
    "Verw. Titel",
    "",// TODO
    "Utilizzare titolo",// Italiano
    "gebruik titel",
    "",// TODO
    "Utiliser titre",
    "",// TODO
    "Käytä ohjelmanimeä",
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
    "",// TODO Èesky (Czech)

  },
  { "Use subtitle",
    "Verw. Untertitel",
    "",// TODO
    "Utilizzare sottotitolo",// Italiano
    "Gebruik ondertitel",
    "",// TODO
    "Utiliser sous-titre",
    "",// TODO
    "Käytä lyhyttä ohjelmakuvausta",
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
    "",// TODO Èesky (Czech)

  },

  { "Use time",
    "Verw. Uhrzeit",
    "",// TODO
    "Utilizzare l'orario",// Italiano
    "Gebruik tijd",
    "",// TODO
    "Utiliser l'heure",
    "",// TODO
    "Käytä aloitusaikaa",
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
    "",// TODO Èesky (Czech)

  },
  { "Use description",
    "Verw. Beschreibung",
    "",// TODO
    "Utilizzare descrizione",// Italiano
    "Gebruik beschrijving",
    "",// TODO
    "Utiliser la description",
    "",// TODO
    "Käytä ohjelmakuvausta",
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
    "",// TODO Èesky (Czech)

  },
  { "Use channel",
    "Verw. Kanal",
    "",// TODO
    "Utilizzare canale",// Italiano
    "Gebruik kanaal",
    "",// TODO
    "Utiliser le canal",
    "",// TODO
    "Käytä kanavaa",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "  Start after",
    "  Start nach",
    "",// TODO
    "  Comincia dopo",// Italiano
    "  Start na",
    "",// TODO
    "  Départ après",
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
    "",// TODO Èesky (Czech)

  },
  { "  Start before",
    "  Start vor",
    "",// TODO
    "  Comincia prima",// Italiano
    "  Start voor",
    "",// TODO
    "  Départ avant",
    "",// TODO
    "  Aloitusaika viimeistään",
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
    "",// TODO Èesky (Czech)

  },
  { "Search results",
    "Suchergebnisse",
    "",// TODO
    "Risultati della ricerca",// Italiano
    "Zoek resultaten",
    "",// TODO
    "Résultat de recherche",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Hide main menu entry",
    "Hauptmenüeintrag verstecken",
    "",// TODO
    "Nascondi voce nel menù principale",// Italiano
    "Verberg hoofd menu ingave",
    "",// TODO
    "Invisible dans le menu principal",
    "",// TODO
    "Piilota valinta päävalikosta",
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
    "",// TODO Èesky (Czech)

  },
  { "Replace original schedule",
    "Originale Programmübersicht ersetzen",
    "",// TODO
    "Sostituisci scheda di programmazione originale",// Italiano
    "Originele inhoud terugzetten",
    "",// TODO
    "",
    "",// TODO
    "Korvaa alkuperäinen ohjelmaopas",
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
    "",// TODO Èesky (Czech)

  },
  { "Edit$Delete search?",
    "Suche löschen?",
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
    "",// TODO Èesky (Czech)

  },
  { "Edit$Search text too short - use anyway?",
    "Suchtext zu kurz - trotzdem verwenden?",
    "",// TODO
    "Il testo da cercare è troppo corto.  Continuare lo stesso?",// Italiano
    "Zoek tekst tekort - toch gebruiken?",
    "",// TODO
    "Texte de recherche est trop court - l'utiliser comme même?",
    "",// TODO
    "Liian suppea hakuehto - etsitäänkö silti?",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Red key",
    "Taste Rot",
    "",// TODO
    "Tasto rosso",// Italiano
    "Rode toets",
    "",// TODO
    "Touche rouge",
    "",// TODO
    "Punainen näppäin",
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
    "",// TODO Èesky (Czech)

  },
  { "Blue key",
    "Taste Blau",
    "",// TODO
    "Tasto blu",// Italiano
    "Blauwe toets",
    "",// TODO
    "Touche bleu",
    "",// TODO
    "Sininen näppäin",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Use duration",
    "Verw. Dauer",
    "",// TODO
    "Utilizzare durata",// Italiano
    "Gebruiks duur",
    "",// TODO
    "Durée d'utilisation",
    "",// TODO
    "Käytä kestoaikaa",
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
    "",// TODO Èesky (Czech)

  },
  { "  Max. duration",
    "  Max. Dauer",
    "",// TODO
    "  Durata Massima",// Italiano
    "  Max. duur",
    "",// TODO
    "  Durée max.",
    "",// TODO
    "  Kestoaika enintään",
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
    "",// TODO Èesky (Czech)

  },
  { "  Min. duration",
    "  Min. Dauer",
    "",// TODO
    "  Durata Minima",// Italiano
    "  Min. duur",
    "",// TODO
    "  Durée min.",
    "",// TODO
    "  Kestoaika vähintään",
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
    "",// TODO Èesky (Czech)

  },
  { "Overview",
    "Übersicht",
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
    "",// TODO Èesky (Czech)

  },
  { "Use user-defined time",
    "Verw. benutzerdef. Zeit",
    "",// TODO
    "Utilizzare ora utente",// Italiano
    "Gebruik gebruiker's tijd",
    "",// TODO
    "Utiliser le temps de l'utilisateur",
    "",// TODO
    "Määrittele ajankohta",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Show progress in 'Now'",
    "Zeige Fortschritt in 'Jetzt'",
    "",// TODO
    "Visualizza progresso in 'Adesso'",// Italiano
    "Toon voortgang in 'Nu'",
    "",// TODO
    "Afficher le progrès dans 'Maintenant'",
    "",// TODO
    "Näytä aikajana 'Nyt'-sivulla",
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
    "",// TODO Èesky (Czech)

  },
  { "Show progress in 'Next'",
    "Zeige Fortschritt in 'Nächste'",
    "",// TODO
    "Visualizza progresso in 'Prossimo'",
    "Toon voortgang in 'Volgende'",
    "",// TODO
    "Afficher le progrès dans 'après'",
    "",// TODO
    "Näytä aikajana 'Seuraavaksi'-sivulla",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },

  { "Show channel numbers",
    "Zeige Kanalnummern",
    "",// TODO
    "Visualizzare i numeri dei canali",// Italiano
    "Toon kanaal nummers",
    "",// TODO
    "Afficher le numéro de la chaîne",
    "",// TODO
    "Näytä kanavien numerointi",
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
    "",// TODO Èesky (Czech)

  },
  { "Use search timers",
    "Verwende Suchtimer",
    "",// TODO
    "Abilitare creazione del timer di ricerca",// TODO
    "Gebruik zoek timer's",
    "",// TODO
    "Utiliser le programmeur de recherche",
    "",// TODO
    "Käytä hakuajastimia",
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
    "",// TODO Èesky (Czech)

  },
  { "  Update interval [min]",
    "  Aktualisierungsintervall [min]",
    "",// TODO
    "  Intervallo d'aggiornamento",// Italiano
    "  Update interval [min]",
    "",// TODO
    "  Interval d'actualisation [min]",
    "",// TODO
    "  Päivitysväli [min]",
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
    "",// TODO Èesky (Czech)

  },
  { "Use day of week",
    "Verw. Wochentag",
    "",// TODO
    "Utilizzare giorno della settimana",// Italiano
    "Gebruik dag van de week",
    "",// TODO
    "Utiliser les jours de la semaine",
    "",// TODO
    "Käytä viikonpäivää",
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
    "",// TODO Èesky (Czech)

  },
  { "  Day of week",
    "  Wochentag",
    "",// TODO
    "  Giorno della settimana",// Italiano
    "  Dag van de week",
    "",// TODO
    "  Jours de la semaine",
    "",// TODO
    "  Viikonpäivä",
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
    "",// TODO Èesky (Czech)

  },
  { "Use as search timer",
    "Als Suchtimer verwenden",
    "",// TODO
    "Utilizzare come timer di ricerca",// Italiano
    "Gebruik als zoek timer",
    "",// TODO
    "Utiliser la recherche",
    "",// TODO
    "Käytä hakuajastimena",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Edit user-defined days of week",
    "Benutzerdefinierte Wochentage editieren",
    "",// TODO
    "Modificare giorni della settimana definiti dall'utente",// TODO
    "Bewerk door gebruiker gedefinieerde dagen van de week",
    "",// TODO
    "Edition des journées définit par l'utilisateur",
    "",// TODO
    "Muokkaa valittuja viikonpäiviä",
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
    "",// TODO Èesky (Czech)

  },
  { "Create search",
    "Suche anlegen",
    "",// TODO
    "Crea ricerca",// TODO
    "Creeer zoeken",
    "",// TODO
    "Créer une recherche",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "found recordings",
    "gefundene Aufnahmen",
    "",// TODO
    "registrazioni trovate",// Italiano 
    "gevonden opname's",
    "",// TODO
    "Enregistrements trouvées",
    "",// TODO
    "löydetyt tallenteet",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Toggle Green/Yellow",
    "Grün/Gelb umschalten",
    "",// TODO
    "Alternare i tasti Verde/Giallo",// Italiano
    "Groen/Geel verwisselen",
    "",// TODO
    "Commuter vert/jaune",
    "",// TODO
    "Käytä aikasiirtymää värinäppäimillä",
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
    "",// TODO Èesky (Czech)

  },
  { "Time interval for FRew/FFwd [min]",
    "Zeitintervall für FRew/FFwd [min]",
    "",// TODO
    "Intervallo tempo indietro veloce/avanti veloce [min]",// Italiano
    "Tijd interval voor FRew/FFwd [min]",
    "",// TODO
    "Interval pour FRew/FFwd [min]",
    "",// TODO
    "Oletusaikasiirtymä [min]",
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
    "",// TODO Èesky (Czech)

  },
  { "Use short channel names",
    "Kurze Sendernamen",
    "",// TODO
    "Utilizzare i nomi corti dei canali",// Italiano
    "Gebruik korte kanaal namen",
    "",// TODO
    "Nom de Chaîne court",
    "",// TODO
    "Käytä lyhyitä kanavien nimiä",
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
    "",// TODO Èesky (Czech)

  },
  { "  from channel",
    "  von Kanal",
    "",// TODO
    "  da canale",// Italiano
    "  van kanaal",
    "",// TODO
    "  de la Chaîne",
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
    "",// TODO Èesky (Czech)

  },
  { "  to channel",
    "  bis Kanal",
    "",// TODO
    "  a canale",// Italiano
    "  tot kanaal",
    "",// TODO
    "  à la Chaîne",
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
    "",// TODO Èesky (Czech)

  },
  { "Start menu",
    "Starte mit",
    "",// TODO
    "Menu d'avvio",// TODO
    "Start menu",
    "",// TODO
    "Menue de départ",
    "",// TODO
    "Oletusnäyttö",
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
    "",// TODO Èesky (Czech)

  },
  { "Please check channel criteria!",
    "Bitte Senderkriterien prüfen!",
    "",// TODO
    "Prego controllare criteri del canale!",
    "AUB kanaal criteria nakijken",
    "",// TODO
    "Vérifier le critère de la chaîne!",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Button$Episode",
    "Episode",
    "",// TODO
    "Episodio",// Italiano
    "Episode",
    "",// TODO
    "Épisode",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "%d timer conflict(s)! First at %s. Show them?",
    "%d Timer-Konflikt(e)! Erster am %s. Anzeigen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "%d de conflits de programmation! Le premier le %s. Afficher?",
    "",// TODO
    "Päällekkäisiä ajastimia (%d kpl %s)! Näytetäänkö?",
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
    "",// TODO Èesky (Czech)

  },
  { "Announce only",
    "Nur ankündigen",
    "",// TODO
    "Solo annuncio (niente timer)",// Italiano
    "Alleen aankondigen (geen timer)",
    "",// TODO
    "Annoncer seulement début d'une programme",
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
    "",// TODO Èesky (Czech)

  },
  { "match exactly",
    "exakt",
    "",// TODO
    "esatta corrispondenza",// Italiano
    "precies passend",
    "",// TODO
    "correspond exactement",
    "",// TODO
    "täsmällinen",
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
    "",// TODO Èesky (Czech)

  },
  { "regular expression",
    "regulärer Ausdruck",
    "",// TODO
    "espressione regolare",// Italiano
    "reguliere uitdrukking",
    "",// TODO
    "expression réguliere",
    "",// TODO
    "säännöllinen lauseke",
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
    "",// TODO Èesky (Czech)

  },

  // The allowed characters in strings:
  { "$ abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aäbcdefghijklmnoöpqrsßtuüvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aàbcdeéèfghiîjklmnopqrstuùvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aàbcçdeéèêfghiîjklmnoôpqrstuùûvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyzåäö0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aábcdeéfghiíjklmnñoópqrstuúvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " áÜâãäåÝæçÞèéßêëìíîïüðñóòôõýö÷øùþ0123456789-.#~abcdefghijklmnopqrstuvwxyz\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyzåäö0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aãâbcdefghiîjklmnopqrsºtþuvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aábcdeéfghiíjklmnoóöpqrstuúüvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " aàbcçdeéèfghiíjklmnoòpqrstuúvwxyz0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyzÐÑÒÓÔÕñÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìîï0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcèædðefghijklmnopqrs¹tuvwxyz¾0123456789-.#~\\^$[]|()*+?{}/:%@&", // hrv
    " abcdefghijklmnopqrsðzþtuvwõäöüxyå0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyzæøå0123456789-.#~\\^$[]|()*+?{}/:%@&",
    " abcdefghijklmnopqrstuvwxyzæøå0123456789-.#~\\^$[]|()*+?{}/:%@&",
  },
  { "Ignore PayTV channels",
    "PayTV-Sender ignorieren",
    "",// TODO
    "Ignora i canali PayTV",// Italiano
    "Ignoriere PayTV kanalen",
    "",// TODO
    "Ignorer les chaînes payantes",// Francais Patrice Staudt 1.12.2005
    "",// TODO
    "Jätä salatut kanavat huomioimatta",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Select directory",
    "Verzeichnis wählen",
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
    "",// TODO Èesky (Czech)

  },
  { "Use extended EPG info",
    "Verw. erweiterte EPG Info",
    "",// TODO
    "Utilizzare informazioni EPG estesa",// Italiano
    "Gebruik uitgebreide EPG info",
    "",// TODO
    "Utiliser les infos EPG avancées",// Francais Pat
    "",// TODO
    "Käytä laajennettua ohjelmaopasta",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Button$Reset",
    "Zurücksetzen",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "interval",
    "Bereich",
    "",// TODO
    "intervallo",// Italiano
    "interval",
    "",// TODO
    "interval",// Francais Pat
    "",// TODO
    "kyllä",
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
    "",// TODO Èesky (Czech)

  },
  { "channel group",
    "Kanalgruppe",
    "",// TODO
    "gruppo canali",// Italiano
    "kanaal groep",
    "",// TODO
    "Groupe de chaînes",// Francais Pat
    "",// TODO
    "kanavaryhmä",
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
    "",// TODO Èesky (Czech)

  },
  { "Channel group",
    "Kanalgruppe",
    "",// TODO
    "Gruppo canali",// Italiano
    "Kanaal groep",
    "",// TODO
    "Groupe de chaînes",// Francais Pat
    "",// TODO
    "Kanavaryhmä",
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
    "",// TODO Èesky (Czech)

  },
  { "Channel groups",
    "Kanalgruppen",
    "",// TODO
    "Gruppi canali",// Italiano
    "Kanaal groepen",
    "",// TODO
    "Groupe de chaînes",// Francais Pat
    "",// TODO
    "Kanavaryhmät",
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
    "",// TODO Èesky (Czech)

  },
  { "Edit$Delete group?",
    "Gruppe löschen?",
    "",// TODO
    "Eliminare il gruppo?",// Italiano
    "Groep wissen?",
    "",// TODO
    "Effacer groupe $Delete?",// Francais Pat
    "",// TODO
    "Poistetaanko kanavaryhmä?",
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
    "",// TODO Èesky (Czech)

  },
  { "Channel group used by:",
    "Gruppe wird verwendet von:",
    "",// TODO
    "Gruppo canali utilizzato da",// Italiano
    "Kanaal groep gebruikt door:",
    "",// TODO
    "Groupe est utilisé par:",// Francais Pat
    "",// TODO
    "Kanavaryhmä käytössä:",
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
    "",// TODO Èesky (Czech)

  },
  { "Group name",
    "Gruppenname",
    "",// TODO
    "Gruppo nome",// Italiano
    "Groep naam",
    "",// TODO
    "Nom de groupe",// Francais Pat
    "",// TODO
    "Kanavaryhmän nimi",
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
    "",// TODO Èesky (Czech)

  },
  { "Edit channel group",
    "Kanalgruppe editieren",
    "",// TODO
    "Editare il gruppo canali",// Italiano
    "Bewerk kanaal groep",
    "",// TODO
    "Editer groupe de chaînes",// Francais Pat
    "",// TODO
    "Muokkaa kanavaryhmää",
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
    "",// TODO Èesky (Czech)

  },
  { "Group name is empty!",
    "Gruppenname ist leer!",
    "",// TODO
    "Il gruppo nomi e' vuoto!",// Italiano
    "Groep naam is leeg!",
    "",// TODO
    "Le nom est vide",// Francais Pat
    "",// TODO
    "Kanavaryhmältä puuttuu nimi!",
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
    "",// TODO Èesky (Czech)

  },
  { "Group name already exists!",
    "Gruppe existiert bereits!",
    "",// TODO
    "Il gruppo nome esiste gia'!",// Italiano
    "Groep naam existeerd al!",
    "",// TODO
    "Le groupe existe déjà!",// Francais Pat
    "",// TODO
    "Samanniminen kanavaryhmä on jo olemassa!",
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
    "",// TODO Èesky (Czech)

  },

  { "Button$Invert selection",
    "Ausw. umdrehen",
    "",// TODO
    "Invertire la selezione",// Italiano
    "Selectie omdraaien",
    "",// TODO
    "Inversion de la selection",// Francais Pat
    "",// TODO
    "Päinvastoin",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Button$All no",
    "Alle nein",
    "",// TODO
    "Tutto no",// Italiano
    "Allen nee",
    "",// TODO
    "Tous non",// Francais Pat
    "",// TODO
    "Ei yhtään",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Avoid repeats",
    "Vermeide Wiederholung",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Eviter les répétions",// Francais Pat
    "",// TODO
    "Estä uusinnat",
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
    "",// TODO Èesky (Czech)

  },
  { "Compare subtitle",
    "Vergleiche Untertitel",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer les sous-titres",// Francais Pat
    "",// TODO
    "Vertaa jakson nimeä",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Search entries",
    "Sucheinträge",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Entrée de recherche",// Francais Pat
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Button$Show all",
    "Zeige alle",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Montrer tous",// Francais Pat
    "",// TODO
    "Näytä kaikki",
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
    "",// TODO Èesky (Czech)

  },

  { "Edit$Delete entry?",
    "Eintrag löschen?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer l'entrée",// Francais Pat
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
    "",// TODO Èesky (Czech)

  },
  { "Edit$Delete all entries?",
    "Alle Einträge löschen?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer toutes les entrées",// Francais Pat
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
    "",// TODO Èesky (Czech)

  },
  { "Button$Delete all",
    "Alle löschen",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Allowed repeats",
    "Erlaubte Wiederholungen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Répétitions autorisées",// Francais Pat
    "",// TODO
    "Sallittujen uusintojen lukumäärä",
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
    "",// TODO Èesky (Czech)

  },
  { "Compare title",
    "Vergleiche Titel",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Comparer titres",// Francais Pat
    "",// TODO
    "Vertaa nimeä",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Button$by date",
    "nach Datum",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "par la date",// Francais Pat
    "",// TODO
    "Päivämäärä",
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
    "",// TODO Èesky (Czech)

  },
  { "Mark as 'already recorded'?",
    "als 'bereits aufgezeichnet' markieren?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "marquer comme 'déjà enregistré'?",// Francais Pat
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
    "",// TODO Èesky (Czech)

  },
  { "Use VDR's timer edit menu",
    "VDR's Timer-Edit-Menu verw.",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Utiliser le menu édition VDR-programme",// Francais Pat
    "",// TODO
    "Käytä alkuperäistä ajastinvalikkoa",
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
    "",// TODO Èesky (Czech)

  },
  { "Run search timer update?",
    "Suchtimer-Update ausführen?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Mise à jours programmation de recherche?",// Francais Pat
    "",// TODO
    "Päivitetäänkö hakuajastimet?",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Only repeats within ... days",
    "Nur Wiederh. innerhalb ... Tagen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Que répétion, pendant ... jours",// Francais Pat
    "",// TODO
    "Vain uusinnat ... päivän sisällä",
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
    "",// TODO Èesky (Czech)

  },

  { "Programming timer failed!",
    "Timer-Programmierung fehlschlagen!",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "La programmation a échoué",// Francais Pat
    "",// TODO
    "Ajastimen ohjelmointi epäonnistui!",
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
    "",// TODO Èesky (Czech)

  },
  { "Button$Timer preview",
    "Timer-Vorsch.",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Prévu de programmation",// Francais Pat
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Execute search",
    "Suche ausführen",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Trigger search timer update",
    "Suchtimer jetzt aktualisieren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Selection",// Francais Pat
    "",// TODO
    "Päivitä hakuajastimet",
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
    "",// TODO Èesky (Czech)

  },
  { "Show recordings done",
    "Zeige erledigte Aufnahmen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les enregistrements effectués",// Francais Pat
    "",// TODO
    "Näytä tehdyt tallennukset",
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
    "",// TODO Èesky (Czech)

  },
  { "Search timer update done!",
    "Suchtimer-Update durchgeführt!",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "La mise à jours de recherche est effectuée!",// Francais Pat
    "",// TODO
    "Hakuajastimet päivitetty!",
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
    "",// TODO Èesky (Czech)

  },
  { "Show channel separators",
    "Zeige Kanal-Separatoren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher le separateur de chaînes",// Francais Pat
    "",// TODO
    "Näytä kanavaerottimet",
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
    "",// TODO Èesky (Czech)

  },
  { "Show channels without EPG",
    "Zeige Kanäle ohne EPG",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les chaînes sans EPG",// Francais Pat
    "",// TODO
    "Näytä ohjelmaoppaattomat kanavat",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Delete recordings after ... days",
    "Aufn. nach ... Tagen löschen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Effacer l'enregistrement après ... jours",// Francais Pat
    "",// TODO
    "Poista tallenteet ... päivän jälkeen",
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
    "",// TODO Èesky (Czech)

  },
  { "Show episode text in schedules",
    "Zeige Untertitel in Übersicht",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les sous-titre dans le programme",
    "",// TODO
    "Näytä jakson nimi ohjelmistossa",
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
    "",// TODO Èesky (Czech)

  },
  { "Button$Templates",
    "Vorlagen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Modéles",
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
    "",// TODO Èesky (Czech)

  },
  { "Search templates",
    "Such-Vorlagen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Rechercher modèles",
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
    "",// TODO Èesky (Czech)

  },
  { "Edit template",
    "Vorlage editieren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Editer modèles",
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
    "",// TODO Èesky (Czech)

  },
  { "Template name",
    "Vorlagenname",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Nom du modèle",
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
    "",// TODO Èesky (Czech)

  },
  { "Use as template",
    "Als Vorlage verwenden",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Utiliser comme modèles",
    "",// TODO
    "Käytä mallipohjana",
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
    "",// TODO Èesky (Czech)

  },
  { "Copy this entry to templates?",
    "Diesen Eintrag in Vorlagen kopieren?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Copier cette entrée dans modèles?",
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
    "",// TODO Èesky (Czech)

  },
  { "Keep ... recordings",
    "Behalte ... Aufnahmen",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Garder .... enregistrements",
    "",// TODO
    "Säilytä ... tallennetta",
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
    "",// TODO Èesky (Czech)

  },
  { "Overwrite existing entries?",
    "Vorhandene Werte überschreiben?",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Ecraser les informations?",
    "",// TODO
    "Kirjoitetaanko olemassaolevan päälle?",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "Configuración",
    "Ñõèìéóåéò",
    "Inställningar",
    "Configuraþie",
    "Beállítások",
    "Configuració",
    "½ÐáâàÞÙÚÐ",
    "Konfiguracija",
    "Sätted",
    "Indstillinger",
    "Nastavení",
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
    "ÐñïóáñìïãÞ",
    "Ändra",
    "Modificã",
    "Beállítani",
    "Editar",
    "ÀÕÔÐÚâØàÞÒÐâì",
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
    "NÝï",
    "Ny",
    "Nou",
    "Új",
    "Nou",
    "´ÞÑÐÒØâì",
    "Novi",
    "Uus",
    "Ny",
    "Nový",
  },
  { "Button$Delete",
    "Löschen",
    "Izbri¹i",
    "Cancella",
    "Verwijder",
    "Apagar",
    "Supprimer",
    "Slett",
    "Poista",
    "Usunac",
    "Borrar",
    "ÄéáãñáöÞ",
    "Ta bort",
    "ªterge",
    "Törölni",
    "Esborrar",
    "ÃÔÐÛØâì",
    "Obri¹i",
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
    "ÅããñáöÞ",
    "Inspelning",
    "Înregistr.",
    "Felvenni",
    "Gravar",
    "·ÐßØáì",
    "Snimi",
    "Salvesta",
    "Optag",
    "Nahrát",
  },
  { "Button$Now",
    "Jetzt",
    "Sedaj",
    "Adesso",
    "Nu",
    "Agora",
    "Maintenant",
    "Nå",
    "Nyt",
    "Teraz",
    "Ahora",
    "Ôþñá",
    "Nu",
    "Acum",
    "Most",
    "Ara",
    "ÁÕÙçÐá",
    "Trenutno",
    "Nüüd",
    "Nu",
    "Nyní",
  },
  { "Button$Next",
    "Nächste",
    "Sledi",
    "Prossimo",
    "Hierna",
    "Proximo",
    "Après",
    "Neste",
    "Seuraavaksi",
    "Nastêpnie",
    "Después",
    "Åðüìåíï",
    "Nästa",
    "Urmãtor",
    "Következö",
    "Següent",
    "´ÐÛÕÕ",
    "Slijedi",
    "Tulekul",
    "Næste",
    "Dal¹í",
  },
  { "Button$Schedule",
    "Programm",
    "Program",
    "Programma",
    "Programma",
    "Programação",
    "Programme",
    "Programmer",
    "Ohjelmisto",
    "Program",
    "Guía",
    "Ðñüãñáììá",
    "Program",
    "Program",
    "Program",
    "Programar",
    "¿àÞÓàÐÜÜÐ",
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
    "ABÃ/áâã",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "ABC/abc",
    "°±²/ÐÑÒ",
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
    "ÂÐÙÜÕà",
    "",// TODO
    "Taimer",
    "Timer",
    "Èasovaè",
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
    "Prze³±cz",
    "Cambiar",
    "AëëáãÞ",
    "Byt",
    "Comutã",
    "Átkapcsolni",
    "Canviar",
    "¿ÕàÕÚÛîçØâì",
    "Prebaci",
    "Vali",
    "Skift",
    "Pøepnout",
  },
  { "recording with device",
    "Aufnahme mit Gerät",
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
    "",// TODO Èesky (Czech)

  },
  { "Add episode to manual timers",
    "Untertitel in manuellen Timern",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Ajouter sous-titre dans la programmation manuel",
    "",// TODO
    "Lisää jakson nimi norm. ajastimiin",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "smart",
    "intelligent",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "intelligent",
    "",// TODO
    "älykkäästi",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Show day separators",
    "Zeige Tages-Separatoren",
    "",// TODO
    "",// Italiano
    "",// TODO
    "",// TODO
    "Afficher les séparateur de jours",// TODO
    "",// TODO
    "Näytä päiväerottimet",
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
    "",// TODO Èesky (Czech)
  },
  { "Add/Remove to/from switch list?",
    "In/Aus Umschaltliste?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajouter/Effacer de la liste de changement?",
    "",// TODO
    "Lisää/poista kanavanvaihtolistalle",
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
    "",// TODO Èesky (Czech)
  },
  { "Add to switch list?",
    "In Umschaltliste aufnehmen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajouter à la liste de changement de chaine?",
    "",// TODO
    "Lisätäänkö kanavanvaihtolistalle?",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Already running!",
    "Läuft bereits!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Est déjà en cours",
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
    "",// TODO Èesky (Czech)
  },
  { "Show switch list",
    "Zeige Umschaltliste",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher liste de changement",// TODO
    "",// TODO
    "Näytä kanavanvaihtolista",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Switch ... minutes before start",
    "Umschalten ... Minuten vor Start",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Changer ... minutes avant le début",// TODO
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
    "",// TODO Èesky (Czech)
  },
  { "Edit entry",
    "Eintrag editieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Editer l'entrée",// TODO
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
    "",// TODO Èesky (Czech)
  },
  { "action at",
    "Ausführung um",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effectuer à",// TODO
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
    "",// TODO Èesky (Czech)
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
    "ÅããñáöÞ",
    "Inspelning",
    "Înregistr.",
    "Felvenni",
    "Gravar",
    "·ÐßØáì",
    "Snimi",
    "Salvesta",
    "Optag",
    "Nahrát",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Button$Extended",
    "Erweitert",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Précision",// TODO
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
    "",// TODO Èesky (Czech)

  },
  { "Pause when ... recordings exist",
    "Pause, wenn ... Aufnahmen exist.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pause, lorsque ... l'enregistrement existe.",// TODO
    "",// TODO
    "Keskeytä ... tallenteen jälkeen",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Show blacklists",
    "Zeige Ausschlusslisten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher la liste des exclus",// TODO
    "",// TODO
    "Näytä mustat listat",
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
    "",// TODO Èesky (Czech)

  },
  { "Use blacklists",
    "Verw. Ausschlusslisten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser la liste des exclus",// TODO
    "",// TODO
    "Käytä mustia listoja",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Edit$Delete blacklist?",
    "Ausschlussliste löschen?",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Select blacklists",
    "Ausschlusslisten auswählen",
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
    "",// TODO Èesky (Czech)

  },
  { "Blacklist results",
    "Ausschluss-Ergebnisse",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Résultats de la liste des exclus",// TODO
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
    "",// TODO Èesky (Czech)

  },
  { "Edit$Delete template?",
    "Vorlage löschen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer le modèle",// Francais
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "Auxiliary Info",
    "Zusatzinfo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Informations supplémentaires",// TODO
    "",// TODO
    "Lisätiedot",
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
    "",// TODO Èesky (Czech)

  },
  { "Button$Aux info",
    "Zusatzinfo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Infos supplémentaires",
    "",// TODO
    "Lisätiedot",
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
    "",// TODO Èesky (Czech)

  },
  { "Create blacklist",
    "Ausschlussliste anlegen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Créer la liste d'exclusions",
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
    "",// TODO Èesky (Czech)

  },
  { "Disable associated timers too?",
    "Zugehörige Timer auch deaktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Désactiver la programmation correspondante",
    "",// TODO
    "Poistetaanko käytöstä myös assosioidut ajastimet?",
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
    "",// TODO Èesky (Czech)

  },
  { "Activate associated timers too?",
    "Zugehörige Timer auch aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Activer la progammation correspondante",// TODO
    "",// TODO
    "Otetaanko käyttöön myös assosioidut ajastimet?",
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
    "",// TODO Èesky (Czech)

  },
  { "Main menu entry",
    "Hauptmenü-Eintrag",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Visible dans le menu principal",// TODO
    "",// TODO
    "Valinta päävalikossa",
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
    "",// TODO Èesky (Czech)

  },
  { "Delete created timers?",
    "Erstellte Timer löschen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer les programmations crées",// TODO
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
    "",// TODO Èesky (Czech)

  },
  { "Delete all timers created from this search?",
    "Alle Timer löschen, die von dieser Suche erzeugt wurden?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Effacer les programmations issues de cette recherche?",
    "",// TODO
    "Poistetaanko kaikki tällä haulla luodut ajastimet?",
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
    "",// TODO Èesky (Czech)

  },
  { "Use Wareagle icons",
    "Verw. Wareagle-Symbole",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les symboles de Wareagle",// TODO
    "",// TODO
    "Käytä Wareagle-symboleita",
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
    "",// TODO Èesky (Czech)

  },
  { "Show radio channels",
    "Zeige Radiokanäle",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les chaînes de radio",// TODO
    "",// TODO
    "Näytä radiokanavat",
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
    "",// TODO Èesky (Czech)

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
    "",// TODO Èesky (Czech)

  },
  { "fuzzy",
    "unscharf",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "imprécis",
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
    "",// TODO Èesky (Czech)

  },
  { "Tolerance",
    "Toleranz",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Tolérance",// TODO
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
    "",// TODO Èesky (Czech)

  },
  { "'One press' timer creation",
    "Timer mit 'Aufnehmen' sofort anlegen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Programmation immédiate avec enregistrement",
    "",// TODO
    "Luo ajastimet yhdellä painalluksella",// TODO
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
    "",// TODO Èesky (Czech)
  },
  { "Ignore below priority",
    "Ignoriere unter Priorität",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ignorer la priorité inférieure",// Francais Patrice Staudt
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
    "",// TODO Èesky (Czech)
  },
  { "Only check within next ... days",
    "Prüfe nur die nächsten ... Tage",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ne vérifier que les prochains ... jours",
    "",// TODO
    "Tarkista vain seuraavat ... päivää",
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
    "",// TODO Èesky (Czech)
  },
  { "Timer conflict check",
    "Auf Timer-Konflikte prüfen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vérifier les conflits de programmation",
    "",// TODO
    "Tarkista päällekkäiset ajastimet",
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
    "",// TODO Èesky (Czech)
  },
  { "Timer conflicts",
    "Timer-Konflikte",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Conflits de programmation",
    "",// TODO
    "Päällekkäiset ajastimet",
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
    "",// TODO Èesky (Czech)
  },
  { "conflicts",
    "Konflikte",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Conflits",
    "",// TODO
    "päällekkäisyyttä",
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
    "",// TODO Èesky (Czech)
  },
  { "Button$Details",
    "Details",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Détails",
    "",// TODO
    "Lisätiedot",
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
    "",// TODO Èesky (Czech)
  },
  { "Button$Repeats",
    "Wiederh.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Répétitions",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "no conflicts!",
    "keine Konflikte!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pas de conflits",
    "",// TODO
    "ei päällekkäisyyksiä!",
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
    "",// TODO Èesky (Czech)
  },
  { "no important conflicts!",
    "keine wichtigen Konflikte!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "pas de conflits importants!",
    "",// TODO
    "ei merkitseviä päällekkäisyyksiä!",
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
    "",// TODO Èesky (Czech)
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
    "C",// TODO Èesky (Czech)
  },
  { "General",
    "Allgemein",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Général",
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
    "",// TODO Èesky (Czech)
  },
  { "EPG menus",
    "EPG Menüs",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Timer conflict checking",
    "Timer-Konflikt-Prüfung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Vérification-Conflits-Programmation",
    "",// TODO
    "Päällekkäisten ajastimien tarkistus",
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
    "",// TODO Èesky (Czech)
  },
  { "--- Automatic checking ---",
    "--- Automatische Prüfung ---",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "--- Vérifications automatiques ---",
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
    "",// TODO Èesky (Czech)
  },
  { "After each search timer update",
    "Nach jedem Suchtimer-Update",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mise à jours après chaque recherche",
    "",// TODO
    "Päivitettäessä hakuajastimia",
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
    "",// TODO Èesky (Czech)
  },
  { "User-defined EPG times",
    "Benutzerdef. EPG-Zeiten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Date utilisateur pour la progammation",
    "",// TODO
    "Käyttäjän määrittelemät ajankohdat",
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Hides the main menu entry and may be useful if this plugin is used to replace the original 'Schedule' entry.",
    "Blendet den Hauptmenüeintrag aus. Das kann nützlich sein, falls dieses Plugin verwendet wird, um den originalen 'Programm'-Eintrag zu ersetzen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Rend le menu invisible. C'est utile lorsque le plugin remplace le menu programmme d'origine",
    "",// TODO
    "Piilottaa valinnan päävalikosta. Suositellaan asetettavaksi päälle, jos laajennoksella korvataan alkuperäinen ohjelmaopas.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$The name of the main menu entry which defaults to 'Programm guide'.",
    "Der Name des Hauptmenüeintrags. Die Standard-Vorgabe ist 'Programmführer'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Le nom dans le menu principale. La prédéfinition est 'Guide du programme'. ",
    "",// TODO
    "Päävalikossa sijaitsevan valinnan nimi, joka on oletuksena 'Ohjelmaopas'.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$When VDR is patched to allow this plugin to replace the original 'Schedule' entry, you can de/activate this replacement here.",
    "Falls VDR gepatched wurde, um dem Plugin zu erlauben, den originalen 'Programm'-Eintrag zu ersetzen, so kann hier diese Ersetzung de/aktiviert werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsque le VDR est muni du patch pour autoriser le remplacement du menu original 'Programme' , vous pouvez activer ou déactiver celui-ci.",
    "",// TODO
    "Korvaa alkuperäisen ohjelmaoppaan päävalikossa tällä laajennoksella, jos VDR on paikattu asianmukaisesti.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Choose between 'Overview - Now' and 'Schedule' as start menu when this plugin is called.",
    "Hier kann zwischen 'Übersicht - Jetzt' und 'Programm' als Start-Menü für das Plugin gewählt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez choisir entre 'Maintenant' et 'Programme' comme menu de départ pour le plugin.",
    "",// TODO
    "Valitsee oletusnäytöksi joko tämänhetkisen yleiskatsauksen tai valitun kanavan ohjelmiston.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Choose which standard function ('Record' or 'Commands') you like to have on the red key.\n(Can be toggled with key '0')",
    "Hier kann ausgewählt werden, welche Standardfunktion ('Aufnehmen' oder 'Befehle') auf der roten Taste liegen soll.\n(Umschalten auch mit Taste '0' möglich)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez choisir entre la fonction standart 'Enregistrer' et 'Commande' comme menu de la touche rouge.\n(Changement possible avec la touch '0')",
    "",// TODO
    "Valitsee oletustoiminnon ('Tallenna' tai 'Komennot') punaiselle näppäimelle.\nPunaisen näppäimen toimintoa voidaan vaihtaa myös lennossa painamalla '0' näppäintä.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Choose which standard function ('Switch'/'Info' or 'Search') you like to have on the blue key.\n(Can be toggled with key '0')",
    "Hier kann ausgewählt werden, welche Standardfunktion ('Umschalten'/'Info' oder 'Suche') auf der blauen Taste liegen soll.\n(Umschalten auch mit Taste '0' möglich)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez choisir entre la fonction standart 'Changer de chaine' et 'Recherche' comme menu de la touche bleu.\n(Changement possible avec la touch '0')",
    "",// TODO
    "Valitsee oletustoiminnon ('Valitse' tai 'Etsi') siniselle näppäimelle.\nSinisen näppäimen toimintoa voidaan vaihtaa myös lennossa painamalla '0' näppäintä.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Shows a progressbar in 'Overview - Now' that informs about the remaining time of the current event.\nDepending on your selected skin you can choose between 'Pipes' or 'graphical' to get the best look.",
    "Zeigt einen Fortschrittsbalken in 'Übersicht - Jetzt' an, der wiedergibt wieweit die Sendung bereits fortgeschritten ist.\nAbhängig vom gewählten Skin kann man zwischen 'Pipes' und 'grafisch' wählen, um die Darstellung zu verbessern.",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Affiche la barre de progression dans le menu 'Maintenant', qui indique l'avancement de l'émission en cours.\nL'afficher est mieu approprié suivant le skin choisi en 'Bar verticale' ou 'graphique'.",
    "",// TODO
    "Näyttää aikajanan tämänhetkisessä yleiskatsauksessa.\nVoit valita käyttämäsi ulkoasun mukaan joko teksti- tai grafiikkapohjaisen aikajanan.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Display channel numbers in 'Overview - Now'.\n\n(To completely define your own menu look please inspect the MANUAL)",
    "Anzeige von Kanalnummern in 'Übersicht - Jetzt'.\n\n(Zur vollständigen Anpassung einer eigenen Menüdarstellung, bitte das MANUAL lesen)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Affiche du numéro de chaîne dans la vue 'Maintenant'.\n\n(Pour les réglages de l'affichage menu propre à l'utilisateur, lire le MANUAL)",
    "",// TODO
    "Näyttää kanavien numeroinnin tämänhetkisessä yleiskatsaksessa.\n\nLisätietoja valikon muokkauksesta löytyy MANUAL-tiedostosta.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Display VDR channel groups as separators between your channels in 'Overview - Now'.",
    "Anzeige von VDR-Kanalgruppen als Trennlinien zwischen den Programmen in 'Übersicht - Jetzt'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les groupes de chaines avec une ligne de séparation dans la vue 'Maintenant'.",
    "",// TODO
    "Näyttää erottimet yleiskatsauksessa kanavaville.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Display a separator line at day break in 'Schedule'.",
    "Anzeige von Trennlinien beim Datumswechsel in der Programmübersicht.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher une ligne de séparation au changement de date dans la vue 'programme'.",
    "",// TODO
    "Näyttää erottimet ohjelmistossa päivän vaihtuessa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Show also radio channels.",
    "Auch Radio-Kanäle anzeigen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher aussi les chaînes de radio.",
    "",// TODO
    "Näyttää myös radiokanavat listauksessa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$When a timer is created with 'Record' you can choose between an immediate creation of the timer or the display of the timer edit menu.",
    "Beim Erzeugen eines Timers mit 'Aufnehmen' kann hier zwischen dem sofortigen Anlegen des Timers oder der Anzeige des Timer-Edit-Menüs gewählt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lors de la programmation avec 'Enregistrer' vous pouvez choisir entre créer directement ou consulter les détails de la programmation.",
    "",// TODO
    "Valitsee oletustoiminnon ajastimen luonnille. Voit luoda ajastimen joko automaattisesti yhdellä painalluksella tai vaihtoehtoisesti avata ajastinvalikon.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Choose 'yes' here if you want to display channels without EPG in 'Overview - Now'. 'Ok' on these entries switches the channel.",
    "Hier kann eingestellt werden, ob auch Programme ohne EPG in 'Übersicht - Jetzt' erscheinen sollen. Ein 'Ok' auf diesen Einträgen schaltet zu diesem Kanal um.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez faire disparaitre ou non les chaines sans infos EPG dans la vue 'Maintenant'. Un 'Ok' sur le programme change sur cette chaînes.",
    "",// TODO
    "Näyttää tämänhetkisessä yleiskatsauksessa myöskin kanavat, joilta ei löydy ohjelmatietoja. Voit vaihtaa tälläiselle kanavalle 'OK' näppäimellä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Choose here the time interval which should be used for jumping through the EPG by pressing FRew/FFwd.\n\n(If you don't have those keys, you can toggle to this functionality pressing '0' and get '<<' and '>>' on the keys green and yellow)",
    "Hier kann die Zeitspanne eingestellt werden, die beim Drücken von FRew/FFwd als Sprung durch den EPG benutzt werden soll.\n\n(Falls diese Tasten nicht vorhanden sind, kann mit '0' ebenfalls diese Funktion erreicht werden. Man erhält dann '<<' und '>>' auf den Tasten Grün/Gelb)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous règlez un laps de temps, qui est utilisé avec les touches '<<' et '>>' à travers les EPG.\n\n(Si vous n'avez pas ces touches sur votre télécommande, vous pouvez aussi atteindre ces fonctions par la touche '0' grâce à laquelle on obtient les fonctions '<<' et '>>' sur les touches Vert/Jaune)",
    "",// TODO
    "Valitsee ohjelmaoppaan oletusaikasiirtymän pikakelausnäppäimille.\n\nJos sinulla ei ole kyseisiä näppäimiä käytössäsi, voit asettaa kyseiset toiminnot vihreälle ja keltaiselle näppäimelle painamalla '0' näppäintä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify if green and yellow shall also be switched when pressing '0'.",
    "Hiermit wird angegeben, ob die Tasten Grün/Gelb beim Drücken von '0' mitumgeschaltet werden sollen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez, si les touches Verte/Jaune sont commutées avec la touches '0'.",
    "",// TODO
    "Määrittelee vaihdetaanko vihreän ja keltaisen näppäimen toimintoja painamalla '0' näppäintä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Besides 'Now' and 'Next' you can specify up to 4 other times in the EPG which can be used by repeatedly pressing the green key, e.g. 'prime time', 'late night',...",
    "Neben 'Jetzt' und 'Nächste' können bis zu 4 weitere Zeiten im EPG angegeben werden, die durch wiederholtes Drücken der Taste Grün verwendet werden können, z.B. 'Abends', 'Spätabend',...",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "En plus de 'Maintenant' et 'Après' vous pouvez définir encore 4 autres crénaux comme 'Le soir', 'La nuit' ou 'encore plus tard' en appuyant plusieurs fois sur la touche verte...",
    "",// TODO
    "Määrittelee käyttäjän muokattavissa olevat ajankohdat 'Nyt' ja 'Seuraavaksi' näyttöjen rinnalle ohjelmaoppaan yleiskatsaukseen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$This is the description for your user-defined time as it will appear as label on the green button.",
    "Dies ist die Beschreibung für die benutzer-definierte Zeit, wie sie als Beschriftung für die Taste Grün verwendet werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici la description des temps utilisateurs, accessible en appuyant plusieurs foix sur la touche verte",
    "",// TODO
    "Asettaa kuvauksen vihreään näppäimeen käyttäjän muokkaamalle ajankohdalle.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify the user-defined time here in 'HH:MM'.",
    "Hier wird die benutzer-definierte Zeit im Format 'HH:MM' angegeben.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous définisez le temps utilisateur dans le format 'HH:MM' par exemple '20:45'",
    "",// TODO
    "Asettaa kellonajan käyttäjän muokkaamalle ajankohdalle.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$This plugin has it's own timer edit menu extending the original one with some extra functionality like\n- an addtional directory entry\n- user-defined days of week for repeating timers\n- adding an episode name\n- support for EPG variables (see MANUAL)",
    "Diese Plugin hat sein eigenes Timer-Edit-Menü, das das orginale um einige zusätzliche Funktionen erweitert:\n\n- ein zusätzlicher Verzeichniseintrag\n- benutzerdef. Wochentage für Wiederh.-Timer\n- Ergänzung um Episodenname\n- Unterstützung von EPG-Variablen (s.MANUAL)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ce plugin a sont propre éditeur de programmation, qui enrichit l'originale avec beaucoup le fonctions interressantes:\n\n- une entrée de dossier supplèmentaire\n- des programmations hebdomadaires définies par l'utilisateur\n- l'utilisation de variable de l'EPG (voire MANUAL)",
    "",// TODO
    "Tällä laajennoksella on oma laajennettu ajastinvalikko mm. seuraavilla lisätoiminnoilla:\n\n- hakemiston määritys\n- vapaasti määriteltävät viikonpäivät toistuville ajastimille\n- jakson nimen täydennys\n- erilaisten EPG-muuttujien tuki (kts. MANUAL-tiedostosta)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$When creating a timer you can specify here a default recording directory.",
    "Beim Anlegen eines Timers kann hier ein Standard-Aufnahmeverzeichnis vorgegeben werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lors de la programmation vous pouvez définir un dossier par défaut.",
    "",// TODO
    "Määrittelee oletushakemiston tallenteille.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$If you create a timer for a series, you can automatically add the episode name.\n\n- never: no addition\n- always: always add episode name if present\n- smart: add only if event lasts less than 80 mins.",
    "Wenn man einen Timer für eine Serie erzeugt, kann mit dieser Option automatisch der Episodenname ergänzt werden.\n\n- niemals: keine Ergänzung\n- immer: immer ergänzen, falls Episodenname vorhanden.\n- intelligent: nur ergänzen, wenn Sendung weniger als 80 min dauert",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsque l'on créé une programmation d'une série, on peut voir automatiquement le nom de l'épisode.\n\n- Jamais: sans nom d'épisode\n- Toujours:Compléter toujours avec le noms de l'épisode, si existant.\n- intelligent: compléter, si l'épisode dure moins de 80 min",
    "",// TODO
    "Asettaa toiminnon sarjatallennuksen jakson nimen lisäykselle:\n\n- ei koskaan: ei lisätä\n- aina: lisätään aina, jos olemassa\n- älykkäästi: lisätään vain, jos tapahtuman kesto on alle 80 min",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$'Search timers' can be used to automatically create timers for events that match your search criterions.",
    "'Suchtimer' können verwendet werden um automatisch Timer für Sendungen zu erstellen, die von einer Suche gefunden werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "'Programmation de recherche' peut être utilisé pour programmer automatiquement des émissions, qui sont trouvées par la recherche.",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify here the time intervall to be used when searching for events in the background.",
    "Hier kann das Zeitintervall angegeben werden, in dem im Hintergrund automatisch nach neuen Sendungen gesucht werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'interval avec lequel de nouvelles émissions a programmer sont recherchées.",
    "",// TODO
    "Määrittelee päivitysvälin hakuajastimien taustapäivitykselle.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Programming of new timers or timer changes is done with SVDRP. The default value should be correct, so change it only if you know what you are doing.",
    "Die Programmierung neuer Timer oder Timer-Änderungen erfolgt mit SVDRP. Der Vorgabewert hier sollte immer korrekt sein, also nur ändern, wenn man weiß, was man tut.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La programmation de nouveaux enregistrements ou changements de programmation est réalisé avec SVDRP. La valeur par défaut doit absolument être correcte, ne la changez que si vous savez ce que vous faites.",
    "",// TODO
    "Uusien ajastimien luonti ja olemassa olevien muokkaus on toteutettu SVDRP-protokollan kautta. Suositellaan käytettävän vain oletusarvoa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify here the default priority of timers created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Priorität für Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous définissez la priorité standard de la programmation crée par le plugin. Cette valeur peut être réglée de facon individuelle pour chaque recherche.",
    "",// TODO
    "Määrittelee oletusprioriteetin tämän laajennoksen kautta luotaville ajastimille. Prioriteettia voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify here the default lifetime of timers/recordings created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Lebensdauer für Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous définissez la durée de vie de l'enregistrement crée par le plugin. Cette valeur pe        ut être réglée de facon individuelle pour chaque recherche.",
    "",// TODO
    "Määrittelee oletuseliniän tämän laajennoksen kautta luotaville ajastimille. Elinaikaa voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify here the default start recording margin of timers/recordings created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Vorlaufzeit für Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous définissez la marge antérieure à la programmation crée par le plugin. Cette valeur peut être réglée de facon individuelle pour chaque recherche.",
    "",// TODO
    "Määrittelee oletusmarginaalin tallennuksen aloitukselle tämän laajennoksen kautta luotaville ajastimille. Marginaaleja voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify here the default stop recording margin of timers/recordings created with this plugin. This value can also be adjusted for each search itself.",
    "Hier wird die Standard-Nachlaufzeit für Timer angegeben, die von diesem Plugin erzeugt werden. Dieser Wert kann aber auch bei jeder Suche einzeln gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous définissez la marge postérieure de la programmation crée par le plugin. Cette valeur peut être réglée de facon individuelle pour chaque recherche.",
    "",// TODO
    "Määrittelee oletusmarginaalin tallennuksen lopetukselle tämän laajennoksen kautta luotaville ajastimille. Marginaaleja voidaan muokata ajastinkohtaisesti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Set this to 'yes' if don't want to see events on PayTV channels when searching for repeats.",
    "Um bei der Suche nach Wiederholungen Sendungen von PayTV-Kanälen zu ignorieren, kann hier die Option auf 'Ja' gesetzt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pour éviter les recherches sur les chaînes payantes, vous mettez l'option sur 'oui'",
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Here you can setup templates for your searches.",
    "Hier können Vorlagen für Suchen erstellt werden.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous créer vos recherches comme modèles",
    "",// TODO
    "Määrittele mallipohjia hakuehdoille.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Here you can setup blacklists which can be used within a search to exclude events you don't like.",
    "Hiermit können Ausschlusslisten erstellt werden, die innerhalb einer Suche verwendet werden können, um Sendungen auszuschließen, die man nicht haben will.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec ceci vous changez les listes d'exclusions, qui sont utilisées par la recherche automatique, pour éviter des emissions que vous ne voulez pas voir.",
    "",// TODO
    "Määrittele mustia listoja hakuehdoille. Mustien listojen tapahtumia ei näytetä hakutuloksissa.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Here you can setup channel groups which can be used within a search. These are different to VDR channel groups and represent a set of arbitrary channels, e.g. 'FreeTV'.",
    "Hier können Kanalgruppen erstellt werden, die innerhalb einer Suche verwendet werden können. Die Kanalgruppen sind nicht mit den VDR-Kanalgruppen zu vergleichen, sondern stellen einfach eine Gruppe von beliebigen Sendern dar, z.B. 'FreeTV'.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous définissez des groupes de chaînes qui sont utilisés par la recherche. Ce ne sont pas les groupes de chaînes définis dans VDR comme 'Cinéma', mais une sélection propre de l'utilisateur.",
    "",// TODO
    "Määrittele kanavaryhmiä hakuehdoille. Tämän laajennoksen kanavaryhmät eivät ole yhteydessä VDR:n omiin kanavaryhmiin.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$If a timer with priority below the given value will fail it will not be classified as important. Only important conflicts will produce an OSD message about the conflict after an automatic conflict check.",
    "Falls ein Timer fehlschlagen wird, dessen Priorität unter dem angegeben Wert liegt, wird er als 'nicht wichtig' eingestuft. Nur bei wichtigen Konflikten erfolgt eine Nachricht per OSD bei der automatischen Konfliktprüfung.",
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsqu'une programmation dont la prioritée est en dessous de la valeur définie, n'aboutit pas, alors elle est qualifiée de 'non-importante'. Seulement les conflits importants sont affichés à l'écran lors de la vérification automatique.",
    "",// TODO
    "",// TODO
    "Asetusarvoa korkeammalla prioriteetilla olevat ajastimet ovat merkitseviä. Vain merkitsevät päällekkäisyydet aiheuttavat viestin kuvaruutunäytölle automaattisen tarkistuksen yhteydessä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$This value reduces the conflict check to the given range of days. All other conflicts are classified as 'not yet important'.",
    "Mit diesem Wert kann der Zeitbereich der Konfliktprüfung eingestellt werden. Alle Konflikte ausserhalb des Bereichs werden als 'noch nicht wichtig' eingestuft.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec cette valeur vous définissez l'interval de temps de la vérification de conflits. Tous ces conflits en dehors de cet interval sont qualifés comme 'pas encore importants.",
    "",// TODO
    "Asettaa ajanjakson päivinä päällekkäisyyksien tarkistukselle. Muita päällekkäisyyksiä ei pidetä vielä merkitsevinä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Set this to 'yes' if the conflict check should be performed after each search timer update.",
    "Hier auf 'Ja' setzen, wenn die Konfliktprüfung nach jedem Suchtimer-Update erfolgen soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec 'Oui' vous impliquez la vérification de conflits à la mise à jours de la recherche.",
    "",// TODO
    "Suorittaa päällekkäisyyksien tarkistuksen jokaisen hakuajastimen päivityksen yhteydessä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Specify here the time intervall to be used for an automatic conflict check in the background.\n('0' disables an automatic check)",
    "Hier kann das Zeitintervall angegeben werden, in dem eine automatische Konfliktprüfung im Hintergrund erfolgen soll.\n(mit '0' wird die automatsiche Prüfung deaktiviert.)",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'interval de temps dans lequel la vérification de conflits est effectuée.\n(avec '0' vous déactivez la vérification.)",
    "",// TODO
    "Määrittää aikajakson taustalla tehtävälle automaattiselle päällekkäisyyksien tarkistukselle.\nArvolla '0' saat asetettua automaattisen tarkistuksen pois päältä",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "if conflicts within next ... minutes",
    "Wenn nächster Konflikt in ... Minuten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Si conflit dans ... minutes suivantes",
    "",// TODO
    "jos päällekkäisyys ... min. kuluessa",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$If the next conflict will appear in the given number of minutes you can specify here a shorter check intervall to get more OSD notifications about it.",
    "Wenn der nächste Konflikt innerhalb der angegebenen Anzahl von Minuten liegt, kann man hiermit ein kürzeres Prüfintervall angeben, damit man häufiger über den Konflikt per OSD benachrichtigt wird.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Si le conflit à lieu dans l'interval de temp indiqué, alors il est possible de règler un interval de temps plus petit pour la vérification de conflits avec affichage à lécran.",
    "",// TODO
    "Jos seuraava päällekkäisyys esiintyy asetetun ajan kuluessa, tällä asetuksella pystyt määrittämään lyhyemmän tarkistusvälin saadaksesi tarkempia kuvaruutuviestejä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Values for EPG category",
    "Werte für EPG-Kategorie",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Valeur pour la catégories EPG",
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
    "",// TODO Èesky (Czech)
  },
  { "Button$Apply",
    "Übernehmen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Appliquer",
    "",// TODO
    "Käytä",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Ignore conflict duration less ... min.",
    "Ignoriere Konfliktdauer unter ... Min.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ignorer les conflits inférieurs à ... min.",
    "",// TODO
    "Sivuuta alle ... min. päällekkäisyydet",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$If a conflicts duration is less then the given number of minutes it will not be classified as important. Only important conflicts will produce an OSD message about the conflict after an automatic conflict check.",
    "Falls die Konfliktdauer unter der angegebenen Anzahl an Minuten liegt, wird er als 'nicht wichtig' eingestuft. Nur bei wichtigen Konflikten erfolgt eine Nachricht per OSD bei der automatischen Konfliktprüfung.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Lorsque le temps de conflit est sous le nombre de minutes indiqué, ceci est considéré comme 'non-important'. Uniquement les conflits importants sont affichés à l'écran lors de la vérification automatique de la programmation.",
    "",// TODO
    "Jos ajastimien päällekkäisyys on alle asetetun minuuttimäärän, sitä ei pidetä merkitsevänä. Vain merkitsevät päällekkäisyydet aiheuttavat viestin kuvaruutunäytölle tarkistuksen yhteydessä.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Timer conflict! Show?",
    "Timer-Konflikt! Anzeigen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher les conflits de programmation?",
    "",// TODO
    "Päällekkäisiä ajastimia! Näytetäänkö?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },

  { "After each timer programming",
    "Nach jeder Timer-Programmierung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Après chaque programmation",
    "",// TODO
    "Jokaisen ajastimen luonnin jälkeen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Set this to 'yes' if the conflict check should be performed after each manual timer programming. In the case of a conflict you get immediately a message that informs you about it. The message is only displayed if this timer is involved in any conflict.",
    "Hier auf 'Ja' setzen, wenn die Konfliktprüfung nach jeder manuellen Timer-Programmierung erfolgen soll. Im Falle eines Konflikts wird dann sofort eine Nachricht angezeigt. Diese erscheint nur, wenn dieser Timer in einen Konflikt verwickelt ist.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Päällekkäisten ajastimien tarkistus suoritaan aina jokaisen ajastimen luonnin jälkeen. Jos luodulle ajastimelle löydetään päällekkäisyyksiä, niin siitä tiedotetaan käyttäjälle heti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Help$Set this to 'yes' if the conflict check should be performed when a recording starts. In the case of a conflict you get immediately a message that informs you about it. The message is only displayed if the conflict is within the next 2 hours.",
    "Hier auf 'Ja' setzen, wenn die Konfliktprüfung beim Beginn jeder Aufnahme erfolgen soll. Im Falle eines Konflikts wird dann sofort eine Nachricht angezeigt. Diese erscheint nur, wenn der Konflikt innerhalb der nächsten 2 Stunden auftritt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Päällekkäisten ajastimien tarkistus suoritaan aina tallennuksen alkaessa. Jos luodulle ajastimelle löydetään päällekkäisyyksiä seuraavan kahden tunnin aikana, niin siitä tiedotetaan käyttäjälle heti.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "Use in favorites menu",
    "In Favoritenmenü verw.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Utiliser dans le menu favoris",
    "",// TODO
    "Käytä suosikkina",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Show favorites menu",
    "Zeige Favoriten-Menü",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Afficher le menu favoris",
    "",// TODO
    "Näytä suosikkivalikko",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
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
    "",// TODO Èesky (Czech)
  },
  { "after user-def. times",
    "nach ben.-def. Zeiten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "après le temp utilisateur",
    "",// TODO
    "ajankohtien jälkeen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },

  { "Help$A favorites menu can display a list of your favorite broadcasts. Enable this if you want an additional menu besides 'Now' and 'Next'\nAny search can be used as a favorite. You only have to set the option 'Use in favorites menu' when editing a search.",
    "Ein Favoriten-Menü kann eine Liste der Lieblingssendungen anzeigen. Hier auswählen, wenn neben 'Jetzt' und 'Nächste' ein solches Menü gewünscht wird.\nJede Suche kann als Favorit verwendet werden. Dazu einfach die Option 'In Favoritenmenü verw.' beim Editieren einer Suche setzen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Un menu favori peut contenir la liste de vos émission préférée. Faire le choix ici, si vous voulez avoir un menu favori à coté de 'Maintenant' et 'plus tard.\nChaque recherche peut être utilisé comme favori. Pour cela simplement utiliser l'option Favori lors de l'édition de recherche.",
    "",// TODO
    "Suosikkivalikossa listataan suosikiksi merkityt ohjelmat. Jos tämä valinta on aktiivinen, suosikkivalikko löytyy 'Nyt'- ja 'Seuraavaksi'-valikkojen rinnalta. \nMikä tahansa haku voidaan merkitä suosikiksi aktivoimalla 'Käytä suosikkina'-valinta.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Ok key",
    "Taste Ok",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Touche Ok",
    "",// TODO
    "'OK' näppäin",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
  },
  { "Help$Choose here the behaviour of key 'Ok'. You can use it to display the summary or to switch to the corresponding channel.\nNote: the functionality of key 'blue' (Switch/Info/Search) depends on this setting.",
    "Hier kann das Verhalten der 'Ok'-Taste bestimmt werden. Man kann damit die Inhaltsangabe anzeigen oder zum entsprechenden Sender wechseln.\nHinweis: Die Funktion der Taste 'Blau' (Umschalten/Info/Suche) hängt von dieser Einstellung ab.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous pouvez influencé le comportement de la touch 'Ok'. On peut afficher le continue d'information ou changer pour la chaînes proposée.\nIndice: La fonction de la touch 'bleu' (changer/Infos/Recherche) dépend de ce réglage.",  // Traduction de Leslie et Patrice Staudt 01.02.2007
    "",// TODO
    "Valitse toiminto 'OK' näppäimelle: näytä ohjelman tiedot tai vaihda kyseiselle kanavalle.\nHuomioi: sinisen näppäimen toiminto (Vaihda/Tiedot/Hae) riippuu tästä asetuksesta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO Eesti
    "",// TODO Dansk
    "",// TODO Èesky (Czech)
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
    "Prze³±cz",
    "Cambiar",
    "AëëáãÞ",
    "Byt",
    "Comutã",
    "Átkapcsolni",
    "Canviar",
    "¿ÕàÕÚÛîçØâì",
    "Prebaci",
    "Vali",
    "Skift",
    "Pøepnout",
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
    "Ðëçñïöïñßåò",
    "Info",
    "Info",
    "",//TODO
    "",//TODO
    "¸ÝäÞ",
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
    "Ðëçñïöïñßåò",
    "Info",
    "Info",
    "",//TODO
    "",//TODO
    "¸ÝäÞ",
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
    "La recherche a été activée dans la configuration.", // Traduction de Leslie et Patrice Staudt 18.10.2006
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
    "",// TODO Èesky (Czech)
  },
  { "Result menu layout",
    "Layout des Ergebnismenüs",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Dispositon du menu de résultat",
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
    "",// TODO Èesky (Czech)
  },
  { "Email notification",
    "Email-Benachrichtigung",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Notification par mail",
    "",// TODO
    "Ilmoitukset sähköpostilla",
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
    "Hier mit 'Ja' auswählen, ob eine Email-Benachrichtigung über automatisch im Hintergrund programmierte Suchtimer versandt werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui',si vous voulez que les nouvelles programmations automatiques soient notifiées par mail.",
    "",// TODO
    "Tällä asetuksella saat ilmoitukset automaattisesti lisätyistä hakuajastimista sähköpostiisi.",
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
    "Ilmoitukset päällekkäisistä ajastimista",
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
    "Hier mit 'Ja' auswählen, ob eine Email-Benachrichtigung über Timer-Konflikte versändet werden soll.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui',si les conflits de programmation doivent être notifiés.",
    "",// TODO
    "Tällä asetuksella saat ilmoitukset päällekkäisistä ajastimista sähköpostiisi.",
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
    "--- Sähköposti ---",
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
    "Ici vous indiquez l'adresse mail, a laquelle les notifications sont envoyées.",
    "",// TODO
    "Määrittelee sähköpostiosoitteen, jonne ilmoitukset lähetetään.",
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
    "Lähetystapa",
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
    "Bitte hier die gewünschte Methode zum Emailversand auswählen. Verfügbar sind:\n - 'sendmail': erfordert ein korrekt konfiguriertes Mailsystem\n - 'SendEmail.pl': ein einfaches Skript zum Mailversand",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Määrittelee lähetystavan sähköposteille.\nVoit valita kahden vaihtoehdon väliltä:\n - 'sendmail': vaatii oikein konfiguroidun sähköpostisysteemin\n - 'SendEmail.pl': yksinkertainen skriptitiedosto sähköpostin lähetykseen",
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
    "Sähköpostiosoite",
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
    "Määrittelee sähköpostiosoitteen, josta ilmoitukset lähetetään.",
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
    "Hier wird der SMTP Server hinterlegt, über den die Benachrichtigungen versandt werden. Falls dieser einen anderen Port als den Standard(25) verwendet, dann mit \":port\" anhängen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez le serveur de mail sortant SMTP, par lequel les notifications vont être envoyées. Si le port standart(25) n'est pas utilisé, alors ajoutez \":port\" par exemple smtp.orange.fr:2525. ",
    "",// TODO
    "Määrittelee käytettävän SMTP-palvelimen. Portti voidaan määritellä erikseen lisäämällä palvelimen osoitteen loppuun \":portti\", jos se eroaa oletuksesta (25).",
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
    "Käytä SMTP-autentikointia",
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
    "Hier auf 'Ja' setzen, wenn das Emailkonto SMTP-Authentifizierung zum Emailversand benötigt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui',si le compte mail utilise l'authentification pour les mails sortants.",
    "",// TODO
    "Asettaa SMTP-autentikoinnin päälle sähköpostin lähetystä varten.",
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
    "SMTP-käyttäjätunnus",
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
    "Hier wird der Auth-Benutzername angegeben, falls dieses Emailkonto eine Authentifizierung für SMTP benötigt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez l'utilisateur pour l'authentification, lorsque le compte mail sortant SMTP en a besoin.",
    "",// TODO
    "Määrittelee käyttäjätunnuksen, jos SMTP-palvelimelle pitää autentikoida.",
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
    "Hier wird das Auth-Passwort angegeben, falls dieses Emailkonto eine Authentifizierung für SMTP benötigt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ici vous indiquez le mot de passe pour l'authentification, lorsque le compte mail sortant SMTP en a besoin.",
    "",// TODO
    "Määrittelee salasanan, jos SMTP-palvelimelle pitää autentikoida.",
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
    "Mailkonto-Prüfung fehlgeschlagen!",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La vérification du compte mail a échoué",
    "",// TODO
    "Sähköpostilaatikon tarkistus epäonnistui!",
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
    "Benachrichtigung über neue Timer",
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
    "Yhtään uutta ajastinta ei lisätty.",
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
    "Es wurden keine Timer geändert.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "La programmation n'a pas été changée.",
    "",// TODO
    "Yhtään ajastinta ei muokattu.",
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
    "Es wurden keine Timer gelöscht.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aucune programmation n'a été effacée.",
    "",// TODO
    "Yhtään ajastinta ei poistettu.",
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
    "für die nächsten ... Stunden",
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
    "Mit diesem Wert wird die Zeitspanne eingestellt, für die Favoriten angezeigt werden sollen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec cette valeur vous réglez un interval, pour afficher les favoris.",
    "",// TODO
    "Tämä arvo määrittelee käytetyn ajanjakson suosikkien näyttämiselle.",
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
    "Überwachung",
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
    "ohne Überwachung",
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
    "à partir du numéro d'émission",
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
    "à partir chaînes/heures",
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
    "Standard-Timer-Prüfmethode",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Méthode standart de vérification de programmation",
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
    "Manuelle Timer können hinsichtlich EPG-Änderungen überwacht werden. Hier kann die Standard-Prüfmethode für jeden Kanal hinterlegt werden. Zur Wahl stehen:\n\n- keine Prüfung\n- anhand Sendungskennung: geprüft wird anhand einer Kennung, die durch den Sender vergeben wird.\n- anhand Sender/Uhrzeit: geprüft wird anhand der Sendung, die am besten zur Dauer der ursprünglichen Sendung passt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Des programmations manuelles peuvent être surveillés par rapport au changement d'EPG. Ici vous pouvez définir une méthode de vérification standart pour chaque chaîne. Les choix:\n\n- pas de vérification\n- à partir du numéro d'émission: la vérification se fait à partir d'un numéro délivré par l'émetteur.\n- à partir de la chaîne/heure: vérification à partir de l'émission, qui est la mieux adaptée avec la durée de l'émission initiale.",
    "",// TODO
    "Käsinluotuja ajastimia voidaan valvoa ohjelmaoppaan muutoksia vasten. Voit asettaa oletustavan jokaista kanavaa kohden:\n\n- ei valvontaa\n- tapahtuman tunniste: ohjelmisto-oppaassa lähetettävän tunnisteen mukaan\n- kanava ja aika: tapahtuman keston ja ajankohdan mukaan",
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
    "Afficher la programmation effectuée",
    "",// TODO
    "Näytä luodut ajastimet",
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
    "Keine Ankündigungen bei Wiedergabe",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Pas d'annonces lors de la lecture",
    "",// TODO
    "Älä muistuta toiston aikana",
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
    "Bitte auf 'Ja' setzen, wenn während einer Wiedergabe keine Ankündigungen über Sendungen erwünscht sind.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez 'oui' si aucune annonce d'émission n'est voulue pendant la lecture.",
    "",// TODO
    "Laittamalla tämän asetuksen päälle et saa ohjelmista muistutuksia toistettaessa tallenteita.",
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
    "Timer nach Löschen neuprogrammieren",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Recrer la programmation après l'éffacement",
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
    "Hier 'Ja' wählen, wenn gelöschte Timer mit dem nächsten Suchtimer-Update neu programmiert werden sollen.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Choisissez 'oui' pour refaire les programmations de recherche lorsque vous les avez effacés.",
    "",// TODO
    "Laittamalla tämän asetuksen päälle saat luotua ajastimet uudelleen seuraava hakuajastin päivityksen yhteydessä, jos olet poistanut ne.",
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
    "Kanäle begrenzen von 1 bis",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Limiter aux chaînes de 1 à",
    "",// TODO
    "Näytettävien kanavien rajoitus",
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
    "Bei einer sehr großen Kanalliste läßt sich der Menü-Aufbau mit dieser Einstellung durch eine Einschränkung der angezeigten Kanäle beschleunigen. Mit '0' wird das Limit aufgehoben.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Avec cette limite vous rendez l'affichage du menu plus rapide si vous avez une liste de chaînes très longue. Avec '0' vous enlevez la limitation.",
    "",// TODO
    "Voit nopeuttaa toimintoja rajoittamalla näytettävien kanavien lukumäärää tällä asetuksella. Poistat rajoituksen asettamalla arvoksi nollan.",
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
    "Älä näytä ilmoituksia toiston aikana",
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
    "Bitte auf 'Ja' setzen, wenn während einer Wiedergabe keine OSD-Benachrichtigungen über Timer-Konflikte gewünscht sind. Die Benachrichtigung erfolgt trotzdem, wenn der nächste Konflikt innerhalb der nächsten 2 Stunden auftritt.",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Mettez sur 'oui', si pendant la lecture vous ne voulez pas être dérangé par les affichages des conflits de programmation. L'affichage est fait si les conflits on lieu dans le 2 heures à venir.",
    "",// TODO
    "Laittamalla tämän asetuksen päälle saat estettyä kuvaruutuviestit päällekkäisistä ajastimista toiston aikana. Kuvaruutuviesti näytetään kuitenkin aina, jos päällekkäinen ajastin on alle kahden tunnin kuluttua.",
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
    "Afficher les %d nouvelles émissions trouvées?",
    "",// TODO
    "Löydettiin %d uutta lähetystä! Näytetäänkö?",
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
    "%d nouvelle(s) émission(s)",
    "",// TODO
    "%d uutta lähetystä",
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
    "Erneut ankündigen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Annoncer à nouveau",
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
    "beim nächsten Update",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "lors de la prochaine mise à jour",
    "",// TODO
    "Seuraavassa päivityksessä",
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
    "recommencer à partir de",
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
    "Ankündigungsdetails: %s",
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
    "Pas de résultats! Recherche à nouveau avec %d de tolérence?",
    "",// TODO
    "Ei hakutuloksia! Yritetäänkö uudelleen toleranssilla %d?",
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

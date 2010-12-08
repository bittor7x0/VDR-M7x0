/*
 * channellists.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Channellists", // English
    "Kanallisten", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Liste des chaînes", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Manage your channellists", // English
    "Verwalte deine Kanallisten", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Organiser la liste des chaînes", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Hide main menu entry", // English
    "Hauptmenüeintrag verstecken", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Invisible dans le menu principale", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Action after switch", // English
    "Aktion nach dem Wechsel", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Menu", // English
    "Menü", // Deutsch
    "Meni", // Slovenski
    "Menu", // Italiano
    "Menu", // Nederlands
    "Menu", // Português
    "Menu", // Français
    "Meny", // Norsk
    "Valikko", // suomi
    "Menu", // Polski
    "Menú", // Español
    "Måíïı", // ÅëëçíéêÜ (Greek)
    "Meny", // Svenska
    "Meniu", // Romaneste
    "Menü", // Magyar
    "Menú", // Català
    "¼Õİî", // ÀãááÚØÙ (Russian)
    "Izbor", // Hrvatski (Croatian)
    "Menüü", // Eesti
    "Menu", // Dansk
    "Menu", // Èesky (Czech)
  },
  { "close", // English
    "schliessen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "none", // English
    "keine", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button Red", // English
    "Taste Rot", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Button rouge", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button Green", // English
    "Taste Grün", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Button vert", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Switch current channellist?", // English
    "Aktuelle Kanalliste wechseln?", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Changer la liste de chaînes actuelle?", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Script", // English
    "Skript", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Script", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Rename", // English
    "Umbenennen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Renommer", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Copy", // English
    "Kopieren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Copier", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Save", // English
    "Sichern", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Sauvegarder", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Name", // English
    "Name", // Deutsch
    "Ime", // Slovenski
    "Nome", // Italiano
    "Naam", // Nederlands
    "Nome",// Português
    "Nom", // Français
    "Navn", // Norsk
    "Nimi", // suomi
    "Nazwa", // Polski
    "Nombre", // Español
    "¼íïìá", // ÅëëçíéêÜ (Greek)
    "Namn", // Svenska
    "Nume", // Romaneste
    "Név", // Magyar
    "Nom", // Català
    "½Ğ×ÒĞİØÕ", // ÀãááÚØÙ (Russian)
    "Naziv", // Hrvatski (Croatian)
    "Nimi", // Eesti
    "Navn", // Dansk
    "Název", // Èesky (Czech)
  },
  { "Delete channellist?", // English
    "Kanalliste löschen?", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Effacer la liste de chaînes?", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Execute update script?", // English
    "Update Skript ausführen?", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Effectuer le script de mise à jours", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Waiting...", // English
    "Warte...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Patience...", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "An error occurs, see syslog...", // English
    "Ein Fehler ist aufgetreten, siehe syslog...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Une erreur se produit, voir syslog...", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Rename", // English
    "Umbenennen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Renommer", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Script", // English
    "Skript", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Script", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Copy", // English
    "Kopieren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Copier", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { "Button$Delete", // English
    "Löschen", // Deutsch
    "Izbri¹i", // Slovenski
    "Cancella", // Italiano
    "Verwijder", // Nederlands
    "Apagar", // Português
    "Supprimer", // Français
    "Slett", // Norsk
    "Poista", // suomi
    "Usuñ", // Polski
    "Borrar", // Español
    "ÄéáãñáöŞ", // ÅëëçíéêÜ (Greek)
    "Ta bort", // Svenska
    "ªterge", // Romaneste
    "Törölni", // Magyar
    "Esborrar", // Català
    "ÃÔĞÛØâì", // ÀãááÚØÙ (Russian)
    "Obri¹i", // Hrvatski (Croatian)
    "Kustuta", // Eesti
    "Slet", // Dansk
    "Smazat", // Èesky (Czech)
  },
  { "Button$Save", // English
    "Sichern", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "Sauvegarder", // Français
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
    "", // ÀãááÚØÙ (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // Èesky (Czech)
  },
  { NULL }
  };

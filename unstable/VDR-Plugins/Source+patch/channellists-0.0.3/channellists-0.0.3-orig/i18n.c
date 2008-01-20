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
  { "Manage your channellists", // English
    "Verwalte deine Kanallisten", // Deutsch
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
  { "Hide main menu entry", // English
    "Hauptmenüeintrag verstecken", // Deutsch
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
  { "Button Green", // English
    "Taste Grün", // Deutsch
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
  { "Switch current channellist?", // English
    "Aktuelle Kanalliste wechseln?", // Deutsch
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
  { "Rename", // English
    "Umbenennen", // Deutsch
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
  { "Copy", // English
    "Kopieren", // Deutsch
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
  { "Save", // English
    "Sichern", // Deutsch
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
  { "Name", // English
    "Name", // Deutsch
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
  { "Delete channellist?", // English
    "Kanalliste löschen?", // Deutsch
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
  { "Execute update script?", // English
    "Update Skript ausführen?", // Deutsch
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
  { "Waiting...", // English
    "Warte...", // Deutsch
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
  { "An error occurs, see syslog...", // English
    "Ein Fehler ist aufgetreten, siehe syslog...", // Deutsch
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
  { "Button$Rename", // English
    "Umbenennen", // Deutsch
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
  { "Button$Script", // English
    "Skript", // Deutsch
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
  { "Button$Copy", // English
    "Kopieren", // Deutsch
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
  { "Button$Delete", // English
    "Löschen", // Deutsch
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
  { "Button$Save", // English
    "Sichern", // Deutsch
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
  { NULL }
  };

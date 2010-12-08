/*
 * i18n.c Internationalization - part of cinebars.c, a plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Overlays \"cinebars\"",		// English
    "legt \"Kinobalken\" aufs Bild",	// Deutsch
    "",// Slovenski
    "Sovrapponi \"cinebars\"",		// Italiano
    "",// Nederlands
    "",// Português
    "",// Français
    "",// Norsk
    "",// suomi
    "",// Polski
    "",// Español
    "",// ÅëëçíéêÜ
    "",// Svenska
    "",// Romaneste
    "",// Magyar
    "",// Català
#if VDRVERSNUM >= 10300
    "" // ÀãááÚØÙ
#endif
  },
  { "Cinebars",		// English
    "Kinobalken",	// Deutsch
    "",// Slovenski
    "Cinebars",		// Italiano
    "",// Nederlands
    "",// Português
    "",// Français
    "",// Norsk
    "",// suomi
    "",// Polski
    "",// Español
    "",// ÅëëçíéêÜ
    "",// Svenska
    "",// Romaneste
    "",// Magyar
    "",// Català
#if VDRVERSNUM >= 10300
    "" // ÀãááÚØÙ
#endif
  },
  { "Hide mainmenu entry", 			// English
    "Hauptmenüeintrag verstecken", 		// Deutsch
    "", // Slovenski
    "Nascondi voce nel menu principale", 	// Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Piilota valinta päävalikosta", 		// suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10300
    ""  // TODO
#endif
  },
  { "Top Bar",
    "Balken Oben",
    "",// TODO
    "Barra superiore",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM >= 10300
    "" // TODO
#endif
  },
  { "Top BarHeight",
    "Balkenhöhe Oben",
    "",// TODO
    "Altezza barra superiore",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM >= 10300
    "" // TODO
#endif
  },
  { "Bottom Bar",
    "Balken Unten",
    "",// TODO
    "Barra inferiore",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM >= 10300
    "" // TODO
#endif
  },
  { "Bottom BarHeight",
    "Balkenhöhe Unten",
    "",// TODO
    "Altezza barra inferiore",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM >= 10300
    "" // TODO
#endif
  },
  { "StepHeight",
    "Schrittweite Höhe",
    "",// TODO
    "Altezza bordo",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM >= 10300
    "" // TODO
#endif
  },
  { "OSD-Mode",
    "OSD-Modus",
    "",// TODO
    "Modalità OSD",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
#if VDRVERSNUM >= 10300
    "" // TODO
#endif
  },
  { NULL }
  };

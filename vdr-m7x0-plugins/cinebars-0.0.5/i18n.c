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
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Français
    "",// Norsk
    "",// suomi
    "",// Polski
    "Superposición \"bandas de cine\"",	// Español
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
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Français
    "",// Norsk
    "",// suomi
    "",// Polski
    "Bandas de cine",	// Español
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
    "", 	// Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", 		// suomi
    "", // Polski
    "Ocultar entrada en menú principal",	// Español
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
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Banda superior",
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
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Altura banda superior",
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
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Banda inferior",
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
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Altura banda inferior",
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
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Incremento de altura",
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
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",
    "",// TODO
    "Modo OSD",
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

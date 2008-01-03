/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Overlays \"cinebars\"",		// English
    "legt \"Kinobalken\" aufs Bild",	// Deutsch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Fran�ais
    "",// Norsk
    "",// suomi
    "",// Polski
    "",// Espa�ol
    "",// ��������
    "",// Svenska
    "",// Romaneste
    "",// Magyar
    "",// Catal�
#if VDRVERSNUM >= 10300
    "" // �������
#endif
  },
  { "Cinebars",		// English
    "Kinobalken",	// Deutsch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Fran�ais
    "",// Norsk
    "",// suomi
    "",// Polski
    "",// Espa�ol
    "",// ��������
    "",// Svenska
    "",// Romaneste
    "",// Magyar
    "",// Catal�
#if VDRVERSNUM >= 10300
    "" // �������
#endif
  },
  { "Hide mainmenu entry", 		// English
    "Hauptmen�eintrag verstecken", 	// Deutsch
    "", // Slovenski
    "Nascondi voce men�", 		// Italiano
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "Piilota valinta p��valikosta", 	// suomi
    "", // Polski
    "", // Espa�ol
    "", // ��������
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
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
    "Balkenh�he Oben",
    "",// TODO
    "",
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
    "",
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
    "Balkenh�he Unten",
    "",// TODO
    "",
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
    "Schrittweite H�he",
    "",// TODO
    "",
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
    "",
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

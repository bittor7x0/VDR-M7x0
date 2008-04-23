 /*
 * autotimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.1 2004/04/24 10:08:10 hflor Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Show recording information",
    "Zeigt Informationen zu Aufzeichnungen",// Deutsch
    "",// TODO Slovenski
    "",// Italiano Sean Carlos 2005.01.16
    "",// TODO Nederlands
    "",// TODO Portugues
    "",// Francais Patrice Staudt 4.1.2005
    "",// TODO Norsk
    "",
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO �������� (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
    "",// TODO ������� (Russian)
    "",// TODO Hrvatski (Croatian)
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "This plugin has no info text!",
    "Dieses Plugin hat keinen Infotext!",
    "",// TODO
    "Questo plugin non ha ulteriori informazioni",// TODO
    "",// TODO
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
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "Hide main menu entry",
    "Hauptmen�eintrag verstecken",
    "",// TODO
    "Nascondi voce nel men� principale",// Italiano
    "",// TODO
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
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  // The allowed characters in strings:
  { " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " a�bcdefghijklmno�pqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " a�bcde��fghi�jklmnopqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",// TODO
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",// TODO
    " a�bc�de���fghi�jklmno�pqrstu��vwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",// TODO
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " a�bcde�fghi�jklmn�o�pqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " ��������������������������������0123456789-.#~abcdefghijklmnopqrstuvwxyz\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:",
    " a��bcdefghi�jklmnopqrs�t�uvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " a�bcde�fghi�jklmno��pqrstu��vwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " a�bc�de��fghi�jklmno�pqrstu�vwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz��������������������������������0123456789-.#~\\^$[]|()*+?{}/:",
    " abc��d�efghijklmnopqrs�tuvwxyz�0123456789-.#~\\^$[]|()*+?{}/:", // hrv
#if VDRVERSNUM && VDRVERSNUM >= 10313
    " abcdefghijklmnopqrs�z�tuvw����xy�0123456789-.#~\\^$[]|()*+?{}/:",
#if VDRVERSNUM >= 10316
    " abcdefghijklmnopqrstuvwxyz���0123456789-.#~\\^$[]|()*+?{}/:",
#endif
#endif
  },

  { NULL }
  };

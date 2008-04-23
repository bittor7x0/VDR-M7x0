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
    "",// TODO Español
    "",// TODO ÅëëçíéêÜ (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
    "",// TODO ÀãááÚØÙ (Russian)
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
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "Hide main menu entry",
    "Hauptmenüeintrag verstecken",
    "",// TODO
    "Nascondi voce nel menù principale",// Italiano
    "",// TODO
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
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  // The allowed characters in strings:
  { " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " aäbcdefghijklmnoöpqrstuüvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " aàbcdeéèfghiîjklmnopqrstuùvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",// TODO
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",// TODO
    " aàbcçdeéèêfghiîjklmnoôpqrstuùûvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",// TODO
    " abcdefghijklmnopqrstuvwxyzåäö0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " aábcdeéfghiíjklmnñoópqrstuúvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " áÜâãäåÝæçÞèéßêëìíîïüðñóòôõýö÷øùþ0123456789-.#~abcdefghijklmnopqrstuvwxyz\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyzåäö0123456789-.#~\\^$[]|()*+?{}/:",
    " aãâbcdefghiîjklmnopqrsºtþuvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " aábcdeéfghiíjklmnoóöpqrstuúüvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " aàbcçdeéèfghiíjklmnoòpqrstuúvwxyz0123456789-.#~\\^$[]|()*+?{}/:",
    " abcdefghijklmnopqrstuvwxyzÐÑÒÓÔÕñÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìîï0123456789-.#~\\^$[]|()*+?{}/:",
    " abcèædðefghijklmnopqrs¹tuvwxyz¾0123456789-.#~\\^$[]|()*+?{}/:", // hrv
#if VDRVERSNUM && VDRVERSNUM >= 10313
    " abcdefghijklmnopqrsðzþtuvwõäöüxyå0123456789-.#~\\^$[]|()*+?{}/:",
#if VDRVERSNUM >= 10316
    " abcdefghijklmnopqrstuvwxyzæøå0123456789-.#~\\^$[]|()*+?{}/:",
#endif
#endif
  },

  { NULL }
  };

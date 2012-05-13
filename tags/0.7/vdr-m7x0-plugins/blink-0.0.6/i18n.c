/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 */

#include "i18n.h"
const tI18nPhrase Phrases[] = {

  { "off",// English
    "aus", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "apagado",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "on",// English
    "an", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "encendido",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "blink",// English
    "blinken", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "parpadeo",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Led on mode",// English
    "Led status wenn Box angeschaltet", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "LED modo iniciado",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Led hotstandby mode",// English
    "Led status wen box in hotstanby", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "LED modo apagado r�pido",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Blink outer led for every running recording",// English
    "Leds blinken f�r jede laufende Aufnahmen", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Parpadeo por cada grabaci�n",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Blink recordings in hotstandby",// English
    "Leds blinken f�r Aufnahmen in Hotstanby", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Parpadeo grabaciones en apagado r�pido",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Control the LEDs of M7x0",// English
    "M7x0 Led Steuerung", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Controla los LEDs del M7x0",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "middle on",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "medio encendido",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "outer on",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "externo encendido",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Delay between led activation",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Retardo entre activaci�n LED",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "==> Led low value",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "==> Valor bajo LED",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "==> Led high value",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "==> Valor alto LED",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "==> Led delay",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "==> Retardo LED",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "==> Led on delay",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "==> Retardo LED iniciado",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Led rec mode",// English
    "", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "LED modo grabaci�n",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Yes",// English
    "Ja", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "S�",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "No",// English
    "Nein", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "No",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { NULL }
  };



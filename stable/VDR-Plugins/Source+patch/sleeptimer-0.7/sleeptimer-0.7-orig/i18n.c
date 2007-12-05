/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Not active",// English
    "Nicht aktiv", //Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "Ei aktiivinen",// Suomi
    "",// Polski
    "",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { "Disable Sleep-Timer in %d minutes",
		"Sleep-Timer in %d Minuten abbrechen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Peruuta %d minuutin ajastin?",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Abort Sleep-Timer?",
		"Sleep-Timer abbrechen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Keskeytä ajastin",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Activate Sleep-Timer at %i:%0.2i?",
		"Sleep-Timer um %i:%0.2i aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aktivoi ajastin %i:%0.2i?",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Activate Sleep-Timer in %d minutes?",
		"Sleep-Timer in %d Minuten aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aktivoi %d minuutin ajastin?",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Sleep-Timer",
		"Sleep-Timer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajastin",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Action",
		"Aktion",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Toiminto",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Going to sleep in about one minute",
		"Sleep-Timer in einer Minute",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajastin aktivoituu noin minuutin kuluttua",// Suomi
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Shutdown-Time",
	  "Shutdown-Zeit",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sammutusaika", // Suomi
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { "Shutdown-Minutes",
	  "Shutdown-Minuten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sammutus minuutteina",
    "",
    "",
    "",
    "",
    "",
    "",
  },
  { NULL }
  };


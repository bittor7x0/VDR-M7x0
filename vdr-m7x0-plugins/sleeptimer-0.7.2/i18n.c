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
    "No activo",// Español
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
    "Desactivar apagado automático en %d minutos",// Español
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
    "¿Cancelar apagado automático?",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Activate Sleep-Timer at %i:%02i?",
		"Sleep-Timer um %i:%02i aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aktivoi ajastin %i:%02i?",// Suomi
    "",// TODO
    "¿Activar apagado automático a las %i:%02i?",// Español
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
    "¿Activar apagado automático en %d minutos?",// Español
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
    "Apagado automático",// Español
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
    "Acción",// Español
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
  },
  { "Going to sleep in about %i minute",
		"Abschaltung in %i Minute[n]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajastin aktivoituu %i minuutin kuluttua",// Suomi
    "",// TODO
    "Apagado automático en %i minuto(s)",// Español
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
    "Apagar-Hora",// Español
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
    "Apagar-Minutos",// Español
    "",
    "",
    "",
    "",
  },
  { "Sleep-Timer for VDR",// English
    "Sleep-Timer für VDR",// Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Apagado automático para VDR",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { NULL }
  };


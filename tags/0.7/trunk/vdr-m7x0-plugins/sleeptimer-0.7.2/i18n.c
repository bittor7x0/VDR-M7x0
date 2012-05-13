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
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "Ei aktiivinen",// Suomi
    "",// Polski
    "No activo",// Espa�ol
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
    "Desactivar apagado autom�tico en %d minutos",// Espa�ol
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
    "Keskeyt� ajastin",// Suomi
    "",// TODO
    "�Cancelar apagado autom�tico?",// Espa�ol
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
    "�Activar apagado autom�tico a las %i:%02i?",// Espa�ol
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
    "�Activar apagado autom�tico en %d minutos?",// Espa�ol
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
    "Apagado autom�tico",// Espa�ol
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
    "Acci�n",// Espa�ol
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
    "Apagado autom�tico en %i minuto(s)",// Espa�ol
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
    "Apagar-Hora",// Espa�ol
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
    "Apagar-Minutos",// Espa�ol
    "",
    "",
    "",
    "",
  },
  { "Sleep-Timer for VDR",// English
    "Sleep-Timer f�r VDR",// Deutch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Portugu�s
    "",// Francais
    "",// Norsk
    "",// Suomi
    "",// Polski
    "Apagado autom�tico para VDR",// Espa�ol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
  },
  { NULL }
  };


/*
 * $Id: i18n.c,v 1.1.1.1 2005/02/17 05:45:52 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: i18n.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:52  sundin
 * initial import ver0.0.5
 *
 * Revision 1.2  2005/01/17 18:29:49  ksltuf
 * added finnish translations
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.4  2004/12/13 07:59:04  ksltuf
 * bugfix setup parsing; new option firstIsMonday
 *
 * Revision 1.3  2004/11/24 21:34:35  ksltuf
 * Added new translation for "Delete Event?"
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 */

#include "i18n.h"

const tI18nPhrase pimPhrases[] = {

  { "Calendar",			// English
    "Kalender",			// Deutsch
    "",				// Slovenski
    "",				// Italiano
    "",				// Nederlands
    "",				// Portugu�s
    "",				// Fran�ais
    "",				// Norsk
    "Kalenteri",		// suomi (Finnish)
    "",				// Polski
    "",				// Espa�ol
    "",				// Greek
    "",				// Svenska
    "",				// Rom�n�
    "",				// Magyar
    "",				// Catal�
#if VDRVERSNUM >= 10302
    "",				// Russian
#endif
#if VDRVERSNUM >= 10307
    "",				// Hrvatski
#endif
#if VDRVERSNUM >= 10313
    "",				// Eesti
#endif
#if VDRVERSNUM >= 10316
    "",				// Dansk
#endif
#if VDRVERSNUM >= 10342
    "",				// Czech
#endif
  },

  { "Simple Personal Information Manager",
    "Simple Personal Information Manager",
    "",
    "",
    "",
    "",
    "",
    "",
    "Kalenteri kuvaruutun�yt�lle",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "January",
    "Januar",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tammikuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "February",
    "Februar",
    "",
    "",
    "",
    "",
    "",
    "",
    "Helmikuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "March",
    "M�rz",
    "",
    "",
    "",
    "",
    "",
    "",
    "Maaliskuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "April",
    "April",
    "",
    "",
    "",
    "",
    "",
    "",
    "Huhtikuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "May",
    "Mai",
    "",
    "",
    "",
    "",
    "",
    "",
    "Toukokuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "June",
    "Juni",
    "",
    "",
    "",
    "",
    "",
    "",
    "Kes�kuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "July",
    "Juli",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hein�kuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "August",
    "August",
    "",
    "",
    "",
    "",
    "",
    "",
    "Elokuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "September",
    "September",
    "",
    "",
    "",
    "",
    "",
    "",
    "Syyskuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "October",
    "Oktober",
    "",
    "",
    "",
    "",
    "",
    "",
    "Lokakuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "November",
    "November",
    "",
    "",
    "",
    "",
    "",
    "",
    "Marraskuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "December",
    "Dezember",
    "",
    "",
    "",
    "",
    "",
    "",
    "Joulukuu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Today",
    "Heute",
    "",
    "",
    "",
    "",
    "",
    "",
    "T�n��n",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Tomorrow",
    "Morgen",
    "",
    "",
    "",
    "",
    "",
    "",
    "Huomisp�iv�",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Mo",
    "Mo",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ma",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Tu",
    "Di",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ti",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "We",
    "Mi",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ke",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Th",
    "Do",
    "",
    "",
    "",
    "",
    "",
    "",
    "To",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Fr",
    "Fr",
    "",
    "",
    "",
    "",
    "",
    "",
    "Pe",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Sa",
    "Sa",
    "",
    "",
    "",
    "",
    "",
    "",
    "La",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Su",
    "So",
    "",
    "",
    "",
    "",
    "",
    "",
    "Su",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Monday",
    "Montag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Maanantai",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Tuesday",
    "Dienstag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tiistai",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Wednesday",
    "Mittwoch",
    "",
    "",
    "",
    "",
    "",
    "",
    "Keskiviikko",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Thursday",
    "Donnerstag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Torstai",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Friday",
    "Freitag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Perjantai",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Saturday",
    "Samstag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Lauantai",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Sunday",
    "Sonntag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sunnuntai",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Events",
    "Termine",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tapahtumat",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Events for",
    "Termine f�r",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tapahtumat:",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Yearly",
    "J�hrlich",
    "",
    "",
    "",
    "",
    "",
    "",
    "vuosittainen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Fix Day",
    "Fester Tag",
    "",
    "",
    "",
    "",
    "",
    "",
    "kerran",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Weekly",
    "W�chentlich",
    "",
    "",
    "",
    "",
    "",
    "",
    "viikottainen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Edit Event",
    "Termin �ndern",
    "",
    "",
    "",
    "",
    "",
    "",
    "Muokkaa tapahtumaa",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Edit",
    "�ndern",
    "",
    "",
    "",
    "",
    "",
    "",
    "Muuttaa",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "New Event",
    "Neuer Termin",
    "",
    "",
    "",
    "",
    "",
    "",
    "Uusi tapahtuma",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "New",
    "Neu",
    "",
    "",
    "",
    "",
    "",
    "",
    "Uusi",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Event type",
    "Art des Termins",
    "",
    "",
    "",
    "",
    "",
    "",
    "Toistuva tapahtuma",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Day",
    "Tag",
    "",
    "",
    "",
    "",
    "",
    "",
    "P�iv�",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Month",
    "Monat",
    "",
    "",
    "",
    "",
    "",
    "",
    "Kuukausi",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Year",
    "Jahr",
    "",
    "",
    "",
    "",
    "",
    "",
    "Vuosi",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Day Of Week",
    "Wochentag",
    "",
    "",
    "",
    "",
    "",
    "",
    "Viikonp�iv� (",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Description",
    "Beschreibung",
    "",
    "",
    "",
    "",
    "",
    "",
    "Kuvaus",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "List",
    "Liste",
    "",
    "",
    "",
    "",
    "",
    "",
    "Listaa",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "OSD Offset hor.",
    "OSD Versatz hor.",
    "",
    "",
    "",
    "",
    "",
    "",
    "Vaakakeskitys",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "OSD Offset vert.",
    "OSD Versatz vert.",
    "",
    "",
    "",
    "",
    "",
    "",
    "Pystykeskitys",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Delete Event?",
    "Termin l�schen?",
    "",
    "",
    "",
    "",
    "",
    "",
    "Poistetaanko tapahtuma?",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Delete",
    "L�schen",
    "",
    "",
    "",
    "",
    "",
    "",
    "Poistaa",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Week starts with",
    "Woche beginnt mit",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ensimm�inen viikonp�iv�",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Reminder mode",
    "Erinnerung anzeigen",
    "",
    "",
    "",
    "",
    "",
    "",
    "Velkomiskirje",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "off",
    "nein",
    "",
    "",
    "",
    "",
    "",
    "",
    "ei",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "on",
    "ja",
    "",
    "",
    "",
    "",
    "",
    "",
    "kyll�",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "after 18 o'clock",
    "nach 18 Uhr",
    "",
    "",
    "",
    "",
    "",
    "",
    "j�ljess� 18 tunti",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Remind me",
    "Erinnerung anzeigen",
    "",
    "",
    "",
    "",
    "",
    "",
    "muistuttaa",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Remind days before",
    "Erinnerungstage zuvor",
    "",
    "",
    "",
    "",
    "",
    "",
    "muistuttaa p�iv� edell�",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Show calendar weeks",
    "Kalenderwoche anzeigen",
    "",
    "",
    "",
    "",
    "",
    "",
    "osoittaa kalenteri viikko",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "CW",
    "KW",
    "",
    "",
    "",
    "",
    "",
    "",
    "KV",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "--- Data invalid ! ---",
    "--- Daten sind fehlerhaft ! ---",
    "",
    "",
    "",
    "",
    "",
    "",
    "--- Data invalidi ! ---",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Preview months",
    "Monats-Vorschau",
    "",
    "",
    "",
    "",
    "",
    "",
    "Kuukausi pennakkoesitys",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Use invers event color",
    "Eventfarben tauschen",
    "",
    "",
    "",
    "",
    "",
    "",
    "Vaihtaa tapahtuma v�ri",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "at time",
    "Zeit",
    "",
    "",
    "",
    "",
    "",
    "",
    "eri aika",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "at time (daily)",
    "Zeit (t�glich)",
    "",
    "",
    "",
    "",
    "",
    "",
    "eri aika (p�ivitt�in)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Reminder time",
    "Erinnerungszeit",
    "",
    "",
    "",
    "",
    "",
    "",
    "Karhukirje aika",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Check user activity",
    "Benutzeraktivit�t testen",
    "",
    "",
    "",
    "",
    "",
    "",
    "K�ytt�j� aktiivisuus",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { "Remind delay (seconds)",
    "Erinnerung verz�gern (Sekunden)",
    "",
    "",
    "",
    "",
    "",
    "",
    "Karhukirje lyk�t� (hylkytavara)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
  },

  { NULL }

};


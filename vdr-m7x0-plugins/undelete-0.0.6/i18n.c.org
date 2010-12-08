/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 0.5 2005/11/17 21:05:05 hflor Exp $
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Deleted Recordings",
    "Gelöschte Aufnahmen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  // Debug_mode
  { "DeleteLine",
    "Zeile löschen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  // Debug_mode
  { "HelpKeys",
    "Hilfstasten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "  -m        --visible_in_mainmenu     Show the plugin in the mainmenu\n  -M        --hide_in_mainmenu        Hide the plugin in the mainmenu\n                                      You can start the Plugin in the setupmenu\n  -n Name   --mainmenu_name=Name      Select Name for entry in the mainmenu\n                                      (set also -m)\n  -c        --confirm_purge_rec       show confirmation for purge recording\n  -C        --not_confirm_purge_rec   purge recording without confirmation\n  -d        --confirm_purge_dir       show confirmation for purge directory\n  -D        --not_confirm_purge_dir   purge directory without confirmation\n  -e        --confirm_purge_all       show confirmation for purge all function\n  -E        --not_confirm_purge_all   purge all function without confirmation\n  -s        --confirm_salvage_rec     show confirmation for salvage recording\n  -S        --not_confirm_salvage_rec salvage recording without confirmation\n  -t        --confirm_salvage_dir     show confirmation for salvage directory\n  -T        --not_confirm_salvage_dir salvage directory without confirmation\n  -u        --confirm_salvage_all     show confirmation for salvage all function\n  -U        --not_confirm_salvage_all salvage all function without confirmation\n  -v        --verbose                 Enable more logging\n  -V        --noverbose               Disable more loggig\n            --nosetup_commandline     Hide the 'Preferr Command Line Parameter'\n                                      form setup-menu\n  @config-file                        Read also parameters from this file\n  note: recording means deleted recordings (there not show in VDR)",
    "  -m        --visible_in_mainmenu     Plugin wird im Hautmenü angezeigt\n  -M        --hide_in_mainmenu        kein Eintrag im Hauptmenü für das Plugin\n                                      es kann über das Setup-Menü gestartet werden\n  -n Name   --mainmenu_name=Name      Name für den Eintrag im Hauptmenü\n                                      (diese Option setzt auch -m)\n  -c        --confirm_purge_rec       Bestätigung für das Löschen von Aufnahmen\n  -C        --not_confirm_purge_rec   keine Bestätigung (löschen Aufnahmen)\n  -d        --confirm_purge_dir       Bestätigung beim Löschen von Verzeichnissen\n  -D        --not_confirm_purge_dir   keine Bestätigung (löschen Verzeichnissen)\n  -e        --confirm_purge_all       Bestätigung beim \"alles Löschen\"\n  -E        --not_confirm_purge_all   \"alles Löschen\" ohne Bestätigung\n  -s        --confirm_salvage_rec     Bestätigung Wiederherstellung von Aufnahmen\n  -S        --not_confirm_salvage_rec keine Best. Wiederherstellung von Aufnahmen\n  -t        --confirm_salvage_dir     Bestätigung Wiederherstellung von Verzei.\n  -T        --not_confirm_salvage_dir keine Best. Wiederherstellung von Verzei.\n  -u        --confirm_salvage_all     Bestätigung beim \"alles Wiederherstellen\"\n  -U        --not_confirm_salvage_all \"alles Wiederherstellen\" ohne Bestätigung\n  -v        --verbose                 erweiterte Protokollierung aktivieren\n  -V        --noverbose               keine erweiterte Protokollierung\n            --nosetup_commandline     Option 'Kommandozeile hat Vorrang'\n                                      im Setup-Menü verbergen\n  @config-file                        Komandozeilenoptionen auch aus der angegebenen\n                                      Datei lesen\n\n  Hinweis: Mit Aufnahmen sind hier die gelöschten Aufnahmen gemeint",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  // Debug_mode
  { "Purge",
    "Löschung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  // Debug_mode
  { "Salvage",
    "Wiederherstellung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "undelete for recordings",
    "Wiederherstellung gelöschter Aufnahmen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  // Debug_mode
  { "unknow",
    "unbekannt",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Blue (1)",
    "Blau (1)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Blue (2)",
    "Blau (2)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$bottom",
    "unten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Display keys",
    "Tastenanzeige",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Green (1)",
    "Grün (1)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Green (2)",
    "Grün (2)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$none",
    "ohne",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$open",
    "Öffnen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$open/summary",
    "Öffnen/Inhalt",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$purge",
    "löschen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$purge all",
    "alle löschen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Red (1)",
    "Rot (1)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Red (2)",
    "Rot (2)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$salvage",
    "wiederherst.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$salvage all",
    "alle wiederh.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$summary",
    "Inhalt",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$switch hotkeys to 1",
    "Tastenbel. 1",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$switch hotkeys to 2",
    "Tastenbel. 2",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$top",
    "oben",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$top and bottom",
    "oben & unten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User1",
    "Benutzer 1",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User2",
    "Benutzer 2",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User3",
    "Benutzer 3",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User4",
    "Benutzer 4",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User5",
    "Benutzer 5",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User6",
    "Benutzer 6",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User7",
    "Benutzer 7",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User8",
    "Benutzer 8",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$User9",
    "Benutzer 9",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Yellow (1)",
    "Gelb (1)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Choise$Yellow (2)",
    "Gelb (2)",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$<--1",
    "<--1",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$2-->",
    "2-->",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$disp. keys",
    "Tastenanzeige",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$please wait ...",
    "bitte warten ...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$prepare recording display...",
    "Listenanzeige wird vorbereitet...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$purge",
    "Löschen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$purge all",
    "alle Löschen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$Purge all records ...",
    "Alle Aufnahmen endgültig löschen ...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$Salavge all records ...",
    "Alle Aufnahmen wiederherstellen ...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$salvage",
    "Wiederherst.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Display$salvage all",
    "alle Wieder.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Error$Key for funtion %s and %s identical.",
    "Tasten von %s und %s sind gleich.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Error$Recording with the same name exists!",
    "Aufnahme mit selben Namen ist vorhanden!",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Help$key\tfunction",
    "Taste\tFunktion",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Help$numerickey",
    "Numerntaste",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Help$OK",
    "OK",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Menu$assigned keys",
    "Zugewiesene Tasten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Menu$Undelete recordings",
    "Aufnahmen wiederherstellen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Question$Purge all recordings?",
    "Alle Aufnahmen endgültig löschen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Question$Purge recording?",
    "Aufnahme endgültig löschen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Question$Purge recordings in diretory?",
    "Aufnahmen in Verzeichnis endgültig löschen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Question$Salvage all recordings?",
    "Alle Aufnahmen wiederherstellen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Question$Salvage recording?",
    "Aufnahme wiederherstellen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Question$Salvage recordings in directory?",
    "Aufnahmen in Verzeichnis wiederherstellen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  ** Spezial handled keys **",
    "  ** Spezialtasten **",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Change assigned functionkeys",
    "Ändern der Tastenbelegung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Confirm purge all deleted recordings",
    "Bestätigung alles löschen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Confirm purge deleted recordings",
    "Aufnahme löschen bestätigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Confirm purge directories",
    "Verzeichnisse löschen bestätigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Confirm salvage all deleted recordings",
    "Bestätigung alles wiederherst.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Confirm salvage deleted recordings",
    "Wiederherstellung Aufn. bestätigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Confirm salvage directories",
    "Wiederherstellung Verz. bestätigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  Display bottom purge ex lines",
    "  Anzahl Einträge für Darstellung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  Display bottom salvage ex lines",
    "  Anzahl Einträge für Darstellung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Display deleted recordings now",
    "Gelöschte Aufnahmen jetzt anzeigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  inward lines",
    "  Innenliegende Zeilen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  Key for function-%s",
    "  Taste für Funktion-%s",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  Name for Mainmenu",
    "  Eintragname im Hauptmenü",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  Numerickey select the entry and",
    "  Auswahl mit Nummern und",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$  OK-key function",
    "  Funktion der OK-Taste",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Preferr Command Line Parameter",
    "Kommandozeile hat Vorrang",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Purgeline position",
    "Löschzeilenposition",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Salvageline position",
    "Wiederherstellungszeilenposition",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Select",
    "Auswahl",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Verbose Log Mode",
    "Ausführliche Protokollierung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { "Setup$Visible in Mainmenu",
    "Im Hauptmenü sichtbar",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Português
    "",// TODO Français
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Español
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
  },
  { NULL }
  };

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
    "Gel�schte Aufnahmen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Enregistrements effac�s",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  // Debug_mode
  { "DeleteLine",
    "Zeile l�schen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Effacer ligne",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  // Debug_mode
  { "HelpKeys",
    "Hilfstasten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Touche d'aide",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "  -m        --visible_in_mainmenu     Show the plugin in the mainmenu\n  -M        --hide_in_mainmenu        Hide the plugin in the mainmenu\n                                      You can start the Plugin in the setupmenu\n  -n Name   --mainmenu_name=Name      Select Name for entry in the mainmenu\n                                      (set also -m)\n  -c        --confirm_purge_rec       show confirmation for purge recording\n  -C        --not_confirm_purge_rec   purge recording without confirmation\n  -d        --confirm_purge_dir       show confirmation for purge directory\n  -D        --not_confirm_purge_dir   purge directory without confirmation\n  -e        --confirm_purge_all       show confirmation for purge all function\n  -E        --not_confirm_purge_all   purge all function without confirmation\n  -s        --confirm_salvage_rec     show confirmation for salvage recording\n  -S        --not_confirm_salvage_rec salvage recording without confirmation\n  -t        --confirm_salvage_dir     show confirmation for salvage directory\n  -T        --not_confirm_salvage_dir salvage directory without confirmation\n  -u        --confirm_salvage_all     show confirmation for salvage all function\n  -U        --not_confirm_salvage_all salvage all function without confirmation\n  -v        --verbose                 Enable more logging\n  -V        --noverbose               Disable more loggig\n            --nosetup_commandline     Hide the 'Preferr Command Line Parameter'\n                                      form setup-menu\n  @config-file                        Read also parameters from this file\n  note: recording means deleted recordings (there not show in VDR)",
    "  -m        --visible_in_mainmenu     Plugin wird im Hautmen� angezeigt\n  -M        --hide_in_mainmenu        kein Eintrag im Hauptmen� f�r das Plugin\n                                      es kann �ber das Setup-Men� gestartet werden\n  -n Name   --mainmenu_name=Name      Name f�r den Eintrag im Hauptmen�\n                                      (diese Option setzt auch -m)\n  -c        --confirm_purge_rec       Best�tigung f�r das L�schen von Aufnahmen\n  -C        --not_confirm_purge_rec   keine Best�tigung (l�schen Aufnahmen)\n  -d        --confirm_purge_dir       Best�tigung beim L�schen von Verzeichnissen\n  -D        --not_confirm_purge_dir   keine Best�tigung (l�schen Verzeichnissen)\n  -e        --confirm_purge_all       Best�tigung beim \"alles L�schen\"\n  -E        --not_confirm_purge_all   \"alles L�schen\" ohne Best�tigung\n  -s        --confirm_salvage_rec     Best�tigung Wiederherstellung von Aufnahmen\n  -S        --not_confirm_salvage_rec keine Best. Wiederherstellung von Aufnahmen\n  -t        --confirm_salvage_dir     Best�tigung Wiederherstellung von Verzei.\n  -T        --not_confirm_salvage_dir keine Best. Wiederherstellung von Verzei.\n  -u        --confirm_salvage_all     Best�tigung beim \"alles Wiederherstellen\"\n  -U        --not_confirm_salvage_all \"alles Wiederherstellen\" ohne Best�tigung\n  -v        --verbose                 erweiterte Protokollierung aktivieren\n  -V        --noverbose               keine erweiterte Protokollierung\n            --nosetup_commandline     Option 'Kommandozeile hat Vorrang'\n                                      im Setup-Men� verbergen\n  @config-file                        Komandozeilenoptionen auch aus der angegebenen\n                                      Datei lesen\n\n  Hinweis: Mit Aufnahmen sind hier die gel�schten Aufnahmen gemeint",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  // Debug_mode
  { "Purge",
    "L�schung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Vider",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  // Debug_mode
  { "Salvage",
    "Wiederherstellung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "undelete for recordings",
    "Wiederherstellung gel�schter Aufnahmen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restauration d'enregistrement",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  // Debug_mode
  { "unknow",
    "unbekannt",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "inconnu",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$bottom",
    "unten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "en bas",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$Display keys",
    "Tastenanzeige",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Affiche des touches",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$none",
    "ohne",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "sans",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$open",
    "�ffnen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "ouvrir",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$open/summary",
    "�ffnen/Inhalt",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Ouvrir/contenu",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$purge",
    "l�schen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "effacer",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$purge all",
    "alle l�schen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "effacer tout",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$salvage",
    "wiederherst.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "restaurer",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$salvage all",
    "alle wiederh.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "restaurer tout",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$summary",
    "Inhalt",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Contenu",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$switch hotkeys to 1",
    "Tastenbel. 1",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Choix touche 1",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$switch hotkeys to 2",
    "Tastenbel. 2",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Choix touche 2",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$top",
    "oben",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "en haut",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$top and bottom",
    "oben & unten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "en haut & en bas",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Choise$User",
    "Benutzer",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Utilisateur",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$<--1",
    "<--1",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "<--1",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$2-->",
    "2-->",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "2-->",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$disp. keys",
    "Tastenanzeige",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Afficher les touches",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$please wait ...",
    "bitte warten ...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "attendez, merci",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$prepare recording display...",
    "Listenanzeige wird vorbereitet...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "L'affichage liste en pr�paration...",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$purge",
    "L�schen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Effacer",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$purge all",
    "alle L�schen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "effacer tout",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$Purge all records ...",
    "Alle Aufnahmen endg�ltig l�schen ...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Effacer tous les enregistrement d�finitivement ...",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$Salavge all records ...",
    "Alle Aufnahmen wiederherstellen ...",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer tout les enregistrements ...",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$salvage",
    "Wiederherst.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Display$salvage all",
    "alle Wieder.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "restaurer tout",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Error$Key for funtion %s and %s identical.",
    "Tasten von %s und %s sind gleich.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Touche de %s et %s sont les m�mes.",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Error$Recording with the same name exists!",
    "Aufnahme mit selben Namen ist vorhanden!",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Des enregistrements avec le m�me nom existent!",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Help$key\tfunction",
    "Taste\tFunktion",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Touch\tFonction",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Help$numerickey",
    "Numerntaste",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Touch num�rique",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Help$OK",
    "OK",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "OK",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Menu$assigned keys",
    "Zugewiesene Tasten",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Touches assign�es",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Menu$Undelete recordings",
    "Aufnahmen wiederherstellen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer enregistrement",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Question$Purge all recordings?",
    "Alle Aufnahmen endg�ltig l�schen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Question$Purge recording?",
    "Aufnahme endg�ltig l�schen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Question$Purge recordings in diretory?",
    "Aufnahmen in Verzeichnis endg�ltig l�schen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Question$Salvage all recordings?",
    "Alle Aufnahmen wiederherstellen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer tout les enregistrements?",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Question$Salvage recording?",
    "Aufnahme wiederherstellen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer enregistrement?",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Question$Salvage recordings in directory?",
    "Aufnahmen in Verzeichnis wiederherstellen?",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Restaurer l'enregistrement dans dossier?",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  ** Spezial handled keys **",
    "  ** Spezialtasten **",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  ** Touche sp�ciale **",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Change assigned functionkeys",
    "�ndern der Tastenbelegung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Changer la configuration des touches",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Confirm purge all deleted recordings",
    "Best�tigung alles l�schen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Confirmation effacer tout",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Confirm purge deleted recordings",
    "Aufnahme l�schen best�tigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Confirmer effacer enregistrement",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Confirm purge directories",
    "Verzeichnisse l�schen best�tigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Confirmer effacer dossier",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Confirm salvage all deleted recordings",
    "Best�tigung alles wiederherst.",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Confirmer restaurer tout",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Confirm salvage deleted recordings",
    "Wiederherstellung Aufn. best�tigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Confirmer restaurer enregistrement",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Confirm salvage directories",
    "Wiederherstellung Verz. best�tigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Confirmer restaurer dossier",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  Display bottom purge ex lines",
    "  Anzahl Eintr�ge f�r Darstellung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Nombre de ligne d'affichage d'effacement",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  Display bottom salvage ex lines",
    "  Anzahl Eintr�ge f�r Darstellung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Nombre de ligne d'affichage",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Display deleted recordings now",
    "Gel�schte Aufnahmen jetzt anzeigen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Afficher les enregistrements effac�s",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  inward lines",
    "  Innenliegende Zeilen",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Ligne interrieur",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  Key for function-%s",
    "  Taste f�r Funktion-%s",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Touch pour fonction-%s",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  Name for Mainmenu",
    "  Eintragname im Hauptmen�",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Nom afficher pour le menu principal",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  Numerickey select the entry and",
    "  Auswahl mit Nummern und",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Choix avec num�ro et",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$  OK-key function",
    "  Funktion der OK-Taste",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "  Fonction touch OK",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Preferr Command Line Parameter",
    "Kommandozeile hat Vorrang",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "La ligne de commande a priorit�",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Purgeline position",
    "L�schzeilenposition",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Position de la ligne d'�ffacement",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Salvageline position",
    "Wiederherstellungszeilenposition",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Position de la ligne de restauration",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Select",
    "Auswahl",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Choix",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Verbose Log Mode",
    "Ausf�hrliche Protokollierung",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Protokolle mode complet",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Setup$Visible in Mainmenu",
    "Im Hauptmen� sichtbar",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "Visible dans le menu principal",
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Error$error while read last filenumber",
    "Fehler beim lesen der letzten Dateinummer",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Error$error accessing vdrfile %03d",
    "Fehler beim lesen der Videodatei %03d",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { "Error$error accessing indexfile",
    "Fehler beim lesen der Indexdatei",
    "",// TODO Slovenski
    "",// TODO Italiano
    "",// TODO Nederlands
    "",// TODO Portugu�s
    "",// TODO Fran�ais
    "",// TODO Norsk
    "",// TODO suomi
    "",// TODO Polski
    "",// TODO Espa�ol
    "",// TODO Ellinika
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Catal�
  },
  { NULL }
  };

/*
 * i18n.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

//***************************************************************************
// Includes
//***************************************************************************

#include "i18n.h"
#include <stdio.h>

//***************************************************************************
// Constants
//***************************************************************************

const tI18nPhrase Phrases[] =
{
   {
		"Childlock plugin",       // English
		"Kindersicherung",        // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection Parentale",   // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekzár", // Magyar
		"", // Catala
		"±ÛŞÚØàŞÒÚĞ", // Russian
		"", // Croatian
	},
   {
		"Pin Code: ", // English
		"Pin Eingabe: ", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code pin", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Pin kód", // Magyar
		"", // Catala
		"¿Øİ ÚŞÔ", // Russian
		"", // Croatian
	},
   {
		"Skip protected channel silent", // English
		"Gesperrte Kanäle ohne Meldung überspringen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Les chaînes protégés seront ignorées sans infos", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Zárolt csatornákat nem megjeleníteni", // Magyar
		"", // Catala
		"¿àŞßãáâØâì ×ĞéØéÕİİëÙ ÚĞİĞÛ âØåŞ", // Russian
		"", // Croatian
	},
   {
		"Pin inactivity time [min]", // English
		"Sperre aktivieren nach Inaktivität [min]", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection après une inactivitées de [min]", // Français
		"", // Norsk
		"", // Suomi
        "", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Zár aktiválása [perc] inaktivitás után", // Magyar
		"", // Catala
		"ÂĞÙÜÕà İÕĞÚâØÒİŞáâØ ßØİ [ÜØİãâ]", // Russian
		"", // Croatian
	},
   {
		"Ignoring invalid pin", // English
		"Ungültiger code", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code est fau", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Érvénytelen pin kód", // Magyar
		"", // Catala
		"¸ÓİŞàØàŞÒĞâì İÕÒÕàİëÙ ßØİ", // Russian
		"", // Croatian
	},
   {
		"Please enable pin protection first", // English
		"Bitte zuerst Kindersicherung freischalten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entrée le code de protection", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Kérem oldja fel a gyerekzárat", // Magyar
		"", // Catala
		"¿ŞÖĞÛãÙáâĞ ÒÒÕÔØâÕ áİĞçĞÛĞ ßØİ ÚŞÔ", // Russian
		"", // Croatian
	},
   {
		"Recording protected, enter pin code first", // English
		"Aufnahme gesperrt, bitte Kindersicherung freischalten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entrée le code de protection", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Kérem oldja fel a gyerekzárat", // Magyar
		"", // Catala
		"¿ŞÖĞÛãÙáâĞ ÒÒÕÔØâÕ áİĞçĞÛĞ ßØİ ÚŞÔ", // Russian
		"", // Croatian
	},
   {
		"already protected", // English
		"bereits gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Est déjà protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Korábban már zárolt", // Magyar
		"", // Catala
		"ãÖÕ ×ĞÑÛŞÚØàŞÒĞİŞ", // Russian
		"", // Croatian
	},
   {
		"added to protection list", // English
		"hinzugefügt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"ajouter", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Zároltakhoz hozzáadva", // Magyar
		"", // Catala
		"ÔŞÑĞÒÛÕİŞ Ò áßØáŞÚ ÑÛŞÚØàŞÒÚØ", // Russian
		"", // Croatian
	},
   {
		"Recording protected", // English
		"Aufnahme gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Engeristrement protégés, d'abord mettre le code parental", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Felvételhez fel kell oldja a gyerekzárat", // Magyar
		"", // Catala
		"·ĞßØáì ×ĞÑÛŞÚØàŞÒĞİĞ, ÒÒÕÔØâÕ áİĞçĞÛĞ ßØİ ÚŞÔ", // Russian
		"", // Croatian
	},
   {
		"channel is protected", // English
		"Kanal gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Chaînes protégée", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Csatorna zárolva", // Magyar
		"", // Catala
		"ÚĞİĞÛ ×ĞÑÛŞÚØàŞÒĞİ", // Russian
		"", // Croatian
	},
   {
		"broadcast is protected", // English
		"Sendung gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Emission protégée", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Adás zárolva", // Magyar
		"", // Catala
		"àĞÔØŞßÕàÕÔĞçĞ ×ĞÑÛŞÚØàŞÒĞİĞ", // Russian
		"", // Croatian
	},
   {
		"Activated pin protection", // English
		"Kindersicherung aktiviert", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Activé la protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekzár aktiválva", // Magyar
		"", // Catala
		"°ÚâØÒØàŞÒĞâì ÑÛŞÚØàŞÒÚã", // Russian
		"", // Croatian
	},
   {
		"Childlock",       // English
		"Kindersicherung", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekzár", // Magyar
		"", // Catala
		"±ÛŞÚØàŞÒÚĞ", // Russian
		"", // Croatian
	},
   {
		"Delete", // English
		"Löschen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Effacer", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Törlés", // Magyar
		"", // Catala
		"ÃÔĞÛØâì", // Russian
		"", // Croatian
	},
   {
		"Add", // English
		"Hinzufügen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ajouter", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Hozzáadás", // Magyar
		"", // Catala
		"´ŞÑĞÒØâì", // Russian
		"", // Croatian
	},
   {
		"Protected channels", // English
		"Gesperrte Kanäle", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Chaînes protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Lezárt csatornák", // Magyar
		"", // Catala
		"·ĞÑÛŞÚØàŞÒĞİİëÕ ÚĞİĞÛë", // Russian
		"", // Croatian
	},
   {
		"Protected broadcasts", // English
		"Gesperrte Sendungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Emissions protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Lezárt adások", // Magyar
		"", // Catala
		"·ĞÑÛŞÚØàŞÒĞİİëÕ àĞÔØŞßÕàÕÔĞçØ", // Russian
		"", // Croatian
	},
   {
		"Plugin protections", // English
		"Sperren / Entsperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Plugin sous protection", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Add current channel to protection list", // English
		"Aktuellen Kanal sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ajouter la chaîne actuelle à la protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Aktuális csatorna zároltakhoz adása", // Magyar
		"", // Catala
		"´ŞÑĞÒØâì âÕÚãéØÙ ÚĞİĞÛ Ò ÑÛŞÚ.  áßØáŞÚ", // Russian
		"", // Croatian
	},
   {
		"Add current broadcast to protection list", // English
		"Aktuelle Sendung sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ajouter l'émission actuelle dans la liste de protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Aktuális adás zároltakhoz adása", // Magyar
		"", // Catala
		"´ŞÑĞÒØâì âÕÚãéãî àĞÔØŞßÕàÕÔĞçã Ò ÑÛŞÚ. áßØáŞÚ", // Russian
		"", // Croatian
	},
   {
		"Edit channel locks", // English
		"Gesperrte Kanäle bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Editer la liste de chaînes", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Zárolt csatornák szerkesztése", // Magyar
		"", // Catala
		"ÀÕÔĞÚâØàŞÒĞâì ÑÛŞÚØàŞÒÚã ÚĞİĞÛĞ", // Russian
		"", // Croatian
	},
   {
		"Edit broadcast locks", // English
		"Gesperrte Sendungen bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Editer les émissions protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Zárolt adások szerkesztése", // Magyar
		"", // Catala
		"ÀÕÔĞÚâØàŞÒĞâì ÑÛŞÚØàŞÒÚã àĞÔØŞßÕàÕÔĞçØ", // Russian
		"", // Croatian
	},
   {
		"Edit plugin locks", // English
		"Systemeinstellungen bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"editer la protection de plugin", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Edit menu locks", // English
		"Menüsperren bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Lock setup", // English
		"Setup sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Unlock setup", // English
		"Setup entsperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Activate childlock", // English
		"Kindersicherung aktivieren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection parentales activée", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekzár aktiválása", // Magyar
		"", // Catala
		"°ÚâØÒØàŞÒĞâì ÑÛŞÚØàŞÒÚã", // Russian
		"", // Croatian
	},
   {
		"Remove entry?", // English
		"Eintrag entfernen?", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Enlever?", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Törlése?", // Magyar
		"", // Catala
		"ÚĞİĞÛ?", // Russian
		"", // Croatian
	},
   {
		"Edit", // English
		"Bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Editer", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"ÀÕÔĞÚâØàŞÒĞâì", // Russian
		"", // Croatian
	},
   {
		"Channel", // English
		"Kanal", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Chaîne", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Csatorna", // Magyar
		"", // Catala
		"ºĞİĞÛ", // Russian
		"", // Croatian
	},
   {
		"Lock active", // English
		"Sperre aktiv", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection active", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Zár aktiválva", // Magyar
		"", // Catala
		"±ÛŞÚØàŞÒÚĞ ĞÚâØÒİĞ", // Russian
		"", // Croatian
	},
   {
		"already in list", // English
		"bereits in der Liste", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
 		"Toujours dans la liste", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"mar a zaroltak között", // Magyar
		"", // Catala
		"ãÖÕ Ò áßØáÚÕ", // Russian
		"", // Croatian
	},
   {
		"Protected channels", // English
		"Gesperrte Kanäle", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protéger chaîne", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Védett csatornák", // Magyar
		"", // Catala
		"·ĞÑÛŞÚØàŞÒĞİİëÕ ÚĞİĞÛë", // Russian
		"", // Croatian
	},
   {
		"Protected broadcasts", // English
		"Gesperrte Sendungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Proteger l'émission", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Védett adások", // Magyar
		"", // Catala
		"·ĞÑÛŞÚØàŞÒĞİİëÕ àĞÔØŞßÕàÕÔĞçØ", // Russian
		"", // Croatian
	},
   {
		"regular expression", // English
		"regulärer Ausdruck", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Expression régulière", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"reguláris kifejezés", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Keyword", // English
		"Suchbegriff", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Expression de recherche", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Kulcsszó", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"includes", // English
		"enthält", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"incluent", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"tartalmazza", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"exact", // English
		"entspricht", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"exactement", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"megeggyezik", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"start with", // English
		"beginnt mit", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Title", // English
		"Titel", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Titre", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Cím", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"always", // English
		"immer", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"toujours", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Mindig", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"intelligent", // English
		"intelligent", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"intelligent", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"inteligens", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"never", // English
		"nie", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"jamais", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"soha", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Autoprotection of recordings", // English
		"Automatischer Schutz der Aufnahmen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection automatique des enregistrement", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Automatikus védelem felvétel ellen", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"is protected", // English
		"ist gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"est protégé", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Open menu after pin request", // English
		"Menü nach Eingabe des Pin öffnen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ouvrir le menu après la rentré de la cléed", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Recording was already protected!", // English
		"Aufnahme bereits gesperrt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"L'enregeristrement est en protection parentale!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Recording now protected!", // English
		"Sperre aktiviert!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"L'enregeristrement est en protection parentale maintenant!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Recording was not protected!", // English
		"Aufnahme ist nicht gesperrt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"L'enregeristrement n'avait pas de protection parentale!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Removed protection of recording!", // English
		"Sperre entfernt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"La protection parentale est enlever!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Please setup a pin code first!", // English
		"Bitte konfigurieren zuerst einen Pin Code!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entrer le code de protection d'abord!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Please enter pin code first", // English
		"Bitte zuerst Pin Code eingeben", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entré d'abord le code", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"locked", // English
		"gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"protégé", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"unlocked", // English
		"entsperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"libéré", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"On/Off", // English
		"An/Aus", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Enclencher/déclencher", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Invalid Code !", // English
		"Falsche Eingabe !", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code est faux!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Code accepted", // English
		"Eingabe angenommen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code est accepté", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Setup your pin code: ", // English
		"Bitte Pin Code festlegen: ", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Introduisez le code d'accès:", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Pin code successfully setup", // English
		"Pin Code erfolgreich festgelegt.", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code d'accès est accepté", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Menu protections", // English
		"Menü sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protégier des menus", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Hide protected menus", // English
		"Gesperrte Menüs verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Menu protégés sont invisibles", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Hide protected plugins", // English
		"Gesperrte Plugins verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Plugins protégés sont invisibles", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Hide protected recordings", // English
		"Gesperrte Aufnahmen verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Enregistrements protégés sont invisibles", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
      NULL
   }
};

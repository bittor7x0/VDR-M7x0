/*
 * i18n.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Italian translations provided by Gringo <vdr-italian@tiscali.it>
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

const tI18nPhrase Phrases[] = {
   {  "Childlock plugin",       // English
		"Kindersicherung",        // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection Parentale",   // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloqueo niños", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Pin Code: ", // English
		"Pin Eingabe: ", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code pin", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Código PIN: ", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Skip protected channel silent", // English
		"Gesperrte Kanäle ohne Meldung überspringen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Les chaînes protégés seront ignorées sans infos", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Saltar canal protegido silenciosamente", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Pin inactivity time [min]", // English
		"Sperre aktivieren nach Inaktivität [min]", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection après une inactivitées de [min]", // Français
		"", // Norsk
		"", // Suomi
        "", // Polski
		"Tiempo inactividad PIN [min]", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Ignoring invalid pin", // English
		"Ungültiger code", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code est fau", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ignorando PIN no válido", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Please enable pin protection first", // English
		"Bitte zuerst Kindersicherung freischalten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entrée le code de protection", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Primero active la protección PIN", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Recording protected, enter pin code first", // English
		"Aufnahme gesperrt, bitte Kindersicherung freischalten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entrée le code de protection", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Grabación protegida, primero introduzca el PIN", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"already protected", // English
		"bereits gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Est déjà protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"ya protegido", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"added to protection list", // English
		"hinzugefügt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"ajouter", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"añadido a la lista de protección", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Recording protected", // English
		"Aufnahme gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Engeristrement protégés, d'abord mettre le code parental", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Grabación protegida", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"channel is protected", // English
		"Kanal gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Chaînes protégée", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"canal protegido", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"broadcast is protected", // English
		"Sendung gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Emission protégée", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"emisión protegida", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Activated pin protection", // English
		"Kindersicherung aktiviert", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Activé la protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Protección PIN activada", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Childlock",       // English
		"Kindersicherung", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloqueo para niños", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Delete", // English
		"Löschen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Effacer", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Borrar", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Add", // English
		"Hinzufügen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ajouter", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Añadir", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Protected channels", // English
		"Gesperrte Kanäle", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Chaînes protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Canales protegidos", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Protected broadcasts", // English
		"Gesperrte Sendungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Emissions protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Emisiones protegidas", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Plugin protections", // English
		"Sperren / Entsperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Plugin sous protection", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Plugins protegidos", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Add current channel to protection list", // English
		"Aktuellen Kanal sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ajouter la chaîne actuelle à la protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Añadir canal actual a la lista de protección", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Add current broadcast to protection list", // English
		"Aktuelle Sendung sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ajouter l'émission actuelle dans la liste de protection parentale", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Añadir emisión actual a la lista de protección", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit channel locks", // English
		"Gesperrte Kanäle bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Editer la liste de chaînes", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar canales bloqueados", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit broadcast locks", // English
		"Gesperrte Sendungen bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Editer les émissions protégés", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar emisiones bloqueadas", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit plugin locks", // English
		"Systemeinstellungen bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"editer la protection de plugin", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar plugins bloqueados", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit menu locks", // English
		"Menüsperren bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar menús bloqueados", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Lock setup", // English
		"Setup sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloquear configuración", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Unlock setup", // English
		"Setup entsperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Desbloquear configuración", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Activate childlock", // English
		"Kindersicherung aktivieren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection parentales activée", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Activar bloqueo para niños", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Remove entry?", // English
		"Eintrag entfernen?", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Enlever?", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¿Borrar entrada?", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit", // English
		"Bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Editer", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Lock active", // English
		"Sperre aktiv", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection active", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloqueo activo", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"already in list", // English
		"bereits in der Liste", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
 		"Toujours dans la liste", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"ya está en la lista", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Protected channels", // English
		"Gesperrte Kanäle", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protéger chaîne", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Canales protegidos", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Protected broadcasts", // English
		"Gesperrte Sendungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Proteger l'émission", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Emisiones protegidas", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"regular expression", // English
		"regulärer Ausdruck", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Expression régulière", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"expresión regular", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Keyword", // English
		"Suchbegriff", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Expression de recherche", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Palabra clave", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"includes", // English
		"enthält", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"incluent", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"incluye", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"exact", // English
		"entspricht", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"exactement", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"exacto", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"start with", // English
		"beginnt mit", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"empieza con", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Title", // English
		"Titel", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Titre", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Título", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"always", // English
		"immer", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"toujours", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"siempre", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"intelligent", // English
		"intelligent", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"intelligent", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"inteligente", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"never", // English
		"nie", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"jamais", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"nunca", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Autoprotection of recordings", // English
		"Automatischer Schutz der Aufnahmen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protection automatique des enregistrement", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Autoprotección de grabaciones", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"is protected", // English
		"ist gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"est protégé", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"está protegido", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Open menu after pin request", // English
		"Menü nach Eingabe des Pin öffnen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Ouvrir le menu après la rentré de la cléed", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Abrir menú despues de pedir el PIN", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Recording was already protected!", // English
		"Aufnahme bereits gesperrt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"L'enregeristrement est en protection parentale!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¡La grabación ya estaba protegida!", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Recording now protected!", // English
		"Sperre aktiviert!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"L'enregeristrement est en protection parentale maintenant!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¡Ahora la grabación está protegida!", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Recording was not protected!", // English
		"Aufnahme ist nicht gesperrt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"L'enregeristrement n'avait pas de protection parentale!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¡La grabación no estaba protegida!", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Removed protection of recording!", // English
		"Sperre entfernt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"La protection parentale est enlever!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¡Protección de grabación borrada!", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Please setup a pin code first!", // English
		"Bitte konfigurieren zuerst einen Pin Code!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entrer le code de protection d'abord!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¡Establezca primero un código PIN!", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Please enter pin code first", // English
		"Bitte zuerst Pin Code eingeben", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Entré d'abord le code", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Introduzca primero el código PIN", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"locked", // English
		"gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"protégé", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"bloqueado", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"unlocked", // English
		"entsperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"libéré", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"desbloqueado", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"On/Off", // English
		"An/Aus", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Enclencher/déclencher", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"On/Off", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Invalid Code !", // English
		"Falsche Eingabe !", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code est faux!", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"¡Código no válido!", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Code accepted", // English
		"Eingabe angenommen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code est accepté", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Código aceptado", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Setup your pin code: ", // English
		"Bitte Pin Code festlegen: ", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Introduisez le code d'accès:", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Establezca su código PIN: ", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Pin code successfully setup", // English
		"Pin Code erfolgreich festgelegt.", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Le code d'accès est accepté", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Código PIN configurado correctamente", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Menu protections", // English
		"Menü sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Protégier des menus", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Protecciones de menú", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Hide protected menus", // English
		"Gesperrte Menüs verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Menu protégés sont invisibles", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ocultar menús protegidos", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Hide protected plugins", // English
		"Gesperrte Plugins verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Plugins protégés sont invisibles", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ocultar plugins protegidos", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Hide protected recordings", // English
		"Gesperrte Aufnahmen verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"Enregistrements protégés sont invisibles", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ocultar grabaciones protegidas", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Search mode", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Modo de búsqueda", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Setup", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Configuración", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Name", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Nombre", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Start", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Inicio", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Stop", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Final", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Pin code", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Código PIN", // Español
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Log Level [0-5]", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Nivel de registro [0-5]", // Español
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

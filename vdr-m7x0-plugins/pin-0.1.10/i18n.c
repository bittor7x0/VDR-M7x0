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
		"", // Portugu�s
		"Protection Parentale",   // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloqueo ni�os", // Espa�ol
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
		"", // Portugu�s
		"Le code pin", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"C�digo PIN: ", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Skip protected channel silent", // English
		"Gesperrte Kan�le ohne Meldung �berspringen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Les cha�nes prot�g�s seront ignor�es sans infos", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Saltar canal protegido silenciosamente", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Pin inactivity time [min]", // English
		"Sperre aktivieren nach Inaktivit�t [min]", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection apr�s une inactivit�es de [min]", // Fran�ais
		"", // Norsk
		"", // Suomi
        "", // Polski
		"Tiempo inactividad PIN [min]", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {  "Ignoring invalid pin", // English
		"Ung�ltiger code", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Le code est fau", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ignorando PIN no v�lido", // Espa�ol
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
		"", // Portugu�s
		"Entr�e le code de protection", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Primero active la protecci�n PIN", // Espa�ol
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
		"", // Portugu�s
		"Entr�e le code de protection", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Grabaci�n protegida, primero introduzca el PIN", // Espa�ol
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
		"", // Portugu�s
		"Est d�j� prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"ya protegido", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"added to protection list", // English
		"hinzugef�gt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"ajouter", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"a�adido a la lista de protecci�n", // Espa�ol
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
		"", // Portugu�s
		"Engeristrement prot�g�s, d'abord mettre le code parental", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Grabaci�n protegida", // Espa�ol
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
		"", // Portugu�s
		"Cha�nes prot�g�e", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"canal protegido", // Espa�ol
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
		"", // Portugu�s
		"Emission prot�g�e", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"emisi�n protegida", // Espa�ol
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
		"", // Portugu�s
		"Activ� la protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Protecci�n PIN activada", // Espa�ol
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
		"", // Portugu�s
		"Protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloqueo para ni�os", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Delete", // English
		"L�schen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Effacer", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Borrar", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Add", // English
		"Hinzuf�gen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Ajouter", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"A�adir", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Protected channels", // English
		"Gesperrte Kan�le", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Cha�nes prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Canales protegidos", // Espa�ol
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
		"", // Portugu�s
		"Emissions prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Emisiones protegidas", // Espa�ol
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
		"", // Portugu�s
		"Plugin sous protection", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Plugins protegidos", // Espa�ol
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
		"", // Portugu�s
		"Ajouter la cha�ne actuelle � la protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"A�adir canal actual a la lista de protecci�n", // Espa�ol
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
		"", // Portugu�s
		"Ajouter l'�mission actuelle dans la liste de protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"A�adir emisi�n actual a la lista de protecci�n", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit channel locks", // English
		"Gesperrte Kan�le bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Editer la liste de cha�nes", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar canales bloqueados", // Espa�ol
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
		"", // Portugu�s
		"Editer les �missions prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar emisiones bloqueadas", // Espa�ol
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
		"", // Portugu�s
		"editer la protection de plugin", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar plugins bloqueados", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Edit menu locks", // English
		"Men�sperren bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar men�s bloqueados", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloquear configuraci�n", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Desbloquear configuraci�n", // Espa�ol
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
		"", // Portugu�s
		"Protection parentales activ�e", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Activar bloqueo para ni�os", // Espa�ol
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
		"", // Portugu�s
		"Enlever?", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�Borrar entrada?", // Espa�ol
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
		"", // Portugu�s
		"Editer", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Editar", // Espa�ol
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
		"", // Portugu�s
		"Protection active", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Bloqueo activo", // Espa�ol
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
		"", // Portugu�s
 		"Toujours dans la liste", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"ya est� en la lista", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Protected channels", // English
		"Gesperrte Kan�le", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Prot�ger cha�ne", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Canales protegidos", // Espa�ol
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
		"", // Portugu�s
		"Proteger l'�mission", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Emisiones protegidas", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"regular expression", // English
		"regul�rer Ausdruck", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Expression r�guli�re", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"expresi�n regular", // Espa�ol
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
		"", // Portugu�s
		"Expression de recherche", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Palabra clave", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"includes", // English
		"enth�lt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"incluent", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"incluye", // Espa�ol
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
		"", // Portugu�s
		"exactement", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"exacto", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"empieza con", // Espa�ol
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
		"", // Portugu�s
		"Titre", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"T�tulo", // Espa�ol
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
		"", // Portugu�s
		"toujours", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"siempre", // Espa�ol
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
		"", // Portugu�s
		"intelligent", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"inteligente", // Espa�ol
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
		"", // Portugu�s
		"jamais", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"nunca", // Espa�ol
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
		"", // Portugu�s
		"Protection automatique des enregistrement", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Autoprotecci�n de grabaciones", // Espa�ol
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
		"", // Portugu�s
		"est prot�g�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"est� protegido", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Open menu after pin request", // English
		"Men� nach Eingabe des Pin �ffnen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Ouvrir le menu apr�s la rentr� de la cl�ed", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Abrir men� despues de pedir el PIN", // Espa�ol
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
		"", // Portugu�s
		"L'enregeristrement est en protection parentale!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�La grabaci�n ya estaba protegida!", // Espa�ol
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
		"", // Portugu�s
		"L'enregeristrement est en protection parentale maintenant!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�Ahora la grabaci�n est� protegida!", // Espa�ol
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
		"", // Portugu�s
		"L'enregeristrement n'avait pas de protection parentale!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�La grabaci�n no estaba protegida!", // Espa�ol
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
		"", // Portugu�s
		"La protection parentale est enlever!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�Protecci�n de grabaci�n borrada!", // Espa�ol
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
		"", // Portugu�s
		"Entrer le code de protection d'abord!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�Establezca primero un c�digo PIN!", // Espa�ol
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
		"", // Portugu�s
		"Entr� d'abord le code", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Introduzca primero el c�digo PIN", // Espa�ol
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
		"", // Portugu�s
		"prot�g�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"bloqueado", // Espa�ol
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
		"", // Portugu�s
		"lib�r�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"desbloqueado", // Espa�ol
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
		"", // Portugu�s
		"Enclencher/d�clencher", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"On/Off", // Espa�ol
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
		"", // Portugu�s
		"Le code est faux!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"�C�digo no v�lido!", // Espa�ol
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
		"", // Portugu�s
		"Le code est accept�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"C�digo aceptado", // Espa�ol
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
		"", // Portugu�s
		"Introduisez le code d'acc�s:", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Establezca su c�digo PIN: ", // Espa�ol
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
		"", // Portugu�s
		"Le code d'acc�s est accept�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"C�digo PIN configurado correctamente", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Menu protections", // English
		"Men� sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Prot�gier des menus", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Protecciones de men�", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {	"Hide protected menus", // English
		"Gesperrte Men�s verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Menu prot�g�s sont invisibles", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ocultar men�s protegidos", // Espa�ol
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
		"", // Portugu�s
		"Plugins prot�g�s sont invisibles", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ocultar plugins protegidos", // Espa�ol
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
		"", // Portugu�s
		"Enregistrements prot�g�s sont invisibles", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Ocultar grabaciones protegidas", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Modo de b�squeda", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Configuraci�n", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Nombre", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Inicio", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Final", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"C�digo PIN", // Espa�ol
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
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"Nivel de registro [0-5]", // Espa�ol
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

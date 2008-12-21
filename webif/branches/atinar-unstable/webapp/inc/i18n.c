/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* (c) Christian Kelinski <k@kille.cx>
* Please checkout the README file!
* 
* Originally written for the open7x0.org VDR-FW project:
* www.open7x0.org
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#include <string.h>

#include "i18n.h"

int langID=-1;
char acceptedLang[3]="";
int numOfPhrases=93;
const char wochentage[7][15]={"Sonntag","Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag"};

// startOfWeek holds the index of the first weekday of the week.
// for example 0=Sunday for US, 1=Monday for Germany and so on
const int startOfWeek[I18NNUM]={0,1,1,1};
const char shortWeekdays[7][4]={"sun","mon","tue","wed","thu","fri","sat"};
const char weekdays[I18NNUM][7][15]={
		{"Sunday" ,"Monday","Tuesday" ,"Wednesday","Tuesday"   ,"Friday" ,"Saturday"},
		{"Sonntag","Montag","Dienstag","Mittwoch" ,"Donnerstag","Freitag","Samstag"},
		{"Domingo" ,"Lunes" ,"Martes"  ,"Miércoles","Jueves"    ,"Viernes" ,"Sábado"},
		{"Dimanche","Lundi" ,"Mardi"   ,"Mercredi" ,"Jeudi"     ,"Vendredi","Samedi"}
};

const tI18nPhrase i18n[]={
		{	"English",		// #0
			"Deutsch",
			"Español",
			"Français"
		},		
// Here comes the Alpha-2 Code as described in ISO 639-1:
// http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
		{	"en",			// #1
			"de",
			"es",
			"fr"
		},
// A suitable charactrer set.
		{	"ISO-8859-15",	// #2
		  "ISO-8859-15",
		  "ISO-8859-15",
		  "ISO-8859-15"
		},
// Please fill in the Alpha-3 Code as described in ISO 639-2:
// http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes
// Currently not used, but hey, who knows...
		{	"eng",			// #3
		  "ger",
		  "spa",
		  "fra"
		},
// First: the menus			
		{	"Start",		// #4
			"Start",
			"Inicio",
			"Lancement"
		},
		{	"Schedule",		// #5
		  "Programm",
		  "Guía de Programación",
		  "Programmation"
		},
		{	"Channels",		// #6
		  "Kanäle",
		  "Canales",
		  "Chaînes"
		},
		{	"Timers",		// #7
		  "Timer",
		  "Programaciones",
		  "Enregistrements programmés"
		},
		{	"Recordings",	// #8
		  "Aufzeichnungen",
		  "Grabaciones",
		  "Enregistrements"
		},
		{	"Setup",
			"Einstellungen",	// #9
			"Configuración",
			"Configuration"
		},
		{	"This &amp; That",	// #10
			"Dies &amp; Das",
			"",
			""
		},
		{	"Help",			// #11
			"Hilfe",
			"Ayuda",
			"Aide"
		},
							// #12
		{	"Important information: This webinterface is still under construction. It is not completed nor error-free.",
			"Wichtiger Hinweis: das Webinterface befindet sich noch in einer sehr frühen Phase. Weder ist es vollständig noch fehlerfrei. Zur Diskussion steht im open7x0.org Forum ein eigenes <a href=\"http://www.open7x0.org/arena/forumdisplay.php?fid=67\" target=\"_blank\">Unterforum</a> bereit.",
			"Información importante: El interfaz web está todavía en construcción y no está libre de errores.",
			"Information importante: cette accès web est en chantier. Il n\'est ni complet ni exempt de défaut"
		},					// #13
		{	"<p><b>Failed to open a SVDRP connection to the local VDR. Possible Reasons:\n\n<ul><li>The VDR is not running.</li>\n<li>The SVDRP connection is still in use by another client like vdradmin-AM</li>\n<li>An error occured while processing an earlier request and now the SVDRP connection is blocked.</li></ul>\n\nPlease check if the VDR is running or try it again later.</b></p>\n",
			"<p><b>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gr&uuml;nde daf&uuml;r:\n\n<ul><li>Der VDR wurde nicht gestartet bzw. l&auml;uft nicht mehr.</li>\n<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>\n<li>Es gab bei einer fr&uuml;heren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li></ul>\n\nBitte &uuml;berpr&uuml;fen sie, dass der VDR l&auml;uft und versuchen sie es ggf. sp&auml;ter noch einmal.</b></p>\n",
			"<p><b>No se puede abrir una conexión SVDRP con el VDR local. Posibles motivos:\n\n<ul><li>El VDR no se está ejecutando.</li>\n<li>La conexión SVDRP está todavía en uso por otro cliente como vdradmin-AM</li>\n<li>Se ha producido un error mientras se procesó una petición anterior y ahora la conexión SVDRP está bloqueada.</li></ul>\n\nPor favor, compruebe si el VDR se está ejecutando o inténtelo más tarde.</b></p>\n",
			"<p><b>Echec d\'accès par connection SVDRP au logiciel VDR. Raisons possibles:\n\n<ul><li>VDR n\'est pas démarré.</li><li>La connexion SVDRP est encore utilisée par un autre client comme \"vdradmin\".</li><li>la connexion SVDRP est bloqué suite à une erreur précédente</li></ul>\n\nVérifier que VDR est bien lancé ou essayer ultérieurement.</b></p>\n"
		},					// #14
		{	"EPG ist not avaible, please try again later. If the problem persists, this channel either has no EPG or it is broken. Feel free to get in touch with someone who is in charge for this channel.",
			"EPG ist nicht verf&uuml;gbar. Bitte versuchen sie es sp&auml;ter nocheinmal. Falls dieses Problem &uuml;ber l&auml;ngere Zeit besteht, wenden sie sich bitte an den Sender.",
			"EPG no disponible, por favor inténtelo más tarde. Si el problema persiste, éste canal no tiene EPG o funciona incorrectamente. Puede ponerse en contacto con algún responsable de éste canal.",
			"L\'EPG (guide des programmes) n\'est pas disponible, merci d\'essayer ultérieurement. Si le problème persiste, cette chaîne n\'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-être essayer de prendre contact avec un responsable de cette chaîne."
		},					// #15
		{	"An error occured, maybe with the translation of a phrase.",
			"Ein Fehler ist aufgetreten, wahrscheinlich konnte eine Übersetzung nicht gefunden werden.",
			"Ocurrió un error debido a la traducción de alguna frase.",
			"Erreur, peut-être due à un absence de traduction pour une phrase"
		},					// #16
		{	"List of available recordings.",					
			"Liste der aktuell verfügbaren Aufzeichnungen.",
			"Lista de grabaciones disponibles.",
			"Liste des enregistrements disponibles"
		},					// #17
		{	"Automatic (DHCP) or manuell configuration of IP address",
			"IP-Adresse automatisch (DHCP) oder manuell konfigurieren",
			"",
			""
		},
		{						// #18
			"IP address",
			"IP-Adresse",
			"",
			""
		},
		{						// #19
			"Subnet mask",
			"Subnetzmaske",
			"",
			""
		},
		{
			"Default Language",		// #20
			"Standardeinstellung für die Sprache",
			"",
			""
		},
		{	"Homepage",			// #
			"Startseite",
			"Página inicial",
			"Page d\'accueil"
		},
		{	"You are watching",	// #
			"Sie sehen zur Zeit",
			"Estás viendo",
			"Vous regardez"
		},
		{	"Channel List",		// #
			"Kanalliste",
			"Lista de canales",
			"Liste des chaînes"
		},						
		{	"channel number",	// #
			"Programmplatz",
			"Número del canal",
			"Numéro de chaîne"
		},
		{	"channel name",		// #
			"Sendername",
			"Nombre del canal",
			"Nom de chaîne"
		},
		{	"multiplex",		// #
			"Bouquet",
			"Multiplex",
			"Bouquet ou multiplex"
		},
		{	"more infos",		// #
			"mehr Infos",
			"más info",
			"Plus d\'informations"
		},
		{	"live stream",		// #
			"Live-Stream",
			"Stream directo",
			"Direct"
		},
		{	"frequency",		// #
			"Frequenz",
			"Frecuencia",
			"Fréquence"
		},
		{	"parameters",		// #
			"Parameter",
			"Parámetros",
			"Paramètres"
		},
		{	"source",			// #
			"Quelle",
			"Fuente",
			"Source"
		},
		{	"conditional access ID",	// #
			"Conditional Access ID",
			"ID acceso condicional",
			"Identifiant d\'accès conditionnel"
		},
		{	"service ID",		// #
			"Service-ID",
			"ID del servicio",
			"Identifiant de service"
		},
		{	"network ID",		// #
			"Netzwerk-ID",
			"ID de la red",
			"Identifiant de réseau"
		},
		{	"transport stream ID",		// #
			"Transponder-ID",
			"ID del transponder",
			"Identifiant du transpondeur"
		},
		{	"radio ID",			// #
			"Radio-ID",
			"ID de la radio",
			"Identifiant de radio"
		},
		{	"summary",			// #
			"Übersicht",
			"Resumen",
			"Résumé"
		},
		{	"edit list",		// #
			"Liste bearbeiten",
			"Editar lista",
			"Edition de la liste"
		},
		{	"watch TV",			// #
			"Fernsehen",
			"Ver TV",
			"Regarder le direct"
		},
		{	"pause",				// #
			"Pause",
			"Pause",
			"Pausa",
		},
		{	"full screen",		// #
			"Vollbild",
			"Pantalla completa",
			"Plein écran"
		},
		{	"mute",				// #
			"Stummschalten",
			"Silenciar",
			"Muet"
		},
		{	"switch",			// #
			"umschalten",
			"cambiar",
			"Basculer"
		},
		{	"Now on TV",		// #
			"Das läuft jetzt im TV",
			"Ahora en TV",
			"En direct"
		},
		{	"channel",			// #
			"Sender",
			"Canal",
			"chaîne"
		},
		{	"record",			// #
			"aufzeichnen",
			"grabar",
			"enregistrement"
		},
		{	"from",				// #
			"von",
			"desde",
			"de"
		},
		{	"to",				// #
			"bis",
			"hasta",
			"à"
		},
		{	"runtime",			// #
			"Dauer",
			"Duración",
			"durée"
		},
		{	"approx.",			// #
			"ca.",
			"aprox.",
			"approximativement"
		},
		{	"minutes",			// #
			"Minuten",
			"minutos",
			"minutes"
		},
		{	"date",				// #
			"Datum",
			"Fecha",
			"date"
		},
		{	"recording",		// #
			"Aufzeichnung",
			"Grabación",
			"enregistrement"
		},
		{	"program",			// #
			"Sendung",
			"Emisión",
			"émission"
		},
		{	"Setup Webinterface",	// #
			"Einstellungen Webinterface",
			"Configurar interfaz web",
			"Configuration de l\'interface web"
		},
		{	"Setup VDR",		// #
			"Einstellungen VDR",
			"Configurar VDR",
			"Configuration de VDR"
		},
		{	"Setup BoxAmp",		// #
			"Einstellungen BoxAmp",
			"Configurar BoxAmp",
			"Configuration de BoxAmp"
		},
		{	"Network shares",	// #
			"Netzlaufwerke",
			"Recursos de red",
			"Partitions réseau"
		},
		{	"Webinterface",		// #
			"Webinterface",
			"Interfaz web",
			"Interface web"
		},
		{	"default language",	// #
			"Voreingestellte Sprache",
			"Idioma por defecto",
			"langue par défaut"
		},
		{	"auto",				// #
			"automatisch",
			"automático",
			"automatique"
		},
		{ "BoxAmp path",		// #
			"BoxAmp-Verzeichnis",
			"Ruta BoxAmp",
			"Répertoire de BoxAmp"
		},
		{	"Path to audio files",  //#
			"Audio-Basisverzeichnis",
			"Ruta a archivos de música",
			"Répertoire des fichiers audio"
		},
		{	"Options",			// #
			"Optionen",
			"Opciones",
			"Options"
		},
		{	"Terminate automatically",	// #
			"Automatisch beenden",
			"Terminar automáticamente",
			"Terminaison automatique"
		},
		{	"for",				// #
			"für",
			"para",
			"pour"
		},
		{	"Time",				// #
			"Uhrzeit",
			"Duración",
			"Heure"
		},
		{	"create timer",		// #
			"Timer erstellen",
			"Crear programación",
			"création d\'un enregistrement programmé"
		},
		{	"Timer added!",		// #
			"Timer hinzugefügt!",
			"¡Programación añadida!",
			"Enregistrement programmé!"
		},
		{	"Failed to add timer!",	// #
			"Timer konnte nicht hinzugefügt werden!",
			"¡Añadir programación ha fallado!",
			"Erreur de programmation de l\'enregistrement!"
		},
		{	"new timer",		// #
			"Neuer Timer",
			"Nueva programación",
			"Nouvel enregistrement programmé"
		},
		{ "edit timer",		// #
			"Timer bearbeiten",
			"Editar programación",
			"modification d\'un enregistrement"
		},
		{	"active",			// #
			"aktiv",
			"activar",
			""
		},
		{	"one-time recording",	// #
			"Einmalige Aufnahme",
			"Grabar una vez",
			""
		},
		{	"regular recording",	// #
			"Regelm&auml;&szlig;ige Aufnahme",
			"Grabar como serie",
			""
		},
		{	"add start/stop margin",	// #
			"Vor-/Nachlaufzeit hinzuf&uuml;gen",
			"Añadir tiempo antes/después",
			""
		},
		{	"priority",		// #
			"Priorit&auml;t",
			"Prioridad",
			""
		},
		{	"lifetime",		// #
			"Lebensdauer",
			"Tiempo duración",
			""
		},
		{	"title",		// #
			"Titel",
			"Título",
			""
		},
		{	"check timer",	// #
			"Angaben pr&uuml;fen",
			"Comprobar programación",
			""
		},
		{	"yes",		// #
			"ja",
			"sí",
			"oui"
		},
		{	"no",		// #
			"nein",
			"no",
			"non"
		},
		{	"add",	// #
			"eintragen",
			"añadir",
			""
		},
		{	"download all",		//#
			"alle herunterladen",
			"descargar todo",
			""
		}, 
		{	"Timer updated!",	// #
			"Timer aktualisiert!",
			"¡Programación actualizada!",
			""
		},
		{	"Failed to update timer! Maybe it changed in the meantime?",	//#
			"Timer konnte nicht aktualisiert werden! Evt. hat sich die Timerliste in der Zwischenzeit verändert?",
			"¡Actualizar programación ha fallado! ¿Tal vez fue cambiada mientras tanto?",
			""
		},
		{	"- or -",		//#
			"- oder -",
			"- o -",
			""
		},
		{	"delete timer",	// #
			"Timer löschen",
			"Borrar programación",
			""
		},
		{	" execute ",	// #
			" ausführen ",
			" ejecutar ",
			""
		},
		{	"Timer deleted!",	// #
			"Timer gelöscht!",
			"¡Programación borrada!",
			""
		},
		{	"Failed to delete timer!",	// #
			"Timer konnte nicht gelöscht werden!",
			"¡Borrar programación ha fallado!",
			""
		},
		{	"browse",
			"schmökern",
			"",
			""
		}
};

const char *tr(const char *s) {
int i=0;
  if ((langID<0) || (langID>=I18NNUM)) {
    langID=0;
  }

  for (i=0;i<numOfPhrases;i++) {
    if (strcmp(s,i18n[i][0])==0) {
    	if (!strcmp(i18n[i][langID],"")==0) 
    		return i18n[i][langID];
    	else
    		return s;
    }
  }
  return s;
//  return i18n[15][langID];
}

char * dateStr(time_t t) {
	
}

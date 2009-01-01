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
int numOfPhrases=96;
char cldate[11];

// startOfWeek holds the index of the first weekday of the week.
// for example 0=Sunday for US, 1=Monday for Germany and so on
const int startOfWeek[I18NNUM]={0,1,1,1};
const char * shortWeekdays[I18NNUM][7]={
		{"Sun","Mon","Tue","Wed","Thu","Fri","Sat"},
		{"Son","Mon","Die","Mit","Don","Fre","Sam"},
		{"Dom","Lun","Mar","Mié","Jue","Vie","Sáb"},
		{"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"}
};
const char * weekdays[I18NNUM][7]={
		{"Sunday" ,"Monday","Tuesday" ,"Wednesday","Thursday"   ,"Friday" ,"Saturday"},
		{"Sonntag","Montag","Dienstag","Mittwoch" ,"Donnerstag","Freitag","Samstag"},
		{"Domingo" ,"Lunes" ,"Martes"  ,"Miércoles","Jueves"   ,"Viernes" ,"Sábado"},
		{"Dimanche","Lundi" ,"Mardi"   ,"Mercredi" ,"Jeudi"    ,"Vendredi","Samedi"}
};

const char * dateFormat[I18NNUM]={"%d/%m/%Y","%d.%m.%Y","%d/%m/%Y","%d-%m-%Y"};

const char * i18n[][I18NNUM]={
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
			"Configuración automática (DHCP) o manual de la dirección IP",
			""
		},
		{						// #18
			"IP address",
			"IP-Adresse",
			"Dirección IP",
			""
		},
		{						// #19
			"Subnet mask",
			"Subnetzmaske",
			"Máscara de subred",
			""
		},
		{
			"Default Language",		// #20
			"Standardeinstellung für die Sprache",
			"Idioma por defecto",
			""
		},
		{	"Homepage",			// #21
			"Startseite",
			"Página inicial",
			"Page d\'accueil"
		},
		{	"You are watching",	// #22
			"Sie sehen zur Zeit",
			"Estás viendo",
			"Vous regardez"
		},
		{	"Channel List",		// #23
			"Kanalliste",
			"Lista de canales",
			"Liste des chaînes"
		},						
		{	"channel number",	// #24
			"Programmplatz",
			"Número del canal",
			"Numéro de chaîne"
		},
		{	"channel name",		// #25
			"Sendername",
			"Nombre del canal",
			"Nom de chaîne"
		},
		{	"multiplex",		// #26
			"Bouquet",
			"Multiplex",
			"Bouquet ou multiplex"
		},
		{	"more infos",		// #27
			"mehr Infos",
			"más info",
			"Plus d\'informations"
		},
		{	"live stream",		// #28
			"Live-Stream",
			"Stream directo",
			"Direct"
		},
		{	"frequency",		// #29
			"Frequenz",
			"Frecuencia",
			"Fréquence"
		},
		{	"parameters",		// #30
			"Parameter",
			"Parámetros",
			"Paramètres"
		},
		{	"source",			// #31
			"Quelle",
			"Fuente",
			"Source"
		},
		{	"conditional access ID",	// #32
			"Conditional Access ID",
			"ID acceso condicional",
			"Identifiant d\'accès conditionnel"
		},
		{	"service ID",		// #33
			"Service-ID",
			"ID del servicio",
			"Identifiant de service"
		},
		{	"network ID",		// #34
			"Netzwerk-ID",
			"ID de la red",
			"Identifiant de réseau"
		},
		{	"transport stream ID",		// #35
			"Transponder-ID",
			"ID del transponder",
			"Identifiant du transpondeur"
		},
		{	"radio ID",			// #36
			"Radio-ID",
			"ID de la radio",
			"Identifiant de radio"
		},
		{	"summary",			// #37
			"Übersicht",
			"Resumen",
			"Résumé"
		},
		{	"edit list",		// #38
			"Liste bearbeiten",
			"Editar lista",
			"Edition de la liste"
		},
		{	"watch TV",			// #39
			"Fernsehen",
			"Ver TV",
			"Regarder le direct"
		},
		{	"pause",				// #40
			"Pause",
			"Pause",
			"Pausa",
		},
		{	"full screen",		// #41
			"Vollbild",
			"Pantalla completa",
			"Plein écran"
		},
		{	"mute",				// #42
			"Stummschalten",
			"Silenciar",
			"Muet"
		},
		{	"switch",			// #43
			"umschalten",
			"cambiar",
			"Basculer"
		},
		{	"Now on TV",		// #44
			"Das läuft jetzt im TV",
			"Ahora en TV",
			"En direct"
		},
		{	"channel",			// #45
			"Sender",
			"Canal",
			"chaîne"
		},
		{	"record",			// #46
			"aufzeichnen",
			"grabar",
			"enregistrement"
		},
		{	"from",				// #47
			"von",
			"Desde",
			"de"
		},
		{	"to",				// #48
			"bis",
			"Hasta",
			"à"
		},
		{	"runtime",			// #49
			"Dauer",
			"Duración",
			"durée"
		},
		{	"approx.",			// #50
			"ca.",
			"aprox.",
			"approximativement"
		},
		{	"minutes",			// #51
			"Minuten",
			"minutos",
			"minutes"
		},
		{	"date",				// #52
			"Datum",
			"Fecha",
			"date"
		},
		{	"recording",		// #53
			"Aufzeichnung",
			"Grabación",
			"enregistrement"
		},
		{	"program",			// #54
			"Sendung",
			"Emisión",
			"émission"
		},
		{	"Setup Webinterface",	// #55
			"Einstellungen Webinterface",
			"Configurar interfaz web",
			"Configuration de l\'interface web"
		},
		{	"Setup VDR",		// #56
			"Einstellungen VDR",
			"Configurar VDR",
			"Configuration de VDR"
		},
		{	"Setup BoxAmp",		// #57
			"Einstellungen BoxAmp",
			"Configurar BoxAmp",
			"Configuration de BoxAmp"
		},
		{	"Network shares",	// #58
			"Netzlaufwerke",
			"Recursos de red",
			"Partitions réseau"
		},
		{	"Webinterface",		// #59
			"Webinterface",
			"Interfaz web",
			"Interface web"
		},
		{	"default language",	// #60
			"Voreingestellte Sprache",
			"Idioma por defecto",
			"langue par défaut"
		},
		{	"auto",				// #61
			"automatisch",
			"automático",
			"automatique"
		},
		{ "BoxAmp path",		// #62
			"BoxAmp-Verzeichnis",
			"Ruta BoxAmp",
			"Répertoire de BoxAmp"
		},
		{	"Path to audio files",  //#63
			"Audio-Basisverzeichnis",
			"Ruta a archivos de música",
			"Répertoire des fichiers audio"
		},
		{	"Options",			// #64
			"Optionen",
			"Opciones",
			"Options"
		},
		{	"Terminate automatically",	// #65
			"Automatisch beenden",
			"Terminar automáticamente",
			"Terminaison automatique"
		},
		{	"for",				// #66
			"für",
			"para",
			"pour"
		},
		{	"Time",				// #67
			"Uhrzeit",
			"Duración",
			"Heure"
		},
		{	"create timer",		// #68
			"Timer erstellen",
			"Crear programación",
			"création d\'un enregistrement programmé"
		},
		{	"Timer added!",		// #69
			"Timer hinzugefügt!",
			"¡Programación añadida!",
			"Enregistrement programmé!"
		},
		{	"Failed to add timer!",	// #70
			"Timer konnte nicht hinzugefügt werden!",
			"¡Añadir programación ha fallado!",
			"Erreur de programmation de l\'enregistrement!"
		},
		{	"new timer",		// #71
			"Neuer Timer",
			"Nueva programación",
			"Nouvel enregistrement programmé"
		},
		{ "edit timer",		// #72
			"Timer bearbeiten",
			"Editar programación",
			"modification d\'un enregistrement"
		},
		{	"active",			// #73
			"aktiv",
			"activar",
			""
		},
		{	"one-time recording",	// #74
			"Einmalige Aufnahme",
			"Grabar una vez",
			""
		},
		{	"regular recording",	// #75
			"Regelm&auml;&szlig;ige Aufnahme",
			"Grabar como serie",
			""
		},
		{	"add start/stop margin",	// #76
			"Vor-/Nachlaufzeit hinzuf&uuml;gen",
			"Añadir tiempo antes/después",
			""
		},
		{	"priority",		// #77
			"Priorit&auml;t",
			"Prioridad",
			""
		},
		{	"lifetime",		// #78
			"Lebensdauer",
			"Tiempo duración",
			""
		},
		{	"title",		// #79
			"Titel",
			"Título",
			""
		},
		{	"check timer",	// #80
			"Angaben pr&uuml;fen",
			"Comprobar programación",
			""
		},
		{	"yes",		// #81
			"ja",
			"sí",
			"oui"
		},
		{	"no",		// #82
			"nein",
			"no",
			"non"
		},
		{	"add",	// #83
			"eintragen",
			"añadir",
			""
		},
		{	"download all",		//#84
			"alle herunterladen",
			"descargar todo",
			""
		}, 
		{	"Timer updated!",	// #85
			"Timer aktualisiert!",
			"¡Programación actualizada!",
			""
		},
		{	//86
			"Failed to update timer! Maybe it changed in the meantime?",	
			"Timer konnte nicht aktualisiert werden! Evt. hat sich die Timerliste in der Zwischenzeit verändert?",
			"¡Actualizar programación ha fallado! ¿Tal vez fue cambiada mientras tanto?",
			""
		},
		{	//87
			"- or -",		
			"- oder -",
			"- o -",
			""
		},
		{	//88
			"delete timer",	
			"Timer löschen",
			"Borrar programación",
			""
		},
		{	//89
			" execute ",	
			" ausführen ",
			" ejecutar ",
			""
		},
		{	//90
			"Timer deleted!",
			"Timer gelöscht!",
			"¡Programación borrada!",
			""
		},
		{	//91
			"Failed to delete timer!",
			"Timer konnte nicht gelöscht werden!",
			"¡Borrar programación ha fallado!",
			""
		},
		{	//92
			"browse",
			"schmökern",
			"navegar",
			""
		},
		{	//93
			"Current server",
			"",
			"Servidor actual",
			""
		},
		{	//94
			"Choose server",
			"",
			"Seleccionar servidor",
			""
		},
		{	//95
			"IP port",
			"",
			"Puerto IP",
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
}

const char *formatDate(struct tm *timeptr){
	strftime(cldate,11,dateFormat[langID],timeptr);
	return cldate;
}

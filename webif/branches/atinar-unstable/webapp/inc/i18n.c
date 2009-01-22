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
int numOfPhrases=105;
char cldate[20];

// startOfWeek holds the index of the first weekday of the week.
// for example 0=Sunday for US, 1=Monday for Germany and so on
const int startOfWeek[I18NNUM]={0,1,1,1};
const char * const shortWeekdays[I18NNUM][7]={
		{"Sun","Mon","Tue","Wed","Thu","Fri","Sat"},
		{"Son","Mon","Die","Mit","Don","Fre","Sam"},
		{"Dom","Lun","Mar","Mié","Jue","Vie","Sáb"},
		{"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"}
};
const char * const weekdays[I18NNUM][7]={
		{"Sunday" ,"Monday","Tuesday" ,"Wednesday","Thursday"   ,"Friday" ,"Saturday"},
		{"Sonntag","Montag","Dienstag","Mittwoch" ,"Donnerstag","Freitag","Samstag"},
		{"Domingo" ,"Lunes" ,"Martes"  ,"Miércoles","Jueves"   ,"Viernes" ,"Sábado"},
		{"Dimanche","Lundi" ,"Mardi"   ,"Mercredi" ,"Jeudi"    ,"Vendredi","Samedi"}
};

const char * const dateFormat[I18NNUM]={"%d/%m/%Y","%d.%m.%Y","%d/%m/%Y","%d-%m-%Y"};

const char * const i18n[][I18NNUM]={
		{	//0
			"English",
			"Deutsch",
			"Español",
			"Français"
		},		
// Here comes the Alpha-2 Code as described in ISO 639-1:
// http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
		{	//1
			"en",
			"de",
			"es",
			"fr"
		},
// A suitable charactrer set.
		{	//2
			"ISO-8859-15",
			"ISO-8859-15",
			"ISO-8859-15",
			"ISO-8859-15"
		},
// Please fill in the Alpha-3 Code as described in ISO 639-2:
// http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes
// Currently not used, but hey, who knows...
		{	//3
			"eng",
			"ger",
			"spa",
			"fra"
		},
// First: the menus			
		{	//4
			"Start",
			"Start",
			"Inicio",
			"Lancement"
		},
		{	//5
			"Schedule",
			"Programm",
			"Guía de Programación",
			"Programmation"
		},
		{	//6
			"Channels",
			"Kanäle",
			"Canales",
			"Chaînes"
		},
		{	//7
			"Timers",		
			"Timer",
			"Programaciones",
			"Enregistrements programmés"
		},
		{	//8
			"Recordings",
			"Aufzeichnungen",
			"Grabaciones",
			"Enregistrements"
		},
		{	//9
			"Setup",
			"Einstellungen",
			"Configuración",
			"Configuration"
		},
		{	//10
			"This &amp; That",
			"Dies &amp; Das",
			"",
			""
		},
		{	//11
			"Help",
			"Hilfe",
			"Ayuda",
			"Aide"
		},
		{	// 12
			"Important information: This webinterface is still under construction. It is not completed nor error-free.",
			"Wichtiger Hinweis: das Webinterface befindet sich noch in einer sehr frühen Phase. Weder ist es vollständig noch fehlerfrei. Zur Diskussion steht im open7x0.org Forum ein eigenes <a href=\"http://www.open7x0.org/arena/forumdisplay.php?fid=67\" target=\"_blank\">Unterforum</a> bereit.",
			"Información importante: El interfaz web está todavía en construcción y no está libre de errores.",
			"Information importante: cette accès web est en chantier. Il n\'est ni complet ni exempt de défaut"
		},
		{	// 13
			"<p><b>Failed to open a SVDRP connection to the local VDR. Possible Reasons:\n\n<ul><li>The VDR is not running.</li>\n<li>The SVDRP connection is still in use by another client like vdradmin-AM</li>\n<li>An error occured while processing an earlier request and now the SVDRP connection is blocked.</li></ul>\n\nPlease check if the VDR is running or try it again later.</b></p>\n",
			"<p><b>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gr&uuml;nde daf&uuml;r:\n\n<ul><li>Der VDR wurde nicht gestartet bzw. l&auml;uft nicht mehr.</li>\n<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>\n<li>Es gab bei einer fr&uuml;heren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li></ul>\n\nBitte &uuml;berpr&uuml;fen sie, dass der VDR l&auml;uft und versuchen sie es ggf. sp&auml;ter noch einmal.</b></p>\n",
			"<p><b>No se puede abrir una conexión SVDRP con el VDR local. Posibles motivos:\n\n<ul><li>El VDR no se está ejecutando.</li>\n<li>La conexión SVDRP está todavía en uso por otro cliente como vdradmin-AM</li>\n<li>Se ha producido un error mientras se procesó una petición anterior y ahora la conexión SVDRP está bloqueada.</li></ul>\n\nPor favor, compruebe si el VDR se está ejecutando o inténtelo más tarde.</b></p>\n",
			"<p><b>Echec d\'accès par connection SVDRP au logiciel VDR. Raisons possibles:\n\n<ul><li>VDR n\'est pas démarré.</li><li>La connexion SVDRP est encore utilisée par un autre client comme \"vdradmin\".</li><li>la connexion SVDRP est bloqué suite à une erreur précédente</li></ul>\n\nVérifier que VDR est bien lancé ou essayer ultérieurement.</b></p>\n"
		},
		{	// 14
			"EPG ist not avaible, please try again later. If the problem persists, this channel either has no EPG or it is broken. Feel free to get in touch with someone who is in charge for this channel.",
			"EPG ist nicht verf&uuml;gbar. Bitte versuchen sie es sp&auml;ter nocheinmal. Falls dieses Problem &uuml;ber l&auml;ngere Zeit besteht, wenden sie sich bitte an den Sender.",
			"EPG no disponible, por favor inténtelo más tarde. Si el problema persiste, éste canal no tiene EPG o funciona incorrectamente. Puede ponerse en contacto con algún responsable de éste canal.",
			"L\'EPG (guide des programmes) n\'est pas disponible, merci d\'essayer ultérieurement. Si le problème persiste, cette chaîne n\'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-être essayer de prendre contact avec un responsable de cette chaîne."
		},
		{	//15
			"An error occured, maybe with the translation of a phrase.",
			"Ein Fehler ist aufgetreten, wahrscheinlich konnte eine Übersetzung nicht gefunden werden.",
			"Ocurrió un error debido a la traducción de alguna frase.",
			"Erreur, peut-être due à un absence de traduction pour une phrase"
		},
		{	//16
			"List of available recordings.",					
			"Liste der aktuell verfügbaren Aufzeichnungen.",
			"Lista de grabaciones disponibles.",
			"Liste des enregistrements disponibles"
		},
		{	//17
			"Automatic (DHCP) or manuell configuration of IP address",
			"IP-Adresse automatisch (DHCP) oder manuell konfigurieren",
			"Configuración automática (DHCP) o manual de la dirección IP",
			""
		},
		{	//18
			"IP address",
			"IP-Adresse",
			"Dirección IP",
			""
		},
		{	//19
			"Subnet mask",
			"Subnetzmaske",
			"Máscara de subred",
			""
		},
		{	//20
			"Default Language",
			"Standardeinstellung für die Sprache",
			"Idioma por defecto",
			""
		},
		{	//21
			"Homepage",
			"Startseite",
			"Página inicial",
			"Page d\'accueil"
		},
		{	//22
			"You are watching",
			"Sie sehen zur Zeit",
			"Estás viendo",
			"Vous regardez"
		},
		{	//23
			"Channel List",
			"Kanalliste",
			"Lista de canales",
			"Liste des chaînes"
		},						
		{	//24
			"channel number",	
			"Programmplatz",
			"Número del canal",
			"Numéro de chaîne"
		},
		{	//25
			"channel name",		
			"Sendername",
			"Nombre del canal",
			"Nom de chaîne"
		},
		{	//26
			"multiplex",		
			"Bouquet",
			"Multiplex",
			"Bouquet ou multiplex"
		},
		{	//27
			"more infos",
			"mehr Infos",
			"más info",
			"Plus d\'informations"
		},
		{	//28
			"live stream",
			"Live-Stream",
			"Stream directo",
			"Direct"
		},
		{	//29
			"frequency",
			"Frequenz",
			"Frecuencia",
			"Fréquence"
		},
		{	//30
			"parameters",
			"Parameter",
			"Parámetros",
			"Paramètres"
		},
		{	//31
			"source",
			"Quelle",
			"Fuente",
			"Source"
		},
		{	//32
			"conditional access ID",
			"Conditional Access ID",
			"ID acceso condicional",
			"Identifiant d\'accès conditionnel"
		},
		{	//33
			"service ID",
			"Service-ID",
			"ID del servicio",
			"Identifiant de service"
		},
		{	//34
			"network ID",
			"Netzwerk-ID",
			"ID de la red",
			"Identifiant de réseau"
		},
		{	//35
			"transport stream ID",
			"Transponder-ID",
			"ID del transponder",
			"Identifiant du transpondeur"
		},
		{	//36
			"radio ID",
			"Radio-ID",
			"ID de la radio",
			"Identifiant de radio"
		},
		{	//37
			"summary",
			"Übersicht",
			"Resumen",
			"Résumé"
		},
		{	//38
			"edit list",
			"Liste bearbeiten",
			"Editar lista",
			"Edition de la liste"
		},
		{	//39
			"watch TV",
			"Fernsehen",
			"Ver TV",
			"Regarder le direct"
		},
		{	//40
			"pause",
			"Pause",
			"Pause",
			"Pausa",
		},
		{	//41
			"full screen",
			"Vollbild",
			"Pantalla completa",
			"Plein écran"
		},
		{	//42
			"mute",
			"Stummschalten",
			"Silenciar",
			"Muet"
		},
		{	//43
			"switch",
			"umschalten",
			"Cambiar",
			"Basculer"
		},
		{	//44
			"Now on TV",
			"Das läuft jetzt im TV",
			"Ahora en TV",
			"En direct"
		},
		{	//45
			"channel",
			"Sender",
			"Canal",
			"chaîne"
		},
		{	//46
			"record",
			"aufzeichnen",
			"grabar",
			"enregistrement"
		},
		{	//47
			"from",
			"von",
			"Inicio",
			"de"
		},
		{	//48
			"to",
			"bis",
			"Final",
			"à"
		},
		{	//49
			"runtime",
			"Dauer",
			"Duración",
			"durée"
		},
		{	//50
			"approx.",
			"ca.",
			"aprox.",
			"approximativement"
		},
		{	//51
			"minutes",
			"Minuten",
			"minutos",
			"minutes"
		},
		{	//52
			"Date",
			"Datum",
			"Fecha",
			""
		},
		{	//53
			"recording",
			"Aufzeichnung",
			"Grabación",
			"enregistrement"
		},
		{	//54
			"program",
			"Sendung",
			"Programa",
			"émission"
		},
		{	//55
			"Setup Webinterface",
			"Einstellungen Webinterface",
			"Configurar interfaz web",
			"Configuration de l\'interface web"
		},
		{	//56
			"Setup VDR",
			"Einstellungen VDR",
			"Configurar VDR",
			"Configuration de VDR"
		},
		{	//57
			"Setup BoxAmp",
			"Einstellungen BoxAmp",
			"Configurar BoxAmp",
			"Configuration de BoxAmp"
		},
		{	//58
			"Network shares",
			"Netzlaufwerke",
			"Recursos de red",
			"Partitions réseau"
		},
		{	//59
			"Webinterface",
			"Webinterface",
			"Interfaz web",
			"Interface web"
		},
		{	//60
			"default language",
			"Voreingestellte Sprache",
			"Idioma por defecto",
			"langue par défaut"
		},
		{	//61
			"auto",
			"automatisch",
			"automático",
			"automatique"
		},
		{	//62
			"BoxAmp path",
			"BoxAmp-Verzeichnis",
			"Ruta BoxAmp",
			"Répertoire de BoxAmp"
		},
		{	//63
			"Path to audio files",
			"Audio-Basisverzeichnis",
			"Ruta a archivos de música",
			"Répertoire des fichiers audio"
		},
		{	//64
			"Options",
			"Optionen",
			"Opciones",
			"Options"
		},
		{	//65
			"Terminate automatically",
			"Automatisch beenden",
			"Terminar automáticamente",
			"Terminaison automatique"
		},
		{	//66
			"for",
			"für",
			"para",
			"pour"
		},
		{	//67
			"Time",
			"Uhrzeit",
			"Hora",
			"Heure"
		},
		{	//68
			"Create timer",
			"Timer erstellen",
			"Crear programación",
			"création d\'un enregistrement programmé"
		},
		{	//69
			"Timer added!",
			"Timer hinzugefügt!",
			"¡Programación añadida!",
			"Enregistrement programmé!"
		},
		{	//70
			"Failed to add timer!",
			"Timer konnte nicht hinzugefügt werden!",
			"¡Añadir programación ha fallado!",
			"Erreur de programmation de l\'enregistrement!"
		},
		{	//71
			"New timer",
			"Neuer Timer",
			"Nueva programación",
			"Nouvel enregistrement programmé"
		},
		{	//72
			"Edit timer",
			"Timer bearbeiten",
			"Editar programación",
			"modification d\'un enregistrement"
		},
		{	//73
			"active",
			"aktiv",
			"activar",
			""
		},
		{	//74
			"one-time recording",
			"Einmalige Aufnahme",
			"Grabar una vez",
			""
		},
		{	//75
			"regular recording",
			"Regelm&auml;&szlig;ige Aufnahme",
			"Grabar como serie",
			""
		},
		{	//76
			"add start/stop margin",
			"Vor-/Nachlaufzeit hinzuf&uuml;gen",
			"Añadir tiempo antes/después",
			""
		},
		{	//77
			"priority",
			"Priorit&auml;t",
			"Prioridad",
			""
		},
		{	//78
			"lifetime",
			"Lebensdauer",
			"Tiempo duración",
			""
		},
		{	//79
			"Title",
			"Titel",
			"Título",
			""
		},
		{	//80
			"Check timer",
			"Angaben pr&uuml;fen",
			"Comprobar programación",
			""
		},
		{	//81
			"yes",
			"ja",
			"sí",
			"oui"
		},
		{	//82
			"no",
			"nein",
			"no",
			"non"
		},
		{	//83
			"add",
			"eintragen",
			"añadir",
			""
		},
		{	//84
			"download all",
			"alle herunterladen",
			"descargar todo",
			""
		}, 
		{	//85
			"Timer updated!",
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
			"Browse",
			"Schmökern",
			"Navegar",
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
		},
		{	//96
			"Links",
			"",
			"Enlaces",
			""
		},
		{	//97
			"Now",
			"Jetz",
			"Ahora",
			""
		},
		{	//98
			"Next",
			"Danach",
			"Luego",
			""
		},
		{	//99
			"Download playlist",
			"",
			"Descargar lista",
			""
		},
		{	//100
			"Free Space",
			"",
			"Espacio libre",
			""
		},
		{	//101
			"Name",
			"",
			"Nombre",
			""
		},
		{	//102
			"Used",
			"",
			"Usado",
			""
		},
		{	//103
			"Available",
			"",
			"Disponible",
			""
		},
		{	//104
			"Change",
			"",
			"Cambiar",
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

const char *formatDate(struct tm *timeptr, int addHour){
	char * format;
	if (!addHour || asprintf(&format,"%s %H:%M",dateFormat[langID])<0) {
		format=(char *)dateFormat[langID];
		addHour=0;
	}
	size_t l = strftime(cldate,sizeof cldate,(const char*)format,timeptr);
	if (addHour) {
		free(format);
	}
	return cldate;
}

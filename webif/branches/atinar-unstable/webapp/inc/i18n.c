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

const i18nVal_t locale={
	"en_GB",
	"de_DE",
	"es_ES",
	"fr_FR"
};

const i18nVal_t language={
	"English",
	"Deutsch",
	"Español",
	"Français"
};

//Alpha-2 Code as described in ISO 639-1: http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
const i18nVal_t alpha2={
	"en",
	"de",
	"es",
	"fr"
};

//3 Alpha-3 Code as described in ISO 639-2: http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes
const i18nVal_t alpha3={
	"eng",
	"ger",
	"spa",
	"fra"
};


const i18nVal_t dateFormat={
	"%d/%m/%Y",
	"%d.%m.%Y",
	"%d/%m/%Y",
	"%d-%m-%Y"
};

//TODO ordenar por key
i18n_t i18n[]={
	{	"zzz", //Should be last after sorting. Don't touch it.
		{
		"",
		"",
		"",
		""
		}
	},
	{	"accept",
		{
		"Accept",
		"",
		"Aceptar",
		""
		}
	},
	{	"active",
		{
		"Active",
		"Aktiv",
		"Activo",
		""
		}
	},
	{	"add",
		{
		"Add",
		"Eintragen",
		"Añadir",
		""
		}
	},
	{	"addMargin",
		{
		"Add start/stop margin",
		"Vor-/Nachlaufzeit hinzuf&uuml;gen",
		"Añadir tiempo antes/después",
		""
		}
	},
	{	"available",
		{
		"Available",
		"",
		"Disponible",
		""
		}
	},
	{	"baAudio",
		{
		"Path to audio files",
		"Audio-Basisverzeichnis",
		"Ruta a archivos de música",
		"Répertoire des fichiers audio"
		}
	},
	{	"baOptions",
		{
		"BoxAmp options",
		"BoxAmp optionen",
		"BoxAmp opciones",
		"BoxAmp options"
		}
	},
	{	"baPath",
		{
		"BoxAmp path",
		"BoxAmp-Verzeichnis",
		"Ruta BoxAmp",
		"Répertoire de BoxAmp"
		}
	},
	{	"baSetup",
		{
		"Setup BoxAmp",
		"Einstellungen BoxAmp",
		"Configuración BoxAmp",
		"Configuration de BoxAmp"
		}
	},
	{	"baTerm",
		{
		"Terminate automatically",
		"Automatisch beenden",
		"Terminar automáticamente",
		"Terminaison automatique"
		}
	},
	{	"bootMenu",
		{
		"Boot Menu",
		"",
		"Menú de arranque",
		""
		}
	},
	{	"browse",
		{
		"Browse",
		"Schmökern",
		"Navegar",
		""
		}
	},
	{	"change",
		{
		"Change",
		"",
		"Cambiar",
		""
		}
	},
	{	"channel",
		{
		"Channel",
		"Sender",
		"Canal",
		"Chaîne"
		}
	},
	{	"channelDetails",
		{
		"Channel details",
		"",
		"Detalles del canal",
		""
		}
	},
	{	"channelList",
		{
		"Channel List",
		"Kanalliste",
		"Lista de canales",
		"Liste des chaînes"
		}
	},						
	{	"channelListEdit",
		{
		"Edit list",
		"Liste bearbeiten",
		"Editar lista",
		"Edition de la liste"
		}
	},
	{	"channelMux",
		{
		"Multiplex",		
		"Bouquet",
		"Multiplex",
		"Bouquet ou multiplex"
		}
	},
	{	"channelName",
		{
		"Channel name",		
		"Sendername",
		"Nombre del canal",
		"Nom de chaîne"
		}
	},
	{	"channelNumber",
		{
		"Channel number",	
		"Programmplatz",
		"Número del canal",
		"Numéro de chaîne"
		}
	},
	{	"channelPlaylistDownload",
		{
		"Download channel playlist",
		"",
		"Descargar lista de reproducción de canales",
		""
		}
	},
	{	"channelWatch",
		{
		"Watch TV",
		"Fernsehen",
		"Ver TV",
		"Regarder le direct"
		}
	},
	{	"channelWatching",
		{
		"You are watching",
		"Sie sehen zur Zeit",
		"Estás viendo",
		"Vous regardez"
		}
	},
	{	"channels",
		{
		"Channels",
		"Kanäle",
		"Canales",
		"Chaînes"
		}
	},
	{	"conditionalAccessId",
		{
		"Conditional access ID",
		"Conditional Access ID",
		"ID de acceso condicional",
		"Identifiant d\'accès conditionnel"
		}
	},
	{	"confIP",
		{
		"IP address (if manual configuration)",
		"IP-Adresse (bei manueller Konfiguration)",
		"Dirección IP (si conf. manual)",
		""
		}
	},
	{	"confNetmask",
		{
		"Subnet mask (if manual configuration)",
		"Subnetzmaske (bei manueller Konfiguration)",
		"Máscara de red (si conf. manual)",
		""
		}
	},
	{	"currentServer",
		{
		"Current server",
		"",
		"Servidor actual",
		""
		}
	},
	{	"date",
		{
		"Date",
		"Datum",
		"Fecha",
		""
		}
	},
	{	"defaultLang",
		{
		"Default Language",
		"Voreingestellte Sprache",
		"Idioma por defecto",
		"langue par défaut"
		}
	},
	{	"desc",
		{
		"Description",
		"",
		"Descripción",
		""
		}
	},
	{	"downloadAll",
		{
		"Download all",
		"Alle herunterladen",
		"Descargar todo",
		""
		}
	}, 
	{	"end",
		{
		"End",	
		"",
		"Fin",
		""
		}
	},
	{	"execute",
		{
		"Execute",	
		"Ausführen",
		"Ejecutar",
		""
		}
	},
	{	"file",
		{
		"File",
		"",
		"Archivo",
		""
		}
	},
	{	"fileDownload",
		{
		"Download file",
		"",
		"Descargar archivo",
		""
		}
	},
	{	"fileView",
		{
		"View files",
		"",
		"Ver archivos",
		""
		}
	},
	{	"for",
		{
		"For",
		"Für",
		"Para",
		"Pour"
		}
	},
	{	"freeSpace",
		{
		"Free Space",
		"",
		"Espacio libre",
		""
		}
	},
	{	"frequency",
		{
		"Frequency",
		"Frequenz",
		"Frecuencia",
		"Fréquence"
		}
	},
	{	"fromToFmt",
		{
		"<span class=\"fromTo\">"
			"<span class=\"l\">From</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"<span class=\"l\">to</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"(<span class=\"l\">Runtime</span>: approx. <span class=\"t\">%d</span> minutes)"
		"</span>"
		,
		"<span class=\"fromTo\">"
			"<span class=\"l\">Von</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"<span class=\"l\">bis</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"(<span class=\"l\">Dauer</span>: ca. <span class=\"t\">%d</span> Minuten)"
		"</span>"
		,
		"<span class=\"fromTo\">"
			"<span class=\"l\">Inicio</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"<span class=\"l\">Final</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"(<span class=\"l\">Duración</span>: <span class=\"t\">%d</span> minutos aprox.)"
		"</span>"
		,
		"<span class=\"fromTo\">"
			"<span class=\"l\">De</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"<span class=\"l\">à</span>&nbsp;<span class=\"t\">%02d:%02d</span> "
			"(<span class=\"l\">Durée</span>: approximativement <span class=\"t\">%d</span> minutes)"
		"</span>"
		}
	},
	{	"fstab",
		{
		"File systems",
		"",
		"Sistemas de ficheros",
		""
		}
	},
	{	"fullScreen",
		{
		"full screen",
		"Vollbild",
		"Pantalla completa",
		"Plein écran"
		}
	},
	{	"help",
		{
		"Help",
		"Hilfe",
		"Ayuda",
		"Aide"
		}
	},
	{	"homepage",
		{
		"Homepage",
		"Startseite",
		"Página inicial",
		"Page d\'accueil"
		}
	},
	{	"ipAddress",
		{
		"IP address",
		"IP-Adresse",
		"Dirección IP",
		""
		}
	},
	{	"ipPort",
		{
		"IP port",
		"",
		"Puerto IP",
		""
		}
	},
	{	"lifetime",
		{
		"Lifetime",
		"Lebensdauer",
		"Tiempo duración",
		""
		}
	},
	{	"links",
		{
		"Links",
		"",
		"Enlaces",
		""
		}
	},
	{	"liveStream",
		{
		"Live stream",
		"Live-Stream",
		"Stream directo",
		"Direct"
		}
	},
	{	"moreInfo",
		{
		"More infos",
		"Mehr Infos",
		"Más info",
		"Plus d\'informations"
		}
	},
	{	"mute",
		{
		"Mute",
		"Stummschalten",
		"Silenciar",
		"Muet"
		}
	},
	{	"name",
		{
		"Name",
		"",
		"Nombre",
		""
		}
	},
	{	"networkId",
		{
		"Network ID",
		"Netzwerk-ID",
		"ID de la red",
		"Identifiant de réseau"
		}
	},
	{	"networkShares",
		{
		"Network shares",
		"Netzlaufwerke",
		"Recursos de red",
		"Partitions réseau"
		}
	},
	{	"next",
		{
		"Next",
		"Danach",
		"Luego",
		""
		}
	},
	{	"no",
		{
		"No",
		"Nein",
		"No",
		"Non"
		}
	},
	{	"noMediaErr",
		{
		"No media found!",
		"",
		"No se han encontrado archivos",
		""
		}
	},
	{	"now",
		{
		"Now",
		"Jetz",
		"Ahora",
		""
		}
	},
	{	"nowOnTv",
		{
		"Now on TV",
		"Das läuft jetzt im TV",
		"Ahora en TV",
		"En direct"
		}
	},
	{	"number",
		{
		"Number",	
		"Platz",
		"Número",
		"Numéro"
		}
	},
	{	"oneTimeRecording",
		{
		"One-time recording",
		"Einmalige Aufnahme",
		"Grabar una vez",
		""
		}
	},
	{	"parameters",
		{
		"Parameters",
		"Parameter",
		"Parámetros",
		"Paramètres"
		}
	},
	{	"partition",
		{
		"Partition",
		"",
		"Partición",
		""
		}
	},
	{	"partitionSpace",
		{
		"Free space per partition",
		"",
		"Espacio libre por particion",
		""
		}
	},
	{	"pause",
		{
		"Pause",
		"Pause",
		"Pause",
		"Pausa",
		}
	},
	{	"play",
		{
		"Play",
		"",
		"Reproducir",
		"",
		}
	},
	{	"playlist",
		{
		"Playlist",
		"",
		"Lista reproducción",
		"",
		}
	},
	{	"playlistDownload",
		{
		"Download playlist",
		"",
		"Descargar lista",
		""
		}
	},
	{	"priority",
		{
		"Priority",
		"Priorit&auml;t",
		"Prioridad",
		""
		}
	},
	{	"program",
		{
		"Program",
		"Sendung",
		"Emisión",
		"Émission"
		}
	},
	{	"radioChannels",
		{
		"Radio channels",
		"",
		"Canales de radio",
		""
		}
	},
	{	"radioId",
		{
		"Radio ID",
		"Radio-ID",
		"ID de la radio",
		"Identifiant de radio"
		}
	},
	{	"rc",
		{
		"rc.conf",
		"",
		"",
		""
		}
	},
	{	"rc.local",
		{
		"rc.local.conf",
		"",
		"",
		""
		}
	},
	{	"recDelete",
		{
		"Delete record",	
		"Aufzeichnen löschen",
		"Borrar grabación",
		""
		}
	},
	{	"recDeleteConfirm",
		{
		"Are you sure you want to delete the record?",
		"Bist du sicher, dass du den Aufzeichnen löschen willst?",
		"¿Seguro que quieres borrar la grabación?",
		""
		}
	},	
	{	"recDeleteErr",
		{
		"Failed to delete record!",
		"Aufzeichnen konnte nicht gelöscht werden!",
		"¡Error al borrar la grabación!",
		""
		}
	},
	{	"recDeleteOk",
		{
		"Record deleted",
		"Aufzeichnen gelöscht",
		"Grabación borrada",
		""
		}
	},
	{	"recEdit",
		{
		"Edit recording",
		"Aufzeichnen bearbeiten",
		"Editar grabación",
		""
		}
	},
	{	"recEditErr",
		{
		"Failed to update recording!",	
		"Aufzeichnen konnte nicht aktualisiert werden!",
		"¡Error al actualizar la grabación!",
		""
		}
	},
	{	"recEditOk",
		{
		"Recording updated",
		"Aufzeichnen aktualisiert",
		"Grabación actualizada",
		""
		}
	},
	{	"record",
		{
		"Record",
		"Aufzeichnen",
		"Grabar",
		"Enregistrement"
		}
	},
	{	"recording",
		{
		"Recording",
		"Aufzeichnung",
		"Grabación",
		"enregistrement"
		}
	},
	{	"recordingList",
		{
		"List of available recordings.",					
		"Liste der aktuell verfügbaren Aufzeichnungen.",
		"Lista de grabaciones disponibles.",
		"Liste des enregistrements disponibles"
		}
	},
	{	"recordings",
		{
		"Recordings",
		"Aufzeichnungen",
		"Grabaciones",
		"Enregistrements"
		}
	},
	{	"regularRecording",
		{
		"Regular recording",
		"Regelm&auml;&szlig;ige Aufnahme",
		"Grabar como serie",
		""
		}
	},
	{	"runtime",
		{
		"Runtime",
		"Dauer",
		"Duración",
		"durée"
		}
	},
	{	"schedule",
		{
		"Schedule",
		"Programm",
		"Guía de Programación",
		"Programmation"
		}
	},
	{	"serviceId",
		{
		"Service ID",
		"Service-ID",
		"ID del servicio",
		"Identifiant de service"
		}
	},
	{	"setup",
		{
		"Setup",
		"Einstellungen",
		"Configuración",
		"Configuration"
		}
	},
	{	"source",
		{
		"Source",
		"Quelle",
		"Fuente",
		"Source"
		}
	},
	{	"start",
		{
		"Start",
		"Start",
		"Inicio",
		"Lancement"
		}
	},
	{	"streamdevHosts",
		{
		"Streamdevice Hosts",
		"",
		"",
		""
		}
	},
	{	"subfolders",
		{
		"Subfolders",
		"",
		"Subcarpetas",
		""
		}
	},
	{	"subtitle",
		{
		"Subtitle",
		"Untertitel",
		"Subtítulo",
		""
		}
	},
	{	"summary",
		{
		"Summary",
		"Übersicht",
		"Resumen",
		"Résumé"
		}
	},
	{	"svdrpHosts",
		{
		"SVDRP Hosts",
		"",
		"Hosts SVDRP",
		""
		}
	},
	{	"switch",
		{
		"Switch",
		"Umschalten",
		"Cambiar",
		"Basculer"
		}
	},
	{	"sysLog",
		{
		"SysLog",
		"",
		"Registro",
		""
		}
	},
	{	"time",
		{
		"Time",
		"Uhrzeit",
		"Hora",
		"Heure"
		}
	},
	{	"timerAdd",
		{
		"New timer",
		"Neuer Timer",
		"Nueva programación",
		"Nouvel enregistrement programmé"
		}
	},
	{	"timerCheck",
		{
		"Check timer",
		"Angaben pr&uuml;fen",
		"Comprobar programación",
		""
		}
	},
	{	"timerCreate",
		{
		"Create timer",
		"Timer erstellen",
		"Crear programación",
		"Création d\'un enregistrement programmé"
		}
	},
	{	"timerCreateErr",
		{
		"Failed to add timer!",
		"Timer konnte nicht hinzugefügt werden!",
		"¡Error al añadir la programación!",
		"Erreur de programmation de l\'enregistrement!"
		}
	},
	{	"timerCreateOk",
		{
		"Timer added",
		"Timer hinzugefügt",
		"Programación añadida",
		"Enregistrement programmé"
		}
	},
	{	"timerDelete",
		{
		"Delete timer",	
		"Timer löschen",
		"Borrar programación",
		""
		}
	},
	{	"timerDeleteConfirm",
		{
		"Are you sure you want to delete the timer?",
		"Bist du sicher, dass du den Timer löschen willst?",
		"¿Seguro que quieres borrar la programación?",
		""
		}
	},	
	{	"timerDeleteErr",
		{
		"Failed to delete timer!",
		"Timer konnte nicht gelöscht werden!",
		"¡Error al borrar la programación!",
		""
		}
	},
	{	"timerDeleteOk",
		{
		"Timer deleted",
		"Timer gelöscht",
		"Programación borrada",
		""
		}
	},
	{	"timerEdit",
		{
		"Edit timer",
		"Timer bearbeiten",
		"Editar programación",
		"Modification d\'un enregistrement"
		}
	},
	{	"timerList",
		{
		"List of timers",					
		"",
		"Lista de programaciones",
		""
		}
	},
	{	"timerUpdateErr",
		{
		"Failed to update timer!",	
		"Timer konnte nicht aktualisiert werden!",
		"¡Error actualizando programación!",
		""
		}
	},
	{	"timerUpdateOk",
		{
		"Timer updated",
		"Timer aktualisiert",
		"Programación actualizada",
		""
		}
	},
	{	"timers",
		{
		"Timers",		
		"Timer",
		"Programaciones",
		"Enregistrements programmés"
		}
	},
	{	"timers.",
		{
		"Timr.",		
		"Timer",
		"Progr.",
		"Enreg."
		}
	},
	{	"title",
		{
		"Title",
		"Titel",
		"Título",
		""
		}
	},
	{	"transportStreamId",
		{
		"Transport stream ID",
		"Transponder-ID",
		"ID del transponder",
		"Identifiant du transpondeur"
		}
	},
	{	"tvChannels",
		{
		"TV channels",
		"",
		"Canales de televisión",
		""
		}
	},
	{	"used",
		{
		"Used",
		"",
		"Usado",
		""
		}
	},
	{	"vdrSetup",
		{
		"Setup VDR",
		"Einstellungen VDR",
		"Configuración VDR",
		"Configuration de VDR"
		}
	},
	{	"volumeDown",
		{
		"Volume down",
		"",
		"Bajar volumen",
		""
		}
	},
	{	"volumeUp",
		{
		"Volume up",
		"",
		"Subir volumen",
		""
		}
	},
	{	"warnNoEPG",
		{
		"EPG ist not available, please try again later. If the problem persists, this channel either has no EPG or it is broken. Feel free to get in touch with someone who is in charge for this channel.",
		"EPG ist nicht verf&uuml;gbar. Bitte versuchen sie es sp&auml;ter nocheinmal. Falls dieses Problem &uuml;ber l&auml;ngere Zeit besteht, wenden sie sich bitte an den Sender.",
		"<p>EPG no disponible, por favor inténtelo más tarde.</p>"
		"<p>Si el problema persiste, éste canal no tiene EPG o funciona incorrectamente.</p>",
		"L\'EPG (guide des programmes) n\'est pas disponible, merci d\'essayer ultérieurement. Si le problème persiste, cette chaîne n\'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-être essayer de prendre contact avec un responsable de cette chaîne."
		}
	},
	{	"warnSvdrpConnection",
		{
		"<p><b>Failed to open a SVDRP connection to the local VDR. Possible Reasons:\n\n<ul><li>The VDR is not running.</li>\n<li>The SVDRP connection is still in use by another client like vdradmin-AM</li>\n<li>An error occured while processing an earlier request and now the SVDRP connection is blocked.</li></ul>\n\nPlease check if the VDR is running or try it again later.</b></p>\n",
		"<p><b>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gr&uuml;nde daf&uuml;r:\n\n<ul><li>Der VDR wurde nicht gestartet bzw. l&auml;uft nicht mehr.</li>\n<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>\n<li>Es gab bei einer fr&uuml;heren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li></ul>\n\nBitte &uuml;berpr&uuml;fen sie, dass der VDR l&auml;uft und versuchen sie es ggf. sp&auml;ter noch einmal.</b></p>\n",
		"<p>No se puede abrir una conexión SVDRP con el VDR local.</p>/n"
		"<p>Posibles motivos:</p>/n"
		"<ul>/n"
		"	<li>El VDR no se está ejecutando.</li>\n"
		"	<li>La conexión SVDRP está todavía en uso por otro cliente como vdradmin-AM</li>\n"
		"	<li>Se ha producido un error mientras se procesó una petición anterior y ahora la conexión SVDRP está bloqueada.</li>\n"
		"</ul>\n"
		"<p>Por favor, compruebe si el VDR se está ejecutando o inténtelo más tarde.</p>\n",
		"<p><b>Echec d\'accès par connection SVDRP au logiciel VDR. Raisons possibles:\n\n<ul><li>VDR n\'est pas démarré.</li><li>La connexion SVDRP est encore utilisée par un autre client comme \"vdradmin\".</li><li>la connexion SVDRP est bloqué suite à une erreur précédente</li></ul>\n\nVérifier que VDR est bien lancé ou essayer ultérieurement.</b></p>\n"
		}
	},
	{	"webif",
		{
		"Webinterface",
		"Webinterface",
		"Interfaz web",
		"Interface web"
		}
	},
	{	"webifSetup",
		{
		"Setup Webinterface",
		"Einstellungen Webinterface",
		"Configuración web",
		"Configuration de l\'interface web"
		}
	},
	{	"yes",
		{
		"Yes",
		"Ja",
		"Sí",
		"Oui"
		}
	}
};

const int i18nLength=sizeof(i18n)/sizeof(i18n_t);

int i18nCompare(const void *a, const void *b){
	const i18n_t *ia=(const i18n_t *)a;
	const i18n_t *ib=(const i18n_t *)b;
	int result;
	if (ia->key==NULL){
		warn( "i18n:Null key found");
		if (ib->key==NULL) {
			result = strcmp(ia->val[0],ib->val[0]);
		} else {
			result = -1;
		}
	} else {
		result = strcmp(ia->key,ib->key);
	}
	return result;
}


const char *tr(const char *s) {
	/*
	if (i18n[0].key[0]=='z'){ 
		info("Before sorting i18n. i18n[0].key:%s",i18n[0].key);
		qsort(&i18n,i18nLength,sizeof(i18n_t),i18nCompare);
		info("After sorting i18n. i18n[0].key:%s",i18n[0].key);
	}
	*/
	i18n_t test = { s, { s,NULL,NULL,NULL}};
	i18n_t *found=(i18n_t *)bsearch(&test,&i18n,i18nLength,sizeof(i18n_t),i18nCompare);
	if (found==NULL) {
		warn("i18n:Key %s not found",s);
	}
	if ((langID<0) || (langID>=I18NNUM)) {
		langID=0;
	}
	const char * result = (found!=NULL) ? (strlen(found->val[langID])>0) ? found->val[langID] : found->val[0] : s;
	return result;
}

const char *old_tr(const char *s) {
	int i;
	if ((langID<0) || (langID>=I18NNUM)) {
		langID=0;
	}
	for (i=0;i<i18nLength;i++) {
		const char *key = (i18n[i].key) ? i18n[i].key : i18n[i].val[0];	
		if (strcmp(s,key)==0) {
			return (strlen(i18n[i].val[langID])>0) ? i18n[i].val[langID] : s;
		}
	}
	return s;
}

const char *formatDate(struct tm *timeptr, boolean_t addHour){
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

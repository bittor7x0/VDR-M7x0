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

int langId=-1;
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


// Important: keep i18n ordered by key. Otherwise binary search will fail.
i18n_t i18n[]={
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
	{	"aux",
		{
		"Aux.",
		"",
		"",
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
	{	"cancel",
		{
		"Cancel",
		"",
		"Cancelar",
		""
		}
	},
	{	"cfgBa",
		{
		"Setup BoxAmp",
		"Einstellungen BoxAmp",
		"Configuración del BoxAmp",
		"Configuration de BoxAmp"
		}
	},
	{	"cfgBa.boxamp_bin",
		{
		"BoxAmp bin",
		"",
		"Ejecutable BoxAmp",
		""
		}
	},
	{	"cfgBa.boxamp_mp3dir",
		{
		"Path to audio files",
		"Audio-Basisverzeichnis",
		"Ruta a archivos de audio",
		"Répertoire des fichiers audio"
		}
	},
	{	"cfgBa.boxamp_opts",
		{
		"BoxAmp options",
		"BoxAmp optionen",
		"Opciones del BoxAmp",
		"BoxAmp options"
		}
	},
	{	"cfgBa.boxamp_path",
		{
		"BoxAmp path",
		"BoxAmp-Verzeichnis",
		"Ruta BoxAmp",
		"Répertoire de BoxAmp"
		}
	},
	{	"cfgBa.boxamp_startup_time",
		{
		"Start up time (sec.)",
		"",
		"Tiempo de inicio (seg.)",
		""
		}
	},
	{	"cfgBa.runboxamp",
		{
		"runboxamp path",
		"",
		"Ruta del runboxamp",
		""
		}
	},
	{	"cfgBa.runboxamp_vars",
		{
		"runboxamp vars",
		"",
		"Variables para runboxamp",
		""
		}
	},
	{	"cfgErrInvalidParamValueFmt",
		{
		"Invalid param value (%s)",
		"",
		"Valor del parámetro (%s) incorrecto",
		""
		}
	},
	{	"cfgErrParamNameValueMismatch",
		{
		"Mismatch between name and value numbers",
		"",
		"El número de nombres y valores de parámetros no coincide.",
		""
		}
	},
	{	"cfgFileChangeDisabledErr",
		{
		"Change of configuration is disabled.",
		"",
		"La modificación de la configuración está bloqueda.",
		""
		}
	},
	{	"cfgFileDoesNotExistFmt",
		{
		"Configuration file #%d ( %s ) doesn't exists",
		"Konfigurationsdatei #%d ( %s ) nicht vorhanden",
		"El archivo de configuración #%d ( %s ) no existe",
		""
		}
	},
	{	"cfgFileIsNew",
		{
		"Configuration file will be created now.",
		"",
		"El archivo de configuración se creará ahora.",
		""
		}
	},
	{	"cfgFileSaveErrFmt",
		{
		"Error saving configuration file %s .",
		"",
		"Ha habido algún error al guardar el archivo de configuración %s.",
		""
		}
	},
	{	"cfgFileSaveOkFmt",
		{
		"Configuration file %s saved.",
		"",
		"El archivo de configuración %s se ha guardado correctamente.",
		""
		}
	},
	{	"cfgRc",
		{
		"VDR Network configuration",
		"VDR Einstellungen Netz",
		"Configuración de red del VDR",
		""
		}
	},
	{	"cfgRc.dhcp_flags",
		{
		"DHCP client options",
		"",
		"Opciones para el cliente DHCP",
		""
		}
	},
	{	"cfgRc.gateway",
		{
		"Gateway",
		"",
		"",
		""
		}
	},
	{	"cfgRc.hostname",
		{
		"Cmd to obtain hostname",
		"",
		"Orden para obtener nombre de host",
		""
		}
	},
	{	"cfgRc.if",
		{
		"Network interface",
		"",
		"Interfaz de red",
		""
		}
	},
	{	"cfgRc.ip",
		{
		"IP address (if manual configuration)",
		"IP-Adresse (bei manueller Konfiguration)",
		"Dirección IP (si conf. manual)",
		""
		}
	},
	{	"cfgRc.lircd_flags",
		{
		"LIRC (Remote Control) server options",
		"",
		"Opciones para el servidor LIRC (Control Remoto)",
		""
		}
	},
	{	"cfgRc.mac_cmd",
		{
		"Cmd to obtain MAC address",
		"",
		"Orden para obtener dirección MAC",
		""
		}
	},
	{	"cfgRc.net",
		{
		"IP assignation",
		"",
		"Asignación de IP",
		""
		}
	},
	{	"cfgRc.netdate",
		{
		"Configure date using network server",
		"",
		"Configurar fecha usando un servidor en red",
		""
		}
	},
	{	"cfgRc.netdate_server",
		{
		"Date network server",
		"",
		"Servidor de fecha en red",
		""
		}
	},
	{	"cfgRc.netmask",
		{
		"Subnet mask (if manual configuration)",
		"Subnetzmaske (bei manueller Konfiguration)",
		"Máscara de red (si conf. manual)",
		""
		}
	},
	{	"cfgRc.pic_tool_flags",
		{
		"pic_tool flags",
		"",
		"Opciones para pic_tool",
		""
		}
	},
	{	"cfgRc.syslogd_flags",
		{
		"syslogd flags",
		"",
		"Opciones para syslogd",
		""
		}
	},
	{	"cfgWi",
		{
		"Setup Webinterface",
		"Einstellungen Webinterface",
		"Configuración del interfaz web",
		"Configuration de l\'interface web"
		}
	},
	{	"cfgWi.always_close_svdrp",
		{
		"Close SVDRP after each cmd (only debug)",
		"",
		"Cerrar SVDRP tras cada comando (sólo depuración)",
		""
		}
	},
	{	"cfgWi.config_change_disabled",
		{
		"Disable configuration modification",
		"",
		"Impedir modificación de la configuración",
		""
		}
	},
	{	"cfgWi.config_view_disabled",
		{
		"Disable configuration view",
		"",
		"Impedir vista de la configuración",
		""
		}
	},
	{	"cfgWi.default language",
		{
		"Default Language",
		"Voreingestellte Sprache",
		"Idioma por defecto",
		"Langue par défaut"
		}
	},
	{	"cfgWi.display_host_id",
		{
		"Display host id",
		"",
		"Mostrar id de host",
		""
		}
	},
	{	"cfgWi.hosts",
		{
		"Hosts",
		"",
		"",
		""
		}
	},
	{	"cfgWi.max_depth",
		{
		"Max. subdir depth",
		"",
		"Subniveles máximos en navegación",
		""
		}
	},
	{	"cfgWi.playlist_type",
		{
		"Playlist type",
		"",
		"Tipo de listas de reproducción",
		""
		}
	},
	{	"cfgWi.rec_deletion_disabled",
		{
		"Disable recording deletion",
		"",
		"Impedir borrado de grabaciones",
		""
		}
	},
	{	"cfgWi.use_external_www_folder",
		{
		"Use external www folder",
		"",
		"Usar carpeta www externa",
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
	{	"langBrowserDefined",
		{
		"Browser language",
		"",
		"Idioma del navegador",
		""
		}
	},
	{	"langEnglish",
		{
		"English",
		"",
		"Inglés",
		""
		}
	},
	{	"langFrench",
		{
		"French",
		"Franzosich",
		"Francés",
		"Français"
		}
	},
	{	"langGerman",
		{
		"German",
		"Deutsch",
		"Alemán",
		""
		}
	},
	{	"langSpanish",
		{
		"Spanish",
		"Spanisch",
		"Español",
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
	{	"recDeletionDisabled",
		{
		"Recording deletion is disabled.",
		"",
		"El borrado de grabaciones está bloqueado",
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
	{	"recErrNoValidPath",
		{
		"No valid recording path present!",	
		"",
		"¡No es una ruta válida de grabación!",
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
	{	"saveConfig",
		{
		"Save configuration",
		"",
		"Guardar configuración",
		""
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
	{	"shortdesc",
		{
		"Subtitle",
		"Untertitel",
		"Subtítulo",
		""
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
	i18n_t test = { s, { s,NULL,NULL,NULL}};
	i18n_t *found=(i18n_t *)bsearch(&test,&i18n,i18nLength,sizeof(i18n_t),i18nCompare);
	if (found==NULL) {
		dbg("i18n:Key %s not found",s);
	}
	const char * result = (found!=NULL) ? (strlen(found->val[langId])>0) ? found->val[langId] : found->val[0] : s;
	return result;
}

const char *formatDate(struct tm *timeptr, boolean_t addHour){
	char * format;
	if (!addHour || asprintf(&format,"%s %%H:%%M",dateFormat[langId])<0) {
		format=(char *)dateFormat[langId];
		addHour=BT_FALSE;
	}
	size_t l = strftime(cldate,sizeof cldate,(const char*)format,timeptr);
	if (addHour) {
		free(format);
	}
	return cldate;
}

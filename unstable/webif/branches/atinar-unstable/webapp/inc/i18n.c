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

const char * const monthNames[I18NNUM][12]={
      {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
      {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"},
      {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"},
      {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"},
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
	{	"action",
		{
		"Action",
		"",
		"Acción",
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
	{	"browser.not_supported",
		{
		"Browser not supported",
		"",
		"Navegador no soportado",
		""
		}
	},
	{	"browser.please_update",
		{
		"Browser not supported, please update it to a more recent version (or a different one...).",
		"",
		"Navegador no soportado, por favor actualízalo o utiliza un navegador diferente...",
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
		"Configuration de l'interface web"
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
	{	"cfgWi.channel_logo_width",
		{
		"Channel logo width",
		"",
		"Ancho del logo de los canales",
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
	{	"cfgWi.default_margin_start",
		{
		"Default margin before",
		"",
		"Margen antes por defecto",
		""
		}
	},
	{	"cfgWi.default_margin_stop",
		{
		"Default margin after",
		"",
		"Margen después por defecto",
		""
		}
	},
	{	"cfgWi.deletion_disabled",
		{
		"Disable deletion",
		"",
		"Desactivar borrado",
		""
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
	{	"cfgWi.epg_grid_hours",
		{
		"Epg schedule grid spanning hours",
		"",
		"Nº de horas en la parrilla de programación",
		""
		}
	},
	{	"cfgWi.eventSearchUrl",
		{
		"Event search URL",
		"",
		"URL para buscar información de eventos",
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
	{	"cfgWi.logos",
		{
		"Add hannel logos file (.tgz)",
		"",
		"Agregar archivo <a class=\"newWindow\" href=\"http://www.assembla.com/spaces/VDR-M7x0/documents/tag/webif\" title=\"Descargar aquí\">"
			"logos-webif-*.tgz"
		"</a>",
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
	{	"cfgWi.password",
		{
		"Password",
		"",
		"Contraseña",
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
	{	"cfgWi.print_rec_folder_summary",
		{
		"Print rec. folder summary",
		"",
		"Mostrar resumen carpetas de grabación",
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
	{	"cfgWi.user",
		{
		"Username",
		"",
		"Usuario",
		""
		}
	},
	{	"cfgWi.video_height",
		{
		"Video height",
		"",
		"Alto de video",
		""
		}
	},
	{	"cfgWi.video_width",
		{
		"Video width",
		"",
		"Ancho de video",
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
	{	"channel.current",
		{
		"Current channel",
		"",
		"Canal actual",
		""
		}
	},
	{	"channel.group",
		{
		"Channel group",
		"",
		"Grupo de canales",
		""
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
	{	"channelFilter",
		{
		"Filter by channel",
		"",
		"Filtrar por canal",
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
	{	"channelMax",
		{
		"Last channel",		
		"",
		"Último canal",
		""
		}
	},
	{	"channelMin",
		{
		"First channel",		
		"",
		"Primer canal",
		""
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
	{	"channels",
		{
		"Channels",
		"Kanäle",
		"Canales",
		"Chaînes"
		}
	},
	{	"channels.filename",
		{
		"channels",
		"kanaele",
		"canales",
		"chaines"
		}
	},
	{	"close",
		{
		"Close",
		"Schliessen",
		"Cerrar",
		""
		}
	},
	{	"command.execute.confirm",
		{
		"Are you sure you want to execute this command?",
		"",
		"¿Seguro que quieres ejecutar la orden?",
		""
		}
	},
	{	"commands",
		{
		"Commands",
		"",
		"Órdenes",
		""
		}
	},
	{	"compareDescription",
		{
		"Compare description",
		"",
		"Comparar descripción",
		""
		}
	},
	{	"compareSubtitle",
		{
		"Compare subtitle",
		"",
		"Comparar subtítulo",
		""
		}
	},
	{	"compareTitle",
		{
		"Compare title",
		"",
		"Comparar título",
		""
		}
	},
	{	"conditionalAccessId",
		{
		"Conditional access ID",
		"Conditional Access ID",
		"ID de acceso condicional",
		"Identifiant d'accès conditionnel"
		}
	},
	{	"create",
		{
		"Create new item",
		"",
		"Crear nuevo item",
		""
		}
	},
	{	"create.err",
		{
		"Failed to create new item!",
		"!",
		"¡Error al crear nuevo item!",
		""
		}
	},
	{	"create.ok",
		{
		"New item added",
		"",
		"Creado nuevo item",
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
	{	"delete",
		{
		"Delete",	
		"Löschen",
		"Borrar",
		""
		}
	},
	{	"delete+",
		{
		"Delete+",
		"",
		"Borrar+",
		""
		}
	},	
	{	"delete.err",
		{
		"Error deleting item!",
		"",
		"¡Error al borrar item!",
		""
		}
	},
	{	"delete.ok",
		{
		"Item deleted",
		"",
		"Item borrado",
		""
		}
	},
	{	"deletionDisabled",
		{
		"Deletion is disabled.",
		"",
		"El borrado está bloqueado",
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
	{	"directory",
		{
		"Directory",
		"",
		"Directorio",
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
	{	"duration",
		{
		"Duration",	
		"",
		"Duración",
		""
		}
	},
	{	"durationFilter",
		{
		"Filter by duration",	
		"",
		"Filtrar por duración",
		""
		}
	},
	{	"durationMax",
		{
		"Max. duration",	
		"",
		"Duración máx.",
		""
		}
	},
	{	"durationMin",
		{
		"Min. duration",	
		"",
		"Duración mín.",
		""
		}
	},
	{	"edit",
		{
		"Edit",
		"Bearbeiten",
		"Editar",
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
	{	"epg",
		{
		"Schedule",
		"Programm",
		"Guía de Programación",
		"Programmation"
		}
	},
	{	"epg.grid",
		{
		"Program",
		"Sendung",
		"Parrilla TV",
		"Émission"
		}
	},
	{	"epg.list",
		{
		"Program list",
		"Sendung",
		"Listado",
		"Émission"
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
			"<span class=\"start\"><span class=\"l\">From&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"end\"><span class=\"l\">to&nbsp;</span><span class=\"t\">%02d:%02d</span> "
			"<span class=\"duration\">(<span class=\"l\">Runtime</span>: approx. <span class=\"t\">%d</span> minutes)</span>"
		"</span>\n"
		,
		"<span class=\"fromTo\">"
			"<span class=\"start\"><span class=\"l\">Von&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"end\"><span class=\"l\">bis&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"duration\">(<span class=\"l\">Dauer</span>: ca. <span class=\"t\">%d</span> Minuten)</span>"
		"</span>\n"
		,
		"<span class=\"fromTo\">"
			"<span class=\"start\"><span class=\"l\">Inicio&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"end\"><span class=\"l\">Final&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"duration\">(<span class=\"l\">Duración</span>: <span class=\"t\">%d</span> minutos aprox.)</span>"
		"</span>\n"
		,
		"<span class=\"fromTo\">"
			"<span class=\"start\"><span class=\"l\">De&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"end\"><span class=\"l\">à&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"duration\">(<span class=\"l\">Durée</span>: approximativement <span class=\"t\">%d</span> minutes)</span>"
		"</span>\n"
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
		"Page d'accueil"
		}
	},
	{	"interval",
		{
		"Interval",
		"",
		"Intervalo",
		""
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
		"Tiempo de vida",
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
	{	"margin.add",
		{
		"Add start/stop margin",
		"Vor-/Nachlaufzeit hinzufügen",
		"Añadir tiempo antes/después",
		""
		}
	},
	{	"margin.after",
		{
		"Margin after",
		"Nachlaufzeit",
		"Margen después",
		""
		}
	},
	{	"margin.before",
		{
		"Margin before",
		"Vorlaufzeit",
		"Margen antes",
		""
		}
	},
	{	"maxAllowed",
		{
		"Máx. allowed",
		"",
		"Permitir máximo",
		""
		}
	},
	{	"mode",
		{
		"Mode",
		"",
		"Modo",
		""
		}
	},
	{	"moreInfo",
		{
		"More infos",
		"Mehr Infos",
		"Más info",
		"Plus d'informations"
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
	{	"noLogosErrTitle",
		{
		"No logos found!",
		"",
		"No se han encontrado logos",
		""
		}
	},
	{	"noLogosErrMessage",
		{
		"No logos found!",
		"",
		"<p>No se ha encontrado el directorio con los logotipos de los canales en tu sistema.</p>"
		"<p>Descarga el archivo de logos apropiado para tu localidad e instálalo a través de este formulario.</p>",
		""
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
		"Pausa",
		"Pausa",
		}
	},
	{	"phrase",
		{
		"Phrase",
		"",
		"Frase",
		"",
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
	{	"playAll",
		{
		"Play all",
		"",
		"Reproducir todo",
		"",
		}
	},
	{	"playlist",
		{
		"Playlist",
		"",
		"Lista de reproducción",
		"",
		}
	},
	{	"playlist.download",
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
		"Priorität",
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
	{	"rec.delete.confirm",
		{
		"Are you sure you want to delete the record?",
		"Bist du sicher, dass du den Aufzeichnen löschen willst?",
		"¿Seguro que quieres borrar la grabación?",
		""
		}
	},	
	{	"rec.delete.err",
		{
		"Failed to delete record!",
		"Aufzeichnen konnte nicht gelöscht werden!",
		"¡Error al borrar la grabación!",
		""
		}
	},
	{	"rec.delete.ok",
		{
		"Record deleted",
		"Aufzeichnen gelöscht",
		"Grabación borrada",
		""
		}
	},
	{	"rec.edit",
		{
		"Edit recording",
		"Aufzeichnen bearbeiten",
		"Editar grabación",
		""
		}
	},
	{	"rec.edit.err",
		{
		"Failed to update recording!",	
		"Aufzeichnen konnte nicht aktualisiert werden!",
		"¡Error al actualizar la grabación!",
		""
		}
	},
	{	"rec.edit.ok",
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
	{	"recording.filename",
		{
		"recording",
		"aufzeichnung",
		"grabacion",
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
	{	"regex",
		{
		"Regular expression",
		"",
		"Expresión regular",
		""
		}
	},
	{	"regularRecording",
		{
		"Regular recording",
		"Regelmä&szlig;ige Aufnahme",
		"Grabar como serie",
		""
		}
	},
	{	"repeats",
		{
		"Repeats",
		"",
		"Repeticiones",
		""
		}
	},
	{	"repeats.avoid",
		{
		"Avoid repeats",
		"",
		"Evitar repeticiones",
		""
		}
	},
	{	"repeats.maxAllowed",
		{
		"Repeats allowed",
		"",
		"Repeticiones permitidas",
		""
		}
	},
	{	"revert",
		{
		"Revert",
		"",
		"Revertir",
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
	{	"search",
		{
		"Search",
		"",
		"Buscar",
		""
		}
	},
	{	"search.allWords",
		{
		"All words",
		"",
		"Todas las palabras",
		""
		}
	},
	{	"search.announce",
		{
		"Announce only",
		"",
		"Sólo avisar",
		""
		}
	},
	{	"search.atLeastOne",
		{
		"At least one word",
		"",
		"Al menos una palabra",
		""
		}
	},
	{	"search.create",
		{
		"Create EPG search",
		"EPG Suche erstellen",
		"Crear búsqueda EPG",
		""
		}
	},
	{	"search.create.err",
		{
		"Failed to add search!",
		"Suche konnte nicht hinzugefügt werden!",
		"¡Error al añadir la búsqueda!",
		""
		}
	},
	{	"search.create.ok",
		{
		"Search added",
		"Suche hinzugefügt",
		"Búsqueda añadida",
		""
		}
	},
	{	"search.delAfterDays",
		{
		"Delete recordings after days",
		"",
		"Borrar grabaciones tras días",
		""
		}
	},	
	{	"search.delete+.title",
		{
		"Delete search and associated timers",
		"",
		"Borrar búsqueda y programaciones asociadas",
		""
		}
	},	
	{	"search.delete.confirm",
		{
		"Are you sure you want to delete the search?",
		"Bist du sicher, dass du den Suche löschen willst?",
		"¿Seguro que quieres borrar la búsqueda?",
		""
		}
	},	
	{	"search.edit",
		{
		"Edit search",
		"Suche bearbeiten",
		"Editar búsqueda",
		""
		}
	},
	{	"search.err.nothingToCompareWith",
		{
		"Specify where to search for text",
		"",
		"Especifica dónde se debe buscar el texto",
		""
		}
	},
	{	"search.err.nothingToSearchFor",
		{
		"Specify text to search for",
		"",
		"Especifica texto a buscar",
		""
		}
	},
	{	"search.exactMatch",
		{
		"Exact match",
		"",
		"Coincidencia exacta",
		""
		}
	},
	{	"search.fuzzy",
		{
		"Fuzzy match",
		"",
		"Coincidencia aproximada",
		""
		}
	},
	{	"search.switch",
		{
		"Switch only",
		"",
		"Sólo cambiar",
		""
		}
	},
	{	"search.switch.minsBefore",
		{
		"Switch min. before",
		"",
		"Cambiar min. antes",
		""
		}
	},
	{	"search.switch.unmuteSound",
		{
		"Unmute sound on switch",
		"",
		"Quitar silencio al cambiar",
		""
		}
	},
	{	"search.timeFilter",
		{
		"Time filter",
		"",
		"Filtrar por tiempo",
		""
		}
	},
	{	"search.useAsSearchTimer",
		{
		"Use as search timer",
		"",
		"Usar como programación por búsqueda",
		""
		}
	},
	{	"search.useEpisode",
		{
		"Series recording",
		"",
		"Grabación en serie",
		""
		}
	},
	{	"search.useExtEpgInfo",
		{
		"Use extended EPG info",
		"",
		"Filtrar por categorías EPG",
		""
		}
	},
	{	"searches",
		{
		"Searches",
		"",
		"Búsquedas",
		""
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
	{	"startAfter",
		{
		"Start after",
		"",
		"Inicio después de",
		""
		}
	},
	{	"startBefore",
		{
		"Start before",
		"",
		"Inicio antes de",
		""
		}
	},
	{	"startFilter",
		{
		"Filter by start",
		"",
		"Filtrar por hora de inicio",
		""
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
	{	"timer.add",
		{
		"New timer",
		"Neuer Timer",
		"Nueva programación",
		"Nouvel enregistrement programmé"
		}
	},
	{	"timer.check",
		{
		"Check timer",
		"Angaben prüfen",
		"Comprobar programación",
		""
		}
	},
	{	"timer.create",
		{
		"Create timer",
		"Timer erstellen",
		"Programar grabación",
		"Création d'un enregistrement programmé"
		}
	},
	{	"timer.create.err",
		{
		"Failed to add timer!",
		"Timer konnte nicht hinzugefügt werden!",
		"¡Error al añadir la programación!",
		"Erreur de programmation de l'enregistrement!"
		}
	},
	{	"timer.create.ok",
		{
		"Timer added",
		"Timer hinzugefügt",
		"Programación añadida",
		"Enregistrement programmé"
		}
	},
	{	"timer.delete",
		{
		"Delete timer",	
		"Timer löschen",
		"Borrar programación",
		""
		}
	},
	{	"timer.delete.confirm",
		{
		"Are you sure you want to delete the timer?",
		"Bist du sicher, dass du den Timer löschen willst?",
		"¿Seguro que quieres borrar la programación?",
		""
		}
	},	
	{	"timer.delete.err",
		{
		"Failed to delete timer!",
		"Timer konnte nicht gelöscht werden!",
		"¡Error al borrar la programación!",
		""
		}
	},
	{	"timer.delete.ok",
		{
		"Timer deleted",
		"Timer gelöscht",
		"Programación borrada",
		""
		}
	},
	{	"timer.edit",
		{
		"Edit timer",
		"Timer bearbeiten",
		"Editar programación",
		"Modification d'un enregistrement"
		}
	},
	{	"timer.update.err",
		{
		"Failed to update timer!",	
		"Timer konnte nicht aktualisiert werden!",
		"¡Error actualizando programación!",
		""
		}
	},
	{	"timer.update.ok",
		{
		"Timer updated",
		"Timer aktualisiert",
		"Programación actualizada",
		""
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
	{	"tolerance",
		{
		"Tolerance",
		"",
		"Tolerancia",
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
	{	"update.err",
		{
		"Error updating!",	
		"",
		"¡Error actualizando!",
		""
		}
	},
	{	"update.ok",
		{
		"Updated",
		"",
		"Actualización realizada",
		""
		}
	},
	{	"useCase",
		{
		"Compare case",
		"",
		"Distinguir mayúsculas",
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
		"<p>EPG ist not available, please try again later.</p>"
		"<p>If the problem persists, this channel either has no EPG or it is broken. Feel free to get in touch with someone who is in charge for this channel.</p>\n"
		,
		"<p>EPG ist nicht verfügbar.</p>"
		"<p>Bitte versuchen sie es später nocheinmal. Falls dieses Problem über längere Zeit besteht, wenden sie sich bitte an den Sender.</p>\n"
		,
		"<p>EPG no disponible, por favor inténtelo más tarde.</p>"
		"<p>Si el problema persiste, éste canal no tiene EPG o funciona incorrectamente.</p>\n"
		,
		"<p>L'EPG (guide des programmes) n'est pas disponible, merci d'essayer ultérieurement.</p>"
		"<p>Si le problème persiste, cette chaîne n'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-être essayer de prendre contact avec un responsable de cette chaîne.</p>\n"
		}
	},
	{	"warnSvdrpConnection",
		{
		"<div>"
			"<p><strong>Failed to open a SVDRP connection to the VDR</strong>. Possible Reasons:</p>"
			"<ul>"
				"<li>The VDR is not running.</li>"
				"<li>The SVDRP connection is still in use by another client like vdradmin-AM</li>"
				"<li>An error occured while processing an earlier request and now the SVDRP connection is blocked.</li>"
			"</ul>"
			"<p>Please check if the VDR is running or try it again later.</p>"
		"</div>\n"
		,
		"<div>"
			"<p><strong>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gründe dafür:</p>"
			"<ul>"
				"<li>Der VDR wurde nicht gestartet bzw. läuft nicht mehr.</li>"
				"<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>"
				"<li>Es gab bei einer früheren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li>"
			"</ul>"
			"<p>Bitte überprüfen sie, dass der VDR läuft und versuchen sie es ggf. später noch einmal.</p>"
		"</div>\n"
		,
		"<div>"
			"<p><strong>No se puede abrir una conexión SVDRP con el VDR. Posibles motivos::</p>"
			"<ul>"
				"<li>El VDR no se está ejecutando.</li>"
				"<li>La conexión SVDRP está todavía en uso por otro cliente como vdradmin-AM</li>"
				"<li>Se ha producido un error mientras se procesó una petición anterior y ahora la conexión SVDRP está bloqueada.</li>"
			"</ul>"
			"<p>Por favor, compruebe si el VDR se está ejecutando o inténtelo más tarde.</p>"
		"</div>\n"
		,
		"<div>"
			"<p><strong>Echec d'accès par connection SVDRP au logiciel VDR. Raisons possibles:</p>"
			"<ul>"
				"<li>VDR n'est pas démarré.</li>"
				"<li>La connexion SVDRP est encore utilisée par un autre client comme \"vdradmin\".</li>"
				"<li>la connexion SVDRP est bloqué suite à une erreur précédente</li>"
			"</ul>"
			"<p>Vérifier que VDR est bien lancé ou essayer ultérieurement.</p>"
		"</div>\n"
		}
	},
	{	"wday",
		{
		"Weekday",
		"",
		"Día de la semana",
		""
		}
	},
	{	"wdayFilter",
		{
		"Weekday filter",
		"",
		"Filtrar por día",
		""
		}
	},
	{	"web.search",
		{
		"Web search",
		"Im Internet suchen",
		"Buscar en Internet",
		""
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
		warn("Null key found");
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
	i18n_t test={s,{s,NULL,NULL,NULL}};
	i18n_t *found=NULL;
	while (!found && test.key) {
		found=(i18n_t *)bsearch(&test,&i18n,i18nLength,sizeof(i18n_t),i18nCompare);
		if (!found) {
			char *dot=strchr(test.key,'.');
			if (dot && dot[1]) {
				test.key=dot+1;
				test.val[0]=dot+1;
			} else {
				test.key=NULL;
			}
		}
	}
	//if (!found) dbg("i18n:Key %s not found",s);
	const char * result = (found!=NULL) ? (strlen(found->val[langId])>0) ? found->val[langId] : found->val[0] : s;
	return result;
}

const char *formatDate(struct tm *timeptr, bool addHour){
	char * format;
	if (!addHour || asprintf(&format,"%s %%H:%%M",dateFormat[langId])<0) {
		format=(char *)dateFormat[langId];
		addHour=false;
	}
	size_t l = strftime(cldate,sizeof cldate,(const char*)format,timeptr);
	if (addHour) {
		free(format);
	}
	return cldate;
}

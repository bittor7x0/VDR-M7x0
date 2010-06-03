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
		{"Dom","Lun","Mar","Mi�","Jue","Vie","S�b"},
		{"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"}
};
const char * const weekdays[I18NNUM][7]={
		{"Sunday" ,"Monday","Tuesday" ,"Wednesday","Thursday"   ,"Friday" ,"Saturday"},
		{"Sonntag","Montag","Dienstag","Mittwoch" ,"Donnerstag","Freitag","Samstag"},
		{"Domingo" ,"Lunes" ,"Martes"  ,"Mi�rcoles","Jueves"   ,"Viernes" ,"S�bado"},
		{"Dimanche","Lundi" ,"Mardi"   ,"Mercredi" ,"Jeudi"    ,"Vendredi","Samedi"}
};

const char * const monthNames[I18NNUM][12]={
      {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
      {"Januar", "Februar", "M�rz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"},
      {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"},
      {"Janvier", "F�vrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Ao�t", "Septembre", "Octobre", "Novembre", "D�cembre"},
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
	"Espa�ol",
	"Fran�ais"
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
		"Acci�n",
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
		"A�adir",
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
		"Men� de arranque",
		""
		}
	},
	{	"browse",
		{
		"Browse",
		"Schm�kern",
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
		"Navegador no soportado, por favor actual�zalo o utiliza un navegador diferente...",
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
		"Configuraci�n del BoxAmp",
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
		"R�pertoire des fichiers audio"
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
		"R�pertoire de BoxAmp"
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
		"Valor del par�metro (%s) incorrecto",
		""
		}
	},
	{	"cfgErrParamNameValueMismatch",
		{
		"Mismatch between name and value numbers",
		"",
		"El n�mero de nombres y valores de par�metros no coincide.",
		""
		}
	},
	{	"cfgFileChangeDisabledErr",
		{
		"Change of configuration is disabled.",
		"",
		"La modificaci�n de la configuraci�n est� bloqueda.",
		""
		}
	},
	{	"cfgFileDoesNotExistFmt",
		{
		"Configuration file #%d ( %s ) doesn't exists",
		"Konfigurationsdatei #%d ( %s ) nicht vorhanden",
		"El archivo de configuraci�n #%d ( %s ) no existe",
		""
		}
	},
	{	"cfgFileIsNew",
		{
		"Configuration file will be created now.",
		"",
		"El archivo de configuraci�n se crear� ahora.",
		""
		}
	},
	{	"cfgFileSaveErrFmt",
		{
		"Error saving configuration file %s .",
		"",
		"Ha habido alg�n error al guardar el archivo de configuraci�n %s.",
		""
		}
	},
	{	"cfgFileSaveOkFmt",
		{
		"Configuration file %s saved.",
		"",
		"El archivo de configuraci�n %s se ha guardado correctamente.",
		""
		}
	},
	{	"cfgRc",
		{
		"VDR Network configuration",
		"VDR Einstellungen Netz",
		"Configuraci�n de red del VDR",
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
		"Direcci�n IP (si conf. manual)",
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
		"Orden para obtener direcci�n MAC",
		""
		}
	},
	{	"cfgRc.net",
		{
		"IP assignation",
		"",
		"Asignaci�n de IP",
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
		"M�scara de red (si conf. manual)",
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
		"Configuraci�n del interfaz web",
		"Configuration de l'interface web"
		}
	},
	{	"cfgWi.always_close_svdrp",
		{
		"Close SVDRP after each cmd (only debug)",
		"",
		"Cerrar SVDRP tras cada comando (s�lo depuraci�n)",
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
		"Impedir modificaci�n de la configuraci�n",
		""
		}
	},
	{	"cfgWi.config_view_disabled",
		{
		"Disable configuration view",
		"",
		"Impedir vista de la configuraci�n",
		""
		}
	},
	{	"cfgWi.default language",
		{
		"Default Language",
		"Voreingestellte Sprache",
		"Idioma por defecto",
		"Langue par d�faut"
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
		"Margen despu�s por defecto",
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
		"N� de horas en la parrilla de programaci�n",
		""
		}
	},
	{	"cfgWi.eventSearchUrl",
		{
		"Event search URL",
		"",
		"URL para buscar informaci�n de eventos",
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
		"Agregar archivo <a class=\"newWindow\" href=\"http://www.assembla.com/spaces/VDR-M7x0/documents/tag/webif\" title=\"Descargar aqu�\">"
			"logos-webif-*.tgz"
		"</a>",
		""
		}
	},
	{	"cfgWi.max_depth",
		{
		"Max. subdir depth",
		"",
		"Subniveles m�ximos en navegaci�n",
		""
		}
	},
	{	"cfgWi.password",
		{
		"Password",
		"",
		"Contrase�a",
		""
		}
	},
	{	"cfgWi.playlist_type",
		{
		"Playlist type",
		"",
		"Tipo de listas de reproducci�n",
		""
		}
	},
	{	"cfgWi.print_rec_folder_summary",
		{
		"Print rec. folder summary",
		"",
		"Mostrar resumen carpetas de grabaci�n",
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
		"Cha�ne"
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
		"Liste des cha�nes"
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
		"�ltimo canal",
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
		"Nom de cha�ne"
		}
	},
	{	"channelNumber",
		{
		"Channel number",	
		"Programmplatz",
		"N�mero del canal",
		"Num�ro de cha�ne"
		}
	},
	{	"channels",
		{
		"Channels",
		"Kan�le",
		"Canales",
		"Cha�nes"
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
		"�Seguro que quieres ejecutar la orden?",
		""
		}
	},
	{	"commands",
		{
		"Commands",
		"",
		"�rdenes",
		""
		}
	},
	{	"compareDescription",
		{
		"Compare description",
		"",
		"Comparar descripci�n",
		""
		}
	},
	{	"compareSubtitle",
		{
		"Compare subtitle",
		"",
		"Comparar subt�tulo",
		""
		}
	},
	{	"compareTitle",
		{
		"Compare title",
		"",
		"Comparar t�tulo",
		""
		}
	},
	{	"conditionalAccessId",
		{
		"Conditional access ID",
		"Conditional Access ID",
		"ID de acceso condicional",
		"Identifiant d'acc�s conditionnel"
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
		"�Error al crear nuevo item!",
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
		"L�schen",
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
		"�Error al borrar item!",
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
		"El borrado est� bloqueado",
		""
		}
	},
	{	"desc",
		{
		"Description",
		"",
		"Descripci�n",
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
		"Duraci�n",
		""
		}
	},
	{	"durationFilter",
		{
		"Filter by duration",	
		"",
		"Filtrar por duraci�n",
		""
		}
	},
	{	"durationMax",
		{
		"Max. duration",	
		"",
		"Duraci�n m�x.",
		""
		}
	},
	{	"durationMin",
		{
		"Min. duration",	
		"",
		"Duraci�n m�n.",
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
		"Gu�a de Programaci�n",
		"Programmation"
		}
	},
	{	"epg.grid",
		{
		"Program",
		"Sendung",
		"Parrilla TV",
		"�mission"
		}
	},
	{	"epg.list",
		{
		"Program list",
		"Sendung",
		"Listado",
		"�mission"
		}
	},
	{	"execute",
		{
		"Execute",	
		"Ausf�hren",
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
		"F�r",
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
		"Fr�quence"
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
			"<span class=\"duration\">(<span class=\"l\">Duraci�n</span>: <span class=\"t\">%d</span> minutos aprox.)</span>"
		"</span>\n"
		,
		"<span class=\"fromTo\">"
			"<span class=\"start\"><span class=\"l\">De&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"end\"><span class=\"l\">�&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
			"<span class=\"duration\">(<span class=\"l\">Dur�e</span>: approximativement <span class=\"t\">%d</span> minutes)</span>"
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
		"Plein �cran"
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
		"P�gina inicial",
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
		"Direcci�n IP",
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
		"Ingl�s",
		""
		}
	},
	{	"langFrench",
		{
		"French",
		"Franzosich",
		"Franc�s",
		"Fran�ais"
		}
	},
	{	"langGerman",
		{
		"German",
		"Deutsch",
		"Alem�n",
		""
		}
	},
	{	"langSpanish",
		{
		"Spanish",
		"Spanisch",
		"Espa�ol",
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
		"Vor-/Nachlaufzeit hinzuf�gen",
		"A�adir tiempo antes/despu�s",
		""
		}
	},
	{	"margin.after",
		{
		"Margin after",
		"Nachlaufzeit",
		"Margen despu�s",
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
		"M�x. allowed",
		"",
		"Permitir m�ximo",
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
		"M�s info",
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
		"Identifiant de r�seau"
		}
	},
	{	"networkShares",
		{
		"Network shares",
		"Netzlaufwerke",
		"Recursos de red",
		"Partitions r�seau"
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
		"<p>Descarga el archivo de logos apropiado para tu localidad e inst�lalo a trav�s de este formulario.</p>",
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
		"Das l�uft jetzt im TV",
		"Ahora en TV",
		"En direct"
		}
	},
	{	"number",
		{
		"Number",	
		"Platz",
		"N�mero",
		"Num�ro"
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
		"Par�metros",
		"Param�tres"
		}
	},
	{	"partition",
		{
		"Partition",
		"",
		"Partici�n",
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
		"Lista de reproducci�n",
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
		"Priorit�t",
		"Prioridad",
		""
		}
	},
	{	"program",
		{
		"Program",
		"Sendung",
		"Emisi�n",
		"�mission"
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
		"Bist du sicher, dass du den Aufzeichnen l�schen willst?",
		"�Seguro que quieres borrar la grabaci�n?",
		""
		}
	},	
	{	"rec.delete.err",
		{
		"Failed to delete record!",
		"Aufzeichnen konnte nicht gel�scht werden!",
		"�Error al borrar la grabaci�n!",
		""
		}
	},
	{	"rec.delete.ok",
		{
		"Record deleted",
		"Aufzeichnen gel�scht",
		"Grabaci�n borrada",
		""
		}
	},
	{	"rec.edit",
		{
		"Edit recording",
		"Aufzeichnen bearbeiten",
		"Editar grabaci�n",
		""
		}
	},
	{	"rec.edit.err",
		{
		"Failed to update recording!",	
		"Aufzeichnen konnte nicht aktualisiert werden!",
		"�Error al actualizar la grabaci�n!",
		""
		}
	},
	{	"rec.edit.ok",
		{
		"Recording updated",
		"Aufzeichnen aktualisiert",
		"Grabaci�n actualizada",
		""
		}
	},
	{	"recErrNoValidPath",
		{
		"No valid recording path present!",	
		"",
		"�No es una ruta v�lida de grabaci�n!",
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
		"Grabaci�n",
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
		"Liste der aktuell verf�gbaren Aufzeichnungen.",
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
		"Expresi�n regular",
		""
		}
	},
	{	"regularRecording",
		{
		"Regular recording",
		"Regelm�&szlig;ige Aufnahme",
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
		"Duraci�n",
		"dur�e"
		}
	},
	{	"saveConfig",
		{
		"Save configuration",
		"",
		"Guardar configuraci�n",
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
		"S�lo avisar",
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
		"Crear b�squeda EPG",
		""
		}
	},
	{	"search.create.err",
		{
		"Failed to add search!",
		"Suche konnte nicht hinzugef�gt werden!",
		"�Error al a�adir la b�squeda!",
		""
		}
	},
	{	"search.create.ok",
		{
		"Search added",
		"Suche hinzugef�gt",
		"B�squeda a�adida",
		""
		}
	},
	{	"search.delAfterDays",
		{
		"Delete recordings after days",
		"",
		"Borrar grabaciones tras d�as",
		""
		}
	},	
	{	"search.delete+.title",
		{
		"Delete search and associated timers",
		"",
		"Borrar b�squeda y programaciones asociadas",
		""
		}
	},	
	{	"search.delete.confirm",
		{
		"Are you sure you want to delete the search?",
		"Bist du sicher, dass du den Suche l�schen willst?",
		"�Seguro que quieres borrar la b�squeda?",
		""
		}
	},	
	{	"search.edit",
		{
		"Edit search",
		"Suche bearbeiten",
		"Editar b�squeda",
		""
		}
	},
	{	"search.err.nothingToCompareWith",
		{
		"Specify where to search for text",
		"",
		"Especifica d�nde se debe buscar el texto",
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
		"S�lo cambiar",
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
		"Usar como programaci�n por b�squeda",
		""
		}
	},
	{	"search.useEpisode",
		{
		"Series recording",
		"",
		"Grabaci�n en serie",
		""
		}
	},
	{	"search.useExtEpgInfo",
		{
		"Use extended EPG info",
		"",
		"Filtrar por categor�as EPG",
		""
		}
	},
	{	"searches",
		{
		"Searches",
		"",
		"B�squedas",
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
		"Configuraci�n",
		"Configuration"
		}
	},
	{	"shortdesc",
		{
		"Subtitle",
		"Untertitel",
		"Subt�tulo",
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
		"Inicio despu�s de",
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
		"�bersicht",
		"Resumen",
		"R�sum�"
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
		"Nueva programaci�n",
		"Nouvel enregistrement programm�"
		}
	},
	{	"timer.check",
		{
		"Check timer",
		"Angaben pr�fen",
		"Comprobar programaci�n",
		""
		}
	},
	{	"timer.create",
		{
		"Create timer",
		"Timer erstellen",
		"Programar grabaci�n",
		"Cr�ation d'un enregistrement programm�"
		}
	},
	{	"timer.create.err",
		{
		"Failed to add timer!",
		"Timer konnte nicht hinzugef�gt werden!",
		"�Error al a�adir la programaci�n!",
		"Erreur de programmation de l'enregistrement!"
		}
	},
	{	"timer.create.ok",
		{
		"Timer added",
		"Timer hinzugef�gt",
		"Programaci�n a�adida",
		"Enregistrement programm�"
		}
	},
	{	"timer.delete",
		{
		"Delete timer",	
		"Timer l�schen",
		"Borrar programaci�n",
		""
		}
	},
	{	"timer.delete.confirm",
		{
		"Are you sure you want to delete the timer?",
		"Bist du sicher, dass du den Timer l�schen willst?",
		"�Seguro que quieres borrar la programaci�n?",
		""
		}
	},	
	{	"timer.delete.err",
		{
		"Failed to delete timer!",
		"Timer konnte nicht gel�scht werden!",
		"�Error al borrar la programaci�n!",
		""
		}
	},
	{	"timer.delete.ok",
		{
		"Timer deleted",
		"Timer gel�scht",
		"Programaci�n borrada",
		""
		}
	},
	{	"timer.edit",
		{
		"Edit timer",
		"Timer bearbeiten",
		"Editar programaci�n",
		"Modification d'un enregistrement"
		}
	},
	{	"timer.update.err",
		{
		"Failed to update timer!",	
		"Timer konnte nicht aktualisiert werden!",
		"�Error actualizando programaci�n!",
		""
		}
	},
	{	"timer.update.ok",
		{
		"Timer updated",
		"Timer aktualisiert",
		"Programaci�n actualizada",
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
		"Enregistrements programm�s"
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
		"T�tulo",
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
		"Canales de televisi�n",
		""
		}
	},
	{	"update.err",
		{
		"Error updating!",	
		"",
		"�Error actualizando!",
		""
		}
	},
	{	"update.ok",
		{
		"Updated",
		"",
		"Actualizaci�n realizada",
		""
		}
	},
	{	"useCase",
		{
		"Compare case",
		"",
		"Distinguir may�sculas",
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
		"Configuraci�n VDR",
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
		"<p>EPG ist nicht verf�gbar.</p>"
		"<p>Bitte versuchen sie es sp�ter nocheinmal. Falls dieses Problem �ber l�ngere Zeit besteht, wenden sie sich bitte an den Sender.</p>\n"
		,
		"<p>EPG no disponible, por favor int�ntelo m�s tarde.</p>"
		"<p>Si el problema persiste, �ste canal no tiene EPG o funciona incorrectamente.</p>\n"
		,
		"<p>L'EPG (guide des programmes) n'est pas disponible, merci d'essayer ult�rieurement.</p>"
		"<p>Si le probl�me persiste, cette cha�ne n'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-�tre essayer de prendre contact avec un responsable de cette cha�ne.</p>\n"
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
			"<p><strong>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gr�nde daf�r:</p>"
			"<ul>"
				"<li>Der VDR wurde nicht gestartet bzw. l�uft nicht mehr.</li>"
				"<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>"
				"<li>Es gab bei einer fr�heren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li>"
			"</ul>"
			"<p>Bitte �berpr�fen sie, dass der VDR l�uft und versuchen sie es ggf. sp�ter noch einmal.</p>"
		"</div>\n"
		,
		"<div>"
			"<p><strong>No se puede abrir una conexi�n SVDRP con el VDR. Posibles motivos::</p>"
			"<ul>"
				"<li>El VDR no se est� ejecutando.</li>"
				"<li>La conexi�n SVDRP est� todav�a en uso por otro cliente como vdradmin-AM</li>"
				"<li>Se ha producido un error mientras se proces� una petici�n anterior y ahora la conexi�n SVDRP est� bloqueada.</li>"
			"</ul>"
			"<p>Por favor, compruebe si el VDR se est� ejecutando o int�ntelo m�s tarde.</p>"
		"</div>\n"
		,
		"<div>"
			"<p><strong>Echec d'acc�s par connection SVDRP au logiciel VDR. Raisons possibles:</p>"
			"<ul>"
				"<li>VDR n'est pas d�marr�.</li>"
				"<li>La connexion SVDRP est encore utilis�e par un autre client comme \"vdradmin\".</li>"
				"<li>la connexion SVDRP est bloqu� suite � une erreur pr�c�dente</li>"
			"</ul>"
			"<p>V�rifier que VDR est bien lanc� ou essayer ult�rieurement.</p>"
		"</div>\n"
		}
	},
	{	"wday",
		{
		"Weekday",
		"",
		"D�a de la semana",
		""
		}
	},
	{	"wdayFilter",
		{
		"Weekday filter",
		"",
		"Filtrar por d�a",
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
		"S�",
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

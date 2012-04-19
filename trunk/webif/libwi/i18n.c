/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 * Thanks for their help with the translation to:
 * - Videre
 * - Thomas Brinkmann
 */

#include <string.h>

#include "i18n.h"

langId_t langId = I18N_BROWSER_SELECT;
char cldate[20];

// startOfWeek holds the index of the first weekday of the week.
// for example 0=Sunday for US, 1=Monday for Germany and so on
const int startOfWeek[I18N_NUM] = {0, 1, 1, 1};
const char * const shortWeekdays[I18N_NUM][7] = {
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
    {"Son", "Mon", "Die", "Mit", "Don", "Fre", "Sam"},
    {"Dom", "Lun", "Mar", "Mi�", "Jue", "Vie", "S�b"},
    {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"}
};
const char * const weekdays[I18N_NUM][7] = {
    {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
    {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"},
    {"Domingo", "Lunes", "Martes", "Mi�rcoles", "Jueves", "Viernes", "S�bado"},
    {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"}
};

const char * const monthNames[I18N_NUM][12] = {
    {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
    {"Januar", "Februar", "M�rz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"},
    {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"},
    {"Janvier", "F�vrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Ao�t", "Septembre", "Octobre", "Novembre", "D�cembre"},
};

const i18nVal_t locale = {
    "en_GB",
    "de_DE",
    "es_ES",
    "fr_FR"
};

const i18nVal_t language = {
    "English",
    "Deutsch",
    "Espa�ol",
    "Fran�ais"
};

//Alpha-2 Code as described in ISO 639-1: http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
const i18nVal_t alpha2 = {
    "en",
    "de",
    "es",
    "fr"
};

//3 Alpha-3 Code as described in ISO 639-2: http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes
const i18nVal_t alpha3 = {
    "eng",
    "ger",
    "spa",
    "fra"
};


const i18nVal_t dateFormat = {
    "%d/%m/%Y",
    "%d.%m.%Y",
    "%d/%m/%Y",
    "%d-%m-%Y"
};


// Important: keep i18n ordered by key. Otherwise binary search will fail.
i18n_t i18n[] = {
    { "accept",
        {
            "Accept",
            "Akzeptieren",
            "Aceptar",
            ""
        }},
    { "action",
        {
            "Action",
            "Aktion",
            "Acci�n",
            ""
        }},
    { "active",
        {
            "Active",
            "Aktiv",
            "Activo",
            ""
        }},
    { "add",
        {
            "Add",
            "Eintragen",
            "A�adir",
            ""
        }},
    { "aux",
        {
            "Aux.",
            "",
            "",
            ""
        }},
    { "available",
        {
            "Available",
            "Verf�gbar",
            "Disponible",
            ""
        }},
    { "bootMenu",
        {
            "Boot Menu",
            "Boot-Men�",
            "Men� de arranque",
            ""
        }},
    { "browse",
        {
            "Browse",
            "Schm�kern",
            "Navegar",
            ""
        }},
    { "browser.not_supported",
        {
            "Browser not supported",
            "Browser nicht unterst�tzt",
            "Navegador no soportado",
            ""
        }},
    { "browser.please_update",
        {
            "Browser not supported, please update it to a more recent version (or a different one...).",
            "Browser nicht unterst�tzt, bitte aktualisieren (oder einen anderen Browser verwenden...)",
            "Navegador no soportado, por favor actual�zalo o utiliza un navegador diferente...",
            ""
        }},
    { "cancel",
        {
            "Cancel",
            "Abbrechen",
            "Cancelar",
            ""
        }},
    { "cfgBa",
        {
            "Setup BoxAmp",
            "Einstellungen BoxAmp",
            "Configuraci�n del BoxAmp",
            "Configuration de BoxAmp"
        }},
    { "cfgErrInvalidParamValue%s",
        {
            "Invalid param value:<br/><span class=\"wrongValue\">%s</span>",
            "Ung�ltiger Parameter-Wert: <br/><span class=\"wrongValue\">%s</span>",
            "Valor del par�metro incorrecto:<br/><span class=\"wrongValue\">%s</span>",
            ""
        }},
    { "cfgErrParamNameValueMismatch",
        {
            "Mismatch between name and value numbers",
            "Name und Parameterwert passen nicht zueinander.",
            "El n�mero de nombres y valores de par�metros no coincide.",
            ""
        }},
    { "cfgFileChangeDisabledErr",
        {
            "Change of configuration is disabled.",
            "Das �ndern der Konfiguration ist deaktiviert.",
            "La modificaci�n de la configuraci�n est� bloqueada.",
            ""
        }},
    { "cfgFileDoesNotExist%s",
        {
            "Configuration file %s doesn't exist",
            "Konfigurationsdatei %s nicht vorhanden",
            "El archivo de configuraci�n %s no existe",
            ""
        }},
    { "cfgFileIsNew_%d_%s",
        {
            "Configuration file #%d ( %s ) will be created now.",
            "Konfigurationsdatei #%d ( %s ) wird jetzt angelegt.",
            "El archivo de configuraci�n #%d ( %s ) se crear� ahora.",
            ""
        }},
    { "cfgFileSaveErr%s",
        {
            "Error saving configuration file %s.",
            "Fehler beim Speichern der Konfigurationsdatei %s.",
            "Ha habido alg�n error al guardar el archivo de configuraci�n %s.",
            ""
        }},
    { "cfgFileSaveOk%s",
        {
            "Configuration file %s saved.",
            "Konfigurationsdatei %s gespeichert.",
            "El archivo de configuraci�n %s se ha guardado correctamente.",
            ""
        }},
    { "cfgFileWrongFileId%d",
        {
            "Wrong id for configuration file %d",
            "Falsche id f�r Konfigurationsdatei %d",
            "Id %d no corresponde a ning�n fichero de configuraci�n",
            ""
        }},
    { "cfgRc",
        {
            "VDR Network configuration",
            "VDR Einstellungen Netz",
            "Configuraci�n de red del VDR",
            ""
        }},
    { "cfgRc.dhcp_flags",
        {
            "DHCP client options",
            "DHCP Client Optionen",
            "Opciones para el cliente DHCP",
            ""
        }},
    { "cfgRc.gateway",
        {
            "Gateway",
            "",
            "",
            ""
        }},
    { "cfgRc.hostname",
        {
            "Cmd to obtain hostname",
            "Befehl zum Ermitteln des Host-Namens",
            "Orden para obtener nombre de host",
            ""
        }},
    { "cfgRc.if",
        {
            "Network interface",
            "Netzwerks-Interface",
            "Interfaz de red",
            ""
        }},
    { "cfgRc.ip",
        {
            "IP address (if manual configuration)",
            "IP-Adresse (bei manueller Konfiguration)",
            "Direcci�n IP (si conf. manual)",
            ""
        }},
    { "cfgRc.lircd_flags",
        {
            "LIRC (Remote Control) server options",
            "Optionen f�r den LIRC-Server (Fernbedienung)",
            "Opciones para el servidor LIRC (Control Remoto)",
            ""
        }},
    { "cfgRc.mac_cmd",
        {
            "Cmd to obtain MAC address",
            "Befehl zum Ermitteln der MAC-Adresse",
            "Orden para obtener direcci�n MAC",
            ""
        }},
    { "cfgRc.net",
        {
            "IP assignation",
            "IP-Zuweisung",
            "Asignaci�n de IP",
            ""
        }},
    { "cfgRc.netdate",
        {
            "Configure date using network server",
            "Datum per Netzwerksserver einstellen",
            "Configurar fecha usando un servidor en red",
            ""
        }},
    { "cfgRc.netdate_server",
        {
            "Date network server",
            "Datumsserver",
            "Servidor de fecha en red",
            ""
        }},
    { "cfgRc.netmask",
        {
            "Subnet mask (if manual configuration)",
            "Subnetzmaske (bei manueller Konfiguration)",
            "M�scara de red (si conf. manual)",
            ""
        }},
    { "cfgRc.pic_tool_flags",
        {
            "pic_tool flags",
            "Optionen f�r pic_tool",
            "Opciones para pic_tool",
            ""
        }},
    { "cfgRc.syslogd_flags",
        {
            "syslogd flags",
            "Optionen f�r syslogd",
            "Opciones para syslogd",
            ""
        }},
    { "cfgWi",
        {
            "Setup Webinterface",
            "Einstellungen Webinterface",
            "Configuraci�n del interfaz web",
            "Configuration de l'interface web"
        }},
    { "cfgWi.ajax_disabled",
        {
            "Ajax disabled",
            "Ajax deaktiviert",
            "Desactivar Ajax",
            ""
        }},
    { "cfgWi.body_font_size",
        {
            "Base font size (CSS)",
            "",
            "Tama�o de fuente base (CSS)",
            ""
        }},
    { "cfgWi.config_change_disabled",
        {
            "Disable configuration modification",
            "�ndern der Konfiguration deaktivieren",
            "Impedir modificaci�n de la configuraci�n",
            ""
        }},
    { "cfgWi.config_view_disabled",
        {
            "Disable configuration view",
            "Konfigurationsansicht deaktivieren",
            "Impedir vista de la configuraci�n",
            ""
        }},
    { "cfgWi.default_language",
        {
            "Default Language",
            "Voreingestellte Sprache",
            "Idioma por defecto",
            "Langue par d�faut"
        }},
    { "cfgWi.default_margin_start",
        {
            "Default margin before",
            "Standard-Vorlaufzeit",
            "Margen antes por defecto",
            ""
        }},
    { "cfgWi.default_margin_stop",
        {
            "Default margin after",
            "Standard-Nachlaufzeit",
            "Margen despu�s por defecto",
            ""
        }},
    { "cfgWi.deletion_disabled",
        {
            "Disable deletion",
            "L�schen deaktivieren",
            "Desactivar borrado",
            ""
        }},
    { "cfgWi.display_host_id",
        {
            "Display host id",
            "Host-ID zeigen",
            "Mostrar id de host",
            ""
        }},
    { "cfgWi.epg_grid_hours",
        {
            "EPG schedule grid spanning hours",
            "Umfang des EPG-Rasters (Stunden)",
            "N� de horas en la parrilla de programaci�n",
            ""
        }},
    { "cfgWi.event_search_url",
        {
            "Event search URL",
            "URL f�r Event-Suche",
            "URL para buscar informaci�n de eventos",
            ""
        }},
    { "cfgWi.hosts",
        {
            "Hosts",
            "",
            "",
            ""
        }},
    { "cfgWi.logos",
        {
            "Add channel logos file (.tgz)",
            "Datei mit Kanallogos hinzuf�gen (.tgz)",
            "Agregar archivo <a class=\"newWindow\" href=\"http://www.assembla.com/spaces/VDR-M7x0/documents/tag/webif\" title=\"Descargar aqu�\">"
            "logos-webif-*.tgz"
            "</a>",
            ""
        }},
    { "cfgWi.max_depth",
        {
            "Max. subdir depth",
            "Max. Tiefe der Unterverzeichnisse",
            "Subniveles m�ximos en navegaci�n",
            ""
        }},
    { "cfgWi.no_box_shadow",
        {
            "Don't use CSS box-shadow capability",
            "",
            "No utilizar box-shadow en CSS",
            ""
        }},
    { "cfgWi.no_css_columns",
        {
            "Don't use CSS columns for layout",
            "",
            "No utilizar columnas CSS para la disposici�n del contenido",
            ""
        }},
    { "cfgWi.no_text_shadow",
        {
            "Don't use CSS text-shadow capability",
            "",
            "No utilizar text-shadow en CSS",
            ""
        }},
    { "cfgWi.password",
        {
            "Password",
            "Passwort",
            "Contrase�a",
            ""
        }},
    { "cfgWi.playlist_type",
        {
            "Playlist type",
            "Playlist-Typ",
            "Tipo de listas de reproducci�n",
            ""
        }},
    { "cfgWi.popups_disabled",
        {
            "Don't show messages in a popup dialog",
            "",
            "No usar di�logos emergentes para mostrar mensajes",
            ""
        }},
    { "cfgWi.print_rec_folder_summary",
        {
            "Print rec. folder summary",
            "Zeige Zusammenfassung f�r Aufnahmeordner",
            "Mostrar resumen carpetas de grabaci�n",
            ""
        }},
    { "cfgWi.use_external_www_folder",
        {
            "Use external www folder",
            "Externen www-Ordner verwenden",
            "Usar carpeta www externa",
            ""
        }},
    { "cfgWi.use_html5_video_tag",
        {
            "Use HTML5 &lt;video&gt; tag instead of vlc plugin",
            "",
            "Usar etiqueta &lt;video&gt; de HTML5 en lugar de plugin vlc",
            ""
        }},
    { "cfgWi.video_height",
        {
            "Video height",
            "Video-H�he",
            "Alto de video",
            ""
        }},
    { "cfgWi.video_width",
        {
            "Video width",
            "Video-Breite",
            "Ancho de video",
            ""
        }},
    { "change",
        {
            "Change",
            "�ndern",
            "Cambiar",
            ""
        }},
    { "channel",
        {
            "Channel",
            "Sender",
            "Canal",
            "Cha�ne"
        }},
    { "channel.current",
        {
            "Current channel",
            "Momentaner Sender",
            "Canal actual",
            ""
        }},
    { "channel.group",
        {
            "Channel group",
            "Sendergruppe",
            "Grupo de canales",
            ""
        }},
    { "channelDetails",
        {
            "Channel details",
            "Sender-Details",
            "Detalles del canal",
            ""
        }},
    { "channelFilter",
        {
            "Filter by channel",
            "Nach Sendern filtern",
            "Filtrar por canal",
            ""
        }},
    { "channelList",
        {
            "Channel List",
            "Kanalliste",
            "Lista de canales",
            "Liste des cha�nes"
        }},
    { "channelListEdit",
        {
            "Edit list",
            "Liste bearbeiten",
            "Editar lista",
            "Edition de la liste"
        }},
    { "channelMax",
        {
            "Last channel",
            "Letzter Sender",
            "�ltimo canal",
            ""
        }},
    { "channelMin",
        {
            "First channel",
            "Erster Sender",
            "Primer canal",
            ""
        }},
    { "channelMux",
        {
            "Multiplex",
            "Bouquet",
            "Multiplex",
            "Bouquet ou multiplex"
        }},
    { "channelName",
        {
            "Channel name",
            "Sendername",
            "Nombre del canal",
            "Nom de cha�ne"
        }},
    { "channelNumber",
        {
            "Channel number",
            "Programmplatz",
            "N�mero del canal",
            "Num�ro de cha�ne"
        }},
    { "channels",
        {
            "Channels",
            "Kan�le",
            "Canales",
            "Cha�nes"
        }},
    { "channels.filename",
        {
            "channels",
            "kanaele",
            "canales",
            "chaines"
        }},
    { "chooseDay",
        {
            "Choose day",
            "Tag ausw�hlen",
            "Escoge d�a",
            ""
        }},
    { "close",
        {
            "Close",
            "Schliessen",
            "Cerrar",
            ""
        }},
    { "command.execute.confirm",
        {
            "Are you sure you want to execute this command?",
            "Bist du sicher, dass du diesen Befehl ausf�hren m�chten?",
            "�Seguro que quieres ejecutar la orden?",
            ""
        }},
    { "commands",
        {
            "Commands",
            "Befehle",
            "�rdenes",
            ""
        }},
    { "compareDescription",
        {
            "Compare description",
            "Vergleiche Beschreibung",
            "Comparar descripci�n",
            ""
        }},
    { "compareSubtitle",
        {
            "Compare subtitle",
            "Vergleiche Untertitel",
            "Comparar subt�tulo",
            ""
        }},
    { "compareTitle",
        {
            "Compare title",
            "Vergleiche Titel",
            "Comparar t�tulo",
            ""
        }},
    { "conditionalAccessId",
        {
            "Conditional access ID",
            "Conditional Access ID",
            "ID de acceso condicional",
            "Identifiant d'acc�s conditionnel"
        }},
    { "cookie.delete",
        {
            "Delete cookie from browser",
            "Browser-Cookie l�schen",
            "Borrar cookie del navegador",
            ""
        }},
    { "cookie.save",
        {
            "Store as cookie in browser",
            "Einen Browser-Cookie speichern",
            "Guardar como cookie en navegador",
            ""
        }},
    { "create",
        {
            "Create new item",
            "Neues Item erzeugen",
            "Crear nuevo item",
            ""
        }},
    { "create.err",
        {
            "Failed to create new item!",
            "Konnte kein neues Item erzeugen!",
            "�Error al crear nuevo item!",
            ""
        }},
    { "create.ok",
        {
            "New item added",
            "Neues Item erzeugt",
            "Creado nuevo item",
            ""
        }},
    { "css_theme",
        {
            "CSS Theme ",
            "",
            "Tema CSS ",
            ""
        }},
    { "currentServer",
        {
            "Current server",
            "Momentaner Server",
            "Servidor actual",
            ""
        }},
    { "date",
        {
            "Date",
            "Datum",
            "Fecha",
            ""
        }},
    { "delete",
        {
            "Delete",
            "L�schen",
            "Borrar",
            ""
        }},
    { "delete+",
        {
            "Delete+",
            "L�schen+",
            "Borrar+",
            ""
        }},
    { "delete.err",
        {
            "Error deleting item!",
            "Fehler beim L�schen!",
            "�Error al borrar item!",
            ""
        }},
    { "delete.ok",
        {
            "Item deleted",
            "Item gel�scht",
            "Item borrado",
            ""
        }},
    { "deletionDisabled",
        {
            "Deletion is disabled.",
            "L�schen ist deaktiviert.",
            "El borrado est� bloqueado",
            ""
        }},
    { "desc",
        {
            "Description",
            "Beschreibung",
            "Descripci�n",
            ""
        }},
    { "directory",
        {
            "Directory",
            "Verzeichnis",
            "Directorio",
            ""
        }},
    { "download",
        {
            "Download",
            "Herunterladen",
            "Descargar",
            ""
        }},
    { "download_all",
        {
            "Download all",
            "Alle herunterladen",
            "Descargar todo",
            ""
        }},
    { "duration",
        {
            "Duration",
            "Dauer",
            "Duraci�n",
            ""
        }},
    { "durationFilter",
        {
            "Filter by duration",
            "Nach dauer filtern",
            "Filtrar por duraci�n",
            ""
        }},
    { "durationMax",
        {
            "Max. duration",
            "Max. Dauer",
            "Duraci�n m�x.",
            ""
        }},
    { "durationMin",
        {
            "Min. duration",
            "Mindestdauer",
            "Duraci�n m�n.",
            ""
        }},
    { "edit",
        {
            "Edit",
            "Bearbeiten",
            "Editar",
            ""
        }},
    { "end",
        {
            "End",
            "Ende",
            "Fin",
            ""
        }},
    { "epg",
        {
            "Program guide",
            "Programm",
            "Gu�a de programaci�n",
            "Programmation"
        }},
    { "epg.grid",
        {
            "Program",
            "Sendung",
            "Parrilla TV",
            "�mission"
        }},
    { "epg.list",
        {
            "Program list",
            "Sendung",
            "Listado",
            "�mission"
        }},
    { "epgFor_%s",
        {
            "Program guide for &raquo;%s&laquo;\n",
            "EPG f�r &raquo;%s&laquo;\n",
            "Gu�a de programaci�n de &raquo;%s&laquo;\n",
            ""
        }},
    { "error",
        {
            "Error",
            "Fehler",
            "Error",
            ""
        }},
    { "error.%d",
        {
            "Error: %d",
            "Fehler: %d",
            "Error: %d",
            ""
        }},
    { "error.401",
        {
            "Not authorized.",
            "Nicht autorisiert.",
            "No autorizado.",
            ""
        }},
    { "error.404_%s",
        {
            "Page %s not found.",
            "Seite %s nicht gefunden",
            "P�gina %s no hallada.",
            ""
        }},
    { "event.duration_%d",
        {
            "<span class=\"duration\">(<span class=\"l\">Runtime</span>: approx. <span class=\"t\">%d</span> minutes)</span>"
            ,
            "<span class=\"duration\">(<span class=\"l\">Dauer</span>: ca. <span class=\"t\">%d</span> Minuten)</span>"
            ,
            "<span class=\"duration\">(<span class=\"l\">Duraci�n</span>: <span class=\"t\">%d</span> minutos aprox.)</span>"
            ,
            "<span class=\"duration\">(<span class=\"l\">Dur�e</span>: approximativement <span class=\"t\">%d</span> minutes)</span>"
        }},
    { "event.from_%d:%d",
        {
            "<span class=\"start\"><span class=\"l\">From&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
            ,
            "<span class=\"start\"><span class=\"l\">Von&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
            ,
            "<span class=\"start\"><span class=\"l\">Inicio&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
            ,
            "<span class=\"start\"><span class=\"l\">De&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
        }},
    { "event.to_%d:%d",
        {
            "<span class=\"end\"><span class=\"l\">to&nbsp;</span><span class=\"t\">%02d:%02d</span> "
            ,
            "<span class=\"end\"><span class=\"l\">bis&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
            ,
            "<span class=\"end\"><span class=\"l\">Final&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
            ,
            "<span class=\"end\"><span class=\"l\">�&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
        }},
    { "execute",
        {
            "Execute",
            "Ausf�hren",
            "Ejecutar",
            ""
        }},
    { "file",
        {
            "File",
            "Datei",
            "Archivo",
            ""
        }},
    { "fileView",
        {
            "View files",
            "Datei ansehen",
            "Ver archivos",
            ""
        }},
    { "filesystem_encoding",
        {
            "Filesytem character encoding",
            "",
            "Codificaci�n de caracteres del sistema de archivos",
            ""
        }},
    { "firstDay",
        {
            "First day",
            "Erster Tag",
            "Primer d�a",
            ""
        }},
    { "for",
        {
            "For",
            "F�r",
            "Para",
            "Pour"
        }},
    { "freeSpace",
        {
            "Free Space",
            "Freier Platz",
            "Espacio libre",
            ""
        }},
    { "frequency",
        {
            "Frequency",
            "Frequenz",
            "Frecuencia",
            "Fr�quence"
        }},
    { "fstab",
        {
            "File systems",
            "Dateisysteme",
            "Sistemas de ficheros",
            ""
        }},
    { "fullScreen",
        {
            "full screen",
            "Vollbild",
            "Pantalla completa",
            "Plein �cran"
        }},
    { "help",
        {
            "Help",
            "Hilfe",
            "Ayuda",
            "Aide"
        }},
    { "homepage",
        {
            "Homepage",
            "Startseite",
            "P�gina inicial",
            "Page d'accueil"
        }},
    { "host.err.nullOrNotVdr",
        {
            "Host NULL or not VDR",
            "Host NULL oder kein VDR",
            "Host nulo o no es VDR",
            ""
        }},
    { "interval",
        {
            "Interval",
            "Intervall",
            "Intervalo",
            ""
        }},
    { "ipAddress",
        {
            "IP address",
            "IP-Adresse",
            "Direcci�n IP",
            ""
        }},
    { "ipPort",
        {
            "IP port",
            "IP Port",
            "Puerto IP",
            ""
        }},
    { "langBrowserDefined",
        {
            "Browser language",
            "Browser-Sprache",
            "Idioma del navegador",
            ""
        }},
    { "langEnglish",
        {
            "English",
            "Englisch",
            "Ingl�s",
            ""
        }},
    { "langFrench",
        {
            "French",
            "Franz�sisch",
            "Franc�s",
            "Fran�ais"
        }},
    { "langGerman",
        {
            "German",
            "Deutsch",
            "Alem�n",
            ""
        }},
    { "langSpanish",
        {
            "Spanish",
            "Spanisch",
            "Espa�ol",
            ""
        }},
    { "lifetime",
        {
            "Lifetime",
            "Lebensdauer",
            "Tiempo de vida",
            ""
        }},
    { "links",
        {
            "Links",
            "Verkn�pfungen",
            "Enlaces",
            ""
        }},
    { "list",
        {
            "List",
            "",
            "Lista",
            ""
        }},
    { "liveStream",
        {
            "Live stream",
            "Live-Stream",
            "Stream directo",
            "Direct"
        }},
    { "loading",
        {
            "Loading...",
            "",
            "Cargando...",
            ""
        }},
    { "margin.add",
        {
            "Add start/stop margin",
            "Vor-/Nachlaufzeit hinzuf�gen",
            "A�adir tiempo antes/despu�s",
            ""
        }},
    { "margin.after",
        {
            "Margin after",
            "Nachlaufzeit",
            "Margen despu�s",
            ""
        }},
    { "margin.before",
        {
            "Margin before",
            "Vorlaufzeit",
            "Margen antes",
            ""
        }},
    { "maxAllowed",
        {
            "M�x. allowed",
            "Max. zugelassen",
            "Permitir m�ximo",
            ""
        }},
    { "messages",
        {
            "Messages",
            "Nachrichten",
            "Mensajes",
            ""
        }},
    { "mode",
        {
            "Mode",
            "Modus",
            "Modo",
            ""
        }},
    { "moreInfo",
        {
            "More infos",
            "Mehr Infos",
            "M�s info",
            "Plus d'informations"
        }},
    { "mute",
        {
            "Mute",
            "Stummschalten",
            "Silenciar",
            "Muet"
        }},
    { "name",
        {
            "Name",
            "Name",
            "Nombre",
            ""
        }},
    { "networkId",
        {
            "Network ID",
            "Netzwerk-ID",
            "ID de la red",
            "Identifiant de r�seau"
        }},
    { "networkShares",
        {
            "Network shares",
            "Netzlaufwerke",
            "Recursos de red",
            "Partitions r�seau"
        }},
    { "next",
        {
            "Next",
            "Danach",
            "Luego",
            ""
        }},
    { "no",
        {
            "No",
            "Nein",
            "No",
            "Non"
        }},
    { "noLogosErrMessage",
        {
            "No logos found!",
            "<p>Das Verzeichnis mit Senderlogos wurde auf Ihrem System nicht gefunden.</p>"
            "<p>Lade das f�r deine Region n�tige Archiv herunter und installiere es mit diesem Webformular.</p>",
            "<p>No se ha encontrado el directorio con los logotipos de los canales en tu sistema.</p>"
            "<p>Descarga el archivo de logos apropiado para tu localidad e inst�lalo a trav�s de este formulario.</p>",
            ""
        }},
    { "noLogosErrTitle",
        {
            "No logos found!",
            "Keine Logos gefunden!",
            "No se han encontrado logos",
            ""
        }},
    { "noMediaErr",
        {
            "No media found!",
            "Keine Dateien gefunden.",
            "No se han encontrado archivos",
            ""
        }},
    { "now",
        {
            "Now",
            "Jetz",
            "Ahora",
            ""
        }},
    { "nowOnTv",
        {
            "Now on TV",
            "Das l�uft jetzt im TV",
            "Ahora en TV",
            "En direct"
        }},
    { "number",
        {
            "Number",
            "Platz",
            "N�mero",
            "Num�ro"
        }},
    { "oneTimeRecording",
        {
            "One-time recording",
            "Einmalige Aufnahme",
            "Grabar una vez",
            ""
        }},
    { "parameters",
        {
            "Parameters",
            "Parameter",
            "Par�metros",
            "Param�tres"
        }},
    { "partition",
        {
            "Partition",
            "Partition",
            "Partici�n",
            ""
        }},
    { "partitionSpace",
        {
            "Free space per partition",
            "Freier Platz pro Partition",
            "Espacio libre por particion",
            ""
        }},
    { "pause",
        {
            "Pause",
            "Pause",
            "Pausa",
            "Pausa",
        }},
    { "phrase",
        {
            "Phrase",
            "Satz",
            "Frase",
            "",
        }},
    { "play",
        {
            "Play",
            "Abspielen",
            "Reproducir",
            "",
        }},
    { "playAll",
        {
            "Play all",
            "Alle abspielen",
            "Reproducir todo",
            "",
        }},
    { "playlist",
        {
            "Playlist",
            "Playlist",
            "Lista de reproducci�n",
            "",
        }},
    { "playlist.download",
        {
            "Download playlist",
            "Playlist herunterladen",
            "Descargar lista",
            ""
        }},
    { "priority",
        {
            "Priority",
            "Priorit�t",
            "Prioridad",
            ""
        }},
    { "program",
        {
            "Program",
            "Sendung",
            "Emisi�n",
            "�mission"
        }},
    { "radioChannels",
        {
            "Radio channels",
            "Radiosender",
            "Canales de radio",
            ""
        }},
    { "radioId",
        {
            "Radio ID",
            "Radio-ID",
            "ID de la radio",
            "Identifiant de radio"
        }},
    { "rc.local",
        {
            "rc.local.conf",
            "",
            "",
            ""
        }},
    { "rec.delete.confirm",
        {
            "Are you sure you want to delete the record?",
            "Bist du sicher, dass du die Aufzeichnung l�schen willst?",
            "�Seguro que quieres borrar la grabaci�n?",
            ""
        }},
    { "rec.delete.err",
        {
            "Failed to delete record!",
            "Aufzeichnung konnte nicht gel�scht werden!",
            "�Error al borrar la grabaci�n!",
            ""
        }},
    { "rec.delete.ok",
        {
            "Record deleted",
            "Aufzeichnung gel�scht",
            "Grabaci�n borrada",
            ""
        }},
    { "rec.edit",
        {
            "Edit recording",
            "Aufzeichnung bearbeiten",
            "Editar grabaci�n",
            ""
        }},
    { "rec.edit.err",
        {
            "Failed to update recording!",
            "Aufzeichnen konnte nicht aktualisiert werden!",
            "�Error al actualizar la grabaci�n!",
            ""
        }},
    { "rec.edit.ok",
        {
            "Recording updated",
            "Aufzeichnen aktualisiert",
            "Grabaci�n actualizada",
            ""
        }},
    { "rec.edited",
        {
            "Recording edited",
            "Aufzeichnung bearbeitet",
            "Grabaci�n ya editada",
            ""
        }},
    { "recErrNoValidPath",
        {
            "No valid recording path present!",
            "Kein g�ltiger Aufnahmepfad vorhanden!",
            "�No es una ruta v�lida de grabaci�n!",
            ""
        }},
    { "record",
        {
            "Record",
            "Aufzeichnen",
            "Grabar",
            "Enregistrement"
        }},
    { "recording",
        {
            "Recording",
            "Aufzeichnung",
            "Grabaci�n",
            "enregistrement"
        }},
    { "recording.filename",
        {
            "recording",
            "Aufzeichnung",
            "grabaci�n",
            "enregistrement"
        }},
    { "recordingList",
        {
            "List of available recordings.",
            "Liste der aktuell verf�gbaren Aufzeichnungen.",
            "Lista de grabaciones disponibles.",
            "Liste des enregistrements disponibles"
        }},
    { "recordings",
        {
            "Recordings",
            "Aufzeichnungen",
            "Grabaciones",
            "Enregistrements"
        }},
    { "regex",
        {
            "Regular expression",
            "Regul�rer Ausdruck",
            "Expresi�n regular",
            ""
        }},
    { "regularRecording",
        {
            "Regular recording",
            "Regelm�&szlig;ige Aufnahme",
            "Grabar como serie",
            ""
        }},
    { "repeats",
        {
            "Repeats",
            "Wiederholungen",
            "Repeticiones",
            ""
        }},
    { "repeats.avoid",
        {
            "Avoid repeats",
            "Wiederholungen vermeiden",
            "Evitar repeticiones",
            ""
        }},
    { "repeats.maxAllowed",
        {
            "Repeats allowed",
            "Erlaubte Wiederholungen",
            "Repeticiones permitidas",
            ""
        }},
    { "response",
        {
            "Response",
            "Antwort",
            "Respuesta",
            ""
        }},
    { "revert",
        {
            "Revert",
            "R�ckg�ngig",
            "Revertir",
            ""
        }},
    { "runtime",
        {
            "Runtime",
            "Dauer",
            "Duraci�n",
            "dur�e"
        }},
    { "saveConfig",
        {
            "Save configuration",
            "Konfiguration speichern",
            "Guardar configuraci�n",
            ""
        }},
    { "search",
        {
            "Search",
            "Suchen",
            "Buscar",
            ""
        }},
    { "search.allWords",
        {
            "All words",
            "Alle W�rter",
            "Todas las palabras",
            ""
        }},
    { "search.announce",
        {
            "Announce only",
            "Nur ank�ndigen",
            "S�lo avisar",
            ""
        }},
    { "search.atLeastOne",
        {
            "At least one word",
            "Mindestens ein Wort",
            "Al menos una palabra",
            ""
        }},
    { "search.create",
        {
            "Create EPG search",
            "EPG Suche erstellen",
            "Crear b�squeda EPG",
            ""
        }},
    { "search.create.err",
        {
            "Failed to add search!",
            "Suche konnte nicht hinzugef�gt werden!",
            "�Error al a�adir la b�squeda!",
            ""
        }},
    { "search.create.ok",
        {
            "Search added",
            "Suche hinzugef�gt",
            "B�squeda a�adida",
            ""
        }},
    { "search.delAfterDays",
        {
            "Delete recordings after days",
            "Nach ... Tagen l�schen",
            "Borrar grabaciones tras d�as",
            ""
        }},
    { "search.delete+.title",
        {
            "Delete search and associated timers",
            "Suche und zugeh�rige Timer l�schen",
            "Borrar b�squeda y programaciones asociadas",
            ""
        }},
    { "search.delete.confirm",
        {
            "Are you sure you want to delete the search?",
            "Bist du sicher, dass du die Suche l�schen willst?",
            "�Seguro que quieres borrar la b�squeda?",
            ""
        }},
    { "search.edit",
        {
            "Edit search",
            "Suche bearbeiten",
            "Editar b�squeda",
            ""
        }},
    { "search.err.nothingToCompareWith",
        {
            "Specify where to search for text",
            "Angeben, wo der Text gesucht werden soll",
            "Especifica d�nde se debe buscar el texto",
            ""
        }},
    { "search.err.nothingToSearchFor",
        {
            "Specify text to search for",
            "Suchtext angeben",
            "Especifica texto a buscar",
            ""
        }},
    { "search.exactMatch",
        {
            "Exact match",
            "Genaue �bereinstimmung",
            "Coincidencia exacta",
            ""
        }},
    { "search.fuzzy",
        {
            "Fuzzy match",
            "Unscharfe Suche",
            "Coincidencia aproximada",
            ""
        }},
    { "search.switch",
        {
            "Switch only",
            "Nur umschalten",
            "S�lo cambiar",
            ""
        }},
    { "search.switch.minsBefore",
        {
            "Switch min. before",
            "Fr�her Umschalten (Min.)",
            "Cambiar min. antes",
            ""
        }},
    { "search.switch.unmuteSound",
        {
            "Unmute sound on switch",
            "Mute beim Umschalten aufheben",
            "Quitar silencio al cambiar",
            ""
        }},
    { "search.timeFilter",
        {
            "Time filter",
            "Nach Zeit filtern",
            "Filtrar por tiempo",
            ""
        }},
    { "search.useAsSearchTimer",
        {
            "Use as search timer",
            "Als Suchtimer verwenden",
            "Usar como programaci�n por b�squeda",
            ""
        }},
    { "search.useEpisode",
        {
            "Series recording",
            "Serienaufnahme",
            "Grabaci�n en serie",
            ""
        }},
    { "search.useExtEpgInfo",
        {
            "Use extended EPG info",
            "Nach EPG-Kategorien filtern",
            "Filtrar por categor�as EPG",
            ""
        }},
    { "searches",
        {
            "Searches",
            "Suchen",
            "B�squedas",
            ""
        }},
    { "serviceId",
        {
            "Service ID",
            "Service-ID",
            "ID del servicio",
            "Identifiant de service"
        }},
    { "setup",
        {
            "Setup",
            "Einstellungen",
            "Configuraci�n",
            "Configuration"
        }},
    { "shortdesc",
        {
            "Subtitle",
            "Untertitel",
            "Subt�tulo",
            ""
        }},
    { "source",
        {
            "Source",
            "Quelle",
            "Fuente",
            "Source"
        }},
    { "start",
        {
            "Start",
            "Start",
            "Inicio",
            "Lancement"
        }},
    { "startAfter",
        {
            "Start after",
            "Start nach",
            "Inicio despu�s de",
            ""
        }},
    { "startBefore",
        {
            "Start before",
            "Start vor",
            "Inicio antes de",
            ""
        }},
    { "startFilter",
        {
            "Filter by start",
            "Nach Startzeit filtern",
            "Filtrar por hora de inicio",
            ""
        }},
    { "streamdevHosts",
        {
            "Streamdevice Hosts",
            "",
            "",
            ""
        }},
    { "subfolders",
        {
            "Subfolders",
            "Unterverzeichnisse",
            "Subcarpetas",
            ""
        }},
    { "summary",
        {
            "Summary",
            "�bersicht",
            "Resumen",
            "R�sum�"
        }},
    { "svdrpHosts",
        {
            "SVDRP Hosts",
            "SVDRP Hosts",
            "Hosts SVDRP",
            ""
        }},
    { "switch",
        {
            "Switch",
            "Umschalten",
            "Cambiar",
            "Basculer"
        }},
    { "sysLog",
        {
            "SysLog",
            "SysLog",
            "Registro",
            ""
        }},
    { "time",
        {
            "Time",
            "Uhrzeit",
            "Hora",
            "Heure"
        }},
    { "timer.add",
        {
            "New timer",
            "Neuer Timer",
            "Nueva programaci�n",
            "Nouvel enregistrement programm�"
        }},
    { "timer.check",
        {
            "Check timer",
            "Angaben pr�fen",
            "Comprobar programaci�n",
            ""
        }},
    { "timer.create",
        {
            "Create timer",
            "Timer erstellen",
            "Programar grabaci�n",
            "Cr�ation d'un enregistrement programm�"
        }},
    { "timer.create.err",
        {
            "Failed to add timer!",
            "Timer konnte nicht hinzugef�gt werden!",
            "�Error al a�adir la programaci�n!",
            "Erreur de programmation de l'enregistrement!"
        }},
    { "timer.create.ok",
        {
            "Timer added",
            "Timer hinzugef�gt",
            "Programaci�n a�adida",
            "Enregistrement programm�"
        }},
    { "timer.delete",
        {
            "Delete timer",
            "Timer l�schen",
            "Borrar programaci�n",
            ""
        }},
    { "timer.delete.confirm",
        {
            "Are you sure you want to delete the timer?",
            "Bist du sicher, dass du den Timer l�schen willst?",
            "�Seguro que quieres borrar la programaci�n?",
            ""
        }},
    { "timer.delete.err",
        {
            "Failed to delete timer!",
            "Timer konnte nicht gel�scht werden!",
            "�Error al borrar la programaci�n!",
            ""
        }},
    { "timer.delete.ok",
        {
            "Timer deleted",
            "Timer gel�scht",
            "Programaci�n borrada",
            ""
        }},
    { "timer.edit",
        {
            "Edit timer",
            "Timer bearbeiten",
            "Editar programaci�n",
            "Modification d'un enregistrement"
        }},
    { "timer.update.err",
        {
            "Failed to update timer!",
            "Timer konnte nicht aktualisiert werden!",
            "�Error actualizando programaci�n!",
            ""
        }},
    { "timer.update.ok",
        {
            "Timer updated",
            "Timer aktualisiert",
            "Programaci�n actualizada",
            ""
        }},
    { "timerList",
        {
            "List of timers",
            "Timerliste",
            "Lista de programaciones",
            ""
        }},
    { "timers",
        {
            "Timers",
            "Timer",
            "Programaciones",
            "Enregistrements programm�s"
        }},
    { "timers.",
        {
            "Timr.",
            "Timer",
            "Progr.",
            "Enreg."
        }},
    { "title",
        {
            "Title",
            "Titel",
            "T�tulo",
            ""
        }},
    { "tolerance",
        {
            "Tolerance",
            "Toleranz",
            "Tolerancia",
            ""
        }},
    { "transportStreamId",
        {
            "Transport stream ID",
            "Transponder-ID",
            "ID del transponder",
            "Identifiant du transpondeur"
        }},
    { "tvChannels",
        {
            "TV channels",
            "TV Sender",
            "Canales de televisi�n",
            ""
        }},
    { "update.err",
        {
            "Error updating!",
            "Fehler bei Aktualisierung",
            "�Error actualizando!",
            ""
        }},
    { "update.ok",
        {
            "Updated",
            "Aktualisierung erfolgreich",
            "Actualizaci�n realizada",
            ""
        }},
    { "useCase",
        {
            "Compare case",
            "Gro&szlig;-/Kleinschreibung beachten",
            "Distinguir may�sculas",
            ""
        }},
    { "used",
        {
            "Used",
            "Verwendet",
            "Usado",
            ""
        }},
    { "user",
        {
            "Username",
            "Benutzer",
            "Usuario",
            ""
        }},
    { "vdrSetup",
        {
            "Setup VDR",
            "Einstellungen VDR",
            "Configuraci�n VDR",
            "Configuration de VDR"
        }},
    { "version_%s",
        {
            "Version %s",
            "Version %s",
            "Versi�n %s",
            ""
        }},
    { "volumeDown",
        {
            "Volume down",
            "Leiser",
            "Bajar volumen",
            ""
        }},
    { "volumeUp",
        {
            "Volume up",
            "Lauter",
            "Subir volumen",
            ""
        }},
    { "warnNoEPG",
        {
            "<p>EPG ist not available, please try again later.</p>"
            "<p>If the problem persists, this channel either has no EPG or it is broken. Feel free to get in touch with someone who is in charge for this channel.</p>\n"
            ,
            "<p>EPG ist nicht verf�gbar.</p>"
            "<p>Bitte versuche es sp�ter noch einmal. Falls dieses Problem �ber l�ngere Zeit besteht, wende dich bitte an den Sender.</p>\n"
            ,
            "<p>EPG no disponible, por favor int�ntelo m�s tarde.</p>"
            "<p>Si el problema persiste, �ste canal no tiene EPG o funciona incorrectamente.</p>\n"
            ,
            "<p>L'EPG (guide des programmes) n'est pas disponible, merci d'essayer ult�rieurement.</p>"
            "<p>Si le probl�me persiste, cette cha�ne n'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-�tre essayer de prendre contact avec un responsable de cette cha�ne.</p>\n"
        }},
    { "warnSvdrpConnection",
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
            "<p>Bitte �berpr�fe, dass der VDR l�uft und versuche es ggf. sp�ter noch einmal.</p>"
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
        }},
    { "wday",
        {
            "Weekday",
            "Wochentag",
            "D�a de la semana",
            ""
        }},
    { "wdayFilter",
        {
            "Weekday filter",
            "Nach Wochentag filtern",
            "Filtrar por d�a",
            ""
        }},
    { "web.search",
        {
            "Web search",
            "Im Internet suchen",
            "Buscar en Internet",
            ""
        }},
    { "webif",
        {
            "Webinterface",
            "Webinterface",
            "Interfaz web",
            "Interface web"
        }},
    { "yes",
        {
            "Yes",
            "Ja",
            "S�",
            "Oui"
        }}
};

const int i18nLength = sizeof (i18n) / sizeof (i18n_t);

#ifdef TESTI18NORDER
static int dumm=0;
#endif

static int i18nCompare(const void *a, const void *b) {
    const i18n_t *ia = (const i18n_t *) a;
    const i18n_t *ib = (const i18n_t *) b;
    int result;
    if (ia->key == NULL) {
        warn("Null key found");
        if (ib->key == NULL) {
            result = strcmp(ia->val[0], ib->val[0]);
        } else {
            result = -1;
        }
    } else {
        result = strcmp(ia->key, ib->key);
    }
#ifdef TESTI18NORDER
    if (result>-1) {
        dumm++;
    }
#endif
    return result;
}

void i18nsort() {
    qsort(&i18n, i18nLength, sizeof (i18n_t), i18nCompare);
}

const char *tr(const char *s) {
    i18n_t test = {s,
        {s, NULL, NULL, NULL}};
    i18n_t *found = NULL;
    while (!found && test.key) {
        found = (i18n_t *) bsearch(&test, &i18n, i18nLength, sizeof (i18n_t), i18nCompare);
        if (!found) {
            char *dot = strchr(test.key, '.');
            if (dot && dot[1]) {
                test.key = dot + 1;
                test.val[0] = dot + 1;
            } else {
                test.key = NULL;
            }
        }
    }
    //if (!found) dbg("i18n:Key %s not found",s);
    const char * result = (found != NULL) ? (strlen(found->val[langId]) > 0) ? found->val[langId] : found->val[0] : s;
    return result;
}

const char *formatDate(struct tm *timeptr, bool addHour) {
    char * format = NULL;
    if (addHour) {
        crit_goto_if(asprintf(&format, "%s %%H:%%M", dateFormat[langId]) < 0, OutOfMemory);
    } else {
        format = (char *) dateFormat[langId];
    }
    warn_ifm(strftime(cldate, sizeof cldate, (const char*) format, timeptr) == 0, "Date greater than cldate size");
    if (addHour) free(format);
    return cldate;
OutOfMemory:
    exit(EXIT_FAILURE);
}

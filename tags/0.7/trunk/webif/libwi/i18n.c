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
    {"Dom", "Lun", "Mar", "Mié", "Jue", "Vie", "Sáb"},
    {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"}
};
const char * const weekdays[I18N_NUM][7] = {
    {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
    {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"},
    {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"},
    {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"}
};

const char * const monthNames[I18N_NUM][12] = {
    {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
    {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"},
    {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"},
    {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"},
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
    "Español",
    "Français"
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
            "Acción",
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
            "Añadir",
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
            "Verfügbar",
            "Disponible",
            ""
        }},
    { "bootMenu",
        {
            "Boot Menu",
            "Boot-Menü",
            "Menú de arranque",
            ""
        }},
    { "browse",
        {
            "Browse",
            "Schmökern",
            "Navegar",
            ""
        }},
    { "browser.not_supported",
        {
            "Browser not supported",
            "Browser nicht unterstützt",
            "Navegador no soportado",
            ""
        }},
    { "browser.please_update",
        {
            "Browser not supported, please update it to a more recent version (or a different one...).",
            "Browser nicht unterstützt, bitte aktualisieren (oder einen anderen Browser verwenden...)",
            "Navegador no soportado, por favor actualízalo o utiliza un navegador diferente...",
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
            "Configuración del BoxAmp",
            "Configuration de BoxAmp"
        }},
    { "cfgErrInvalidParamValue%s",
        {
            "Invalid param value:<br/><span class=\"wrongValue\">%s</span>",
            "Ungültiger Parameter-Wert: <br/><span class=\"wrongValue\">%s</span>",
            "Valor del parámetro incorrecto:<br/><span class=\"wrongValue\">%s</span>",
            ""
        }},
    { "cfgErrParamNameValueMismatch",
        {
            "Mismatch between name and value numbers",
            "Name und Parameterwert passen nicht zueinander.",
            "El número de nombres y valores de parámetros no coincide.",
            ""
        }},
    { "cfgFileChangeDisabledErr",
        {
            "Change of configuration is disabled.",
            "Das Ändern der Konfiguration ist deaktiviert.",
            "La modificación de la configuración está bloqueada.",
            ""
        }},
    { "cfgFileDoesNotExist%s",
        {
            "Configuration file %s doesn't exist",
            "Konfigurationsdatei %s nicht vorhanden",
            "El archivo de configuración %s no existe",
            ""
        }},
    { "cfgFileIsNew_%d_%s",
        {
            "Configuration file #%d ( %s ) will be created now.",
            "Konfigurationsdatei #%d ( %s ) wird jetzt angelegt.",
            "El archivo de configuración #%d ( %s ) se creará ahora.",
            ""
        }},
    { "cfgFileSaveErr%s",
        {
            "Error saving configuration file %s.",
            "Fehler beim Speichern der Konfigurationsdatei %s.",
            "Ha habido algún error al guardar el archivo de configuración %s.",
            ""
        }},
    { "cfgFileSaveOk%s",
        {
            "Configuration file %s saved.",
            "Konfigurationsdatei %s gespeichert.",
            "El archivo de configuración %s se ha guardado correctamente.",
            ""
        }},
    { "cfgFileWrongFileId%d",
        {
            "Wrong id for configuration file %d",
            "Falsche id für Konfigurationsdatei %d",
            "Id %d no corresponde a ningún fichero de configuración",
            ""
        }},
    { "cfgRc",
        {
            "VDR Network configuration",
            "VDR Einstellungen Netz",
            "Configuración de red del VDR",
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
            "Dirección IP (si conf. manual)",
            ""
        }},
    { "cfgRc.lircd_flags",
        {
            "LIRC (Remote Control) server options",
            "Optionen für den LIRC-Server (Fernbedienung)",
            "Opciones para el servidor LIRC (Control Remoto)",
            ""
        }},
    { "cfgRc.mac_cmd",
        {
            "Cmd to obtain MAC address",
            "Befehl zum Ermitteln der MAC-Adresse",
            "Orden para obtener dirección MAC",
            ""
        }},
    { "cfgRc.net",
        {
            "IP assignation",
            "IP-Zuweisung",
            "Asignación de IP",
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
            "Máscara de red (si conf. manual)",
            ""
        }},
    { "cfgRc.pic_tool_flags",
        {
            "pic_tool flags",
            "Optionen für pic_tool",
            "Opciones para pic_tool",
            ""
        }},
    { "cfgRc.syslogd_flags",
        {
            "syslogd flags",
            "Optionen für syslogd",
            "Opciones para syslogd",
            ""
        }},
    { "cfgWi",
        {
            "Setup Webinterface",
            "Einstellungen Webinterface",
            "Configuración del interfaz web",
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
            "Tamaño de fuente base (CSS)",
            ""
        }},
    { "cfgWi.config_change_disabled",
        {
            "Disable configuration modification",
            "Ändern der Konfiguration deaktivieren",
            "Impedir modificación de la configuración",
            ""
        }},
    { "cfgWi.config_view_disabled",
        {
            "Disable configuration view",
            "Konfigurationsansicht deaktivieren",
            "Impedir vista de la configuración",
            ""
        }},
    { "cfgWi.default_language",
        {
            "Default Language",
            "Voreingestellte Sprache",
            "Idioma por defecto",
            "Langue par défaut"
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
            "Margen después por defecto",
            ""
        }},
    { "cfgWi.deletion_disabled",
        {
            "Disable deletion",
            "Löschen deaktivieren",
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
            "Nº de horas en la parrilla de programación",
            ""
        }},
    { "cfgWi.event_search_url",
        {
            "Event search URL",
            "URL für Event-Suche",
            "URL para buscar información de eventos",
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
            "Datei mit Kanallogos hinzufügen (.tgz)",
            "Agregar archivo <a class=\"newWindow\" href=\"http://www.assembla.com/spaces/VDR-M7x0/documents/tag/webif\" title=\"Descargar aquí\">"
            "logos-webif-*.tgz"
            "</a>",
            ""
        }},
    { "cfgWi.max_depth",
        {
            "Max. subdir depth",
            "Max. Tiefe der Unterverzeichnisse",
            "Subniveles máximos en navegación",
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
            "No utilizar columnas CSS para la disposición del contenido",
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
            "Contraseña",
            ""
        }},
    { "cfgWi.playlist_type",
        {
            "Playlist type",
            "Playlist-Typ",
            "Tipo de listas de reproducción",
            ""
        }},
    { "cfgWi.popups_disabled",
        {
            "Don't show messages in a popup dialog",
            "",
            "No usar diálogos emergentes para mostrar mensajes",
            ""
        }},
    { "cfgWi.print_rec_folder_summary",
        {
            "Print rec. folder summary",
            "Zeige Zusammenfassung für Aufnahmeordner",
            "Mostrar resumen carpetas de grabación",
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
            "Video-Höhe",
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
            "Ändern",
            "Cambiar",
            ""
        }},
    { "channel",
        {
            "Channel",
            "Sender",
            "Canal",
            "Chaîne"
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
            "Liste des chaînes"
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
            "Último canal",
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
            "Nom de chaîne"
        }},
    { "channelNumber",
        {
            "Channel number",
            "Programmplatz",
            "Número del canal",
            "Numéro de chaîne"
        }},
    { "channels",
        {
            "Channels",
            "Kanäle",
            "Canales",
            "Chaînes"
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
            "Tag auswählen",
            "Escoge día",
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
            "Bist du sicher, dass du diesen Befehl ausführen möchten?",
            "¿Seguro que quieres ejecutar la orden?",
            ""
        }},
    { "commands",
        {
            "Commands",
            "Befehle",
            "Órdenes",
            ""
        }},
    { "compareDescription",
        {
            "Compare description",
            "Vergleiche Beschreibung",
            "Comparar descripción",
            ""
        }},
    { "compareSubtitle",
        {
            "Compare subtitle",
            "Vergleiche Untertitel",
            "Comparar subtítulo",
            ""
        }},
    { "compareTitle",
        {
            "Compare title",
            "Vergleiche Titel",
            "Comparar título",
            ""
        }},
    { "conditionalAccessId",
        {
            "Conditional access ID",
            "Conditional Access ID",
            "ID de acceso condicional",
            "Identifiant d'accès conditionnel"
        }},
    { "cookie.delete",
        {
            "Delete cookie from browser",
            "Browser-Cookie löschen",
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
            "¡Error al crear nuevo item!",
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
            "Löschen",
            "Borrar",
            ""
        }},
    { "delete+",
        {
            "Delete+",
            "Löschen+",
            "Borrar+",
            ""
        }},
    { "delete.err",
        {
            "Error deleting item!",
            "Fehler beim Löschen!",
            "¡Error al borrar item!",
            ""
        }},
    { "delete.ok",
        {
            "Item deleted",
            "Item gelöscht",
            "Item borrado",
            ""
        }},
    { "deletionDisabled",
        {
            "Deletion is disabled.",
            "Löschen ist deaktiviert.",
            "El borrado está bloqueado",
            ""
        }},
    { "desc",
        {
            "Description",
            "Beschreibung",
            "Descripción",
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
            "Duración",
            ""
        }},
    { "durationFilter",
        {
            "Filter by duration",
            "Nach dauer filtern",
            "Filtrar por duración",
            ""
        }},
    { "durationMax",
        {
            "Max. duration",
            "Max. Dauer",
            "Duración máx.",
            ""
        }},
    { "durationMin",
        {
            "Min. duration",
            "Mindestdauer",
            "Duración mín.",
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
            "Guía de programación",
            "Programmation"
        }},
    { "epg.grid",
        {
            "Program",
            "Sendung",
            "Parrilla TV",
            "Émission"
        }},
    { "epg.list",
        {
            "Program list",
            "Sendung",
            "Listado",
            "Émission"
        }},
    { "epgFor_%s",
        {
            "Program guide for &raquo;%s&laquo;\n",
            "EPG für &raquo;%s&laquo;\n",
            "Guía de programación de &raquo;%s&laquo;\n",
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
            "Página %s no hallada.",
            ""
        }},
    { "event.duration_%d",
        {
            "<span class=\"duration\">(<span class=\"l\">Runtime</span>: approx. <span class=\"t\">%d</span> minutes)</span>"
            ,
            "<span class=\"duration\">(<span class=\"l\">Dauer</span>: ca. <span class=\"t\">%d</span> Minuten)</span>"
            ,
            "<span class=\"duration\">(<span class=\"l\">Duración</span>: <span class=\"t\">%d</span> minutos aprox.)</span>"
            ,
            "<span class=\"duration\">(<span class=\"l\">Durée</span>: approximativement <span class=\"t\">%d</span> minutes)</span>"
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
            "<span class=\"end\"><span class=\"l\">à&nbsp;</span><span class=\"t\">%02d:%02d</span></span> "
        }},
    { "execute",
        {
            "Execute",
            "Ausführen",
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
            "Codificación de caracteres del sistema de archivos",
            ""
        }},
    { "firstDay",
        {
            "First day",
            "Erster Tag",
            "Primer día",
            ""
        }},
    { "for",
        {
            "For",
            "Für",
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
            "Fréquence"
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
            "Plein écran"
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
            "Página inicial",
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
            "Dirección IP",
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
            "Inglés",
            ""
        }},
    { "langFrench",
        {
            "French",
            "Französisch",
            "Francés",
            "Français"
        }},
    { "langGerman",
        {
            "German",
            "Deutsch",
            "Alemán",
            ""
        }},
    { "langSpanish",
        {
            "Spanish",
            "Spanisch",
            "Español",
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
            "Verknüpfungen",
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
            "Vor-/Nachlaufzeit hinzufügen",
            "Añadir tiempo antes/después",
            ""
        }},
    { "margin.after",
        {
            "Margin after",
            "Nachlaufzeit",
            "Margen después",
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
            "Máx. allowed",
            "Max. zugelassen",
            "Permitir máximo",
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
            "Más info",
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
            "Identifiant de réseau"
        }},
    { "networkShares",
        {
            "Network shares",
            "Netzlaufwerke",
            "Recursos de red",
            "Partitions réseau"
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
            "<p>Lade das für deine Region nötige Archiv herunter und installiere es mit diesem Webformular.</p>",
            "<p>No se ha encontrado el directorio con los logotipos de los canales en tu sistema.</p>"
            "<p>Descarga el archivo de logos apropiado para tu localidad e instálalo a través de este formulario.</p>",
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
            "Das läuft jetzt im TV",
            "Ahora en TV",
            "En direct"
        }},
    { "number",
        {
            "Number",
            "Platz",
            "Número",
            "Numéro"
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
            "Parámetros",
            "Paramètres"
        }},
    { "partition",
        {
            "Partition",
            "Partition",
            "Partición",
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
            "Lista de reproducción",
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
            "Priorität",
            "Prioridad",
            ""
        }},
    { "program",
        {
            "Program",
            "Sendung",
            "Emisión",
            "Émission"
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
            "Bist du sicher, dass du die Aufzeichnung löschen willst?",
            "¿Seguro que quieres borrar la grabación?",
            ""
        }},
    { "rec.delete.err",
        {
            "Failed to delete record!",
            "Aufzeichnung konnte nicht gelöscht werden!",
            "¡Error al borrar la grabación!",
            ""
        }},
    { "rec.delete.ok",
        {
            "Record deleted",
            "Aufzeichnung gelöscht",
            "Grabación borrada",
            ""
        }},
    { "rec.edit",
        {
            "Edit recording",
            "Aufzeichnung bearbeiten",
            "Editar grabación",
            ""
        }},
    { "rec.edit.err",
        {
            "Failed to update recording!",
            "Aufzeichnen konnte nicht aktualisiert werden!",
            "¡Error al actualizar la grabación!",
            ""
        }},
    { "rec.edit.ok",
        {
            "Recording updated",
            "Aufzeichnen aktualisiert",
            "Grabación actualizada",
            ""
        }},
    { "rec.edited",
        {
            "Recording edited",
            "Aufzeichnung bearbeitet",
            "Grabación ya editada",
            ""
        }},
    { "recErrNoValidPath",
        {
            "No valid recording path present!",
            "Kein gültiger Aufnahmepfad vorhanden!",
            "¡No es una ruta válida de grabación!",
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
            "Grabación",
            "enregistrement"
        }},
    { "recording.filename",
        {
            "recording",
            "Aufzeichnung",
            "grabación",
            "enregistrement"
        }},
    { "recordingList",
        {
            "List of available recordings.",
            "Liste der aktuell verfügbaren Aufzeichnungen.",
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
            "Regulärer Ausdruck",
            "Expresión regular",
            ""
        }},
    { "regularRecording",
        {
            "Regular recording",
            "Regelmä&szlig;ige Aufnahme",
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
            "Rückgängig",
            "Revertir",
            ""
        }},
    { "runtime",
        {
            "Runtime",
            "Dauer",
            "Duración",
            "durée"
        }},
    { "saveConfig",
        {
            "Save configuration",
            "Konfiguration speichern",
            "Guardar configuración",
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
            "Alle Wörter",
            "Todas las palabras",
            ""
        }},
    { "search.announce",
        {
            "Announce only",
            "Nur ankündigen",
            "Sólo avisar",
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
            "Crear búsqueda EPG",
            ""
        }},
    { "search.create.err",
        {
            "Failed to add search!",
            "Suche konnte nicht hinzugefügt werden!",
            "¡Error al añadir la búsqueda!",
            ""
        }},
    { "search.create.ok",
        {
            "Search added",
            "Suche hinzugefügt",
            "Búsqueda añadida",
            ""
        }},
    { "search.delAfterDays",
        {
            "Delete recordings after days",
            "Nach ... Tagen löschen",
            "Borrar grabaciones tras días",
            ""
        }},
    { "search.delete+.title",
        {
            "Delete search and associated timers",
            "Suche und zugehörige Timer löschen",
            "Borrar búsqueda y programaciones asociadas",
            ""
        }},
    { "search.delete.confirm",
        {
            "Are you sure you want to delete the search?",
            "Bist du sicher, dass du die Suche löschen willst?",
            "¿Seguro que quieres borrar la búsqueda?",
            ""
        }},
    { "search.edit",
        {
            "Edit search",
            "Suche bearbeiten",
            "Editar búsqueda",
            ""
        }},
    { "search.err.nothingToCompareWith",
        {
            "Specify where to search for text",
            "Angeben, wo der Text gesucht werden soll",
            "Especifica dónde se debe buscar el texto",
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
            "Genaue Übereinstimmung",
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
            "Sólo cambiar",
            ""
        }},
    { "search.switch.minsBefore",
        {
            "Switch min. before",
            "Früher Umschalten (Min.)",
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
            "Usar como programación por búsqueda",
            ""
        }},
    { "search.useEpisode",
        {
            "Series recording",
            "Serienaufnahme",
            "Grabación en serie",
            ""
        }},
    { "search.useExtEpgInfo",
        {
            "Use extended EPG info",
            "Nach EPG-Kategorien filtern",
            "Filtrar por categorías EPG",
            ""
        }},
    { "searches",
        {
            "Searches",
            "Suchen",
            "Búsquedas",
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
            "Configuración",
            "Configuration"
        }},
    { "shortdesc",
        {
            "Subtitle",
            "Untertitel",
            "Subtítulo",
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
            "Inicio después de",
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
            "Übersicht",
            "Resumen",
            "Résumé"
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
            "Nueva programación",
            "Nouvel enregistrement programmé"
        }},
    { "timer.check",
        {
            "Check timer",
            "Angaben prüfen",
            "Comprobar programación",
            ""
        }},
    { "timer.create",
        {
            "Create timer",
            "Timer erstellen",
            "Programar grabación",
            "Création d'un enregistrement programmé"
        }},
    { "timer.create.err",
        {
            "Failed to add timer!",
            "Timer konnte nicht hinzugefügt werden!",
            "¡Error al añadir la programación!",
            "Erreur de programmation de l'enregistrement!"
        }},
    { "timer.create.ok",
        {
            "Timer added",
            "Timer hinzugefügt",
            "Programación añadida",
            "Enregistrement programmé"
        }},
    { "timer.delete",
        {
            "Delete timer",
            "Timer löschen",
            "Borrar programación",
            ""
        }},
    { "timer.delete.confirm",
        {
            "Are you sure you want to delete the timer?",
            "Bist du sicher, dass du den Timer löschen willst?",
            "¿Seguro que quieres borrar la programación?",
            ""
        }},
    { "timer.delete.err",
        {
            "Failed to delete timer!",
            "Timer konnte nicht gelöscht werden!",
            "¡Error al borrar la programación!",
            ""
        }},
    { "timer.delete.ok",
        {
            "Timer deleted",
            "Timer gelöscht",
            "Programación borrada",
            ""
        }},
    { "timer.edit",
        {
            "Edit timer",
            "Timer bearbeiten",
            "Editar programación",
            "Modification d'un enregistrement"
        }},
    { "timer.update.err",
        {
            "Failed to update timer!",
            "Timer konnte nicht aktualisiert werden!",
            "¡Error actualizando programación!",
            ""
        }},
    { "timer.update.ok",
        {
            "Timer updated",
            "Timer aktualisiert",
            "Programación actualizada",
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
            "Enregistrements programmés"
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
            "Título",
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
            "Canales de televisión",
            ""
        }},
    { "update.err",
        {
            "Error updating!",
            "Fehler bei Aktualisierung",
            "¡Error actualizando!",
            ""
        }},
    { "update.ok",
        {
            "Updated",
            "Aktualisierung erfolgreich",
            "Actualización realizada",
            ""
        }},
    { "useCase",
        {
            "Compare case",
            "Gro&szlig;-/Kleinschreibung beachten",
            "Distinguir mayúsculas",
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
            "Configuración VDR",
            "Configuration de VDR"
        }},
    { "version_%s",
        {
            "Version %s",
            "Version %s",
            "Versión %s",
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
            "<p>EPG ist nicht verfügbar.</p>"
            "<p>Bitte versuche es später noch einmal. Falls dieses Problem über längere Zeit besteht, wende dich bitte an den Sender.</p>\n"
            ,
            "<p>EPG no disponible, por favor inténtelo más tarde.</p>"
            "<p>Si el problema persiste, éste canal no tiene EPG o funciona incorrectamente.</p>\n"
            ,
            "<p>L'EPG (guide des programmes) n'est pas disponible, merci d'essayer ultérieurement.</p>"
            "<p>Si le problème persiste, cette chaîne n'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-être essayer de prendre contact avec un responsable de cette chaîne.</p>\n"
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
            "<p><strong>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gründe dafür:</p>"
            "<ul>"
            "<li>Der VDR wurde nicht gestartet bzw. läuft nicht mehr.</li>"
            "<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>"
            "<li>Es gab bei einer früheren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li>"
            "</ul>"
            "<p>Bitte überprüfe, dass der VDR läuft und versuche es ggf. später noch einmal.</p>"
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
        }},
    { "wday",
        {
            "Weekday",
            "Wochentag",
            "Día de la semana",
            ""
        }},
    { "wdayFilter",
        {
            "Weekday filter",
            "Nach Wochentag filtern",
            "Filtrar por día",
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
            "Sí",
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

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
		{"Domingo" ,"Lunes" ,"Martes"  ,"Mi�rcoles","Jueves"    ,"Viernes" ,"S�bado"},
		{"Dimanche","Lundi" ,"Mardi"   ,"Mercredi" ,"Jeudi"     ,"Vendredi","Samedi"}
};

const tI18nPhrase i18n[]={
		{	"English",		// #0
			"Deutsch",
			"Espa�ol",
			"Fran�ais"
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
		  "Gu�a de Programaci�n",
		  "Programmation"
		},
		{	"Channels",		// #6
		  "Kan�le",
		  "Canales",
		  "Cha�nes"
		},
		{	"Timers",		// #7
		  "Timer",
		  "Programaciones",
		  "Enregistrements programm�s"
		},
		{	"Recordings",	// #8
		  "Aufzeichnungen",
		  "Grabaciones",
		  "Enregistrements"
		},
		{	"Setup",
			"Einstellungen",	// #9
			"Configuraci�n",
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
			"Wichtiger Hinweis: das Webinterface befindet sich noch in einer sehr fr�hen Phase. Weder ist es vollst�ndig noch fehlerfrei. Zur Diskussion steht im open7x0.org Forum ein eigenes <a href=\"http://www.open7x0.org/arena/forumdisplay.php?fid=67\" target=\"_blank\">Unterforum</a> bereit.",
			"Informaci�n importante: El interfaz web est� todav�a en construcci�n y no est� libre de errores.",
			"Information importante: cette acc�s web est en chantier. Il n\'est ni complet ni exempt de d�faut"
		},					// #13
		{	"<p><b>Failed to open a SVDRP connection to the local VDR. Possible Reasons:\n\n<ul><li>The VDR is not running.</li>\n<li>The SVDRP connection is still in use by another client like vdradmin-AM</li>\n<li>An error occured while processing an earlier request and now the SVDRP connection is blocked.</li></ul>\n\nPlease check if the VDR is running or try it again later.</b></p>\n",
			"<p><b>Es konnte keine SVDRP-Verbindung zum VDR aufgebaut werden. Typische Gr&uuml;nde daf&uuml;r:\n\n<ul><li>Der VDR wurde nicht gestartet bzw. l&auml;uft nicht mehr.</li>\n<li>Die SVDRP-Verbindung wird von einem anderen Client verwendet, z.B. vdradmin-AM</li>\n<li>Es gab bei einer fr&uuml;heren Abfrage einen Fehler und die SVDRP-Verbindung ist blockiert.</li></ul>\n\nBitte &uuml;berpr&uuml;fen sie, dass der VDR l&auml;uft und versuchen sie es ggf. sp&auml;ter noch einmal.</b></p>\n",
			"<p><b>No se puede abrir una conexi�n SVDRP con el VDR local. Posibles motivos:\n\n<ul><li>El VDR no se est� ejecutando.</li>\n<li>La conexi�n SVDRP est� todav�a en uso por otro cliente como vdradmin-AM</li>\n<li>Se ha producido un error mientras se proces� una petici�n anterior y ahora la conexi�n SVDRP est� bloqueada.</li></ul>\n\nPor favor, compruebe si el VDR se est� ejecutando o int�ntelo m�s tarde.</b></p>\n",
			"<p><b>Echec d\'acc�s par connection SVDRP au logiciel VDR. Raisons possibles:\n\n<ul><li>VDR n\'est pas d�marr�.</li><li>La connexion SVDRP est encore utilis�e par un autre client comme \"vdradmin\".</li><li>la connexion SVDRP est bloqu� suite � une erreur pr�c�dente</li></ul>\n\nV�rifier que VDR est bien lanc� ou essayer ult�rieurement.</b></p>\n"
		},					// #14
		{	"EPG ist not avaible, please try again later. If the problem persists, this channel either has no EPG or it is broken. Feel free to get in touch with someone who is in charge for this channel.",
			"EPG ist nicht verf&uuml;gbar. Bitte versuchen sie es sp&auml;ter nocheinmal. Falls dieses Problem &uuml;ber l&auml;ngere Zeit besteht, wenden sie sich bitte an den Sender.",
			"EPG no disponible, por favor int�ntelo m�s tarde. Si el problema persiste, �ste canal no tiene EPG o funciona incorrectamente. Puede ponerse en contacto con alg�n responsable de �ste canal.",
			"L\'EPG (guide des programmes) n\'est pas disponible, merci d\'essayer ult�rieurement. Si le probl�me persiste, cette cha�ne n\'a pas de guide des programmes ou il engendre des erreurs. Vous pouvez peut-�tre essayer de prendre contact avec un responsable de cette cha�ne."
		},					// #15
		{	"An error occured, maybe with the translation of a phrase.",
			"Ein Fehler ist aufgetreten, wahrscheinlich konnte eine �bersetzung nicht gefunden werden.",
			"Ocurri� un error debido a la traducci�n de alguna frase.",
			"Erreur, peut-�tre due � un absence de traduction pour une phrase"
		},					// #16
		{	"List of available recordings.",					
			"Liste der aktuell verf�gbaren Aufzeichnungen.",
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
			"Standardeinstellung f�r die Sprache",
			"",
			""
		},
		{	"Homepage",			// #
			"Startseite",
			"P�gina inicial",
			"Page d\'accueil"
		},
		{	"You are watching",	// #
			"Sie sehen zur Zeit",
			"Est�s viendo",
			"Vous regardez"
		},
		{	"Channel List",		// #
			"Kanalliste",
			"Lista de canales",
			"Liste des cha�nes"
		},						
		{	"channel number",	// #
			"Programmplatz",
			"N�mero del canal",
			"Num�ro de cha�ne"
		},
		{	"channel name",		// #
			"Sendername",
			"Nombre del canal",
			"Nom de cha�ne"
		},
		{	"multiplex",		// #
			"Bouquet",
			"Multiplex",
			"Bouquet ou multiplex"
		},
		{	"more infos",		// #
			"mehr Infos",
			"m�s info",
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
			"Fr�quence"
		},
		{	"parameters",		// #
			"Parameter",
			"Par�metros",
			"Param�tres"
		},
		{	"source",			// #
			"Quelle",
			"Fuente",
			"Source"
		},
		{	"conditional access ID",	// #
			"Conditional Access ID",
			"ID acceso condicional",
			"Identifiant d\'acc�s conditionnel"
		},
		{	"service ID",		// #
			"Service-ID",
			"ID del servicio",
			"Identifiant de service"
		},
		{	"network ID",		// #
			"Netzwerk-ID",
			"ID de la red",
			"Identifiant de r�seau"
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
			"�bersicht",
			"Resumen",
			"R�sum�"
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
			"Plein �cran"
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
			"Das l�uft jetzt im TV",
			"Ahora en TV",
			"En direct"
		},
		{	"channel",			// #
			"Sender",
			"Canal",
			"cha�ne"
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
			"�"
		},
		{	"runtime",			// #
			"Dauer",
			"Duraci�n",
			"dur�e"
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
			"Grabaci�n",
			"enregistrement"
		},
		{	"program",			// #
			"Sendung",
			"Emisi�n",
			"�mission"
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
			"Partitions r�seau"
		},
		{	"Webinterface",		// #
			"Webinterface",
			"Interfaz web",
			"Interface web"
		},
		{	"default language",	// #
			"Voreingestellte Sprache",
			"Idioma por defecto",
			"langue par d�faut"
		},
		{	"auto",				// #
			"automatisch",
			"autom�tico",
			"automatique"
		},
		{ "BoxAmp path",		// #
			"BoxAmp-Verzeichnis",
			"Ruta BoxAmp",
			"R�pertoire de BoxAmp"
		},
		{	"Path to audio files",  //#
			"Audio-Basisverzeichnis",
			"Ruta a archivos de m�sica",
			"R�pertoire des fichiers audio"
		},
		{	"Options",			// #
			"Optionen",
			"Opciones",
			"Options"
		},
		{	"Terminate automatically",	// #
			"Automatisch beenden",
			"Terminar autom�ticamente",
			"Terminaison automatique"
		},
		{	"for",				// #
			"f�r",
			"para",
			"pour"
		},
		{	"Time",				// #
			"Uhrzeit",
			"Duraci�n",
			"Heure"
		},
		{	"create timer",		// #
			"Timer erstellen",
			"Crear programaci�n",
			"cr�ation d\'un enregistrement programm�"
		},
		{	"Timer added!",		// #
			"Timer hinzugef�gt!",
			"�Programaci�n a�adida!",
			"Enregistrement programm�!"
		},
		{	"Failed to add timer!",	// #
			"Timer konnte nicht hinzugef�gt werden!",
			"�A�adir programaci�n ha fallado!",
			"Erreur de programmation de l\'enregistrement!"
		},
		{	"new timer",		// #
			"Neuer Timer",
			"Nueva programaci�n",
			"Nouvel enregistrement programm�"
		},
		{ "edit timer",		// #
			"Timer bearbeiten",
			"Editar programaci�n",
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
			"A�adir tiempo antes/despu�s",
			""
		},
		{	"priority",		// #
			"Priorit&auml;t",
			"Prioridad",
			""
		},
		{	"lifetime",		// #
			"Lebensdauer",
			"Tiempo duraci�n",
			""
		},
		{	"title",		// #
			"Titel",
			"T�tulo",
			""
		},
		{	"check timer",	// #
			"Angaben pr&uuml;fen",
			"Comprobar programaci�n",
			""
		},
		{	"yes",		// #
			"ja",
			"s�",
			"oui"
		},
		{	"no",		// #
			"nein",
			"no",
			"non"
		},
		{	"add",	// #
			"eintragen",
			"a�adir",
			""
		},
		{	"download all",		//#
			"alle herunterladen",
			"descargar todo",
			""
		}, 
		{	"Timer updated!",	// #
			"Timer aktualisiert!",
			"�Programaci�n actualizada!",
			""
		},
		{	"Failed to update timer! Maybe it changed in the meantime?",	//#
			"Timer konnte nicht aktualisiert werden! Evt. hat sich die Timerliste in der Zwischenzeit ver�ndert?",
			"�Actualizar programaci�n ha fallado! �Tal vez fue cambiada mientras tanto?",
			""
		},
		{	"- or -",		//#
			"- oder -",
			"- o -",
			""
		},
		{	"delete timer",	// #
			"Timer l�schen",
			"Borrar programaci�n",
			""
		},
		{	" execute ",	// #
			" ausf�hren ",
			" ejecutar ",
			""
		},
		{	"Timer deleted!",	// #
			"Timer gel�scht!",
			"�Programaci�n borrada!",
			""
		},
		{	"Failed to delete timer!",	// #
			"Timer konnte nicht gel�scht werden!",
			"�Borrar programaci�n ha fallado!",
			""
		},
		{	"browse",
			"schm�kern",
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

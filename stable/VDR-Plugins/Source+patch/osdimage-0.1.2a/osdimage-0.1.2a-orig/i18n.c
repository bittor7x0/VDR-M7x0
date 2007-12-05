/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.4 2004/01/16 13:28:23 kls Exp $
 */

 
#include "i18n.h"

const tI18nPhrase Phrases[] =
{
	{  "OSD Image",
		"OSD Image",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},
	{  "OSD Image Viewer",
		"OSD Bildbetrachter",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},
	{  "Show all",
		"Alle anzeigen",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},		
	{  "Exit",
		"Beenden",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},			
	{  "Alpha-value picture",
		"Alpha-Wert Bild",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},			
	{  "Alpha-value background",
		"Alpha-Wert Hintergrund",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},			
	{  "Alpha-value text-area",
		"Alpha-Wert Schriftfeld",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Delay for the Slideshow [s]",
		"Wartezeit für Diashow [s]",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Directory",
		"Verzeichnis",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Loading",
		"Lade",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Rotating picture cw",
		"Drehe Bild im UZS",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Rotating picture ccw",
		"Drehe Bild gegen den UZS",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Resetting rotation",
		"Mache Drehung rückgängig",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "No pictures found",
		"Keine Bilder gefunden",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "No pictures found or directory not valid!",
		"Keine Bilder gefunden oder Verzeichnis ungültig!",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Error loading picture!",
		"Fehler beim Laden des Bildes!",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Activate dithering",
		"Dithering aktivieren",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "yes",
		"ja",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "no",
		"nein",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  "Quantizer",
		"Quantisierer",
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
		"",// TODO
	},	
	{  NULL  }
};


/*
 *  $Id: i18n.c,v 1.8.2.9 2010/07/19 13:50:11 schmirl Exp $
 */
 
#include "i18n.h"

const char *i18n_name = NULL;

const tI18nPhrase Phrases[] = {
	{	"VDR Streaming Server",		// English
		"VDR Streaming Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Serveur de streaming VDR",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika / Greek
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"VTP Streaming Client",		// English
		"VTP Streaming Client",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Client de streaming VTP",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika / Greek
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Start VDR-to-VDR Server",		// English
		"VDR-zu-VDR Server starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Démarrer le serveur VDR-to-VDR",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika / Greek
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Start HTTP Server",		// English
		"HTTP Server starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Démarrer le serveur HTTP",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"HTTP Streamtype",		// English
		"HTTP Streamtyp",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Type de Streaming HTTP",		// Français
		"",		// Norsk
		"",		// Suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Start Client",		// English
		"Client starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Démarrage du client",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"VDR-to-VDR Server Port",		// English
		"Port des VDR-zu-VDR Servers",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Port du serveur VDR-to-VDR",		// Français
		"",		// Norsk
		"",		// Suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"HTTP Server Port",		// English
		"Port des HTTP Servers",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Port du serveur HTTP",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Maximum Number of Clients",		// English
		"Maximalanzahl an Clients",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Nombre maximun de clients",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Remote IP",		// English
		"IP der Gegenseite",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Adresse IP du serveur",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Remote Port",		// English
		"Port der Gegenseite",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Port du serveur",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Common Settings",		// English
		"Allgemeines",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Paramètres communs",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"VDR-to-VDR Server",		// English
		"VDR-zu-VDR Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"VDR-to-VDR Serveur",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"HTTP Server",		// English
		"HTTP Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Serveur HTTP",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Minimum Priority",		// English
		"Minimale Priorität",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Maximum Priority",		// English
		"Maximale Priorität",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Suspend Live TV",		// English
		"Live-TV pausieren",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Suspendre Live TV",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Suspend behaviour",		// English
		"Pausierverhalten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Suspendre",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Offer suspend mode",		// English
		"Pausieren anbieten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Offrir le mode suspendre",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Always suspended",		// English
		"Immer pausiert",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Toujours suspendre",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Never suspended",		// English
		"Nie pausiert",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Jamais suspendre",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Suspend Server",		// English
		"Server pausieren",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Suspendre le serveur",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Server is suspended",		// English
		"Server ist pausiert",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Le serveur est suspendu",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Couldn't suspend Server!",		// English
		"Konnte Server nicht pausieren!",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Impossible de suspendre le serveur!",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Client may suspend",		// English
		"Client darf pausieren",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Le client peut suspendre",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Bind to IP",		// English
		"Binde an IP",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Attacher aux IP",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
        },
	{	"Filter Streaming",		// English
		"Filter-Daten streamen",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Filtre streaming",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
        },
	{	"Streaming active",		// English
		"Streamen im Gange",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Streaming actif",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
        },
	{	"Hide Mainmenu Entry",		// English
		"Hauptmenüeintrag verstecken",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"Masquer dans le menu principal",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Multicast Streaming Server",		// English
		"Multicast Streaming Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Start IGMP Server",		// English
		"IGMP Server starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Multicast Client Port",		// English
		"Port des Multicast Clients",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{	"Multicast Streamtype",		// English
		"Multicast Streamtyp",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Português
		"",		// Français
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Español
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"",		// Russian
		"",		// Hrvatski
		"",		// Eesti
		"",		// Dansk
		"",		// Czech
#if VDRVERSNUM >= 10502
		"",		// Türkçe
#endif
	},
	{ NULL }
};

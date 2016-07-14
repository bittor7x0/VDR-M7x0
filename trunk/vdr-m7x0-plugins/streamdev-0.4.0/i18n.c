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
		"",		// Portugu�s
		"Serveur de streaming VDR",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"VTP Streaming Client",		// English
		"VTP Streaming Client",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Client de streaming VTP",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Start VDR-to-VDR Server",		// English
		"VDR-zu-VDR Server starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"D�marrer le serveur VDR-to-VDR",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Start HTTP Server",		// English
		"HTTP Server starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"D�marrer le serveur HTTP",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"HTTP Streamtype",		// English
		"HTTP Streamtyp",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Type de Streaming HTTP",		// Fran�ais
		"",		// Norsk
		"",		// Suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Start Client",		// English
		"Client starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"D�marrage du client",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"VDR-to-VDR Server Port",		// English
		"Port des VDR-zu-VDR Servers",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Port du serveur VDR-to-VDR",		// Fran�ais
		"",		// Norsk
		"",		// Suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"HTTP Server Port",		// English
		"Port des HTTP Servers",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Port du serveur HTTP",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Maximum Number of Clients",		// English
		"Maximalanzahl an Clients",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Nombre maximun de clients",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Remote IP",		// English
		"IP der Gegenseite",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Adresse IP du serveur",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Remote Port",		// English
		"Port der Gegenseite",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Port du serveur",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Common Settings",		// English
		"Allgemeines",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Param�tres communs",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"VDR-to-VDR Server",		// English
		"VDR-zu-VDR Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"VDR-to-VDR Serveur",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"HTTP Server",		// English
		"HTTP Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Serveur HTTP",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Minimum Priority",		// English
		"Minimale Priorit�t",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Maximum Priority",		// English
		"Maximale Priorit�t",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Suspend Live TV",		// English
		"Live-TV pausieren",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Suspendre Live TV",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Suspend behaviour",		// English
		"Pausierverhalten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Suspendre",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Offer suspend mode",		// English
		"Pausieren anbieten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Offrir le mode suspendre",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Always suspended",		// English
		"Immer pausiert",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Toujours suspendre",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Never suspended",		// English
		"Nie pausiert",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Jamais suspendre",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Suspend Server",		// English
		"Server pausieren",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Suspendre le serveur",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Server is suspended",		// English
		"Server ist pausiert",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Le serveur est suspendu",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Couldn't suspend Server!",		// English
		"Konnte Server nicht pausieren!",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Impossible de suspendre le serveur!",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Client may suspend",		// English
		"Client darf pausieren",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Le client peut suspendre",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Bind to IP",		// English
		"Binde an IP",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Attacher aux IP",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
        },
	{	"Filter Streaming",		// English
		"Filter-Daten streamen",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Filtre streaming",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
        },
	{	"Streaming active",		// English
		"Streamen im Gange",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Streaming actif",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
        },
	{	"Hide Mainmenu Entry",		// English
		"Hauptmen�eintrag verstecken",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Masquer dans le menu principal",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Multicast Streaming Server",		// English
		"Multicast Streaming Server",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Start IGMP Server",		// English
		"IGMP Server starten",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Multicast Client Port",		// English
		"Port des Multicast Clients",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{	"Multicast Streamtype",		// English
		"Multicast Streamtyp",		// Deutsch
		"",		// Slovenski
		"",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"",		// suomi
		"",		// Polski
		"",		// Espa�ol
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
		"",		// T�rk�e
#endif
	},
	{ NULL }
};

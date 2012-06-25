/*
 *  $Id: i18n.c,v 1.8.2.9 2010/07/19 13:50:11 schmirl Exp $
 */
 
#include "i18n.h"

const char *i18n_name = NULL;

const tI18nPhrase Phrases[] = {
	{	"VDR Streaming Server",		// English
		"VDR Streaming Server",		// Deutsch
		"",		// Slovenski
		"Server trasmissione VDR",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Serveur de streaming VDR",		// Fran�ais
		"",		// Norsk
		"VDR-suoratoistopalvelin",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika / Greek
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"VDR Streaming ������",		// Russian
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
		"Client trasmissione VTP",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Client de streaming VTP",		// Fran�ais
		"",		// Norsk
		"VTP-suoratoistoasiakas ",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika / Greek
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"VTP Streaming ������",		// Russian
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
		"Avvia Server VDR-a-VDR",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"D�marrer le serveur VDR-to-VDR",		// Fran�ais
		"",		// Norsk
		"K�ynnist� VDR-palvelin",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika / Greek
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"����� VDR-to-VDR ������",		// Russian
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
		"Avvia Server HTTP",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"D�marrer le serveur HTTP",		// Fran�ais
		"",		// Norsk
		"K�ynnist� HTTP-palvelin",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"����� HTTP �������",		// Russian
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
		"Tipo flusso HTTP",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Type de Streaming HTTP",		// Fran�ais
		"",		// Norsk
		"HTTP-l�hetysmuoto",		// Suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"��� HTTP ������",		// Russian
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
		"Avvia Client",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"D�marrage du client",		// Fran�ais
		"",		// Norsk
		"K�ynnist� VDR-asiakas",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"����� �������",		// Russian
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
		"Porta Server VDR-a-VDR",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Port du serveur VDR-to-VDR",		// Fran�ais
		"",		// Norsk
		"VDR-palvelimen portti",		// Suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"VDR-to-VDR ���� �������",		// Russian
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
		"Porta Server HTTP",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Port du serveur HTTP",		// Fran�ais
		"",		// Norsk
		"HTTP-palvelimen portti",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"HTTP ������ ����",		// Russian
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
		"Numero massimo di Client",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Nombre maximun de clients",		// Fran�ais
		"",		// Norsk
		"Suurin sallittu asiakkaiden m��r�",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"����. ���������� ��������",		// Russian
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
		"Indirizzo IP del Server",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Adresse IP du serveur",		// Fran�ais
		"",		// Norsk
		"Et�koneen IP-osoite",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"��������� IP",		// Russian
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
		"Porta Server Remoto",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Port du serveur",		// Fran�ais
		"",		// Norsk
		"Et�koneen portti",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"��������� ����",		// Russian
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
		"Impostazioni comuni",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Param�tres communs",		// Fran�ais
		"",		// Norsk
		"Yleiset asetukset",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"���������",		// Russian
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
		"Server VDR-a-VDR",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"VDR-to-VDR Serveur",		// Fran�ais
		"",		// Norsk
		"VDR-palvelin",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"VDR-to-VDR ������",		// Russian
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
		"Server HTTP",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Serveur HTTP",		// Fran�ais
		"",		// Norsk
		"HTTP-palvelin",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"HTTP ������",		// Russian
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
		"Priorit� minima",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"Pienin prioriteetti",		// suomi
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
		"Priorit� massima",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"Suurin prioriteetti",		// suomi
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
		"Sospendi TV dal vivo",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Suspendre Live TV",		// Fran�ais
		"",		// Norsk
		"Pys�yt� suora TV-l�hetys",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"��������� Live TV",		// Russian
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
		"Tipo sospensione",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Suspendre",		// Fran�ais
		"",		// Norsk
		"Pys�ytystoiminto",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"��������� ���������",		// Russian
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
		"Offri mod. sospensione",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Offrir le mode suspendre",		// Fran�ais
		"",		// Norsk
		"tyrkyt�",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"���������� ���������",		// Russian
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
		"Sempre sospeso",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Toujours suspendre",		// Fran�ais
		"",		// Norsk
		"aina",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"������ ����������",		// Russian
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
		"Mai sospeso",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Jamais suspendre",		// Fran�ais
		"",		// Norsk
		"ei koskaan",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"������� �� ����������",		// Russian
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
		"Sospendi Server",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Suspendre le serveur",		// Fran�ais
		"",		// Norsk
		"Pys�yt� palvelin",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"���������� ������",		// Russian
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
		"Server sospeso",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Le serveur est suspendu",		// Fran�ais
		"",		// Norsk
		"Palvelin on pys�ytetty",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"������ ����������",		// Russian
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
		"Impossibile sospendere il server!",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Impossible de suspendre le serveur!",		// Fran�ais
		"",		// Norsk
		"Palvelinta ei onnistuttu pys�ytt�m��n!",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"�� ���� ���������� ������",		// Russian
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
		"Permetti sospensione al Client",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Le client peut suspendre",		// Fran�ais
		"",		// Norsk
		"Asiakas saa pys�ytt�� palvelimen",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"������ ����� �������������",		// Russian
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
		"IP associati",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Attacher aux IP",		// Fran�ais
		"",		// Norsk
		"Sido osoitteeseen",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"�������������� � IP",		// Russian
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
		"Filtra trasmissione",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Filtre streaming",		// Fran�ais
		"",		// Norsk
		"Suodatetun tiedon suoratoisto",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"������ ������",		// Russian
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
		"Trasmissione attiva",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Streaming actif",		// Fran�ais
		"",		// Norsk
		"Suoratoistopalvelin aktiivinen",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"�������� �������",		// Russian
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
		"Nascondi voce menu principale",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"Masquer dans le menu principal",		// Fran�ais
		"",		// Norsk
		"Piilota valinta p��valikosta",		// suomi
		"",		// Polski
		"",		// Espa�ol
		"",		// Ellinika
		"",		// Svenska
		"",		// Romaneste
		"",		// Magyar
		"",		// Catala
		"�������� � ������� ����",		// Russian
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
		"Server trasmissione Multicast",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"Multicast-suoratoistopalvelin",		// suomi
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
		"Avvia Server IGMP",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"K�ynnist� IGMP-palvelin",		// suomi
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
		"Porta Client Multicast",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"Multicast-portti",		// suomi
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
		"Tipo flusso Multicast",		// Italiano
		"",		// Nederlands
		"",		// Portugu�s
		"",		// Fran�ais
		"",		// Norsk
		"Multicast-l�hetysmuoto",		// suomi
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

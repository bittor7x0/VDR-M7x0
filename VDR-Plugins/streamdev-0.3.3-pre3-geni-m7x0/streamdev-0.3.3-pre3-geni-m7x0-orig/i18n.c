/*
 *  $Id$
 */
 
#include "i18n.h"

const char *i18n_name = NULL;

const tI18nPhrase Phrases[] = {
	{	"VDR Streaming Server",	// English
		"VDR Streaming Server",	// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"VDR-suoratoistopalvelin",								// suomi
		"",											// Polski
		"Servidor Streaming VDR", // Español
		"",											// Ellinika / Greek
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"VTP Streaming Client",	// English
		"VTP Streaming Client",	// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"VTP-suoratoistoasiakas ",								// suomi
		"",											// Polski
		"Cliente Streaming VTP", // Español
		"",											// Ellinika / Greek
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Start VDR-to-VDR Server",// English
		"VDR-zu-VDR Server starten",// Deutsch
		"",											// Slovenski
		"Avvia il Server VDR-toVDR",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Käynnistä VDR-palvelin",								// suomi
		"",											// Polski
		"Iniciar Servidor VDR-a-VDR", // Español
		"",											// Ellinika / Greek
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Start HTTP Server",		// English
		"HTTP Server starten",	// Deutsch
		"",											// Slovenski
		"Avvia il Server HTTP", // Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Käynnistä HTTP-palvelin",								// suomi
		"",											// Polski
		"Iniciar Servidor HTTP", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"HTTP Streamtype",		  // English
		"HTTP Streamtyp",	      // Deutsch
		"",											// Slovenski
		"Tipo di Stream HTTP",  // Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"HTTP-lähetysmuoto",									// Suomi
		"",											// Polski
		"Tipo de Stream HTTP", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Start Client",					// English
		"Client starten",				// Deutsch
		"",											// Slovenski
		"Avvia il Client",			// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Käynnistä VDR-asiakas",								// suomi
		"",											// Polski
		"Iniciar Cliente", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"VDR-to-VDR Server Port",// English
		"Port des VDR-zu-VDR Servers",// Deutsch
		"",											// Slovenski
		"Porta del Server VDR-to-VDR",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"VDR-palvelimen portti",								// Suomi
		"",											// Polski
		"Puerto Servidor VDR-a-VDR", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"HTTP Server Port",			// English
		"Port des HTTP Servers",// Deutsch
		"",											// Slovenski
		"Porta del Server HTTP",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"HTTP-palvelimen portti",								// suomi
		"",											// Polski
		"Puerto Servidor HTTP", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Maximum Number of Clients",// English
		"Maximalanzahl an Clients",// Deutsch
		"",											// Slovenski
		"Numero Massimo di Client",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Suurin sallittu asiakkaiden määrä",							// suomi
		"",											// Polski
		"Número Máximo de Clientes", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Remote IP",						// English
		"IP der Gegenseite",		// Deutsch
		"",											// Slovenski
		"Indirizzo IP del Server",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Etäkoneen IP-osoite",									// suomi
		"",											// Polski
		"IP Remota", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Remote Port",					// English
		"Port der Gegenseite",	// Deutsch
		"",											// Slovenski
		"Porta del Server Remoto",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Etäkoneen portti",									// suomi
		"",											// Polski
		"Puerto Remoto", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Remote Streamtype",		// English
		"Streamtyp von Gegenseite",// Deutsch
		"",											// Slovenski
		"Tipo di Stream",       // Italiano (oppure Flusso ?)
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Etäkoneen lähetysmuoto",								// suomi
		"",											// Polski
		"Tipo de Stream Remoto", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Common Settings",		  // English
		"Allgemeines",          // Deutsch
		"",											// Slovenski
		"Settaggi Comuni",			// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Yleiset asetukset",									// suomi
		"",											// Polski
		"Ajustes comunes", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"VDR-to-VDR Server",		// English
		"VDR-zu-VDR Server",    // Deutsch
		"",											// Slovenski
		"Server VDR-to-VDR",		// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"VDR-palvelin",										// suomi
		"",											// Polski
		"Servidor VDR-a-VDR", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"HTTP Server",		      // English
		"HTTP Server",          // Deutsch
		"",											// Slovenski
		"Server HTTP",					// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"HTTP-palvelin",									// suomi
		"",											// Polski
		"Servidor HTTP", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"VDR-to-VDR Client",		// English
		"VDR-zu-VDR Client",    // Deutsch
		"",											// Slovenski
		"Client VDR-to-VDR",		// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"VDR-asiakas",										// suomi
		"",											// Polski
		"Cliente VDR-a-VDR", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Please restart VDR to activate changes",// English
		"Bitte starten Sie für die Änderungen VDR neu",// Deutsch
		"",											// Slovenski
		"Riavviare VDR per attivare i cambiamenti",// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Aktivoi muutokset käynnistämällä VDR uudelleen",					// suomi
		"",											// Polski
		"Reinicie VDR para activar los cambios", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Synchronize EPG",			// English
		"EPG synchronisieren",	// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Päivitä ohjelmaopas",									// suomi
		"",											// Polski
		"Sincronizar EPG", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Suspend Live TV",			// English
		"Live-TV pausieren",		// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Pysäytä suora TV-lähetys",								// suomi
		"",											// Polski
		"Suspender TV-Directo", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Suspend behaviour",		// English
		"Pausierverhalten",			// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Pysäytystoiminto",									// suomi
		"",											// Polski
		"Suspender funcionamiento", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Offer suspend mode",		// English
		"Pausieren anbieten",		// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"tyrkytä",										// suomi
		"",											// Polski
		"Ofrecer suspender", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Always suspended",			// English
		"Immer pausiert",				// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"aina",											// suomi
		"",											// Polski
		"Siempre suspendido", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Never suspended",			// English
		"Nie pausiert",					// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"ei koskaan",										// suomi
		"",											// Polski
		"Nunca suspendido", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Streaming Control",		// English
		"Streamkontrolle",			// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Suoratoiston hallinta",								// suomi
		"",											// Polski
		"Control Streaming", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Fetching recordings...",// English
		"Hole Aufnahmen...",		// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Haetaan tallenteita...",								// suomi
		"",											// Polski
		"Obtener grabaciones...", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Remote Recordings",		// English
		"Entfernte Aufnahmen",	// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Etätallenteet",									// suomi
		"",											// Polski
		"Grabaciones Remotas", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Remote Timers",				// English
		"Entfernte Timer",			// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Etäajastimet",										// suomi
		"",											// Polski
		"Programaciones Remotas", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Suspend Server",				// English
		"Server pausieren",			// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Pysäytä palvelin",									// suomi
		"",											// Polski
		"Suspender Servidor", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Server is suspended",	// English
		"Server ist pausiert",	// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Palvelin on pysäytetty",								// suomi
		"",											// Polski
		"Servidor suspendido", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Couldn't suspend Server!",// English
		"Konnte Server nicht pausieren!",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Palvelinta ei onnistuttu pysäyttämään!",						// suomi
		"",											// Polski
		"¡No puede suspender el Servidor!", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Edit remote timer",		// English
		"Entfernten Timer editieren",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Muokkaa etäajastinta",									// suomi
		"",											// Polski
		"Editar programación remota", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Timers not in sync! Try again...",// Englisch
		"Timer nicht synchron! Bitte wiederholen...",//Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Ajastimet eivät täsmää! Yritä uudelleen...",						// suomi
		"",											// Polski
		"¡Programaciones desincronizadas! Pruebe otra vez...", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Couldn't save timer! Try again...",// English
		"Konnte Timer nicht speichern! Bitte wiederholen...",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Ajastimen tallennus epäonnistui! Yritä uudelleen...",					// suomi
		"",											// Polski
		"¡No puedo guardar programación! Pruebe otra vez...", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Couldn't delete timer! Try again...",// English
		"Konnte Timer nicht löschen! Bitte wiederholen...",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Ajastimen poistaminen epäonnistui! Yritä uudelleen...",				// suomi
		"",											// Polski
		"¡No puedo borrar programación! Pruebe otra vez...", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Server error! Try again...",// English
		"Serverfehler! Bitte wiederholen...",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Palvelimessa virhe! Yritä uudelleen...",						// suomi
		"",											// Polski
		"¡Error Servidor! Pruebe otra vez...", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"MultiPID Streaming",		// English
		"Multiple PIDs streamen",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Usean PID:in suoratoisto",								// suomi
		"",											// Polski
		"Streaming MultiPID", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Client may suspend",		// English
		"Client darf pausieren",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Asiakas saa pysäyttää palvelimen",							// suomi
		"",											// Polski
		"Debe suspender el cliente", // Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
        {       "Bind to IP",           // English
                "",// Deutsch
                "",                                                                                     // Slovenski
                "",                                                                                     // Italiano
                "",                                                                                     // Nederlands
                "",                                                                                     // Português
                "",                                                                                     // Français
                "",                                                                                     // Norsk
                "Sido osoitteeseen",                                                                    // suomi
                "",                                                                                     // Polski
                "Asociar a IP", // Español
                "",                                                                                     // Ellinika
                "",                                                                                     // Svenska
                "",                                                                                     // Romaneste
                "",                                                                                     // Magyar
                "",                                                                                     // Catala
#if VDRVERSNUM >= 10300
                ""                      // Russian
#endif
        },
        {       "Remote Schedule",      // English
                "",// Deutsch
                "",                                                                                     // Slovenski
                "",                                                                                     // Italiano
                "",                                                                                     // Nederlands
                "",                                                                                     // Português
                "",                                                                                     // Français
                "",                                                                                     // Norsk
                "Etäkoneen ohjelmaopas",                                                                // suomi
                "",                                                                                     // Polski
                "Programado Remoto", // Español
                "",                                                                                     // Ellinika
                "",                                                                                     // Svenska
                "",                                                                                     // Romaneste
                "",                                                                                     // Magyar
                "",                                                                                     // Catala
#if VDRVERSNUM >= 10300
                ""                      // Russian
#endif
        },
        {       "Filter Streaming",     // English
                "",// Deutsch
                "",                                                                                     // Slovenski
                "",                                                                                     // Italiano
                "",                                                                                     // Nederlands
                "",                                                                                     // Português
                "",                                                                                     // Français
                "",                                                                                     // Norsk
                "Suodatetun tiedon suoratoisto",                                                        // suomi
                "",                                                                                     // Polski
                "Filtro Streaming", // Español
                "",                                                                                     // Ellinika
                "",                                                                                     // Svenska
                "",                                                                                     // Romaneste
                "",                                                                                     // Magyar
                "",                                                                                     // Catala
#if VDRVERSNUM >= 10300
                ""                      // Russian
#endif
        },
        {       "Streaming active",     // English
                "Streamen im Gange",// Deutsch
                "",                                                                                     // Slovenski
                "",                                                                                     // Italiano
                "",                                                                                     // Nederlands
                "",                                                                                     // Português
                "",                                                                                     // Français
                "",                                                                                     // Norsk
                "Suoratoistopalvelin aktiivinen", // suomi
                "",                                                                                     // Polski
                "Activar Streaming", // Español
                "",                                                                                     // Ellinika
                "",                                                                                     // Svenska
                "",                                                                                     // Romaneste
                "",                                                                                     // Magyar
                "",                                                                                     // Catala
#if VDRVERSNUM >= 10300
                ""                      // Russian
#endif
        },
	{ NULL }
};

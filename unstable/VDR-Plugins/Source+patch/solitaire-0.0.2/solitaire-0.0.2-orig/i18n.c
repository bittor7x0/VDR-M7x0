// originally from freecell by Sascha Volkenandt (sascha@akv-soft.de)

#include "i18n.h"

const tI18nPhrase Phrases[] = {
	{	"Solitaire",							// English
		"Solitaire",							// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Pasianssi",	// suomi
		"",											// Polski
		"",											// Español
		"",											// Ellinika / Greek
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"The well-known card game",// English
		"Das bekannte Kartenspiel",// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Korttipeliklassikko",	// suomi
		"",											// Polski
		"",											// Español
		"",											// Ellinika / Greek
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"You won!",							// English
		"Sie haben gewonnen!",	// Deutsch
		"",											// Slovenski
		"",											// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Voitit!",	// suomi
		"",											// Polski
		"",											// Español
		"",											// Ellinika / Greek
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Select stack",					// English
		"Stapel wählen",				// Deutsch
		"",											// Slovenski
		"", 										// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Valitse pino",	// suomi
		"",											// Polski
		"",											// Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"Press OK to start a new game",// English
		"OK beginnt ein neues Spiel",// Deutsch
		"",											// Slovenski
		"", 										// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Käynnistä uusi peli painamalla 'OK'",// suomi
		"",											// Polski
		"",											// Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{	"This is Solitaire, a card-game for the Video Disk Recorder.\n\n"
		"Up/Down: Jump cells/stacks\n"
		"Left/Right: Move hand/cards\n"
		"OK: Grab card or stack\n"
		"Up/Down: Select/Deselect cards\n"
		"OK: Confirm stack selection\n"
		"Blue: Start new game\n\n"
		"Press OK to resume playing", // English
		"Dies ist Solitaire, ein Kartenspiel für den Video Disk Recorder.\n\n"
		"Rauf/Runter: Zellen/Stapel springen\n"
		"Links/Rechts: Hand/Stapel bewegen\n"
		"OK: Karte oder Stapel nehmen\n"
		"Rauf/Runter: Karten An-/Abwählen\n"
		"OK: Stapelwahl bestätigen\n"
		"Blau: Neues Spiel starten\n\n"
		"OK drücken um fortzusetzen", // Deutsch
		"",											// Slovenski
		"", 										// Italiano
		"",											// Nederlands
		"",											// Português
		"",											// Français
		"",											// Norsk
		"Pasianssi - korttipeliklassikko VDR:lle.\n\n" 
		"'Ylös/Alas': Siirry kenttien/pinojen välillä\n" 
		"'Vasen/Oikea': Siirrä kursoria/korttia\n" 
		"'OK': Valitse kortti tai pino\n" 
		"'Ylös/Alas': Valitse kortti\n" 
		"'OK': Hyväksy pinon valinta\n" 
		"'Sininen': Käynnistä uusi peli\n\n" 
		"Paina 'OK' jatkaaksesi peliä",	// suomi
		"",											// Polski
		"",											// Español
		"",											// Ellinika
		"",											// Svenska
		"",											// Romaneste
		"",											// Magyar
		"",											// Catala
#if VDRVERSNUM >= 10300
		""                      // Russian
#endif
	},
	{ NULL }
};

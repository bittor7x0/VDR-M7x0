/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id:
 */

#include "i18n.h"

 const tI18nPhrase Phrases[] = {
  { "Search Transponders for DVB Channels", //EN
    "Durchsucht Transponder nach DVB Kan�len.",// DE
    "", // Slovenski
    "Ricerca Transponders di Canali DVB",// IT
    "Doorzoek Transponders naar DVB kanalen", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "Kanavahaku DVB-transpondereille",// FI
    "", // Polski
    "Buscar transponders de canales DVB", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "����� �������", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Search Mode",
    "Suchmodus",
    "",
    "Modalita' di Ricerca",
    "Zoek mode",
    "",
    "",
    "",
    "Hakutapa",
    "",
    "Modo b�squeda",
    "",
    "",
    "",
    "",
    "",
    "����� ������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "scanning on transponder",
    "Suche auf Transponder",
    "",
    "Ricerca trasponder",
    "Scan op transponder",
    "",
    "",
    "",
    "haetaan transponderilta",
    "",
    "buscando transponder",
    "",
    "",
    "",
    "",
    "",
    "������������ ������������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#ifdef M750S
  { "Scanning configured satellites",
    "Durchsuche eingerichtete Satelliten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando sat�lites configurados",
    "",
    "",
    "",
    "",
    "",
    "������������ ����������� ���������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "DiSEqC",
    "DiSEqC",
    "DiSEqC",
    "DiSEqC",
    "DiSEqC",
    "",
    "",
    "",
    "DiSEqC-kytkin",
    "",
    "DiSEqC",
    "",
    "",
    "",
    "",
    "",
    "DiSEqC",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#endif
  { "FEC",
    "FEC",
    "",
    "FEC",
    "FEC",
    "",
    "",
    "",
    "FEC",
    "",
    "FEC",
    "",
    "",
    "",
    "",
    "",
    "FEC",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "None",
    "Keine",
    "",
    "Nessun",
    "Geen",
    "",
    "",
    "",
    "ei",
    "",
    "Ninguno",
    "",
    "",
    "",
    "",
    "",
    "���",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Auto",
    "Auto",
    "",
    "Auto",
    "Auto",
    "",
    "",
    "",
    "auto",
    "",
    "Auto",
    "",
    "",
    "",
    "",
    "",
    "����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "SearchMode$Auto",
    "Automatisch",
    "",
    "Ricerca automatica",
    "Automatisch",
    "",
    "",
    "",
    "automaattinen",
    "",
    "Autom�tica",
    "",
    "",
    "",
    "",
    "",
    "����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Manual",
    "Manuell",
    "",
    "Manuale",
    "Handmatig",
    "",
    "",
    "",
    "manuaalinen",
    "",
    "Manual",
    "",
    "",
    "",
    "",
    "",
    "������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channelscan",
    "Kanalsuche",
    "",
    "Scansione canali",
    "Kanaal scan",
    "",
    "",
    "",
    "Kanavahaku",
    "",
    "Buscar canales",
    "",
    "",
    "",
    "",
    "",
    "����� �������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Detailed search",
    "Detaillierte Suche",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "B�sq. detallada",
    "",
    "",
    "",
    "",
    "",
    "������ �����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Position",
    "Position",
    "",
    "Posizione",
    "Positie",
    "",
    "",
    "",
    "Sijainti",
    "",
    "Posici�n",
    "",
    "",
    "",
    "",
    "",
    "�������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Tuner Error",
    "Tuner Fehler",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Error en sintonizador",
    "",
    "",
    "",
    "",
    "",
    "������ ������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning on transponder",
    "Durchsuche Transponder",
    "",
    "Ricerca Transponders",
    "Scannen op transponder",
    "",
    "",
    "",
    "Haetaan transponderilta",
    "",
    "Buscando transponder",
    "",
    "",
    "",
    "",
    "",
    "������������ ������������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "channels in current list",
    "Kan�le in aktueller Liste",
    "",
    "Canali presenti nella lista",
    "Kanalen in huidige lijst",
    "",
    "",
    "",
    "T�m�nhetkiset kanavat",
    "",
    "canales en lista actual",
    "",
    "",
    "",
    "",
    "",
    "������� � ������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "TV CHANNELS",
    "TV KAN�LE",
    "",
    "CANALI TV",
    "TV KANALEN",
    "",
    "",
    "",
    "TV-kanavat",
    "",
    "CANALES TV",
    "",
    "",
    "",
    "",
    "",
    "������ ²",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "No new channels found",
    "Keine neuen Kan�le gefunden",
    "",
    "Non sono stati trovati nuovi canali",
    "Geen nieuwe kanalen gevonden",
    "",
    "",
    "",
    "Uusia kanavia ei l�ydetty",
    "",
    "Ning�n nuevo canal encontrado",
    "",
    "",
    "",
    "",
    "",
    "��� ����� �������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning aborted by User",
    "Suche abgebrochen",
    "",
    "Ricerca interrotta dall'Utente",
    "Scannen afgebroken door User",
    "",
    "",
    "",
    "Haku keskeytetty",
    "",
    "B�squeda cancelada por el usuario",
    "",
    "",
    "",
    "",
    "",
    "����� ������� �������������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },

  { "TV CHANNELS                     RADIO",
    "TV KAN�LE                       RADIO",
    "",
    "CANALI TV                       RADIO",
    "TV KANALEN                      RADIO",
    "",
    "",
    "",
    "TV-kanavat                      Radio",
    "",
    "CANALES TV                      RADIO",
    "",
    "",
    "",
    "",
    "",
    "������ ²                       �����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Symbolrate",
    "Symbolrate",
    "",
    "Symbolrate",
    "Symbolrate",
    "",
    "",
    "",
    "Symbolinopeus",
    "",
    "S�mbolo tipo",
    "",
    "",
    "",
    "",
    "",
    "����.��������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Frequency",
    "Frequenz",
    "",
    "Frequenza",
    "Frequentie",
    "",
    "",
    "",
    "Taajuus",
    "",
    "Frequencia",
    "",
    "",
    "",
    "",
    "",
    "�������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s\tPlease Wait",
    "Durchsuche %s\tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Haku k�ynniss� %s.\tOdota hetkinen...",
    "",
    "Buscando %s\tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "������������ �����\t�����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%iMHz) \tPlease Wait",
    "Durchsuche %s (%iMHz) \tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Haku k�ynniss� %s.\tOdota hetkinen...",
    "",
    "Buscando %s (%iMHz) \tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "����� %s (%iMHz) \t�����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%.3fMHz) \tPlease Wait",
    "Durchsuche %s (%.3fMHz) \tBitte warten",
    "",
    "",
    "",
    "",
    "",
    "",
    "Haku k�ynniss� %s.\tOdota hetkinen...",
    "",
    "Buscando %s (%.3fMHz) \tPor favor, espere",
    "",
    "",
    "",
    "",
    "",
    "����� %s (%.3fMHz) \t�����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Change",
    "Ausw�hlen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambiar",
    "",
    "",
    "",
    "",
    "",
    "�������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Start",
    "Start",
    "",
    "Start",
    "Start",
    "",
    "",
    "",
    "Aloita",
    "",
    "Iniciar",
    "",
    "",
    "",
    "",
    "",
    "����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Backup",
    "Sichern",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Guardar",
    "",
    "",
    "",
    "",
    "",
    "���������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Delete",
    "L�schen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Borrar",
    "",
    "",
    "",
    "",
    "",
    "�������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$New",
    "Neu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Nuevo",
    "",
    "",
    "",
    "",
    "",
    "�����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Radio only",
    "nur Radio",
    "",
    "Solo radio",
    "Alleen Radio",
    "",
    "",
    "",
    "vain radio",
    "",
    "S�lo radio",
    "",
    "",
    "",
    "",
    "",
    "������ �����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "TV only",
    "nur TV",
    "",
    "Solo TV",
    "Alleen TV",
    "",
    "",
    "",
    "vain TV",
    "",
    "S�lo TV",
    "",
    "",
    "",
    "",
    "",
    "������ ²",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Radio + TV",
    "Radio + TV",
    "",
    "Radio + TV",
    "Radio + TV",
    "",
    "",
    "",
    "radio + TV",
    "",
    "Radio + TV",
    "",
    "",
    "",
    "",
    "",
    "����� + ²",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Radio + TV + NVOD",
    "Radio + TV + NVOD",
    "",
    "Radio + TV + NVOD",
    "Radio + TV + NVOD",
    "",
    "",
    "",
    "radio + TV + NVOD",
    "",
    "Radio + TV + NVOD",
    "",
    "",
    "",
    "",
    "",
    "����� + ² + NVOD",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Service Type",
    "Service Arten",
    "",
    "Tipo servizio",
    "Service type",
    "",
    "",
    "",
    "Haettavat palvelut",
    "",
    "Tipo de servicio",
    "",
    "",
    "",
    "",
    "",
    "���� ��������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "enabled",
    "aktiviert",
    "",
    "abilitato",
    "ingeschakeld",
    "",
    "",
    "",
    "p��ll�",
    "",
    "activado",
    "",
    "",
    "",
    "",
    "",
    "�������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "disabled",
    "deaktiviert",
    "",
    "disabilitato",
    "uitgeschakeld",
    "",
    "",
    "",
    "pois",
    "",
    "desactivado",
    "",
    "",
    "",
    "",
    "",
    "��������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Added new channels",
    "Neue Kan�le hinzugef�gt",
    "",
    "Aggiunti nuovi canali",
    "Nieuwe kanalen toegevoegd",
    "",
    "",
    "",
    "Uudet kanavat lis�tty",
    "",
    "Nuevos canales a�adidos",
    "",
    "",
    "",
    "",
    "",
    "��������� ����� �������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Frequency (kHz)",
    "Frequenz (kHz)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Frecuencia (kHz)",
    "",
    "",
    "",
    "",
    "",
    "������� (ڳ�)",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Frequency (MHz)",
    "Frequenz (MHz)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Frecuencia (MHz)",
    "",
    "",
    "",
    "",
    "",
    "������� (���)",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Expert",                                                                                               
    "Experten",                                                                                             
    "",                                                                                                     
    "Esperto",                                                                                              
    "Expert",                                                                                               
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "Experto",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "�������",                                                                                                     
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channel sel.",                                                                                               
    "Kanalliste",                                                                                             
    "",                                                                                                     
    "",                                                                                              
    "",                                                                                               
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "Lista canales",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "",                                                                                                     
    "������ �������",                                                                                              
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#ifdef M750S
  { "DVB-S - Satellite",
    "DVB-S - Satellit",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DVB-S - Sat�lite",
    "",
    "",
    "",
    "",
    "",
    "DVB-S - �������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "DVB-C - Cable",
    "DVB-C - Kabel",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DVB-C - Cable",
    "",
    "",
    "",
    "",
    "",
    "DVB-C - ������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#endif
  { "DVB-T - Terrestrial",
    "DVB-T - Terrestrisch",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "DVB-T - Terrestre",
    "",
    "",
    "",
    "",
    "",
    "DVB-T - ����",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Press OK to proceede",
    "Dr�cken Sie OK um fortzufahren",
    "",
    "Premere OK per continuare",
    "Druk OK om te vervolgen",
    "",
    "",
    "",
    "",
    "",
    "Pulse OK para continuar",
    "",
    "",
    "",
    "",
    "",
    "������� �� ����� ����������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Press OK to finish or Exit for new scan",
    "Dr�cken Sie OK zum Beenden oder Exit f�r eine neue Kanalsuche.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Pulse OK para terminar o Exit para escanear otra vez",
    "",
    "",
    "",
    "",
    "",
    "������� �� ����� ��������� ��� ��� ������ ������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Standard",
    "Standard",
    "",
    "Standard", // Italiano
    "Standaard",
    "",
    "Standart",
    "",
    "Vakio",
    "",
    "Est�ndar",
    "",
    "",
    "",
    "",
    "",
    "�����������",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Extended",
    "Erweitert",
    "",
    "",
    "",
    "",
    "Pr�cision",
    "",
    "Laaja",
    "",
    "Ampliado",
    "",
    "",
    "",
    "",
    "",
    "�����������",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Retrieving transponder list from %s",
    "Erhalte Transponderliste von %s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Recibiendo lista de transponders desde %s",
    "",
    "",
    "",
    "",
    "",
    "������� ������ ������������� �� %s",
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Terrestrial",
    "Terr.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Terrestre",
    "",
    "",
    "",
    "",
    "",
    "����",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#ifdef M750S
  { "Cable",
    "Kabel",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cable",
    "",
    "",
    "",
    "",
    "",
    "������",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
#endif
  { "Running services on transponder: %i / %i",
    "Aktive Dienste auf Transponder: %i / %i",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ejecutando servicios en transponder: %i / %i",
    "",
    "",
    "",
    "",
    "",
    "�������� ������� �� ������������: %i / %i",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%iMHz)\t%s",
    "Durchsuche %s (%iMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s (%iMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "����� %s (%iMHz)\t%s",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Scanning %s (%.3fMHz)\t%s",
    "Durchsuche %s (%.3fMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Buscando %s (%.3fMHz)\t%s",
    "",
    "",
    "",
    "",
    "",
    "����� %s (%.3fMHz)\t%s",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Changes Done",
    "�nderung �bernommen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambios realizados",
    "",
    "",
    "",
    "",
    "",
    "��������� ���������",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Changes failed",
    "�nderung fehlgeschlagen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Cambios err�neos",
    "",
    "",
    "",
    "",
    "",
    "��������� �� ���������",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Channel lists",
    "Kanallisten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Listas de canales",
    "",
    "",
    "",
    "",
    "",
    "������ �������",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Please check your Timers!",
    "Bitte Timer �berpr�fen!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "�Compruebe sus programaciones!",
    "",
    "",
    "",
    "",
    "",
    "��������� ���������� �������!",
    "",
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { NULL }
  };


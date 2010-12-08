/*
 * channellists.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "i18n.h"

const tI18nPhrase Phrases[] = {
  { "Channellists", // English
    "Kanallisten", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Liste des cha�nes", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Manage your channellists", // English
    "Verwalte deine Kanallisten", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Organiser la liste des cha�nes", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Hide main menu entry", // English
    "Hauptmen�eintrag verstecken", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Invisible dans le menu principale", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Action after switch", // English
    "Aktion nach dem Wechsel", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Menu", // English
    "Men�", // Deutsch
    "Meni", // Slovenski
    "Menu", // Italiano
    "Menu", // Nederlands
    "Menu", // Portugu�s
    "Menu", // Fran�ais
    "Meny", // Norsk
    "Valikko", // suomi
    "Menu", // Polski
    "Men�", // Espa�ol
    "M����", // �������� (Greek)
    "Meny", // Svenska
    "Meniu", // Romaneste
    "Men�", // Magyar
    "Men�", // Catal�
    "����", // ������� (Russian)
    "Izbor", // Hrvatski (Croatian)
    "Men��", // Eesti
    "Menu", // Dansk
    "Menu", // �esky (Czech)
  },
  { "close", // English
    "schliessen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "none", // English
    "keine", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button Red", // English
    "Taste Rot", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Button rouge", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button Green", // English
    "Taste Gr�n", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Button vert", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Switch current channellist?", // English
    "Aktuelle Kanalliste wechseln?", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Changer la liste de cha�nes actuelle?", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Script", // English
    "Skript", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Script", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Rename", // English
    "Umbenennen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Renommer", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Copy", // English
    "Kopieren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Copier", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Save", // English
    "Sichern", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Sauvegarder", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Name", // English
    "Name", // Deutsch
    "Ime", // Slovenski
    "Nome", // Italiano
    "Naam", // Nederlands
    "Nome",// Portugu�s
    "Nom", // Fran�ais
    "Navn", // Norsk
    "Nimi", // suomi
    "Nazwa", // Polski
    "Nombre", // Espa�ol
    "�����", // �������� (Greek)
    "Namn", // Svenska
    "Nume", // Romaneste
    "N�v", // Magyar
    "Nom", // Catal�
    "��������", // ������� (Russian)
    "Naziv", // Hrvatski (Croatian)
    "Nimi", // Eesti
    "Navn", // Dansk
    "N�zev", // �esky (Czech)
  },
  { "Delete channellist?", // English
    "Kanalliste l�schen?", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Effacer la liste de cha�nes?", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Execute update script?", // English
    "Update Skript ausf�hren?", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Effectuer le script de mise � jours", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Waiting...", // English
    "Warte...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Patience...", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "An error occurs, see syslog...", // English
    "Ein Fehler ist aufgetreten, siehe syslog...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Une erreur se produit, voir syslog...", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Rename", // English
    "Umbenennen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Renommer", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Script", // English
    "Skript", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Script", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Copy", // English
    "Kopieren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Copier", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { "Button$Delete", // English
    "L�schen", // Deutsch
    "Izbri�i", // Slovenski
    "Cancella", // Italiano
    "Verwijder", // Nederlands
    "Apagar", // Portugu�s
    "Supprimer", // Fran�ais
    "Slett", // Norsk
    "Poista", // suomi
    "Usu�", // Polski
    "Borrar", // Espa�ol
    "��������", // �������� (Greek)
    "Ta bort", // Svenska
    "�terge", // Romaneste
    "T�r�lni", // Magyar
    "Esborrar", // Catal�
    "�������", // ������� (Russian)
    "Obri�i", // Hrvatski (Croatian)
    "Kustuta", // Eesti
    "Slet", // Dansk
    "Smazat", // �esky (Czech)
  },
  { "Button$Save", // English
    "Sichern", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Portugu�s
    "Sauvegarder", // Fran�ais
    "", // Norsk
    "", // suomi
    "", // Polski
    "", // Espa�ol
    "", // �������� (Greek)
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Catal�
    "", // ������� (Russian)
    "", // Hrvatski (Croatian)
    "", // Eesti
    "", // Dansk
    "", // �esky (Czech)
  },
  { NULL }
  };

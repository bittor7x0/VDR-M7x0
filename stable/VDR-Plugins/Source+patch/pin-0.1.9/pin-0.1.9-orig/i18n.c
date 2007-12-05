/*
 * i18n.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

//***************************************************************************
// Includes
//***************************************************************************

#include "i18n.h"
#include <stdio.h>

//***************************************************************************
// Constants
//***************************************************************************

const tI18nPhrase Phrases[] =
{
   {
		"Childlock plugin",       // English
		"Kindersicherung",        // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection Parentale",   // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekz�r", // Magyar
		"", // Catala
		"����������", // Russian
		"", // Croatian
	},
   {
		"Pin Code: ", // English
		"Pin Eingabe: ", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Le code pin", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Pin k�d", // Magyar
		"", // Catala
		"��� ���", // Russian
		"", // Croatian
	},
   {
		"Skip protected channel silent", // English
		"Gesperrte Kan�le ohne Meldung �berspringen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Les cha�nes prot�g�s seront ignor�es sans infos", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Z�rolt csatorn�kat nem megjelen�teni", // Magyar
		"", // Catala
		"���������� ���������� ����� ����", // Russian
		"", // Croatian
	},
   {
		"Pin inactivity time [min]", // English
		"Sperre aktivieren nach Inaktivit�t [min]", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection apr�s une inactivit�es de [min]", // Fran�ais
		"", // Norsk
		"", // Suomi
        "", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Z�r aktiv�l�sa [perc] inaktivit�s ut�n", // Magyar
		"", // Catala
		"������ ������������ ��� [�����]", // Russian
		"", // Croatian
	},
   {
		"Ignoring invalid pin", // English
		"Ung�ltiger code", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Le code est fau", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"�rv�nytelen pin k�d", // Magyar
		"", // Catala
		"������������ �������� ���", // Russian
		"", // Croatian
	},
   {
		"Please enable pin protection first", // English
		"Bitte zuerst Kindersicherung freischalten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Entr�e le code de protection", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"K�rem oldja fel a gyerekz�rat", // Magyar
		"", // Catala
		"���������� ������� ������� ��� ���", // Russian
		"", // Croatian
	},
   {
		"Recording protected, enter pin code first", // English
		"Aufnahme gesperrt, bitte Kindersicherung freischalten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Entr�e le code de protection", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"K�rem oldja fel a gyerekz�rat", // Magyar
		"", // Catala
		"���������� ������� ������� ��� ���", // Russian
		"", // Croatian
	},
   {
		"already protected", // English
		"bereits gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Est d�j� prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Kor�bban m�r z�rolt", // Magyar
		"", // Catala
		"��� �������������", // Russian
		"", // Croatian
	},
   {
		"added to protection list", // English
		"hinzugef�gt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"ajouter", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Z�roltakhoz hozz�adva", // Magyar
		"", // Catala
		"��������� � ������ ����������", // Russian
		"", // Croatian
	},
   {
		"Recording protected", // English
		"Aufnahme gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Engeristrement prot�g�s, d'abord mettre le code parental", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Felv�telhez fel kell oldja a gyerekz�rat", // Magyar
		"", // Catala
		"������ �������������, ������� ������� ��� ���", // Russian
		"", // Croatian
	},
   {
		"channel is protected", // English
		"Kanal gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Cha�nes prot�g�e", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Csatorna z�rolva", // Magyar
		"", // Catala
		"����� ������������", // Russian
		"", // Croatian
	},
   {
		"broadcast is protected", // English
		"Sendung gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Emission prot�g�e", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Ad�s z�rolva", // Magyar
		"", // Catala
		"������������� �������������", // Russian
		"", // Croatian
	},
   {
		"Activated pin protection", // English
		"Kindersicherung aktiviert", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Activ� la protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekz�r aktiv�lva", // Magyar
		"", // Catala
		"������������ ����������", // Russian
		"", // Croatian
	},
   {
		"Childlock",       // English
		"Kindersicherung", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekz�r", // Magyar
		"", // Catala
		"����������", // Russian
		"", // Croatian
	},
   {
		"Delete", // English
		"L�schen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Effacer", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"T�rl�s", // Magyar
		"", // Catala
		"�������", // Russian
		"", // Croatian
	},
   {
		"Add", // English
		"Hinzuf�gen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Ajouter", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Hozz�ad�s", // Magyar
		"", // Catala
		"��������", // Russian
		"", // Croatian
	},
   {
		"Protected channels", // English
		"Gesperrte Kan�le", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Cha�nes prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Lez�rt csatorn�k", // Magyar
		"", // Catala
		"��������������� ������", // Russian
		"", // Croatian
	},
   {
		"Protected broadcasts", // English
		"Gesperrte Sendungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Emissions prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Lez�rt ad�sok", // Magyar
		"", // Catala
		"��������������� �������������", // Russian
		"", // Croatian
	},
   {
		"Plugin protections", // English
		"Sperren / Entsperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Plugin sous protection", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Add current channel to protection list", // English
		"Aktuellen Kanal sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Ajouter la cha�ne actuelle � la protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Aktu�lis csatorna z�roltakhoz ad�sa", // Magyar
		"", // Catala
		"�������� ������� ����� � ����.  ������", // Russian
		"", // Croatian
	},
   {
		"Add current broadcast to protection list", // English
		"Aktuelle Sendung sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Ajouter l'�mission actuelle dans la liste de protection parentale", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Aktu�lis ad�s z�roltakhoz ad�sa", // Magyar
		"", // Catala
		"�������� ������� ������������� � ����. ������", // Russian
		"", // Croatian
	},
   {
		"Edit channel locks", // English
		"Gesperrte Kan�le bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Editer la liste de cha�nes", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Z�rolt csatorn�k szerkeszt�se", // Magyar
		"", // Catala
		"������������� ���������� ������", // Russian
		"", // Croatian
	},
   {
		"Edit broadcast locks", // English
		"Gesperrte Sendungen bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Editer les �missions prot�g�s", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Z�rolt ad�sok szerkeszt�se", // Magyar
		"", // Catala
		"������������� ���������� �������������", // Russian
		"", // Croatian
	},
   {
		"Edit plugin locks", // English
		"Systemeinstellungen bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"editer la protection de plugin", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Edit menu locks", // English
		"Men�sperren bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Lock setup", // English
		"Setup sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Unlock setup", // English
		"Setup entsperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Activate childlock", // English
		"Kindersicherung aktivieren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection parentales activ�e", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Gyerekz�r aktiv�l�sa", // Magyar
		"", // Catala
		"������������ ����������", // Russian
		"", // Croatian
	},
   {
		"Remove entry?", // English
		"Eintrag entfernen?", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Enlever?", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"T�rl�se?", // Magyar
		"", // Catala
		"�����?", // Russian
		"", // Croatian
	},
   {
		"Edit", // English
		"Bearbeiten", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Editer", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"�������������", // Russian
		"", // Croatian
	},
   {
		"Channel", // English
		"Kanal", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Cha�ne", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Csatorna", // Magyar
		"", // Catala
		"�����", // Russian
		"", // Croatian
	},
   {
		"Lock active", // English
		"Sperre aktiv", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection active", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Z�r aktiv�lva", // Magyar
		"", // Catala
		"���������� �������", // Russian
		"", // Croatian
	},
   {
		"already in list", // English
		"bereits in der Liste", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
 		"Toujours dans la liste", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"mar a zaroltak k�z�tt", // Magyar
		"", // Catala
		"��� � ������", // Russian
		"", // Croatian
	},
   {
		"Protected channels", // English
		"Gesperrte Kan�le", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Prot�ger cha�ne", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"V�dett csatorn�k", // Magyar
		"", // Catala
		"��������������� ������", // Russian
		"", // Croatian
	},
   {
		"Protected broadcasts", // English
		"Gesperrte Sendungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Proteger l'�mission", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"V�dett ad�sok", // Magyar
		"", // Catala
		"��������������� �������������", // Russian
		"", // Croatian
	},
   {
		"regular expression", // English
		"regul�rer Ausdruck", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Expression r�guli�re", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"regul�ris kifejez�s", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Keyword", // English
		"Suchbegriff", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Expression de recherche", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Kulcssz�", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"includes", // English
		"enth�lt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"incluent", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"tartalmazza", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"exact", // English
		"entspricht", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"exactement", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"megeggyezik", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"start with", // English
		"beginnt mit", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Title", // English
		"Titel", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Titre", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"C�m", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"always", // English
		"immer", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"toujours", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Mindig", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"intelligent", // English
		"intelligent", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"intelligent", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"inteligens", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"never", // English
		"nie", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"jamais", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"soha", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Autoprotection of recordings", // English
		"Automatischer Schutz der Aufnahmen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Protection automatique des enregistrement", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"Automatikus v�delem felv�tel ellen", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"is protected", // English
		"ist gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"est prot�g�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Open menu after pin request", // English
		"Men� nach Eingabe des Pin �ffnen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Ouvrir le menu apr�s la rentr� de la cl�ed", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Recording was already protected!", // English
		"Aufnahme bereits gesperrt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"L'enregeristrement est en protection parentale!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Recording now protected!", // English
		"Sperre aktiviert!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"L'enregeristrement est en protection parentale maintenant!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Recording was not protected!", // English
		"Aufnahme ist nicht gesperrt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"L'enregeristrement n'avait pas de protection parentale!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Removed protection of recording!", // English
		"Sperre entfernt!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"La protection parentale est enlever!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Please setup a pin code first!", // English
		"Bitte konfigurieren zuerst einen Pin Code!", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Entrer le code de protection d'abord!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Please enter pin code first", // English
		"Bitte zuerst Pin Code eingeben", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Entr� d'abord le code", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"locked", // English
		"gesperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"prot�g�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"unlocked", // English
		"entsperrt", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"lib�r�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"On/Off", // English
		"An/Aus", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Enclencher/d�clencher", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Invalid Code !", // English
		"Falsche Eingabe !", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Le code est faux!", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Code accepted", // English
		"Eingabe angenommen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Le code est accept�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Setup your pin code: ", // English
		"Bitte Pin Code festlegen: ", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Introduisez le code d'acc�s:", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Pin code successfully setup", // English
		"Pin Code erfolgreich festgelegt.", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Le code d'acc�s est accept�", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Menu protections", // English
		"Men� sperren", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Prot�gier des menus", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Hide protected menus", // English
		"Gesperrte Men�s verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Menu prot�g�s sont invisibles", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Hide protected plugins", // English
		"Gesperrte Plugins verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Plugins prot�g�s sont invisibles", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
		"Hide protected recordings", // English
		"Gesperrte Aufnahmen verstecken", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"Enregistrements prot�g�s sont invisibles", // Fran�ais
		"", // Norsk
		"", // Suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
		"", // Russian
		"", // Croatian
	},
   {
      NULL
   }
};

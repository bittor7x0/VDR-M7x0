/*
 * Fritz!Box plugin for VDR
 *
 * Copyright (C) 2007 Joachim Wilke <vdr@joachim-wilke.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

#ifndef FONBUCH_H_
#define FONBUCH_H_

#include <string>
#include <vdr/i18n.h>

/**
 * General telephonebook entry.
 * This definies the class, to be used by every telephonebook implementation.
 */

class cFonbuchEntry {
public:
	enum eType {
		TYPE_NONE,
		TYPE_HOME,
		TYPE_MOBILE,
		TYPE_WORK
	};
private:
	std::string name;
	std::string number;
	eType type;
public:
	cFonbuchEntry(std::string &name, std::string &number, eType type);
	std::string &getName() { return name; }
	std::string &getNumber() { return number; }
	eType getType() { return type; }
	std::string getTypeName();
	bool operator<(const cFonbuchEntry & fe) const;
};	

/**
 * General telephonebook base class.
 * All specific telephonebooks have to inherit from this class.
 */

class cFonbuch
{
	protected:
	/**
	 * The constructor may only be used by cFonbuchManager.
	 * Subclasses must make their constructor private, too.
	 */
	cFonbuch() {}
	/**
	 * The descriptive title of this phonebook.
	 */
	std::string title;
	/**
	 * The technical id of this phonebook (should be a short letter code).
	 */
	std::string techId;
	/**
	 * True, if this phonebook has displayable entries.
	 */
	bool displayable;
public:
	virtual ~cFonbuch();
	/**
	 * Take action to fill phonebook with content. 
	 * Initialize() may be called more than once per session.
	 * @return if initialization was successful
	 */
	virtual bool Initialize(void) { return true; }
	/**
	 * Resolves the given number to the corresponding name.
	 * @param number the number to resolve
	 * @return the resolved name or the number, if unsuccesful
	 */
	virtual std::string ResolveToName(std::string &number) { return number; }
	/**
	 * Returns a specific telephonebook entry.
	 * @param id unique identifier of the requested entry
	 * @return the entry with key id or NULL, if unsuccesful
	 */
	virtual cFonbuchEntry *RetrieveFonbuchEntry(size_t id) { return NULL; }
	/**
	 * Returns if it is possible to display the entries of this phonebook. 
	 * @return true, if this phonebook has displayable entries. "Reverse lookup only" phonebooks must return false here.
	 */
	virtual bool isDisplayable() { return displayable; }
	/**
	 *  Returns the number of entries in the telephonebook.
	 * @return the number of entries
	 */
	virtual size_t GetFonbuchSize() { return 0; }
	/**
	 *  Reloads the telephonebook's content
	 */
	virtual void Reload() { }
	/**
	 *  Returns a string that should be displayed as title in the menu when the telephonebook is displayed.
	 * @return the long title of this phonebook
	 */ 
	virtual std::string GetTitle() { return title; }
	/**
	 * Returns the technical id of this phonebook. This id has to be unique among all phonebooks and is used when storing
	 * the plugin's setup.
	 * @return the technical id
	 */
	virtual std::string &GetTechId() { return techId; }
};
#endif /*FONBUCH_H_*/

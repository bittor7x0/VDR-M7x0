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

#ifndef OERTLICHESFONBUCH_H_
#define OERTLICHESFONBUCH_H_

#include <string>
#include "fonbuch.h"

class cOertlichesFonbuch : public cFonbuch
{
	friend class cFactory;
	friend class cFonbuchManager;
private:
	cOertlichesFonbuch();
public:
	virtual ~cOertlichesFonbuch();
	/**
	 * Resolves the given number to the corresponding name.
	 * @param number the number to resolve
	 * @return the resolved name or the number, if unsuccesful
	 */
	std::string ResolveToName(std::string &number);
};

#endif /*OERTLICHESFONBUCH_H_*/

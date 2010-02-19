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

#ifndef LOCALFONBUCH_H_
#define LOCALFONBUCH_H_

#include "fonbuch.h"
#include <vector>

class cLocalFonbuch : public cFonbuch {
	friend class cFonbuchManager;	
private:
	cLocalFonbuch();
public:
	virtual ~cLocalFonbuch();
	bool Initialize();
	void Reload();	
};

#endif /*LOCALFONBUCH_H_*/

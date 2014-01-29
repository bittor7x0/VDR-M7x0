/*
    ee24lc16.h	A driver for 24LC16 Serial EEPROMs

    Version: $Id: ee24lc16.h,v 1.2 2004-02-22 14:51:35 ttittmann Exp $
    
    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _EE24LC16_H
#define _EE24LC16_H

#define I2C_EEPROM_LOW	0x50
#define I2C_EEPROM_HIGH	0x58

#ifndef I2C_DRIVERID_AT24Cxx
#define I2C_DRIVERID_AT24Cxx	I2C_DRIVERID_EXP3
#endif

#define GET_EEPROM_DATA	_IOR('e',1, struct eeprom_s)
#define SET_EEPROM_DATA _IOW('e',2, struct eeprom_s)

#endif


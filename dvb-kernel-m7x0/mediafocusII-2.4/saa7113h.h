/*
    saa7113h.h		A driver for Philips SAA7113H
			9-bit Video Input Processor 

    Version: $Id: saa7113h.h,v 1.2 2004-02-22 14:51:36 ttittmann Exp $

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

/*
 *  Derived from
 *
 *  saa711x.h - definitions for the generic i2c-driver for the
 *
 *   - saa7111a (Enhanced Video Input Processor) and
 *   - saa7112 (saa7111a + high performace scaler)
 *
 *  by Philips Semiconductors.
 *
 *  Copyright (C) 1998,1999 Michael Hunold <michael@mihu.de>
 */
 
#ifndef _SAA7113H_H
#define _SAA7113H_H

#define I2C_SAA7113H_1	0x24	/* I2C address (7-bit right-aligned)	*/
#define I2C_SAA7113H_2	0x25

#define SAA7113H_MAX_INPUTS	3
#define SAA7113H_MAX_OUTPUTS	0

#ifndef I2C_DRIVERID_SAA7113
/* select the EXP? id dependent on the current whole I2C bus */
#define I2C_DRIVERID_SAA7113	I2C_DRIVERID_EXP1
#endif

struct saa7113h {
	struct i2c_adapter *adapter;

	int brightness;
	int contrast;
	int saturation;
	int hue;
	int norm;
	int input;
};

#endif


/*
    sp5055II.h		A driver for Mitel SP5055 2.6 GHz Bidirectional
			I2C Bus Controlled Synthesizer

    Version: $Id: sp5055II.h,v 1.3 2004-02-22 14:51:36 ttittmann Exp $
			
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
 *  sp5055.h	MITEL 2.6GHz Bidirectional I2C BUS Controlled Synthesizer
 *		(tuner) driver
 *
 *  (c) 1999-2000 Peter Schlaf <peter.schlaf@org.chemie.uni-giessen.de>
 */

#ifndef _SP5055II_H
#define _SP5055II_H

#define I2C_SP5055_LOW	0x60
#define I2C_SP5055_HIGH	0x63

#ifndef I2C_DRIVERID_SP5055
#define I2C_DRIVERID_SP5055 I2C_DRIVERID_TUNER
#endif


struct sp5055 {
	unsigned long	divider;

	unsigned char	control0;	/* byte 4 of I2C telegram:				*/
#define SP5055_CTRL0_CP	0x40		/* bit 6: charge pump current select (CP)		*/
#define SP5055_CTRL0_T1	0x20		/* bit 5: test mode select (T1)				*/
#define SP5055_CTRL0_T0 0x10		/* bit 4: charge pump disable (T0)			*/
#define SP5055_CTRL0_OS 0x01		/* bit 0: varactor drive output disable switch (OS)	*/
					/* bits 7, 3, 2 and 1 must be set to 1	(0x8e)		*/

	unsigned char	control1;	/* byte 5 of I2C telegram:				*/
#define SP5055_CTRL1_P7	0x80		/* bits 7..3 + bit 0: port P7...P3,P0 output states	*/
#define SP5055_CTRL1_P6	0x40		/* (bits 1 and 2 ignored)				*/
#define SP5055_CTRL1_P5	0x20
#define SP5055_CTRL1_P4	0x10
#define SP5055_CTRL1_P3	0x08
#define SP5055_CTRL1_P0	0x01
	
};

#endif


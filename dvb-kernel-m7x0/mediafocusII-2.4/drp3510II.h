/*
    drp3510II.h		A driver for Micronas Intermetall DRP3510A
			Digital Radio Processor

    Version: $Id: drp3510II.h,v 1.2 2004-02-22 14:51:35 ttittmann Exp $

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
 *  drp3510.h	MICRONAS INTERMETALL DRP3510 digital radio processor driver
 *
 *  (c) 1999-2000 Peter Schlaf <peter.schlaf@org.chemie.uni-giessen.de>
 */

#ifndef _DRP3510II_H
#define _DRP3510II_H

#define I2C_DRP3510		0x6a	/* i2c device address, 7 Bit LSB bounded */

#ifndef I2C_DRIVERID_DRP3510
#define I2C_DRIVERID_DRP3510	I2C_DRIVERID_EXP2
#endif

struct drp3510 {
	struct i2c_adapter	*adapter;
	__u8			adrdata[20];
};

#define DRP_ADR		_IO('d', 1)		/* init drp adr mode */
#define DRP_PIO_DMA	_IO('d', 2)		/* init drp pio-dma */
#define DRP_STATUS	_IOR('d', 3, __u16)	/* read drp status + index */
#define DRP_ADR_DATA	_IOR('d', 4, __u8 *)	/* read adr data field (return field address) */

#endif


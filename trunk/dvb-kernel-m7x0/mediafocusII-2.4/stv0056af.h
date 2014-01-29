/*
    stv0056af.h		A driver for SGS-Thomson STV0056AF
			Satellite Sound and Video Processor

    Version: $Id: stv0056af.h,v 1.3 2004-02-22 14:51:36 ttittmann Exp $
			
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
 *  stv0030.h	I2C-controlled Satellite Sound and Video Processor
 *		from SGS Thompson
 *
 *  (c) 1999-2000 Peter Schlaf <peter.schlaf@org.chemie.uni-giessen.de>
 */

#ifndef _STV0056AF_H
#define _STV0056AF_H

#define I2C_STV0056AF_1		0x03	/* I2C addresses:	*/
#define I2C_STV0056AF_2		0x23	/* 0x03 and 0x23 (7-bit) are available	*/

#ifndef I2C_DRIVERID_STV0056
#define	I2C_DRIVERID_STV0056	I2C_DRIVERID_EXP0
#endif

struct stv0056af {
	struct		i2c_adapter *adapter;

	unsigned char	video_gain;
#define STV_GET_GAIN	_IOR('s', 1, int)
#define STV_SET_GAIN	_IOW('s', 2, int)
	int		source_scart1;
#define	STV_GET_SCART1	_IOR('s', 3, int)
#define STV_SET_SCART1	_IOW('s', 4, int)
	int		source_scart2;
#define STV_GET_SCART2	_IOR('s', 5, int)
#define STV_SET_SCART2	_IOW('s', 6, int)
	int		source_decoder;
#define	STV_GET_DECODER	_IOR('s', 7, int)
#define	STV_SET_DECODER	_IOW('s', 8, int)
	
#define	STV_BASEBAND		0
#define	STV_DEEMPHASIZED	1
#define	STV_NORMAL_VIDEO	2
#define	STV_DECODER_RET		3
#define	STV_SCART2_RET		4
#define	STV_SCART1_RET		5
#define	STV_NOTHING		6
#define	STV_HIGHZ		7

	int		freq1;		/* audio carrier left channel	*/
	int		freq2;		/* audio carrier right channel	*/

	int		audio_mode;
#define	STV_GET_STEREO	_IOR('s', 9, int)
#define STV_SET_STEREO	_IOW('s',10, int)
	uint		volume;
#define	STV_SWITCH_MUTE	_IOW('s',11, int)
#define STV_GET_VOLUME	_IOR('s',12, int)
#define STV_SET_VOLUME	_IOW('s',13, int)
	
	int		sig22khz;
#define	STV_GET_22KHZ	_IOR('s',14, int)
#define	STV_SET_22KHZ	_IOW('s',15, int)
	
#define	STV_22KHZ_OFF	0
#define	STV_22KHZ_ON	1

	unsigned char	writeregs[6];	/* reg. 0x00..0x05 can't be read back	*/
};

#endif


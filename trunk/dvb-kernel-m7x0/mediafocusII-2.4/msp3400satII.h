/*
    msp3400satII.h	A driver for Micronas Intermetall MSP3400C
			Multistandard Sound Processor driver

    Version: $Id: msp3400satII.h,v 1.3 2004-02-22 14:51:35 ttittmann Exp $

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
 *  msp3400sat.h	MICRONAS INTERMETALL MSP3400 audio driver
 *
 *  (c) 1999-2000 Peter Schlaf, <peter.schlaf@org.chemie.uni-giessen.de>
 */

#ifndef _MSP3400SATII_H
#define _MSP3400SATII_H

/* Possible I2C device addresses, 7-bit LSB bound */
#define I2C_MSP3400C_1		0x40
#define I2C_MSP3400C_2		0x42
#define I2C_MSP3400C_3		0x44
/* Sub addresses */
#define I2C_MSP3400C_CONTROL	0x00
#define I2C_MSP3400C_DEM	0x10
#define I2C_MSP3400C_DFP	0x12
#define I2C_MSP3400C_AGC	0x1e
#define I2C_MSP3400C_PLLCAP	0x1f

struct msp3400c {
	struct i2c_adapter     *adapter;

	int mode, loudness;
	int volume, balance;
	int bass, treble;
	int left_freq, right_freq, adr;	/* this line was created for adr reception (carrier-frequence in kHz) */
	
};

/* ---------------------------------------------------------------------- */

#define MSP_SET_TVNORM		_IOW('m', 1,int)  /* TV mode + PAL/SECAM/NTSC  */
#define MSP_SET_RADIO		_IO('m',  2)	   /* Radio mode		   */
#define MSP_NEWCHANNEL		_IO('m',  3)	   /* indicate new channel	   */

#define MSP_GET_VOLUME		_IOR('m', 4,__u16)
#define MSP_SET_VOLUME		_IOW('m', 5,__u16)

#define MSP_GET_BALANCE		_IOR('m',31, __u16)
#define MSP_SET_BALANCE		_IOW('m',32, __u16)

#define MSP_GET_LOUDNESS	_IOR('m',33, __u16)
#define MSP_SET_LOUDNESS	_IOW('m',34, __u16)

#define MSP_GET_STEREO		_IOR('m', 6,__u16)
#define MSP_SET_STEREO		_IOW('m', 7,__u16)

#define MSP_GET_DC		_IOW('m', 8,__u16)

#define MSP_GET_BASS		_IOR('m', 9,__u16)
#define MSP_SET_BASS		_IOW('m',10,__u16)
#define MSP_GET_TREBLE		_IOR('m',11,__u16)
#define MSP_SET_TREBLE		_IOW('m',12,__u16)

#define MSP_GET_UNIT		_IOR('m',13,int)
#define MSP_SWITCH_MUTE		_IO('m',14)

#define MSP_GET_LEFT_FREQ	_IOR('m', 15, __u16)
#define MSP_SET_LEFT_FREQ	_IOW('m', 16, __u16)

#define MSP_GET_RIGHT_FREQ	_IOR('m', 17, __u16)
#define MSP_SET_RIGHT_FREQ	_IOW('m', 18, __u16)

#define MSP_ADR_ON		_IO('m', 19)	/* ADR mode on */
#define MSP_ADR_OFF		_IO('m', 20)	/* ADR mode off */

/* select source of demodulator */
#define MSP_SLCT_DEM_SRC	_IOW('m', 21, __u16)
#define MSP_ANA1_TO_DEM		0
#define MSP_ANA2_TO_DEM		1

/* select source of internal DFP analog ("SCART") input */
#define MSP_SLCT_DFPANALOG_SRC	_IOW('m', 22, __u16)
#define MSP_SC1_TO_DFP		0
#define MSP_MONO_TO_DFP		1
#define MSP_SC2_TO_DFP		2
#define MSP_SC3_TO_DFP		3

/* select source of internal DFP analog ("SCART") output */
#define MSP_SLCT_SCART_SRC	_IOW('m', 23, __u16)
/* select source of loudspeaker output */
#define MSP_SLCT_LSP_SRC	_IOW('m', 24, __u16)
#define MSP_DEM_SRC		0
#define MSP_NONE_SRC		1
#define MSP_SCART_SRC		2
#define MSP_SBUS12_SRC		3
#define MSP_SBUS34_SRC		4
#define MSP_I2S1_SRC		5
#define MSP_I2S2_SRC		6

/* select source of MSP3400C SCART1 output */
#define MSP_SLCT_SC1_SRC	_IOW('m', 25, __u16)
#define MSP_SC3_TO_SC1		0
#define	MSP_SC2_TO_SC1		1
#define MSP_MONO_TO_SC1		2
#define MSP_DFP_TO_SC1		3

/* select source of MSP3400C SCART2 output */
#define MSP_SLCT_SC2_SRC	_IOW('m', 26, __u16)
#define MSP_DFP_TO_SC2		0
#define MSP_SC1_TO_SC2		1
#define MSP_MONO_TO_SC2		2

/* set logic state of D_CTR_OUT0 output */
#define MSP_GET_DIGCTR0		_IOR('m', 27, __u16)
#define MSP_SET_DIGCTR0		_IOW('m', 28, __u16)
#define MSP_DIGCTR0_OFF		0
#define MSP_DIGCTR0_ON		1

/* set logic state of D_CTR_OUT1 output */
#define MSP_GET_DIGCTR1		_IOR('m', 29, __u16)
#define MSP_SET_DIGCTR1		_IOW('m', 30, __u16)
#define	MSP_DIGCTR1_OFF		0
#define MSP_DIGCTR1_ON		1

#endif


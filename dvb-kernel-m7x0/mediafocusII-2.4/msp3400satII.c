/*
    msp3400satII.c	A driver for Micronas Intermetall MSP3400C
			Multistandard Sound Processor

    Version: $Id: msp3400satII.c,v 1.4 2004-04-17 15:32:13 ttittmann Exp $

    Copyright (C) 2003 Torsten Tittmann <Torsten.Tittmann@gmx.de>

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
    Derived from
  
    msp3400satII.c	Micronas Intermetall MSP3400C I2C driver

    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>
 */

#define DEBUG_VARIABLE msp3400satII_debug
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>

#if CONFIG_MODVERSIONS==1
#define MODVERSIONS
#include <linux/modversions.h>
#endif

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/videodev2.h>

#include <media/saa7146.h>
#include "videodev_priv.h"

#include "msp3400satII.h"

static int msp3400satII_debug = 0;	/* module load parameter */
#ifdef MODULE
MODULE_PARM(msp3400satII_debug, "i");
MODULE_PARM_DESC(msp3400satII_debug, "debug level (default: 0)");
#endif

/* I2C addresses to scan */
static unsigned short int normal_i2c[] = { I2C_MSP3400C_1, I2C_MSP3400C_2, I2C_MSP3400C_3, I2C_CLIENT_END };
static unsigned short int normal_i2c_range[] = { I2C_CLIENT_END };

/* magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/* unique ID allocation */
static int msp3400c_id = 0;

static struct i2c_driver msp_driver;

/*****************************************************************************/


/* write to a msp3400c register with long I2C format */
static int msp3400c_write_long (struct i2c_client *msp_client, __u8 subaddr, __u16 reg, __u16 val)
{
	__u8 buf[5];
	int count = sizeof (buf);

	buf[0] = subaddr;
	buf[1] = (reg & 0xff00) >> 8;	/* hi-byte first */
	buf[2] = reg & 0xff;
	buf[3] = (val & 0xff00) >> 8;	/* hi-byte first */
	buf[4] = val & 0xff;

	DEB_D(("I2C client %s; subaddr=0x%02x; reg=0x%04x; val=0x%04x.\n", msp_client->name, subaddr, reg, val));

	if ( count != i2c_master_send (msp_client, buf, count) ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}


/* write to a msp3400c register with short I2C format */
/* -> realized by a direct call of i2c_smbus_write_word_data */


/* read from a msp3400c register with long I2C format */
static int msp3400c_read_long (struct i2c_client *msp_client, __u8 subaddr, __u16 reg)
{
	int ret;
	__u16 val = 0;
	__u8 send_buf[3] = { subaddr + 1, (reg & 0xff00) >> 8, reg & 0xff };
	__u8 recv_buf[2];
	struct i2c_msg transfer_msgs[] = {
		{	msp_client->addr,
			msp_client->flags,
			sizeof (send_buf),
			send_buf },
		{	msp_client->addr,
			msp_client->flags | I2C_M_RD,
			sizeof (recv_buf),
			recv_buf }
	};
	int num_of_msgs = sizeof (transfer_msgs) / sizeof (transfer_msgs[0]);

	DEB_D(("I2C client %s; subaddr=0x%02x; reg=0x%04x; val=0x%04x.\n", msp_client->name, subaddr, reg, val));
	
	ret = i2c_transfer (msp_client->adapter, transfer_msgs, num_of_msgs);
	
	DEB_D(("I2C client %s; num_of_msgs=%d; ret=%d; rbuf[0]=0x%02x; rbuf[1]=0x%02x.\n", msp_client->name, num_of_msgs, ret, recv_buf[0], recv_buf[1]));

	if (ret != num_of_msgs) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}

	val = ((__u16) recv_buf[0] << 8) | recv_buf[1];

	return val;
}


/* read from a msp3400c register with short I2C format */
/* -> realized by a direct call of i2c_smbus_read_word_data */

/*****************************************************************************/

static int msp3400c_setregister (struct i2c_client *msp_client, __u8 subaddr, __u16 reg, __u16 mask, __u16 set)
{
	__u16 val;
	int ret = 0;
	
	DEB_D(("I2C client %s; subaddr=0x%02x; reg=0x%04x; mask=0x%04x; set=0x%04x.\n", msp_client->name, subaddr, reg, mask, set));
	
	if (!(mask & 0x0001)) {
		val = mask;
		do {
			set <<= 1;
			val >>= 1;
		} while (!(val & 0x0001));
	}
	
	val = msp3400c_read_long (msp_client, subaddr, reg);
	
	val &= ~mask;
	val |= set & mask;
	
	ret = msp3400c_write_long (msp_client, subaddr, reg, val);
	
	return ret;
}

#if 0	/*can't read out demodulator register */
/* select demodulator source: ANA_IN1+ or ANA_IN2+ */
static int
msp3400c_select_demod_src (struct i2c_client *msp_client, __u16 select)
{
	
dprintk (KERN_DEB_S "msp3400satII: ==> msp3400c_select_demod_src (sel=0x%04x)\n", select);

	return msp3400c_setregister (msp_client, I2C_MSP3400C_DEM, 0x00bb, 0x0100, select);
}			 
#endif

/* select source of analog DFP input */
static int msp3400c_select_dfpanalog_src (struct i2c_client *msp_client, __u16 select)
{

	DEB_S(("I2C client %s; sel=0x%04x.\n", msp_client->name, select));

	return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0013, 0x0300, select);
}


/* select source of loudspeaker output */
static int msp3400c_select_lsp_src (struct i2c_client *msp_client, __u16 select)
{

	DEB_S(("I2C client %s; sel=0x%04x.\n", msp_client->name, select));

	return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0008, 0xff00, select);
}


/* select source of DFP "SCART" output */
static int msp3400c_select_scart_src (struct i2c_client *msp_client, __u16 select)
{

	DEB_S(("I2C client %s; sel=0x%04x.\n", msp_client->name, select));

	return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x000a, 0xff00, select);
}


/* select source of MSP3400C SCART1 output */
static int msp3400c_select_sc1_src (struct i2c_client *msp_client, __u16 select)
{

	DEB_S(("I2C client %s; mask=0x%04x.\n", msp_client->name, select));

	return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0013, 0x0c00, select);
}


/* select source of MSP3400C SCART2 output */
static int msp3400c_select_sc2_src (struct i2c_client *msp_client, __u16 select)
{

	DEB_S(("I2C client %s; mask=0x%04x.\n", msp_client->name, select));

	return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0013, 0x3000, select);
}


/* set audio mute */
static int msp3400c_setmute (struct i2c_client *msp_client)
{
	int ret = 0;
	int val = 0xffe << 4;					/* use 'fast mute' mode */

	DEB_S(("I2C client '%s': enable fast mute mode.\n", msp_client->name));
	
	if ( 0 != (ret = msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x0000, val)) ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}

/* set volume level */
static int msp3400c_setvolume (struct i2c_client *msp_client, int volume)
{
	int ret = 0;
	int val = ( (volume * 0x73 / 65535) << 8 ) + 1;		/* max: 0x73 == 0 dB */
								/* +1 == reduce tone control clipping mode */

	DEB_S(("I2C client '%s': volume=%d; val=0x%x.\n", msp_client->name, volume, val));
	
	if ( 0 != (ret = msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x0000, val)) ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}

/* set loudness */
/* calculated reciprocal (!) from volume level with offset.
 * If loudness is enabled, volumes below the threshold get max. loudness effect,
 * over the treshold the loudness effect is reduced with each volume step in dependece of
 * the given relation factor.
 */
static int msp3400c_setloudness(struct i2c_client *msp_client, int arg)
{
	int ret = 0;
	int relation = 1;		/* 1 volume steps (0.124 dB) =  1 loudness steps (0.25 dB) */
	int val = (arg * 0x73 / 65535);
	
	if ( (0x73 - 0x44/relation) >= val )			/* treshold of max. loudness level */
		val = (0x44 << 8) + 0;				/* +0 == normal mode (const. 1kHz volume) */
	else
		val = ( (relation*(0x73-val)) << 8 ) + 0;
	
	DEB_S(("I2C client '%s': arg=%d; val=0x%x.\n", msp_client->name, arg, val));
	
	if ( 0 != (ret = msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x0004, val)) ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}

/* set balance */
static int msp3400c_setbalance (struct i2c_client *msp_client, int balance)
{
	int ret = 0;
	int val = ( ((balance - 32768) * 0x7f * 2 / 65535) << 8 ) + 0;	/* 0x7f*2 = control range */
									/* +0 = set linear mode */

	DEB_S(("I2c client %s; balance=%d; val=0x%x.\n", msp_client->name, balance, val));

	if ( 0 != (ret=msp3400c_write_long(msp_client, I2C_MSP3400C_DFP, 0x0001, val)) ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}

/* set bass level */
static int msp3400c_setbass (struct i2c_client *msp_client, int bass)
{
	int ret = 0;
	int val = ((bass - 32768) * 0x60 * 2 / 65535) << 8;	/* 0x60*2 = control range +/- 12 dB */

	DEB_S(("I2c client %s; bass=%d; val=0x%x.\n", msp_client->name, bass, val ));

	if ( 0 != (ret=msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x0002, val)) ) {	
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}

	return 0;
}


/* set treble level */
static int msp3400c_settreble (struct i2c_client *msp_client, int treble)
{
	int ret = 0;
	int val = ((treble - 32768) * 0x60 * 2 / 65535) << 8;	/* 0x60*2 = control range +/- 12 dB */

	DEB_S(("I2c client %s; treble=%d; val=0x%x.\n", msp_client->name, treble, val));

	if ( 0 != (ret=msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x0003, val )) ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}


/* set audio carrier frequencies */
static int msp3400c_setfrequency (struct i2c_client *msp_client, int left_freq, int right_freq)
{
	int ret = 0;

	/* frequencies are delivered in kHz */
	int dco_left  = (left_freq << 13) / 9;		/* ((freq << 13) / 9) same as (freq * 2^24/18432) */
	int dco_right = (right_freq << 13) / 9;

	DEB_S(("I2C client %s; left=%d; dco_left=0x%06x; right=%d; dco_right=0x%06x.\n", msp_client->name, left_freq, dco_left, right_freq, dco_right));

	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0093, dco_right & 0x0FFF);		/* increment channel 1 LOW  */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x009B, (dco_right >> 12) & 0x0FFF);	/* increment channel 1 HIGH */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x00A3, dco_left & 0x0FFF);		/* increment channel 2 LOW  */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x00AB, (dco_left >> 12) & 0x0FFF);	/* increment channel 2 HIGH */
#if 0
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0056, 0x0000);			/* load_reg 1/2 */	
#endif
	if ( 0 != ret ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}


/* set mono/stereo/ADR mode */
static int msp3400c_setmode (struct i2c_client *msp_client, int mode)
{
	struct msp3400c *msp = (struct msp3400c *) msp_client->data;
	int choice, ret = 0;

	const __u16 modes[7][14] = {
	/*   0	   1	2    3	  4    5    6	   7	  8	9     10	11	12     13	*/
	/*							  FM prescale	     Channel-matrix	*/
	/* AD_CV FIR5 FIR4 FIR3 FIR2 FIR1 FIR0 MODE_REG DCO L DCO R +matrix  FM deemp  mask  value	*/
	{ 0x00c6, 127, 101, 119, 64,  53,  73,	0x3c04, 6500, 6500, 0x1000,   0x013f, 0x00ff,0x0000 },
	   /* 6.50MHz, MSP-Ch2 (FM1) only, Mono-L, 75us+WP1, 130kHz */

	{ 0x00c6, 127, 101, 119, 64,  53,  73,	0x3c84, 7020, 7200, 0x1000,   0x013f, 0x00ff,0x0020 },
	   /* 7.02+7.20MHz, Stereo, 75us+WP1, 130kHz */

	{ 0x00c6, 127, 101, 119, 64,  53,  73,	0x3c84, 7020, 7200, 0x1000,   0x013f, 0x00ff,0x0000 },
	   /* 7.02+7.20MHz, Mono-L, 75us+WP1, 130kHz */

	{ 0x00c6, 127, 101, 119, 64,  53,  73,	0x3c84, 7020, 7200, 0x1000,   0x013f, 0x00ff,0x0010 },
	   /* 7.02+7.20MHz, Mono-R, 75us+WP1, 130kHz */

	{ 0x00a8, 127, 112,  84, 52,  23,   7,	0x6c88, 6120, 6120, 0x0000,   0x0000, 0x00ff,0x0020 },
	   /* ADR 6.12MHz, Stereo */

	{ 0x00a8, 127, 112,  84, 52,  23,   7,	0x6c88, 6120, 6120, 0x0000,   0x0000, 0x00ff,0x0000 },
	   /* ADR 6.12MHz, Mono-L */

	{ 0x00a8, 127, 112,  84, 52,  23,   7,	0x6c88, 6120, 6120, 0x0000,   0x0000, 0x00ff,0x0010 }
	   /* ADR 6.12MHz, Mono-R */
	};
	
	switch (mode) {
		case V4L2_TUNER_MODE_MONO:	 choice = 0; break;	/* 6.50 MHz Mono	*/
		case V4L2_TUNER_MODE_STEREO: choice = 1; break;	/* 7.02/7.20 MHz Stereo	*/
		case V4L2_TUNER_MODE_LANG1:  choice = 2; break;	/* 7.02 MHz Mono	*/
		case V4L2_TUNER_MODE_LANG2:  choice = 3; break;	/* 7.20 MHz Mono	*/
#if 0
		  case VIDEO_SOUND_ADR:	 choice = 4; break;	/* ADR Stereo */
		  case VIDEO_SOUND_ADR_L:	 choice = 5; break;	/* ADR left channel as Mono */
		  case VIDEO_SOUND_ADR_R:	 choice = 6; break;	/* ADR right channel as Mono */
#endif
		default:
			return -EINVAL;
	}
	
	DEB_S(("I2C client %s; mode=%d.\n", msp_client->name, mode));

	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x00bb, modes[choice][0]);	/* AD_CV reg.	*/
	
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, 0x0004);		/* imreg1	*/
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, 0x0040);		/* imreg1/2	*/
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, 0x0000);		/* imreg2	*/
	
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, modes[choice][1]);	/* fir coeff. 5	*/
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, modes[choice][2]);	/* fir coeff. 4 */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, modes[choice][3]);	/* fir coeff. 3 */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, modes[choice][4]);	/* fir coeff. 2 */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, modes[choice][5]);	/* fir coeff. 1 */
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0005, modes[choice][6]);	/* fir coeff. 0 */
	
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DEM, 0x0083, modes[choice][7]);	/* MODE_REG reg. */

	switch (mode) {
	case V4L2_TUNER_MODE_MONO:
	case V4L2_TUNER_MODE_STEREO:
	case V4L2_TUNER_MODE_LANG1:
	case V4L2_TUNER_MODE_LANG2:
		msp->left_freq = modes[choice][8];
		msp->right_freq = modes[choice][9];
		ret |= msp3400c_setfrequency (msp_client, msp->left_freq, msp->right_freq);	/* DCO freqs	*/
		break;
	default: {
		/* ADR modes keep old carrier freqs */
		break;
		}
	}
	
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x000E, modes[choice][10]);	/* FM prescale, FM matrix	*/
	ret |= msp3400c_write_long (msp_client, I2C_MSP3400C_DFP, 0x000F, modes[choice][11]);	/* FM deemphasis		*/

	/* loudspeaker channel matrix */
	ret |= msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0008, modes[choice][12], modes[choice][13]);
	/* scart channel matrix */
	ret |= msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x000a, modes[choice][12], modes[choice][13]);
	
	if ( 0 != ret ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}
	
	return 0;
}


static int msp3400c_chip_init (struct i2c_client *msp_client)
{
	struct msp3400c *msp = (struct msp3400c *) msp_client->data;
	int i, ret = 0;

	const int init_regs[][3] = {
	/*  sub address    register  value				*/
	
	{ I2C_MSP3400C_DFP, 0x0007, 0x4000 },	/* scart output volume (0 dB gain) */
	{ I2C_MSP3400C_DFP, 0x000D, 0x1900 },	/* scart input prescale (0 dB gain) */
	};
	
	DEB_EE(("I2C client %s.\n", msp_client->name));

	/* MSP3400C software reset */
	/* set bit 15 of CONTROL register */
	ret |= i2c_smbus_write_word_data (msp_client, I2C_MSP3400C_CONTROL, 0x8000);
	/* FIXME!! Need delay? */
	/* mdelay(2); */
	/* clear bit 15 of CONTROL register */
	ret |= i2c_smbus_write_word_data (msp_client, I2C_MSP3400C_CONTROL, 0x0000);
	/* FIXME!! Need delay? */
	/* mdelay(2); */
	
	if ( 0 != ret ) {
		DEB_S(("I2C client %s; software reset failed.\n", msp_client->name));
		return -EIO;
	}

	for (i = 0; i < (sizeof(init_regs)/sizeof(init_regs[0])); i++)
		ret |= msp3400c_write_long (msp_client, init_regs[i][0], init_regs[i][1], init_regs[i][2]);

	msp->mode = V4L2_TUNER_MODE_STEREO;
	ret |= msp3400c_setmode (msp_client, msp->mode);
	
	msp->adr = 0;

	msp->volume = 0;
	ret |= msp3400c_setvolume (msp_client, msp->volume);

	msp->balance=32768;
	ret |= msp3400c_setbalance (msp_client, msp->balance);
	
	msp->bass = 32768;
	ret |= msp3400c_setbass (msp_client, msp->bass);

	msp->treble = 32768;
	ret |= msp3400c_settreble (msp_client, msp->treble);


	if ( 0 != ret ) {
		DEB_S(("I2C client %s failed.\n", msp_client->name));
		return -EIO;
	}

	return 0;
}

/*****************************************************************************/

/* this function is called by i2c_probe */
static int msp3400c_detect (struct i2c_adapter *adap, int addr, unsigned short int flags, int kind)
{
	struct i2c_client *msp_client;
	struct msp3400c *msp;
	int ret;

	DEB_EE(("I2C adapter %s; addr=0x%02x, flags=0x%x, kind=0x%02x.\n", adap->name, addr, flags, kind));

	/* let's see whether this adapter can support what we need */
	if ( 0 == i2c_check_functionality (adap, I2C_FUNC_I2C) )
		return 0;

	/* allocate memory for client structure */
	if ( NULL == (msp_client = kmalloc (sizeof (struct i2c_client), GFP_KERNEL)) ) 
		return -ENOMEM;

	/* fill client structure */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	sprintf (msp_client->name, "Micronas MSP3400C");
#else
	sprintf (msp_client->dev.name, "Micronas MSP3400C");
#endif
	msp_client->id = msp3400c_id++;
	msp_client->flags = 0;
	msp_client->addr = addr;
	msp_client->adapter = adap;
	msp_client->driver = &msp_driver;
	msp_client->data = NULL;


	if ( 0 == msp3400c_id ) {
		DEB_S(("I2C client %s; No hardware detected.\n", msp_client->name));
	} else {
		DEB_S(("I2C client %s detected at i2c address 0x%02x.\n", msp_client->name, msp_client->addr));
	}

	/* allocate memory for msp structure */
	if ( NULL == (msp_client->data = msp = kmalloc (sizeof (struct msp3400c), GFP_KERNEL)) ) {
		kfree (msp_client);
		return -ENOMEM;
	}

	/* tell the i2c layer a new client has arrived */
	if ( 0 != (ret=i2c_attach_client (msp_client)) ) {
		DEB_S(("I2C client registration failed, %s @ 0x%02x not attached.\n", msp_client->name, msp_client->addr));
		kfree (msp_client->data);
		kfree (msp_client);
		return ret;
	}

	DEB_S(("I2C client %s @ 0x%02x attached to adapter %s.\n", msp_client->name, msp_client->addr, msp_client->adapter->name));

	/* MSP3400C chip init */
	if ( 0 != (ret=msp3400c_chip_init (msp_client)) ) {
		DEB_S(("Init of I2C client %s @ 0x%02x failed, continuing anyway.\n", msp_client->name, msp_client->addr));
		return ret;
	}

	DEB_S(("I2C client %s; hardware/revision code  : 0x%04x.\n", msp_client->name, msp3400c_read_long (msp_client, I2C_MSP3400C_DFP, 0x001e) ));
	DEB_S(("I2C client %s; product+rom version code: 0x%04x.\n", msp_client->name, msp3400c_read_long (msp_client, I2C_MSP3400C_DFP, 0x001f) ));

	return 0;
}


static int msp3400c_attach (struct i2c_adapter *adap)
{

	DEB_EE(("I2C adapter %s.\n", adap->name));

	return i2c_probe (adap, &addr_data, &msp3400c_detect);
}


static int msp3400c_detach (struct i2c_client *msp_client)
{
	int ret;

	DEB_EE(("I2C client %s; addr=0x%02x; adap=%s.\n", msp_client->name, msp_client->addr, msp_client->adapter->name));

#if 0
	ret = msp3400c_reset (msp_client);
#endif
	if ( 0 != (ret=i2c_detach_client (msp_client)) ) {
		DEB_S(("I2C client deregistration failed, %s @ 0x%02x not detached.\n", msp_client->name, msp_client->addr));
		return ret;
	}

	DEB_S(("I2C client %s @ 0x%02x detached from adapter %s.\n", msp_client->name, msp_client->addr, msp_client->adapter->name));

	kfree (msp_client->data);
	kfree (msp_client);

	return 0;
}


static int msp3400c_command (struct i2c_client *msp_client, unsigned int cmd, void *arg)
{
	struct msp3400c *msp = (struct msp3400c *) msp_client->data;
	__u16		*sarg = arg;
	int ret = 0;
	
	switch (cmd) {
		case MSP_SWITCH_MUTE:

			DEB_EE(("I2C client %s; MSP_SWITCH_MUTE\n", msp_client->name));

			return msp3400c_setmute (msp_client);

		case MSP_GET_VOLUME:
			*sarg = msp->volume;

			DEB_EE(("I2C client %s; MSP_GET_VOLUME (volume=%d)\n", msp_client->name, *sarg));

			return ret;
			
		case MSP_SET_VOLUME:
			msp->volume = *sarg;
			
			DEB_EE(("I2C client %s; MSP_SET_VOLUME (volume=%d)\n", msp_client->name, *sarg));
			
			return msp3400c_setvolume (msp_client, msp->volume);
		
		case MSP_GET_LOUDNESS:
			*sarg = msp->loudness;

			DEB_EE(("I2C client %s; MSP_GET_LOUDNESS (arg=%d)\n", msp_client->name, *sarg));
				
			return ret;
			
		case MSP_SET_LOUDNESS:
			msp->loudness = *sarg;
			
			DEB_EE(("I2C client %s; MSP_SET_LOUDNESS (arg=%d)\n", msp_client->name, *sarg));
			
			return msp3400c_setloudness (msp_client, msp->loudness);

		case MSP_GET_BALANCE:
			*sarg = msp->balance;

			DEB_EE(("I2C client %s; MSP_GET_BALANCE (balance=%d)\n", msp_client->name, *sarg));
			
			return ret;

		case MSP_SET_BALANCE:
			msp->balance = *sarg;

			DEB_EE(("I2C client %s; MSP_SET_BALANCE (balalance=%d)\n", msp_client->name, *sarg));

			return msp3400c_setbalance (msp_client, msp->balance);
			
		case MSP_GET_BASS:
			*sarg = msp->bass;

			DEB_EE(("I2C client %s; MSP_GET_BASS (bass=%d)\n", msp_client->name, *sarg));

			return ret;
			
		case MSP_SET_BASS:
			msp->bass = *sarg;

			DEB_EE(("I2C client %s; MSP_SET_BASS (bass=%d)\n", msp_client->name, *sarg));

			return msp3400c_setbass (msp_client, msp->bass);

		case MSP_GET_TREBLE:
			*sarg = msp->treble;

			DEB_EE(("I2C client %s; MSP_GET_TREBLE (treble=%d)\n", msp_client->name, *sarg));

			return ret;
			
		case MSP_SET_TREBLE:
			msp->treble = *sarg;

			DEB_EE(("I2C client %s; MSP_SET_TREBLE (treble=%d)\n", msp_client->name, *sarg));

			return msp3400c_settreble (msp_client, msp->treble);

		case MSP_GET_STEREO:
			*sarg = (V4L2_TUNER_MODE_MONO | V4L2_TUNER_MODE_STEREO | V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2);

			DEB_EE(("I2C client %s; MSP_GET_STEREO (mode=0x%02x)\n", msp_client->name, *sarg));

			return ret;

		case MSP_SET_STEREO:

			DEB_EE(("I2C client %s; MSP_SET_STEREO (mode=0x%02x)\n", msp_client->name, *sarg));

			switch (*sarg) {
				case V4L2_TUNER_MODE_MONO:
				case V4L2_TUNER_MODE_STEREO:
				case V4L2_TUNER_MODE_LANG1:
				case V4L2_TUNER_MODE_LANG2:
					if (msp->adr) {	/* switch back from ADR to normal */
						/* lsp and scart source: demodulator */
						ret |= msp3400c_select_lsp_src (msp_client, MSP_DEM_SRC);
						ret |= msp3400c_select_scart_src (msp_client, MSP_DEM_SRC);
						msp->adr = 0;
					}
					break;
#if 0
				  case VIDEO_SOUND_ADR:
				  case VIDEO_SOUND_ADR_L:
				  case VIDEO_SOUND_ADR_R:
					if (!msp->adr) {	/* switch from normal to ADR */
						/* lsp and scart source: I2S_1 */
						ret |= msp3400c_select_lsp_src (msp_client, MSP_I2S1_SRC);
						  ret |= msp3400c_select_scart_src (msp_client, MSP_I2S1_SRC);
						  msp->adr = 1;
					  }
					  break;
#endif
				default:
					return -EINVAL;
			}

			if ( 0 != (ret |= msp3400c_setmode (msp_client, *sarg)) ) 
				return -EIO;
			
			msp->mode = *sarg;
						
			return 0;

		case MSP_GET_LEFT_FREQ:
			*sarg = msp->left_freq;

			DEB_EE(("I2C client %s; MSP_GET_LEFT_FREQ (freq=%d)\n", msp_client->name, *sarg));

			return ret;
			
		case MSP_SET_LEFT_FREQ:
			msp->left_freq = *sarg;

			DEB_EE(("I2C client %s; MSP_SET_LEFT_FREQ (freq=%d)\n", msp_client->name, *sarg));

			return msp3400c_setfrequency (msp_client, msp->left_freq, msp->right_freq);

		case MSP_GET_RIGHT_FREQ:
			*sarg = msp->right_freq;

			DEB_EE(("I2C client %s; MSP_GET_RIGHT_FREQ (freq=%d)\n", msp_client->name, *sarg));

			return ret;
			
		case MSP_SET_RIGHT_FREQ:
			msp->right_freq = *sarg;

			DEB_EE(("I2C client %s; MSP_SET_RIGHT_FREQ (freq=%d)\n", msp_client->name, *sarg));

			return msp3400c_setfrequency (msp_client, msp->left_freq, msp->right_freq);
#if 0
		/* select demodulator source: ANA_IN1+ or ANA_IN2+ */
		case MSP_SLCT_DEM_SRC:

dprintk (KERN_DEB_S "msp3400satII: ==> MSP_SLCT_DEM_SRC (src=0x%x)\n", *sarg);

			return msp3400c_select_demod_src (msp_client, *sarg);
#endif			      
		/* select internal DFP analog input source */
		case MSP_SLCT_DFPANALOG_SRC:

			DEB_EE(("I2C client %s; MSP_SLCT_DFPANALOG_SRC (src=0x%x)\n", msp_client->name, *sarg));

			return msp3400c_select_dfpanalog_src (msp_client, *sarg);

		/* select loudspeaker output source */
		case MSP_SLCT_LSP_SRC:

			DEB_EE(("I2C client %s; MSP_SLCT_LSP_SRC (src=0x%x)\n", msp_client->name, *sarg));

			return msp3400c_select_lsp_src (msp_client, *sarg);

		/* select internal SCART output source */
		case MSP_SLCT_SCART_SRC:

			DEB_EE(("I2C client %s; MSP_SLCT_SCART_SRC (src=0x%x)\n", msp_client->name, *sarg));

			return msp3400c_select_scart_src (msp_client, *sarg);

		/* select source of MSP3400C SCART1 output */
		case MSP_SLCT_SC1_SRC:

			DEB_EE(("I2C client %s; MSP_SLCT_SC1_SRC (src=0x%x)\n", msp_client->name, *sarg));

			return msp3400c_select_sc1_src (msp_client, *sarg);
			
		/* select source of MSP3400C SCART2 output */
		case MSP_SLCT_SC2_SRC:

			DEB_EE(("I2C client %s; MSP_SLCT_SC2_SRC (src=0x%x)\n", msp_client->name, *sarg));

			return msp3400c_select_sc2_src (msp_client, *sarg);
			
		/* set state of digital control output 0 */
		case MSP_SET_DIGCTR0:

			DEB_EE(("I2C client %s; MSP_SET_DIGCTR0 (arg=0x%04x)\n", msp_client->name, *sarg));

			return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0013, 0x4000, *sarg);

		/* set state of digital control output 1 */
		case MSP_SET_DIGCTR1:

			DEB_EE(("I2C client %s; MSP_SET_DIGCTR1 (arg=0x%04x)\n", msp_client->name, *sarg));

			return msp3400c_setregister (msp_client, I2C_MSP3400C_DFP, 0x0013, 0x8000, *sarg);
			
		default:
		
			DEB_EE(("I2C client %s; Unknown ioctl cmd (0x%x)\n", msp_client->name, cmd)); 

			return -ENOIOCTLCMD;
	}
}


void msp3400c_inc_use (struct i2c_client *msp_client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


void msp3400c_dec_use (struct i2c_client *msp_client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


static struct i2c_driver msp_driver = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,54)
	.owner		= THIS_MODULE,
#endif
	.name		= "msp3400satII.o",
	.id		= I2C_DRIVERID_MSP3400,
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= msp3400c_attach,
	.detach_client	= msp3400c_detach,
	.command	= msp3400c_command,
	.inc_use	= msp3400c_inc_use,
	.dec_use	= msp3400c_dec_use
};

/*****************************************************************************/

EXPORT_NO_SYMBOLS;

int __devinit msp3400c_init (void)
{
	int ret;

	if ( 0 != (ret=i2c_add_driver (&msp_driver)) ) {
		DEB_S(("Driver %s registration failed, module not inserted.\n", msp_driver.name));
		return ret;
	}

	return 0;
}


void __devexit msp3400c_cleanup (void)
{
	if ( 0 != i2c_del_driver (&msp_driver) ) {
		DEB_S(("Driver %s deregistration failed, module not removed.\n", msp_driver.name));
	}
}

/*****************************************************************************/

MODULE_AUTHOR ("Torsten Tittmann <Torsten.Tittmann@gmx.de>");
MODULE_DESCRIPTION ("Micronas Intermetall MSP3400C I2C driver");
MODULE_LICENSE("GPL");

module_init(msp3400c_init);
module_exit(msp3400c_cleanup);


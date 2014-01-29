/*
    stv0056af.c		A driver for SGS-Thompson STV0056AF
			Satellite Sound and Video Processor 

    Version: $Id: stv0056af.c,v 1.4 2004-04-17 15:32:13 ttittmann Exp $
			
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
   
    stv0056af.c		SGS-Thomson STV0056AF I2C driver
 
    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>
*/

#define DEBUG_VARIABLE stv0056af_debug

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
#include <asm/delay.h>

#include <media/saa7146.h>
#include "stv0056af.h"

static int stv0056af_debug = 0;	/* module load parameter */
#ifdef MODULE
MODULE_PARM(stv0056af_debug,"i");
MODULE_PARM_DESC(stv0056af_debug, "debug level (default: 0)");
MODULE_PARM(video_gain,"i");
MODULE_PARM_DESC(video_gain,"gain = n * 0.202 dB (default: 0)");
MODULE_PARM(decoder_output,"i");
MODULE_PARM_DESC(decoder_output,"output mode of Mini DIN jack (default: 0)");
#endif

static int video_gain = -1;
static int decoder_output = -1;

/* I2C addresses to scan */
static unsigned short int normal_i2c[] = { I2C_STV0056AF_1, I2C_STV0056AF_2, I2C_CLIENT_END };
static unsigned short int normal_i2c_range[] = { I2C_CLIENT_END };

/* magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/* unique ID allocation */
static int stv0056af_id = 0;

static struct i2c_driver stv_driver;

/* The STV0056AF doesn't feature a single register read via I2C. Only	*/
/* a block read of its read-able registers 6..10 is possible. But this	*/
/* is NOT an SMBUS block read because the "command" byte (number of	*/
/* first register) is omitted. To make things worse the STV0056AF	*/
/* transmits registers 6..10 in a 8-9-10-7-6 sequence.			*/
/* So, every time we need a certain register we use 'i2c_master_recv'	*/
/* to read the whole block into the array 'stv_readregs'. The macro	*/
/* GET_STV_REG gets that very register from the array. Only numbers in	*/
/* the range 6..10 are allowed as GET_STV_REG argument.			*/
unsigned char stv_readregs[5];
#define GET_STV_REG( reg )	stv_readregs[(reg>7) ? (reg-8) : (10-reg)]

/* STV0056AF registers and bit masks */
#define STV_REG0_VOLUME	0x1f
#define STV_REG1_GAIN	0x3f
#define STV_REG2_B7	0x80
#define STV_REG2_B6	0x40
#define STV_REG234_OSEL	0x07	/* output source select */
#define STV_REG6_SW1	0x20
#define STV_REG6_SW2	0x08
#define	STV_REG6_SW3	0x10
#define STV_REG6_SW4	0x04
#define STV_REG6_IODIR	0x02	/* direction of IO pin; 1 = output*/
#define	STV_REG6_IO	0x01	/* state of IO pin */
#define STV_REG6_B7B6	0xc0
#define STV_REG8_LSUB	0x01
#define	STV_REG8_RSUB	0x10

#define STV_SCART1	0
#define STV_SCART2	1
#define STV_DECODER	2

/*****************************************************************************/

static int stv0056af_set_volume (struct i2c_client *stv_client, unsigned char volume)	/* select volume */
{
	struct stv0056af *stv = (struct stv0056af *) stv_client->data;
	unsigned char stv_register_0;
	int ret;

	DEB_D(("I2C client '%s'; vol=0x%02x.\n", stv_client->name, volume));

	stv_register_0 = stv->writeregs[0] & !STV_REG0_VOLUME;	/* read and erase old volume */
	stv_register_0 |= volume & STV_REG0_VOLUME;		/* set new volume */
	
	if ( (ret=i2c_smbus_write_byte_data (stv_client, 0x00, stv_register_0)) != 0) {
		DEB_S(("I2C client '%s' failed.\n", stv_client->name));
		return -EIO;
	}
	
	stv->writeregs[0] = stv_register_0;

	return 0;
}


static int stv0056af_set_gain (struct i2c_client *stv_client, unsigned char gain)	/* select video gain */
{
	struct stv0056af *stv = (struct stv0056af *) stv_client->data;
	unsigned char stv_register_1;
	int ret;

	DEB_D(("I2C client '%s'; gain=0x%02x.\n", stv_client->name, gain));

	stv_register_1 = stv->writeregs[1] & !STV_REG1_GAIN;	/* read and erase old gain */
	stv_register_1 |= gain & STV_REG1_GAIN;			/* set new gain */
	
	if ( (ret=i2c_smbus_write_byte_data (stv_client, 0x01, stv_register_1)) != 0) {
		DEB_S(("I2C client '%s' failed.\n", stv_client->name));
		return -EIO;
	}
	
	stv->writeregs[1] = stv_register_1;

	return 0;
}


static int stv0056af_select_output (struct i2c_client *stv_client, int output, int source)
{
	struct stv0056af *stv = (struct stv0056af *) stv_client->data;
	unsigned char reg_nr, stv_register;
	int ret;
	
	DEB_D(("I2C client '%s'; out=%d; src=%d\n", stv_client->name, output, source));

	switch (output) {
		case STV_SCART1:
			reg_nr = 0x02;
			break;
		case STV_SCART2:
			reg_nr = 0x03;
			break;
		case STV_DECODER:
			reg_nr = 0x04;
			break;
		default:
			DEB_S(("I2C client '%s'; Wrong output channel.\n", stv_client->name));
			return -EINVAL;
	}
	stv_register = stv->writeregs[reg_nr] & !STV_REG234_OSEL;
	stv_register |= source & STV_REG234_OSEL;
	
	if ( (ret=i2c_smbus_write_byte_data (stv_client, reg_nr, stv_register)) != 0) {
		DEB_S(("I2C client '%s' failed.\n", stv_client->name));
		return -EIO;
	}
	
	stv->writeregs[reg_nr] = stv_register;

	return 0;
}


/* !! Untested !! */
/* Not used & not needed by MediaFocusII */
static int stv0056af_set_carrier (struct i2c_client *stv_client, unsigned int freq1, unsigned int freq2)	/* select frequencies */
{
	struct stv0056af *stv = (struct stv0056af *) stv_client->data;
	int ret = 0, watchdog1, watchdog2, leftsub = 0, maxcount;
	unsigned char org_vol, stv_register_2, stv_register_6;
   
	DEB_D(("I2C client '%s'; f1=%d; f2=%d.\n", stv_client->name, freq1, freq2));

	org_vol = stv->writeregs[0] & STV_REG0_VOLUME;		/* original volume */
	ret |= stv0056af_set_volume (stv_client, 0);	/* mute audio */

	ret |= (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof (stv_readregs));
	stv_register_6 = GET_STV_REG(6) & !(STV_REG6_SW1 | STV_REG6_SW2 | STV_REG6_SW3 | STV_REG6_SW4);	/* clear old bits */
	stv_register_6 |= STV_REG6_SW4;		/* SW3(L) to Bias, SW4(L) closed */
	ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);	/* SW1(R) to Bias, SW2(R) open */
	stv_register_6 &= !STV_REG6_B7B6;			/* clear old bits*/
	stv_register_6 |= ((freq1 << 6) & STV_REG6_B7B6);	/* set new freq1bits */
	ret |= i2c_smbus_write_byte_data (stv_client, 0x07, freq1 >> 2);	/* set synth to left chan subcarrier */
	ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);
	maxcount = 200;
	do {
		udelay (10000);					/* okay ??? */
		ret |= (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof (stv_readregs));
		watchdog1 = ((int) GET_STV_REG(9) << 2) | ((GET_STV_REG(8) >> 2) & 0x03);
		maxcount--;
	} while ((abs (watchdog1 - freq1) > 1) && (maxcount != 0));	/* loop until |watchdog1 - freq1| <= 10 kHz */
	if (maxcount == 0) {
		DEB_S(("I2C client '%s'; tracking of left audio channel synth failed!\n", stv_client->name));
		return -1;
	}
	stv_register_6 &= !STV_REG6_SW4;
	stv_register_6 |= STV_REG6_SW3;
	ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);	/* SW3(L) to FM sig, SW4(L) opened */
	/* FIXME!! Delay needed ??? */
	ret |= (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof (stv_readregs));
	if (GET_STV_REG(8) & STV_REG8_LSUB) {			/* Left Subcarrier present? */
		leftsub = 1;					/* yes: Unmute Audio to Left Mono */
		stv_register_2 = stv->writeregs[2] & !(STV_REG2_B7 | STV_REG2_B6);
		ret |= i2c_smbus_write_byte_data (stv_client, 0x02, stv_register_2);
		stv->writeregs[2] = stv_register_2;
		ret |= stv0056af_set_volume (stv_client, org_vol);
	}

	stv_register_6 |= STV_REG6_SW2;
	ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);	/* SW2(R) closed */
	stv_register_6 &= !STV_REG6_B7B6;
	stv_register_6 |= (freq2 << 6) & STV_REG6_B7B6;
	ret |= i2c_smbus_write_byte_data (stv_client, 0x07, freq2 >> 2);	/* set synth to right chan subcarrier */
	ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);
	maxcount = 200;
	do {
		udelay (10000);
		ret |= (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof (stv_readregs));
		watchdog2 = (GET_STV_REG(10) << 2) | ((GET_STV_REG(8) >> 6) & 0x03);
		maxcount--;
	} while ((abs (watchdog2 - freq2) > 1) && (maxcount != 0));	/* loop until |watchdog2 - freq2| <= 10 kHz */
	if (maxcount == 0) {
		DEB_S(("I2C client '%s'; tracking of right audio channel synth failed!\n", stv_client->name));
		return -1;
	}
	stv_register_6 &= !STV_REG6_SW2;
	stv_register_6 |= STV_REG6_SW1;
	ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);	/* SW1(R) to FM sig, SW2(R) opened */

	ret |= (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof (stv_readregs));
	if (GET_STV_REG(8) & STV_REG8_RSUB) {			/* Right Subcarrier present? */
		if (leftsub) {					/* Was Left Subcarrier present? */
			stv_register_2 = stv->writeregs[2] | (STV_REG2_B7 | STV_REG2_B6);	/* Unmute Audio to Stereo */
			ret |= i2c_smbus_write_byte_data (stv_client, 0x02, stv_register_2);
			stv->writeregs[2] = stv_register_2;	/* original volume already set */
		} else {
			stv_register_2 = stv->writeregs[2] & !STV_REG2_B6;
			stv_register_2 |= STV_REG2_B7;		/* Unmute Audio to Right Mono */
			ret |= i2c_smbus_write_byte_data (stv_client, 0x02, stv_register_2);
			stv->writeregs[2] = stv_register_2;
			ret |= stv0056af_set_volume (stv_client, org_vol);
		}
	}

	if (ret != 0) {
		DEB_S(("I2C client '%s' failed.\n", stv_client->name));
		return -EIO;
	}

	return 0;
}


static int stv0056af_chip_init (struct i2c_client *stv_client)	/* initialize the stv0056af */
{
	struct stv0056af *stv = (struct stv0056af *) stv_client->data;
	int i, ret = 0;
	const unsigned char init[][2] = {
	/* addr	value */
	{ 0x00,	0x80 },	/* 5 register read cycle, no ANRS, audio i/p from PLL, muted audio	*/
	{ 0x01,	0x00 },	/* video deemp = VIDEEM1, video non-inverted, video gain = 0dB */
	{ 0x02,	0xcf },	/* vol o/p = stereo, vol i/p = audio deemp, 4MHz clk, scart1 video = high Z */
	{ 0x03,	0xf7 },	/* audio deemp = 75us, scart2 audio = high Z, pin 9 = VIDEEM2, scart2 video = high Z */
	{ 0x04, 0x8a },	/* black level = 0, scart3 audio = PLL, low power, scart3 = normal video */
	{ 0x05,	0x36 },	/* pin29 = i/o, 22kHz off, 49.1 kHz FM deviation */
	{ 0x06,	0x42 },	/* SW1=Bias,SW3=Bias,SW2 opened,SW4 opened, i/o = output, i/o = L */
	{ 0x07,	0xa6 },	/* synth freq = 6.65 MHz */
	};

	DEB_EE(("I2C client '%s'.\n", stv_client->name));

	for (i = 0; i < (sizeof(init) / (sizeof(init[0]))); i++) {
		ret |= i2c_smbus_write_byte_data (stv_client, init[i][0], init[i][1]);
		if (i <= 5) stv->writeregs[i] = init[i][1];

		udelay (50);
	}
	
	memset (stv, 0, sizeof (struct stv0056af));

	stv->adapter = stv_client->adapter;
	if ((video_gain > -1) && (video_gain <= 0x3f)) {	/* valid insmod arg? */
		stv->video_gain = video_gain;
		stv0056af_set_gain (stv_client, video_gain);
		DEB_S(("I2C client '%s'; set by insmod option: video gain = +%.3f dB\n", stv_client->name, 0.202*video_gain));
	}
	else
		stv->video_gain = 0;
	stv->source_scart1 = STV_HIGHZ;
	stv->source_scart2 = STV_HIGHZ;
	if ((decoder_output > -1) && (decoder_output <= 2)) {	/* valid insmod arg? */
		stv->source_decoder = decoder_output;
		stv0056af_select_output (stv_client, STV_DECODER, decoder_output);
		switch (decoder_output) {
		case 0:
			DEB_S(("I2C client '%s'; set by insmod option: decoder output mode = baseband unclamped\n", stv_client->name));
			break;
		case 1:
			DEB_S(("I2C client '%s'; set by insmod option: decoder output mode = deemphasized unclamped\n", stv_client->name));
			break;
		case 2:
			DEB_S(("I2C client '%s'; set by insmod option: decoder output mode = deemphasized clamped\n", stv_client->name));
			break;
		default:
			printk ("(?)\n");
		}
	} else {
		stv->source_decoder = STV_NORMAL_VIDEO;
	}
	
	stv->volume = 0;
	stv->sig22khz = STV_22KHZ_OFF;
		
	/* FIXME!! stv0056af_set_carrier could be called at this place to set initial audio carriers */

	if (ret != 0) {
		DEB_S(("I2C client '%s' failed.\n", stv_client->name));
		return -EIO;
	}

	return 0;
}

/*****************************************************************************/

/* this function is called by i2c_probe */
static int stv0056af_detect (struct i2c_adapter *adap, int addr, unsigned short int flags, int kind)
{
	struct i2c_client *stv_client;
	struct stv0056af *stv;
	int ret;

	DEB_EE(("I2C adapter '%s'; addr=0x%02x.\n", adap->name, addr));

	/* let's see whether this adapter can support what we need */
	if (i2c_check_functionality (adap, I2C_FUNC_I2C | I2C_FUNC_SMBUS_WRITE_BYTE_DATA) == 0) {
		return 0;
	}

	/* allocate memory for client structure */
	if ( (stv_client = kmalloc (sizeof (struct i2c_client), GFP_KERNEL)) == NULL) 
		return -ENOMEM;

	/* fill client structure */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	sprintf (stv_client->name, "SGS-Thomson STV0056AF");
#else
	sprintf (stv_client->dev.name, "SGS-Thomson STV0056AF");
#endif
	stv_client->id = stv0056af_id++;
	stv_client->flags = 0;
	stv_client->addr = addr;
	stv_client->adapter = adap;
	stv_client->driver = &stv_driver;
	stv_client->data = NULL;

	if ( stv0056af_id == 0) {
		DEB_S(("I2C client '%s'; No hardware detected.\n", stv_client->name));
	} else {
		DEB_S(("I2C client '%s' detected at i2c address 0x%02x.\n", stv_client->name, stv_client->addr));
	}

	/* allocate memory for stv structure */
	if ( (stv_client->data = stv = kmalloc (sizeof (struct stv0056af), GFP_KERNEL)) == NULL) {
		kfree (stv_client);
		return -ENOMEM;
	}

	/* tell the i2c layer a new client has arrived */
	if ( (ret=i2c_attach_client (stv_client)) != 0) {
		DEB_S(("I2c client registration failed, '%s' @ 0x%02x not attached.\n", stv_client->name, stv_client->addr));
		kfree (stv_client->data);
		kfree (stv_client);
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x attached to adapter '%s'.\n", stv_client->name, stv_client->addr, stv_client->adapter->name));

	/* STV0056AF chip init */
	if ( (ret=stv0056af_chip_init (stv_client)) != 0) {
		DEB_S(("Init of I2C client '%s' @ 0x%02x failed, continuing anyway.\n", stv_client->name, stv_client->addr));
		return ret;
	}

	return 0;
}


static int stv0056af_attach (struct i2c_adapter *adap)
{
	DEB_EE(("I2C adapter '%s'.\n", adap->name));

	return i2c_probe (adap, &addr_data, &stv0056af_detect);
}


static int stv0056af_detach (struct i2c_client *stv_client)
{
	int ret = 0;

	DEB_EE(("I2C client '%s'; addr=0x%02x; adap='%s'.\n", stv_client->name, stv_client->addr, stv_client->adapter->name));

	/* Not used & not needed by MediaFocusII */
	/* ret |= stv0056af_set_volume (stv_client, 0); */

	/* FIXME!! switch off audio before unload the module */
	/* ret |= stv0056af_write (stv, 0x06, 0x02); */
	if (ret != 0) {
		DEB_S(("Audio mute before detaching of I2C client '%s' @ 0x%02x failed.\n", stv_client->name, stv_client->addr));
	}

	if ( (ret=i2c_detach_client (stv_client)) != 0) {
		DEB_S(("I2C client deregistration failed, '%s' @ 0x%02x not detached.\n", stv_client->name, stv_client->addr));
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x detached from adapter '%s'.\n", stv_client->name, stv_client->addr, stv_client->adapter->name));

	kfree (stv_client->data);
	kfree (stv_client);

	return 0;
}


static int stv0056af_command (struct i2c_client *stv_client, unsigned int cmd, void *arg)
{
	struct stv0056af *stv = (struct stv0056af *) stv_client->data;
	unsigned long f = *(unsigned long *) arg;
	unsigned int *sarg = arg;
	int ret = 0;
	unsigned char stv_register_6;

	switch (cmd) {
		case STV_GET_GAIN: {
			*sarg = stv->video_gain;

			DEB_EE(("I2C client '%s'; STV_GET_GAIN (gain=0x%x)\n", stv_client->name, *sarg));

			return ret;
		}

		/* set gain of video path */
		case STV_SET_GAIN: {
			stv->video_gain = *sarg;

			DEB_EE(("I2C client '%s'; STV_SET_GAIN (gain=0x%x)\n", stv_client->name, *sarg));

			return stv0056af_set_gain (stv_client, stv->video_gain >> 10);
		}

		case STV_GET_SCART1: {
			*sarg = stv->source_scart1;

			DEB_EE(("I2C client '%s'; STV_GET_SCART1 (src=%d)\n", stv_client->name, *sarg));

			return ret;
		}

		/* select source for SCART1 output of STV0056AF */
		/* mediafocusII: normal video path which is shown on-screen */
		case STV_SET_SCART1: {
			stv->source_scart1 = *sarg;

			DEB_EE(("I2C client '%s'; STV_SET_SCART1 (src=%d)\n", stv_client->name, *sarg));

			return stv0056af_select_output (stv_client, STV_SCART1, stv->source_scart1);
		}

		case STV_GET_SCART2: {
			*sarg = stv->source_scart2;

			DEB_EE(("I2C client '%s'; STV_GET_SCART2 (src=%d)\n", stv_client->name, *sarg));

			return ret;
		}

		/* select source for SCART2 output of STV0056AF */
		/* Not used & not needed by MediaFocusII */
		case STV_SET_SCART2: {
			stv->source_scart2 = *sarg;

			DEB_EE(("I2C client '%s'; STV_SET_SCART2 (src=%d)\n", stv_client->name, *sarg));

			return stv0056af_select_output (stv_client, STV_SCART2, stv->source_scart2);
		}
			
		case STV_GET_DECODER: {
			*sarg = stv->source_decoder;

			DEB_EE(("I2C client '%s'; STV_GET_DECODER (src=%d)\n", stv_client->name, *sarg));

			return ret;
		}

		/* select source for SCART3 (= "Decoder") output of STV0056AF */
		/* mediafocusII: video output path to Mini-DIN jack */
		case STV_SET_DECODER: {
			stv->source_decoder = *sarg;

			DEB_EE(("I2C client '%s'; STV_SET_DECODER (src=%d)\n", stv_client->name, *sarg));

			return stv0056af_select_output (stv_client, STV_DECODER, stv->source_decoder);
		}

		/* Not used & not needed by MediaFocusII */
		case STV_GET_STEREO: {
			*sarg = stv->audio_mode;

			DEB_EE(("I2C client '%s'; STV_GET_STEREO (mode=0x%x)\n", stv_client->name, *sarg));

			return ret;
		}

		/* Not used & not needed by MediaFocusII */
		case STV_SET_STEREO: {
			stv->audio_mode = *sarg;

			DEB_EE(("I2C client '%s'; STV_SET_STEREO (mode=0x%x)\n", stv_client->name, *sarg));

			switch (stv->audio_mode) {
				case V4L2_TUNER_MODE_MONO:
					return stv0056af_set_carrier (stv_client, 0x0199, 0x0199); /* 6.60 MHz Mono	*/
				case V4L2_TUNER_MODE_STEREO:
					return stv0056af_set_carrier (stv_client, 0x0205, 0x0233); /* 7.02/7.20 MHz Stereo */
				case V4L2_TUNER_MODE_LANG1:
					return stv0056af_set_carrier (stv_client, 0x01A6, 0x01A6); /* 6.65 MHz Mono	*/	
				case V4L2_TUNER_MODE_LANG2:
					return stv0056af_set_carrier (stv_client, 0x0180, 0x0180); /* 6.50 MHz Mono	*/
				default:
					return -EINVAL;
			}
		}
		
		/* Not used & not needed by MediaFocusII */
		case STV_SWITCH_MUTE: {
			stv->volume = 0;

			DEB_EE(("I2C client '%s'; STV_SWITCH_MUTE\n", stv_client->name));

			return stv0056af_set_volume (stv_client, 0x00);
		}
		
		/* Not used & not needed by MediaFocusII */
		case STV_GET_VOLUME: {
			*sarg = stv->volume;

			DEB_EE(("I2C client '%s'; STV_GET_VOLUME (vol=%d)\n", stv_client->name, *sarg));

			return ret;
		}
		
		/* Not used & not needed by MediaFocusII */
		case STV_SET_VOLUME: {

			DEB_EE(("I2C client '%s'; STV_SET_VOLUME (vol=%d; stv_vol=0x%x)\n", stv_client->name, (uint) f, (unsigned char) (f >> 11)));

			ret = stv0056af_set_volume (stv_client, (unsigned char)(f >> 11));
			stv->volume = (uint)(f);

			return ret;
		}
		
		/* select generation of 22 kHz tone */
		/* MediaFocusII: pin "IO" is used as pure digital output */
		/*  to control 22 kHz generation in LNB voltage chip	 */
		case STV_SET_22KHZ: {

			DEB_EE(("I2C client '%s'; STV_SET_22KHZ (state=0x%x)\n", stv_client->name, *sarg));

			switch (*sarg) {
				case STV_22KHZ_ON:
					if (stv->sig22khz == STV_22KHZ_OFF) {
						ret = (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof(stv_readregs));
						stv_register_6 = GET_STV_REG(6) | STV_REG6_IO;
						stv_register_6 |= STV_REG6_IODIR;	/* FIX */
						ret |= i2c_smbus_write_byte_data (stv_client, 0x06, stv_register_6);
						stv->sig22khz = STV_22KHZ_ON;
						
						if (ret != 0) {
							DEB_S(("I2C client '%s'; STV_22KHZ_ON failed.\n", stv_client->name));
							return -EIO;
						}

						DEB_EE(("I2C client '%s'; SET_22KHZ: 22kHz was off, now switched on\n", stv_client->name));

					}
					break;
					
				case STV_22KHZ_OFF:
					if (stv->sig22khz == STV_22KHZ_ON) {
						ret = (i2c_master_recv (stv_client, stv_readregs, sizeof (stv_readregs)) != sizeof (stv_readregs));
						stv_register_6 = GET_STV_REG(6) & !STV_REG6_IO;
						stv_register_6 |= STV_REG6_IODIR;	/* FIX */
						ret |= i2c_smbus_write_byte_data (stv_client, 0x06,stv_register_6);
						stv->sig22khz = STV_22KHZ_OFF;

						if (ret != 0) {
							DEB_S(("I2C client '%s'; STV_22KHZ_OFF failed.\n", stv_client->name));
							return -EIO;
						}

						DEB_EE(("I2C client '%s'; SET_22KHZ: 22kHz was on, now switched off\n", stv_client->name));

					}
					break;
					
				default:
					return -EINVAL;
			}
			return ret;
		}
		
		default:
		
			DEB_EE(("I2C client '%s'; own ioctl cmd (0x%x).\n", stv_client->name, cmd));

			return -ENOIOCTLCMD;
	}
}


void stv0056af_inc_use (struct i2c_client *stv_client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


void stv0056af_dec_use (struct i2c_client *stv_client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


static struct i2c_driver stv_driver = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,54)
	.owner		= THIS_MODULE,
#endif
	.name		= "stv0056af.o",
	.id		= I2C_DRIVERID_STV0056,
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= stv0056af_attach,
	.detach_client	= stv0056af_detach,
	.command	= stv0056af_command,
	.inc_use	= stv0056af_inc_use,
	.dec_use	= stv0056af_dec_use
};

/*****************************************************************************/

EXPORT_NO_SYMBOLS;

int __devinit stv0056af_init (void)
{
	int ret;

	if ( 0 != (ret=i2c_add_driver (&stv_driver)) ) {
		DEB_S(("Driver '%s' registration failed, module not inserted.\n", stv_driver.name));
		return ret;
	}

	return 0;
}


void __devexit stv0056af_cleanup (void)
{
	if ( 0 != i2c_del_driver (&stv_driver) )
		DEB_S(("Driver '%s' deregistration failed, module not removed.\n", stv_driver.name));
}

/*****************************************************************************/

MODULE_AUTHOR ("Torsten Tittmann <Torsten.Tittmann@gmx.de>");
MODULE_DESCRIPTION ("SGS-Thomson STV0056AF I2C driver");
MODULE_LICENSE("GPL");

module_init(stv0056af_init);
module_exit(stv0056af_cleanup);


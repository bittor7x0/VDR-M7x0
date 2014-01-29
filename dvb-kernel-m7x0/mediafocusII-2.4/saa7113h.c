/*
    saa7113h.c		A driver for Philips SAA7113H
			9-bit Video Input Processor 

    Version: $Id: saa7113h.c,v 1.4 2004-04-17 15:32:13 ttittmann Exp $

    Copyright (C) 2003 Torsten Tittmann <Torsten.Torsten@gmx.de>

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
 
    saa7113h.c		Philips SAA7113H I2C driver
 
    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>
 */

#define DEBUG_VARIABLE saa7113h_debug

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
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <linux/video_decoder.h>

#include <media/saa7146.h>
#include "video_decoder_priv.h"

#include "saa7113h.h"

static int saa7113h_debug = 0;	/* module load parameter */
#ifdef MODULE
MODULE_PARM(saa7113h_debug,"i");
MODULE_PARM_DESC(saa7113h_debug, "debug level (default: 0)");
#endif

/* I2C addresses to scan */
static unsigned short int normal_i2c[] = { I2C_SAA7113H_1, I2C_SAA7113H_2, I2C_CLIENT_END };
static unsigned short int normal_i2c_range[] = { I2C_CLIENT_END };

/* magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/* unique ID allocation */
static int saa7113h_id = 0;

static struct i2c_driver saa7113h_driver;

/***************************************/

/* select video input */
static int saa7113h_set_video_input (struct i2c_client *decoder_client, int video_input)
{
	struct saa7113h *decoder = (struct saa7113h *) decoder_client->data;
	unsigned char val;
	int ret = 0;

	DEB_D(("I2C client '%s'; mode=%d.\n", decoder_client->name, video_input));

	/* read Analog control 1 register (SA 02) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x02);
	
	/*val &= 0x0f;*/			/* clr bits 3..0 */
	val &= 0xf0;			/* clr bits 3..0 */
	val |= video_input & 0x0f;	/* merge new input mode */
	
	/* write back Analog control1 register */	 
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x02, val);

	/* bypass chrominance trap filter in SVHS modes (according to SAA7113H data sheet) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x09);
	if (video_input >= DECODER_INPUT_MODE6)
		val |= 0x80;	/* set BYPS bit (bit7 of SA 09) */
	else
		val &= 0x7f;	/* clear BYPS bit */
	
	if ( (ret |= i2c_smbus_write_byte_data (decoder_client, 0x09, val)) != 0) {
		DEB_S(("I2C client '%s' failed.\n", decoder_client->name));
		return -EIO;
	}

	decoder->input = video_input;
	
	return 0;
}


static int saa7113h_set_fuse (struct i2c_client *decoder_client, unsigned int fuse)
{
	/* struct saa7113h *decoder = (struct saa7113h *) decoder_client->data; */
	unsigned char val, fuse_arg;
	int ret = 0;

	DEB_D(("I2C client '%s'; fuse=0x%x.\n", decoder_client->name, fuse));

	switch (fuse) {
		case 0:
			fuse_arg = 0;
			break;
		case DECODER_USE_ANTIALIAS:
			return -EINVAL;
		case DECODER_USE_AMPLIFIER:
			fuse_arg = 0x80;
			break;
		case DECODER_USE_AMPLIFIER | DECODER_USE_ANTIALIAS:
			fuse_arg = 0xc0;
			break;
		default:
			return -EINVAL;
	}
	
	/* set FUSE bits (bits 7..6 of SA 02) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x02);
	val &= 0x3f;		/* clear bits */
	val |= fuse_arg;	/* set bits */
	
	if ( (ret=i2c_smbus_write_byte_data (decoder_client, 0x02, val)) != 0) {
		DEB_S(("I2C client '%s' failed.\n", decoder_client->name));
		return -EIO;
	}
	
	return 0;
}


static int saa7113h_set_video_attributes (struct i2c_client *decoder_client)
{
	struct saa7113h *decoder = (struct saa7113h *) decoder_client->data;
	unsigned char brightness, contrast, saturation, hue;
	int ret = 0;
	
	brightness = decoder->brightness >> 8;
	contrast = decoder->contrast >> 9;
	saturation = decoder->saturation >> 9;
	hue = decoder->hue >> 8;
	if (hue >= 0x80)
		hue -= 0x80;
	else
		hue += 0x80;
	
	DEB_D(("I2C client '%s'; bri=0x%02x; con=0x%02x; sat=0x%02x; hue=0x%02x.\n", decoder_client->name, brightness, contrast, saturation, hue));

	ret |= i2c_smbus_write_byte_data (decoder_client, 0x0a, brightness);
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x0b, contrast);
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x0c, saturation);
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x0d, hue);
	
	if (ret != 0) {
		DEB_S(("I2C client '%s' failed.\n", decoder_client->name));
		return -EIO;
	}
	
	return 0;
}


static int saa7113h_set_video_norm (struct i2c_client *decoder_client, unsigned short int video_norm)
{
	struct saa7113h *decoder = (struct saa7113h *) decoder_client->data;
	unsigned char val;
	unsigned char fsel_arg = 0x00;	/* field selection, default: 50 Hz, 625 lines */
	unsigned char cstd_chbw_arg = 0x01;	/* colour standard, default: PAL-BGHIN resp. NTSC-M */
						/* chrominance bandwidth, default: 800 kHz */
	unsigned char fiset_arg = 0x00;	/* field size select, default: 50 Hz field rate */
	unsigned char voffhi_arg = 0x00, vofflo_arg = 0x07;	/* vertical offset, default: 50 Hz 625 lines input */
	int ret = 0;

	DEB_D(("I2C client '%s'; norm=%d.\n", decoder_client->name, video_norm));

	switch (video_norm) {
		case VIDEO_MODE_PAL:
			break;
		case VIDEO_MODE_SECAM:
			cstd_chbw_arg = 0x50;	/* colour standard -> SECAM; chrominance bw -> 620 kHz */
			break;
		case VIDEO_MODE_NTSC:
			fsel_arg = 0x40;	/* field selection -> 60 Hz, 525 lines */
			fiset_arg = 0x80;	/* field size select -> 60 Hz field rate */
			vofflo_arg = 0x0a;	/* vertical offset -> 60 Hz 525 lines input */
			break;
		default:
			return -EINVAL;
	}
	
	/* set FSEL bit (bit 6 of SA 08) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x08);
	val &= 0xbf;		/* clear bit */
	val |= fsel_arg;	/* set bit */
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x08, val);
	
	/* set CSTD+CHBW bits (bits 6..4,1..0 of SA 0E) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x0e);
	val &= 0x8c;
	val |= cstd_chbw_arg;
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x0e, val);
	
	/* set FISET bit (bit 7 of SA 40) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x40);
	val &= 0x7f;
	val |= fiset_arg;
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x40, val);
	
	/* set VOFF bits (bit 4 (8) of SA 5B, bits 7..0 (7..0) of SA 5A) */
	val = i2c_smbus_read_byte_data (decoder_client, 0x5b);
	val &= 0xef;
	val |= voffhi_arg;
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x5b, val);
	ret |= i2c_smbus_write_byte_data (decoder_client, 0x5a, vofflo_arg);
	
	if (ret |= 0) {
		DEB_S(("I2C client '%s' failed.\n", decoder_client->name));
		return -EIO;
	}
	
	decoder->norm = video_norm;
	
	return 0;
}


static int saa7113h_dump (struct i2c_client *decoder_client)
{
	/* struct saa7113h *decoder = (struct saa7113h *) decoder_client->data; */
	/* int ret = 0; */

	DEB_EE(("I2C client '%s'.\n", decoder_client->name));

	return 0;
}


static int saa7113h_chip_init (struct i2c_client *decoder_client)
{
	struct saa7113h *decoder = (struct saa7113h *) decoder_client->data;
	int i, ret = 0;
	const unsigned char init_from_01[] = {
	/* initial register values as recommended by SAA7113H data sheet  */
	/* only some settings are departing here			  */
      0x08,	/* SA 01:   X	  X	X     X   IDEL3 IDEL2 IDEL1 IDEL0 */
		/*							  */
		/* IDEL3 IDEL2 IDEl1 IDEL0 = 1 0 0 0:			  */
		/* Horizontal increment delay				  */
		
      0xc0,	/* SA 02: FUSE1 FUSE0 GUDL1 GUDL0 MODE3 MODE2 MODE1 MODE0 */
		/*							  */
		/* FUSE1 FUSE0 = 1 1:					  */
		/* Amplifier plus anti-alias filter active		  */
		/*							  */
		/* GUDL1 GUDL0 = 0 0:					  */
		/* Update Hysteresis for 9-Bit Gain: off		  */
		/*							  */
		/* MODE3 MODE2 MODE1 MODE0 = 0 0 0 0:			  */
		/* Mode 0, CVBS from AI11				  */
		
      0x33,	/* SA 03:   X	HLNRS VBSL  WPOFF HOLDG GAFIX GAI28 GAI18 */
		/*							  */
		/* HLNRS = 0:						  */
		/* Normal clamping if decoder is in unlocked state	  */
		/*							  */
		/* VBSL = 1:						  */
		/* Long AGC hold during vertical blanking period	  */
		/*							  */
		/* WPOFF = 1:						  */
		/* White peak control off				  */
		/*							  */
		/* HOLDG = 0:						  */
		/* AGC active						  */
		/*							  */
		/* GAFIX = 0:						  */
		/* AGC active						  */
		
      0x00,	/* SA 04: GAI17 GAI16 GAI15 GAI14 GAI13 GAI12 GAI11 GAI10 */
		/*							  */
		/* (Static gain channel 1 = -3 dB; not active)		  */
		
      0x00,	/* SA 05: GAI27 GAI26 GAI25 GAI24 GAI23 GAI22 GAI21 GAI20 */
		/*							  */
		/* (Static gain channel 2 = -3 dB; not active)		  */
		
      0x00,	/* SA 06: HSB7	HSB6  HSB5  HSB4  HSB3	HSB2  HSB1  HSB0  */
		/*							  */
		/* !!! Departing from SAA7113H datasheet !!!		  */

      0xec,	/* SA 07: HSS7	HSS6  HSS5  HSS4  HSS3	HSS2  HSS1  HSS0  */
		/*							  */
		/* !!! Departing from SAA7113H datasheet		  */

      0x18,	/* SA 08: AUFD	FSEL  FOET  HTC1  HTC0	HPLL  VNOI1 VNOI0 */
		/*							  */
		/* AUFD = 0:						  */
		/* No Automatic field detection				  */
		/* !!! Departing from SAA7113H datasheet		  */
		/*							  */
		/* FSEL = 0:						  */
		/* 50 Hz, 625 lines					  */
		/*							  */
		/* FOET = 0:						  */
		/* ODD/EVEN signal toggles only with interlaced source	  */
		/*							  */
		/* HTC1 HTC0 = 1 1:					  */
		/* Horizontal time constant selection: Fast locking mode  */
		/*							  */
		/* HPLL = 0:						  */
		/* Horizontal PLL closed				  */
		/*							  */
		/* VNOI1 VNOI0 = 0 0:					  */
		/* Vertical noise reduction: Normal mode		  */
		
      0x09,	/* SA 09: BYPS	PREF  BPSS1 BPSS0 VBLB	UPTCV APER1 APER0 */
		/*							  */
		/* BYPS = 0:						  */
		/* Chrominance trap active				  */
		/*							  */
		/* PREF = 0:						  */
		/* Prefilter bypassed					  */
		/*							  */
		/* BPSS1 BPSS0 = 0 0:					  */
		/* Aperture band-pass center frquency: 4.1 MHz		  */
		/*							  */
		/* VBLB = 1:						  */
		/* Vertical blanking luminance processing inactive	  */
		/* !!! Departing from SAA7113H datasheet !!!		  */
		/*							  */
		/* UPTCV = 0:						  */
		/* Analog AGC value updated once per line		  */
		/*							  */
		/* APER1 APER0 = 0 1:					  */
		/* Aperture factor = 0.25				  */
		
      0x80,	/* SA 0A: BRIG7 BRIG6 BRIG5 BRIG4 BRIG3 BRIG2 BRIG1 BRIG0 */
      
      0x47,	/* SA 0B: CONT7 CONT6 CONT5 CONT4 CONT3 CONT2 CONT1 CONT0 */
      
      0x40,	/* SA 0C: SATN7 SATN6 SATN5 SATN4 SATN3 SATN2 SATN1 SATN0 */
      
      0x00,	/* SA 0D: HUEC7 HUEC6 HUEC5 HUEC4 HUEC3 HUEC2 HUEC1 HUEC0 */
      
      0x01,	/* SA 0E: CDTO	CSTD2 CSTD1 CSTD0 DCCF	FCTC  CHBW1 CHBW0 */
		/*							  */
		/* CDTO = 0:						  */
		/* Clear DTO: Disabled					  */
		/*							  */
		/* CSTD2 CSTD1 CSTD0 = 0 0 0:				  */
		/* PAL-BGHIN @ 50 Hz (resp. NTSC-M @ 60 Hz; not active)	  */
		/*							  */
		/* DCCF = 0:						  */
		/* Chrominance comb filter on				  */
		/*							  */
		/* FCTC = 0:						  */
		/* Nominal colour time constant				  */
		/*							  */
		/* CHBW1 CHBW0 = 0 1:					  */
		/* Chrominance nominal bandwidth 800 kHz		  */
		
      0x2a,	/* SA 0F: ACGC	CGAIN6CGAIN5CGAIN4CGAIN3CGAIN2CGAIN1CGAIN0*/
		/*							  */
		/* ACGC = 0:						  */
		/* Automatic chrominance gain control on		  */

      0x10,	/* SA 10: OFTS1 OFTS0 HDEL1 HDEL0 VRLN	YDEL2 YDEL1 YDEL0 */
		/*							  */
		/* OFTS1 OFTS0 = 0 0:					  */
		/* Output format selection: Standard ITU 656-format	  */
		/*							  */
		/* HDEL1 HDEL0 = 0 1:					  */
		/* Fine position of HS: 1 step(s) delay			  */
		/* !!! Departing from SAA7113H datasheet !!!		  */
		/*							  */
		/* VRLN = 0:						  */
		/*							  */
		/* YDEL2 YDEL1 YDEL0 = 0 0 0:				  */
		/* Luminance delay: 0 steps				  */
		
      0x0c,	/* SA 11: GPSW1 CM99  GPSW0 HLSEL OEYC	OERT  VIPB  COLO  */
      
      0x8b,	/* SA 12: RTSE13RTSE12RTSE11RTSE10RTSE03RTSE02RTSE01RTSE00*/
		/*							  */
		/* RTSE13 RTSE12 RTSE11 RTSE10 = 1 0 0 0:		  */
		/* RTS1 is HS output					  */
		/* !!! Departing from SAA7113h data sheet !!!		  */
		/*							  */
		/* RTSE03 RTSE02 RTSE01 RTSE00 = 1 0 1 1:		  */
		/* RTS0 is VS output					  */
		/* !!! Departing from SAA7113H data sheet !!!		  */
      
      0x00,	/* SA 13: ADLSB   X	X   OLDSB FIDP	  X   AOSL1 AOSL0 */
      
      0x00,	/* SA 14:   X	  X	X     X     X	  X	X     X   */
      
      0x00,	/* SA 15: VSTA7 VSTA6 VSTA5 VSTA4 VSTA3 VSTA2 VSTA1 VSTA0 */
      
      0x00,	/* SA 16: VSTO7 VSTO6 VSTO5 VSTO4 VSTO3 VSTO2 VSTO1 VSTO0 */
      
      0x00	/* SA 17:   X	  X	X     X     X	  X   VSTO8 VSTA8 */
	};
      
	const unsigned char init_from_40[] = {
      0x02,	/* SA 40: FISET HAM_N  FCE  HUNT_N  X  CLKSEL1CLKSEL0 X   */
      0xff,	/* SA 41: */
      0xff,	/* SA 42: */
      0xff,	/* SA 43: */
      0xff,	/* SA 44: */
      0xff,	/* SA 45: */
      0xff,	/* SA 46: */
      0xff,	/* SA 47: */
      0xff,	/* SA 48: */
      0xff,	/* SA 49: */
      0x66,	/* SA 4A: */
      0x66,	/* SA 4B: */
      0x66,	/* SA 4C: */
      0x66,	/* SA 4D: */
      0x66,	/* SA 4E: */
      0x66,	/* SA 4F: */
      0x66,	/* SA 50: */
      0x66,	/* SA 51: */
      0x66,	/* SA 52: */
      0x66,	/* SA 53: */
      0x66,	/* SA 54: */
      0xff,	/* SA 55: */
      0xff,	/* SA 56: */
      0xff,	/* SA 57: */
      0x00,	/* SA 58:  FC7	 FC6   FC5   FC4   FC3	 FC2   FC1   FC0  */
      0x54,	/* SA 59: HOFF7 HOFF6 HOFF5 HOFF4 HOFF3 HOFF2 HOFF1 HOFF0 */
      0x07,	/* SA 5A: VOFF7 VOFF6 VOFF5 VOFF4 VOFF3 VOFF2 VOFF1 VOFF0 */
      0x83,	/* SA 5B: FOFF	  X	X   VOFF8   X	HOFF10HOFF9 HOFF8 */
      0x00,	/* SA 5C:   X	  X	X     X     X	  X	X     X   */
      0x00,	/* SA 5D:   X	  X	X     X     X	  X	X     X   */
      0x00,	/* SA 5E:   X	  X   SDID5 SDID4 SDID3 SDID2 SDID1 SDID0 */
      0X00,	/* SA 5F:   X	  X	X     X     X	  X	X     X   */
	};

	DEB_EE(("I2C client '%s'.\n", decoder_client->name));

	for (i = 0; i < (sizeof (init_from_01) / sizeof (init_from_01[0])); i++)
		ret |= i2c_smbus_write_byte_data (decoder_client, 0x01 + i, init_from_01[i]);
	for (i = 0; i < (sizeof (init_from_40) / sizeof (init_from_40[0])); i++)
		ret |= i2c_smbus_write_byte_data (decoder_client, 0x40 + i, init_from_40[i]);

	memset (decoder, 0, sizeof (struct saa7113h));

	decoder->adapter = decoder_client->adapter;
	decoder->brightness = 32768;
	decoder->contrast = 36352;
	decoder->saturation = 32768;
	decoder->hue = 32768;
	decoder->norm = VIDEO_MODE_PAL;
	decoder->input = DECODER_INPUT_MODE0;

	if (ret != 0) {
		DEB_S(("I2C client '%s' failed.\n", decoder_client->name));
		return -EIO;
	}
	
	return 0;
}

/***************************************/

/* this function is called by i2c_probe */
static int saa7113h_detect (struct i2c_adapter *adap, int addr, unsigned short int flags, int kind)
{
	struct i2c_client *decoder_client;
	struct saa7113h *decoder;
	int ret;

	DEB_EE(("I2C adapter '%s'; addr=0x%02x)\n", adap->name, addr));

	/* let's see whether this adapter supports what we need */
	if (i2c_check_functionality (adap, I2C_FUNC_SMBUS_BYTE_DATA) == 0) {
		return 0;
	}

	/* allocate memory for client structure */
	decoder_client = kmalloc (sizeof (struct i2c_client), GFP_KERNEL);
	if (decoder_client == NULL) {
		return -ENOMEM;
	}

	/* fill client structure */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	sprintf (decoder_client->name, "Philips SAA7113H");
#else
	sprintf (decoder_client->dev.name, "Philips SAA7113H");
#endif
	decoder_client->id = saa7113h_id++;
	decoder_client->flags = 0;
	decoder_client->addr = addr;
	decoder_client->adapter = adap;
	decoder_client->driver = &saa7113h_driver;
	decoder_client->data = NULL;

	if ( saa7113h_id == 0) {
		DEB_S(("I2C client '%s'; No hardware detected.\n", decoder_client->name));
	} else {
		DEB_S(("I2C client '%s' detected at i2c address 0x%02x.\n", decoder_client->name, decoder_client->addr));
	}

	/* allocate memory for decoder structure */
	decoder_client->data = decoder = kmalloc (sizeof (struct saa7113h), GFP_KERNEL);
	if (decoder == NULL) {
		kfree (decoder_client);
		return -ENOMEM;
	}

	/* tell the i2c layer a new client has arrived */
	if ( (ret=i2c_attach_client (decoder_client)) != 0) {
		DEB_S(("I2C client registration failed, '%s' @ 0x%02x not attached.\n", decoder_client->name, decoder_client->addr));
		kfree (decoder_client->data);
		kfree (decoder_client);
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x attached to adapter '%s'.\n", decoder_client->name, decoder_client->addr, decoder_client->adapter->name));

	/* SAA7113H chip init */
	if ( (ret=saa7113h_chip_init (decoder_client)) != 0) {
		DEB_S(("Init of I2C client '%s' @ 0x%02x failed, continuing anyway.\n", decoder_client->name, decoder_client->addr));
		return ret;
	}

	return 0;
}


static int saa7113h_attach (struct i2c_adapter *adap)
{

	DEB_EE(("I2C adapter '%s'.\n", adap->name));

	return i2c_probe (adap, &addr_data, &saa7113h_detect);
}


static int saa7113h_detach (struct i2c_client *decoder_client)
{
	int ret;

	DEB_EE(("I2C client '%s'; addr=0x%02x; adap='%s')\n", decoder_client->name, decoder_client->addr, decoder_client->adapter->name));

	/* stop further output = clear OEYC bit */
	/* ret |= saa7113h_write (decoder, 0x11, saa7113h_read (decoder, 0x11) & 0xf7); */

	if ( (ret=i2c_detach_client (decoder_client)) != 0) {
		DEB_S(("I2C client deregistration failed, '%s' @ 0x%02x not detached.\n", decoder_client->name, decoder_client->addr));
		return ret;
	}

	DEB_S(("I2C Client '%s' @ 0x%02x detached from adapter '%s'.\n", decoder_client->name, decoder_client->addr, decoder_client->adapter->name));

	kfree (decoder_client->data);
	kfree (decoder_client);

	return 0;
}


static int saa7113h_command (struct i2c_client *decoder_client, unsigned int cmd, void *arg)
{
	struct saa7113h *decoder = (struct saa7113h *) decoder_client->data;
	int ret = 0;

	switch (cmd) {
		case DECODER_GET_CAPABILITIES: {
			struct video_decoder_capability *dc = arg;
	 
			DEB_EE(("I2C client '%s'; DECODER_GET_CAPABILITIES\n", decoder_client->name));

			dc->flags =		VIDEO_DECODER_PAL
					|	VIDEO_DECODER_NTSC
					|	VIDEO_DECODER_SECAM
/*					|	VIDEO_DECODER_VBI	*/
			;
			dc->inputs = SAA7113H_MAX_INPUTS;
			dc->outputs = SAA7113H_MAX_OUTPUTS;
			return ret;
		}
		
		case DECODER_GET_STATUS: {
			int status = 0;
			unsigned char oldsb;
			
			DEB_EE(("I2C client '%s'; DECODER_GET_STATUS\n", decoder_client->name));

			oldsb = i2c_smbus_read_byte_data (decoder_client, 0x13);
			/* set OLDSB = 1 */
			ret |= i2c_smbus_write_byte_data (decoder_client, 0x13, oldsb | 0x10);
			/* get colour status */
			if (i2c_smbus_read_byte_data (decoder_client, 0x1f) & 0x01)
				status |= DECODER_STATUS_COLOR;
			/* set OLDSB = 0 */
			ret |= i2c_smbus_write_byte_data (decoder_client, 0x13, oldsb & 0xef) << 1;
			/* get general status */
			if (i2c_smbus_read_byte_data (decoder_client, 0x1f) & 0x01)
				status |= DECODER_STATUS_GOOD;
	 
			switch (decoder->norm) {
				case VIDEO_MODE_PAL:
					status |= DECODER_STATUS_PAL;
					break;
				case VIDEO_MODE_NTSC:
					status |= DECODER_STATUS_NTSC;
					break;
				case VIDEO_MODE_SECAM:
					status |= DECODER_STATUS_SECAM;
					break;
				default:
					break;
			}
			
			*(int *) arg = status;
			
			return ret;
		}

		case DECODER_SET_NORM: {
			unsigned short int video_norm = *(unsigned short int *) arg;

			DEB_EE(("I2C client '%s'; DECODER_SET_NORM (norm=%d)\n", decoder_client->name, video_norm));

			return saa7113h_set_video_norm (decoder_client, video_norm);
		}
		
		case DECODER_SET_INPUT: {
			int video_input = *(int *) arg;

			DEB_EE(("I2C client '%s'; DECODER_SET_INPUT (mode=%d)\n", decoder_client->name, video_input));

			return saa7113h_set_video_input (decoder_client, video_input);
		}
		
		case DECODER_SET_OUTPUT:
		
			DEB_EE(("I2C client '%s'; DECODER_SET_OUTPUT (not supported)\n", decoder_client->name));

			return -EOPNOTSUPP;
	 
		case DECODER_ENABLE_OUTPUT:
		
			DEB_EE(("I2C client '%s'; DECODER_ENABLE_OUTPUT (not supported)\n", decoder_client->name));

			return -EOPNOTSUPP;

		case DECODER_GET_PICTURE: {
			struct video_picture *pic = arg;

			memset (pic, 0, sizeof (pic));
			
			pic->brightness = decoder->brightness;
			pic->contrast = decoder->contrast;
			pic->colour = decoder->saturation;
			pic->hue = decoder->hue;

			DEB_EE(("I2C client '%s'; DECODER_GET_PICTURE (bri=%d; con=%d; sat=%d; hue=%d)\n", decoder_client->name, pic->brightness, pic->contrast, pic->colour, pic->hue));

			return ret;
		}

		case DECODER_SET_PICTURE: {
			struct video_picture *pic = arg;

			DEB_EE(("I2C client '%s'; DECODER_SET_PICTURE (bri=%d; con=%d; sat=%d; hue=%d)\n", decoder_client->name, pic->brightness, pic->contrast, pic->colour, pic->hue));

			decoder->brightness = pic->brightness;
			decoder->contrast = pic->contrast;
			decoder->saturation = pic->colour;
			decoder->hue = pic->hue;
	 
			return saa7113h_set_video_attributes (decoder_client);
		}
		
		case DECODER_SET_VBI:
		
			DEB_EE(("I2C client '%s'; DECODER_SET_VBI\n", decoder_client->name));

			return -EOPNOTSUPP;

		case DECODER_SET_FUSE: {
			unsigned int fuse = *(unsigned int *) arg;
			
			DEB_EE(("I2C client '%s'; DECODER_SET_FUSE (fuse=0x%x)\n", decoder_client->name, fuse));

			return saa7113h_set_fuse (decoder_client, fuse);
		}

		case DECODER_SET_GAIN: {
			unsigned int gain = *(unsigned int *) arg;

			DEB_EE(("I2C client '%s'; DECODER_SET_GAIN (gain=%d)\n", decoder_client->name, gain));

			return ret;
		}

		case DECODER_DUMP:
		
			DEB_EE(("I2C client '%s'; DECODER_DUMP\n", decoder_client->name));

			return saa7113h_dump (decoder_client);

		default:
		
			DEB_EE(("I2C client '%s'; unknown ioctl cmd (0x%x)\n", decoder_client->name, cmd));

			return -ENOIOCTLCMD;
	}
}


void saa7113h_inc_use (struct i2c_client *decoder_client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


void saa7113h_dec_use (struct i2c_client *decoder_client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


static struct i2c_driver saa7113h_driver = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,54)
	.owner		= THIS_MODULE,
#endif
	.name		= "saa7113h.o",
	.id		= I2C_DRIVERID_SAA7113,
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= saa7113h_attach,
	.detach_client	= saa7113h_detach,
	.command	= saa7113h_command,
	.inc_use	= saa7113h_inc_use,
	.dec_use	= saa7113h_dec_use
};

/***************************************/

EXPORT_NO_SYMBOLS;

int __devinit saa7113h_init (void)
{
	int ret;

	if ( 0 != (ret=i2c_add_driver (&saa7113h_driver)) ) {
		DEB_S(("Driver '%s' registration failed, module not inserted.\n", saa7113h_driver.name));
		return ret;
	}

	return 0;
}


void __devexit saa7113h_cleanup (void)
{
	if ( 0 != i2c_del_driver (&saa7113h_driver) )
		DEB_S(("Driver '%s' deregistration failed, module not removed.\n", saa7113h_driver.name));
}

/***************************************/

MODULE_AUTHOR ("Torsten Tittmann <Torsten.Tittmann@gmx.de>");
MODULE_DESCRIPTION ("Philips SAA7113H I2C driver");
MODULE_LICENSE("GPL");

module_init(saa7113h_init);
module_exit(saa7113h_cleanup);


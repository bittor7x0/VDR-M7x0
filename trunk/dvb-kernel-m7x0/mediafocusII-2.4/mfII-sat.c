/* 
    mfII-sat.c - v4l2 driver for the TechniSat MediaFocus II.

    Version: $Id: mfII-sat.c,v 1.3 2004-02-22 14:51:35 ttittmann Exp $
    
    Copyright (C) 2003 Torsten Tittmann <Torsten.Tittmann@gmx.de>

    Visit http://www.mihu.de/linux/saa7146/
    for further details about this card.
	     
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
    Large pieces of code have been taken from the original 
    TechniSat MediaFocus II driver by Rolf Siebrecht.
	    
    Copyright (C) 2000,2001 Rolf Siebrecht
*/

#define DEBUG_VARIABLE mfII_debug

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ioctl.h>

#include <linux/i2c.h>
#include <linux/videodev2.h>
#include <linux/video_decoder.h>

#include <media/tuner.h>
#include "videodev_priv.h"
#include "video_decoder_priv.h"

#include "stv0056af.h"
#include "msp3400satII.h"
#include "drp3510II.h"
#include "saa7113h.h"
#include "ee24lc16.h"

#include "mfII.h"

/* Some initial settings of helper ICs */

#if 0
#define INITIAL_SAA7113_GAIN		0
#endif
#define INITIAL_SAA7113_BRIGHTNESS	32768
#define INITIAL_SAA7113_CONTRAST	32768
#define INITIAL_SAA7113_SATURATION	32768
#define INITIAL_SAA7113_HUE		32768

#define INITIAL_SAA7146_BRIGHTNESS	32768
#define INITIAL_SAA7146_CONTRAST	32768
#define INITIAL_SAA7146_SATURATION	32768
#define INITIAL_SAA7146_DEPTH		16
#define INITIAL_SAA7146_PALETTE		VIDEO_PALETTE_RGB565;

#define INITIAL_ORBIT_POSITION		0		/* 1st pos. = 19,2°E */
#define INITIAL_TUNER_FREQUENCY		1464000000UL	/* RTL2 */
#define INITIAL_LNB_POLARITY		MSP_DIGCTR1_ON	/* H-pol */
#define INITIAL_22KHZ_STATE		STV_22KHZ_OFF	/* no 22kHz signal */

static int diseqc = 0;
MODULE_PARM(diseqc,"i");
MODULE_PARM_DESC(diseqc, "enable diseqc control (default: 0)");

extern int mfII_debug;

/* ------------------------*/
/*	 DiSEqC stuff	   */
/* ------------------------*/

static int diseqc_sendbit (struct mf2 *mfII, int databit)
{
	int i, ret = 0;

	DEB_EE(("I2C_Adapter '%s', databit=%d.\n", mfII->i2c_adapter.name, databit));
	
	/* STV0056AF: 22 kHz tone on */
	i = STV_22KHZ_ON;
	if ( 0 != (ret|=mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_22KHZ, &i)) ) 
	   goto diseqc_sendbit_error;
	/* tone width according to data bit */
	udelay (databit ? 500 : 1000);

	/* STV0056AF: 22 kHz tone off */
	i = STV_22KHZ_OFF;
	if ( 0 != (ret|=mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_22KHZ, &i)) ) 
	   goto diseqc_sendbit_error;
	/* pause width according to data bit */
	udelay (databit ? 1000 : 500);

diseqc_sendbit_error:
	if (ret != 0) {
		ret = -1;
		DEB_S(("diseqc_sendbit(): I2C_Adapter '%s', can't send diseqc_sendbit.\n", mfII->i2c_adapter.name));
	}
	
	return ret;
}

static int diseqc_sendbyte (struct mf2 *mfII, __u8 databyte)
{
	int i, b, p = 1, ret = 0;

	DEB_EE(("I2C_Adapter '%s', databyte=%d.\n", mfII->i2c_adapter.name, databyte));

	for (i = 7; i >= 0; i--) {
		b = (databyte >> i) & 0x01;
		p ^= b;				/* compute parity bit */
		if ( 0 != (ret|=diseqc_sendbit (mfII, b)) )
		   goto diseqc_sendbyte_error;
	}
	ret |= diseqc_sendbit (mfII, p);	/* send parity bit */

diseqc_sendbyte_error:
	if (ret != 0) {
		ret = -1;
		DEB_S(("diseqc_sendbyte(): I2C_Adapter '%s', can't send DiSEqC byte.\n", mfII->i2c_adapter.name));
	}

	return ret;
}

static int diseqc_sendmsg (struct mf2 *mfII, __u8 frame, __u8 address, __u8 command, __u8 *data)
{
	int i, ret = 0;

	DEB_EE(("I2C_Adapter '%s', frame=0x%02x; address=0x%02x; command=0x%02x; data=0x%02x;  current_22kHz_state=%s)\n", mfII->i2c_adapter.name, frame, address, command, *data, (mfII->current_22khz_state) ? "on" : "off" ));

	/* turn continous 22 kHz signal off, if it is on yet */
	if (mfII->current_22khz_state) {
		i = STV_22KHZ_OFF;
		ret |= mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_22KHZ, &i);
		mdelay (15);
	}

	ret |= diseqc_sendbyte (mfII, frame);
	if (ret != 0) goto diseqc_sendmsg_error;
	ret |= diseqc_sendbyte (mfII, address);
	if (ret != 0) goto diseqc_sendmsg_error;
	ret |= diseqc_sendbyte (mfII, command);
	if (ret != 0) goto diseqc_sendmsg_error;
	if (data != NULL) ret |= diseqc_sendbyte (mfII, *data);	/* FIXME: caution, if number of data bytes is different from 1 ! */
	if (ret != 0) goto diseqc_sendmsg_error;

	mdelay (20);
	/* turn continous 22 kHz signal on, if necessary */
	if (mfII->current_22khz_state) {
		i = STV_22KHZ_ON;
		ret |= mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_22KHZ, &i);
	}

diseqc_sendmsg_error:
	if (ret != 0) {
		DEB_S(("diseqc_sendmsg(): I2C_Adapter '%s', Can't send DiSEqC message.\n", mfII->i2c_adapter.name));
		ret = -1;
	}

	return ret;
}

static int diseqc_reset (struct mf2 *mfII)
{
	DEB_S(("diseqc_reset(): I2C_Adapter '%s'\n", mfII->i2c_adapter.name));

	if (diseqc) {
		diseqc_sendmsg (mfII, 0xe0, 0x00, 0x00, NULL);
		diseqc_sendmsg (mfII, 0xe0, 0x00, 0x00, NULL);
	}
	return 0;
}

/* ==DiSEqC controlled action==			*/
/* Set orbit position, i.e. select a certain LNB*/
/* through DiSEqC Option switches plus DisEqC	*/
/* Position switches.				*/
/*						*/
/* 1st orbit position -> Option A/Position A	*/
/* 2nd	 "	"     -> Option A/Position B	*/
/* 3rd	 "	"     -> Option B/Position A	*/
/* 4th	 "	"     -> Option B/Position B	*/
/*						*/
/* <orbitpos> is a bitmask:			*/
/* no bit set -> 1st orbit position		*/
/* bit 0 set  -> 2nd orbit position		*/
/* ...						*/
/* Only one bit of <orbitpos> at a time may be	*/
/* asserted.					*/
static int set_orbit_position (struct mf2 *mfII, int orbitpos)
{
	DEB_EE(("I2C_Adapter '%s', orbitpos(mask)=0x%02x)\n", mfII->i2c_adapter.name, orbitpos));

	if (diseqc) {
		switch (orbitpos & 0x07) {
		case 0:	/* first orbit position */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x23, NULL);	/* select Option A */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x22, NULL);	/* select Position A */
			break;
		case 1:	/* second orbit position */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x23, NULL);	/* select Option A */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x26, NULL);	/* select Position B */
			break;
		case 2:	/* third orbit position */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x27, NULL);	/* select Option B */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x22, NULL);	/* select Position A */
			break;
		case 4:	/* fourth orbit position */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x27, NULL);	/* select Option B */
			diseqc_sendmsg (mfII, 0xe0, 0x10, 0x26, NULL);	/* select Position B */
			break;
		default:
			break;
		} /* switch (orbitpos) */
	} /* if (diseqc) */
	return 0;
}

/* ------------------------------*/
/* basic module / init functions */
/* ------------------------------*/

/* initialize the helper ics to useful values */
int init_helper_ics (struct mf2 *mfII)
{
	int arg, ret = 0;
	unsigned long int larg;
	struct video_picture vp;

	DEB_EE(("I2C adapter '%s'.\n", mfII->i2c_adapter.name));

	/* enable LNB voltage */
	arg = MSP_DIGCTR0_ON;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_DIGCTR0, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable LNB voltage failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable LNB voltage.\n", mfII->i2c_adapter.name));
	}

	/* initialize DiSEqC settings */
	diseqc_reset (mfII);
	set_orbit_position (mfII, INITIAL_ORBIT_POSITION);
	mfII->current_orbit_position = INITIAL_ORBIT_POSITION;

	/* initialize LNB polarity */
	arg = INITIAL_LNB_POLARITY;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_DIGCTR1, &arg)) ) {
		DEB_S(("I2C adapter '%s': init LNB polarity failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': init LNB polarity.\n", mfII->i2c_adapter.name));
	}

	/* initialize tuner frequency */
	larg = INITIAL_TUNER_FREQUENCY;
	if ( 0 != (ret=mfII->sp5055->driver->command (mfII->sp5055, TUNER_SET_TVFREQ, &larg)) ) {
		DEB_S(("I2C adapter '%s': init tuner frequency failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': init tuner frequency.\n", mfII->i2c_adapter.name));
	}
	mfII->current_freq = larg / 1000000 * 16;

	/* initialize 22 kHz signal state */
	arg = INITIAL_22KHZ_STATE;
	if ( 0 != (ret=mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_22KHZ, &arg)) ) {
		DEB_S(("I2C adapter '%s': init 22 kHz signal failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': init 22 kHz signal.\n", mfII->i2c_adapter.name));
	}
	if (arg == STV_22KHZ_ON)
		mfII->current_22khz_state = 1;
	else
		mfII->current_22khz_state = 0;

#if 0	/* this setting now initialized by stv0056af module parameter */
	/* was: set video gain = 0 dB in STV0056AF */
	arg = 0;
	ret = mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_GAIN, &arg);
	if (ret != 0) {
		return ret;
	}
#endif

	/* enable video for SAA7113H, coming from STV0056AF */
	arg = STV_NORMAL_VIDEO;
	if ( 0 != (ret=mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_SCART1, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable video for saa7113h coming from stv0056af failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable video for saa7113h coming from stv0056af.\n", mfII->i2c_adapter.name));
	}

#if 1	/* this setting now initialized by stv0056af module parameter */	
	/* was: enable video for Mini DIN jack, coming from STV0056AF */
	arg = STV_BASEBAND;
	if ( 0 != (ret=mfII->stv0056af->driver->command (mfII->stv0056af, STV_SET_DECODER, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable video for Mini DIN jack coming from stv0056af failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable video for Mini DIN jack coming from stv0056af voltage.\n", mfII->i2c_adapter.name));
	}
#endif

	/* select video input in SAA7113H */
	arg = DECODER_INPUT_MODE0;
	if ( 0 != (ret=mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_SET_INPUT, &arg)) ) {
		DEB_S(("I2C adapter '%s': select video input in saa7113h failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': select video input in saa7113h.\n", mfII->i2c_adapter.name));
	}
	mfII->current_input = 0;
	
	/* select video processing in SAA7113H */
	arg = 0;
	arg |= DECODER_USE_ANTIALIAS;
	arg |= DECODER_USE_AMPLIFIER; 
	if ( 0 != (ret=mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_SET_FUSE, &arg)) ) {
		DEB_S(("I2C adapter '%s': select video processing in saa7113h failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': select video processing in saa7113h.\n", mfII->i2c_adapter.name));
	}

#if 0	     
	/* set video gain in SAA7113H */
	arg = INITIAL_SAA7113_GAIN;
	if ( 0 != (ret=mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_SET_GAIN, &arg)) ) {
		DEB_S(("I2C adapter '%s': set video gain in saa7113h failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set video gain in saa7113h.\n", mfII->i2c_adapter.name));
	}
#endif
	
	/* select video norm in SAA7113H */
	/* FIXME!! use v4l2 constants, not v4l constants */
	arg = VIDEO_MODE_PAL;
	if ( 0 != (ret=mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_SET_NORM, &arg)) ) {
		DEB_S(("I2C adapter '%s': select video norm in saa7113h failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': select video norm in saa7113h.\n", mfII->i2c_adapter.name));
	}
	mfII->current_video_norm = VIDEO_MODE_PAL;

	/* initialize picture properties in SAA7113H */
	memset (&vp, 0, sizeof (vp));
	vp.brightness = INITIAL_SAA7113_BRIGHTNESS;
	vp.contrast = INITIAL_SAA7113_CONTRAST;
	vp.colour = INITIAL_SAA7113_SATURATION;
	vp.hue = INITIAL_SAA7113_HUE;
	if ( 0 != (ret=mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_SET_PICTURE, &vp)) ) {
		DEB_S(("I2C adapter '%s': init of picture properties in saa7113h failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': init of picture properties in saa7113h.\n", mfII->i2c_adapter.name));
	}

	/* mfII->current_overlay_pict.hue = vp.hue; */

	/* initialize picture properties in SAA7146AH */
	memset (&vp, 0, sizeof (vp));
	vp.brightness = INITIAL_SAA7146_BRIGHTNESS;
	vp.contrast = INITIAL_SAA7146_CONTRAST;
	vp.colour = INITIAL_SAA7146_SATURATION;
	vp.depth = INITIAL_SAA7146_DEPTH;
	vp.palette = INITIAL_SAA7146_PALETTE;

	/* FIXME!! initial bcs settings */
/*
	mfII->spci->command (mfII->saa7146, SAA7146_V4L_SPICT, &vp);
*/
/*
	mfII->current_overlay_pict.brightness = vp.brightness;
	mfII->current_overlay_pict.contrast = vp.contrast;
	mfII->current_overlay_pict.colour = vp.colour;
	mfII->current_overlay_pict.depth = vp.depth;
	mfII->current_overlay_pict.palette = vp.palette;
*/
       
#if 0	     
	/* enable audio FM demodulation input on sound processor chip */
	arg = MSP_ANA1_TO_DEM;
	if ( != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SLCT_DEM_SRC, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable audio FM demodulation failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable audio FM demodulation.\n", mfII->i2c_adapter.name));
	}
#endif
	/* enable audio going to Mini DIN jack */
	arg = MSP_DFP_TO_SC1;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SLCT_SC1_SRC, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable audio outout to Mini DIN jack failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable audio outout to Mini DIN jack.\n", mfII->i2c_adapter.name));
	}
	
	/* enable audio coming from Mini DIN jack */
	arg = MSP_SC1_TO_DFP;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SLCT_DFPANALOG_SRC, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable audio input from Mini DIN failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable audio input from Mini DIN.\n", mfII->i2c_adapter.name));
	}
	
	/* enable audio going to (Internal+External) Audio-Out */
	arg = MSP_DEM_SRC;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SLCT_LSP_SRC, &arg)) ) {
		DEB_S(("I2C adapter '%s': enable audio output to Internal/External Audio-out failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable audio output to Internal/External Audio-out. \n", mfII->i2c_adapter.name));
	}
	
	/* set audio volume level */
	/* arg = 65535; */	/* 100% volume */
	arg = 52428;	/* 80% volume */
	/* don't set realy the volume, just initialized the variable! */
	mfII->current_volume = arg;
	/*
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_VOLUME, &arg)) ) {
		DEB_S(("I2C adapter '%s': set audio volume failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set audio volume.\n", mfII->i2c_adapter.name));
	}
	*/
	
	/* mute audio */
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SWITCH_MUTE, NULL)) ) {
		DEB_S(("I2C adapter '%s': mute audio failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': mute audio.\n", mfII->i2c_adapter.name));
	}
	mfII->current_audio_state = MFII_AUDIO_MUTED;

	/* set loudness off  !!! reciprocal to volume level !!! */
	arg = 65535;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_LOUDNESS, &arg)) ) {
		DEB_S(("I2C adapter '%s': set loudness off failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set loudness off.\n", mfII->i2c_adapter.name));
	}
	mfII->current_loudness_state = MFII_LOUDNESS_OFF;

	
	/* set audio balance */
	arg = 32768;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_BALANCE, &arg)) ) {
		DEB_S(("I2C adapter '%s': set audio balance failed.\n",mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set audio balance.\n", mfII->i2c_adapter.name));
	}
		       
	/* set audio bass level to +-0 */
	arg = 32768;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_BASS, &arg)) ) {
		DEB_S(("I2C adapter '%s': set audio bass level failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set audio bass level.\n", mfII->i2c_adapter.name));
	}
	mfII->current_bass = arg;
	
	/* set audio treble level to +-0 */
	arg = 32768;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_TREBLE, &arg)) ) {
		DEB_S(("I2C adapter '%s': set audio treble level failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set audio treble level.\n", mfII->i2c_adapter.name));
	}
	mfII->current_treble = arg;
	
	/* FIXME??!!?? */
	mfII->current_balance = 32768;

	/* set audio mode */
	arg = V4L2_TUNER_MODE_STEREO;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_STEREO, &arg)) ) {
		DEB_S(("I2C adapter '%s': set audio mode failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': set audio mode.\n", mfII->i2c_adapter.name));
	}
	mfII->current_audio_mode = arg;
	mfII->current_left_carrier = 7020;
	mfII->current_right_carrier = 7200;

	/* enable ADR mode in ADR chip */
	if ( 0 != (ret=mfII->drp3510a->driver->command (mfII->drp3510a, DRP_ADR, NULL)) ) {
		DEB_S(("I2C adapter '%s': enable ADR mode in ADR chip failed.\n", mfII->i2c_adapter.name));
		return ret;
	} else {
		DEB_S(("I2C adapter '%s': enable ADR mode in ADR chip.\n", mfII->i2c_adapter.name));
	}

	return 0;
} /* init_helper_ics() */

int cleanup_helper_ics (struct mf2 *mfII)
{
	int arg, ret = 0;

	DEB_S(("cleanup_helper_ics(): I2C_Adapter '%s'\n", mfII->i2c_adapter.name));

	/* enable pass thru of Internal Audio-In to (Internal + External) Audio-Out */
	arg = MSP_SC2_TO_DFP;
	ret |= mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SLCT_DFPANALOG_SRC, &arg);
	arg = MSP_SCART_SRC;
	ret |= mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SLCT_LSP_SRC, &arg);
	arg = 52428;	/* 80% volume */
	ret |= mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_VOLUME, &arg);
	arg = 32768;	/* +-0% bass */
	ret |= mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_BASS, &arg);
	arg = 32768;	/* +-0% treble */
	if ( 0 != (ret |= mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_TREBLE, &arg)) )
		DEB_S(("cleanup_helper_ics(): I2C_Adapter '%s', Can't address sound decoder chip to enable pass thru of internal audio input. Continuing anyway.\n", mfII->i2c_adapter.name));
	
	/* turn off LNB voltage */
	arg = MSP_DIGCTR0_OFF;
	if ( 0 != (ret=mfII->msp3400c->driver->command (mfII->msp3400c, MSP_SET_DIGCTR0, &arg)) )
		DEB_S(("cleanup_helper_ics(): I2C_Adapter '%s', Can't address sound decoder chip to turn off LNB voltage. Continuing anyway.\n", mfII->i2c_adapter.name));
	
	return 0;
} /* cleanup_helper_ics() */


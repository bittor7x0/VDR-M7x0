/* 
    mfII.h - v4l2 driver for the TechniSat MediaFocus II.

    Version: $Id: mfII.h,v 1.3 2004-02-22 14:51:35 ttittmann Exp $
    
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


#ifndef __MFII_H__
#define __MFII_H__

#include <media/saa7146_vv.h>

struct mf2
{
	struct video_device	video_dev;
	struct video_device	vbi_dev;
/*
	struct video_device	radio_dev;
*/
	struct i2c_adapter	i2c_adapter;	

	struct i2c_client	*saa7113h;
	struct i2c_client	*sp5055;
	struct i2c_client	*stv0056af;
	struct i2c_client	*msp3400c;
	struct i2c_client	*drp3510a;
	struct i2c_client	*eeprom;

	int	in_use;
	int	has_eeprom;

	u16	current_input;		/* channel (= video source) number */
	u16	current_video_norm;	/* PAL/SECAM/NTSC */
	u16	current_overlay_state;	/* != 0: overlay capturing is running */
	u16	current_tuner;		/* tuner number */
	u32	current_freq;		/* transponder frequency */
	u16	current_22khz_state;	/* != 0: 22 kHz signal is on */
	u16	current_orbit_position; /* bit mask according to our private V4L extension */
	u16	current_audio_channel;	/* audio channel number */
	u16	current_audio_mode;	/* mono/stereo/.. */
	u16	current_audio_state;	/* mute/unmute */
	u16	current_loudness_state;	/* audio loudness on/off */
	u16	current_volume;
	u16	current_bass;
	u16	current_treble;
	u16	current_balance;
	u16	current_left_carrier;
	u16	current_right_carrier;
};

/* states of current_audio_state */
#define MFII_AUDIO_MUTED	1
#define MFII_AUDIO_NOT_MUTED 	0

#define MFII_LOUDNESS_ON	1
#define MFII_LOUDNESS_OFF	0

#endif


/*
    mfII-core.c - v4l2 driver for the TechniSat MediaFocus II

    Version: $Id: mfII-core.c,v 1.8 2004-04-06 13:24:52 hunold Exp $
    
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

#include <media/saa7146_vv.h>
#include "mfII.h"

#include <linux/video_decoder.h>
#include <media/tuner.h>
#include "videodev_priv.h"
#include "video_decoder_priv.h"

#include "stv0056af.h"
#include "msp3400satII.h"
#include "drp3510II.h"
#include "saa7113h.h"
#include "ee24lc16.h"


/* global variable */
static int mf2_num = 0;

static int mfII_debug = 0;
MODULE_PARM(mfII_debug,"i");
MODULE_PARM_DESC(mfII_debug, "debug level (default: 0)");

#define MFII_INPUTS		4
static struct v4l2_input mf2_inputs[MFII_INPUTS] = {
	{ 0, "SatTuner",	V4L2_INPUT_TYPE_TUNER,	1, 1, V4L2_STD_PAL | V4L2_STD_NTSC | V4L2_STD_SECAM, 0 }, 
	{ 1, "SatTuner+Decoder",V4L2_INPUT_TYPE_TUNER,	1, 1, V4L2_STD_PAL | V4L2_STD_NTSC | V4L2_STD_SECAM, 0 }, 
	{ 2, "Composite-Video", V4L2_INPUT_TYPE_CAMERA, 1, 0, V4L2_STD_PAL | V4L2_STD_NTSC | V4L2_STD_SECAM, 0 }, 
	{ 3, "SVHS-Video",	V4L2_INPUT_TYPE_CAMERA, 1, 0, V4L2_STD_PAL | V4L2_STD_NTSC | V4L2_STD_SECAM, 0 }, 
};
enum{ MFII_INPUT_SATTUNER, MFII_INPUT_SATTUNER_DECODER, MFII_INPUT_COMPOSITE_VIDEO, MFII_INPUT_SVHS_VIDEO};

static struct v4l2_audio mf2_audios[MFII_INPUTS] = {
	{ 0, "SatTuner-FM",	V4L2_AUDCAP_STEREO, 0},
	{ 1, "SatTuner-AM",	V4L2_AUDCAP_STEREO, 0},
	{ 2, "Mini-DIN",	V4L2_AUDCAP_STEREO, 0},
	{ 3, "Internal-Audio",	V4L2_AUDCAP_STEREO, 0},
};

#define MFII_TUNERS		1
static struct v4l2_tuner mf2_tuners[MFII_TUNERS] = {
	{ 0,
	  "Satellite",
	  V4L2_TUNER_ANALOG_TV,
	  V4L2_TUNER_CAP_NORM | V4L2_TUNER_CAP_STEREO | V4L2_TUNER_CAP_LANG1 | V4L2_TUNER_CAP_LANG2 | V4L2_TUNER_CAP_SAP,
	  15200,
	  34400,
	  0,
	  V4L2_TUNER_MODE_MONO | V4L2_TUNER_MODE_STEREO | V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2 | V4L2_TUNER_MODE_SAP,
	  0,
	  0,
	},
};

/* this array holds the information, which port of the saa7146 each
   input actually uses. the mf2 uses port 0 for every input */
static struct {
	int hps_source;
	int hps_sync;
} input_port_selection[MFII_INPUTS] = {		
	{ SAA7146_HPS_SOURCE_PORT_A, SAA7146_HPS_SYNC_PORT_A },
	{ SAA7146_HPS_SOURCE_PORT_A, SAA7146_HPS_SYNC_PORT_A },
	{ SAA7146_HPS_SOURCE_PORT_A, SAA7146_HPS_SYNC_PORT_A },
	{ SAA7146_HPS_SOURCE_PORT_A, SAA7146_HPS_SYNC_PORT_A },
};

/* this array holds the information of the audio source (mf2_audios),
   which has to be switched corresponding to the video source (mf2_channels) */
static int video_audio_connect[MFII_INPUTS] =
	{ 0, 0, 2, 2};


static struct v4l2_queryctrl mf2_controls[] = {
	{ V4L2_CID_AUDIO_MUTE,	 	V4L2_CTRL_TYPE_BOOLEAN, "Mute"  , 	0, 1    , 1, 	0    , 0},	/* default: off */
	{ V4L2_CID_AUDIO_LOUDNESS,	V4L2_CTRL_TYPE_BOOLEAN, "Loudness", 	0, 1    , 1, 	0    , 0},	/* default: off */
	{ V4L2_CID_AUDIO_VOLUME, 	V4L2_CTRL_TYPE_INTEGER, "Volume", 	0, 65535, 255,	52428, 0},	/* default: 80% volume */
	{ V4L2_CID_AUDIO_BALANCE,	V4L2_CTRL_TYPE_INTEGER, "Balance", 	0, 65535, 255,	32767, 0},	/* default: +/- 0 */
	{ V4L2_CID_AUDIO_BASS,		V4L2_CTRL_TYPE_INTEGER, "Bass"  , 	0, 65535, 255,	32767, 0},	/* default: +/- 0 */
	{ V4L2_CID_AUDIO_TREBLE, 	V4L2_CTRL_TYPE_INTEGER, "Treble", 	0, 65535, 255,	32767, 0},	/* default: +/- 0 */
};

#define MAXCONTROLS	( sizeof (mf2_controls) / sizeof (mf2_controls[0]) )

static struct saa7146_extension_ioctls ioctls[] = {
	{ VIDIOC_ENUMINPUT,	SAA7146_EXCLUSIVE },
	{ VIDIOC_G_INPUT,	SAA7146_EXCLUSIVE },
	{ VIDIOC_S_INPUT,	SAA7146_EXCLUSIVE },
	{ VIDIOC_QUERYCTRL,	SAA7146_BEFORE },
	{ VIDIOC_G_CTRL,	SAA7146_BEFORE },
	{ VIDIOC_S_CTRL,	SAA7146_BEFORE },
/* FIXME: remove when videodev2.h update is in kernel */
#ifdef VIDIOC_S_CTRL_OLD
	{ VIDIOC_S_CTRL_OLD,	SAA7146_BEFORE },
#endif
	{ VIDIOC_G_TUNER,	SAA7146_EXCLUSIVE },
	{ VIDIOC_S_TUNER,	SAA7146_EXCLUSIVE },
	{ VIDIOC_G_FREQUENCY,	SAA7146_EXCLUSIVE },
	{ VIDIOC_S_FREQUENCY,	SAA7146_EXCLUSIVE },
	{ VIDIOC_G_AUDIO,	SAA7146_EXCLUSIVE },
	{ VIDIOC_S_AUDIO,	SAA7146_EXCLUSIVE },
	{ 0,			0 }
};

static struct saa7146_extension extension;

static int mf2_probe(struct saa7146_dev* dev)
{
	struct mf2* mf2 = 0;
	int i = 0;	

	DEB_EE(("Device '%s'.\n", dev->name));
	
	request_module("drp3510II");
	request_module("ee24lc16");
	request_module("msp3400satII");
	request_module("saa7113h");
	request_module("sp5055II");
	request_module("stv0056af");

	if ( (mf2=(struct mf2*)kmalloc(sizeof(struct mf2), GFP_KERNEL)) == NULL) {;
		ERR(("not enough kernel memory.\n"));
		return -ENOMEM;
	}
	memset(mf2, 0x0, sizeof(struct mf2));	

	/* FIXME: enable i2c-port pins, video-port-pins
	   video port pins should be enabled here ?! */
	saa7146_write(dev, MC1, (MASK_08 | MASK_24 | MASK_10 | MASK_26));

	saa7146_i2c_adapter_prepare(dev, &mf2->i2c_adapter, 0, SAA7146_I2C_BUS_BIT_RATE_480);
	if(0 > i2c_add_adapter(&mf2->i2c_adapter) ) {
		ERR(("cannot register i2c-device. skipping.\n"));
		kfree(mf2);
		return -EFAULT;
	}

	/* loop through all i2c-devices on the bus and look who is there */
	for(i = 0; i < I2C_CLIENT_MAX; i++) {
		if( NULL == mf2->i2c_adapter.clients[i] ) {
			continue;
		}

		if( I2C_DRIVERID_SP5055  == mf2->i2c_adapter.clients[i]->driver->id )
			mf2->sp5055 = mf2->i2c_adapter.clients[i];
		if( I2C_DRIVERID_STV0056 == mf2->i2c_adapter.clients[i]->driver->id ) 
			mf2->stv0056af = mf2->i2c_adapter.clients[i];
		if( I2C_DRIVERID_SAA7113 == mf2->i2c_adapter.clients[i]->driver->id ) 
			mf2->saa7113h = mf2->i2c_adapter.clients[i];
		if( I2C_DRIVERID_MSP3400 == mf2->i2c_adapter.clients[i]->driver->id ) 
			mf2->msp3400c = mf2->i2c_adapter.clients[i];
		if( I2C_DRIVERID_DRP3510 == mf2->i2c_adapter.clients[i]->driver->id ) 
			mf2->drp3510a = mf2->i2c_adapter.clients[i];
		if( I2C_DRIVERID_AT24Cxx == mf2->i2c_adapter.clients[i]->driver->id ) {
			mf2->eeprom = mf2->i2c_adapter.clients[i];
			mf2->has_eeprom = 1;
		}
	}

	/* check if all devices are present */
	if(    0 == mf2->sp5055		|| 0 == mf2->stv0056af	|| 0 == mf2->saa7113h
	    || 0 == mf2->msp3400c	|| 0 == mf2->drp3510a ) {

		ERR(("did not find all i2c devices\n"));
		i2c_del_adapter(&mf2->i2c_adapter);
		kfree(mf2);
		return -ENODEV;
	}

	/* all devices are present, probe was successful */	

	/* we store the pointer in our private data field */
	(struct mf2*)dev->ext_priv = mf2;

	return 0;
}

int init_helper_ics (struct mf2 *mfII);

/* bring hardware to a sane state. this has to be done, just in case someone
   wants to capture from this device before it has been properly initialized.
   the capture engine would badly fail, because no valid signal arrives on the
   saa7146, thus leading to timeouts and stuff. */
static int mf2_init_done(struct saa7146_dev* dev)
{
	struct mf2* mf2 = (struct mf2*)dev->ext_priv;

	DEB_EE(("Device '%s'.\n", dev->name));
	
	if (0 != init_helper_ics(mf2)) {
		DEB_D(("Device '%s': init_helper_ics() failed!\n", dev->name));
		return -EINVAL;
	} else {
		DEB_D(("Device '%s': init_helper_ics() successful!\n", dev->name));
	}

	DEB_D(("Device '%s': mf2->current_input=%d!\n", dev->name,mf2->current_input));

	/* some stuff is done via variables */
	saa7146_set_hps_source_and_sync(dev, input_port_selection[mf2->current_input].hps_source, input_port_selection[mf2->current_input].hps_sync);

	/* some stuff is done via direct write to the registers */

	/* this is ugly, but because of the fact that this is completely
	   hardware dependend, it should be done directly... */

	/* fixme: needs to be changed probably */
	saa7146_write(dev, DD1_STREAM_B,	0x00000000);
	saa7146_write(dev, DD1_INIT,		0x07000000);
	saa7146_write(dev, MC2, (MASK_09 | MASK_25 | MASK_10 | MASK_26));

	return 0;
}

/* interrupt-handler. this gets called when irq_mask is != 0.
   it must clear the interrupt-bits in irq_mask it has handled */
/*
void mf2_irq_bh(struct saa7146_dev* dev, u32* irq_mask)
{
	struct mf2* mf2 = (struct mf2*)dev->ext_priv;
}
*/

static struct saa7146_ext_vv vv_data;

/* this function only gets called when the probing was successful */
static int mf2_attach(struct saa7146_dev* dev, struct saa7146_pci_extension_data *info)
{
	struct mf2* mf2 = (struct mf2*)dev->ext_priv;
	
	DEB_EE(("Device '%s', Extension '%s'.\n", dev->name, (char*) info->ext_priv));

	/* checking for i2c-devices can be omitted here, because we
	   already did this in "mf2_vl42_probe" */

	saa7146_vv_init(dev,&vv_data);
	if( 0 != saa7146_register_device(&mf2->video_dev, dev, "mf2", VFL_TYPE_GRABBER)) {
		ERR(("cannot register capture v4l2 device. skipping.\n"));
		return -1;
	}
	
#if 0
	/* initialization stuff (vbi) (only for revision > 0 and for extensions which want it)*/
	if( 0 != MFII_BOARD_CAN_DO_VBI(dev)) {
		if( 0 != saa7146_register_device(&mf2->vbi_dev, dev, "mf2", VFL_TYPE_VBI)) {
			ERR(("cannot register vbi v4l2 device. skipping.\n"));
		}
	}
#endif
	
	i2c_use_client(mf2->eeprom);
	i2c_use_client(mf2->drp3510a);
	i2c_use_client(mf2->msp3400c);
	i2c_use_client(mf2->saa7113h);
	i2c_use_client(mf2->stv0056af);
	i2c_use_client(mf2->sp5055);

	INFO(("found 'MediaFocusII-%d'.\n",mf2_num));

	mf2_num++;
	mf2_init_done(dev);
	return 0;
}

static int mf2_detach(struct saa7146_dev* dev)
{
	struct mf2* mf2 = (struct mf2*)dev->ext_priv;

	DEB_EE(("Device '%s'.\n", dev->name));

	i2c_release_client(mf2->eeprom);
	i2c_release_client(mf2->drp3510a);
	i2c_release_client(mf2->msp3400c);
	i2c_release_client(mf2->saa7113h);
	i2c_release_client(mf2->stv0056af);
	i2c_release_client(mf2->sp5055);

	saa7146_unregister_device(&mf2->video_dev,dev);
#if 0
	if( 0 != MFII_BOARD_CAN_DO_VBI(dev)) {
		saa7146_unregister_device(&mf2->vbi_dev,dev);
	}
#endif
	saa7146_vv_release(dev);

	mf2_num--;

	i2c_del_adapter(&mf2->i2c_adapter);
	kfree(mf2);

	return 0;
}

int set_orbit_position(struct mf2 *mfII, int orbitpos);

static int mf2_ioctl(struct saa7146_fh *fh, unsigned int cmd, void *arg) 
{
	struct saa7146_dev *dev = fh->dev;
	struct mf2* mf2 = (struct mf2*)dev->ext_priv;
	/* struct saa7146_vv *vv = dev->vv_data; */
	int ret;

	
	switch(cmd) {
	case VIDIOC_ENUMINPUT: {
		struct v4l2_input *i = arg;
		
		DEB_EE(("Device '%s', VIDIOC_ENUMINPUT: %d.\n", dev->name, i->index));
		if( i->index < 0 || i->index >= MFII_INPUTS) {
			return -EINVAL;
		}
		memcpy(i, &mf2_inputs[i->index], sizeof(struct v4l2_input));

		return 0;
	}
	/* the saa7146 provides some controls (brightness, contrast, saturation)
	   which gets registered *after* this function. because of this we have
	   to return with a value != 0 even if the function succeded.. */
	case VIDIOC_QUERYCTRL: {
		struct v4l2_queryctrl *qc = arg;
		int i;

		for (i = MAXCONTROLS - 1; i >= 0; i--) {
			if (mf2_controls[i].id == qc->id) {
				*qc = mf2_controls[i];
				/*DEB_EE(("Device '%s', VIDIOC_QUERYCTRL: 0x%04x.\n",dev->name, (qc->id)-V4L2_CID_BASE ));*/
				DEB_EE(("Device '%s', VIDIOC_QUERYCTRL: 0x%04x.\n",dev->name, qc->id ));
				return 0;
			}
		}
		return -EAGAIN;
	}
	case VIDIOC_G_CTRL: {
		struct v4l2_control *vc = arg;
		int i;

		for (i = MAXCONTROLS - 1; i >= 0; i--) {
			if (mf2_controls[i].id == vc->id) {
				break;
			}
		}
		
		if( i < 0 ) {
			return -EAGAIN;
		}
			
		switch (vc->id ) {
			case V4L2_CID_AUDIO_VOLUME: {
				vc->value = mf2->current_volume;
				DEB_EE(("Device '%s', VIDIOC_G_CTRL, V4L2_CID_AUDIO_VOLUME: volume %05d.\n", dev->name, vc->value));

				break;
			}
			case V4L2_CID_AUDIO_LOUDNESS: {
				vc->value = mf2->current_loudness_state;
				DEB_EE(("Device '%s', VIDIOC_G_CTRL, V4L2_CID_AUDIO_LOUDNESS: loudness %s.\n", dev->name, (vc->value) ? "on" : "off"));
				break;
			}
			case V4L2_CID_AUDIO_BALANCE: {
				vc->value = mf2->current_balance;
				DEB_EE(("Device '%s', VIDIOC_G_CTRL, V4L2_CID_AUDIO_BALANCE: balance %05d.\n", dev->name, vc->value));
				break;
			}
			case V4L2_CID_AUDIO_BASS: {
				vc->value = mf2->current_bass;
				DEB_EE(("Device '%s', VIDIOC_G_CTRL, V4L2_CID_AUDIO_BASS: bass %05d.\n",dev->name, vc->value));
				break;
			}
			case V4L2_CID_AUDIO_TREBLE: {
				vc->value = mf2->current_treble;
				DEB_EE(("Device '%s', VIDIOC_G_CTRL, V4L2_CID_AUDIO_TREBLE: treble %05d.\n",dev->name, vc->value));
				break;
			}
			case V4L2_CID_AUDIO_MUTE: {
				vc->value = mf2->current_audio_state;
				DEB_EE(("Device '%s', VIDIOC_G_CTRL, V4L2_CID_AUDIO_MUTE: %smuted.\n",dev->name, (vc->value) ? "" :  "not " ));
				break;
			}
		}
		
		return 0;
	}
	case VIDIOC_S_CTRL:
/* FIXME: remove when videodev2.h update is in kernel */
#ifdef VIDIOC_S_CTRL_OLD
	case VIDIOC_S_CTRL_OLD:
#endif
		{
		struct	v4l2_control	*vc = arg;
		int i = 0;
		
		for (i = MAXCONTROLS - 1; i >= 0; i--) {
			if (mf2_controls[i].id == vc->id) {
				break;
			}
		}
		
		if( i < 0 ) {
			return -EAGAIN;
		}
		
		switch (vc->id ) {
			case V4L2_CID_AUDIO_VOLUME: {
				mf2->current_volume   = vc->value;
				
				DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_VOLUME: volume %05d\n", dev->name,mf2->current_volume));
				
				if ( 0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SET_VOLUME,&mf2->current_volume)) ) {
				   DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_VOLUME: Can't address sound decoder chip to set volume level!\n", dev->name));
				   return ret;
				}
				
				if ( MFII_LOUDNESS_ON == mf2->current_loudness_state ) {
					DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_VOLUME: set loudness level.\n", dev->name));
					
					if (0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SET_LOUDNESS, &mf2->current_volume)) ) {
				   		DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_VOLUME: Can't address sound decoder chip to set loudness level!\n", dev->name));
						return ret;
					}
				}

				if ( 0 == mf2->current_volume )
				   mf2->current_audio_state = MFII_AUDIO_MUTED;
				else
				   mf2->current_audio_state = MFII_AUDIO_NOT_MUTED;

				break;
			}
			case V4L2_CID_AUDIO_LOUDNESS: {
				int iarg;
				mf2->current_loudness_state = vc->value;
			
				DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_LOUDNESS: loudness %s.\n", dev->name, (mf2->current_loudness_state) ? "on" : "off"));
				if ( MFII_LOUDNESS_ON == mf2->current_loudness_state ) {
					if ( 0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SET_LOUDNESS, &mf2->current_volume)) ) {
				   		DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_LOUDNESS: Can't address sound decoder chip to set loudness state!\n", dev->name));
						return ret;
					}
				} else {
					iarg = 65535;	/* == loudness off */
					if ( 0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SET_LOUDNESS, &iarg)) ) {
				   		DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_LOUDNESS: Can't address sound decoder chip to set loudness state!\n", dev->name));
						return ret;
					}
				}
					
				
				break;
			}
			case V4L2_CID_AUDIO_BALANCE: {
				mf2->current_balance = vc->value;
				DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_BALANCE: balance %05d\n", dev->name,mf2->current_balance));

				if ( 0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SET_BALANCE,&mf2->current_balance)) ) {
				   DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_BALANCE: Can't address sound decoder chip to set balancel!\n", dev->name));
				   return ret;
				}

				break;
		     }
			case V4L2_CID_AUDIO_BASS: {
				mf2->current_bass = vc->value;
				
				DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_BASS: bass %05d\n", dev->name,mf2->current_bass));
				
				if (0 != (ret = mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_BASS, &mf2->current_bass))) {
				   DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_BASS: Can't address sound decoder chip to set bass level.\n", dev->name));
				   return ret;
				}
				break;
			}
			case V4L2_CID_AUDIO_TREBLE: {
				mf2->current_treble = vc->value;
				
				DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_TREBLE: treble %05d\n", dev->name,mf2->current_treble));
				
				if (0 != (ret = mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_TREBLE, &mf2->current_treble))) {
				   DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_TREBLE: Can't address sound decoder chip to set treble level.\n", dev->name));
				   return ret;
				}
				break;
			}
			case V4L2_CID_AUDIO_MUTE: {
				mf2->current_audio_state = vc->value;
				
				DEB_EE(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_MUTE: %smuted.\n", dev->name,(mf2->current_audio_state) ? "" : "not " ));

				if ( vc->value == MFII_AUDIO_MUTED) {
				   if ( 0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SWITCH_MUTE, NULL)) ) {
				      DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_MUTE: Can't address sound decoder chip to mute audio.\n", dev->name));
				      return ret;
				   }
				} else {
				   if ( 0 != (ret=mf2->msp3400c->driver->command(mf2->msp3400c, MSP_SET_VOLUME,&mf2->current_volume)) ) {
				      DEB_S(("Device '%s', VIDIOC_S_CTRL, V4L2_CID_AUDIO_MUTE: Can't address sound decoder chip to unmute audio!\n", dev->name));
				      return ret;
				   }
				}
				break;
			}
		}
		return 0;
	}
	case VIDIOC_G_INPUT: {
		int *input = (int *)arg;

		DEB_EE(("Device '%s', VIDIOC_G_INPUT %d.\n", dev->name,*input));

		if( *input < 0 || *input >= MFII_INPUTS) {
			DEB_S(("Device '%s', VIDIOC_G_INPUT: invalid video input #%d.\n", dev->name, *input));
			return -EINVAL;
		}
		
		/* get current video input */
		*input = mf2->current_input;
	
		return 0;		
	}	
	case VIDIOC_S_INPUT: {
		int input = *(int *)arg;
		int stv_arg, decoder_arg, msp_lsp_arg, msp_scart_arg;

		DEB_EE(("Device '%s', VIDIOC_S_INPUT %d.\n", dev->name,input));

		if (input < 0 || input >= MFII_INPUTS) {
			DEB_S(("Device '%s', VIDIOC_G_INPUT: invalid video input #%d.\n", dev->name, input));
			return -EINVAL;
		}
		
		/* keep current video input in mind */
		mf2->current_input = input;
		saa7146_set_hps_source_and_sync(dev, input_port_selection[input].hps_source, input_port_selection[input].hps_sync);

		/* fixme */		

		/* defaults: */
		stv_arg = STV_NORMAL_VIDEO;	/* stv0056af sends clamped video from tuner to saa7113h */
		decoder_arg = DECODER_INPUT_MODE0;	/* saa7113h receives CVBS video from stv0056af */
		msp_lsp_arg = MSP_DEM_SRC;	/* FM demodulator is source of MSP3400 "loudspeaker" output */
		msp_scart_arg = MSP_DEM_SRC;	/* FM demodulator is source of MSP3400 internal "SCART" output */
			
		switch (input) {
			/* Sat tuner */
			case MFII_INPUT_SATTUNER:
				
				{
					break;				
				}
				
			/* Sat tuner with external crypt decoder */
			case MFII_INPUT_SATTUNER_DECODER:
				{
					stv_arg = STV_DECODER_RET;	/* stv0056af sends video from Mini-DIN to saa7113h */
					break;
				}
				
			/* Composite Video from Mini-DIN plug */
			case MFII_INPUT_COMPOSITE_VIDEO:
				{
					stv_arg = STV_NOTHING;		/* stv0056af sends nothing to saa7113h */
					decoder_arg = DECODER_INPUT_MODE2;	/* saa7113h receives CVBS video from Mini-DIN */
					msp_lsp_arg = MSP_SCART_SRC;	/* loudspeaker source is SCART (MSP internal input) */
					msp_scart_arg = MSP_NONE_SRC;	/* SCART (MSP internal output) disabled */
					break;
				}
			
			/* S-VHS Video from Mini-DIN plug */
			case MFII_INPUT_SVHS_VIDEO:
				{
					stv_arg = STV_NOTHING;		/* stv0056af sends nothing to saa7113h */
					decoder_arg = DECODER_INPUT_MODE9;	/* saa7113h receives SVHS video from Mini-DIN */
					msp_lsp_arg = MSP_SCART_SRC;	/* loudspeaker source is (MSP-internal) SCART */
					msp_scart_arg = MSP_NONE_SRC;	/* SCART (MSP internal output) disabled */
					break;
				}
		}

		/* select input for STV0056AF S1 output */
		if ( 0 != (ret=mf2->stv0056af->driver->command (mf2->stv0056af, STV_SET_SCART1, &stv_arg)) ) {
			DEB_S(("Device '%s', VIDIOC_S_INPUT: Can't address STV0056AF to select S1 video source.\n", dev->name));
			return ret;
		}

		/* select input mode for SAA7113H */
		if ( 0 != (ret=mf2->saa7113h->driver->command (mf2->saa7113h, DECODER_SET_INPUT, &decoder_arg)) ) {
			DEB_S(("Device '%s', VIDIOC_S_INPUT: Can't address SAA7113H to select input mode.\n", dev->name));
			return ret;
		}
		
		/* select source for MSP3400C loudspeaker output */
		if ( 0 != (ret=mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SLCT_LSP_SRC, &msp_lsp_arg)) ){
			DEB_S(("Device '%s', VIDIOC_S_INPUT: Can't address MSP3400C to select loudspeaker source.\n", dev->name));
			return ret;
		}
		/* mediafocusII doesn't use headphone output */
		
		/* select source for (internal) MSP3400C SCART output */
		if ( 0 != (ret=mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SLCT_SCART_SRC, &msp_scart_arg)) ) {
			DEB_S(("Device '%s', VIDIOC_S_INPUT: Can't address MSP3400C to select SCART source.\n", dev->name));
			return ret;
		}
		
		return 0;
	}
	case VIDIOC_G_TUNER: {
		struct v4l2_tuner *t = arg;

		DEB_EE(("Device '%s', VIDIOC_G_TUNER: %d\n", dev->name, t->index));

		if( (t->index < 0) || (t->index >= MFII_TUNERS) ) {
			DEB_S(("Device '%s', VIDIOC_G_TUNER: channel %d does not have a tuner attached.\n", dev->name, t->index));
			return -EINVAL;
		}

		memcpy(t, &mf2_tuners[t->index], sizeof(struct v4l2_tuner));
		
		if ( 0 != (ret=mf2->sp5055->driver->command(mf2->sp5055, TUNER_GET_SIGNAL, &t->signal)) ) {
		   DEB_S(("Device '%s', VIDIOC_G_TUNER: Can't address tuner chip to get signal strenght.\n", dev->name));
		   return ret;
		}
		t->afc = 0;		
		/* FIXME!! retrieve current audmode from tuner */
		t->audmode = mf2->current_audio_mode;
		t->rxsubchans = V4L2_TUNER_SUB_STEREO | V4L2_TUNER_SUB_MONO;

		/* get current tuner */
		t->index = mf2->current_tuner;
		
		return 0;
	}
	case VIDIOC_S_TUNER: {
		struct v4l2_tuner *t = arg;
		int iarg;
		
		if ( (t->index < 0) || (t->index >= MFII_TUNERS) ) {
			DEB_S(("Device '%s', VIDIOC_S_TUNER: channel %d does not have a tuner attached.\n", dev->name,t->index));
			return -EINVAL;
		}
		/* keep current tuner in mind */
		mf2->current_tuner = t->index;
		
		switch(t->audmode) {
			case V4L2_TUNER_MODE_STEREO: {
				mf2->current_audio_mode = V4L2_TUNER_MODE_STEREO;
				DEB_EE(("Device '%s', VIDIOC_S_TUNER: V4L2_TUNER_MODE_STEREO\n", dev->name));
				mf2->current_left_carrier = 7020;
				mf2->current_right_carrier = 7200;
				break;
			}
			case V4L2_TUNER_MODE_LANG1: {
				mf2->current_audio_mode = V4L2_TUNER_MODE_LANG1;
				DEB_EE(("Device '%s', VIDIOC_S_TUNER: V4L2_TUNER_MODE_LANG1\n", dev->name));
				mf2->current_left_carrier = 7380;
				mf2->current_right_carrier = 7560;
				break;
			}
			case V4L2_TUNER_MODE_LANG2: {
				mf2->current_audio_mode = V4L2_TUNER_MODE_LANG2;
				DEB_EE(("Device '%s', VIDIOC_S_TUNER: V4L2_TUNER_MODE_LANG2\n", dev->name));
				mf2->current_left_carrier = 7380;
				mf2->current_right_carrier = 7560;
				break;
			}
			default: { /* case V4L2_TUNER_MODE_MONO: {*/
				mf2->current_audio_mode = V4L2_TUNER_MODE_MONO;
				DEB_EE(("Device '%s', VIDIOC_S_TUNER: V4L2_TUNER_MODE_MONO\n", dev->name));
				mf2->current_left_carrier = 6500;
				mf2->current_right_carrier = 6500;
				break;
			}
		}

		/* switch audiomode in msp3400c */
		if ( 0 != (ret=mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_STEREO, &t->audmode)) ) {
			DEB_S(("Device '%s', VIDIOC_S_TUNER: Can't address MSP3400C to select audio mode.\n", dev->name));
			return -EINVAL;
		}

		/* special treatment of irregular audio carrier frequencies */
		if ( (t->audmode == V4L2_TUNER_MODE_MONO) && ( 0 != (mf2->current_freq & (MONO580 | MONO660 | MONO665)) ) ) {
			if (mf2->current_freq & MONO580) iarg = 5800;
			if (mf2->current_freq & MONO660) iarg = 6600;
			if (mf2->current_freq & MONO665) iarg = 6650;
			if (0 != (ret = mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_LEFT_FREQ, &iarg))) {
				DEB_S(("Device '%s', VIDIOC_S_TUNER: Can't address sound decoder chip to set special audio carrier frequency.\n", dev->name));
				return ret;
			}
			/* keep current left audio carrier in mind */
			mf2->current_left_carrier = iarg;
			
			iarg = mf2->current_left_carrier + 180;
			if (0 != (ret = mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_RIGHT_FREQ, &iarg))) {
				DEB_S(("Device '%s', VIDIOC_S_TUNER: Can't address sound decoder chip to set special audio carrier frequency.\n", dev->name));
				return ret;
			}
			/* keep current right audio carrier in mind */
			mf2->current_right_carrier = iarg;
		}

		return 0;
	}
	case VIDIOC_G_FREQUENCY: {
		struct v4l2_frequency *f = arg;

		if( (f->tuner < 0) && (f->tuner >= MFII_TUNERS) ) {
			DEB_S(("Device '%s', VIDIOC_G_FREQUENCY: channel %d does not have a tuner!\n", dev->name,mf2->current_input));
			return -EINVAL;
		}

		DEB_EE(("Device '%s', VIDIOC_G_FREQUENCY: frequency 0x%0x.\n", dev->name, mf2->current_freq));
		
		/* memset(f,0,sizeof(*f)); */
		/* f->type = V4L2_TUNER_ANALOG_TV; */
		f->frequency =	mf2->current_freq;

		return 0;
	}
	case VIDIOC_S_FREQUENCY: {
		struct v4l2_frequency *f = arg;
		int iarg, orbitpos;

		if ( (f->tuner < 0) && (f->tuner >= MFII_TUNERS) ) {
			DEB_S(("Device '%s', VIDIOC_S_FREQUENCY: channel %d does not have a tuner!\n", dev->name,mf2->current_input));
			return -EINVAL;
		}

		if (V4L2_TUNER_ANALOG_TV != f->type) {
			DEB_S(("Device '%s', VIDIOC_S_FREQUENCY: channel %d does not have a analog TV tuner!\n", dev->name,mf2->current_input));
			return -EINVAL;
		}
		
		DEB_EE(("Device '%s', VIDIOC_S_FREQUENCY: frequency 0x%0x.\n", dev->name,f->frequency));

		if ( ((f->frequency & 0xffff) < mf2_tuners[mf2->current_tuner].rangelow) || ((f->frequency > 0xffff) > mf2_tuners[mf2->current_tuner].rangehigh) ) {
			DEB_S(("Device '%s', VIDIOC_S_FREQUENCY: requested frequency 0x%0x not valid.\n", dev->name, f->frequency));
			return -EINVAL;
		}
		
		/* mute audio to avoid strange tuning noises */ 
		if (mf2->current_audio_state == MFII_AUDIO_NOT_MUTED) {
		   mf2->msp3400c->driver->command (mf2->msp3400c, MSP_GET_VOLUME, &mf2->current_volume);
		   mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SWITCH_MUTE, NULL);
		}

		/* make "orbit" bits right-aligned */
		orbitpos = f->frequency;
		iarg = POS2ND;
		while (!(iarg & 1)) {
			orbitpos >>= 1;
			iarg >>=1;
		}
		orbitpos &= 0x0f;
		/* select orbit position if necessary */
		if (mf2->current_orbit_position != orbitpos) {
			set_orbit_position (mf2, orbitpos);
			mf2->current_orbit_position = orbitpos;
		}

		/* tune in desired frequency */
		mf2->current_freq = ((f->frequency & 0x0000ffff) >> 4) * 1000000;	/* mask off control flags and convert to Hz */
		if ( 0 != (ret=mf2->sp5055->driver->command (mf2->sp5055, TUNER_SET_TVFREQ, &mf2->current_freq)) ) {
		   DEB_S(("Device '%s', VIDIC_S_FREQUENCY: Can't address tuner chip to set frequency.\n", dev->name));
		   return ret;
		}

		/* switch MSP3400C pin "DCTR1" according to LNB polarity */
		if (f->frequency & VPOL) {
		   iarg = MSP_DIGCTR1_OFF; /* polarity = V -> LNB voltage = 14 V */
		} else {
		   iarg = MSP_DIGCTR1_ON;  /* polarity = H -> LNB voltage = 18 V */
		}
		
		if ( 0 != (ret=mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_DIGCTR1, &iarg)) ) {
		   DEB_S(("Device '%s', VIDIOC_S_FREQUENCY: Can't address MSP3400C to select LNB polarity.\n", dev->name));
		   return ret;
																							}
	       
		/* switch STV0056AF pin "IO" according to 22 kHz signal state */
		if (f->frequency & HIBAND) {
		   iarg = STV_22KHZ_ON;	/* 22 kHz tone enabled */
		   mf2->current_22khz_state = 1;
		} else {
		   iarg = STV_22KHZ_OFF;	/* 22 kHz tone disabled */
		   mf2->current_22khz_state = 0;
		}
		if ( 0 != (ret=mf2->stv0056af->driver->command (mf2->stv0056af, STV_SET_22KHZ, &iarg)) ) {
		   DEB_S(("Device '%s', VIDIOC_S_FREQUENCY: Can't address STV0056AF to select 22 kHz state.\n", dev->name));
		   return ret;
		}

		/* restore audio if it should not be muted */
		if (mf2->current_audio_state == MFII_AUDIO_NOT_MUTED) {
		   mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SET_VOLUME, &mf2->current_volume);
		}

		/* keep current tuner frequency in mind */
		mf2->current_freq = f-> frequency;

		return 0;
	}
	case VIDIOC_G_AUDIO: {
		struct v4l2_audio *a = arg;

		if( a->index < 0 || a->index > MFII_INPUTS ) {
			DEB_S(("Device '%s', VIDIOC_G_AUDIO: %d out of range.\n", dev->name,a->index));
			return -EINVAL;
		}
		
		DEB_EE(("Device '%s', VIDIOC_G_AUDIO: %d.\n", dev->name,a->index));
		memcpy(a, &mf2_audios[video_audio_connect[mf2->current_input]], sizeof(struct v4l2_audio));

		if (mf2->current_audio_state == MFII_AUDIO_MUTED) {
		   mf2->msp3400c->driver->command (mf2->msp3400c, MSP_SWITCH_MUTE, NULL);
		}
	
		return 0;
	}
	case VIDIOC_S_AUDIO: {
		struct v4l2_audio *a = arg;
/*
		if( a->index < 0 || a->index > MFII_INPUTS ) {
			DEB_S(("Device '%s', VIDIOC_S_AUDIO: %d out of range.\n", dev->name,a->index));
			return -EINVAL;
		}
*/		
		DEB_EE(("Device '%s', VIDIOC_S_AUDIO: %d.\n", dev->name,a->index));
/*
		mf2->current_audio_channel = a->index;
*/		
		return 0;
	}	
	default:

		DEB_EE(("Device '%s', does not handle this ioctl #%d.\n", dev->name,cmd));
		return -ENOIOCTLCMD;
	}
	return 0;
}

static int std_callback(struct saa7146_dev* dev, struct saa7146_standard *std)
{
	struct mf2* mf2 = (struct mf2*)dev->ext_priv;
	int ret;

	/* FIXME!! use of v4l2 constants even in the i2c modules! */
	
	if ( std->id == V4L2_STD_PAL) {
		DEB_EE(("Device '%s', VIDIOC_S_STD, V4L2_STD_PAL: video norm %llu selected.\n", dev->name, (v4l2_std_id)std->id));
		mf2->current_video_norm = VIDEO_MODE_PAL;
	} else if ( std->id == V4L2_STD_NTSC ) {
		DEB_EE(("Device '%s', VIDIOC_S_STD, V4L2_STD_NTSC: video norm %llu selected.\n", dev->name, (v4l2_std_id)std->id));
		mf2->current_video_norm = VIDEO_MODE_NTSC;
	} else if ( std->id == V4L2_STD_SECAM) {
		DEB_EE(("Device '%s', VIDIOC_S_STD, V4L2_STD_SECAM: video norm %llu selected.\n", dev->name, (v4l2_std_id)std->id));
		mf2->current_video_norm = VIDEO_MODE_SECAM;
	} else {
		DEB_EE(("Device '%s', VIDIOC_S_STD: video norm %llu isn't supported.\n", dev->name, (long long)std->id));
		return -EINVAL;
	}
	
	DEB_EE(("Device '%s', VIDIOC_S_STD, v4l2 video norm translated to v4l1 norm %d.\n", dev->name, mf2->current_video_norm));
	
	/* FIXME!! switch norm in STV0056AF */
			
	/* switch norm in SAA7113H */
	if ( 0 != (ret=mf2->saa7113h->driver->command (mf2->saa7113h, DECODER_SET_NORM, &mf2->current_video_norm)) ) {
		DEB_S(("Device '%s', VIDIOC_S_STD, VIDEO_MODE_*: Can't address SAA7113H to select video norm %d.\n", dev->name, mf2->current_video_norm));
		return ret;
	}

	/* FIXME!! switch norm in SAA7146AH */
	/*
	if ( 0 != (ret=mf2->spci->command (mf2->saa7146, SAA7146_V4L_SNORM, &std->id)) ) {
		DEB_S(("Device '%s', VIDIOC_S_STD: SAA7146 driver rejects selection of video norm.\n", dev->name));
		return ret;
	}
	*/
	
	return 0;
}

static struct saa7146_standard standard[] = {
	{
		.name	= "PAL",	.id	= V4L2_STD_PAL,
		.v_offset	= 0x19,	.v_field	= 288,
		.h_offset	= 0x14,	.h_pixels	= 680,
		.v_max_out	= 576,	.h_max_out	= 768,
	}, {
		.name	= "NTSC",	.id	= V4L2_STD_NTSC,
		.v_offset	= 0x16,	.v_field	= 240,
		.h_offset	= 0x06,	.h_pixels	= 708,
		.v_max_out	= 480,	.h_max_out	= 640,
	}, {
		.name	= "SECAM",	.id	= V4L2_STD_SECAM,
		.v_offset	= 0x14,	.v_field	= 288,
		.h_offset	= 0x14,	.h_pixels	= 720,
		.v_max_out	= 576,	.h_max_out	= 768,
	}
};		

static struct saa7146_pci_extension_data mf2 = {
	.ext_priv = "Technisat MediafocusII",
	.ext = &extension,
};

static struct pci_device_id pci_tbl[] = {
	{
		.vendor    = PCI_VENDOR_ID_PHILIPS,
		.device	   = PCI_DEVICE_ID_PHILIPS_SAA7146,
		.subvendor = 0xffff,
		.subdevice = 0xffff,
		.driver_data = (unsigned long)&mf2,
	}, {
		.vendor	= 0,
	}
};

MODULE_DEVICE_TABLE(pci, pci_tbl);

static struct saa7146_ext_vv vv_data = {
	.inputs		= MFII_INPUTS,
	.capabilities	= V4L2_CAP_TUNER,
	.stds		= &standard[0],
	.num_stds	= sizeof(standard)/sizeof(struct saa7146_standard),
	.std_callback	= &std_callback, 
	.ioctls		= &ioctls[0],
	.ioctl		= mf2_ioctl,
};

static struct saa7146_extension extension = {
	.name		= "Technisat MediaFocusII",
	.flags		= 0, /* SAA7146_USE_I2C_IRQ, */
	
	.pci_tbl	= &pci_tbl[0],
	.module		= THIS_MODULE,

	.probe		= mf2_probe,
	.attach		= mf2_attach,
	.detach		= mf2_detach,

	.irq_mask	= 0,
	.irq_func	= NULL,
};	

int __init mf2_init_module(void) 
{
	int ret = 0;

	DEB_EE(("'%s'.\n", extension.name));
	
	if ( (ret=saa7146_register_extension(&extension)) != 0) {
		DEB_S(("failed to register extension.\n"));
	}
	
	return ret;
}

void __exit mf2_cleanup_module(void) 
{
	DEB_EE(("'%s'.\n", extension.name));
	saa7146_unregister_extension(&extension);
}

module_init(mf2_init_module);
module_exit(mf2_cleanup_module);

MODULE_DESCRIPTION("video4linux-2 driver for the TechniSat MediaFocusII");
MODULE_AUTHOR("Torsten Tittmann <torsten.tittmann@gmx.de>");
MODULE_LICENSE("GPL");

#if 0

/* ------------------------*/
/* mfII - ioctl - routines */
/* and supporting funtions */
/* ------------------------*/
		

		/* Set image properties */
		case VIDIOCSPICT: /* => hmm, hue evtl auch übernehmen?*/
		{
			struct video_picture vp;
			__u16 hue;

			if (copy_from_user (&vp, (struct video_picture *) arg, sizeof (vp)))
				return -EFAULT;

dprintk (KERN_INFO "mfII: ==> VIDIOCSPICT (bri=%d; con=%d; sat=%d; hue=%d; dep=%d; pal=%d)\n", vp.brightness, vp.contrast, vp.colour, vp.hue, vp.depth, vp.palette);

			hue = vp.hue;
			/* set "brightness", "contrast" + "saturation" only in SAA7146AH */
			if (0 != (ret = mfII->spci->command (mfII->saa7146, SAA7146_V4L_SPICT, &vp))) {
				printk (KERN_WARNING "mfII: VIDIOCSPICT: Error during SAA7146_V4L_SPICT ioctl call.\n");
				return ret;
			}
			/* get those values back which are really accepted by SAA7146AH... */
			mfII->spci->command (mfII->saa7146, SAA7146_V4L_GPICT, &vp);
			/* ...and keep them in mind */
			memcpy (&(mfII->current_overlay_pict), &vp, sizeof (struct video_picture));
			
			/* set "hue" only in SAA7113H */
			ret |= mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_GET_PICTURE, &vp);	/* get existing values */
			vp.hue = hue;	/* update "hue" */
			ret |= mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_SET_PICTURE, &vp);	/* set values */
			ret |= mfII->saa7113h->driver->command (mfII->saa7113h, DECODER_GET_PICTURE, &vp);	/* get _actual_ "hue" value */
			if (ret != 0) {
				printk (KERN_ERR "mfII: VIDIOCSPICT: Can't address SAA7113H to set picture properties.\n");
				return ret;
			}
			/* keep _actual_ "hue" value in mind */
			mfII->current_overlay_pict.hue = vp.hue;
			
			return 0;
		}


		case VIDIOCGEEPROM:
		{
			struct eeprom_s ee;

			if (!mfII->has_eeprom)
				return -ENODEV;
			if (copy_from_user (&ee, (struct eeprom_s *) arg, sizeof (ee)))
				return -EFAULT;
			if ((ee.addr >= 256) || (ee.count != 0))
				return -EINVAL;
			if (0 != (ret = mfII->eeprom->driver->command (mfII->eeprom, GET_EEPROM_DATA, &ee))) {
				printk (KERN_ERR "mfII: VIDIOCGEEPROM: Can't address EEPROM chip to read data.\n");
				return ret;
			}

dprintk (KERN_INFO "mfII: ==> VIDIOCGEEPROM\n");

			if (copy_to_user ((struct eeprom_s *) arg, &ee, sizeof (ee)))
				return -EFAULT;

			return 0;
		}


		case VIDIOCSEEPROM:
		{
			struct eeprom_s ee;

			if (!mfII->has_eeprom)
				return -ENODEV;
			if (copy_from_user (&ee, (struct eeprom_s *) arg, sizeof (ee)))
				return -EFAULT;

dprintk (KERN_INFO "mfII: ==> VIDIOCSEEPROM\n");

			if ((ee.addr >= 256) || (ee.count != 0))
				return -EINVAL;
			if (0 != (ret = mfII->eeprom->driver->command (mfII->eeprom, SET_EEPROM_DATA, & ee))) {
				printk (KERN_ERR "mfII: VIDIOCSEEPROM: Can't address EEPROM chip to write data.\n");
				return ret;
			}
			if (copy_to_user ((struct eeprom_s *) arg, &ee, sizeof (ee)))
				return -EFAULT;

			return 0;
		}

	return 0;
}

#endif

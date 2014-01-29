/*
    cm7326.c - v4l2 driver for pc104+ framegrabber module
    http://www.rtdusa.com/PC104/UM/video/cm7326.htm
    
    Copyright (C) 2002 Miguel Freitas
    Copyright (C) 2003 Michael Hunold <michael@mihu.de>

    Visit http://www.mihu.de/linux/saa7146
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

#define DEBUG_VARIABLE debug
#include <media/saa7146_vv.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,51)
	#define KBUILD_MODNAME cm7326
#endif

/* module parameters */
static int debug = 0;
MODULE_PARM(debug,"i");
MODULE_PARM_DESC(debug, "debug verbosity");

/* global variables */
static int cm7326_num = 0;

/* we simply use the two video-interfaces (aka port a and port b) */
#define CM7326_INPUTS	6
static struct v4l2_input cm7326_inputs[CM7326_INPUTS] =
{
	{ 0, "VID1",	V4L2_INPUT_TYPE_CAMERA,	2, 0, V4L2_STD_PAL_BG|V4L2_STD_NTSC_M, 0 },
	{ 1, "VID2",	V4L2_INPUT_TYPE_CAMERA,	2, 0, V4L2_STD_PAL_BG|V4L2_STD_NTSC_M, 0 },
	{ 2, "VID3",	V4L2_INPUT_TYPE_CAMERA,	2, 0, V4L2_STD_PAL_BG|V4L2_STD_NTSC_M, 0 },
	{ 3, "VID4",	V4L2_INPUT_TYPE_CAMERA,	2, 0, V4L2_STD_PAL_BG|V4L2_STD_NTSC_M, 0 },
	{ 4, "VID5",	V4L2_INPUT_TYPE_CAMERA,	2, 0, V4L2_STD_PAL_BG|V4L2_STD_NTSC_M, 0 },
	{ 5, "VID6",	V4L2_INPUT_TYPE_CAMERA,	2, 0, V4L2_STD_PAL_BG|V4L2_STD_NTSC_M, 0 },
};

#define CM7326_AUDIOS	0

/* you can demand that your extension get's called for any v4l2-ioctl. here,
   we want to be called exclusively when the ioctls VIDIOC_S_INPUT and
   VIDIOC_ENUMINPUT get called by an user application */
static struct saa7146_extension_ioctls ioctls[] =
{
	{ VIDIOC_ENUMINPUT, 	SAA7146_EXCLUSIVE },
	{ VIDIOC_G_INPUT,	SAA7146_EXCLUSIVE },
	{ VIDIOC_S_INPUT,	SAA7146_EXCLUSIVE },
	{ VIDIOC_S_STD,		SAA7146_AFTER },
	{ 0,			0 }
};

struct cm7326
{
	struct video_device	video_dev;
	struct video_device	vbi_dev;

	struct i2c_adapter	i2c_adapter;	

	int cur_input;	/* current input */
};

static unsigned char saa7110_initseq[] = {
	0x4C, 0x3C, 0x0D, 0xEF, 0xBD, 0xF0, 0x00, 0x00,
	0xF8, 0xF8, 0x60, 0x60, 0x00, 0x06, 0x18, 0x90,
	0x00, 0x2C, 0x40, 0x46, 0x42, 0x1A, 0xFF, 0xDA,
	0xF0, 0x8B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xD9, 0x17, 0x40, 0x41, 0x80, 0x41, 0x80, 0x4F,
	0xFE, 0x01, 0xCF, 0x0F, 0x03, 0x01, 0x81, 0x03,
	0x40, 0x75, 0x01, 0x8C, 0x03
};

static int saa7110_init(struct cm7326 *cm7326)
{
	union i2c_smbus_data data;
	int ret = 0;
	int i = 0;
	
	for (i = 0; i < sizeof(saa7110_initseq); i++) {
		data.byte = saa7110_initseq[i];
		if (0 != i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, i, I2C_SMBUS_BYTE_DATA, &data)) {
			DEB_D(("failed for address 0x%02x\n", i));
			return -EFAULT;
		}
	}

	data.byte = 0x16;
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x21, I2C_SMBUS_BYTE_DATA, &data);

	/* fixme: unclear why there are two writes to register 0x0d here ... */
	data.byte = 0x04;
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x0D, I2C_SMBUS_BYTE_DATA, &data);
	data.byte = 0x06;
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x0D, I2C_SMBUS_BYTE_DATA, &data);
	
	if (0 != ret) {
		DEB_S(("writing to saa7110 failed.\n"));	
		return -EFAULT;
	}

	return 0;
}

static const unsigned char input_modes[9][8] = {
/* mode 0 */	{ 0x00, 0xD9, 0x17, 0x40, 0x03, 0x44, 0x75, 0x16 },
/* mode 1 */	{ 0x00, 0xD8, 0x17, 0x40, 0x03, 0x44, 0x75, 0x16 },
/* mode 2 */	{ 0x00, 0xBA, 0x07, 0x91, 0x03, 0x60, 0xB5, 0x05 },
/* mode 3 */	{ 0x00, 0xB8, 0x07, 0x91, 0x03, 0x60, 0xB5, 0x05 },
/* mode 4 */	{ 0x00, 0x7C, 0x07, 0xD2, 0x83, 0x60, 0xB5, 0x03 },
/* mode 5 */	{ 0x00, 0x78, 0x07, 0xD2, 0x83, 0x60, 0xB5, 0x03 },
/* mode 6 */	{ 0x80, 0x59, 0x17, 0x42, 0xA3, 0x44, 0x75, 0x12 },
/* mode 7 */	{ 0x80, 0x9A, 0x17, 0xB1, 0x13, 0x60, 0xB5, 0x14 },
/* mode 8 */	{ 0x80, 0x3C, 0x27, 0xC1, 0x23, 0x44, 0x75, 0x21 }
};

static int saa7110_selmux(struct cm7326 *cm7326, int chan)
{
	const unsigned char* ptr = input_modes[chan];
	union i2c_smbus_data data;
	int ret = 0;
	
	data.byte = ptr[0]; /* Luminance control */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x06, I2C_SMBUS_BYTE_DATA, &data);

	data.byte = ptr[1]; /* Analog Control #1 */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x20, I2C_SMBUS_BYTE_DATA, &data);

	data.byte = ptr[2]; /* Analog Control #2 */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x21, I2C_SMBUS_BYTE_DATA, &data);

	data.byte = ptr[3]; /* Mixer Control #1 */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x22, I2C_SMBUS_BYTE_DATA, &data);

	data.byte = ptr[4]; /* Mixer Control #2 */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x2c, I2C_SMBUS_BYTE_DATA, &data);

	data.byte = ptr[5]; /* ADCs gain control */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x30, I2C_SMBUS_BYTE_DATA, &data);

	data.byte = ptr[6]; /* Mixer Control #3 */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x31, I2C_SMBUS_BYTE_DATA, &data);

	/* fixme: unclear why there are two writes analog control #2 above and here */
	data.byte = ptr[7]; /* Analog Control #2 */
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x21, I2C_SMBUS_BYTE_DATA, &data);

	if (0 != ret) {
		DEB_S(("writing to saa7110 failed.\n"));	
		return -EFAULT;
	}

	return 0;
}

static int cm7326_probe(struct saa7146_dev *dev)
{
	struct cm7326 *cm7326 = NULL;	
	union i2c_smbus_data data;
	int err = 0;

	DEB_D(("cm7326_probe called.\n"));

	cm7326 = (struct cm7326*)kmalloc(sizeof(struct cm7326), GFP_KERNEL);
	if( NULL == cm7326 ) {
		printk("cm7326: cm7326_probe: not enough kernel memory.\n");
		return -ENOMEM;
	}
	memset(cm7326, 0x0, sizeof(struct cm7326));	

	saa7146_i2c_adapter_prepare(dev, &cm7326->i2c_adapter, 0, SAA7146_I2C_BUS_BIT_RATE_480);
	if(i2c_add_adapter(&cm7326->i2c_adapter) < 0) {
		DEB_S(("cannot register i2c-device. skipping.\n"));
		kfree(cm7326);
		return -EFAULT;
	}

	/* have a look if a saa7110 is present */
	if (0 != (err = i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_READ, 0x00, I2C_SMBUS_BYTE_DATA, &data))) {
		DEB_D(("cm7326_probe failed for this device.\n"));	
		i2c_del_adapter(&cm7326->i2c_adapter);
		kfree(cm7326);
		return -ENODEV;
	}

	DEB_D(("cm7326_probe succeeded for this device.\n"));	

	/* we store the pointer in our private data field */
	(struct cm7326*)dev->ext_priv = cm7326;

	return 0;
}

/* bring hardware to a sane state. this has to be done, just in case someone
   wants to capture from this device before it has been properly initialized.
   the capture engine would badly fail, because no valid signal arrives on the
   saa7146, thus leading to timeouts and stuff. */
static int cm7326_init_done(struct saa7146_dev *dev)
{
	struct cm7326* cm7326 = (struct cm7326*)dev->ext_priv;
	int ret = 0;
	
	DEB_D(("cm7326_init_done called.\n"));

	/* initialize the helper ics to useful values */
	if (0 != (ret = saa7110_init(cm7326))) {
		DEB_S(("initialization of saa7110 failed\n"));
		return -EFAULT;
	}
	
	/* the rest for saa7146: you should definitely set some basic values
	   for the input-port handling of the saa7146. */
	   
	/* some stuff is done via variables */
	saa7146_set_hps_source_and_sync(dev, SAA7146_HPS_SOURCE_PORT_A, SAA7146_HPS_SYNC_PORT_A);

	/* some stuff is done via direct write to the registers */

	/* this is ugly, but because of the fact that this is completely
	   hardware dependend, it should be done directly... */
      	saa7146_write(dev, DD1_STREAM_B,	0x00000000);
	saa7146_write(dev, DD1_INIT,		0x02000200);
	saa7146_write(dev, MC2, (MASK_09 | MASK_25 | MASK_10 | MASK_26));

	return 0;
}

static struct saa7146_ext_vv vv_data;

/* this function only gets called when the probing was successful */
static int cm7326_attach(struct saa7146_dev *dev, struct saa7146_pci_extension_data *info)
{
	struct cm7326* cm7326 = (struct cm7326*)dev->ext_priv;
	
	DEB_D(("cm7326_attach called.\n"));

	/* checking for i2c-devices can be omitted here, because we
	   already did this in "cm7326_vl42_probe" */

	saa7146_vv_init(dev,&vv_data);
	if( 0 != saa7146_register_device(&cm7326->video_dev, dev, "cm7326", VFL_TYPE_GRABBER)) {
		ERR(("cannot register capture v4l2 device. skipping.\n"));
		return -1;
	}

	printk("cm7326: found 'cm7326'-%d.\n",cm7326_num);
	cm7326_num++;

	/* the rest */
	cm7326->cur_input = 0;
	return cm7326_init_done(dev);
}

static int cm7326_detach(struct saa7146_dev *dev)
{
	struct cm7326* cm7326 = (struct cm7326*)dev->ext_priv;
	
	DEB_EE(("dev:%p\n",dev));

	saa7146_unregister_device(&cm7326->video_dev,dev);
	saa7146_vv_release(dev);

	cm7326_num--;

	i2c_del_adapter(&cm7326->i2c_adapter);
	kfree(cm7326);
	return 0;
}

static int cm7326_ioctl(struct saa7146_fh *fh, unsigned int cmd, void *arg) 
{
	struct saa7146_dev *dev = fh->dev;
	struct cm7326* card = (struct cm7326*)dev->ext_priv;

	switch(cmd)
	{
	case VIDIOC_G_INPUT:
	{
		int* input = (int *)arg;
		*input = card->cur_input;

		DEB_D(("VIDIOC_G_INPUT %d.\n",*input));
		return 0;		
	}	
	case VIDIOC_S_INPUT:
	{
		int	input = *(int *)arg;
		int 	source = 0, sync = 0;
		int	i;
		static  int saa7110_inputs[6] = {5,4,3,2,1,0};
		
		if (input < 0 || input >= CM7326_INPUTS) {
			DEB_D(("v4l2_ioctl: VIDIOC_S_INPUT: invalid input %d.\n",input));
			return -EINVAL;
		}

		DEB_D(("v4l2_ioctl: VIDIOC_S_INPUT %d.\n",input));

		source = SAA7146_HPS_SOURCE_PORT_A;
		sync = SAA7146_HPS_SYNC_PORT_A;
		card->cur_input = input;

		/* switch video in saa7110 */
		saa7146_set_hps_source_and_sync(dev, source, sync);
		i = saa7110_inputs[input];
		return saa7110_selmux(card, i);
	}
	case VIDIOC_ENUMINPUT:
	{
		struct v4l2_input *i = arg;
		
		/* sanity check to satisfy xawtv */
		if( i->index < 0 || i->index >= CM7326_INPUTS) {
			DEB_D(("v4l2_ioctl: VIDIOC_ENUMINPUT: invalid input %d.\n",i->index));
			return -EINVAL;
		}
		
		memcpy(i, &cm7326_inputs[i->index], sizeof(struct v4l2_input));

		DEB_D(("v4l2_ioctl: VIDIOC_ENUMINPUT %d.\n",i->index));
		return 0;
	}
	default:
		DEB_D(("v4l2_ioctl does not handle this ioctl.\n"));
		return -ENOIOCTLCMD;
	}
	return 0;
}

static int std_callback(struct saa7146_dev *dev, struct saa7146_standard *std)
{
	struct cm7326* cm7326 = (struct cm7326*)dev->ext_priv;
	union i2c_smbus_data data;
	int ret = 0;
	
	data.byte = 0x00;
	ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x06, I2C_SMBUS_BYTE_DATA, &data);

	switch (std->id) {
		case V4L2_STD_NTSC:
		{
			data.byte = 0x06;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x0D, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0x2C;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x11, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0x81;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x30, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0xDF;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x2A, I2C_SMBUS_BYTE_DATA, &data);
			break;
		}
		case V4L2_STD_PAL:
		{
			data.byte = 0x06;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x0D, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0x59;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x11, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0x9A;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x2E, I2C_SMBUS_BYTE_DATA, &data);
			break;
		}
		case V4L2_STD_SECAM:
		{
			data.byte = 0x07;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x0D, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0x59;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x11, I2C_SMBUS_BYTE_DATA, &data);
			data.byte = 0x9A;
			ret += i2c_smbus_xfer(&cm7326->i2c_adapter, 0x4e, 0, I2C_SMBUS_WRITE, 0x2E, I2C_SMBUS_BYTE_DATA, &data);
			break;
		}
		default:
		{
			DEB_S(("invalid standard.\n"));	
			return -EFAULT;
		}
	}

	if (0 != ret) {
		DEB_S(("writing to saa7110 failed.\n"));	
		return -EFAULT;
	}

	return 0;
}

static struct saa7146_standard standard[] = {
	{
		.name	= "PAL", 	.id	= V4L2_STD_PAL,
		.v_offset	= 0x17,	.v_field 	= 288,
		.h_offset	= 0x14,	.h_pixels 	= 680,
		.v_max_out	= 576,	.h_max_out	= 768,
	}, {
		.name	= "NTSC", 	.id	= V4L2_STD_NTSC,
		.v_offset	= 0x14,	.v_field 	= 240,
		.h_offset	= 0x00,	.h_pixels 	= 640,
		.v_max_out	= 480,	.h_max_out	= 640,
	}, {
		.name	= "SECAM", 	.id	= V4L2_STD_SECAM,
		.v_offset	= 0x14,	.v_field 	= 288,
		.h_offset	= 0x14,	.h_pixels 	= 720,
		.v_max_out	= 576,	.h_max_out	= 768,
	}
};		

static struct saa7146_extension extension;

static struct saa7146_pci_extension_data cm7326 = {
        .ext_priv = "cm7326",
        .ext = &extension,
};

static struct pci_device_id pci_tbl[] = {
	{
		.vendor    = PCI_VENDOR_ID_PHILIPS,
		.device	   = PCI_DEVICE_ID_PHILIPS_SAA7146,
		.subvendor = 0x1435,
		.subdevice = 0x3303,
		.driver_data = (unsigned long)&cm7326,
	}, {
		.vendor = 0,
	}
};

static struct saa7146_ext_vv vv_data = {
	.inputs		= CM7326_INPUTS,
	.capabilities	= 0,
	.stds		= &standard[0],
	.num_stds	= sizeof(standard)/sizeof(struct saa7146_standard),
	.std_callback	= &std_callback, 
	.ioctls		= &ioctls[0],
	.ioctl		= cm7326_ioctl,
};

static struct saa7146_extension extension = {
	.name		= "cm7326",
	.flags		= SAA7146_USE_I2C_IRQ,
	
	.pci_tbl	= &pci_tbl[0],
	.module		= THIS_MODULE,

	.probe		= cm7326_probe,
	.attach		= cm7326_attach,
	.detach		= cm7326_detach,

	.irq_mask	= 0,
	.irq_func	= NULL,
};	


int __init cm7326_init_module(void) 
{
	int ret = 0;

	ret = saa7146_register_extension(&extension);
	if (0 != ret) {
		DEB_S(("failed to register extension.\n"));
	}
	
	return ret;
}

void __exit cm7326_cleanup_module(void) 
{
	saa7146_unregister_extension(&extension);
}

module_init(cm7326_init_module);
module_exit(cm7326_cleanup_module);

MODULE_AUTHOR("Miguel Freitas, Michael Hunold");
MODULE_DESCRIPTION("CM7326 frame grabber v4l2-driver");
MODULE_LICENSE("GPL");


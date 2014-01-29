/*
    sp5055II.c		A driver for MITEL SP5055 2.6GHz Bidirectional
			I2C BUS Controlled Synthesizer

    Version: $Id: sp5055II.c,v 1.4 2004-04-17 15:32:13 ttittmann Exp $
			
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
   
    sp5055II.c		MITEL SP5055 I2C driver
 
    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>
 */

#define DEBUG_VARIABLE sp5055II_debug
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

#include <media/saa7146.h>
#include <media/tuner.h>

#include "sp5055II.h"

static int sp5055II_debug = 0;	/* module load parameter */
#ifdef MODULE
MODULE_PARM(sp5055II_debug, "i");
MODULE_PARM_DESC(sp5055II_debug, "debug level (default: 0)");
MODULE_PARM(foffset, "i");
MODULE_PARM_DESC(foffset, "shift of receiving frequency (default:0, meaningful range: ca. +/- 50)");
#endif

static int foffset = 0;

/* I2C addresses to scan */
static unsigned short int normal_i2c[] = { I2C_CLIENT_END };
static unsigned short int normal_i2c_range[] = { I2C_SP5055_LOW, I2C_SP5055_HIGH, I2C_CLIENT_END };

/* magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/* unique ID allocation */
static int sp5055_id = 0;

static struct i2c_driver sp5055_driver;

/*****************************************************************************/

/* sp5055_setfreq: set tuner osc freq by sending four bytes to SP5055 */
static int sp5055_setfreq (struct i2c_client *sp5055_client, unsigned long int freq)
{
	struct sp5055 *tuner = (struct sp5055 *) sp5055_client->data;
	unsigned long int div;
	unsigned char b[4];
	int ret;

	/* Compute PLL divider: */
	/* - freq parameter is tv channel frequency in units of Hz */
	/* - Tuner IF is 480 MHz */
	div = (freq / 125000) + 3840 + foffset;
	
	b[0] = (unsigned char) ((div >> 8) & 0x7f);
	b[1] = (unsigned char) (div & 0xff);
	/* choose high charge pump current in case of "big" frequency difference */
	b[2] = tuner->control0;
	b[2] |= ((abs (div - tuner->divider) > 1600) ? SP5055_CTRL0_CP : 0);
	b[3] = tuner->control1;

	DEB_D(("I2C client '%s'; f=%ld; div=%ld; b0=0x%02x, b1=0x%02x, b2=0x%02x, b3=0x%02x.\n", sp5055_client->name, freq, div, b[0], b[1], b[2], b[3]));
	
	if ( (ret=i2c_master_send (sp5055_client, b, sizeof (b))) != sizeof(b) ) {
		DEB_S(("I2C client '%s' failed.\n", sp5055_client->name));
		return -EIO;
	}
	
	tuner->divider = div;
	
	return 0;
}


/* sp5055_chip_init: initialize SP5055 chip */
static int sp5055_chip_init (struct i2c_client *sp5055_client)
{
	struct sp5055 *tuner = (struct sp5055 *) sp5055_client->data;
	int ret = 0;
	
	DEB_EE(("I2C client '%s'.\n", sp5055_client->name));

	memset (tuner, 0, sizeof (struct sp5055));
	
	tuner->control0 = 0x8e;
	
	/* Setup for an initial TV channel */
	if ( (ret=sp5055_setfreq (sp5055_client, 1464000000)) != 0) {	/* Astra Transponder 1 */
		DEB_S(("I2C client '%s' failed.\n", sp5055_client->name));
		return ret;
	}
	
	return 0;
}

/*****************************************************************************/

/* this function is called by i2c_probe */
static int sp5055_detect (struct i2c_adapter *adap, int addr, unsigned short int flags, int kind)
{
	struct i2c_client *sp5055_client;
	struct sp5055 *tuner;
	int ret;

	DEB_EE(("I2C adapter '%s'; addr=0x%02x.\n", adap->name, addr));

	/* let's see whether this adapter can support what we need */
	if (i2c_check_functionality (adap, I2C_FUNC_I2C | I2C_FUNC_SMBUS_READ_BYTE) == 0) {
		return 0;
	}

	/* allocate memory for client structure */
	sp5055_client = kmalloc (sizeof (struct i2c_client), GFP_KERNEL);
	if (sp5055_client == NULL) {
		return -ENOMEM;
	}
	
	/* fill client structure */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	sprintf (sp5055_client->name, "MITEL SP5055");
#else
	sprintf (sp5055_client->dev.name, "MITEL SP5055");
#endif	
	sp5055_client->id = sp5055_id++;
	sp5055_client->flags = 0;
	sp5055_client->addr = addr;
	sp5055_client->adapter = adap;
	sp5055_client->driver = &sp5055_driver;
	sp5055_client->data = NULL;

	if ( sp5055_id == 0) {
		DEB_S(("I2C client '%s'; No hardware detected.\n", sp5055_client->name));
	} else {
		DEB_S(("I2C client '%s' detected at i2c address 0x%02x.\n", sp5055_client->name, sp5055_client->addr));
	}

	/* allocate memory for tuner structure */
	if ( (sp5055_client->data = tuner = kmalloc (sizeof (struct sp5055), GFP_KERNEL)) == NULL) {
		kfree (sp5055_client);
		return -ENOMEM;
	}

	/* tell the i2c layer a new client has arrived */
	if ( (ret=i2c_attach_client (sp5055_client)) != 0) {
		DEB_S(("I2C client registration failed, '%s' @ 0x%02x not attached.\n", sp5055_client->name, sp5055_client->addr));
		kfree (sp5055_client->data);
		kfree (sp5055_client);
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x attached to adapter '%s'.\n", sp5055_client->name, sp5055_client->addr, sp5055_client->adapter->name));

	/* SP5055 chip init */
	if ( (ret=sp5055_chip_init (sp5055_client)) != 0) {
		DEB_S(("I2C client '%s' @ 0x%02x failed, continuing anyway.\n", sp5055_client->name, sp5055_client->addr));
		return ret;
	}

	return 0;
}


static int sp5055_attach (struct i2c_adapter *adap)
{
	DEB_EE(("I2C adapter '%s'.\n", adap->name));

	return i2c_probe (adap, &addr_data, &sp5055_detect);
}


static int sp5055_detach (struct i2c_client *sp5055_client)
{
	int ret = 0;

	DEB_EE(("I2C client '%s'; addr=0x%02x; adap='%s'.\n", sp5055_client->name, sp5055_client->addr, sp5055_client->adapter->name));

	if ( (ret=i2c_detach_client (sp5055_client)) != 0) {
		DEB_S(("I2C client deregistration failed, '%s' @ 0x%02x not detached.\n", sp5055_client->name, sp5055_client->addr));
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x detached from adapter '%s'.\n", sp5055_client->name, sp5055_client->addr, sp5055_client->adapter->name));

	kfree (sp5055_client->data);
	kfree (sp5055_client);

	return 0;
}


static int sp5055_command (struct i2c_client *sp5055_client, unsigned int cmd, void *arg)
{
	int ret = 0;

	switch (cmd) {
		case TUNER_SET_TVFREQ: {
			unsigned long int freq = *(unsigned long *) arg;
			
			DEB_EE(("I2C client '%s'; TUNER_SET_TVFREQ (freq=%ld)\n", sp5055_client->name, freq));

			return sp5055_setfreq (sp5055_client, freq);
		}
		
		case TUNER_SET_TYPE:
		
			DEB_EE(("I2C client '%s'; TUNER_SET_TYPE (not supported)\n", sp5055_client->name));

			return -EINVAL;

/*
		case TUNER_SET_RADIOFREQ:
		
			DEB_EE(("I2C client '%s'; TUNER_SET_RADIOFREQ (not supported)\n", sp5055_client->name)); 

			return -EINVAL;
*/
		case TUNER_GET_SIGNAL: {
			unsigned char stat = i2c_smbus_read_byte (sp5055_client) & 0x07;
			
			*(int *) arg = ((int) (stat & 0x07) << 13);

			DEB_EE(("I2C client '%s'; TUNER_GET_SIGNAL (afc=%d; stat=0x%02x; rtrn val=%d)\n", sp5055_client->name, stat & 0x07, stat, *(int *) arg));

			return ret;
		}
		
		default:
		
			DEB_EE(("I2C client '%s'; ctl cmd (0x%x)\n", sp5055_client->name, cmd));

			return -ENOIOCTLCMD;
	}
}


void sp5055_inc_use (struct i2c_client *sp5055_client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


void sp5055_dec_use (struct i2c_client *sp5055_client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


static struct i2c_driver sp5055_driver = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,54)
	.owner		= THIS_MODULE,
#endif
	.name		= "sp5055II.o",
	.id		= I2C_DRIVERID_SP5055,
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= sp5055_attach,
	.detach_client	= sp5055_detach,
	.command	= sp5055_command,
	.inc_use	= sp5055_inc_use,
	.dec_use	= sp5055_dec_use
};

/*****************************************************************************/

EXPORT_NO_SYMBOLS;

int __devinit sp5055_init (void)
{
	int ret;

	if ( 0 != (ret=i2c_add_driver (&sp5055_driver)) ) {
		DEB_S(("Driver '%s' registration failed, module not inserted.\n", sp5055_driver.name));
		return ret;
	}

	return 0;
}


void __devexit sp5055_cleanup (void)
{
	if ( 0 != i2c_del_driver (&sp5055_driver) )
		DEB_S(("Driver '%s' deregistration failed, module not removed.\n", sp5055_driver.name));
}

/*****************************************************************************/

MODULE_AUTHOR ("Torsten Tittmann <Torsten.Tittmann@gmx.de>");
MODULE_DESCRIPTION ("MITEL SP5055 I2C driver");
MODULE_LICENSE("GPL");

module_init(sp5055_init);
module_exit(sp5055_cleanup);


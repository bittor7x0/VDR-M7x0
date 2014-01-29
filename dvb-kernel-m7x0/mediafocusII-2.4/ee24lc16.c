/*
    ee24lc16.c	A driver for 24LC16 Serial EEPROMs

    Version: $Id: ee24lc16.c,v 1.4 2004-04-17 15:32:13 ttittmann Exp $
    
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

    ee24lc16.c		24LC16 Serial EEPROM I2C driver
   
    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>
*/

/* For the time being only the first 256 from 2048 bytes of a	*/
/* 24LC16 Serial EEPROM can be read/written by this driver.	*/
/* (= No page switching possible.)				*/

#define DEBUG_VARIABLE ee24lc16_debug

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>

#if CONFIG_MODVERSIONS==1
#define MODVERSIONS
#include <linux/modversions.h>
#endif

#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/i2c.h>

#include <media/saa7146.h>
#include "videodev_priv.h"

#include "ee24lc16.h"

static int ee24lc16_debug = 0;	/* module load parameter */
#ifdef MODULE
MODULE_PARM(ee24lc16_debug,"i");
MODULE_PARM_DESC(ee24lc16_debug, "debug level (default: 0)");
#endif

/* I2C addresses to scan */
static unsigned short int normal_i2c[] = { I2C_CLIENT_END };
static unsigned short int normal_i2c_range[] = { I2C_EEPROM_LOW, I2C_EEPROM_HIGH, I2C_CLIENT_END };

/* magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/* unique ID allocation */
static int eeprom_id = 0;

static struct i2c_driver eeprom_driver;

/*****************************************************************************/

static int eeprom_getbyte (struct i2c_client *eeprom_client, __u16 addr)
{
	int val;

	val = i2c_smbus_read_byte_data (eeprom_client, addr);

	DEB_D(("I2C client '%s'; addr=0x%x; byte=0x%02x.\n", eeprom_client->name, addr, val));

	if ( 0 != val ) {
		DEB_S(("I2C client '%s' failed.\n", eeprom_client->name));
	}

	return val;
}

static int eeprom_setbyte (struct i2c_client *eeprom_client, __u16 addr, __u8 byt)
{
	int ret = 0;

	DEB_D(("I2C client '%s'; addr=0x%x; byte=0x%02x.\n", eeprom_client->name, addr, byt));

	ret |= i2c_smbus_write_byte_data (eeprom_client, addr, byt);
	mdelay (10);

	if ( 0 != ret ) {
		DEB_S(("I2C client '%s' failed.\n", eeprom_client->name));
		return -EIO;
	}

	return 0;
}

/*****************************************************************************/

/* this function is called by i2c_probe */
static int eeprom_detect (struct i2c_adapter *adap, int addr, unsigned short int flags, int kind)
{
	struct i2c_client *eeprom_client;
	int ret;

	DEB_EE(("I2C adapter '%s'; addr=0x%02x.\n", adap->name, addr));

	/* let's see whether this adapter can support what we need */
	if ( 0 == i2c_check_functionality (adap, I2C_FUNC_I2C | I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE) ) 
		return 0;

	/* allocate memory for client structure */
	if ( NULL == (eeprom_client=kmalloc (sizeof (struct i2c_client), GFP_KERNEL)) )
		return -ENOMEM;

	/* fill client structure */
	eeprom_client->id = eeprom_id++;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	sprintf (eeprom_client->name, "Serial EEPROM(%d)", eeprom_client->id);
#else
	sprintf (eeprom_client->dev.name, "Serial EEPROM(%d)", eeprom_client->id);
#endif
	eeprom_client->flags = 0;
	eeprom_client->addr = addr;
	eeprom_client->adapter = adap;
	eeprom_client->driver = &eeprom_driver;
	eeprom_client->data = NULL;

	if (  0 == eeprom_id ) {
		DEB_S(("I2C client '%s'; No hardware detected.\n", eeprom_client->name));
	} else {
		DEB_S(("I2C client '%s' detected at i2c address 0x%02x.\n", eeprom_client->name, eeprom_client->addr));
	}

	/* tell the i2c layer a new client has arrived */
	if ( 0 != (ret=i2c_attach_client (eeprom_client)) ) {
		DEB_S(("I2C client registration failed, '%s' @ 0x%02x not attached.\n", eeprom_client->name, eeprom_client->addr));
		/* kfree (eeprom_client->data); */
		kfree (eeprom_client);
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x attached to adapter '%s'.\n", eeprom_client->name, eeprom_client->addr, eeprom_client->adapter->name));

	return 0;
}


static int eeprom_attach (struct i2c_adapter *adap)
{

	DEB_EE(("I2C adapter '%s'.\n", adap->name));

	return i2c_probe (adap, &addr_data, &eeprom_detect);
}


static int eeprom_detach (struct i2c_client *eeprom_client)
{
	int ret = 0;

	DEB_EE(("I2C client '%s'; addr=0x%02x; adap='%s'.\n", eeprom_client->name, eeprom_client->addr, eeprom_client->adapter->name));

	if ( 0 != (ret=i2c_detach_client (eeprom_client)) ) {
		DEB_S(("I2C client deregistration failed, '%s' @ 0x%02x not detached.\n", eeprom_client->name, eeprom_client->addr));
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x detached from adapter '%s'.\n", eeprom_client->name, eeprom_client->addr, eeprom_client->adapter->name));

	/* kfree (eeprom_client->data); */
	kfree (eeprom_client);

	return 0;
}


static int eeprom_command (struct i2c_client *eeprom_client, unsigned int cmd, void *arg)
{
	int ret = 0;

	switch (cmd) {
		case GET_EEPROM_DATA:
		{
			struct eeprom_s ee;
			int val;

			memcpy (&ee, arg, sizeof (ee));

			DEB_EE(("I2C client '%s'; GET_EEPROM_DATA (addr=0x%x; cnt=%d; *data=0x%p).\n", eeprom_client->name, ee.addr, ee.count, ee.data));

			if ((ee.addr >= 256) || (ee.count > 256) || ((ee.addr + ee.count) > 256))
				return -EINVAL;

			if ( 0 == ee.count ) {
				val = eeprom_getbyte (eeprom_client, ee.addr);
				if (val < 0)
					return val;
				*ee.data = (__u8) val;
			} else
				do {
					val = eeprom_getbyte (eeprom_client, ee.addr++);
					if (val < 0)
						return val;
					*ee.data++ = (__u8) val;
				} while (--ee.count);

			return 0;
		}

		case SET_EEPROM_DATA:
		{
			struct eeprom_s ee;

			memcpy (&ee, arg, sizeof (ee));

			DEB_EE(("I2C client '%s'; SET_EEPROM_DATA (addr=0x%x; cnt=%d; *data=0x%p).\n", eeprom_client->name, ee.addr, ee.count, ee.data));

			if ((ee.addr >= 256) || (ee.count > 256) || ((ee.addr + ee.count) > 256))
				return -EINVAL;

			if ( 0 == ee.count ) {
				ret = eeprom_setbyte (eeprom_client, ee.addr, *ee.data);
			} else {
				do {
					ret = eeprom_setbyte (eeprom_client, ee.addr++, *ee.data++);
					if (ret < 0)
						return ret;
				} while (--ee.count);
				ret = 0;
			}
			return ret;
		}

		default:

		DEB_EE(("I2C client '%s'; Unknown ioctl cmd (0x%x)\n", eeprom_client->name, cmd));

			return -ENOIOCTLCMD;
	}
}


void eeprom_inc_use (struct i2c_client *eeprom_client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


void eeprom_dec_use (struct i2c_client *eeprom_client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


static struct i2c_driver eeprom_driver = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,54)
	.owner		= THIS_MODULE,
#endif
	.name		= "ee24lc16.o",
	.id		= I2C_DRIVERID_AT24Cxx,
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= eeprom_attach,
	.detach_client	= eeprom_detach,
	.command	= eeprom_command,
	.inc_use	= eeprom_inc_use,
	.dec_use	= eeprom_dec_use
};

/*****************************************************************************/

EXPORT_NO_SYMBOLS;

int __devinit eeprom_init (void)
{
	int ret;

	if ( 0 != (ret=i2c_add_driver (&eeprom_driver)) ) {
		DEB_S(("Driver '%s' registration failed, module not inserted.\n", eeprom_driver.name))
		return ret;
	}


	return 0;
}


void __devexit eeprom_cleanup (void)
{
	if ( 0 != i2c_del_driver (&eeprom_driver) )
		DEB_S(("Driver '%s' deregistration failed, module not removed.\n", eeprom_driver.name));
}

/*****************************************************************************/

MODULE_AUTHOR ("Torsten Tittmann <Torsten.Tittmann@gmx.de>");
MODULE_DESCRIPTION ("24LC16 Serial EEPROM I2C driver");
MODULE_LICENSE("GPL");

module_init(eeprom_init);
module_exit(eeprom_cleanup);


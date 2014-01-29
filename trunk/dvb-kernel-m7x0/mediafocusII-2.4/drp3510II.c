/*
    drp3510II.c		A driver for Micronas Intermetall DRP3510A 
			Digital Radio Processor

    Version: $Id: drp3510II.c,v 1.4 2004-04-17 15:32:12 ttittmann Exp $

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
 
    drp3510II.c  Micronas Intermetall DRP3510A I2C driver

    Copyright (C) 2000,2001 Rolf Siebrecht <rolf.siebrecht@t-online.de>
 */

#define DEBUG_VARIABLE drp3510II_debug

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
#include "drp3510II.h"

static int drp3510II_debug = 0;	/* module load parameter */
#ifdef MODULE
MODULE_PARM(drp3510II_debug, "i");
MODULE_PARM_DESC(drp3510II_debug, "debug level (default: 0)");
#endif

/* I2C addresses to scan */
static unsigned short int normal_i2c[] = { I2C_DRP3510, I2C_CLIENT_END };
static unsigned short int normal_i2c_range[] = { I2C_CLIENT_END };

/* magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/* unique ID allocation */
static int drp3510_id = 0;

static struct i2c_driver drp_driver;

/*****************************************************************************/

/* write to a DRP3510 data register */
static int drp3510_write_data (struct i2c_client *drp_client, unsigned int reg, unsigned long data)
{
	unsigned char buf[5] = {
		0x68,					/* b[0] r/w cmd = "write" */
		0x90 | ((reg >> 4) & 0x0f),		/* b[1] 0x9 r1	*/
		((reg << 4) & 0xf0) | (data & 0x0f),	/* b[2] r0  d0	*/
		(data >> 12) & 0xff,			/* b[3] d4  d3	*/
		(data >> 4) & 0xff			/* b[4] d2  d1	*/
	};
	int count = sizeof (buf);

	DEB_D(("I2C client '%s'; reg=0x%x, data=0x%lx.\n", drp_client->name, reg, data));

	if ( count != i2c_master_send (drp_client, buf, count) ) {
		DEB_S(("I2C client '%s' failed.\n", drp_client->name));
		return -EIO;
	}
	
	return 0;
}


/* read from a DRP3510 data register */
static int drp3510_read_data (struct i2c_client *drp_client, __u16 reg)
{
	int ret = 0;
	__u32 val = 0;
	__u8 addr_buf[3] = {
		0x68,				/* b[0] r/w cmd = "write" */
		0xd0 | ((reg >> 4) & 0x0f),	/* b[1] 0xd r1	*/
		(reg << 4) & 0xf0		/* b[2] r0  0	*/
	};
	int addr_count = sizeof (addr_buf);
	__u8 send_buf[1] = {
		0x69				/* b[0] r/w cmd = "read" */
	};
	__u8 recv_buf[4];
	struct i2c_msg transfer_msgs[] = {
	{	
		drp_client->addr,
		drp_client->flags,
		sizeof (send_buf),
		send_buf
	},
	{
		drp_client->addr,
		drp_client->flags | I2C_M_RD,
		sizeof (recv_buf),
		recv_buf
	} };
	int num_of_msgs = sizeof (transfer_msgs) / sizeof (transfer_msgs[0]);

	DEB_D(("I2C client '%s'; reg=0x%04x.\n", drp_client->name, reg));
	
	ret |= (addr_count != i2c_master_send (drp_client, addr_buf, addr_count));	/* send register address */
	ret |= (num_of_msgs != i2c_transfer (drp_client->adapter, transfer_msgs, num_of_msgs));

	val = (((__u32) recv_buf[3] << 16) & 0xf0000) | (((__u32) recv_buf[0] << 8) & 0xff00) | recv_buf[1];
	
	if ( 0 != ret) {
		DEB_S(("I2C client '%s' failed.\n", drp_client->name));
		return -EIO;
	}

	return val;
}


/* write to DRP3510 control register */
static int drp3510_write_control (struct i2c_client *drp_client, __u16 data)
{
	int ret;
	
	DEB_D(("I2C client '%s'; data=0x%x.\n", drp_client->name, data));

	if ( 0 != (ret=i2c_smbus_write_word_data (drp_client, 0x6a, data)) ) {
		DEB_S(("I2C client '%s' failed.\n", drp_client->name));
		return -EIO;
	}
	
	return 0;
}


/* read DRP3510 status + index */
static int drp3510_read_statusindex (struct i2c_client *drp_client)
{
	int ret;
	
	if ( 0 != (ret=i2c_smbus_read_word_data (drp_client, 0x69)) ) {
		DEB_S(("I2C client '%s' failed.\n", drp_client->name));
		return -EIO;
	}
	
	DEB_D(("I2C client '%s'; data=0x%04x.\n", drp_client->name, ret));

	return ret;
}

/* read DRP3510 ADR data field */
static __u8 * drp3510_read_adrdata (struct i2c_client *drp_client, __u8 offset, __u8 count)
{
	struct drp3510 *drp = (struct drp3510 *) drp_client->data;
	int ret = 0;
	__u8 addr_buf[3] = {
		0x68,	/* b[0] r/w cmd = "write"	*/
		0x60,	/* b[1] 0x60	*/
		0	/* b[2] later: offset count	*/
	};
	int addr_count = sizeof (addr_buf);
	__u8 send_buf[1] = {
		0x69	/* b[0] r/w cmd = "read"	*/
	};
	struct i2c_msg transfer_msgs[] = {
	{	drp_client->addr,
		drp_client->flags,
		sizeof (send_buf),
		send_buf
	},
	{
		drp_client->addr,
		drp_client->flags | I2C_M_RD,
		sizeof (drp->adrdata),		/* preliminary */
		drp->adrdata
	} };
	int num_of_msgs = sizeof (transfer_msgs) / sizeof (transfer_msgs[0]);
	
	if (offset > 9) offset = 9;
	if (count > 10) count = 10;
	if (count == 0) count = 1;

	DEB_D(("I2C client '%s'; ofs=%d; cnt=%d.\n", drp_client->name, offset, count));

	addr_buf[2] = (offset << 4) | (count & 0x0f);
	transfer_msgs[1].len = count << 1;
	
	ret |= (addr_count != i2c_master_send (drp_client, addr_buf, addr_count));

	DEB_D(("I2C client '%s'; i2c_master_send %s!\n", drp_client->name, ret? "error" : "ok"));

	ret |= (num_of_msgs != i2c_transfer (drp_client->adapter, transfer_msgs, num_of_msgs));

	DEB_D(("I2C client '%s'; i2c_transfer %s!\n", drp_client->name, ret ? "error" : "ok"));

	if (0 != ret) {
		DEB_S(("I2C client '%s' failed.\n", drp_client->name));
		/* return -EIO;		can't return an error code here */
	}
	
	return drp->adrdata;
}


static int drp3510_chip_init (struct i2c_client *drp_client)
{
	struct drp3510 *drp = (struct drp3510 *) drp_client->data;
	int ret = 0;
	
	DEB_EE(("I2C client '%s'.\n", drp_client->name));

	/* FIXME!! Initialize DRP3510 chip here as far as needed */

	memset (drp, 0, sizeof (struct drp3510));
	
	drp->adapter = drp_client->adapter;
	/* FIXME!! set more variables of struct drp to initial values */
	
	if ( 0 != ret ) {
		DEB_S(("I2C client '%s' failed.\n", drp_client->name));
		return ret;
	}
	
	return 0;
}

/*****************************************************************************/

/* this function is called by i2c_probe */
static int drp3510_detect (struct i2c_adapter *adap, int addr, unsigned short int flags, int kind)
{
	struct i2c_client *drp_client;
	struct drp3510 *drp;
	int ret;

	DEB_EE(("I2C adapter '%s'; addr=0x%02x.\n", adap->name, addr));


	/* let's see whether this adapter can support what we need */
	/* doesn't work yet!
	  if (i2c_check_functionality (adap, I2C_FUNC_I2C | I2C_FUNC_SMBUS_WORD_DATA) == 0) {
		return 0;
	  }
	*/

	/* allocate memory for client structure */
	drp_client = kmalloc (sizeof (struct i2c_client), GFP_KERNEL);
	if ( NULL == drp_client ) {
		return -ENOMEM;
	}

	/* fill client structure */
	drp_client->id = drp3510_id++;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0))
	sprintf (drp_client->name, "Micronas DRP3510(%d)", drp_client->id);
#else
	sprintf (drp_client->dev.name, "Micronas DRP3510(%d)", drp_client->id);
#endif
	drp_client->flags = 0;
	drp_client->addr = addr;
	drp_client->adapter = adap;
	drp_client->driver = &drp_driver;
	drp_client->data = NULL;

	if ( 0 == drp3510_id ) {
		DEB_S(("I2C client '%s'; No hardware detected.\n", drp_client->name));
	} else {
		DEB_S(("I2C client '%s' detected at i2c address 0x%02x.\n", drp_client->name, drp_client->addr));
	}

	/* allocate memory for drp structure */
	if ( NULL == (drp_client->data = drp = kmalloc (sizeof (struct drp3510), GFP_KERNEL)) ) {
		kfree (drp_client);
		return -ENOMEM;
	}

	/* tell the i2c layer a new client has arrived */
	if ( 0 != (ret=i2c_attach_client (drp_client)) ) {
		DEB_S(("I2C Client registration failed, '%s' @ 0x%02x not attached.\n", drp_client->name, drp_client->addr));
		kfree (drp_client->data);
		kfree (drp_client);
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x attached to adapter '%s'.\n", drp_client->name, drp_client->addr, drp_client->adapter->name));

	/* DRP3510 chip init */
	if ( 0 != (ret=drp3510_chip_init (drp_client)) ) {
		DEB_S(("Init of I2C client '%s' @ 0x%02x failed, continuing anyway.\n", drp_client->name, drp_client->addr));
		return ret;
	}

	return 0;
}


static int drp3510_attach (struct i2c_adapter *adap)
{
	DEB_EE(("I2C adapter '%s'.\n", adap->name));

	return i2c_probe (adap, &addr_data, &drp3510_detect);
}


static int drp3510_detach (struct i2c_client *drp_client)
{
	int ret;

	DEB_EE(("I2C client '%s'; addr=0x%02x; adap='%s'.\n", drp_client->name, drp_client->addr, drp_client->adapter->name));

	/* FIXME!! disable ADR, mute ADR output, disable PIO DMA mode,... */

	if ( 0 != (ret=i2c_detach_client (drp_client)) ) {
		DEB_S(("I2C client deregistration failed, '%s' @ 0x%02x not detached.\n", drp_client->name, drp_client->addr));
		return ret;
	}

	DEB_S(("I2C client '%s' @ 0x%02x detached from adapter '%s'.\n", drp_client->name, drp_client->addr, drp_client->adapter->name));

	kfree (drp_client->data);
	kfree (drp_client);

	return 0;
}


static int drp3510_command (struct i2c_client *drp_client, unsigned int cmd, void *arg)
{
	/* struct drp3510 *drp = (struct drp3510 *) drp_client->data; */
	
	__u16 *sarg = arg;	/* status pointer */
	__u8  **darg = arg;	/* data field pointer */
	int ret = 0;
	
	switch (cmd) {
		case DRP_ADR:
			ret |= drp3510_write_control (drp_client, 0x0100);	/* reset drp */
			ret |= drp3510_write_control (drp_client, 0x0000);	/* release drp reset */
	
			ret |= drp3510_write_data (drp_client, 0xA8, 0x00000200);
			ret |= drp3510_write_data (drp_client, 0x67, 0x00008000);
			ret |= drp3510_write_data (drp_client, 0x60, 0x00070000);
			ret |= drp3510_write_data (drp_client, 0xF4, 0x00000000);
			
			DEB_EE(("I2C client '%s'; DRP_ADR\n", drp_client->name));

			if ( 0 != ret )
				return -EIO;
			
			return 0;
			
		case DRP_PIO_DMA:
			
			DEB_EE(("I2C client '%s'; DRP_PIO_DMA\n", drp_client->name));

			return drp3510_write_data (drp_client, 0x60, 0x00010800);


		case DRP_STATUS:
			*sarg = drp3510_read_statusindex (drp_client);
			
			DEB_EE(("I2C client '%s'; DRP_STATUS\n", drp_client->name));

			return ret;


		case DRP_ADR_DATA:
			*darg = drp3510_read_adrdata (drp_client, 0, 10);	/* no offset, 10 words -> 20 bytes */
			
			DEB_EE(("I2C client '%s'; DRP_ADR_DATA (address: 0x%p)\n", drp_client->name, darg));

			return ret;


		default:
		
			DEB_EE(("I2C client '%s'; Unknown ioctl cmd (0x%x)\n", drp_client->name, cmd)); 

			return -ENOIOCTLCMD;
	}
}


void drp3510_inc_use (struct i2c_client *drp_client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}


void drp3510_dec_use (struct i2c_client *drp_client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}


static struct i2c_driver drp_driver = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,54)
	.owner		= THIS_MODULE, 
#endif
	.name		= "drp3510II.o",
	.id		= I2C_DRIVERID_DRP3510,
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= drp3510_attach,
	.detach_client	= drp3510_detach,
	.command	= drp3510_command,
	.inc_use	= drp3510_inc_use,
	.dec_use	= drp3510_dec_use
};

/*****************************************************************************/

EXPORT_NO_SYMBOLS;

int __devinit drp3510_init (void)
{
	int ret;

	if ( 0 != (ret=i2c_add_driver (&drp_driver)) ) {
		DEB_S(("Driver '%s' registration failed, module not inserted.\n", drp_driver.name));
		return ret;
	}

	return 0;
}


void __devexit drp3510_cleanup (void)
{
	if ( 0 != i2c_del_driver (&drp_driver) )
		DEB_S(("Driver '%s' deregistration failed, module not removed.\n", drp_driver.name));
}

/*****************************************************************************/

MODULE_AUTHOR ("Torsten Tittmann <Torsten.Tittmann@gmx.de>");
MODULE_DESCRIPTION ("Micronas Intermetall DRP3510A I2C driver");
MODULE_LICENSE("GPL");

module_init(drp3510_init);
module_exit(drp3510_cleanup);


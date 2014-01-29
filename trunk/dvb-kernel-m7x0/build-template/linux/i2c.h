#ifndef _LINUX_I2C_H_FAKE_
#define _LINUX_I2C_H_FAKE_

#include <asm/types.h>

struct i2c_msg {
	u8 addr;
	int flags;
#define I2C_M_RD        0x01
	int len;
	char *buf;
};


#endif


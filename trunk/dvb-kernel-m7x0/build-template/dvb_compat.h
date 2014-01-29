#ifndef _DVB_COMPAT_H_
#define _DVB_COMPAT_H_

#include <asm/types.h>
#include <linux/fs.h>

extern void dvb_delay(int ms);

extern void kernel_thread_setup(const char *name);

extern u32 crc32_le(u32 crc, unsigned char const *data, size_t len);

extern int dvb_usercopy(struct inode *inode, struct file *file,
			unsigned int cmd, unsigned long arg,
			int (*func)(struct inode *inode, struct file *file,
			unsigned int cmd, void *arg));


#endif


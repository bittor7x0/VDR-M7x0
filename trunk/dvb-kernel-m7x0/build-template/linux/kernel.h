#ifndef _LINUX_KERNEL_H_FAKE_
#define _LINUX_KERNEL_H_FAKE_

#include <asm/types.h>

extern int printf(const char *format, ...);
extern int snprintf(char * buf, size_t size, const char * fmt, ...);

#define printk(x...) printf(x)

#endif


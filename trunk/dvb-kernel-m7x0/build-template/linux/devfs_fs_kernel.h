#ifndef _DEVFS_FS_KERNEL_H_FAKE_
#define _DEVFS_FS_KERNEL_H_FAKE_

#include <linux/fs.h>

typedef void* devfs_handle_t;

#define DEVFS_FL_DEFAULT 0

#warning setup umode_t enums S_IFCHR, S_IRUSR and S_IWUSR!!
typedef enum {
	S_IFCHR = 0,
	S_IRUSR = 0,
	S_IWUSR = 0
} umode_t;

extern devfs_handle_t devfs_mk_dir(devfs_handle_t dir, const char *name, void *info);

extern devfs_handle_t devfs_register(devfs_handle_t dir, const char *name,
				     unsigned int flags,
				     unsigned int major, unsigned int minor,
				     umode_t mode, void *ops, void *info);

extern void devfs_unregister(devfs_handle_t *de);

#endif


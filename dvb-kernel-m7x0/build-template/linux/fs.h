#ifndef _LINUX_FS_H_FAKE_
#define _LINUX_FS_H_FAKE_

#include <asm/types.h>
#include <linux/module.h>

#define O_ACCMODE          0003
#define O_RDONLY             00
#define O_WRONLY             01
#define O_RDWR               02
#define O_NONBLOCK        04000

struct inode;

struct file {
	int f_flags;
	void *private_data;
};

struct poll_table_struct;

struct file_operations {
	struct module *owner;
	int (*open) (struct inode *, struct file *);
	int (*release) (struct inode *, struct file *);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
};

#endif


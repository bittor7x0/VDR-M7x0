#ifndef _LINUX_POLL_H_FAKE_
#define _LINUX_POLL_H_FAKE_

#include <linux/fs.h>
#include <linux/wait.h>

/* These are specified by iBCS2 */
#define POLLIN          0x0001
#define POLLPRI         0x0002
#define POLLOUT         0x0004
#define POLLERR         0x0008
#define POLLHUP         0x0010
#define POLLNVAL        0x0020

/* The rest seem to be more-or-less nonstandard. Check them! */
#define POLLRDNORM      0x0040
#define POLLRDBAND      0x0080
#define POLLWRNORM      0x0100
#define POLLWRBAND      0x0200
#define POLLMSG         0x0400

typedef struct poll_table_struct poll_table;

extern void poll_wait(struct file * filp, wait_queue_head_t * wait_address, struct poll_table_struct *p);

#endif


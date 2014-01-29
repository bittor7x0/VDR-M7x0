#ifndef _LINUX_SLAB_H_FAKE_
#define _LINUX_SLAB_H_FAKE_

extern void *malloc (long size);
extern void free (void *ptr);

#define NULL ((void*) 0)

#define GFP_KERNEL 0

#define kmalloc(size,flags) malloc(size)
#define kfree(ptr) free(ptr)

#endif


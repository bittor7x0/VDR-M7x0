#ifndef _ASM_UACCESS_H_FAKE_
#define _ASM_UACCESS_H_FAKE_

extern int copy_to_user(void *to, const void *from, unsigned long len);
extern int copy_from_user(void *to, const void *from, unsigned long len);

#endif


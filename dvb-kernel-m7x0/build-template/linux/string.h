#ifndef _ASM_STRING_H_FAKE
#define _ASM_STRING_H_FAKE

#include <asm/types.h>

extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *dest, int c, size_t n);

extern int strcmp(const char *s1, const char *s2);
extern int memcmp(const void *s1, const void *s2, size_t n);

#endif


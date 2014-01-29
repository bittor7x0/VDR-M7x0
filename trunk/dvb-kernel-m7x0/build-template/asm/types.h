#ifndef _ASM_TYPES_H_FAKE
#define _ASM_TYPES_H_FAKE

typedef unsigned char __u8;
typedef signed char __s8;
typedef unsigned short __u16;
typedef signed short __s16;
typedef unsigned long __u32;
typedef signed long __s32;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef unsigned long long __u64;
#else
#error specify __u64 definition!!
#endif

#define u8  __u8
#define u16 __u16
#define u32 __u32
#define u64 __u64

#define s8  __s8
#define s16 __s16
#define s32 __s32


typedef unsigned long size_t;
typedef long ssize_t;

typedef long loff_t;

typedef long time_t;

#endif


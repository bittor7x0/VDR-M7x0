#ifndef _LINUX_TIME_H_FAKE_
#define _LINUX_TIME_H_FAKE_


#include <asm/types.h>


struct timespec {
	time_t  tv_sec;         /* seconds */
	long    tv_nsec;        /* nanoseconds */
};


#endif


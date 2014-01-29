#ifndef _LINUX_TIMER_H_FAKE_
#define _LINUX_TIMER_H_FAKE_

#include <linux/list.h>

struct timer_list {
        struct list_head list;
        unsigned long expires;
        unsigned long data;
        void (*function)(unsigned long);
};

extern void init_timer(struct timer_list * timer);
extern void add_timer(struct timer_list * timer);
extern int del_timer(struct timer_list * timer);


#endif


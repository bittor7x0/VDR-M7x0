#ifndef _LINUX_SCHED_H_FAKE_
#define _LINUX_SCHED_H_FAKE_

#include <linux/wait.h>

#define FASTCALL(x) x

extern void *current;
extern long jiffies;

#warning adjust jiffies according to your architecture!!
#define HZ 100

extern void wake_up(wait_queue_head_t *q);
extern void wake_up_interruptible(wait_queue_head_t *q);
extern void interruptible_sleep_on(wait_queue_head_t *q);
extern void interruptible_sleep_on_timeout(wait_queue_head_t *q, long timeout);

#warning implement wait_event_interruptible() macro!!
#define wait_event_interruptible(wq, condition) 0


#warning adjust TASK_INTERRUPTIBLE definition!!
#define TASK_INTERRUPTIBLE 1

#warning implement set_current_state() macro!!
#define set_current_state(state)

extern signed long schedule_timeout(signed long timeout);


#warning implement signal_pending()!!
static inline int signal_pending(void *p) { return 0; }


#endif


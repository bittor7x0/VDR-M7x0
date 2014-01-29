#ifndef _LINUX_WAIT_H_FAKE_
#define _LINUX_WAIT_H_FAKE_

typedef struct __wait_queue_head {
} wait_queue_head_t;

extern void init_waitqueue_head(wait_queue_head_t *q);

#endif


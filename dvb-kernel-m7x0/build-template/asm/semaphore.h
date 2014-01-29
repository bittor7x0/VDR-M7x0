#ifndef _ASM_SEMAPHORE_H_FAKE_
#define _ASM_SEMAPHORE_H_FAKE_


struct semaphore {};

#warning implement DECLARE_MUTEX() macro!!
#define DECLARE_MUTEX(mutex) struct semaphore mutex = {}

extern int sema_init(struct semaphore *mutex, int val);

extern void init_MUTEX (struct semaphore *sem);

extern int up(struct semaphore *mutex);
extern int down(struct semaphore *mutex);
extern int down_interruptible(struct semaphore *mutex);

#endif

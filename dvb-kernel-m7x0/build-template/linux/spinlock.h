#ifndef _ASM_SPINLOCK_H_FAKE_
#define _ASM_SPINLOCK_H_FAKE_

typedef struct {
	volatile unsigned int lock;
} spinlock_t;


#define SPIN_LOCK_UNLOCKED(spinlock_t) { 1 }

#warning adjust spin_lock_init() according to your OS!!
#define spin_lock_init(x)       do { (*(x)).lock = 1; } while(0)

#warning implement spin_lock()!!
#define spin_lock(x)

#warning implement spin_unlock()!!
#define spin_unlock(x)

#warning implement spin_lock_irq()!!
#define spin_lock_irq(x)

#warning implement spin_unlock_irq()!!
#define spin_unlock_irq(x)

#warning implement spin_lock_irqsave()!!
#define spin_lock_irqsave(x...)

#warning implement spin_unlock_irqrestore()!!
#define spin_unlock_irqrestore(x...)

#endif


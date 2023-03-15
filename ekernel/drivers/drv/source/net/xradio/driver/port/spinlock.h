#ifndef __LINUX_SPINLOCK_TYPES_H
#define __LINUX_SPINLOCK_TYPES_H

#include "xr_types.h"
#include "semaphore.h"
#include "irqflags.h"

typedef struct spinlock {
	xr_drv_sema_t semp;
} spinlock_t;

static XR_INLINE void spin_lock_init(spinlock_t *lock)
{
	xr_drv_sema_init(&lock->semp, 1);
}

static XR_INLINE void spin_lock(spinlock_t *lock)
{
	xr_drv_sema_wait(&lock->semp, XR_DRV_WAIT_FOREVER);
}

static XR_INLINE void spin_unlock(spinlock_t *lock)
{
	xr_drv_sema_release(&lock->semp);
}

static XR_INLINE void spin_lock_exit(spinlock_t *lock)
{
	xr_drv_sema_destroy(&lock->semp);
}

#define spin_lock_bh(l)		spin_lock(l)
#define spin_unlock_bh(l)	spin_unlock(l)

#define xr_spin_lock_irqsave(lock, flags)		raw_local_irq_save(flags)
#define xr_spin_unlock_irqrestore(lock, flags)	raw_local_irq_restore(flags)

#endif /* __LINUX_SPINLOCK_TYPES_H */

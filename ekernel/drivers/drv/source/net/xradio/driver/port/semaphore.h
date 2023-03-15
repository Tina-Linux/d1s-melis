/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Distributed under the terms of the GNU GPL, version 2
 *
 * Please see semaphore.c for documentation of these functions
 */
#ifndef __LINUX_SEMAPHORE_H
#define __LINUX_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "xr_types.h"

#if defined(__MELIS__)

#elif (defined(__CONFIG_ARCH_STM32))
typedef struct xr_semaphore xr_drv_sema_t

#include "xr_rtos/xr_semaphore.h"

typedef struct xr_semaphore xr_drv_sema_t;

#define xr_drv_sema_init(sem, val) xr_semaphore_init(sem, val)
#define xr_drv_sema_wait(sem, ms) xr_semaphore_wait(sem, ms)
#define xr_drv_sema_release(sem) xr_semaphore_release(sem)
#define xr_drv_sema_destroy(sem) xr_semaphore_destroy(sem)

#elif (defined(__CONFIG_CHIP_XRADIO))
#include "kernel/os/os_semaphore.h"

typedef XR_OS_Semaphore_t xr_drv_sema_t;

#define xr_drv_sema_init(sem, val) XR_OS_SemaphoreCreate(sem, val, XR_DRV_SEMAPHORE_MAX_COUNT)
#define xr_drv_sema_destroy(sem) XR_OS_SemaphoreDelete(sem)
#define xr_drv_sema_release(sem) XR_OS_SemaphoreRelease(sem)

static __always_inline int xr_drv_sema_wait(xr_drv_sema_t *sem, XR_DRV_Time_t ms)
{
	return (XR_OS_SemaphoreWait(sem, ms) == XR_DRV_OK ? 1 : 0);
}

#elif (defined(CONFIG_OS_RTTHREAD) || defined(CONFIG_OS_YUNOS) || (defined(CONFIG_OS_NUTTX)))
#include "kernel/os/os_semaphore.h"
#ifdef __CONFIG_ETF
#include "errno.h"
#endif

typedef XR_OS_Semaphore_t xr_drv_sema_t;

#define xr_drv_sema_init(sem, val) XR_OS_SemaphoreCreate(sem, val, XR_DRV_SEMAPHORE_MAX_COUNT)
#define xr_drv_sema_destroy(sem) XR_OS_SemaphoreDelete(sem)
#define xr_drv_sema_release(sem) XR_OS_SemaphoreRelease(sem)

static __always_inline int xr_drv_sema_wait(xr_drv_sema_t *sem, XR_DRV_Time_t ms)
{
	return (XR_OS_SemaphoreWait(sem, ms) == XR_DRV_OK ? 1 : 0);
}
/*
#elif (defined(CONFIG_OS_NUTTX))

#include <semaphore.h>

typedef sem_t xr_drv_sema_t;

int nuttx_sem_timeout(xr_drv_sema_t *sem, XR_DRV_Time_t ms);

#define xr_drv_sema_init(sem, val) nxsem_init(sem,0,val)
#define xr_drv_sema_wait(sem, ms)  nuttx_sem_timeout(sem,ms)
#define xr_drv_sema_release(sem) nxsem_post(sem)
#define xr_drv_sema_destroy(sem) nxsem_destroy(sem)
*/
#endif

/**
 * down - acquire the semaphore
 * @sem: the semaphore to be acquired
 *
 * Acquires the semaphore.  If no more tasks are allowed to acquire the
 * semaphore, calling this function will put the task to sleep until the
 * semaphore is released.
 *
 * Use of this function is deprecated, please use down_interruptible() or
 * down_killable() instead.
 */
#define down(l)                 xr_drv_sema_wait(l, XR_DRV_WAIT_FOREVER)

/**
 * up - release the semaphore
 * @sem: the semaphore to release
 *
 * Release the semaphore.  Unlike mutexes, up() may be called from any
 * context and even by tasks which have never called down().
 */
#define up(l)                   xr_drv_sema_release(l)

/**
 * down_trylock - try to acquire the semaphore, without waiting
 * @sem: the semaphore to be acquired
 *
 * Try to acquire the semaphore atomically.  Returns 0 if the mutex has
 * been acquired successfully or 1 if it it cannot be acquired.
 *
 * NOTE: This return value is inverted from both spin_trylock and
 * mutex_trylock!  Be careful about this when converting code.
 *
 * Unlike mutex_trylock, this function can be used from interrupt context,
 * and the semaphore can be released by any task or interrupt.
 */
#define down_trylock(l)         (xr_drv_sema_wait(l, 0) <= 0)

/**
 * down_timeout - acquire the semaphore within a specified time
 * @sem: the semaphore to be acquired
 * @jiffies: how long to wait before failing
 *
 * Attempts to acquire the semaphore.  If no more tasks are allowed to
 * acquire the semaphore, calling this function will put the task to sleep.
 * If the semaphore is not released within the specified number of jiffies,
 * this function returns -ETIME.  It returns 0 if the semaphore was acquired.
 */
#define down_timeout(l, ms)    (xr_drv_sema_wait(l, ms) <= 0)

#ifdef __cplusplus
}
#endif

#endif /* __LINUX_SEMAPHORE_H */

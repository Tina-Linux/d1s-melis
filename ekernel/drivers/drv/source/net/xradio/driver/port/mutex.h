/*
 * Mutexes: blocking mutual exclusion locks
 *
 * started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005, 2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * This file contains the main data structure and API definitions.
 */
#ifndef __LINUX_MUTEX_H
#define __LINUX_MUTEX_H

#include "semaphore.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef xr_drv_sema_t xr_drv_mtx_t;

#define xr_drv_mutex_init(mtx)  do {xr_drv_sema_init(mtx, 1); } while (0)
#define xr_drv_mutex_destroy(mtx)  xr_drv_sema_destroy(mtx)
#define xr_drv_mutex_lock(mtx) do {xr_drv_sema_wait(mtx, XR_DRV_WAIT_FOREVER); } while (0)
#define xr_drv_mutex_trylock(mtx) (xr_drv_sema_wait(mtx, 0) >= 1 ? 1 : 0)
#define xr_drv_mutex_unlock(mtx) do {xr_drv_sema_release(mtx); } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __LINUX_MUTEX_H */

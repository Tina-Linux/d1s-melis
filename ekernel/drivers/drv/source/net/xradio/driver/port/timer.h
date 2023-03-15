#ifndef _LINUX_TIMER_H
#define _LINUX_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MELIS__

#elif (defined(__CONFIG_ARCH_STM32))

#elif (defined(__CONFIG_CHIP_XRADIO))

#define XR_DRV_TIMER_ONCE XR_OS_TIMER_ONCE

typedef XR_OS_Timer_t XR_DRV_Timer_t;

#define xr_drv_timer_init XR_OS_TimerCreate
#define xr_drv_timer_destroy XR_OS_TimerDelete
#define xr_drv_timer_active XR_OS_TimerIsActive

#elif (defined(__CONFIG_CHIP_XRADIO))

#include "kernel/os/os_timer.h"

#elif (defined(CONFIG_OS_RTTHREAD) || defined(CONFIG_OS_YUNOS) || (defined(CONFIG_OS_NUTTX)))

#include "kernel/os/os_timer.h"

typedef XR_OS_Timer_t XR_DRV_Timer_t;

#define XR_DRV_TimerCallback_t	XR_OS_TimerCallback_t
#define XR_DRV_TimerType		XR_OS_TimerType
#define XR_DRV_TIMER_ONCE		XR_OS_TIMER_ONCE
#define XR_DRV_TIMER_PERIODIC	XR_OS_TIMER_PERIODIC

#define xr_drv_timer_init		XR_OS_TimerCreate
#define xr_drv_timer_destroy	XR_OS_TimerDelete
#define xr_drv_timer_active		XR_OS_TimerIsActive
#define xr_drv_timer_change_period		XR_OS_TimerChangePeriod
#define xr_drv_timer_stop		XR_OS_TimerStop
/*
#elif (defined(CONFIG_OS_NUTTX))

#include <signal.h>
#define  MY_TIMER_SIGNAL SIGUSR1 //用户自定义信号???

#define NX_TIMER_LOGE sinfo

typedef uint32_t nx_time_t;

typedef enum {
	NX_TIMER_ONCE = 0,
	NX_TIMER_PERIODIC = 1
}nx_timer_type_t;

typedef void (*nx_timer_cb_t)(int signo, FAR siginfo_t * info,
		FAR void *ucontext);

typedef struct nuttx_timer {
	timer_t handle;
	nx_timer_type_t type;
	nx_time_t init_time_ms;
}nx_timer_t;

#define MAX_TIME_DELAY  0xffffffffU

typedef nx_timer_t XR_DRV_Timer_t;

int nx_timer_delete(nx_timer_t *timer);
int nx_timer_stop(nx_timer_t *timer);
int nx_timer_start(nx_timer_t *timer);
int nx_timer_change_period(nx_timer_t *timer, nx_time_t ms);
int nx_timer_create(nx_timer_t *timer, nx_timer_type_t type,
		nx_timer_cb_t handler, void *ptr_param, nx_time_t ms);
int nx_timer_is_active(nx_time_t *timer);

#define XR_DRV_TimerCallback_t	nx_timer_cb_t
#define XR_DRV_TimerType		nx_timer_type_t
#define XR_DRV_TIMER_ONCE		NX_TIMER_ONCE
#define XR_DRV_TIMER_PERIODIC	NX_TIMER_PERIODIC

#define xr_drv_timer_init			nx_timer_create
#define xr_drv_timer_destroy		nx_timer_delete
#define xr_drv_timer_active			nx_timer_is_active
#define xr_drv_timer_change_period	nx_timer_change_period
#define xr_drv_timer_stop			nx_timer_stop
*/
#endif /* __MELIS__ */

#ifdef __cplusplus
}
#endif

#endif /* _LINUX_TIMER_H */

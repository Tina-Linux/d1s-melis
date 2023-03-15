#ifndef _XR_DELAY_H_
#define _XR_DELAY_H_

#ifdef __MELIS__

void xr_drv_msleep(unsigned int ms);

#elif (defined(__CONFIG_ARCH_STM32))

#define xr_drv_msleep(msec) xr_msleep(msec)

#elif (defined(__CONFIG_CHIP_XRADIO))

#include "kernel/os/os_time.h"

#define xr_drv_msleep(msec) XR_OS_MSleep(msec)

#else
#include "kernel/os/os_time.h"
#define xr_drv_msleep(msec) XR_OS_MSleep(msec)
#endif

#endif /* _XR_GPIO_H_ */

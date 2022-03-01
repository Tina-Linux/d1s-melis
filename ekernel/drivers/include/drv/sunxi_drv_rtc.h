#ifndef _SUNXI_DRV_RTC_H
#define _SUNXI_DRV_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtthread.h>
#include "../hal/sunxi_hal_rtc.h"

typedef struct sunxi_hal_driver_rtc
{
    int (*initialize)(void);
    int (*uninitialize)(void);
    int (*control)(hal_rtc_transfer_cmd_t cmd, void *args);
} const sunxi_hal_driver_rtc_t;

typedef struct sunxi_driver_rtc
{
    struct rt_device   base;
    int32_t            dev_id;
    const void        *hal_drv;
} sunxi_driver_rtc_t;

#ifdef __cplusplus
}
#endif


#endif /* _SUNXI_DRV_RTC_H */

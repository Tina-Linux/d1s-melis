/*
 * ===========================================================================================
 *
 *       Filename:  sunxi_drv_eise.h
 *
 *    Description:  sunxi_driver headfile.
 *
 *        Version:  Melis3.0
 *         Create:  2020-01-08 17:47:21
 *       Revision:  none
 *       Compiler:
 *
 *         Author:  ganqiuye(ganqiuye@allwinnertech.com)
 *   Organization:  SWC-MPD
 *  Last Modified:
 *
 * ===========================================================================================
 */
#ifndef SUNXI_DRV_EISE_H
#define SUNXI_DRV_EISE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>

typedef struct sunxi_driver_eise
{
    struct rt_device   base;
    int32_t            dev_id;
    const void        *hal_drv;
} sunxi_driver_eise_t;

int sunxi_driver_eise_init(void);

#ifdef __cplusplus
}
#endif

#endif

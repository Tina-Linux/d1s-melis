/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                             ir keyboard driver
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File    : drv_irkey_i.h
* By      : jerry
* Version : V1.00
*********************************************************************************************************
*/
#ifndef  _DRV_POWER_I_H_
#define  _DRV_POWER_I_H_

//#include "epdk.h"
#include <mod_defs.h>
#include <sys_clock.h>
#include <elibs_stdio.h>
#include <rtthread.h>
#include <aw_list.h>

//define fm module type
#define FM_MODULE_TYPE_RDA5820      (1)
#define FM_MODULE_TYPE_TEA5767      (2)
#define FM_MODULE_TYPE_RDA5807P     (3)
#define FM_MODULE_TYPE_RDA5807SS    (4)//兼容RDA5807SP
#define FM_MODULE_TYPE_RDA4702      (5)
#define FM_MODULE_TYPE_RDA5807      (6)//兼容RDA5807P、RDA5807SP、RDA5807SS、RDA5807H系列、RDA5807N系列
#define FM_MODULE_TYPE_RDA5807M     (7)//兼容RDA5807P、RDA5807SP、RDA5807SS、RDA5807H系列、RDA5807N系列
#define FM_MODULE_TYPE_ALL     (8)


//config fm module type
#define FM_MODULE_TYPE      FM_MODULE_TYPE_ALL


typedef struct 
{
    __u32           status;
    __hdle          amplify_hdl;
    int             twi_dev;
    int             pwm_dev;
} __fm_dev_t;

typedef struct 
{
    __u32           mid;
    __u32           used;
    __hdle          hReg;
    __fm_dev_t      fm_dev;
} __fm_drv_t;

extern  int   fiic;
extern  __dev_devop_t  fm_dev_ops;

int32_t DRV_FM_MInit(void);
int32_t DRV_FM_MExit(void);
__mp *DRV_FM_MOpen(__u32 mid, __u32 mode);
int32_t DRV_FM_MClose(__mp *mp);
uint32_t DRV_FM_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
uint32_t DRV_FM_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long DRV_FM_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer);


typedef struct sunxi_driver_fm
{
    struct rt_device   base;
    int32_t            dev_id;
    const void        *hal_drv;
} sunxi_driver_fm_t;

typedef struct sunxi_hal_driver_fm
{
    int32_t (*initialize)(void);
    int32_t (*uninitialize)(void);
    int32_t (*control)(int cmd, void *args);
}  sunxi_hal_driver_fm_t;


#endif /*_DRV_POWER_I_H_*/

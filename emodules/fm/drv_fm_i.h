/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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

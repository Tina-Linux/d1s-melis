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
#ifndef SUNXI_DRV_SPINOR_H
#define SUNXI_DRV_SPINOR_H

#include <stdint.h>
#include <blkpart.h>
#include <sunxi_hal_common.h>
#include <sunxi_hal_spinor.h>

typedef struct sunxi_hal_driver_spinor
{
    sunxi_hal_version_t (*get_version)(int32_t dev);
    sunxi_hal_spinor_capabilities_t (*get_capabilities)(void);
    int32_t (*initialize)(sunxi_hal_spinor_signal_event_t cb_event);
    int32_t (*uninitialize)(void);
    int32_t (*power_control)(sunxi_hal_power_state_e state);
    int32_t (*read_data)(uint32_t addr, void *data, uint32_t cnt);
    int32_t (*program_data)(uint32_t addr, const void *data, uint32_t cnt);
    int32_t (*erase_sector)(uint32_t addr, uint32_t len);
    int32_t (*erase_chip)(void);
    sunxi_hal_spinor_status_t (*get_status)(void);
    sunxi_hal_spinor_info *(*get_info)(void);
    void (* signal_event)(uint32_t event);
    int32_t (*control)(int32_t dev, uint32_t command, uint32_t arg);
} sunxi_hal_driver_spinor_t;

int sunxi_driver_spinor_init(void);
int sunxi_driver_spinor_deinit(void);

#endif  /*SUNXI_DRV_SPINOR_H*/

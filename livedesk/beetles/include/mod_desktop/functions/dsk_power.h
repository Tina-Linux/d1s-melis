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
#ifndef __DSK_POWER_H__
#define __DSK_POWER_H__
#include <kconfig.h>

typedef enum tag_power_level
{
    DSK_POWER_LEVEL_0 = 0,
    DSK_POWER_LEVEL_1,
    DSK_POWER_LEVEL_2,
    DSK_POWER_LEVEL_3,
    DSK_POWER_LEVEL_4,
    DSK_POWER_LEVEL_5,
    DSK_POWER_LEVEL_
} power_level_e;

typedef enum tag_power_state
{
    DSK_POWER_BATTARY_CHARGE = 1,
    DSK_POWER_BATTARY_FULL,
    DSK_POWER_BATTARY_DISCHARGE,
    DSK_POWER_BATTARY_NOT_EXIST,
    DSK_POWER_BATTERY_DCIN_NOT_ENOUGH,
    DSK_POWER_BATTARY_
} power_state_e;



__s32 dsk_power_dev_init(void); 

__s32 dsk_power_dev_uninit(void);

__s32 dsk_power_get_battery_level(power_level_e *level);        // 剩余电量level

__s32 dsk_power_get_voltage_level(power_level_e *level);        // 工作电压level

__s32 dsk_power_get_battary_state(power_state_e *state);        // 查询 battary 状态

__bool dsk_power_is_full(void);

//__s32 dsk_power_off(void);                                        // 请求桌面系统关机
__s32 dsk_power_set_off_time(__u32 time);                       // 设置自动关机时间 单位 1ms

__bool dsk_power_is_low(void);  /* 是否低电 */

void dsk_speaker_turnoff(void);

void dsk_speaker_turnon(void);

void dsk_speaker_resume(void);

__bool dsk_get_charge_state(void);

__s32 dsk_speak_init(void);

__s32 dsk_speak_exit(void);

#endif /* __DSK_POWER_H__ */

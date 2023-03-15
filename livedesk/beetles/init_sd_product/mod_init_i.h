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
#ifndef __MOD_INIT_I_H__
#define __MOD_INIT_I_H__
#include "mod_update.h"
//#include "sunxi_drv_watchdog.h"
#include "apps.h"

#define THEM_BIN_PATH   BEETLES_APP_ROOT"apps\\theme.bin"

#define NOTIFY_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_SendNotifyMessage(&exmsg); \
    }while(0)

#define SEND_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_SendMessage(&exmsg); \
    }while(0)

#define POST_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_PostMessage(&exmsg); \
    }while(0)

typedef struct
{
    __u32           mid;
    __u32           init_tid;
} __init_mod_t;

__s32 INIT_MInit(void);
__s32 INIT_MExit(void);
__mp *INIT_MOpen(__u32 mid, __u32 mode);
__s32 INIT_MClose(__mp *mp);
__u32 INIT_MRead(void *pdata, __u32 size, __u32 n, __mp *mp);
__u32 INIT_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp);
long INIT_MIoctrl(__mp *mp, uint32_t cmd, int32_t aux, void *pbuffer);
#endif /* __MOD_INIT_I_H__ */

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
#ifndef     __FSYS_TIMER_H__
#define     __FSYS_TIMER_H__

#include "buffer_head.h"
#include "list.h"
#include "page_pool.h"
#include "fsys_debug.h"

struct timer_cfg_s
{
    int on;
};

typedef enum ttype_e
{
    diodevicetime = 0,
    bufdevicetime,
    memcpytime,
    getblktime,
    fatcachetime,
    scanfattime,
    oneaccesstime,
    bmaptime,
    hashtime,
    shrinkcachetime,
    partiotime,

    invalidtime = 0x80,
    alltime = 0xff,
} ttype_t;

typedef struct item_s
{
    ttype_t      type;
    __u32        value;
    __u32        start;
    const char  *info;
} titem_t;

typedef struct timers_s
{
    int             count;
    titem_t         item[12];
} timers_t;

#if     FSYS_DEBUG_ON

extern void     debug_timerclr(ttype_t target);
extern void     debug_timerstart(ttype_t target);
extern __u32    debug_timerend(ttype_t target);
extern void     debug_timerprint(char *fmt, ...);
extern void     debug_timershow(ttype_t target);
extern __bool   fs_timer_parse_option(const char *option);

#else

#define debug_timerclr(target)
#define debug_timerstart(target)
#define debug_timerend(target)
#define debug_timerprint(fmt, ...)
#define debug_timershow(target)
#define fs_timer_parse_option(option)

#endif  /* FSYS_DEBUG_ON */


#endif  /* __FSYS_TIMER_H__ */
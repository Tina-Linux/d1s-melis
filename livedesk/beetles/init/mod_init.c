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
#include "mod_init_i.h"
#include <log.h>
#include <kconfig.h>

__init_mod_t init_data;

extern void application_init_process(void *arg);

__s32 INIT_MInit(void)
{
    return EPDK_OK;
}

__s32 INIT_MExit(void)
{
    return EPDK_OK;
}

__mp *INIT_MOpen(__u32 mid, __u32 mod)
{
    __inf("----------------------INIT_MOpen --------------------------\n");
    msg_emit_init();

    init_data.mid      = mid;
    init_data.init_tid = esKRNL_TCreate(application_init_process, NULL, 0x10000, KRNL_priolevel3);
    esKRNL_TaskNameSet(init_data.init_tid, "initprocess");
    if (init_data.init_tid == 0)
    {
        __err(" application_init_process main thread create error\n");
        return NULL;
    }

    return (__mp *)&init_data;
}

__s32 INIT_MClose(__mp *mp)
{
    while (esKRNL_TDelReq(init_data.init_tid) != OS_TASK_NOT_EXIST)
    {
        esKRNL_TimeDly(1);
    }

    return EPDK_OK;
}

__u32 INIT_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

__u32 INIT_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

long INIT_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    return EPDK_OK;
}

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
#include "common_module_i.h"
#include <kapi.h>
#include <string.h>

extern AppletPlugin common_applet_plugin_interface__;

__s32 Common_MInit(void)
{
    return EPDK_OK;
}

__mp *Common_MOpen(__u32 mid, __u32 mode)
{
    __common_t *common = NULL;
    common = (__common_t *)esMEMS_Malloc(0, sizeof(__common_t));

    if (common == NULL)
    {
        __err("common module malloc err");
        return NULL;
    }

    memset(common, 0, sizeof(__common_t));
    common->mid     = mid;
    common->iface   = &common_applet_plugin_interface__;
    return (__mp *)common;
}

__u32 Common_MRead(void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

__u32 Common_MWrite(const void *pdata, __u32 size, __u32 n, __mp *mp)
{
    return size * n;
}

long Common_MIoctrl(__mp *mp, __u32 cmd, __s32 aux, void *pbuffer)
{
    __common_t *com_ctr = (__common_t *)mp;

    switch (cmd)
    {
        case APPLET_CMD_START:
            (*com_ctr->iface->start)((Activity *)pbuffer);
            break;

        case APPLET_CMD_SUSPEND:
            (*com_ctr->iface->suspend)((Activity *)pbuffer);
            break;

        case APPLET_CMD_RESUME:
            (*com_ctr->iface->resume)((Activity *)pbuffer);
            break;

        case APPLET_CMD_BACKGROUND:
            (*com_ctr->iface->background)((Activity *)pbuffer);
            break;

        case APPLET_CMD_STOP:
            (*com_ctr->iface->stop)((Activity *)pbuffer);
            break;

        default:
            __msg("unknown command ");
            break;
    }

    return EPDK_OK;
}

__s32 Common_MClose(__mp *mp)
{
    __common_t *com_ctr = (__common_t *)mp;
    esMEMS_Mfree(0, com_ctr);
    return EPDK_OK;
}

__s32 Common_MExit(void)
{
    return EPDK_OK;
}


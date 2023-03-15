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
#include "lyrwin_i.h"
#include <kconfig.h>
#ifdef CONFIG_SOC_SUN3IW1
#include <mod_display.h>
#endif

/*
**********************************************************************************************************************
*                                               GUI_LyrSetBottom
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
__s32 GUI_LyrWinSetBottom(H_LYR h_lyr)
{
    __s32                    ret = EPDK_FAIL;
    __gui_lyrwin_t           *lyr_attr   = (__gui_lyrwin_t *)h_lyr;
    __u32 dispbuffer[3];
    //////////////因DE2.0图层策略问题，暂不再支持set top操作
    //de2.0在同一通道内不支持图层优先级修改
    //de2.0图层优先级的修改只支持不同通道间
    //return EPDK_OK;
#if GUI_CHECK_ARG_EN > 0

    if (!lyr_attr)
    {
        __wrn("h_lyr empty!\n");
        return EPDK_FAIL;
    }

#endif
    GUI_LyrLock();

    if (lyr_attr->h_hwlyr)
    {
        if (lyr_attr->type == GUI_LYRWIN_NORMAL)
        {
            /*modified by Derek,2010-12-9 19:26:03*/
            dispbuffer[0] = (unsigned long)(lyr_attr->h_hwlyr);
            dispbuffer[1] = 0;
            dispbuffer[2] = 0;
#ifdef CONFIG_SOC_SUN3IW1
            ret = eLIBs_fioctrl(lyr_attr->devfp, MOD_DISP_CMD_LAYER_BOTTOM, SEL_SCREEN, (void *)dispbuffer); //end modified

            if (ret != EPDK_OK)
            {
                __wrn("layer set top failed!\n");
            }

#endif
        }
        else
        {
            __wrn("don't support sprite!\n");
        }

        if (lyr_attr->status == GUI_LYRWIN_STA_ON)
        {
            Layer_ListSetBottom(h_lyr);
        }
    }

    GUI_LyrUnlock();
    return ret;
}

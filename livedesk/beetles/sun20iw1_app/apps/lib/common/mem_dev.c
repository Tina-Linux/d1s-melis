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
#include <log.h>
#include "mem_dev.h"

//不适合UI多线程
//create 和 delete必须成对调用

static GUI_MEMDEV_Handle g_memdev_hdl = NULL;

static __s32 __com_memdev_create(H_LYR hlyr, __s32 x, __s32 y, __s32 w, __s32 h)
{
    if (g_memdev_hdl)
    {
        __err("memdev already exist...\n");
        return EPDK_FAIL;
    }

    if (!hlyr)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(hlyr);
    g_memdev_hdl = GUI_MEMDEV_Create(x, y, w, h);

    if (!g_memdev_hdl)
    {
        return EPDK_FAIL;
    }

    GUI_MEMDEV_Select(g_memdev_hdl);
    return EPDK_OK;
}

__s32 com_memdev_create(H_LYR hlyr)
{
    __s32 ret = 0;
    RECT rect = {0};
    GUI_LyrWinGetScnWindow(hlyr, &rect);
    rect.x = 0;
    rect.y = 0;
    ret = __com_memdev_create(hlyr, rect.x, rect.y, rect.width, rect.height);
    return ret;
}

__s32 com_memdev_create_ex(H_LYR hlyr, __s32 x, __s32 y, __s32 w, __s32 h)
{
    __s32 ret;
    ret = __com_memdev_create(hlyr, x, y, w, h);
    return ret;
}

__s32 com_memdev_delete(void)
{
    if (!g_memdev_hdl)
    {
        //////__err("g_memdev_hdl is null...\n");   ///112350
        return EPDK_FAIL;
    }

    GUI_MEMDEV_CopyToLCD(g_memdev_hdl);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(g_memdev_hdl);
    g_memdev_hdl = NULL;
    return EPDK_OK;
}

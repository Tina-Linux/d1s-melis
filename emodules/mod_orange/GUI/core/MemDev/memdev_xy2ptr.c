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
#include "gui_core_memdev_private.h"

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       modul internal routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV__XY2PTR
*/
void *GUI_MEMDEV__XY2PTR(int x, int y)
{
    GUI_MEMDEV *pDev = GUI_MEMDEV_H2P(GUI_Context.hDevData);
    U8 *pData = (U8 *)(pDev + 1);
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL

    if ((x >= pDev->x0 + pDev->XSize) | (x < pDev->x0) | (y >= pDev->y0 + pDev->YSize) | (y < pDev->y0))
    {
        GUI_DEBUG_ERROROUT2("GUI_MEMDEV__XY2PTR: parameters out of bounds", x, y);
    }

#endif
    pData += (y - pDev->y0) * pDev->BytesPerLine;
    return pData + (x - pDev->x0) * (pDev->BitsPerPixel / 8);
}

/*********************************************************************
*
*       GUI_MEMDEV__XY2PTREx
*/
void *GUI_MEMDEV__XY2PTREx(GUI_MEMDEV *pDev, int x, int y)
{
    U8 *pData = (U8 *)(pDev + 1);
    pData += y * pDev->BytesPerLine;
    return pData + x * (pDev->BitsPerPixel / 8);
}

#else

void GUIDEV_XY2PTR_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/

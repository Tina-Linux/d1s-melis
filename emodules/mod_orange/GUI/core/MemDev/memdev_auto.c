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
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_CreateAuto
*/
int GUI_MEMDEV_CreateAuto(GUI_AUTODEV *pAutoDev)
{
    pAutoDev->FirstCall = 1;
    return 0;
}

/*********************************************************************
*
*       GUI_MEMDEV_DeleteAuto
*/
void GUI_MEMDEV_DeleteAuto(GUI_AUTODEV *pAutoDev)
{
    GUI_USE_PARA(pAutoDev);
}

/*********************************************************************
*
*       GUI_MEMDEV_DrawAuto
*/
int GUI_MEMDEV_DrawAuto(GUI_AUTODEV *pAutoDev, GUI_AUTODEV_INFO *pAutoDevInfo, GUI_CALLBACK_VOID_P *pfDraw, void *pData)
{
    GUI_RECT rTemp;
    GUI_HMEM hMem = GUI_MEASDEV_Create();

    if (!hMem)
    {
        return 1;
    }

    GUI_MEASDEV_Select(hMem);

    /* Measure area to draw */
    if (pAutoDev->FirstCall)
    {
        /* Measure complete area */
        pAutoDevInfo->DrawFixed = 1;
        pfDraw(pData);
        GUI_MEASDEV_GetRect(hMem, &pAutoDev->rView);
        /* Measure variable area only */
        GUI_MEASDEV_ClearRect(hMem);
        pAutoDevInfo->DrawFixed = 0;
        pfDraw(pData);
        GUI_MEASDEV_GetRect(hMem, &pAutoDev->rPrev);
    }
    else
    {
        /* Measure variable area only */
        pfDraw(pData);
        GUI_MEASDEV_GetRect(hMem, &pAutoDev->rView);
        rTemp = pAutoDev->rView;
        GUI_MergeRect(&pAutoDev->rView, &pAutoDev->rView, &pAutoDev->rPrev);
        pAutoDev->rPrev = rTemp;
    }

    GUI_MEASDEV_Delete(hMem);
    /* Do the actual drawing */
    pAutoDevInfo->DrawFixed = 1;

    if (GUI_MEMDEV_Draw(&pAutoDev->rView, pfDraw, pData, 0, GUI_MEMDEV_NOTRANS))
    {
        return 1;
    }

    pAutoDevInfo->DrawFixed = 0;
    pAutoDev->FirstCall = 0;
    return 0;
}

#else

void GUIDEV_Auto(void) {} /* avoid empty object files */

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/

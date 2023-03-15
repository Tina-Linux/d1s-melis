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
#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispNextLine
*/
void GUI_DispNextLine(void)
{
    GUI_LOCK();
    GUI_Context.DispPosY[GUI_Context.SelLayer] += GUI_GetFontDistY();
    GUI_Context.DispPosX[GUI_Context.SelLayer]  = GUI_Context.LBorder;
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GL_DispChar
*/
void GL_DispChar(U16 c)
{
    /* check for control characters */
    if (c == '\n')
    {
        GUI_DispNextLine();
    }
    else
    {
        if (c != '\r')
        {
            GUI_LOCK();
            //c = eLIBs_GB2312_to_Unicode(c);
            GUI_Context.pAFont->pfDispChar(c);
            //if (GUI_pfDispCharStyle) {
            //GUI_pfDispCharStyle(c);
            //}
            GUI_UNLOCK();
        }
    }
}

/*********************************************************************
*
*       GUI_GetYAdjust
*
* Returns adjustment in vertical (Y) direction
*
* Note: The return value needs to be subtracted from
*       the y-position of the character.
*/
int GUI_GetYAdjust(void)
{
    int r = 0;
    GUI_LOCK();

    switch (GUI_Context.TextAlign & GUI_TA_VERTICAL)
    {
        case GUI_TA_BOTTOM:
            r = GUI_Context.pAFont->YSize - 1;
            break;

        case GUI_TA_VCENTER:
            r = GUI_Context.pAFont->YSize / 2;
            break;

        case GUI_TA_BASELINE:
            r = GUI_Context.pAFont->YSize / 2;
    }

    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetFontDistY
*/
int GUI_GetFontDistY(void)
{
    int r;
    GUI_LOCK();
    //  r = GUI_Context.pAFont->YDist;
    r = GUI_Context.pAFont->YDist * GUI_Context.pAFont->YMag;

    if (GUI_Context.FontMode == LCD_FONTMODE_32BPPMAG2 || GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME_MAG)
    {
        r = r * 2;
    }

    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetCharDistX
*/
int GUI_GetCharDistX(U16 c)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.pAFont->pfGetCharDistX(c);
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/

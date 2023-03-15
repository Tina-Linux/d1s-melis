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
/*
**********************************************************************************************************************
*                                               GUI_LyrMove
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
__s32 GUI_LyrMove(H_LYR h_lyr, __s32 x, __s32 y)
{
    __s32               ret = EPDK_FAIL;
    __gui_lyrwin_t     *p_lyr = (__gui_lyrwin_t *)h_lyr;
    RECT                rect;
#if GUI_CHECK_ARG_EN > 0

    if (!p_lyr)
    {
        __wrn("h_lyr empty!\n");
        return EPDK_FAIL;
    }

#endif
    GUI_Lock();
    GUI_LyrWinGetScnWindow(h_lyr, &rect);
    rect.x = rect.x + x;
    rect.y = rect.y + y;
    GUI_LyrWinSetScnWindow(h_lyr, (const RECT *)&rect);

    if (1)
    {
        __gui_win_t *p_win = GUI_WinH2P(p_lyr->ManWinZOrder.pTopMost->hWnd);
        __gui_msg_t msg;
        msg.id = GUI_MSG_LYR_MOVE;
        msg.h_srcwin = (H_WIN)0;
        msg.p_arg    = 0;

        while (p_win)
        {
            msg.h_deswin = (H_WIN)p_win;
            GUI_SendMessage(&msg);
            p_win = p_win->Next;
        }
    }

    GUI_Unlock();
    return ret;
}

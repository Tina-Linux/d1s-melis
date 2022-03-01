/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : lyr_move.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
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

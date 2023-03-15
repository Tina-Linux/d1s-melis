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
#ifndef __GUI_STATIC_C__
#define __GUI_STATIC_C__

#include "static_i.h"
#include <kconfig.h>

static __u32  last_key = 0xffffffff;

static __s32 StaticCtrlProc(__gui_msg_t  *msg);

/*
**********************************************************************************************************************
*                                               RegisterstaticControl
*
* author:
*
* date:             2009-8-4
*
* Description:      registerstaticcontrol
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool RegisterStaticControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_STATIC;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = StaticCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               staticCtrlProc
*
* author:
*
* date:             2009-8-4
*
* Description:      staticctrlproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 StaticCtrlProc(__gui_msg_t *msg)
{
    __u32                   dwStyle;
    static_data_t          *pData;
    dwStyle = GUI_WinGetStyle(msg->h_deswin);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            pData = (static_data_t *) orange_malloc(sizeof(static_data_t));

            if (pData == NULL)
            {
                return ORANGE_FAIL;
            }

            pData->status       = 0;
            pData->hmem         = 0;
            pData->str_move     = 0;
            pData->data         = (unsigned long)GUI_WinGetAttr(msg->h_deswin);
            GUI_CtrlWinSetAddData(msg->h_deswin, (unsigned long)pData);
            return 0;

        case GUI_MSG_DESTROY:
            pData = (static_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->str_move)
            {
                GUI_LongStringDelete((__hdle)(pData->str_move));
            }

            orange_mfree((void *)GUI_CtrlWinGetAddData(msg->h_deswin));
            return 0;

        case GUI_MSG_ENABLE:
            if (msg->dwAddData1 && (dwStyle & WS_DISABLED))
            {
                dwStyle &= ~WS_DISABLED;
                GUI_WinSetStyle(msg->h_deswin, dwStyle);
            }
            else if (!msg->dwAddData1 && !(dwStyle & WS_DISABLED))
            {
                dwStyle |= WS_DISABLED;
                GUI_WinSetStyle(msg->h_deswin, dwStyle);
            }
            else
            {
                return 0;
            }

            return 0;

        case GUI_MSG_SET_UNFOCUS:
            if ((STATIC_DRAWSTATUS(msg->h_deswin) & STATICST_FOCUS))
            {
                STATIC_DRAWSTATUS(msg->h_deswin) &= (~STATICST_FOCUS);
                GUI_InvalidateRect(msg->h_deswin, NULL, ORANGE_TRUE);
            }

            return 0;

        case GUI_MSG_SET_FOCUS:
            if (!(STATIC_DRAWSTATUS(msg->h_deswin) & STATICST_FOCUS))
            {
                STATIC_DRAWSTATUS(msg->h_deswin) |= STATICST_FOCUS;
                GUI_InvalidateRect(msg->h_deswin, NULL, ORANGE_TRUE);
            }

            return 0;

        case GUI_MSG_KEY:
            switch (msg->dwAddData1)
            {
                /*case GUI_MSG_KEY_RISE:
                {
                    {
                        __gui_notify_msg_t      notify_msg;

                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = STN_CLICKED;
                        notify_msg.dwAddData= last_key;

                        GUI_NotifyParent (&notify_msg);
                    }
                }
                return 0;*/
                case GUI_MSG_KEY_DOWN:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = STN_CLICKED;
                            notify_msg.dwAddData = last_key;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = STN_PUSHED;
                            notify_msg.dwAddData = GUI_MSG_KEY_DOWN;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }
                }

                return 0;

                case GUI_MSG_KEY_ENTER:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = STN_CLICKED;
                            notify_msg.dwAddData = last_key;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = STN_PUSHED;
                            notify_msg.dwAddData = GUI_MSG_KEY_ENTER;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }
                }

                return 0;

                case GUI_MSG_KEY_RIGHT:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_UP:
                default:
                {
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = STN_CLICKED;
                            notify_msg.dwAddData = last_key;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        {
                            __gui_notify_msg_t      notify_msg;
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = STN_PUSHED;
                            notify_msg.dwAddData = msg->dwAddData1;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }
                }
                break;
            }

            /*if(msg->dwAddData1 != GUI_MSG_KEY_RISE)
            {
                last_key = msg->dwAddData1;
            }
            else
            {
                last_key = 0xffffffff;
            }*/
            last_key = msg->dwAddData1;
            return 0;

        case GUI_MSG_TOUCH:
        {
            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_DOWN:
                {
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = STN_PUSHED;
                        notify_msg.dwAddData = 0;
                        GUI_NotifyParent(&notify_msg);
                    }
                }
                break;

                case GUI_MSG_TOUCH_UP:
                {
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd     = msg->h_deswin;
                        notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode  = STN_CLICKED;
                        notify_msg.dwAddData = 0;
                        GUI_NotifyParent(&notify_msg);
                    }
                }
                break;
            }
        }

        return 0;

        case GUI_MSG_WIN_WAKEUP:
        case GUI_MSG_PAINT:
        {
            __gui_rect_t    rcrect;
            RECT            fbrect;
            //void          *pBMP;
            static_data_t   *static_data;
            static_para_t   *user_data;
            //__u32          color;
            //static_data = (static_para_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin))->data);
            static_data = (static_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            GUI_LyrWinSel(GUI_LyrP2H(GUI_WinGetLyrWin(msg->h_deswin)));
            GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);

            if (!static_data->hmem && (GUI_WinGetStyle(msg->h_deswin) & WS_MEMDEV))
            {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
                static_data->hmem = GUI_MEMDEV_Create(fbrect.x, fbrect.y, fbrect.width, fbrect.height);
#endif
            }

            SysRectToGuiRect(&fbrect, &rcrect);

            if (static_data->hmem)
            {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
                GUI_MEMDEV_Select(static_data->hmem);           // start memory device
#endif
            }

            if (STATIC_DRAWSTATUS(msg->h_deswin) & STATICST_FOCUS)
            {
                GUI_RECT        rect;
                GUI_SetBkColor(user_data->bk_color);
                GUI_ClearRect(rcrect.left, rcrect.top,
                              rcrect.right, rcrect.bottom);

                if (user_data->focus_bmp)
                {
                    GUI_BMP_Draw(user_data->focus_bmp, rcrect.left + user_data->bmp_pos.x,
                                 rcrect.top + user_data->bmp_pos.y);
                }

                if (user_data->alpha_en)
                {
                    GUI_OpenAlphaBlend();
                }

                GUI_CharSetToEncode(user_data->draw_code);
                GUI_SetFont(user_data->draw_font);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_SetColor(user_data->ftxt_color);
                rect.x0 = rcrect.left;
                rect.y0 = rcrect.top;
                rect.x1 = rcrect.right;
                rect.y1 = rcrect.bottom;

                if (!user_data->txt_align)
                {
                    GUI_DispStringAt(user_data->text,
                                     rcrect.left + user_data->text_pos.x,
                                     rcrect.top + user_data->text_pos.y);
                }
                else
                {
                    GUI_DispStringInRect(user_data->text, &rect, user_data->txt_align);
                }
            }
            else
            {
                GUI_RECT        rect;
                GUI_SetBkColor(user_data->bk_color);
                GUI_ClearRect(rcrect.left, rcrect.top,
                              rcrect.right, rcrect.bottom);

                if (user_data->unfocus_bmp)
                {
                    GUI_BMP_Draw(user_data->unfocus_bmp, rcrect.left + user_data->bmp_pos.x,
                                 rcrect.top + user_data->bmp_pos.y);
                }

                if (user_data->alpha_en)
                {
                    GUI_OpenAlphaBlend();
                }

                GUI_CharSetToEncode(user_data->draw_code);
                GUI_SetFont(user_data->draw_font);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_SetColor(user_data->uftxt_color);
                rect.x0 = rcrect.left;
                rect.y0 = rcrect.top;
                rect.x1 = rcrect.right;
                rect.y1 = rcrect.bottom;

                if (!user_data->txt_align)
                {
                    GUI_DispStringAt(user_data->text,
                                     rcrect.left + user_data->text_pos.x,
                                     rcrect.top + user_data->text_pos.y);
                }
                else
                {
                    GUI_DispStringInRect(user_data->text, &rect, user_data->txt_align);
                }
            }

            if (static_data->hmem)
            {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
                GUI_MEMDEV_CopyToLCD(static_data->hmem);            // copy from memory device to fb
                GUI_MEMDEV_Select(0);                      // stop memory device
                GUI_MEMDEV_Delete(static_data->hmem);
#endif
                static_data->hmem = 0;
            }

            if (user_data->alpha_en)
            {
                GUI_CloseAlphaBlend();
            }

            GUI_LyrWinFlushFB(GUI_WinGetLyrWin(msg->h_deswin));
        }

        return 0;

        default:
            break;
    }

    return GUI_CtrlWinDefaultProc(msg);
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetFocusBmp
*
* author:
*
* date:             2009-9-4
*
* Description:      设置static的focus bmp
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetFocusBmp(H_WIN hstatic, void *bmp)
{
    static_para_t   *user_data;

    if ((!hstatic) || (!bmp))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->focus_bmp = bmp;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetUnFocusBmp
*
* author:
*
* date:             2009-9-4
*
* Description:      设置static的un focus bmp
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetUnFocusBmp(H_WIN hstatic, void *bmp)
{
    static_para_t   *user_data;

    if ((!hstatic) || (!bmp))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->unfocus_bmp = bmp;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetText
*
* author:
*
* date:             2009-9-4
*
* Description:      设置static的Text
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetText(H_WIN hstatic, char *text)
{
    static_para_t   *user_data;

    if ((!hstatic) || (!text))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->text = text;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetUnFcsTxtColor
*
* author:
*
* date:             2009-9-4
*
* Description:      gui staticsetunfcstxtcolor
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetUnFcsTxtColor(H_WIN hstatic, __u32 color)
{
    static_para_t   *user_data;

    if ((!hstatic))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->uftxt_color = color;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetFcsTxtColor
*
* author:
*
* date:             2009-9-4
*
* Description:      gui staticsetfcstxtcolor
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetFcsTxtColor(H_WIN hstatic, __u32  color)
{
    static_data_t   *pData;
    static_para_t   *user_data;
    RECT            fbrect;
    __gui_rect_t    rcrect;

    if ((!hstatic))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    pData       = (static_data_t *)GUI_CtrlWinGetAddData(hstatic);
    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->ftxt_color = color;

    if (pData->str_move)
    {
        __show_info_move_t  str_move;
        GUI_WinGetClientFBRect(hstatic, &fbrect);
        SysRectToGuiRect(&fbrect, &rcrect);
        str_move.bkColor         = user_data->bk_color;
        str_move.fontColor       = user_data->ftxt_color;
        str_move.hlayer          = GUI_WinGetLyrWin(hstatic);
        str_move.string          = user_data->text;
        str_move.region.x        = rcrect.left;
        str_move.region.y        = rcrect.top;
        str_move.region.width    = rcrect.right - rcrect.left + 1;
        str_move.region.height   = rcrect.bottom - rcrect.top + 1;
        str_move.encode_id       = user_data->draw_code;
        str_move.pFont           = user_data->draw_font;
        str_move.bmp             = NULL;
        str_move.align           = user_data->txt_align;
        str_move.alpha_en        = user_data->alpha_en;
        GUI_LongStringReset((__hdle)(pData->str_move), &str_move);
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetBmpPos
*
* author:
*
* date:             2009-9-4
*
* Description:      gui staticsetbmppos
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetBmpPos(H_WIN hstatic, __pos_t *pos)
{
    static_para_t   *user_data;

    if ((!hstatic) || (!pos))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->bmp_pos.x = pos->x;
    user_data->bmp_pos.y = pos->y;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetTxtPos
*
* author:
*
* date:             2009-9-4
*
* Description:      gui staticsettxtpos
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetTxtPos(H_WIN hstatic, __pos_t *pos)
{
    static_para_t   *user_data;

    if ((!hstatic) || (!pos))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->text_pos.x = pos->x;
    user_data->text_pos.y = pos->y;
    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticStartMove
*
* author:
*
* date:             2009-11-24
*
* Description:      gui staticstartmove
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticStartMove(H_WIN hstatic)
{
    static_data_t   *pData;
    static_para_t   *user_data;
    RECT            fbrect;
    __gui_rect_t    rcrect;
    __s32           ret;

    if (!hstatic)
    {
        ORANGE_WRN("input parameter error!\n");
        return ORANGE_FAIL;
    }

    pData       = (static_data_t *)GUI_CtrlWinGetAddData(hstatic);
    user_data   = (static_para_t *)(pData->data);

    if (pData->str_move)
    {
        __show_info_move_t  str_move;
        GUI_WinGetClientFBRect(hstatic, &fbrect);
        SysRectToGuiRect(&fbrect, &rcrect);
        str_move.bkColor         = user_data->bk_color;
        str_move.fontColor       = user_data->ftxt_color;
        str_move.hlayer          = GUI_WinGetLyrWin(hstatic);
        str_move.string          = user_data->text;
        str_move.region.x        = rcrect.left;
        str_move.region.y        = rcrect.top;
        str_move.region.width    = rcrect.right - rcrect.left + 1;
        str_move.region.height   = rcrect.bottom - rcrect.top + 1;
        str_move.encode_id       = user_data->draw_code;
        str_move.pFont           = user_data->draw_font;
        str_move.align           = user_data->txt_align;
        str_move.bmp             = user_data->unfocus_bmp;//NULL;modified by Derek,2011-1-21 15:30:14
        str_move.bmp_pos.x       = 0;
        str_move.bmp_pos.y       = 0;
        str_move.alpha_en        = user_data->alpha_en;
        GUI_LongStringReset((__hdle)(pData->str_move), &str_move);
        GUI_LongStringStart((__hdle)(pData->str_move));
    }
    else
    {
        __show_info_move_t  str_move;
        GUI_WinGetClientFBRect(hstatic, &fbrect);
        SysRectToGuiRect(&fbrect, &rcrect);
        str_move.bkColor         = user_data->bk_color;
        str_move.fontColor       = user_data->ftxt_color;
        str_move.hlayer          = GUI_WinGetLyrWin(hstatic);
        str_move.string          = user_data->text;
        str_move.region.x        = rcrect.left;
        str_move.region.y        = rcrect.top;
        str_move.region.width    = rcrect.right - rcrect.left + 1;
        str_move.region.height   = rcrect.bottom - rcrect.top + 1;
        str_move.encode_id       = user_data->draw_code;
        str_move.pFont           = user_data->draw_font;
        str_move.align           = user_data->txt_align;
        str_move.bmp             = user_data->unfocus_bmp;//NULL;modified by Derek,2011-1-21 15:30:14
        str_move.bmp_pos.x       = 0;
        str_move.bmp_pos.y       = 0;
        str_move.alpha_en        = user_data->alpha_en;
        pData->str_move          = (unsigned long)GUI_LongStringCreate(&str_move);

        if (!pData->str_move)
        {
            ORANGE_WRN("create move handle failed!\n");
            return ORANGE_FAIL;
        }

        ret                      = GUI_LongStringStart((__hdle)(pData->str_move));

        if (ret != ORANGE_OK)
        {
            ORANGE_WRN("start move handle failed!\n");
            return ORANGE_FAIL;
        }
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticStopMove
*
* author:
*
* date:             2009-11-24
*
* Description:      gui staticstopmove
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticStopMove(H_WIN hstatic)
{
    static_data_t   *pData;

    if (!hstatic)
    {
        ORANGE_WRN("input parameter error!\n");
        return ORANGE_FAIL;
    }

    pData       = (static_data_t *)GUI_CtrlWinGetAddData(hstatic);

    if (pData->str_move)
    {
        GUI_LongStringStop((__hdle)(pData->str_move));
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_StaticSetCharSet
*
* author:
*
* date:             2010-1-15
*
* Description:      gui staticsetcharset
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_StaticSetCharSet(H_WIN hstatic, __epdk_charset_enm_e draw_code)
{
    static_para_t   *user_data;

    if ((!hstatic))
    {
        __wrn("input parameter error!\n");
        return ORANGE_FAIL;
    }

    user_data   = (static_para_t *)(((static_data_t *)GUI_CtrlWinGetAddData(hstatic))->data);
    user_data->draw_code = draw_code;
    return ORANGE_OK;
}
#endif

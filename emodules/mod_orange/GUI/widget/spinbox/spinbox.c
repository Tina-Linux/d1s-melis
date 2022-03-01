/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    spinbox.c
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-17
*
*Description :  spinbox control implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <calendar>         <version>     <description>
*
* William Wang     2009-8-17         1.1.0          Create File
*
*******************************************************************************************************************
*/

#include "spinbox_i.h"
#include <elibs_stdlib.h>
#include <kconfig.h>

static __u32        last_key = 0xffffffff;

static void _spinbox_ctrl_paint(__gui_msg_t *msg);
static int _spinbox_ctrl_proc(__gui_msg_t  *msg);

static void _key_proc(__gui_msg_t *msg)
{
    spinbox_para_t      *user_data;
    user_data = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin))->data);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGLEFT:
        case GUI_MSG_KEY_LONGUP:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                {
                    __gui_notify_msg_t      notify_msg;
                    notify_msg.hWnd     = msg->h_deswin;
                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                    notify_msg.msgcode  = SPN_STOPCHG;
                    notify_msg.dwAddData = user_data->cur_value;
                    GUI_NotifyParent(&notify_msg);
                }
                SPINBOX_DRAWSTATUS(msg->h_deswin)  &= (~ BST_PUSHED);
                GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            }

            if (msg->dwAddData2 == KEY_DOWN_ACTION)
            {
                user_data->cur_value--;

                if (user_data->cur_value < user_data->min_value)
                {
                    user_data->cur_value = user_data->max_value;
                }

                {
                    __gui_notify_msg_t      notify_msg;
                    notify_msg.hWnd     = msg->h_deswin;
                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                    notify_msg.msgcode  = SPN_CHG;
                    notify_msg.dwAddData = user_data->cur_value;
                    GUI_NotifyParent(&notify_msg);
                }

                SPINBOX_DRAWFLAG(msg->h_deswin) |= BST_LEFT;
                SPINBOX_DRAWSTATUS(msg->h_deswin) |= BST_PUSHED;
                GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            }

            break;
        }

        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_LONGDOWN:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                {
                    __gui_notify_msg_t      notify_msg;
                    notify_msg.hWnd     = msg->h_deswin;
                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                    notify_msg.msgcode  = SPN_STOPCHG;
                    notify_msg.dwAddData = user_data->cur_value;
                    GUI_NotifyParent(&notify_msg);
                }
                SPINBOX_DRAWSTATUS(msg->h_deswin)  &= (~ BST_PUSHED);
                GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            }

            if (msg->dwAddData2 == KEY_DOWN_ACTION)
            {
                user_data->cur_value++;

                if (user_data->cur_value > user_data->max_value)
                {
                    user_data->cur_value = user_data->min_value;
                }

                {
                    __gui_notify_msg_t      notify_msg;
                    notify_msg.hWnd     = msg->h_deswin;
                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                    notify_msg.msgcode  = SPN_CHG;
                    notify_msg.dwAddData = user_data->cur_value;
                    GUI_NotifyParent(&notify_msg);
                }

                SPINBOX_DRAWFLAG(msg->h_deswin) &= (~ BST_LEFT);
                SPINBOX_DRAWSTATUS(msg->h_deswin) |= BST_PUSHED;
                GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            }

            break;
        }

        /*case GUI_MSG_KEY_RISE:
            {
                switch( last_key )
                {
                    case GUI_MSG_KEY_DOWN:
                    case GUI_MSG_KEY_UP:
                    case GUI_MSG_KEY_LEFT:
                    case GUI_MSG_KEY_RIGHT:
                    case GUI_MSG_KEY_LONGRIGHT:
                    case GUI_MSG_KEY_LONGDOWN:
                    case GUI_MSG_KEY_LONGLEFT:
                    case GUI_MSG_KEY_LONGUP:
                        {
                            {
                                __gui_notify_msg_t      notify_msg;

                                notify_msg.hWnd     = msg->h_deswin;
                                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                notify_msg.msgcode  = SPN_STOPCHG;
                                notify_msg.dwAddData= user_data->cur_value;

                                GUI_NotifyParent (&notify_msg);
                            }
                            SPINBOX_DRAWSTATUS(msg->h_deswin)  &= (~ BST_PUSHED);
                            GUI_InvalidateRect (msg->h_deswin, NULL, EPDK_TRUE);
                        }
                        break;

                    default:
                        break;
                }
            }
            break;*/

        default:
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
}

static void _touch_proc(__gui_msg_t *msg)
{
    __u32               tmp_sta = 0;
    __s32               x, y;
    RECT                client_rect;
    spinbox_para_t      *user_data;
    user_data = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin))->data);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    GUI_WinGetClientRECT(msg->h_deswin, &client_rect);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_LONGDOWN:
        case GUI_MSG_TOUCH_DOWN:
        {
            if (GUI_WinGetCaptureWin() != msg->h_deswin)
            {
                GUI_WinSetCaptureWin(msg->h_deswin);
            }

            if (user_data->spin_sta == 0)
            {
                if (x < client_rect.width / 2)
                {
                    tmp_sta = 0;
                }
                else
                {
                    tmp_sta = 1;
                }
            }
            else
            {
                if (y < client_rect.height / 2)
                {
                    tmp_sta = 0;
                }
                else
                {
                    tmp_sta = 1;
                }
            }

            if (tmp_sta == 0)
            {
                user_data->cur_value--;

                if (user_data->cur_value < user_data->min_value)
                {
                    user_data->cur_value = user_data->max_value;
                }

                SPINBOX_DRAWFLAG(msg->h_deswin) |= BST_LEFT;
            }
            else
            {
                user_data->cur_value++;

                if (user_data->cur_value > user_data->max_value)
                {
                    user_data->cur_value = user_data->min_value;
                }

                SPINBOX_DRAWFLAG(msg->h_deswin) &= (~ BST_LEFT);
            }

            {
                __gui_notify_msg_t      notify_msg;
                notify_msg.hWnd     = msg->h_deswin;
                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                notify_msg.msgcode  = SPN_CHG;
                notify_msg.dwAddData = user_data->cur_value;
                GUI_NotifyParent(&notify_msg);
            }

            SPINBOX_DRAWSTATUS(msg->h_deswin) |= BST_PUSHED;
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
        }
        break;

        case GUI_MSG_TOUCH_MOVE:
        {
        }
        break;

        case GUI_MSG_TOUCH_UP:
        {
            {
                __gui_notify_msg_t      notify_msg;
                notify_msg.hWnd     = msg->h_deswin;
                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                notify_msg.msgcode  = SPN_STOPCHG;
                notify_msg.dwAddData = user_data->cur_value;
                GUI_NotifyParent(&notify_msg);
            }
            SPINBOX_DRAWSTATUS(msg->h_deswin) &= (~ BST_PUSHED);
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);

            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }
        }
        break;

        default:
            break;
    }
}

/*
**********************************************************************************************************************
*                                               RegisterSpinboxControl
*
* author:
*
* date:             2009-8-4
*
* Description:      registerspinboxcontrol
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool RegisterSpinboxControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_SPINBOX;
    WndClass.dwStyle     = WS_NONE | WS_MEMDEV;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_spinbox_ctrl_proc);
    return GUI_AddNewControlClass(&WndClass) == EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _spinbox_ctrl_proc
*
* author:
*
* date:             2009-8-4
*
* Description:      spinboxctrlproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/
static int _spinbox_ctrl_proc(__gui_msg_t *msg)
{
    __u32                   dwStyle;
    spinbox_data_t         *pData;
    dwStyle = GUI_WinGetStyle(msg->h_deswin);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            pData = (spinbox_data_t *) esMEMS_Malloc(0, sizeof(spinbox_data_t));

            if (pData == NULL)
            {
                return EPDK_FAIL;
            }

            pData->draw_sts     = 0;
            pData->status       = 0;
            pData->draw_flag    = 0;
            pData->hmem         = 0;
            pData->data         = (__u32)GUI_WinGetAttr(msg->h_deswin);
            GUI_CtrlWinSetAddData(msg->h_deswin, (__u32)pData);
            return 0;

        case GUI_MSG_DESTROY:
            pData = (spinbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            esMEMS_Mfree(0, (void *)GUI_CtrlWinGetAddData(msg->h_deswin));
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
            if (SPINBOX_STATUS(msg->h_deswin) & BST_FOCUS)
            {
                SPINBOX_STATUS(msg->h_deswin) &= (~BST_FOCUS);
                GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            }

            return 0;

        case GUI_MSG_SET_FOCUS:
            if (!(SPINBOX_STATUS(msg->h_deswin) & BST_FOCUS))
            {
                SPINBOX_STATUS(msg->h_deswin) |= BST_FOCUS;
                GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            }

            return 0;

        case GUI_MSG_KEY:
            _key_proc(msg);
            return 0;

        case GUI_MSG_TOUCH:
            _touch_proc(msg);
            return 0;

        case GUI_MSG_PAINT:
            _spinbox_ctrl_paint(msg);
            return 0;

        default:
            break;
    }

    return GUI_CtrlWinDefaultProc(msg);
}

static void _spinbox_ctrl_paint(__gui_msg_t *msg)
{
    char                    str[128];
    __gui_rect_t            rcrect;
    RECT                    fbrect;
    spinbox_data_t          *spinbox_data;
    spinbox_para_t          *user_data;
    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin))->data);
    spinbox_data = (spinbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
    GUI_LyrWinSel((H_LYR)GUI_WinGetLyrWin(msg->h_deswin));
    GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);

    if (!spinbox_data->hmem)
    {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        spinbox_data->hmem = GUI_MEMDEV_Create(fbrect.x, fbrect.y, fbrect.width, fbrect.height);
        GUI_MEMDEV_Select(spinbox_data->hmem);
#endif
    }

    SysRectToGuiRect(&fbrect, &rcrect);
    GUI_OpenAlphaBlend();

    if (SPINBOX_DRAWSTATUS(msg->h_deswin) & BST_PUSHED)
    {
        GUI_SetBkColor(user_data->bk_color);
        GUI_ClearRect(rcrect.left, rcrect.top, rcrect.right, rcrect.bottom);
        GUI_CharSetToEncode(user_data->draw_code);
        GUI_SetFont(user_data->draw_font);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

        if (SPINBOX_DRAWFLAG(msg->h_deswin) & BST_LEFT)
        {
            GUI_BMP_Draw(user_data->ufup_bmp, rcrect.left + user_data->upbmp_pos.x,
                         rcrect.top + user_data->upbmp_pos.y);
            GUI_BMP_Draw(user_data->fdn_bmp, rcrect.left + user_data->dnbmp_pos.x,
                         rcrect.top + user_data->dnbmp_pos.y);
        }
        else
        {
            GUI_BMP_Draw(user_data->fup_bmp, rcrect.left + user_data->upbmp_pos.x,
                         rcrect.top + user_data->upbmp_pos.y);
            GUI_BMP_Draw(user_data->ufdn_bmp, rcrect.left + user_data->dnbmp_pos.x,
                         rcrect.top + user_data->dnbmp_pos.y);
        }

        GUI_SetColor(user_data->ftxt_color);
        eLIBs_uint2str_dec(user_data->cur_value, str);
        GUI_DispStringHCenterAt(str, rcrect.left + user_data->text_pos.x, rcrect.top + user_data->text_pos.y);
    }
    else
    {
        GUI_SetBkColor(user_data->bk_color);
        GUI_ClearRect(rcrect.left, rcrect.top, rcrect.right, rcrect.bottom);
        GUI_CharSetToEncode(user_data->draw_code);
        GUI_SetFont(user_data->draw_font);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_BMP_Draw(user_data->ufup_bmp, rcrect.left + user_data->upbmp_pos.x,
                     rcrect.top + user_data->upbmp_pos.y);
        GUI_BMP_Draw(user_data->ufdn_bmp, rcrect.left + user_data->dnbmp_pos.x,
                     rcrect.top + user_data->dnbmp_pos.y);
        GUI_SetColor(user_data->uftxt_color);
        eLIBs_uint2str_dec(user_data->cur_value, str);
        GUI_DispStringHCenterAt(str, rcrect.left + user_data->text_pos.x, rcrect.top + user_data->text_pos.y);
    }

    GUI_LyrWinFlushFB((H_LYR)GUI_WinGetLyrWin(msg->h_deswin));
    GUI_CloseAlphaBlend();

    if (spinbox_data->hmem)
    {
#ifdef CONFIG_GUI_CORE_MEMDEV_EN        //avoid complier error, by Derek
        GUI_MEMDEV_CopyToLCD(spinbox_data->hmem);
        GUI_MEMDEV_Select(0);
        GUI_MEMDEV_Delete(spinbox_data->hmem);
#endif
        spinbox_data->hmem = 0;
    }
}

__s32  GUI_SpinboxGetCurValue(H_WIN hspin, __s16  *value)
{
    spinbox_para_t      *user_data;

    if (!hspin)
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    *value = user_data->cur_value;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetCurValue(H_WIN hspin, __s16  value)
{
    spinbox_para_t      *user_data;

    if (!hspin)
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->cur_value = value;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetMaxValue(H_WIN hspin, __s16  value)
{
    spinbox_para_t      *user_data;

    if (!hspin)
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->max_value = value;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetMinValue(H_WIN hspin, __s16  value)
{
    spinbox_para_t      *user_data;

    if (!hspin)
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->min_value = value;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetBkColor(H_WIN hspin, __u32 color)
{
    spinbox_para_t      *user_data;

    if ((!hspin))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->bk_color = color;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetUnFcsTxtColor(H_WIN hspin, __u32 color)
{
    spinbox_para_t      *user_data;

    if ((!hspin))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->uftxt_color = color;
    return EPDK_OK;
}


__s32  GUI_SpinboxSetFcsTxtColor(H_WIN hspin, __u32  color)
{
    spinbox_para_t      *user_data;

    if ((!hspin))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->ftxt_color = color;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetFocusUpBmp(H_WIN hspin, void *bmp)
{
    spinbox_para_t      *user_data;

    if ((!hspin) || (!bmp))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->fup_bmp = bmp;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetFocusDownBmp(H_WIN hspin, void *bmp)
{
    spinbox_para_t      *user_data;

    if ((!hspin) || (!bmp))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->fdn_bmp = bmp;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetUnFocusUpBmp(H_WIN hspin, void *bmp)
{
    spinbox_para_t      *user_data;

    if ((!hspin) || (!bmp))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->ufup_bmp = bmp;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetUnFocusDownBmp(H_WIN hspin, void *bmp)
{
    spinbox_para_t      *user_data;

    if ((!hspin) || (!bmp))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->ufdn_bmp = bmp;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetUpBmpPos(H_WIN hspin, __pos_t *pos)
{
    spinbox_para_t      *user_data;

    if ((!hspin) || (!pos))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->upbmp_pos.x = pos->x;
    user_data->upbmp_pos.y = pos->y;
    return EPDK_OK;
}

__s32  GUI_SpinboxSetDownBmpPos(H_WIN hspin, __pos_t *pos)
{
    spinbox_para_t      *user_data;

    if ((!hspin) || (!pos))
    {
        __wrn("input parameter error!\n");
        return EPDK_FAIL;
    }

    user_data   = (spinbox_para_t *)(((spinbox_data_t *)GUI_CtrlWinGetAddData(hspin))->data);
    user_data->dnbmp_pos.x = pos->x;
    user_data->dnbmp_pos.y = pos->y;
    return EPDK_OK;
}


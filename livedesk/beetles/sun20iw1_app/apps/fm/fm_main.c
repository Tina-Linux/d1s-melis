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
#include "fm_main.h"
#include "fm_sset.h"
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
//
/************************************************************************/

/************************************************************************/
//加载资源
/************************************************************************/
static __s32 fm_mainfrw_create_res(FM_MAINWND_RES_T *res)
{
    //create_bmp_res(ID_FM_BG_BMP, res->bg);
    fm_freq_scale_create_res(&res->scale);
    return EPDK_OK;
}

/************************************************************************/
//释放资源
/************************************************************************/
static __s32 fm_mainfrw_destroy_res(FM_MAINWND_RES_T *res)
{
    fm_freq_scale_destroy_res(&res->scale);
    //destroy_bmp_res(res->bg);
    return EPDK_OK;
}

#define FM_SEND_MSG(mid, src, des, d1, d2) \
    do{\
        __gui_msg_t  exmsg; \
        exmsg.id = mid; \
        exmsg.h_srcwin = src; \
        exmsg.h_deswin = des; \
        exmsg.dwAddData1= (__u32)d1; \
        exmsg.dwAddData2 = (__u32)d2; \
        GUI_SendNotifyMessage(&exmsg); \
    }while(0)

/************************************************************************/
//按键消息处理
/************************************************************************/
static __s32 fm_mainfrw_key_proc(__gui_msg_t *msg)
{
    FM_MAINWND_T *wnd_para;
    static __s32  lastkey = -1;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);

    //eLIBs_printf("----fm frm 收到按键消息 lastkey = %d, msg->dwAddData2 = %d , msg->dwAddData1 = %d  \n",lastkey ,msg->dwAddData2,msg->dwAddData1);
    if (msg->dwAddData2 == KEY_UP_ACTION)
    {
        switch (lastkey)
        {
            //case VK_LEFT:
            //case VK_RIGHT:
            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_MODE:
                //  case GUI_MSG_KEY_MENU:
                FM_SEND_MSG(GUI_MSG_COMMAND, msg->h_deswin, FM_GetManWnd(),
                            MAKELONG(FM_MAINWND_ID, ID_FM_MAINWND_CLICKUP), lastkey);
                break;

            default:
                break;
        }

        lastkey = -1;
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case VK_MENU:

            //case VK_LEFT_REPEAT:
            //case VK_RIGHT_REPEAT:
            //case VK_ENTER:
            case VK_LEFT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGENTER:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LONGPREV:
            case GUI_MSG_KEY_LONGPLAY_PAUSE:
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
                FM_SEND_MSG(GUI_MSG_COMMAND, msg->h_deswin, FM_GetManWnd(),
                            MAKELONG(FM_MAINWND_ID, ID_FM_MAINWND_CLICKUP), msg->dwAddData1);
                break;

            case GUI_MSG_KEY_MENU:
                FM_SEND_MSG(GUI_MSG_COMMAND, msg->h_deswin, FM_GetManWnd(),
                            MAKELONG(FM_MAINWND_ID, ID_FM_MAINWND_NOTIFY_TOUCHUP), 0);
                break;
        }

        lastkey = msg->dwAddData1;
    }

    /*switch(msg->dwAddData1)
    {
        case GUI_MSG_KEY_MENU:
            FM_SEND_MSG(GUI_MSG_COMMAND, msg->h_deswin, FM_GetManWnd(),
                    MAKELONG(FM_MAINWND_ID, ID_FM_MAINWND_NOTIFY_TOUCHUP), 0);
            break;
        }*/
    return 0;
}

/************************************************************************/
//命令消息处理
/************************************************************************/
static __s32 on_fm_mainfrw_command(__gui_msg_t *msg)
{
    FM_MAINWND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);
    //  switch (LOSWORD(msg->dwAddData1))
    //  {
    //  case ID_FM_SCENE_SSET:
    //      return do_fm_mainfrw_sset_cmd(msg->h_deswin, HISWORD(msg->dwAddData1), msg->dwAddData2);
    //  case ID_FM_IS_AUTO_SEARCH_DLG:
    //      return do_fm_mainfrw_auto_search_dlg_cmd(msg->h_deswin, HISWORD(msg->dwAddData1), msg->dwAddData2);
    //  }
    return EPDK_FAIL;
}

/************************************************************************/
//触摸消息按下处理
/************************************************************************/
// static __s32 on_fm_mainfrw_touch_down(H_WIN hwnd, int x, int y)
// {
//  FM_MAINWND_T *wnd_para;
//  FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
//
//
//  return EPDK_OK;
// }

/************************************************************************/
//触摸消息移动处理
/************************************************************************/
// static __s32 on_fm_mainfrw_touch_move(H_WIN hwnd, int x, int y)
// {
//  FM_MAINWND_T *wnd_para;
//  FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
//
//  return EPDK_OK;
// }

/************************************************************************/
//触摸消息弹起处理
/************************************************************************/
// static __s32 on_fm_mainfrw_touch_up(H_WIN hwnd, int x, int y)
// {
//  FM_MAINWND_T *wnd_para;
//  FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
//
//  FM_SEND_MSG(GUI_MSG_COMMAND, hwnd, FM_GetManWnd(),
//      MAKELONG(FM_MAINWND_ID, ID_FM_MAINWND_NOTIFY_TOUCHUP), MAKELONG(x, y));
//
//  return EPDK_OK;
// }


/************************************************************************/
//触摸消息处理
/************************************************************************/
static __s32 fm_mainfrw_touch_proc(__gui_msg_t *msg)
{
    int x = 0, y = 0;
    FM_MAINWND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);

    if (wnd_para->fm_ctrl->search_mode == SRHMODE_AUTO
        || wnd_para->fm_ctrl->search_mode == SRHMODE_MANUAL)
    {
        return EPDK_OK;
    }

    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    __msg("fm_mainfrw_touch_proc %d (%d, %d)\n", msg->dwAddData1, x, y);

    if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN || msg->dwAddData1 == GUI_MSG_TOUCH_LONGDOWN)
    {
        GUI_WinSetCaptureWin(msg->h_deswin);
    }
    else if (msg->dwAddData1 == GUI_MSG_TOUCH_UP || msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP)
    {
        if (GUI_WinGetCaptureWin() == msg->h_deswin)
        {
            GUI_WinReleaseCapture();
        }
    }

    if (on_freq_scale_touch(msg->h_deswin, x, y, msg->dwAddData1))
    {
        return EPDK_OK;
    }

    if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
    {
        FM_SEND_MSG(GUI_MSG_COMMAND, msg->h_deswin, FM_GetManWnd(),
                    MAKELONG(FM_MAINWND_ID, ID_FM_MAINWND_NOTIFY_TOUCHUP), MAKELONG(x, y));
    }

    return EPDK_OK;
}

/************************************************************************/
// 画频道表及当前频道游标
/************************************************************************/
// static __s32 draw_freq_scale(H_WIN hwnd, H_LYR layer, FM_UI_MAINWND *ui)
// {
//  char buf[8 * 2] = {0};
//  int x;
//  GUI_RECT rt;
//  FM_CTRLDATA_T*   fm_ctrl;
//  FM_MAINWND_RES_T *res;
//  FM_MAINWND_T *wnd_para;
//
//  FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
//
//  res = &wnd_para->res;
//
//  fm_ctrl = wnd_para->fm_ctrl;
//
//  GUI_LyrWinSel(layer);
//
//  GUI_LyrWinCacheOn();
//
//  //画标尺
//  GUI_BMP_RES_Draw(res->bmp_scale, ui->scale.x, ui->scale.y);
//
//  //画游标
//  x = RANGE_MAPPING(ui->scale.x, ui->scale.x + ui->scale.width,
//      fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq);
//
//  GUI_BMP_RES_Draw(res->bmp_cursor_sel, x, ui->scale.y + ui->cursor_y);
//
//  //
//  GUI_SetFont(SWFFont);
//  GUI_UC_SetEncodeUTF8();
//  GUI_SetColor(ui->clr_txt_n);
//
//  rt.x0 = ui->scale.x;
//  rt.x1 = rt.x0 + ui->scale.width;
//  rt.y0 = ui->scale.y + ui->txt_y;
//  rt.y1 = ui->scale.y + ui->scale.height;
//
//  MAKE_STR_CHANNEL_FREQ(buf, fm_ctrl->cur_min_freq);
//  GUI_DispStringInRect(STR_MIN_CHANNEL_FREQ, &rt, GUI_TA_LEFT);
//
//  MAKE_STR_CHANNEL_FREQ(buf, fm_ctrl->cur_freq);
//  GUI_DispStringInRect(buf, &rt, GUI_TA_CENTER);
//
//  MAKE_STR_CHANNEL_FREQ(buf, fm_ctrl->cur_max_freq);
//  GUI_DispStringInRect(STR_MAX_CHANNEL_FREQ, &rt, GUI_TA_RIGHT);
//
//  GUI_LyrWinCacheOff();
//
//  return EPDK_OK;
// }

/***********************************************************************************
设置窗口按钮游标随着4702模块内部搜索浮动显示函数
*************************************************************************************/
/*
__s32 fm_auto_search_cb(FM_WND_T *fmplay_menu_attr)
{

    if (GUI_LyrWinGetSta(fmplay_menu_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return;
    }

    GUI_LyrWinSel(fmplay_menu_attr->layer);
    //GUI_SetFont(fmplay_menu_attr->font);
    //GUI_UC_SetEncodeUTF8();

    //fm_view_show(fmplay_menu_attr);
    draw_freq_scale(fmplay_menu_attr);
    return EPDK_OK;
}
*/

/************************************************************************/
// paint window
/************************************************************************/
static __s32 on_fm_mainfrw_paint(__gui_msg_t *msg)
{
    H_LYR layer = GUI_WinGetLyrWin(msg->h_deswin);

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_ON)
    {
        FM_UI_MAINWND *ui = NULL;
        GUI_MEMDEV_Handle draw_mem = NULL;
        FM_MAINWND_T *wnd_para = NULL;
        FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);
        //..GUI_LyrWinSetSta(layer,GUI_LYRWIN_STA_ON);
        GUI_LyrWinSel(layer);
        ui = &get_fm_uipara()->main;
        draw_mem = GUI_MEMDEV_Create(0, 0, ui->rt.width, ui->rt.height);
        GUI_MEMDEV_Select(draw_mem);
        //      GUI_LyrWinCacheOn();
        GUI_SetBkColor(0);
        GUI_Clear();
        //GUI_BMP_RES_Draw(wnd_para->res.bg, 0, 0);
        draw_freq_scale(msg->h_deswin, layer, ui);
        //      GUI_LyrWinCacheOff();
        GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}
/************************************************************************/
// paint current window
/************************************************************************/
static __s32 on_fm_mainfrw_updatepaint(__gui_msg_t *msg)
{
    H_LYR layer = GUI_WinGetLyrWin(msg->h_deswin);

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_ON)
    {
        FM_UI_MAINWND *ui;
        GUI_MEMDEV_Handle draw_mem;
        FM_MAINWND_T *wnd_para;
        //__s16 x1, y1;
        __s16 x0 = 80, y0 = 171, w = 626, h = 188;
        //x1 = w+x0;
        //y1 = h+y0;
        FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);
        GUI_LyrWinSel(layer);
        ui = &get_fm_uipara()->main;
        draw_mem = GUI_MEMDEV_Create(x0, y0, w, h);
        GUI_MEMDEV_Select(draw_mem);
        //      GUI_LyrWinCacheOn();
        GUI_SetBkColor(0);
        //..GUI_Clear();
        //..GUI_ClearRect(210, 171, 550, 239);
        //GUI_ClearRect(90, 279, 696, 359);
        //GUI_BMP_RES_Draw(wnd_para->res.bg, 0, 0);
        draw_currentfreq_scale(msg->h_deswin, layer, ui);
        //      GUI_LyrWinCacheOff();
        GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}


/************************************************************************/
//窗口销毁处理
/************************************************************************/
static __s32 on_fm_mainfrw_destory(__gui_msg_t *msg)
{
    FM_MAINWND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);
    //TODO:release resource
    fm_mainfrw_destroy_res(&wnd_para->res);
    BFreeType(wnd_para, FM_MAINWND_T);
    return 0;
}

/************************************************************************/
//窗口创建处理
/************************************************************************/
static __s32 on_fm_mainfrw_create(__gui_msg_t *msg)
{
    FM_MAINWND_T *wnd_para = NULL;
    FM_CTRLDATA_T *init_para = NULL;
    init_para = (FM_CTRLDATA_T *)GUI_WinGetAttr(msg->h_deswin);
    BallocType(wnd_para, FM_MAINWND_T);
    ASSERT(wnd_para != NULL);
    ZeroTypeMem(wnd_para, FM_MAINWND_T);
    GUI_WinSetAttr(msg->h_deswin, wnd_para);
    wnd_para->all_paint_flag = 1;
    //TODO:init wnd_para
    wnd_para->fm_ctrl = init_para;
    fm_mainfrw_create_res(&wnd_para->res);
    //gscene_hbar_set_state(HBAR_ST_SHOW);
    return 0;
}

//消息处理函数
static __s32 _fm_mainfrw_proc(__gui_msg_t *msg)
{
    APP_DEBUG_CBMSGEx;

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            return on_fm_mainfrw_create(msg);
        }

        case GUI_MSG_DESTROY:
        {
            return on_fm_mainfrw_destory(msg);
        }

        case DSK_MSG_HOME:
            notify_to_close_fm(msg->h_deswin, SWITCH_TO_MMENU, 0);
            return EPDK_OK;

        case DSK_MSG_KILL:
        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            if (on_fm_mainfrw_command(msg) == EPDK_OK)
            {
                return EPDK_OK;
            }
        }
        break;

        case GUI_MSG_KEY:
        {
            fm_mainfrw_key_proc(msg);
        }

        return EPDK_OK;

        case GUI_MSG_TOUCH:
        {
            fm_mainfrw_touch_proc(msg);
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            FM_MAINWND_T *wnd_para = NULL;
            FM_GetWndPara(wnd_para, FM_MAINWND_T, msg->h_deswin);
            __wrn("///////wnd_para->all_paint_flag=%d\n", wnd_para->all_paint_flag);

            if (wnd_para->all_paint_flag == 1)
            {
                on_fm_mainfrw_paint(msg);
                wnd_para->all_paint_flag = 0;
            }
            else if (wnd_para->all_paint_flag == 0)
            {
                on_fm_mainfrw_updatepaint(msg);
            }
        }

        return EPDK_OK;

        /*case CMD_UPDATE:
        {

               on_fm_mainfrw_updatepaint(msg);

        }
        return EPDK_OK;  */
        case CMD_AUTOSRH_FINDCH:
        {
            //自动搜索状态下搜索到一个频点:
            FM_UpdateUI(msg->h_deswin);
        }

        return EPDK_OK;

        case CMD_AUTOSRH_FINDCHFAIL:
        {
            FM_UpdateUI(msg->h_deswin);
        }

        return EPDK_OK;

        case CMD_AUTOSRH_OVER:
        {
            //自动搜索完成
            FM_UpdateUI(msg->h_deswin);
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

//窗口创建
H_WIN fm_mainfrw_create(H_WIN h_parent, H_LYR layer, void *para)
{
    __gui_framewincreate_para_t framewin_para;
    FB fb;
    GUI_LyrWinGetFB(layer, &fb);
    ZeroTypeMem(&framewin_para, __gui_framewincreate_para_t);
    framewin_para.name          = FM_MAINWND_NAME;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = FM_MAINWND_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_fm_mainfrw_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = para;
    framewin_para.hLayer        = layer;
    return (GUI_FrmWinCreate(&framewin_para));
}
//////////////////////////////////////////////////////////////////////////
// end of file

/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : photo_main.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/15 17:26  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "photo_main.h"

//////////////////////////////////////////////////////////////////////////


/************************************************************************/
//
/************************************************************************/

/************************************************************************/
//加载资源
/************************************************************************/
static int32_t photo_main_frw_create_res(PHOTO_MAIN_FRW_RES_T *res)
{
    __log("vasvarb...photo_main_frw_create_res");

    create_bmp_res(ID_PHOTO_ICON_AUTOPLAY_BMP,      res->status.bmp_auto_play);
    create_bmp_res(ID_PHOTO_ICON_PLAYBACKWARD_BMP,  res->status.bmp_play_backward);
    create_bmp_res(ID_PHOTO_ICON_PLAYFORWARD_BMP,   res->status.bmp_play_forward);
    create_bmp_res(ID_PHOTO_ICON_PAUSE_BMP,         res->status.bmp_pause);
    create_bmp_res(ID_PHOTO_ICON_PLAYPREV_BMP,      res->status.bmp_prev);
    create_bmp_res(ID_PHOTO_ICON_PLAYNEXT_BMP,      res->status.bmp_next);

    photo_file_info_create_res(&res->file_info);
    return EPDK_OK;
}

/************************************************************************/
//释放资源
/************************************************************************/
static int32_t photo_main_frw_destroy_res(PHOTO_MAIN_FRW_RES_T *res)
{
    destroy_bmp_res(res->status.bmp_auto_play);
    destroy_bmp_res(res->status.bmp_play_backward);
    destroy_bmp_res(res->status.bmp_play_forward);
    destroy_bmp_res(res->status.bmp_pause);
    destroy_bmp_res(res->status.bmp_prev);
    destroy_bmp_res(res->status.bmp_next);
    photo_file_info_destroy_res(&res->file_info);
    return EPDK_OK;
}

/************************************************************************/
// draw scale panel temp bmp
/************************************************************************/
static void draw_scale_panel_res_bmp(/*H_LYR layer, */int32_t res_id, GUI_RECT *rt, int32_t TA_flag)
{
    int32_t     x = 0, y = 0;
    int32_t     bmp_w = 0, bmp_h = 0;
    load_bmp_res_t  res_bmp;
    HBMP        hbmp;

    create_bmp_res(res_id, res_bmp);

    hbmp    = bmp_open(ResHandle2Data(res_bmp));

    if (hbmp)
    {
        bmp_w   = bmp_get_width(hbmp);
        bmp_h   = bmp_get_height(hbmp);
        bmp_close(hbmp);
    }

    switch (TA_flag & GUI_TA_HORIZONTAL)
    {
        case GUI_TA_LEFT:
            x = rt->x0;
            break;

        case GUI_TA_RIGHT:
            x = rt->x1 - bmp_w;
            break;

        case GUI_TA_HCENTER:
            x = (rt->x0 + rt->x1 - bmp_w) / 2;
            break;

        default:
            break;
    }

    switch (TA_flag & GUI_TA_VERTICAL)
    {
        case GUI_TA_TOP:
            y = rt->y0;
            break;

        case GUI_TA_BOTTOM:
            y = rt->y1 - bmp_h;
            break;

        case GUI_TA_VCENTER:
            y = (rt->y0 + rt->y1 - bmp_h) / 2;
            break;

        default:
            break;
    }

    GUI_BMP_RES_Draw(res_bmp, x, y);
    destroy_bmp_res(res_bmp);
}

static void draw_scale_panel_old_iten(__photo_scale_item_draw_e old_item_id, GUI_RECT *p_rt)
{
    switch (old_item_id)
    {
        case PHOTO_SCALE_ITEM_ROTATELEFT:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ROTATELEFT_N_BMP, p_rt, GUI_TA_LEFT | GUI_TA_TOP);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_UP:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMUP_N_BMP, p_rt, GUI_TA_HCENTER | GUI_TA_TOP);
            break;
        }

        case PHOTO_SCALE_ITEM_RPTATERIGHT:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ROTATERIGHT_N_BMP, p_rt, GUI_TA_RIGHT | GUI_TA_TOP);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_LEFT:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMLEFT_N_BMP, p_rt, GUI_TA_LEFT | GUI_TA_VCENTER);
            break;
        }

        case PHOTO_SCALE_ITEM_EXIT:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMCLOSE_N_BMP, p_rt, GUI_TA_HCENTER | GUI_TA_VCENTER);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_RIGHT:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMRIGHT_N_BMP, p_rt, GUI_TA_RIGHT | GUI_TA_VCENTER);
            break;
        }

        case PHOTO_SCALE_ITEM_ZOOMDEC:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMOUT_N_BMP, p_rt, GUI_TA_LEFT | GUI_TA_BOTTOM);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_DOWN:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMDOWN_N_BMP, p_rt, GUI_TA_HCENTER | GUI_TA_BOTTOM);
            break;
        }

        case PHOTO_SCALE_ITEM_ZOOMINC:
        {
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMIN_N_BMP, p_rt, GUI_TA_RIGHT | GUI_TA_BOTTOM);
            break;
        }

        default:
            break;
    }
}


/************************************************************************/
// draw scale panel
/************************************************************************/
static __s32 draw_scale_panel(H_WIN hwnd)
{
    GUI_RECT rt;
    H_LYR layer;
    FB fb;
    PHOTO_MAIN_FRW_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, hwnd);
    layer = GUI_WinGetLyrWin(hwnd);
    GUI_LyrWinSel(layer);
    GUI_LyrWinGetFB(layer, &fb);
    rt.x0 = 0;
    rt.y0 = 0;
    rt.x1 = fb.size.width;
    rt.y1 = fb.size.height;

    switch (wnd_para->ctrl_para.scale_new_item)
    {
        case PHOTO_SCALE_ITEM_ALL:      //画全部
        {
            //逆时针旋转
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ROTATELEFT_F_BMP, &rt, GUI_TA_LEFT | GUI_TA_TOP);
            //顺时针旋转
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ROTATERIGHT_N_BMP, &rt, GUI_TA_RIGHT | GUI_TA_TOP);
            //放大
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMIN_N_BMP, &rt, GUI_TA_RIGHT | GUI_TA_BOTTOM);
            //缩小
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMOUT_N_BMP, &rt, GUI_TA_LEFT | GUI_TA_BOTTOM);
            //向上
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMUP_N_BMP, &rt, GUI_TA_HCENTER | GUI_TA_TOP);
            //向下
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMDOWN_N_BMP, &rt, GUI_TA_HCENTER | GUI_TA_BOTTOM);
            //向左
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMLEFT_N_BMP, &rt, GUI_TA_LEFT | GUI_TA_VCENTER);
            //向右
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMRIGHT_N_BMP, &rt, GUI_TA_RIGHT | GUI_TA_VCENTER);
            //关闭
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMCLOSE_N_BMP, &rt, GUI_TA_HCENTER | GUI_TA_VCENTER);
            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ROTATELEFT;
            break;
            break;
        }

        case PHOTO_SCALE_ITEM_ROTATELEFT:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ROTATELEFT_F_BMP, &rt, GUI_TA_LEFT | GUI_TA_TOP);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_UP:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMUP_F_BMP, &rt, GUI_TA_HCENTER | GUI_TA_TOP);
            break;
        }

        case PHOTO_SCALE_ITEM_RPTATERIGHT:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ROTATERIGHT_F_BMP, &rt, GUI_TA_RIGHT | GUI_TA_TOP);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_LEFT:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMLEFT_F_BMP, &rt, GUI_TA_LEFT | GUI_TA_VCENTER);
            break;
        }

        case PHOTO_SCALE_ITEM_EXIT:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMCLOSE_F_BMP, &rt, GUI_TA_HCENTER | GUI_TA_VCENTER);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_RIGHT:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMRIGHT_F_BMP, &rt, GUI_TA_RIGHT | GUI_TA_VCENTER);
            break;
        }

        case PHOTO_SCALE_ITEM_ZOOMDEC:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMOUT_F_BMP, &rt, GUI_TA_LEFT | GUI_TA_BOTTOM);
            break;
        }

        case PHOTO_SCALE_ITEM_MOVE_DOWN:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMDOWN_F_BMP, &rt, GUI_TA_HCENTER | GUI_TA_BOTTOM);
            break;
        }

        case PHOTO_SCALE_ITEM_ZOOMINC:
        {
            draw_scale_panel_old_iten(wnd_para->ctrl_para.scale_old_item, &rt);
            draw_scale_panel_res_bmp(ID_PHOTO_BTN_ZOOMIN_F_BMP, &rt, GUI_TA_RIGHT | GUI_TA_BOTTOM);
            break;
        }
    }

    return EPDK_OK;
}

/************************************************************************/
// draw showpara
/************************************************************************/
#define PHOTO_STATUS_NUM_STR_FMT        " %d "
#define PHOTO_STATUS_NUM_UI_OFFSET      2
static void draw_photo_main_frw_status(PHOTO_PLAY_STATUS_RES_T *res, PHOTO_STATE_UI_T *ui, __s32 showpara)
{
    GUI_RECT rt_str;
    char     buf[32] = {0};
    __s32    str_fmt  = GUI_TA_RIGHT;
    void    *bmp_data;
    __wrn("-----开始画东西draw_photo_main_frw_status    showpara = %d\n", showpara);

    switch (showpara)
    {
        //  case PHOTO_SHOW_MANUAL_PLAY:
        //      {
        //          str_fmt  = GUI_TA_LEFT;
        //          bmp_data = ResHandle2Data(res->bmp_play_backward);
        //      }
        //      break;
        case PHOTO_SHOW_PLAY_NEXT:
        {
            str_fmt  = GUI_TA_LEFT;
            bmp_data = ResHandle2Data(res->bmp_next);
        }
        break;

        case PHOTO_SHOW_PLAY_PREV:
        {
            bmp_data = ResHandle2Data(res->bmp_prev);
        }
        break;

        case PHOTO_SHOW_PAUSE:
        {
            bmp_data = ResHandle2Data(res->bmp_pause);
        }
        break;

        case PHOTO_SHOW_AUTO_PLAY:
        default:
        {
            bmp_data = ResHandle2Data(res->bmp_auto_play);
        }
        break;
    }

    //底
    GUI_BMP_Draw(bmp_data, ui->rt.x, ui->rt.y);
    //当前
    app_sprintf(buf, PHOTO_STATUS_NUM_STR_FMT, APP_ANOLE_GetCurPicIndex());
    rt_str.x0 = ui->rt.x + PHOTO_STATUS_NUM_UI_OFFSET;
    rt_str.x1 = ui->rt.x + ui->rt.width - PHOTO_STATUS_NUM_UI_OFFSET;
    rt_str.y0 = ui->rt.y + PHOTO_STATUS_NUM_UI_OFFSET;
    rt_str.y1 = ui->rt.y + ui->rt.height / 2;
    GUI_DispStringInRect(buf, &rt_str, str_fmt);
    //总数
    app_sprintf(buf, PHOTO_STATUS_NUM_STR_FMT, APP_ANOLE_GetPicNum());
    rt_str.y0 = rt_str.y1;
    rt_str.y1 = ui->rt.y + ui->rt.height - PHOTO_STATUS_NUM_UI_OFFSET;
    GUI_DispStringInRect(buf, &rt_str, str_fmt);
}

/************************************************************************/
// paint window
/************************************************************************/
static __s32 on_photo_main_frw_paint(H_WIN hwnd)
{
    H_LYR layer;
    layer = GUI_WinGetLyrWin(hwnd);
    __wrn("-----------将要画点东西\n");
    __wrn("layer=%x\n", layer);

    if (GUI_LyrWinGetSta(layer) == GUI_LYRWIN_STA_ON)
    {
        static __s32 old_showpara = -1;
        PHOTO_UI_PARA_T *ui;
        PHOTO_MAIN_FRW_T *wnd_para;
        PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, hwnd);
        GUI_LyrWinSetTop(layer);
        {
            RECT rect;
            GUI_LyrWinGetScnWindow(layer, &rect);
            __wrn("rect.x=%d", rect.x);
            __wrn("rect.y=%d", rect.y);
            __wrn("rect.width=%d", rect.width);
            __wrn("rect.height=%d\n", rect.height);
        }
        ui = get_photo_uipara();
        GUI_LyrWinSel(layer);

        if (old_showpara != wnd_para->ctrl_para.show_para)
        {
            GUI_SetBkColor(0);
            GUI_Clear();
            old_showpara = wnd_para->ctrl_para.show_para;
        }

        GUI_SetFont(SWFFont);
        GUI_UC_SetEncodeUTF8();
        GUI_SetColor(ui->colour.txt_n);
        __wrn("ui->colour.txt_n=%x\n", ui->colour.txt_n);
        __wrn("-----开始画东西-old_showpara = %d, wnd_para->ctrl_para.show_para = %d\n", old_showpara, wnd_para->ctrl_para.show_para);

        switch (wnd_para->ctrl_para.show_para)
        {
            //  case PHOTO_SHOW_MANUAL_PLAY:
            case PHOTO_SHOW_PLAY_NEXT:
            case PHOTO_SHOW_PLAY_PREV:
            case PHOTO_SHOW_PAUSE:
            case PHOTO_SHOW_AUTO_PLAY:
            {
                //..draw_photo_main_frw_status(&wnd_para->res.status, &ui->bg.state, wnd_para->ctrl_para.show_para);
            }
            break;

            case PHOTO_SHOW_FILEINFO:
            {
                __wrn("draw_photo_file_info...\n");
                draw_photo_file_info(hwnd, &wnd_para->res.file_info, &ui->bg.file_info);
                esKRNL_TimeDly(300);
                do_photo_command(hwnd, ID_PHOTO_CMD_KILL_PHOTO_FRMWIN, HIDE_UI);
            }
            break;

            case PHOTO_SHOW_SCALE:
                draw_scale_panel(hwnd);
                break;

            default:
                break;
        }

        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

/************************************************************************/
//通知退出photo
/************************************************************************/
static __s32 to_close_app_photo(H_WIN hwnd)
{
    __gui_msg_t msg;
    msg.h_deswin   = GUI_WinGetParent(hwnd);
    msg.h_srcwin   = hwnd;
    msg.id         = MSG_PHOTO_COMMAND;
    msg.dwAddData1 = ID_PHOTO_CMD_EXIT;
    msg.dwAddData2 = 0;
    msg.dwReserved = 0;
    return (GUI_SendNotifyMessage(&msg));
}

/************************************************************************/
//Enter按键消息处理
/************************************************************************/
static __s32 on_photo_main_frw_key_enter(H_WIN hwnd, PHOTO_MAIN_FRW_T *wnd_para)
{
    __anole_sta_t state;
    ASSERT(wnd_para);
    state = APP_ANOLE_GetSta();
    __wrn("state:%d\r\n", state);
    __wrn("----图片收到enter 消息wnd_para->ctrl_para.show_para = %d\n", wnd_para->ctrl_para.show_para);

    if (state == ANOLE_MODE_PLAY)
    {
        APP_ANOLE_Pause();
        wnd_para->ctrl_para.show_para = PHOTO_SHOW_PAUSE;
        do_photo_command(hwnd, ID_PHOTO_CMD_PAUSE, SHOW_UI);
    }
    else
    {
        switch (wnd_para->ctrl_para.show_para)
        {
            case PHOTO_SHOW_FILEINFO:
            {
                //              do_photo_command(hwnd, ID_ANOLE_CMD_FILEINFO, HIDE_UI);
                do_photo_command(hwnd, ID_ANOLE_CMD_PAUSE, 0);
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, hwnd, ID_PHOTO_CMD_PAUSE, SHOW_UI);
                //              do_photo_command(hwnd, ID_PHOTO_CMD_PAUSE, SHOW_UI);
            }
            break;

            case  PHOTO_SHOW_SCALE:
            {
                /*刷新scale 显示焦点*/
                if (TouchValidID(wnd_para->ctrl_para.scale_new_item))
                {
                    todo_anole_command(hwnd, ID_ANOLE_CMD_SCALE, wnd_para->ctrl_para.scale_new_item);
                }
            }
            break;

            case  PHOTO_SHOW_PLAY_NEXT:
            case  PHOTO_SHOW_PLAY_PREV:
            case  PHOTO_SHOW_PAUSE:
            {
                //              do_photo_command(hwnd, ID_PHOTO_CMD_AUTOPLAY, SHOW_UI);
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, hwnd, ID_PHOTO_CMD_AUTOPLAY, SHOW_UI);
                APP_ANOLE_AutoPlay();
            }
            break;
        }
    }

    return EPDK_OK;
}

/************************************************************************/
//按键消息处理注意,当有sset的时候 这里收不到KEY 消息
/************************************************************************/
static __s32 photo_main_frw_key_proc(__gui_msg_t *msg)
{
    PHOTO_MAIN_FRW_T *wnd_para;
    static __s32 lastkey = -1;
    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);
    __wrn("-------图片FRW 按键msg->dwAddData1 = %d, msg->dwAddData2 = %d\n", msg->dwAddData1, msg->dwAddData2);
    __wrn("-------lastkey = %d,wnd_para->ctrl_para.show_para = %d \n", lastkey, wnd_para->ctrl_para.show_para);

    if (msg->dwAddData2 == KEY_UP_ACTION)
    {
        switch (lastkey)
        {
            case VK_PHOTO_ESC:
            {
                /*删除窗口、释放资源*/
                todo_anole_command(msg->h_deswin, ID_ANOLE_CMD_SCALE, PHOTO_SCALE_ITEM_EXIT);
                //do_photo_command(msg->h_deswin, ID_PHOTO_CMD_KILL_PHOTO_FRMWIN,HIDE_UI);
#if 0

                if (wnd_para->ctrl_para.show_para == PHOTO_SHOW_FILEINFO)
                {
                    do_photo_command(msg->h_deswin, ID_ANOLE_CMD_FILEINFO, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para  == PHOTO_SHOW_MENU)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para  == PHOTO_SHOW_AUTO_PLAY)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_AUTOPLAY, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para  == PHOTO_SHOW_PAUSE)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PAUSE, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para  == PHOTO_SHOW_PLAY_NEXT)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para  == PHOTO_SHOW_PLAY_PREV)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para == PHOTO_SHOW_SCALE)
                {
                    wnd_para->ctrl_para.scale_new_item = wnd_para->ctrl_para.scale_old_item = PHOTO_SCALE_ITEM_ALL;
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, HIDE_UI);
                }
                else if (wnd_para->ctrl_para.show_para == PHOTO_SHOW_NULL)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_EXIT, HIDE_UI);
                }

#endif
            }

            lastkey = -1;
            break;

            case VK_ENTER:
            {
                on_photo_main_frw_key_enter(msg->h_deswin, wnd_para);
            }

            lastkey = -1;
            break;

            case VK_PHOTO_MENU_POP:
            {
                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_POP_MENU, 0);
            }

            lastkey = -1;
            break;

            case VK_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                switch (wnd_para->ctrl_para.show_para)
                {
                    case PHOTO_SHOW_PAUSE:
                    case PHOTO_SHOW_PLAY_NEXT:
                    case PHOTO_SHOW_PLAY_PREV:
                    {
                        anole_play_prev();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PAUSE, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV,SHOW_UI);
                        break;
                    }

                    case PHOTO_SHOW_AUTO_PLAY:
                    {
                        APP_ANOLE_Pause();
                        anole_play_prev();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PAUSE, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, SHOW_UI);
                        break;
                    }

                    case PHOTO_SHOW_SCALE:
                    {
                        __wrn("----left1\n");
                        wnd_para->ctrl_para.scale_old_item = wnd_para->ctrl_para.scale_new_item;
                        wnd_para->ctrl_para.scale_new_item--;

                        if (wnd_para->ctrl_para.scale_new_item < PHOTO_SCALE_ITEM_ROTATELEFT
                            || wnd_para->ctrl_para.scale_new_item > PHOTO_SCALE_ITEM_ZOOMINC)
                        {
                            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ZOOMINC;
                        }

                        SEND_MSG(MSG_PHOTO_SCALE, msg->h_deswin, msg->h_deswin, SHOW_UI, SHOW_UI);
                        //      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);
                        break;
                    }
                }
            }

            lastkey = -1;
            break;

            case VK_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                switch (wnd_para->ctrl_para.show_para)
                {
                    case PHOTO_SHOW_PAUSE:
                    case PHOTO_SHOW_PLAY_PREV:
                    case PHOTO_SHOW_PLAY_NEXT:
                    {
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        anole_play_next();
                        break;
                    }

                    case PHOTO_SHOW_AUTO_PLAY:
                    {
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        APP_ANOLE_Pause();
                        anole_play_next();
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        break;
                    }

                    case PHOTO_SHOW_SCALE:
                    {
                        __wrn("----right1\n");
                        wnd_para->ctrl_para.scale_old_item = wnd_para->ctrl_para.scale_new_item;
                        wnd_para->ctrl_para.scale_new_item++;

                        if (wnd_para->ctrl_para.scale_new_item > PHOTO_SCALE_ITEM_ZOOMINC)
                        {
                            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ROTATELEFT;
                        }

                        SEND_MSG(MSG_PHOTO_SCALE, msg->h_deswin, msg->h_deswin, SHOW_UI, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);
                        break;
                    }
                }
            }

            lastkey = -1;
            break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case  GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LEFT:
            {
                switch (wnd_para->ctrl_para.show_para)
                {
                    case PHOTO_SHOW_PAUSE:
                    case PHOTO_SHOW_PLAY_NEXT:
                    case PHOTO_SHOW_PLAY_PREV:
                    {
                        anole_play_prev();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, 0);
                        break;
                    }

                    case PHOTO_SHOW_AUTO_PLAY:
                    {
                        APP_ANOLE_Pause();
                        anole_play_prev();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, 0);
                        break;
                    }

                    case PHOTO_SHOW_SCALE:
                    {
                        __wrn("----longleft1\n");
                        wnd_para->ctrl_para.scale_old_item = wnd_para->ctrl_para.scale_new_item;
                        wnd_para->ctrl_para.scale_new_item--;

                        if (wnd_para->ctrl_para.scale_new_item < PHOTO_SCALE_ITEM_ROTATELEFT
                            || wnd_para->ctrl_para.scale_new_item > PHOTO_SCALE_ITEM_ZOOMINC)
                        {
                            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ZOOMINC;
                        }

                        SEND_MSG(MSG_PHOTO_SCALE, msg->h_deswin, msg->h_deswin, SHOW_UI, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);
                        break;
                    }
                }

                break;
            }

            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_RIGHT:
            {
                switch (wnd_para->ctrl_para.show_para)
                {
                    case PHOTO_SHOW_PAUSE:
                    case PHOTO_SHOW_PLAY_PREV:
                    case PHOTO_SHOW_PLAY_NEXT:
                    {
                        anole_play_next();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, 0);
                        break;
                    }

                    case PHOTO_SHOW_AUTO_PLAY:
                    {
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        APP_ANOLE_Pause();
                        anole_play_next();
                        //                  do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, 0);
                        break;
                    }

                    case PHOTO_SHOW_SCALE:
                    {
                        __wrn("----longright1\n");
                        wnd_para->ctrl_para.scale_old_item = wnd_para->ctrl_para.scale_new_item;
                        wnd_para->ctrl_para.scale_new_item++;

                        if (wnd_para->ctrl_para.scale_new_item > PHOTO_SCALE_ITEM_ZOOMINC)
                        {
                            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ROTATELEFT;
                        }

                        SEND_MSG(MSG_PHOTO_SCALE, msg->h_deswin, msg->h_deswin, SHOW_UI, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);
                        break;
                    }
                }

                break;
            }

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            {
                switch (wnd_para->ctrl_para.show_para)
                {
                    case PHOTO_SHOW_PAUSE:
                    case PHOTO_SHOW_PLAY_PREV:
                    case PHOTO_SHOW_PLAY_NEXT:
                    {
                        anole_play_next();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, 0);
                        break;
                    }

                    case PHOTO_SHOW_AUTO_PLAY:
                    {
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, SHOW_UI);
                        APP_ANOLE_Pause();
                        anole_play_next();
                        //                  do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYNEXT, 0);
                        break;
                    }

                    case PHOTO_SHOW_SCALE:
                    {
                        __wrn("----longright1\n");
                        wnd_para->ctrl_para.scale_old_item = wnd_para->ctrl_para.scale_new_item;
                        wnd_para->ctrl_para.scale_new_item += 1;

                        if (wnd_para->ctrl_para.scale_new_item > PHOTO_SCALE_ITEM_ZOOMINC)
                        {
                            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ROTATELEFT;
                        }

                        SEND_MSG(MSG_PHOTO_SCALE, msg->h_deswin, msg->h_deswin, SHOW_UI, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);
                        break;
                    }
                }

                break;
            }

            case  GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_UP:
            {
                switch (wnd_para->ctrl_para.show_para)
                {
                    case PHOTO_SHOW_PAUSE:
                    case PHOTO_SHOW_PLAY_NEXT:
                    case PHOTO_SHOW_PLAY_PREV:
                    {
                        anole_play_prev();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, 0);
                        break;
                    }

                    case PHOTO_SHOW_AUTO_PLAY:
                    {
                        APP_ANOLE_Pause();
                        anole_play_prev();
                        SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, msg->h_deswin, msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_PLAYPREV, 0);
                        break;
                    }

                    case PHOTO_SHOW_SCALE:
                    {
                        __wrn("----longleft1\n");
                        wnd_para->ctrl_para.scale_old_item = wnd_para->ctrl_para.scale_new_item;
                        wnd_para->ctrl_para.scale_new_item -= 1;

                        if (wnd_para->ctrl_para.scale_new_item < PHOTO_SCALE_ITEM_ROTATELEFT
                            || wnd_para->ctrl_para.scale_new_item > PHOTO_SCALE_ITEM_ZOOMINC)
                        {
                            wnd_para->ctrl_para.scale_new_item = PHOTO_SCALE_ITEM_ZOOMINC;
                        }

                        SEND_MSG(MSG_PHOTO_SCALE, msg->h_deswin, msg->h_deswin, SHOW_UI, SHOW_UI);
                        //                      do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);
                        break;
                    }
                }

                break;
            }
        }

        lastkey = msg->dwAddData1;
    }

    return EPDK_OK;
}

/************************************************************************/
//命令消息处理
/************************************************************************/
static __s32 on_photo_main_frw_command(__gui_msg_t *msg)
{
    PHOTO_MAIN_FRW_T    *wnd_para;

    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);
    //  switch (msg->dwAddData1)
    //  {
    //  case :
    //      break;
    //  }
    return 0;
}

#define SCALE_PANEL_W       800
#define SCALE_PANEL_H       480
#define SCALE_BTN_W         70
#define SCALE_BTN_H         70
#define CENTER_VALUE(_v1, _v2, _v)      (((_v1) + (_v2) - (_v)) / 2)
#define SCALE_PANEL_X_CT    CENTER_VALUE(0, SCALE_PANEL_W, SCALE_BTN_W)
#define SCALE_PANEL_Y_CT    CENTER_VALUE(0, SCALE_PANEL_H, SCALE_BTN_H)
#define SCALE_PANEL_X_END   (SCALE_PANEL_W - SCALE_BTN_W)
#define SCALE_PANEL_Y_END   (SCALE_PANEL_H - SCALE_BTN_H)
/************************************************************************/
//
/************************************************************************/
static int32_t hit_test_scale_panel_btn(int32_t x, int32_t y)
{
    TOUCH_UI_RECT rt[] =
    {
        {0,                 0,  SCALE_BTN_W,    SCALE_BTN_H},
        {SCALE_PANEL_X_CT,  0,  SCALE_BTN_W,    SCALE_BTN_H},
        {SCALE_PANEL_X_END, 0,  SCALE_BTN_W,    SCALE_BTN_H},

        {0,                 SCALE_PANEL_Y_CT,   SCALE_BTN_W,    SCALE_BTN_H},
        {SCALE_PANEL_X_CT,  SCALE_PANEL_Y_CT,   SCALE_BTN_W,    SCALE_BTN_H},
        {SCALE_PANEL_X_END, SCALE_PANEL_Y_CT,   SCALE_BTN_W,    SCALE_BTN_H},

        {0,                 SCALE_PANEL_Y_END,  SCALE_BTN_W,    SCALE_BTN_H},
        {SCALE_PANEL_X_CT,  SCALE_PANEL_Y_END,  SCALE_BTN_W,    SCALE_BTN_H},
        {SCALE_PANEL_X_END, SCALE_PANEL_Y_END,  SCALE_BTN_W,    SCALE_BTN_H},
    };
    __photo_scale_item_draw_e item[] =
    {
        PHOTO_SCALE_ITEM_ROTATELEFT,
        PHOTO_SCALE_ITEM_MOVE_UP,
        PHOTO_SCALE_ITEM_RPTATERIGHT,
        PHOTO_SCALE_ITEM_MOVE_LEFT,
        PHOTO_SCALE_ITEM_EXIT,
        PHOTO_SCALE_ITEM_MOVE_RIGHT,
        PHOTO_SCALE_ITEM_ZOOMDEC,
        PHOTO_SCALE_ITEM_MOVE_DOWN,
        PHOTO_SCALE_ITEM_ZOOMINC,
    };
    return item[touch_check_rect_sz(rt, BEETLES_TBL_SIZE(rt), (TOUCH_UI_INT)x, (TOUCH_UI_INT)y)];
}

#if 0
/************************************************************************/
//触摸消息处理(zoomin)
/************************************************************************/
static __s32 on_photo_main_frw_touch_zoomin(H_WIN hwnd, __s32 x, __s32 y, __s32 pixel_change)
{
    return APP_ANOLE_TouchZoomOut(pixel_change); //局部放大(gui发送的消息字面意义相反了)
}

/************************************************************************/
//触摸消息处理(zoomout)
/************************************************************************/
static __s32 on_photo_main_frw_touch_zoomout(H_WIN hwnd, __s32 x, __s32 y, __s32 pixel_change)
{
    return APP_ANOLE_TouchZoomIn(pixel_change); //局部缩小(gui发送的消息字面意义相反了)
}

/************************************************************************/
//触摸消息处理(down)
/************************************************************************/
static __s32 on_photo_main_frw_touch_down(H_WIN hwnd, __s32 x, __s32 y)
{
    PHOTO_MAIN_FRW_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, hwnd);
    __msg("on_photo_main_frw_touch_down(%d, %d)\n", x, y);
    __msg("Anole sta = %d\n", APP_ANOLE_GetSta());
    wnd_para->ctrl_para.down_pos.x = x;
    wnd_para->ctrl_para.down_pos.y = y;
    return EPDK_OK;
}

/************************************************************************/
//触摸消息处理(move)
/************************************************************************/
static __s32 on_photo_main_frw_touch_move(H_WIN hwnd, __s32 x, __s32 y)
{
    __s32 delta_x, delta_y;
    PHOTO_MAIN_FRW_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, hwnd);
    __msg("on_photo_main_frw_touch_move(%d, %d)\n", x, y);
    __msg("Anole sta = %d\n", APP_ANOLE_GetSta());

    if (wnd_para->ctrl_para.last_pos.x == 0 && wnd_para->ctrl_para.last_pos.y == 0)
    {
        wnd_para->ctrl_para.last_pos.x = x;
        wnd_para->ctrl_para.last_pos.y = y;
    }

    delta_x = x - wnd_para->ctrl_para.last_pos.x;
    delta_y = y - wnd_para->ctrl_para.last_pos.y;
    APP_ANOLE_TouchMove(delta_x, delta_y);
    wnd_para->ctrl_para.last_pos.x = x;
    wnd_para->ctrl_para.last_pos.y = y;
    return EPDK_OK;
}

/************************************************************************/
//触摸消息处理(up)
/************************************************************************/
static __s32 on_photo_main_frw_touch_up(H_WIN hwnd, __s32 x, __s32 y)
{
    __s32 ret = EPDK_FAIL;
    PHOTO_MAIN_FRW_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, hwnd);
    __msg("on_photo_main_frw_touch_up(%d, %d)\n", x, y);
    __msg("Anole sta = %d, show_para = %d\n", APP_ANOLE_GetSta(), wnd_para->ctrl_para.show_para);
    //为下一次动作清空数据
    wnd_para->ctrl_para.last_pos.x = 0;
    wnd_para->ctrl_para.last_pos.y = 0;

    if (wnd_para->ctrl_para.show_para == PHOTO_SHOW_SCALE)//TODO:be a function
    {
        __s32 id;
        id = hit_test_scale_panel_btn(x, y);

        if (0 <= id)
        {
            todo_anole_command(hwnd, ID_ANOLE_CMD_SCALE, id);
            ret = EPDK_OK;
        }
    }
    else
    {
        //ret = APP_ANOLE_TouchSwitchPic(x - wnd_para->ctrl_para.down_pos.x, y - wnd_para->ctrl_para.down_pos.y);
    }

    return ret;
}
#endif

/************************************************************************/
//触摸消息处理
/************************************************************************/
static int32_t photo_main_frw_touch_proc(__gui_msg_t *msg)
{
    int32_t     speed = 0, direct = 0;
    int32_t     x = 0, y = 0;
    PHOTO_MAIN_FRW_T    *wnd_para;

    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);

    x   = LOSWORD(msg->dwAddData2);

    y   = HISWORD(msg->dwAddData2);

    //__wrn(" %s  %d----------photo frm win 触摸到(%d,%d)\n",__FILE__,__LINE__,x,y );
    speed   = LOSWORD(msg->dwReserved);

    direct  = HISWORD(msg->dwReserved);

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

#ifdef __MSG
    {
        // for debug
        char *str[] =
        {
            Symbol2Str(GUI_MSG_TOUCH_DOWN),
            Symbol2Str(GUI_MSG_TOUCH_UP),
            Symbol2Str(GUI_MSG_TOUCH_MOVE),
            Symbol2Str(GUI_MSG_TOUCH_ZOOMIN),
            Symbol2Str(GUI_MSG_TOUCH_ZOOMOUT),
            Symbol2Str(GUI_MSG_TOUCH_NCDOWN),
            Symbol2Str(GUI_MSG_TOUCH_NCUP),
            Symbol2Str(GUI_MSG_TOUCH_NCMOVE),
            Symbol2Str(GUI_MSG_TOUCH_OVERDOWN),
            Symbol2Str(GUI_MSG_TOUCH_OVERUP),
            Symbol2Str(GUI_MSG_TOUCH_OVERMOVE),
            Symbol2Str(GUI_MSG_TOUCH_OVERZOOMIN),
            Symbol2Str(GUI_MSG_TOUCH_OVERZOOMOUT),
            Symbol2Str(GUI_MSG_TOUCH_LONGDOWN),
            Symbol2Str(GUI_MSG_TOUCH_OVERLONGDOWN),
        };

        if (msg->dwAddData1 - GUI_MSG_TOUCH_DOWN < BEETLES_TBL_SIZE(str))
        {
            DEBUG_photo("photo_main_frw_touch_proc[ %s ](%d, %d, %d, %d)\n", str[msg->dwAddData1 - GUI_MSG_TOUCH_DOWN], x, y, speed, direct);
        }
        else
        {
            DEBUG_photo("photo_main_frw_touch_proc[%x](%d, %d, %d, %d)\n", msg->dwAddData1, x, y, speed, direct);
        }
    }
#endif

    if (APP_ANOLE_IsEnableTouch())
    {
        msg->h_deswin   = GetPhotoManWnd();

        //GUI_SendMessage(msg);               /*发给man win*/
        /*删除窗口、释放资源*/
        do_photo_command(msg->h_deswin, ID_PHOTO_CMD_KILL_PHOTO_FRMWIN, HIDE_UI);

        wnd_para->ctrl_para.show_para   = PHOTO_SHOW_NULL;
#if  0

        if (APP_ANOLE_Touch(x, y, msg->dwAddData1, LOSWORD(msg->dwReserved)))
        {
            __here__;
            return EPDK_OK;
        }

#endif
    }

    __msg("APP_ANOLE_Touch do nothing\n");
    {
        PHOTO_MAIN_FRW_CTRL_PARA_T *ctrl_para   = &wnd_para->ctrl_para;

        if (msg->dwAddData1 == GUI_MSG_TOUCH_UP || msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP)
        {
            if (ctrl_para->show_para == PHOTO_SHOW_SCALE)
            {
                int32_t     id;

                id  = hit_test_scale_panel_btn(x, y);

                if (TouchValidID(id))
                {
                    wnd_para->ctrl_para.scale_old_item  = wnd_para->ctrl_para.scale_new_item;
                    wnd_para->ctrl_para.scale_new_item  = id;

                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_SCALE, SHOW_UI);

                    todo_anole_command(msg->h_deswin, ID_ANOLE_CMD_SCALE, id);
                }
            }
            else
            {
                if (ctrl_para->last_action == GUI_MSG_TOUCH_DOWN || ctrl_para->last_action == GUI_MSG_TOUCH_LONGDOWN)
                {
                    //无移动操作
                    int32_t     d1, d2;

                    d1  = ABS(x - ctrl_para->last_pos.x);
                    d2  = ABS(y - ctrl_para->last_pos.y);

                    if (d1 <= TouchMinDist() && d2 <= TouchMinDist())
                    {
                        do_photo_command(msg->h_deswin, ID_PHOTO_CMD_TOUCH_UP, msg->dwAddData2);
                    }
                }
            }

            //为下一次动作清空数据
            ctrl_para->last_pos.x   = 0;
            ctrl_para->last_pos.y   = 0;
        }
        else
        {
            ctrl_para->last_pos.x   = x;
            ctrl_para->last_pos.y   = y;
            ctrl_para->last_action  = msg->dwAddData1;
        }
    }
    return EPDK_OK;
}

/************************************************************************/
//窗口销毁处理
/************************************************************************/
static int32_t on_photo_main_frw_destory(__gui_msg_t *msg)
{
    PHOTO_MAIN_FRW_T    *wnd_para;

    PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);

    //TODO:release resource
    do_anole_command(msg->h_deswin, ID_ANOLE_CMD_UNINIT, 0);

    photo_main_frw_destroy_res(&wnd_para->res);

    //  BFreeType(wnd_para, PHOTO_MAIN_FRW_T);
    __msg("!!----on_photo_main_frw_destory end\n");
    return 0;
}

/************************************************************************/
//窗口创建处理
/************************************************************************/
static int32_t on_photo_main_frw_create(__gui_msg_t *msg)
{
    PHOTO_MAIN_FRW_T    *wnd_frm_para;
    PHOTO_WND_T         *wnd_para;

    wnd_para        = (PHOTO_WND_T *)GUI_WinGetAttr(msg->h_deswin); /*创建窗口的时候获取窗口私有参数，他是一个PHOTO_WND_T 的一个指针*/

    wnd_frm_para    = wnd_para->photo_frm_para; /*重新设置frm 窗口的属性数据*/

    //BallocType(wnd_para, PHOTO_MAIN_FRW_T);
    ASSERT(wnd_frm_para != NULL);

    //  ZeroTypeMem(wnd_para, PHOTO_MAIN_FRW_T);
    GUI_WinSetAttr(msg->h_deswin, wnd_frm_para);

    //TODO:init wnd_para
    photo_main_frw_create_res(&wnd_frm_para->res);

    do_anole_command(msg->h_deswin, ID_ANOLE_CMD_INIT, 0);

    GUI_WinSetFocusChild(msg->h_deswin);
    return 0;
}

//消息处理函数
static int32_t _photo_main_frw_proc(__gui_msg_t *msg)
{
    APP_DEBUG_CBMSGEx;

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            return on_photo_main_frw_create(msg);
        }

        case GUI_MSG_DESTROY:
        {
            return on_photo_main_frw_destory(msg);
        }

        case GUI_MSG_COMMAND:
        {
            on_photo_main_frw_command(msg);
        }
        break;

        case GUI_MSG_KEY:
        {
            photo_main_frw_key_proc(msg);
        }
        break;

        case GUI_MSG_TOUCH:
        {
            return photo_main_frw_touch_proc(msg);
        }

        case GUI_MSG_PAINT:
        {
            on_photo_main_frw_paint(msg->h_deswin);
        }
        break;

        case MSG_PHOTO_NEW_PLAY:
        {
            PHOTO_MAIN_FRW_T    *wnd_para;

            PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);

            if (wnd_para && wnd_para->ctrl_para.show_para == PHOTO_SHOW_AUTO_PLAY)
            {
                todo_anole_command(msg->h_deswin, ID_PHOTO_CMD_AUTOPLAY, SHOW_UI);
            }

            return EPDK_OK;
        }

        case MSG_PHOTO_SHOW_HIDE_PARA:
        {
            PHOTO_MAIN_FRW_T    *wnd_para = NULL ;
            H_LYR   layer;

            PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);

            if (wnd_para)
            {
                if (msg->dwAddData2 == SHOW_UI)
                {
                    switch (msg->dwAddData1)
                    {
                        case ID_ANOLE_CMD_FILEINFO:
                        {
                            wnd_para->ctrl_para.show_para = PHOTO_SHOW_FILEINFO;
                            break;
                        }

                        case ID_PHOTO_CMD_PAUSE:
                        {
                            wnd_para->ctrl_para.show_para = PHOTO_SHOW_PAUSE;
                            break;
                        }

                        case ID_PHOTO_CMD_AUTOPLAY:
                        {
                            wnd_para->ctrl_para.show_para = PHOTO_SHOW_AUTO_PLAY;
                            break;
                        }

                        case ID_PHOTO_CMD_PLAYPREV:
                        {
                            wnd_para->ctrl_para.show_para = PHOTO_SHOW_PLAY_PREV;
                            break;
                        }

                        case ID_PHOTO_CMD_PLAYNEXT:
                        {
                            wnd_para->ctrl_para.show_para = PHOTO_SHOW_PLAY_NEXT;
                            break;
                        }
                    }

                    layer   = GUI_WinGetLyrWin(msg->h_deswin);

                    __wrn("GUI_LyrWinGetSta(layer) is %d %x\n", GUI_LyrWinGetSta(layer), layer);

                    GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
                }
                else
                {
                    /*发消息删除窗口释放资源*/
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_KILL_PHOTO_FRMWIN, msg->dwAddData2);
                }
            }
        }

        return EPDK_OK;

        case MSG_PHOTO_SCALE:
        {
            PHOTO_MAIN_FRW_T    *wnd_para;

            PHOTO_GetWndPara(wnd_para, PHOTO_MAIN_FRW_T, msg->h_deswin);

            if (wnd_para)
            {
                if (msg->dwAddData2 == SHOW_UI)
                {
                    wnd_para->ctrl_para.show_para = PHOTO_SHOW_SCALE;
                    APP_ANOLE_EnableTouch(EPDK_FALSE);
                    GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
                }
                else
                {
                    /*发消息删除窗口释放资源*/
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_KILL_PHOTO_FRMWIN, msg->dwAddData2);

                    //APP_ANOLE_EnableTouch(EPDK_TRUE);
                }
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

//窗口创建
H_WIN photo_main_frw_create(H_WIN h_parent, H_LYR layer, void *para)
{
    __gui_framewincreate_para_t framewin_para;
    FB  fb;

    __log("asfafe photo_main_frw_create");

    GUI_LyrWinGetFB(layer, &fb);

    ZeroTypeMem(&framewin_para, __gui_framewincreate_para_t);

    framewin_para.name          = PHOTO_MAIN_FRW_NAME;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = PHOTO_MAIN_FRW_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_photo_main_frw_proc);
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



#include <log.h>
#include "beetles_app.h"

#include "explorer_dialog_frame.h"

static __s32 explorer_dialog_tab[] =
{
    STRING_EXPLR_COPY,
    STRING_EXPLR_PARSE,
    STRING_EXPLR_CUT,
    STRING_EXPLR_DEL,
    STRING_EXPLR_CANNEL,
};

static __s32 explorer_dialog_cmd[] =
{
    EXPLORER_DIALOG_COPY,
    EXPLORER_DIALOG_PARSE,
    EXPLORER_DIALOG_CUT,
    EXPLORER_DIALOG_DEL,
    EXPLORER_DIALOG_CANCEL,

};

enum
{
    ID_EXPLR_COPY,
    ID_EXPLR_PARSE,
    ID_EXPLR_CUT,
    ID_EXPLR_DEL,
    ID_EXPLR_CANNEL,

    ID_EXPLR_MAX
};

//draw bg of dialog
static __s32  explorer_dialog_draw_bmp(__s32 bmp_id, __s32 x, __s32 y)
{
    void *p_buf;
    p_buf = explorer_get_listview_icon_res(bmp_id);

    if (p_buf)
    {
        GUI_BMP_Draw(p_buf, x, y);
    }

    return EPDK_OK;
}



//draw bg of dialog
static __s32  explorer_draw_bg_dialog(void)
{
    __s32 x, y;
    void *p_buf;
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();
    x = ui_param->cp_mov_dialog_coor.x;
    y = ui_param->cp_mov_dialog_coor.y;
    explorer_dialog_draw_bmp(ID_EXP_CP_MOV, x, y);
    return EPDK_OK;
}

//draw string on bg of dialog
static __s32  explorer_draw_string_dialog(explr_dialog_para_t *dlg_para)
{
    __s32 i;
    __s32 height, width;
    GUI_RECT GUI_TextRect;
    explorer_viewer_ui_t *ui_param;
    char str[1024];
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    ui_param = explorer_get_viewer_ui_param();
    //根据dialog背景图尺寸求得每个item的height  \ width
    //并根据起始坐标画字符串
    height = ui_param->cp_mov_dialog_coor.height / ID_EXPLR_MAX;
    width = ui_param->cp_mov_dialog_coor.width;

    for (i = 0; i < ID_EXPLR_MAX; i++)
    {
        GUI_TextRect.x0 = ui_param->cp_mov_dialog_coor.x;
        GUI_TextRect.y0 = ui_param->cp_mov_dialog_coor.y + i * height;
        GUI_TextRect.x1 = (GUI_TextRect.x0 + width);
        GUI_TextRect.y1 = (GUI_TextRect.y0 + height);

        if (i == dlg_para->focus_id)
        {
            explorer_dialog_draw_bmp(ID_EXP_CP_MOV_F, GUI_TextRect.x0, GUI_TextRect.y0 + 2);
        }

        dsk_langres_get_menu_text(explorer_dialog_tab[i], str, sizeof(str)) ;
        GUI_DispStringInRect(str, &GUI_TextRect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    return 0;
}


static __s32 explorer_update_dlg_item(explr_dialog_para_t *dlg_para)
{
    GUI_MEMDEV_Handle    draw_mem;
    explorer_viewer_ui_t *ui_param;
    RECT rect;
    ui_param = explorer_get_viewer_ui_param();
    rect.x = ui_param->cp_mov_dialog_coor.x ;
    rect.y = ui_param->cp_mov_dialog_coor.y ;
    rect.width = ui_param->cp_mov_dialog_coor.width ;
    rect.height = ui_param->cp_mov_dialog_coor.height ;
    GUI_LyrWinSel(dlg_para->dialog_layer);
    draw_mem = GUI_MEMDEV_Create(rect.x, rect.y, rect.width, rect.height);
    GUI_MEMDEV_Select(draw_mem);
    explorer_draw_bg_dialog();
    explorer_draw_string_dialog(dlg_para);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}


/*
*****************************************************************************************
*Function   :           static __s32 explorer_dialog_onpaint(__gui_msg_t *msg)
*
*Description    :       OnPaint处理函数,画背景图，前景ListBar showpage, 其他icon元素
*
*Arguments      :
*
*Return         :
******************************************************************************************
*/
static __s32 explorer_dialog_onpaint(__gui_msg_t *msg)
{
    __s32 ret = 0;
    explr_dialog_para_t *dlg_para;
    explorer_viewer_ui_t *ui_param;
    file_list_t *pfilelist = NULL;
    dlg_para = (explr_dialog_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ui_param = explorer_get_viewer_ui_param();

    if (GUI_LyrWinGetSta(dlg_para->dialog_layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(dlg_para->dialog_layer) == GUI_LYRWIN_STA_ON)
    {
        __msg("to make sure layer status on\n");
    }

    GUI_LyrWinSel(dlg_para->dialog_layer);
    GUI_LyrWinSetTop(dlg_para->dialog_layer);
    explorer_update_dlg_item(dlg_para);
    //GUI_WinSetFocusChild(dlg_win);
    return EPDK_OK;
}

//释放Listwindow
static __s32 explorer_dlg_frame_win_delete(H_WIN win)
{
    GUI_FrmWinDelete(win);
    return EPDK_OK;
}


static __s32 explorer_dlg_win_on_destroy(__gui_msg_t *msg)
{
    explr_dialog_para_t *dlg_para;
    dlg_para = (explr_dialog_para_t *)GUI_WinGetAttr(msg->h_deswin);
    //GUI_LyrWinDelete(dlg_para->dialog_layer);
    //dlg_para->dialog_layer = NULL;
    esMEMS_Mfree(0, dlg_para);
    dlg_para = NULL;
    return 0;
}

static __s32 explorer_dlg_pre_item(explr_dialog_para_t *dlg_para)
{
    if (dlg_para->focus_id > 0)
    {
        dlg_para->focus_id--;
    }
    else
    {
        dlg_para->focus_id = ID_EXPLR_MAX - 1;
    }

    explorer_update_dlg_item(dlg_para);
    return 0;
}



static __s32 explorer_dlg_next_item(explr_dialog_para_t *dlg_para)
{
    if (dlg_para->focus_id < ID_EXPLR_MAX - 1)
    {
        dlg_para->focus_id++;
    }
    else
    {
        dlg_para->focus_id = 0;
    }

    explorer_update_dlg_item(dlg_para);
    return 0;
}

static __s32 explorer_dlg_win_on_command(__gui_msg_t *msg)
{
    explr_dialog_para_t *dlg_para;
    dlg_para = (explr_dialog_para_t *)GUI_WinGetAttr(msg->h_deswin);

    switch (HIWORD(msg->dwAddData1))
    {
        case ADLG_CMD_EXIT:
            if (dlg_para->h_dialog_yesno != NULL)
            {
                app_dialog_destroy(dlg_para->h_dialog_yesno);       //to delete dialog

                if (msg->dwAddData2 == ADLG_IDYES)
                {
                    __gui_msg_t dmsg;
                    eLIBs_memset(&dmsg, 0, sizeof(__gui_msg_t));
                    dmsg.id = GUI_MSG_COMMAND;
                    dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    dmsg.dwAddData1 = explorer_dialog_cmd[dlg_para->focus_id];
                    GUI_SendNotifyMessage(&dmsg);
                }
                else//ADLG_IDNO
                {
                    __msg("fw not update...\n");
                    explorer_update_dlg_item(dlg_para);
                }

                dlg_para->h_dialog_yesno = NULL;
            }

            break;
    }

    return EPDK_OK;
}


static __s32 explorer_dlg_win_key_proc(__gui_msg_t *msg)
{
    explr_dialog_para_t *dlg_para;
    static __s32 last_key = 0;
    dlg_para = (explr_dialog_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if ((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                explorer_dlg_next_item(dlg_para);
            }
            break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            {
                explorer_dlg_pre_item(dlg_para);
            }
            break;

            case GUI_MSG_KEY_VADD:      // 用于九宫格和列表模式切换
                break;

            case GUI_MSG_KEY_VDEC:
                break;

            case GUI_MSG_KEY_ENTER:
                last_key = GUI_MSG_KEY_ENTER;
                break;

            case GUI_MSG_KEY_MENU:
                //case GUI_MSG_KEY_ESCAPE:
                last_key = GUI_MSG_KEY_MENU;
                break;

            case GUI_MSG_KEY_LONGMENU:
                last_key = GUI_MSG_KEY_LONGMENU;
                break;

            case GUI_MSG_KEY_RISE:
                break;

            default:
                break;
        }
    }
    else if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_MENU:
            {
                if (last_key == GUI_MSG_KEY_MENU)       //回到上一级菜单.
                {
                }

                break ;
            }

            case GUI_MSG_KEY_LONGMENU:
            {
                if (last_key == GUI_MSG_KEY_LONGMENU) //USB and SD switch
                {
                }

                break;
            }

            case GUI_MSG_KEY_ENTER:
            {
                //if(EXPLORER_DIALOG_CANCEL == explorer_dialog_cmd[dlg_para->focus_id])
                {
                    //   __s32 lang_id[2] = {0,STRING_EXPLR_DELETE_CONFIRM};
                    //  default_dialog(dlg_para->h_dialog_yesno,GUI_WinGetParent(msg->h_deswin),EXPLR_DIALOG_WIN_ID,ADLG_YESNOCANCEL,lang_id)
                }
                //else
                {
                    __gui_msg_t dmsg;
                    eLIBs_memset(&dmsg, 0, sizeof(__gui_msg_t));
                    dmsg.id = GUI_MSG_COMMAND;
                    dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    dmsg.dwAddData1 = explorer_dialog_cmd[dlg_para->focus_id];
                    GUI_SendNotifyMessage(&dmsg);
                }
            }
            break;
        }
    }

    return EPDK_OK;
}


/*
************************************************************************************
*Function   :          static __s32 _explorer_dialog_win_cb(__gui_msg_t *msg)
*
*Description    :       ListWindow contrl block
*
*Arguments      :
*
*Return         :
*
*************************************************************************************
*/
static __s32 _explorer_dialog_win_cb(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    __gui_msg_t send;
    eLIBs_memset(&send, 0, sizeof(__gui_msg_t));

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            __msg("----explorer frame window GUI_MSG_CREATE begin----\n");
            //_explorer_listview_create(msg);
            return EPDK_OK;

        case GUI_MSG_PAINT:
            //__msg("----explorer frame window GUI_MSG_ONPAINT----\n");
            explorer_dialog_onpaint(msg);
            return EPDK_OK;

        case GUI_MSG_CLOSE:
            //__msg("----explorer frame window GUI_MSG_CLOSE begin----\n");
            //explorer_dlg_win_on_close(msg);
            explorer_dlg_frame_win_delete(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
            //__msg("----explorer frame window GUI_MSG_DESTROY begin----\n");
            explorer_dlg_win_on_destroy(msg);
            //__msg("----explorer frame window GUI_MSG_DESTROY end----\n");
            return EPDK_OK;

        case GUI_MSG_COMMAND:
            //__here__;
            explorer_dlg_win_on_command(msg);
            return EPDK_OK;

        case GUI_MSG_KEY:
            explorer_dlg_win_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            //explorer_dlg_win_touch_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TIMER:
            //explorer_list_on_timer(msg);
            return EPDK_OK ;

        case DSK_MSG_FS_PART_PLUGIN:
        {
            //__msg( "explorer list on DSK_MSG_FS_PART_PLUGIN msg begin \n " );
            //explorer_on_fs_part_in( msg );
            //__msg( "explorer list on DSK_MSG_FS_PART_PLUGIN msg end \n " );
            return EPDK_OK ;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            //explorer_on_fs_part_out( msg );
            return EPDK_OK ;
        }

        case DSK_MSG_ALARM: //explorer list 收到这个消息时，只需把当前弹出的窗口删除掉即可
        {
            /**explr_dialog_para_t *dlg_para;
            dlg_para = (explr_dialog_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if(dlg_para->h_dialog!=NULL)
            {
                app_dialog_destroy( dlg_para->h_dialog );       //to delete dialog
                dlg_para->h_dialog = NULL ;
            }
                if(dlg_para->h_dialog_msg!=NULL)
            {
                app_dialog_destroy( dlg_para->h_dialog_msg );       //to delete dialog
                dlg_para->h_dialog_msg = NULL ;
            }   **/
            return EPDK_OK ;
        }

        default:
            ret = GUI_FrmWinDefaultProc(msg);
            break;
    }

    return ret;
}

/*
********************************************************************************************
*Function   :      H_WIN explorer_dlg_win_create(H_WIN h_parent, explr_dialog_para_t *para)
*
*Description    :       根据ManagerWin传递过来的参数创建FrameWIn场景
*
*Arguments      :
*
*Return         :
*
********************************************************************************************
*/
H_WIN explorer_dialog_frame_win_create(H_WIN h_parent, explr_dialog_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    explr_dialog_para_t *DlgPara;
    FB fb;
    GUI_LyrWinGetFB(para->dialog_layer, &fb);
    DlgPara = (explr_dialog_para_t *)esMEMS_Malloc(0, sizeof(explr_dialog_para_t));

    if (!DlgPara)
    {
        __msg("memory alloc fail , return \n");
        return (H_WIN)EPDK_FAIL ;
    }

    eLIBs_memset((void *)DlgPara, 0, sizeof(explr_dialog_para_t));
    DlgPara->font = para->font;
    DlgPara->dialog_layer = para->dialog_layer;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "Explorer dialog window",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id        = EXPLR_DIALOG_WIN_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_explorer_dialog_win_cb);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 255;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)DlgPara;
    framewin_para.hLayer = DlgPara->dialog_layer;
    return (GUI_FrmWinCreate(&framewin_para));
}




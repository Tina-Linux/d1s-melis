#include "record_disk_view_frame.h"
#include <log.h>

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

extern __u32 app_record_check_disk(void);

static void rec_disk_view_frm_sendParent(__gui_msg_t *msg, __u32 id)
{
    __gui_msg_t sendMsg;
    sendMsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;
    GUI_SendNotifyMessage(&sendMsg);
}

static void _rec_disk_view_draw(__gui_msg_t *msg)
{
    prec_disk_view_frame_t pdiskviewFrm_ctl;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return;
    }

    draw_record_bmp_coor(&pdiskviewFrm_ctl->prec_ctrl->rec_ui, RECORD_FORMAT_BK_BMP, 182, 109);

    if (pdiskviewFrm_ctl->curRootType == RAT_TF)
    {
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_DispStringAt("USB", 220, 119);
        GUI_SetColor(APP_COLOR_YELLOW);
        GUI_DispStringAt("SD", 220, 145);
    }
    else if (pdiskviewFrm_ctl->curRootType == RAT_USB)
    {
        GUI_SetColor(APP_COLOR_YELLOW);
        GUI_DispStringAt("USB", 220, 119);
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_DispStringAt("SD", 220, 145);
    }
    else
    {
        GUI_SetColor(APP_COLOR_WHITE);
        GUI_DispStringAt("USB", 220, 119);
        GUI_DispStringAt("SD", 220, 145);
    }
}

static __s32 rec_disk_view_frm_on_create(__gui_msg_t *msg)
{
    prec_disk_view_frame_t pdiskviewFrm_ctl = NULL;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    pdiskviewFrm_ctl->recTimeID     = 0xFF;
    GUI_SetTimer(msg->h_deswin, pdiskviewFrm_ctl->recTimeID, 50, NULL);
    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_on_close(__gui_msg_t *msg)
{
    prec_disk_view_frame_t pdiskviewFrm_ctl;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_FrmWinDelete(msg->h_deswin);
    __wrn("frn win close .............\n");
    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_on_destory(__gui_msg_t *msg)
{
    prec_disk_view_frame_t pdiskviewFrm_ctl;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_KillTimer(msg->h_deswin, pdiskviewFrm_ctl->recTimeID);
    __wrn("----test 4\n");
    esMEMS_Mfree(0, pdiskviewFrm_ctl);
    __wrn("----test 5\n");
    GUI_WinSetAttr(msg->h_deswin, NULL);
    __wrn("----test 6\n");
    esKRNL_TimeDly(500);
    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_box_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __s32 ret = EPDK_FAIL;
    prec_disk_view_frame_t pdiskviewFrm_ctl;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (record_is_touch_icon(RECORD_DISK_VIEW_USB_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_UP)
        {
            if (pdiskviewFrm_ctl->curRootType == RAT_USB)
            {
                pdiskviewFrm_ctl->prec_ctrl->root_type = pdiskviewFrm_ctl->curRootType;
                rec_disk_view_frm_sendParent(msg, RECORD_TO_VIEW_FRAME);
            }
            else
            {
                pdiskviewFrm_ctl->curRootType = RAT_USB;
            }
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_DISK_VIEW_SD_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_UP)
        {
            if (pdiskviewFrm_ctl->curRootType == RAT_TF)
            {
                pdiskviewFrm_ctl->prec_ctrl->root_type = pdiskviewFrm_ctl->curRootType;
                rec_disk_view_frm_sendParent(msg, RECORD_TO_VIEW_FRAME);
            }
            else
            {
                pdiskviewFrm_ctl->curRootType = RAT_TF;
            }
        }

        ret = EPDK_OK;
    }
    else
    {
        rec_disk_view_frm_sendParent(msg, RECORD_TO_HOME);
    }

    _rec_disk_view_draw(msg);
    return ret;
}

static __s32 _rec_disk_view_frm_on_touch(__gui_msg_t *msg)
{
    __s32 speed = 0, direct = 0;
    __u16 x = 0, y = 0;
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);
    __msg("photo_main_frw_touch_proc(\n\n\n%d:   %02d, %02d, %02d, %02d)\n\n\n\n\n\n\n\n", msg->dwAddData1, x, y, speed, direct);

    if (_rec_disk_view_frm_box_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
    {
        return EPDK_OK;
    }

    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_on_key(__gui_msg_t *msg)
{
    prec_disk_view_frame_t pdiskviewFrm_ctl;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ENTER:
                break;

            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_RIGHT:
                if (pdiskviewFrm_ctl->curRootType == RAT_TF)
                {
                    pdiskviewFrm_ctl->curRootType = RAT_USB;
                }
                else
                {
                    pdiskviewFrm_ctl->curRootType = RAT_TF;
                }

                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_MENU:
                rec_disk_view_frm_sendParent(msg, RECORD_TO_HOME);
                break;

            case GUI_MSG_KEY_ENTER:
                pdiskviewFrm_ctl->prec_ctrl->root_type = pdiskviewFrm_ctl->curRootType;
                rec_disk_view_frm_sendParent(msg, RECORD_TO_VIEW_FRAME);
                break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_on_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_on_paint(__gui_msg_t *msg)
{
    GUI_RECT rect = {0, 0, 479, 271};
    prec_disk_view_frame_t pdiskviewFrm_ctl = NULL;
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pdiskviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_SetFont(pdiskviewFrm_ctl->sys_font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    return EPDK_OK;
}
static __s32 _rec_disk_view_frm_manager_proc(__gui_msg_t *msg)
{
    __msg("_rec_view_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return rec_disk_view_frm_on_create(msg);

        case GUI_MSG_CLOSE:
            return _rec_disk_view_frm_on_close(msg);

        case GUI_MSG_DESTROY:
            return _rec_disk_view_frm_on_destory(msg);

        case GUI_MSG_PAINT:
            return _rec_disk_view_frm_on_paint(msg);

        case GUI_MSG_KEY:
            return _rec_disk_view_frm_on_key(msg);

        case GUI_MSG_COMMAND:
            return _rec_disk_view_frm_on_command(msg);

        case GUI_MSG_TIMER:
        {
            __u32 disk;
            prec_disk_view_frame_t pdiskviewFrm_ctl;
            pdiskviewFrm_ctl = (prec_disk_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (pdiskviewFrm_ctl == NULL)
            {
                return EPDK_FAIL;
            }

            disk = app_record_check_disk();

            if (disk != ((RAT_TF << 8) | RAT_USB))
            {
                pdiskviewFrm_ctl->prec_ctrl->root_type = disk;
                rec_disk_view_frm_sendParent(msg, RECORD_TO_VIEW_FRAME);
            }
            else
            {
                _rec_disk_view_draw(msg);
            }
        }

        return EPDK_OK;

        case GUI_MSG_TOUCH:
            return _rec_disk_view_frm_on_touch(msg);

        case GUI_MSG_WIN_WAKEUP:
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            return EPDK_OK;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN rec_disk_view_frm_create(H_WIN h_parent, precord_ctrl_t para)
{
    __gui_framewincreate_para_t framewin_para;
    prec_disk_view_frame_t      pdiskviewFrm_ctl;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    pdiskviewFrm_ctl = (prec_disk_view_frame_t)esMEMS_Malloc(0, sizeof(rec_disk_view_frame_t));

    if (pdiskviewFrm_ctl == NULL)
    {
        __msg("esMEMS_Malloc fail\n");
        return NULL;
    }

    eLIBs_memset(pdiskviewFrm_ctl, 0, sizeof(rec_disk_view_frame_t));
    pdiskviewFrm_ctl->sys_font      = para->pfont;
    pdiskviewFrm_ctl->layer         = para->layer;
    pdiskviewFrm_ctl->prec_ctrl     = para;
    pdiskviewFrm_ctl->curRootType   = RAT_TF;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = "record_disk_view_frm",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = APP_RECORD_DISK_VIEW_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_rec_disk_view_frm_manager_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)pdiskviewFrm_ctl;
    framewin_para.hLayer        = para->layer;
    GUI_LyrWinSetSta(para->layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(para->layer);
    gscene_hbar_set_state(HBAR_ST_HIDE);
    return (GUI_FrmWinCreate(&framewin_para));
}



















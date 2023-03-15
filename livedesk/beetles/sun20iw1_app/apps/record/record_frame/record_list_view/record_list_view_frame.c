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
#include "record_list_view_frame.h"

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

static __s32 _rec_list_view_frm_play(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_pre(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_next(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_del(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_pause(__gui_msg_t *msg);
static __s32 _rec_list_view_frm_continue(__gui_msg_t *msg);




static __s32 _rec_list_view_frm_set_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}

static void _rec_list_view_frm_sendSelf(__gui_msg_t *msg, __u32 id)
{
    __gui_msg_t sendMsg;
    sendMsg.h_deswin = msg->h_deswin;
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;
    GUI_SendNotifyMessage(&sendMsg);
}

static void _rec_list_view_frm_sendParent(__gui_msg_t *msg, __u32 id)
{
    __gui_msg_t sendMsg;
    sendMsg.h_deswin = GUI_WinGetParent(msg->h_deswin);;
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;
    GUI_SendNotifyMessage(&sendMsg);
}

/*typedef struct tag_recordlist_theme_res_s//....
{
    __s32   res_id;
    HTHEME  h_bmp_handle;
    void*   res_buffer;

}recordlist_theme_res_t;
typedef enum tag_goto_nbmp_theme_res_s
{
     NMENU_BACK,
     NMENU_PRE,
     NMENU_,
     GOTO_4N,
     GOTO_5N,
     GOTO_6N,
     GOTO_7N,
     GOTO_8N,
     GOTO_9N,
     GOTO_0N,
     GOTO_DELN,
     GOTO_ENTERN,
     GOTO_QUITN,
     GOTO_NBMP_MAX,
}GOTO_NBMP_theme_res_e;


typedef struct tag_recordlist_menu_ctrl
{

  recordlist_theme_res_t   recordlist_nmenu[NMENU_MAX];
  recordlist_theme_res_t   recordlist_fmenu[FMENU_MAX];
}recordlist_menu_ctrl_t;
void recordlist_menu_init(void)
{
     eLIBs_memset( (void *)recordlist_nmenu ,  0 , sizeof(recordlist_nmenu ) );
     eLIBs_memset( (void *)recordlist_fmenu ,  0 , sizeof(recordlist_fmenu) );
     goto_ctrl->goto_bottom_bmp.res_id = ID_EBOOK_GOTO_BOTTOM_BMP ;
    goto_ctrl->goto_bottom_bmp.h_bmp_handle = theme_open( goto_ctrl->goto_bottom_bmp.res_id );
    goto_ctrl->goto_bottom_bmp.res_buffer = theme_hdl2buf( goto_ctrl->goto_bottom_bmp.h_bmp_handle  );
}
void recordlist_menu_uinit(void)
{
  dsk_theme_close( goto_ctrl->goto_res_nbmp[GOTO_0N].h_bmp_handle );
}//....*/


static __s32 _rec_list_view_frm_on_create(__gui_msg_t *msg)

{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    record_list_view_musicList_init(plistviewFrm_ctl->plistView, msg->h_deswin);
    plistviewFrm_ctl->plistDoc->musicTimeID = 0xff;
    GUI_SetTimer(msg->h_deswin, plistviewFrm_ctl->plistDoc->musicTimeID, REC_LIST_TIMER_TIMES, NULL);
    //record_list_play(plistviewFrm_ctl->plistDoc);  //默认不播放
    plistviewFrm_ctl->plistDoc->playSt = CEDAR_STAT_STOP;
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_close(__gui_msg_t *msg)
{
    GUI_FrmWinDelete(msg->h_deswin);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_destory(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_WinGetCaptureWin() == msg->h_deswin)
    {
        GUI_WinReleaseCapture();
    }

    if (plistviewFrm_ctl->delDlg)
    {
        app_dialog_destroy(plistviewFrm_ctl->delDlg);
        plistviewFrm_ctl->delDlg = NULL;
    }

    record_list_view_musicList_UnInit(plistviewFrm_ctl->plistView);
    record_list_MusciUninit(plistviewFrm_ctl->plistDoc);
    GUI_KillTimer(msg->h_deswin, plistviewFrm_ctl->plistDoc->musicTimeID);
    esMEMS_Mfree(0, plistviewFrm_ctl);
    GUI_WinSetAttr(msg->h_deswin, NULL);
    __wrn("normal escape 6\n");
    //esKRNL_TimeDly(500);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_menu_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
                if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACKWARD)
                {
                    record_list_backward(plistviewFrm_ctl->plistDoc);
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_FORWARD)
                {
                    record_list_forward(plistviewFrm_ctl->plistDoc);
                }

                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_PRESS;
                break;

            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
                if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_FORWARD)
                {
                    plistviewFrm_ctl->plistView->curMenu = plistviewFrm_ctl->plistView->playMenu;
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE ||
                         plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACKWARD;
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACK)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACK;
                    plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_LIST;
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
                }
                else
                {
                    plistviewFrm_ctl->plistView->curMenu = (MUSIC_LIST_MENU)((plistviewFrm_ctl->plistView->curMenu + MUSIC_MENU_MAX - 1) % MUSIC_MENU_MAX);
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                }

                break;

            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
                if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACKWARD)
                {
                    plistviewFrm_ctl->plistView->curMenu = plistviewFrm_ctl->plistView->playMenu;
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE ||
                         plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_FORWARD;
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_DEL)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_DEL;
                    plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_LIST;
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
                }
                else
                {
                    plistviewFrm_ctl->plistView->curMenu = (MUSIC_LIST_MENU)((plistviewFrm_ctl->plistView->curMenu + 1) % MUSIC_MENU_MAX);
                    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                }

                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
#if 0

            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_LONGPLAY_PAUSE:
                plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_LIST;
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
                break;
#endif

            //case GUI_MSG_KEY_LONGVADD:
            //  plistviewFrm_ctl->plistView->oldPart = plistviewFrm_ctl->plistView->curPart;
            //  plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_VOLUME;
            //  break;
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
                if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACK)
                {
                    _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
                    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
                    _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PAUSE);
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PLAY;
                    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;
                    _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_DEL)
                {
                    __s32 lang_id[] = {STRING_DELETE_RC, STRING_MSG4_RC};
                    //plistviewFrm_ctl->plistView->delState = RECORD_DEL_CAL;
                    plistviewFrm_ctl->plistView->curPart  = RECORD_MUSIC_DELETE;
                    //_rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PAUSE);
                    _rec_list_view_frm_pause(msg);

                    if (plistviewFrm_ctl->delDlg == NULL)
                    {
                        default_dialog(plistviewFrm_ctl->delDlg, GUI_WinGetParent(msg->h_deswin), RECORD_LIST_VIEW_FRAME_DELETE, ADLG_YESNO, lang_id);
                    }
                }
                else if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_BACKWARD ||
                         plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_FORWARD)
                {
                    _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
                }

                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                break;

            case GUI_MSG_KEY_LEFT:
                __wrn("RL3\n");
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                break;

            case GUI_MSG_KEY_RIGHT:
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                break;

            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_MODE:
                __wrn("normal escape 1\n");
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
                break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_list_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
                if (plistviewFrm_ctl->plistDoc->playIndex != plistviewFrm_ctl->plistDoc->curIndex)
                {
                    record_list_play(plistviewFrm_ctl->plistDoc);
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PLAY;
                    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;
                }
                else if (plistviewFrm_ctl->plistView->playMenu == MUSIC_MENU_PLAY)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
                    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
                    _rec_list_view_frm_pause(msg);
                }
                else if (plistviewFrm_ctl->plistView->playMenu == MUSIC_MENU_PAUSE)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PLAY;
                    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;
                    _rec_list_view_frm_continue(msg);
                }
                else
                {
                    record_list_play(plistviewFrm_ctl->plistDoc);
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PLAY;
                    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;
                }

                break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
                _rec_list_view_frm_pre(msg);
                break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
                _rec_list_view_frm_next(msg);
                break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_DEL;
            }
            break;

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACK;
            }
            break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_MODE:
                __wrn("normal escape 1\n");
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
                break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_delete_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LEFT:
                if (plistviewFrm_ctl->plistView->delState == RECORD_DEL_OK)
                {
                    plistviewFrm_ctl->plistView->delState = RECORD_DEL_CAL;
                }
                else
                {
                    plistviewFrm_ctl->plistView->delState = RECORD_DEL_OK;
                }

                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_MENU:
                __wrn("menu3\n");
                plistviewFrm_ctl->plistView->delState = RECORD_DEL_NONE;
                plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;

                if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
                    plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
                }

                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PLAY);
                break;

            case GUI_MSG_KEY_ENTER:
                if (plistviewFrm_ctl->plistView->delState == RECORD_DEL_OK)
                {
                    _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_DELETE);
                }

                plistviewFrm_ctl->plistView->delState = RECORD_DEL_NONE;
                plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;

                if (plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
                    plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE)
                {
                    plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
                }

                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PLAY);
                break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_volume_key(__gui_msg_t *msg)
{
    __s32 vol = 0;
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LONGLEFT: //volume down
            case GUI_MSG_KEY_LEFT:
                __wrn("*RL5\n");
                plistviewFrm_ctl->plistView->timeCnt = 0;
                vol = dsk_volume_get();

                if (vol != 0)
                {
                    dsk_volume_set(vol - 1);
                }

                break;

            case GUI_MSG_KEY_LONGRIGHT://volume up
            case GUI_MSG_KEY_RIGHT:
                plistviewFrm_ctl->plistView->timeCnt = 0;
                vol = dsk_volume_get();

                if (vol <= 30)
                {
                    dsk_volume_set(vol + 1);
                }

                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_VADD:
                __wrn("menu4\n");
                plistviewFrm_ctl->plistView->timeCnt = 0;
                plistviewFrm_ctl->plistView->curPart = plistviewFrm_ctl->plistView->oldPart;
                break;
        }
    }

    return EPDK_OK;
}

static __s32 _rec_list_view_frm_list_touch(__gui_msg_t *msg, __u32 keyState, __s32 x, __s32 y, __s32 direct, __s32 speed)
{
    __s32 ret = EPDK_FAIL;
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (keyState == GUI_MSG_TOUCH_DOWN)
    {
        if (LISTBAR_TouchDown(plistviewFrm_ctl->plistView->listbar_handle, x, y, direct, speed) == EPDK_OK)
        {
            plistviewFrm_ctl->isTouchMusicList = EPDK_TRUE;
            GUI_WinSetCaptureWin(msg->h_deswin);
            ret = EPDK_OK;
        }
    }
    else if (keyState == GUI_MSG_TOUCH_UP)
    {
        if (GUI_WinGetCaptureWin() == msg->h_deswin && plistviewFrm_ctl->isTouchMusicList)
        {
            if (plistviewFrm_ctl->isTouchMusicListMove == EPDK_FALSE)
            {
                _rec_list_view_frm_play(msg);
            }

            plistviewFrm_ctl->isTouchMusicListMove = EPDK_FALSE;
            plistviewFrm_ctl->isTouchMusicList = EPDK_FALSE;
            GUI_WinReleaseCapture();
            LISTBAR_TouchUp(plistviewFrm_ctl->plistView->listbar_handle, x, y, direct, speed);
            ret = EPDK_OK;
        }
    }
    else if (keyState == GUI_MSG_TOUCH_MOVE)
    {
        if (plistviewFrm_ctl->isTouchMusicList)
        {
            GUI_LongStringStop(plistviewFrm_ctl->plistView->long_str_handle);
            plistviewFrm_ctl->isTouchMusicListMove = EPDK_TRUE;
            ret = LISTBAR_TouchMove(plistviewFrm_ctl->plistView->listbar_handle, x, y, direct, speed);
        }
    }

    return ret;
}

static __s32 _rec_list_view_playing_bar_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __u32 total_time = 0, cur_time = 0;
    __u16 width = 0;
    GUI_RECT *rect = NULL;
    __s32 ret = EPDK_FAIL;
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (keyState == GUI_MSG_TOUCH_UP)
    {
        if (GUI_WinGetCaptureWin() == msg->h_deswin && plistviewFrm_ctl->isTouchPlayingBar)
        {
            GUI_WinReleaseCapture();
            plistviewFrm_ctl->isTouchPlayingBar = EPDK_FALSE;
            robin_set_cmd_jump(plistviewFrm_ctl->plistDoc->musicCurTimes);
            ret = EPDK_OK;
        }
    }
    else if (keyState == GUI_MSG_TOUCH_MOVE || keyState == GUI_MSG_TOUCH_DOWN || keyState == GUI_MSG_TOUCH_LONGDOWN)
    {
        if (plistviewFrm_ctl->isTouchPlayingBar == EPDK_TRUE)
        {
            rect = record_get_touch_rect(RECORD_LIST_VIEW_PLAYINGBAR_TOUCH);
            total_time = plistviewFrm_ctl->plistDoc->musicMaxTimes;
            width = rect->x1 - rect->x0;

            if (x > rect->x1)
            {
                x = rect->x1;
            }
            else if (x < rect->x0)
            {
                x = rect->x0;
            }

            plistviewFrm_ctl->plistDoc->musicCurTimes = (x - rect->x0) * total_time / width;
            //__here__;
            record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
            ret = EPDK_OK;
        }
    }

    if (record_is_touch_icon(RECORD_LIST_VIEW_PLAYINGBAR_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_DOWN)
        {
            //__here__;
            plistviewFrm_ctl->isTouchPlayingBar = EPDK_TRUE;
            GUI_WinSetCaptureWin(msg->h_deswin);
        }

        ret = EPDK_OK;
    }

    return ret;
}

static __s32 _rec_list_view_menu_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __s32 ret = EPDK_FAIL;
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;

    if (record_is_touch_icon(RECORD_LIST_VIEW_BACK_TOUCH, x, y)) //返回
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACK;
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
            _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
        }

        record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_LIST_VIEW_PREV_TOUCH, x, y)) //快退
    {
        if (keyState == GUI_MSG_TOUCH_DOWN)
        {
            plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACKWARD;
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
            record_list_backward(plistviewFrm_ctl->plistDoc);
            //..plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_BACKWARD;
            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
        }
        else if (keyState == GUI_MSG_TOUCH_LONGDOWN)
        {
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
            plistviewFrm_ctl->plistDoc->musicMaxTimes = robin_get_total_time();
            plistviewFrm_ctl->plistDoc->musicCurTimes = robin_get_cur_time();
            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
            _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
            GUI_ResetTimer(msg->h_deswin, plistviewFrm_ctl->plistDoc->musicTimeID, REC_LIST_TIMER_TIMES, NULL);
            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_LIST_VIEW_PLAY_PAUSE_TOUCH, x, y)) //播放暂停
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            if (plistviewFrm_ctl->plistView->playMenu == MUSIC_MENU_PLAY)
            {
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PLAY;
            }
            else
            {
                plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_PAUSE;
            }

            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            if (plistviewFrm_ctl->plistView->playMenu == MUSIC_MENU_PLAY)
            {
                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
                record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
            }
            else
            {
                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;
                plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
                record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
                _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_PAUSE);
            }

            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_LIST_VIEW_PLAY_NEXT_TOUCH, x, y)) //快进
    {
        if (keyState == GUI_MSG_TOUCH_DOWN)
        {
            plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_FORWARD;
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
            record_list_forward(plistviewFrm_ctl->plistDoc);
            //..plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_FORWARD;
            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
        }
        else if (keyState == GUI_MSG_TOUCH_LONGDOWN)
        {
            plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_FORWARD;
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
            plistviewFrm_ctl->plistDoc->musicMaxTimes = robin_get_total_time();
            plistviewFrm_ctl->plistDoc->musicCurTimes = robin_get_cur_time();
            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
            _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_CONTINUE);
            GUI_ResetTimer(msg->h_deswin, plistviewFrm_ctl->plistDoc->musicTimeID, REC_LIST_TIMER_TIMES, NULL);
            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_LIST_VIEW_DELE_TOUCH, x, y)) //删除
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            plistviewFrm_ctl->plistView->curMenu = MUSIC_MENU_DEL;
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_OVER;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            __s32 lang_id[] = {STRING_DELETE_RC, STRING_MSG4_RC};
            plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
            record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..

            if (plistviewFrm_ctl->delDlg == NULL)
            {
                _rec_list_view_frm_pause(msg);
                default_dialog(plistviewFrm_ctl->delDlg, GUI_WinGetParent(msg->h_deswin), RECORD_LIST_VIEW_FRAME_DELETE, ADLG_YESNO, lang_id);
            }

            //..plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
        }

        ret = EPDK_OK;
    }

    //..record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
    //..record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
    return ret;
}

static __s32 _rec_list_view_frm_on_touch(__gui_msg_t *msg)
{
    __s32 speed = 0, direct = 0;
    __s16 x = 0, y = 0;
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);
    __msg("photo_main_frw_touch_proc(\n\n\n%d:   %02d, %02d, %02d, %02d)\n\n\n\n\n\n\n\n", msg->dwAddData1, x, y, speed, direct);

    if (x >= 367 && y >= 150 && x <= 767 && y <= 170)
    {
        if (_rec_list_view_playing_bar_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
        {
            return EPDK_OK;
        }
    }
    else if (x >= 0 && y >= 29 && x <= 334 && y <= 480)
    {
        if (_rec_list_view_frm_list_touch(msg, msg->dwAddData1, x, y, direct, speed) == EPDK_OK)
        {
            return EPDK_OK;
        }
    }
    else
    {
        if (_rec_list_view_menu_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
        {
            return EPDK_OK;
        }
    }

    return EPDK_OK;
}

static __s32 _rec_list_view_frm_on_key(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_LIST)
    {
        _rec_list_view_frm_on_list_key(msg);
    }
    else if (plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_DELETE)
    {
        _rec_list_view_frm_on_delete_key(msg);
    }
    else if (plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_VOLUME)
    {
        _rec_list_view_frm_on_volume_key(msg);
    }
    else
    {
        _rec_list_view_frm_on_menu_key(msg);
    }

    record_list_icon_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);//..
    //..record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_command(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (plistviewFrm_ctl)
    {
        switch (LOWORD(msg->dwAddData1))
        {
            case RECORD_LIST_VIEW_FRAME_DELETE:
            {
                if (ADLG_IDYES == msg->dwAddData2)
                {
                    //_rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_DELETE);
                    if (plistviewFrm_ctl->plistView->playMenu == MUSIC_MENU_PAUSE)
                    {
                        record_list_continue(plistviewFrm_ctl->plistDoc);
                    }

                    _rec_list_view_frm_del(msg);
                }
                else
                {
                    if (plistviewFrm_ctl->plistView->playMenu == MUSIC_MENU_PAUSE)
                    {
                        record_list_continue(plistviewFrm_ctl->plistDoc);
                    }
                }

                app_dialog_destroy(plistviewFrm_ctl->delDlg);
                plistviewFrm_ctl->delDlg = NULL;
                break;
            }

            default:
                break;
        }
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_on_paint(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_SetFont(plistviewFrm_ctl->sys_font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_back(__gui_msg_t *msg)
{
    //prec_list_view_frame_t plistviewFrm_ctl;
    //plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);
    __wrn("normal escape 3\n");
    _rec_list_view_frm_sendParent(msg, RECORD_TO_VIEW_FRAME);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_pre(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    record_list_pre(plistviewFrm_ctl->plistDoc);
    //LISTBAR_SetFocusItem(plistviewFrm_ctl->plistView->listbar_handle, plistviewFrm_ctl->plistDoc->curIndex);
    //LISTBAR_UpdatePage(plistviewFrm_ctl->plistView->listbar_handle);
    LISTBAR_PrevItem(plistviewFrm_ctl->plistView->listbar_handle);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_play(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    record_list_play(plistviewFrm_ctl->plistDoc);
    plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_continue(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    record_list_continue(plistviewFrm_ctl->plistDoc);
    LISTBAR_SetFocusItem(plistviewFrm_ctl->plistView->listbar_handle, plistviewFrm_ctl->plistDoc->curIndex);
    LISTBAR_UpdatePage(plistviewFrm_ctl->plistView->listbar_handle);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_pause(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    record_list_pause(plistviewFrm_ctl->plistDoc);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_next(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    __wrn("record list next\n");
    record_list_next(plistviewFrm_ctl->plistDoc);
    //LISTBAR_SetFocusItem(plistviewFrm_ctl->plistView->listbar_handle, plistviewFrm_ctl->plistDoc->curIndex);
    //LISTBAR_UpdatePage(plistviewFrm_ctl->plistView->listbar_handle);
    //LISTBAR_ShowPage(plistviewFrm_ctl->plistView->listbar_handle);
    LISTBAR_NextItem(plistviewFrm_ctl->plistView->listbar_handle);
    return EPDK_OK;
}
static __s32 _rec_list_view_frm_del(__gui_msg_t *msg)
{
    prec_list_view_frame_t plistviewFrm_ctl;
    plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (plistviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (record_list_delete(plistviewFrm_ctl->plistDoc) == EPDK_FAIL)
    {
        __wrn("------test1\n");
        _rec_list_view_frm_sendSelf(msg, RECORD_LIST_VIEW_FRAME_BACK);
    }
    else
    {
        LISTBAR_SetFocusItem(plistviewFrm_ctl->plistView->listbar_handle, plistviewFrm_ctl->plistDoc->curIndex);
        LISTBAR_ShowPage(plistviewFrm_ctl->plistView->listbar_handle);
        LISTBAR_SetTotalItem(plistviewFrm_ctl->plistView->listbar_handle, plistviewFrm_ctl->plistDoc->total);
        plistviewFrm_ctl->plistView->curPart = RECORD_MUSIC_MENU;
    }

    return EPDK_OK;
}
static __s32 _rec_list_view_frm_manager_proc(__gui_msg_t *msg)
{
    __msg("_rec_list_view_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return _rec_list_view_frm_on_create(msg);

        case GUI_MSG_CLOSE:
            return _rec_list_view_frm_on_close(msg);

        case GUI_MSG_DESTROY:
            return _rec_list_view_frm_on_destory(msg);

        //case GUI_MSG_PAINT:
        // return _rec_list_view_frm_on_paint(msg);

        case GUI_MSG_KEY:
            return _rec_list_view_frm_on_key(msg);

        case GUI_MSG_COMMAND:
            return _rec_list_view_frm_on_command(msg);

        case GUI_MSG_TIMER:
        {
            prec_list_view_frame_t plistviewFrm_ctl;
            plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (plistviewFrm_ctl == NULL)
            {
                return EPDK_FAIL;
            }

            if (plistviewFrm_ctl->plistView->curPart == RECORD_MUSIC_VOLUME)
            {
                plistviewFrm_ctl->plistView->timeCnt++;

                if (plistviewFrm_ctl->plistView->timeCnt > VOLUME_EXIST_TIMES)
                {
                    plistviewFrm_ctl->plistView->timeCnt = 0;
                    plistviewFrm_ctl->plistView->curPart = plistviewFrm_ctl->plistView->oldPart;
                }
            }

            if (record_list_timer(plistviewFrm_ctl->plistDoc) == EPDK_FAIL)
            {
                /*if( plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PLAY ||
                    plistviewFrm_ctl->plistView->curMenu == MUSIC_MENU_PAUSE )
                    plistviewFrm_ctl->plistView->curMenu  = MUSIC_MENU_PLAY;
                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PLAY;*/
                //record_list_playNext(plistviewFrm_ctl->plistDoc);
                //LISTBAR_NextItem(plistviewFrm_ctl->plistView->listbar_handle);
                //..LISTBAR_SetFocusItem(plistviewFrm_ctl->plistView->listbar_handle, plistviewFrm_ctl->plistDoc->curIndex);
                //..LISTBAR_UpdatePage(plistviewFrm_ctl->plistView->listbar_handle);
                // LISTBAR_NextItem(plistviewFrm_ctl->plistView->listbar_handle);
                plistviewFrm_ctl->plistView->playMenu = MUSIC_MENU_PAUSE;
            }

            record_list_view_show(plistviewFrm_ctl->plistView, plistviewFrm_ctl->layer);
        }

        return EPDK_OK;

        case RECORD_LIST_VIEW_FRAME_BACK:
        {
            _rec_list_view_frm_back(msg);
        }

        return EPDK_OK;

        case RECORD_LIST_VIEW_FRAME_PRE:
        {
            _rec_list_view_frm_pre(msg);
        }
        break;

        case RECORD_LIST_VIEW_FRAME_NEXT:
        {
            _rec_list_view_frm_next(msg);
        }
        break;

        case RECORD_LIST_VIEW_FRAME_PLAY:
        {
            _rec_list_view_frm_play(msg);
        }
        break;

        case RECORD_LIST_VIEW_FRAME_CONTINUE:
        {
            __wrn("RECORD_LIST_VIEW_FRAME_CONTINUE\r\n");
            _rec_list_view_frm_continue(msg);
        }
        break;

        case RECORD_LIST_VIEW_FRAME_PAUSE:
        {
            __wrn("RECORD_LIST_VIEW_FRAME_PAUSE\r\n");
            _rec_list_view_frm_pause(msg);
        }
        break;

        case RECORD_LIST_VIEW_FRAME_DELETE:
        {
            __wrn("RECORD_LIST_VIEW_FRAME_DELETE\r\n");
            _rec_list_view_frm_del(msg);
        }
        break;

        case GUI_MSG_TOUCH:
            return _rec_list_view_frm_on_touch(msg);

        case GUI_MSG_WIN_WAKEUP:
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            return EPDK_OK;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN rec_list_view_frm_create(H_WIN h_parent, precord_ctrl_t para)
{
    __gui_framewincreate_para_t framewin_para;
    prec_list_view_frame_t      plistviewFrm_ctl;
    FB fb;
    plistviewFrm_ctl = (prec_list_view_frame_t)esMEMS_Malloc(0, sizeof(rec_list_view_frame_t));

    if (plistviewFrm_ctl == NULL)
    {
        __msg("esMEMS_Malloc fail\n");
        return NULL;
    }

    eLIBs_memset(plistviewFrm_ctl, 0, sizeof(rec_list_view_frame_t));
    GUI_LyrWinSetSta(para->record_list_layer, GUI_LYRWIN_STA_SLEEP);
    plistviewFrm_ctl->sys_font = para->pfont;
    plistviewFrm_ctl->layer    = para->record_list_layer;
    plistviewFrm_ctl->plistView = &para->rec_listView;
    plistviewFrm_ctl->plistDoc = &para->rec_listDoc;
    plistviewFrm_ctl->plistView->precord_list_doc = &para->rec_listDoc;
    plistviewFrm_ctl->plistView->sys_font = plistviewFrm_ctl->sys_font;
    plistviewFrm_ctl->plistView->layer = plistviewFrm_ctl->layer;
    plistviewFrm_ctl->isTouchPlayingBar = EPDK_FALSE;
    plistviewFrm_ctl->isTouchMusicList = EPDK_FALSE;
    plistviewFrm_ctl->isTouchMusicListMove = EPDK_FALSE;
    plistviewFrm_ctl->plistDoc->curRootType = para->root_type;          //get from app_record

    if (record_list_MusicInit(plistviewFrm_ctl->plistDoc) == EPDK_FAIL)   //错误返回
    {
        esMEMS_Mfree(0, plistviewFrm_ctl);
        __wrn("record_list_MusicInit fail!\r\n");
        return NULL;
    }

    plistviewFrm_ctl->plistView->curMenu    = MUSIC_MENU_BACK;
    plistviewFrm_ctl->plistView->curMenuSt = FOCUSE_NORMAL;
    plistviewFrm_ctl->plistView->playMenu   = MUSIC_MENU_PLAY;
    plistviewFrm_ctl->plistView->curPart    = RECORD_MUSIC_LIST;
    plistviewFrm_ctl->plistView->delState   = RECORD_DEL_NONE;
    plistviewFrm_ctl->plistView->timeCnt    = 0;
    GUI_LyrWinGetFB(plistviewFrm_ctl->layer, &fb);
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = "record_list_view_frm",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = APP_RECORD_LIST_VIEW_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_rec_list_view_frm_manager_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)plistviewFrm_ctl;
    framewin_para.hLayer        = plistviewFrm_ctl->layer;
    GUI_LyrWinSetSta(plistviewFrm_ctl->layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(plistviewFrm_ctl->layer);
    _rec_list_view_frm_set_title(STRING_RECORD_PLAYER_RC);
    gscene_hbar_set_state(HBAR_ST_SHOW);
    return (GUI_FrmWinCreate(&framewin_para));
}































#include "record_view_frame.h"
#include <log.h>

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

//#define TEST_DUAL_TP

#ifdef TEST_DUAL_TP
static __s32 OpenTpDualMode(void)
{
    __s32 ret = EPDK_FAIL;
    ES_FILE *tp_hdl = NULL;
    tp_hdl = eLIBs_fopen("b:\\INPUT\\TP", "r+");

    if (tp_hdl == NULL)
    {
        __msg("open tp handle fail...\n\n\n\n\n\n\n");
    }
    else
    {
        ret =  esMODS_MIoctrl(tp_hdl, DRV_TP_CMD_SET_WORKMODE, DRV_TP_MODE_DUAL, NULL);

        if (ret == EPDK_FAIL)
        {
            __msg("set tp dual mode fail...\n\n\n\n\n\n\n\n");
        }

        __msg("Open Tp Dual Mode...\n");
        eLIBs_fclose(tp_hdl);
    }

    return ret;
}
static __s32 CloseTpDualMode(void)
{
    __s32 ret = EPDK_FAIL;
    ES_FILE *tp_hdl = NULL;
    tp_hdl = eLIBs_fopen("b:\\INPUT\\TP", "r+");

    if (tp_hdl == NULL)
    {
        __msg("open tp handle fail...\n\n\n\n\n\n\n\n");
    }
    else
    {
        ret =  esMODS_MIoctrl(tp_hdl, DRV_TP_CMD_SET_WORKMODE, DRV_TP_MODE_NORMAL, NULL);

        if (ret == EPDK_FAIL)
        {
            __msg("set tp normal mode fail...\n\n\n\n\n\n\n\n");
        }

        __msg("Close Tp Dual Mode(MODE_NORMAL, now)...\n");
        eLIBs_fclose(tp_hdl);
    }

    return ret;
}
#endif

static __s32 _rec_view_frm_set_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}

static void rec_view_frm_sendSelf(__gui_msg_t *msg, __u32 id)
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

static void rec_view_frm_sendParent(__gui_msg_t *msg, __u32 id)
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

static __s32 rec_view_frm_on_create(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    //eLIBs_printf("rec view frm create .....\n");
    GUI_LyrWinSetSta(pviewFrm_ctl->layer, GUI_LYRWIN_STA_SLEEP);
    //eLIBs_printf("GUI_LYRWIN_STA_SLEEP .....\n");
    //eLIBs_printf("_rec_view_frm_on_paint .....\n");
    pviewFrm_ctl->recTimeID    = APP_RECORD_VIEW_ID + 0xFF;
    GUI_SetTimer(msg->h_deswin, pviewFrm_ctl->recTimeID, REC_TIMER_TIMES, NULL);
    return EPDK_OK;
}
static __s32 _rec_view_frm_on_close(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (pviewFrm_ctl->precordDoc->recState != RECORD_STOP)
    {
        rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
    }

    GUI_FrmWinDelete(msg->h_deswin);
    return EPDK_OK;
}
static __s32 _rec_view_frm_on_destory(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_KillTimer(msg->h_deswin, pviewFrm_ctl->recTimeID);
    record_unInit(pviewFrm_ctl->precordDoc);
    esMEMS_Mfree(0, pviewFrm_ctl);
#ifdef TEST_DUAL_TP
    CloseTpDualMode();

    if (GUI_WinGetCaptureWin() == msg->h_deswin)
    {
        GUI_WinReleaseCapture();
    }

#endif
    GUI_WinSetAttr(msg->h_deswin, NULL);
    return EPDK_OK;
}
#if 0
static __s32 _rec_view_frm_box_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __s32 ret = EPDK_FAIL;
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (pviewFrm_ctl->pview->curMenu == MENU_QUALITY && pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS)
    {
        if (record_is_touch_icon(RECORD_VIEW_POOR_QUALITY_TOUCH, x, y))
        {
            if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
            {
                pviewFrm_ctl->precordDoc->recQuality = pviewFrm_ctl->precordDoc->recQualityTemp = QUALITY_NORMAL;
                record_updateFreeTime(pviewFrm_ctl->precordDoc);
            }
        }
        else if (record_is_touch_icon(RECORD_VIEW_NORMAL_QUALITY_TOUCH, x, y))
        {
            if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
            {
                pviewFrm_ctl->precordDoc->recQuality = pviewFrm_ctl->precordDoc->recQualityTemp = QUALITY_GOOD;
                record_updateFreeTime(pviewFrm_ctl->precordDoc);
            }
        }
        else if (record_is_touch_icon(RECORD_VIEW_GOOD_QUALITY_TOUCH, x, y))
        {
            if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
            {
                pviewFrm_ctl->precordDoc->recQuality = pviewFrm_ctl->precordDoc->recQualityTemp = QUALITY_VERY_GOOD;
                record_updateFreeTime(pviewFrm_ctl->precordDoc);
            }
        }
        else
        {
            if (keyState == GUI_MSG_TOUCH_DOWN)
            {
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_NORMAL;
            }
        }

        ret = EPDK_OK;
    }

    /*else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT && pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS )
    {
        if( record_is_touch_icon(RECORD_VIEW_MP3_TOUCH, x, y) )
        {
            if( keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN )
            {
                pviewFrm_ctl->precordDoc->audioType = pviewFrm_ctl->precordDoc->audioTypeTemp = RECORD_MP3;
            }
        }
        else if( record_is_touch_icon(RECORD_VIEW_WAV_TOUCH, x, y) )
        {
            if( keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN )
            {
                pviewFrm_ctl->precordDoc->audioType = pviewFrm_ctl->precordDoc->audioTypeTemp = RECORD_WAV;
            }
        }
    #if 0
        else if( record_is_touch_icon(RECORD_VIEW_WMA_TOUCH, x, y) )
        {
            if( keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN )
            {
                pviewFrm_ctl->precordDoc->audioType = pviewFrm_ctl->precordDoc->audioTypeTemp = RECORD_WMA;
            }
            ret = EPDK_OK;
        }
    #endif
        else
        {
            if( keyState == GUI_MSG_TOUCH_DOWN )
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_NORMAL;
        }
        ret = EPDK_OK;
    }*/
    record_updateview_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    return ret;
}
static __s32 _rec_view_frm_bottom_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __s32 ret = EPDK_FAIL;
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    if (record_is_touch_icon(RECORD_VIEW_BACK_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            pviewFrm_ctl->pview->curMenu = MENU_BACK;
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            rec_view_frm_sendParent(msg, RECORD_TO_HOME);
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_VIEW_QUALITY_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            pviewFrm_ctl->pview->curMenu = MENU_QUALITY;
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_VIEW_RECORD_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            pviewFrm_ctl->pview->curMenu = MENU_RECORD;
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            if (pviewFrm_ctl->precordDoc->recState == RECORD_STOP)
            {
                rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STARTRECORD);
            }
            else if (pviewFrm_ctl->precordDoc->recState == RECORD_START)
            {
                rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
            }
            else
            {
                rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STARTRECORD);
            }
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_VIEW_SAVE_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            pviewFrm_ctl->pview->curMenu = MENU_SAVE;
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_NORMAL;
            rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STOPRECORD);
        }

        ret = EPDK_OK;
    }
    else if (record_is_touch_icon(RECORD_VIEW_MUSIC_LIST_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            pviewFrm_ctl->pview->curMenu = MENU_MUSIC_LIST;
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_NORMAL;

            if (record_get_files(pviewFrm_ctl->precordDoc) != 0)
            {
                rec_view_frm_sendParent(msg, RECORD_TO_LIST_VIEW_FRAME);
            }
            else
            {
                pviewFrm_ctl->precordDoc->timeCnt = 0;
            }
        }

        ret = EPDK_OK;
    }
    /*else if( record_is_touch_icon(RECORD_VIEW_FORMAT_TOUCH, x, y) )
    {
        if( keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN )
        {
            pviewFrm_ctl->pview->curMenu = MENU_FORMAT;
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
        }
        else if( keyState == GUI_MSG_TOUCH_UP )
        {
        }
        ret = EPDK_OK;
    }*/
    else
    {
        if (pviewFrm_ctl->pview->curMenu != MENU_QUALITY /*&& pviewFrm_ctl->pview->curMenu != RECORD_VIEW_FORMAT_TOUCH*/)
        {
            pviewFrm_ctl->pview->curMenuSt = FOCUSE_NORMAL;
        }
    }

    if (ret == EPDK_OK)
    {
        record_updateview_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    }

    //..record_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    return ret;
}

static __s32 _rec_view_frm_on_touch(__gui_msg_t *msg)
{
    __s32 speed = 0, direct = 0;
    __u16 x = 0, y = 0;
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);
    __msg("photo_main_frw_touch_proc(%d:   %02d, %02d, %02d, %02d)\n", msg->dwAddData1, x, y, speed, direct);
#ifdef TEST_DUAL_TP

    if (msg->dwAddData1 == GUI_MSG_TOUCH_ZOOMIN)
    {
        __inf("\n\n\n\n\n                    DOUBLE TOUCH IN!\n\n\n");
        pviewFrm_ctl->pview->curMenu = MENU_SAVE;
        pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
    }
    else if (msg->dwAddData1 == GUI_MSG_TOUCH_ZOOMOUT)
    {
        __inf("\n\n\n\n\n                    DOUBLE TOUCH OUT!\n\n\n");
        pviewFrm_ctl->pview->curMenu = MENU_BACK;
        pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
    }

    return EPDK_OK;
#endif

    if (_rec_view_frm_bottom_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
    {
        return EPDK_OK;
    }

    if (_rec_view_frm_box_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
    {
        return EPDK_OK;
    }

    return EPDK_OK;
}
#endif
static __s32 _rec_view_frm_on_key(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (pviewFrm_ctl == NULL)
    {
        __wrn("pviewFrm_ctl == NULL\r\n");
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION != msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
            {
                switch (pviewFrm_ctl->pview->curMenu)
                {
                    case MENU_RECORD:
                    {
                        /*if( pviewFrm_ctl->precordDoc->recState == RECORD_STOP )
                        rec_view_frm_sendSelf(msg,RECORD_VIEW_FRAME_STARTRECORD);
                        else */
                        if (pviewFrm_ctl->precordDoc->recState == RECORD_START)
                        {
                            rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
                        }
                        else
                        {
                            rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STARTRECORD);
                        }
                    }
                    break;
#if !RECORD_CHANGE_0815

                    case MENU_BACK:
                    {
                        if (pviewFrm_ctl->precordDoc->recState != RECORD_STOP)
                        {
                            rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
                        }

                        rec_view_frm_sendParent(msg, RECORD_TO_HOME);
                    }
                    break;
#endif

                    case MENU_MUSIC_LIST:
                    {
                        if (pviewFrm_ctl->precordDoc->recState != RECORD_STOP)
                        {
                            rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
                        }

                        if (record_get_files(pviewFrm_ctl->precordDoc) != 0)
                        {
                            pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
                            rec_view_frm_sendParent(msg, RECORD_TO_LIST_VIEW_FRAME);
                        }
                        else
                        {
                            H_WIN Default_Dialog = NULL;
                            __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_ROOT_NO_ENY_AUDIO_FILE};
                            default_dialog(Default_Dialog, GUI_WinGetParent(msg->h_deswin), 0x2, ADLG_OK, lang_id);
                            esKRNL_TimeDly(300);
                            app_dialog_destroy(Default_Dialog);
                            Default_Dialog = NULL ;
                            pviewFrm_ctl->precordDoc->timeCnt = 0;
                        }
                    }
                    break;
#if !RECORD_CHANGE_0815

                    case MENU_QUALITY:
                    {
                        //pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER;
                        //pviewFrm_ctl->pview->curMenu   = MENU_RECORD;
                        if (pviewFrm_ctl->precordDoc->recState != RECORD_STOP)
                        {
                            rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_PAUSERECORD);
                        }

                        pviewFrm_ctl->precordDoc->recQuality = pviewFrm_ctl->precordDoc->recQualityTemp;
                        record_updateFreeTime(pviewFrm_ctl->precordDoc);
                    }
                    break;
#endif

                    case MENU_SAVE:
                    {
                        rec_view_frm_sendSelf(msg, RECORD_VIEW_FRAME_STOPRECORD);
                    }
                    break;

                    default:
                        break;
                }

                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
            }
            break;
#if !RECORD_CHANGE_0815

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:

                //case GUI_MSG_KEY_PLAY_PAUSE:
                if (pviewFrm_ctl->pview->curMenu == MENU_QUALITY && pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS)
                {
                    pviewFrm_ctl->precordDoc->recQualityTemp = (QUALITY_STATE)((pviewFrm_ctl->precordDoc->recQualityTemp + 1) % QUALITY_STATE_MAX);
                }

                /*else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT &&
                        pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS )
                {
                    pviewFrm_ctl->precordDoc->audioTypeTemp = (RECORD_AUDIO_TYPE)((pviewFrm_ctl->precordDoc->audioTypeTemp+1)%RECORD_AUDIO_TYPE_MAX);
                }*/
                break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_SEL_LIST:
                if (pviewFrm_ctl->pview->curMenu == MENU_QUALITY && pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS)
                {
                    if (pviewFrm_ctl->precordDoc->recQualityTemp == 0)
                    {
                        pviewFrm_ctl->precordDoc->recQualityTemp = 3;
                    }

                    pviewFrm_ctl->precordDoc->recQualityTemp = (QUALITY_STATE)((pviewFrm_ctl->precordDoc->recQualityTemp - 1) % QUALITY_STATE_MAX);
                }

                /*else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT &&
                        pviewFrm_ctl->pview->curMenuSt == FOCUSE_PRESS )
                {
                    pviewFrm_ctl->precordDoc->audioTypeTemp = (RECORD_AUDIO_TYPE)((pviewFrm_ctl->precordDoc->audioTypeTemp+1)%RECORD_AUDIO_TYPE_MAX);
                }*/
                break;
#endif
#if RECORD_CHANGE_0815

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
#endif
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
                pviewFrm_ctl->pview->curMenu = (RECORD_MENU)((pviewFrm_ctl->pview->curMenu + MENU_MAX - 1) % MENU_MAX);
                __wrn("pviewFrm_ctl->pview->curMenu====%d\n", pviewFrm_ctl->pview->curMenu);
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
#if !RECORD_CHANGE_0815

                if (pviewFrm_ctl->pview->curMenu == MENU_QUALITY)
                {
                    pviewFrm_ctl->precordDoc->recQualityTemp = pviewFrm_ctl->precordDoc->recQuality;
                    pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                    pviewFrm_ctl->pview->menuOsffset = 0;
                }
                /*else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT )
                {
                    pviewFrm_ctl->precordDoc->audioTypeTemp = pviewFrm_ctl->precordDoc->audioType;
                    pviewFrm_ctl->pview->menuOsffset = -1;
                    pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                }*/
                else
#endif
                    if (pviewFrm_ctl->pview->curMenu == MENU_RECORD)
                    {
                        pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                    }

                break;
#if RECORD_CHANGE_0815

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
#endif
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
                pviewFrm_ctl->pview->curMenu = (RECORD_MENU)((pviewFrm_ctl->pview->curMenu + 1) % MENU_MAX);
                pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
#if !RECORD_CHANGE_0815

                if (pviewFrm_ctl->pview->curMenu == MENU_QUALITY)
                {
                    pviewFrm_ctl->precordDoc->recQualityTemp = pviewFrm_ctl->precordDoc->recQuality;
                    pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                    pviewFrm_ctl->pview->menuOsffset = 0;
                }
                /*else if( pviewFrm_ctl->pview->curMenu == MENU_FORMAT )
                {
                    pviewFrm_ctl->precordDoc->audioTypeTemp = pviewFrm_ctl->precordDoc->audioType;
                    pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                    pviewFrm_ctl->pview->menuOsffset = -1;
                }*/
                else
#endif
                    if (pviewFrm_ctl->pview->curMenu == MENU_RECORD)
                    {
                        pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                    }

                break;

            default:
                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_MODE:
                rec_view_frm_sendParent(msg, RECORD_TO_HOME);
                break;

            case GUI_MSG_KEY_ENTER:
                if (pviewFrm_ctl->pview->curMenu == MENU_SAVE)
                {
                    // pviewFrm_ctl->pview->curMenuSt = FOCUSE_OVER; //shiql change for save button
                    pviewFrm_ctl->pview->curMenuSt = FOCUSE_PRESS;
                }

                break;

            default:
                break;
        }
    }

    record_updateview_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    return EPDK_OK;
}
static __s32 _rec_view_frm_on_command(__gui_msg_t *msg)
{
    return EPDK_OK;
}
static __s32 _rec_view_frm_on_paint(__gui_msg_t *msg)
{
    prec_view_frame_t pviewFrm_ctl;
    pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

    if (GUI_LyrWinGetSta(pviewFrm_ctl->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("&&&&&&&&&&&&&&&&&&&   GUI_LYRWIN_STA_SUSPEND! &&&&&&&&&&&&&&&&&&& \n");
        return EPDK_FAIL;
    }

    GUI_SetFont(pviewFrm_ctl->sys_font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    record_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
    //GUI_LyrWinSetSta(pviewFrm_ctl->layer, GUI_LYRWIN_STA_OFF);
    return EPDK_OK;
}
static __s32 _rec_view_frm_manager_proc(__gui_msg_t *msg)
{
    __msg("_rec_view_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return rec_view_frm_on_create(msg);

        case GUI_MSG_CLOSE:
            return _rec_view_frm_on_close(msg);

        case GUI_MSG_DESTROY:
            return _rec_view_frm_on_destory(msg);

        case GUI_MSG_PAINT:
        {
            prec_view_frame_t pviewFrm_ctl;
            pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (GUI_LyrWinGetSta(pviewFrm_ctl->layer) == GUI_LYRWIN_STA_SUSPEND)
            {
                return EPDK_FAIL;
            }

            _rec_view_frm_on_paint(msg);
            GUI_LyrWinSetSta(pviewFrm_ctl->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(pviewFrm_ctl->layer);
            gscene_hbar_set_state(HBAR_ST_SHOW);
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
            return _rec_view_frm_on_key(msg);

        case GUI_MSG_COMMAND:
            return _rec_view_frm_on_command(msg);

        case RECORD_VIEW_FRAME_PAUSERECORD:
        {
            prec_view_frame_t pviewFrm_ctl;
            pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (pviewFrm_ctl == NULL)
            {
                break;
            }

            record_stop(pviewFrm_ctl->precordDoc, EPDK_FALSE);
            //pviewFrm_ctl->precordDoc->recState == RECORD_STOP;
            pviewFrm_ctl->precordDoc->recState = RECORD_STOP;
            //..record_pause(pviewFrm_ctl->precordDoc);
        }
        break;

        case RECORD_VIEW_FRAME_STARTRECORD:     //¿ªÊ¼Â¼Òô
        {
            prec_view_frame_t pviewFrm_ctl;
            pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (pviewFrm_ctl == NULL)
            {
                break;
            }

            if (pviewFrm_ctl->precordDoc->recState == RECORD_STOP)
            {
                __wrn("======RECORD_STOP\n");
                record_start(pviewFrm_ctl->precordDoc);
            }
            else if (pviewFrm_ctl->precordDoc->recState == RECORD_PAUSE)
            {
                __wrn("======RECORD_PAUSE\n");
                record_continue(pviewFrm_ctl->precordDoc);
            }
        }
        break;

        case RECORD_VIEW_FRAME_STOPRECORD:      //½áÊøÂ¼Òô
        {
            prec_view_frame_t pviewFrm_ctl;
            pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (pviewFrm_ctl == NULL)
            {
                break;
            }

            if (pviewFrm_ctl->precordDoc->recState != RECORD_STOP)
            {
                __wrn("======RECORD_STOP\n");
                record_stop(pviewFrm_ctl->precordDoc, EPDK_TRUE);
                pviewFrm_ctl->precordDoc->recState = RECORD_STOP;
            }
        }
        break;

        case GUI_MSG_TIMER:
        {
            prec_view_frame_t pviewFrm_ctl;
            pviewFrm_ctl = (prec_view_frame_t)GUI_WinGetAttr(msg->h_deswin);

            if (pviewFrm_ctl == NULL)
            {
                return EPDK_OK;
            }

            pviewFrm_ctl->precordDoc->timeCnt++;
            record_updateCurTime(pviewFrm_ctl->precordDoc);
            record_timer_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
            //..record_view_show(pviewFrm_ctl->pview, pviewFrm_ctl->layer);
        }

        return EPDK_OK;
#if 0

        case GUI_MSG_TOUCH:
            return _rec_view_frm_on_touch(msg);
#endif

        case GUI_MSG_WIN_WAKEUP:
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            return EPDK_OK;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN rec_view_frm_create(H_WIN h_parent, precord_ctrl_t para)
{
    __gui_framewincreate_para_t framewin_para;
    prec_view_frame_t           pviewFrm_ctl;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    pviewFrm_ctl = (prec_view_frame_t)esMEMS_Malloc(0, sizeof(rec_view_frame_t));

    if (pviewFrm_ctl == NULL)
    {
        __msg("esMEMS_Malloc fail\n");
        return NULL;
    }

    eLIBs_memset(pviewFrm_ctl, 0, sizeof(rec_view_frame_t));
    pviewFrm_ctl->sys_font      = para->pfont;
    pviewFrm_ctl->layer         = para->layer;
    pviewFrm_ctl->pview         = &para->rec_view;
    pviewFrm_ctl->precordDoc        = &para->rec_Doc;
    pviewFrm_ctl->pview->precord_doc = &para->rec_Doc;
    pviewFrm_ctl->pview->curMenu        = MENU_RECORD;
    pviewFrm_ctl->pview->curMenuSt      = FOCUSE_NORMAL;
    pviewFrm_ctl->pview->menuOsffset    = 0;
    pviewFrm_ctl->precordDoc->timeCnt   = NO_FILES_TIMES;
    pviewFrm_ctl->precordDoc->curRootType   = para->root_type;  //get from app_record

    if (para->bIsFMRecord)                                      //get from app_record
    {
        pviewFrm_ctl->precordDoc->recordType = RECORD_FM;
    }
    else
    {
        pviewFrm_ctl->precordDoc->recordType = RECORD_MIC;
    }

    pviewFrm_ctl->precordDoc->recQuality = QUALITY_GOOD;

    if (record_init(pviewFrm_ctl->precordDoc) == EPDK_FAIL)
    {
        __inf("rec_view_frm_create() create fail!\n");
        esMEMS_Mfree(0, pviewFrm_ctl);
        return NULL;
    }

    _rec_view_frm_set_title(STRING_RECORD_RC);
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = "record_view_frm",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = APP_RECORD_VIEW_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_rec_view_frm_manager_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)pviewFrm_ctl;
    framewin_para.hLayer        = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}



















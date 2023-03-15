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
#include "app_fm_i.h"
#include "fm_sset.h"
#include "fm_main.h"
/************************************************************************/
//
/************************************************************************/
H_WIN g_FMManWnd = NULL;
//////////////////////////////////////////////////////////////////////////
// 建立图层
#define fm_layer_create(_lyr, _prt, _name)   app_fm_layer_create_default(_lyr, _prt, _name)

//////////////////////////////////////////////////////////////////////////
static __s32 _fm_op_notify(signed long op_function);
static __s32 fmplay_finetune(__gui_msg_t *msg, __u8 ucChMod, __u8  playmode);


#define fm_op_notify(_fun)    _fm_op_notify((signed long)_fun)
typedef void *H_PARSER;

//////////////////////////////////////////////////////////////////////////
//当前找到一个有效频率
static void on_fm_test_freq_ok(FM_CTRLDATA_T *fm_ctrl)
{
    if (fm_ctrl->search_mode == SRHMODE_MANUAL) //手动搜索
    {
        dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//by cky
        __wrn("draw:fm_ctrl->cur_freq = %d", fm_ctrl->cur_freq);
    }
    else//SRHMODE_AUTO  自动搜索
    {
        if (dsk_radio_rcv_get_search_result_vn() != 0)
        {
            dsk_radio_chan_t chan_info;
            fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();
            fm_ctrl->channel_id = fm_ctrl->channel_count - 1;
            __wrn("@fm_ctrl->seach_channel_count = %d", fm_ctrl->channel_count);
            __wrn("@fm_ctrl->seach_channel_id = %d", fm_ctrl->channel_id);
            dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
            __wrn("@fm_ctrl->channel_id = %d", fm_ctrl->channel_id);
            fm_ctrl->cur_freq = chan_info.freq;
            __wrn("@:fm_ctrl->cur_freq = %d", fm_ctrl->cur_freq);
        }
    }
}

//当前找到的是无效频率
static void on_fm_test_freq_fail(FM_CTRLDATA_T *fm_ctrl)
{
    if (fm_ctrl->search_mode == SRHMODE_MANUAL) //手动搜索
    {
        dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
        __wrn("draw:fm_ctrl->cur_freq = %d", fm_ctrl->cur_freq);
    }
    else//SRHMODE_AUTO  自动搜索
    {
        dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
        __wrn("fm_ctrl->cur_freq = %d", fm_ctrl->cur_freq);
    }
}


//搜索结束
static void on_fm_test_freq_end(FM_CTRLDATA_T *fm_ctrl)
{
    dsk_radio_chan_t chan_info;

    if (fm_ctrl->search_mode == SRHMODE_MANUAL) //手动搜索
    {
        dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq); //by cky modify
        //dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
        //fm_ctrl->cur_freq = chan_info.freq;
        __wrn("fm_ctrl->channel_id = %d,fm_ctrl->cur_freq = %d", fm_ctrl->channel_id, fm_ctrl->cur_freq);
    }
    else//SRHMODE_AUTO  自动搜索
    {
        fm_ctrl->channel_id = 0;

        if (dsk_radio_rcv_get_search_result_vn() == 0) //no channel info
        {
            fm_ctrl->cur_freq = fm_ctrl->cur_min_freq;
            fm_ctrl->channel_count = 1;
            __wrn("find 0 channel, select min-freq %d as channel 1", fm_ctrl->cur_freq);
        }
        else
        {
            dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
            fm_ctrl->cur_freq = chan_info.freq;
            fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();
            __wrn("find %d channel(s), select %d as channel 1", fm_ctrl->channel_count, fm_ctrl->cur_freq);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //更新列表
    {
        reg_fm_para_t *para;
        get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
        dsk_radio_rcv_save_search_result(); //save to reg
        fm_reg_set_channel_count(para, fm_ctrl->channel_count);
		fm_reg_set_sel_channel_id(para,fm_ctrl->channel_id);
#ifdef __MSG
        {
            //for debug
            __s32 i, num = 0;

            for (i = 0; i < MAX_CHANNEL_NUM; i++)
            {
                __msg("FM_channel[%d]:%d", i, para->FM_channel[i]);

                if (0 != para->FM_channel[i])
                {
                    num++;
                }
            }

            __msg("valid_num = %d, sel channel_id:%d", num, para->channel_id);
        }
#endif

        if (fm_ctrl->channel_count > 0)
        {
            update_to_reg();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    fm_ctrl->search_mode = SRHMODE_IDLE;
}

//////////////////////////////////////////////////////////////////////////
#define FM_CB_NEED_NOTIFY_FRW(_p) (_p && _p->h_main_lyr && GUI_LyrWinGetSta(_p->h_main_lyr) == GUI_LYRWIN_STA_ON)
__s32 cb_srhch_success(void *arg_p)
{
    FM_WND_T *wnd_para;
    H_WIN hwnd = (H_WIN)arg_p;
    FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

    if (wnd_para)
    {
        on_fm_test_freq_ok(&wnd_para->fm_ctrl);
        __wrn("draw:fm_ctrl->cur_freq = %d", wnd_para->fm_ctrl.cur_freq);
    }

    if (FM_CB_NEED_NOTIFY_FRW(wnd_para))
    {
        NOTIFY_MSG(CMD_AUTOSRH_FINDCH, hwnd, wnd_para->h_main_wnd, 0, 0);
    }

    return 0;
}

__s32 cb_srhch_fail(void *arg_p)
{
    FM_WND_T *wnd_para;
    H_WIN hwnd = (H_WIN)arg_p;
    FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

    if (wnd_para)
    {
        on_fm_test_freq_fail(&wnd_para->fm_ctrl);
        __wrn("draw:fm_ctrl->cur_freq = %d", wnd_para->fm_ctrl.cur_freq);
    }

    if (FM_CB_NEED_NOTIFY_FRW(wnd_para))
    {
        NOTIFY_MSG(CMD_AUTOSRH_FINDCHFAIL, hwnd, wnd_para->h_main_wnd, 0, 0);
    }

    return 0;
}

__s32 cb_srhch_over(void *arg_p)
{
    FM_WND_T *wnd_para;
    H_WIN hwnd = (H_WIN)arg_p;
    FM_GetWndPara(wnd_para, FM_WND_T, hwnd);
    __wrn("cb_srhch_over=%d", cb_srhch_over);

    if (wnd_para)
    {
        on_fm_test_freq_end(&wnd_para->fm_ctrl);
    }

    if (FM_CB_NEED_NOTIFY_FRW(wnd_para))
    {
        NOTIFY_MSG(CMD_AUTOSRH_OVER, hwnd, wnd_para->h_main_wnd, 0, 0);
    }

    dsk_display_lcd_on();
    return 0;
}

/************************************************************************/
//通知关闭fm
/************************************************************************/
__s32 notify_to_close_fm(H_WIN hwnd, __s32 id, __s32 para)
{
    __gui_msg_t msg;
    msg.h_deswin   = GUI_WinGetParent(FM_GetManWnd());
    msg.h_srcwin   = hwnd;
    msg.id         = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwnd), id);
    msg.dwAddData2 = para;
    msg.dwReserved = 0;
    GUI_SendNotifyMessage(&msg);
    return EPDK_OK;
}

static __s32 fm_set_band(FM_CTRLDATA_T *fm_ctrl, dsk_radio_band_mode_t mode)
{
    dsk_radio_set_band(mode);
    dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);
    dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);
    __msg("freq range = [%d, %d]", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
    return EPDK_OK;
}


static __u32 fm_check_disk(void)
{
    __u32 disk = 0;
    __s32 i = 0, ret = 0;
    char diskname[RAT_MAX_PARTITION][4];
    __msg("**********check_disk**********");
    disk = RAT_UNKNOWN;
    eLIBs_memset(diskname, 0x00, sizeof(diskname));
    ret = rat_get_partition_name(RAT_USB_DISK, diskname, 0);

    if (EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("USB : diskname[%d]=%s", i, diskname[i]);
                disk |= RAT_USB;
            }

            i++;
        }
    }

    eLIBs_memset(diskname, 0x00, sizeof(diskname));
    ret = rat_get_partition_name(RAT_SD_CARD, diskname, 0);

    if (EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("SD : diskname[%d]=%s", i, diskname[i]);
                disk |= (RAT_TF << 8);
            }

            i++;
        }
    }

    return disk;
}
/************************************************************************/
// 进入录音
/************************************************************************/
static __s32 fm_call_record_app(H_WIN hwnd, __s32 root_type)
{
    H_WIN dlg;
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

    if (wnd_para)
    {
        if (fm_check_disk() == RAT_UNKNOWN)
        {
            __s32 lang_id[] = {STRING_HOME_CUE, STRING_HOME_NULL_DISK};

            if (wnd_para->h_sset)
            {
                SSET_Destroy(wnd_para->h_sset);
                wnd_para->h_sset = NULL;

                if (wnd_para->sset_create_para)
                {
                    FM_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
                    wnd_para->sset_create_para = NULL;
                }

                __msg("sset destroyed ");
            }

            gscene_hbar_set_state(HBAR_ST_HIDE);
            default_dialog(dlg, hwnd, 0x2, ADLG_OK, lang_id);
            esKRNL_TimeDly(100);
            app_dialog_destroy(dlg);
        }
        else
        {
            wnd_para->fm_ctrl.is_record = EPDK_TRUE;
            notify_to_close_fm(hwnd, SWITCH_TO_OTHER_APP, MAKELONG(FM_SW_TO_RECORD, RAT_TF));
        }
    }

    return EPDK_OK;
}

/************************************************************************/
// 选择某个频道
//因为增加了退出一项，所以sel_id要减1
/************************************************************************/
static __s32 fm_change_channelno(FM_CTRLDATA_T *fm_ctrl, __s32 sel_id)
{
    if (dsk_radio_rcv_get_search_result_vn() != fm_ctrl->channel_count)
    {
        __wrn(" somewhere error,   check");
    }

    __wrn("  channel_count = %d,   channel_id = %d, sel %d",
          fm_ctrl->channel_count, fm_ctrl->channel_id, sel_id);

    /*
        if(fm_ctrl->channel_count <= 1)
        {
            __wrn("  fm_ctrl->channel_count <= 1");
            return EPDK_OK;
        }
    */
    if (/*fm_ctrl->channel_id == sel_id - 1
        ||*/ sel_id > fm_ctrl->channel_count)
    {
        return EPDK_OK;
    }

    if (0 == sel_id)
    {
        app_fm_notify_delete_sub_scene(g_FMManWnd);
        return EPDK_OK;
    }
    else
    {
        dsk_radio_chan_t chan_info;
        reg_fm_para_t *para;
        get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
        //dsk_radio_rcv_get_search_result_chaninfo(sel_id, &chan_info);
        dsk_radio_rcv_get_search_result_chaninfo(sel_id - 1, &chan_info);
        __wrn(" sel freq = %d", chan_info.freq);
        fm_ctrl->cur_freq = chan_info.freq;

        if (dsk_radio_rcv_set_freq_play(chan_info.freq) == EPDK_OK)
        {
            fm_ctrl->channel_id = sel_id - 1;
            fm_reg_set_sel_channel_id(para, sel_id - 1);
        }

        dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);//just for mistake
        __wrn(" get sel freq = %d", fm_ctrl->cur_freq);
        //refresh ui:
        fm_op_notify(fm_change_channelno);
        __wrn("  fm_change_channelno  return");
    }

    return EPDK_OK;
}

//手动搜索
static __s32 fm_manual_search_start(FM_CTRLDATA_T *fm_ctrl, __s32 way)
{
    __u32 search_flag;
    dsk_radio_get_search_flag(&search_flag) ;

    if (1 == search_flag) //正在搜索过程中不允许中断，必须得搜索完后才能进行
    {
        __wrn("fm_manual_search_start:return");
        return EPDK_OK;
    }
    else
    {
        __wrn("fm_manual_search_start:");
        fm_ctrl->search_mode = SRHMODE_MANUAL;
        __wrn("FM::way = %d", way); //0:right;1:left
        dsk_radio_set_manual_way(way);
        dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_MANUAL);
        dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
        __wrn(" fm_manual:fm_ctrl->cur_freq = %d, fm_ctrl->channel_id + 1=%d", fm_ctrl->cur_freq, fm_ctrl->channel_id + 1);
        dsk_radio_rcv_search_all(fm_ctrl->cur_freq, fm_ctrl->channel_id + 1); //TODO:fm_ctrl->channel_id+1 ?
        return EPDK_OK;
    }
}

//自动搜索
static __s32 fm_auto_search_start(FM_CTRLDATA_T *fm_ctrl)
{
    __u32 search_flag;
    dsk_radio_get_search_flag(&search_flag) ;

    if (1 == search_flag) //正在搜索过程中不允许中断，必须得搜索完后才能进行
    {
        __wrn("fm_auto_search_start:return");
        return EPDK_OK;
    }
    else
    {
        __wrn("fm_auto_search_start:");
        fm_ctrl->search_mode = SRHMODE_AUTO;
        dsk_radio_set_automanual_mode(DSK_RADIO_SEARCH_AUTO);
        dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
        __wrn("fm_auto_search_start:");
        dsk_radio_rcv_search_all(0, 0);
        dsk_display_lcd_off();
        return EPDK_OK;
    }
}

//初始化相关数据
static __s32 fm_init_data(FM_CTRLDATA_T *fm_ctrl)
{
    fm_ctrl->search_mode = SRHMODE_IDLE;
    fm_ctrl->thumb_show = FM_SHOW_THUMB_N;
    __wrn("init module#################################");

    if (dsk_radio_is_open())
    {
        dsk_radio_chan_t chan_info;
        reg_fm_para_t *para;
        get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
        fm_ctrl->channel_count = dsk_radio_rcv_get_search_result_vn();
        __wrn("fm_ctrl->channel_count = %d", fm_ctrl->channel_count);

        if (fm_ctrl->channel_count <= 0) //no channel info
        {
            //__here__;
            fm_ctrl->channel_id = 0;
            fm_ctrl->cur_freq = MIN_CHANNEL_FREQ;
            fm_ctrl->channel_count = 0;
            //初始化reg_fm_para_t
            ZeroMemory(para, sizeof(reg_fm_para_t));
            fm_reg_set_sel_channel_id(para, 0);
            //          fm_reg_set_channel_(para, 0, fm_ctrl->cur_freq); //设置默认一个频道频率
            //          fm_reg_set_channel_count(para, fm_ctrl->channel_count);
            dsk_radio_rcv_get_search_result(para);
        }
        else
        {
            //__here__;
            if (ValueBetweenRange(MIN_CHANNEL_FREQ, fm_reg_get_curfreq(para), MAX_CHANNEL_FREQ)
                && fm_reg_get_channel_count(para) == 1 && fm_ctrl->channel_count == 1
                && fm_reg_get_channel_(para, 0) == MIN_CHANNEL_FREQ)
            {
                //曾经有手动设置频率(没搜索过)
                fm_ctrl->cur_freq = fm_reg_get_curfreq(para);
                __wrn("fm_init:fm_ctrl->cur_freq = %d", fm_ctrl->cur_freq);
                __wrn("fm_init:fm_ctrl->channel_count  = %d", fm_ctrl->channel_count);

                if ((fm_ctrl->cur_freq < MIN_CHANNEL_FREQ) && (fm_ctrl->cur_freq > MIN_CHANNEL_FREQ))
                {
                    return EPDK_FAIL;
                }
                else
                {
                    if (87500 == MIN_CHANNEL_FREQ)//只针对4702，FM play3次声音才最清晰
                    {
                        dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);
                        dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
                    }

                    dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
                }
            }

            //          else
            {
                if (ValueInRange(0, para->channel_id, fm_ctrl->channel_count))
                {
                    fm_ctrl->channel_id = fm_reg_get_sel_channel_id(para);
                    __wrn("fm_ctrl->channel_id  = %d", fm_ctrl->channel_id);
                }
                else
                {
                    fm_ctrl->channel_id = 0;
                    fm_reg_set_sel_channel_id(para, 0);
                }

                dsk_radio_rcv_get_search_result_chaninfo(fm_ctrl->channel_id, &chan_info);
                __wrn(" fm_ctrl->channel_id  = %d", fm_ctrl->channel_id);

                if (ValueOutOfRange(MIN_CHANNEL_FREQ, fm_ctrl->cur_freq, MAX_CHANNEL_FREQ))
                {
                    fm_ctrl->cur_freq = chan_info.freq;
                }

                __wrn(" fm_ctrl->cur_freq = %d", fm_ctrl->cur_freq);

                if ((fm_ctrl->cur_freq < MIN_CHANNEL_FREQ) && (fm_ctrl->cur_freq > MIN_CHANNEL_FREQ))
                {
                    return EPDK_FAIL;
                }//FM play3次声音才最清晰
                else
                {
                    if (87500 == MIN_CHANNEL_FREQ)//只针对4702，FM play3次声音才最清晰
                    {
                        dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq + 100);
                        dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq - 100);
                    }

                    dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
                }
            }
        }

        dsk_radio_get_min_freq(&fm_ctrl->cur_min_freq);
        dsk_radio_get_max_freq(&fm_ctrl->cur_max_freq);
        __msg("freq range = [%d, %d]", fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
    }

    return EPDK_OK;
}

//初始化模块
static __s32 fm_init_module(void *cb_arg, FM_CTRLDATA_T *fm_ctrl)
{
    __s32 ret = EPDK_FAIL;

    if (!dsk_radio_is_open())
    {
        reg_fm_para_t *para;
        get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
        //__here__;
        ret = dsk_radio_open();

        if (ret == EPDK_FAIL)
        {
            return EPDK_FAIL;
        }

        //__here__;
        dsk_radio_rcv_open();
        //__here__;
        
        dsk_radio_set_vol(30);
        dsk_radio_set_band(DSK_RADIO_BAND_US_EUP);
        //fm_set_band(fm_ctrl, DSK_RADIO_BAND_US_EUP);
        dsk_radio_set_threshold(DSK_RADIO_THRESHOLD_MID);
        dsk_radio_set_audio_mode(DSK_RADIO_AUDIO_STEREO);
#ifdef __MSG
        {
            //for debug
            __s32 i, num = 0;

            for (i = 0; i < MAX_CHANNEL_NUM; i++)
            {
                __msg("FM_channel[%d]:%d", i, para->FM_channel[i]);

                if (0 != para->FM_channel[i])
                {
                    num++;
                }
            }

            __msg("valid_num = %d, sel channel_id:%d", num, para->channel_id);
        }
#endif
        dsk_radio_rcv_get_search_result(para);
        dsk_radio_rcv_set_play_list_type(PLAY_LIST_SEARCH_RESULT);
    }

    dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_SUCCESS, esKRNL_GetCallBack(cb_srhch_success), cb_arg);
    dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_FAIL, esKRNL_GetCallBack(cb_srhch_fail), cb_arg);
    dsk_radio_set_cb(DSK_RADIO_EVENT_SEARCH_OVER, esKRNL_GetCallBack(cb_srhch_over), cb_arg);
    //dsk_radio_rcv_set_search_cb(esKRNL_GetCallBack(fm_auto_search_cb),(void *)fmplay_menu_attr);
    fm_init_data(fm_ctrl);
    return EPDK_OK;
}

/************************************************************************/
// fm 操作通知
/************************************************************************/
#define IsOPFunc(_op, _opFun)  ((_op) == (signed long)_opFun)
static __s32 _fm_op_notify(signed long op_function)
{
    __wrn("fm_change_channelno");

    if (IsOPFunc(op_function, fm_change_channelno))
    {
        FM_WND_T *wnd_para;
        FM_GetWndPara(wnd_para, FM_WND_T, FM_GetManWnd());

        if (wnd_para)
        {
            if (wnd_para->h_main_lyr && wnd_para->h_main_wnd)
            {
                FM_UpdateUI(wnd_para->h_main_wnd);
            }
        }
    }

    __wrn("fm_change_channelno end");
    //  else if (IsOPFunc(op_function, fm_change_channelno))
    //  {
    //  }
    return EPDK_OK;
}


//////////////////////////////////////////////////////////////////////////
//按键消息处理,主要因为在sset起来以后FM 的frm没收到按键消息
//返回true 为不需要向下发、否则需要
//////////////////////////////////////////////////////////////////////////
static __bool fm_key_proc(__gui_msg_t *msg)
{
    FM_WND_T *wnd_para;
    static __s32 manw_lastkey = -1;
    __bool ret = EPDK_FALSE;
    FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

    //__wrn("---FM manwin-data 2 =%d, data1 = %d, lastkey = %d",msg->dwAddData2,msg->dwAddData1,manw_lastkey );

    if (wnd_para->h_sset)
    {
        if (msg->dwAddData2 == KEY_UP_ACTION)
        {
            switch (manw_lastkey)
            {
                case GUI_MSG_KEY_LONGESCAPE://..
                case GUI_MSG_KEY_ESCAPE://..
                case GUI_MSG_KEY_MENU://..
                case GUI_MSG_KEY_LONGMENU://..
                {
                    NOTIFY_MSG(FM_COMMAND, msg->h_deswin, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0);
                    ret = EPDK_TRUE;
                    break;
                }

                default:
                {
                    ret = EPDK_FALSE;
                    break;
                }
            }

            manw_lastkey = -1;
        }
        else
        {
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_LONGESCAPE:
                case GUI_MSG_KEY_ESCAPE:
                case GUI_MSG_KEY_MENU://..
                case GUI_MSG_KEY_LONGMENU://..
                {
                    manw_lastkey = msg->dwAddData1;
                    ret = EPDK_TRUE;
                    break;
                }

                default:
                {
                    ret = EPDK_FALSE;
                    break;
                }
            }
        }
    }
    else
    {
        ret = EPDK_FALSE;
    }

    return ret;
}

/************************************************************************/
// 处理sset命令
/************************************************************************/
static __s32 on_fm_sset_cmd(H_WIN hwnd, __s32 id, __s32 value, __gui_msg_t *msg)
{
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

    if (FM_SSET_NC_ID_UNKNOW == value)
    {
        __wrn("FM_SSET_NC_ID_UNKNOW");
        return EPDK_OK;
    }

    switch (id)
    {
        case FM_SSET_NC_EXIT:
        {
            NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0);
            notify_to_close_fm(hwnd, SWITCH_TO_MMENU, 0);
        }
        break;

        case FM_SSET_NC_CHANNEL:
        {
            //sset自己处理
        }
        break;

        case FM_SSET_NC_MSEARCH:
        {
            //..fm_manual_search_start(&wnd_para->fm_ctrl, 0);
            __wrn("------manal search");
            fmplay_finetune(msg, 0, 1);
        }
        break;

        case FM_SSET_NC_ASEARCH:
        {
            NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0);
            NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_SHOW_ASEARCH_DLG, 0);
            //fm_auto_search_start(&wnd_para->fm_ctrl);
        }
        break;

        case FM_SSET_NC_AUDIO:
        {
            dsk_radio_set_audio_mode(value);
        }
        break;

        case FM_SSET_NC_RECORD:
        {
            fm_call_record_app(hwnd, value);
        }
        break;

        case FM_SSET_NC_SEL_CHANNEL:
        {
            __wrn("  fm_change_channelno:id = %d,    value = %d", id, value);
            fm_change_channelno(&wnd_para->fm_ctrl, value);
        }
        break;

        case FM_SSET_NC_SEL_AUDIO:
        {
            __gui_msg_t   mymsg;
            ASSERT(value == DSK_RADIO_AUDIO_STEREO || value == DSK_RADIO_AUDIO_MONO
                   || value == DSK_RADIO_AUDIO_EXIT);

            if (value < 3) //重刷图形，切换显示"mono"，"stereo"
            {
                __wrn("value = %d", value);
                dsk_radio_set_audio_mode(value);
                mymsg.h_deswin = wnd_para->h_main_wnd ;
                mymsg.id = GUI_MSG_PAINT;
                GUI_SendNotifyMessage(&mymsg);
            }
            else //退出
            {
                NOTIFY_MSG(FM_COMMAND, NULL, hwnd, ID_FM_CMD_DESTROY_SSET, 0);
            }
        }
        break;

        case  FM_SSET_NC_SAVEFREQ:
        {
            reg_fm_para_t *para;
            __gui_msg_t   mymsg;
            get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
            dsk_radio_rcv_save_cur_freq();
            wnd_para->fm_ctrl.channel_count = dsk_radio_rcv_get_search_result_vn();
            __wrn("wnd_para->fm_ctrl.channel_count=%d", wnd_para->fm_ctrl.channel_count);
            __wrn("wnd_para->fm_ctrl.cur_freq = %d, para->channel_id=%d", wnd_para->fm_ctrl.cur_freq, para->channel_id);
            wnd_para->fm_ctrl.channel_id = para->channel_id;
            __wrn("wnd_para->fm_ctrl.channel_id=%d", wnd_para->fm_ctrl.channel_id);
            //重刷FM主界面图层，更新保存手动搜索的台数
            mymsg.h_deswin = wnd_para->h_main_wnd ;
            mymsg.id = GUI_MSG_PAINT;
            GUI_SendNotifyMessage(&mymsg);
            app_fm_notify_delete_sub_scene(msg->h_deswin);
            {
                H_WIN dlg;
                __s32 lang_id[] = {STRING_FM_CUE, STRING_FM_SAVE_FREQ_OK};
                default_dialog(dlg, msg->h_deswin, ID_FM_SAVE_FREQ_DLG, ADLG_OK, lang_id);
                esKRNL_TimeDly(100);
                app_dialog_destroy(dlg);
            }
        }
        break;

        case SSET_NC_TIMER:
        {
            NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0);
            gscene_hbar_set_state(HBAR_ST_HIDE);
            break;
        }
    }

    return EPDK_OK;
}

/************************************************************************/
// 处理dialog命令
/************************************************************************/
static __s32 on_fm_auto_search_dlg_cmd(H_WIN hwnd, __s32 id, __s32 para)
{
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, hwnd);

    switch (id)
    {
        case ADLG_CMD_EXIT:
        {
            app_dialog_destroy(wnd_para->h_dlg);
            wnd_para->h_dlg = NULL;

            if (para == ADLG_IDYES)
            {
                H_WIN dlg;
                __s32 lang_id[2] = {STRING_FM_CUE, STRING_FM_SEARCH_TIPS};
                default_dialog(dlg, hwnd, APP_FM_ID + 0xf, ADLG_OK, lang_id);
                esKRNL_TimeDly(150);
                app_dialog_destroy(dlg);
                fm_auto_search_start(&wnd_para->fm_ctrl);
            }
            else
            {
            }
        }
        break;
    }

    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////
//命令消息处理 FM_COMMAND
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_command(__gui_msg_t *msg)
{
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

    switch (msg->dwAddData1)
    {
        case ID_FM_CMD_CREATE_SSET:
        {
            gscene_hbar_set_state(HBAR_ST_SHOW);

            if (wnd_para->h_sset)
            {
                __msg("err, sset created ");
            }
            else
            {
                wnd_para->sset_create_para = FM_SSET_Build_CreatePara(FM_GetManWnd());

                if (wnd_para->sset_create_para)
                {
                    //创建sset
                    wnd_para->h_sset = SSET_Create(wnd_para->sset_create_para);
                    SSET_Set_Nodelist("fm", NODELIST_ON);
                    __msg("sset created ");
                }
            }
        }
        break;

        case ID_FM_CMD_DESTROY_SSET:
        {
            if (wnd_para->h_sset)
            {
                SSET_Destroy(wnd_para->h_sset);
                wnd_para->h_sset = NULL;

                if (wnd_para->sset_create_para)
                {
                    FM_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
                    wnd_para->sset_create_para = NULL;
                }

                __msg("sset destroyed ");
            }
            else
            {
                __msg("err, sset destroyed ");
            }

            gscene_hbar_set_state(HBAR_ST_HIDE);
        }
        break;

        case ID_FM_CMD_SHOW_ASEARCH_DLG:
        {
            __s32 str[] = {STRING_FM_CHSRH_TITLE, STRING_FM_CHSRH_TIPS};
            default_dialog(wnd_para->h_dlg, FM_GetManWnd(), ID_FM_IS_AUTO_SEARCH_DLG, ADLG_YESNO, str);
        }
        break;
    }

    return EPDK_OK;
}

static __s32 fmplay_finetune(__gui_msg_t *msg, __u8 ucChMod, __u8  playmode)
{
    __u32 uFreq = 0;
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
    __wrn("..........uFreq 0= %d", uFreq);

    if (1 == ucChMod) //    1:减
    {
        uFreq = wnd_para->fm_ctrl.cur_freq - LARK_SEARCH_STEP_US;

        if (uFreq < FM_SEARCH_CHN_MIN_FREQ)
        {
            if (playmode == 1) // 循环
            {
                uFreq = FM_SEARCH_CHN_MAX_FREQ;
                __wrn("..........uFreq 1= %d", uFreq);
            }
            else            //不循环
            {
                uFreq = FM_SEARCH_CHN_MIN_FREQ;
                __wrn("..........uFreq 2= %d", uFreq);
            }
        }
    }
    else            //0: 加
    {
        uFreq = wnd_para->fm_ctrl.cur_freq + LARK_SEARCH_STEP_US;

        if (uFreq > FM_SEARCH_CHN_MAX_FREQ)
        {
            if (playmode == 1) // 循环
            {
                uFreq = FM_SEARCH_CHN_MIN_FREQ;
                __wrn("..........uFreq 3= %d", uFreq);
            }
            else            //不循环
            {
                uFreq = FM_SEARCH_CHN_MAX_FREQ;
                __wrn("..........uFreq 4= %d", uFreq);
            }
        }
    }

    __wrn("..........uFreq 5= %d", uFreq);
    wnd_para->fm_ctrl.cur_freq = uFreq;
    dsk_radio_rcv_set_freq_play(uFreq);
    //dek_radio_set_FM1_3(fmplay_menu_attr->curFM1_3AM1_2_id,fmplay_menu_attr->curFM1_3AM1_2_index, uFreq);     //add by Kingvan
    //dsk_radio_rcv_save_search_result();
    //refresh ui:
    __wrn("fm_change_channelno");
    fm_op_notify(fm_change_channelno);
    return EPDK_OK;
}


//////////////////////////////////////////////////////////////////////////
//命令消息处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_manwnd_command(__gui_msg_t *msg)
{
    H_WIN hwnd = msg->h_deswin;
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
    APP_DEBUG_CBMSGEx;

    switch (LOSWORD(msg->dwAddData1))
    {
            __wrn("msg->dwAddData1 == %d", msg->dwAddData1);

        case ID_FM_SCENE_SSET:
            return on_fm_sset_cmd(msg->h_deswin, HISWORD(msg->dwAddData1), msg->dwAddData2, msg);

        case ID_FM_IS_AUTO_SEARCH_DLG:
            return on_fm_auto_search_dlg_cmd(msg->h_deswin, HISWORD(msg->dwAddData1), msg->dwAddData2);

        case FM_MAINWND_ID:
        {
            switch (HISWORD(msg->dwAddData1))
            {
                case ID_FM_MAINWND_NOTIFY_TOUCHUP:
                {
                    if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                    {
                    }
                    else
                    {
                        if (wnd_para->h_sset)
                        {
                            NOTIFY_MSG(FM_COMMAND, msg->h_deswin, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0);
                        }
                        else
                        {
                            NOTIFY_MSG(FM_COMMAND, msg->h_deswin, FM_GetManWnd(), ID_FM_CMD_CREATE_SSET, 0);
                        }
                    }
                }
                break;

                case  ID_FM_MAINWND_CLICKUP:
                {
                    switch (msg->dwAddData2)
                    {
                        case GUI_MSG_KEY_LONGPLAY_PAUSE:
                        {
                            reg_fm_para_t *para;
                            __gui_msg_t   mymsg;
                            get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
                            dsk_radio_rcv_save_cur_freq();
                            wnd_para->fm_ctrl.channel_count = dsk_radio_rcv_get_search_result_vn();
                            __wrn("wnd_para->fm_ctrl.channel_count=%d", wnd_para->fm_ctrl.channel_count);
                            __wrn("wnd_para->fm_ctrl.cur_freq = %d, para->channel_id=%d", wnd_para->fm_ctrl.cur_freq, para->channel_id);
                            wnd_para->fm_ctrl.channel_id = para->channel_id;
                            __wrn("wnd_para->fm_ctrl.channel_id=%d", wnd_para->fm_ctrl.channel_id);
                            //重刷FM主界面图层，更新保存手动搜索的台数
                            mymsg.h_deswin = wnd_para->h_main_wnd ;
                            mymsg.id = GUI_MSG_PAINT;
                            GUI_SendNotifyMessage(&mymsg);
                            app_fm_notify_delete_sub_scene(msg->h_deswin);
                            {
                                H_WIN dlg;
                                __s32 lang_id[] = {STRING_FM_CUE, STRING_FM_SAVE_FREQ_OK};
                                default_dialog(dlg, msg->h_deswin, ID_FM_SAVE_FREQ_DLG, ADLG_OK, lang_id);
                                esKRNL_TimeDly(100);
                                app_dialog_destroy(dlg);
                            }
                            break;
                        }

                        case GUI_MSG_KEY_MENU:
                        case GUI_MSG_KEY_ESCAPE://..
                            //  case GUI_MSG_KEY_MODE: // shiql set o for D100 Compile
                        {
                            if (wnd_para->h_sset)
                            {
                                NOTIFY_MSG(FM_COMMAND, hwnd, FM_GetManWnd(), ID_FM_CMD_DESTROY_SSET, 0);
                            }
                            else
                            {
                                gscene_hbar_set_state(HBAR_ST_HIDE);
                            }

                            notify_to_close_fm(hwnd, SWITCH_TO_MMENU, 0);
                            break;
                        }

                        case GUI_MSG_KEY_NEXT:
                        case GUI_MSG_KEY_LONGNEXT:
                        case GUI_MSG_KEY_UP:
                        {
                            if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                            {
                            }
                            else
                            {
                                __s32  channel_id = wnd_para->fm_ctrl.channel_id + 1;
                                __wrn("channel_id = %d", channel_id);

                                if (channel_id <= 1)
                                {
                                    channel_id = wnd_para->fm_ctrl.channel_count;
                                }
                                else
                                {
                                    channel_id -= 1;
                                }

                                __wrn("fm_change_channelno:%d", channel_id);
                                fm_change_channelno(&wnd_para->fm_ctrl, channel_id);
                            }
                        }
                        break;

                        case GUI_MSG_KEY_PREV:
                        case GUI_MSG_KEY_LONGPREV:
                        case GUI_MSG_KEY_DOWN:
                        case GUI_MSG_KEY_ENTER:
                        case GUI_MSG_KEY_PLAY_PAUSE:
                        {
                            if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                            {
                            }
                            else
                            {
                                __s32  channel_id = wnd_para->fm_ctrl.channel_id + 1;
                                __wrn("channel_id = %d", channel_id);

                                if (channel_id >= wnd_para->fm_ctrl.channel_count)
                                {
                                    channel_id = 0;
                                }

                                __wrn("fm_change_channelno:%d", channel_id + 1);
                                fm_change_channelno(&wnd_para->fm_ctrl, channel_id + 1);
                            }

                            break;
                        }

                        case GUI_MSG_KEY_RIGHT:
                        case GUI_MSG_KEY_LONGRIGHT:
                            //case GUI_MSG_KEY_NEXT:
                            //case GUI_MSG_KEY_LONGNEXT:
                        {
                            __wrn("------fm right");
                            fmplay_finetune(msg, 0, 1);
#if 0

                            if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
                                || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                            {
                            }
                            else
                            {
                                __s32  channel_id = wnd_para->fm_ctrl.channel_id + 1;

                                if (channel_id >= wnd_para->fm_ctrl.channel_count)
                                {
                                    channel_id = 0;
                                }

                                fm_change_channelno(&wnd_para->fm_ctrl, channel_id);
                            }

#endif
                            break;
                        }

                        /*case GUI_MSG_KEY_LONGENTER:
                        {
                            __wrn(",,,,,,,,longright1");
                            if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
                                || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                            {
                            }
                            else
                            {
                                __wrn(",,,,,,,,longright1");
                                fm_manual_search_start(&wnd_para->fm_ctrl, 0);
                                //dsk_radio_rcv_set_search_cb(esKRNL_GetCallBack(fm_auto_search_cb),(void *)fmplay_menu_attr);
                            }
                            break;

                        }*/
                        /*case GUI_MSG_KEY_LONGLEFT:
                        {
                            if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
                                || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                            {
                            }
                            else
                            {
                                fm_manual_search_start(&wnd_para->fm_ctrl, 1);
                                //dsk_radio_rcv_set_search_cb(esKRNL_GetCallBack(fm_auto_search_cb),(void *)fmplay_menu_attr);
                            }
                            break;

                        }*/
                        case GUI_MSG_KEY_LEFT:
                        case GUI_MSG_KEY_LONGLEFT:
                            //case GUI_MSG_KEY_PREV:
                            //case GUI_MSG_KEY_LONGPREV:
                        {
                            fmplay_finetune(msg, 1, 1);
#if 0

                            if (wnd_para->fm_ctrl.search_mode == SRHMODE_AUTO
                                || wnd_para->fm_ctrl.search_mode == SRHMODE_MANUAL)
                            {
                            }
                            else
                            {
                                __s32  channel_id = wnd_para->fm_ctrl.channel_id - 1;

                                if (channel_id < 0)
                                {
                                    channel_id = wnd_para->fm_ctrl.channel_count - 1;
                                }

                                fm_change_channelno(&wnd_para->fm_ctrl, channel_id);
                            }

#endif
                            break;
                        }

                        default:
                            break;
                    }
                }
                break;
            }
        }

        return EPDK_OK;
    }

    return EPDK_FAIL;
}

//////////////////////////////////////////////////////////////////////////
//主窗口销毁处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_destory(__gui_msg_t *msg)
{
    FM_WND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
    //允许自动关屏
    //g_enable_close_scn();
    {
        //恢复系统自动关屏时间
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            g_set_close_scn_time(para->backlightoff);
            __msg("para->backlightoff=%d", para->backlightoff);
        }
    }
    g_enable_standby();
    //  if (dsk_radio_rcv_get_search_result_vn() <= 1)
    {
        reg_fm_para_t *para;
        get_reg_pointer(para, reg_fm_para_t, REG_APP_FM);
        //      ASSERT(wnd_para->fm_ctrl.channel_count == 1);
        //      ASSERT(ValueBetweenRange(wnd_para->fm_ctrl.cur_min_freq, wnd_para->fm_ctrl.cur_freq, wnd_para->fm_ctrl.cur_max_freq));
        //      fm_reg_set_channel_(para, 0, wnd_para->fm_ctrl.cur_freq);
        fm_reg_set_curfreq(para, wnd_para->fm_ctrl.cur_freq);
        __msg("cur_freq = %d, channel_count = %d", fm_reg_get_curfreq(para), fm_reg_get_channel_count(para));
    }
    //刷新注册表数据
    //update_to_reg();

    if (! wnd_para->fm_ctrl.is_record)
    {
        dsk_radio_close();
    }

    //TODO:release resource

    if (wnd_para->h_sset)
    {
        SSET_Destroy(wnd_para->h_sset);
        wnd_para->h_sset = NULL;
    }

    if (wnd_para->sset_create_para)
    {
        FM_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
        wnd_para->sset_create_para = NULL;
    }

    if (wnd_para->h_dlg)
    {
        app_dialog_destroy(wnd_para->h_dlg);
        wnd_para->h_dlg = NULL;
    }

    if (wnd_para->h_frm_alarm_coming)
    {
        app_alarm_frm_destory(wnd_para->h_frm_alarm_coming);
        wnd_para->h_frm_alarm_coming = NULL ;
    }

    app_com_layer_destroy(wnd_para->h_main_lyr);
    BFreeType(wnd_para, FM_WND_T);
    //EndOfDebugAPPMEM();
    return 0;
}

static __s32 get_auto_close_scn_time_fm(void)
{
    __s32 bok;
    __s32 close_scn_time;
    {
        //init key val by app_config.fex
        char *path = "d:\\app_config.bin";
        H_PARSER parser;
        __s32 ret;
        __s32 i;
        char keyname[64];
        int val;
        char str_val[32];
        bok = 1;

        do
        {
            parser = NULL;
            parser = esCFG_Init_Ex(path);

            if (NULL == parser)
            {
                __wrn("esCFG_Init_Ex fail...");
                bok = 0;
                break;
            }

            __wrn("before esCFG_GetKeyValue_Ex");
            eLIBs_strcpy(keyname, "fm");
            eLIBs_strcat(keyname, "/");
            eLIBs_strcat(keyname, "auto_close_scn");
            ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);
            __wrn("after esCFG_GetKeyValue_Ex, ret=%d", ret);

            if (EPDK_OK == ret)
            {
                bok = 1;
                close_scn_time = val;
            }
            else
            {
                bok = 0;
            }
        } while (0);

        if (parser)
        {
            esCFG_Exit_Ex(parser);
            parser = NULL;
        }
    }

    if (0 == bok) //default is not auto close scn
    {
        close_scn_time = 0;
    }

    __wrn("close_scn_time=%d", close_scn_time);
    return close_scn_time;
}

//////////////////////////////////////////////////////////////////////////
//主窗口创建处理
//////////////////////////////////////////////////////////////////////////
static __s32 on_fm_create(__gui_msg_t *msg)
{
    FM_UI_PARA_T *ui = NULL;
    FM_WND_T *wnd_para = NULL;
    __s32 auto_close_scn_time = 0;
    __s32 ret = EPDK_FAIL;
    //__here__;
    FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);
    //不显示headbar
    //gscene_hbar_set_state(HBAR_ST_HIDE);
    //禁止自动关屏
    //  g_disable_close_scn();
    //__here__;
    g_disable_standby();
    //设置调色板
    //com_set_palette_by_id(ID_FM_COLOURTABLE_BMP);
    //初始化radio
    ret = fm_init_module((void *)msg->h_deswin, &wnd_para->fm_ctrl);

    if (ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    //__here__;
    //TODO:init wnd_para
    //gscene_bgd_set_state(BGD_STATUS_HIDE);
    ui = get_fm_uipara();
    //__here__;
    fm_layer_create(wnd_para->h_main_lyr, &ui->rt, "fm_main");
    wnd_para->h_main_wnd = fm_mainfrw_create(msg->h_deswin, wnd_para->h_main_lyr, (void *)&wnd_para->fm_ctrl);
    __msg("mainwnd = %x, lyr = %x", wnd_para->h_main_wnd, wnd_para->h_main_lyr);

    if (NULL == wnd_para->h_main_wnd)
    {
        return EPDK_FAIL;
    }

    _app_show_focus_wnd(wnd_para->h_main_wnd, wnd_para->h_main_lyr);
    GUI_LyrWinSetTop(wnd_para->h_main_lyr);
    //auto_close_scn_time = get_auto_close_scn_time_fm();
    __wrn("auto_close_scn_time=%d", auto_close_scn_time);
    //if (auto_close_scn_time > 0) //设置FM的自动关屏时间
    {
        //    g_set_close_scn_time(auto_close_scn_time);
    }
    //关背景
    //gscene_bgd_set_state(BGD_STATUS_HIDE);
    return 0;
}

//主消息处理函数
static __s32 _app_fm_Proc(__gui_msg_t *msg)
{
    APP_DEBUG_CBMSGEx;

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            esPWRMAN_LockCpuFreq();//禁止CPU自动调节频率，防止FM杂音
            dsk_set_auto_off_time(0);
            //__here__;
            dsk_amplifier_onoff(1);//打开功放
            //__here__;
            g_FMManWnd = msg->h_deswin;
            return on_fm_create(msg);
        }

        case GUI_MSG_DESTROY:
        {
            esPWRMAN_UnlockCpuFreq();//允许CPU自动调节频率
            {
                reg_system_para_t *para;
                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    dsk_set_auto_off_time(para->poweroff);
                    __wrn("para->poweroff=%d", para->poweroff);
                }
            }
            dsk_amplifier_onoff(0);//关闭功放
            return on_fm_destory(msg);
        }

        case DSK_MSG_HOME:
        {
            notify_to_close_fm(msg->h_deswin, SWITCH_TO_MMENU, 0);
        }

        return EPDK_OK;

        case DSK_MSG_KILL:
        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
        }
        break;

        case GUI_MSG_COMMAND:
        {
            if (on_fm_manwnd_command(msg) == EPDK_OK)
            {
                return EPDK_OK;
            }
        }
        break;

        case FM_COMMAND:
            return on_fm_command(msg);

        case GUI_MSG_KEY:
        {
            if (fm_key_proc(msg) == EPDK_FALSE)
            {
                break;
            }

            return EPDK_OK;
        }
        break;

        case DSK_MSG_ALARM:
        {
            FM_WND_T *wnd_para = NULL;
            FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

            if (wnd_para->h_dlg)
            {
                app_dialog_destroy(wnd_para->h_dlg);
                wnd_para->h_dlg = NULL ;
            }

            if (wnd_para->h_sset)
            {
                SSET_Destroy(wnd_para->h_sset);
                wnd_para->h_sset = NULL ;
            }

            wnd_para->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
            return EPDK_OK ;
        }

        case ALARM_MSG_CLOSE:
        {
            FM_WND_T *wnd_para = NULL;
            FM_GetWndPara(wnd_para, FM_WND_T, msg->h_deswin);

            if (wnd_para->h_frm_alarm_coming)
            {
                wnd_para->h_frm_alarm_coming = NULL ;
            }

            __wrn("@@ALARM_MSG_CLOSE begin");
            GUI_WinSetFocusChild(msg->h_deswin);
            dsk_amplifier_onoff(1);//闹钟关闭后会关功放，这里把它打开
            __wrn("@@ALARM_MSG_CLOSE end");
            return EPDK_OK ;
        }

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}


H_WIN app_fm_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    FM_WND_T *wnd_para;
    //StartDebugAPPMEM();
    __inf("**************************************");
    __inf("********  enter fm app  **************");
    __inf("**************************************");
    app_set_title(STRING_HOME_FM);
    gscene_bgd_set_state(BGD_STATUS_SHOW);
    BallocType(wnd_para, FM_WND_T);
    ZeroTypeMem(wnd_para, FM_WND_T);
    GUI_Memdev_FrameBuffer_Release(1);
    wnd_para->font      = para->font;
    wnd_para->root_type = (H_WIN)para->root_type;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = APP_FM;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_fm_Proc);
    create_info.attr            = (void *)wnd_para;
    create_info.id              = APP_FM_ID;
    create_info.hHosting        = NULL;
    return (GUI_ManWinCreate(&create_info));
}

__s32 app_fm_notify_delete_sub_scene(H_WIN hmanwin)
{
    if (NULL == hmanwin)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    SEND_MSG(FM_COMMAND, NULL, hmanwin, ID_FM_CMD_DESTROY_SSET, 0);
    return EPDK_OK;
}

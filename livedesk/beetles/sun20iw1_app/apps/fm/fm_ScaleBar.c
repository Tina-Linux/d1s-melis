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
#include "fm_ScaleBar.h"
#include "fm_main.h"
//////////////////////////////////////////////////////////////////////////
#if 0 //(为了打印简单, 可把下面重定义的GUI宏临时拷到有需要的C文件中使用)
//re define GUI_BMP_Draw
#ifdef GUI_BMP_Draw
#define MY_GUI_BMP_DRAW                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw            )
#undef GUI_BMP_Draw
#define GUI_BMP_Draw(_p, _x, _y)             do \
    {\
        {__msg("    GUI_BMP_Draw(%d, %d)", _x, _y);} \
        MY_GUI_BMP_DRAW(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringAt
#ifdef GUI_DispStringAt
#define MY_GUI_DispStringAt                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt        )
#undef GUI_DispStringAt
#define GUI_DispStringAt(_p, _x, _y)         do\
    { \
        {__msg("    GUI_DispStringAt(%d, %d)->( %s )", _x, _y, _p);} \
        MY_GUI_DispStringAt(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringInRect
#ifdef GUI_DispStringInRect
#define MY_GUI_DispStringInRect              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect    )
#undef GUI_DispStringInRect
#define GUI_DispStringInRect(_p, _prt, _fmt)  do \
    { \
        {__msg("    GUI_DispStringInRect(%d, %d, %d, %d)->( %s )", (_prt)->x0, (_prt)->y0, (_prt)->x1, (_prt)->y1, _p);} \
        MY_GUI_DispStringInRect(_p, _prt, _fmt); \
    } while (0)
#endif

#endif
//////////////////////////////////////////////////////////////////////////
//源范围中的值_src_val映射到目标范围中
#define RANGE_MAPPING(_dst_min, _dst_max, _src_min, _src_max, _src_val)  \
    (((_src_val) - (_src_min)) * ((_dst_max) - (_dst_min)) / ((_src_max) - (_src_min)) + (_dst_min))

/************************************************************************/
//加载资源
/************************************************************************/
__s32 fm_freq_scale_create_res(FM_FREQ_SCALE_RES_T *res)
{
    create_bmp_res(ID_FM_SCALE_THUMB_N_BMP, res->bmp_btn_thumb[BTN_NORMAL]);
    create_bmp_res(ID_FM_SCALE_THUMB_F_BMP, res->bmp_btn_thumb[BTN_FOCUS]);
#if (FM_SEARCH_CHN_MIN_FREQ == 87500)
    create_bmp_res(ID_FM_87_5FREQSCALE_BMP, res->bmp_scale);
    create_bmp_res(ID_FM_87_5MHZ_BMP, res->bmp_87_5MHZ);
#elif (FM_SEARCH_CHN_MIN_FREQ == 87000)
    create_bmp_res(ID_FM_87FREQSCALE_BMP, res->bmp_scale);
    create_bmp_res(ID_FM_87MHZ_BMP, res->bmp_87MHZ);
#endif
    create_bmp_res(ID_FM_CURSOR_SEL_BMP, res->bmp_cursor_sel);
    create_bmp_res(ID_FM_POINT_BMP, res->bmp_point);
    create_bmp_res(ID_FM_108MHZ_BMP, res->bmp_108MHZ);
    create_bmp_res(ID_FM_CH_BMP, res->bmp_ch);
    create_bmp_res(ID_FM_MHZ_BMP, res->bmp_MHZ);
    create_bmp_res(ID_FM_PRESET_BMP, res->bmp_preset);
    create_bmp_res(ID_FM_SLASH_BMP, res->bmp_slash);
    create_bmp_res(ID_FM_STEREO_BMP, res->bmp_stereo);
    create_bmp_res(ID_FM_MONO_BMP, res->bmp_mono);
    create_bmp_res(ID_FM_UPDATE_BG_BMP, res->bmp_update_bg);
    create_bmp_res(ID_FM_UPDATE0_BG_BMP, res->bmp_update0_bg);
    create_bmp_res(ID_FM_L0_BMP, res->bmp_l[0]);
    create_bmp_res(ID_FM_L1_BMP, res->bmp_l[1]);
    create_bmp_res(ID_FM_L2_BMP, res->bmp_l[2]);
    create_bmp_res(ID_FM_L3_BMP, res->bmp_l[3]);
    create_bmp_res(ID_FM_L4_BMP, res->bmp_l[4]);
    create_bmp_res(ID_FM_L5_BMP, res->bmp_l[5]);
    create_bmp_res(ID_FM_L6_BMP, res->bmp_l[6]);
    create_bmp_res(ID_FM_L7_BMP, res->bmp_l[7]);
    create_bmp_res(ID_FM_L8_BMP, res->bmp_l[8]);
    create_bmp_res(ID_FM_L9_BMP, res->bmp_l[9]);
    create_bmp_res(ID_FM_M0_BMP, res->bmp_m[0]);
    create_bmp_res(ID_FM_M1_BMP, res->bmp_m[1]);
    create_bmp_res(ID_FM_M2_BMP, res->bmp_m[2]);
    create_bmp_res(ID_FM_M3_BMP, res->bmp_m[3]);
    create_bmp_res(ID_FM_M4_BMP, res->bmp_m[4]);
    create_bmp_res(ID_FM_M5_BMP, res->bmp_m[5]);
    create_bmp_res(ID_FM_M6_BMP, res->bmp_m[6]);
    create_bmp_res(ID_FM_M7_BMP, res->bmp_m[7]);
    create_bmp_res(ID_FM_M8_BMP, res->bmp_m[8]);
    create_bmp_res(ID_FM_M9_BMP, res->bmp_m[9]);
    create_bmp_res(ID_FM_B0_BMP, res->bmp_b[0]);
    create_bmp_res(ID_FM_B1_BMP, res->bmp_b[1]);
    create_bmp_res(ID_FM_B2_BMP, res->bmp_b[2]);
    create_bmp_res(ID_FM_B3_BMP, res->bmp_b[3]);
    create_bmp_res(ID_FM_B4_BMP, res->bmp_b[4]);
    create_bmp_res(ID_FM_B5_BMP, res->bmp_b[5]);
    create_bmp_res(ID_FM_B6_BMP, res->bmp_b[6]);
    create_bmp_res(ID_FM_B7_BMP, res->bmp_b[7]);
    create_bmp_res(ID_FM_B8_BMP, res->bmp_b[8]);
    create_bmp_res(ID_FM_B9_BMP, res->bmp_b[9]);
    return EPDK_OK;
}

/************************************************************************/
//释放资源
/************************************************************************/
__s32 fm_freq_scale_destroy_res(FM_FREQ_SCALE_RES_T *res)
{
    destroy_bmp_res(res->bmp_btn_thumb[BTN_NORMAL]);
    destroy_bmp_res(res->bmp_btn_thumb[BTN_FOCUS]);
#if (FM_SEARCH_CHN_MIN_FREQ == 87500)
    destroy_bmp_res(res->bmp_scale);
    destroy_bmp_res(res->bmp_87_5MHZ);
#elif (FM_SEARCH_CHN_MIN_FREQ == 87000)
    destroy_bmp_res(res->bmp_scale);
    destroy_bmp_res(res->bmp_87MHZ);
#endif
    destroy_bmp_res(res->bmp_cursor_sel);
    destroy_bmp_res(res->bmp_point);
    destroy_bmp_res(res->bmp_108MHZ);
    destroy_bmp_res(res->bmp_ch);
    destroy_bmp_res(res->bmp_MHZ);
    destroy_bmp_res(res->bmp_preset);
    destroy_bmp_res(res->bmp_slash);
    destroy_bmp_res(res->bmp_stereo);
    destroy_bmp_res(res->bmp_mono);
    destroy_bmp_res(res->bmp_update_bg);
    destroy_bmp_res(res->bmp_update0_bg);
    destroy_bmp_res(res->bmp_l[0]);
    destroy_bmp_res(res->bmp_l[1]);
    destroy_bmp_res(res->bmp_l[2]);
    destroy_bmp_res(res->bmp_l[3]);
    destroy_bmp_res(res->bmp_l[4]);
    destroy_bmp_res(res->bmp_l[5]);
    destroy_bmp_res(res->bmp_l[6]);
    destroy_bmp_res(res->bmp_l[7]);
    destroy_bmp_res(res->bmp_l[8]);
    destroy_bmp_res(res->bmp_l[9]);
    destroy_bmp_res(res->bmp_m[0]);
    destroy_bmp_res(res->bmp_m[1]);
    destroy_bmp_res(res->bmp_m[2]);
    destroy_bmp_res(res->bmp_m[3]);
    destroy_bmp_res(res->bmp_m[4]);
    destroy_bmp_res(res->bmp_m[5]);
    destroy_bmp_res(res->bmp_m[6]);
    destroy_bmp_res(res->bmp_m[7]);
    destroy_bmp_res(res->bmp_m[8]);
    destroy_bmp_res(res->bmp_m[9]);
    destroy_bmp_res(res->bmp_b[0]);
    destroy_bmp_res(res->bmp_b[1]);
    destroy_bmp_res(res->bmp_b[2]);
    destroy_bmp_res(res->bmp_b[3]);
    destroy_bmp_res(res->bmp_b[4]);
    destroy_bmp_res(res->bmp_b[5]);
    destroy_bmp_res(res->bmp_b[6]);
    destroy_bmp_res(res->bmp_b[7]);
    destroy_bmp_res(res->bmp_b[8]);
    destroy_bmp_res(res->bmp_b[9]);
    return EPDK_OK;
}

__s32 fm_auto_search_cb(FM_WND_T *fmplay_menu_attr)
{
    __msg("auto_seach@@@@@@@@  ");
    return EPDK_OK;
}
/************************************************************************/
// 画频道表及当前频道游标
/************************************************************************/

__s32 draw_currentfreq_scale(H_WIN hwnd, H_LYR layer, FM_UI_MAINWND *ui)
{
    char buf[8 * 2] = {0};
    int x;
    GUI_RECT rt;
    FM_CTRLDATA_T   *fm_ctrl;
    FM_FREQ_SCALE_RES_T *res;
    FM_MAINWND_T *wnd_para;
    GUI_RECT    gui_rect;//..
    __s8 a = 0, b = 0, c = 0, d = 0;
    __u32 cur_freq ;
    __u32 channel_id ;
    __u32 channel_count ;
    __u32 audiomode ;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
    res = &wnd_para->res.scale;
    fm_ctrl = wnd_para->fm_ctrl;
    x = RANGE_MAPPING(ui->scale.d0, ui->scale.d1,
                      fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq);
    //画背景
    GUI_BMP_RES_Draw(res->bmp_update_bg, 90, 279);
    GUI_BMP_RES_Draw(res->bmp_update_bg, 373, 279);
    //画标尺(背景)
    GUI_BMP_RES_Draw(res->bmp_scale, ui->scale_bg.x, ui->scale_bg.y);
    //画游标
    //x = RANGE_MAPPING(ui->scale.d0, ui->scale.d1,
    //  fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq);
    GUI_BMP_RES_Draw(res->bmp_cursor_sel, x, ui->scale_bg.y - ui->cursor_y);
    //当前频道:
    //当前频道
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetColor(GUI_WHITE);
    gui_rect.x0 = 315;
    gui_rect.y0 = 242;
    gui_rect.x1 = 412;
    gui_rect.y1 = 262;

    if (fm_ctrl->channel_count > 0)
    {
        b = fm_ctrl->channel_id + 1 ;
    }
    else
    {
        b = fm_ctrl->channel_id ;
    }

    //频道总数
    c = fm_ctrl->channel_count;
    GUI_BMP_RES_Draw(res->bmp_ch, 315, 242);
    app_sprintf(buf, "ch %d/%d", b, c);
    GUI_DispStringInRect(buf, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //显示中心文字
    //87000 ~ 108000KHZ换算成87MHZ ~ 108MHZ
    //87500 ~ 108000KHZ换算成87MHZ ~ 108MHZ
    //当前频段:
    a = (fm_ctrl->cur_freq)  / 100000;  //显示UI上的百位
    b = (fm_ctrl->cur_freq) % 100000 / 10000;
    c = (fm_ctrl->cur_freq) % 10000 / 1000;
    d = (fm_ctrl->cur_freq) % 1000 / 100;
    //__wrn("draw:fm_ctrl->cur_freq = %d",fm_ctrl->cur_freq);

    if ((a > MAX_NUM || a < 0) || \
        (b > MAX_NUM || b < 0) || \
        (c > MAX_NUM || c < 0) || \
        (d > MAX_NUM || d < 0))
    {
        return EPDK_FAIL;
    }

    if (a > 0)
    {
        GUI_BMP_RES_Draw(res->bmp_b[a], 210, 172);  // 显示百位  295,202
    }
    else
    {
        GUI_BMP_RES_Draw(res->bmp_update0_bg, 210, 172);
    }

    GUI_BMP_RES_Draw(res->bmp_b[b], 260, 172);      //显示十位   330,202
    GUI_BMP_RES_Draw(res->bmp_b[c], 318, 172);      //显示个位   373,202
    GUI_BMP_RES_Draw(res->bmp_point, 375, 172);     //显示小数点 415,202
    GUI_BMP_RES_Draw(res->bmp_b[d], 399, 172);      //显示小数点后一位    424,202
    //..GUI_BMP_RES_Draw(res->bmp_MHZ, 460,171);        //显示"MHZ"  470,201
    return EPDK_OK;
}

/************************************************************************/
// 画频道表及频道游标
/************************************************************************/

__s32 draw_freq_scale(H_WIN hwnd, H_LYR layer, FM_UI_MAINWND *ui)
{
    char buf[8 * 2] = {0};
    int x;
    GUI_RECT rt;
    FM_CTRLDATA_T   *fm_ctrl;
    FM_FREQ_SCALE_RES_T *res;
    FM_MAINWND_T *wnd_para;
    GUI_RECT    gui_rect;//..
    __s8 a = 0, b = 0, c = 0, d = 0;
    __u32 cur_freq ;
    __u32 channel_id ;
    __u32 channel_count ;
    __u32 audiomode ;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
    res = &wnd_para->res.scale;
    fm_ctrl = wnd_para->fm_ctrl;
    //  GUI_LyrWinSel(layer);
    /*
    {//for debug
        __u32 cur_freq ;
        __u32 channel_id ;
        __u32 channel_count ;
        __u32 audiomode ;

        dsk_radio_rcv_get_cur_freq(&cur_freq);
        channel_count = dsk_radio_rcv_get_search_result_vn();
        dsk_radio_get_audio_mode(&audiomode);
        __msg("draw:dsk_radio_rcv_get_cur_freq() = %d", cur_freq);
        __msg("draw:dsk_radio_rcv_get_search_result_vn() = %d", channel_count);
        __msg("draw:audiomode = %d ",audiomode); //cky
    }
    */
    //画背景
    GUI_BMP_RES_Draw(res->bmp_l[0], 0, 0);
    //画标尺(背景)
    GUI_BMP_RES_Draw(res->bmp_scale, ui->scale_bg.x, ui->scale_bg.y);
    //画游标
    x = RANGE_MAPPING(ui->scale.d0, ui->scale.d1,
                      fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq);
    GUI_BMP_RES_Draw(res->bmp_cursor_sel, x, ui->scale_bg.y - ui->cursor_y);
    //画按钮
# if 0 //..

    if (IsFmShowThumbBtn(fm_ctrl->thumb_show))
    {
        x -= (ui->btn_thumb.width >> 1);

        if (IsFmFocusThumbBtn(fm_ctrl->thumb_show))
        {
            GUI_BMP_RES_Draw(res->bmp_btn_thumb[BTN_FOCUS], x, ui->scale_bg.y - ui->btn_thumb.height);
        }
        else
        {
            GUI_BMP_RES_Draw(res->bmp_btn_thumb[BTN_FOCUS], x, ui->scale_bg.y - ui->btn_thumb.height);
        }
    }

#endif
    //a:百位；b:十位；c:个位；d:小数点后一位 。
    //显示左上角文字
    //当前频道:
    //当前频道
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetColor(GUI_WHITE);
    gui_rect.x0 = 315;
    gui_rect.y0 = 242;
    gui_rect.x1 = 410;
    gui_rect.y1 = 262;

    if (fm_ctrl->channel_count > 0)
    {
        b = fm_ctrl->channel_id + 1 ;
    }
    else
    {
        b = fm_ctrl->channel_id ;
    }

    //频道总数
    c = fm_ctrl->channel_count;
    app_sprintf(buf, "ch %d/%d", b, c);
    GUI_DispStringInRect(buf, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#if 0
    GUI_BMP_RES_Draw(res->bmp_ch, 195, 142);//显示"ch"    35, 38

    if (fm_ctrl->channel_count > 0)
    {
        b = (fm_ctrl->channel_id + 1) / 10;
        c = (fm_ctrl->channel_id + 1) % 10;
    }
    else
    {
        b = (fm_ctrl->channel_id) / 10;
        c = (fm_ctrl->channel_id) % 10;
    }

    GUI_BMP_RES_Draw(res->bmp_l[b], 230, 142);//显示十位    70, 38
    GUI_BMP_RES_Draw(res->bmp_l[c], 242, 142);//显示个位    82, 38
    //频道总数:
    GUI_BMP_RES_Draw(res->bmp_slash, 250, 142);  //显示斜杠"/"  90, 38
    b = fm_ctrl->channel_count / 10;
    c = fm_ctrl->channel_count % 10;
    GUI_BMP_RES_Draw(res->bmp_l[b], 262, 142);//显示十位   102, 38
    GUI_BMP_RES_Draw(res->bmp_l[c], 274, 142);//显示个位    114, 38
    //显示右上角文字
    //当前频道:
    GUI_BMP_RES_Draw(res->bmp_preset, 485, 140);//显示"preset"   325, 36

    if (fm_ctrl->channel_count > 0)
    {
        b = (fm_ctrl->channel_id + 1) / 10;
        c = (fm_ctrl->channel_id + 1) % 10;
    }
    else
    {
        b = (fm_ctrl->channel_id) / 10;
        c = (fm_ctrl->channel_id) % 10;
    }

    GUI_BMP_RES_Draw(res->bmp_m[b], 565, 142);  //显示十位   405,38
    GUI_BMP_RES_Draw(res->bmp_m[c], 580, 142);  //显示个位       420,38
#endif
    //显示中心文字
    //87000 ~ 108000KHZ换算成87MHZ ~ 108MHZ
    //87500 ~ 108000KHZ换算成87MHZ ~ 108MHZ
    //当前频段:
    a = (fm_ctrl->cur_freq)  / 100000;  //显示UI上的百位
    b = (fm_ctrl->cur_freq) % 100000 / 10000;
    c = (fm_ctrl->cur_freq) % 10000 / 1000;
    d = (fm_ctrl->cur_freq) % 1000 / 100;
    //__wrn("draw:fm_ctrl->cur_freq = %d",fm_ctrl->cur_freq);

    if ((a > MAX_NUM || a < 0) || \
        (b > MAX_NUM || b < 0) || \
        (c > MAX_NUM || c < 0) || \
        (d > MAX_NUM || d < 0))
    {
        return EPDK_FAIL;
    }

    if (a > 0)
    {
        GUI_BMP_RES_Draw(res->bmp_b[a], 210, 172);  // 显示百位  295,202
    }

    GUI_BMP_RES_Draw(res->bmp_b[b], 260, 172);      //显示十位   330,202
    GUI_BMP_RES_Draw(res->bmp_b[c], 318, 172);      //显示个位   373,202
    GUI_BMP_RES_Draw(res->bmp_point, 375, 172);     //显示小数点 415,202
    GUI_BMP_RES_Draw(res->bmp_b[d], 399, 172);      //显示小数点后一位    424,202
    GUI_BMP_RES_Draw(res->bmp_MHZ, 460, 171);       //显示"MHZ"  470,201
    //卡尺下频段文字
#if  (FM_SEARCH_CHN_MIN_FREQ == 87500)
    GUI_BMP_RES_Draw(res->bmp_87_5MHZ, 173, 276);       //显示"87.5MHZ"   13,172
#elif  (FM_SEARCH_CHN_MIN_FREQ == 87000)
    GUI_BMP_RES_Draw(res->bmp_87MHZ, 14, 331);      //显示"87MHZ"   13,172
#endif
    GUI_BMP_RES_Draw(res->bmp_108MHZ, 708, 331); //显示"108MHZ"   393,172
    //声音模式文字
#if 0
    dsk_radio_get_audio_mode(&fm_ctrl->audio_mode);
    __wrn("fm_ctrl->audio_mode = %d", fm_ctrl->audio_mode);

    if (fm_ctrl->audio_mode == 1)
    {
        GUI_BMP_RES_Draw(res->bmp_mono, 369, 276);      //显示"mono" 209,172
    }
    else if (fm_ctrl->audio_mode == 2)
    {
        GUI_BMP_RES_Draw(res->bmp_stereo, 369, 276);    //显示"stereo" 209,172
    }

#endif
    /*
        //显示文字
        GUI_SetFont(SWFFont);
        GUI_UC_SetEncodeUTF8();
        GUI_SetColor(ui->clr_txt_n);

        rt.x0 = ui->scale_bg.x;
        rt.x1 = rt.x0 + ui->scale_bg.width;
        rt.y0 = ui->scale_bg.y + ui->txt_y;
        rt.y1 = ui->scale_bg.y + ui->scale_bg.height;

        MAKE_STR_CHANNEL_FREQ(buf, fm_ctrl->cur_min_freq);
        GUI_DispStringInRect(STR_MIN_CHANNEL_FREQ, &rt, GUI_TA_LEFT);

        MAKE_STR_CHANNEL_FREQ(buf, fm_ctrl->cur_freq);
        GUI_DispStringInRect(buf, &rt, GUI_TA_CENTER);

        MAKE_STR_CHANNEL_FREQ(buf, fm_ctrl->cur_max_freq);
        GUI_DispStringInRect(STR_MAX_CHANNEL_FREQ, &rt, GUI_TA_RIGHT);
    */
    return EPDK_OK;
}

static __s32 scale_set_freq(H_WIN hwnd, __s16 x, __s16 y, __s32 action)
{
    __s16          y0, y1;
    FM_UI_MAINWND *ui;
    FM_CTRLDATA_T *fm_ctrl;
    FM_MAINWND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
    fm_ctrl = wnd_para->fm_ctrl;
    //__wrn("scale_set_freq...");
    ui = &get_fm_uipara()->main;
    y0 = ui->scale_bg.y - ui->btn_thumb.height;
    y1 = ui->scale_bg.y;

    if (1)//(ValueInRange(y0, y, y1))
    {
        __u32 old_freq;
        __s32 thumb_show;
        //__wrn("draw:fm_ctrl->cur_freq = %d",fm_ctrl->cur_freq);
        old_freq = fm_ctrl->cur_freq;
        thumb_show = fm_ctrl->thumb_show;

        //__here__;
        if (action != GUI_MSG_TOUCH_DOWN)//down时不设置freq
        {
            //__here__;
            if (x < ui->scale.d0)
            {
                x = ui->scale.d0;
            }
            else if (ui->scale.d1 < x)
            {
                x = ui->scale.d1;
            }

            fm_ctrl->cur_freq = RANGE_MAPPING(fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq,
                                              ui->scale.d0, ui->scale.d1, x);
            __msg("(x, y) -> calc cur_freq = %d (%d, %d)",
                  x, y, fm_ctrl->cur_freq, fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq);
        }

        switch (action)
        {
            case GUI_MSG_TOUCH_DOWN:
            {
                if (IsFmShowThumbBtn(fm_ctrl->thumb_show))
                {
                    fm_ctrl->thumb_show = FM_SHOW_THUMB_F;
                }
            }
            break;

            case GUI_MSG_TOUCH_MOVE://move时不设置频率
                //          {
                //              if (IsFmFocusThumbBtn(fm_ctrl->thumb_show))
                //              {
                //                  dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
                //                  dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
                //              }
                //          }
                break;

            case GUI_MSG_TOUCH_UP:
            {
                __wrn("scale bar GUI_MSG_TOUCH_UP...");

                if (IsFmFocusThumbBtn(fm_ctrl->thumb_show))
                {
                    //__here__;
                    __wrn("fm_ctrl->cur_freq=%d", fm_ctrl->cur_freq);
                    dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
                    //__here__;
                    dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq);
                    fm_ctrl->thumb_show = FM_SHOW_THUMB_N;
                }
            }
            break;
        }

        if (old_freq != fm_ctrl->cur_freq || thumb_show != fm_ctrl->thumb_show)
        {
            UI_UpdateFMNow(hwnd);
        }

        return EPDK_OK;
    }

    return EPDK_FAIL;
}

//测试是否点击到按钮,返回TOUCH_UI_INVALID_ID, *pFlag为中点到x的位移
static __s32 hit_test_scale_set_freq_btn(H_WIN hwnd, __s16 x, __s16 y, __s32 *pFlag)
{
    FM_CTRLDATA_T   *fm_ctrl;
    FM_MAINWND_T *wnd_para;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);

    if (wnd_para)
    {
        fm_ctrl = wnd_para->fm_ctrl;

        if (IsFmShowThumbBtn(fm_ctrl->thumb_show))
        {
            RECT btn_rt;
            FM_UI_MAINWND *ui;
            ui = &get_fm_uipara()->main;
            btn_rt.width  = ui->btn_thumb.width;
            btn_rt.height = ui->btn_thumb.height;
            btn_rt.x = RANGE_MAPPING(ui->scale.d0, ui->scale.d1,
                                     fm_ctrl->cur_min_freq, fm_ctrl->cur_max_freq, fm_ctrl->cur_freq); //中点

            if (pFlag)
            {
                *pFlag = x - btn_rt.x;
            }

            btn_rt.x -= (ui->btn_thumb.width >> 1);
            btn_rt.y = ui->scale_bg.y - ui->btn_thumb.height;

            if (XYInRect(x, y, btn_rt))
            {
                return 0;
            }
        }
    }

    return TOUCH_UI_INVALID_ID;
}


/************************************************************************/
// 微调fm播放频率
// step -- 频率位移,
// return value -- 1:播放频率改变, 0:播放频率不变
/************************************************************************/
__s32 fm_freq_inching(FM_CTRLDATA_T *fm_ctrl, __s32 step)
{
    __u32 cur_freq;
    ASSERT(fm_ctrl);

    if (fm_ctrl->search_mode == SRHMODE_AUTO || fm_ctrl->search_mode == SRHMODE_MANUAL)
    {
        return 0;
    }

    dsk_radio_rcv_get_cur_freq(&cur_freq);
    cur_freq = ((__s32)cur_freq + step);

    if (cur_freq < fm_ctrl->cur_min_freq)
    {
        cur_freq = fm_ctrl->cur_min_freq;
    }
    else if (fm_ctrl->cur_max_freq < cur_freq)
    {
        cur_freq = fm_ctrl->cur_max_freq;
    }

    //__wrn("draw:fm_ctrl->cur_freq = %d",fm_ctrl->cur_freq);
    if (cur_freq != fm_ctrl->cur_freq)
    {
        fm_ctrl->cur_freq = cur_freq;
        dsk_radio_rcv_set_freq_play(fm_ctrl->cur_freq);
        dsk_radio_rcv_get_cur_freq(&fm_ctrl->cur_freq); //for safe
        return 1;
    }
    else
    {
        return 0;
    }
}

/************************************************************************/
// 触摸处理
/************************************************************************/
__s32 on_freq_scale_touch(H_WIN hwnd, __s16 x, __s16 y, __s32 touch_action)
{
    static __s32 last_touch_action = GUI_MSG_TOUCH_DOWN;
    static __pos_t last_pos = {0, 0};
#define EmptyTouchAction()      {last_touch_action = GUI_MSG_TOUCH_DOWN; last_pos.x = 0; last_pos.y = 0;}
#define RecordTouchPos(_x, _y)  {last_pos.x = _x; last_pos.y = _y;}
#define RecordTouch(_a, _x, _y) {last_touch_action = _a; RecordTouchPos(_x, _y);}
    static __s32 old_id = TOUCH_UI_INVALID_ID;
    __s32 ret = 0;
    __s32 id;
    __s32 flag = 0;
    FM_UI_MAINWND *ui;
    FM_CTRLDATA_T *fm_ctrl = NULL;
    FM_MAINWND_T  *wnd_para;
    FM_GetWndPara(wnd_para, FM_MAINWND_T, hwnd);
    fm_ctrl = wnd_para->fm_ctrl;
    ui = &get_fm_uipara()->main;
    id = hit_test_scale_set_freq_btn(hwnd, x, y, &flag);
    __msg("hit_test_scale_set_freq_btn = %d, focus = %d", id, fm_ctrl->thumb_show);

    //在频表范围内
    if (ValueInRange(ui->scale_bg.y - ui->btn_thumb.height - 30, y, ui->scale_bg.y + ui->scale_bg.height + 30))
    {
        ret = 1;

        switch (touch_action)
        {
            case GUI_MSG_TOUCH_DOWN:
            case GUI_MSG_TOUCH_LONGDOWN:
            {
                //GUI_WinSetCaptureWin(hwnd);
                // __wrn("on_freq_scale_touch, GUI_MSG_TOUCH_DOWN");
                if (TouchValidID(id))
                {
                    scale_set_freq(hwnd, x, y, GUI_MSG_TOUCH_DOWN);
                }
            }
            break;

            case GUI_MSG_TOUCH_MOVE:
            {
                if (IsFmFocusThumbBtn(fm_ctrl->thumb_show))
                {
                    scale_set_freq(hwnd, x, y, GUI_MSG_TOUCH_MOVE);
                }
            }
            break;

            case GUI_MSG_TOUCH_UP:
            {
                if (IsFmFocusThumbBtn(fm_ctrl->thumb_show))
                {
                    if ((last_touch_action == GUI_MSG_TOUCH_DOWN || last_touch_action == GUI_MSG_TOUCH_LONGDOWN)
                        && (id == old_id && TouchValidID(id)))
                    {
                        if (fm_freq_inching(fm_ctrl,
                                            (flag < 0) ? -FM_FREQ_INCHING_VALUE : FM_FREQ_INCHING_VALUE))
                        {
                            fm_ctrl->thumb_show = FM_SHOW_THUMB_N;
                            UI_UpdateFMNow(hwnd);
                        }
                    }
                    else if (scale_set_freq(hwnd, x, y, GUI_MSG_TOUCH_UP) != EPDK_OK)
                    {
                        fm_ctrl->thumb_show = FM_SHOW_THUMB_N;
                        UI_UpdateFMNow(hwnd);
                    }
                }
            }
            break;
        }
    }
    else if (IsFmFocusThumbBtn(fm_ctrl->thumb_show))
    {
        ret = 1;

        if (GUI_MSG_TOUCH_UP == touch_action)
        {
            fm_ctrl->thumb_show = FM_SHOW_THUMB_N;
            UI_UpdateFMNow(hwnd);
        }
    }

    old_id = id;
    RecordTouch(touch_action, x, y)
    return ret;
}

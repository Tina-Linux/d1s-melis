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
#include "scan_proc_uipara.h"
#include "scan_proc.h"
#include "../dtv_mid.h"
#include "../dtv_i.h"
#include <emodules/mod_ca.h>

#undef  SCREEN_WIDTH
#define SCREEN_WIDTH    480
#undef  SCREEN_HEIGHT
#define SCREEN_HEIGHT   272

#define SCAN_STEP       10 //进度条步长
#define SEARCH_PROC_WIDTH  300 //进度条长度
#define SEARCH_PROC_HEIGHT  20 //进度条宽度

#define DTV_SCAN_UPDATE_MAX_ICON_RES_NUM 2


typedef struct
{
    __s32 x;
    __s32 y;
    __s32 w;
    __s32 h;
    __s32 res_id[DTV_SCAN_UPDATE_MAX_ICON_RES_NUM];
    HTHEME res_hdl[DTV_SCAN_UPDATE_MAX_ICON_RES_NUM];
} dtv_scan_update_rect_t;


typedef enum
{
    dtv_scan_update_prog_bg = 0,
    dtv_scan_update_prog_left,
    dtv_scan_update_prog_mid,
    dtv_scan_update_prog_cursor,
    dtv_scan_update_prog_icon_num
} dtv_scan_update_icon_t;

static __s32 last_key = -1;
static __u32 key_cnt = 0;
static __s32 create_flag = 0;
static __u32 before_filter_program_num = 0;

typedef struct tag_dvb_scan_proc_ctrl
{
    dvb_scan_proc_para_t *dvb_scan_proc_scene_para;

    __s32  total_ch;//搜索到的台数

    dtv_scan_update_rect_t prog_ui[dtv_scan_update_prog_icon_num];

} dvb_scan_proc_ctrl_t;

__s32 dvb_scan_proc_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    //char buf[128];
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    return (GUI_SendMessage(&msg));
}


static __s32 dtv_scan_draw_progress(dtv_scan_update_rect_t *prog_bg, dtv_scan_update_rect_t *prog_left,
                                    dtv_scan_update_rect_t *prog_mid, dtv_scan_update_rect_t *prog_cursor,
                                    __s32 min, __s32 max, __s32 cur)
{
    //画进度条背景
    {
        void *pbmp;

        if (!prog_bg->res_hdl[0])
        {
            __msg("prog_bg->res_hdl[0] is null...\n");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_bg->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_bg->x, prog_bg->y);
    }
    //画进度条左边的图标
    {
        void *pbmp;
        __s32 focus;
        focus = 0;

        if (!prog_left->res_hdl[focus])
        {
            __msg("prog_left->res_hdl[focus] is null...\n");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_left->res_hdl[focus]);

        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_left->x, prog_left->y);
    }

    //画进度条和cursor
    if (cur > max) //保护一下
    {
        cur = max;
    }

    if (cur < min) //保护一下
    {
        cur = min;
    }

    if (cur >= min
        && cur <= max)
    {
        void *pbmp;

        if (!prog_mid->res_hdl[0])
        {
            __msg("prog_mid->res_hdl[0] is null...\n");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_mid->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            return EPDK_FAIL;
        }

        {
            __s32 i;
            __s32 n;
            __s32 mid_w;
            __s32 max_mid_w;
            __s32 cursor_w;
            __s32 cursor_pos;
            __s32 bg_pos;
            __s32 bg_w;

            if (cur == min)
            {
                n = 0;
            }
            else
            {
                n = (prog_bg->w - prog_cursor->w) * (cur - min)
                    / prog_mid->w / (max - min);
            }

            mid_w = prog_mid->w;
            max_mid_w = prog_bg->w - prog_cursor->w;
            cursor_w = prog_cursor->w;
            bg_pos = prog_bg->x;
            bg_w = prog_bg->w;

            //防止非整数倍时画不满
            if (max_mid_w - n * mid_w < mid_w)
            {
                n++;
                cursor_pos = bg_pos + bg_w - cursor_w;
            }
            else
            {
                cursor_pos = bg_pos + n * mid_w;
            }

            for (i = 1 ; i < n ; i++)
            {
                GUI_BMP_Draw(pbmp, prog_mid->x + i * prog_mid->w
                             , prog_mid->y);
            }

            //画cursor
            if (!prog_cursor->res_hdl[0])
            {
                __msg("prog_cursor->res_hdl[0] is null...\n");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(prog_cursor->res_hdl[0]);

            if (!pbmp)
            {
                __msg("pbmp is null...\n");
                return EPDK_FAIL;
            }

            GUI_BMP_Draw(pbmp, cursor_pos, prog_cursor->y);
        }
    }

    return EPDK_OK;
}


static __s32 dtv_scan_res_init(__gui_msg_t *msg)
{
    __s32 width, height;
    dtv_scan_update_rect_t prog_ui[dtv_scan_update_prog_icon_num];
    dvb_scan_proc_ctrl_t *scan_ctrl = NULL;
    scan_ctrl = (dvb_scan_proc_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    dsk_display_get_size(&width, &height);
    scan_ctrl->prog_ui[dtv_scan_update_prog_bg].x = (width - 680) / 2;
    scan_ctrl->prog_ui[dtv_scan_update_prog_bg].y = height / 2 + 60;
    scan_ctrl->prog_ui[dtv_scan_update_prog_bg].w = 680;
    scan_ctrl->prog_ui[dtv_scan_update_prog_bg].h = 26;
    scan_ctrl->prog_ui[dtv_scan_update_prog_bg].res_id[0] = ID_INIT_PROG_BG_BMP;
    scan_ctrl->prog_ui[dtv_scan_update_prog_bg].res_id[1] = 0;
    scan_ctrl->prog_ui[dtv_scan_update_prog_left].x = (width - 680) / 2;
    scan_ctrl->prog_ui[dtv_scan_update_prog_left].y = height / 2 + 60;
    scan_ctrl->prog_ui[dtv_scan_update_prog_left].w = 12;
    scan_ctrl->prog_ui[dtv_scan_update_prog_left].h = 26;
    scan_ctrl->prog_ui[dtv_scan_update_prog_left].res_id[0] = ID_INIT_PROG_LEFT_BMP;
    scan_ctrl->prog_ui[dtv_scan_update_prog_left].res_id[1] = 0;
    scan_ctrl->prog_ui[dtv_scan_update_prog_mid].x = (width - 680) / 2;
    scan_ctrl->prog_ui[dtv_scan_update_prog_mid].y = height / 2 + 60;
    scan_ctrl->prog_ui[dtv_scan_update_prog_mid].w = 12;
    scan_ctrl->prog_ui[dtv_scan_update_prog_mid].h = 26;
    scan_ctrl->prog_ui[dtv_scan_update_prog_mid].res_id[0] = ID_INIT_PROG_MID_BMP;
    scan_ctrl->prog_ui[dtv_scan_update_prog_mid].res_id[1] = 0;
    scan_ctrl->prog_ui[dtv_scan_update_prog_cursor].x = (width - 680) / 2;
    scan_ctrl->prog_ui[dtv_scan_update_prog_cursor].y = height / 2 + 60;
    scan_ctrl->prog_ui[dtv_scan_update_prog_cursor].w = 26;
    scan_ctrl->prog_ui[dtv_scan_update_prog_cursor].h = 26;
    scan_ctrl->prog_ui[dtv_scan_update_prog_cursor].res_id[0] = ID_INIT_PROG_CURSOR_BMP;
    scan_ctrl->prog_ui[dtv_scan_update_prog_cursor].res_id[1] = 0;
    {
        int i;
        int j;

        for (i = 0 ; i < dtv_scan_update_prog_icon_num ; i++)
        {
            for (j = 0 ; j < DTV_SCAN_UPDATE_MAX_ICON_RES_NUM ; j++)
            {
                if (scan_ctrl->prog_ui[i].res_id[j])
                {
                    scan_ctrl->prog_ui[i].res_hdl[j] = dsk_theme_open(scan_ctrl->prog_ui[i].res_id[j]);

                    if (!scan_ctrl->prog_ui[i].res_hdl[j])
                    {
                        __wrn("dsk_theme_open fail...\n");
                    }
                    else//先读取nor数据占住到内存
                    {
                        dsk_theme_hdl2buf(scan_ctrl->prog_ui[i].res_hdl[j]);
                    }
                }
                else
                {
                    scan_ctrl->prog_ui[i].res_hdl[j] = NULL;
                }
            }
        }
    }
    return 0;
}
static void dtv_scan_res_uninit(__gui_msg_t *msg)
{
    int i;
    int j;
    dvb_scan_proc_ctrl_t *scan_ctrl = NULL;
    scan_ctrl = (dvb_scan_proc_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    for (i = 0 ; i < dtv_scan_update_prog_icon_num ; i++)
    {
        for (j = 0 ; j < DTV_SCAN_UPDATE_MAX_ICON_RES_NUM ; j++)
        {
            if (scan_ctrl->prog_ui[i].res_hdl[j])
            {
                dsk_theme_close(scan_ctrl->prog_ui[i].res_hdl[j]);
                scan_ctrl->prog_ui[i].res_hdl[j] = NULL;
            }
        }
    }
}
//msg->dwAddData1存的是搜索到的台数
//msg->dwAddData2高16位存的是搜索的频点数，低16位存是当前正在搜索的第几个频点

static __s32 dvb_scan_proc_loading_paint(__gui_msg_t *msg)
{
    dvb_scan_proc_ctrl_t *scan_ctrl;
    dvb_scan_proc_uipara_t *dvb_scan_proc_uipara;
    GUI_RECT gui_rect;
    char txt[128];
    H_WIN h_win = msg->h_deswin;
    __s32 search_ch_total = 0;
    __s32 total = maple_get_cur_search_freq_tatol();
    __s32 cur_index = 0;
    __s32 cur_freq;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    //__here__;
    scan_ctrl = (dvb_scan_proc_ctrl_t *)GUI_WinGetAddData(h_win);
    dvb_scan_proc_uipara = (dvb_scan_proc_uipara_t *)submenu_dvb_scan_proc_get_uipara();

    if (1 != create_flag)
    {
        __msg("-------------------------------------paint err. create_flag:%d\n", create_flag);
    }

    GUI_LyrWinSel(scan_ctrl->dvb_scan_proc_scene_para->layer);
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    GUI_SetFont(scan_ctrl->dvb_scan_proc_scene_para->font);
    GUI_SetColor(GUI_WHITE);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    //__here__;
    //先画图片，再写字
    dtv_scan_draw_progress(&scan_ctrl->prog_ui[dtv_scan_update_prog_bg],
                           &scan_ctrl->prog_ui[dtv_scan_update_prog_left],
                           &scan_ctrl->prog_ui[dtv_scan_update_prog_mid],
                           &scan_ctrl->prog_ui[dtv_scan_update_prog_cursor],
                           0, maple_get_cur_search_freq_tatol(), 1);
    GUI_SetColor(GUI_WHITE);
    //显示"正在搜索"
    dsk_langres_get_menu_text(STRING_DTV_SEARCH_NOW, txt, sizeof(txt));
    __wrn("txt = %s\n", txt);
    gui_rect.x0 = dvb_scan_proc_uipara->dvb_scan_str.x ;
    gui_rect.y0 = dvb_scan_proc_uipara->dvb_scan_str.y ;
    gui_rect.x1 = gui_rect.x0 + dvb_scan_proc_uipara->dvb_scan_str.width - 1 ;
    gui_rect.y1 = gui_rect.y0 + dvb_scan_proc_uipara->dvb_scan_str.height - 1 ;
    GUI_DispStringInRect(txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#if 0
    //显示频点
    //dsk_langres_get_menu_text(STRING_DTV_SEARCH_NOW,txt,sizeof(txt));
    cur_freq = maple_get_cur_search_freq(cur_index);
    eLIBs_sprintf(txt, "%d.%03d MHz", cur_freq / 1000000, cur_freq % 1000000);
    __wrn("txt = %s\n", txt);
    gui_rect.x0 = dvb_scan_proc_uipara->dvb_scan_freq.x ;
    gui_rect.y0 = dvb_scan_proc_uipara->dvb_scan_freq.y ;
    gui_rect.x1 = gui_rect.x0 + dvb_scan_proc_uipara->dvb_scan_freq.width - 1 ;
    gui_rect.y1 = gui_rect.y0 + dvb_scan_proc_uipara->dvb_scan_freq.height - 1 ;
    GUI_DispStringInRect(txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#endif
    //搜索到的台数
    eLIBs_memset(txt, 0x00, sizeof(txt));
#if (MAPLE_CA_STANDAR == DTV_CA_NONE)
    {
        char temp_txt[128] = {0};
        before_filter_program_num = 0;
        dsk_langres_get_menu_text(STRING_DTV_SEARCH_CH, temp_txt, sizeof(temp_txt));
        eLIBs_sprintf(txt, "Total: %d %s%d\n", before_filter_program_num, temp_txt, scan_ctrl->total_ch);
    }
#else
    {
        dsk_langres_get_menu_text(STRING_DTV_SEARCH_CH, txt, sizeof(txt));
        eLIBs_sprintf(txt, "%s%d\n", txt, scan_ctrl->total_ch);
    }
#endif
    __wrn("txt = %s\n", txt);
    gui_rect.x0 = dvb_scan_proc_uipara->dvb_scan_ch.x ;
    gui_rect.y0 = dvb_scan_proc_uipara->dvb_scan_ch.y ;
    gui_rect.x1 = gui_rect.x0 + dvb_scan_proc_uipara->dvb_scan_ch.width - 1 ;
    gui_rect.y1 = gui_rect.y0 + dvb_scan_proc_uipara->dvb_scan_ch.height - 1 ;
    GUI_DispStringInRect(txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) != GUI_LYRWIN_STA_ON)
    {
        GUI_LyrWinSetSta(GUI_WinGetLyrWin(h_win), GUI_LYRWIN_STA_ON);
    }

    return EPDK_OK;
}

static __s32 dvb_scan_cmd_proc(__gui_msg_t *msg)
{
    dvb_scan_proc_ctrl_t *scan_ctrl;
    dvb_scan_proc_uipara_t *dvb_scan_proc_uipara;
    GUI_RECT gui_rect;
    char txt[128] = {0};
    H_WIN h_win = msg->h_deswin;
    scan_ctrl = (dvb_scan_proc_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    dvb_scan_proc_uipara = (dvb_scan_proc_uipara_t *)submenu_dvb_scan_proc_get_uipara();

    if (NULL == scan_ctrl)
    {
        __wrn("NULL == scan_ctrl->\n");
        return EPDK_OK;
    }

    if (NULL == scan_ctrl->dvb_scan_proc_scene_para)
    {
        __wrn("NULL == scan_ctrl->dvb_scan_proc_scene_para.\n");
        return EPDK_OK;
    }

    if (NULL == scan_ctrl->dvb_scan_proc_scene_para->layer)
    {
        __wrn("NULL == scan_ctrl->dvb_scan_proc_scene_para->layer.\n");
        return EPDK_OK;
    }

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scan_ctrl->dvb_scan_proc_scene_para->layer)
        && GUI_LYRWIN_STA_SLEEP != GUI_LyrWinGetSta(scan_ctrl->dvb_scan_proc_scene_para->layer))
    {
        __wrn("------------------------Err Frame Buf fail.\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(scan_ctrl->dvb_scan_proc_scene_para->layer);
    GUI_LyrWinSetTop(scan_ctrl->dvb_scan_proc_scene_para->layer);
    GUI_SetFont(scan_ctrl->dvb_scan_proc_scene_para->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    __msg("msg->dwAddData1 = %08x \n", msg->dwAddData1);

    switch (msg->dwAddData1 & 0xff)
    {
        case SLIDE_UPDATE_FREQ:
        {
            __s32 freq = msg->dwAddData2;//当前搜索频点
            __s32 x, y;
            __s32 total = (msg->dwAddData1 >> 8) & 0xff; //maple_get_cur_search_freq_tatol();
            __s32 cur_index = HIWORD(msg->dwAddData1);
            //__msg("%08x  %08x\n",(msg->dwAddData1&0xffff0000),((msg->dwAddData1&0xffff0000)>>16));
            x = dvb_scan_proc_uipara->dvb_scan_proc_bmp_pos.x + cur_index * (SEARCH_PROC_WIDTH) / total;
            y = dvb_scan_proc_uipara->dvb_scan_proc_bmp_pos.y;
            __msg("cur_index = %d  x = %d \n", cur_index, x);
#if 1   //贴图方式      
            dtv_scan_draw_progress(&scan_ctrl->prog_ui[dtv_scan_update_prog_bg],
                                   &scan_ctrl->prog_ui[dtv_scan_update_prog_left],
                                   &scan_ctrl->prog_ui[dtv_scan_update_prog_mid],
                                   &scan_ctrl->prog_ui[dtv_scan_update_prog_cursor],
                                   1, total, cur_index);
#else   //填充颜色方式
            {
                GUI_LyrWinSel(scan_ctrl->dvb_scan_proc_scene_para->layer);
                GUI_SetColor(GUI_RED);
                GUI_FillRect(x, y, x + (SEARCH_PROC_WIDTH) / total, y + SEARCH_PROC_HEIGHT - 1);
            }
#endif
            //显示频点
            //dsk_langres_get_menu_text(STRING_DTV_SEARCH_NOW,txt,sizeof(txt));
            GUI_SetColor(GUI_WHITE);
            eLIBs_sprintf(txt, "%d.%03d MHz", freq / 1000, freq % 1000);
            //  eLIBs_sprintf(txt,"%d KHz\n",freq);
            __wrn("txt = %s\n", txt);
            gui_rect.x0 = dvb_scan_proc_uipara->dvb_scan_freq.x ;
            gui_rect.y0 = dvb_scan_proc_uipara->dvb_scan_freq.y ;
            gui_rect.x1 = gui_rect.x0 + dvb_scan_proc_uipara->dvb_scan_freq.width - 1 ;
            gui_rect.y1 = gui_rect.y0 + dvb_scan_proc_uipara->dvb_scan_freq.height - 1 ;
            GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_DispStringInRect(txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case SLIDE_UPDATE_CH:
        {
            __wrn("dvb_scan_cmd_proc, SLIDE_UPDATE_CH.\n");
            GUI_LyrWinSel(scan_ctrl->dvb_scan_proc_scene_para->layer);
            //搜索到的台数
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
            scan_ctrl->total_ch = msg->dwAddData2;
#if (MAPLE_CA_STANDAR == DTV_CA_NONE)
            {
                char temp_txt[128] = {0};
                before_filter_program_num += msg->dwReserved;
                dsk_langres_get_menu_text(STRING_DTV_SEARCH_CH, temp_txt, sizeof(temp_txt));
                eLIBs_sprintf(txt, "Total: %d %s%d\n", before_filter_program_num, temp_txt, scan_ctrl->total_ch);
            }
#else
            {
                dsk_langres_get_menu_text(STRING_DTV_SEARCH_CH, txt, sizeof(txt));
                eLIBs_sprintf(txt, "%s%d\n", txt, scan_ctrl->total_ch);
            }
#endif
#endif
            __wrn("txt = %s\n", txt);
            gui_rect.x0 = dvb_scan_proc_uipara->dvb_scan_ch.x ;
            gui_rect.y0 = dvb_scan_proc_uipara->dvb_scan_ch.y ;
            gui_rect.x1 = gui_rect.x0 + dvb_scan_proc_uipara->dvb_scan_ch.width - 1 ;
            gui_rect.y1 = gui_rect.y0 + dvb_scan_proc_uipara->dvb_scan_ch.height - 1 ;
            GUI_SetBkColor(GUI_BLUE);
            //GUI_SetColor(GUI_WHITE);
            //GUI_FillRect(gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1);
            GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringInRect(txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;
    }

    return EPDK_OK;
}




static __s32 _dvb_scan_proc_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            dvb_scan_proc_ctrl_t *scan_ctrl;
            dvb_scan_proc_para_t *dvb_scan_proc_para;
            //__here__;
            create_flag = 1;
            scan_ctrl = (dvb_scan_proc_ctrl_t *)My_Malloc(0, sizeof(dvb_scan_proc_ctrl_t));

            if (!scan_ctrl)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(scan_ctrl, 0, sizeof(dvb_scan_proc_ctrl_t));
            dvb_scan_proc_para = (dvb_scan_proc_para_t *)GUI_WinGetAttr(msg->h_deswin);
            scan_ctrl->dvb_scan_proc_scene_para = dvb_scan_proc_para;
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)scan_ctrl);
            //__here__;
            //GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(scan_ctrl->dvb_scan_proc_scene_para->layer, GUI_LYRWIN_STA_OFF);
            GUI_LyrWinSetTop(scan_ctrl->dvb_scan_proc_scene_para->layer);
            //GUI_LyrWinCacheOff();
            dtv_scan_res_init(msg);
            //__here__;
            last_key = -1;
            dvb_scan_proc_loading_paint(msg);
            GUI_LyrWinSetSta(scan_ctrl->dvb_scan_proc_scene_para->layer, GUI_LYRWIN_STA_ON);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            dvb_scan_proc_ctrl_t *scan_ctrl;
            scan_ctrl = (dvb_scan_proc_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");

            if (!scan_ctrl)
            {
                __wrn("scan_ctrl is null.......\n");
                return EPDK_FAIL;
            }

            if (scan_ctrl->dvb_scan_proc_scene_para->layer)
            {
                GUI_LyrWinSetBottom(scan_ctrl->dvb_scan_proc_scene_para->layer);
                GUI_LyrWinDelete(scan_ctrl->dvb_scan_proc_scene_para->layer);
                //scan_ctrl->dvb_scan_proc_scene_para->layer = NULL;
            }
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            dvb_scan_proc_ctrl_t *scan_ctrl;
            scan_ctrl = (dvb_scan_proc_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            create_flag--;

            if (0 != create_flag)
            {
                __wrn("-------------------------------------destroy err. create_flag:%d\n", create_flag);
            }

            if (!scan_ctrl)
            {
                __wrn("scan_ctrl is null.......\n");
                return EPDK_FAIL;
            }

            __wrn("GUI_MSG_DESTROY...\n");

            if (scan_ctrl->dvb_scan_proc_scene_para->layer)
            {
                GUI_LyrWinSetBottom(scan_ctrl->dvb_scan_proc_scene_para->layer);
                GUI_LyrWinSel(scan_ctrl->dvb_scan_proc_scene_para->layer);
                GUI_LyrWinSetSta(scan_ctrl->dvb_scan_proc_scene_para->layer, GUI_LYRWIN_STA_SUSPEND);
            }

            dtv_scan_res_uninit(msg);
            //if(scan_ctrl->h_bmp_loading)
            {
                //  dsk_theme_close(scan_ctrl->h_bmp_loading);
                //  scan_ctrl->h_bmp_loading = 0;
            }

            if (scan_ctrl->dvb_scan_proc_scene_para)
            {
                __wrn("...\n");
                My_Mfree(0, scan_ctrl->dvb_scan_proc_scene_para);
                scan_ctrl->dvb_scan_proc_scene_para = NULL;
            }

            if (scan_ctrl)
            {
                __wrn("...\n");
                My_Mfree(0, scan_ctrl);
                scan_ctrl = NULL;
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            //__here__;
            return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            dvb_scan_cmd_proc(msg);
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            __wrn("_dvb_scan_proc_proc() : GUI_MSG_KEY : msg->dwAddData1 = %d, msg->dwAddData2 = %d\n", msg->dwAddData1, msg->dwAddData2);

            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_ESCAPE:

                //      case GUI_MSG_KEY_MENU:
                case GUI_MSG_KEY_LONGMENU:
                    if ((msg->dwAddData2 == KEY_UP_ACTION))
                    {
                        __gui_msg_t my_msg;
                        my_msg.id = GUI_MSG_COMMAND;
                        my_msg.h_srcwin = NULL;
                        my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        my_msg.dwAddData1 = DTV_BACK2PLAY_PROGRAM;
                        my_msg.dwAddData2 = 0;
                        GUI_SendNotifyMessage(&my_msg);
                        eLIBs_printf("GUI_MSG_KEY_ESCAPE\n");
                        return EPDK_OK;
                    }

                default :
                    break;
            }
        }

        return EPDK_OK;

        case GUI_MSG_TOUCH:
            return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
    建立窗口
*/
static H_WIN dvb_scan_proc_win_create(H_WIN h_parent, dvb_scan_proc_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    dvb_scan_proc_para_t *dvb_scan_proc_scene_para;
    FB fb;
    __wrn("****************************************************************************************\n");
    __wrn("********  enter dvb scan proc application  **************\n");
    __wrn("****************************************************************************************\n");
    GUI_LyrWinGetFB(para->layer, &fb);
    //__here__;
    dvb_scan_proc_scene_para = (dvb_scan_proc_para_t *)My_Malloc(0, sizeof(dvb_scan_proc_para_t));

    if (!dvb_scan_proc_scene_para)
    {
        __msg("memory alloc fail.........\n");
        return NULL;
    }

    //__here__;
    eLIBs_memset(dvb_scan_proc_scene_para, 0, sizeof(dvb_scan_proc_para_t));
    dvb_scan_proc_scene_para->font = para->font;
    dvb_scan_proc_scene_para->layer = para->layer;
    //__here__;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    //__here__;
    framewin_para.name          = SUBMENU_DVB_SCAN;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.hHosting      = h_parent;
    framewin_para.id            = SUBMENU_DVB_SCAN_ID;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dvb_scan_proc_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)dvb_scan_proc_scene_para;
    framewin_para.hLayer        = para->layer;
    //__here__;
    return (GUI_FrmWinCreate(&framewin_para));
}

void  dvb_scan_proc_win_delete(H_WIN dvb_scan_proc_win)
{
    if (dvb_scan_proc_win != NULL)
    {
        GUI_LyrWinDelete(GUI_WinGetLyrWin(dvb_scan_proc_win));
    }
}
H_WIN submenu_dvb_scan_proc_create(H_WIN hparent, H_LYR layer)
{
    dvb_scan_proc_para_t dvb_scan_proc_scene_para;
    //dvb_scan_proc_uipara_t* dvb_scan_proc_uipara;
    H_WIN h_wnd;
    dvb_scan_proc_scene_para.layer = layer;
    dvb_scan_proc_scene_para.font = SWFFont;
    h_wnd = dvb_scan_proc_win_create(hparent, &dvb_scan_proc_scene_para);
    return h_wnd;
}

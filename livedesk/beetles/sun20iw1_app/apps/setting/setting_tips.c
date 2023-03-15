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
#include "setting_tips.h"
#include "setting_uipara.h"

#define     UDISK_NAME          "f:\\"

typedef struct tag_tips_ctrl
{
    setting_tip_para_t *tips_para;

    __s32 focus_txt_color;

    HTHEME h_sys_info_bmp;
    HTHEME h_ok_bmp;
    char ok_txt[32];    //string yes
    char tsize[128];    //total size
    char fsize[128];    //free size
    char svers[128];    //soft version

    char total_size[128];
    char free_size[128];
    tips_uipara_t *tips_uipara;
} tips_ctrl_t;

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
H_LYR setting_tips_layer_create()
{
    H_LYR layer = NULL;
    setting_uipara_t *uipara ;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        // {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        //    DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        MOD_DISP_LAYER_WORK_MODE_NORMAL,             /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        10,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "setting tips layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    uipara = (setting_uipara_t *)setting_get_uipara();
    fb.size.width       = uipara->setting_tips_uipara.tips_lyr.w ;
    fb.size.height      = uipara->setting_tips_uipara.tips_lyr.h ;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = uipara->setting_tips_uipara.tips_lyr.w ;
    lstlyr.src_win.height   = uipara->setting_tips_uipara.tips_lyr.h;
    lstlyr.scn_win.x        = uipara->setting_tips_uipara.tips_lyr.x ;
    lstlyr.scn_win.y        = uipara->setting_tips_uipara.tips_lyr.y ;
    lstlyr.scn_win.width    = uipara->setting_tips_uipara.tips_lyr.w ;
    lstlyr.scn_win.height   = uipara->setting_tips_uipara.tips_lyr.h ;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("tips layer create error !\n");
    }

    return layer;
}
static __u64 setting_dick_total_capacity_get(void)
{
    __u64  size = 0;
    __s32  i;
    char    *partition[RAT_MAX_PARTITION - 1] = {"f:", "g:", "h:", "i:", "j:", "k:", "l:", "m:", "n:", "o:"};
    __wrn("size = %ud \n", size);

    for (i = 0; i < RAT_MAX_PARTITION - 1; i++)
    {
        size += eLIBs_GetVolTSpace(partition[i]);
        __wrn("size = %ud \n", size);
    }

    __wrn("total size = %ud \n", size);
    return size;
}

static __u64 setting_dick_total_capacity_get_except_encryption(void)
{
    __u64 size = 0;
    __u64 total_size = 0;
    char  name[16];
    total_size = setting_dick_total_capacity_get();
    __wrn("t size = %ud \n", total_size);

    if (rat_is_encryption_partition_Insert(name, 1))
    {
        size = eLIBs_GetVolTSpace(name);
    }

    __wrn("e size = %ud \n", total_size);
    total_size = total_size - size;
    __wrn("except size = %ud \n", total_size);
    return total_size;
}


static __u64 setting_dick_free_capacity_get(void)
{
    __u64  size = 0;
    __s32  i;
    char    *partition[RAT_MAX_PARTITION - 1] = {"f:", "g:", "h:", "i:", "j:", "k:", "l:", "m:", "n:", "o:"};

    for (i = 0; i < RAT_MAX_PARTITION - 1; i++)
    {
        size += eLIBs_GetVolFSpace(partition[i]);
    }

    __wrn("total size = %ud \n", size);
    return size;
}

static __u64 setting_dick_free_capacity_get_except_encryption(void)
{
    __u64 size = 0;
    __u64 total_size = 0;
    char  name[16];
    total_size = setting_dick_free_capacity_get();

    if (rat_is_encryption_partition_Insert(name, 1))
    {
        size = eLIBs_GetVolFSpace(name);
    }

    total_size = total_size - size;
    __wrn("except size = %ud \n", total_size);
    return total_size;
}



void setting_tips_get_volume_size(__gui_msg_t *msg)
{
    tips_ctrl_t *tips_ctrl;
    char        str[768];
    char        numstr[32];
    __u64       size = 0;
    __u64       gnum = 0;
    __u64        snum = 0;
    int i;
    char *p;
    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    eLIBs_memset(str, 0, 768);
    eLIBs_strcat(str, tips_ctrl->tsize);
    //size = eLIBs_GetVolTSpace(UDISK_NAME);
    size = setting_dick_total_capacity_get_except_encryption();
    p = (char *)&size;

    for (i = 0; i < 8; i++)
    {
        __wrn("%d \n ", *p++);
    }

    __wrn("0 size = %ud \n", size);
    //size = 0;
    //  if(((__s64)0) == size)
    //  {
    //      snum = (__s64)0;
    //  }
    //  else
    //  {
    gnum = (size * 100) >> 30;//先乘100，用于取.GB 后面两位小数
    snum = (gnum % 100);        //去小数点后面的数值
    gnum = (gnum / 100);        //取小数点前面的数值
    //  }
    __wrn("gnum = %ud snum = %ud \n", gnum, snum);
    eLIBs_uint2str_dec(gnum, numstr);
    __wrn("%s \n", numstr);
    eLIBs_strcat(str, numstr);
    eLIBs_strcat(str, ".");
    eLIBs_uint2str_dec(snum, numstr);
    __wrn("%s \n", numstr);

    if (snum >= 10)
    {
        eLIBs_strcat(str, numstr);
    }
    else
    {
        eLIBs_strcat(str, "0");
        eLIBs_strcat(str, numstr);
    }

    eLIBs_strcat(str, "GB");
    eLIBs_strcpy(tips_ctrl->total_size, (const char *)str);
    eLIBs_memset(str, 0, 768);
    eLIBs_strcat(str, tips_ctrl->fsize);
    //size = eLIBs_GetVolFSpace(UDISK_NAME);
    size = setting_dick_free_capacity_get_except_encryption();
    __wrn("0 size = %ud \n", size);
    //size = 0;
    gnum = (size * 100) >> 30;
    snum = (gnum % 100);
    gnum = (gnum / 100);
    __wrn("gnum = %ud snum = %ud \n", gnum, snum);
    eLIBs_uint2str_dec(gnum, numstr);
    __wrn("%s \n", numstr);
    eLIBs_strcat(str, numstr);
    eLIBs_strcat(str, ".");
    eLIBs_uint2str_dec(snum, numstr);
    __wrn("%s \n", numstr);

    if (snum >= 10)
    {
        eLIBs_strcat(str, numstr);
    }
    else
    {
        eLIBs_strcat(str, "0");
        eLIBs_strcat(str, numstr);
    }

    eLIBs_strcat(str, "GB");
    eLIBs_strcpy(tips_ctrl->free_size, (const char *)str);
}


static void setting_tip_init(tips_ctrl_t *tips_ctrl)
{
    if (!tips_ctrl->h_sys_info_bmp)
    {
        tips_ctrl->h_sys_info_bmp = theme_open(ID_SETTING_SYS_INFO_TIPS_BMP);
    }

    if (!tips_ctrl->h_ok_bmp)
    {
        tips_ctrl->h_ok_bmp = theme_open(ID_SETTING_OK_BMP);
    }

    get_menu_text(STRING_OK, tips_ctrl->ok_txt,  sizeof(tips_ctrl->ok_txt));
    get_menu_text(STRING_SET_TIPS_TSIZE, tips_ctrl->tsize, sizeof(tips_ctrl->tsize));
    get_menu_text(STRING_SET_TIPS_FSIZE, tips_ctrl->fsize, sizeof(tips_ctrl->fsize));
    //get_menu_text(STRING_SET_TIPS_SVERS, tips_ctrl->svers, 128);
}

static __s32 setting_tips_paint(__gui_msg_t *msg)
{
    tips_ctrl_t *tips_ctrl;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara ;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_LyrWinSel(tips_ctrl->tips_para->layer);
    GUI_SetFont(tips_ctrl->tips_para->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(BEETLES_BLACK_COLOR);
    GUI_SetColor(tips_ctrl->focus_txt_color);
    setting_tips_get_volume_size(msg);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->h_sys_info_bmp), uipara->setting_tips_uipara.sys_info_bmp_pos.x, uipara->setting_tips_uipara.sys_info_bmp_pos.y);
    GUI_DispStringAt(tips_ctrl->total_size, uipara->setting_tips_uipara.tip_tsize_pos.x, uipara->setting_tips_uipara.tip_tsize_pos.y);
    GUI_DispStringAt(tips_ctrl->free_size, uipara->setting_tips_uipara.tip_fsize_pos.x, uipara->setting_tips_uipara.tip_fsize_pos.y);
    gui_rect.x0 = uipara->setting_tips_uipara.ok_bmp_pos.x ;
    gui_rect.y0 = uipara->setting_tips_uipara.ok_bmp_pos.y ;
    gui_rect.x1 = gui_rect.x0 + uipara->setting_tips_uipara.ok_bmp_pos.w - 1  ;
    gui_rect.y1 = gui_rect.y0 + uipara->setting_tips_uipara.ok_bmp_pos.h  - 1 ;
    //GUI_ClearRectEx(&gui_rect);
    GUI_BMP_Draw(theme_hdl2buf(tips_ctrl->h_ok_bmp), uipara->setting_tips_uipara.ok_bmp_pos.x, uipara->setting_tips_uipara.ok_bmp_pos.y);
    GUI_DispStringInRect(tips_ctrl->ok_txt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    {
        //version info
        GUI_RECT rect;
        reg_system_para_t *para;
        rect.x0 = uipara->setting_tips_uipara.tip_fsize_pos.x;
        rect.x1 = rect.x0 + uipara->setting_tips_uipara.tip_fsize_pos.w + 16;
        rect.y0 = uipara->setting_tips_uipara.tip_fsize_pos.y + 24;
        rect.y1 = rect.y0 + 24;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            GUI_DispStringAt(para->ver_inf, rect.x0, rect.y0);
        }
        else
        {
            GUI_DispStringAt("version unknown", rect.x0, rect.y0);
        }
    }
    return EPDK_OK;
}

static __s32  setting_tips_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    tips_ctrl_t *tips_ctrl;
    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_ENTER == last_key)
        {
            __gui_msg_t mymsg;
            mymsg.id = GUI_MSG_CLOSE;
            mymsg.h_deswin = msg->h_deswin;
            mymsg.h_srcwin = msg->h_srcwin;
            mymsg.dwAddData1 = mymsg.dwAddData1;
            mymsg.dwAddData2 = mymsg.dwAddData2;
            GUI_SendMessage(&mymsg);
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LEFT:      //映射为up
            case GUI_MSG_KEY_LONGLEFT:
                break;

            case GUI_MSG_KEY_RIGHT:     //映射为down
            case GUI_MSG_KEY_LONGRIGHT:
                break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
                break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
                break;

            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_MENU:
            {
                __gui_msg_t mymsg;
                __msg("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                mymsg.id = GUI_MSG_CLOSE;
                mymsg.h_deswin = msg->h_deswin;
                mymsg.h_srcwin = msg->h_srcwin;
                mymsg.dwAddData1 = mymsg.dwAddData1;
                mymsg.dwAddData2 = mymsg.dwAddData2;
                GUI_SendMessage(&mymsg);
            }
            break;

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}
static __s32  tips_touch_proc(__gui_msg_t *msg)
{
    tips_ctrl_t *tips_ctrl;
    setting_uipara_t *uipara ;
    tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    uipara = (setting_uipara_t *) setting_get_uipara();
    __msg("msg->dwAddData1 = %x\n", msg->dwAddData1);

    if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
    {
        GUI_RECT gui_rect;
        __u32 pos_x, pos_y;
        gui_rect.x0 = uipara->setting_tips_uipara.ok_bmp_pos.x ;
        gui_rect.y0 = uipara->setting_tips_uipara.ok_bmp_pos.y ;
        gui_rect.x1 = gui_rect.x0 + uipara->setting_tips_uipara.ok_bmp_pos.w ;
        gui_rect.y1 = gui_rect.y0 + uipara->setting_tips_uipara.ok_bmp_pos.h ;
        //__here__ ;
        pos_x = LOWORD(msg->dwAddData2);
        pos_y = HIWORD(msg->dwAddData2);

        if ((pos_x >= gui_rect.x0 && pos_x <= gui_rect.x1) && (pos_y >= gui_rect.y0 && pos_y <= gui_rect.y1))
        {
            __gui_msg_t mymsg;
            mymsg.id = GUI_MSG_CLOSE;
            mymsg.h_deswin = msg->h_deswin;
            mymsg.h_srcwin = msg->h_srcwin;
            mymsg.dwAddData1 = mymsg.dwAddData1;
            mymsg.dwAddData2 = mymsg.dwAddData2;
            GUI_SendMessage(&mymsg);
        }
    }

    return EPDK_OK ;
}

/*
    回调
*/
static __s32 _setting_tips_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            tips_ctrl_t *tips_ctrl;
            setting_tip_para_t *tips_para;
            tips_ctrl = (tips_ctrl_t *)My_Balloc(sizeof(tips_ctrl_t));

            if (!tips_ctrl)
            {
                __err("memory alloc fail........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(tips_ctrl, 0, sizeof(tips_ctrl_t));
            tips_para = (setting_tip_para_t *)GUI_WinGetAttr(msg->h_deswin);
            tips_ctrl->focus_txt_color = 0xffea961d;
            tips_ctrl->tips_para = tips_para;
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)tips_ctrl);
            setting_tip_init(tips_ctrl);
            GUI_LyrWinSetSta(tips_ctrl->tips_para->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(tips_ctrl->tips_para->layer);
            GUI_WinSetFocusChild(msg->h_deswin) ;
            GUI_WinSetCaptureWin(msg->h_deswin);
        }

        return EPDK_OK;

        case DSK_MSG_FS_PART_PLUGIN:
        case DSK_MSG_FS_PART_PLUGOUT:
        {
            setting_tips_get_volume_size(msg);
            setting_tips_paint(msg);     //重新获取磁盘的容量
            //__here__;
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_GENERAL, 0, 0);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            tips_ctrl_t *tips_ctrl;
            tips_ctrl = (tips_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!tips_ctrl)
            {
                __err("tips_ctrl is null........\n");
                return EPDK_FAIL;
            }

            __msg("tips win destroy\n");

            if (GUI_WinGetCaptureWin()  == msg->h_deswin)
            {
                __msg(" setting tips release captute win \n");
                GUI_WinReleaseCapture();
            }

            theme_close(tips_ctrl->h_sys_info_bmp);
            theme_close(tips_ctrl->h_ok_bmp);

            if (tips_ctrl->tips_para)
            {
                __msg("tips win free tips_para.....\n");
                My_Mfree(0, tips_ctrl->tips_para);
            }

            if (tips_ctrl)
            {
                __msg("tips win free tips_ctrl.....\n");
                My_Mfree(0, tips_ctrl);
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            setting_tips_paint(msg);
            return EPDK_OK;

        case GUI_MSG_KEY:
            setting_tips_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            tips_touch_proc(msg);
            return EPDK_OK ;

        case GUI_MSG_TIMER:
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
    建立窗口
*/
H_WIN setting_tips_win_create(H_WIN h_parent, setting_tip_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_tip_para_t *tips_para;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    tips_para = (setting_tip_para_t *)My_Malloc(0, sizeof(setting_tip_para_t));

    if (!tips_para)
    {
        __err("memory alloc fail........\n");
        return (H_WIN)EPDK_FAIL;
    }

    tips_para->font = para->font;
    tips_para->layer = para->layer;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "setting_tips_win";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = GENERAL_PRODUCT_INFO_TIPS_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_tips_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width =  fb.size.width;
    framewin_para.rect.height =  fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)tips_para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

__s32  setting_tips_win_delete(H_WIN tips_win)
{
    H_LYR h_lyr ;

    if (!tips_win)
    {
        __msg(" win handle is null .........\n");
        return EPDK_FAIL ;
    }

    h_lyr = GUI_WinGetLyrWin(tips_win);

    if (!h_lyr)
    {
        __msg(" lyr handle is null .........\n");
        return EPDK_FAIL ;
    }

    GUI_LyrWinDelete(h_lyr);
    h_lyr = 0 ;
    return EPDK_OK ;
}

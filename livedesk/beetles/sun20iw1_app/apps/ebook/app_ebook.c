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
#include "app_ebook.h"
#include "ebook_sset.h"
#include "ebook_tips.h"
#include "ebook_uipara.h"
#include "page_com.h"
#include "mbook/mbook_private.h"
#include "tts/jt_tts.h"

#define NAME_EBOOK_BKPOINT "EBOOKINF"


#define BOOK_BMK_NAME_SAVE_BMK1  "ebook\\save_bmk\\bmk1"//..
#define BOOK_BMK_NAME_SAVE_BMK2  "ebook\\save_bmk\\bmk2"
#define BOOK_BMK_NAME_SAVE_BMK3  "ebook\\save_bmk\\bmk3"
#define BOOK_BMK_NAME_SAVE_BMK4  "ebook\\save_bmk\\bmk4"
#define BOOK_BMK_NAME_SAVE_BMK5  "ebook\\save_bmk\\bmk5"
#define BOOK_BMK_NAME_SAVE_BMK6  "ebook\\save_bmk\\bmk6"
#define BOOK_BMK_NAME_SAVE_BMK7  "ebook\\save_bmk\\bmk7"
#define BOOK_BMK_NAME_SAVE_BMK8  "ebook\\save_bmk\\bmk8"//..



#define LABEL1  0
#define LABEL2  1
#define LABEL3  2
#define LABEL4  3
#define LABEL5  4
#define LABEL6  5

void *My_Malloc(void *heap, __u32 size);


static __u8 audio_is_mute = 0;
static __u8 audio_power_is_on = 0;
static __u8 power_on_first_time = 0;
static __s32 alarm_play_flag = 0;

__u8 *tts_page_data;
static __s32 char_num = 0;

typedef     void   *H_BMK;//..

typedef struct bottom_bg_theme_res_s//..
{
    __s32   res_id;
    HTHEME  h_bmp_handle;
    void    *res_buffer;
} bottom_bg_theme_res_t;



typedef struct tag_ebook_ctrl
{
    GUI_FONT *ebook_font;
    __s32 root_type;

    __sset_create_para_t  *sset_create_para;    //sset 参数

    __ebook_bkpoint_t  book_bkpoint;
    H_DECODE            mbook;
    H_BMK               mbmk;//..
    //ebook_sset_para_t *bmk_para;//..电子书签
    __decode_config_t   config;             /* monkey core show config*/

    H_LYR               pageLyr;            /* slider layer */
    H_LYR               tipsLyr;
    H_LYR               bottom_bg_lyr; //..底图背景图层
    H_WIN               h_goto;//..
    H_LYR               goto_lyr;
    H_WIN               h_page;         /* page win component   */
    H_WIN               h_sset;
    H_WIN               h_tips;
    H_WIN               h_loading;
    H_WIN               h_frm_alarm_coming ;    //闹钟定时时间到的时候弹出的对话框的窗口句柄
    __bool              need_to_open_music ;    // 1 表示需要重新打开背影音乐，主要是闹铃来了之后，会关背景音
    __s32               cur_page;
    __s32               total_page;
    __s32               page_sta;
    __s32               auto_time;
    __bool                  tts_has_start;      //tts 是否已经开始合成数据
    __bool              tts_has_init;           //tts 是否已经初始化资源
    __s32               bkpoint_enable; //是否加载断点提示框
    bottom_bg_theme_res_t    bottom_bg_res_bmp;//..图片资源
} ebook_ctrl_t;

static __s32 ebook_timeid = 0xff;
static reg_ebook_para_t *reg_ebook_para = 0;


///
#define _ADDA_MEM_BASE  (0Xf1C23C00)
#define _ADDA_R_OFST(n)     (0X00 + (n)*4U)
#define ADDA_o_DAC_ANA_CTL              _ADDA_R_OFST(4)
#define ADDA_o_ADC_FIFO_CTL             _ADDA_R_OFST(7)
typedef struct _DacFifoCtl
{
    __u32 fifoFlushBit        : 1;
    __u32 fifoOvRunIrqEnbit   : 1;
    __u32 fifoUnRunIrqEnbit   : 1;
    __u32 fifoEmptyIrqEnbit   : 1;//b3
    __u32 fifoEmptyDrqEnbit   : 1;
    __u32 reserved0           : 1;//b5
    __u32 txSampResBit        : 1;//0/1--16/24 bits resolution
    __u32 monoEnBit           : 1;//b7
    __u32 addaLoopEnBit       : 1;//b8
    __u32 reserved1           : 2;
    __u32 fifoEmpTrigLevel    : 5;
    /*TX DATA Reg 32 bits width, Tx Fifo Item 24 bits width,
     *Data in Tx fifo is always MSB, if inputModeBit==1, hw dac will fetch Low 16 bits to fill hgih 16 bits of fifo item*/
    __u32 fifoInputModeBit    : 1;//b16, tx FIFO Input Mode, 0/1-- data of TX DATA Reg at MSB/LSB
    __u32 unRunSampSelBit     : 1;//0/1---send zero/last audio sample when under run.(from Jerry: always send last sample)
    __u32 reserved2           : 11;
    /*fs select map: 0 48Khz; 1 32Khz;    2 24Khz;    3 16Khz;
                     4 12Khz; 5 8Khz;     6 reseved;  7 reserved;  */
    __u32 fsSelect            : 3;//b29-31
} DacFifoCtl_t;


/***********************************************************************************************************
    建立图层
************************************************************************************************************/
H_LYR ebook_tips_layer_create(RECT *rect)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        0,                                              /* pipe      */
        7,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "ebook tips layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = rect->width;
    fb.size.height      = rect->height;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    lstlyr.scn_win.x        = (480 - rect->width) / 2;
    lstlyr.scn_win.y        = (272 - rect->height) / 2;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe = 0;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("tips layer create error !");
    }

    return layer;
}


/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
static H_WIN ebook_layer_create(RECT *rect)
{
    FB  fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,
        0,
        0,

        0,
        8,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };
    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        0
    };
    fb.size.width  = rect->width;
    fb.size.height = rect->height;
    para.pipe           = 1;
    para.scn_win.x      = rect->x;
    para.scn_win.y      = rect->y;
    para.scn_win.width  = rect->width;
    para.scn_win.height = rect->height;
    para.src_win.x      = 0;
    para.src_win.y      = 0;
    para.src_win.width  = rect->width;
    para.src_win.height = rect->height;
    para.fb = &fb;
    create_info.lyrpara = &para;
    return (GUI_LyrWinCreate(&create_info));
}
/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/


H_LYR bottom_bg_layer_creat(RECT *rect, __s32 pipe) //..
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        0,                                              /* pipe      */
        5,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "bottom bg layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    lyrcreate_info.name = "ebook_main";
    lyrcreate_info.initsta  = GUI_LYRWIN_STA_SUSPEND;
    fb.size.width           = rect->width;
    fb.size.height          = rect->height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.mode = MOD_DISP_LAYER_WORK_MODE_NORMAL;
    // framebuffer source window
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    //screen region
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe             = pipe;
    lstlyr.fb               = &fb;
    lyrcreate_info.lyrpara  = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app layer create error !");
    }

    return layer;
}

/*********************************************************************************************************************************************************
*
*       向父窗口发信息
*
***********************************************************************************************************************************************************/
__s32 Ebook_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    return (GUI_SendNotifyMessage(&msg));
}

/*********************************************************************************************************************************************************
*
*       画文本背景底图
*
***********************************************************************************************************************************************************/


static void ebook_ctrl_paint_init(ebook_ctrl_t *ebook_ctrl)
{
    RECT rect;
    GUI_MEMDEV_Handle   draw_mem;
    __s16 x1, y1;
    __s16 x0 = 0, y0 = 0, w = 800, h = 480;
    //建立图层
    ebook_uipara_t *ebook_uipara;
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    rect.x = ebook_uipara->bottom_bg.x;
    rect.y = ebook_uipara->bottom_bg.y;
    rect.width = ebook_uipara->bottom_bg.w;
    rect.height = ebook_uipara->bottom_bg.h;
    ebook_ctrl->bottom_bg_lyr = bottom_bg_layer_creat(&rect, 0);

    if (ebook_ctrl->bottom_bg_lyr == NULL)
    {
        __msg("&&&&&&&bottom_bg_layer_create fail &&&&&&&& ");
        return;
    }

    //操作图层
    x1 = w + x0;
    y1 = h + y0;
    GUI_LyrWinSetSta(ebook_ctrl->bottom_bg_lyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(ebook_ctrl->bottom_bg_lyr);
    eLIBs_memset(&ebook_ctrl->bottom_bg_res_bmp,  0, sizeof(ebook_ctrl->bottom_bg_res_bmp));
    //初始化资源
    ebook_ctrl->bottom_bg_res_bmp.res_id = ID_EBOOK_PAGE_BG_BMP ;
    ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle = dsk_theme_open(ebook_ctrl->bottom_bg_res_bmp.res_id);
    ebook_ctrl->bottom_bg_res_bmp.res_buffer = dsk_theme_hdl2buf(ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle);
    GUI_LyrWinSel(ebook_ctrl->bottom_bg_lyr);
    draw_mem = GUI_MEMDEV_Create(x0, y0, w, h);

    if (draw_mem == 0)
    {
        __msg("&&&&&&&&&&&&&&&&&&&   Create Memory drever fail! &&&&&&&&&&&&&&&&&&& ");
        //return;
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_SetBkColor(0x00);
    GUI_ClearRect(x0, y0, x1,  y1);
    GUI_BMP_Draw(ebook_ctrl->bottom_bg_res_bmp.res_buffer, 0, 0);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
}


/* static void ebook_ctrl_paint_init(ebook_ctrl_t *ebook_ctrl)
{
    RECT rect;

//建立图层
   ebook_uipara_t *ebook_uipara;
   ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
   rect.x = ebook_uipara->bottom_bg.x;
   rect.y = ebook_uipara->bottom_bg.y;
   rect.width = ebook_uipara->bottom_bg.w;
   rect.height = ebook_uipara->bottom_bg.h;
   ebook_ctrl->bottom_bg_lyr=bottom_bg_layer_creat(&rect,0);

   //操作图层

   GUI_LyrWinSetSta(ebook_ctrl->bottom_bg_lyr, GUI_LYRWIN_STA_ON);
   GUI_LyrWinSetTop(ebook_ctrl->bottom_bg_lyr);
   eLIBs_memset( &ebook_ctrl->bottom_bg_res_bmp ,  0 , sizeof(ebook_ctrl->bottom_bg_res_bmp) );
   //初始化资源
   ebook_ctrl->bottom_bg_res_bmp.res_id = ID_EBOOK_PAGE_BG_BMP ;
   ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle = dsk_theme_open( ebook_ctrl->bottom_bg_res_bmp.res_id );
   ebook_ctrl->bottom_bg_res_bmp.res_buffer = dsk_theme_hdl2buf( ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle  );
   GUI_LyrWinSel(ebook_ctrl->bottom_bg_lyr);
   GUI_BMP_Draw(ebook_ctrl->bottom_bg_res_bmp.res_buffer, 0, 0);
}
*/



/*********************************************************************************************************************************************************
*
*       向父窗口发信息
*
***********************************************************************************************************************************************************/
__s32 Ebook_cmd2parent_syn(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128];
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    return (GUI_SendNotifyMessage(&msg));
}

__s32 __ebook_set_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}
static __s32 mbook_load_breakpoint_info(__ebook_bkpoint_t  *book_bkpoint)
{
    __s32 ret;
    ret = MBOOK_Decode_get_filename(book_bkpoint->ebookname);

    if (-1 == ret)
    {
        __msg("get file name fail...");
        return -1;
    }

    if (!fifo_db_has_exist(NAME_EBOOK_BKPOINT))
    {
        __wrn("fifo_db_has_exist false\r");
        return -1;
    }

    if (get_item_content_from_fifo_db(NAME_EBOOK_BKPOINT, book_bkpoint->ebookname, book_bkpoint))
    {
        __wrn("get_item_content_from_fifo_db fail\r");
        return -1;
    }

    return 0;
}

static void mbook_save_breakpoint_info(__gui_msg_t *msg)
{
    __s32 ret;
    HRATNPL h_rat_npl;
    __s32  npl_index;
    ebook_ctrl_t         *ebook_ctrl;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ebook_ctrl->book_bkpoint.page_no = ebook_ctrl->cur_page;    //获取当前页码
    ret = MBOOK_Decode_get_filename(ebook_ctrl->book_bkpoint.ebookname);    //获取电子书的书面

    if (ret != 0)
    {
        __msg("get file name fail ........");
    }

    if (!fifo_db_has_exist(NAME_EBOOK_BKPOINT))
    {
        ret = create_fifo_db(NAME_EBOOK_BKPOINT, 4, sizeof(__ebook_bkpoint_t)); //不存在，创建数据库

        if (0 != ret)
        {
            __msg("create_fifo_db fail...");
        }
    }

    ret = add_item_to_fifo_db(NAME_EBOOK_BKPOINT, ebook_ctrl->book_bkpoint.ebookname, &ebook_ctrl->book_bkpoint);

    if (ret != 0)
    {
        __msg("add_item_to_fifo_db fail...");
    }
}

void ebook_set_auto_time(__gui_msg_t *msg)
{
    __u32 auto_play_speed;
    ebook_ctrl_t         *ebook_ctrl;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (reg_ebook_para)
    {
        auto_play_speed = reg_ebook_para->switch_time;

        if (auto_play_speed == 10)
        {
            if (reg_ebook_para->tts_switch == 0)        /*TTS  打开的情况下*/
            {
                ebook_ctrl->tts_has_start = EPDK_TRUE;
            }
            else
            {
                GUI_SetTimer(msg->h_deswin, ebook_timeid, 10 * 100, NULL);
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            ebook_ctrl->auto_time = 10 * 100;
        }
        else if (auto_play_speed == 20)
        {
            if (reg_ebook_para->tts_switch == 0)
            {
                ebook_ctrl->tts_has_start = EPDK_TRUE;
            }
            else
            {
                GUI_SetTimer(msg->h_deswin, ebook_timeid, 20 * 100, NULL);
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            ebook_ctrl->auto_time = 20 * 100;
        }
        else if (auto_play_speed == 30)
        {
            if (reg_ebook_para->tts_switch == 0)
            {
                ebook_ctrl->tts_has_start = EPDK_TRUE;
            }
            else
            {
                GUI_SetTimer(msg->h_deswin, ebook_timeid, 30 * 100, NULL);
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            ebook_ctrl->auto_time = 30 * 100;
        }
        else
        {
            if (reg_ebook_para->tts_switch == 0)
            {
                ebook_ctrl->tts_has_start = EPDK_TRUE;
            }
            else
            {
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            ebook_ctrl->auto_time = 0;
        }
    }
}

static H_WIN ebook_show_dialog(__gui_msg_t  *msg)
{
    H_WIN dlg;
    __s32 lang_id[2]={STRING_EBOOK_CUE,STRING_EBOOK_LOADING};
    default_dialog(dlg, msg->h_deswin, EBOOK_SBMK_ID, ADLG_OK, lang_id);
    return dlg;
}

__s32   mbook_on_create(__gui_msg_t *msg)
{
    __s32               tmp, time;
    __s32               tmp_point;
    char                path[1024] = {0};
    ebook_ctrl_t        *ebook_ctrl = NULL;
    ebook_uipara_t      *ebook_uipara = NULL;
    __page_create_para_t page_para = {0};
    power_state_e state;
    RECT rect;
    __s32 ret;
    __u8    err;
    __u32 loading_time = 0;

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    loading_time = esKRNL_TimeGet();
    if (ebook_ctrl == NULL)
    {
        __wrn(" CShowScene malloc error ");
        return EPDK_FAIL;
    }

    /*加载loading 提示框*/
    ebook_ctrl->h_loading = ebook_show_dialog(msg);//loading dialog
    //需要把8bpp图片换成32bpp图片
    // ebook_ctrl->h_loading = app_loading_win_create(msg->h_deswin , EBOOK_LOADING_ID);
    // __wrn("ebook_ctrl->h_loading=0x%x", ebook_ctrl->h_loading);
    /* 创建 mbook core 句柄 */
    err = 0;
    ebook_ctrl->mbook = MBOOK_Decode_Init(NULL, &err);

    if (ebook_ctrl->mbook == NULL)
    {
        __wrn(" init mbook error ");
        goto EBOOK_ERROR_1;
    }

    MBOOK_Decode_GetFilePath(ebook_ctrl->mbook, path);
    __wrn("path=%s", path);
    // 设置默认编码
    MBOOK_Decode_SetDefaultCharset(ebook_ctrl->mbook, EBookGetSysDefaultCharset());

    if (reg_ebook_para)
    {
        if ((reg_ebook_para->text_colour == APP_COLOR_BLACK) || (reg_ebook_para->text_colour == APP_COLOR_WHITE)
            || (reg_ebook_para->text_colour == APP_COLOR_YELLOW))
        {
            ebook_ctrl->config.font_color  =  reg_ebook_para->text_colour;
        }
        else
        {
            ebook_ctrl->config.font_color  = APP_COLOR_WHITE ;
            reg_ebook_para->text_colour = APP_COLOR_WHITE ;
        }
    }
    else
    {
        ebook_ctrl->config.font_color    = APP_COLOR_WHITE;
    }

    ebook_ctrl->config.back_color    = APP_COLOR_WHITE;
    ebook_ctrl->config.show_rotate   = 0;
    ebook_ctrl->config.font_size     = 16;
    ebook_ctrl->config.char_font     = ebook_ctrl->ebook_font;
    ebook_ctrl->config.row_space     = 10;
    ebook_ctrl->config.col_space     = 10;
    ebook_ctrl->config.border_width  = 50;//..
    ebook_ctrl->config.scroll_width  =  0;
    ebook_ctrl->config.bottom_width  = 30;
    ebook_ctrl->config.show_width    = 800;//..
    ebook_ctrl->config.show_height   = 480; //..
    MBOOK_Decode_Config(ebook_ctrl->mbook, &ebook_ctrl->config);
    // 打开页面显示
    ebook_ctrl->total_page = MBOOK_Decode_GetTotalPage(ebook_ctrl->mbook);
    __wrn("ebook_ctrl->total_page=%d", ebook_ctrl->total_page);
    __wrn("ebook_ctrl->book_bkpoint.page_no=%d", ebook_ctrl->book_bkpoint.page_no);
    loading_time = esKRNL_TimeGet() - loading_time;
    if(loading_time < 30)
    {
        esKRNL_TimeDly(30);
    }
    app_dialog_destroy(ebook_ctrl->h_loading); //删除loading 窗口
    ebook_ctrl->h_loading = NULL ;
    if (ebook_ctrl->book_bkpoint.page_no == 0)
    {
        ebook_ctrl->cur_page = MBOOK_Decode_ShowPage(ebook_ctrl->mbook, 0); //不存在断点信息或者存在断点信息但从头开始阅读
    }
    else
    {
        __wrn("ebook_ctrl->mbook=%d", ebook_ctrl->mbook);
        ebook_ctrl->cur_page = MBOOK_Show_LastPage(ebook_ctrl->mbook, ebook_ctrl->book_bkpoint.page_no);//从上一次阅读的地方，接着往下阅读
    }

    ebook_ctrl->mbmk = (H_BMK)BOOK_Mark_Init("book_mark.bmk");//..

    if (ebook_ctrl->mbmk == NULL)
    {
        __wrn(" init book mark error ");
        goto EBOOK_ERROR_2;
    }

    BOOK_Mark_Open(ebook_ctrl->mbmk, path);  //..
    ebook_ctrl->tts_has_start = EPDK_FALSE;         /*默认是关闭*/
    ebook_ctrl->tts_has_init = EPDK_FALSE;              /*TTS 资源没有初始化*/
    rect.x = ebook_uipara->page_lyr.x;
    rect.y = ebook_uipara->page_lyr.y;
    rect.width = ebook_uipara->page_lyr.w;
    rect.height = ebook_uipara->page_lyr.h;
    __inf("rect.x:%d, rect.y:%d, rect.width:%d, rect.height:%d\r", rect.x, rect.y, rect.width, rect.height);
    ebook_ctrl->pageLyr = ebook_layer_create(&rect);//创建page 图层

    if (ebook_ctrl->pageLyr == NULL)
    {
        __wrn("ebook_layer_create fail;ebook_ctrl->pageLyr null\r ");
        return EPDK_FAIL;
    }

#if 0
    //创建page_num_progress_lyr
    rect.x = ebook_uipara->page_num_progress_lyr.x ;
    rect.y = ebook_uipara->page_num_progress_lyr.y ;
    rect.width = ebook_uipara->page_num_progress_lyr.w ;
    rect.height = ebook_uipara->page_num_progress_lyr.h ;
    __inf("rect.x:%d, rect.y:%d, rect.width:%d, rect.height:%d\r", rect.x, rect.y, rect.width, rect.height);
    page_para.page_num_progress_lyr = ebook_layer_create(&rect);//创建page_num_progress_lyr 图层

    if (page_para.page_num_progress_lyr == NULL)
    {
        __wrn("ebook_layer_create fail;page_para.page_num_progress_lyr null\r ");
        return EPDK_FAIL;
    }

#else
    page_para.page_num_progress_lyr = NULL;
#endif
    page_para.font = ebook_ctrl->ebook_font;
    page_para.page_layer = ebook_ctrl->pageLyr;

    if (ebook_ctrl->auto_time != 0)
    {
        page_para.play_status = 1;
    }
    else
    {
        page_para.play_status = 0;
    }

    page_para.cur_page = ebook_ctrl->cur_page;
    page_para.total_page = ebook_ctrl->total_page;
    eLIBs_strcpy(page_para.name, (void *)(eLIBs_strchrlast(path, '\\') + 1));   //获取当前阅读的电子书的书名
    //GUI_LyrWinDelete(ebook_ctrl->loading_lyr);    //删除loading 窗口
    // app_loading_win_delete(ebook_ctrl->h_loading);    //删除loading 窗口
    //  ebook_ctrl->h_loading = NULL ;
    ebook_ctrl->h_page = page_frmwin_create(msg->h_deswin, &page_para);
    GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
    //ebook_set_auto_time(msg); //设置自动播放时间(通过读取注册表)
    //这句话主要是因为客户的特定需要，要求每次开机后应该默认为手动播放
    ebook_ctrl->auto_time = 0;

    if (reg_ebook_para)
    {
        reg_ebook_para->switch_time = 0;
    }

    if (reg_ebook_para->tts_switch == TTS_ON)
    {
        __mbook_decode_t *mbook_decode_p;
        Ebook_cmd2parent_syn(msg->h_deswin, CLOSE_AUDIO_APP, 0, 0);//关闭后台音频应用

        //if(is_app_exist(APP_MUSIC))
        //{
        //    Ebook_cmd2parent_syn(msg->h_deswin, CLOSE_BG_MUSIC, 0, 0);//关闭背景音乐
        //}
        if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE) //关闭自动播放
        {
            GUI_KillTimer(msg->h_deswin, ebook_timeid);
        }

        if (dsk_amplifier_is_on() != 1)
        {
            dsk_amplifier_onoff(1);//打开功放
        }

        mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;

        if (ebook_ctrl->tts_has_init == EPDK_FALSE)
        {
            TTS_play_server_init(mbook_decode_p->file_hdle, NULL);
            ebook_ctrl->tts_has_init = EPDK_TRUE;
        }

        if (ebook_ctrl->tts_has_start == EPDK_FALSE)
        {
            __gui_msg_t setmsg;
            MBOOK_TTS_court_page(ebook_ctrl->mbook, (char **)&tts_page_data, &char_num);
            TTS_play_server_start(mbook_decode_p->file_hdle, tts_page_data, char_num, msg->h_deswin);
            dsk_keytone_set_state(SET_KEYTONE_OFF);
            ebook_ctrl->tts_has_start  =  EPDK_TRUE;
            setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = 1;      //更新自动播放ICOM
            GUI_SendNotifyMessage(&setmsg);
        }
    }

    GUI_WinSetFocusChild(ebook_ctrl->h_page);
    return EPDK_OK;
EBOOK_ERROR_2:

    if (ebook_ctrl->mbook)
    {
        MBOOK_Decode_Uninit(ebook_ctrl->mbook);
        ebook_ctrl->mbook = NULL;
    }

EBOOK_ERROR_1:
    {
        __msg("*******err = %d*******", err);
        app_loading_win_delete(ebook_ctrl->h_loading);  //删除loading 窗口
        ebook_ctrl->h_loading = 0 ;

        if (1 != err) //如果拔卡则不退到浏览器
        {
            __gui_msg_t setmsg;
            setmsg.id = GUI_MSG_CLOSE;
            setmsg.h_deswin = msg->h_deswin;
            GUI_SendMessage(&setmsg);
        }
    }
    return EPDK_FAIL;
}

__s32   mbook_on_delete(__gui_msg_t *msg)
{
    char            context[100];
    __s32           offset, ret, ebook_num;
    char        ebook_file[1024];
    ebook_ctrl_t     *ebook_ctrl;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    mbook_save_breakpoint_info(msg);    //保存断点信息
    // 关闭mbook//
    //__here__;

    if (ebook_ctrl->mbmk) //..
    {
        // 书签操作
        //__here__;
        //__wait__;
        BOOK_Mark_Close(ebook_ctrl->mbmk);  //把书签内容写进文件中，并关闭文件
        //__here__;
        BOOK_Mark_Uninit(ebook_ctrl->mbmk);
        //__here__;
        ebook_ctrl->mbmk = NULL;
    }   //..

    if (ebook_ctrl->mbook)
    {
        MBOOK_Decode_Uninit(ebook_ctrl->mbook);
        ebook_ctrl->mbook = NULL;
    }

    //__here__;

    if (ebook_ctrl->h_loading)
    {
        app_loading_win_delete(ebook_ctrl->h_loading) ;
        ebook_ctrl->h_loading = NULL ;
    }

    if (ebook_ctrl->tipsLyr)
    {
        GUI_LyrWinDelete(ebook_ctrl->tipsLyr);
        ebook_ctrl->tipsLyr = NULL;
    }

    //__here__;
    __msg("ebook_ctrl->pageLyr=%x", ebook_ctrl->pageLyr);

    if (ebook_ctrl->pageLyr)
    {
        __msg("before GUI_LyrWinDelete");
        GUI_LyrWinDelete(ebook_ctrl->pageLyr);
        //__here__;
        ebook_ctrl->pageLyr = NULL;
    }

    //__here__;
    // 释放主窗口控制结构 //
    My_Mfree(0, ebook_ctrl);
    //__here__;
    ebook_ctrl = NULL;
    // 保存控制句柄//
    GUI_WinSetAttr(msg->h_deswin, NULL);
    // 恢复自动关屏功能 //
    //dsk_display_enable_close_scn();
    return EPDK_OK;
}
/**********************************************************************************************************************
* 跳转窗口的创建
**********************************************************************************************************************/
__s32 ebook_set_goto_creat(H_WIN parent)
{
    RECT rect;
    ebook_uipara_t *ebook_uipara;
    ebook_ctrl_t *ebook_ctrl;//..
    __goto_create_para_t  goto_para;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(parent);//..
    ebook_uipara = (ebook_uipara_t *)get_ebook_uipara();
    rect.x = ebook_uipara->goto_lyr.x;
    rect.y = ebook_uipara->goto_lyr.y;
    rect.width = ebook_uipara->goto_lyr.w;
    rect.height = ebook_uipara->goto_lyr.h;
    goto_para.goto_layer = bottom_bg_layer_creat(&rect, 1);
    ebook_ctrl->goto_lyr = goto_para.goto_layer;
    __wrn("creat layer=%x", goto_para.goto_layer);
    ebook_ctrl->h_goto = goto_frmwin_create(parent, &goto_para);
    return EPDK_OK;
}


/*
    sset create
*/
static __s32 page_info_proc(__gui_msg_t *msg)
{
    __gui_msg_t setmsg;
    ebook_ctrl_t     *ebook_ctrl;
    ebook_uipara_t  *ebook_uipara;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch (HIWORD(msg->dwAddData1))
    {
        case CMD_PAGE_SET_NEXT: //播放前一页
            /*{
                if(ebook_ctrl->tts_has_start)
                {
                    TTS_stop_server();
                    dsk_keytone_set_state(SET_KEYTONE_ON);
                    ebook_ctrl->tts_has_init =EPDK_FALSE;
                    ebook_ctrl->tts_has_start = EPDK_FALSE;
                }

                ebook_ctrl->cur_page= MBOOK_Decode_ShowNext(ebook_ctrl->mbook);
                setmsg.id = COM_CMD_UPDATE_PAGE;
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = ebook_ctrl->cur_page;
                GUI_SendMessage(&setmsg);
                MBOOK_TTS_court_page(ebook_ctrl ->mbook,(char **)&tts_page_data,&char_num);

            __msg("-------tts_has_start = %d  ebook_ctrl->tts_has_init = %d",tts_has_start,ebook_ctrl->tts_has_init);

                if(tts_has_start == EPDK_TRUE)
                {
                    __mbook_decode_t *mbook_decode_p;
                    mbook_decode_p  = (__mbook_decode_t*) ebook_ctrl->mbook;
                    if(ebook_ctrl->tts_has_init == EPDK_FALSE)
                    {
                        TTS_play_server_init(mbook_decode_p->file_hdle,NULL);
                        ebook_ctrl->tts_has_init = EPDK_TRUE;
                    }
                    if(ebook_ctrl->tts_has_start == EPDK_FALSE)
                    {
                        TTS_play_server_start(mbook_decode_p->file_hdle,tts_page_data,char_num, msg->h_deswin);
                        dsk_keytone_set_state(SET_KEYTONE_OFF);
                        ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                    }
                }
                break;
            }*/
        {
            __bool  tts_has_start = ebook_ctrl->tts_has_start;
            __msg("-------tts_has_start = %d", ebook_ctrl->tts_has_start);

            if (ebook_ctrl->tts_has_start == EPDK_TRUE)
            {
                TTS_stop_server();
                dsk_keytone_set_state(SET_KEYTONE_ON);
                ebook_ctrl->tts_has_init = EPDK_FALSE;
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            ebook_ctrl->cur_page = MBOOK_Decode_ShowNext(ebook_ctrl->mbook);
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
            MBOOK_TTS_court_page(ebook_ctrl ->mbook, (char **)&tts_page_data, &char_num);

            if (tts_has_start == EPDK_TRUE)
            {
                __mbook_decode_t *mbook_decode_p;
                __msg("*****************************");
                mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;

                if (ebook_ctrl->tts_has_init == EPDK_FALSE)
                {
                    TTS_play_server_init(mbook_decode_p->file_hdle, NULL);
                    ebook_ctrl->tts_has_init = EPDK_TRUE;
                }

                if (ebook_ctrl->tts_has_start == EPDK_FALSE)
                {
                    __msg("*****************************");
                    TTS_play_server_start(mbook_decode_p->file_hdle, tts_page_data, char_num, msg->h_deswin);
                    dsk_keytone_set_state(SET_KEYTONE_OFF);
                    ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                }
            }

            break;
        }

        case CMD_PAGE_SET_PREV: //播放下一页
            /*{

                if(ebook_ctrl->tts_has_start)
                {
                    TTS_stop_server();
                    dsk_keytone_set_state(SET_KEYTONE_ON);
                    ebook_ctrl->tts_has_init =EPDK_FALSE;
                    ebook_ctrl->tts_has_start = EPDK_FALSE;
                }
                ebook_ctrl->cur_page= MBOOK_Decode_ShowPrev(ebook_ctrl->mbook);
                setmsg.id = COM_CMD_UPDATE_PAGE;
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = ebook_ctrl->cur_page;
                GUI_SendMessage(&setmsg);
                MBOOK_TTS_court_page(ebook_ctrl ->mbook,(char **)&tts_page_data,&char_num);
                if(tts_has_start == EPDK_TRUE)
                {
                    __mbook_decode_t *mbook_decode_p;
                    mbook_decode_p  = (__mbook_decode_t*) ebook_ctrl->mbook;
                    if(ebook_ctrl->tts_has_init == EPDK_FALSE)
                    {
                        TTS_play_server_init(mbook_decode_p->file_hdle,NULL);
                        ebook_ctrl->tts_has_init = EPDK_TRUE;
                    }
                    if(ebook_ctrl->tts_has_start == EPDK_FALSE)
                    {
                        TTS_play_server_start(mbook_decode_p->file_hdle,tts_page_data,char_num, msg->h_deswin);
                        dsk_keytone_set_state(SET_KEYTONE_OFF);
                        ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                    }
                }
                break;
            }*/
        {
            __bool  tts_has_start = ebook_ctrl->tts_has_start;

            if (ebook_ctrl->tts_has_start == EPDK_TRUE)
            {
                TTS_stop_server();
                dsk_keytone_set_state(SET_KEYTONE_ON);
                ebook_ctrl->tts_has_init = EPDK_FALSE;
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            ebook_ctrl->cur_page = MBOOK_Decode_ShowPrev(ebook_ctrl->mbook);
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
            MBOOK_TTS_court_page(ebook_ctrl ->mbook, (char **)&tts_page_data, &char_num);

            if (tts_has_start == EPDK_TRUE)
            {
                __mbook_decode_t *mbook_decode_p;
                mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;
                esKRNL_TimeDly(50);

                if (ebook_ctrl->tts_has_init == EPDK_FALSE)
                {
                    TTS_play_server_init(mbook_decode_p->file_hdle, NULL);
                    ebook_ctrl->tts_has_init = EPDK_TRUE;
                }

                if (ebook_ctrl->tts_has_start == EPDK_FALSE)
                {
                    TTS_play_server_start(mbook_decode_p->file_hdle, tts_page_data, char_num, msg->h_deswin);
                    dsk_keytone_set_state(SET_KEYTONE_OFF);
                    ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                }
            }

            break;
        }

        case CMD_PAGE_SET_CLOSE:    //退出到浏览器
        {
            Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);//..SWITCH_TO_EXPLORER
            break;
        }

        case CMD_PAGE_SET_OUT_STOP_PLAY:
        {
            //如果没有后台音频，则允许自动关机
            if (EPDK_FALSE == is_app_exist(APP_MUSIC))
            {
                reg_system_para_t *para;
                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    dsk_set_auto_off_time(para->poweroff);
                    __wrn("para->poweroff=%d", para->poweroff);
                }
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
            {
                GUI_KillTimer(msg->h_deswin, ebook_timeid);
            }

            if (reg_ebook_para)
            {
                reg_ebook_para->switch_time = 0;        //注册表记录
            }

            ebook_ctrl->auto_time = 0 ;     //停止自动播放
            //g_enable_close_scn();
            break;
        }

        case CMD_PAGE_SET_PLAY: //播放，暂停状态切换
        {
            __msg("--------------msg->dwAddData2 = %d", msg->dwAddData2);
            alarm_play_flag = ebook_ctrl->auto_time;

            if (reg_ebook_para->tts_switch == TTS_ON)
            {
                __mbook_decode_t *mbook_decode_p;
                mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;

                if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                {
                    GUI_KillTimer(msg->h_deswin, ebook_timeid);
                }

                if (reg_ebook_para)
                {
                    reg_ebook_para->switch_time = 0;        //注册表记录
                }

                ebook_ctrl->auto_time = 0 ;     //停止自动播放

                if (msg->dwAddData2 == 0)
                {
                    if (ebook_ctrl->tts_has_start == EPDK_TRUE)     /*TTS  已经打开的情况下*/
                    {
                        __gui_msg_t setmsg;
                        TTS_stop_server();
                        dsk_keytone_set_state(SET_KEYTONE_ON);
                        ebook_ctrl->tts_has_init = EPDK_FALSE;
                        ebook_ctrl->tts_has_start = EPDK_FALSE;
                        setmsg.h_deswin = ebook_ctrl->h_page;
                        setmsg.dwAddData1 = ebook_ctrl->cur_page;
                        setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                        setmsg.h_deswin = ebook_ctrl->h_page;
                        setmsg.dwAddData1 = 0;
                        GUI_SendMessage(&setmsg);
                        dsk_amplifier_onoff(0);//关闭功放
                    }
                }
                else
                {
                    if (ebook_ctrl->tts_has_init == EPDK_FALSE)
                    {
                        TTS_play_server_init(mbook_decode_p->file_hdle, NULL);
                        ebook_ctrl->tts_has_init = EPDK_TRUE;
                    }

                    if (ebook_ctrl->tts_has_start == EPDK_FALSE)
                    {
                        MBOOK_TTS_court_page(ebook_ctrl->mbook, (char **)&tts_page_data, &char_num);
                        TTS_play_server_start(mbook_decode_p->file_hdle, tts_page_data, char_num, msg->h_deswin);
                        dsk_keytone_set_state(SET_KEYTONE_OFF);
                        ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                    }

                    dsk_amplifier_onoff(1);//打开功放
                }
            }
            else
            {
                if (msg->dwAddData2 == 0)   //电子书播放状态, 1 表示正在播放， 0 不是不是正常播放状态
                {
                    //如果没有后台音频，则允许自动关机
                    if (EPDK_FALSE == is_app_exist(APP_MUSIC))
                    {
                        reg_system_para_t *para;
                        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                        if (para)
                        {
                            dsk_set_auto_off_time(para->poweroff);
                            __wrn("para->poweroff=%d", para->poweroff);
                        }
                    }

                    if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                    {
                        GUI_KillTimer(msg->h_deswin, ebook_timeid);
                    }

                    if (reg_ebook_para)
                    {
                        reg_ebook_para->switch_time = 0;        //注册表记录
                    }

                    ebook_ctrl->auto_time = 0 ;     //停止自动播放
                    //g_enable_close_scn();
                }
                else
                {
                    __u32 ebook_auto_time;
                    dsk_set_auto_off_time(0);

                    switch (ebook_ctrl->auto_time)
                    {
                        case 0:
                            ebook_ctrl->auto_time  = 10 * 100; //开始不是自动播放模式的话，按下enter键后，默认的自动播放速度为10s
                            ebook_auto_time = 10;
                            break;

                        case 1000:
                            ebook_ctrl->auto_time  = 10 * 100;
                            ebook_auto_time = 10;
                            break;

                        case 2000:
                            ebook_ctrl->auto_time  = 20 * 100;
                            ebook_auto_time = 20;
                            break;

                        case 3000:
                            ebook_ctrl->auto_time  = 30 * 100;
                            ebook_auto_time = 30;
                            break;

                        default:
                            ebook_ctrl->auto_time = 10 * 100;
                            ebook_auto_time = 10;
                            break;
                    }

                    if (reg_ebook_para)
                    {
                        reg_ebook_para->switch_time = ebook_auto_time;
                    }

                    if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                    {
                        GUI_ResetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, 0);
                    }
                    else
                    {
                        GUI_SetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, 0);
                    }

                    g_disable_close_scn();
                }
            }

            break;
        }

        case CMD_PAGE_SET_SET:  //进入sset 场景
        {
            /* 停止长字符串滚动*/
            //__here__;
            {
                __gui_msg_t setmsg ;
                setmsg.id = COM_CMD_LONGSTRING_STOP ;
                setmsg.h_deswin = ebook_ctrl->h_page;
                GUI_SendMessage(&setmsg);
            }

            if (ebook_ctrl->pageLyr)
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_SLEEP);
            }

            if (EPDK_TRUE == GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid))
            {
                GUI_KillTimer(msg->h_deswin, ebook_timeid);
            }

            if (ebook_ctrl->sset_create_para)
            {
                My_Mfree(0, ebook_ctrl->sset_create_para);
                ebook_ctrl->sset_create_para = NULL;
            }

            ebook_ctrl->sset_create_para = Ebook_SSET_Build_CreatePara(msg->h_deswin);
            ebook_sset_init_focus_item_by_reg(ebook_ctrl->sset_create_para);

            if (ebook_ctrl->h_sset != NULL && SSET_court_is_tbar() == EPDK_TRUE)
            {
                if (SSET_tbar_is_show() == EPDK_FALSE)
                {
                    SSET_show_tbar();
                }
            }
            else
            {
                ebook_ctrl->h_sset = SSET_Create(ebook_ctrl->sset_create_para);
            }

            SSET_Set_Nodelist("ebook", NODELIST_ON);
            gscene_hbar_set_state(HBAR_ST_SHOW) ;   //显示headbar
        }
        break;

        case COM_CMD_SEEKPAGE :
        {
            ebook_ctrl->cur_page = msg->dwAddData1 ;
            MBOOK_Decode_SeekPage(ebook_ctrl->mbook, ebook_ctrl->cur_page);
            break ;
        }

        default:
            break;
    }

    return EPDK_OK ;
}
/*

*/
static __s32 ebook_tips_proc(__gui_msg_t *msg)
{
    ebook_ctrl_t     *ebook_ctrl;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch (HIWORD(msg->dwAddData1))
    {
        case ADLG_CMD_EXIT:
        {
            if (ADLG_IDYES == msg->dwAddData2)  // 选择断点阅读
            {
                __msg("read from page of last time  ");
            }
            else  if (ADLG_IDNO == msg->dwAddData2) // 选择从第一页开始阅读
            {
                ebook_ctrl->book_bkpoint.page_no = 0;
            }

            if (ebook_ctrl->h_tips)
            {
                app_dialog_destroy(ebook_ctrl->h_tips);
                ebook_ctrl->h_tips = NULL;
            }

            mbook_on_create(msg);
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

static __s32 ebook_tts_proc(__gui_msg_t *msg)
{
    __gui_msg_t        setmsg;
    ebook_ctrl_t     *ebook_ctrl;;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    __msg("______收到TTS  发过来的消息 %d", HIWORD(msg->dwAddData1));

    switch (HIWORD(msg->dwAddData1))
    {
        case CMD_TTS_SET_NEXT:  //播放前一页
        {
            if (ebook_ctrl->cur_page == ebook_ctrl->total_page)
            {
                setmsg.h_deswin = msg->h_deswin;
                setmsg.h_srcwin = msg->h_deswin;
                setmsg.id = GUI_MSG_COMMAND;
                setmsg.dwAddData1 = MAKELONG(EBOOK_TTS_ID, CMD_TTS_SET_STOP);
                GUI_SendNotifyMessage(&setmsg);
                return EPDK_FALSE;
            }

            ebook_ctrl->cur_page = MBOOK_Decode_ShowNext(ebook_ctrl->mbook);
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
            MBOOK_TTS_court_page(ebook_ctrl ->mbook, (char **)&tts_page_data, &char_num);
            break;
        }

        case CMD_TTS_SET_STOP:
        {
            __msg("----收到停止消息，ebook_ctrl->tts_has_start = %d", ebook_ctrl->tts_has_start);

            if (ebook_ctrl->tts_has_start == EPDK_TRUE)
            {
                TTS_stop_server();
                dsk_keytone_set_state(SET_KEYTONE_ON);
                ebook_ctrl->tts_has_init = EPDK_FALSE;
                ebook_ctrl->tts_has_start = EPDK_FALSE;
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = ebook_ctrl->cur_page;
                setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = 0;
                GUI_SendMessage(&setmsg);
            }

            break;
        }
    }

    return EPDK_OK;
}




static __s32 ebook_sset_proc(__gui_msg_t *msg)
{
    __gui_msg_t        setmsg;
    ebook_ctrl_t     *ebook_ctrl;
    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (msg->dwAddData2 == EBOOK_SSET_NC_ID_UNKNOW)
    {
        __wrn("msg->dwAddData2 == EBOOK_SSET_NC_ID_UNKNOW");
        return EPDK_OK;
    }

    switch (HIWORD(msg->dwAddData1))
    {
        case SSET_NC_TIMER:
        {
            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
                gscene_hbar_set_state(HBAR_ST_HIDE);
            }

            if (ebook_ctrl->pageLyr)
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
            }

            GUI_WinSetFocusChild(ebook_ctrl->h_page);
            break;
        }

        case EBOOK_SET_EXIT:        //sset 场景发送上来的消息码,通知退出应用程序
        {
            __msg("sset send exit message to main_win");

            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
            }

            if (ebook_ctrl->pageLyr)
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
            }

            GUI_WinSetFocusChild(ebook_ctrl->h_page);
            Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);  //退出到浏览器
            break;
        }

        case EBOOK_SET_MUSIC:   //背景音乐开关切换
        {
            if (ebook_ctrl->tts_has_start == EPDK_TRUE)     /*TTS  已经打开的情况下*/
            {
                TTS_stop_server();
                dsk_keytone_set_state(SET_KEYTONE_ON);
                ebook_ctrl->tts_has_init = EPDK_FALSE;
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            reg_ebook_para->tts_switch = TTS_OFF;
            __msg("sset end music switch message to main_wnd");

            if (is_app_exist(APP_MUSIC))
            {
                Ebook_cmd2parent_syn(msg->h_deswin, CLOSE_BG_MUSIC, 0, 0);//关闭背景音乐
                reg_ebook_para->bg_music_switch = BG_MUSIC_CLOSE;
            }
            else
            {
                //app_root_audio_power(1);
                __wrn("ebook OPEN_BG_MUSIC");
                Ebook_cmd2parent_syn(msg->h_deswin, OPEN_BG_MUSIC, 0, 0);//打开背景音乐
                reg_ebook_para->bg_music_switch = BG_MUSIC_OPEN;
            }

            {
                setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = 0;
                GUI_SendMessage(&setmsg);
            }

            break;
        }

        case EBOOK_SET_FCOLOR:  //设置字体颜色
        {
            __s32 color;
            __wrn("sset send set font color message to main_wnd");
            __wrn("sset send set font color message to main_wnd");

            if (msg->dwAddData2 == FONT_COLOR_EXIT)
            {
                app_ebook_notify_delete_sub_scene(msg->h_deswin);

                if (ebook_ctrl->pageLyr)//..
                {
                    GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                    GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
                }

                GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
                gscene_hbar_set_state(HBAR_ST_HIDE);  //..
                return EPDK_OK;
            }
            else
            {
                switch (msg->dwAddData2)
                {
                    case FONT_COLOR_BLACK:
                        color = APP_COLOR_BLACK;
                        break;

                    case FONT_COLOR_GREEN:
                        color = APP_COLOR_GREEN;
                        break;

                    case FONT_COLOR_BLUE:
                        color = GUI_BLUE;
                        break;

                    default:
                        color = APP_COLOR_BLACK;
                        break;
                }
            }

            if (reg_ebook_para)
            {
                reg_ebook_para->text_colour = color;
            }

            MBOOK_Decode_SetFontColor(ebook_ctrl->mbook, color);    //设置电子书颜色
            break;
        }

        case EBOOK_SET_TTS:             /*设置TTS */
        {
            __msg("****************msg->dwAddData2=%d***", msg->dwAddData2);

            if (msg->dwAddData2 == TTS_ON)
            {
                __mbook_decode_t *mbook_decode_p;
                mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;
                Ebook_cmd2parent_syn(msg->h_deswin, CLOSE_AUDIO_APP, 0, 0);//关闭后台音频应用
                //if(is_app_exist(APP_MUSIC))
                //{
                //    Ebook_cmd2parent_syn(msg->h_deswin, CLOSE_BG_MUSIC, 0, 0);//关闭背景音乐
                //}
                dsk_amplifier_onoff(1);//打开功放

                if (ebook_ctrl->tts_has_init == EPDK_FALSE)
                {
                    TTS_play_server_init(mbook_decode_p->file_hdle, NULL);
                    ebook_ctrl->tts_has_init = EPDK_TRUE;
                }

                //app_root_audio_power(1);
                //__here__
                if (ebook_ctrl->tts_has_start == EPDK_FALSE)
                {
                    MBOOK_TTS_court_page(ebook_ctrl->mbook, (char **)&tts_page_data,  &char_num);
                    __msg("-----------char_num =%d", char_num);
                    TTS_play_server_start(mbook_decode_p->file_hdle, tts_page_data, char_num, msg->h_deswin);
                    dsk_keytone_set_state(SET_KEYTONE_OFF);
                    ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                }

                setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = 1;      //自动播放
                GUI_SendMessage(&setmsg);
                reg_ebook_para->tts_switch = TTS_ON;
            }
            else if (msg->dwAddData2 == TTS_OFF)
            {
                ebook_ctrl_t *ebook_ctrl;
                ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                //__here__;
                dsk_amplifier_onoff(0);//关闭功放

                if (ebook_ctrl->tts_has_start == EPDK_TRUE)     /*TTS  已经打开的情况下*/
                {
                    __gui_msg_t setmsg;
                    TTS_stop_server();
                    dsk_keytone_set_state(SET_KEYTONE_ON);
                    ebook_ctrl->tts_has_init = EPDK_FALSE;
                    ebook_ctrl->tts_has_start = EPDK_FALSE;
                    //__here__;
                    setmsg.h_deswin = ebook_ctrl->h_page;
                    setmsg.dwAddData1 = ebook_ctrl->cur_page;
                    setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                    setmsg.dwAddData1 = 0;
                    GUI_SendMessage(&setmsg);
                }

                reg_ebook_para->tts_switch  = TTS_OFF;
            }
            else if (msg->dwAddData2 == TTS_EXIT)
            {
                app_ebook_notify_delete_sub_scene(msg->h_deswin);

                if (ebook_ctrl->pageLyr)//..
                {
                    GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                    GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
                }

                GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
                gscene_hbar_set_state(HBAR_ST_HIDE);  //..
            }
            else
            {
                /*if(ebook_ctrl->tts_has_init == EPDK_FALSE)
                {
                    __mbook_decode_t *mbook_decode_p;
                    char pTextCn[] ="  ";
                    tts_page_data=pTextCn;
                    mbook_decode_p  = (__mbook_decode_t*) ebook_ctrl->mbook;

                    if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                    {
                        GUI_KillTimer(msg->h_deswin,  ebook_timeid );
                    }
                    if( is_app_exist(APP_MUSIC) )
                    {
                        Ebook_cmd2parent(msg->h_deswin, CLOSE_BG_MUSIC, 0, 0);//关闭背景音乐
                        reg_ebook_para->bg_music_switch = BG_MUSIC_CLOSE;
                    }
                    //app_root_audio_power(1);
                    //__here__
                    TTS_play_server_init(mbook_decode_p->file_hdle,tts_page_data);
                    dsk_keytone_set_state(SET_KEYTONE_OFF);
                    ebook_ctrl->tts_has_init = EPDK_TRUE;
                    ebook_ctrl->tts_has_start = EPDK_FALSE;
                    setmsg.h_deswin = ebook_ctrl->h_page;
                    setmsg.dwAddData1 = ebook_ctrl->cur_page;
                    setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                    setmsg.h_deswin = ebook_ctrl->h_page;
                    setmsg.dwAddData1 = 0;
                    GUI_SendMessage(&setmsg);
                }
                else
                {

                }*/
                if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE) //关闭自动播放
                {
                    GUI_KillTimer(msg->h_deswin, ebook_timeid);
                }

                {
                    __mbook_decode_t *mbook_decode_p;
                    mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;

                    if (ebook_ctrl->tts_has_init == EPDK_FALSE)
                    {
                        //TTS_play_server_init(mbook_decode_p->file_hdle,NULL);
                        //ebook_ctrl->tts_has_init = EPDK_TRUE;
                    }

                    if (ebook_ctrl->tts_has_start == EPDK_FALSE)
                    {
                        MBOOK_TTS_court_page(ebook_ctrl->mbook, (char **)&tts_page_data, &char_num);
                        //TTS_play_server_start(mbook_decode_p->file_hdle,tts_page_data,char_num, msg->h_deswin);
                        dsk_keytone_set_state(SET_KEYTONE_OFF);
                        //ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                    }
                }
            }

            break;
        }

        case EBOOK_SET_AUTO:    //设置自动播放时间
        {
            __u32 ebook_auto_time;

            if (ebook_ctrl->tts_has_start == EPDK_TRUE)     /*TTS  已经打开的情况下*/
            {
                TTS_stop_server();
                dsk_keytone_set_state(SET_KEYTONE_ON);
                ebook_ctrl->tts_has_init = EPDK_FALSE;
                ebook_ctrl->tts_has_start = EPDK_FALSE;
                reg_ebook_para->tts_switch  = TTS_OFF;
            }

            __msg("sset send play pause status switch message to main_wnd");

            switch (msg->dwAddData2)
            {
                case AUTO_TIME_10:  // 10s 自动播放
                    ebook_ctrl->auto_time = 10 * 100;
                    ebook_auto_time = 10;
                    dsk_set_auto_off_time(0);
                    break;

                case AUTO_TIME_20:  // 20s 自动播放
                    ebook_ctrl->auto_time = 20 * 100;
                    ebook_auto_time = 20;
                    dsk_set_auto_off_time(0);
                    break;

                case AUTO_TIME_30:  // 30s 自动播放
                    ebook_ctrl->auto_time = 30 * 100;
                    ebook_auto_time = 30;
                    dsk_set_auto_off_time(0);
                    break;

                case AUTO_TIME_EXIT:    //退出
                    app_ebook_notify_delete_sub_scene(msg->h_deswin);

                    if (ebook_ctrl->pageLyr)//..
                    {
                        GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                        GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
                    }

                    GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
                    gscene_hbar_set_state(HBAR_ST_HIDE);  //..
                    return EPDK_OK;

                case AUTO_TIME_0:   //停止自动播放
                default:
                {
                    if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                    {
                        GUI_KillTimer(msg->h_deswin,  ebook_timeid);
                    }

                    ebook_auto_time = 0;
                    setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                    setmsg.h_deswin = ebook_ctrl->h_page;
                    setmsg.dwAddData1 = 0;      //不自动播放
                    GUI_SendMessage(&setmsg);
                    /*更新页数*/
                    {
                        __gui_msg_t setmsg;
                        setmsg.id = COM_CMD_UPDATE_PAGE;
                        setmsg.h_deswin = ebook_ctrl->h_page;
                        setmsg.dwAddData1 = ebook_ctrl->cur_page;
                        GUI_SendMessage(&setmsg);
                    }
                    MBOOK_TTS_court_page(ebook_ctrl ->mbook, (char **)&tts_page_data, &char_num);
                    return EPDK_OK;
                }

                if (ebook_ctrl->h_sset)
                {
                    SSET_Destroy(ebook_ctrl->h_sset);
                    ebook_ctrl->h_sset = 0;
                }
            }

            if (reg_ebook_para)
            {
                reg_ebook_para->switch_time = ebook_auto_time;
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
            {
                GUI_ResetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, NULL);
            }
            else
            {
                GUI_SetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, NULL);
            }

            setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = 1;      //自动播放
            GUI_SendMessage(&setmsg);
            break;
        }

        case EBOOK_SET_GOTO_PAGE:
        {
            __msg("sset send goto page message to main_wnd");

            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
            }

            __wrn("goto msg->h_deswin=%x", msg->h_deswin);
            ebook_set_goto_creat(msg->h_deswin);
            break;
        }

        case EBOOK_SET_OBMK:
        {
            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
            }

            {
                __s32        ret, i ;
                //__here__;
                __wrn("ebook_ctrl->mbmk=%d", ebook_ctrl->mbmk);
                __wrn("ebook_ctrl->book_bkpoint.bmk_numth=%d", ebook_ctrl->book_bkpoint.bmk_numth);
                ret = BOOK_Mark_GetPageOffset(ebook_ctrl->mbmk, msg->dwAddData2 + 1);

                if (EPDK_FAIL != ret)
                {
                    ebook_ctrl->cur_page = MBOOK_Decode_SeekOffset(ebook_ctrl->mbook, ret);
                }
            }

            if (ebook_ctrl->pageLyr)//..
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
            }

            GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
            gscene_hbar_set_state(HBAR_ST_HIDE);  //..
            break;
        }

        case EBOOK_SET_SBMK:
        {
            __s32       tmp;//..
            __s64       size;
            __s32       ret;
            char        str[64];
            __s32 page_no ;
            //__here__;
            tmp = MBOOK_Decode_GetPageOffset(ebook_ctrl->mbook, ebook_ctrl->cur_page);

            if (EPDK_FAIL == tmp)
            {
                __wrn("get page offset fail");
                return EPDK_FAIL ;
            }

            page_no = MBOOK_Decode_GetPage(ebook_ctrl->mbook, tmp);
            __wrn("page_no=%d", page_no);
            // msg->dwAddData2 对应于第几个书签( 从0 开始算)
            __wrn("ebook_ctrl->book_bkpoint.bmk_numth=%d", ebook_ctrl->book_bkpoint.bmk_numth);
            ret = BOOK_Mark_SetPage(ebook_ctrl->mbmk, ebook_ctrl->book_bkpoint.bmk_numth + 1, tmp, NULL);
            ebook_ctrl->book_bkpoint.bmk_numth++;

            if (ebook_ctrl->book_bkpoint.bmk_numth == 7)
            {
                ebook_ctrl->book_bkpoint.bmk_numth = 1;
            }

            /*if( (EPDK_FAIL != ret) && ( EPDK_FAIL != page_no) )
            {
                eLIBs_uint2str_dec(page_no, str);
                __wrn("str=%s",str);
                eLIBs_strcpy(ebook_ctrl->book_bkpoint.bmk_page_no[0], (const char*)str);
                __wrn("ebook_ctrl->bmk_para->bmk_page_no[0]=%s",ebook_ctrl->book_bkpoint.bmk_page_no[0]);
            }*///..
            {
                H_WIN dlg;
                __s32 lang_id[2];//;={STRING_EBOOK_CUE, STRING_EBOOK_LABEL1};

                if (ebook_ctrl->book_bkpoint.bmk_numth == 1)
                {
                    lang_id[0] = STRING_EBOOK_CUE;
                    lang_id[1] = STRING_EBOOK_LABEL1;
                }
                else if (ebook_ctrl->book_bkpoint.bmk_numth == 2)
                {
                    lang_id[0] = STRING_EBOOK_CUE;
                    lang_id[1] = STRING_EBOOK_LABEL2;
                }
                else if (ebook_ctrl->book_bkpoint.bmk_numth == 3)
                {
                    lang_id[0] = STRING_EBOOK_CUE;
                    lang_id[1] = STRING_EBOOK_LABEL3;
                }
                else if (ebook_ctrl->book_bkpoint.bmk_numth == 4)
                {
                    lang_id[0] = STRING_EBOOK_CUE;
                    lang_id[1] = STRING_EBOOK_LABEL4;
                }
                else if (ebook_ctrl->book_bkpoint.bmk_numth == 5)
                {
                    lang_id[0] = STRING_EBOOK_CUE;
                    lang_id[1] = STRING_EBOOK_LABEL5;
                }
                else
                {
                    lang_id[0] = STRING_EBOOK_CUE;
                    lang_id[1] = STRING_EBOOK_LABEL6;
                }

                if (ebook_ctrl->book_bkpoint.bmk_numth == 6)
                {
                    ebook_ctrl->book_bkpoint.bmk_numth = 0;
                }

                if (ebook_ctrl->h_sset)
                {
                    SSET_Destroy(ebook_ctrl->h_sset);
                    ebook_ctrl->h_sset = 0;
                }

                default_dialog(dlg, msg->h_deswin, EBOOK_SBMK_ID, ADLG_OK, lang_id);
                esKRNL_TimeDly(100);
                app_dialog_destroy(dlg);
            }

            if (ebook_ctrl->pageLyr)//..
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
            }

            GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
            gscene_hbar_set_state(HBAR_ST_HIDE);  //..
            break;
        }

        case SSET_NC_STA_CHG:
        {
            __msg("sset send SSET_NC_STA_CHG to ebook main_win  ");

            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
            }

            if (reg_ebook_para->tts_switch  == TTS_OFF)
            {
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = ebook_ctrl->cur_page;
                setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                setmsg.dwAddData1 = 0;
                GUI_SendMessage(&setmsg);
            }
            else if (reg_ebook_para->tts_switch  == TTS_ON)
            {
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = ebook_ctrl->cur_page;
                setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                setmsg.dwAddData1 = 1;
                GUI_SendMessage(&setmsg);
            }

            gscene_hbar_set_state(HBAR_ST_HIDE) ;   //隐藏headbar

            if (ebook_ctrl->pageLyr)
            {
                __s32 ret ;
                ret = GUI_LyrWinSetSta(ebook_ctrl->pageLyr, GUI_LYRWIN_STA_ON);
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);

                if (EPDK_FAIL == ret)
                {
                    __msg(" pagelyr set status_on fail");
                }
            }

            /* 开始长字符串滚动*/
            {
                __gui_msg_t setmsg ;
                setmsg.id = COM_CMD_LONGSTRING_START;
                setmsg.h_deswin = ebook_ctrl->h_page;
                GUI_SendMessage(&setmsg);
            }
            MBOOK_TTS_court_page(ebook_ctrl ->mbook, (char **)&tts_page_data, &char_num);
            GUI_WinSetFocusChild(ebook_ctrl->h_page);
            break ;
        }

        default:
        {
            break;
        }
    }

    /*更新页数*/
    {
        __gui_msg_t setmsg;
        setmsg.id = COM_CMD_UPDATE_PAGE;
        setmsg.h_deswin = ebook_ctrl->h_page;
        setmsg.dwAddData1 = ebook_ctrl->cur_page;
        GUI_SendMessage(&setmsg);
    }
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               _MainProc
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/

static __s32 _app_ebook_Proc(__gui_msg_t  *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            ebook_ctrl_t *ebook_ctrl;
            __msg("******ebook create begin*******");
            g_disable_close_scn();

            if (0)
            {
                volatile DacFifoCtl_t *pDacFifoCtl  = (volatile DacFifoCtl_t *)(ADDA_o_ADC_FIFO_CTL + _ADDA_MEM_BASE);
                pDacFifoCtl->unRunSampSelBit = 1;
            }

            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __ebook_set_title(STRING_EBOOK_TITLE);          //设置headbar title
            gscene_hbar_set_state(HBAR_ST_HIDE);            //隐藏headbar
            gscene_bgd_set_state(BGD_STATUS_SHOW);
            ebook_ctrl_paint_init(ebook_ctrl);//..画背景底图
            reg_ebook_para = (reg_ebook_para_t *)dsk_reg_get_para_by_app(REG_APP_EBOOK);
            reg_ebook_para->text_colour = APP_COLOR_BLACK;
            //reg_ebook_para->text_colour = APP_COLOR_WHITE;
            //com_set_palette_by_id(ID_EBOOK_WKM_BMP);  //设置调色板
            {
                //背景色，文字编辑框底色
                //  __u32 color[]={BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR};
                //  com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
            }
            /*加载断点信息*/
            {
                ebook_tip_para_t tips_para;
                __s32 ret;
                RECT rect;
                ret = mbook_load_breakpoint_info(&ebook_ctrl->book_bkpoint);    //获取断点信息
                __wrn("mbook_load_breakpoint_info, ret=%d", ret);

                if (ret != 0)   //不存在断点信息
                {
                    __msg("mbook_load_breakpoint_info fail or there is no breakpoint info........");
                    ebook_ctrl->book_bkpoint.page_no = 0;
                    mbook_on_create(msg);
                    //__here__;
                }
                else
                {
                    //__msg("mbook_load_breakpoint_info succeed");
                    //if(ebook_ctrl->book_bkpoint.page_no>1)
                    //{
                    //  __s32 lang_id[]={ STRING_EBOOK_TIPS , STRING_EBOOK_BKPOINT };
                    //  default_dialog( ebook_ctrl->h_tips  , msg->h_deswin , EBOOK_TIPS_ID , ADLG_YESNO , lang_id ) ;
                    //
                    //}
                    //else
                    {
                        //  ebook_ctrl->book_bkpoint.page_no = 0;
                        __wrn("ebook_ctrl->book_bkpoint.page_no=%d", ebook_ctrl->book_bkpoint.page_no);
                        __wrn("ebook_ctrl->book_bkpoint.ebookname=%s", ebook_ctrl->book_bkpoint.ebookname);
                        mbook_on_create(msg);
                    }
                }
            }
            esKRNL_TimeDly(50);
            //app_misc_audio_mute(1);
            __msg("*********ebook create end************");
            return 0;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __bool  to_exist;//是否退出
            HRATNPL h_rat_npl;
            __s32   npl_index;
            char    file_path[768] = {0};
            char    diskname[2] = {0};
            ebook_ctrl_t *ebook_ctrl;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (!ebook_ctrl)
            {
                __msg("ebook_ctrl is null...");
                return EPDK_OK;
            }

            diskname[0] = (char)(msg->dwAddData2);
            diskname[1] = 0;
            to_exist = 1;//默认需要退出
            h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
            npl_index = rat_npl_get_cur(h_rat_npl);
            rat_npl_index2file(h_rat_npl, npl_index, file_path);
            rat_npl_close(h_rat_npl);

            if (0 != eLIBs_strnicmp(file_path, diskname, 1))
            {
                to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
            }

            __msg("******to_exist=%d******", to_exist);

            if (1 == to_exist) //需要退出
            {
                //__here__;
                Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
            }

            //__here__;
            return EPDK_OK;
        }

        case DSK_MSG_ALARM:
        {
            ebook_ctrl_t *ebook_ctrl;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("app_ebook:DSK_MSG_ALARM");

            if (!ebook_ctrl)
            {
                __msg("ebook_ctrl is null...");
                return EPDK_OK;
            }

            /* 闹钟到来先把sset给删除掉*/
            gscene_hbar_set_state(HBAR_ST_HIDE);

            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
            }

            if (ebook_ctrl->h_loading)
            {
                app_loading_win_delete(ebook_ctrl->h_loading) ;
                ebook_ctrl->h_loading = NULL ;
            }

            if (ebook_ctrl->h_tips)
            {
                //  mbook_on_create(msg);
                app_dialog_destroy(ebook_ctrl->h_tips) ;
                ebook_ctrl->h_tips = NULL ;
            }

            /* 如果音乐存在，先关掉音乐*/
            /*
            if( is_app_exist(APP_MUSIC) )
            {
                //关闭背景音乐
                __gui_msg_t sendmsg;

                sendmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                sendmsg.h_srcwin = NULL;
                sendmsg.id = GUI_MSG_COMMAND;
                sendmsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), CLOSE_BG_MUSIC);
                sendmsg.dwAddData2 = 0;
                sendmsg.dwReserved = 0;

                GUI_SendMessage(&sendmsg);
                ebook_ctrl->need_to_open_music = 1 ;
                reg_ebook_para->bg_music_switch = BG_MUSIC_CLOSE;
            }
            */
            if (reg_ebook_para->tts_switch  == TTS_ON)
            {
                __mbook_decode_t *mbook_decode_p;
                mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;

                if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                {
                    GUI_KillTimer(msg->h_deswin, ebook_timeid);
                }

                if (reg_ebook_para)
                {
                    reg_ebook_para->switch_time = 0;        //注册表记录
                }

                ebook_ctrl->auto_time = 0 ;     //停止自动播放

                if (msg->dwAddData2 == 0)
                {
                    if (ebook_ctrl->tts_has_start == EPDK_TRUE)     /*TTS  已经打开的情况下*/
                    {
                        __gui_msg_t setmsg;
                        TTS_stop_server();
                        dsk_keytone_set_state(SET_KEYTONE_ON);
                        ebook_ctrl->tts_has_init = EPDK_FALSE;
                        ebook_ctrl->tts_has_start = EPDK_FALSE;
                        setmsg.h_deswin = ebook_ctrl->h_page;
                        setmsg.dwAddData1 = ebook_ctrl->cur_page;
                        setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                        setmsg.h_deswin = ebook_ctrl->h_page;
                        setmsg.dwAddData1 = 0;
                        GUI_SendMessage(&setmsg);
                    }
                }
            }

            __wrn("app_ebook:before app_alarm_frm_create");
            ebook_ctrl->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
            __wrn("app_ebook:ebook_ctrl->h_frm_alarm_coming=%x", ebook_ctrl->h_frm_alarm_coming);
            //app_misc_audio_mute(1);
            return EPDK_OK ;
        }

        case ALARM_MSG_CLOSE:
        {
            ebook_ctrl_t *ebook_ctrl;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (!ebook_ctrl)
            {
                __msg("ebook_ctrl is null...");
                return EPDK_OK;
            }

            if (ebook_ctrl->h_frm_alarm_coming)
            {
                ebook_ctrl->h_frm_alarm_coming = NULL ;
            }

            /* 防止在sset弹出来时，闹铃时间到，把sset销毁，sset销毁后应该把page_lyr打开*/
            if (ebook_ctrl->pageLyr)
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
            }

            if (alarm_play_flag != 0)
            {
                if (reg_ebook_para->tts_switch  == TTS_ON)
                {
                    __mbook_decode_t *mbook_decode_p;
                    __gui_msg_t setmsg;
                    mbook_decode_p  = (__mbook_decode_t *) ebook_ctrl->mbook;

                    if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                    {
                        GUI_KillTimer(msg->h_deswin, ebook_timeid);
                    }

                    setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                    setmsg.h_deswin = ebook_ctrl->h_page;
                    setmsg.dwAddData1 = 1;
                    GUI_SendMessage(&setmsg);
                    {
                        TTS_play_server_init(mbook_decode_p->file_hdle, NULL);
                        ebook_ctrl->tts_has_init = EPDK_TRUE;
                    }

                    if (ebook_ctrl->tts_has_start == EPDK_FALSE)
                    {
                        MBOOK_TTS_court_page(ebook_ctrl->mbook, (char **)&tts_page_data, &char_num);
                        TTS_play_server_start(mbook_decode_p->file_hdle, tts_page_data, char_num, msg->h_deswin);
                        dsk_keytone_set_state(SET_KEYTONE_OFF);
                        ebook_ctrl->tts_has_start  =  EPDK_TRUE;
                    }
                }
                else
                {
                    __gui_msg_t setmsg;

                    if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
                    {
                        GUI_ResetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, 0);
                    }
                    else
                    {
                        GUI_SetTimer(msg->h_deswin, ebook_timeid, ebook_ctrl->auto_time, 0);
                    }

                    setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                    setmsg.h_deswin = ebook_ctrl->h_page;
                    setmsg.dwAddData1 = 1;
                }
            }
            else
            {
                __gui_msg_t setmsg;
                setmsg.id = COM_CMD_SET_PLAY_STATUS;    //设置暂停，播放状态
                setmsg.h_deswin = ebook_ctrl->h_page;
                setmsg.dwAddData1 = 0;
                GUI_SendMessage(&setmsg);
            }

            /*
            if( 1 == ebook_ctrl->need_to_open_music )   //背景音已经做了对闹铃来时的处理了，不需再判断
            {
                __gui_msg_t sendmsg;

                sendmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                sendmsg.h_srcwin = NULL;
                sendmsg.id = GUI_MSG_COMMAND;
                sendmsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), OPEN_BG_MUSIC);
                sendmsg.dwAddData2 = 0;
                sendmsg.dwReserved = 0;

                GUI_SendMessage(&sendmsg);

            }
            */
            GUI_WinSetFocusChild(msg->h_deswin);
            return EPDK_OK;
        }

        case DSK_MSG_HOME:
        case DSK_MSG_KILL:
        {
            ebook_ctrl_t *ebook_ctrl;
            __goto_create_para_t            *g_para = NULL ;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            g_para = (__goto_create_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (g_para->goto_hwin)
            {
                goto_frmwin_delete(g_para->goto_hwin);
            }

            if (!ebook_ctrl)
            {
                __msg("ebook_ctrl is null...");
                return EPDK_OK;
            }

            if (ebook_ctrl->h_sset)
            {
                SSET_Destroy(ebook_ctrl->h_sset);
                ebook_ctrl->h_sset = 0;
            }

            if (ebook_ctrl->pageLyr)
            {
                GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);
                GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
            }

            GUI_WinSetFocusChild(ebook_ctrl->h_page);
            Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
            break;
        }

        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
            return EPDK_OK;
            //Ebook_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, 0, 0);
            //break;
        }

        case GUI_MSG_DESTROY:
        {
            ebook_ctrl_t *ebook_ctrl;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __msg("*****ebook destroy begin********");

            if (!ebook_ctrl)
            {
                __err("ebook_ctrl is null ......");
                return EPDK_FAIL;
            }

            g_enable_standby();
            __wrn("ebook_ctrl->goto_lyr=====%d", ebook_ctrl->goto_lyr);

            if (ebook_ctrl->goto_lyr)
            {
                GUI_LyrWinDelete(ebook_ctrl->goto_lyr);
                ebook_ctrl->goto_lyr = NULL;
            }

            __wrn("ebook_ctrl->h_goto=====%d", ebook_ctrl->h_goto);

            if (ebook_ctrl->pageLyr)
            {
                GUI_LyrWinDelete(ebook_ctrl->pageLyr);
                ebook_ctrl->pageLyr = NULL;
            }

            if (ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle) //..
            {
                dsk_theme_close(ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle);
                ebook_ctrl->bottom_bg_res_bmp.h_bmp_handle = 0;
            }

            if (ebook_ctrl->bottom_bg_lyr) //..
            {
                GUI_LyrWinDelete(ebook_ctrl->bottom_bg_lyr);
                ebook_ctrl->bottom_bg_lyr = NULL;
            }

            if (ebook_ctrl->h_tips)
            {
                app_dialog_destroy(ebook_ctrl->h_tips);
                ebook_ctrl->h_tips = NULL;
            }

            //__here__;
            if (ebook_ctrl->h_frm_alarm_coming)
            {
                app_alarm_frm_destory(ebook_ctrl->h_frm_alarm_coming);
                ebook_ctrl->h_frm_alarm_coming = NULL ;
            }

            if (ebook_ctrl->tts_has_start == EPDK_TRUE)     /*TTS  已经打开的情况下*/
            {
                TTS_stop_server();
                dsk_keytone_set_state(SET_KEYTONE_ON);
                ebook_ctrl->tts_has_init = EPDK_FALSE;
                ebook_ctrl->tts_has_start = EPDK_FALSE;
            }

            //__here__;

            //__here__;
            if (ebook_ctrl->h_frm_alarm_coming)
            {
                app_alarm_frm_destory(ebook_ctrl->h_frm_alarm_coming);
                ebook_ctrl->h_frm_alarm_coming = NULL ;
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, ebook_timeid) == EPDK_TRUE)
            {
                GUI_KillTimer(msg->h_deswin, ebook_timeid);
            }

            //__here__;
            {
                HRATNPL     h_rat_npl;
                __s32       npl_index;
                char        file_path[768] = {0};
                char        *p_filename;
                h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
                npl_index = rat_npl_get_cur(h_rat_npl);
                rat_npl_index2file(h_rat_npl, npl_index, file_path);
                rat_npl_close(h_rat_npl);
                p_filename = file_path;//eLIBs_strchrlast(file_path,'\\') + 1;
                dsk_reg_save_cur_play_info(REG_APP_EBOOK, npl_index, p_filename, ebook_ctrl->root_type);
            }
            __msg("**********before mbook_on_delete********");
            mbook_on_delete(msg);

            //__here__;

            if (ebook_ctrl->sset_create_para)
            {
                My_Mfree(0, ebook_ctrl->sset_create_para);
                ebook_ctrl->sset_create_para = NULL;
            }

            {
                reg_root_para_t *root_reg_para;
                root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

                if (root_reg_para)
                {
                    root_reg_para->cur_play_storage = -1;
                }
                else
                {
                    __wrn("para is null...");
                }
            }

            if (!is_app_exist(APP_MUSIC))
            {
                dsk_amplifier_onoff(0);//关闭功放
            }
            else
            {
                dsk_amplifier_onoff(1);//打开功放
            }

            g_enable_close_scn();

            //如果没有后台音频，则允许自动关机
            if (EPDK_FALSE == is_app_exist(APP_MUSIC))
            {
                reg_system_para_t *para;
                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    dsk_set_auto_off_time(para->poweroff);
                    __wrn("para->poweroff=%d", para->poweroff);
                }
            }

            //dsk_reg_flush();
            __msg("*****ebook destroy end********");
            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            __wrn("----------命令的CMMD = %d", LOWORD(msg->dwAddData1));

            if (EBOOK_PAGE_ID == LOWORD(msg->dwAddData1))
            {
                page_info_proc(msg);
                mbook_save_breakpoint_info(msg);
                //dsk_reg_flush();  
            }
            else if (EBOOK_TIPS_ID == LOWORD(msg->dwAddData1))
            {
                ebook_tips_proc(msg);
            }
            else if (EBOOK_SSET_SCENE_ID == LOWORD(msg->dwAddData1))
            {
                ebook_sset_proc(msg);
            }
            else if (EBOOK_TTS_ID == LOWORD(msg->dwAddData1))
            {
                ebook_tts_proc(msg);
            }
            else if (EBOOK_GOTO_ID == LOWORD(msg->dwAddData1))
            {
                ebook_ctrl_t     *ebook_ctrl;
                ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                switch (HIWORD(msg->dwAddData1))
                {
                    case GOTO_ENTERF:
                    {
                        __wrn("goto msg->dwAddData2=%d", msg->dwAddData2);
                        goto_frmwin_delete(msg->h_srcwin);
                        ebook_ctrl->goto_lyr = NULL;

                        if (ebook_ctrl->pageLyr)//..
                        {
                            GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                            GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
                            //SEND_MSG(COM_CMD_UPDATE_PAGE, msg->h_deswin, ebook_ctrl->h_page, msg->dwAddData2, 0);
                        }

                        GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
                        gscene_hbar_set_state(HBAR_ST_HIDE);  //..

                        if (msg->dwAddData2 > ebook_ctrl->total_page)
                        {
                            H_WIN dlg;
                            __s32 lang_id[2] = {STRING_EBOOK_CUE, STRING_EBOOK_PAGEFULL};
                            default_dialog(dlg, msg->h_deswin, EBOOK_PAGEFULL_ID, ADLG_OK, lang_id);
                            esKRNL_TimeDly(150);
                            app_dialog_destroy(dlg);
                        }
                        else
                        {
                            ebook_ctrl->cur_page = MBOOK_Decode_SeekPage(ebook_ctrl->mbook, msg->dwAddData2);
                            NOTIFY_MSG(COM_CMD_UPDATE_PAGE, msg->h_deswin, ebook_ctrl->h_page, msg->dwAddData2, 0);//..
                        }

                        __wrn("goto msg->h_srcwin=%d", msg->h_srcwin);
                        break;
                    }

                    case GOTO_QUITF:
                    {
                        __wrn("goto msg->h_srcwin=%d", msg->h_srcwin);
                        goto_frmwin_delete(msg->h_srcwin);
                        ebook_ctrl->goto_lyr = NULL;

                        if (ebook_ctrl->pageLyr)//..
                        {
                            GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);   //..
                            GUI_LyrWinSetTop(ebook_ctrl->pageLyr);  //..
                            //SEND_MSG(COM_CMD_UPDATE_PAGE, msg->h_deswin, ebook_ctrl->h_page, msg->dwAddData2, 0);
                        }

                        GUI_WinSetFocusChild(ebook_ctrl->h_page);  //..
                        gscene_hbar_set_state(HBAR_ST_HIDE);  //..
                        break;
                    }
                }
            }

            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            static __s32 last_key = -1;
            ebook_ctrl_t *ebook_ctrl = NULL;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("aaaaa menulist longdown 9");

            if (ebook_ctrl->h_sset != NULL)
            {
                if (KEY_UP_ACTION == msg->dwAddData2)
                {
                    if (last_key == GUI_MSG_KEY_MENU || last_key == GUI_MSG_KEY_LONGMENU || last_key == GUI_MSG_KEY_ESCAPE || last_key == GUI_MSG_KEY_LONGESCAPE)
                    {
                        SSET_Destroy(ebook_ctrl->h_sset);
                        ebook_ctrl->h_sset = 0;

                        if (ebook_ctrl->pageLyr)
                        {
                            GUI_LyrWinSetSta(ebook_ctrl->pageLyr,  GUI_LYRWIN_STA_ON);
                            GUI_LyrWinSetTop(ebook_ctrl->pageLyr);
                        }

                        GUI_WinSetFocusChild(ebook_ctrl->h_page);
                        gscene_hbar_set_state(HBAR_ST_HIDE);
                        last_key = -1;
                        return EPDK_OK;
                    }

                    last_key = -1;
                }
                else
                {
                    if (msg->dwAddData1 == GUI_MSG_KEY_MENU || msg->dwAddData1 == GUI_MSG_KEY_LONGMENU || msg->dwAddData1 == GUI_MSG_KEY_ESCAPE || msg->dwAddData1 == GUI_MSG_KEY_LONGESCAPE)
                    {
                        last_key = msg->dwAddData1;
                        return EPDK_OK;
                    }
                }
            }
            else
            {
                __wrn("aaaaa menulist longdown 10");

                if (KEY_DOWN_ACTION == msg->dwAddData2 || KEY_REPEAT_ACTION == msg->dwAddData2)
                {
                    /* switch(msg->dwAddData1)
                        {
                            case GUI_MSG_KEY_VDEC:
                            case GUI_MSG_KEY_LONGVDEC:
                            {
                                 dsk_volume_dec();
                            }
                            break;
                            case GUI_MSG_KEY_VADD:
                            case GUI_MSG_KEY_LONGVADD:
                            {
                            }
                            break;
                        }*/
                }
            }
        }
        break;

        case GUI_MSG_TIMER:
        {
            ebook_ctrl_t     *ebook_ctrl;
            __gui_msg_t     setmsg;
            ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            ebook_ctrl->cur_page = MBOOK_Decode_ShowNext(ebook_ctrl->mbook);
            setmsg.id = COM_CMD_UPDATE_PAGE;
            setmsg.h_deswin = ebook_ctrl->h_page;
            setmsg.dwAddData1 = ebook_ctrl->cur_page;
            GUI_SendMessage(&setmsg);
            MBOOK_TTS_court_page(ebook_ctrl ->mbook, (char **)&tts_page_data, &char_num);

            if (ebook_ctrl->cur_page + 1 >= ebook_ctrl->total_page)
            {
                //如果没有后台音频，则允许自动关机
                if (EPDK_FALSE == is_app_exist(APP_MUSIC))
                {
                    reg_system_para_t *para;
                    para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                    if (para)
                    {
                        dsk_set_auto_off_time(para->poweroff);
                        __wrn("para->poweroff=%d", para->poweroff);
                    }
                }
            }

            break;
        }

        default:
            break;
    }

    return  GUI_ManWinDefaultProc(msg);
}

H_WIN app_ebook_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    ebook_ctrl_t *ebook_ctrl;
    H_WIN hManWin;
    __inf("****************************************************************************************");
    __inf("********  enter ebook app  **************");
    __inf("****************************************************************************************");
    //__wait__;
    g_disable_standby();
    ebook_ctrl = (ebook_ctrl_t *)My_Malloc(0, sizeof(ebook_ctrl_t));

    if (!ebook_ctrl)
    {
        __msg("memory alloc fail.........");
        return NULL;
    }

    eLIBs_memset(ebook_ctrl, 0, sizeof(ebook_ctrl_t));
    __wrn("ebook_ctrl->book_bkpoint.bmk_numth=%d", ebook_ctrl->book_bkpoint.bmk_numth);
    ebook_ctrl->ebook_font = para->font;
    ebook_ctrl->root_type = para->root_type;
    GUI_Memdev_FrameBuffer_Release(1);
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = APP_EBOOK;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_ebook_Proc);
    create_info.attr            = (void *)ebook_ctrl;
    create_info.id              = APP_EBOOK_ID;
    create_info.hHosting        = NULL;
    hManWin = GUI_ManWinCreate(&create_info);

    if (hManWin)
    {
        reg_root_para_t *root_reg_para;
        __s32 reg_storage_type;
        reg_storage_type = 0;

        if (para)
        {
            if (RAT_TF == para->root_type)
            {
                reg_storage_type = 0;
            }
            else if (RAT_USB == para->root_type)
            {
                reg_storage_type = 1;
            }
            else
            {
                __wrn("para->root_para->root_type invalid...");
            }
        }

        root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

        if (root_reg_para)
        {
            __s32 reg_app_type;
            reg_app_type = 2;
            root_reg_para->cur_play_storage = reg_storage_type;
            root_reg_para->last_play_app = reg_app_type;
            root_reg_para->last_app_play_storage[reg_app_type] = reg_storage_type;
            root_reg_para->last_storage_play_app[reg_storage_type] = reg_app_type;
        }
        else
        {
            __wrn("para is null...");
        }
    }

    return hManWin;
}

__s32 app_ebook_notify_delete_sub_scene(H_WIN hmanwin)
{
    ebook_ctrl_t *ebook_ctrl;

    if (NULL == hmanwin)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    ebook_ctrl = (ebook_ctrl_t *)GUI_WinGetAttr(hmanwin);

    if (!ebook_ctrl)
    {
        __wrn("ebook_ctrl is null...");
        return EPDK_FAIL;
    }

    if (ebook_ctrl->h_sset)
    {
        SSET_Destroy(ebook_ctrl->h_sset);
        ebook_ctrl->h_sset = 0;
    }

    gscene_hbar_set_state(HBAR_ST_HIDE);
    return EPDK_OK;
}

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
#include "listbar_private.h"
#include <kconfig.h>

typedef enum tag_DECODE_MODE
{
    DECODE_NULL             = 0,                    // 不解码任何页
    DECODE_CURR,                                    // 解码当前页
    DECODE_PREV,                                    // 解码当前页的前面页
    DECODE_NEXT,                                    // 解码所有页的后面页
    DECODE_ITEM,                                    // 只显示当前页
} __decode_mode_e;

typedef struct tag_ONE_PAGE
{
    __s32                   page_sta;               // 页面解码成功标示：1 成功、 0 失败
    __s32                   page_no;                // 缓存里的页面号
    H_WIN                   page_lyr;               // 缓存页对应的软图层
} __one_page_t;

typedef struct  tag_LISTBAR
{
    H_WIN                   fwin_hdle;              // 窗口操作句柄
    H_WIN                   lwin_hdle;              // MENU所在图层句柄

    __one_page_t            lbar_page[PAGE_NUM];    // 缓存页面
    // 控制部分
    RECT                    lbar_rect;              // MENU所在窗口区域

    __s32                   lbar_page_num;          // 单页面内总条目数
    __s32                   lbar_page_row;          // 行
    __s32                   lbar_page_col;          // 列
    __s32                   lbar_page_width;        // 有效页宽
    __s32                   lbar_page_height;       // 有效页高

    __s32                   lbar_current_move;      // 当前移动步长
    __s32                   lbar_current_page;      // 当前显示页面号
    __s32                   lbar_current_total;     // 当前总条目数
    __s32                   lbar_current_focus;     // 当前焦点条目号

    __s32                   lbar_item;              // 焦点条目在显示页面的编号，从0开始。
    __s32                   lbar_move;              // 页面移动指示

    __s32                   lbar_focus_sta;         // 按键消息状态位
    __s32                   lbar_touch_sta;         // 触摸消息状态位
    __pos_t                 lbar_last_pos;          // 触摸消息的位置
    __s32                   lbar_last_direct;       // 滑动的方向

    // 预解部分
    __s32                   lbar_cnt;               // 预解条目数
    __decode_mode_e         lbar_dec_state;         // 预解线程状态
    __u32                   lbar_dec_tsk;           // 预解线程
    __krnl_event_t         *lbar_dec_sem;           // 预解控制信号量

    // 扩散线程
    __s32                   lbar_dif_page[PAGE_NUM * ITEM_NUM];     // 存储待扩撒页
    __s32                   lbar_dif_num;           // 扩撒开始索引
    __u32                   lbar_dif_tsk;           // 预解线程
    __krnl_event_t         *lbar_dif_sem;           // 预解控制信号量

    // 绘制回调和配置参数
    H_WIN                   item_layer;             // item缓存图层
    __draw_item             lbar_draw;              // 绘制函数
    __listbar_config_t      config;                 // 配置信息

    // 滚动条参数
    H_WIN                   scroll_b_layer;         // 背景缓存图层
    H_WIN                   scroll_f_layer;         // 前景缓存图层
    __u32                   scroll_state;           // 滚动条状态
    __scroll_bar_t          scroll_para;            // 滚动条参数

    // 滑动线程
    __s32                   lbar_sld_speed;         // 滑动速度
    __s32                   lbar_sld_state;         // 滑动状态
    __u32                   lbar_sld_tsk;           // 滑动线程
    __krnl_event_t         *lbar_sld_sem;           // 滑动控制信号量
} __listbar_t;


/*
**********************************************************************************************************************
*                                               __listbar_layer_create
*
* Description:
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static H_WIN __listbar_layer_create(__u32 width, __u32 height, __s32 pipe)
{
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0xff,                                           /* alpha_val */
        1,                                              /* pipe      */
        10,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t create_info =
    {
        "layer",
        NULL,
        GUI_LYRWIN_STA_SLEEP,
        0
    };
    fb.size.width  = width;
    fb.size.height = height;
    para.pipe           = pipe;
    para.scn_win.x      = 0;
    para.scn_win.y      = 0;
    para.scn_win.width  = width;
    para.scn_win.height = height;
    para.src_win.x      = 0;
    para.src_win.y      = 0;
    para.src_win.width  = width;
    para.src_win.height = height;
    para.fb = &fb;
    create_info.lyrpara = &para;
    return (GUI_LyrWinCreate(&create_info));
}

/*
**********************************************************************************************************************
*                                               __listbar_brush_page
*
* Description: 刷新缓存中的页面数据
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_brush_page(__listbar_t *hdle, __decode_mode_e mode)
{
    __s32               i;
    __one_page_t        tmp_page;
    __listbar_t         *p_listbar;
    p_listbar = hdle;

    if (mode == DECODE_PREV)
    {
        tmp_page.page_no    = p_listbar->lbar_page[PAGE_NUM - 1].page_no;
        tmp_page.page_lyr   = p_listbar->lbar_page[PAGE_NUM - 1].page_lyr;

        for (i = PAGE_NUM - 1; i > 0; i--)
        {
            p_listbar->lbar_page[i].page_sta    = p_listbar->lbar_page[i - 1].page_sta;
            p_listbar->lbar_page[i].page_no     = p_listbar->lbar_page[i - 1].page_no;
            p_listbar->lbar_page[i].page_lyr    = p_listbar->lbar_page[i - 1].page_lyr;
        }

        p_listbar->lbar_page[0].page_sta    = 0;
        p_listbar->lbar_page[0].page_no     = tmp_page.page_no;
        p_listbar->lbar_page[0].page_lyr    = tmp_page.page_lyr;

        for (i = p_listbar->lbar_page_num * PAGE_NUM - 1; i >= p_listbar->lbar_page_num; i--)
        {
            p_listbar->lbar_dif_page[i] = p_listbar->lbar_dif_page[i - p_listbar->lbar_page_num];
        }

        for (i = p_listbar->lbar_page_num - 1; i >= 0; i--)
        {
            p_listbar->lbar_dif_page[i] = -1;
        }

        p_listbar->lbar_dif_num = p_listbar->lbar_page_num + p_listbar->lbar_item - 1;
    }
    else if (mode == DECODE_NEXT)
    {
        tmp_page.page_no    = p_listbar->lbar_page[0].page_no;
        tmp_page.page_lyr   = p_listbar->lbar_page[0].page_lyr;

        for (i = 0; i < PAGE_NUM - 1; i++)
        {
            p_listbar->lbar_page[i].page_sta    = p_listbar->lbar_page[i + 1].page_sta;
            p_listbar->lbar_page[i].page_no     = p_listbar->lbar_page[i + 1].page_no;
            p_listbar->lbar_page[i].page_lyr    = p_listbar->lbar_page[i + 1].page_lyr;
        }

        p_listbar->lbar_page[PAGE_NUM - 1].page_sta = 0;
        p_listbar->lbar_page[PAGE_NUM - 1].page_no  = tmp_page.page_no;
        p_listbar->lbar_page[PAGE_NUM - 1].page_lyr = tmp_page.page_lyr;

        for (i = 0; i < p_listbar->lbar_page_num * (PAGE_NUM - 1); i++)
        {
            p_listbar->lbar_dif_page[i] = p_listbar->lbar_dif_page[p_listbar->lbar_page_num + i];
        }

        for (i = p_listbar->lbar_page_num * (PAGE_NUM - 1); i < p_listbar->lbar_page_num * PAGE_NUM; i++)
        {
            p_listbar->lbar_dif_page[i] = -1;
        }

        p_listbar->lbar_dif_num = p_listbar->lbar_page_num + p_listbar->lbar_item - 1;
    }
    else if (mode == DECODE_CURR)
    {
        for (i = 0; i < PAGE_NUM; i++)
        {
            p_listbar->lbar_page[i].page_sta = 0;
            GUI_LyrWinSel(p_listbar->lbar_page[i].page_lyr);
            GUI_SetBkColor(p_listbar->config.bk_color);
            GUI_Clear();
        }

        for (i = 0; i < p_listbar->lbar_page_num * PAGE_NUM; i++)
        {
            p_listbar->lbar_dif_page[i] = -1;
        }

        p_listbar->lbar_dif_num = p_listbar->lbar_page_num + p_listbar->lbar_item - 1;
    }
}

/*
**********************************************************************************************************************
*                                               __listbar_show_scroll
*
* Description: 显示进度条
*
* Arguments  :
*
* Returns    :

* Notes      :      //modified by libaiao,2011.5.10
*
**********************************************************************************************************************
*/
static void __listbar_show_scroll(__listbar_t *hdle)
{
    __s32               i;
    FB                  fb;
    __s32               tmp, tmp1, tmp2, tmp_x, tmp_y, tmp_h;
    RECT                fb_rect1, fb_rect2, cp_rect1, cp_rect2;
    HBLOCK              l_block1;
    HBLOCK              l_block2;
    __listbar_t         *p_listbar;
    p_listbar           = hdle;
    //modified by libaiao,2011.5.10
    GUI_LyrWinSel(p_listbar->lwin_hdle);

    if (p_listbar->scroll_para.head_bmp != NULL)
    {
        tmp_x = p_listbar->scroll_para.show_rect.x;
        tmp_y = p_listbar->scroll_para.show_rect.y;
        GUI_ARGB_Draw(p_listbar->scroll_para.head_bmp, tmp_x, tmp_y);
    }

    if (p_listbar->scroll_para.back_bmp != NULL)
    {
        // 背景图标
        tmp_x = p_listbar->scroll_para.scroll_rect.x;
        tmp_y = p_listbar->scroll_para.scroll_rect.y;
        GUI_ARGB_Draw(p_listbar->scroll_para.back_bmp, tmp_x, tmp_y);
    }
    else
    {
        //GUI_SetBkColor(p_listbar->config.bk_color);
        //GUI_ClearRect);
        __msg("no bmp file for scrollbar!\n");
        return ;
    }

    if (p_listbar->scroll_para.tail_bmp != NULL)
    {
        tmp_x = p_listbar->scroll_para.show_rect.x;
        tmp_y = p_listbar->scroll_para.show_rect.y + p_listbar->scroll_para.show_rect.height - p_listbar->scroll_para.tail_height;
        GUI_ARGB_Draw(p_listbar->scroll_para.tail_bmp, tmp_x, tmp_y);
    }

    tmp_h = p_listbar->scroll_para.scroll_rect.height;

    if (p_listbar->lbar_current_total <= p_listbar->lbar_page_num)
    {
        // 不超过一屏数据
        tmp_x = p_listbar->scroll_para.scroll_rect.x;
        tmp_y = p_listbar->scroll_para.scroll_rect.y;

        for (i = 0; i < tmp_h; i++)
        {
            GUI_ARGB_Draw(p_listbar->scroll_para.body_bmp, tmp_x, tmp_y + i);//body 的高度必须为1
        }
    }
    else
    {
        tmp1  = p_listbar->lbar_page_num * p_listbar->scroll_para.scroll_rect.height / p_listbar->lbar_current_total;

        if (tmp1 < 1)
        {
            tmp1 = 1;
        }

        tmp   = p_listbar->lbar_current_move * p_listbar->scroll_para.scroll_rect.height /
                ((p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->config.item_height);

        if (p_listbar->lbar_current_move < 0)
        {
            tmp = 0;
        }
        else if (tmp > (p_listbar->scroll_para.scroll_rect.height - tmp1))
        {
            tmp = p_listbar->scroll_para.scroll_rect.height - tmp1;
        }

        tmp_x = p_listbar->scroll_para.scroll_rect.x;
        tmp_y = p_listbar->scroll_para.scroll_rect.y + tmp;
        tmp2  = tmp1;

        for (i = 0; i < tmp2; i++)
        {
            GUI_ARGB_Draw(p_listbar->scroll_para.body_bmp, tmp_x, tmp_y + i);
        }
    }
}

/*
**********************************************************************************************************************
*                                               __listbar_draw_page
*
* Description: 绘制一页
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_draw_page(__listbar_t *hdle)
{
    __u32                   i;
    __u32                   num;
    __s32                   tmp_page;
    __listbar_t             *p_listbar;
    __lbar_draw_para_t      draw_para;
    p_listbar = hdle;

    if (p_listbar->lbar_page[p_listbar->lbar_cnt].page_sta == 0)
    {
        tmp_page = p_listbar->lbar_current_page + (p_listbar->lbar_cnt - PAGE_NUM / 2) * p_listbar->lbar_page_num;
        GUI_LyrWinSel(p_listbar->lbar_page[p_listbar->lbar_cnt].page_lyr);
        GUI_SetBkColor(p_listbar->config.bk_color);
        GUI_Clear();

        if (tmp_page < 0)
        {
            p_listbar->lbar_page[p_listbar->lbar_cnt].page_no   = 0;
            p_listbar->lbar_page[p_listbar->lbar_cnt].page_sta  = 1;
        }
        else if (tmp_page >= p_listbar->lbar_current_total)
        {
            p_listbar->lbar_page[p_listbar->lbar_cnt].page_no   = 0;
            p_listbar->lbar_page[p_listbar->lbar_cnt].page_sta  = 1;
        }
        else
        {
            GUI_LyrWinSel(p_listbar->lbar_page[p_listbar->lbar_cnt].page_lyr);
            num = p_listbar->lbar_page_num * p_listbar->lbar_cnt;

            for (i = 0; i < p_listbar->lbar_page_num; i++)
            {
                p_listbar->lbar_dif_page[num + i] = tmp_page + i;       // 设置后台标志位
                draw_para.rect.x      = (i % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                draw_para.rect.y      = (i / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                draw_para.rect.width  = p_listbar->config.item_width;
                draw_para.rect.height = p_listbar->config.item_height;
                draw_para.index = tmp_page + i;
                draw_para.attr  = p_listbar->config.list_attr;

                if (draw_para.index >= p_listbar->lbar_current_total)
                {
                    // 当绘制条目号超过最大条目时，发空白绘制消息
                    draw_para.mode = LBAR_MODE_VACANT;
                    p_listbar->lbar_draw(&draw_para);
                }
                else
                {
                    // 发普通绘制消息
                    draw_para.mode = LBAR_MODE_NORMAL;
                    p_listbar->lbar_draw(&draw_para);
                }
            }

            GUI_LyrWinFlushFB(p_listbar->lbar_page[p_listbar->lbar_cnt].page_lyr);
            p_listbar->lbar_page[p_listbar->lbar_cnt].page_no   = tmp_page;
            p_listbar->lbar_page[p_listbar->lbar_cnt].page_sta  = 1;
        }
    }
}


/*
**********************************************************************************************************************
*                                               __listbar_draw_focus
*
* Description: 绘制一个焦点条目
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_draw_focus(__listbar_t *hdle)
{
    FB                      fb;
    RECT                    fb_rect;
    RECT                    cp_rect;
    HBLOCK                  l_block;
    __listbar_t             *p_listbar;
    __lbar_draw_para_t      draw_para;
    p_listbar = hdle;
    p_listbar->lbar_item = p_listbar->lbar_current_focus - p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col;

    if (p_listbar->lbar_item < 0)
    {
        p_listbar->lbar_item = 0;
    }

    if (p_listbar->lbar_focus_sta == 1)
    {
        p_listbar->lbar_focus_sta = 0;
        GUI_LyrWinSel(p_listbar->lwin_hdle);
        draw_para.rect.x        = p_listbar->lbar_rect.x + (p_listbar->lbar_item % p_listbar->lbar_page_col) * p_listbar->config.item_width;
        draw_para.rect.y        = p_listbar->lbar_rect.y + (p_listbar->lbar_item / p_listbar->lbar_page_col) * p_listbar->config.item_height;
        draw_para.rect.width    = p_listbar->config.item_width;
        draw_para.rect.height   = p_listbar->config.item_height;
        draw_para.index         = p_listbar->lbar_current_focus;
        draw_para.attr          = p_listbar->config.list_attr;
        draw_para.mode          = LBAR_MODE_FOCUS;
        p_listbar->lbar_draw(&draw_para);
    }
}

/*
**********************************************************************************************************************
*                                               __listbar_draw_miniature
*
* Description: 绘制一个当前焦点对应的缩略图( 对于图片才有效，故应用需作判断)
*           应用需判断该焦点缩略图是否已经画过了，以避免重复画此缩略图
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_draw_miniature(__listbar_t *hdle)
{
    __listbar_t            *p_listbar;
    __lbar_draw_para_t      draw_para;
    p_listbar = hdle;
    GUI_LyrWinSel(p_listbar->lwin_hdle);
    draw_para.index = p_listbar->lbar_current_focus;
    draw_para.attr  = p_listbar->config.list_attr;
    draw_para.mode = LBAR_MODE_MINIATURE;
    p_listbar->lbar_draw(&draw_para);
}

/*
**********************************************************************************************************************
*                                               __listbar_show_page
*
* Description: 显示一页
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_show_page(__listbar_t *hdle, __decode_mode_e mode)
{
    FB                  fb;
    FB                  fb2;
    RECT                fb_rect;
    RECT                cp_rect;
    HBLOCK              l_block;
    __listbar_t         *p_listbar = NULL;
    p_listbar = hdle;

    GUI_LyrWinCacheOn();
    __listbar_brush_page(p_listbar, mode);
    p_listbar->lbar_dec_state = mode;
    p_listbar->lbar_cnt = PAGE_NUM / 2;

    if (p_listbar->lbar_current_move < 0)
    {
        p_listbar->lbar_move = (0 - p_listbar->lbar_current_move) % p_listbar->lbar_page_height;
        __listbar_draw_page(p_listbar);
        GUI_LyrWinSel(p_listbar->lwin_hdle);
        GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2 - 1].page_lyr, &fb);
        // FB的指针下移到指定位置
        fb.addr[0]      = (void *)((unsigned long)fb.addr[0] + ((fb.size.height - p_listbar->lbar_move) * fb.size.width * 4));
        fb.size.width   = fb.size.width;
        fb.size.height  = p_listbar->lbar_move;
        GUI_BitString_DrawEx(&fb, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y);
        GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2].page_lyr, &fb2);
        fb2.size.width   = fb2.size.width;
        fb2.size.height  = fb2.size.height - p_listbar->lbar_move;
        GUI_BitString_DrawEx(&fb2, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y + fb.size.height);
    }
    else if (p_listbar->lbar_current_move > (p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->config.item_height)
    {
        p_listbar->lbar_move = p_listbar->lbar_current_move % p_listbar->lbar_page_height;
        __listbar_draw_page(p_listbar);
        GUI_LyrWinSel(p_listbar->lwin_hdle);
        GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2].page_lyr, &fb);
        // FB的指针下移到指定位置
        fb.addr[0]      = (void *)((unsigned long)fb.addr[0] + (p_listbar->lbar_move * fb.size.width * 4));
        fb.size.width   = fb.size.width;
        fb.size.height  = fb.size.height - p_listbar->lbar_move;
        GUI_BitString_DrawEx(&fb, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y);
        GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2 + 1].page_lyr, &fb2);
        fb2.size.width   = fb2.size.width;
        fb2.size.height  = p_listbar->lbar_move;
        GUI_BitString_DrawEx(&fb2, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y + fb.size.height);
    }
    else
    {
        p_listbar->lbar_move = p_listbar->lbar_current_move % p_listbar->lbar_page_height;

        if (p_listbar->lbar_move != 0)
        {
            __listbar_draw_page(p_listbar);
            p_listbar->lbar_cnt++;
            __listbar_draw_page(p_listbar);
        }
        else
        {
            __listbar_draw_page(p_listbar);
        }

        GUI_LyrWinSel(p_listbar->lwin_hdle);

        if (p_listbar->lbar_move == 0)
        {
            // 整页面显示
            GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2].page_lyr, &fb);
            GUI_BitString_DrawEx(&fb, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y);
        }
        else
        {
            GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2].page_lyr, &fb);
            // FB的指针下移到指定位置
            fb.addr[0]      = (void *)((unsigned long)fb.addr[0] + (p_listbar->lbar_move * fb.size.width * 4));
            fb.size.width   = fb.size.width;
            fb.size.height  = fb.size.height - p_listbar->lbar_move;
            GUI_BitString_DrawEx(&fb, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y);
            GUI_LyrWinGetFB(p_listbar->lbar_page[PAGE_NUM / 2 + 1].page_lyr, &fb2);
            fb2.size.width   = fb2.size.width;
            fb2.size.height  = p_listbar->lbar_move;
            GUI_BitString_DrawEx(&fb2, p_listbar->lbar_rect.x, p_listbar->lbar_rect.y + fb.size.height);
        }
    }

    // 绘制焦点项
    if (p_listbar->lbar_focus_sta == 1)
    {
        __listbar_draw_focus(p_listbar);
    }

    // 绘制进度条
    if (p_listbar->scroll_state == 1)
    {
        __listbar_show_scroll(p_listbar);
    }

    GUI_LyrWinFlushFB(p_listbar->lwin_hdle);
    GUI_LyrWinCacheOff();
}

/*
**********************************************************************************************************************
*                                               __listbar_move_page
*
* Description: 页面移动
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_move_page(__listbar_t *hdle, __s32 step)
{
    __u8                err;
    __listbar_t        *p_listbar;
    p_listbar = hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_current_move = p_listbar->lbar_current_move - step;

    if (p_listbar->lbar_current_move < 0)
    {
        if (p_listbar->lbar_current_move < (p_listbar->config.item_height - p_listbar->lbar_page_height))
        {
            if (p_listbar->lbar_sld_speed > 0)
            {
                p_listbar->lbar_sld_speed = 0;
            }

            p_listbar->lbar_current_move = p_listbar->config.item_height - p_listbar->lbar_page_height;
        }

        __listbar_show_page(p_listbar, DECODE_ITEM);
    }
    else if ((p_listbar->lbar_current_move + p_listbar->config.item_height)
             > ((p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->config.item_height))
    {
        if (p_listbar->lbar_sld_speed > 0)
        {
            p_listbar->lbar_sld_speed = 0;
        }

        p_listbar->lbar_current_move = (p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1)
                                       / p_listbar->lbar_page_col * p_listbar->config.item_height - p_listbar->config.item_height;

        if ((p_listbar->lbar_current_move - p_listbar->lbar_page_height) >= (p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height))
        {
            p_listbar->lbar_current_page = p_listbar->lbar_current_page + p_listbar->lbar_page_num;
            __listbar_show_page(p_listbar, DECODE_NEXT);
        }
        else if (p_listbar->lbar_current_move < (p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height))
        {
            p_listbar->lbar_current_page = p_listbar->lbar_current_page - p_listbar->lbar_page_num;
            __listbar_show_page(p_listbar, DECODE_PREV);
        }
        else
        {
            __listbar_show_page(p_listbar, DECODE_ITEM);
        }
    }
    else if ((p_listbar->lbar_current_move - p_listbar->lbar_page_height) >= (p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height))
    {
        p_listbar->lbar_current_page = p_listbar->lbar_current_page + p_listbar->lbar_page_num;
        __listbar_show_page(p_listbar, DECODE_NEXT);
    }
    else if (p_listbar->lbar_current_move < (p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height))
    {
        p_listbar->lbar_current_page = p_listbar->lbar_current_page - p_listbar->lbar_page_num;
        __listbar_show_page(p_listbar, DECODE_PREV);
    }
    else
    {
        __listbar_show_page(p_listbar, DECODE_ITEM);
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
}

/*
**********************************************************************************************************************
*                                               __listbar_decode_task
*
* Description: 预解线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_decode_task(void *p_arg)
{
    __u8                err;
    __listbar_t        *p_listbar;
    p_listbar = (__listbar_t *)p_arg;

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_DEC_TASK;
        }

        esKRNL_TimeDly(2);

        if (p_listbar->lbar_dec_state != DECODE_NULL)
        {
            if (p_listbar->lbar_page[p_listbar->lbar_cnt].page_sta == 0)
            {
                esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
                // 解一页内容
                __listbar_draw_page(p_listbar);
                // 页面计数器变化
                p_listbar->lbar_cnt++;

                if (p_listbar->lbar_cnt == PAGE_NUM)
                {
                    p_listbar->lbar_cnt = 0;
                }

                esKRNL_SemPost(p_listbar->lbar_dec_sem);
            }
            else
            {
                // 页面计数器变化
                p_listbar->lbar_cnt++;

                if (p_listbar->lbar_cnt == PAGE_NUM)
                {
                    p_listbar->lbar_cnt = 0;
                }
            }
        }
    }

EXIT_DEC_TASK:
    esKRNL_TDel(EXEC_prioself);
}

/*
**********************************************************************************************************************
*                                               __listbar_diffuse_task
*
* Description: 扩撒线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_diffuse_task(void *p_arg)
{
    __u8                err;
    __s32               tmp_cnt;
    __s32               tmp_num;
    __s32               tmp_page;
    OS_SEM_DATA         pdata;
    __listbar_t        *p_listbar;
    __lbar_draw_para_t  draw_para;
    p_listbar = (__listbar_t *)p_arg;

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_DIF_TASK;
        }

        esKRNL_TimeDly(1);
        esKRNL_SemQuery(p_listbar->lbar_dif_sem, &pdata);

        if (pdata.OSCnt == 1)
        {
            esKRNL_SemPend(p_listbar->lbar_dif_sem, 0, &err);

            if (p_listbar->lbar_dec_state != DECODE_NULL)
            {
                if (p_listbar->lbar_sld_state == 0)
                {
                    // 二次判断当前状态
                    esKRNL_SemQuery(p_listbar->lbar_dec_sem, &pdata);

                    if (pdata.OSCnt == 1)
                    {
                        // 扩撒中挂起预绘线程。
                        esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
                        tmp_cnt = p_listbar->lbar_dif_num / p_listbar->lbar_page_num;
                        tmp_page = p_listbar->lbar_dif_page[p_listbar->lbar_dif_num];

                        if ((tmp_page >= 0) && (tmp_page < p_listbar->lbar_current_total))
                        {
                            /* 在当前需要直接显示在屏幕上的范围内*/
                            if ((tmp_page >= (p_listbar->lbar_current_focus - p_listbar->lbar_item)) &&
                                (tmp_page <= (p_listbar->lbar_current_focus - p_listbar->lbar_item + p_listbar->lbar_page_num - 1)))
                            {
                                GUI_LyrWinSel(p_listbar->lwin_hdle);
                                tmp_num               = tmp_page - (p_listbar->lbar_current_focus - p_listbar->lbar_item);
                                draw_para.rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                                draw_para.rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                                draw_para.rect.width  = p_listbar->config.item_width;
                                draw_para.rect.height = p_listbar->config.item_height;
                                draw_para.index = tmp_page;
                                draw_para.attr  = p_listbar->config.list_attr;
                                draw_para.mode = LBAR_MODE_DIFFUSE;
                                p_listbar->lbar_draw(&draw_para);
                            }

                            /* 不在当前需要直接显示在屏幕上的范围内*/
                            GUI_LyrWinSel(p_listbar->lbar_page[tmp_cnt].page_lyr);
                            tmp_num = tmp_page % p_listbar->lbar_page_num;
                            draw_para.rect.x      = 0 + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                            draw_para.rect.y      = 0 + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                            draw_para.rect.width  = p_listbar->config.item_width;
                            draw_para.rect.height = p_listbar->config.item_height;
                            draw_para.index = tmp_page;
                            draw_para.attr  = p_listbar->config.list_attr;
                            draw_para.mode = LBAR_MODE_DIFFUSE;
                            p_listbar->lbar_draw(&draw_para);
                            p_listbar->lbar_dif_page[p_listbar->lbar_dif_num] = -1;
                        }

                        p_listbar->lbar_dif_num++;

                        if (p_listbar->lbar_dif_num >= (p_listbar->lbar_page_num * PAGE_NUM))
                        {
                            p_listbar->lbar_dif_num = 0;
                        }

                        __listbar_draw_miniature(p_listbar);    //////
                        esKRNL_SemPost(p_listbar->lbar_dec_sem);
                    }
                }
            }

            esKRNL_SemPost(p_listbar->lbar_dif_sem);
        }
    }

EXIT_DIF_TASK:
    esKRNL_TDel(EXEC_prioself);
}

/*
**********************************************************************************************************************
*                                               __listbar_slider_task
*
* Description: 滑动线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __listbar_slider_task(void *p_arg)
{
    __u8                err;
    __s32               tmp = 0;
    __s32               tmp_height = 0;
    __s32               tmp_number = -1;
    OS_SEM_DATA         pdata;
    __listbar_t         *p_listbar;
    p_listbar = (__listbar_t *)p_arg;

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_SLD_TASK;
        }

        esKRNL_SemQuery(p_listbar->lbar_sld_sem, &pdata);

        if (pdata.OSCnt == 1)
        {
            if (p_listbar->lbar_sld_speed >= 0)
            {
                esKRNL_SemPend(p_listbar->lbar_sld_sem, 0, &err);

                if (p_listbar->lbar_sld_speed == 0)
                {
                    // 速度为0时，进行微调。
                    if (tmp_height == 0)
                    {
                        tmp_height = (p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1)
                                     / p_listbar->lbar_page_col * p_listbar->config.item_height;
                    }

                    if (p_listbar->lbar_current_move < 0)
                    {
                        // 上方向溢出
                        if (tmp_number == -1)
                        {
                            tmp_number = (0 - p_listbar->lbar_current_move) / MAX_GLIDE_STEP;
                        }

                        if (tmp_number)
                        {
                            tmp_number--;
                            __listbar_move_page(p_listbar, 0 - MAX_GLIDE_STEP);
                        }
                        else
                        {
                            tmp_number--;
                            p_listbar->lbar_current_move = 0;
                            __listbar_move_page(p_listbar, 0);
                        }
                    }
                    else if (p_listbar->lbar_current_move >= (tmp_height - p_listbar->lbar_page_height))
                    {
                        // 下方向溢出
                        if ((tmp_height - p_listbar->lbar_page_height) < 0)
                        {
                            if (tmp_number == -1)
                            {
                                tmp_number = p_listbar->lbar_current_move / MAX_GLIDE_STEP;
                            }

                            if (tmp_number)
                            {
                                tmp_number--;
                                __listbar_move_page(p_listbar, MAX_GLIDE_STEP);
                            }
                            else
                            {
                                tmp_number--;
                                p_listbar->lbar_current_move = 0;
                                __listbar_move_page(p_listbar, 0);
                            }
                        }
                        else
                        {
                            if (tmp_number == -1)
                            {
                                tmp_number = (p_listbar->lbar_current_move - (tmp_height - p_listbar->lbar_page_height)) / MAX_GLIDE_STEP;
                            }

                            if (tmp_number)
                            {
                                tmp_number--;
                                __listbar_move_page(p_listbar, MAX_GLIDE_STEP);
                            }
                            else
                            {
                                tmp_number--;
                                p_listbar->lbar_current_move = tmp_height - p_listbar->lbar_page_height;
                                __listbar_move_page(p_listbar, 0);
                            }
                        }
                    }
                    else
                    {
                        if (tmp == 0)
                        {
                            tmp = p_listbar->lbar_current_move % p_listbar->config.item_height;
                        }

                        if (tmp != 0)
                        {
                            if (p_listbar->lbar_last_direct > 0)
                            {
                                if (tmp_number == -1)
                                {
                                    tmp_number = tmp / (p_listbar->config.item_height / 5);
                                }

                                if (tmp_number)
                                {
                                    tmp_number--;
                                    __listbar_move_page(p_listbar, p_listbar->config.item_height / 5);
                                }
                                else
                                {
                                    tmp_number--;
                                    p_listbar->lbar_current_move = p_listbar->lbar_current_move
                                                                   / p_listbar->config.item_height * p_listbar->config.item_height;
                                    __listbar_move_page(p_listbar, 0);
                                }
                            }
                            else if (p_listbar->lbar_last_direct < 0)
                            {
                                if (tmp_number == -1)
                                {
                                    tmp_number = (p_listbar->config.item_height - tmp) / (p_listbar->config.item_height / 5);
                                }

                                if (tmp_number)
                                {
                                    tmp_number--;
                                    __listbar_move_page(p_listbar, (0 - p_listbar->config.item_height / 5));
                                }
                                else
                                {
                                    tmp_number--;
                                    p_listbar->lbar_current_move = (p_listbar->lbar_current_move + p_listbar->config.item_height / 2)
                                                                   / p_listbar->config.item_height * p_listbar->config.item_height;
                                    __listbar_move_page(p_listbar, 0);
                                }
                            }
                            else
                            {
                                tmp_number = -1;
                            }
                        }
                        else
                        {
                            tmp_number = -1;
                        }
                    }

                    if (tmp_number == -1)
                    {
                        // 滑动完成
                        tmp = 0;
                        tmp_height = 0;
                        tmp_number = -1;
                        p_listbar->lbar_sld_speed = -1;

                        if (p_listbar->lbar_focus_sta == 0)
                        {
                            p_listbar->lbar_focus_sta = 1;
                            p_listbar->lbar_current_focus = p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col;
                            __listbar_draw_focus(p_listbar);
                        }
                    }
                }
                else
                {
                    // 根据滑动速度进行惯性滑动。
                    if (p_listbar->lbar_last_direct > 0)
                    {
                        if (p_listbar->lbar_sld_speed)
                        {
                            p_listbar->lbar_sld_speed--;
                            __listbar_move_page(p_listbar, MAX_GLIDE_STEP);
                        }
                    }
                    else if (p_listbar->lbar_last_direct < 0)
                    {
                        if (p_listbar->lbar_sld_speed)
                        {
                            p_listbar->lbar_sld_speed--;
                            __listbar_move_page(p_listbar, 0 - MAX_GLIDE_STEP);
                        }
                    }
                    else
                    {
                        p_listbar->lbar_sld_speed = 0;
                    }

                    if (p_listbar->lbar_sld_speed == 0)
                    {
                        tmp = 0;
                        tmp_height = 0;
                        tmp_number = -1;
                    }
                }

                esKRNL_SemPost(p_listbar->lbar_sld_sem);

                if (p_listbar->lbar_sld_speed == 0)
                {
                    esKRNL_TimeDly(4);
                }
                else if (p_listbar->lbar_sld_speed < 6)
                {
                    esKRNL_TimeDly(2);
                }
                else
                {
                    esKRNL_TimeDly(1);
                }
            }
            else
            {
                if (p_listbar->lbar_sld_state == -1)
                {
                    // 挂起扩撒线程
                    p_listbar->lbar_sld_state = 0;
                }

                tmp = 0;
                tmp_height = 0;
                tmp_number = -1;
                esKRNL_TimeDly(5);
            }
        }
        else
        {
            tmp = 0;
            tmp_height = 0;
            tmp_number = -1;
            esKRNL_TimeDly(5);
        }
    }

EXIT_SLD_TASK:
    esKRNL_TDel(EXEC_prioself);
}

/*
**********************************************************************************************************************
*                                               LISTBAR_Create
*
* Description: 创建一个控制操作句柄
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
H_LBAR  LISTBAR_Create(H_WIN hwin)
{
    __listbar_t        *p_listbar;
    p_listbar = (__listbar_t *)esMEMS_Malloc(0, sizeof(__listbar_t));   // 申请内存

    if (p_listbar == NULL)
    {
        __err(" frame ctrl malloc error \n");
        return NULL;
    }

    eLIBs_memset(p_listbar, 0, sizeof(__listbar_t));
    p_listbar->fwin_hdle = hwin;
    // 获取当前操作图层句柄
    p_listbar->lwin_hdle = GUI_WinGetLyrWin(hwin);
    p_listbar->lbar_dec_state   = DECODE_NULL;              // 解码状态位
    p_listbar->lbar_cnt         = PAGE_NUM / 2;             // 解码页数计数
    // 创建后台预绘线程
    p_listbar->lbar_dec_sem     = esKRNL_SemCreate(1);      // 预绘控制信号量
    p_listbar->lbar_dec_tsk     = esKRNL_TCreate(__listbar_decode_task, (void *)p_listbar, 0x8000, KRNL_priolevel4);    // 后台预绘线程
    // 创建后台滑动线程
    p_listbar->lbar_sld_speed   = -1;
    p_listbar->lbar_sld_state   = 0;
    p_listbar->lbar_sld_sem     = esKRNL_SemCreate(1);      // 滑动控制信号量
    p_listbar->lbar_sld_tsk     = esKRNL_TCreate(__listbar_slider_task, (void *)p_listbar, 0x8000, KRNL_priolevel5);
    // 创建后台扩撒线程
    p_listbar->lbar_dif_sem     = esKRNL_SemCreate(1);      // 扩散控制信号量
    p_listbar->lbar_dif_tsk     = esKRNL_TCreate(__listbar_diffuse_task, (void *)p_listbar, 0x8000, KRNL_priolevel5);   // 显示扩散线程
    __msg("task 1 = %d, task2 = %d, task3 = %d \n", p_listbar->lbar_sld_tsk, p_listbar->lbar_dec_tsk, p_listbar->lbar_dif_tsk);
    return p_listbar;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_Config
*
* Description: 配置listbar相关属性
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   LISTBAR_Config(H_LBAR hdle, __draw_item draw_cb, __listbar_config_t *config)
{
    __u32               i;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    p_listbar->lbar_draw = draw_cb;
    __wrn("11111111111..\n");
    eLIBs_memcpy(&p_listbar->config, config, sizeof(__listbar_config_t));
    __wrn("22222222222..\n");
    p_listbar->lbar_rect = config->list_rect;
    __wrn("x = %d, y = %d, w = %d, h = %d \n", config->list_rect.x, config->list_rect.y, config->list_rect.width, config->list_rect.height);
    __wrn("item widht = %d, item height = %d \n", config->item_width, config->item_height);
    p_listbar->item_layer = __listbar_layer_create(p_listbar->config.item_width, p_listbar->config.item_height, 0);
    __wrn("3333333333..\n");

    for (i = 0; i < PAGE_NUM; i++)
    {
        p_listbar->lbar_page[i].page_lyr = __listbar_layer_create(p_listbar->lbar_rect.width, p_listbar->lbar_rect.height, 0);
        GUI_LyrWinSel(p_listbar->lbar_page[i].page_lyr);
        GUI_SetLCDDirection(GUI_SCNDIR_NORMAL);
        __wrn("page i =%d\n", i);
    }

    __wrn("4444444444444..\n");
    p_listbar->lbar_page_col            = p_listbar->lbar_rect.width / config->item_width;
    p_listbar->lbar_page_row            = p_listbar->lbar_rect.height / config->item_height;
    p_listbar->lbar_page_num            = p_listbar->lbar_page_row * p_listbar->lbar_page_col;
    p_listbar->lbar_page_width          = p_listbar->lbar_page_col * config->item_width;
    p_listbar->lbar_page_height         = p_listbar->lbar_page_row * config->item_height;

    if (config->start_id == -1)
    {
        p_listbar->lbar_current_page    = config->focus_id / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
        p_listbar->lbar_current_focus   = config->focus_id;
        p_listbar->lbar_current_move    = p_listbar->lbar_current_page / p_listbar->lbar_page_col * config->item_height;
        p_listbar->lbar_current_total   = config->item_cnt;
    }
    else
    {
        p_listbar->lbar_current_page    = config->start_id / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
        p_listbar->lbar_current_focus   = config->focus_id;
        p_listbar->lbar_current_move    = config->start_id / p_listbar->lbar_page_col * config->item_height;
        p_listbar->lbar_current_total   = config->item_cnt;
    }

    p_listbar->lbar_focus_sta   = 1;
    p_listbar->lbar_touch_sta   = 0;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_ScrollConfig
*
* Description: 配置listbar相关属性
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
__s32   LISTBAR_ScrollConfig(H_LBAR hdle, __scroll_bar_t *scroll_para)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    eLIBs_memcpy(&p_listbar->scroll_para, scroll_para, sizeof(__scroll_bar_t));
    p_listbar->scroll_state     = 1;
    p_listbar->scroll_b_layer   = __listbar_layer_create(p_listbar->scroll_para.show_rect.width, p_listbar->scroll_para.show_rect.height, 0);
    p_listbar->scroll_f_layer   = __listbar_layer_create(p_listbar->scroll_para.show_rect.width, p_listbar->scroll_para.show_rect.height, 0);
    GUI_LyrWinSel(p_listbar->scroll_b_layer);

    if (p_listbar->scroll_para.back_bmp != NULL)
    {
        // 背景图标
        GUI_ARGB_Draw(p_listbar->scroll_para.back_bmp, 0, 0);
    }
    else
    {
        GUI_SetBkColor(p_listbar->config.bk_color);
        GUI_Clear();
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_ShowPage
*
*Description: 显示当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_ShowPage(H_LBAR hdle)
{
    __u8                err;
    __listbar_t         *p_listbar = NULL;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta = 1;
    __listbar_show_page(p_listbar, DECODE_CURR);
    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_NextPage
*
*Description: 向下翻一页
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_NextPage(H_LBAR hdle)
{
    __u8                err;
    __listbar_t         *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta   = 1;

    if (p_listbar->lbar_current_total > p_listbar->lbar_page_num)
    {
        p_listbar->lbar_current_page        = p_listbar->lbar_current_focus;   //add by libiaoao,2011.4.26
        p_listbar->lbar_current_page        = p_listbar->lbar_current_page + p_listbar->lbar_page_num;

        if (p_listbar->lbar_current_page >= p_listbar->lbar_current_total)
        {
            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_page;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_CURR);
        }
        else
        {
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_page;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_NEXT);
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_PrevPage
*
*Description: 向上翻一页
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_PrevPage(H_LBAR hdle)
{
    __u8                err;
    __listbar_t         *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta   = 1;

    if (p_listbar->lbar_current_total > p_listbar->lbar_page_num)
    {
        p_listbar->lbar_current_page = p_listbar->lbar_current_focus;

        if (p_listbar->lbar_current_page < p_listbar->lbar_page_num)
        {
            return EPDK_FAIL;   //不向前翻页
        }

        p_listbar->lbar_current_page = p_listbar->lbar_current_page - p_listbar->lbar_page_num;

        if (p_listbar->lbar_current_page < p_listbar->lbar_page_num)
        {
            p_listbar->lbar_current_page = 0;
        }

        if (p_listbar->lbar_current_page < 0)
        {
            p_listbar->lbar_current_page = (p_listbar->lbar_current_total - 1) / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_page;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_CURR);
        }
        else
        {
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_page;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_PREV);
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_NextItem
*
*Description: 下移个条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_NextItem(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta   = 1;
    // 焦点下移一个条目
    p_listbar->lbar_current_focus++;

    if (p_listbar->lbar_current_focus >= p_listbar->lbar_current_total)
    {
        // 已是最后一个条目，跳转到第一个条目
        if (p_listbar->lbar_current_total <= p_listbar->lbar_page_num)
        {
            p_listbar->lbar_current_focus   = 0;
            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_move    = 0;
            __listbar_show_page(p_listbar, DECODE_ITEM);
        }
        else
        {
            p_listbar->lbar_current_focus   = 0;
            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_move    = 0;
            __listbar_show_page(p_listbar, DECODE_CURR);
        }
    }
    else
    {
        if (p_listbar->lbar_current_focus >= (p_listbar->lbar_current_page + p_listbar->lbar_page_num + p_listbar->lbar_page_num - p_listbar->lbar_page_col))
        {
            p_listbar->lbar_current_page    = p_listbar->lbar_current_page + p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_move + p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_NEXT);
        }
        else
        {
            if (p_listbar->lbar_item < (p_listbar->lbar_page_num - 1))
            {
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
            else
            {
                if (p_listbar->lbar_page_col == 1)
                {
                    p_listbar->lbar_current_move    = p_listbar->lbar_current_move + p_listbar->config.item_height;
                }
                else
                {
                    if ((p_listbar->lbar_current_focus % p_listbar->lbar_page_col) == 0)
                    {
                        p_listbar->lbar_current_move    = p_listbar->lbar_current_move + p_listbar->config.item_height;
                    }
                }

                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_PrevItem
*
*Description: 上移个条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_PrevItem(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta   = 1;
    // 焦点上移一个条目
    p_listbar->lbar_current_focus--;

    if (p_listbar->lbar_current_focus < 0)
    {
        // 已是第一个条目，跳转到最后一个条目
        if (p_listbar->lbar_current_total <= p_listbar->lbar_page_num)
        {
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_total - 1;
            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_move    = 0;
            __listbar_show_page(p_listbar, DECODE_ITEM);
        }
        else
        {
#ifdef CONFIG_SOC_SUN3IW1

            if (1 == p_listbar->lbar_page_col)
            {
#endif
                p_listbar->lbar_current_focus   = p_listbar->lbar_current_total - 1;
                p_listbar->lbar_current_page    = ((p_listbar->lbar_current_total - p_listbar->lbar_page_num) / p_listbar->lbar_page_num) * p_listbar->lbar_page_num;
                p_listbar->lbar_current_move    = (p_listbar->lbar_current_total - p_listbar->lbar_page_num + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->config.item_height;
#ifdef CONFIG_SOC_SUN3IW1
            }
            else
            {
                p_listbar->lbar_current_focus   = p_listbar->lbar_current_total - 1;

                if (p_listbar->lbar_current_total <= 0)
                {
                    p_listbar->lbar_current_page = 0;
                }
                else
                {
                    p_listbar->lbar_current_page    = ((p_listbar->lbar_current_total - 1) / p_listbar->lbar_page_num) * p_listbar->lbar_page_num;
                }

                p_listbar->lbar_current_move    = (p_listbar->lbar_current_page) / p_listbar->lbar_page_col * p_listbar->config.item_height;
            }

#endif
            __listbar_show_page(p_listbar, DECODE_CURR);
        }
    }
    else
    {
        if (p_listbar->lbar_current_focus < p_listbar->lbar_current_page)
        {
            p_listbar->lbar_current_page    = p_listbar->lbar_current_page - p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_move - p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_PREV);
        }
        else
        {
            if (p_listbar->lbar_item > 0)
            {
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
            else
            {
                if (p_listbar->lbar_page_col == 1)
                {
                    p_listbar->lbar_current_move    = p_listbar->lbar_current_move - p_listbar->config.item_height;
                }
                else
                {
                    if ((p_listbar->lbar_current_focus % p_listbar->lbar_page_col) == (p_listbar->lbar_page_col - 1))
                    {
                        p_listbar->lbar_current_move    = p_listbar->lbar_current_move - p_listbar->config.item_height;
                    }
                }

                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

__s32   LISTBAR_UpdateCurItem(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    // 刷新预绘线程，并且刷新页面显示
    p_listbar->lbar_focus_sta   = 1;
    __listbar_show_page(p_listbar, DECODE_ITEM);
    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}



__s32   LISTBAR_UpdateFocusItem(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        eLIBs_printf("LISTBAR_UpdateFocusItem===========NULL\n");
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    eLIBs_printf("LISTBAR_UpdateFocusItem===========%s %d\n", __FUNCTION__, __LINE__);
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    // 刷新预绘线程，并且刷新页面显示
    p_listbar->lbar_focus_sta   = 1;
    eLIBs_printf("LISTBAR_UpdateFocusItem======1=====%s %d\n", __FUNCTION__, __LINE__);

    // __listbar_show_page(p_listbar, DECODE_ITEM);
    // 绘制焦点项
    if (p_listbar->lbar_focus_sta == 1)
    {
        eLIBs_printf("LISTBAR_UpdateFocusItem=========2==%s %d\n", __FUNCTION__, __LINE__);
        __listbar_draw_focus(p_listbar);
    }

    eLIBs_printf("LISTBAR_UpdateFocusItem====3=======%s %d\n", __FUNCTION__, __LINE__);
    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_NextRow
*
*Description: 向下移动(一行)
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_NextRow(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta   = 1;
    // 焦点下移一行条目
    p_listbar->lbar_current_focus = p_listbar->lbar_current_focus + p_listbar->lbar_page_col;

    if (p_listbar->lbar_current_focus >= p_listbar->lbar_current_total)
    {
        // 已是最后一个条目，跳转到第一个条目
        if (p_listbar->lbar_current_total <= p_listbar->lbar_page_num)
        {
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_focus % p_listbar->lbar_page_col;
            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_move    = 0;
            __listbar_show_page(p_listbar, DECODE_ITEM);
        }
        else
        {
            p_listbar->lbar_current_focus   = p_listbar->lbar_current_focus % p_listbar->lbar_page_col;
            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_move    = 0;
            __listbar_show_page(p_listbar, DECODE_CURR);
        }
    }
    else
    {
        if (p_listbar->lbar_current_focus >= (p_listbar->lbar_current_page + p_listbar->lbar_page_num + p_listbar->lbar_page_num - 1))
        {
            p_listbar->lbar_current_page    = p_listbar->lbar_current_page + p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_move + p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_NEXT);
        }
        else
        {
            if (p_listbar->lbar_item < (p_listbar->lbar_page_num - p_listbar->lbar_page_col))
            {
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
            else
            {
                p_listbar->lbar_current_move    = p_listbar->lbar_current_move + p_listbar->config.item_height;
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_PrevRow
*
*Description: 向上移动(一行)
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
2************************************************************************************************************************
*/
__s32   LISTBAR_PrevRow(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    p_listbar->lbar_focus_sta   = 1;
    // 焦点上移一个条目
    p_listbar->lbar_current_focus = p_listbar->lbar_current_focus - p_listbar->lbar_page_col;

    if (p_listbar->lbar_current_focus < 0)
    {
        // 已是第一个条目，跳转到最后一个条目
        if (p_listbar->lbar_current_total <= p_listbar->lbar_page_num)
        {
            p_listbar->lbar_current_focus   = (p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->lbar_page_col
                                              + p_listbar->lbar_current_focus;

            if (p_listbar->lbar_current_focus >= p_listbar->lbar_current_total)
            {
                p_listbar->lbar_current_focus = p_listbar->lbar_current_focus - p_listbar->lbar_page_col;
            }

            p_listbar->lbar_current_page    = 0;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_page / p_listbar->lbar_page_col * p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_ITEM);
        }
        else
        {
            p_listbar->lbar_current_focus   = (p_listbar->lbar_current_total + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->lbar_page_col
                                              + p_listbar->lbar_current_focus;

            if (p_listbar->lbar_current_focus >= p_listbar->lbar_current_total)
            {
                p_listbar->lbar_current_focus = p_listbar->lbar_current_focus - p_listbar->lbar_page_col;
            }

            p_listbar->lbar_current_page    = ((p_listbar->lbar_current_focus - p_listbar->lbar_page_num) / p_listbar->lbar_page_num) * p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = (p_listbar->lbar_current_total - p_listbar->lbar_page_num + p_listbar->lbar_page_col - 1) / p_listbar->lbar_page_col * p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_CURR);
        }
    }
    else
    {
        if (p_listbar->lbar_current_focus < p_listbar->lbar_current_page)
        {
            p_listbar->lbar_current_page    = p_listbar->lbar_current_page - p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = p_listbar->lbar_current_move - p_listbar->config.item_height;
            __listbar_show_page(p_listbar, DECODE_PREV);
        }
        else
        {
            if (p_listbar->lbar_item >= p_listbar->lbar_page_col)
            {
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
            else
            {
                p_listbar->lbar_current_move    = p_listbar->lbar_current_move - p_listbar->config.item_height;
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_TouchDown
*
*Description: 触摸按下
*
*Arguments  : hdle: 操作句柄， x: X点坐标， y：Y点坐标 (x, y 是否有效需判断)
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_TouchDown(H_LBAR hdle, __s32 x, __s32 y, __s32 direct, __s32 speed)
{
    __u8                err;
    __s32               tmp;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if (p_listbar->lbar_current_total == 0)
    {
        // 总条目数为空时，不作响应。
        return EPDK_OK;
    }

    if ((x < p_listbar->config.list_rect.x) || (y < p_listbar->config.list_rect.y)
        || (x > (p_listbar->config.list_rect.x + p_listbar->config.list_rect.width))
        || (y > (p_listbar->config.list_rect.y + p_listbar->config.list_rect.height)))
    {
        // 坐标不合法
        p_listbar->lbar_touch_sta = -1;
        return EPDK_FAIL;
    }

    esKRNL_SemPend(p_listbar->lbar_sld_sem, 0, &err);

    if (p_listbar->lbar_sld_state == 0)
    {
        // 挂起扩撒线程
        p_listbar->lbar_sld_state = 1;
    }

    if (p_listbar->lbar_sld_speed == -1)
    {
        // 当前处于静止状态
        p_listbar->lbar_last_pos.x  = x;
        p_listbar->lbar_last_pos.y  = y;

        if (p_listbar->lbar_touch_sta == 0)
        {
            // 按下绘制焦点状态
            esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
            p_listbar->lbar_touch_sta   = 1;
            tmp = p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col
                  + ((x - p_listbar->config.list_rect.x) / p_listbar->config.item_width)
                  + ((y - p_listbar->config.list_rect.y) / p_listbar->config.item_height * p_listbar->lbar_page_col);

            if (tmp < p_listbar->lbar_current_total)
            {
                // 点到条目内部
                p_listbar->lbar_focus_sta   = 1;
                p_listbar->lbar_current_focus = tmp;
                __listbar_show_page(p_listbar, DECODE_ITEM);
            }

            esKRNL_SemPost(p_listbar->lbar_dec_sem);
        }
        else if (p_listbar->lbar_touch_sta == 1)
        {
            // 防止长按时重复绘制
            p_listbar->lbar_touch_sta   = 2;
        }
    }
    else
    {
        // 当前处于滑动状态
        p_listbar->lbar_last_pos.x = x;
        p_listbar->lbar_last_pos.y = y;

        if (p_listbar->lbar_touch_sta == 0)
        {
            // 继续滑动
            p_listbar->lbar_touch_sta   = 1;
        }
        else if (p_listbar->lbar_touch_sta == 1)
        {
            // 长按急停
            p_listbar->lbar_touch_sta   = 2;
            p_listbar->lbar_sld_speed   = -1;
            p_listbar->lbar_current_move = (p_listbar->lbar_current_move + p_listbar->config.item_height / 2)
                                           / p_listbar->config.item_height * p_listbar->config.item_height;
            p_listbar->lbar_move = p_listbar->lbar_current_move % p_listbar->lbar_page_height;
            __listbar_move_page(p_listbar, 0);
            tmp = p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col
                  + ((x - p_listbar->config.list_rect.x) / p_listbar->config.item_width)
                  + ((y - p_listbar->config.list_rect.y) / p_listbar->config.item_height * p_listbar->lbar_page_col);

            if (tmp < p_listbar->lbar_current_total)
            {
                // 点到条目内部
                p_listbar->lbar_focus_sta   = 1;
                p_listbar->lbar_current_focus = tmp;
                __listbar_draw_focus(p_listbar);
            }
        }
    }

    //dsk_keytone_on();
    esKRNL_SemPost(p_listbar->lbar_sld_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_TouchMove
*
*Description: 触摸移动
*
*Arguments  : hdle: 操作句柄， x: X点坐标， y：Y点坐标
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_TouchMove(H_LBAR hdle, __s32 x, __s32 y, __s32 direct, __s32 speed)
{
    __u8                err;
    __s32               step;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if (p_listbar->lbar_current_total == 0)
    {
        // 总条目数为空时，不作响应。
        return EPDK_OK;
    }

    if (p_listbar->lbar_touch_sta == -1)
    {
        return EPDK_FAIL;
    }

    esKRNL_SemPend(p_listbar->lbar_sld_sem, 0, &err);

    if (p_listbar->lbar_sld_speed == -1)
    {
        // 没有惯性滑动时，步长小于10不做滑动。
        step = y - p_listbar->lbar_last_pos.y;

        if ((step > 10) || (step < -10))
        {
            p_listbar->lbar_touch_sta = 3;
            p_listbar->lbar_last_pos.x = x;
            p_listbar->lbar_last_pos.y = y;
            p_listbar->lbar_last_direct = step;
            __listbar_move_page(p_listbar, step);
        }
    }
    else
    {
        // 有惯性滑动时，调整当前滑动方向。
        step = y - p_listbar->lbar_last_pos.y;

        if ((step > 10) || (step < -10))
        {
            p_listbar->lbar_touch_sta = 3;
            p_listbar->lbar_last_pos.x = x;
            p_listbar->lbar_last_pos.y = y;
            p_listbar->lbar_last_direct = step;
        }
    }

    esKRNL_SemPost(p_listbar->lbar_sld_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_TouchUp
*
*Description: 触摸抬起
*
*Arguments  : hdle: 操作句柄， x: X点坐标， y：Y点坐标
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_TouchUp(H_LBAR hdle, __s32 x, __s32 y, __s32 direct, __s32 speed)
{
    __u8                err;
    __listbar_t       *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if (p_listbar->lbar_current_total == 0)
    {
        // 总条目数为空时，不作响应。
        return EPDK_OK;
    }

    if (p_listbar->lbar_touch_sta == -1)
    {
        p_listbar->lbar_touch_sta = 0;
        return EPDK_FAIL;
    }

    p_listbar->lbar_last_pos.x = x;
    p_listbar->lbar_last_pos.y = y;

    if (p_listbar->lbar_touch_sta == 3)
    {
        // 非长按抬起后，做惯性滑动。
        p_listbar->lbar_touch_sta = 0;
        esKRNL_SemPend(p_listbar->lbar_sld_sem, 0, &err);

        if (p_listbar->lbar_sld_speed == -1)
        {
            p_listbar->lbar_sld_speed = speed * 2;
        }
        else
        {
            p_listbar->lbar_sld_speed = p_listbar->lbar_sld_speed + speed;

            if (p_listbar->lbar_sld_speed > 200)
            {
                p_listbar->lbar_sld_speed = 200;
            }
        }

        if (p_listbar->lbar_sld_state == 1)
        {
            p_listbar->lbar_sld_state = -1;
        }

        esKRNL_SemPost(p_listbar->lbar_sld_sem);
    }
    else
    {
        p_listbar->lbar_touch_sta = 0;
        p_listbar->lbar_focus_sta = 1;

        if (p_listbar->lbar_sld_state == 1)
        {
            p_listbar->lbar_sld_state = 0;
        }
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_GetTotalItem
*
*Description: 获取当前焦点条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 返回总编号（失败返回-1）
*
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetTotalItem(H_LBAR hdle)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return -1;
    }

    p_listbar = (__listbar_t *)hdle;
    return p_listbar->lbar_current_total;
}

/*
************************************************************************************************************************
*                                       LISTBAR_GetListAttr
*
*Description: 获取listbar私有数据
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 返回list私有数据（失败返回NULL）
*
*
************************************************************************************************************************
*/
void   *LISTBAR_GetListAttr(H_LBAR hdle)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return NULL;
    }

    p_listbar = (__listbar_t *)hdle;
    return p_listbar->config.list_attr;
}

/*
************************************************************************************************************************
*                                       LISTBAR_GetFocusItem
*
*Description: 获取当前焦点条目
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 返回焦点条目编号（失败返回-1）
*
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetFocusItem(H_LBAR hdle)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return -1;
    }

    p_listbar = (__listbar_t *)hdle;
    return p_listbar->lbar_current_focus;
}

/*
************************************************************************************************************************
*                                       LISTBAR_GetItemRect
*
*Description: 获取指定条目的RECT(只能是屏上显示的条目号)。
*
*Arguments  : hdle: 操作句柄，item：要获取的条目号，rect：获取后的区域
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetItemRect(H_LBAR hdle, __s32 item, RECT *rect)
{
    __s32               tmp;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    tmp = item - p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col;

    if ((tmp < 0) || (tmp >= p_listbar->lbar_page_num))
    {
        __wrn("input item error !\n");
        return EPDK_FAIL;
    }

    rect->x      = p_listbar->lbar_rect.x + (tmp % p_listbar->lbar_page_col) * p_listbar->config.item_width;
    rect->y      = p_listbar->lbar_rect.y + (tmp / p_listbar->lbar_page_col) * p_listbar->config.item_height;
    rect->width  = p_listbar->config.item_width;
    rect->height = p_listbar->config.item_height;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                             static void LISTBAR_LostFocus(__listbar_t * hdle)
*
* Description: 将获得焦点的函数失去焦点
*
* Arguments  :
*
* Returns    :
* Author        :  libaiao, 2011.5.14
* Notes      :
*
**********************************************************************************************************************
*/
void LISTBAR_LostFocus(H_LBAR *hdle)
{
    FB                      fb;
    RECT                    fb_rect;
    RECT                    cp_rect;
    HBLOCK                  l_block;
    __listbar_t            *p_listbar;
    __lbar_draw_para_t      draw_para;

    if (hdle == NULL)
    {
        return;
    }

    p_listbar = (__listbar_t *)hdle;
    p_listbar->lbar_item = p_listbar->lbar_current_focus -
                           p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col;

    if (p_listbar->lbar_item < 0)
    {
        p_listbar->lbar_item = 0;
    }

    //if(p_listbar->lbar_focus_sta == 1)
    {
        p_listbar->lbar_focus_sta = 0;
        GUI_LyrWinSel(p_listbar->lwin_hdle);
        draw_para.rect.x      = p_listbar->lbar_rect.x + (p_listbar->lbar_item % p_listbar->lbar_page_col) * p_listbar->config.item_width;
        draw_para.rect.y      = p_listbar->lbar_rect.y + (p_listbar->lbar_item / p_listbar->lbar_page_col) * p_listbar->config.item_height;
        draw_para.rect.width  = p_listbar->config.item_width;
        draw_para.rect.height = p_listbar->config.item_height;
        GUI_ClearRect(draw_para.rect.x, draw_para.rect.y, draw_para.rect.x + draw_para.rect.width, draw_para.rect.y + draw_para.rect.height);
        draw_para.index = p_listbar->lbar_current_focus;
        draw_para.attr  = p_listbar->config.list_attr;
        draw_para.mode = LBAR_MODE_NORMAL;
        p_listbar->lbar_draw(&draw_para);
    }
}


/*
************************************************************************************************************************
*                                       LISTBAR_SetFocusItem
*
*Description: 设置一个条目焦点条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetFocusItem(H_LBAR hdle, __s32 item)
{
    __u8                err;
    __s32               tmp_num;
    FB                  fb;
    RECT                fb_rect;
    RECT                cp_rect;
    HBLOCK              l_block;
    __listbar_t        *p_listbar;
    __lbar_draw_para_t  draw_para;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if ((item < 0) || (item >= p_listbar->lbar_current_total))
    {
        return EPDK_FAIL;
    }

    if (p_listbar->lbar_dec_state == DECODE_NULL)
    {
        p_listbar->lbar_current_focus   = item;
        p_listbar->lbar_current_page    = p_listbar->lbar_current_focus / p_listbar->lbar_page_num * p_listbar->lbar_page_num;

        if (p_listbar->lbar_current_move > p_listbar->lbar_current_focus / p_listbar->lbar_page_col * p_listbar->config.item_height)
        {
            p_listbar->lbar_current_move    = (p_listbar->lbar_current_focus + p_listbar->lbar_page_col - 1)
                                              / p_listbar->lbar_page_col * p_listbar->config.item_height;
        }
    }
    else
    {
        if ((item >= (p_listbar->lbar_current_focus - p_listbar->lbar_item)) &&
            (item <= (p_listbar->lbar_current_focus - p_listbar->lbar_item + p_listbar->lbar_page_num - 1)))
        {
            tmp_num = item - (p_listbar->lbar_current_focus - p_listbar->lbar_item);
            p_listbar->lbar_current_focus = item;
            //---------------------------------------------------------------
            //add by libaiao
            p_listbar->lbar_focus_sta = 1;
            //if(p_listbar->lbar_focus_sta == 1)
            {
                __listbar_draw_focus(p_listbar);
            }
            //---------------------------------------------------------------
            /*
            GUI_LyrWinSel(p_listbar->item_layer);
            //GUI_LyrWinSel(p_listbar->lwin_hdle);          //modified by libaiao,2011.5.14
            GUI_SetBkColor(p_listbar->config.bk_color);
            GUI_Clear();

            draw_para.rect.x      = 0;
            draw_para.rect.y      = 0;
            draw_para.rect.width  = p_listbar->config.item_width;
            draw_para.rect.height = p_listbar->config.item_height;

            draw_para.index = p_listbar->lbar_current_focus;
            draw_para.attr  = p_listbar->config.list_attr;
            draw_para.mode  = LBAR_MODE_FOCUS;

            p_listbar->lbar_draw(&draw_para);
            */
            /*
                GUI_LyrWinGetFB(p_listbar->item_layer, &fb);
                fb_rect.x = 0;
                fb_rect.y = 0;
                fb_rect.width = fb.size.width;
                fb_rect.height = fb.size.height;
                l_block = GUI_BlockCreateFrom(&fb_rect, fb.fmt.fmt.rgb.pixelfmt, fb.size.width, fb.size.height, fb.addr[0], fb.fmt.fmt.rgb.pixseq);

                cp_rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                cp_rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                cp_rect.width  = p_listbar->config.item_width;
                cp_rect.height = p_listbar->config.item_height;

                GUI_BlockLayerBlit(p_listbar->lwin_hdle, &cp_rect, 0, 0, l_block, NULL, BLITFLAG_COLORSET, NULL);
                 GUI_BlockDelete(l_block);
                    */
        }
        else
        {
            esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
            p_listbar->lbar_current_focus   = item;
            p_listbar->lbar_current_page    = p_listbar->lbar_current_focus / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = (p_listbar->lbar_current_focus + p_listbar->lbar_page_col - 1)
                                              / p_listbar->lbar_page_col * p_listbar->config.item_height;
            p_listbar->lbar_focus_sta = 1;      //add by libaiao,2011.5.14
            __listbar_show_page(p_listbar, DECODE_CURR);
            //p_listbar->lbar_focus_sta = 0;        //add by libaiao,2011.5.14
            esKRNL_SemPost(p_listbar->lbar_dec_sem);
            tmp_num               = 0;
            GUI_LyrWinSel(p_listbar->item_layer);
            //GUI_LyrWinSel(p_listbar->lwin_hdle);          //modified by libaiao,2011.5.14
            GUI_SetBkColor(p_listbar->config.bk_color);
            GUI_Clear();
            draw_para.rect.x      = 0;
            draw_para.rect.y      = 0;
            draw_para.rect.width  = p_listbar->config.item_width;
            draw_para.rect.height = p_listbar->config.item_height;
            draw_para.index = p_listbar->lbar_current_focus;
            draw_para.attr  = p_listbar->config.list_attr;
            draw_para.mode  = LBAR_MODE_FOCUS;
            p_listbar->lbar_draw(&draw_para);
            /*
                GUI_LyrWinGetFB(p_listbar->item_layer, &fb);
                fb_rect.x = 0;
                fb_rect.y = 0;
                fb_rect.width = fb.size.width;
                fb_rect.height = fb.size.height;
                l_block = GUI_BlockCreateFrom(&fb_rect, fb.fmt.fmt.rgb.pixelfmt, fb.size.width, fb.size.height, fb.addr[0], fb.fmt.fmt.rgb.pixseq);

                cp_rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                cp_rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                cp_rect.width  = p_listbar->config.item_width;
                cp_rect.height = p_listbar->config.item_height;

                GUI_BlockLayerBlit(p_listbar->lwin_hdle, &cp_rect, 0, 0, l_block, NULL, BLITFLAG_COLORSET, NULL);
                GUI_BlockDelete(l_block);
                    */
        }
    }

    return EPDK_OK;
}


/*
************************************************************************************************************************
*                                       LISTBAR_SetFocusItem
*
*Description: 设置一个条目焦点条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*author     : libaiao, 没有显示，只设置focus item
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetFocusItemExt(H_LBAR hdle, __s32 item)
{
    __u8                err;
    __s32               tmp_num;
    FB                  fb;
    RECT                fb_rect;
    RECT                cp_rect;
    HBLOCK              l_block;
    __listbar_t        *p_listbar;
    __lbar_draw_para_t  draw_para;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if ((item < 0) || (item >= p_listbar->lbar_current_total))
    {
        return EPDK_FAIL;
    }

    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);

    if (p_listbar->lbar_dec_state == DECODE_NULL)
    {
        p_listbar->lbar_current_focus   = item;
        p_listbar->lbar_current_page    = p_listbar->lbar_current_focus / p_listbar->lbar_page_num * p_listbar->lbar_page_num;

        if (p_listbar->lbar_current_move > p_listbar->lbar_current_focus / p_listbar->lbar_page_col * p_listbar->config.item_height)
        {
            p_listbar->lbar_current_move    = (p_listbar->lbar_current_focus + p_listbar->lbar_page_col - 1)
                                              / p_listbar->lbar_page_col * p_listbar->config.item_height;
        }
    }
    else
    {
        if ((item >= (p_listbar->lbar_current_focus - p_listbar->lbar_item)) &&
            (item <= (p_listbar->lbar_current_focus - p_listbar->lbar_item + p_listbar->lbar_page_num - 1)))
        {
            tmp_num = item - (p_listbar->lbar_current_focus - p_listbar->lbar_item);
            p_listbar->lbar_current_focus = item;
            /*
            GUI_LyrWinSel(p_listbar->item_layer);
            //GUI_LyrWinSel(p_listbar->lwin_hdle);          //modified by libaiao,2011.5.14
            GUI_SetBkColor(p_listbar->config.bk_color);
            GUI_Clear();

            draw_para.rect.x      = 0;
            draw_para.rect.y      = 0;
            draw_para.rect.width  = p_listbar->config.item_width;
            draw_para.rect.height = p_listbar->config.item_height;

            draw_para.index = p_listbar->lbar_current_focus;
            draw_para.attr  = p_listbar->config.list_attr;
            draw_para.mode  = LBAR_MODE_FOCUS;

            p_listbar->lbar_draw(&draw_para);
            */
            /*
                GUI_LyrWinGetFB(p_listbar->item_layer, &fb);
                fb_rect.x = 0;
                fb_rect.y = 0;
                fb_rect.width = fb.size.width;
                fb_rect.height = fb.size.height;
                l_block = GUI_BlockCreateFrom(&fb_rect, fb.fmt.fmt.rgb.pixelfmt, fb.size.width, fb.size.height, fb.addr[0], fb.fmt.fmt.rgb.pixseq);

                cp_rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                cp_rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                cp_rect.width  = p_listbar->config.item_width;
                cp_rect.height = p_listbar->config.item_height;

                GUI_BlockLayerBlit(p_listbar->lwin_hdle, &cp_rect, 0, 0, l_block, NULL, BLITFLAG_COLORSET, NULL);
                 GUI_BlockDelete(l_block);
                    */
        }
        else
        {
            //esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
            p_listbar->lbar_current_focus   = item;
            p_listbar->lbar_current_page    = p_listbar->lbar_current_focus / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
            p_listbar->lbar_current_move    = (p_listbar->lbar_current_focus + p_listbar->lbar_page_col - 1)
                                              / p_listbar->lbar_page_col * p_listbar->config.item_height;
            //p_listbar->lbar_focus_sta = 1;        //add by libaiao,2011.5.14
            //__listbar_show_page(p_listbar, DECODE_CURR);
            //p_listbar->lbar_focus_sta = 0;        //add by libaiao,2011.5.14
            //esKRNL_SemPost(p_listbar->lbar_dec_sem);
            /*
                tmp_num               = 0;

                GUI_LyrWinSel(p_listbar->item_layer);
                //GUI_LyrWinSel(p_listbar->lwin_hdle);          //modified by libaiao,2011.5.14
                GUI_SetBkColor(p_listbar->config.bk_color);
                GUI_Clear();

                draw_para.rect.x      = 0;
                draw_para.rect.y      = 0;
                draw_para.rect.width  = p_listbar->config.item_width;
                draw_para.rect.height = p_listbar->config.item_height;

                draw_para.index = p_listbar->lbar_current_focus;
                draw_para.attr  = p_listbar->config.list_attr;
                draw_para.mode  = LBAR_MODE_FOCUS;

                p_listbar->lbar_draw(&draw_para);
            */
            /*
                GUI_LyrWinGetFB(p_listbar->item_layer, &fb);
                fb_rect.x = 0;
                fb_rect.y = 0;
                fb_rect.width = fb.size.width;
                fb_rect.height = fb.size.height;
                l_block = GUI_BlockCreateFrom(&fb_rect, fb.fmt.fmt.rgb.pixelfmt, fb.size.width, fb.size.height, fb.addr[0], fb.fmt.fmt.rgb.pixseq);

                cp_rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
                cp_rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
                cp_rect.width  = p_listbar->config.item_width;
                cp_rect.height = p_listbar->config.item_height;

                GUI_BlockLayerBlit(p_listbar->lwin_hdle, &cp_rect, 0, 0, l_block, NULL, BLITFLAG_COLORSET, NULL);
                GUI_BlockDelete(l_block);
                    */
        }
    }

    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_SetNormalItem
*
*Description: 获得一个条目为普通条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetNormalItem(H_LBAR hdle, __s32 item)
{
    __s32               tmp_num;
    FB                  fb;
    RECT                fb_rect;
    RECT                cp_rect;
    HBLOCK              l_block;
    __listbar_t        *p_listbar;
    __lbar_draw_para_t  draw_para;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if ((item < 0) || (item >= p_listbar->lbar_current_total))
    {
        return EPDK_FAIL;
    }

    if ((item >= (p_listbar->lbar_current_focus - p_listbar->lbar_item)) &&
        (item <= (p_listbar->lbar_current_focus - p_listbar->lbar_item + p_listbar->lbar_page_num - 1)))
    {
        tmp_num     = item - (p_listbar->lbar_current_focus - p_listbar->lbar_item);
        //add by libaiao, 2011.5.14
        GUI_LyrWinSel(p_listbar->lwin_hdle);
        draw_para.rect.x      = p_listbar->lbar_rect.x + (p_listbar->lbar_item % p_listbar->lbar_page_col) * p_listbar->config.item_width;
        draw_para.rect.y      = p_listbar->lbar_rect.y + (p_listbar->lbar_item / p_listbar->lbar_page_col) * p_listbar->config.item_height;
        draw_para.rect.width  = p_listbar->config.item_width;
        draw_para.rect.height = p_listbar->config.item_height;
        draw_para.index = p_listbar->lbar_current_focus;
        draw_para.attr  = p_listbar->config.list_attr;
        draw_para.mode  = LBAR_MODE_NORMAL;
        p_listbar->lbar_draw(&draw_para);
        /*
                GUI_LyrWinSel(p_listbar->item_layer);
                GUI_SetBkColor(p_listbar->config.bk_color);
                GUI_Clear();

                draw_para.rect.x      = 0;
                draw_para.rect.y      = 0;
                draw_para.rect.width  = p_listbar->config.item_width;
                draw_para.rect.height = p_listbar->config.item_height;

                draw_para.index = p_listbar->lbar_current_focus;
                draw_para.attr  = p_listbar->config.list_attr;
                draw_para.mode  = LBAR_MODE_NORMAL;

                p_listbar->lbar_draw(&draw_para);
        */
        /*
            GUI_LyrWinGetFB(p_listbar->item_layer, &fb);
            fb_rect.x = 0;
            fb_rect.y = 0;
            fb_rect.width = fb.size.width;
            fb_rect.height = fb.size.height;
            l_block = GUI_BlockCreateFrom(&fb_rect, fb.fmt.fmt.rgb.pixelfmt, fb.size.width, fb.size.height, fb.addr[0], fb.fmt.fmt.rgb.pixseq);

            cp_rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
            cp_rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
            cp_rect.width  = p_listbar->config.item_width;
            cp_rect.height = p_listbar->config.item_height;

            GUI_BlockLayerBlit(p_listbar->lwin_hdle, &cp_rect, 0, 0, l_block, NULL, BLITFLAG_COLORSET, NULL);
          GUI_BlockDelete(l_block);
            */
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_SetInvalidItem
*
*Description: 获得一个条目为无效条目
*
*Arguments  : hdle: 操作句柄， item：条目编号
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetInvalidItem(H_LBAR hdle, __s32 item)
{
    __s32               tmp_num;
    __listbar_t        *p_listbar;
    __lbar_draw_para_t  draw_para;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if ((item >= (p_listbar->lbar_current_focus - p_listbar->lbar_item)) &&
        (item <= (p_listbar->lbar_current_focus - p_listbar->lbar_item + p_listbar->lbar_page_num - 1)))
    {
        GUI_LyrWinSel(p_listbar->lwin_hdle);

        if (p_listbar->config.alpha_status == 1)
        {
            GUI_OpenAlphaBlend();
        }

        tmp_num               = item - (p_listbar->lbar_current_focus - p_listbar->lbar_item);
        draw_para.rect.x      = p_listbar->lbar_rect.x + (tmp_num % p_listbar->lbar_page_col) * p_listbar->config.item_width;
        draw_para.rect.y      = p_listbar->lbar_rect.y + (tmp_num / p_listbar->lbar_page_col) * p_listbar->config.item_height;
        draw_para.rect.width  = p_listbar->config.item_width;
        draw_para.rect.height = p_listbar->config.item_height;
        draw_para.index = item;
        draw_para.attr  = p_listbar->config.list_attr;
        draw_para.mode  = LBAR_MODE_INVALID;
        p_listbar->lbar_draw(&draw_para);

        if (p_listbar->config.alpha_status == 1)
        {
            GUI_LyrWinFlushFB(p_listbar->lwin_hdle);
            GUI_CloseAlphaBlend();
        }
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_SetTotalItem
*
*Description: 设置总的条目数
*
*Arguments  : hdle: 操作句柄， num：条目总数
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetTotalItem(H_LBAR hdle, __u32 num)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    LISTBAR_SuspendPage(hdle);              //add by libaiao,2011.4.22
    p_listbar = (__listbar_t *)hdle;
    p_listbar->lbar_current_total = num;
    LISTBAR_ResumePage(hdle);               //add by libaiao,2011.4.22
    LISTBAR_ShowPage(hdle);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_GetListAttr
*
*Description: 获取listbar私有数据
*
*Arguments  : hdle: 操作句柄，attr：用户私有数据
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetListAttr(H_LBAR hdle, void *attr)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    p_listbar->config.list_attr = attr;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_ChangeMode
*
*Description: 改变当前显示方式
*
*Arguments  : hdle: 操作句柄，item_width：条目的宽度，item_height：条目的高度
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_ChangeMode(H_LBAR hdle, __s32 item_width, __s32 item_height)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    LISTBAR_SuspendPage(hdle);          //add by libaiao,2011.4.22
    // 保存新的条目宽高信息
    p_listbar->config.item_width  = item_width;
    p_listbar->config.item_height = item_height;

    if (p_listbar->item_layer)
    {
        // 删除ITEM缓存图层
        GUI_LyrWinDelete(p_listbar->item_layer);
        p_listbar->item_layer = NULL;
    }

    p_listbar->item_layer = __listbar_layer_create(p_listbar->config.item_width, p_listbar->config.item_height, 0);
    // 重新计算当前行列及单页条目数信息
    p_listbar->lbar_page_col        = p_listbar->lbar_rect.width / p_listbar->config.item_width;
    p_listbar->lbar_page_row        = p_listbar->lbar_rect.height / p_listbar->config.item_height;
    p_listbar->lbar_page_num        = p_listbar->lbar_page_row * p_listbar->lbar_page_col;
    p_listbar->lbar_page_width      = p_listbar->lbar_page_col * p_listbar->config.item_width;
    p_listbar->lbar_page_height     = p_listbar->lbar_page_row * p_listbar->config.item_height;
    // 调整当前界面现场
    p_listbar->lbar_current_page    = p_listbar->lbar_current_focus / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
    p_listbar->lbar_current_move    = p_listbar->lbar_current_focus / p_listbar->lbar_page_col * p_listbar->config.item_height;
    LISTBAR_ResumePage(hdle);           //add by libaiao,2011.4.22
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_SuspendPage
*
*Description: 挂起当前显示界面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SuspendPage(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_sld_sem, 0, &err);

    // 停止滑动线程
    if (p_listbar->lbar_sld_speed != -1)
    {
        p_listbar->lbar_sld_speed   = -1;
        p_listbar->lbar_current_move = (p_listbar->lbar_current_move + p_listbar->config.item_height / 2)
                                       / p_listbar->config.item_height * p_listbar->config.item_height;
        p_listbar->lbar_move = p_listbar->lbar_current_move % p_listbar->lbar_page_height;
        __listbar_move_page(p_listbar, 0);
        p_listbar->lbar_focus_sta   = 1;
        p_listbar->lbar_current_focus = p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col;
        __listbar_draw_focus(p_listbar);
    }

    esKRNL_SemPost(p_listbar->lbar_sld_sem);
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    // 停止预绘
    p_listbar->lbar_dec_state = DECODE_NULL;
    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_ResumePage
*
*Description: 恢复当前显示界面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_ResumePage(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    // 启动预绘线程，但不刷新已绘制好的页面，并且更新进度条
    p_listbar->lbar_dec_state = DECODE_ITEM;
    __listbar_show_scroll(p_listbar);
    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_UpdatePage
*
*Description: 更新当前显示界面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/

__s32   LISTBAR_UpdatePage(H_LBAR hdle)
{
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    esKRNL_SemPend(p_listbar->lbar_dec_sem, 0, &err);
    // 刷新预绘线程，并且刷新页面显示
    p_listbar->lbar_focus_sta   = 1;
    __listbar_show_page(p_listbar, DECODE_CURR);
    esKRNL_SemPost(p_listbar->lbar_dec_sem);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_SetScene
*
*Description: 设置listbar内部现场
*
*Arguments  : hdle: 操作句柄， scene：现场数据
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_SetScene(H_LBAR hdle, __listbar_scene_t *scene)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if (scene->start_id == -1)
    {
        p_listbar->lbar_current_page    = scene->focus_id / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
        p_listbar->lbar_current_focus   = scene->focus_id;
        p_listbar->lbar_current_move    = scene->focus_id / p_listbar->lbar_page_col * p_listbar->config.item_height;
    }
    else
    {
        p_listbar->lbar_current_page    = scene->start_id / p_listbar->lbar_page_num * p_listbar->lbar_page_num;
        p_listbar->lbar_current_focus   = scene->focus_id;
        p_listbar->lbar_current_move    = scene->start_id / p_listbar->lbar_page_col * p_listbar->config.item_height;
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       LISTBAR_GetScene
*
*Description: 获取listbar内部现场
*
*Arguments  : hdle: 操作句柄， scene：现场数据
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   LISTBAR_GetScene(H_LBAR hdle, __listbar_scene_t *scene)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;
    scene->start_id     = p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col;
    scene->focus_id     = p_listbar->lbar_current_focus;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_Delete
*
* Description: 删除listbar操作句柄
*
* Arguments  : hdle：操作句柄
*
* Returns    : EPDK_OK / EPDK_FAIL

* Notes      :
*
**********************************************************************************************************************
*/
__s32   LISTBAR_Delete(H_LBAR hdle)
{
    __u32               i;
    __u8                err;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)hdle;

    if (p_listbar->lbar_dif_tsk)
    {
        // 删除扩撒线程
        while (1)
        {
            if (esKRNL_TDelReq(p_listbar->lbar_dif_tsk) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            esKRNL_TimeDly(1);
        }

        p_listbar->lbar_dif_tsk = 0;
    }

    if (p_listbar->lbar_dif_sem)
    {
        esKRNL_SemDel(p_listbar->lbar_dif_sem, OS_DEL_ALWAYS, &err);
        p_listbar->lbar_dif_sem = NULL;
    }

    if (p_listbar->lbar_sld_tsk)
    {
        // 删除滑动线程
        while (1)
        {
            if (esKRNL_TDelReq(p_listbar->lbar_sld_tsk) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            esKRNL_TimeDly(1);
        }

        p_listbar->lbar_sld_tsk = 0;
    }

    if (p_listbar->lbar_sld_sem)
    {
        // 删除滑动线程访问控制信号量
        esKRNL_SemDel(p_listbar->lbar_sld_sem, OS_DEL_ALWAYS, &err);
        p_listbar->lbar_sld_sem = NULL;
    }

    if (p_listbar->lbar_dec_tsk)
    {
        // 删除预解线程
        while (1)
        {
            if (esKRNL_TDelReq(p_listbar->lbar_dec_tsk) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            esKRNL_TimeDly(1);
        }

        p_listbar->lbar_dec_tsk = 0;
    }

    if (p_listbar->lbar_dec_sem)
    {
        // 删除预解线程访问控制信号量
        esKRNL_SemDel(p_listbar->lbar_dec_sem, OS_DEL_ALWAYS, &err);
        p_listbar->lbar_dec_sem = NULL;
    }

    if (p_listbar->item_layer)
    {
        // 删除ITEM缓存图层
        GUI_LyrWinDelete(p_listbar->item_layer);
        p_listbar->item_layer = NULL;
    }

    for (i = 0; i < PAGE_NUM; i++)
    {
        // 删除缓存图层
        if (p_listbar->lbar_page[i].page_lyr)
        {
            GUI_LyrWinDelete(p_listbar->lbar_page[i].page_lyr);
            p_listbar->lbar_page[i].page_lyr = NULL;
        }
    }

    if (p_listbar->scroll_b_layer)
    {
        // 删除进度条背景缓存图层
        GUI_LyrWinDelete(p_listbar->scroll_b_layer);
        p_listbar->scroll_b_layer = NULL;
    }

    if (p_listbar->scroll_f_layer)
    {
        // 删除进度条前景缓存图层
        GUI_LyrWinDelete(p_listbar->scroll_f_layer);
        p_listbar->scroll_f_layer = NULL;
    }

    esMEMS_Mfree(0, p_listbar);
    p_listbar = NULL;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_GetState
*
* Description: 获取当前滑动状态
*
* Arguments  : hdle：操作句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__lbar_state_e   LISTBAR_GetState(H_LBAR hdle)
{
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return LBAR_STATE_STOP;
    }

    p_listbar = (__listbar_t *)hdle;

    if (p_listbar->lbar_sld_state == 0)
    {
        return LBAR_STATE_STOP;
    }

    return LBAR_STATE_MOVING;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_GetItemNo
*
* Description: 获取指定位置的条目号
*
* Arguments  : hdle：操作句柄
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
**********************************************************************************************************************
*/
__s32   LISTBAR_GetItemNo(H_LBAR hdle, __pos_t pos)
{
    __s32               num;
    __listbar_t        *p_listbar;

    if (hdle == NULL)
    {
        return -1;
    }

    p_listbar = (__listbar_t *)hdle;
    num = p_listbar->lbar_current_move / p_listbar->config.item_height * p_listbar->lbar_page_col
          + ((pos.x - p_listbar->config.list_rect.x) / p_listbar->config.item_width)
          + ((pos.y - p_listbar->config.list_rect.y) / p_listbar->config.item_height * p_listbar->lbar_page_col);
    return num;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_GetPageItemCount
*
* Description   : 获得每页有多少个条目
*
* Arguments  : hdle：操作句柄
*
*Return         :条目数
*             EPDK_FAIL: 失败
*
*Author     : add by Libaiao, 2011.04.21
**********************************************************************************************************************
*/
__s32 LISTBAR_GetPageItemCount(H_LBAR handle)
{
    __listbar_t        *p_listbar;

    if (handle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)handle;
    return  p_listbar->lbar_page_num;
}

/*
**********************************************************************************************************************
*                                               LISTBAR_GetRowItemNumber
*
* Description   : 获得每行有多少个条目
*
* Arguments  : hdle：操作句柄
*
*Return         : 条目数
*             EPDK_FAIL: 失败
*
*Author     : add by Libaiao, 2011.04.21
**********************************************************************************************************************
*/
__s32 LISTBAR_GetRowItemCount(H_LBAR handle)
{
    __listbar_t        *p_listbar;

    if (handle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)handle;
    return  p_listbar->lbar_page_col;
}

__s32 LISTBAR_Suspend_dif_task(H_LBAR handle)
{
    __u8                err;
    OS_SEM_DATA         pdata;
    __listbar_t        *p_listbar;

    if (handle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)handle;
    esKRNL_SemQuery(p_listbar->lbar_dif_sem, &pdata);

    if (pdata.OSCnt == 1)
    {
        esKRNL_SemPend(p_listbar->lbar_dif_sem, 0, &err);
    }

    return EPDK_OK;
}
__s32 LISTBAR_Resume_dif_task(H_LBAR handle)
{
    __u8                err;
    OS_SEM_DATA         pdata;
    __listbar_t        *p_listbar;

    if (handle == NULL)
    {
        return EPDK_FAIL;
    }

    p_listbar = (__listbar_t *)handle;
    esKRNL_SemQuery(p_listbar->lbar_dif_sem, &pdata);

    if (pdata.OSCnt == 0)
    {
        esKRNL_SemPost(p_listbar->lbar_dif_sem);
    }

    return EPDK_OK;
}

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
#include <kapi.h>
#include "mbook_private.h"

extern void *file_core;
typedef     void   *H_BMK;
#define     MAX_MARK_ITEM   10//..
#define     MAX_CONTEXT     100
#define     BOOK_MARK_PATH      "e:\\book_mark\\"//..
typedef     void   *H_BMK;//..

void *My_Malloc(void *heap, __u32 size);
void  My_Mfree(void *heap, void *f);
void *My_Palloc(__u32 npage, __u32 mode);
void  My_Pfree(void *mblk, __u32 npage);

typedef struct tag_ITEM_CONTEXT
{
    __s32           number;
    __s32           start;
    char            context[MAX_CONTEXT];
    void           *data;
} __item_context_t;

typedef struct tag_MARK_ITEM
{
    char                    file_path[1024];
    __epdk_charset_enm_e    file_charset;

    __item_context_t        book_mark[MAX_MARK_ITEM];
} __mark_item_t;

typedef struct tag_BOOK_MARK
{
    ES_FILE        *file_hdle;              // 文件句柄

    __u32           mark_size;              // 书签的大小
    __mark_item_t   mark_item;              // 书签的数据结构
    __u32           mark_state;             // 书签的打开状态
    __u32           mark_offset;            // 书签的文件偏移
} __book_mark_t; //..


/*
**********************************************************************************************************************
*                                               __decode_get_charset
*
* Description: 获取当前文件编码格式
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_charset(__mbook_decode_t *hdle)
{
    char                buff[3];
    __mbook_decode_t    *p_decode;
    p_decode = hdle;
    eLIBs_fseek(p_decode->file_hdle, 0, SEEK_SET);
    eLIBs_fread(buff, 3, sizeof(char), p_decode->file_hdle);

    if ((buff[0] == 0xff) && (buff[1] == 0xfe))
    {
        p_decode->charset = EPDK_CHARSET_ENM_UTF16LE;
    }
    else if ((buff[0] == 0xfe) && (buff[1] == 0xff))
    {
        p_decode->charset = EPDK_CHARSET_ENM_UTF16BE;
    }
    else if ((buff[0] == 0xef) && (buff[1] == 0xbb) && (buff[2] == 0xbf))
    {
        p_decode->charset = EPDK_CHARSET_ENM_UTF8;
    }
    else
    {
        p_decode->charset = p_decode->default_charset;
    }

    __wrn("p_decode->charset=%d\n", p_decode->charset);
}

/*
**********************************************************************************************************************
*                                               MBOOK_TTS_court_page
*
* Description: 读取当前页文件为解析数据数据,为。
*
* Arguments  :
*           @   hdle    :解码句柄
*           @   tts_curt_page_data: 输出，指向当前页文件数据
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
void MBOOK_TTS_court_page(H_DECODE hdle, char  **tts_curt_page_data, __s32 *char_num)
{
    __u32               size;
    __one_page_t        tmp_page;
    __mbook_decode_t    *p_decode;
    p_decode = hdle;
    __msg(" --------p_decode->file_hdle=%d------p_decode->current_page  = %d\n", p_decode->file_hdle, p_decode->current_page);
    // 从文件中读取当前页的数据数据
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page, &tmp_page);
    size = tmp_page.page_end - tmp_page.page_start;
    __msg("--------页buff的大小是%d, page_start = %d,page_end = %d \n", size, tmp_page.page_start, tmp_page.page_end);
    eLIBs_fseek(p_decode->file_hdle, tmp_page.page_start, SEEK_SET);
    eLIBs_fread(p_decode->curt_page_data, 1, size, p_decode->file_hdle);
    *(p_decode->curt_page_data + size) = 0xff;
    *(p_decode->curt_page_data + size + 1) = 0xfe;
    *tts_curt_page_data = p_decode->curt_page_data;
    *char_num = size;
    //  tts_page_data=p_decode->curt_page_data;
}




/*
**********************************************************************************************************************
*                                               __decode_read_page
*
* Description: 读取一页数据到缓存BUF里。
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_read_page(__mbook_decode_t *hdle)
{
    __u32               size;
    __u32               read_len;
    __one_page_t        tmp_page;
    __mbook_decode_t    *p_decode;
    p_decode = hdle;
    // 从文件中读取数据
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2, &tmp_page);
    __msg("start = %d, end = %d, page = %d \n", tmp_page.page_start, tmp_page.page_end, tmp_page.page_no);
    size = tmp_page.page_end - tmp_page.page_start;
    __msg("size = %d\r\n", size);
    eLIBs_fseek(p_decode->file_hdle, tmp_page.page_start, SEEK_SET);
    read_len = eLIBs_fread(p_decode->page_data, 1, size, p_decode->file_hdle);
    __msg("read_len:%d p_decode->page_data = %s\r\n", read_len, p_decode->page_data);
    *(p_decode->page_data + size) = 0xff;
    *(p_decode->page_data + size + 1) = 0xfe;

    // 将读取的数据写到软图层上
    if (p_decode->config.show_rotate == 0)
    {
        MBOOK_Layer_Rotate(p_decode->page[p_decode->decode_cnt].page_lyr, p_decode->config.show_width,
                           p_decode->config.show_height - p_decode->config.bottom_width, 0);
    }
    else
    {
        MBOOK_Layer_Rotate(p_decode->page[p_decode->decode_cnt].page_lyr,
                           p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1);
    }

    p_decode->page[p_decode->decode_cnt].page_sta = 1;
    p_decode->page[p_decode->decode_cnt].page_no = p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2;
    MBOOK_Show_Page(p_decode->show_hdle, p_decode->page[p_decode->decode_cnt].page_lyr, p_decode->page_data);
}

/*
**********************************************************************************************************************
*                                               __decode_show_page
*
* Description: 读取一页数据到缓存BUF里。
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_show_page(__mbook_decode_t *hdle)
{
    __s32               i, tmp;
    __u8               *pdes = NULL;
    __u8               *psrc1 = NULL;
    __u8               *psrc2 = NULL;
    __u8                bpp = 0;
    FB                  fb_des;
    FB                  fb_src1;
    FB                  fb_src2;
    __mbook_decode_t    *p_decode = NULL;
    __wrn("Enter %s\r\n", __FUNCTION__);
    p_decode = hdle;
    eLIBs_memset(&fb_des, 0x00, sizeof(FB));
    eLIBs_memset(&fb_src1, 0x00, sizeof(FB));
    eLIBs_memset(&fb_src2, 0x00, sizeof(FB));
    GUI_LyrWinGetFB(p_decode->page[DECODE_MBOOK_NUM / 2].page_lyr, &fb_src1);
    GUI_LyrWinGetFB(p_decode->page[DECODE_MBOOK_NUM / 2 + 1].page_lyr, &fb_src2);
    GUI_LyrWinGetFB(p_decode->show_lyr, &fb_des);

    switch (fb_des.fmt.fmt.rgb.pixelfmt)
    {
        case PIXEL_COLOR_ARGB8888:
        {
            bpp = 32 >> 3;
        }
        break;

        default:
        {
            bpp = 8 >> 3;
        }
        break;
    }

    if (p_decode->config.show_rotate == 0)
    {
        eLIBs_memcpy(fb_des.addr[0], (void *)((unsigned long)fb_src1.addr[0] + p_decode->move_height * fb_des.size.width * bpp),
                     fb_des.size.width * (fb_des.size.height - p_decode->move_height) * bpp);
        eLIBs_memcpy((void *)((unsigned long)fb_des.addr[0] + (fb_des.size.height - p_decode->move_height) * fb_des.size.width * bpp), fb_src2.addr[0],
                     fb_des.size.width * p_decode->move_height * bpp);
    }
    else
    {
        tmp     = (fb_des.size.width - p_decode->move_height) * bpp;
        pdes    = fb_des.addr[0];
        psrc1   = fb_src1.addr[0];
        psrc2   = fb_src2.addr[0];
        psrc1   = psrc1 + p_decode->move_height * bpp;

        for (i = 0; i < fb_des.size.height; i++)
        {
            eLIBs_memcpy(pdes, psrc1, tmp);
            pdes    = pdes + tmp;
            psrc1   = psrc1 + fb_des.size.width * bpp;
            eLIBs_memcpy(pdes, psrc2, p_decode->move_height * bpp);
            pdes    = pdes + p_decode->move_height * bpp;
            psrc2   = psrc2 + fb_des.size.width * bpp;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __decode_brush_page
*
* Description: 刷新页面缓存里的信息
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_brush_page(__mbook_decode_t *hdle, __decode_mode_e mode)
{
    __u32               i;
    __page_cache_t      tmp_page;
    __mbook_decode_t    *p_decode;
    p_decode = hdle;

    if (mode == DECODE_PREV)
    {
        tmp_page.page_no    = p_decode->page[DECODE_MBOOK_NUM - 1].page_no;
        tmp_page.page_lyr   = p_decode->page[DECODE_MBOOK_NUM - 1].page_lyr;

        for (i = DECODE_MBOOK_NUM - 1; i > 0; i--)
        {
            p_decode->page[i].page_sta  = p_decode->page[i - 1].page_sta;
            p_decode->page[i].page_no   = p_decode->page[i - 1].page_no;
            p_decode->page[i].page_lyr  = p_decode->page[i - 1].page_lyr;
        }

        p_decode->page[0].page_sta  = 0;
        p_decode->page[0].page_no   = tmp_page.page_no;
        p_decode->page[0].page_lyr  = tmp_page.page_lyr;
    }
    else if (mode == DECODE_NEXT)
    {
        tmp_page.page_no    = p_decode->page[0].page_no;
        tmp_page.page_lyr   = p_decode->page[0].page_lyr;

        for (i = 0; i < DECODE_MBOOK_NUM - 1; i++)
        {
            p_decode->page[i].page_sta  = p_decode->page[i + 1].page_sta;
            p_decode->page[i].page_no   = p_decode->page[i + 1].page_no;
            p_decode->page[i].page_lyr  = p_decode->page[i + 1].page_lyr;
        }

        p_decode->page[DECODE_MBOOK_NUM - 1].page_sta   = 0;
        p_decode->page[DECODE_MBOOK_NUM - 1].page_no    = tmp_page.page_no;
        p_decode->page[DECODE_MBOOK_NUM - 1].page_lyr   = tmp_page.page_lyr;
    }
    else if (mode == DECODE_CURR)
    {
        for (i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta = 0;
        }
    }
}

/*
**********************************************************************************************************************
*                                               __decode_get_page
*
* Description: 获得一个已解好的页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_page(__mbook_decode_t *hdle, __decode_mode_e mode)
{
    __u8                err;
    __mbook_decode_t    *p_decode;
    p_decode = hdle;
    // 拿住解码信号量decode_sem。
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 刷新页面缓冲池
    if (mode == DECODE_CURR)
    {
        __decode_brush_page(p_decode, DECODE_CURR);
    }
    else if (mode == DECODE_PREV)
    {
        p_decode->current_page--;
        __decode_brush_page(p_decode, DECODE_PREV);
    }
    else if (mode == DECODE_NEXT)
    {
        p_decode->current_page++;
        __decode_brush_page(p_decode, DECODE_NEXT);
    }

    // 判断当前页解码是否完成，未完成则立刻解当前页。
    p_decode->decode_cnt = DECODE_MBOOK_NUM / 2;

    if ((p_decode->page[p_decode->decode_cnt].page_sta == 0)
        && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) >= 1)
        && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) <= p_decode->total_page))
    {
        //__here__;
        __decode_read_page(p_decode);
    }

    p_decode->decode_sta = mode;
    // 释放解码信号量decode_sem。
    esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_get_next_page
*
* Description: 获得当前页的下一页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_next_page(__mbook_decode_t *hdle)
{
    __u8                err;
    __mbook_decode_t    *p_decode;
    p_decode = hdle;
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    p_decode->decode_cnt = DECODE_MBOOK_NUM / 2 + 1;

    if ((p_decode->page[p_decode->decode_cnt].page_sta == 0)
        && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) > 0))
    {
        __decode_read_page(p_decode);
    }

    esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_get_prev_page
*
* Description: 获得当前页的上一页面
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_get_prev_page(__mbook_decode_t *hdle)
{
    __u8                err;
    __mbook_decode_t    *p_decode;
    p_decode = hdle;
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    p_decode->decode_cnt = DECODE_MBOOK_NUM / 2 - 1;

    if ((p_decode->page[p_decode->decode_cnt].page_sta == 0)
        && ((p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) > 0))
    {
        __decode_read_page(p_decode);
    }

    esKRNL_SemPost(p_decode->decode_sem);
}

/*
**********************************************************************************************************************
*                                               __decode_move_down
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_move_down(__mbook_decode_t *hdle, __s32 step)
{
    __s32                   tmp_h;
    __mbook_decode_t       *p_decode;
    p_decode = hdle;
    // 获取当前显示页的屏幕窗口起始Y。
    tmp_h   = MBOOK_Layer_GetSrcH(p_decode->show_lyr);

    if ((p_decode->current_page >= p_decode->total_page) && (p_decode->move_state == 0))
    {
        return;
    }

    p_decode->move_state = 1;

    if ((p_decode->move_height + step) >= tmp_h)
    {
        // 翻页
        __decode_get_page(p_decode, DECODE_NEXT);

        if (p_decode->current_page < p_decode->total_page)
        {
            __decode_get_next_page(p_decode);
            p_decode->move_height = p_decode->move_height + step - tmp_h;
            __decode_show_page(p_decode);
        }
        else
        {
            p_decode->move_height = 0;
            p_decode->move_state = 0;
            __decode_show_page(p_decode);
        }
    }
    else
    {
        p_decode->move_height = p_decode->move_height + step;
        __decode_show_page(p_decode);
    }
}

/*
**********************************************************************************************************************
*                                               __decode_move_up
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_move_up(__mbook_decode_t *hdle, __s32 step)
{
    __s32                   tmp_h;
    __mbook_decode_t       *p_decode;
    p_decode = hdle;
    // 获取当前显示页的屏幕窗口起始Y。
    tmp_h   = MBOOK_Layer_GetSrcH(p_decode->show_lyr);

    if ((p_decode->current_page <= 1) && (p_decode->move_state == 0))
    {
        return;
    }

    p_decode->move_state = -1;

    if ((p_decode->move_height - step) <= 0)
    {
        // 翻页
        if (p_decode->current_page > 1)
        {
            __decode_get_page(p_decode, DECODE_PREV);
            p_decode->move_height = p_decode->move_height - step + tmp_h;
            __decode_show_page(p_decode);
        }
        else
        {
            p_decode->move_height = 0;
            p_decode->move_state = 0;
            __decode_show_page(p_decode);
        }
    }
    else
    {
        p_decode->move_height = p_decode->move_height - step;
        __decode_show_page(p_decode);
    }
}

/*
**********************************************************************************************************************
*                                               __decode_task
*
* Description: 页面解析写软图层线程
*
* Arguments  :
*
* Returns    :

* Notes      :
*
**********************************************************************************************************************
*/
static void __decode_task(void *p_arg)
{
    __u8                err;
    __mbook_decode_t    *p_decode = NULL;
    p_decode = (__mbook_decode_t *)p_arg;

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_DEC_TASK;
        }

        esKRNL_TimeDly(5);
        esKRNL_SemPend(p_decode->decode_sem, 0, &err);

        if (p_decode->decode_sta != DECODE_NULL)
        {
            if ((p_decode->page[p_decode->decode_cnt].page_sta == 0)
                && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) >= 1)
                && ((__s32)(p_decode->current_page + p_decode->decode_cnt - DECODE_MBOOK_NUM / 2) <= p_decode->total_page))
            {
                __decode_read_page(p_decode);
            }

            p_decode->decode_cnt++;

            if (p_decode->decode_cnt == DECODE_MBOOK_NUM)
            {
                p_decode->decode_cnt = 0;
            }
        }

        esKRNL_SemPost(p_decode->decode_sem);
    }

EXIT_DEC_TASK:
    esKRNL_TDel(EXEC_prioself);
    return;
}

/*
************************************************************************************************************************
*                                       __decode_show_config
*
*Description: 配置绘制模块相关参数
*
*Arguments  : hdle： 操作句柄
*
*
*Return     : NULL
*
************************************************************************************************************************
*/
static void __decode_show_config(__mbook_decode_t *hdle)
{
    __show_config_t         show_cfg;
    __mbook_decode_t       *p_decode;
    __decode_config_t      *p_config;
    p_decode = hdle;
    p_config = &p_decode->config;

    // 配置数据读写模块
    if (p_config->show_rotate == 0)
    {
        show_cfg.show_start = p_config->border_width;
        show_cfg.show_width = p_config->show_width - 2 * p_config->border_width + p_config->row_space - p_config->scroll_width;
        show_cfg.page_row   = (p_config->show_height - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        show_cfg.row_height = p_config->font_size + p_config->row_space;
        show_cfg.col_width  = p_config->font_size + p_config->col_space;
        show_cfg.font_color = p_config->font_color;
        show_cfg.back_color = p_config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
    }
    else
    {
        show_cfg.show_start = p_config->border_width;
        show_cfg.show_width = p_config->show_height - 2 * p_config->border_width + p_config->row_space - p_config->scroll_width;
        show_cfg.page_row   = (p_config->show_width - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        show_cfg.row_height = p_config->font_size + p_config->row_space;
        show_cfg.col_width  = p_config->font_size + p_config->col_space;
        show_cfg.font_color = p_config->font_color;
        show_cfg.back_color = p_config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
    }

    MBOOK_Show_Config(p_decode->show_hdle, &show_cfg);
}

/*
************************************************************************************************************************
*                                       __decode_analysis_config
*
*Description: 配置解码模块相关参数
*
*Arguments  : hdle： 操作句柄
*
*
*Return     : NULL
*
************************************************************************************************************************
*/
static void __decode_analysis_config(__mbook_decode_t *hdle)
{
    __one_page_t            tmp_page;
    __analysis_config_t     analysis_cfg;
    __mbook_decode_t       *p_decode;
    __decode_config_t      *p_config;
    p_decode = hdle;
    p_config = &p_decode->config;
    // 读取当前页的信息
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page, &tmp_page);

    // 配置文件解析模块
    if (p_config->show_rotate == 0)
    {
        analysis_cfg.show_width = p_config->show_width - 2 * p_config->border_width + p_config->row_space;
        analysis_cfg.page_row   = (p_config->show_height - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        analysis_cfg.col_width  = p_config->font_size + p_config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
    }
    else
    {
        analysis_cfg.show_width = p_config->show_height - 2 * p_config->border_width + p_config->row_space;
        analysis_cfg.page_row   = (p_config->show_width - p_config->bottom_width) / (p_config->font_size + p_config->row_space);
        analysis_cfg.col_width  = p_config->font_size + p_config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
    }

    MBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
    // 启动文件分析
    MBOOK_Analysis_Work(p_decode->analysis_hdle);
    // 获取总页数
    p_decode->total_page = MBOOK_Analysis_GetTotalPage(p_decode->analysis_hdle);
    // 定位当前页
    p_decode->current_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, tmp_page.page_start);
}


__s32   MBOOK_Show_LastPage(H_DECODE hdle, __u32 page_no)
{
    __u8                    err;
    __mbook_decode_t        *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if ((page_no > p_decode->total_page) || (page_no < 1))
    {
        return p_decode->current_page;
    }

    //  if(p_decode->current_page == page_no)
    //  {
    //      return p_decode->current_page;
    //  }
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    p_decode->current_page = page_no;
    esKRNL_SemPost(p_decode->decode_sem);
    p_decode->move_height = 0;
    p_decode->move_state = 0;
    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    __wrn("p_decode->show_lyr=%d\n", p_decode->show_lyr);
    MBOOK_Layer_OpenShow(p_decode->show_lyr);
    GUI_LyrWinSetTop(p_decode->show_lyr);
    return p_decode->current_page;
}

__s32  MBOOK_Decode_get_filename(char *filename)
{
    HRATNPL             h_rat_npl;
    __s32               npl_index;
    h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
    npl_index = rat_npl_get_cur(h_rat_npl);
    rat_npl_index2file(h_rat_npl, npl_index, filename);
    __msg("------------filename=%s\n----------", filename);
    rat_npl_close(h_rat_npl);
    filename = eLIBs_strchrlast(filename, '\\') + 1;
    __msg("------------filename=%s\n----------", filename);
    return EPDK_OK;
}

__epdk_charset_enm_e EBookGetSysDefaultCharset(void)
{
    __epdk_language_enm_e e_lang;
    __epdk_charset_enm_e e_charset;
    reg_system_para_t *para;
    para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

    if (!para)
    {
        return EPDK_CHARSET_ENM_GBK;
    }

    e_lang = para->language;

    if (EPDK_LANGUAGE_ENM_CHINESES == e_lang)
    {
        e_charset = EPDK_CHARSET_ENM_GBK;
    }
    else if (EPDK_LANGUAGE_ENM_CHINESET == e_lang)
    {
        e_charset = EPDK_CHARSET_ENM_BIG5;
    }
    else
    {
        e_charset = EPDK_CHARSET_ENM_GBK;
    }

    return e_charset;
}


/*
************************************************************************************************************************
*                                       MBOOK_Decode_Init
*
*Description: 初始化页面解码模块
*
*Arguments  : filename：文件全路径名
*
*
*Return     : H_DECODE: 返回解码操作句柄 err :0 代表无错误，1代表文件不存在 2代表其它错误
*
************************************************************************************************************************
*/
H_DECODE   MBOOK_Decode_Init(char *filename, __u8 *err)
{
    __mbook_decode_t    *p_decode = NULL;
    *err = 0;
    p_decode = (__mbook_decode_t *)My_Malloc(0, sizeof(__mbook_decode_t));  // 申请内存

    if (p_decode == 0)
    {
        *err = 2;
        __err(" anole core malloc error \n");
        return NULL;
    }

    eLIBs_memset(p_decode, 0, sizeof(__mbook_decode_t));

    if (filename == NULL)
    {
        __s32       npl_index;
        HRATNPL     h_rat_npl;
        h_rat_npl = rat_npl_open(RAT_MEDIA_TYPE_EBOOK);
        npl_index = rat_npl_get_cur(h_rat_npl);
        rat_npl_index2file(h_rat_npl, npl_index, p_decode->file_path);
        rat_npl_close(h_rat_npl);
    }
    else
    {
        eLIBs_memcpy(p_decode->file_path, filename, MAX_FILE_NAME);
    }

    __wrn("file_path:%s\r\n", p_decode->file_path);
    p_decode->file_hdle = eLIBs_fopen(p_decode->file_path, "rb");

    if (p_decode->file_hdle == NULL)
    {
        *err = 1;
        __wrn("eLIBs_fopen fail p_decode->file_hdle == NULL\r\n");
        goto INIT_ERROR_1;
    }

    p_decode->file_size = eLIBs_GetFileSize(p_decode->file_path);
    __wrn("file_size:%d\r\n", p_decode->file_size);
    p_decode->analysis_hdle = MBOOK_Analysis_Init(p_decode->file_hdle);

    if (p_decode->analysis_hdle == NULL)
    {
        *err = 2;
        __wrn("MBOOK_Analysis_Init fail p_decode->analysis_hdle == NULL\r\n");
        goto INIT_ERROR_2;
    }

    p_decode->show_hdle = MBOOK_Show_Init();

    if (p_decode->show_hdle == NULL)
    {
        *err = 2;
        __wrn("MBOOK_Show_Init fail p_decode->show_hdle == NULL\r\n");
        goto INIT_ERROR_3;
    }

    p_decode->data_len = 15;
    p_decode->page_data = (void *)My_Palloc(p_decode->data_len, 0);     // 申请内存
    p_decode->curt_page_data = (void *)esMEMS_Palloc(p_decode->data_len, 0);  //申请当前页数据内存，为tts服务

    if (p_decode->page_data == 0 || p_decode->curt_page_data == NULL)
    {
        *err = 2;
        __wrn("My_Palloc Create fail\r\n");
        goto INIT_ERROR_4;
    }

    p_decode->total_page        = 0;
    p_decode->current_page      = 0;
    p_decode->move_height       = 0;
    p_decode->move_state        = 0;
    p_decode->charset           = EBookGetSysDefaultCharset();
    p_decode->default_charset   = EBookGetSysDefaultCharset();
    p_decode->decode_cnt    = DECODE_MBOOK_NUM / 2;
    p_decode->decode_sta    = DECODE_NULL;  // 解码状态位
    p_decode->decode_sem    = esKRNL_SemCreate(1);  // 解码控制信号量
    p_decode->decode_tsk    = esKRNL_TCreate(__decode_task, (void *)p_decode, 0x2000, KRNL_priolevel5);     // 加载预解线程
    //__here__;
    return p_decode;
INIT_ERROR_4:

    if (p_decode->show_hdle)
    {
        MBOOK_Show_Uninit(p_decode->show_hdle);
        p_decode->show_hdle = NULL;
    }

INIT_ERROR_3:

    if (p_decode->analysis_hdle)
    {
        MBOOK_Analysis_Uninit(p_decode->analysis_hdle);
        p_decode->analysis_hdle = NULL;
    }

INIT_ERROR_2:

    if (p_decode->file_hdle)
    {
        eLIBs_fclose(p_decode->file_hdle);
        p_decode->file_hdle = NULL;
    }

INIT_ERROR_1:

    if (p_decode)
    {
        My_Mfree(0, p_decode);
        p_decode = NULL;
    }

    return NULL;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_Config
*
*Description: 配置页面解码模块
*
*Arguments  : hdle: 操作句柄
*             config: 参数配置信息指针
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_Config(H_DECODE hdle, __decode_config_t *config)
{
    __u32                   i;
    __show_config_t         show_cfg;
    __analysis_config_t     analysis_cfg;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        __wrn("%s hdle == NULL\r\n", __FUNCTION__);
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    // 获取文件编码格式
    __decode_get_charset(p_decode);
    // 获取字符字库信息
    eLIBs_memcpy(&(p_decode->config), config, sizeof(__decode_config_t));
    p_decode->font_hdle     = MBOOK_Font_Init(config->char_font, config->font_size);
    p_decode->char_font     = MBOOK_Font_GetFont(p_decode->font_hdle);
    p_decode->char_table    = MBOOK_Font_GetTable(p_decode->font_hdle);
    __wrn("config->show_rotate=%d\n", config->show_rotate);

    if (config->show_rotate == 0)
    {
        for (i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(config->show_width, config->show_height - config->bottom_width, 1);    // 申请软图层

            if (p_decode->page[i].page_lyr == NULL)
            {
                __wrn("MBOOK_Layer_CreatLayer fail...\n");
                goto INIT_ERROR_1;
            }
        }

        // 创建显示图层
        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(config->show_width, config->show_height - config->bottom_width, 1);    // 申请显示图层
        __wrn("p_decode->show_lyr=%d\n", p_decode->show_lyr);

        if (p_decode->show_lyr == NULL)
        {
            __wrn("MBOOK_Layer_CreatLayer fail...\n");
            goto INIT_ERROR_2;
        }

        // 配置文件解析模块
        analysis_cfg.show_width = config->show_width - 2 * config->border_width + config->row_space;
        analysis_cfg.page_row   = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
        analysis_cfg.col_width  = config->font_size + config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
        MBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
        // 配置数据读写模块
        show_cfg.show_start = config->border_width;
        show_cfg.show_width = config->show_width - 2 * config->border_width + config->row_space - config->scroll_width;
        show_cfg.page_row   = (config->show_height - config->bottom_width) / (config->font_size + config->row_space);
        show_cfg.row_height = config->font_size + config->row_space;
        show_cfg.col_width  = config->font_size + config->col_space;
        show_cfg.font_color = config->font_color;
        show_cfg.back_color = config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
        MBOOK_Show_Config(p_decode->show_hdle, &show_cfg);
        // 启动文件分析
        MBOOK_Analysis_Work(p_decode->analysis_hdle);
    }
    else
    {
        for (i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(config->show_width - config->bottom_width, config->show_height, 1);    // 申请软图层

            if (p_decode->page[i].page_lyr == NULL)
            {
                __wrn("MBOOK_Layer_CreatLayer fail...\n");
                goto INIT_ERROR_1;
            }
        }

        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(config->show_width - config->bottom_width, config->show_height, 1);    // 申请显示图层

        if (p_decode->show_lyr == NULL)
        {
            __wrn("MBOOK_Layer_CreatLayer fail...\n");
            goto INIT_ERROR_2;
        }

        // 配置文件解析模块
        analysis_cfg.show_width = config->show_height - 2 * config->border_width + config->row_space - config->scroll_width;
        analysis_cfg.page_row   = (config->show_width - config->bottom_width) / (config->font_size + config->row_space);
        analysis_cfg.col_width  = config->font_size + config->col_space;
        analysis_cfg.char_table = p_decode->char_table;
        analysis_cfg.charset    = p_decode->charset;
        MBOOK_Analysis_Config(p_decode->analysis_hdle, &analysis_cfg);
        // 配置数据读写模块
        show_cfg.show_start = config->border_width;
        show_cfg.show_width = config->show_height - 2 * config->border_width + config->row_space;
        show_cfg.page_row   = (config->show_width - config->bottom_width) / (config->font_size + config->row_space);
        show_cfg.row_height = config->font_size + config->row_space;
        show_cfg.col_width  = config->font_size + config->col_space;
        show_cfg.font_color = config->font_color;
        show_cfg.back_color = config->back_color;
        show_cfg.char_font  = p_decode->char_font;
        show_cfg.char_table = p_decode->char_table;
        show_cfg.charset    = p_decode->charset;
        MBOOK_Show_Config(p_decode->show_hdle, &show_cfg);
        // 启动文件分析
        MBOOK_Analysis_Work(p_decode->analysis_hdle);
    }

    //__here__
    // 获取总页数
    p_decode->current_page  = 1;
    p_decode->total_page  = MBOOK_Analysis_GetTotalPage(p_decode->analysis_hdle);
    //__here__
    return EPDK_OK;
INIT_ERROR_2:
INIT_ERROR_1:

    for (i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if (p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    return EPDK_FAIL;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_ShowPage
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
__s32   MBOOK_Decode_ShowPage(H_DECODE hdle, __u32 offset)
{
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    p_decode->current_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, offset);
    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    MBOOK_Layer_OpenShow(p_decode->show_lyr);
    GUI_LyrWinSetTop(p_decode->show_lyr);
    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_ShowNext
*
*Description: 显示下一页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_ShowNext(H_DECODE hdle)
{
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        __wrn("%s hdle == NULL\r\n", __FUNCTION__);
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->current_page < p_decode->total_page)
    {
        p_decode->move_height = 0;
        p_decode->move_state = 0;
        __decode_get_page(p_decode, DECODE_NEXT);
        __decode_show_page(p_decode);
    }

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_ShowPrev
*
*Description: 显示上一页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_ShowPrev(H_DECODE hdle)
{
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->current_page > 1)
    {
        p_decode->move_height = 0;
        p_decode->move_state = 0;
        __decode_get_page(p_decode, DECODE_PREV);
        __decode_show_page(p_decode);
    }

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SeekPage
*
*Description: 跳转到指定页面
*
*Arguments  : hdle: 操作句柄
*             page_no: 要显示的页码
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SeekPage(H_DECODE hdle, __u32 page_no)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if ((page_no > p_decode->total_page) || (page_no < 1))
    {
        return p_decode->current_page;
    }

    if (p_decode->current_page == page_no)
    {
        return p_decode->current_page;
    }

    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    p_decode->current_page = page_no;
    esKRNL_SemPost(p_decode->decode_sem);
    p_decode->move_height = 0;
    p_decode->move_state = 0;
    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SeekOffset
*
*Description: 跳转到指定偏移量
*
*Arguments  : hdle: 操作句柄
*             offset: 文件偏移量
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SeekOffset(H_DECODE hdle, __u32 offset)
{
    __u8                    err;
    __u32                   tmp_page;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    tmp_page = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, offset);
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    p_decode->current_page = tmp_page;
    esKRNL_SemPost(p_decode->decode_sem);
    p_decode->move_height = 0;
    p_decode->move_state = 0;
    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_MovePage
*
*Description: 跳转到指定页面
*
*Arguments  : hdle: 操作句柄
*             x, y: 移动的偏移量
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_MovePage(H_DECODE hdle, __s32 x, __s32 y)
{
    __s32                   tmp_step;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.show_rotate == 0)
    {
        tmp_step = y;
    }
    else
    {
        tmp_step = x;
    }

    if ((tmp_step > 200) || ((tmp_step > 0) && (tmp_step < 20)))
    {
        tmp_step = 25;
    }
    else if (((tmp_step > -20) && (tmp_step < 0)) || (tmp_step < -200))
    {
        tmp_step = -25;
    }

    if (tmp_step < 0)
    {
        __decode_move_down(p_decode, 0 - tmp_step);
    }
    else
    {
        __decode_move_up(p_decode, tmp_step);
    }

    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_RotatePage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：0 - 0度，90 - 90度
*             失败：-1
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_RotatePage(H_DECODE hdle)
{
    __u32                   i;
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return -1;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.show_rotate == 0)
    {
        p_decode->config.show_rotate = 90;
    }
    else
    {
        p_decode->config.show_rotate = 0;
    }

    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);

    // 删除图层
    for (i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if (p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    if (p_decode->show_lyr)
    {
        MBOOK_Layer_DeleteLayer(p_decode->show_lyr);
        p_decode->show_lyr = NULL;
    }

    //创建图层
    if (p_decode->config.show_rotate == 0)
    {
        for (i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width, p_decode->config.show_height - p_decode->config.bottom_width, 1); // 申请软图层

            if (p_decode->page[i].page_lyr == NULL)
            {
                goto INIT_ERROR_1;
            }
        }

        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width, p_decode->config.show_height - p_decode->config.bottom_width, 1); // 申请显示图层

        if (p_decode->show_lyr == NULL)
        {
            goto INIT_ERROR_2;
        }
    }
    else
    {
        for (i = 0; i < DECODE_MBOOK_NUM; i++)
        {
            p_decode->page[i].page_sta  = 0;
            p_decode->page[i].page_no   = 0;
            p_decode->page[i].page_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1); // 申请软图层

            if (p_decode->page[i].page_lyr == NULL)
            {
                goto INIT_ERROR_1;
            }
        }

        p_decode->show_lyr  = MBOOK_Layer_CreatLayer(p_decode->config.show_width - p_decode->config.bottom_width, p_decode->config.show_height, 1); // 申请显示图层

        if (p_decode->show_lyr == NULL)
        {
            goto INIT_ERROR_2;
        }
    }

    // 配置解码
    __decode_analysis_config(p_decode);
    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);
    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);
    // 获取当前页
    p_decode->move_height   = 0;
    p_decode->move_state    = 0;
    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    MBOOK_Layer_OpenShow(p_decode->show_lyr);
    return p_decode->config.show_rotate;
INIT_ERROR_2:
INIT_ERROR_1:

    for (i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if (p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    return -1;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_RotatePage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：0度，90度
*             失败：返回 EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetRotate(H_DECODE hdle)
{
    __mbook_decode_t        *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    return p_decode->config.show_rotate;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetTotalPage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：回放总页数
*             失败：返回 0
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetTotalPage(H_DECODE hdle)
{
    __mbook_decode_t        *p_decode;

    if (hdle == NULL)
    {
        return 0;
    }

    p_decode = (__mbook_decode_t *)hdle;
    return p_decode->total_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetCurrentPage
*
*Description: 旋转当前页面
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：回放当前页数
*             失败：返回 0
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetCurrentPage(H_DECODE hdle)
{
    __mbook_decode_t        *p_decode;

    if (hdle == NULL)
    {
        return 0;
    }

    p_decode = (__mbook_decode_t *)hdle;
    return p_decode->current_page;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetCurrentPage
*
*Description: 获取指定页的页面偏移值
*
*Arguments  : hdle: 操作句柄
*             page: 指定页码
*
*Return     : 成功：指定页面偏移量
*             失败：返回-1
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetPageOffset(H_DECODE hdle, __u32 page_no)
{
    __one_page_t        tmp_page;
    __mbook_decode_t   *p_decode;

    if (hdle == NULL)
    {
        return -1;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if ((page_no > p_decode->total_page) || (page_no < 1))
    {
        return -1;
    }

    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, page_no, &tmp_page);
    return tmp_page.page_start;
}
/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetPage
*
*Description: 获取指定面偏移值来获取对应的页面
*
*Arguments  : hdle: 操作句柄
*             page_offset: 指定页面偏移量
*
*Return     : 成功：指定页面偏移量所在页的页码
*             失败：返回-1
*
************************************************************************************************************************
*/

//..
__s32   MBOOK_Decode_GetPage(H_DECODE hdle, __u32 page_offset)
{
    __mbook_decode_t   *p_decode;
    __s32   page_no ;

    if (hdle == NULL)
    {
        return -1;
    }

    p_decode = (__mbook_decode_t *)hdle;
    page_no = MBOOK_Analysis_GetPage(p_decode->analysis_hdle, page_offset);
    return page_no ;
}
/*
************************************************************************************************************************
*                                       BOOK_Mark_SetPage
*
*Description: 保存一个书签
*
*Arguments  : hdle: 操作句柄
*             num ：要保存的书签编号
*             offset：相对文件的偏移量
*             context: 书签的内容信息
*
*Return     : 成功：EPDK_OK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/

//..
__s32   BOOK_Mark_SetPage(H_BMK hdle, __u32 num, __u32 offset, char *context)
{
    __book_mark_t      *p_mark;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_mark = (__book_mark_t *)hdle;

    if ((num < 1) || (num >= MAX_MARK_ITEM))
    {
        return EPDK_FAIL;
    }

    p_mark->mark_item.book_mark[num].number    = num;
    p_mark->mark_item.book_mark[num].start     = offset;
    eLIBs_strncpy(p_mark->mark_item.book_mark[num].context, context, MAX_CONTEXT);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       BOOK_Mark_Init
*
*Description: 加载书签模块
*
*Arguments  : mark_file: 要打开的书签文件
*
*
*Return     : 成功：返回操作句柄
*             失败：返回NULL
*
************************************************************************************************************************
*/

//..
H_BMK   BOOK_Mark_Init(char *mark_file)
{
    char                path[1024];
    ES_DIR             *dir;
    __book_mark_t      *p_mark;
    p_mark = (__book_mark_t *)esMEMS_Malloc(0, sizeof(__book_mark_t));  // 申请内存

    if (p_mark == 0)
    {
        __err(" book mark malloc error \n");
        return NULL;
    }

    eLIBs_memset(p_mark, 0, sizeof(__book_mark_t));
    dir = eLIBs_opendir(BOOK_MARK_PATH);

    if (dir == NULL)
    {
        __wrn("%s not exist, Create %s\r\n", BOOK_MARK_PATH, BOOK_MARK_PATH);
        eLIBs_mkdir(BOOK_MARK_PATH);
    }
    else
    {
        __wrn("%s is exist\r\n", BOOK_MARK_PATH);
        eLIBs_closedir(dir);
    }

    eLIBs_strcpy(path, BOOK_MARK_PATH);
    eLIBs_strcat(path, mark_file);
    p_mark->file_hdle = eLIBs_fopen(path, "r");

    if (p_mark->file_hdle == NULL)
    {
        __wrn("%s not exist, Create %s\r\n", path, path);
        p_mark->file_hdle = eLIBs_fopen(path, "wb");
        eLIBs_fclose(p_mark->file_hdle);
        p_mark->file_hdle = NULL;
    }
    else
    {
        __wrn("%s is exist\r\n", path);
        eLIBs_fclose(p_mark->file_hdle);
        p_mark->file_hdle = NULL;
    }

    __wrn("open file %s\r\n", path);
    p_mark->file_hdle = eLIBs_fopen(path, "rb+");
    p_mark->mark_size = sizeof(__mark_item_t);
    return p_mark;
}
/*
************************************************************************************************************************
*                                       BOOK_Mark_Open
*
*Description: 打开一个文件书签
*
*Arguments  : hdle: 操作句柄
*             file：要打开的文件
*
*Return     : 成功：EPDK_OK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/

//..
__s32   BOOK_Mark_Open(H_BMK hdle, char *file)
{
    __u32               len;
    __s32               code;
    __book_mark_t      *p_mark;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_mark = (__book_mark_t *)hdle;
    p_mark->mark_state = 0;
    p_mark->mark_offset = 0;

    while (1)
    {
        eLIBs_fseek(p_mark->file_hdle, p_mark->mark_offset, SEEK_SET);
        len = eLIBs_fread(&p_mark->mark_item, 1, p_mark->mark_size, p_mark->file_hdle);

        if (len < p_mark->mark_size)
        {
            break;
        }

        if (0 == eLIBs_strcmp(p_mark->mark_item.file_path, file))
        {
            p_mark->mark_state = 1;
            break;
        }

        p_mark->mark_offset = p_mark->mark_offset + p_mark->mark_size;  //指向下一个书签
    }

    if (p_mark->mark_state == 1)
    {
        return EPDK_OK;
    }
    else
    {
        eLIBs_strcpy(p_mark->mark_item.file_path, file);
        eLIBs_GetFSCharset(file, (__s32 *)&code);
        p_mark->mark_item.file_charset = (__epdk_charset_enm_e)code;
        eLIBs_memset(p_mark->mark_item.book_mark, 0xff, sizeof(__item_context_t) * MAX_MARK_ITEM);
        return EPDK_FAIL;
    }
}
/*
************************************************************************************************************************
*                                       BOOK_Mark_Close
*
*Description: 获取一个指定书签的内容信息
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：EPDK_OK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/
//..

__s32   BOOK_Mark_Close(H_BMK hdle)
{
    __book_mark_t      *p_mark;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_mark = (__book_mark_t *)hdle;
    __msg("p_mark->mark_offset=%d\n", p_mark->mark_offset);
    eLIBs_fseek(p_mark->file_hdle, p_mark->mark_offset, SEEK_SET);
    //__here__;
    eLIBs_fwrite(&p_mark->mark_item, 1, p_mark->mark_size, p_mark->file_hdle);
    //__here__;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       BOOK_Mark_Uninit
*
*Description: 卸载书签模块
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：EPDK_OK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/
//..
__s32   BOOK_Mark_Uninit(H_BMK hdle)
{
    __book_mark_t      *p_mark;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_mark = (__book_mark_t *)hdle;

    if (p_mark->file_hdle)
    {
        eLIBs_fclose(p_mark->file_hdle);
        p_mark->file_hdle = NULL;
    }

    esMEMS_Mfree(0, p_mark);
    p_mark = NULL;
    return EPDK_OK;
}


/*
************************************************************************************************************************
*                                       BOOK_Mark_GetPageOffset
*
*Description: 获取一个指定书签的文件偏移量
*
*Arguments  : hdle: 操作句柄
*             num ：书签编号
*
*Return     : 成功：返回文件的偏移量
*             失败：-1
*
************************************************************************************************************************
*/
//..
__s32   BOOK_Mark_GetPageOffset(H_BMK hdle, __u32 num)
{
    __book_mark_t      *p_mark;

    if (hdle == NULL)
    {
        return -1;
    }

    p_mark = (__book_mark_t *)hdle;

    if ((num < 1) || (num >= MAX_MARK_ITEM))
    {
        return -1;
    }

    return p_mark->mark_item.book_mark[num].start;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetFileName
*
*Description: 获取文件的文件名
*
*Arguments  : hdle: 操作句柄
*             name: 文件名
*
*Return     : 成功：PDK_OKK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetFileName(H_DECODE hdle, char *name)
{
    char                   *p;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    p = eLIBs_strchrlast(p_decode->file_path, '\\');
    eLIBs_strcpy(name, p + 1);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetFileSize
*
*Description: 获取文件的文件名
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : 成功：总大小
*             失败：-1
*
************************************************************************************************************************
*/
__s64   MBOOK_Decode_GetFileSize(H_DECODE hdle)
{
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return -1;
    }

    p_decode = (__mbook_decode_t *)hdle;
    return p_decode->file_size;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetFilePath
*
*Description: 获取文件的全路径名
*
*Arguments  : hdle: 操作句柄
*             path: 路径名
*
*Return     : 成功：PDK_OKK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetFilePath(H_DECODE hdle, char *path)
{
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    eLIBs_strcpy(path, p_decode->file_path);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_GetFileContext
*
*Description: 获取当前页面的信息
*
*Arguments  : hdle: 操作句柄
*             context: 文件内容
*
*Return     : 成功：PDK_OKK
*             失败：EPDK_FAIL
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_GetFileContext(H_DECODE hdle, char *context)
{
    __one_page_t        tmp_page;
    __mbook_decode_t    *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    MBOOK_Analysis_GetInfo(p_decode->analysis_hdle, p_decode->current_page, &tmp_page);
    eLIBs_fseek(p_decode->file_hdle, tmp_page.page_start, SEEK_SET);
    eLIBs_fread(context, 1, sizeof(context), p_decode->file_hdle);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SetRowSpace
*
*Description: 设置行间距
*
*Arguments  : hdle: 操作句柄
*             space: 行间距
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetRowSpace(H_DECODE hdle, __u32 space)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.row_space == space)
    {
        return EPDK_OK;
    }

    p_decode->config.row_space = space;
    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    // 配置解码
    __decode_analysis_config(p_decode);
    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);
    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);
    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);

    if (p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }

    __decode_show_page(p_decode);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SetColSpace
*
*Description: 设置列间距
*
*Arguments  : hdle: 操作句柄
*             space: 列间距
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetColSpace(H_DECODE hdle, __u32 space)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.col_space == space)
    {
        return EPDK_OK;
    }

    p_decode->config.col_space = space;
    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    // 配置解码
    __decode_analysis_config(p_decode);
    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);
    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);
    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);

    if (p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }

    __decode_show_page(p_decode);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SetBorderWidth
*
*Description: 设置边距
*
*Arguments  : hdle: 操作句柄
*             space: 边距
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetBorderWidth(H_DECODE hdle, __u32 width)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.border_width == width)
    {
        return EPDK_OK;
    }

    p_decode->config.border_width = width;
    __msg("p_decode->config.border_width = %d \n", p_decode->config.border_width);
    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    // 配置解码
    __decode_analysis_config(p_decode);
    // 配置显示
    __decode_show_config(p_decode);
    __decode_brush_page(p_decode, DECODE_CURR);
    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);
    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);

    if (p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }

    __decode_show_page(p_decode);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SetBackColor
*
*Description: 设置显示的背景色
*
*Arguments  : hdle: 操作句柄
*             color: 颜色
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetBackColor(H_DECODE hdle, __u32 color)
{
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.back_color == color)
    {
        return EPDK_OK;
    }

    p_decode->config.back_color = color;
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SetFontColor
*
*Description: 设置显示文字的颜色
*
*Arguments  : hdle: 操作句柄
*             color: 颜色
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetFontColor(H_DECODE hdle, __u32 color)
{
    __mbook_decode_t       *p_decode;
    __u8 err;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->config.font_color == color)
    {
        return EPDK_OK;
    }

    p_decode->config.font_color = color;
    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    // 配置解码
    __decode_analysis_config(p_decode);
    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);
    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);
    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);
    __decode_show_page(p_decode);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_SetDefaultCharset
*
*Description: 设置默认编码格式
*
*Arguments  : hdle: 操作句柄
*             charset: 默认编码
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_SetDefaultCharset(H_DECODE hdle, __epdk_charset_enm_e charset)
{
    __u8                    err;
    __mbook_decode_t       *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;
    __wrn("p_decode->default_charset=%d\n", p_decode->default_charset);
    __wrn("charset=%d\n", charset);

    if (p_decode->default_charset == charset)
    {
        return EPDK_OK;
    }

    p_decode->default_charset = charset;

    if ((p_decode->charset == EPDK_CHARSET_ENM_UTF16LE)
        || (p_decode->charset == EPDK_CHARSET_ENM_UTF16BE)
        || (p_decode->charset == EPDK_CHARSET_ENM_UTF8))
    {
        // 非未知编码
        return EPDK_OK;
    }

    if (p_decode->decode_sta == DECODE_NULL)
    {
        // 未开始解码
        return EPDK_OK;
    }

    // 挂起预解线程
    esKRNL_SemPend(p_decode->decode_sem, 0, &err);
    // 获取文件编码格式
    __decode_get_charset(p_decode);
    // 配置解码
    __decode_analysis_config(p_decode);
    // 配置显示
    __decode_brush_page(p_decode, DECODE_CURR);
    __decode_show_config(p_decode);
    // 恢复预解线程
    esKRNL_SemPost(p_decode->decode_sem);
    // 获取当前页
    __decode_get_page(p_decode, DECODE_CURR);

    if (p_decode->move_height)
    {
        __decode_get_next_page(p_decode);
    }

    __decode_show_page(p_decode);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*                                       MBOOK_Decode_Uninit
*
*Description: 删除页码解码模块
*
*Arguments  : hdle: 操作句柄
*
*
*Return     : EPDK_OK: 成功
*             EPDK_FAIL: 失败
*
************************************************************************************************************************
*/
__s32   MBOOK_Decode_Uninit(H_DECODE hdle)
{
    __s32                   i;
    __u8                    err;
    __mbook_decode_t        *p_decode;

    if (hdle == NULL)
    {
        return EPDK_FAIL;
    }

    p_decode = (__mbook_decode_t *)hdle;

    if (p_decode->decode_tsk)
    {
        while (1)
        {
            if (esKRNL_TDelReq(p_decode->decode_tsk) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            esKRNL_TimeDly(1);
        }

        p_decode->decode_tsk = 0;
    }

    if (p_decode->decode_sem)
    {
        esKRNL_SemDel(p_decode->decode_sem, OS_DEL_ALWAYS, &err);
        p_decode->decode_sem = NULL;
    }

    if (p_decode->curt_page_data)
    {
        esMEMS_Pfree(p_decode->curt_page_data, p_decode->data_len);
        p_decode->curt_page_data = NULL;
    }

    if (p_decode->page_data)
    {
        My_Pfree(p_decode->page_data, p_decode->data_len);
        p_decode->page_data = NULL;
    }

    for (i = 0; i < DECODE_MBOOK_NUM; i++)
    {
        if (p_decode->page[i].page_lyr)
        {
            MBOOK_Layer_DeleteLayer(p_decode->page[i].page_lyr);
            p_decode->page[i].page_lyr = NULL;
        }
    }

    if (p_decode->show_lyr)
    {
        MBOOK_Layer_DeleteLayer(p_decode->show_lyr);
        p_decode->show_lyr = NULL;
    }

    if (p_decode->show_hdle)
    {
        MBOOK_Show_Uninit(p_decode->show_hdle);
        p_decode->show_hdle = NULL;
    }

    if (p_decode->analysis_hdle)
    {
        MBOOK_Analysis_Uninit(p_decode->analysis_hdle);
        p_decode->analysis_hdle = NULL;
    }

    if (p_decode->font_hdle)
    {
        MBOOK_Font_Uninit(p_decode->font_hdle);
        p_decode->font_hdle = NULL;
    }

    if (p_decode->file_hdle)
    {
        eLIBs_fclose(p_decode->file_hdle);
        p_decode->file_hdle = NULL;
    }

    if (p_decode)
    {
        My_Mfree(0, p_decode);
        p_decode = NULL;
    }

    return EPDK_OK;
}
